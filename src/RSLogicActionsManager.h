#ifndef RSLOGICACTIONSMANAGER_H
#define RSLOGICACTIONSMANAGER_H

#include <QObject>
#include <QMutex>
#include <QMap>
#include <Signaler.h>
#include  "IRSTimesManager.h"
#include "IRSOptionsManager.h"
#include <IRSGraphView.h>
#include "IRSMainwindow.h"
#include "RSFidelityAndNoise.h"
#include "RSExportWidget.h"
#include "IRSFiltersManager.h"
#include "IRexDataGrid.h"

class RSDatabaseAccess;



class RSLogicActionsManager : public QObject
{
    Q_OBJECT
public:


    explicit RSLogicActionsManager(RSDatabaseAccess*
                                   ,IRSMainWindow*
                                   ,IRSTimesManager*
                                   ,IRSOptionsManager*
                                   ,IRSGraphView*
                                   ,IRSFiltersManager*
                                   ,QObject *parent = 0);

signals:

public slots:
    void slot_exportToExcel();
protected slots:

    void slot_plotCurves();


    void slot_emitMessage(QMessageBox::Icon icon, const QString& color, const QString& title, const QString& msg);

    void slot_stepViewValueChanged(int);

    void slot_rsFilterIndexChanged();
    void slot_graphViewStepChanged(int data);
    void slot_clearGraphsAndSensorList();
    void slot_stepViewMaxChanged(int stepMax);
    void slot_quit();
    void slot_readyToStart();
    void slot_computeAverage();
    void slot_runReportsCalculations();

    void slot_updateSensorsList();
    void slot_cancel();

protected:
    void connectSignals();
    RSFidelityAndNoise::FidelitySettings initSettings();

    //Contains for each filter, the name and the value
    QMap<FiltersEnum,QPair<QString,QString>> m_filterNameValuePairMap;

    void computeStatisticalTable();

    void buildCurves();

    void updateSensorsListByExperimentation();

private:
    IRSTimesManager* m_timeManagerPtr;
    IRSOptionsManager* m_optionsMgrPtr;
    IRSGraphView* m_graphView;
    IRSMainWindow* m_mainWidnow;
    RSDatabaseAccess* m_databaseAccess;
    IRSFiltersManager* m_filterWidget;
    IRSExportManager* m_exportManager;
    RSFidelityAndNoise::FidelitySettings m_settings;

    bool m_exitLoop;
    QMutex m_exitLoopMutex;

    QString m_settingsFile;

};

#endif // RSLOGICACTIONSMANAGER_H
