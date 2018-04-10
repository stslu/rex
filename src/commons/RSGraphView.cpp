#include "RSGraphView.h"
#include "ui_RSGraphView.h"
#include "RSPictoManager.h"
#include "RSSettingsManager.h"
#include "RSDataManager.h"
#include "RSMessageView.h"
#include "RSDatabaseAccess.h"
#include <QApplication>
#include <QDesktopWidget>
#include <RSLogger.h>

#include <Signaler.h>

namespace RSGraphViewDefaultSettings
{
const int   DEFAULT_SENSOR_NAME_INDEX = 0;
const int   DEFAULT_STEP_INDEX = 0;
const int   DEFAULT_STEP_VIEW_IDX_MIN = 0;

} // REX_NAMESPACE



//QMap<HistoEnum,HistoWidget> HistoWidget::s_histogramMap;

RSGraphView::RSGraphView(RSDatabaseAccess* dbAccess
                         ,QWidget *parent) : QWidget(parent)
  , IRSGraphView()
  , ui(new Ui::RSGraphView)
  , m_databaseAccess(dbAccess)
  , m_noiseFailsFidelityDataGrid(0)

  //sensors distributions
  , m_fidelityByTechnoHisto("Technology",QStringList() << tr("Fidelity") , tr("Fidelity by technology"))
  , m_fidelityByOutputSignalHisto("O. Signal",QStringList() << "Fidelity" , "Fidelity by output signal")
  , m_fidelityByPhysicalMeasHisto("Phys. Meas.",QStringList() << "Fidelity" , "Fidelity by Physical Meas.")

  , m_noiseByPhysicalMeasHisto("Phys Meas.",QStringList() << "Noise" , "Noise by Physical Meas.")
  , m_noiseByOutputSignalHisto("O. Signal",QStringList() << "Noise" , "Noise by output signal")
  , m_noiseByTechnoHisto("Technology",QStringList() << "Noise" , "Noise by technology")

  , m_nbSensorByBrandHisto("Brand",QStringList() << "Nb. sensors" , "Nb. sensors by brand")
  , m_nbSensorByExpHisto("Experimentation",QStringList() << "Nb. sensors" , "Nb. sensors by experimentation")
  , m_nbSensorByTechnoHisto("Technology",QStringList() << "Nb. sensors" , "Nb. sensors by technology")
  , m_nbSensorByPhysicalMeasHisto("Phys. Meas.",QStringList() << "Nb. sensors" , "Nb. sensors by Physical Meas.")
  , m_nbSensorByOutputSignalHisto("O. Signal",QStringList() << "Nb. sensors" , "Nb. sensors by O. Signal")

  , m_failuresByTechnoHisto("Technology",QStringList()  << "Nb. Failures"<< "Avg failures (days)"<< "MTBF" , "Avg. failures(days) by techno.")
  , m_failuresByOutputSignalHisto("O. Signal",QStringList()  << "NB Failures"<< "Avg failures (Days)"<< "MTBF" , "Avg. failures(days) by O. signal")
  , m_failuresByPhysicalMeasHisto("Phys. Meas.",QStringList()  << "Nb. Failures"<< "Avg failures(Days)"<< "MTBF" , "Avg. failures(days) by phys. Meas.")

  , m_nbFailuresByTechnoHisto("Technology",QStringList() << "Nb. failures" , "Nb. failures by technology")

  , m_mtbfByOutputSignalHisto("Technology",QStringList()  << "Nb. Failures"<< "MTBF" , "MTBF by technology")
  , m_mtbfByTechnoHisto("O. signal",QStringList() << "Nb. Failures" << "MTBF" , "MTBF by O. signal")
  , m_mtbfByPhysicalMeasHisto("Physical Meas",QStringList()  << "Nb. Failures"<< "MTBF" , "MTBF by Physical Meas")

  , m_sensorByExpDataGrid(0)
  , m_sensorByBrandDataGrid(0)
  , m_sensorByTechnologyDataGrid(0)
  , m_sensorByOutputSignalDataGrid(0)
  , m_sensorByPhysicalMeasDataGrid(0)
  , m_sensorsDataGrid(0)
{
    ui->setupUi(this);

    position();

    createObjects();

    createConnections();

    updateSensorsDatagrid(0);

    displayTab(0);
}

RSGraphView::~RSGraphView()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
    delete ui;
}


