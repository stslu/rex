#include <RSGraphView.h>
#include <RSLogger.h>
#include "RSLogicActionsManager.h"
#include "RSDataManager.h"
#include "RSDatabaseAccess.h"
#include "RSMessageView.h"
#include "RSDataComputation.h"
#include "RSExportToExcel.h"


RSLogicActionsManager::RSLogicActionsManager(RSDatabaseAccess* dbAccess,IRSMainWindow* mainWindow
                                             ,IRSTimesManager* timeManager
                                             ,IRSOptionsManager* optionsMgr
                                             ,IRSGraphView* graphView
                                             ,IRSFiltersManager* filter
                                             ,QObject *parent) : QObject(parent)
  , m_timeManagerPtr(timeManager)
  , m_optionsMgrPtr(optionsMgr)
  , m_graphView(graphView)
  , m_mainWidnow(mainWindow)
  , m_databaseAccess(dbAccess)
  , m_filterWidget(filter)
  , m_exitLoop(false)
{
    if(!m_graphView || !m_optionsMgrPtr || !m_graphView || !m_mainWidnow || !m_databaseAccess || !m_filterWidget)
    {
        QMessageBox::information(0,"Null pointer",Q_FUNC_INFO);
        exit(0);
    }

    connectSignals();
}

void  RSLogicActionsManager::connectSignals()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    Signaler* signaler = Signaler::instance();

    connect(signaler,SIGNAL(signal_runReportsCalculations()),this, SLOT(slot_runReportsCalculations()));

    connect(signaler,SIGNAL(signal_readyToStart()),this, SLOT(slot_readyToStart()));

    if(! connect(signaler,SIGNAL(signal_quit()),this, SLOT(slot_quit())))
        RSLogger::instance()->info(Q_FUNC_INFO,"Fail to connect  signal_quit");

    connect(signaler,SIGNAL(signal_plotCurves()),  this, SLOT(slot_plotCurves()));

    connect(signaler,SIGNAL(signal_exportToExcel()),  this, SLOT(slot_exportToExcel()));

    /*connect(signaler,SIGNAL(signal_emitMessage(QMessageBox::Icon , const QString&, const QString& , const QString& )()),
            this, SLOT(slot_emitMessage(QMessageBox::Icon , const QString&, const QString& , const QString& )()));*/

    connect(signaler,SIGNAL(signal_stepViewValueChanged(int)),  this, SLOT(slot_stepViewValueChanged(int)));

    connect(signaler,SIGNAL(signal_clearGraphsAndSensorList()),  this, SLOT(slot_clearGraphsAndSensorList()));

    if(!connect(signaler,SIGNAL(signal_rsFilterIndexChanged()) , this, SLOT(slot_rsFilterIndexChanged())))
        RSLogger::instance()->info(Q_FUNC_INFO,"Fail to connect  signal_rsFilterIndexChanged");

    if(! connect(signaler,SIGNAL(signal_graphViewStepChanged(int)),
                 this, SLOT(slot_graphViewStepChanged(int))))
        RSLogger::instance()->info(Q_FUNC_INFO,"Fail to connect  signal_graphViewStepChanged");

    connect(signaler,SIGNAL(signal_stepViewMaxChanged(int)),  this, SLOT(slot_stepViewMaxChanged(int)));

    connect(signaler,SIGNAL(signal_computeAverage()),  this, SLOT(slot_computeAverage()));

    connect(signaler,SIGNAL(signal_updateSensorsList()),  this, SLOT(slot_updateSensorsList()));

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSLogicActionsManager::slot_cancel()
{
    while(!m_exitLoopMutex.tryLock(1000))
        m_exitLoop = true;
    m_exitLoopMutex.unlock();
}


void RSLogicActionsManager::slot_readyToStart()
{

    RSLogger::instance()->info(Q_FUNC_INFO, "Start. Load settings");
    if(m_timeManagerPtr)
        m_timeManagerPtr->loadSettings(m_settingsFile);

    if(m_optionsMgrPtr)
        m_optionsMgrPtr->loadSettings(m_settingsFile);

    if(m_databaseAccess)
        m_databaseAccess->loadSettings(m_settingsFile);

    if(m_filterWidget)
        m_filterWidget->loadSettings(m_settingsFile);

    if(m_graphView)
    {
        m_graphView->loadSettings(m_settingsFile);

        m_graphView->setStepViewMax(m_timeManagerPtr->stepViewMax());
    }

    //Build the query
    m_filterWidget->initQueryAndRexFilterTable();

    m_graphView->displayTab(0);

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSLogicActionsManager::slot_quit()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start : save settings");

    if(m_timeManagerPtr)
        m_timeManagerPtr->saveSettings(m_settingsFile);

    if(m_optionsMgrPtr)
        m_optionsMgrPtr->saveSettings(m_settingsFile);

    if(m_graphView)
        m_graphView->saveSettings(m_settingsFile);

    if(m_databaseAccess)
        m_databaseAccess->saveSettings(m_settingsFile);

    if(m_filterWidget)
        m_filterWidget->saveSettings(m_settingsFile);


    RSLogger::instance()->info(Q_FUNC_INFO,"End");

}

