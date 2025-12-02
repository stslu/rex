/****************************************************************************
**
** Qt Firebird SQL Driver Plugin
**
** This plugin provides a Qt SQL driver for Firebird/Interbase databases
** with proper charset handling for legacy databases.
**
****************************************************************************/

#include <qsqldriverplugin.h>
#include "qsql_firebird_p.h"

QT_BEGIN_NAMESPACE

class QFirebirdDriverPlugin : public QSqlDriverPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QSqlDriverFactoryInterface" FILE "firebird.json")
    
public:
    QFirebirdDriverPlugin();

    QSqlDriver *create(const QString &) override;
};

QFirebirdDriverPlugin::QFirebirdDriverPlugin()
    : QSqlDriverPlugin()
{
}

QSqlDriver *QFirebirdDriverPlugin::create(const QString &name)
{
    if (name == QLatin1String("QFIREBIRD") || name == QLatin1String("QIBASE")) {
        QFirebirdDriver *driver = new QFirebirdDriver();
        return driver;
    }
    return nullptr;
}

QT_END_NAMESPACE

#include "main.moc"
