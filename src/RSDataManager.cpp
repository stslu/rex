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

/*void RSDataManager::setData(DataEnum dataEnum, const QVariant& value)
{
    RSLogger::instance()->info(Q_FUNC_INFO,QString("%1 = %2").arg(dataEnum).arg(value.toString()));
    m_dataEnumMap[dataEnum] = value;
}*/



QVariant RSDataManager::getData(const QString& key) const
{
    return m_dataMap.value(key);
}

/*const QVariant& RSDataManager::getData(DataEnum dataEnum) const
{
    RSLogger::instance()->info(Q_FUNC_INFO,"return data:" + QString::number(dataEnum));
    return m_dataEnumMap[dataEnum];
}*/

void RSDataManager::setFidelityForBrandMap(const QString& brand, const double& value)
{
    m_fidelityByBrandMap[brand] = value;
}

const QMap<QString, double>& RSDataManager::getSensorFidelityByBrandMap() const
{
    return m_fidelityByBrandMap;
}

void RSDataManager::setNoiseForBrandMap(const QString& brand, const double& value)
{
    m_noiseByBrandMap[brand] = value;
}

const QMap<QString, double>& RSDataManager::getNoiseForBrandMap() const
{
    return m_noiseByBrandMap;
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

void RSDataManager::setSensorMtbfArray(const QString& brand,const QString& sensor, const QString& techno, double mtbf)
{
    MtbfBySensorBrandTechnology data;

    data.brand = brand;
    data.sensor = sensor;
    data.technology = techno;
    data.mtbf = mtbf;

    m_sensorMtbfArray.push_back(data);
}


const  QVector<RSDataManager::NoiseFidelitySensorBrand>&  RSDataManager::getNoiseFidelitySensorBrandArray() const
{
    return m_noiseFidelitySensorBrandArray;
}

const  QVector<RSDataManager::MtbfBySensorBrandTechnology>&  RSDataManager::getMtbfBySensorBrandTechnologyArray() const
{
    return m_sensorMtbfArray;
}

void RSDataManager::clearFidelityAndNoiseMap()
{
    m_noiseFidelitySensorBrandArray.clear();
}

void RSDataManager::clearMtbfMap()
{
    m_sensorMtbfArray.clear();
}
