#ifndef RSDEFAULTSETTINGS_H
#define RSDEFAULTSETTINGS_H

#include <QObject>
#include <QDate>

namespace REX {

//[RexSensors]
const QString  strPathBase = "../db/";
const QString   DEFAULT_REX_VERSION = "0.0.1";
const QString   DEFAULT_REX_NAME = "RexSensor";
const QString   DEFAULT_REX_CONFIG_FILE = "RexSensors.ini";
const QString   DEFAULT_REX_DB_FILE_G6 = strPathBase + "G6_GWDB.FDB";
const QString   DEFAULT_REX_DB_FILE_G7 = strPathBase + "G7MAINDB.FDB";
const bool      DEFAULT_SETTINGS_FLAG = false;

//[RSTimesManager]
const QDate     DEFAULT_START_DATE = QDate::currentDate().addYears(-5);
const QDate     DEFAULT_START_DATE_MIN = QDate::fromString("01-01-2000", "dd-MM-yyyy");
const QDate     DEFAULT_END_DATE = QDate::currentDate();
const QDate     DEFAULT_END_DATE_MAX = QDate::currentDate();
const int       DEFAULT_STEP_DATE_DAY = 7;
const int       DEFAULT_STEP_DATE_DAY_MIN = 1;
const int       DEFAULT_STEP_DATE_DAY_MAX = 365;
const int       DEFAULT_STEP_VIEW_DAY = 0;
const int       DEFAULT_STEP_VIEW_DAY_MIN = 0;
const int       DEFAULT_STEP_VIEW_DAY_MAX = 10;

//[RSOptionsManager]
const int       DEFAULT_TREND_DEGREE = 1;
const int       DEFAULT_TREND_DEGREE_MIN = 0;
const int       DEFAULT_TREND_DEGREE_MAX = 10;
const int       DEFAULT_SIGMA_PERCENTAGE = 80;
const int       DEFAULT_SIGMA_PERCENTAGE_MIN = 10;
const int       DEFAULT_SIGMA_PERCENTAGE_MAX = 90;
const double    DEFAULT_NOISE_FACTOR = 2.25;
const double    DEFAULT_NOISE_FACTOR_MIN = 0.25;
const double    DEFAULT_NOISE_FACTOR_MAX = 10.25;
const double    DEFAULT_NOISE_FACTOR_STEP = 0.25;

} // REX_NAMESPACE

class RSDefaultSettings : public QObject
{
    Q_OBJECT

private:
    RSDefaultSettings(QObject *parent = 0);

public:
    ~RSDefaultSettings();
    static RSDefaultSettings* Instance(QObject *parent = 0);

private:
    void createObjects();
    void createConnections();

public:
    //-------------------------------------------------------------------
    // load settings
    //-------------------------------------------------------------------

    //[RexSensors]
    QVariant loadDefaultSettingsFlag();

    //[RSTimesManagerDefault]
    QVariant loadStartDate();
    QVariant loadEndDate();
    QVariant loadStepDate();
    QVariant loadStepView();
    QVariant loadStepViewMax();

    //[RSOptionsManagerDefault]
    QVariant loadTrend();
    QVariant loadSigma();
    QVariant loadNoise();

    //-------------------------------------------------------------------
    // save settings
    //-------------------------------------------------------------------

    void save();

    //[RexSensors]
    void saveDefaultSettingsFlag();

    //[RSTimesManagerDefault]
    void saveStartDate();
    void saveEndDate();
    void saveStepDate();
    void saveStepView();
    void saveStepViewMax();

    //[RSOptionsManagerDefault]
    void saveTrend();
    void saveSigma();
    void saveNoise();

private:
    static RSDefaultSettings* m_instance;
};


#endif // RSDEFAULTSETTINGS_H
