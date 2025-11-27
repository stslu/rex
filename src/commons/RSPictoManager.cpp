#include "RSPictoManager.h"
#include <QApplication>

using fa::QtAwesome;

RSPictoManager* RSPictoManager::m_instance = 0;

RSPictoManager::RSPictoManager(QObject *parent) : QObject(parent)
{
    createObjects();
    createConnections();
}

RSPictoManager::~RSPictoManager()
{

}

RSPictoManager* RSPictoManager::Instance(QObject *parent)
{
    if(m_instance == 0)
    {
        m_instance = new RSPictoManager(parent);
    }

    return m_instance;
}

void RSPictoManager::createObjects()
{
    m_awesomePicto = new QtAwesome(qApp);
    m_awesomePicto->initFontAwesome();
}

void RSPictoManager::createConnections()
{

}

void RSPictoManager::setColor(const QVariant &color)
{
    m_awesomePicto->setDefaultOption("color", color);
}

void RSPictoManager::setActiveColor(const QVariant &color)
{
    m_awesomePicto->setDefaultOption("color-active", color);
}

QIcon RSPictoManager::getIcon(fa::fa_common_icons faEnum, const QVariant &color)
{
   setColor(color);

   return m_awesomePicto->icon(fa::fa_solid, faEnum);
}
