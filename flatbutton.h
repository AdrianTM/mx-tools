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

#ifndef FLATBUTTON_H
#define FLATBUTTON_H

#include <QEvent>
#include <QPushButton>

class FlatButton : public QPushButton
{
    Q_OBJECT
public:
    explicit FlatButton(QWidget *parent = nullptr);
    explicit FlatButton(const QString &name, QWidget *parent = nullptr);
    void setIconSize(int, int);
    void setIconSize(QSize size);

protected:
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
};

#endif // FLATBUTTON_H
