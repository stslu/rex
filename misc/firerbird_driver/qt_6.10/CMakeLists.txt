cmake_minimum_required(VERSION 3.16)
project(QFirebirdDriver)

find_package(Qt6 REQUIRED COMPONENTS Core Sql)
find_package(Qt6 REQUIRED COMPONENTS CorePrivate SqlPrivate)

# --- CONFIGURATION FIREBIRD ---
set(FIREBIRD_PATH "D:/Tools/Firebird/Firebird_5_0" CACHE PATH "D:/Tools/Firebird/Firebird_5_0")

add_library(Firebird::Client UNKNOWN IMPORTED)
set_target_properties(Firebird::Client PROPERTIES
    IMPORTED_LOCATION "${FIREBIRD_PATH}/lib/fbclient_ms.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${FIREBIRD_PATH}/include"
)

# --- CREATION DU PLUGIN ---
# On active l'automatisation de Qt (Moc, Rcc, Uic)
set(CMAKE_AUTOMOC ON)

qt_add_plugin(QFirebirdDriverPlugin
    OUTPUT_NAME qsqlfirebird
    CLASS_NAME QFirebirdDriverPlugin
    main.cpp
    qsql_firebird.cpp
    qsql_firebird_p.h
)

# --- CONFIGURATION DU SUFFIXE DEBUG ---
# DEBUG_POSTFIX applique le "d" à la DLL, au LIB et au PDB automatiquement
# uniquement en mode Debug.
set_target_properties(QFirebirdDriverPlugin PROPERTIES
    DEBUG_POSTFIX "d"
)

# --- LIENS ---
target_link_libraries(QFirebirdDriverPlugin PRIVATE
    Qt6::Core
    Qt6::CorePrivate
    Qt6::Sql
    Qt6::SqlPrivate
    Firebird::Client
)

target_compile_definitions(QFirebirdDriverPlugin PRIVATE
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_TO_ASCII
)

# --- INSTALLATION ---
#install(TARGETS QFirebirdDriverPlugin
#        DESTINATION "${QT6_INSTALL_PLUGINS}/sqldrivers"
#)

# --- INSTALLATION ---
# On installe simplement dans "plugins/sqldrivers"
# Ce chemin sera relatif au répertoire d'installation qu'on définira en ligne de commande.
install(TARGETS QFirebirdDriverPlugin
        DESTINATION "plugins/sqldrivers"
)
