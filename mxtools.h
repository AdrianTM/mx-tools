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

#ifndef MXTOOLS_H
#define MXTOOLS_H

#include <flatbutton.h>
#include <QMessageBox>
#include <QProcess>
#include <QMultiMap>


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

    FlatButton *btn;
    QMultiMap<QString, QStringList> category_map;
    QMultiMap<QString, QMultiMap<QString, QStringList> > info_map;
    QStringList live_list;
    QStringList maintenance_list;
    QStringList setup_list;
    QStringList software_list;
    QStringList utilities_list;

    void readInfo(QMultiMap<QString, QStringList>);
    void addButtons(QMultiMap<QString, QMultiMap<QString, QStringList> > info_map);
    void hideShowIcon(QString file_name, bool hide);
    QIcon findIcon(QString icon_name);
    QString getCmdOut(QString cmd);
    QString getVersion(QString name);
    QStringList listDesktopFiles(QString search_string, QString location);

private slots:
    void on_buttonAbout_clicked();
    void on_buttonHelp_clicked();
    void btn_clicked();
    void on_hideCheckBox_clicked(bool checked);

    void on_lineSearch_textChanged(const QString &arg1);

private:
    Ui::mxtools *ui;
};

#endif // MXTOOLS_H
