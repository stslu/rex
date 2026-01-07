#include "RSDataManager.h"
#include <RSLogger.h>

RSDataManager* RSDataManager::m_instance = 0;

RSDataManager::RSDataManager(QObject *parent) : QObject(parent)
{
    createObjects();
    createConnections();
}

RSDataManager::~RSDataManager()
{
 RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
}

RSDataManager* RSDataManager::Instance(QObject *parent)
{
    if(m_instance == 0)
    {
        m_instance = new RSDataManager(parent);
    }

    return m_instance;
}

void RSDataManager::createObjects()
{

}

void RSDataManager::createConnections()
{

}

void RSDataManager::setData(const QString& key, QVariant value)
{
    RSLogger::instance()->info(Q_FUNC_INFO,QString("%1 = %2").arg(key).arg(value.toString()));
    m_dataMap.insert(key, value);
}


QVariant RSDataManager::getData(const QString& key) const
{
    return m_dataMap.value(key);
}


void RSDataManager::setFidelityForBrandMap(const QString& brand, const double& value)
{
    m_fidelityByBrandMap[brand] = value;
}


void RSDataManager::setNoiseForBrandMap(const QString& brand, const double& value)
{
    m_noiseByBrandMap[brand] = value;
}


void RSDataManager::setNoiseFidelitySensorBrandArray(const QString& brand, const QString& sensor,const double& fidelity,const double& noise)
{
    NoiseFidelitySensorBrand data;

    data.brandName = brand;
    data.sensorName = sensor;
    data.fidelity = fidelity;
    data.noise = noise;

    m_noiseFidelitySensorBrandArray.push_back(data);
}