void RSLogicActionsManager::slot_clearGraphsAndSensorList()
{
    m_graphView->clearGraphsAndSensorList();
}

void RSLogicActionsManager::slot_rsFilterIndexChanged()
{
    //update the sensor list
    RSLogger::instance()->info(Q_FUNC_INFO,"update the sensor list");

    m_graphView->setSensorNameList();

    m_filterWidget->displayNbSensors(m_graphView->sensorsDetailedInfo().size());

    //m_graphView->initSensorsListDatagrid();
}

void RSLogicActionsManager::slot_graphViewStepChanged(int data)
{
    buildCurves();
}

void  RSLogicActionsManager::slot_stepViewValueChanged(int data)
{
    m_timeManagerPtr->setStepViewValue(data);
}

void RSLogicActionsManager::slot_emitMessage(QMessageBox::Icon icon, const QString& color, const QString& title, const QString& msg)
{
    m_mainWidnow->treatMessage(icon, color, title, msg);
}

void RSLogicActionsManager::slot_exportToExcel()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");
    RSGraphView* graphView = dynamic_cast<RSGraphView*>( m_graphView);

    QMessageBox message;
    message.setWindowTitle(tr("Export Data to Excel Format"));
    message.setIcon(QMessageBox::Question);
    message.setText(tr("<font style=\"color:lime; font-weight: bold;\">Export Data to Excel Format</font>"));
    message.setInformativeText("Are you sure you want to continue ?");
    message.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    message.setCursor(Qt::PointingHandCursor);

    QSpacerItem* m_spacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* m_layout = (QGridLayout*)message.layout();
    m_layout->addItem(m_spacer, m_layout->rowCount(), 0, 1, m_layout->columnCount());

    if( message.exec() == QMessageBox::No)
        return;

    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    m_settings = initSettings();

    //--- -- Be sure the REXFILTER table is ready
