#ifndef RSGRAPHVIEW_H
#define RSGRAPHVIEW_H

#include <QWidget>
#include <QBoxLayout>
#include <QCustomPlot.h>
#include <QPushButton>
#include "RSGlobalMethods.h"
#include "IRSGraphView.h"
#include "RexDataGrid.h"
#include "RexDataGridModel.h"
#include "RSLogicActionsManager.h"
#include "RexDataGridModel.h"

class RSDatabaseAccess;
class SensorInfos;
class RSLogicActionsManager;

enum HistoEnum
{
    fidelityByTechno,
    noiseByTechno,
    fidelityByOutputSignal,
    noiseByOutputSignal,
    fidelityByPhysicalMeas,
    noiseByPhysicalMeas,
    nbSensorByBrand,
    nbSensorByExp,
    failuresByTechno,
    failuresByOutputSignal,
    failuresByPhysicalMeas,
    nbSensorByTechno
    , nbFailuresByTechno
};

struct HistoPlot
{
    QString tick;
    double value;
    QVariantList dataLine;
    int anyUse;
    int nbFailures;

    HistoPlot():value(0),anyUse(0),nbFailures(0)
    { }
};


class HistoWidget : public QWidget
{
public:

    QCustomPlot* customPlot(){return m_customPlot;}
    QVector<HistoPlot>& histoPlotArray() {return m_histoArray;}
    const QString& name() const {return m_name;}
    QString xLabel() const { return m_xLabel.size() ? m_xLabel : QString("X");}
    QString yLabel() const
    {
        if(!m_yFieldsList.isEmpty())
            return  m_yFieldsList.at(0) ;
        else
            return "Y";
    }
    RexDataGrid* dataGrid()  {return m_dataGrid;}
    const QStringList& getFields()const { return m_dataGrid->getFields(); }
    const  QList<QVariantList>& getData()const { return m_dataGrid->getData(); }
    void setTitle(const QString& title){lineEdit->setText(title);}

    void updateModel()
    {
        QList<QVariantList> data = QList<QVariantList>();
        Q_FOREACH(const HistoPlot& plot,m_histoArray )
            data << plot.dataLine;

        QStringList fields = m_yFieldsList;
        fields.prepend(m_xLabel);
        m_dataGrid->setDataModelMatrix(fields, data);
    }

    HistoWidget(const QString& x,const QStringList& y,const QString& name):
        m_xLabel(x)
      , m_yFieldsList(y)
      , m_name(name)
    {
        if (this->objectName().isEmpty())
            this->setObjectName(QStringLiteral("Frame"));
        this->resize(308, 312);

        lineEdit = new QLineEdit(this);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setMaximumSize(QSize(16777215, 22));

        m_dataGrid = new RexDataGrid(false,this);
        m_dataGrid->setObjectName(QStringLiteral("m_dataTableView"));
        m_dataGrid->setMaximumSize(QSize(500, 16777215));


        QMetaObject::connectSlotsByName(this);

        m_customPlot = new QCustomPlot(this);
        m_customPlot->setObjectName(QStringLiteral("m_customPlot"));
        m_customPlot->setMinimumSize(QSize(211, 0));

        updateModel();

        m_dataGrid->setVisible(true);
        m_dataGrid->ui().progressBar->setVisible(false);
        m_dataGrid->ui().m_runReportsCalculations->setVisible(false);

        m_dataGrid->view()->options().setColumnAutoWidth(true);

        verticalLayout = new QVBoxLayout(this);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->addWidget(lineEdit);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->addWidget(m_dataGrid);
        horizontalLayout->addWidget(m_customPlot);
        verticalLayout->addLayout(horizontalLayout);

        lineEdit->setText(m_name);

        m_dataGrid->ui().m_comboBox->setVisible(false);

        setTitle(name);


    } // setupUi


private:
    QHBoxLayout *horizontalLayout;
    RexDataGrid *m_dataGrid;
    QCustomPlot*  m_customPlot ;
    QVector<HistoPlot> m_histoArray;
    QString m_xLabel;
    QStringList m_yFieldsList;
    QString m_name;
    QLineEdit *lineEdit;
    QVBoxLayout *verticalLayout;
};

