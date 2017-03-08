#include "RSOptionsManager.h"
#include "ui_RSOptionsManager.h"
#include "RSDataManager.h"
#include "RSOptionEntityCheckBox.h"
#include "RSOptionEntityGroupBox.h"
#include "RSMessageView.h"
#include "RSSettingsManager.h"
#include "RSGlobalMethods.h"
#include "RSLogger.h"

namespace RexOptionsDefaultSettings
{
const bool      DEFAULT_FIDELITY = true;
const bool      DEFAULT_FIDELITY_MIN = true;
const bool      DEFAULT_FIDELITY_MAX = true;
const bool      DEFAULT_FIDELITY_AVG = true;

const bool      DEFAULT_NOISE = false;
const bool      DEFAULT_NOISE_MIN = false;
const bool      DEFAULT_NOISE_MAX = false;
const bool      DEFAULT_NOISE_AVG = false;

const bool      DEFAULT_FAILURE = false;
const bool      DEFAULT_FAILURE_MIN = false;
const bool      DEFAULT_FAILURE_MAX = false;
const bool      DEFAULT_FAILURE_AVG = false;

const int       DEFAULT_TREND_DEGREE = 1;
const int       DEFAULT_TREND_DEGREE_MIN = 0;
const int       DEFAULT_TREND_DEGREE_MAX = 20;
const int       DEFAULT_SIGMA_PERCENTAGE = 80;
const int       DEFAULT_SIGMA_PERCENTAGE_MIN = 5;
const int       DEFAULT_SIGMA_PERCENTAGE_MAX = 95;
const double    DEFAULT_NOISE_FACTOR = 2.25;
const double    DEFAULT_NOISE_FACTOR_MIN = 0.25;
const double    DEFAULT_NOISE_FACTOR_MAX = 10.25;
const double    DEFAULT_NOISE_FACTOR_STEP = 0.05;
const double    DEFAULT_NOISE_FACTOR_DEC = 2;
} // REX_NAMESPACE


RSOptionsManager::RSOptionsManager(QWidget *parent) : QWidget(parent), ui(new Ui::RSOptionsManager),IRSOptionsManager()
{
    ui->setupUi(this);

    createObjects();
    createConnections();
}

RSOptionsManager::~RSOptionsManager()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
    saveSettings(QString());

    delete ui;
}


void RSOptionsManager::createObjects()
{

    ui->m_trendEdit->setRange(RexOptionsDefaultSettings::DEFAULT_TREND_DEGREE_MIN, RexOptionsDefaultSettings::DEFAULT_TREND_DEGREE_MAX);
    ui->m_sigmaEdit->setRange(RexOptionsDefaultSettings::DEFAULT_SIGMA_PERCENTAGE_MIN, RexOptionsDefaultSettings::DEFAULT_SIGMA_PERCENTAGE_MAX);

    ui->m_noiseFactorEdit->setRange(RexOptionsDefaultSettings::DEFAULT_NOISE_FACTOR_MIN, RexOptionsDefaultSettings::DEFAULT_NOISE_FACTOR_MAX);
    ui->m_noiseFactorEdit->setDecimals(RexOptionsDefaultSettings::DEFAULT_NOISE_FACTOR_DEC);
    ui->m_noiseFactorEdit->setSingleStep(RexOptionsDefaultSettings::DEFAULT_NOISE_FACTOR_STEP);

    loadSettings(QString());
}

void RSOptionsManager::createConnections()
{
    //Fidelity minMax
    connect(ui->m_fidelityCheckEdit, SIGNAL(clicked(bool)), this,
            SLOT(slotFidelityClicked(bool)));
    connect(ui->m_fidelityMinCheckEdit, SIGNAL(clicked(bool)), this,
            SLOT(slotFidelityMinClicked(bool)));
    connect(ui->m_fidelityMaxCheckEdit, SIGNAL(clicked(bool)), this,
            SLOT(slotFidelityMaxClicked(bool)));
    connect(ui->m_fidelityAvgCheckEdit, SIGNAL(clicked(bool)), this,
            SLOT(slotFidelityAvgClicked(bool)));


    //Trend
    connect(ui->m_trendEdit, SIGNAL(valueChanged(int)), this,
            SLOT(slotTrendValueChanged(int)));
    connect(ui->m_sigmaEdit, SIGNAL(valueChanged(int)), this,
            SLOT(slotSigmaValueChanged(int)));
    connect(ui->m_noiseFactorEdit, SIGNAL(valueChanged(double)), this,
            SLOT(slotNoiseValueChanged(double)));

    connect(ui->m_calculationsButton, SIGNAL(clicked()), Signaler::instance(), SIGNAL(signal_runReportsCalculations()));
    connect(ui->m_excelButton, SIGNAL(clicked()), Signaler::instance(), SIGNAL(signal_exportToExcel()));
    connect(ui->m_updateSensorsListBtn, SIGNAL(clicked()), Signaler::instance(), SIGNAL(signal_updateSensorsList()));
}

