#include "RSFidelityAndNoise.h"
#include "RSDatabaseAccess.h"
#include "RSGlobalMethods.h"
#include "RSFidelityComputation.h"
#include "RSDataManager.h"
#include "RSMessageView.h"
#include <RSLogger.h>
#include <QApplication>

RSFidelityAndNoise::RSFidelityAndNoise(RSDatabaseAccess* db) : m_databaseAccess(db)
{

}


void  RSFidelityAndNoise::computeFidelityAndNoiseFactor( const RSFidelityAndNoise::FidelitySettings& settings, bool showMsg)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    RSFidelityComputation* fidelityComputationInstance =  new RSFidelityComputation();
    //Get the parameters
    QDate startDate ;
    QDate endDate ;
    int stepDate ;
    int degree ;
    int sigma ;
    double noiseFactor ;
    int sensorCode ;

    // if(m_settings)
    RSFidelityAndNoise::FidelitySettings mySettings = settings;
    startDate = settings.startDateUi;
    endDate = settings.endDateUi;
    stepDate = settings.stepDateUi;
    degree = settings.degreeUi;;
    sigma = settings.sigmaUi;
    noiseFactor = settings.noiseFactorUi;
    sensorCode = settings.sensorCode;

    //--- --Get the m_stepViewMax
    int stepViewMax;
    {
        QDate startRelativeDate = m_databaseAccess->getAcquisitionRelativeFirstTime( startDate, endDate, sensorCode).date();

        QDate endRelativeDate = m_databaseAccess->getAcquisitionRelativeLastTime(startDate, endDate, sensorCode).date();

        stepViewMax = RSGlobalMethods::Instance()->getStepNumber( startRelativeDate, endRelativeDate, stepDate);
    }

    fidelityComputationInstance->clearFidelityStep();
    fidelityComputationInstance->clearNoiseStep();

    for(int step = 0; step <= stepViewMax; step++)
    {
        mySettings.stepViewUi = step;

        if(showMsg)
            RSMessageView::Instance()->showData(QString(("Compute averages for %1. Step %2/%3")).arg(settings.sensorName).arg(step).arg(stepViewMax));
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        //--- --Build the interval of the step. [stepStartDate, stepEndDate]
        QDate stepStartDate = RSGlobalMethods::Instance()->getLowerBoundDateOfInterval(startDate, endDate, stepDate, step);
        QDate stepEndDate = RSGlobalMethods::Instance()->getUpperBoundDateOfInterval(startDate, endDate, stepDate, step);

        //--- --In this interval, we look for the min dateTime of acquisition. startRelativeDate = min of the sensor
        QDate startRelativeDate = m_databaseAccess->getAcquisitionRelativeFirstTime(stepStartDate, stepEndDate, sensorCode).date();
        //--- --In this interval, we look for the Max dateTime of acquisition. endRelativeDate = MaxDate of the sensor
        QDate endRelativeDate = m_databaseAccess->getAcquisitionRelativeLastTime(stepStartDate, stepEndDate, sensorCode).date();

        //--- --skip If the sensor has no data in this step
        if(startRelativeDate.isNull() || endRelativeDate.isNull() || startRelativeDate >= endRelativeDate)
        {
            //no data
            fidelityComputationInstance->addFidelityStep(0.0);
            fidelityComputationInstance->addNoiseStep(0.0);
            continue;
        }


        {
            //Build the relative time for each point  in the interval. the x-axis
            QList<double> m_timeListR = m_databaseAccess->getAcquisitionTimeList(startRelativeDate, endRelativeDate, sensorCode);

            //Build the standard deviation of each point in the list
            QList<double> m_timeList = RSGlobalMethods::Instance()->getStdList(m_timeListR, qPow(10.0, 10.0));

            //Get the valus (y axis) for each point . the y-axis
            QList<double> m_valueList = m_databaseAccess->getAcquisitionValueList(startRelativeDate, endRelativeDate, sensorCode);

            //Compute the fidelity
            fidelityComputationInstance->initialize(m_timeList, m_valueList, degree, sigma, noiseFactor);
        }
        fidelityComputationInstance->compute();

        double m_fidelityByStep = fidelityComputationInstance->getFidelityStep();
        double m_noiseByStep = fidelityComputationInstance->getNoiseStep();

        fidelityComputationInstance->addFidelityStep(m_fidelityByStep);
        fidelityComputationInstance->addNoiseStep(m_noiseByStep);

    }

    //The sensor fidelity is the noise average in the interval
    double fidelity = fidelityComputationInstance->getFidelityBySensor();

    //same, the sensor noise is the noise average  in the interval
    double noise = fidelityComputationInstance->getNoiseBySensor();

    setFidelityBySensor(fidelity);

    setNoiseBySensor(noise);

    delete fidelityComputationInstance;

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSFidelityAndNoise::initialize(const QString& brandName, const QString& sensorName)
{
    setBrandName(brandName);
    setSensorName(sensorName);
}

void RSFidelityAndNoise::setBrandName(const QString& brandName)
{
    m_brandName = brandName;
}

