TEMPLATE = lib

TARGET = qtndbgrid

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

include($$PWD/../platform.pri)
include($$PWD/../../grid/dbgrid.pri)

contains(CONFIG, demo) {
    include($$PWD/../qtitandemo.pri)
}

DESTDIR = $$PWD/../../../bin
DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX

VERSION = $$QTN_COMPONENT_VERSION

QMAKE_TARGET_COMPANY = Developer Machines
QMAKE_TARGET_PRODUCT = QtitanDataGrid
QMAKE_TARGET_DESCRIPTION = QtitanDataGrid UI component for Digia Qt.C++
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2009-2016 Developer Machines

!lessThan(QT_VER_MAJ, 5) {
  QT += widgets printsupport core-private widgets-private gui-private
} else {
  DLLDESTDIR = $$PWD/../../../bin
  DLLDESTDIR = $$member(DLLDESTDIR, 0)$$QTITAN_LIB_PREFIX
}

QTITAN_ROOT=$$PWD/../../..
COMPONENT_LANGUAGES = de_de fr_fr it_it ru_ru pl_pl
COMPONENT_SOURCE_TREE=$$QTITAN_ROOT/src/grid
COMPONENT_TRANSLATIONS_DIR=$$QTITAN_ROOT/translations/grid

include($$QTITAN_ROOT/src/shared/translations.pri)

DEFINES += QTITAN_LIBRARY

