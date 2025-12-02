#-------------------------------------------------
#
# Qt Firebird SQL Driver Plugin - CMake Configuration
#
# This plugin provides proper charset handling for 
# legacy Firebird databases using ISC_DPB_LC_CTYPE
#
#-------------------------------------------------

cmake_minimum_required(VERSION 3.16)
project(QFirebirdSqlDriverPlugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql)

# Find Firebird client library
find_library(FBCLIENT_LIBRARY NAMES fbclient fbclient_ms gds32 
    PATHS 
        /usr/lib 
        /usr/local/lib 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../inc/drivers
)

find_path(FBCLIENT_INCLUDE_DIR ibase.h
    PATHS 
        /usr/include 
        /usr/local/include
        /usr/include/firebird
)

if(NOT FBCLIENT_LIBRARY OR NOT FBCLIENT_INCLUDE_DIR)
    message(FATAL_ERROR "Firebird client library not found")
endif()

qt_add_plugin(QFirebirdSqlDriverPlugin
    CLASS_NAME QFirebirdDriverPlugin
    PLUGIN_TYPE sqldrivers
    SOURCES
        main.cpp
        qsql_firebird.cpp
        qsql_firebird_p.h
)

target_include_directories(QFirebirdSqlDriverPlugin PRIVATE
    ${FBCLIENT_INCLUDE_DIR}
    ${Qt6Sql_PRIVATE_INCLUDE_DIRS}
)

target_link_libraries(QFirebirdSqlDriverPlugin PRIVATE
    Qt6::Core
    Qt6::Sql
    Qt6::SqlPrivate
    ${FBCLIENT_LIBRARY}
)

# Installation
install(TARGETS QFirebirdSqlDriverPlugin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins/sqldrivers
)
