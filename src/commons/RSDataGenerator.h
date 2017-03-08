#ifndef RSDATAGENERATOR_H
#define RSDATAGENERATOR_H

#include <QObject>

class RSDataGenerator : public QObject
{
    Q_OBJECT

private:
    RSDataGenerator(QObject *parent = 0);

public:
    ~RSDataGenerator();
    static RSDataGenerator* Instance(QObject *parent = 0);

private:
    void createObjects();
    void createConnections();

    void swap(double& x1, double& x2);

public:
    double randomData(double min, double max);
    QList<double> randomData(double min, double max, int size);
    double dataFunctionPx(double x, QList<double> params);
    double dataFunctionPx(double x, QList<double> params, double noiseWidth);
    QList<double> dataFunctionPx(QList<double> x, QList<double> params);
    QList<double> dataFunctionPx(QList<double> x, QList<double> params, double noiseWidth);
    double dataFunctionFx(double x);
    QList<double> dataFunctionFx(QList<double> x);
    double dataFunctionFx(double x, double noiseWidth);
    QList<double> dataFunctionFx(QList<double> x, double noiseWidth);
    QList<double> X(double x1, double x2, int size);
    double noise(double width);
    QList<double> noise(double width, int size);

private:
    static RSDataGenerator* m_instance;
};

#endif // RSDATAGENERATOR_H