void RSGraphView::plotCharts()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    //--- --Plot the HistoGrams
    //--Technology : Fidelity, noise, Failures
    initFidelityNoiseFailuresHistoWidgetData(m_fidelityByTechnoHisto.histoPlotArray() ,
                                             m_noiseByTechnoHisto.histoPlotArray() ,
                                             m_failuresByTechnoHisto.histoPlotArray(),
                                             RexStatistics::technologyColName());
    m_fidelityByTechnoHisto.updateModel();
    m_noiseByTechnoHisto.updateModel();
    m_failuresByTechnoHisto.updateModel();

    plotHistoGram(m_fidelityByTechnoHisto.customPlot(),
                  m_fidelityByTechnoHisto.name(),
                  m_fidelityByTechnoHisto.yLabel(),
                  m_fidelityByTechnoHisto.histoPlotArray());
    plotHistoGram(m_noiseByTechnoHisto.customPlot(),
                  m_noiseByTechnoHisto.name(),
                  m_noiseByTechnoHisto.yLabel(),
                  m_noiseByTechnoHisto.histoPlotArray());
    plotHistoGram(m_failuresByTechnoHisto.customPlot(),
                  m_failuresByTechnoHisto.name(),
                  m_failuresByTechnoHisto.yLabel(),
                  m_failuresByTechnoHisto.histoPlotArray());
    //nb fails by technology
    {
        m_nbFailuresByTechnoHisto.histoPlotArray().clear();
        const QVector<HistoPlot>& plots =  m_failuresByTechnoHisto.histoPlotArray();
        for(int i =0 ; i < plots.size(); i++ )
        {
            HistoPlot histoPlot;
            histoPlot.value =  plots[i].nbFailures;
            histoPlot.tick =  plots[i].tick;
            histoPlot.dataLine.push_back(histoPlot.tick);
            histoPlot.dataLine.push_back(histoPlot.value);

            m_nbFailuresByTechnoHisto.histoPlotArray().push_back(histoPlot);
        }
        m_nbFailuresByTechnoHisto.updateModel();

        plotHistoGram(m_nbFailuresByTechnoHisto.customPlot(),
                      m_nbFailuresByTechnoHisto.name(),
                      m_nbFailuresByTechnoHisto.yLabel(),
                      plots);

    }


    //--Output signal : Fidelity, noise, Failures
    initFidelityNoiseFailuresHistoWidgetData(m_fidelityByOutputSignalHisto.histoPlotArray() ,
                                             m_noiseByOutputSignalHisto.histoPlotArray() ,
                                             m_failuresByOutputSignalHisto.histoPlotArray(),
                                             RexStatistics::outputSignalColName());
    m_fidelityByOutputSignalHisto.updateModel();
    m_noiseByOutputSignalHisto.updateModel();
    m_failuresByOutputSignalHisto.updateModel();

    plotHistoGram(m_fidelityByOutputSignalHisto.customPlot(),
                  m_fidelityByOutputSignalHisto.name(),
                  m_fidelityByOutputSignalHisto.yLabel()
                  ,m_fidelityByOutputSignalHisto.histoPlotArray());
    plotHistoGram(m_noiseByOutputSignalHisto.customPlot(),
                  m_noiseByOutputSignalHisto.name(),
                  m_noiseByOutputSignalHisto.yLabel(),
                  m_noiseByOutputSignalHisto.histoPlotArray());
    plotHistoGram(m_failuresByOutputSignalHisto.customPlot(),
                  m_failuresByOutputSignalHisto.name(),
                  m_failuresByOutputSignalHisto.yLabel(),
                  m_failuresByOutputSignalHisto.histoPlotArray());

    //--Physical measurment signal : Fidelity, noise, Failures
    initFidelityNoiseFailuresHistoWidgetData(m_fidelityByPhysicalMeasHisto.histoPlotArray() ,
                                             m_noiseByPhysicalMeasHisto.histoPlotArray() ,
                                             m_failuresByPhysicalMeasHisto.histoPlotArray(),
                                             RexStatistics::physicalMeasColName());
    m_fidelityByPhysicalMeasHisto.updateModel();
    m_noiseByPhysicalMeasHisto.updateModel();
    m_failuresByPhysicalMeasHisto.updateModel();
    //m_mtbfByPhysicalMeasHisto.updateModel();

    plotHistoGram(m_fidelityByPhysicalMeasHisto.customPlot(),
                  m_fidelityByPhysicalMeasHisto.name(),
                  m_fidelityByPhysicalMeasHisto.yLabel()
                  ,m_fidelityByPhysicalMeasHisto.histoPlotArray());
    plotHistoGram(m_noiseByPhysicalMeasHisto.customPlot(),
                  m_noiseByPhysicalMeasHisto.name(),
                  m_noiseByPhysicalMeasHisto.yLabel(),
                  m_noiseByPhysicalMeasHisto.histoPlotArray());
    plotHistoGram(m_failuresByPhysicalMeasHisto.customPlot(),
                  m_failuresByPhysicalMeasHisto.name(),
                  m_failuresByPhysicalMeasHisto.yLabel(),
                  m_failuresByPhysicalMeasHisto.histoPlotArray());

    //sensors distributions
    //-------------------------------------------------------------------------------------------------
    //--- --Nb sensor by brand
    initNbSensorHistoWidgetData(RexStatistics::brandColName(),m_nbSensorByBrandHisto.histoPlotArray());
    m_nbSensorByBrandHisto.updateModel();
    plotHistoGram(m_nbSensorByBrandHisto.customPlot(),m_nbSensorByBrandHisto.name(),
                  m_nbSensorByBrandHisto.yLabel(),m_nbSensorByBrandHisto.histoPlotArray());

    //--- --Nb sensor by technology
    initNbSensorHistoWidgetData(RexStatistics::technologyColName(),m_nbSensorByTechnoHisto.histoPlotArray());
    m_nbSensorByTechnoHisto.updateModel();
    plotHistoGram(m_nbSensorByTechnoHisto.customPlot(),m_nbSensorByTechnoHisto.name(),
                  m_nbSensorByTechnoHisto.yLabel(),m_nbSensorByTechnoHisto.histoPlotArray());

    //--- --Nb sensor by experimentation
    initNbSensorByExperimentationHistoArray(m_nbSensorByExpHisto.histoPlotArray());
    m_nbSensorByExpHisto.updateModel();
    plotHistoGram(m_nbSensorByExpHisto.customPlot(),m_nbSensorByExpHisto.name(),
                  m_nbSensorByExpHisto.yLabel(),m_nbSensorByExpHisto.histoPlotArray());


    initNbSensorHistoWidgetData(RexStatistics::physicalMeasColName(),m_nbSensorByPhysicalMeasHisto.histoPlotArray());
    m_nbSensorByPhysicalMeasHisto.updateModel();
    plotHistoGram(m_nbSensorByPhysicalMeasHisto.customPlot(),m_nbSensorByPhysicalMeasHisto.name(),
                  m_nbSensorByPhysicalMeasHisto.yLabel(),m_nbSensorByPhysicalMeasHisto.histoPlotArray());


    initNbSensorHistoWidgetData(RexStatistics::outputSignalColName(),m_nbSensorByOutputSignalHisto.histoPlotArray());
    m_nbSensorByOutputSignalHisto.updateModel();
    plotHistoGram(m_nbSensorByOutputSignalHisto.customPlot(),m_nbSensorByOutputSignalHisto.name(),
                  m_nbSensorByOutputSignalHisto.yLabel(),m_nbSensorByOutputSignalHisto.histoPlotArray());

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSGraphView::position()
{
    QSize m_desktopSize = QApplication::desktop()->size();
    QSize m_screenSize = QSize(600, 600);

    int m_x = m_desktopSize.width() - m_screenSize.width() - 20;
    int m_y = m_desktopSize.height() - m_screenSize.height() - 400;
    int m_w = m_screenSize.width();
    int m_h = m_screenSize.height();

    setGeometry(m_x, m_y, m_w, m_h);
    show();
}


void  RSGraphView::createDistributionsHistogramsPage()
{
    ui->m_tabView->tabBar()->setExpanding(true);

    QWidget* container = ui->m_sensorsDistributionChartTab;
    QSplitter *mainSplitter = new QSplitter(/*Parent killer*/container);
    container->setLayout(new QGridLayout(container));

    //brand
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_nbSensorByBrandHisto);
        mainSplitter->addWidget(splitter);
    };

    //Technology
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_nbSensorByTechnoHisto);
        mainSplitter->addWidget(splitter);
    };

    //Experimentation
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_nbSensorByExpHisto);
        mainSplitter->addWidget(splitter);
    };

    //Physical Measurement
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_nbSensorByPhysicalMeasHisto);
        mainSplitter->addWidget(splitter);
    };

    //Output signal
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_nbSensorByOutputSignalHisto);
        mainSplitter->addWidget(splitter);
    };

    mainSplitter->setOrientation(Qt::Vertical);
    mainSplitter->setOpaqueResize( false );
    mainSplitter->setChildrenCollapsible( true );
    mainSplitter->setWindowState(Qt::WindowMaximized);
    mainSplitter->setFrameStyle(QFrame::StyledPanel);
    mainSplitter->setHandleWidth(1);

    mainSplitter->setHandleWidth(1);
    container->layout()->addWidget(mainSplitter);
}

void RSGraphView::createFailuresHistogramsPage()
{
    ui->m_tabView->tabBar()->setExpanding(true);

    QWidget* container = ui->m_failuresChartsPage;
    QSplitter *mainSplitter = new QSplitter(/*Parent killer*/container);
    container->setLayout(new QGridLayout(container));

    //TECHNOLOGY
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_failuresByTechnoHisto);
        mainSplitter->addWidget(splitter);
    };

    //Output signal
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_failuresByOutputSignalHisto);
        mainSplitter->addWidget(splitter);
    };

    //Output signal
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_failuresByPhysicalMeasHisto);
        mainSplitter->addWidget(splitter);
    };
    //Output signal
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_nbFailuresByTechnoHisto);
        mainSplitter->addWidget(splitter);
    };

    mainSplitter->setOrientation(Qt::Vertical);
    mainSplitter->setOpaqueResize( false );
    mainSplitter->setChildrenCollapsible( true );
    mainSplitter->setWindowState(Qt::WindowMaximized);
    mainSplitter->setFrameStyle(QFrame::StyledPanel);
    mainSplitter->setHandleWidth(1);

    mainSplitter->setHandleWidth(1);
    container->layout()->addWidget(mainSplitter);
}

