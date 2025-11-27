
#include <QGlobal.h>
#include <QTime>

#include "RSDataComputation.h"
#include "RSDataGenerator.h"
#include "RSMessageView.h"
#include "RSGraphView.h"
#include "RSFidelityComputation.h"
#include "RSDatabaseAccess.h"
#include "RSDataManager.h"
#include "RSFidelityAndNoise.h"

#include <RSLogger.h>


RSDataComputation::RSDataComputation(RSDatabaseAccess* dbAccess,
                                     RSFidelityAndNoise::FidelitySettings& settings,
                                     IRSGraphView* graphView,QObject *parent) : QObject(parent)
  , m_settings(settings)
  , m_databaseAccess(dbAccess)
  , m_graphView(graphView)
{
    createObjects();
    createConnections();
}

void RSDataComputation::createObjects()
{

}

void RSDataComputation::createConnections()
{

}

void  RSDataComputation::runFidelityNoiseAndMtbfCalculations(const QList<SensorInfos>& map
                                                             ,const RSFidelityAndNoise::FidelitySettings&  settings
                                                             , IRSGraphView* graphView
                                                             , QVector<RexStatistics>& statsArray
                                                             )
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");
    statsArray.clear();
    int progress = 0;
    bool showFidelityMsg = false;

    m_settings = settings;
    // QDateTime minDate(m_settings.startDateUi) ;
    // QDateTime maxDate(m_settings.endDateUi) ;
    QDateTime minDate(m_settings.startDateUi, QTime(0, 0, 0)); // Qt 6.10
    QDateTime maxDate(m_settings.endDateUi,   QTime(0, 0, 0));

    quint64 gapMsec = minDate.msecsTo(maxDate);
    QString msg = QString("minDate = %1\t maxDate : %2 gapMsec = %3 \n settings: %4")
            .arg(minDate.toString())
            .arg(maxDate.toString())
            .arg(gapMsec)
            .arg(settings.toString());
    RSLogger::instance()->info(Q_FUNC_INFO,msg);

    int maxProgress = map.count();
    if(maxProgress == 1)
        showFidelityMsg = true;

    Q_FOREACH(const SensorInfos& sensorInfo,map)
    {
        //The sensorName
        ++progress;
        if(graphView)
            graphView->setProgressBar((progress/(float)maxProgress) );

        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        //We display the progression
        RSLogger::instance()->info(Q_FUNC_INFO,QString("%1 / %2. Sensor = %3 mpCode = %4 ")
                                   .arg(progress).
                                   arg(maxProgress)
                                   .arg(sensorInfo.code)
                                   .arg(sensorInfo.mpCode));
        RexStatistics stat;

        stat.brand = sensorInfo.brand;
        stat.technology = sensorInfo.technology;
        stat.sensorName = sensorInfo.name;
        stat.sensorCode = sensorInfo.code;
        stat.outputSignal = sensorInfo.outputSignal;
        stat.model = sensorInfo.model;
        stat.mpCode = sensorInfo.mpCode;
        stat.unit = sensorInfo.unit;
        stat.theoricalAccuracy = sensorInfo.theoricalAccuracy;
        stat.physicalMeasurment= sensorInfo.physicalMeas;
        stat.measurementRange = sensorInfo.measRange;

        //--- --Display message
        {
            QString dateTime = QDateTime::currentDateTime().toString("yyyy_MM_dd, hh:mm:ss");
            QString msg = QString(tr("Step %1/%2 .Brand= %3 Sensor= %4 \t %5")).
                    arg(progress).arg(maxProgress).arg(stat.brand).arg(stat.sensorName).arg(dateTime);
            RSLogger::instance()->info(Q_FUNC_INFO,msg);
            RSMessageView::Instance()->showData(msg);
        }
        //--- --Update the sensor code. Other settings does not change

        //Compute the fidelity
        m_settings.sensorCode = sensorInfo.code;
        m_settings.sensorName = sensorInfo.name;
        {
            RSFidelityAndNoise fidelityByBrandThread (m_databaseAccess);

            RSLogger::instance()->info(Q_FUNC_INFO,"initialize Fidelity");
            fidelityByBrandThread.initialize(stat.brand, sensorInfo.name);

            RSLogger::instance()->info(Q_FUNC_INFO,"run computeFidelityAndNoiseFactor");
            fidelityByBrandThread.computeFidelityAndNoiseFactor(m_settings, showFidelityMsg);

            stat.fidelity = fidelityByBrandThread.getFidelityBySensor();
            stat.noise = fidelityByBrandThread.getNoiseBySensor();
        }

        //--- --Compute the average failures for this period
        RSLogger::instance()->info(Q_FUNC_INFO,"Compute Failures");
        {
            QList<quint64> failuresList;

            RSLogger::instance()->info(Q_FUNC_INFO,"Get the MTBF");
            bool succeeded = m_databaseAccess->initSensorFailureList(
                        stat.mpCode,m_settings.startDateUi,m_settings.endDateUi,2 /*evtCode*/,failuresList);

            if(!succeeded)
            {
                break;
                RSLogger::instance()->info(Q_FUNC_INFO,"Stop the calculations. Failed to get the failures");
            }

            stat.avgFailSec = 0;
            if(failuresList.count())
                stat.avgFailSec = std::accumulate(failuresList.begin(),failuresList.end(),0)/failuresList.count();

            RSLogger::instance()->info(Q_FUNC_INFO, stat.toString());
        }

        statsArray.push_back(stat);

        {
            const QString msg = QString("\t fidelity = %1 noise = %2 avgFailuresSec = %3\n").arg(stat.fidelity).arg(stat.noise).arg(stat.avgFailSec);
            RSLogger::instance()->info(Q_FUNC_INFO,stat.toString());
            RSMessageView::Instance()->showData(msg);
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        if(graphView)
            graphView->setProgressBar((progress/(float)maxProgress) );



    }

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSDataComputation::addFidelitiesAndNoiseInDataManager(RSFidelityAndNoise *fidelityByBrandThread)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    if(!fidelityByBrandThread)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End. qobject_cast Fail.");
        return;
    }

    QString brand = fidelityByBrandThread->getBrandName();
    double fidelity = fidelityByBrandThread->getFidelityBySensor();

    QString sensor = fidelityByBrandThread->getSensorName();
    double noise = fidelityByBrandThread->getNoiseBySensor();

    RSMessageView::Instance()->showData(QString("%1 : %2 : %3 (FID) : %4 (%)")
                                        .arg(brand).arg(sensor)
                                        .arg(fidelity).arg(noise));

    //Store the value in the RSDataManager
    RSLogger::instance()->info(Q_FUNC_INFO,QString("brand = %1 sensor = %2 fidelity = %3 noise = %4")
                               .arg(brand).arg(sensor).arg(fidelity).arg(noise));

    RSDataManager::Instance()->setFidelityForBrandMap(brand,fidelity);
    RSDataManager::Instance()->setNoiseForBrandMap(brand,noise);
    RSDataManager::Instance()->setNoiseFidelitySensorBrandArray(brand,sensor,fidelity,noise);

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}



