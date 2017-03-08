#include <QtMath>
#include <RSlogger.h>
#include "RSFidelityComputation.h"
#include "RSGlobalMethods.h"
#include "RSMatrix.h"
#include "RSMessageView.h"

#include <QDebug>

RSFidelityComputation* RSFidelityComputation::m_instance = 0;

RSFidelityComputation::RSFidelityComputation(QObject *parent) : QObject(parent)
{
    createObjects();
    createConnections();
}

RSFidelityComputation::~RSFidelityComputation()
{

}

void RSFidelityComputation::createObjects()
{

}

void RSFidelityComputation::createConnections()
{

}

void RSFidelityComputation::validate()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start",1);

    Q_ASSERT(m_x.size() != 0);
    Q_ASSERT(m_y.size() != 0);
    Q_ASSERT(m_degree >= 0);
    Q_ASSERT(m_sigma >= 0 && m_sigma <= 100);
    Q_ASSERT(m_noiseFactor >= 0.1 && m_noiseFactor <= 10);

    RSLogger::instance()->info(Q_FUNC_INFO,"End",1);
}

// di = ai*xi^i
double RSFidelityComputation::polynomial(double x) const
{
    double m_data = 0;

    for(int i = 0; i < m_params.size(); i++)
    {
        double m_ai = m_params.at(i);
        double m_pi = qPow(x, (double)i);
        m_data += m_ai*m_pi;
    }

    return m_data;
}

// di = (ni/n)*100
double RSFidelityComputation::getNoiseStep() const
{
    double m_fidelityStep = getFidelityStep();
    double m_noiseWidth = m_fidelityStep*m_noiseFactor;
    double m_noiseCount = 0.0;
    int m_size = m_x.size();

    for(int i = 0; i < m_size; i++)
    {
        double m_di = getDeviation(i);

        if(m_di > m_noiseWidth)
        {
            m_noiseCount += 1.0;
        }
    }

    m_noiseCount = (m_noiseCount*100) / m_size;

    return m_noiseCount;
}

void RSFidelityComputation::addNoiseStep(double noiseStep)
{
    m_noiseStepList.push_back(noiseStep);
}

void RSFidelityComputation::clearNoiseStep()
{
    m_noiseStepList.clear();
}

double RSFidelityComputation::getNoiseBySensor() const
{
    return RSGlobalMethods::Instance()->avg(m_noiseStepList);
}

void RSFidelityComputation::addNoiseBySensor(double noiseBySensor)
{
    m_noiseBySensorList.push_back(noiseBySensor);
}

void RSFidelityComputation::clearNoiseBySensor()
{
    m_noiseBySensorList.clear();
}

double RSFidelityComputation::getNoiseMin() const
{
    return RSGlobalMethods::Instance()->min(m_noiseBySensorList);
}

double RSFidelityComputation::getNoiseMax() const
{
    return RSGlobalMethods::Instance()->max(m_noiseBySensorList);
}

double RSFidelityComputation::getNoiseAvg() const
{
    return RSGlobalMethods::Instance()->avg(m_noiseBySensorList);
}

void RSFidelityComputation::setDegree(int degree)
{
    m_degree = degree;
}

void RSFidelityComputation::setX(const QList<double>& x)
{
    m_x.clear();
    m_x = x;
}

void RSFidelityComputation::setY(const QList<double>& y)
{
    m_y.clear();
    m_y = y;
}

void RSFidelityComputation::setSigma(double sigma)
{
    m_sigma = sigma;
}

void RSFidelityComputation::setNoiseFactor(double noiseFactor)
{
    m_noiseFactor = noiseFactor;
}

void RSFidelityComputation::initialize(const QList<double>& x, const QList<double>& y, int degree, double sigma, double noiseFactor)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    RSLogger::instance()->info(Q_FUNC_INFO,QString("setY array with %1 elements ").arg(y.size()));
    setY(y);

    RSLogger::instance()->info(Q_FUNC_INFO,QString("setX array with %1 elements ").arg(x.size()));
    setX(x);

    RSLogger::instance()->info(Q_FUNC_INFO,QString("set degree %1 ").arg(degree));
    setDegree(degree);

    RSLogger::instance()->info(Q_FUNC_INFO, QString("setSigma : %1").arg(sigma));
    setSigma(sigma);

    RSLogger::instance()->info(Q_FUNC_INFO, QString("setNoiseFactor : %1").arg(noiseFactor)  );
    setNoiseFactor(noiseFactor);

    RSLogger::instance()->info(Q_FUNC_INFO, "End ");
}


