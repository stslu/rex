#include "RSTimesManager.h"
#include "ui_RSTimesManager.h"
#include "RSDataManager.h"
#include "RSDatabaseAccess.h"
#include "RSSettingsManager.h"
#include "RSGlobalMethods.h"
#include "RSGraphView.h"
#include "RSMessageView.h"

#include <QDate>
#include <QCalendarWidget>
#include <RSLogger.h>

namespace RexTimeDefaultSettings
{
//[RSTimesManager]
const QDate     DEFAULT_START_DATE = QDate::currentDate().addYears(-5);
const QDate     DEFAULT_START_DATE_MIN = QDate::fromString("01-01-2000", "dd-MM-yyyy");
const QDate     DEFAULT_END_DATE = QDate::currentDate();
const QDate     DEFAULT_END_DATE_MAX = QDate::currentDate();
const int       DEFAULT_STEP_DATE_DAY = 7;
const int       DEFAULT_STEP_DATE_DAY_MIN = 1;
const int       DEFAULT_STEP_DATE_DAY_MAX = 365;
const int       DEFAULT_STEP_VIEW_IDX = 0;
const int       DEFAULT_STEP_VIEW_IDX_MIN = 0;
const int       DEFAULT_STEP_VIEW_IDX_MAX = 1000;
}


RSTimesManager::RSTimesManager(RSDatabaseAccess* dbAccess,QWidget *parent) : QWidget(parent)
  , ui(new Ui::RSTimesManager)
  , IRSTimesManager()
  , m_databaseAccess(dbAccess)
  , m_id ("RSTimesManager")
{

    ui->setupUi(this);

    createObjects();

    createConnections();
}

RSTimesManager::~RSTimesManager()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
    saveSettings(QString());

    delete ui;
}

void RSTimesManager::createObjects()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    loadSettings(QString());

    updateStepViewMax(/*emitSignal?*/false);

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSTimesManager::createConnections()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    connect(ui->m_startDateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(slotStartDateValueChanged(QDate)));

    connect(ui->m_endDateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(slotEndDateValueChanged(QDate)));

    connect(ui->m_stepDateEdit, SIGNAL(valueChanged(int)), this, SLOT(slotStepDateValueChanged(int)));

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSTimesManager::updateStepViewMax(bool emitSignal )
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    /*int m_sensorNameCode = RSDataManager::Instance()->getData("SensorNameCode").toInt();

    QDate startRelativeDate = m_databaseAccess->getAcquisitionRelativeFirstTime(ui->m_startDateEdit->date(),
                                                                                ui->m_endDateEdit->date(), m_sensorNameCode).date();
    QDate endRelativeDate = m_databaseAccess->getAcquisitionRelativeLastTime(ui->m_startDateEdit->date(),
                                                                             ui->m_endDateEdit->date(), m_sensorNameCode).date();*/
    //QDate stepDate = ui->m_stepDateEdit->value();

    //m_stepViewMax = RSGlobalMethods::Instance()->getStepNumber(startRelativeDate, endRelativeDate, ui->m_stepDateEdit->value());
    m_stepViewMax = RSGlobalMethods::Instance()->getStepNumber(ui->m_startDateEdit->date(), ui->m_endDateEdit->date(), ui->m_stepDateEdit->value());

    RSLogger::instance()->info(Q_FUNC_INFO, "New stepViewMax : " + QString::number(m_stepViewMax));

    ui->m_stepViewEdit->setValue(m_stepViewMax);

    if(emitSignal)
        emit Signaler::instance()->signal_stepViewMaxChanged(m_stepViewMax);

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

int RSTimesManager::stepViewMax() const
{
    return ui->m_stepViewEdit->value();
}

void RSTimesManager::slotStartDateValueChanged(const QDate& data)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    ui->m_endDateEdit->setMinimumDate(data);
    RSDataManager::Instance()->setData("StartDate", data);

    updateStepViewMax();

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSTimesManager::slotEndDateValueChanged(const QDate& data)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    ui->m_startDateEdit->setMaximumDate(data);
    RSDataManager::Instance()->setData("EndDate", data);

    updateStepViewMax();

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSTimesManager::slotStepDateValueChanged(int data)
{    
    RSDataManager::Instance()->setData("New step : ", data);

    updateStepViewMax();//Will emit the signal

}

void RSTimesManager::setStepViewValue(int data)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "set value to " + QString::number(data));
    ui->m_stepViewEdit->setValue(data);
}

QDateTime RSTimesManager::getStartDateInView() const
{
    return ui->m_startDateEdit->dateTime();
}

QDateTime RSTimesManager::getEndDateInView() const
{
    return ui->m_endDateEdit->dateTime();
}

qint64 RSTimesManager::getStepDate() const
{
    return ui->m_stepDateEdit->value();
}

