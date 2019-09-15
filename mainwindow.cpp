/**********************************************************************
 * Copyright (C) 2014 MX Authors
 *
 * Authors: Adrian
 *          MX Linux <http://mxlinux.org>
 *
 * This file is part of MX Tools.
 *
 * MX Tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MX Tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MX Tools.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "flatbutton.h"
#include "version.h"

#include <QDesktopWidget>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QTextEdit>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainWindow)
{
    qDebug().noquote() << QCoreApplication::applicationName() << "version:" << VERSION;
    ui->setupUi(this);
    setWindowFlags(Qt::Window); // for the close, min and max buttons
    // detect if tools are displayed in the menu (check for only one since all are set at the same time)
    if (system("grep -q \"NoDisplay=true\" /home/$USER/.local/share/applications/mx-user.desktop") == 0) {
        ui->hideCheckBox->setChecked(true);
    }

    QString search_folder = "/usr/share/applications";
    live_list = listDesktopFiles("MX-Live", search_folder);
    maintenance_list = listDesktopFiles("MX-Maintenance", search_folder);
    setup_list = listDesktopFiles("MX-Setup", search_folder);
    software_list = listDesktopFiles("MX-Software", search_folder);
    utilities_list = listDesktopFiles("MX-Utilities", search_folder);

    QVector<QStringList *> lists;
    lists << &live_list
          << &maintenance_list
          << &setup_list
          << &software_list
          << &utilities_list;

    QString test = getCmdOut("df -T / |tail -n1 |awk '{print $2}'");

    // remove mx-remastercc and live-kernel-updater from list if not running Live
    bool live = (test == "aufs" || test == "overlay");
    if (!live) { // installed
        const QStringList live_list_copy = live_list;
        for (const QString &item : live_list_copy) {
            if (item.contains("mx-remastercc.desktop") || item.contains("live-kernel-updater.desktop")) {
                live_list.removeOne(item);
            }
        }
    }
    for (int i = 0; i < lists.size(); ++i) {
        removeEnvExclusive(*lists[i], live);
    }

    // remove item from list if it is only meant for XFCE
    if (qgetenv("XDG_CURRENT_DESKTOP") != "XFCE") {
        for (int i = 0; i < lists.size(); ++i) {
            removeXfceOnly(*lists[i]);
        }
    }

    category_map.insertMulti("MX-Live", live_list);
    category_map.insertMulti("MX-Maintenance", maintenance_list);
    category_map.insertMulti("MX-Setup", setup_list);
    category_map.insertMulti("MX-Software", software_list);
    category_map.insertMulti("MX-Utilities", utilities_list);

    readInfo(category_map);
    addButtons(info_map);
    ui->lineSearch->setFocus();
    this->adjustSize();
    this->resize(this->width() + 80, this->height() + 130);
    int width = this->width();
    int height = this->height();

    QSettings settings("mx-tools");
    restoreGeometry(settings.value("geometry").toByteArray());

    if (this->isMaximized()) {  // if started maximized give option to resize to normal window size
        this->resize(width, height);
        QRect screenGeometry = QApplication::desktop()->screenGeometry();
        int x = (screenGeometry.width()-this->width()) / 2;
        int y = (screenGeometry.height()-this->height()) / 2;
        this->move(x, y);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Util function
QString MainWindow::getCmdOut(const QString &cmd) {
    proc = new QProcess(this);
    proc->start("/bin/bash", QStringList() << "-c" << cmd);
    proc->setReadChannel(QProcess::StandardOutput);
    proc->setReadChannelMode(QProcess::MergedChannels);
    proc->waitForFinished(-1);
    return proc->readAllStandardOutput().trimmed();
}

// List .desktop files that contain a specific string
QStringList MainWindow::listDesktopFiles(const QString &search_string, const QString &location)
{
    QStringList listDesktop;
    QString cmd = QString("grep -Elr %1 %2 | sort").arg(search_string).arg(location);
    QString out = getCmdOut(cmd);
    if (out != "") {
        listDesktop = out.split("\n");
    }
    return listDesktop;
}

// Load info (name, comment, exec, icon_name, category, terminal) to the info_map
void MainWindow::readInfo(const QMultiMap<QString, QStringList> &category_map)
{
    QString name;
    QString comment;
    QString exec;
    QString icon_name;
    QString terminal_switch;
    QStringList list;
    QLocale locale;
    QString lang = locale.bcp47Name();
    QMultiMap<QString, QStringList> map;

    for (const QString &category : category_map.keys()) {
        list = category_map.value(category);
        for (const QString &file_name : list) {
            name = "";
            comment = "";
            if (lang != "en") {
                name = getCmdOut("grep -i ^'Name\\[" + lang + "\\]=' " + file_name + " | cut -f2 -d=");
                comment = getCmdOut("grep -i ^'Comment\\[" + lang + "\\]=' " + file_name + " | cut -f2 -d=");
            }
            if (lang == "pt" && name == "") { // Brazilian if Portuguese and name empty
                name = getCmdOut("grep -i ^'Name\\[pt_BR]=' " + file_name + " | cut -f2 -d=");
            }
            if (lang == "pt" && comment == "") { // Brazilian if Portuguese and comment empty
                comment = getCmdOut("grep -i ^'Comment\\[pt_BR]=' " + file_name + " | cut -f2 -d=");
            }
            if (name == "") { // backup if Name is not translated
                name = getCmdOut("grep -i ^Name= " + file_name + " | cut -f2 -d=");
                name = name.remove("MX ");
            }
            if (comment == "") { // backup if Comment is not translated
                comment = getCmdOut("grep ^Comment= " + file_name + " | cut -f2 -d=");
            }
            exec = getCmdOut("grep ^Exec= " + file_name + " | cut -f2 -d=");
            icon_name = getCmdOut("grep ^Icon= " + file_name + " | cut -f2 -d=");
            terminal_switch = getCmdOut("grep ^Terminal= " + file_name + " | cut -f2 -d=");
            QStringList info;
            map.insert(file_name, info << name << comment << icon_name << exec << category << terminal_switch);
        }
        info_map.insert(category, map);
        map.clear();
    }
}

// read the info_map and add the buttons to the UI
void MainWindow::addButtons(const QMultiMap<QString, QMultiMap<QString, QStringList> > &info_map)
{
    int col = 0;
    int row = 0;
    int max = 3; // no. max of col
    QString name;
    QString comment;
    QString exec;
    QString icon_name;
    QString file_name;
    QString terminal_switch;

    for (const QString &category : info_map.keys()) {
        if (!info_map.value(category).isEmpty()) {
            // add empty row and delimiter except for the first row
            if (row != 0) {
                col = 0;
                row += 1;
                QFrame *line = new QFrame();
                line->setFrameShape(QFrame::HLine);
                line->setFrameShadow(QFrame::Sunken);
                ui->gridLayout_btn->addWidget(line, row, col, 1, -1);
            }
            QLabel *label = new QLabel();
            QFont font;
            font.setBold(true);
            font.setUnderline(true);
            label->setFont(font);
            QString label_txt = category;
            label_txt.remove("MX-");
            label->setText(label_txt);
            col = 0;
            row += 1;
            ui->gridLayout_btn->addWidget(label, row, col);
            row += 1;
            for (const QString &file_name : info_map.value(category).keys()) {
                QStringList file_info = info_map.value(category).value(file_name);
                name = file_info[0];
                comment = file_info[1];
                icon_name = file_info[2];
                exec = file_info[3];
                terminal_switch = file_info[5];
                //qDebug() << "terminal switch" << terminal_switch;
                btn = new FlatButton(name);
                btn->setToolTip(comment);
                btn->setAutoDefault(false);
                btn->setIcon(findIcon(icon_name));
                btn->setIconSize(32, 32);
                ui->gridLayout_btn->addWidget(btn, row, col);
                //ui->gridLayout_btn->setRowStretch(row, 0);
                col += 1;
                if (col >= max) {
                    col = 0;
                    row += 1;
                }
                //add "x-termial-emulator -e " if terminal_switch = true
                QString cmd = "x-terminal-emulator -e ";
                if (terminal_switch == "true") {
                    btn->setObjectName(cmd + exec); // add the command to be executed to the object name
                } else {
                    btn->setObjectName(exec); // add the command to be executed to the object name
                }

                //qDebug() << "button exec" << btn->objectName();
                QObject::connect(btn, &FlatButton::clicked, this, &MainWindow::btn_clicked);
            }
        }
    }
    ui->gridLayout_btn->setRowStretch(row, 1);
}

// find icon by name specified in .desktop file
// return in order: fromTheme, pixmaps, hicolor, icons
QIcon MainWindow::findIcon(QString icon_name)
{
    // return icon if fully specified
    if (QFile::exists("/" + icon_name) && QFileInfo("/" + icon_name).isFile()) { // make sure it looks for icon in root, not in current folder
        return QIcon(icon_name);
    } else {
        QString icon_name_no_ext = icon_name.remove(".png").remove(".svg").remove(".xpm");

        // return the icon from the theme if it exists
        if (QIcon::hasThemeIcon(icon_name_no_ext)) {
            return QIcon::fromTheme(icon_name_no_ext);

            // try /usr/share/pixmaps
        } else if (QFile::exists("/usr/share/pixmaps/" + icon_name)) {
            return QIcon("/usr/share/pixmaps/" + icon_name);

            // fallback to hicolor icons
        }  else {
            QString name = getCmdOut("find /usr/share/icons/hicolor/ -iname \"" + icon_name + " -print -quit");
            if (!name.isEmpty()) {
                return QIcon(name);

                // fallback to /usr/share/icons
            } else if (QFile::exists("/usr/share/icons/" + icon_name)) {
                return QIcon("/usr/share/icons/" + icon_name);
            }
        }

        // Try file names regardless of specified extension
        // return png, svg, xpm icons from /usr/share/pixmaps
        if (QFile::exists("/usr/share/pixmaps/" + icon_name_no_ext + ".png")) {
            return QIcon("/usr/share/pixmaps/" + icon_name_no_ext + ".png");
        } else if (QFile::exists("/usr/share/pixmaps/" + icon_name_no_ext + ".svg")) {
            return QIcon("/usr/share/pixmaps/" + icon_name_no_ext + ".svg");
        } else if (QFile::exists("/usr/share/pixmaps/" + icon_name_no_ext + ".xpm")) {
            return QIcon("/usr/share/pixmaps/" + icon_name_no_ext + ".xpm");
        } else if (QFile::exists("/usr/share/pixmaps/" + icon_name_no_ext)) {
            return QIcon("/usr/share/pixmaps/" + icon_name_no_ext);

            // fallback to hicolor icons
        } else {
            QString name = getCmdOut("find /usr/share/icons/hicolor/ -iname \"" + icon_name_no_ext + ".svg\" -print -quit");
            if (name.isEmpty()) { // try first .png of 48x48 size
                name = getCmdOut("find /usr/share/icons/hicolor/ -iname \"" + icon_name_no_ext + ".png\" | grep -m1 48x48");
            }
            if (name.isEmpty()) { // return first .png icon found
                name = getCmdOut("find /usr/share/icons/hicolor/ -iname \"" + icon_name_no_ext + ".png\" -print -quit");
            }

            // if still empty check /usr/share/icons
            if (name.isEmpty()) {
                if (QFile::exists("/usr/share/icons/" + icon_name_no_ext + ".png")) {
                    return QIcon("/usr/share/icons/" + icon_name_no_ext + ".png");
                } else if (QFile::exists("/usr/share/icons/" + icon_name_no_ext + ".svg")) {
                    return QIcon("/usr/share/icons/" + icon_name_no_ext + ".svg");
                } else if (QFile::exists("/usr/share/icons/" + icon_name_no_ext + ".xpm")) {
                    return QIcon("/usr/share/icons/" + icon_name_no_ext + ".xpm");
                } else if (QFile::exists("/usr/share/icons/" + icon_name_no_ext)) {
                    return QIcon("/usr/share/icons/" + icon_name_no_ext);
                }
            } else {
                return QIcon(name);
            }
        }
    }
    return QIcon();
}

// run code when button is clicked
void MainWindow::btn_clicked()
{
    this->hide();
    //qDebug() << sender()->objectName();
    system(sender()->objectName().toUtf8());
    this->show();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    QSettings settings("mx-tools");
    settings.setValue("geometry", saveGeometry());
}

// hide icons in menu checkbox
void MainWindow::on_hideCheckBox_clicked(bool checked) {
    for (const QStringList &list : category_map) {
        for (const QString &file_name : list) {
            hideShowIcon(file_name, checked);
        }
    }
    system("xfce4-panel --restart");
}

// hide or show icon for .desktop file
void MainWindow::hideShowIcon(const QString &file_name, bool hide)
{
    QString hide_str = hide ? "true" : "false";
    qDebug() << "hide_str" << hide_str;
    qDebug() << "filename for hide" << file_name;
    QFileInfo file(file_name);
    qDebug() << "filename basename" << file.fileName();
    qDebug() << "filename full path" << file.filePath();

    //QString cmd = "cp " + file.filePath() + " /home/$USER/.local/share/applications";
    //QString out = getCmdOut(cmd);
    QString cmd;
    QString out;

    QString filenamehome = getCmdOut("echo /home/$USER/.local/share/applications/" + file.fileName());
    QFileInfo filenamehomeinfo(filenamehome);
    qDebug() << "filnamehome " << filenamehome;

    qDebug() << "filenamehomeinfo exists" << filenamehomeinfo.exists();
    if (filenamehomeinfo.exists()) {
        qDebug() << "filenamehomeinfo exists" << filenamehomeinfo.exists();
        //check for modified files, assume our file only has 1 line.  leave other files alone but change any hide variable
        int count = getCmdOut("wc -l " + filenamehome + "| cut -d' ' -f1").toInt();
        qDebug() << "count is " << count;
        if ( count != 1 ) {
            cmd = "cat " + filenamehome + " | grep -m1 '^NoDisplay=' | cut -d '=' -f2";
            out = getCmdOut(cmd);

            if (out.compare("true", Qt::CaseInsensitive) == 0 || out.compare("false", Qt::CaseInsensitive) == 0) {
                cmd = "sed -i 's/^NoDisplay=.*/NoDisplay=" + hide_str +  "/' " + filenamehome;
            } else { // take care of the instances when there's no "NoDisplay=" line in .desktop
                cmd = "echo 'NoDisplay=" + hide_str + "' >> " + filenamehome;
            }
        } else {
            if (hide_str == "false") {
            cmd = "rm -f " + filenamehome;
            }
        }
    } else {
        cmd = "echo NoDisplay=true >" + filenamehome;
    }
    system(cmd.toUtf8());
}

