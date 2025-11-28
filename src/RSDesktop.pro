#-------------------------------------------------
#
# Project created by QtCreator 2016-09-26T10:46:43
#
#-------------------------------------------------

QT       += core gui sql printsupport widgets

CONFIG +=  libxl qtitangrid qcustomplot gsl qtawesome

TARGET = RexSensors
TEMPLATE = app
DESTDIR = bin

#INCLUDEPATH += commons\
#            $$PWD ..\GSL\gsl\include\
#            $$PWD ..\GSL\gsl\include\gsl\
#            $$PWD ..\GSL\gsl\lib



SOURCES += main.cpp \
    RSExportToExcel.cpp \
    RSDatabaseConfig.cpp \
    RSDataManager.cpp \
    RSLogicActionsManager.cpp \
    RSMainWindow.cpp \
    RSFiltersManager.cpp \
    RSOptionsManager.cpp \
    Signaler.cpp \
    RSTimesManager.cpp \
    commons/RSMessageView.cpp

HEADERS  += \
    RSExportToExcel.h \
    RSDatabaseConfig.h \
    RSDataManager.h \
    RSLogicActionsManager.h \
    Signaler.h \
    RSMainWindow.h \
    IRSMainwindow.h \
    commons/RSMessageView.h \
    irsexportmanager.h \
    IRSFiltersManager.h \
    RSFiltersManager.h \
    RSOptionsManager.h \
    IRSOptionsManager.h \
    IRSTimesManager.h \
    RSTimesManager.h

FORMS += \
    RSMainWindow.ui \
    RSTimesManager.ui \
    RSFiltersManager.ui \
    RSOptionsManager.ui \
    RSDatabaseConfig.ui \
    RSOptionsManagerWidget.ui \
    commons/RSMessageView.ui


RESOURCES += \
    RSDesktop.qrc

include(commons\RSCommonResources.pri)
include(computations\RSDataComputations.pri)
include(interfaces\RSInterfaceResources.pri)
#include(..\QtAwesome\QtAwesome\QtAwesome.pri)

DISTFILES += \
    resources/image/img/excel.png

