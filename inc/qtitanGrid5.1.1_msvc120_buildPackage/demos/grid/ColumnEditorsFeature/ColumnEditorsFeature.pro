TEMPLATE = app

QTITANDIR = $$quote($$(QTITANDIR))

isEmpty(QTITANDIR):QTITANDIR = $$quote($$PWD/../../../)

include($$QTITANDIR/src/shared/qtitangrid.pri)

DESTDIR = $$QTITANDIR/bin
DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX
DESTDIR = "$$DESTDIR"

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

HEADERS      += window.h \
                pieview.h

SOURCES      += main.cpp \
                window.cpp \
                pieview.cpp

RESOURCES     = chart.qrc

include($$PWD/../../shared/demomainwindow.pri)

macx {
    CONFIG-=app_bundle
}


