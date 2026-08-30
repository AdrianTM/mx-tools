/**********************************************************************
 * Copyright (C) 2014-2026 MX Authors
 *
 * This file is part of MX Tools and is licensed under GPL-3.0-or-later.
 **********************************************************************/
#include "toolmodel.h"

#include <algorithm>

#include <QDesktopServices>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QLocale>
#include <QProcess>
#include <QRegularExpression>
#include <QStorageInfo>
#include <QTextStream>
#include <QUrl>

namespace
{
constexpr auto applicationsPath = "/usr/share/applications";
constexpr auto userApplicationsPath = "/.local/share/applications";
constexpr auto manualPath = "/usr/share/mx-docs/mxum_en.pdf";
constexpr auto licensePath = "/usr/share/doc/mx-tools/license.html";
constexpr auto changelogPath = "/usr/share/doc/mx-tools/changelog.gz";

const QList<QPair<QString, QStringList>> categoryDefinitions {
    {QStringLiteral("Live"), {QStringLiteral("MX-Live"), QStringLiteral("X-MX-Live")}},
    {QStringLiteral("Maintenance"), {QStringLiteral("MX-Maintenance"), QStringLiteral("X-MX-Maintenance")}},
    {QStringLiteral("Setup"), {QStringLiteral("MX-Setup"), QStringLiteral("X-MX-Setup")}},
    {QStringLiteral("Software"), {QStringLiteral("MX-Software"), QStringLiteral("X-MX-Software")}},
    {QStringLiteral("Utilities"), {QStringLiteral("MX-Utilities"), QStringLiteral("X-MX-Utilities")}}
};

QStringList currentDesktops()
{
    QStringList desktops = QString::fromUtf8(qgetenv("XDG_CURRENT_DESKTOP")).split(QLatin1Char(':'), Qt::SkipEmptyParts);
    if (desktops.isEmpty()) {
        desktops = QString::fromUtf8(qgetenv("XDG_SESSION_DESKTOP")).split(QLatin1Char(':'), Qt::SkipEmptyParts);
    }
    for (QString &desktop : desktops) {
        desktop = desktop.trimmed().toUpper();
    }
    return desktops;
}

bool isLiveEnvironment()
{
    const QByteArray fileSystem = QStorageInfo(QStringLiteral("/")).fileSystemType();
    return fileSystem == "aufs" || fileSystem == "overlay";
}

QString translatedCategory(const QString &category)
{
    if (category == QLatin1String("Live")) {
        return ToolModel::tr("Live");
    }
    if (category == QLatin1String("Maintenance")) {
        return ToolModel::tr("Maintenance");
    }
    if (category == QLatin1String("Setup")) {
        return ToolModel::tr("Setup");
    }
    if (category == QLatin1String("Software")) {
        return ToolModel::tr("Software");
    }
    return ToolModel::tr("Utilities");
}

QStringList listValue(const QString &text, const QString &key)
{
    const QString prefix = key + QLatin1Char('=');
    for (const QString &line : text.split(QLatin1Char('\n'))) {
        if (line.startsWith(prefix, Qt::CaseInsensitive)) {
            QStringList result = line.mid(prefix.size()).split(QLatin1Char(';'), Qt::SkipEmptyParts);
            for (QString &item : result) {
                item = item.trimmed().toUpper();
            }
            return result;
        }
    }
    return {};
}
}

ToolIconProvider::ToolIconProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

void ToolIconProvider::insert(const QString &key, const QIcon &icon)
{
    m_icons.insert(key, icon);
}

QPixmap ToolIconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    const QSize target = requestedSize.isValid() ? requestedSize : QSize(48, 48);
    QPixmap pixmap = m_icons.value(id).pixmap(target);
    if (size != nullptr) {
        *size = pixmap.size();
    }
    return pixmap;
}

ToolModel::ToolModel(ToolIconProvider *iconProvider, QObject *parent)
    : QAbstractListModel(parent),
      m_iconProvider(iconProvider)
{
    loadTools();
    detectMenuVisibility();
    refilter();
}

int ToolModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_visibleRows.size());
}

QVariant ToolModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_visibleRows.size()) {
        return {};
    }
    const ToolInfo &tool = m_allTools.at(m_visibleRows.at(index.row()));
    switch (role) {
    case NameRole:
        return tool.name;
    case CommentRole:
        return tool.comment;
    case CategoryRole:
        return tool.category;
    case IconSourceRole:
        return tool.iconSource;
    case FileNameRole:
        return tool.fileName;
    default:
        return {};
    }
}

