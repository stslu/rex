#-------------------------------------------------
#
# Project created by QtCreator 2016-09-26T10:46:43
#
#-------------------------------------------------

QT       += core gui sql printsupport

CONFIG +=  excel_lib.prf qtitangrid

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RexSensors
TEMPLATE = app
DESTDIR = bin

INCLUDEPATH += commons\
            $$PWD ..\GSL\gsl\include\
            $$PWD ..\GSL\gsl\include\gsl\
            $$PWD ..\GSL\gsl\lib

LIBS += -L$$PWD ..\GSL\gsl\lib\gsl.lib
LIBS += -L$$PWD ..\GSL\gsl\lib\cblas.lib

#The custom plot
INCLUDEPATH += $$PWD ../QCustomPlot

SOURCES += main.cpp \
    RSExportToExcel.cpp \
    RSDatabaseConfig.cpp \
    RSDataManager.cpp \
    QCustomPlot.cpp \
    RSLogicActionsManager.cpp \
    RSMainWindow.cpp \
    RSFiltersManager.cpp \
    RSOptionsManager.cpp \
    Signaler.cpp \
    RSTimesManager.cpp

HEADERS  += \
    RSExportToExcel.h \
    RSDatabaseConfig.h \
    RSDataManager.h \
    QCustomPlot.h \
    RSLogicActionsManager.h \
    Signaler.h \
    RSMainWindow.h \
    IRSMainwindow.h \
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
    RSOptionsManagerWidget.ui


RESOURCES += \
    RSDesktop.qrc

include(commons\RSCommonResources.pri)
include(computations\RSDataComputations.pri)
include(interfaces\RSInterfaceResources.pri)
include(..\QtAwesome\QtAwesome\QtAwesome.pri)

DISTFILES += \
    resources/image/img/excel.png

