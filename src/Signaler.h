#ifndef RSSIGNALER_H
#define RSSIGNALER_H

#include <QObject>
#include <QMessageBox>

enum FiltersEnum
{
    Brand = 0,
    Model,
    Technology,
    PhysicalMeasurement,
    OutputSignal,
    TheoricalAccuracy,
    MeasurementRange,
    Unit,
    experimentation
};

class Signaler : public QObject
{
    Q_OBJECT

public:
    static Signaler* instance(QObject* parent = 0);

    static void killInstance();

signals:

    void signal_rsFilterIndexChanged();

    void signal_graphViewStepChanged(int data);

    void signal_emitUpdateStepViewMax(int data);

    void signal_updateRex();

    void signal_runReportsCalculations();

    void signal_sensorNameIndexChanged(const QString&);

    void signal_plotCurves();

    void signal_plotDeviationCurves();

    void signal_exportToExcel();

    void signal_emitMessage(QMessageBox::Icon icon, const QString& color, const QString& title, const QString& msg);

    void signal_clearGraphsAndSensorList();

    void signal_stepViewMaxChanged(int stepMax);

    void signal_quit();

    void signal_readyToStart();

    void signal_computeAverage();

    void signal_updateSensorsList();

    void signal_closeAppli();

    void signal_cancel();

protected:
    explicit Signaler(QObject *parent = 0):QObject(parent){}

private:
    static Signaler* s_instance ;
};



#endif // RSSIGNALER_H
