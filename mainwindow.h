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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMessageBox>
#include <QProcess>
#include <QMultiMap>

#include <flatbutton.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QDialog
{
    Q_OBJECT

protected:
    QProcess *proc;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    FlatButton *btn;
    QMultiMap<QString, QStringList> category_map;
    QMultiMap<QString, QMultiMap<QString, QStringList> > info_map;
    QStringList live_list;
    QStringList maintenance_list;
    QStringList setup_list;
    QStringList software_list;
    QStringList utilities_list;

    void addButtons(const QMultiMap<QString, QMultiMap<QString, QStringList> > &info_map);
    void hideShowIcon(const QString &file_name, bool hide);
    void readInfo(const QMultiMap<QString, QStringList> &category_map);

    QIcon findIcon(QString icon_name);
    QString getCmdOut(const QString &cmd);
    QStringList listDesktopFiles(const QString &search_string, const QString &location);

private slots:
    void btn_clicked();
    void closeEvent(QCloseEvent *);
    void on_buttonAbout_clicked();
    void on_buttonHelp_clicked();
    void on_hideCheckBox_clicked(bool checked);
    void on_lineSearch_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    void removeXfceOnly(QStringList &list);
    void removeFLUXBOXonly(QStringList &list);
    void removeEnvExclusive(QStringList &list, bool live);
};

#endif // MAINWINDOW_H
