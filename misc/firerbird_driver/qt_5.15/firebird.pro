TARGET = qsqlfirebird

HEADERS += $$PWD/qsql_firebird_p.h
SOURCES += $$PWD/qsql_firebird.cpp $$PWD/main.cpp

# FIXME: ignores libfb (unix)/fbclient (win32) - but that's for the test anyway
#QMAKE_USE += firebird

OTHER_FILES += firebird.json

PLUGIN_CLASS_NAME = QFirebirdDriverPlugin
include(../qsqldriverbase.pri)