QHash<int, QByteArray> ToolModel::roleNames() const
{
    return {{NameRole, "name"}, {CommentRole, "comment"}, {CategoryRole, "category"},
            {IconSourceRole, "iconSource"}, {FileNameRole, "fileName"}};
}

QString ToolModel::search() const
{
    return m_search;
}

void ToolModel::setSearch(const QString &search)
{
    if (m_search == search) {
        return;
    }
    m_search = search;
    emit searchChanged();
    refilter();
}

QString ToolModel::selectedCategory() const
{
    return m_selectedCategory;
}

void ToolModel::setSelectedCategory(const QString &category)
{
    if (m_selectedCategory == category) {
        return;
    }
    m_selectedCategory = category;
    emit selectedCategoryChanged();
    refilter();
}

QStringList ToolModel::categories() const
{
    return m_categories;
}

int ToolModel::totalCount() const
{
    return static_cast<int>(m_allTools.size());
}

bool ToolModel::hideFromMenu() const
{
    return m_hideFromMenu;
}

void ToolModel::setHideFromMenu(bool hide)
{
    if (m_hideFromMenu == hide) {
        return;
    }
    for (const QString &fileName : std::as_const(m_menuFiles)) {
        updateDesktopFileVisibility(fileName, hide);
    }
    m_hideFromMenu = hide;
    emit hideFromMenuChanged();

    QProcess panelCheck;
    panelCheck.start(QStringLiteral("pgrep"), {QStringLiteral("xfce4-panel")});
    panelCheck.waitForFinished();
    if (panelCheck.exitCode() == 0) {
        QProcess::startDetached(QStringLiteral("xfce4-panel"), {QStringLiteral("--restart")});
    }
}

void ToolModel::loadTools()
{
    m_categories = {tr("All tools")};
    int iconNumber = 0;
    for (const auto &[category, tokens] : categoryDefinitions) {
        const QString categoryName = translatedCategory(category);
        QStringList files = desktopFilesForCategory(tokens);
        m_menuFiles.append(files);
        QVector<ToolInfo> categoryTools;
        for (const QString &fileName : std::as_const(files)) {
            QFile file(fileName);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                continue;
            }
            const QString text = QString::fromUtf8(file.readAll());
            if (!visibleInCurrentEnvironment(text)) {
                continue;
            }
            if (!isLiveEnvironment()
                && (QFileInfo(fileName).fileName() == QLatin1String("mx-remastercc.desktop")
                    || QFileInfo(fileName).fileName() == QLatin1String("live-kernel-updater.desktop"))) {
                continue;
            }

            ToolInfo tool;
            tool.fileName = fileName;
            tool.name = translatedValue(text, QStringLiteral("Name"));
            if (tool.name.isEmpty()) {
                tool.name = value(text, QStringLiteral("Name"));
            }
            tool.name.remove(QRegularExpression(QStringLiteral("^MX ")));
            tool.comment = translatedValue(text, QStringLiteral("Comment"));
            if (tool.comment.isEmpty()) {
                tool.comment = value(text, QStringLiteral("Comment"));
            }
            tool.exec = value(text, QStringLiteral("Exec"));
            tool.exec.remove(QRegularExpression(QStringLiteral(R"( %[a-zA-Z])")));
            tool.category = categoryName;
            tool.runInTerminal = value(text, QStringLiteral("Terminal")).compare(QStringLiteral("true"), Qt::CaseInsensitive) == 0;

            const QString iconName = value(text, QStringLiteral("Icon"));
            const QString iconKey = QString::number(iconNumber++);
            m_iconProvider->insert(iconKey, lookupIcon(iconName).value_or(fallbackIcon()));
            tool.iconSource = QStringLiteral("image://toolicons/") + iconKey;
            categoryTools.append(tool);
        }
        std::ranges::sort(categoryTools, {}, &ToolInfo::name);
        if (!categoryTools.isEmpty()) {
            m_categories.append(categoryName);
            m_allTools.append(categoryTools);
        }
    }
    m_menuFiles.removeDuplicates();
}

void ToolModel::refilter()
{
    beginResetModel();
    m_visibleRows.clear();
    const QString allTools = m_categories.value(0);
    for (int i = 0; i < m_allTools.size(); ++i) {
        const ToolInfo &tool = m_allTools.at(i);
        const bool categoryMatches = !m_search.trimmed().isEmpty() || m_selectedCategory.isEmpty()
                                     || m_selectedCategory == allTools
                                     || tool.category == m_selectedCategory;
        const bool textMatches = m_search.trimmed().isEmpty()
                                 || tool.name.contains(m_search, Qt::CaseInsensitive)
                                 || tool.comment.contains(m_search, Qt::CaseInsensitive)
                                 || tool.category.contains(m_search, Qt::CaseInsensitive);
        if (categoryMatches && textMatches) {
            m_visibleRows.append(i);
        }
    }
    endResetModel();
}

