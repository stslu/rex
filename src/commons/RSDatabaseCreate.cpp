#include "RSDatabaseCreate.h"

#include <QFile>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QTextStream>

#include "Signaler.h"

RSDatabaseCreate* RSDatabaseCreate::m_instance = 0;

RSDatabaseCreate::RSDatabaseCreate(QObject *parent) : QObject(parent)
{
    createObjects();
    createConnections();
}

RSDatabaseCreate::~RSDatabaseCreate()
{

}

RSDatabaseCreate* RSDatabaseCreate::Instance(QObject *parent)
{
    if(m_instance == 0)
    {
        m_instance = new RSDatabaseCreate(parent);
    }

    return m_instance;
}

void RSDatabaseCreate::createObjects()
{
    addScriptFileName("G6", ":/sql/RexSensorsG6.sql");
    addScriptFileName("G7", ":/sql/RexSensorsG7.sql");
}

void RSDatabaseCreate::createConnections()
{

}

void RSDatabaseCreate::addScriptFileName(const QString& databaseName, const QString& scriptFileName)
{
    m_scriptFileNameMap.insert(databaseName, scriptFileName);
}

void RSDatabaseCreate::loadScript(const QString& databaseName, const QString& scriptFileName)
{
    QFile m_file(scriptFileName);
    bool m_open = m_file.open(QIODevice::ReadOnly);

    if (m_open == false)
    {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red",
                     tr("Error %1 Database").arg(databaseName),
                     tr("%1 database script is not loaded.").arg(databaseName));
        return;
    }

    QStringList m_queryList = QTextStream(&m_file).readAll().split(';');

    foreach (QString m_queryItem, m_queryList)
    {
        QString m_querySimplified = m_queryItem.trimmed();
        bool m_isEmpty = m_querySimplified.isEmpty();

        if (m_isEmpty == true)
        {
            continue;
        }

        QSqlDatabase m_databaseSql = QSqlDatabase::database(databaseName);
        QSqlQuery m_querySql(m_databaseSql);
        bool m_exec = m_querySql.exec(m_querySimplified);

        if (m_exec == false)
        {
            emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red",
                         tr("Error %1 Database").arg(databaseName),
                         tr("%1 database script is not executed.").arg(databaseName));
            return;
         }

        m_querySql.finish();
     }
}

void RSDatabaseCreate::createDatabase(const QString& databaseName)
{
    QString m_scriptFileName = m_scriptFileNameMap.value(databaseName);
    loadScript(databaseName, m_scriptFileName);
}
