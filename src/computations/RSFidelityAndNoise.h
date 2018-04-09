#ifndef RSFidelityAndNoise_H
#define RSFidelityAndNoise_H

#include <QDate>
#include "RSDatabaseAccess.h"

class RSDatabaseAccess;

class RSFidelityAndNoise
{
public:

    struct FidelitySettings
    {
        QDate startDateUi ;
        QDate endDateUi ;
        int stepDateUi ;
        int stepViewUi ;
        int degreeUi ;
        int sigmaUi ;
        double noiseFactorUi ;
        int sensorCode ;
        QString sensorName;
        MeasPointType measPointType;

        bool plotMinFidelity;
        bool plotMaxFidelity;
        bool plotAvgFidelity;

        bool plotMinNoise;
        bool plotMaxNoise;
        bool plotAvgNoise;

        bool plotMinMTBF;
        bool plotMaxMTBF;
        bool plotAvgMTBF;

        QString toString() const
        {
            return QString("startDate = %1 endDate = %2 stepDate = %3 stepView = %4 degree= %5 sigma=%6 noiseFactor=%7 sensorCode=%8 measPointType=%9")
                    .arg(startDateUi.toString())
                    .arg(endDateUi.toString())
                    .arg(stepDateUi)
                    .arg(stepViewUi)
                    .arg(degreeUi)
                    .arg(sigmaUi)
                    .arg(noiseFactorUi)
                    .arg(sensorCode).
                     arg(measPointType) ;
        }

        FidelitySettings():plotMinFidelity(true)
          , plotMaxFidelity(true)
          , plotAvgFidelity(true)
          , plotMinNoise(true)
          , plotMaxNoise(true)
          , plotAvgNoise(true)
          , plotMinMTBF(true)
          , plotMaxMTBF(true)
          , plotAvgMTBF(true)
        {}
    };

    RSFidelityAndNoise(RSDatabaseAccess* dbAccess);

    void initialize(const QString& brandName, const QString& sensorName);

    QString getBrandName() const;

    QString getSensorName() const;

    double getFidelityBySensor() const;

    double getNoiseBySensor() const;

    /**
     * @brief computeFidelityAndNoiseFactor : compute the fidelity and noise factor from the settings
     */
    void computeFidelityAndNoiseFactor(const RSFidelityAndNoise::FidelitySettings& settings, bool showMsg = false);

    QPair<double,double> computeFidelityNoiseStep(const RSFidelityAndNoise::FidelitySettings&  settings );

    void setSensorName(const QString& sensorName);
    void setBrandName(const QString& brandName);

protected:
    void setNoiseBySensor(double noiseBySensor);
    void setFidelityBySensor(double fidelityBySensor);



private:
    QString m_brandName;
    QString m_sensorName;
    double m_fidelityBySensor;
    double m_noiseBySensor;
    RSDatabaseAccess* m_databaseAccess;
};

#endif // RSFidelityAndNoise_H