QString ToolModel::value(const QString &text, const QString &key)
{
    const QRegularExpression expression(QStringLiteral("^") + QRegularExpression::escape(key)
                                        + QStringLiteral("=(.*)$"), QRegularExpression::MultilineOption);
    return expression.match(text).captured(1).trimmed();
}

QString ToolModel::translatedValue(const QString &text, const QString &key)
{
    const QLocale locale;
    const QStringList localeNames {locale.name(), locale.name().section(QLatin1Char('_'), 0, 0)};
    for (const QString &localeName : localeNames) {
        const QRegularExpression expression(QStringLiteral("^") + QRegularExpression::escape(key)
                                            + QStringLiteral("\\[") + QRegularExpression::escape(localeName)
                                            + QStringLiteral("\\]=(.*)$"), QRegularExpression::MultilineOption);
        const QString result = expression.match(text).captured(1).trimmed();
        if (!result.isEmpty()) {
            return result;
        }
    }
    return {};
}

QStringList ToolModel::desktopFilesForCategory(const QStringList &tokens)
{
    QStringList matchingFiles;
    QDirIterator iterator(QString::fromLatin1(applicationsPath), {QStringLiteral("*.desktop")}, QDir::Files,
                          QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        const QString path = iterator.next();
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue;
        }
        const QString categories = value(QString::fromUtf8(file.readAll()), QStringLiteral("Categories"));
        const QStringList entries = categories.split(QLatin1Char(';'), Qt::SkipEmptyParts);
        if (std::ranges::any_of(tokens, [&entries](const QString &token) { return entries.contains(token); })) {
            matchingFiles.append(path);
        }
    }
    return matchingFiles;
}

bool ToolModel::visibleInCurrentEnvironment(const QString &text)
{
    const bool live = isLiveEnvironment();
    if ((live && text.contains(QStringLiteral("MX-OnlyInstalled"), Qt::CaseInsensitive))
        || (!live && text.contains(QStringLiteral("MX-OnlyLive"), Qt::CaseInsensitive))) {
        return false;
    }
    const QStringList desktops = currentDesktops();
    const QStringList onlyShowIn = listValue(text, QStringLiteral("OnlyShowIn"));
    if (!onlyShowIn.isEmpty()
        && std::ranges::none_of(onlyShowIn, [&desktops](const QString &desktop) { return desktops.contains(desktop); })) {
        return false;
    }
    const QStringList notShowIn = listValue(text, QStringLiteral("NotShowIn"));
    return std::ranges::none_of(notShowIn, [&desktops](const QString &desktop) { return desktops.contains(desktop); });
}

std::optional<QIcon> ToolModel::lookupIcon(const QString &iconName)
{
    if (QFileInfo(iconName).isAbsolute() && QFile::exists(iconName)) {
        return QIcon(iconName);
    }
    QString name = iconName;
    name.remove(QRegularExpression(QStringLiteral(R"(\.(png|svg|xpm)$)")));
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return QIcon::fromTheme(name);
    }
    const QStringList roots {QDir::homePath() + QStringLiteral("/.local/share/icons/"),
                             QStringLiteral("/usr/share/pixmaps/"), QStringLiteral("/usr/local/share/icons/"),
                             QStringLiteral("/usr/share/icons/hicolor/scalable/apps/"),
                             QStringLiteral("/usr/share/icons/hicolor/48x48/apps/"),
                             QStringLiteral("/usr/share/icons/Adwaita/48x48/legacy/")};
    for (const QString &root : roots) {
        for (const QString &extension : {QString(), QStringLiteral(".svg"), QStringLiteral(".png"), QStringLiteral(".xpm")}) {
            const QString candidate = root + (extension.isEmpty() ? iconName : name + extension);
            if (QFile::exists(candidate)) {
                return QIcon(candidate);
            }
        }
    }
    return std::nullopt;
}

QIcon ToolModel::fallbackIcon()
{
    static const QIcon icon = QIcon::fromTheme(QStringLiteral("applications-utilities"),
                                               QIcon(QStringLiteral(":/qt/qml/MxTools/icons/logo.svg")));
    return icon;
}