void RSOptionsManager::updateFidelityChecked()
{
    bool m_checked = true;
    m_checked &= ui->m_fidelityMinCheckEdit->isChecked();
    m_checked &= ui->m_fidelityMaxCheckEdit->isChecked();
    m_checked &= ui->m_fidelityAvgCheckEdit->isChecked();

    ui->m_fidelityCheckEdit->setChecked(m_checked);
    RSDataManager::Instance()->setData("Fidelity", m_checked);
}



void RSOptionsManager::slotFidelityClicked(bool data)
{
    //ui->m_body2->setVisible(data);
    ui->m_fidelityMinCheckEdit->setChecked(data);
    ui->m_fidelityMaxCheckEdit->setChecked(data);
    ui->m_fidelityAvgCheckEdit->setChecked(data);

}

void RSOptionsManager::slotFidelityMinClicked(bool data)
{
    // RSDataManager::Instance()->setData("FidelityMin", data);
    updateFidelityChecked();
}

void RSOptionsManager::slotFidelityMaxClicked(bool data)
{
    //RSDataManager::Instance()->setData("FidelityMax", data);
    updateFidelityChecked();
}

void RSOptionsManager::slotFidelityAvgClicked(bool data)
{
    //RSDataManager::Instance()->setData("FidelityAvg", data);
    updateFidelityChecked();
}

void RSOptionsManager::slotTrendValueChanged(int data)
{
    //RSDataManager::Instance()->setData("Trend", data);
}

void RSOptionsManager::slotSigmaValueChanged(int data)
{
    //RSDataManager::Instance()->setData("Sigma", data);
}

void RSOptionsManager::slotNoiseValueChanged(double data)
{
    //RSDataManager::Instance()->setData("NoiseFactor", data);
}


double  RSOptionsManager::noiseFactor() const
{
    return ui->m_noiseFactorEdit->value();
}

double  RSOptionsManager::sigma() const
{
    return ui->m_sigmaEdit->value();
}



int RSOptionsManager::trend() const
{
    return ui->m_trendEdit->value();
}

bool RSOptionsManager::isFidelityChecked() const
{
    return ui->m_fidelityCheckEdit->isChecked();
}


bool RSOptionsManager::isFidelityMinChecked() const
{
    return ui->m_fidelityMinCheckEdit->isChecked();
}


bool RSOptionsManager::isFidelityMaxChecked() const
{
    return ui->m_fidelityMaxCheckEdit->isChecked();
}


bool RSOptionsManager::isFidelityAvgChecked() const
{
    return ui->m_fidelityAvgCheckEdit->isChecked();
}

void RSOptionsManager::loadSettings(const QString& fileName)
{
    //[RSOptionsManager]
    loadFidelity();
    loadFidelityMin();
    loadFidelityMax();
    loadFidelityAvg();




    loadTrend();
    loadSigma();
    loadNoiseFactor();
}

void RSOptionsManager::saveSettings(const QString& fileName) const
{
    saveFidelity();

    saveFidelityMin();
    saveFidelityMax();
    saveFidelityAvg();

    saveTrend();
    saveSigma();
    saveNoiseFactor();
}


void RSOptionsManager::setCheckBoxValue(QCheckBox* checkBox, bool state)
{
    checkBox->blockSignals(true);
    checkBox->setChecked(state);
    checkBox->blockSignals(false);
}

QVariant RSOptionsManager::loadFidelity()
{
    QString m_id = "RSOptionsManager";
    QString m_key = "RSOptionsManager.Fidelity";
    QVariant m_default = RexOptionsDefaultSettings::DEFAULT_FIDELITY;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
    setCheckBoxValue(ui->m_fidelityCheckEdit,data.value<bool>());
    RSLogger::instance()->info(Q_FUNC_INFO,QString("Saved value : %1").arg(data.value<bool>()));
    return data;
}

QVariant RSOptionsManager::loadFidelityMin()
{
    QString m_id = "RSOptionsManager";
    QString m_key = "RSOptionsManager.FidelityMin";
    QVariant m_default = RexOptionsDefaultSettings::DEFAULT_FIDELITY_MIN;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
    setCheckBoxValue(ui->m_fidelityMinCheckEdit,data.value<bool>());
    RSLogger::instance()->info(Q_FUNC_INFO,QString("Saved value : %1").arg(data.value<bool>()));
    return data;
}

QVariant RSOptionsManager::loadFidelityMax()
{
    QString m_id = "RSOptionsManager";
    QString m_key = "RSOptionsManager.FidelityMax";
    QVariant m_default = RexOptionsDefaultSettings::DEFAULT_FIDELITY_MAX;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
    setCheckBoxValue(ui->m_fidelityMaxCheckEdit,data.value<bool>());
    RSLogger::instance()->info(Q_FUNC_INFO,QString("Saved value : %1").arg(data.value<bool>()));
    return data;
}