void  RSGraphView::createNoisesHistogramsPage()
{
    ui->m_tabView->tabBar()->setExpanding(true);

    QWidget* container = ui->m_noisesChartsPage;
    QSplitter *mainSplitter = new QSplitter(/*Parent killer*/container);
    container->setLayout(new QGridLayout(container));

    //TECHNOLOGY
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_noiseByTechnoHisto);
        mainSplitter->addWidget(splitter);
    };

    //Output signal
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_noiseByOutputSignalHisto);
        mainSplitter->addWidget(splitter);
    };

    //Output signal
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_noiseByPhysicalMeasHisto);
        mainSplitter->addWidget(splitter);
    };

    mainSplitter->setOrientation(Qt::Vertical);
    mainSplitter->setOpaqueResize( false );
    mainSplitter->setChildrenCollapsible( true );
    mainSplitter->setWindowState(Qt::WindowMaximized);
    mainSplitter->setFrameStyle(QFrame::StyledPanel);
    mainSplitter->setHandleWidth(1);

    mainSplitter->setHandleWidth(1);
    container->layout()->addWidget(mainSplitter);
}

void RSGraphView::createFidelitiesHistogramsPage()
{
    ui->m_tabView->tabBar()->setExpanding(true);

    QWidget* container = ui->m_fidelitiesChartsTab;
    QSplitter *mainSplitter = new QSplitter(/*Parent killer*/container);
    container->setLayout(new QGridLayout(container));

    //TECHNOLOGY
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_fidelityByTechnoHisto);
        mainSplitter->addWidget(splitter);
    };

    //Output signal
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_fidelityByOutputSignalHisto);
        mainSplitter->addWidget(splitter);
    };

    //Output signal
    //--------------------------------------------------
    {
        QSplitter* splitter = new QSplitter(/*Parent killer*/container);
        splitter->addWidget(&m_fidelityByPhysicalMeasHisto);
        mainSplitter->addWidget(splitter);
    };

    mainSplitter->setOrientation(Qt::Vertical);
    mainSplitter->setOpaqueResize( false );
    mainSplitter->setChildrenCollapsible( true );
    mainSplitter->setWindowState(Qt::WindowMaximized);
    mainSplitter->setFrameStyle(QFrame::StyledPanel);
    mainSplitter->setHandleWidth(1);

    mainSplitter->setHandleWidth(1);
    container->layout()->addWidget(mainSplitter);

}

void  RSGraphView::createStatsDataPage()
{
    initDatagridPointers();

    createDatagridPage( ui->m_dataPageNoiseFailsFidelity,m_noiseFailsFidelityDataGrid);

    createDatagridPage( ui->m_sensorsDatagridTab,m_sensorsDataGrid);

}

void RSGraphView::createDatagridPage(QWidget* container,RexDataGrid* dataGrid)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Create the general data table");

    initDatagridPointers();

    QSplitter *mainSplitter = new QSplitter(/*Parent killer*/container);

    ui->m_tabView->tabBar()->setExpanding(true);

    mainSplitter->addWidget(dataGrid);

    mainSplitter->setOrientation(Qt::Horizontal);

    if(!mainSplitter)
    {
        QMessageBox::information(0,"Error","Failed to build an object");
        return ;
    }

    mainSplitter->setOrientation(Qt::Vertical);
    mainSplitter->setOpaqueResize( false );
    mainSplitter->setChildrenCollapsible( true );
    mainSplitter->setWindowState(Qt::WindowMaximized);
    mainSplitter->setFrameStyle(QFrame::StyledPanel);
    mainSplitter->setHandleWidth(1);
    container->setLayout(new QGridLayout(container));
    container->layout()->addWidget(mainSplitter);
    mainSplitter->setHandleWidth(1);

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSGraphView::createObjects()
{
    ui->m_stepMinButton->setIcon(RSPictoManager::Instance()->getIcon(fa::fastbackward, "black"));
    ui->m_stepMaxButton->setIcon(RSPictoManager::Instance()->getIcon(fa::fastforward, "black"));
    ui->m_stepPreviousButton->setIcon(RSPictoManager::Instance()->getIcon(fa::stepbackward, "black"));
    ui->m_stepNextButton->setIcon(RSPictoManager::Instance()->getIcon(fa::stepforward, "black"));

    //m_sensorsComboBox
    initDatagridPointers();
    m_noiseFailsFidelityDataGrid->ui().m_comboBox->setVisible(false);

    m_sensorsDataGrid->ui().progressBar->setVisible(false);
    m_sensorsDataGrid->ui().m_comboBox->setVisible(true);
    m_sensorsDataGrid->ui().m_comboBox->addItem("List of sensors by experimentation");
    m_sensorsDataGrid->ui().m_comboBox->addItem("List of sensors by brand");
    m_sensorsDataGrid->ui().m_comboBox->addItem("List of sensors by technology");
    m_sensorsDataGrid->ui().m_comboBox->addItem("List of sensors by physical measurement");
    m_sensorsDataGrid->ui().m_comboBox->addItem("List of sensors by Measure range");

    m_sensorsDataGrid->ui().m_runReportsCalculations->setVisible(false);
    loadSettings(QString());

    m_colors << "red" << "green" << "blue" << "cyan" << "teal" << "maroon";
    m_plotMap.insert("Trend", ui->m_trendPlot);
    m_plotMap.insert("Deviation", ui->m_deviationPlot);
    ui->m_tabView->setCurrentWidget(ui->m_trendTab);
    ui->m_tabView->setStyleSheet("font: 11pt Arial Narrow;");

    m_lastOneOnly = true;
    m_runFlag = false;
    m_sensorNameIndexOneOnly = true;

    createStatsDataPage();

    createFidelitiesHistogramsPage();
    createFailuresHistogramsPage();
    createNoisesHistogramsPage();
    createDistributionsHistogramsPage();


    ui->m_tabView->tabBar()->setExpanding(true);

    m_sensorsDataGrid->progressBar()->setVisible(false);

}

int RSGraphView::step() const
{
    return ui->m_stepViewEdit->value();
}

int RSGraphView::minStep() const
{
    return ui->m_stepViewEdit->minimum();
}

int RSGraphView::maxStep() const
{
    return ui->m_stepViewEdit->maximum();
}

void RSGraphView::createConnections()
{
    //Button Run
    connect(ui->m_trendRunButton, SIGNAL(clicked()),
            this, SLOT(slotTrendRunButtonClicked()));

    //Button Clear
    connect(ui->m_trendClearButton, SIGNAL(clicked()),
            this, SLOT(slotTrendClearButtonClicked()));

    //Button Run
    connect(ui->m_deviationRunButton, SIGNAL(clicked()),
            this, SLOT(slotTrendRunButtonClicked()));

    //Button Clear
    connect(ui->m_deviationClearButton, SIGNAL(clicked()),
            this, SLOT(slotTrendClearButtonClicked()));

    //Button averages
    connect(ui->m_averagesButton, SIGNAL(clicked()),
            this, SLOT(slot_averagesButtonClicked()));

    //Steps buttons
    connect(ui->m_stepMinButton, SIGNAL(clicked()), this, SLOT(slotStepMinButtonClicked()));
    connect(ui->m_stepMaxButton, SIGNAL(clicked()), this, SLOT(slotStepMaxButtonClicked()));
    connect(ui->m_stepPreviousButton, SIGNAL(clicked()), this, SLOT(slotStepPreviousButtonClicked()));
    connect(ui->m_stepNextButton, SIGNAL(clicked()), this, SLOT(slotStepNextButtonClicked()));
    connect(ui->m_stepViewEdit, SIGNAL(valueChanged(int)), this, SLOT(slotStepViewValueChanged(int)));

    //ComboBox
    connect(ui->m_sensorNameEdit, SIGNAL(currentIndexChanged(QString))
            , this, SLOT(slotSensorNameIndexChanged(QString)));
    //ComboBox
    connect(m_sensorsDataGrid->ui().m_comboBox, SIGNAL(currentIndexChanged(int))
            , this, SLOT(slotSensorListNameIndexChanged(int)));

}

