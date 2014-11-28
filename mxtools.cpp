/**********************************************************************
 * Copyright (C) 2014 MX Authors
 *
 * Authors: Adrian
 *          MEPIS Community <http://forum.mepiscommunity.org>
 *
 * This file is part of MX Tools.
 *
 * MX Tolls is free software: you can redistribute it and/or modify
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


mxtools::mxtools(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mxtools)
{
    ui->setupUi(this);
    checkApps();
    // detect if tools are displayed in the menu (check for only one since all are set at the same time)
    if (system("grep -q \"NoDisplay=true\" /usr/share/applications/mx/mx-user.desktop") == 0) {
        ui->hideCheckBox->setChecked(true);
    }
}

mxtools::~mxtools()
{
    delete ui;
}

// Util function
QString mxtools::getCmdOut(QString cmd) {
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

// Check if the apps are installed
void mxtools::checkApps() {
    // MX User
    if (getCmdOut("dpkg -s mx-user | grep Status") != "Status: install ok installed") {
        ui->buttonUser->setEnabled(false);
    }
    // MX PackageInstaller
    if (getCmdOut("dpkg -s mx-packageinstaller | grep Status") != "Status: install ok installed") {
        ui->buttonPackageInstaller->setEnabled(false);
    }
    // MX Codecs
    if (getCmdOut("dpkg -s mx-codecs | grep Status") != "Status: install ok installed") {
        ui->buttonCodecs->setEnabled(false);
    }
    // MX Flash
    if (getCmdOut("dpkg -s mx-flash | grep Status") != "Status: install ok installed") {
        ui->buttonFlash->setEnabled(false);
    }
    // MX CheckAptGPG
    if (getCmdOut("dpkg -s checkaptgpg | grep Status") != "Status: install ok installed") {
        ui->buttonCheckAptGPG->setEnabled(false);
    }
    // MX AptNotifier
    if (getCmdOut("dpkg -s apt-notifier | grep Status") != "Status: install ok installed") {
        ui->buttonAptNotifier->setEnabled(false);
    }
    // MX Findshares
    if (getCmdOut("dpkg -s mx-findshares | grep Status") != "Status: install ok installed") {
        ui->buttonFindShares->setEnabled(false);
    }
    // MX Switchuser
    if (getCmdOut("dpkg -s mx-switchuser | grep Status") != "Status: install ok installed") {
        ui->buttonSwitchUser->setEnabled(false);
    }
    // MX BootRepair
    if (getCmdOut("dpkg -s mx-bootrepair | grep Status") != "Status: install ok installed") {
        ui->buttonBootrepair->setEnabled(false);
    }

}


// About button clicked
void mxtools::on_buttonAbout_clicked() {
    QMessageBox msgBox(QMessageBox::NoIcon,
                       tr("About MX Tools"), "<p align=\"center\"><b><h2>" +
                       tr("MX Tools") + "</h2></b></p><p align=\"center\">" + tr("Version: ") +
                       getVersion("mx-tools") + "</p><p align=\"center\"><h3>" +
                       tr("Configuration Tools for MX Linux") + "</h3></p><p align=\"center\"><a href=\"http://www.mepiscommunity.org/mx\">http://www.mepiscommunity.org/mx</a><br /></p><p align=\"center\">" +
                       tr("Copyright (c) antiX") + "<br /><br /></p>", 0, this);
    msgBox.addButton(tr("License"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("Cancel"), QMessageBox::DestructiveRole);
    if (msgBox.exec() == QMessageBox::AcceptRole)
        system("mx-viewer http://www.mepiscommunity.org/doc_mx/mx-tools-license.html 'MX Tools License'");
}


void mxtools::on_buttonUser_clicked() {
    this->hide();
    system("su-to-root -X -c mx-user");
    this->show();
}

void mxtools::on_buttonPackageInstaller_clicked() {
    this->hide();
    system("su-to-root -X -c mx-packageinstaller");
    this->show();
}

void mxtools::on_buttonCodecs_clicked() {
    this->hide();
    system("su-to-root -X -c mx-codecs");
    this->show();
}

void mxtools::on_buttonFlash_clicked() {
    this->hide();
    system("su-to-root -X -c mx-flash");
    this->show();
}

void mxtools::on_buttonCheckAptGPG_clicked() {
    this->hide();
    system("su-to-root -X -c \"xfce4-terminal -e 'bash checkaptgpg --wait-at-end'\" 2>/dev/null");
    this->show();
}

void mxtools::on_buttonAptNotifier_clicked() {
    this->hide();
    system("/usr/bin/apt-notifier-unhide-Icon");
    this->show();
}

void mxtools::on_buttonFindShares_clicked() {
    this->hide();
    system("mx-findshares");
    this->show();
}

void mxtools::on_buttonSwitchUser_clicked() {
    this->hide();
    system("mx-switchuser");
    this->show();
}

void mxtools::on_buttonBootrepair_clicked() {
    this->hide();
    system("su-to-root -X -c mx-bootrepair");
    this->show();
}

void mxtools::on_hideCheckBox_clicked() {
    system("su-to-root -X -c /usr/share/mx-tools/hideshowicons.sh");
}
