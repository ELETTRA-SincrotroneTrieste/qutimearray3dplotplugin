#-------------------------------------------------
#
# Project created by QtCreator 2019-09-03T15:28:56
#
#-------------------------------------------------

isEmpty(INSTALL_ROOT)  {
    INSTALL_ROOT=/usr/local/cumbia-libs
}

# INSTALL_ROOT is used to install the target
# prefix is used within DEFINES +=
#
# cumbia installation script uses a temporary INSTALL_ROOT during build
# and then files are copied into the destination prefix. That's where
# configuration files must be found by the application when the script
# installs everything at destination
#
isEmpty(prefix) {
    prefix = $${INSTALL_ROOT}
}

include($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

CUMBIA_QTCONTROLS_PLUGIN_INTERFACES_DIR=$${INSTALL_ROOT}/include/cumbia-qtcontrols

CUMBIA_QTCONTROLS_PLUGIN_LIBDIR=$${INSTALL_ROOT}/lib/qumbia-plugins

QT       += core gui datavisualization

greaterThan(QT_MAJOR_VERSION, 5) {
# QT += graphs
}

TARGET = qutimearray3dplotplugin
TEMPLATE = lib
CONFIG += plugin

isEmpty(buildtype) {
        buildtype = release
} else {
    equals(buildtype, debug) {
        message("")
        message("debug build")
        message("")
    }
}

CONFIG += $${buildtype}

DEFINES -= QT_NO_DEBUG_OUTPUT

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
	src/qdatetime3daxisformatter.cpp \
	src/qutimearray3dplot.cpp \
        src/qutimearray3dplotplugin.cpp \
    src/qutimearray3dplotsettingswidget.cpp \
    src/qutimearrayreader.cpp

HEADERS += \
	src/qutimearray3dplot.h \
	src/qutimearray3dplotplugin.h \
	src/qutimearray3dplotplugin_i.h \
        src/qdatetime3daxisformatter.h \
    src/qutimearray3dplotsettingswidget.h \
    src/qutimearrayreader.h

INC_PATH = $${INSTALL_ROOT}/include/qumbia-plugins
inc.files = src/qutimearray3dplot.h
inc.path = $${INC_PATH}

pluginif.files = src/qutimearray3dplotplugin_i.h
pluginif.path = $${CUMBIA_QTCONTROLS_INCLUDES}

DISTFILES += qutimearrayplotplugin.json  \
    README.md

unix {
    target.path = $${CUMBIA_QTCONTROLS_PLUGIN_LIBDIR}
    INSTALLS += target inc  pluginif
}

# priority to src/ includes
INCLUDEPATH = src + $${INCLUDEPATH}

message("qutimearrayplotplugin: plugin *interface* installation dir:  $${CUMBIA_QTCONTROLS_INCLUDES}")
message("qutimearrayplotplugin: plugin installation dir:  $${CUMBIA_QTCONTROLS_PLUGIN_LIBDIR}")
message("qutimearrayplotplugin: include installation dir: $${INC_PATH}")