//    if(!RSDatabaseAccess::Instance()->checkFilterQueryAndBuildRexFilterTable(QString()))
    QString temp;
    if(!RSDatabaseAccess::Instance()->checkFilterQueryAndBuildRexFilterTable(temp))
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End.\t Failed to update REXFILTER ");
        return ;
    }

    QDate start = m_timeManagerPtr->getStartDateInView().date();
    QDate end = m_timeManagerPtr->getEndDateInView().date();

    RSExportToExcel exportExcel(m_optionsMgrPtr->trend()
                                ,m_optionsMgrPtr->sigma()
                                ,m_optionsMgrPtr->noiseFactor()
                                ,m_timeManagerPtr->stepDays()
                                ,m_timeManagerPtr->stepViewMax());

    QString info;
    if(!m_databaseAccess->deadEntitiesLoaded())
    {
        info.append("   Dead entities excluded");
    }
    if(!m_databaseAccess->nodesWithNoAstLoaded())
    {
        info.append("   Nodes without sensor type code excluded");
    }

    //--- -- Export the Statistics
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export the data table");
        const QStringList& fields = m_graphView->fidelityNoiseFailsDataGrid()->getFields();
        const QList<QVariantList>& data = m_graphView->fidelityNoiseFailsDataGrid()->getData();

        QString sheetName = "data(fidelities noises Failures)";
        QString pageTitle = "Average fidelity, Noise, Failures By sensor";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }

    //--- -- Sensors list by experimentation
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Sensors list by experimentation");
        const QStringList& fields = m_graphView->sensorByExpDataGrid()->getFields();
        const QList<QVariantList>& data = m_graphView->sensorByExpDataGrid()->getData();
        QString sheetName = "Sensors list by experimentation";
        QString pageTitle = "Sensors list by experimentation";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }

    //--- -- Nb sensors by experimentation
    if(graphView)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Number of sensors by experimentation");
        const QStringList& fields = graphView->m_nbSensorByExpHisto.dataGrid()->getFields();
        const QList<QVariantList>& data = graphView->m_nbSensorByExpHisto.dataGrid()->getData();
        QString sheetName = "NB sensors by experimentation";
        QString pageTitle = "Number of sensors by experimentation";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }

    //--- -- List of sensors by Brand
    if(graphView)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export List of sensors by Brand");
        const QStringList& fields = graphView->m_sensorByBrandDataGrid->getFields();
        const QList<QVariantList>& data = graphView->m_sensorByBrandDataGrid->getData();
        QString sheetName = "Sensors list by brand";
        QString pageTitle = "List of sensors by brand";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }

    //--- -- Nb sensors by Brand
    if(graphView)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Number of sensors by brand");
        const QStringList& fields = graphView->m_nbSensorByBrandHisto.dataGrid()->getFields();
        const QList<QVariantList>& data = graphView->m_nbSensorByBrandHisto.dataGrid()->getData();
        QString sheetName = "NB sensors by brand";
        QString pageTitle = "Number of sensors by brand";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }

    //--- -- List of sensors by technology
    if(graphView)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Number of sensors by technology");
        const QStringList& fields = graphView->m_sensorByTechnologyDataGrid->getFields();
        const QList<QVariantList>& data = graphView->m_sensorByTechnologyDataGrid->getData();
        QString sheetName = "Sensors list by technology";
        QString pageTitle = "List of sensors by technology";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }

    //--- -- Nb sensors by technology
    if(graphView)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Number of sensors by technology");
        const QStringList& fields = graphView->m_nbSensorByTechnoHisto.dataGrid()->getFields();
        const QList<QVariantList>& data = graphView->m_nbSensorByTechnoHisto.dataGrid()->getData();
        QString sheetName = "NB sensors by technology";
        QString pageTitle = "Number of sensors by technology";
        exportExcel.addSheet(fields,data,sheetName, pageTitle, start, end,info);
    }

    //--- -- Sensors list by output signal
    if(graphView)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Number of sensors by output signal");
        const QStringList& fields = graphView->m_sensorByOutputSignalDataGrid->getFields();
        const QList<QVariantList>& data = graphView->m_sensorByOutputSignalDataGrid->getData();
        QString sheetName = "Sensors list by output signal";
        QString pageTitle = "Sensors list by  output signal";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }

    //--- -- Nb sensors by output signal
    if(graphView)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Number of sensors by output signal");
        const QStringList& fields = graphView->m_nbSensorByOutputSignalHisto.dataGrid()->getFields();
        const QList<QVariantList>& data = graphView->m_nbSensorByOutputSignalHisto.dataGrid()->getData();
        QString sheetName = "NB sensors by output signal";
        QString pageTitle = "Number of sensors by output signal";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }

    //--- -- Sensors list by Physical meas
    if(graphView)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Number of sensors by physical measurement");
        const QStringList& fields = graphView->m_sensorByPhysicalMeasDataGrid->getFields();
        const QList<QVariantList>& data = graphView->m_sensorByPhysicalMeasDataGrid->getData();
        QString sheetName = "Sensors list by Phys meas";
        QString pageTitle = "List of sensors by physical measurement";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }

    //--- -- Nb sensors by physical measurement
    if(graphView)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Number of sensors by physical measurement");
        const QStringList& fields = graphView->m_nbSensorByPhysicalMeasHisto.dataGrid()->getFields();
        const QList<QVariantList>& data = graphView->m_nbSensorByPhysicalMeasHisto.dataGrid()->getData();
        QString sheetName = "NB sensors by Phys meas";
        QString pageTitle = "Number of sensors by physical measurement";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }

    //Fidelity by technology
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Number of sensors by Brand");
        const QStringList& fields = graphView->m_fidelityByTechnoHisto.getFields();
        const QList<QVariantList>& data = graphView->m_fidelityByTechnoHisto.getData();
        QString sheetName = "Fidelity by technology";
        QString pageTitle = "Fidelity by technology";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }
    //Noise by technology
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Number of sensors by Brand");
        const QStringList& fields = graphView->m_noiseByTechnoHisto.getFields();
        const QList<QVariantList>& data = graphView->m_noiseByTechnoHisto.getData();
        QString sheetName = "Noise by technology";
        QString pageTitle = "Noise by technology";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }
    //Failures by technology
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Number of sensors by Brand");
        const QStringList& fields = graphView->m_failuresByTechnoHisto.getFields();
        const QList<QVariantList>& data = graphView->m_failuresByTechnoHisto.getData();
        QString sheetName = "Failures by technology";
        QString pageTitle = "Failures by technology";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }

    //Fidelity by output signal
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Number of sensors by Brand");
        const QStringList& fields = graphView->m_fidelityByOutputSignalHisto.getFields();
        const QList<QVariantList>& data = graphView->m_fidelityByOutputSignalHisto.getData();
        QString sheetName = "Fidelity by output signal";
        QString pageTitle = "Fidelity by output signal";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }
    //Noise by output signal
    if(graphView)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Number of sensors by Brand");
        const QStringList& fields = graphView->m_noiseByOutputSignalHisto.getFields();
        const QList<QVariantList>& data = graphView->m_noiseByOutputSignalHisto.getData();
        QString sheetName = "Noise by output signal";
        QString pageTitle = "Noise by output signal";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }
    //Failures by output signal
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Number of sensors by Brand");
        const QStringList& fields = graphView->m_failuresByOutputSignalHisto.getFields();
        const QList<QVariantList>& data = graphView->m_failuresByOutputSignalHisto.getData();
        QString sheetName = "Failures by output signal";
        QString pageTitle = "Failures by output signal";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }


    //Fidelity by Physical measurement
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Fidelity by PhysicalMeasurement");
        const QStringList& fields = graphView->m_fidelityByPhysicalMeasHisto.getFields();
        const QList<QVariantList>& data = graphView->m_fidelityByPhysicalMeasHisto.getData();
        QString sheetName = "Fidelity by Phys meas";
        QString pageTitle = "Fidelity by Physical measurement";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }
    //Noise by output signal
    if(graphView)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Noise by PhysicalMeasurement");
        const QStringList& fields = graphView->m_noiseByOutputSignalHisto.getFields();
        const QList<QVariantList>& data = graphView->m_noiseByOutputSignalHisto.getData();
        QString sheetName = "Noise by Phys meas";
        QString pageTitle = "Noise by Physical measurement";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }
    //Failures by Phys meas
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Export Failures by Physical Measurement");
        const QStringList& fields = graphView->m_failuresByPhysicalMeasHisto.getFields();
        const QList<QVariantList>& data = graphView->m_failuresByPhysicalMeasHisto.getData();
        QString sheetName = "Failures by Phys meas";
        QString pageTitle = "Failures by Physical measurement";
        exportExcel.addSheet(fields,data, sheetName, pageTitle, start, end,info);
    }

    exportExcel.saveFile();

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}


