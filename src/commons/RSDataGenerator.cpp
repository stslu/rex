#include "RSDataGenerator.h"
#include "RSMessageView.h"
#include <QTime>
#include <QtMath>

RSDataGenerator* RSDataGenerator::m_instance = 0;

RSDataGenerator::RSDataGenerator(QObject *parent) : QObject(parent)
{
    createObjects();
    createConnections();
}

RSDataGenerator::~RSDataGenerator()
{

}

RSDataGenerator* RSDataGenerator::Instance(QObject *parent)
{
    if(m_instance == 0)
    {
        m_instance = new RSDataGenerator(parent);
    }

    return m_instance;
}

void RSDataGenerator::createObjects()
{
    //qsrand(QTime::currentTime().msec());
}

void RSDataGenerator::createConnections()
{

}

void RSDataGenerator::swap(double& x1, double& x2)
{
    double m_tmp = x1;
    x1 = x2;
    x2 = m_tmp;
}

double RSDataGenerator::randomData(double min, double max)
{
    if(min > max)
    {
        swap(min, max);
    }

    double m_factor = (double)qrand()/RAND_MAX;
    double m_data = min + m_factor*(max - min);
    return m_data;
}

QList<double> RSDataGenerator::randomData(double min, double max, int size)
{
    QList<double> m_dataList;

    for(int i = 0; i < size; i++)
    {
        double m_data = randomData(min, max);
        m_dataList.push_back(m_data);
    }

    return m_dataList;
}

// a0*x^0 + a1*x^1 + a2*x^2 + ... + an*x^n = Sum(ai*x^i)
double RSDataGenerator::dataFunctionPx(double x, QList<double> params)
{
    double m_data = 0;

    for(int i = 0; i < params.size(); i++)
    {
        double m_ai = params.at(i);
        double m_pi = qPow(x, (double)i);
        m_data += m_ai*m_pi;
    }

    return m_data;
}

// yR = yT + yN
double RSDataGenerator::dataFunctionPx(double x, QList<double> params, double noiseWidth)
{
    double m_yTheorical = dataFunctionPx(x, params);
    double m_yNoise = noise(noiseWidth);
    double m_yReal = m_yTheorical + m_yNoise;
    return m_yReal;
}

// yi = f(xi)
QList<double> RSDataGenerator::dataFunctionPx(QList<double> x, QList<double> params)
{
    QList<double> m_dataList;

     for(int i = 0; i < x.size(); i++)
     {
         double m_xi = x.at(i);
         double m_data = dataFunctionPx(m_xi, params);
         m_dataList.push_back(m_data);
     }

     return m_dataList;
}

// yR = yT + yN
QList<double> RSDataGenerator::dataFunctionPx(QList<double> x, QList<double> params, double noiseWidth)
{
    QList<double> m_dataList;

     for(int i = 0; i < x.size(); i++)
     {
         double m_xi = x.at(i);
         double m_yTheorical = dataFunctionPx(m_xi, params);
         double m_yNoise = noise(noiseWidth);
         double m_yReal = m_yTheorical + m_yNoise;
         m_dataList.push_back(m_yReal);
     }

     return m_dataList;
}

// yi = f(xi)
double RSDataGenerator::dataFunctionFx(double x)
{
    double m_max = 10.0;
    //double m_data = m_max*qSin(x) + 5*x;
    double m_data = 5;
    return m_data;
}

// yi = f(xi)
QList<double> RSDataGenerator::dataFunctionFx(QList<double> x)
{
    QList<double> m_data;

    for(int i = 0; i < x.size(); i++)
    {
        double m_xi = x.at(i);
        double m_yi = dataFunctionFx(m_xi);
        m_data.push_back(m_yi);
    }

    return m_data;
}

// yR = yT + yN
double RSDataGenerator::dataFunctionFx(double x, double noiseWidth)
{
    double m_yTheorical = dataFunctionFx(x);
    double m_yNoise = noise(noiseWidth);
    double m_yReal = m_yTheorical + m_yNoise;
    return m_yReal;
}

// yR = yT + yN
QList<double> RSDataGenerator::dataFunctionFx(QList<double> x, double noiseWidth)
{
    QList<double> m_data;

     for(int i = 0; i < x.size(); i++)
     {
         double m_xi = x.at(i);
         double m_yTheorical = dataFunctionFx(m_xi);
         double m_yNoise = noise(noiseWidth);

         double m_pic = 0.0;

         if(i % 10 == 0)
         {
             m_pic = noise(5*noiseWidth);

             if(m_pic < 0)
             {
                 m_pic = 0.0;
             }
         }

         //m_pic = 0.0;
         double m_yReal = m_yTheorical + m_yNoise + m_pic;
         m_data.push_back(m_yReal);
     }

     return m_data;
}

// xi = x0 + i*d
QList<double> RSDataGenerator::X(double x1, double x2, int size)
{
    if(x1 > x2)
    {
        swap(x1, x2);
    }

    double m_factor = (x2 - x1) / (size - 1);
    QList<double> m_X;

    for(int i = 0; i < size; i++)
    {
        double m_xi = x1 + m_factor*i;
        m_X.push_back(m_xi);
    }

    return m_X;
}

double RSDataGenerator::noise(double width)
{
    double m_factor = randomData(-1, 1);
    double m_noise = m_factor*width;
    return m_noise;
}

QList<double> RSDataGenerator::noise(double width, int size)
{
    QList<double> m_dataList;

    for(int i = 0; i < size; i++)
    {
        double m_data = noise(width);
        m_dataList.push_back(m_data);
    }

    return m_dataList;
}
