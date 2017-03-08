#ifndef RSDATABASECREATE_H
#define RSDATABASECREATE_H

#include <QObject>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QMap>

class RSDatabaseCreate : public QObject
{
    Q_OBJECT

private:
    RSDatabaseCreate(QObject *parent = 0);

public:
    ~RSDatabaseCreate();
    static RSDatabaseCreate* Instance(QObject *parent = 0);

private:
    void createObjects();
    void createConnections();

    void addScriptFileName(const QString& databaseName, const QString& scriptFileName);
    void loadScript(const QString& databaseName, const QString& scriptFileName);

public:
    void createDatabase(const QString &databaseName);

private:
    static RSDatabaseCreate* m_instance;

    QMap<QString, QString> m_scriptFileNameMap;
};






#endif // RSDATABASECREATE_H