RSFidelityAndNoise::FidelitySettings RSLogicActionsManager::initSettings()
{
    //--- --Prepare the settings
    RSLogger::instance()->info(Q_FUNC_INFO,"Prepare the settings");
    RSFidelityAndNoise::FidelitySettings settings;
    if(m_timeManagerPtr)
    {
        settings.startDateUi = m_timeManagerPtr->getStartDateInView().date();
        settings.endDateUi =  m_timeManagerPtr->getEndDateInView().date();
        settings.stepDateUi =   m_timeManagerPtr->getStepDate();
    }

    if(m_graphView)
    {
        settings.stepViewUi  = m_graphView->step();
        settings.sensorName = m_graphView->sensorName();
    }

    if(m_optionsMgrPtr)
    {
        settings.noiseFactorUi = m_optionsMgrPtr->noiseFactor();
        settings.sigmaUi = m_optionsMgrPtr->sigma();
        settings.degreeUi = m_optionsMgrPtr->trend();

        settings.plotMinFidelity = m_optionsMgrPtr->isFidelityMinChecked();
        settings.plotMaxFidelity = m_optionsMgrPtr->isFidelityMaxChecked();
        settings.plotAvgFidelity = m_optionsMgrPtr->isFidelityAvgChecked();
    }

    if(m_graphView)
    {
        QPair<int,MeasPointType> pair =  m_graphView->getSensorCodeTypePair();
        settings.sensorCode = pair.first;
        settings.measPointType = pair.second;
    }

    RSLogger::instance()->info(Q_FUNC_INFO,settings.toString());

    return settings;
}