void RSGraphView::slot_rsFilterIndexChanged()
{
    RSLogger::instance()->info(Q_FUNC_INFO," setSensorNameList");

    setSensorNameList();
}

void RSGraphView::slotTrendRunButtonClicked()
{
    RSLogger::instance()->info(Q_FUNC_INFO," emit signal_plotCurves");

    emit Signaler::instance()->signal_plotCurves();
}

void RSGraphView::slot_deviationRunButtonClicked()
{
    RSLogger::instance()->info(Q_FUNC_INFO," emit signal Plots curves");

    emit Signaler::instance()->signal_plotDeviationCurves();
}

void RSGraphView::slotTrendClearButtonClicked()
{
    RSLogger::instance()->info(Q_FUNC_INFO," clearGraphs");

    clearGraphs();
}

void RSGraphView::slotStepMinButtonClicked()
{
    RSLogger::instance()->info(Q_FUNC_INFO," set step : " + RSGraphViewDefaultSettings::DEFAULT_STEP_VIEW_IDX_MIN);

    ui->m_stepViewEdit->setValue(RSGraphViewDefaultSettings::DEFAULT_STEP_VIEW_IDX_MIN + 1);
}

void RSGraphView::slotStepMaxButtonClicked()
{
    int stepViewMax = RSDataManager::Instance()->getData("StepViewMax").toInt();

    RSLogger::instance()->info(Q_FUNC_INFO," set step : " + QString::number(stepViewMax));

    ui->m_stepViewEdit->setValue(stepViewMax);
}

void RSGraphView::slotStepPreviousButtonClicked()
{
    RSLogger::instance()->info(Q_FUNC_INFO," step Down m_stepViewEdit" );
    ui->m_stepViewEdit->stepDown();
}

void RSGraphView::slotStepNextButtonClicked()
{
    RSLogger::instance()->info(Q_FUNC_INFO," step Up " );

    ui->m_stepViewEdit->stepUp();
}

void RSGraphView::slotStepViewValueChanged(int data)
{
    RSDataManager::Instance()->setData("StepView", data);

    //Inform the Logic
    RSLogger::instance()->info(Q_FUNC_INFO," emit signal_graphViewStepChanged" );

    emit Signaler::instance()->signal_graphViewStepChanged(data);
}

void RSGraphView::slotStepViewValueChangedEx(int data)
{
    RSLogger::instance()->info(Q_FUNC_INFO," set value : " + QString::number(data) );
    ui->m_stepViewEdit->setValue(data + 1);
}

void RSGraphView::slotStepViewMaxValueChanged(int data)
{
    ui->m_stepViewEdit->setMaximum(data);
    ui->m_stepViewEdit->setSuffix(QString(" / %1").arg(data));
}

void RSGraphView::slotSensorNameIndexChanged(const QString& sensorName)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

//    m_sensorCode = m_databaseAccess->getSensorNameCode(sensorName);

    QPair<int,MeasPointType> pair = m_databaseAccess->getSensorNameCodeAndType(sensorName);
    m_sensorCode = pair.first;
    m_measPointType = pair.second;

    int index = ui->m_sensorNameEdit->currentIndex();

    RSDataManager::Instance()->setData("SensorName", sensorName);
    RSDataManager::Instance()->setData("SensorNameCode", m_sensorCode);
    RSDataManager::Instance()->setData("measPointType", static_cast<int>( m_measPointType));
    RSDataManager::Instance()->setData("SensorNameIndex", index);
    RSMessageView::Instance()->showData(QString("%1 \t CODE: %2").arg(sensorName).arg(m_sensorCode));

    //Put the step to the lowest value
    ui->m_stepViewEdit->setValue(this->maxStep());

    RSLogger::instance()->info(Q_FUNC_INFO, "End. emitUpdateStepViewMax");

    //Emit a signal ?
    //Clear the graphics
    clearGraphs();

}

void RSGraphView::slotSensorListNameIndexChanged(int index)
{
    updateSensorsDatagrid(index);
}

void RSGraphView::updateSensorsDatagrid(int index)
{
    if(!m_sensorsDataGrid || !m_sensorByExpDataGrid || !m_sensorByBrandDataGrid || !m_sensorByTechnologyDataGrid || !m_sensorByPhysicalMeasDataGrid)
    {
        initDatagridPointers();
        initSensorsListDatagrid();
    }

    if(index == 0) //Experi
    {
        m_sensorsDataGrid->setDataModelMatrix(m_sensorByExpDataGrid->getFields(),m_sensorByExpDataGrid->getData());
    }
    else if(index == 1) //Experi
    {
        m_sensorsDataGrid->setDataModelMatrix(m_sensorByBrandDataGrid->getFields(),m_sensorByBrandDataGrid->getData());
    }
    else if(index == 2) //Experi
    {
        m_sensorsDataGrid->setDataModelMatrix(m_sensorByTechnologyDataGrid->getFields(),m_sensorByTechnologyDataGrid->getData());
    }
    else if(index == 3) //Experi
    {
        m_sensorsDataGrid->setDataModelMatrix(m_sensorByPhysicalMeasDataGrid->getFields(),m_sensorByPhysicalMeasDataGrid->getData());
    }
    else if(index == 4) //Experi
    {
        m_sensorsDataGrid->setDataModelMatrix(m_sensorsByMeasureRangeModel.fieldsList(),m_sensorsByMeasureRangeModel.dataMatrix());
    }
    else if(index == 5) //Experi
    {
        m_sensorsDataGrid->setDataModelMatrix(m_sensorByOutputSignalDataGrid->getFields(),m_sensorByOutputSignalDataGrid->getData());
    }


    ui->m_tabView->setCurrentWidget(ui->m_sensorsDatagridTab);
}

QPair<int,MeasPointType> RSGraphView::getSensorCodeTypePair() const
{
    QString& sensorName = ui->m_sensorNameEdit->currentText();
    QPair<int,MeasPointType>  pair = m_databaseAccess->getSensorNameCodeAndType(sensorName);

    return pair;
}


void RSGraphView::clearGraphs()
{
    RSLogger::instance()->info(Q_FUNC_INFO, QString("Start. Clear %1 Graphs").arg(m_plotMap.count()));
    foreach (QCustomPlot *plot, m_plotMap.values())
    {
        plot->clearGraphs();
        plot->clearItems();
        plot->replot();
    }

    m_lastOneOnly = true;
    RSLogger::instance()->info(Q_FUNC_INFO, QString("End"));
}

