# Qt Firebird SQL Driver Plugin with Charset Fix

This is a custom Qt SQL driver plugin for Firebird/Interbase databases that properly handles the `ISC_DPB_LC_CTYPE` connection option to support legacy databases with non-UTF8 encodings.

## Problem

The standard Qt QFIREBIRD driver always uses `QString::fromUtf8()` for text data, which corrupts accented characters in databases using charset "NONE" or Latin1-compatible encodings.

**Example:** The string "Série 4000" (stored correctly in the database with `é` = `0xE9` in Latin1) is read as "S�rie 4000" because the driver forces UTF-8 conversion.

## Solution

This plugin:

1. Stores the charset setting from `ISC_DPB_LC_CTYPE` connection option
2. Uses the appropriate text decoder (Latin1 or UTF8) when reading string data from `SQL_VARYING` and `SQL_TEXT` columns
3. Properly handles arrays of strings with the same charset awareness

## Supported Charsets

The following charsets will use **Latin1 decoding** (for legacy databases):
- `NONE`
- `OCTETS`
- `WIN1252` / `CP1252`
- `ISO8859_1` / `ISO-8859-1` / `LATIN1`
- `ISO8859_15` / `ISO-8859-15`
- `ASCII`
- `ANSI`

All other charsets (including `UTF8`) will use **UTF-8 decoding**.

## Usage

```cpp
QSqlDatabase db = QSqlDatabase::addDatabase("QFIREBIRD");
db.setDatabaseName("mydb.fdb");
db.setUserName("SYSDBA");
db.setPassword("masterkey");

// For legacy databases with Latin1/Windows-1252 encoding:
db.setConnectOptions("ISC_DPB_LC_CTYPE=WIN1252");
// or
db.setConnectOptions("ISC_DPB_LC_CTYPE=NONE");

// For modern UTF-8 databases:
db.setConnectOptions("ISC_DPB_LC_CTYPE=UTF8");

if (db.open()) {
    QSqlQuery query("SELECT name FROM products");
    while (query.next()) {
        // Accented characters like "Série 4000" will now be correct
        QString name = query.value(0).toString();
        qDebug() << name;
    }
}
```

## Building

### Using qmake
```bash
cd src/plugins/QFirebirdSqlDriverPlugin
qmake
make
```

### Using CMake
```bash
cd src/plugins/QFirebirdSqlDriverPlugin
mkdir build && cd build
cmake ..
make
```

### Prerequisites

- Qt 6.x with SQL module
- Firebird client library (fbclient)
- Firebird header files (ibase.h)

## Installation

Copy the built plugin (`qsqlfirebird.dll` on Windows, `libqsqlfirebird.so` on Linux) to your Qt installation's `plugins/sqldrivers` directory.

## Key Changes from Standard Driver

1. **`QFirebirdDriverPrivate::TextCodec` enum**: Defines `UTF8` and `Latin1` codec types

2. **`QFirebirdDriverPrivate::textCodec` member**: Stores the codec to use for string decoding

3. **`open()` function**: Parses `ISC_DPB_LC_CTYPE` and sets `textCodec` appropriately

4. **`decodeString()` helper**: Decodes strings using the appropriate charset

5. **`gotoNext()` function**: Uses `decodeString()` for `SQL_VARYING` and `SQL_TEXT` types

6. **`readArrayBuffer()` function**: Uses `decodeString()` for array elements

## Files

- `qsql_firebird.cpp` - Main driver implementation
- `qsql_firebird_p.h` - Private header with class declarations
- `main.cpp` - Plugin entry point
- `firebird.json` - Plugin metadata
- `QFirebirdSqlDriverPlugin.pro` - qmake project file
- `CMakeLists.txt` - CMake build file
