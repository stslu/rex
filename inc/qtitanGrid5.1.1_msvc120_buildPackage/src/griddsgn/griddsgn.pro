TEMPLATE    = lib

TARGET      = qtngriddsgn

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

include($$PWD/../shared/platform.pri)
include($$PWD/../grid/dbgrid.pri)

DESTDIR = $$PWD/../../bin
DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX

VERSION = $$QTN_COMPONENT_VERSION

QMAKE_TARGET_COMPANY = Developer Machines
QMAKE_TARGET_PRODUCT = QtitanDataGrid Design-Time
QMAKE_TARGET_DESCRIPTION = QtitanDataGrid plugin for QtDesigner
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2009-2015 Developer Machines

contains(CONFIG, demo) {
    QMAKE_TARGET_DESCRIPTION = QtitanDataGrid plugin for QtDesigner (Demo version)
}

DSGN_HEADERS     += $$PWD/QtnGridDsgnPlugin.h \
               $$PWD/QtnPlugins.h

DSGN_SOURCES     += $$PWD/QtnGridDsgnPlugin.cpp \
               $$PWD/QtnPlugins.cpp

DSGN_RESOURCES += \
    $$PWD/QtnGridDsgnResources.qrc

HEADERS += $$DSGN_HEADERS
SOURCES += $$DSGN_SOURCES
RESOURCES += $$DSGN_RESOURCES

DEFINES += QTITAN_LIBRARY_STATIC
DEFINES += QTITAN_DESIGNER

CONFIG += plugin

!lessThan(QT_VER_MAJ, 5) {
  QT += widgets printsupport core-private widgets-private gui-private designer
} else {
  DLLDESTDIR = $$PWD/../../bin
  DLLDESTDIR = $$member(DLLDESTDIR, 0)$$QTITAN_LIB_PREFIX
  CONFIG += designer
}



