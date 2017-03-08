

QTITAN_ROOT = $$PWD/../..

include($$QTITAN_ROOT/src/shared/platform.pri)

QTITAN_LIB_PATH = $$QTITAN_ROOT/bin$$QTITAN_LIB_PREFIX
INCLUDEPATH += $$QTITAN_ROOT/include
QTN_COMPONENT_VERSION = $$fromfile($$QTITAN_ROOT/src/grid/dbgrid.pri, QTN_COMPONENT_VERSION)
DEFINES += QTN_VERSION_DATAGRID=$$QTN_COMPONENT_VERSION

win32 {
	win32-g++ {
		LIBS += -L$$QTITAN_LIB_PATH
		CONFIG(debug, debug|release):LIBS += -lqtndbgridd5
			else:LIBS += -lqtndbgrid5
	}
	else {

		CONFIG(debug, debug|release):LIBS += $$QTITAN_LIB_PATH/qtndbgridd5.lib
			else:LIBS += $$QTITAN_LIB_PATH/qtndbgrid5.lib
	}
}

unix {

    LIBS += -L$$QTITAN_LIB_PATH
    CONFIG(debug, debug|release):LIBS += -lqtndbgridd
        else:LIBS += -lqtndbgrid
}

!lessThan(QT_VER_MAJ, 5) {
  QT += widgets printsupport
}

