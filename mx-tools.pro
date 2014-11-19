# **********************************************************************
# * Copyright (C) 2014 MX Authors
# *
# * Authors: Adrian
# *          MEPIS Community <http://forum.mepiscommunity.org>
# *
# * This file is part of MX Tools.
# *
# * MX Tolls is free software: you can redistribute it and/or modify
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

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mx-tools
TEMPLATE = app


SOURCES += main.cpp\
        mxtools.cpp

HEADERS  += mxtools.h

FORMS    += mxtools.ui

TRANSLATIONS += translations/mx-findshares_ca.ts \
                translations/mx-findshares_es.ts \
                translations/mx-findshares_ja.ts \
                translations/mx-findshares_nl.ts \
                translations/mx-findshares_ro.ts \
                translations/mx-findshares_sv.ts