void RSGraphView::clearGraphs(const QDate& startDate, const QDate& endDate)
{
    foreach (QCustomPlot *m_plot, m_plotMap.values())
    {
        m_plot->clearGraphs();
        m_plot->clearItems();

        QCPItemText *m_noData = new QCPItemText(m_plot);
        QString m_startFormat = startDate.toString("yyyy-MM-dd");
        QString m_endFormat = endDate.toString("yyyy-MM-dd");
        QString m_message = QString("No Data between %1 and %2")
                .arg(m_startFormat).arg(m_endFormat);
        m_noData->setText(m_message);
        m_noData->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
        m_noData->position->setType(QCPItemPosition::ptAxisRectRatio);
        m_noData->position->setCoords(0.5, 0.5);
        m_noData->setFont(QFont("Impact", 16));
        m_noData->setPen(QPen(Qt::NoPen));

        QVector<double> m_xTicks(2); m_xTicks[0] = 0.1; m_xTicks[1] = 1;
        QVector<QString> m_xLabels(2); m_xLabels[0] = m_startFormat; m_xLabels[1] = m_endFormat;
        QVector<QString> m_yLabels(2); m_yLabels[0] = "ND"; m_yLabels[1] = "ND";

        QSharedPointer<QCPAxisTickerText> m_xTicker(new QCPAxisTickerText);
        m_xTicker->addTicks(m_xTicks, m_xLabels);

        QSharedPointer<QCPAxisTickerText> m_yTicker(new QCPAxisTickerText);
        m_yTicker->addTicks(m_xTicks, m_yLabels);

        m_plot->xAxis->setRange(0, 1.1);
        m_plot->xAxis->setLabel("x");
        m_plot->xAxis->setTicker(m_xTicker);
        m_plot->xAxis->setTickLabelRotation(60);

        m_plot->yAxis->setRange(0, 1.1);
        m_plot->yAxis->setLabel("y");
        m_plot->yAxis->setTicker(m_yTicker);

        m_plot->replot();
    }

    m_lastOneOnly = true;
}

void RSGraphView::add(const QList<double>& x, const QList<double>& y, const QString& plotName, int width)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"start");

    double min = RSGlobalMethods::Instance()->min(y);
    double max = RSGlobalMethods::Instance()->max(y);

    if(m_lastOneOnly == true)
    {
        m_lastMinMap.insert(plotName, min);
        m_lastMaxMap.insert(plotName, max);
        m_lastOneOnly = false;
    }

    if(m_lastMinMap.value(plotName) > min)
    {
        m_lastMinMap.insert(plotName, min);
    }

    if(m_lastMaxMap.value(plotName) < max)
    {
        m_lastMaxMap.insert(plotName, max);
    }

    double xMin = x.first();
    double xMax = x.last();
    //xMin = xMin - 0.1*(m_xMax - xMin);
    xMax = xMax + 0.1*(xMax - xMin);
    double m_yMin = m_lastMinMap.value(plotName) - 0.1*(m_lastMaxMap.value(plotName) - m_lastMinMap.value(plotName));
    double m_yMax = m_lastMaxMap.value(plotName) + 0.1*(m_lastMaxMap.value(plotName) - m_lastMinMap.value(plotName));

    QCustomPlot *m_plot = m_plotMap.value(plotName);
    m_plot->addGraph();
    int m_index = m_plot->graphCount() - 1;
    int m_colorId = m_index % m_colors.size();
    m_plot->graph(m_index)->setData(x.toVector(), y.toVector());
    m_plot->graph(m_index)->setPen(QPen(QBrush(QColor(m_colors.at(m_colorId))), width));

    m_plot->xAxis->setRange(xMin, xMax);
    m_plot->xAxis->setLabel("x");

    m_plot->yAxis->setRange(m_yMin, m_yMax);
    m_plot->yAxis->setLabel("y");

    m_plot->replot();

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSGraphView::add(const QList<double>& x, const QList<double>& y, const QDateTime& startDateTime, const QString& plotName, int step, int width)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"start");

    double m_min = RSGlobalMethods::Instance()->min(y);
    double m_max = RSGlobalMethods::Instance()->max(y);

    if(m_lastOneOnly == true)
    {
        m_lastMinMap.insert(plotName, m_min);
        m_lastMaxMap.insert(plotName, m_max);
        m_lastOneOnly = false;
    }

    if(m_lastMinMap.value(plotName) > m_min)
    {
        m_lastMinMap.insert(plotName, m_min);
    }

    if(m_lastMaxMap.value(plotName) < m_max)
    {
        m_lastMaxMap.insert(plotName, m_max);
    }

    double xMin = x.first();
    double xMax = x.last();
    //m_xMin = m_xMin - 0.1*(m_xMax - m_xMin);
    xMax = xMax + 0.1*(xMax - xMin);
    double m_yMin = m_lastMinMap.value(plotName) - 0.1*(m_lastMaxMap.value(plotName) - m_lastMinMap.value(plotName));
    double m_yMax = m_lastMaxMap.value(plotName) + 0.1*(m_lastMaxMap.value(plotName) - m_lastMinMap.value(plotName));
    int m_step = RSGlobalMethods::Instance()->getStepNumber(x.size(), step);

    QCustomPlot *m_plot = m_plotMap.value(plotName);
    m_plot->addGraph();
    int m_index = m_plot->graphCount() - 1;
    int m_colorId = m_index % m_colors.size();
    m_plot->graph(m_index)->setData(x.toVector(), y.toVector());
    m_plot->graph(m_index)->setPen(QPen(QBrush(QColor(m_colors.at(m_colorId))), width));

    QVector<double> m_xTicks = RSGlobalMethods::Instance()->getRealTime(x, m_step).toVector();
    QVector<QString> m_xLabels = RSGlobalMethods::Instance()->getRealTime(x, startDateTime, m_step).toVector();
    QSharedPointer<QCPAxisTickerText> m_xTicker(new QCPAxisTickerText);
    m_xTicker->addTicks(m_xTicks, m_xLabels);
    m_xTicker->setSubTickCount(step);

    m_plot->xAxis->setRange(xMin, xMax);
    m_plot->xAxis->setLabel("x");
    m_plot->xAxis->setTicker(m_xTicker);
    m_plot->xAxis->setTickLabelRotation(60);
    m_plot->xAxis->setSubTickLength(3, 3);
    m_plot->xAxis->setTickLength(6, 6);

    m_plot->yAxis->setRange(m_yMin, m_yMax);
    m_plot->yAxis->setLabel("y");
    QSharedPointer<QCPAxisTicker> m_yTicker(new QCPAxisTicker);
    m_plot->yAxis->setTicker(m_yTicker);

    m_plot->replot();
    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSGraphView::setSensorNameList()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");
    ui->m_sensorNameEdit->blockSignals(true);

    //2--- --Clear the current list
    ui->m_sensorNameEdit->clear();

    //1--- --Get the current sensors in REXFILTER table
    m_sensorsDetailedInfo = m_databaseAccess->getSensorsDetailedInfoSet();
    QStringList dataList = m_databaseAccess->getSensorNameList();

    //Block if bad structure
    if(!RSDatabaseAccess::Instance()->isG6StructureOK() || !RSDatabaseAccess::Instance()->isG7StructureOK())
        dataList.clear();

    if(dataList.size() == 0)
    {
        ui->m_sensorNameEdit->blockSignals(false);
        return;
    }

    //3--- --Add the new list
    ui->m_sensorNameEdit->addItems(dataList);
    int sensorNameIndex = RSDataManager::Instance()->getData("SensorNameIndex").toInt();
    sensorNameIndex = 0;
    ui->m_sensorNameEdit->setCurrentIndex(sensorNameIndex);

    //4--- --Get the sensor code
    int sensorCode = m_databaseAccess->getSensorNameCode(dataList.at(sensorNameIndex));

    //--- --Display the sensor code in the message view
    QString msg = QString("%1 \t AP_CODE: %2").arg(dataList.at(sensorNameIndex)).arg(sensorCode);
    RSLogger::instance()->info(Q_FUNC_INFO, msg);
    RSMessageView::Instance()->showData(msg);

    //Get the current sensor name on the combo
    QString m_sensorName = ui->m_sensorNameEdit->currentText();
    int m_sensorNameCode = m_databaseAccess->getSensorNameCode(m_sensorName);

    //update the DataMaager
    RSDataManager::Instance()->setData("SensorName", m_sensorName);
    RSDataManager::Instance()->setData("SensorNameCode", m_sensorNameCode);
    RSDataManager::Instance()->setData("SensorNameIndex", sensorNameIndex);

    ui->m_sensorNameEdit->blockSignals(false);

    RSLogger::instance()->info(Q_FUNC_INFO, QString("End. m_sensorName = %1  \t nbSensors = %2").arg(m_sensorName).arg(dataList.count()) );
}


