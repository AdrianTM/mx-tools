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

#include "mxtools.h"
#include "ui_mxtools.h"
#include "flatbutton.h"

#include <QFile>
//#include <QDebug>

mxtools::mxtools(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mxtools)
{
    ui->setupUi(this);
    // detect if tools are displayed in the menu (check for only one since all are set at the same time)
    if (system("grep -q \"NoDisplay=true\" /usr/share/applications/mx/mx-user.desktop") == 0) {
        ui->hideCheckBox->setChecked(true);
    }

    QString search_folder = "/usr/share/applications";
    live_list = listDesktopFiles("MX-Live", search_folder);
    maintenance_list = listDesktopFiles("MX-Maintenance", search_folder);
    setup_list = listDesktopFiles("MX-Setup", search_folder);
    software_list = listDesktopFiles("MX-Software", search_folder);
    utilities_list = listDesktopFiles("MX-Utilities", search_folder);

    QString test = getCmdOut("df -T / |tail -n1 |awk '{print $2}'");
    // remove mx-remastercc and live-kernel-updater from list if not running Live
    if (!(test == "aufs" || test == "overlay")) {
        foreach(QString item, live_list) {
            if (item.contains("mx-remastercc.desktop") || item.contains("live-kernel-updater.desktop")) {
                live_list.removeOne(item);
            }
        }
    }

    // remove item from list if it is only meant for XFCE
    if (qgetenv("XDG_CURRENT_DESKTOP") != "XFCE") {
        removeXfceOnly(live_list);
        removeXfceOnly(maintenance_list);
        removeXfceOnly(setup_list);
        removeXfceOnly(software_list);
        removeXfceOnly(utilities_list);
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
    //this->resize(this->width() + 15, this->height());
    //qDebug() << "list" << category_map;
    this->resize(ui->gridLayout_btn->sizeHint().width() + 90, this->height());
    //qDebug() << "width window" << this->width();
    //qDebug() << "width btn layout area" << ui->gridLayout_btn->sizeHint().width();
}

mxtools::~mxtools()
{
    delete ui;
}

// Util function
QString mxtools::getCmdOut(const QString &cmd) {
    proc = new QProcess(this);
    proc->start("/bin/bash", QStringList() << "-c" << cmd);
    proc->setReadChannel(QProcess::StandardOutput);
    proc->setReadChannelMode(QProcess::MergedChannels);
    proc->waitForFinished(-1);
    return proc->readAllStandardOutput().trimmed();
}

// Get version of the program
QString mxtools::getVersion(QString name) {
    QString cmd = QString("dpkg -l %1 | awk 'NR==6 {print $3}'").arg(name);
    return getCmdOut(cmd);
}


// List .desktop files that contain a specific string
QStringList mxtools::listDesktopFiles(const QString &search_string, const QString &location)
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
void mxtools::readInfo(const QMultiMap<QString, QStringList> &category_map)
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

    foreach (QString category, category_map.keys()) {
        list = category_map.value(category);
        foreach (QString file_name, list) {
            name = "";
            comment = "";
            if (lang != "en") {
                name = getCmdOut("grep -i ^'Name\\￼[" + lang + "\\]=' " + file_name + " | cut -f2 -d=");
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
void mxtools::addButtons(const QMultiMap<QString, QMultiMap<QString, QStringList> > &info_map)
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

    foreach (QString category, info_map.keys()) {
        if (!info_map.values(category).isEmpty()) {
            QLabel *label = new QLabel();
            //label->setStyleSheet("QLabel {color:black}");
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
            ui->gridLayout_btn->setRowStretch(row, 0);
            row += 1;
            foreach (QString file_name, info_map.value(category).keys()) {
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
                ui->gridLayout_btn->addWidget(btn, row, col);
                 ui->gridLayout_btn->setRowStretch(row, 0);
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
                QObject::connect(btn, SIGNAL(clicked()), this, SLOT(btn_clicked()));
            }
        }
        // add empty row if it's not the last key
        //if (category != category_map.lastKey()) {
            col = 0;
            row += 1;
            QFrame *line = new QFrame();
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            ui->gridLayout_btn->addWidget(line, row, col, 1, -1);
            ui->gridLayout_btn->setRowStretch(row, 0);
        //}
    }
    ui->gridLayout_btn->setRowStretch(row, 1);
}

// find icon by name specified in .desktop file
QIcon mxtools::findIcon(QString icon_name)
{
    // return icon if fully specified
    if (QFile("/" + icon_name).exists()) { // make sure it looks for icon in root, not in home
        return QIcon(icon_name);
    } else {
        icon_name = icon_name.remove(".png");
        icon_name = icon_name.remove(".svg");
        icon_name = icon_name.remove(".xpm");
        // return the icon from the theme if it exists
        if (QIcon::fromTheme(icon_name).name() != "") {
            return QIcon::fromTheme(icon_name);
        // return png, svg, xpm icons from /usr/share/pixmaps
        } else if (QFile("/usr/share/pixmaps/" + icon_name + ".png").exists()) {
            return QIcon("/usr/share/pixmaps/" + icon_name + ".png");
        } else if (QFile("/usr/share/pixmaps/" + icon_name + ".svg").exists()) {
            return QIcon("/usr/share/pixmaps/" + icon_name + ".svg");
        } else if (QFile("/usr/share/pixmaps/" + icon_name + ".xpm").exists()) {
            return QIcon("/usr/share/pixmaps/" + icon_name + ".xpm");
        } else if (QFile("/usr/share/pixmaps/" + icon_name).exists()) {
            return QIcon("/usr/share/pixmaps/" + icon_name);
        } else {
            return QIcon();
        }
    }
}

// run code when button is clicked
void mxtools::btn_clicked()
{
    this->hide();
    //qDebug() << sender()->objectName();
    system(sender()->objectName().toUtf8());
    this->show();
}

// hide icons in menu checkbox
void mxtools::on_hideCheckBox_clicked(bool checked) {
    foreach (QStringList list, category_map) {
        foreach (QString file_name, list) {
            hideShowIcon(file_name, checked);
        }
    }
    system("xfce4-panel --restart");
}

// hide or show icon for .desktop file
void mxtools::hideShowIcon(const QString &file_name, bool hide)
{
    QString hide_str = hide ? "true" : "false";

    QString cmd = "cat " + file_name + " | grep -m1 '^NoDisplay=' | cut -d '=' -f2";
    QString out = getCmdOut(cmd);

    if (out.compare("true", Qt::CaseInsensitive) == 0 || out.compare("false", Qt::CaseInsensitive) == 0) {
        cmd = "sed -i 's/^NoDisplay=.*/NoDisplay=" + hide_str +  "/' " + file_name;
    } else { // take care of the instances when there's no "NoDisplay=" line in .desktop
        cmd = "echo 'NoDisplay=" + hide_str + "' >> " + file_name;
    }
    system("su-to-root -X -c \"" + cmd.toUtf8() + "\"");
}

// About button clicked
void mxtools::on_buttonAbout_clicked()
{
    this->hide();
    QMessageBox msgBox(QMessageBox::NoIcon,
                       tr("About MX Tools"), "<p align=\"center\"><b><h2>" +
                       tr("MX Tools") + "</h2></b></p><p align=\"center\">" + tr("Version: ") +
                       getVersion("mx-tools") + "</p><p align=\"center\"><h3>" +
                       tr("Configuration Tools for MX Linux") + "</h3></p><p align=\"center\"><a href=\"http://mxlinux.org\">http://mxlinux.org</a><br /></p><p align=\"center\">" +
                       tr("Copyright (c) MX Linux") + "<br /><br /></p>", 0, this);
    msgBox.addButton(tr("License"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole)
        system("mx-viewer file:///usr/share/doc/mx-tools/license.html 'MX Tools License'");
    this->show();
}


// Help button clicked
void mxtools::on_buttonHelp_clicked()
{
    this->hide();
    QString cmd = QString("mx-viewer file:///usr/local/share/doc/mxum.html#toc-Subsection-3.2");
    system(cmd.toUtf8());
    this->show();
}

// text changed in search field
void mxtools::on_lineSearch_textChanged(const QString &arg1)
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
    foreach (QString category, info_map.keys()) {
        //qDebug() << category;
        QMultiMap<QString, QStringList> file_info =  info_map.value(category);
        foreach (QString file_name, category_map.value(category)) {
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
void mxtools::removeXfceOnly(QStringList &list)
{
    foreach(QString file_name, list) {
        if (system("grep -iq 'OnlyShowIn=XFCE' "+ file_name.toUtf8()) == 0) {
            list.removeOne(file_name);
        }
    }
}

