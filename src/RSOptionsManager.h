#ifndef RSOPTIONSMANAGER_H
#define RSOPTIONSMANAGER_H

#include <QWidget>
#include <QMap>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include "IRSOptionsManager.h"

namespace Ui
{
class RSOptionsManager;
}

class RSOptionsManager : public QWidget, public IRSOptionsManager
{
    Q_OBJECT

public:
    RSOptionsManager(QWidget *parent = 0);
    ~RSOptionsManager();

    virtual double noiseFactor() const ;
    virtual double sigma() const ;

    virtual int trend() const ;

    virtual bool isFidelityChecked() const ;
    virtual bool isFidelityMinChecked() const ;
    virtual bool isFidelityMaxChecked() const ;
    virtual bool isFidelityAvgChecked() const;

    void loadSettings(const QString& fileName) ;
    void saveSettings(const QString& fileName) const;

protected:
    void createObjects();
    void createConnections();

    void updateFidelityChecked();
    void updateNoiseChecked();
    void updateFailureChecked();

    //[RSOptionsManager]
    QVariant loadFidelity();
    QVariant loadFidelityMin();
    QVariant loadFidelityMax();
    QVariant loadFidelityAvg();

    QVariant loadTrend();
    QVariant loadSigma();
    QVariant loadNoiseFactor();

    //[RSOptionsManager]
    void saveFidelity() const;
    void saveFidelityMin() const;
    void saveFidelityMax() const;
    void saveFidelityAvg() const;

    void saveTrend() const;
    void saveSigma() const;
    void saveNoiseFactor()const;

    void setSpinBoxVaue(QSpinBox*, int value);
    void setSpinBoxVaue(QDoubleSpinBox*, double value);
    void setCheckBoxValue(QCheckBox*, bool state);

public slots:
    void slotFidelityClicked(bool data);
    void slotFidelityMinClicked(bool data);
    void slotFidelityMaxClicked(bool data);
    void slotFidelityAvgClicked(bool data);

    void slotTrendValueChanged(int data);
    void slotSigmaValueChanged(int data);
    void slotNoiseValueChanged(double data);

private:
    Ui::RSOptionsManager *ui;
};

#endif // RSOPTIONSMANAGER_H
