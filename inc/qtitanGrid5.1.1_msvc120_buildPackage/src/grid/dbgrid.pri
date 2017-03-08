
QTN_COMPONENT_VERSION = 5.1.1
DEFINES += QTN_VERSION_DATAGRID=$$QTN_COMPONENT_VERSION

INCLUDEPATH += $$quote($$PWD/../../include)

include($$PWD/grid.pri)

ALL_SOURCES += \
    $$PWD/QtnDBGrid.cpp \
    $$PWD/QtnDBGridBandedTableView.cpp \
    $$PWD/QtnDBGridTableView.cpp

PUBLIC_HEADERS += \
    $$PWD/QtnDBGrid.h \
    $$PWD/QtnDBGridBandedTableView.h \
    $$PWD/QtnDBGridTableView.h

INCLUDE_HEADERS += \
    $$PWD/../../include/QtitanDBGrid.h \
    $$PWD/../../include/QtitanDef.h \
    $$PWD/../../include/QtitanGrid.h \
    $$PWD/../../include/QtnDBGrid.h \
    $$PWD/../../include/QtnDBGridBandedTableView.h \
    $$PWD/../../include/QtnDBGridTableView.h \
    $$PWD/../../include/QtnGrid.h \
    $$PWD/../../include/QtnGridBandedTableView.h \
    $$PWD/../../include/QtnGridFilter.h \
    $$PWD/../../include/QtnGridTableView.h \
    $$PWD/../../include/QtnGridViewGraphics.h \
    $$PWD/../../include/QtnGridCardView.h

HEADERS += $$PRIVATE_HEADERS
HEADERS += $$PUBLIC_HEADERS

SOURCES += \
    $$ALL_SOURCES

win32:SOURCES += $$WINDOWS_SOURCES
linux*:SOURCES += $$LINUX_SOURCES
macx:OBJECTIVE_SOURCES += $$MACOSX_SOURCES

macx {
    LIBS += -framework AppKit
}

win32 {
    DEFINES += _WINDOWS
    LIBS += -luser32 -lgdi32 -ladvapi32 -lshlwapi
}

QT += sql
