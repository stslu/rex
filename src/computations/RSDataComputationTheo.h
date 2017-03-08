#ifndef RSDATACOMPUTATIONTHEO_H
#define RSDATACOMPUTATIONTHEO_H

#include <QObject>

class RSDataComputationTheo : public QObject
{
    Q_OBJECT

private:
    RSDataComputationTheo(QObject *parent = 0);

public:
    ~RSDataComputationTheo();
    static RSDataComputationTheo* Instance(QObject *parent = 0);

private:
    void createObjects();
    void createConnections();

public:
    void run();

private:
    static RSDataComputationTheo* m_instance;
};

#endif // RSDATACOMPUTATIONTHEO_H