void RSTimesManager::loadSettings(const QString& fileName)
{
    Q_UNUSED(fileName)
    //[RSTimesManager]
    /*RSDataManager::Instance()->setData("RSTimesManager.StartDate", loadStartDate());
    RSDataManager::Instance()->setData("RSTimesManager.EndDate", loadEndDate());
    RSDataManager::Instance()->setData("RSTimesManager.StepDate", loadStepDate());
    RSDataManager::Instance()->setData("RSTimesManager.StepView", loadStepView());
    RSDataManager::Instance()->setData("RSTimesManager.StepViewMax", loadStepViewMax());*/

    loadStartDate();

    loadEndDate();

    loadStepDate();
}

void RSTimesManager::saveSettings(const QString& fileName) const
{
    Q_UNUSED(fileName)

    saveStartDate();

    saveEndDate();

    saveStepDate();
}


void  RSTimesManager::setDateEditValue(QDateEdit* dateEdit, QDate& dateValue)
{
    dateEdit->blockSignals(true);
    dateEdit->setDate(dateValue);
    dateEdit->blockSignals(false);
}

QVariant RSTimesManager::loadStartDate()
{
    //--- --Upper bound and lower bound
    {
        ui->m_startDateEdit->setMinimumDate(RexTimeDefaultSettings::DEFAULT_START_DATE_MIN);
        QVariant m_default = RexTimeDefaultSettings::DEFAULT_END_DATE;
        QVariant data = RSGlobalMethods::Instance()->loadData(m_id, "RSTimesManager.EndDate", m_default);
        ui->m_startDateEdit->setMaximumDate(data.value<QDate>());
    }

    {
        QString m_key = "RSTimesManager.StartDate";
        QVariant m_default = RexTimeDefaultSettings::DEFAULT_START_DATE;
        QVariant data =  RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
        setDateEditValue(ui->m_startDateEdit, data.value<QDate>());
        RSLogger::instance()->info(Q_FUNC_INFO, "StartDate:  " +  data.value<QDate>().toString());
        return data;
    }
}

QVariant RSTimesManager::loadEndDate()
{
    {
        ui->m_endDateEdit->setMaximumDate(RexTimeDefaultSettings::DEFAULT_END_DATE_MAX);
        QVariant m_default = RexTimeDefaultSettings::DEFAULT_START_DATE;
        QVariant data = RSGlobalMethods::Instance()->loadData(m_id, "RSTimesManager.StartDate", m_default);
        ui->m_endDateEdit->setMinimumDate(data.value<QDate>());
    }

    {
        QString m_key = "RSTimesManager.EndDate";
        QVariant m_default = RexTimeDefaultSettings::DEFAULT_END_DATE;
        QVariant data =  RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
        setDateEditValue(ui->m_endDateEdit, data.value<QDate>());
        RSLogger::instance()->info(Q_FUNC_INFO, "EndDate:  " +  data.value<QDate>().toString());
        return data;
    }
}

void RSTimesManager::saveStartDate()const
{
    QString m_id = "RSTimesManager";
    QString m_key = "RSTimesManager.StartDate";
    QVariant data = ui->m_startDateEdit->date();

    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
    RSLogger::instance()->info(Q_FUNC_INFO, "StartDate:  " +  data.value<QDate>().toString());
}

void RSTimesManager::saveEndDate()const
{
    QString m_id = "RSTimesManager";
    QString m_key = "RSTimesManager.EndDate";
    QVariant data =  ui->m_endDateEdit->date();

    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
    RSLogger::instance()->info(Q_FUNC_INFO, "StartDate:  " +  data.value<QDate>().toString());
}

QVariant RSTimesManager::loadStepDate()
{
    ui->m_stepDateEdit->setRange(RexTimeDefaultSettings::DEFAULT_STEP_DATE_DAY_MIN, RexTimeDefaultSettings::DEFAULT_STEP_DATE_DAY_MAX);
    ui->m_stepDateEdit->setSuffix(" Day(s)");

    QString m_id = "RSTimesManager";
    QString m_key = "RSTimesManager.StepDate";
    QVariant m_default = RexTimeDefaultSettings::DEFAULT_STEP_DATE_DAY;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
    ui->m_stepDateEdit->blockSignals(true);
    ui->m_stepDateEdit->setValue(data.value<int>());
    ui->m_stepDateEdit->blockSignals(false);

    RSLogger::instance()->info(Q_FUNC_INFO, "StepDate:  " + QString::number(data.value<int>()));

    return data;
}

void RSTimesManager::saveStepDate()const
{
    QString m_id = "RSTimesManager";
    QString m_key = "RSTimesManager.data";
    QVariant data =  ui->m_stepDateEdit->value();

    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
    RSLogger::instance()->info(Q_FUNC_INFO, "StepDate:  " + QString::number(data.value<int>()));
}

int RSTimesManager::stepDays() const
{
    return ui->m_stepDateEdit->value();
}
