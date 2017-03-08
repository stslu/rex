#ifndef RSDATACOMPUTATION_H
#define RSDATACOMPUTATION_H

#include <QObject>
#include <QProgressBar>
#include "RSFidelityAndNoise.h"
#include "RSDataManager.h"


class RSFidelityAndNoise;
class RSMeanTimeBetweenFailuresComputation;
class IRSGraphView;
class SensorInfos;


class RSDataComputation : public QObject
{
    Q_OBJECT

public:
     RSDataComputation(RSDatabaseAccess* ,RSFidelityAndNoise::FidelitySettings& settings,IRSGraphView* graphView,QObject *parent = 0);

    /**
     * @brief runFidelityAndNoiseCalculations : prepare and run the thread RSFidelityAndNoise
     */

    void runFidelityNoiseAndMtbfCalculations(const QList<SensorInfos>& map
                                             ,  const RSFidelityAndNoise::FidelitySettings&  settings
                                             , IRSGraphView* graphView
                                             ,  QVector<RexStatistics>& rexStatisticsArray);
    /**
     * @brief runMtbfCalculationBySensorBrandTecnology :
     */
    //void runMtbfCalculations();

    QPair<double,double> computeFidelityNoiseStepAndInitGraphView(bool  addGraphs, const RSFidelityAndNoise::FidelitySettings&  settings, bool showMsg = true);



protected:
    void createObjects();
    void createConnections();


protected slots:
    void addFidelitiesAndNoiseInDataManager(RSFidelityAndNoise*);

private:
    RSFidelityAndNoise::FidelitySettings  m_settings;
    RSDatabaseAccess * m_databaseAccess;
    IRSGraphView* m_graphView;

};



#endif // RSDataComputation_H
