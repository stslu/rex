#include "RSDataComputationTheo.h"
#include "RSDataGenerator.h"
#include "RSMessageView.h"
#include "RSGraphView.h"
#include "RSFidelityComputation.h"
#include "RSMeanTimeBetweenFailuresComputation.h"
#include "IRSDatabaseAccess.h"
#include "RSDataManager.h"

RSDataComputationTheo* RSDataComputationTheo::m_instance = 0;

RSDataComputationTheo::RSDataComputationTheo(QObject *parent) : QObject(parent)
{
    createObjects();
    createConnections();
}

RSDataComputationTheo::~RSDataComputationTheo()
{

}

RSDataComputationTheo* RSDataComputationTheo::Instance(QObject *parent)
{
    if(m_instance == 0)
    {
        m_instance = new RSDataComputationTheo(parent);
    }

    return m_instance;
}

void RSDataComputationTheo::createObjects()
{
    RSDataGenerator::Instance(this);
    RSFidelityComputation::Instance(this);
}

void RSDataComputationTheo::createConnections()
{

}

void RSDataComputationTheo::run()
{
    int m_degree = RSDataManager::Instance()->getData("Trend").toInt();
    int m_sigma = RSDataManager::Instance()->getData("Sigma").toInt();
    double m_noiseFactor = RSDataManager::Instance()->getData("NoiseFactor").toDouble();

    int m_sensorStep = 10;
    int m_sensorsModel = 10;
    int m_sensorsType = 10;

    //double m_minX = qDegreesToRadians(-180.0);
    //double m_maxX = qDegreesToRadians(180.0);

    double m_minX = -1*qPow(10, 10);
    double m_maxX = +1*qPow(10, 10);

    /* Fidelity & Noise Computation */
    double m_fideliy;
    QString m_fidelityName;
    double m_noise;
    QString m_noiseName;

    QList<double> m_xR = RSDataGenerator::Instance()->X(m_minX, m_maxX, 100);
    QList<double> m_x = RSGlobalMethods::Instance()->getStdList(m_xR, 100000000);
    QList<double> m_yC = RSDataGenerator::Instance()->dataFunctionFx(m_x);
    QList<double> m_yR = RSDataGenerator::Instance()->dataFunctionFx(m_x, 10);
    QList<double> m_xS;
    QList<double> m_yT;

    RSFidelityComputation::Instance()->initialize(m_x, m_yR, m_degree, m_sigma, m_noiseFactor);
    RSFidelityComputation::Instance()->compute();

    RSMessageView::Instance()->showData(m_degree, "m_degree");
    RSMessageView::Instance()->showData(m_sigma, "m_sigma");
    RSMessageView::Instance()->showData(m_noiseFactor, "m_noiseFactor");
    RSMessageView::Instance()->separate();
    RSMessageView::Instance()->showData(m_xR, "m_x", 10);
    RSMessageView::Instance()->separate();
    /*
    // fidelity
    m_fideliy = RSFidelityComputation::Instance()->getFidelityStep();
    //RSFidelityComputation::Instance()->addFidelityStep(m_fideliy);

    m_fidelityName = QString("FidelityStep(Sensor[%1]: Step[%2])")
            .arg(sensorsModel).arg(sensorStep);
    //RSMessageView::Instance()->showData(m_fideliy, m_fidelityName);

    // noise
    m_noise = RSFidelityComputation::Instance()->getNoiseStep();
    RSFidelityComputation::Instance()->addNoiseStep(m_noise);

    m_noiseName = QString("NoiseStep(Sensor[%1]: Step[%2])")
            .arg(sensorsModel).arg(sensorStep);
    //RSMessageView::Instance()->showData(m_noise, m_noiseName);*/

    RSGraphView::Instance()->clearGraphs();

    RSGraphView::Instance()->add(m_xR, m_yC, "Trend");
    RSGraphView::Instance()->add(m_xR, m_yR, "Trend");

    m_yT = RSFidelityComputation::Instance()->getTrend();
    RSGraphView::Instance()->add(m_xR, m_yT, "Trend");

    m_yT = RSFidelityComputation::Instance()->getBollingerMin();
    RSGraphView::Instance()->add(m_xR, m_yT, "Trend", 3);

    m_yT = RSFidelityComputation::Instance()->getBollingerMax();
    RSGraphView::Instance()->add(m_xR, m_yT, "Trend", 3);

    m_yT = RSFidelityComputation::Instance()->getDeviation();
    m_xS = RSFidelityComputation::Instance()->getSigmas();
    RSGraphView::Instance()->add(m_xR, m_yT, "Deviation");


    /*
    for(int sensorsType = 0; sensorsType < m_sensorsType; sensorsType++)
    {
        RSFidelityComputation::Instance()->clearFidelityBySensor();
        RSFidelityComputation::Instance()->clearNoiseBySensor();

        for(int sensorsModel = 0; sensorsModel < m_sensorsModel; sensorsModel++)
        {
            RSFidelityComputation::Instance()->clearFidelityStep();
            RSFidelityComputation::Instance()->clearNoiseStep();

            for(int sensorStep = 0; sensorStep < m_sensorStep; sensorStep++)
            {
                QList<double> m_x = RSDataGenerator::Instance()->X(m_minX, m_maxX, 1001);
                QList<double> m_yC = RSDataGenerator::Instance()->dataFunctionFx(m_x);
                QList<double> m_yR = RSDataGenerator::Instance()->dataFunctionFx(m_x, 10);

                RSFidelityComputation::Instance()->initialize(m_x, m_yR, m_degree, m_sigma, m_noiseFactor);
                RSFidelityComputation::Instance()->compute();

                // fidelity
                m_fideliy = RSFidelityComputation::Instance()->getFidelityStep();
                RSFidelityComputation::Instance()->addFidelityStep(m_fideliy);

                m_fidelityName = QString("FidelityStep(Sensor[%1]: Step[%2])")
                        .arg(sensorsModel).arg(sensorStep);
                //RSMessageView::Instance()->showData(m_fideliy, m_fidelityName);

                // noise
                m_noise = RSFidelityComputation::Instance()->getNoiseStep();
                RSFidelityComputation::Instance()->addNoiseStep(m_noise);

                m_noiseName = QString("NoiseStep(Sensor[%1]: Step[%2])")
                        .arg(sensorsModel).arg(sensorStep);
                //RSMessageView::Instance()->showData(m_noise, m_noiseName);

                if(sensorStep == 0 && sensorsModel == 0 && sensorsType == 0)
                {
                    RSGraphView::Instance()->add(m_x, m_yC);
                    RSGraphView::Instance()->add(m_x, m_yR);

                    m_yT = RSFidelityComputation::Instance()->getTrend();
                    RSGraphView::Instance()->add(m_x, m_yT, "Trend");

                    m_yT = RSFidelityComputation::Instance()->getBollingerMin();
                    RSGraphView::Instance()->add(m_x, m_yT, "Trend", 3);

                    m_yT = RSFidelityComputation::Instance()->getBollingerMax();
                    RSGraphView::Instance()->add(m_x, m_yT, "Trend", 3);

                    m_yT = RSFidelityComputation::Instance()->getDeviation();
                    m_xS = RSFidelityComputation::Instance()->getSigmas();
                    RSGraphView::Instance()->add(m_xS, m_yT, "Deviation");
                }
            }

            // fidelity
            m_fideliy = RSFidelityComputation::Instance()->getFidelityBySensor();
            RSFidelityComputation::Instance()->addFidelityBySensor(m_fideliy);

            m_fidelityName = QString("FidelityBySensor(Sensor[%1])--->")
                    .arg(sensorsModel);
            //RSMessageView::Instance()->showData(m_fideliy, m_fidelityName);

            // noise
            m_noise = RSFidelityComputation::Instance()->getNoiseBySensor();
            RSFidelityComputation::Instance()->addNoiseBySensor(m_noise);

            m_noiseName = QString("NoiseBySensor(Sensor[%1])--->")
                    .arg(sensorsModel);
            //RSMessageView::Instance()->showData(m_noise, m_noiseName);
        }


        // fidelity
        double m_fidelityMin = RSFidelityComputation::Instance()->getFidelityMin();
        double m_fidelityMax = RSFidelityComputation::Instance()->getFidelityMax();
        double m_fidelityAverage = RSFidelityComputation::Instance()->getFidelityAverage();

        QString m_fidelityValue = QString("Min: %1 | Average: %2 | Max: %3")
                .arg(m_fidelityMin).arg(m_fidelityAverage).arg(m_fidelityMax);

        m_fidelityName = QString("Fidelity(SensorType[%1])--->")
                .arg(sensorsType);
        RSMessageView::Instance()->showData(m_fidelityValue, m_fidelityName);

        // noise
        double m_noiseMin = RSFidelityComputation::Instance()->getNoiseMin();
        double m_noiseMax = RSFidelityComputation::Instance()->getNoiseMax();
        double m_noiseAverage = RSFidelityComputation::Instance()->getNoiseAverage();

        QString m_noiseValue = QString("Min: %1 | Average: %2 | Max: %3")
                .arg(m_noiseMin).arg(m_noiseAverage).arg(m_noiseMax);

        m_noiseName = QString("Noise(SensorType[%1])--->")
                .arg(sensorsType);
        RSMessageView::Instance()->showData(m_noiseValue, m_noiseName);
    }*/
}