void RSFidelityComputation::compute()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    validate();
    m_params = RSGlobalMethods::Instance()->interpolation(m_x, m_y, m_degree);

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

QList<double> RSFidelityComputation::getTrend() const
{
    QList<double> m_trend;

    for(int i = 0; i < m_x.size(); i++)
    {
        double m_di = getTrend(i);
        m_trend.push_back(m_di);
    }

    return m_trend;
}

double RSFidelityComputation::getTrend(int i) const
{
    double m_xi = m_x.at(i);
    double m_di = polynomial(m_xi);
    return m_di;
}

QList<double> RSFidelityComputation::getDeviation() const
{
    QList<double> m_deviation;

    for(int i = 0; i < m_x.size(); i++)
    {
        double m_di = getDeviation(i);
        m_deviation.push_back(m_di);
    }

    qSort(m_deviation.begin(), m_deviation.end(), qLess<double>());

    return m_deviation;
}

double RSFidelityComputation::getDeviation(int i) const
{
    double m_xi = m_x.at(i);
    double m_pi = polynomial(m_xi);
    double m_yi = m_y.at(i);
    double m_di = qAbs(m_yi - m_pi);
    return m_di;
}

int RSFidelityComputation::getSigmaSize(int sigma) const
{
    int m_size = m_x.size() - 1;
    int m_data = (sigma*m_size)/100;
    return m_data;
}

QList<double> RSFidelityComputation::getSigmas() const
{
    int m_size = m_x.size();
    QList<double> m_sigmas;

    for(int i = 0; i < m_size; i++)
    {
        double m_xi = (double)i;
        double m_di = (m_xi*100)/m_size;
        m_sigmas.push_back(m_di);
    }

    return m_sigmas;
}

double RSFidelityComputation::getFidelityStep() const
{
    QList<double> m_deviation = getDeviation();
    int m_size = m_x.size();
    int m_sigmaSize = getSigmaSize(m_sigma);
    double m_data;

    for(int i = 0; i < m_size; i++)
    {
        if(i >= m_sigmaSize)
        {
            m_data = m_deviation.at(i);
            break;
        }
    }

    return m_data;
}

void RSFidelityComputation::addFidelityStep(double fidelityStep)
{
    m_fidelityStepList.push_back(fidelityStep);
}

void RSFidelityComputation::clearFidelityStep()
{
    m_fidelityStepList.clear();
}

double RSFidelityComputation::getFidelityBySensor() const
{
    return RSGlobalMethods::Instance()->avg(m_fidelityStepList);
}

void RSFidelityComputation::addFidelityBySensor(double fidelityBySensor)
{
    m_fidelityBySensorList.push_back(fidelityBySensor);
}

void RSFidelityComputation::clearFidelityBySensor()
{
    m_fidelityBySensorList.clear();
}

// di = min(ai)
double RSFidelityComputation::getFidelityMin() const
{
    return RSGlobalMethods::Instance()->min(m_fidelityBySensorList);
}

// di = max(ai)
double RSFidelityComputation::getFidelityMax() const
{
    return RSGlobalMethods::Instance()->max(m_fidelityBySensorList);
}

// di = Sum(ai)/n
double RSFidelityComputation::getFidelityAvg() const
{
    RSLogger::instance()->info(Q_FUNC_INFO,QString("m_fidelityBySensorList.count : %1").arg(m_fidelityBySensorList.count()));
    return RSGlobalMethods::Instance()->avg(m_fidelityBySensorList);
}

// di = ti - noise
QList<double>  RSFidelityComputation::getBollingerMin() const
{
    double m_fidelityStep = getFidelityStep();
    double m_noiseWidth = m_fidelityStep*m_noiseFactor;
    QList<double> m_data;


    for(int i = 0; i < m_x.size(); i++)
    {
        double m_ti = getTrend(i);
        double m_di = m_ti - m_noiseWidth;
        m_data.push_back(m_di);
    }

    return m_data;
}

// di = ti + noise
QList<double>  RSFidelityComputation::getBollingerMax() const
{
    double m_fidelityStep = getFidelityStep();
    double m_noiseWidth = m_fidelityStep*m_noiseFactor;
    QList<double> m_data;

    for(int i = 0; i < m_x.size(); i++)
    {
        double m_ti = getTrend(i);
        double m_di = m_ti + m_noiseWidth;
        m_data.push_back(m_di);
    }

    return m_data;
}

void  RSFidelityComputation::clearVectors()
{
    m_x.clear();

    m_y.clear();

    m_params.clear();

    m_fidelityStepList.clear();

    m_fidelityBySensorList.clear();

    m_noiseStepList.clear();

    m_noiseBySensorList.clear();
}
