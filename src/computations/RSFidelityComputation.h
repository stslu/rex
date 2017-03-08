#ifndef RSFIDELITYCOMPUTATION_H
#define RSFIDELITYCOMPUTATION_H

#include <QObject> // big data // not save data

class RSFidelityComputation : public QObject
{
    Q_OBJECT

public:
    enum SigmasType {Sigma0 = 80, Sigma1 = 68, Sigma2 = 95} ;
    Q_ENUM(SigmasType)

public:
    RSFidelityComputation(QObject *parent = 0);

public:
    ~RSFidelityComputation();
    //static RSFidelityComputation* Instance(QObject *parent = 0);

    void setDegree(int degree);

    void setX(const QList<double>& x);

    void setY(const QList<double>& y);

    void setSigma(double sigma);

    void setNoiseFactor(double noiseFactor);

    void initialize(const QList<double>& x, const QList<double>& y, int degree, double sigma, double noiseFactor);

    void compute();

    QList<double> getTrend() const;

    double getTrend(int i) const;

    QList<double> getDeviation() const;

    double getDeviation(int i) const;

    int getSigmaSize(int sigma) const;

    QList<double> getSigmas() const;

    double getFidelityStep() const;

    void addFidelityStep(double fidelityStep);

    void clearFidelityStep();

    double getFidelityBySensor() const;

    void addFidelityBySensor(double fidelityBySensor);

    void clearFidelityBySensor();

    double getFidelityMin() const;

    double getFidelityMax() const;

    double getFidelityAvg() const;

    double getNoiseStep()const;

    void addNoiseStep(double noiseStep);

    void clearNoiseStep();

    double getNoiseBySensor() const;

    void addNoiseBySensor(double noiseBySensor);

    void clearNoiseBySensor();

    double getNoiseMin() const;

    double getNoiseMax() const;

    double getNoiseAvg() const;

    QList<double>  getBollingerMin() const;

    QList<double>  getBollingerMax() const;

    void clearVectors();

protected:
    void createObjects();

    void createConnections();

    void validate();

    double polynomial(double x) const;

private:
    static RSFidelityComputation* m_instance;

    // inputs
    int m_degree;

    QList<double> m_x;

    QList<double> m_y;

    double m_sigma;

    double m_noiseFactor;

    // outpus
    QList<double> m_params;

    //double m_fidelityStep;
    QList<double> m_fidelityStepList;

    QList<double> m_fidelityBySensorList;

    QList<double> m_noiseStepList;

    QList<double> m_noiseBySensorList;
};

#endif // RSFIDELITYCOMPUTATION_H
