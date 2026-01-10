#-------------------------------------------------
#
# Qt Firebird SQL Driver Plugin
#
# This plugin provides proper charset handling for 
# legacy Firebird databases using ISC_DPB_LC_CTYPE
#
#-------------------------------------------------

TARGET = qsqlfirebird

TEMPLATE = lib
CONFIG += plugin

QT = core sql sql-private

HEADERS += qsql_firebird_p.h

SOURCES += qsql_firebird.cpp \
           main.cpp

OTHER_FILES += firebird.json

# Firebird client library
# Adjust path as needed for your system
win32 {
    LIBS += -lfbclient
    # Or specify full path:
    # LIBS += -L$$PWD/../../../inc/drivers -lfbclient
}
unix {
    LIBS += -lfbclient
}

PLUGIN_TYPE = sqldrivers
PLUGIN_CLASS_NAME = QFirebirdDriverPlugin
load(qt_plugin)
