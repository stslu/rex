#ifndef IRSGRAPHVIEW
#define IRSGRAPHVIEW

#include <QStringList>
#include <QWidget>
class IRexDataGrid;
class SensorInfos;

enum MeasPointType;

class IRSGraphView
{
public:

    virtual void clearGraphs() = 0;
    virtual void clearGraphs(const QDate& startDate, const QDate& endDate) = 0;
    virtual void add(const QList<double>& x, const QList<double>& y, const QString& plotName = "Trend", int width = 1) = 0;
    virtual void add(const QList<double>& x, const QList<double>& y, const QDateTime& startDateTime, const QString& plotName = "Trend", int step = 5, int width = 1) = 0;

    /**
     * @brief setSensorNameList : add the list of sensors for the selecte experimentation. "All" experimentation means all sensors
     */
    virtual void setSensorNameList() = 0;

    virtual QStringList getCurSensorNamesList() const = 0;

    virtual QString currentExperimentationName() const = 0;

    virtual QPair<int,MeasPointType> getSensorCodeTypePair() const = 0;

    virtual void clearGraphsAndSensorList() = 0;

    virtual int step() const = 0;

    virtual int minStep() const = 0;

    virtual int maxStep()  const = 0;

    virtual void loadSettings(const QString& fileName) = 0;

    virtual void saveSettings(const QString& fileName) const = 0;

    virtual void setStepViewMax(int step) = 0;

    virtual const QMap<int,QString>& sensorsCodeNameMap()  =  0;

    virtual void initSensorsCodeNameMap() = 0;

    virtual void displayAverageValues(double fidelityAvg, double noiseAvg) = 0;

    virtual void displayStepValues(double fidelityAvg, double noiseAvg) = 0;

    virtual QString sensorName() const = 0;

    virtual  IRexDataGrid* fidelityNoiseFailsDataGrid() = 0;

    virtual  IRexDataGrid* sensorByExpDataGrid() = 0;

    virtual const QList<SensorInfos>& sensorsDetailedInfo() const = 0;

    virtual void plotCharts() = 0;

    virtual ~IRSGraphView(){}

    virtual void setProgressBar(double rate) = 0 ;

    virtual void clearDataGrids() = 0;

    virtual void initSensorsListDatagrid() = 0;

    virtual void updateSensorsDatagrid(int) = 0;

    virtual void displayDataTab() = 0;
    virtual void displayTab(int) = 0;
};

#endif // IRSGRAPHVIEW

