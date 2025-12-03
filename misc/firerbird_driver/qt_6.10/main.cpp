// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#include <qsqldriverplugin.h>
#include <qstringlist.h>
#include "qsql_firebird_p.h"

QT_BEGIN_NAMESPACE

using namespace Qt::StringLiterals;

class QFirebirdDriverPlugin : public QSqlDriverPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QSqlDriverFactoryInterface" FILE "firebird.json")

public:
    QFirebirdDriverPlugin();

    QSqlDriver* create(const QString &) override;
};

QFirebirdDriverPlugin::QFirebirdDriverPlugin()
    : QSqlDriverPlugin()
{
}

QSqlDriver* QFirebirdDriverPlugin::create(const QString &name)
{
    if (name == "QFIREBIRD"_L1) {
        QFirebirdDriver* driver = new QFirebirdDriver();
        return driver;
    }
    return 0;
}

QT_END_NAMESPACE

#include "main.moc"