void RSGraphView::initSensorsCodeNameMap()
{
    m_sensorsCodeNameMap.clear();
    QStringList dataList = m_databaseAccess->getSensorNameList();
    Q_FOREACH(const QString& name,dataList)
    {
        int code =  m_databaseAccess->getSensorNameCode(name);
        m_sensorsCodeNameMap[code] = name;
    }

}

const QMap<int,QString>& RSGraphView::sensorsCodeNameMap()
{
    if(m_sensorsCodeNameMap.isEmpty())
        initSensorsCodeNameMap();
    return m_sensorsCodeNameMap;
}

QStringList RSGraphView::getCurSensorNamesList() const
{
    QStringList sensorsList;

    for (int i = 0; i < ui->m_sensorNameEdit->count(); i++)
        sensorsList.append( ui->m_sensorNameEdit->itemText(i));

    return sensorsList;
}

QString RSGraphView::currentExperimentationName() const
{
    return ui->m_sensorNameEdit->currentText();
}

void RSGraphView::clearGraphsAndSensorList()
{
    ui->m_sensorNameEdit->blockSignals(true);
    ui->m_sensorNameEdit->clear();
    ui->m_sensorNameEdit->blockSignals(false);
    clearGraphs();
}



void RSGraphView::loadSettings(const QString& fileName)
{
    loadSensorName();

    loadStepView();
}

void RSGraphView::saveSettings(const QString& fileName) const
{
    saveSensorName();

    saveStepView();
}

QVariant RSGraphView::loadSensorName()
{
    QString m_id = "RSGraphView";
    QString m_key = "RSGraphView.SensorName";
    QVariant m_default = RSGraphViewDefaultSettings::DEFAULT_SENSOR_NAME_INDEX;

    //Get the sensor Name
    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, ui->m_sensorNameEdit->itemData(0).value<QString>());
    const QString sensorName = data.value<QString>();

    //Look for the index
    int index = ui->m_sensorNameEdit->findText(sensorName);
    if(index < 0)
        index = RSGraphViewDefaultSettings::DEFAULT_SENSOR_NAME_INDEX;

    ui->m_sensorNameEdit->blockSignals(true);
    ui->m_sensorNameEdit->setCurrentIndex(index);
    ui->m_sensorNameEdit->blockSignals(false);

    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
}

void RSGraphView::saveSensorName()const
{
    QString m_id = "RSGraphView";
    QString m_key = "RSGraphView.SensorName";
    QVariant data = ui->m_sensorNameEdit->currentText();
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

QVariant  RSGraphView::loadStepView()
{
    QString m_id = "RSGraphView";
    QString m_key = "RSGraphView.stepView";
    QVariant m_default = RSGraphViewDefaultSettings::DEFAULT_STEP_INDEX;

    ui->m_stepViewEdit->blockSignals(true);
    ui->m_stepViewEdit->setValue(RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default).value<int>());
    ui->m_stepViewEdit->blockSignals(false);

    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
}

void  RSGraphView::saveStepView()const
{
    QString m_id = "RSGraphView";
    QString m_key = "RSGraphView.stepView";
    QVariant m_data = ui->m_stepViewEdit->value();

    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
}

void  RSGraphView::setStepViewMax(int step)
{
    m_stepViewMax = step;
    RSLogger::instance()->info(Q_FUNC_INFO,QString("maxStep = %1").arg(step));

    //If the current step is over the maxStep

    ui->m_stepViewEdit->blockSignals(true);
    ui->m_stepViewEdit->setValue(0 + 1 );
    ui->m_stepViewEdit->setMaximum(m_stepViewMax);
    ui->m_stepViewEdit->setSuffix(QString(" / %1").arg(m_stepViewMax));
    ui->m_stepViewEdit->blockSignals(false);

}

QString  RSGraphView::sensorName() const
{
    return ui->m_sensorNameEdit->currentText();
}

void RSGraphView::slot_averagesButtonClicked()
{
    ui->m_averagesLabel->setText("Compute Average......");
    emit Signaler::instance()->signal_computeAverage();
}

void RSGraphView::displayAverageValues(double fidelityAvg, double noiseAvg)
{
    ui->m_averagesLabel->setText(QString(" Average fidelity : %1 \t average Noise: %2").arg(fidelityAvg).arg(noiseAvg));
}

void RSGraphView::displayStepValues(double fidelityAvg, double noiseAvg)
{

}

IRexDataGrid* RSGraphView::fidelityNoiseFailsDataGrid()
{
    return m_noiseFailsFidelityDataGrid;
}


IRexDataGrid* RSGraphView::sensorByExpDataGrid()
{
    return m_sensorByExpDataGrid;
}

const QList<SensorInfos>& RSGraphView::sensorsDetailedInfo() const
{
    return m_sensorsDetailedInfo;
}

bool RSGraphView::initNbSensorByExperimentationHistoArray(QVector<HistoPlot>& outputArray)
{

    RSLogger::instance()->info(Q_FUNC_INFO,"Start");
    outputArray.clear();

    const DataGridModel* model = m_sensorByExpDataGrid->model();
    if(!model)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End. Fail. m_sensorsDataGrid->model() is NULL");
        return false;
    }

    //Get nb sensors by experimentation
    const QMap<QString,int>& map =  model->getNbValuesByFieldMap();
    //Get nb sensors
    outputArray.clear();
    Q_FOREACH(const QString& key, map.keys())
    {
        HistoPlot histoPlot;
        histoPlot.tick = key;
        histoPlot.value =  map[key];
        histoPlot.dataLine.push_back( key);//Tick = key
        histoPlot.dataLine.push_back( map[key]);
        outputArray.push_back(histoPlot);
    }

    //Create a model for the QTableView
    RSLogger::instance()->info(Q_FUNC_INFO,"End");
    return true;
}

