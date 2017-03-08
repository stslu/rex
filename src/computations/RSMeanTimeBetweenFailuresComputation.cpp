#include "RSMeanTimeBetweenFailuresComputation.h"
#include <QDateTime>

RSMeanTimeBetweenFailuresComputation* RSMeanTimeBetweenFailuresComputation::m_instance = 0;

RSMeanTimeBetweenFailuresComputation::RSMeanTimeBetweenFailuresComputation(QObject *parent) : QObject(parent),m_avgTime(0)
{
    createObjects();
    createConnections();
}

void RSMeanTimeBetweenFailuresComputation::createObjects()
{

}

void RSMeanTimeBetweenFailuresComputation::createConnections()
{

}

void RSMeanTimeBetweenFailuresComputation::setFailureTimes(QList<qint64> failureTimes)
{
    m_failureTimes = failureTimes;
}

void RSMeanTimeBetweenFailuresComputation::setStartOfDowntime(double startOfDowntime)
{
    m_startOfDowntime = startOfDowntime;
}

void RSMeanTimeBetweenFailuresComputation::setStartOfUptime(double startOfUptime)
{
    m_totalFailuresTimes = startOfUptime;
}

void RSMeanTimeBetweenFailuresComputation::setNumberOfFailures(double numberOfFailures)
{
    m_numberOfFailures = numberOfFailures;
}

void RSMeanTimeBetweenFailuresComputation::validate()
{
    Q_ASSERT(m_startOfDowntime > 0);
    Q_ASSERT(m_totalFailuresTimes >= 0);
    Q_ASSERT(m_numberOfFailures >= 0);

    if(m_numberOfFailures == 0)
    {
        m_numberOfFailures = 1;
    }
}

void RSMeanTimeBetweenFailuresComputation::initialize(const QString& sensorBrand
                                                      , int sensorCode
                                                      , const QString& sensorName
                                                      , const QString& sensorTechno
                                                      , QList<qint64> failureTimes
                                                      , const QDateTime& startTime,const QDateTime& endTime)
{
    m_sensorName = sensorName;
    m_brandName = sensorBrand;
    m_technology = sensorTechno;
    m_sensorCode = sensorCode;


    setFailureTimes(failureTimes);

    double totalTime = 0;

    for(int i = 0; i < failureTimes.size(); i++)
    {
        double _time = m_failureTimes.at(i);
        totalTime += _time;
    }

    if(failureTimes.size() > 0)
        m_avgTime = totalTime/failureTimes.size();
    else
        m_avgTime = 0;

    double numberOfFailures = failureTimes.size();
    double totalFailuresTimes = totalTime;
    double startOfDowntime = startTime.msecsTo(endTime);

    initBoundValues(startOfDowntime, totalFailuresTimes, numberOfFailures);
}


double RSMeanTimeBetweenFailuresComputation::getAvgTimeMsec()
{
    return m_avgTime;
}

void RSMeanTimeBetweenFailuresComputation::initBoundValues(double startOfDowntime, double startOfUptime, double numberOfFailures)
{
    setStartOfDowntime(startOfDowntime);

    setStartOfUptime(startOfUptime);

    setNumberOfFailures(numberOfFailures);
}

void RSMeanTimeBetweenFailuresComputation::compute()
{
    validate();


    m_mtbf = (m_startOfDowntime - m_totalFailuresTimes)/m_numberOfFailures;
}

void RSMeanTimeBetweenFailuresComputation::runComputations()
{
    compute();
}

double RSMeanTimeBetweenFailuresComputation::getMTBF() const
{
    return m_mtbf;
}


const QString& RSMeanTimeBetweenFailuresComputation::brandName()const
{
    return m_brandName;
}

const QString& RSMeanTimeBetweenFailuresComputation::sensorName()const
{
    return m_sensorName;
}

const QString& RSMeanTimeBetweenFailuresComputation::technology()const
{
    return m_technology;
}
