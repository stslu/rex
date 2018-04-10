#ifndef RSDATAMANAGER_H
#define RSDATAMANAGER_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QSharedPointer>
#include <RSFiltersManager.h>
#include <QVariantList>

enum MeasPointType
{
    AcqPoint = 0,
    Node ,
    MeasPointTypeCount
};


class SensorInfos
{
public:
    int   code;
    int mpCode;//All measurse points who use this sensor
    QString name;
    QString brand;
    QString model;
    QString technology;
    QString outputSignal;
    QString measRange;
    QString physicalMeas;
    QString theoricalAccuracy;
    QString unit;
    MeasPointType measPointType;

    QString toString() const
    {
        return QString("Sensor = %1 mpCode = %2").arg(name).arg(mpCode);
    }
};


struct RexStatistics
{
    int sensorCode;
    int mpCode;
    QString sensorName;
    MeasPointType measPointType;
    QString brand;
    QString model;
    QString technology;
    QString physicalMeasurment;
    QString outputSignal;
    QString measurementRange;
    QString theoricalAccuracy;
    QString unit;


    QString experimentation;
    double noise;
    double fidelity;
    qint64 avgFailSec;

    static QString technologyColName(){return "Technology";}
    static QString brandColName(){return "Brand";}
    static QString outputSignalColName(){return "O. Signal";}
    static QString fidelityColName(){return "Fidelity";}
    static QString noiseColName(){return "Noise";}
    static QString failuresColName(){return "Avg Fail.Days";}
    static QString physicalMeasColName(){return "Phys. Meas.";}
    static QString measurementRangeColName(){return "Meas. range";}
    static QString sensorCodeColName(){return "Sensor code";}
    static QString sensorMeasPointType(){return "Meas. Point Type";}
    static QString sensorNameColName(){return "Sensor name";}
    static QString mpCodeColName(){return "Meas. Pt code";}
    static QString mpNameColName(){return "Meas. Pt name";}
    static QString modelColName(){return "Model";}
    static QString unitColName(){return "Unit";}
    static QString theoricalAccuracyColName(){return "Accuracy";}

    static QStringList fieldsList()
    {
        return  QStringList() << sensorNameColName()
                              << sensorCodeColName()
//                              << sensorMeasPointType()
                              << mpCodeColName()
                              << fidelityColName()
                              << noiseColName()
                              << failuresColName()
                              << brandColName()
                              << modelColName()
                              << technologyColName()
                              << physicalMeasColName()
                              << outputSignalColName()
                              << measurementRangeColName()
                              << theoricalAccuracyColName()
                              << unitColName();
    }

    QVariantList toQVariantList() const
    {
        QVariantList data;
        data.append(sensorName);
        data.append(sensorCode);
//        data.append(static_cast<int>(measPointType));
        data.append(mpCode);
        data.append(fidelity);
        data.append(noise);
        data.append(avgFailSec/(24*3600.0));//Convert in days
        data.append(brand);
        data.append(model);
        data.append(technology);
        data.append(physicalMeasurment);
        data.append(outputSignal);
        data.append(measurementRange);
        data.append(theoricalAccuracy);
        data.append(unit);
        return data;
    }

    QString toString() const
    {
        return QString ("sensorName = %1\n "
                        "sensorCode = %2\n "
//                        "measPointType = %10\n "
                        "brand = %3\n "
                        "model = %4\n "
                        "technology = %5\n "
                        "outputSignal = %6\n "
                        "fidelity = %7\n"
                        "noise = %8\n "
                        "avgFailSec = %9")
                .arg(sensorName)
                .arg(sensorCode)
//                .arg(static_cast<int>(measPointType))
                .arg(brand)
                .arg(model)
                .arg(technology)
                .arg(outputSignal)
                .arg(QString::number(fidelity))
                .arg(noise)
                .arg(avgFailSec/(24*3600*1000.0));
    }

    RexStatistics():sensorCode(0),noise(0),fidelity(0),avgFailSec(0)
    {

    }
};


class RSDataManager : public QObject
{
public:


    struct NoiseFidelitySensorBrand
    {
        double noise;
        double fidelity;
        QString technology;
        QString sensorName;
        QString brandName;


        QString toString()
        {
            return QString("Brand= %1\t sensor = %2 \t fidelity = %3 \t noise = %4")
                    .arg(brandName).arg(sensorName).arg(fidelity).arg(noise);
        }
    };

    struct MtbfBySensorBrandTechnology
    {
        double mtbf;
        QString sensor;
        QString brand;
        QString technology;
        QString outputSignal;

        QString toString()
        {
            return QString("Brand= %1\t sensor = %2 \t technology = %3 \t mtbf = %4")
                    .arg(brand).arg(sensor).arg(technology).arg(mtbf);
        }
    };

    Q_OBJECT

private:
    RSDataManager(QObject *parent = 0);

public:
    ~RSDataManager();
    static RSDataManager* Instance(QObject *parent = 0);

    void setFidelityForBrandMap(const QString& brand, const double& value);
    const QMap<QString, double>& getSensorFidelityByBrandMap() const;

    void setNoiseForBrandMap(const QString& sensor, const double& value);
    const QMap<QString, double>& getNoiseForBrandMap() const;

    void setNoiseFidelitySensorBrandArray(const QString& brand, const QString& sensor,const double& fidelity,const double& noise);
    const  QVector<RSDataManager::NoiseFidelitySensorBrand>& getNoiseFidelitySensorBrandArray() const;

    const  QVector<RSDataManager::MtbfBySensorBrandTechnology>& getMtbfBySensorBrandTechnologyArray() const;

    void setSensorMtbfArray(const QString& brand,const QString& sensor, const QString& techno, double mtbf);

    void clearFidelityAndNoiseMap();
    void clearMtbfMap();

private:

    void createObjects();

    void createConnections();

public:
    void setData(const QString& key, QVariant value);
    //void setData(DataEnum dataEnum, const QVariant& value);

    QVariant getData(const QString& key) const;
    //const QVariant& getData(DataEnum) const;


private:
    static RSDataManager *m_instance;

    QMap<QString, QVariant> m_dataMap;

    QMap<QString, double> m_fidelityByBrandMap;

    QMap<QString, double> m_noiseByBrandMap;

    QVector<NoiseFidelitySensorBrand> m_noiseFidelitySensorBrandArray;

    QVector<MtbfBySensorBrandTechnology> m_sensorMtbfArray;


};
#endif // RSDATAMANAGER_H