bool RSGraphView::initNbSensorHistoWidgetData(const QString& dataName,QVector<HistoPlot>& outputArray)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");
    const DataGridModel* model  =  m_noiseFailsFidelityDataGrid->model();
    if(!model)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End. Fail. m_noiseFailsFidelityDataGrid->model() is NULL");
        return false;
    }

    //indexex in the dataGrid
    int col = model->headerIndex(dataName);
    QMap<QString,HistoPlot> map;

    //--- -- A Gather the datas in noiseFailsFidelityDataGrid
    //-----------------------------------------------------------
    for(int row  = 0; row < model->rowCount(); row++)
    {
        QString key = model->data(model->index(row, col)).toString().trimmed();
        if(key.isNull() || key.isEmpty() )
            continue;

        //--- --Nb sensor by brand
        if(map.contains(key))
        {
            map[key].value++;
        }
        else
        {
            map[key].tick = key;
            map[key].value = 1;
        }
    }

    //Get nb sensors by brand
    //--- --Noise by technology average
    outputArray.clear();
    Q_FOREACH(const QString& key, map.keys())
    {
        HistoPlot histoPlot;
        histoPlot.tick = key;
        histoPlot.value =  map[key].value;
        histoPlot.dataLine.push_back( key);
        histoPlot.dataLine.push_back( map[key].value);
        outputArray.push_back(histoPlot);
    }

    RSLogger::instance()->info(Q_FUNC_INFO,"End");

    return true;
}

bool RSGraphView::initFidelityNoiseFailuresHistoWidgetData(  QVector<HistoPlot>& fidelityArray
                                                             , QVector<HistoPlot>& noiseArray
                                                             , QVector<HistoPlot>& failuresArray
                                                             , const QString& dataName)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");
    const DataGridModel* model  =  m_noiseFailsFidelityDataGrid->model();
    if(!model)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End. Fail. m_noiseFailsFidelityDataGrid->model() is NULL");
        return false;
    }

    int dataNameCol =  model->headerIndex(dataName); //Column index of the data name
    int fidelityCol = model->headerIndex(RexStatistics::fidelityColName()); //Fdelity column
    int noiseCol =  model->headerIndex(RexStatistics::noiseColName()); //Noise column
    int failCol =  model->headerIndex(RexStatistics::failuresColName()); //Failures column

    QMap<QString,HistoPlot> fidelityMap;
    QMap<QString,HistoPlot> noiseMap;
    QMap<QString,HistoPlot> failMap;

    //--- -- A Gather the datas in noiseFailsFidelityDataGrid
    //-----------------------------------------------------------


    for(int row  = 0; row < model->rowCount(); row++)
    {
        double fidelity = 0;
        double noise = 0 ;
        double failure = 0 ;

        if( !model->data(model->index(row, fidelityCol)).isNull())
            fidelity = model->data(model->index(row, fidelityCol)).value<double>();
        if( !model->data(model->index(row, noiseCol)).isNull())
            noise = model->data(model->index(row, noiseCol)).value<double>();
        if( !model->data(model->index(row, failCol)).isNull())
            failure = model->data(model->index(row, failCol)).value<double>();


        if(! model->data(model->index(row, dataNameCol)).isNull())
        {
            QString key = model->data(model->index(row, dataNameCol)).toString().trimmed();
            if(key.isNull() || key.isEmpty() )
                continue;
            //--- -- fidelity
            if(fidelityMap.contains(key))
            {
                fidelityMap[key].value += fidelity;
                fidelityMap[key].anyUse++;
            }
            else
            {
                fidelityMap[key].value = fidelity;
                fidelityMap[key].anyUse = 1;
                fidelityMap[key].tick = key;
            }

            //--- -- Noise
            if(noiseMap.contains(key))
            {
                noiseMap[key].value += noise;
                noiseMap[key].anyUse++;
            }
            else
            {
                noiseMap[key].value = noise;
                noiseMap[key].anyUse = 1;
                noiseMap[key].tick = key;
            }

            //--- -- Failures
            if(failMap.contains(key))
            {
                failMap[key].value += failure;
                failMap[key].anyUse++;
                //number of failures
                if(failure > 0.001)
                    failMap[key].nbFailures++;
            }
            else
            {
                failMap[key].value = failure;
                failMap[key].anyUse = 1;
                failMap[key].tick = key;
                failMap[key].nbFailures = (failure > 0.001 ? 1 : 0);
            }

        }
    }

    //--- --Compute the averages
    fidelityArray.clear();
    Q_FOREACH(const QString& key, fidelityMap.keys())
    {
        if(fidelityMap.value(key).anyUse > 0)
        {
            HistoPlot histPlot = fidelityMap.value(key);
            histPlot.value =  histPlot.value/ histPlot.anyUse;
            histPlot.dataLine.push_back( histPlot.tick);
            histPlot.dataLine.push_back( histPlot.value);
            fidelityArray.push_back(histPlot);
        }
    }

    //--- --Noise by technology average
    noiseArray.clear();
    Q_FOREACH(const QString& key, noiseMap.keys())
    {
        if(noiseMap.value(key).anyUse > 0)
        {
            HistoPlot histPlot = noiseMap.value(key);
            histPlot.value =  histPlot.value/ histPlot.anyUse;
            histPlot.dataLine.push_back( histPlot.tick);
            histPlot.dataLine.push_back( histPlot.value);
            noiseArray.push_back(histPlot);

        }
    }

    //--- --Failures
    failuresArray.clear();
    Q_FOREACH(const QString& key, failMap.keys())
    {
        HistoPlot histPlot = failMap.value(key);
        double mtbf = 0;
        if(failMap.value(key).anyUse > 0)
        {
            histPlot.value =  histPlot.value/ histPlot.anyUse; //average failure
            mtbf = histPlot.nbFailures/histPlot.anyUse;
        }

        histPlot.dataLine.push_back( histPlot.tick);// avg failures/ nb sensors
        histPlot.dataLine.push_back( histPlot.value);// avg failures/ nb sensors
        histPlot.dataLine.push_back( histPlot.nbFailures);// Nb failures/ nb sensors
        histPlot.dataLine.push_back(mtbf);// MTBF = Nb failures/ nb sensors

        failuresArray.push_back(histPlot);
    }

    RSLogger::instance()->info(Q_FUNC_INFO,"End");

    return true;
}