namespace Ui
{
class RSGraphView;
}

class RSGraphView : public QWidget, public IRSGraphView
{


    Q_OBJECT

public:
    ~RSGraphView();
    RSGraphView(RSDatabaseAccess* dbAccess, QWidget *parent = 0);
    //static RSGraphView* Instance(QWidget *parent = 0);

    void clearGraphs();
    void clearGraphs(const QDate& startDate, const QDate& endDate);
    void add(const QList<double>& x, const QList<double>& y, const QString& plotName = "Trend", int width = 1);
    void add(const QList<double>& x, const QList<double>& y, const QDateTime& startDateTime, const QString& plotName = "Trend", int step = 5, int width = 1);

    /**
     * @brief setSensorNameList : add the list of sensors for the selecte experimentation. "All" experimentation means all sensors
     */
    void setSensorNameList();

    //const QPair<int,QString>& sensorsCodeNameMap() const;

    QStringList getCurSensorNamesList() const;
    QString currentExperimentationName() const;
    void clearGraphsAndSensorList();
    int sensorCode() ;
    QString sensorName() const;

    IRexDataGrid* fidelityNoiseFailsDataGrid();
    IRexDataGrid* sensorByExpDataGrid() ;
    void updateSensorsDatagrid(int index);


    int step() const;
    int minStep() const;
    int maxStep() const;

    void loadSettings(const QString& fileName) ;
    void saveSettings(const QString& fileName) const;
    void setStepViewMax(int step) ;
    const QMap<int,QString>& sensorsCodeNameMap() ;
    void initSensorsCodeNameMap();
    virtual void displayAverageValues(double fidelityAvg, double noiseAvg) ;
    virtual void displayStepValues(double fidelityAvg, double noiseAvg) ;
    void setProgressBar(double rate) ;
    void clearDataGrids();

    void initSensorsListDatagrid();
    QList<QVariantList> getDataMaxtrixFromQMap( const QMap<QString,QStringList>& map) const;
    void displayDataTab() ;
    void displayTab(int);
protected slots:
    void slotTrendRunButtonClicked();
    void slotTrendClearButtonClicked();
    void slot_deviationRunButtonClicked();

    void slotStepMinButtonClicked();
    void slotStepMaxButtonClicked();
    void slotStepPreviousButtonClicked();
    void slotStepNextButtonClicked();
    void slotStepViewValueChanged(int data);
    void slotStepViewValueChangedEx(int data);
    void slotStepViewMaxValueChanged(int data);

    void slotSensorNameIndexChanged(const QString& data);
    void slot_rsFilterIndexChanged();
    void slot_averagesButtonClicked();

    const QList<SensorInfos>& sensorsDetailedInfo() const;
    void plotCharts();
    void slotSensorListNameIndexChanged(int);

protected:
    void position();
    void createObjects();
    void createConnections();

    QVariant loadSensorName();
    void saveSensorName()const;

    QVariant loadStepView();
    void saveStepView()const;
    void plotHistoGram(QCustomPlot* customPlot, const QString& histoName,const QString& yLabel,const QVector<HistoPlot>& dataArray);


    // bool initHistoPlotArraysFromFidelityNoiseDataGrid();

    bool initNbSensorByExperimentationHistoArray(QVector<HistoPlot>&);
    bool initNbSensorHistoWidgetData(const QString& dataName,QVector<HistoPlot>& nbSensorMap);

    bool initFidelityNoiseFailuresHistoWidgetData(QVector<HistoPlot>& fidelityArray
                                                  , QVector<HistoPlot>& noiseArray
                                                  , QVector<HistoPlot>& failuresArray
                                                  , const QString& dataName);

