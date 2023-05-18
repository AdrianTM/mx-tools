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

QT       += widgets
CONFIG   += c++1z

TARGET = mx-tools
TEMPLATE = app

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
    about.cpp \
    flatbutton.cpp \
    mainwindow.cpp

HEADERS  += \
    about.h \
    flatbutton.h \
    mainwindow.h \
    version.h

FORMS    += \
    mainwindow.ui

TRANSLATIONS += translations/mx-tools_af.ts \
                translations/mx-tools_am.ts \
                translations/mx-tools_ar.ts \
                translations/mx-tools_be.ts \
                translations/mx-tools_bg.ts \
                translations/mx-tools_bn.ts \
                translations/mx-tools_bs_BA.ts \
                translations/mx-tools_bs.ts \
                translations/mx-tools_ca.ts \
                translations/mx-tools_ceb.ts \
                translations/mx-tools_co.ts \
                translations/mx-tools_cs.ts \
                translations/mx-tools_cy.ts \
                translations/mx-tools_da.ts \
                translations/mx-tools_de.ts \
                translations/mx-tools_el.ts \
                translations/mx-tools_en_GB.ts \
                translations/mx-tools_en.ts \
                translations/mx-tools_en_US.ts \
                translations/mx-tools_eo.ts \
                translations/mx-tools_es_ES.ts \
                translations/mx-tools_es.ts \
                translations/mx-tools_et.ts \
                translations/mx-tools_eu.ts \
                translations/mx-tools_fa.ts \
                translations/mx-tools_fi_FI.ts \
                translations/mx-tools_fil_PH.ts \
                translations/mx-tools_fil.ts \
                translations/mx-tools_fi.ts \
                translations/mx-tools_fr_BE.ts \
                translations/mx-tools_fr_FR.ts \
                translations/mx-tools_fr.ts \
                translations/mx-tools_fy.ts \
                translations/mx-tools_ga.ts \
                translations/mx-tools_gd.ts \
                translations/mx-tools_gl_ES.ts \
                translations/mx-tools_gl.ts \
                translations/mx-tools_gu.ts \
                translations/mx-tools_ha.ts \
                translations/mx-tools_haw.ts \
                translations/mx-tools_he_IL.ts \
                translations/mx-tools_he.ts \
                translations/mx-tools_hi.ts \
                translations/mx-tools_hr.ts \
                translations/mx-tools_ht.ts \
                translations/mx-tools_hu.ts \
                translations/mx-tools_hy.ts \
                translations/mx-tools_id.ts \
                translations/mx-tools_is.ts \
                translations/mx-tools_it.ts \
                translations/mx-tools_ja.ts \
                translations/mx-tools_jv.ts \
                translations/mx-tools_ka.ts \
                translations/mx-tools_kk.ts \
                translations/mx-tools_km.ts \
                translations/mx-tools_kn.ts \
                translations/mx-tools_ko.ts \
                translations/mx-tools_ku.ts \
                translations/mx-tools_ky.ts \
                translations/mx-tools_lb.ts \
                translations/mx-tools_lo.ts \
                translations/mx-tools_lt.ts \
                translations/mx-tools_lv.ts \
                translations/mx-tools_mg.ts \
                translations/mx-tools_mi.ts \
                translations/mx-tools_mk.ts \
                translations/mx-tools_ml.ts \
                translations/mx-tools_mn.ts \
                translations/mx-tools_mr.ts \
                translations/mx-tools_ms.ts \
                translations/mx-tools_mt.ts \
                translations/mx-tools_my.ts \
                translations/mx-tools_nb_NO.ts \
                translations/mx-tools_nb.ts \
                translations/mx-tools_ne.ts \
                translations/mx-tools_nl_BE.ts \
                translations/mx-tools_nl.ts \
                translations/mx-tools_ny.ts \
                translations/mx-tools_or.ts \
                translations/mx-tools_pa.ts \
                translations/mx-tools_pl.ts \
                translations/mx-tools_ps.ts \
                translations/mx-tools_pt_BR.ts \
                translations/mx-tools_pt.ts \
                translations/mx-tools_ro.ts \
                translations/mx-tools_rue.ts \
                translations/mx-tools_ru_RU.ts \
                translations/mx-tools_ru.ts \
                translations/mx-tools_rw.ts \
                translations/mx-tools_sd.ts \
                translations/mx-tools_si.ts \
                translations/mx-tools_sk.ts \
                translations/mx-tools_sl.ts \
                translations/mx-tools_sm.ts \
                translations/mx-tools_sn.ts \
                translations/mx-tools_so.ts \
                translations/mx-tools_sq.ts \
                translations/mx-tools_sr.ts \
                translations/mx-tools_st.ts \
                translations/mx-tools_su.ts \
                translations/mx-tools_sv.ts \
                translations/mx-tools_sw.ts \
                translations/mx-tools_ta.ts \
                translations/mx-tools_te.ts \
                translations/mx-tools_tg.ts \
                translations/mx-tools_th.ts \
                translations/mx-tools_tk.ts \
                translations/mx-tools_tr.ts \
                translations/mx-tools_tt.ts \
                translations/mx-tools_ug.ts \
                translations/mx-tools_uk.ts \
                translations/mx-tools_ur.ts \
                translations/mx-tools_uz.ts \
                translations/mx-tools_vi.ts \
                translations/mx-tools_xh.ts \
                translations/mx-tools_yi.ts \
                translations/mx-tools_yo.ts \
                translations/mx-tools_yue_CN.ts \
                translations/mx-tools_zh_CN.ts \
                translations/mx-tools_zh_HK.ts \
                translations/mx-tools_zh_TW.ts

RESOURCES += \
    images.qrc