QPair<double,double> RSDataComputation::computeFidelityNoiseStepAndInitGraphView(bool  addGraphs,const RSFidelityAndNoise::FidelitySettings&  settings , bool showMsg )
{
    QPair<double,double> fidelityNoisePair;

    RSLogger::instance()->info(Q_FUNC_INFO ,"Start");

    RSFidelityComputation* rsFidelityComputation = new RSFidelityComputation();

    //The lower bound date of this step
    QDate stepStartDate = RSGlobalMethods::Instance()->getLowerBoundDateOfInterval(settings.startDateUi, settings.endDateUi, settings.stepDateUi, settings.stepViewUi);

    //The upper bound date of this step
    QDate stepEndDate = RSGlobalMethods::Instance()->getUpperBoundDateOfInterval(settings.startDateUi, settings.endDateUi,  settings.stepDateUi,  settings.stepViewUi);
    RSLogger::instance()->info(Q_FUNC_INFO,QString("startDateInterval = %1 endDateInterval = %2").arg(stepStartDate.toString()).arg(stepEndDate.toString()));

    //The min date for data in this interval (the fisrt data)
    QDate startRelativeDate = m_databaseAccess->getAcquisitionRelativeFirstTime(stepStartDate, stepEndDate, settings.sensorCode,settings.measPointType).date();

    //The max date for data in this interval (The last data)
    QDate endRelativeDate = m_databaseAccess->getAcquisitionRelativeLastTime(stepStartDate, stepEndDate,  settings.sensorCode,settings.measPointType).date();
    RSLogger::instance()->info(Q_FUNC_INFO,QString("minDateInInterval = %1 maxDateInInterval = %2").arg(startRelativeDate.toString()).arg(endRelativeDate.toString()));

    //Exit if no data, or if data dateTime = last data dateTime
    if(startRelativeDate.isNull()|| endRelativeDate.isNull()|| startRelativeDate >= endRelativeDate)
    {
        m_graphView->clearGraphs(stepStartDate, stepEndDate);
        RSMessageView::Instance()->showData("----- No Data-------", "REX");
        RSMessageView::Instance()->showData("StartRelativeDate: " + startRelativeDate.toString());
        RSMessageView::Instance()->showData("endRelativeDate :" + endRelativeDate.toString());
        RSMessageView::Instance()->separate();
        RSLogger::instance()->info(Q_FUNC_INFO, QString("End. Bad settings. startRelativeDate = %1 endRelativeDate = %2")
                                   .arg(startRelativeDate.toString()).arg(endRelativeDate.toString()));
        return fidelityNoisePair;
    }

    //-- The x-axis vector. It's the elapsed time for each point in interval since startRelativeDate
    RSLogger::instance()->info(Q_FUNC_INFO, "Build relativeTimeArray");
    QList<double> relativeTimeArray = m_databaseAccess->getAcquisitionTimeList(startRelativeDate, endRelativeDate, settings.sensorCode,settings.measPointType);//init m_startDateTime

    //-- Divide the relativeTimeArray by the  factor qPow(10.0, 10.0)
    RSLogger::instance()->info(Q_FUNC_INFO, "Build timeArray");
    QList<double> timeArray = RSGlobalMethods::Instance()->getStdList(relativeTimeArray, qPow(10.0, 10.0));

    //-- Get the start dateTime.  = startRelativeDate. is domputed by getAcquisitionTimeList
    QDateTime startDateTime = m_databaseAccess->getStartDateTime();
    RSLogger::instance()->info(Q_FUNC_INFO, "startDateTime = " + startDateTime.toString());

    //-- Get the values associated to each point in the interval
    RSLogger::instance()->info(Q_FUNC_INFO, "Build valueList");
    QList<double> valueList = m_databaseAccess->getAcquisitionValueList(startRelativeDate, endRelativeDate, settings.sensorCode,settings.measPointType);

    //----------------------------------------------------------------------------
    //--- -- Compute the fidelity and noise
    //----------------------------------------------------------------------------
    RSLogger::instance()->info(Q_FUNC_INFO, " RSFidelityComputation : clearVectors");
    rsFidelityComputation->clearVectors();

    RSLogger::instance()->info(Q_FUNC_INFO, "Init RSFidelityComputation");
    rsFidelityComputation->initialize(timeArray, valueList, settings.degreeUi, settings.sigmaUi, settings.noiseFactorUi);

    RSLogger::instance()->info(Q_FUNC_INFO, "rsFidelityComputation->compute");
    rsFidelityComputation->compute();

    double fidelityStep = rsFidelityComputation->getFidelityStep();
    double noiseStep = rsFidelityComputation->getNoiseStep();

    fidelityNoisePair.first = fidelityStep;
    fidelityNoisePair.second = noiseStep;

    //--- --RSMessageView
    RSLogger::instance()->info(Q_FUNC_INFO, QString("fidelityStep = %1 : noiseStep = %2").arg(fidelityStep).arg(noiseStep));
    if(showMsg)
    {
        int dataRelativeSize = m_databaseAccess->getAcquisitionValueSize(startRelativeDate, endRelativeDate, settings.sensorCode,settings.measPointType);
        RSLogger::instance()->info(Q_FUNC_INFO,"init RSMessageView. showData");
        RSMessageView::Instance()->showData(settings.stepViewUi, "stepView");
        RSMessageView::Instance()->showData(stepStartDate, "stepStartDate", "MM-dd-yyyy");
        RSMessageView::Instance()->showData(stepEndDate, "stepEndDate", "MM-dd-yyyy");
        RSMessageView::Instance()->showData(startRelativeDate, "startRelativeDate", "MM-dd-yyyy");
        RSMessageView::Instance()->showData(endRelativeDate, "endRelativeDate", "MM-dd-yyyy");
        RSMessageView::Instance()->showData(settings.degreeUi, "degree");
        RSMessageView::Instance()->showData(settings.sigmaUi, "sigma");
        RSMessageView::Instance()->showData( settings.noiseFactorUi, "noiseFactor");
        RSMessageView::Instance()->showData(startDateTime, "startDateTime", "yyyy-MM-dd hh:mm:ss");
        RSMessageView::Instance()->separate();
        RSMessageView::Instance()->showData(dataRelativeSize, "dataRelativeSize");
        RSMessageView::Instance()->separate();
        RSMessageView::Instance()->showData(fidelityStep, "fidelityStep", "(FID)");
        RSMessageView::Instance()->showData(noiseStep, "noise Step", "(%)");

        RSMessageView::Instance()->separate();
    }

    //--- --Plot Graphics
    if(addGraphs)
    {
        m_graphView->clearGraphs();

        //--- -- Add the graphic of evolutions. (value/Time)
        RSLogger::instance()->info(Q_FUNC_INFO, "Add empty trend Graph");
        m_graphView->add(relativeTimeArray, valueList, startDateTime, "Trend");

        // Trend
        //----------------
        if(settings.plotAvgFidelity)
        {
            RSLogger::instance()->info(Q_FUNC_INFO, "Add Trend curve");
            QList<double> m_yList = rsFidelityComputation->getTrend();
            m_graphView->add(relativeTimeArray, m_yList, "Trend");
        }

        //Minimum fidelity curve
        //----------------
        if(settings.plotMinFidelity)
        {
            RSLogger::instance()->info(Q_FUNC_INFO, "Add Trend curve bollingerMin");
            QList<double> m_yList = rsFidelityComputation->getBollingerMin();
            m_graphView->add(relativeTimeArray, m_yList, "Trend", 2);
        }

        //Maximum curve
        //----------------
        if(settings.plotMaxFidelity)
        {
            RSLogger::instance()->info(Q_FUNC_INFO, "Add Trend curve bollingerMax");
            QList<double> m_yList = rsFidelityComputation->getBollingerMax();
            m_graphView->add(relativeTimeArray, m_yList, "Trend", 2);
        }

        //Deviation curves
        //----------------
        {
            RSLogger::instance()->info(Q_FUNC_INFO, "Add deviation curve");
            QList<double> m_yList = rsFidelityComputation->getDeviation();
            QList<double> m_xList = rsFidelityComputation->getSigmas();
            m_graphView->add(m_xList, m_yList, "Deviation");
        }
    }


    delete rsFidelityComputation;
    RSLogger::instance()->info(Q_FUNC_INFO,"End");

    return fidelityNoisePair;
}