void RSLogicActionsManager::buildCurves()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    //Get the current sensor Code

    //--- --Prepare the settings
    RSLogger::instance()->info(Q_FUNC_INFO,"Prepare the settings");

    m_settings = initSettings();

    RSLogger::instance()->info(Q_FUNC_INFO,"settings : " + m_settings.toString());

    RSLogger::instance()->info(Q_FUNC_INFO,"init dataComputation");
    RSDataComputation dataComputation (m_databaseAccess,m_settings,m_graphView);

    dataComputation.computeFidelityNoiseStepAndInitGraphView(true /*addGraphs*/,m_settings);


    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSLogicActionsManager::slot_plotCurves()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    buildCurves();

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void  RSLogicActionsManager::slot_stepViewMaxChanged(int stepMax)
{
    m_graphView->setStepViewMax(stepMax);
}


//The statistical table is based on the map :
void RSLogicActionsManager::computeStatisticalTable()
{

}

void RSLogicActionsManager::slot_computeAverage()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    m_settings = initSettings();
    RSLogger::instance()->info(Q_FUNC_INFO,"settings : " + m_settings.toString());
    RSDataComputation dataComputation (m_databaseAccess,m_settings,m_graphView);

    const QPair<int,MeasPointType> codeTypePair = m_graphView->getSensorCodeTypePair();
    QString name  = m_graphView->sensorName();
    QMap<int,QString> sensorsCodeNameMap ;

    sensorsCodeNameMap[codeTypePair.first] = name;

    QVector<RexStatistics> statsArray;
    const QList<SensorInfos>& map = m_graphView->sensorsDetailedInfo();

    if(map.isEmpty() )
        RSLogger::instance()->info(Q_FUNC_INFO,"No sensor found");
    else
        RSLogger::instance()->info(Q_FUNC_INFO,QString("Loop on sensors. Try to find the code %1").arg(codeTypePair.first));

    //Look for the current sensor in the sensor list
    Q_FOREACH(const SensorInfos& sensorInfo,map)
    {
        if(sensorInfo.code == codeTypePair.first)
        {
            RSLogger::instance()->info(Q_FUNC_INFO,QString("Run computations"));
            //Compute on all periods
            dataComputation.runFidelityNoiseAndMtbfCalculations(QList<SensorInfos> () << sensorInfo,m_settings,NULL, statsArray);

            RSLogger::instance()->info(Q_FUNC_INFO,QString("displayAverageValues"));
            m_graphView->displayAverageValues(statsArray.at(0).fidelity,statsArray.at(0).noise);
            QMessageBox::information(0,"Finished.",QString("%1 \n Average fidelity %2\t Average noise : %3 ")
                                     .arg(name)
                                     .arg(statsArray.at(0).fidelity)
                                     .arg(statsArray.at(0).noise));
        }
    }


    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}


void  RSLogicActionsManager::slot_runReportsCalculations()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");
    RSMessageView::Instance()->showData(QString("===== START : Build the table of Fidelity, Noise, Failures======="));

    //Clear the dataGrid
    m_graphView->clearDataGrids();

    //display the data table
    m_graphView->displayDataTab();

    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    m_settings = initSettings();

    //--- -- Be sure the REXFILTER table is ready
    RSMessageView::Instance()->showData(QString("\t - update REXFILTER table in local database"));
    QString temp;
    bool m_checkFilter = RSDatabaseAccess::Instance()->checkFilterQueryAndBuildRexFilterTable(temp);
    if(!m_checkFilter)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End.\t Failed to update REXFILTER ");
        RSMessageView::Instance()->showData(QString("CANCELLED. Failed to update REXFILTER"));
        return ;
    }

    //Build the table data table sensor techno outputSignal mtbf fidelity noise
    //-----------------------------------------------------------------------
    RSMessageView::Instance()->showData(QString("\t - Build the table of Fidelity, Noise, Failures"));
    //Loop on every sensor code
    const QList<SensorInfos> sensorsInfosList = m_graphView->sensorsDetailedInfo();
    {
        if(m_exitLoopMutex.tryLock() && m_exitLoop)
        {
            m_exitLoop = false;
            m_exitLoopMutex.unlock();
            return;
        }

        RSLogger::instance()->info(Q_FUNC_INFO,"Compute the fidelities and noises");
        //Prepare the objects
        QList<QVariantList> dataForGrid;
        RSDataComputation dataComputation(m_databaseAccess, m_settings,m_graphView);
        QVector<RexStatistics> statsArray;
        RSLogger::instance()->info(Q_FUNC_INFO, QString(" Got %1 sensors ").arg(sensorsInfosList.count()));

        m_graphView->fidelityNoiseFailsDataGrid()->setDataModelMatrix(RexStatistics::fieldsList(),dataForGrid);
        //Compute the fidelity, noise, for this sensor
        dataComputation.runFidelityNoiseAndMtbfCalculations(sensorsInfosList,m_settings,m_graphView, statsArray);

        //Store the results in the table
        RSLogger::instance()->info(Q_FUNC_INFO,"Store the results in the table");
        Q_FOREACH(const RexStatistics& rowData,statsArray)
        {
            dataForGrid.push_back(rowData.toQVariantList());
            RSLogger::instance()->info(Q_FUNC_INFO,RexStatistics::fieldsList().join(",") +"\n" + rowData.toString());
        }
        RSLogger::instance()->info(Q_FUNC_INFO,"Store the results in the table");
        m_graphView->fidelityNoiseFailsDataGrid()->setDataModelMatrix(RexStatistics::fieldsList(),dataForGrid);
    }

    //--- -- sensors by experimentation Data
    //-----------------------------------------------------------------------
    m_graphView->initSensorsListDatagrid();
    updateSensorsListByExperimentation();
    m_graphView->updateSensorsDatagrid(0);

    //Draw the charts when all dataGrid are ready. (Charts are built with dataGrid data)
    m_graphView->plotCharts();

    QMessageBox::information(0,"Finished.","Data and histograms are updated..");

    RSLogger::instance()->info(Q_FUNC_INFO,"End");

}


