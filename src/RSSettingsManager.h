//#ifndef RSSettingsManager_H
//#define RSSettingsManager_H

//#include <QObject>
//#include <QVariant>
//#include <QDate>

//namespace REX {

////[RexSensors]
//const QString  strPathBase = "../data";
//const QString   DEFAULT_REX_VERSION = "0.0.1";
//const QString   DEFAULT_REX_NAME = "RexSensor";
//const QString   DEFAULT_REX_CONFIG_FILE = "./RexSensors.ini";
//const QString   DEFAULT_REX_APP_DB_FILE =  "./RexSensors.dat";
//const QString   DEFAULT_REX_G6_DB_FILE = strPathBase +"./GWDBS.fdb";
//const QString   DEFAULT_REX_G7_DB_FILE = strPathBase +"./G7MainDB.fdb";

////[RSFiltersManager]
//const QString   DEFAULT_FILTER_FIELD_DATA = "All";
//const int       DEFAULT_FILTER_FIELD_INDEX = 0;
//const QString   DEFAULT_FILTER_QUERY = "";
//const int       DEFAULT_SENSOR_NAME_INDEX = 10;

////[RSTimesManager]
//const QDate     DEFAULT_START_DATE = QDate::currentDate().addYears(-5);
//const QDate     DEFAULT_START_DATE_MIN = QDate::fromString("01-01-2000", "dd-MM-yyyy");
//const QDate     DEFAULT_END_DATE = QDate::currentDate();
//const QDate     DEFAULT_END_DATE_MAX = QDate::currentDate();
//const int       DEFAULT_STEP_DATE_DAY = 7;
//const int       DEFAULT_STEP_DATE_DAY_MIN = 1;
//const int       DEFAULT_STEP_DATE_DAY_MAX = 365;
//const int       DEFAULT_STEP_VIEW_IDX = 0;
//const int       DEFAULT_STEP_VIEW_IDX_MIN = 0;
//const int       DEFAULT_STEP_VIEW_IDX_MAX = 1000;

////[RSOptionsManager]
//const bool      DEFAULT_FIDELITY = true;
//const bool      DEFAULT_FIDELITY_MIN = true;
//const bool      DEFAULT_FIDELITY_MAX = true;
//const bool      DEFAULT_FIDELITY_AVG = true;

//const bool      DEFAULT_NOISE = true;
//const bool      DEFAULT_NOISE_MIN = true;
//const bool      DEFAULT_NOISE_MAX = true;
//const bool      DEFAULT_NOISE_AVG = true;

//const bool      DEFAULT_FAILURE = true;
//const bool      DEFAULT_FAILURE_MIN = true;
//const bool      DEFAULT_FAILURE_MAX = true;
//const bool      DEFAULT_FAILURE_AVG = true;

//const int       DEFAULT_TREND_DEGREE = 1;
//const int       DEFAULT_TREND_DEGREE_MIN = 0;
//const int       DEFAULT_TREND_DEGREE_MAX = 20;
//const int       DEFAULT_SIGMA_PERCENTAGE = 80;
//const int       DEFAULT_SIGMA_PERCENTAGE_MIN = 5;
//const int       DEFAULT_SIGMA_PERCENTAGE_MAX = 95;
//const double    DEFAULT_NOISE_FACTOR = 2.25;
//const double    DEFAULT_NOISE_FACTOR_MIN = 0.25;
//const double    DEFAULT_NOISE_FACTOR_MAX = 10.25;
//const double    DEFAULT_NOISE_FACTOR_STEP = 0.05;
//const double    DEFAULT_NOISE_FACTOR_DEC = 2;

//} // REX_NAMESPACE

//class RSSettingsManager : public QObject
//{
//    Q_OBJECT

//private:
//    RSSettingsManager(QObject *parent = 0);

//public:
//    ~RSSettingsManager();
//    static RSSettingsManager* Instance(QObject *parent = 0);

//private:
//    void createObjects();
//    void createConnections();

//public:
//    //-------------------------------------------------------------------
//    // load settings
//    //-------------------------------------------------------------------

//    void load();

//private:
//    //[RexSensors]
//    QVariant loadG6DatabaseFile();
//    QVariant loadG7DatabaseFile();

//    //[RSTimesManager]
//    QVariant loadStartDate();
//    QVariant loadEndDate();
//    QVariant loadStepDate();
//    QVariant loadStepView();
//    QVariant loadStepViewMax();

//    //[RSFiltersManager]
//    QVariant loadBrandIndex();
//    QVariant loadModelIndex();
//    QVariant loadTechnologyIndex();
//    QVariant loadPhysicalMeasureIndex();
//    QVariant loadOutputSignalIndex();
//    QVariant loadMeasureRangeIndex();
//    QVariant loadTheoricalAccuracyIndex();
//    QVariant loadUnitIndex();
//    QVariant loadExperimentationIndex();

//    QVariant loadFilterQuery();
//    QVariant loadSensorNameIndex();

//    //[RSOptionsManager]
//    QVariant loadFidelity();
//    QVariant loadFidelityMin();
//    QVariant loadFidelityMax();
//    QVariant loadFidelityAvg();

//    QVariant loadNoise();
//    QVariant loadNoiseMin();
//    QVariant loadNoiseMax();
//    QVariant loadNoiseAvg();

//    QVariant loadFailure();
//    QVariant loadFailureMin();
//    QVariant loadFailureMax();
//    QVariant loadFailureAvg();

//    QVariant loadTrend();
//    QVariant loadSigma();
//    QVariant loadNoiseFactor();

//public:
//    //-------------------------------------------------------------------
//    // save settings
//    //-------------------------------------------------------------------

//    void save();

//private:
//    //[RexSensors]
//    void saveG6DatabaseFile();
//    void saveG7DatabaseFile();

//    //[RSTimesManager]
//    void saveStartDate();
//    void saveEndDate();
//    void saveStepDate();
//    void saveStepView();
//    void saveStepViewMax();

//    //[RSFiltersManager]
//    void saveBrandIndex();
//    void saveModelIndex();
//    void saveTechnologyIndex();
//    void savePhysicalMeasureIndex();
//    void saveOutputSignalIndex();
//    void saveMeasureRangeIndex();
//    void saveTheoricalAccuracyIndex();
//    void saveUnitIndex();
//    void saveExperimentationIndex();

//    void saveFilterQuery();
//    void saveSensorNameIndex();

//    //[RSOptionsManager]
//    void saveFidelity();
//    void saveFidelityMin();
//    void saveFidelityMax();
//    void saveFidelityAvg();

//    void saveNoise();
//    void saveNoiseMin();
//    void saveNoiseMax();
//    void saveNoiseAvg();

//    void saveFailure();
//    void saveFailureMin();
//    void saveFailureMax();
//    void saveFailureAvg();

//    void saveTrend();
//    void saveSigma();
//    void saveNoiseFactor();

//private:
//    static RSSettingsManager* m_instance;
//};

//#endif // RSSettingsManager_H
