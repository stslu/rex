

INCLUDEPATH += $$quote($$PWD/../../include)
INCLUDEPATH += $$quote($$PWD/../base)

ALL_SOURCES = \
    $$PWD/QtnGridDef.cpp \
    $$PWD/QtnGridGraphics.cpp \
    $$PWD/QtnGridTableViewBase.cpp \
    $$PWD/QtnGrid.cpp \
    $$PWD/QtnGridEditors.cpp \
    $$PWD/QtnGridTableView.cpp \
    $$PWD/QtnGridBandedTableView.cpp \
    $$PWD/QtnGridModelController.cpp \
    $$PWD/QtnGridFilter.cpp \
    $$PWD/QtnGridFilterWindow.cpp \
    $$PWD/QtnGridViewGraphics.cpp \
    $$PWD/QtnGridCardView.cpp

PUBLIC_HEADERS = \
    $$PWD/QtnGridDef.h \
    $$PWD/QtnGridGraphics.h \
    $$PWD/QtnGridTableViewBase.h \
    $$PWD/QtnGrid.h \
    $$PWD/QtnGridEditors.h \
    $$PWD/QtnGridTableView.h \
    $$PWD/QtnGridBandedTableView.h \
    $$PWD/QtnGridModelController.h \
    $$PWD/QtnGridFilter.h \
    $$PWD/QtnGridFilterWindow.h \
    $$PWD/QtnGridViewGraphics.h \
    $$PWD/QtnGridCardView.h

PRIVATE_HEADERS =

WINDOWS_SOURCES = $$PWD/QtnGridGraphics_win.cpp
LINUX_SOURCES = $$PWD/QtnGridGraphics_linux.cpp
MACOSX_SOURCES = $$PWD/QtnGridGraphics_mac.mm

RESOURCES += \
    $$PWD/QtnGridResources.qrc \
    $$PWD/QtnGridTranslations.qrc


QT += xml

