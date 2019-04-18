# **********************************************************************
# * Copyright (C) 2014 MX Authors
# *
# * Authors: Adrian
# *          MX Linux <http://mxlinux.org>
# *
# * This file is part of MX Tools.
# *
# * MX Tools is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * MX Tools is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with MX Tools.  If not, see <http://www.gnu.org/licenses/>.
# **********************************************************************

#-------------------------------------------------
#
# Project created by QtCreator 2014-11-18T20:36:30
#
#-------------------------------------------------

QT       += core gui

CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mx-tools
TEMPLATE = app


SOURCES += main.cpp\
    flatbutton.cpp \
    mainwindow.cpp

HEADERS  += \
    flatbutton.h \
    mainwindow.h

FORMS    += \
    mainwindow.ui

TRANSLATIONS += translations/mx-tools_am.ts \
                translations/mx-tools_ar.ts \
                translations/mx-tools_bg.ts \
                translations/mx-tools_ca.ts \
                translations/mx-tools_cs.ts \
                translations/mx-tools_da.ts \
                translations/mx-tools_de.ts \
                translations/mx-tools_el.ts \
                translations/mx-tools_es.ts \
                translations/mx-tools_et.ts \
                translations/mx-tools_eu.ts \
                translations/mx-tools_fa.ts \
                translations/mx-tools_fi.ts \
                translations/mx-tools_fr.ts \
                translations/mx-tools_he_IL.ts \
                translations/mx-tools_hi.ts \
                translations/mx-tools_hr.ts \
                translations/mx-tools_hu.ts \
                translations/mx-tools_id.ts \
                translations/mx-tools_is.ts \
                translations/mx-tools_it.ts \
                translations/mx-tools_ja.ts \
                translations/mx-tools_ja_JP.ts \
                translations/mx-tools_kk.ts \
                translations/mx-tools_ko.ts \
                translations/mx-tools_lt.ts \
                translations/mx-tools_mk.ts \
                translations/mx-tools_nb.ts \
                translations/mx-tools_nl.ts \
                translations/mx-tools_pl.ts \
                translations/mx-tools_pt.ts \
                translations/mx-tools_pt_BR.ts \
                translations/mx-tools_ro.ts \
                translations/mx-tools_ru.ts \
                translations/mx-tools_sk.ts \
                translations/mx-tools_sl.ts \
                translations/mx-tools_sq.ts \
                translations/mx-tools_sr.ts \
                translations/mx-tools_sv.ts \
                translations/mx-tools_tr.ts \
                translations/mx-tools_uk.ts \
                translations/mx-tools_zh_CN.ts \
                translations/mx-tools_zh_TW.ts

RESOURCES += \
    images.qrc