QString RSFidelityAndNoise::getBrandName() const
{
    return m_brandName;
}

void RSFidelityAndNoise::setSensorName(const QString& sensorName)
{
    m_sensorName = sensorName;
}

QString RSFidelityAndNoise::getSensorName() const
{
    return m_sensorName;
}

void RSFidelityAndNoise::setFidelityBySensor(double fidelityBySensor)
{
    m_fidelityBySensor = fidelityBySensor;
}

double RSFidelityAndNoise::getFidelityBySensor() const
{
    return m_fidelityBySensor;
}

void RSFidelityAndNoise::setNoiseBySensor(double noiseBySensor)
{
    m_noiseBySensor = noiseBySensor;
}

double RSFidelityAndNoise::getNoiseBySensor() const
{
    return m_noiseBySensor;
}



QPair<double,double> RSFidelityAndNoise::computeFidelityNoiseStep(const RSFidelityAndNoise::FidelitySettings&  settings )
{
    QPair<double,double> fidelityNoisePair;

    RSLogger::instance()->info(Q_FUNC_INFO ,QString("Start. SensorCode = %1").arg(settings.sensorCode));

    RSFidelityComputation fidelityComputation ;

    //The lower bound date of this step
    QDate stepStartDate = RSGlobalMethods::Instance()->getLowerBoundDateOfInterval(settings.startDateUi, settings.endDateUi, settings.stepDateUi, settings.stepViewUi);

    //The upper bound date of this step
    QDate stepEndDate = RSGlobalMethods::Instance()->getUpperBoundDateOfInterval(settings.startDateUi, settings.endDateUi,  settings.stepDateUi,  settings.stepViewUi);
    RSLogger::instance()->info(Q_FUNC_INFO,QString("startDateInterval = %1 endDateInterval = %2").arg(stepStartDate.toString()).arg(stepEndDate.toString()));

    //The min date for data in this interval (the fisrt data)
    QDate startRelativeDate = m_databaseAccess->getAcquisitionRelativeFirstTime(stepStartDate, stepEndDate, settings.sensorCode).date();

    //The max date for data in this interval (The last data)
    QDate endRelativeDate = m_databaseAccess->getAcquisitionRelativeLastTime(stepStartDate, stepEndDate,  settings.sensorCode).date();
    RSLogger::instance()->info(Q_FUNC_INFO,QString("minDateInInterval = %1 maxDateInInterval = %2").arg(startRelativeDate.toString()).arg(endRelativeDate.toString()));

    //Exit if no data, or if data dateTime = last data dateTime
    if(startRelativeDate.isNull()|| endRelativeDate.isNull()|| startRelativeDate >= endRelativeDate)
    {
        RSMessageView::Instance()->showData("----- No Data-------", "REX");
        RSMessageView::Instance()->showData("StartRelativeDate: " + startRelativeDate.toString());
        RSMessageView::Instance()->showData("endRelativeDate :" + endRelativeDate.toString());
        RSMessageView::Instance()->separate();
        RSLogger::instance()->info(Q_FUNC_INFO, QString("End. Bad settings. startRelativeDate = %1 endRelativeDate = %2")
                                   .arg(startRelativeDate.toString()).arg(endRelativeDate.toString()));
        return fidelityNoisePair;
    }

    //-- The x-axis vector. It's the elapsed time for each point in interval since startRelativeDate
    QList<double> relativeTimeArray = m_databaseAccess->getAcquisitionTimeList(startRelativeDate, endRelativeDate, settings.sensorCode);//init m_startDateTime

    //-- Divide the relativeTimeArray by the  factor qPow(10.0, 10.0)
    QList<double> timeArray = RSGlobalMethods::Instance()->getStdList(relativeTimeArray, qPow(10.0, 10.0));

    //-- Get the start dateTime.  = startRelativeDate. is domputed by getAcquisitionTimeList
    QDateTime startDateTime = m_databaseAccess->getStartDateTime();
    RSLogger::instance()->info(Q_FUNC_INFO, "startDateTime = " + startDateTime.toString());

    //-- Get the values associated to each point in the interval
    QList<double> valueList = m_databaseAccess->getAcquisitionValueList(startRelativeDate, endRelativeDate, settings.sensorCode);

    //----------------------------------------------------------------------------
    //--- -- Compute the fidelity and noise
    //----------------------------------------------------------------------------
    fidelityComputation.clearVectors();

    fidelityComputation.initialize(timeArray, valueList, settings.degreeUi, settings.sigmaUi, settings.noiseFactorUi);

    fidelityComputation.compute();

    double fidelityStep = fidelityComputation.getFidelityStep();
    double noiseStep = fidelityComputation.getNoiseStep();

    fidelityNoisePair.first = fidelityStep;
    fidelityNoisePair.second = noiseStep;

    //--- --RSMessageView
    RSLogger::instance()->info(Q_FUNC_INFO, QString("End\t fidelityAvg = %1 : noiseAvg = %2").arg(fidelityStep).arg(noiseStep));

    return fidelityNoisePair;
}