// About button clicked
void MainWindow::on_buttonAbout_clicked()
{
    this->hide();
    QMessageBox msgBox(QMessageBox::NoIcon,
                       tr("About MX Tools"), "<p align=\"center\"><b><h2>" +
                       tr("MX Tools") + "</h2></b></p><p align=\"center\">" + tr("Version: ") +
                       VERSION + "</p><p align=\"center\"><h3>" +
                       tr("Configuration Tools for MX Linux") + "</h3></p><p align=\"center\"><a href=\"http://mxlinux.org\">http://mxlinux.org</a><br /></p><p align=\"center\">" +
                       tr("Copyright (c) MX Linux") + "<br /><br /></p>", 0, this);
    QPushButton *btnLicense = msgBox.addButton(tr("License"), QMessageBox::HelpRole);
    QPushButton *btnChangelog = msgBox.addButton(tr("Changelog"), QMessageBox::HelpRole);
    QPushButton *btnCancel = msgBox.addButton(tr("Cancel"), QMessageBox::NoRole);
    btnCancel->setIcon(QIcon::fromTheme("window-close"));

    msgBox.exec();

    if (msgBox.clickedButton() == btnLicense) {
        system("mx-viewer file:///usr/share/doc/mx-tools/license.html 'MX Tools License'");
    } else if (msgBox.clickedButton() == btnChangelog) {
        QDialog *changelog = new QDialog(this);
        changelog->resize(600, 500);

        QTextEdit *text = new QTextEdit;
        text->setReadOnly(true);
        text->setText(getCmdOut("zless /usr/share/doc/" + QFileInfo(QCoreApplication::applicationFilePath()).fileName()  + "/changelog.gz"));

        QPushButton *btnClose = new QPushButton(tr("&Close"));
        btnClose->setIcon(QIcon::fromTheme("window-close"));
        connect(btnClose, &QPushButton::clicked, changelog, &QDialog::close);

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(text);
        layout->addWidget(btnClose);
        changelog->setLayout(layout);
        changelog->exec();
    }
    this->show();
}