void RSLogicActionsManager::updateSensorsListByExperimentation()
{

    RSMessageView::Instance()->showData(tr("Build sensors list datagrid..Start "));

    RSLogger::instance()->info(Q_FUNC_INFO,"Init Sensors by experimentation Data");
    const QList<SensorInfos> sensorsInfosList = m_graphView->sensorsDetailedInfo();

    QStringList experiencesList = m_filterWidget->getExperimentationsList();
    QMap<QString, QStringList> sensorsByExperimenationMap;
    const QMap<int,QStringList>* experienceBySensorMap = RSDatabaseAccess::Instance()->experienceBySensorMap() ;

    Q_FOREACH(const SensorInfos& sensorInfo, sensorsInfosList)
    {
        if(experienceBySensorMap->contains(sensorInfo.mpCode) )
        {
            //RSMessageView::Instance()->showData ( " sensorInfo = " + sensorInfo.name);
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

            Q_FOREACH(const QString& tagName, experienceBySensorMap->value(sensorInfo.mpCode))
            {
                //Select the experience in the list
                if(experiencesList.contains(tagName))
                {
                    sensorsByExperimenationMap[tagName].append(sensorInfo.name);
                }
            }
        }
    }


    int rowCount = 0;//Get the greatest size of the table
    Q_FOREACH(const QString& key, sensorsByExperimenationMap.keys())
        rowCount = qMax(rowCount,sensorsByExperimenationMap.value(key).count());

    //--- --Empty Table first
    {
        RSLogger::instance()->info(Q_FUNC_INFO, "Fill the data. rowCount=" + QString::number(rowCount));
        QList<QVariantList> dataForGrid;

        RSLogger::instance()->info(Q_FUNC_INFO, "Create an empty dataGrid");
        for(int row = 0; row < rowCount; row++)
        {
            QVariantList list;
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            for(int col = 0; col < sensorsByExperimenationMap.keys().count(); col++)
                list << QVariant();
            dataForGrid.push_back(list);
        }

        RSLogger::instance()->info(Q_FUNC_INFO, "Fill the dataGrid");
        for(int col = 0; col < sensorsByExperimenationMap.keys().count(); col++)
        {
            const QString key = sensorsByExperimenationMap.keys().at(col);
            const QStringList sensorsList = sensorsByExperimenationMap[key];
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            for(int row = 0; row < sensorsList.count(); row++)
            {
                QVariantList& rowData = const_cast<QVariantList&> (dataForGrid.at(row));
                const QString& sensorName = sensorsList.at(row);
                rowData.replace(col,sensorName);
            }
        }

        m_graphView->sensorByExpDataGrid()->setDataModelMatrix(sensorsByExperimenationMap.keys(),dataForGrid);
    }
    RSMessageView::Instance()->showData(tr("Build sensors list datagrid..End "));

}

void RSLogicActionsManager::slot_updateSensorsList()
{
    updateSensorsListByExperimentation();

    m_graphView->initSensorsListDatagrid();

    m_graphView->updateSensorsDatagrid(0);

    QMessageBox::information(0,"Finished","Sensor lists data  grid are updated!");
}
