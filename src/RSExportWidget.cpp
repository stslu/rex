#include "RSExportWidget.h"
#include "ui_RSExportManager.h"
#include "RSExportToExcel.h"
#include "RSDataComputation.h"
#include "RSGraphView.h"
#include "RSMessageView.h"
#include "RSDatabaseAccess.h"
#include "RSDataManager.h"
#include "RSFiltersManager.h"
#include "IRSGraphView.h"
#include "Signaler.h"

#include <QMessageBox>

RSExportManager::RSExportManager(QWidget *parent) : QWidget(parent), ui(new Ui::RSExportManager), IRSExportManager()
{
    ui->setupUi(this);

    createObjects();

    createConnections();
}

RSExportManager::~RSExportManager()
{
     RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
    delete ui;
    delete m_movie;
}

void RSExportManager::createObjects()
{
    m_movie = new QMovie("excelExport.gif");


}

void RSExportManager::createConnections()
{
    connect(ui->m_updateDataBtn, SIGNAL(clicked()), this, SLOT(slot_updateDataButtonClicked()));
    connect(ui->m_excelButton, SIGNAL(clicked()), this, SLOT(slotExcelButtonClicked()));
    connect(ui->m_updateSensorsListBtn, SIGNAL(clicked()), this, SLOT(slot_updateSensorsListButtonClicked()));
}

void RSExportManager::slot_updateDataButtonClicked()
{   
    emit Signaler::instance()->signal_runReportsCalculations();
}

void RSExportManager::slotTestButtonClicked()
{
    /*RSGraphView::Instance()->setVisible(!RSGraphView::Instance()->isVisible());
    RSMessageView::Instance()->setVisible(!RSMessageView::Instance()->isVisible());*/
}

void RSExportManager::slotExcelButtonClicked()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"emit signal_exportToExcel");
    emit Signaler::instance()->signal_exportToExcel();


}

void RSExportManager::slot_updateSensorsListButtonClicked()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"emit signal_updateSensorsList");

    emit Signaler::instance()->signal_updateSensorsList();

}

void RSExportManager::setProgressBarRate(double rate)
{

}
