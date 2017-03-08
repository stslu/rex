#include "RSStylesManager.h"
#include <QApplication>
#include <QFile>

RSStylesManager* RSStylesManager::m_instance = 0;

RSStylesManager::RSStylesManager(QObject *parent) : QObject(parent)
{
    createObjects();
    createConnections();
}

RSStylesManager::~RSStylesManager()
{

}

RSStylesManager* RSStylesManager::Instance(QObject *parent)
{
    if(m_instance == 0)
    {
        m_instance = new RSStylesManager(parent);
    }

    return m_instance;
}

void RSStylesManager::createObjects()
{
    QFile m_file(":/css/RexSensors.css");
    m_file.open(QFile::ReadOnly);
    QString m_styleSheet = QLatin1String(m_file.readAll());
    qApp->setStyleSheet(m_styleSheet);
}

void RSStylesManager::createConnections()
{

}