QVariant RSOptionsManager::loadFidelityAvg()
{
    QString m_id = "RSOptionsManager";
    QString m_key = "RSOptionsManager.FidelityAvg";
    QVariant m_default = RexOptionsDefaultSettings::DEFAULT_FIDELITY_AVG;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
    setCheckBoxValue(ui->m_fidelityAvgCheckEdit,data.value<bool>());
    RSLogger::instance()->info(Q_FUNC_INFO,QString("Saved value : %1").arg(data.value<bool>()));
    return data;
}

void RSOptionsManager::setSpinBoxVaue(QSpinBox* spinBox, int value)
{

    spinBox->blockSignals(true);
    spinBox->setValue(value);
    spinBox->blockSignals(false);
}

void RSOptionsManager::setSpinBoxVaue(QDoubleSpinBox* spinBox, double value)
{

    spinBox->blockSignals(true);
    spinBox->setValue(value);
    spinBox->blockSignals(false);
}

QVariant RSOptionsManager::loadTrend()
{
    QString m_id = "RSOptionsManager";
    QString m_key = "RSOptionsManager.Trend";
    QVariant m_default = RexOptionsDefaultSettings::DEFAULT_TREND_DEGREE;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
    setSpinBoxVaue(ui->m_trendEdit,data.value<int>());
    RSLogger::instance()->info(Q_FUNC_INFO,"Loaded : " + QString::number(data.value<int>()));
    return data;
}


QVariant RSOptionsManager::loadSigma()
{
    QString m_id = "RSOptionsManager";
    QString m_key = "RSOptionsManager.Sigma";
    QVariant m_default = RexOptionsDefaultSettings::DEFAULT_SIGMA_PERCENTAGE;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
    setSpinBoxVaue(ui->m_sigmaEdit,data.value<int>());
    RSLogger::instance()->info(Q_FUNC_INFO,"Loaded : " + QString::number(data.value<int>()));
    return data;
}

QVariant RSOptionsManager::loadNoiseFactor()
{
    QString m_id = "RSOptionsManager";
    QString m_key = "RSOptionsManager.NoiseFactor";
    QVariant m_default = RexOptionsDefaultSettings::DEFAULT_NOISE_FACTOR;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
    setSpinBoxVaue(ui->m_noiseFactorEdit,data.value<double>());
    RSLogger::instance()->info(Q_FUNC_INFO,"Loaded : " + QString::number(data.value<double>()));
    return data;
}

void RSOptionsManager::saveFidelity() const
{
    QString m_id = "RSOptionsManager";
    QString m_key = "RSOptionsManager.Fidelity";

    QVariant data = ui->m_fidelityCheckEdit->isChecked();
    RSLogger::instance()->info(Q_FUNC_INFO,QString("Saved value : %1").arg(data.value<bool>()));
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

void RSOptionsManager::saveFidelityMin() const
{
    QString m_id = "RSOptionsManager";
    QString m_key = "RSOptionsManager.FidelityMin";

    QVariant data = ui->m_fidelityMinCheckEdit->isChecked();
    RSLogger::instance()->info(Q_FUNC_INFO,QString("Saved value : %1").arg(data.value<bool>()));
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

void RSOptionsManager::saveFidelityMax() const
{
    QString m_id = "RSOptionsManager";
    QString m_key = "RSOptionsManager.FidelityMax";

    QVariant data = ui->m_fidelityMaxCheckEdit->isChecked();
    RSLogger::instance()->info(Q_FUNC_INFO,QString("Saved value : %1").arg(data.value<bool>()));
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

void RSOptionsManager::saveFidelityAvg() const
{
    QString m_id = "RSOptionsManager";
    QString m_key = "RSOptionsManager.FidelityAvg";

    QVariant data = ui->m_fidelityAvgCheckEdit->isChecked();
    RSLogger::instance()->info(Q_FUNC_INFO,QString("Saved value : %1").arg(data.value<bool>()));
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}


void RSOptionsManager::saveTrend() const
{
    QString m_id = "RSOptionsManager";
    QString m_key = "RSOptionsManager.Trend";

    QVariant data = ui->m_trendEdit->value();
    RSLogger::instance()->info(Q_FUNC_INFO,"Saved : " + QString::number(data.value<int>()));
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

void RSOptionsManager::saveSigma() const
{
    QString m_id = "RSOptionsManager";
    QString m_key = "RSOptionsManager.Sigma";

    QVariant data = ui->m_sigmaEdit->value();
    RSLogger::instance()->info(Q_FUNC_INFO,QString("Saved value : %1").arg(data.value<int>()));
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);;
}

void RSOptionsManager::saveNoiseFactor()const
{
    QString m_id = "RSOptionsManager";
    QString m_key = "RSOptionsManager.NoiseFactor";

    QVariant data = ui->m_noiseFactorEdit->value();
    RSLogger::instance()->info(Q_FUNC_INFO,"Saved : " + QString::number(data.value<double>()));
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}