void ToolModel::launch(const QString &fileName)
{
    const auto iterator = std::ranges::find(m_allTools, fileName, &ToolInfo::fileName);
    if (iterator == m_allTools.cend()) {
        emit errorOccurred(tr("Unable to launch tool"), tr("The selected tool is no longer available."));
        return;
    }
    const qint64 runningProcessId = m_runningTools.value(fileName);
    if (runningProcessId > 0
        && QFileInfo::exists(QStringLiteral("/proc/%1").arg(runningProcessId))) {
        emit errorOccurred(tr("Tool already running"), tr("%1 is already running.").arg(iterator->name));
        return;
    }
    m_runningTools.remove(fileName);

    QString commandText = iterator->runInTerminal ? QStringLiteral("x-terminal-emulator -e ") + iterator->exec
                                                   : iterator->exec;
    QStringList arguments = QProcess::splitCommand(commandText);
    if (arguments.isEmpty()) {
        emit errorOccurred(tr("Unable to launch tool"), tr("The selected tool has no launch command."));
        return;
    }
    const QString program = arguments.takeFirst();
    if (!arguments.isEmpty() && arguments.constLast() == QLatin1String("&")) {
        arguments.removeLast();
    }
    qint64 processId = 0;
    if (!QProcess::startDetached(program, arguments, {}, &processId)) {
        emit errorOccurred(tr("Unable to launch tool"), tr("Could not start %1.").arg(iterator->name));
    } else if (processId > 0) {
        m_runningTools.insert(fileName, processId);
    }
}

void ToolModel::openLocalOrReport(const QString &path, ToolModel *model, const QString &title)
{
    if (!QFileInfo::exists(path) || !QDesktopServices::openUrl(QUrl::fromLocalFile(path))) {
        emit model->errorOccurred(title, tr("Could not open %1.").arg(path));
    }
}

void ToolModel::openManual()
{
    openLocalOrReport(QString::fromLatin1(manualPath), this, tr("Manual unavailable"));
}

void ToolModel::openLicense()
{
    openLocalOrReport(QString::fromLatin1(licensePath), this, tr("License unavailable"));
}

void ToolModel::openWebsite()
{
    if (!QDesktopServices::openUrl(QUrl(QStringLiteral("https://mxlinux.org")))) {
        emit errorOccurred(tr("Website unavailable"), tr("Could not open the MX Linux website."));
    }
}

void ToolModel::openChangelog()
{
    if (!QFileInfo::exists(QString::fromLatin1(changelogPath))) {
        emit errorOccurred(tr("Changelog unavailable"), tr("Could not open %1.").arg(QString::fromLatin1(changelogPath)));
        return;
    }
    QProcess process;
    process.start(QStringLiteral("zcat"), {QString::fromLatin1(changelogPath)}, QIODevice::ReadOnly);
    if (!process.waitForFinished() || process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        emit errorOccurred(tr("Changelog unavailable"), tr("Could not read the application changelog."));
        return;
    }
    emit documentReady(tr("Changelog"), QString::fromUtf8(process.readAllStandardOutput()));
}

void ToolModel::detectMenuVisibility()
{
    QFile file(QDir::homePath() + QString::fromLatin1(userApplicationsPath) + QStringLiteral("/mx-user.desktop"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_hideFromMenu = QString::fromUtf8(file.readAll()).contains(QStringLiteral("NoDisplay=true"));
    }
}

void ToolModel::updateDesktopFileVisibility(const QString &fileName, bool hide)
{
    const QDir directory(QDir::homePath() + QString::fromLatin1(userApplicationsPath));
    if (!QDir().mkpath(directory.absolutePath())) {
        emit errorOccurred(tr("Menu setting failed"), tr("Could not create %1.").arg(directory.absolutePath()));
        return;
    }
    const QString destination = directory.filePath(QFileInfo(fileName).fileName());
    if (!hide) {
        QFile::remove(destination);
        return;
    }
    QFile::remove(destination);
    if (!QFile::copy(fileName, destination)) {
        emit errorOccurred(tr("Menu setting failed"), tr("Could not update %1.").arg(destination));
        return;
    }
    QFile file(destination);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    QStringList lines = QString::fromUtf8(file.readAll()).split(QLatin1Char('\n'));
    file.close();
    lines.removeIf([](const QString &line) {
        return line.startsWith(QStringLiteral("NoDisplay=")) || line.startsWith(QStringLiteral("Hidden="));
    });
    const qsizetype header = lines.indexOf(QStringLiteral("[Desktop Entry]"));
    lines.insert(header >= 0 ? header + 1 : 0, QStringLiteral("NoDisplay=true"));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream stream(&file);
        stream << lines.join(QLatin1Char('\n'));
    }
}