// Help button clicked
void MainWindow::on_buttonHelp_clicked()
{
    QString cmd;

    if (QFile::exists("/usr/bin/mx-manual")) {
        cmd = QString("mx-manual");
    } else {
        cmd = QString("mx-viewer file:///usr/local/share/doc/mxum.html#toc-Subsection-3.2");
    }

    system(cmd.toUtf8());
}

// text changed in search field
void MainWindow::on_lineSearch_textChanged(const QString &arg1)
{
    // remove all items from the layout
    QLayoutItem *child;
    while ((child = ui->gridLayout_btn->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }

    QMultiMap<QString, QMultiMap<QString, QStringList> > new_map;
    QMultiMap<QString, QStringList>  map;

    // creat a new_map with items that match the search argument
    for (const QString &category : info_map.keys()) {
        //qDebug() << category;
        QMultiMap<QString, QStringList> file_info =  info_map.value(category);
        for (const QString &file_name : category_map.value(category)) {
            //qDebug() << file_name;
            QString name = file_info.value(file_name)[0];
            QString comment = file_info.value(file_name)[1];
            QString category = file_info.value(file_name)[4];
            if (name.contains(arg1, Qt::CaseInsensitive) || comment.contains(arg1, Qt::CaseInsensitive)
                    || category.contains(arg1, Qt::CaseInsensitive)) {
                map.insert(file_name, info_map.value(category).value(file_name));
            }
        }
        if (!map.empty()) {
            new_map.insert(category, map);
            map.clear();
        }
    }
    if (!new_map.empty()) {
        arg1 == "" ? addButtons(info_map) : addButtons(new_map);
    }
}

// remove Xfce-only apps from the list
void MainWindow::removeXfceOnly(QStringList &list)
{
    const QStringList list_copy = list;
    for (const QString &file_name : list_copy) {
        if (system("grep -iq 'OnlyShowIn=XFCE' "+ file_name.toUtf8()) == 0) {
            list.removeOne(file_name);
        }
    }
}

// when running live remove programs meant only for installed environments and the other way round
void MainWindow::removeEnvExclusive(QStringList &list, bool live)
{
    QString term = live ? "MX-OnlyInstalled" : "MX-OnlyLive";
    const QStringList list_copy = list;
    for (const QString &file_name : list_copy) {
        if (system("grep -iq " + term.toUtf8() + " " + file_name.toUtf8()) == 0) {
            list.removeOne(file_name);
        }
    }
}