    void initDatagridPointers();

private:
    //static RSGraphView* m_instance;

    Ui::RSGraphView *ui;
    QStringList m_colors;
    QMap<QString, QCustomPlot*> m_plotMap;
    QMap<QString, double> m_xMinMap;
    QMap<QString, double> m_xMaxMap;
    QMap<QString, double> m_yMinMap;
    QMap<QString, double> m_yMaxMap;
    QMap<QString, double> m_lastMinMap;
    QMap<QString, double> m_lastMaxMap;
    bool m_lastOneOnly;
    bool m_runFlag;
    bool m_sensorNameIndexOneOnly;
    int m_sensorCode;
    RSDatabaseAccess* m_databaseAccess;

    int m_stepViewMax;
    QMap<int,QString> m_sensorsCodeNameMap;

    QList<SensorInfos> m_sensorsDetailedInfo;

    QVector<HistoPlot> m_fidelityByTechnologyHistoPlotArray;
    QVector<HistoPlot> m_noiseByTechnologyHistoPlotArray;
    QVector<HistoPlot> m_fidelityByOutputSignalHistoPlotArray;
    QVector<HistoPlot> m_noiseByOutputSignalHistoPlotArray;
    QVector<HistoPlot> m_nbSensorByBrandHistoPlotArray;
    QVector<HistoPlot> m_nbSensorByExpHistoPlotArray;

    //The following variables are initialized by initFidelityNoiseFailuresHistoWidgetData()
    //-----------------------------------------------------------------------------------
    //Fidelities
    HistoWidget m_fidelityByTechnoHisto;
    HistoWidget m_fidelityByOutputSignalHisto;
    HistoWidget m_fidelityByPhysicalMeasHisto;

    //Noises
    HistoWidget m_noiseByTechnoHisto;
    HistoWidget m_noiseByOutputSignalHisto;
    HistoWidget m_noiseByPhysicalMeasHisto;

    //Failures
    HistoWidget m_failuresByTechnoHisto;
    HistoWidget m_failuresByOutputSignalHisto;
    HistoWidget m_failuresByPhysicalMeasHisto;
    HistoWidget m_nbFailuresByTechnoHisto;

    //Mtbf
    HistoWidget m_mtbfByTechnoHisto;
    HistoWidget m_mtbfByOutputSignalHisto;
    HistoWidget m_mtbfByPhysicalMeasHisto;

    //Sensors distributions
    //The following variables are initialized by initNbSensorHistoWidgetData()
    //-----------------------------------------------------------------------------------
    HistoWidget m_nbSensorByBrandHisto;
    HistoWidget m_nbSensorByExpHisto;
    HistoWidget m_nbSensorByTechnoHisto;

    //DatagGrid. All data
    RexDataGrid* m_noiseFailsFidelityDataGrid;
    RexDataGrid* m_sensorByExpDataGrid;//List of sensor by experimentation
    RexDataGrid* m_sensorByBrandDataGrid;//List of sensor by Brand
    RexDataGrid* m_sensorByTechnologyDataGrid;//List of sensor by Technology
    RexDataGrid* m_sensorByOutputSignalDataGrid;//List of sensor by OutputSignal
    RexDataGrid* m_sensorByPhysicalMeasDataGrid;//List of sensor by PhysicalMeas
    RexDataGrid* m_sensorsDataGrid;//List of sensor by experimentation
    DataGridModel m_sensorsByMeasureRangeModel;


    void createStatsDataPage();
    void createDatagridPage(QWidget* container,RexDataGrid* dataGrid);
    void createFidelitiesHistogramsPage();
    void createNoisesHistogramsPage();
    void createFailuresHistogramsPage();
    void createDistributionsHistogramsPage();

    friend void RSLogicActionsManager::slot_exportToExcel();

};

#endif // RSGRAPHVIEW_H
