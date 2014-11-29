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

#ifndef MXTOOLS_H
#define MXTOOLS_H

#include <QMessageBox>
#include <QProcess>


namespace Ui {
class mxtools;
}

class mxtools : public QDialog
{
    Q_OBJECT

protected:
    QProcess *proc;

public:
    explicit mxtools(QWidget *parent = 0);
    ~mxtools();

    QString getCmdOut(QString cmd);
    QString getVersion(QString name);    
    void checkApps();

private slots:
    void on_buttonAbout_clicked();
    void on_buttonUser_clicked();
    void on_buttonPackageInstaller_clicked();
    void on_buttonCodecs_clicked();
    void on_buttonFlash_clicked();
    void on_buttonCheckAptGPG_clicked();
    void on_buttonAptNotifier_clicked();
    void on_buttonFindShares_clicked();
    void on_buttonSwitchUser_clicked();
    void on_buttonBootrepair_clicked();
    void on_hideCheckBox_clicked(bool checked);

private:
    Ui::mxtools *ui;
};

#endif // MXTOOLS_H