void RSGraphView::plotHistoGram(QCustomPlot* customPlot
                                , const QString& serieName
                                , const QString& yLabel
                                , const QVector<HistoPlot>& histoPlotArray)
{

    RSLogger::instance()->info(Q_FUNC_INFO,"Add. serieName : " + serieName + "\tyLabel = " + yLabel);

    if(!customPlot)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"customPlot Null pointer");
        return;
    }


    customPlot->clearGraphs();
    customPlot->clearItems();
    customPlot->clearFocus();
    customPlot->clearPlottables();
    customPlot->clearMask();

    //--- --set dark background gradient:
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(90, 90, 90));
    gradient.setColorAt(0.38, QColor(105, 105, 105));
    gradient.setColorAt(1, QColor(70, 70, 70));
    customPlot->setBackground(QBrush(gradient));

    //--- -- create empty bar chart objects:
    QCPBars *chart = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    chart->setAntialiased(false);
    chart->setStackingGap(1);

    // set names and colors:
    chart->setName(serieName);
    chart->setPen(QPen(QColor(111, 9, 176).lighter(170)));
    chart->setBrush(QColor(111, 9, 176));

    //--- -- prepare x axis with labels:
    QVector<double> ticks;
    QVector<QString> labels;
    QVector<double> dataArray;

    int i = 0;
    Q_FOREACH(const HistoPlot& val,histoPlotArray)
    {
        if(val.tick.size() <= 9)
            labels << val.tick;
        else
            labels << val.tick.mid(0,7) + ".";

        ticks << i++;
        dataArray  << val.value;
    }

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);

    customPlot->xAxis->setTicker(textTicker);
    customPlot->xAxis->setTickLabelRotation(60);
    customPlot->xAxis->setSubTicks(false);
    customPlot->xAxis->setTickLength(0, 4);
    customPlot->xAxis->setRange(0, histoPlotArray.count());
    customPlot->xAxis->setBasePen(QPen(Qt::white));
    customPlot->xAxis->setTickPen(QPen(Qt::white));
    customPlot->xAxis->grid()->setVisible(true);
    customPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    customPlot->xAxis->setTickLabelColor(Qt::white);
    customPlot->xAxis->setLabelColor(Qt::white);

    //--- -- prepare y axis:
    {
        double yMax = 0;
        Q_FOREACH(const HistoPlot& histo,histoPlotArray)
            yMax = qMax(yMax,histo.value);

        if(yMax == 0)
            yMax = 5;
        RSLogger::instance()->info(Q_FUNC_INFO,QString("%1 YMAX = %2").arg(serieName).arg(yMax));
        customPlot->yAxis->setRange(0, yMax);
    }

    //customPlot->yAxis->setPadding(5);
    customPlot->yAxis->setLabel(yLabel);
    customPlot->yAxis->setBasePen(QPen(Qt::white));
    customPlot->yAxis->setTickPen(QPen(Qt::white));
    customPlot->yAxis->setSubTickPen(QPen(Qt::white));
    customPlot->yAxis->grid()->setSubGridVisible(true);
    customPlot->yAxis->setTickLabelColor(Qt::white);
    customPlot->yAxis->setLabelColor(Qt::white);
    customPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
    customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

    //--- -- Add data:
    chart->setData(ticks, dataArray);

    //--- --setup legend:
    customPlot->legend->setVisible(true);
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    ;
    customPlot->legend->setBrush(QColor(255, 255, 255, 100));

    customPlot->legend->setBorderPen(Qt::NoPen);

    QFont legendFont = font();

    legendFont.setPointSize(10);

    customPlot->legend->setFont(legendFont);

    customPlot->setInteractions(QCP::iRangeZoom);

    customPlot->replot();

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSGraphView::setProgressBar(double rate)
{
    if(m_noiseFailsFidelityDataGrid)
        m_noiseFailsFidelityDataGrid->progressBar()->setValue(rate  *  m_noiseFailsFidelityDataGrid->progressBar()->maximum());
}


void RSGraphView::clearDataGrids()
{
    m_sensorsDataGrid->clearData();
    m_noiseFailsFidelityDataGrid->clearData();
}

void RSGraphView::initSensorsListDatagrid()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Init datagrd pointers");

    initDatagridPointers();

    QMap<QString,QStringList> BrandMap;
    QMap<QString,QStringList> TechnoMap;
    QMap<QString,QStringList>  OutputSignalMap;
    QMap<QString,QStringList>  PhysicalMeasMap;
    QMap<QString,QStringList>  MeasRangeMap;

    //Build the maps
    Q_FOREACH(const SensorInfos& sensor, m_sensorsDetailedInfo)
    {
        if(!sensor.brand.isEmpty())
        {
            BrandMap[sensor.brand].append(sensor.name);
        }

        if(!sensor.technology.isEmpty())
        {
            TechnoMap[sensor.technology].append(sensor.name);
        }

        if(!sensor.outputSignal.isEmpty())
        {
            OutputSignalMap[sensor.outputSignal].append(sensor.name);
        }

        if(!sensor.measRange.isEmpty())
        {
            MeasRangeMap[sensor.measRange].append(sensor.name);
        }

        if(!sensor.physicalMeas.isEmpty())
        {
            PhysicalMeasMap[sensor.physicalMeas].append(sensor.name);
        }
    }

    //Set the dataGrid
    m_sensorByBrandDataGrid->setDataModelMatrix( BrandMap.keys(),getDataMaxtrixFromQMap(BrandMap));

    m_sensorByTechnologyDataGrid->setDataModelMatrix( TechnoMap.keys(),getDataMaxtrixFromQMap(TechnoMap));
    m_sensorByOutputSignalDataGrid->setDataModelMatrix( OutputSignalMap.keys(),getDataMaxtrixFromQMap(OutputSignalMap));
    m_sensorByPhysicalMeasDataGrid->setDataModelMatrix( PhysicalMeasMap.keys(),getDataMaxtrixFromQMap(PhysicalMeasMap));
    m_sensorsByMeasureRangeModel.setMatrix(MeasRangeMap.keys(),getDataMaxtrixFromQMap(MeasRangeMap));
}

QList<QVariantList> RSGraphView::getDataMaxtrixFromQMap( const QMap<QString,QStringList>& map) const
{
    QMap<QString,QStringList>;
    int rowCount = 0;

    Q_FOREACH(const QString& key, map.keys())
        rowCount = qMax(rowCount,map[key].size());

    QList<QVariantList> dataForGrid;
    for(int row = 0; row < rowCount; row++)
    {
        QVariantList list;
        for(int col = 0; col < map.keys().count(); col++)
            list << QVariant();
        dataForGrid.push_back(list);
    }

    for(int col = 0; col < map.keys().count(); col++)
    {
        const QString key = map.keys().at(col);
        const QStringList sensorsList = map[key];

        for(int row = 0; row < sensorsList.count(); row++)
        {
            QVariantList& rowData = const_cast<QVariantList&> (dataForGrid.at(row));
            const QString& sensorName = sensorsList.at(row);
            rowData.replace(col,sensorName);
        }
    }

    return dataForGrid;
}

void RSGraphView::initDatagridPointers()
{
    //Init datagrid
    if(!m_sensorByExpDataGrid)
    {
        m_sensorByExpDataGrid = new RexDataGrid(false/*use table view*/,this);
        m_sensorByExpDataGrid->setVisible(false);
    }
    if(!m_sensorByBrandDataGrid)
    {
        m_sensorByBrandDataGrid = new RexDataGrid(false/*use table view*/,this);
        m_sensorByBrandDataGrid->setVisible(false);
    }
    if(!m_sensorByTechnologyDataGrid)
    {
        m_sensorByTechnologyDataGrid = new RexDataGrid(false/*use table view*/,this);
        m_sensorByTechnologyDataGrid->setVisible(false);
    }
    if(!m_sensorByOutputSignalDataGrid)
    {
        m_sensorByOutputSignalDataGrid = new RexDataGrid(false/*use table view*/,this);
        m_sensorByOutputSignalDataGrid->setVisible(false);
    }
    if(!m_sensorByPhysicalMeasDataGrid)
    {
        m_sensorByPhysicalMeasDataGrid = new RexDataGrid(false/*use table view*/,this);
        m_sensorByPhysicalMeasDataGrid->setVisible(false);
    }

    if(!m_sensorsDataGrid)
        m_sensorsDataGrid = new RexDataGrid(false/*use table view*/,this);
    if(!m_noiseFailsFidelityDataGrid)
        m_noiseFailsFidelityDataGrid = new RexDataGrid(true/*use table view*/,this);

}

void RSGraphView::displayDataTab()
{
    ui->m_tabView->setCurrentWidget(ui->m_dataPageNoiseFailsFidelity);
}

void  RSGraphView::displayTab(int tableIndex)
{
    if(tableIndex < ui->m_tabView->count())
        ui->m_tabView->setCurrentIndex(tableIndex);
}
