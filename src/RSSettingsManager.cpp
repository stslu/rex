//#include "RSSettingsManager.h"
//#include "RSGlobalMethods.h"
//#include "RSDataManager.h"
//#include "RSMessageView.h"

//RSSettingsManager* RSSettingsManager::m_instance = 0;

//RSSettingsManager::RSSettingsManager(QObject *parent) : QObject(parent)
//{
//    createObjects();
//    createConnections();
//}

//RSSettingsManager::~RSSettingsManager()
//{

//}

//RSSettingsManager* RSSettingsManager::Instance(QObject *parent)
//{
//    if(m_instance == 0)
//    {
//        m_instance = new RSSettingsManager(parent);
//    }

//    return m_instance;
//}

//void RSSettingsManager::createObjects()
//{

//}

//void RSSettingsManager::createConnections()
//{

//}

////-------------------------------------------------------------------
//// load settings
////-------------------------------------------------------------------

//void RSSettingsManager::load()
//{
//    //[RexSensors]
//    RSDataManager::Instance()->setData("G6DatabaseFile", loadG6DatabaseFile());
//    RSDataManager::Instance()->setData("G7DatabaseFile", loadG7DatabaseFile());

//    //[RSTimesManager]
//    RSDataManager::Instance()->setData("StartDate", loadStartDate());
//    RSDataManager::Instance()->setData("EndDate", loadEndDate());
//    RSDataManager::Instance()->setData("StepDate", loadStepDate());
//    RSDataManager::Instance()->setData("TimesManager.StepView", loadStepView());
//    RSDataManager::Instance()->setData("TimesManager.StepViewMax", loadStepViewMax());

//    //[RSFiltersManager]
//    RSDataManager::Instance()->setData("BrandIndex", loadBrandIndex());
//    RSDataManager::Instance()->setData("ModelIndex", loadModelIndex());
//    RSDataManager::Instance()->setData("TechnologyIndex", loadTechnologyIndex());
//    RSDataManager::Instance()->setData("PhysicalMeasureIndex", loadPhysicalMeasureIndex());
//    RSDataManager::Instance()->setData("OutputSignalIndex", loadOutputSignalIndex());
//    RSDataManager::Instance()->setData("MeasureRangeIndex", loadMeasureRangeIndex());
//    RSDataManager::Instance()->setData("TheoricalAccuracyIndex", loadTheoricalAccuracyIndex());
//    RSDataManager::Instance()->setData("UnitIndex", loadUnitIndex());
//    RSDataManager::Instance()->setData("ExperimentationIndex", loadExperimentationIndex());

//    RSDataManager::Instance()->setData("FilterQuery", loadFilterQuery());
//    RSDataManager::Instance()->setData("SensorNameIndex", loadSensorNameIndex());

//    //[RSOptionsManager]
//    RSDataManager::Instance()->setData("Fidelity", loadFidelity());
//    RSDataManager::Instance()->setData("FidelityMin", loadFidelityMin());
//    RSDataManager::Instance()->setData("FidelityMax", loadFidelityMax());
//    RSDataManager::Instance()->setData("FidelityAvg", loadFidelityAvg());

//    RSDataManager::Instance()->setData("Noise", loadNoise());
//    RSDataManager::Instance()->setData("NoiseMin", loadNoiseMin());
//    RSDataManager::Instance()->setData("NoiseMax", loadNoiseMax());
//    RSDataManager::Instance()->setData("NoiseAvg", loadNoiseAvg());

//    RSDataManager::Instance()->setData("Failure", loadFailure());
//    RSDataManager::Instance()->setData("FailureMin", loadFailureMin());
//    RSDataManager::Instance()->setData("FailureMax", loadFailureMax());
//    RSDataManager::Instance()->setData("FailureAvg", loadFailureAvg());

//    RSDataManager::Instance()->setData("Trend", loadTrend());
//    RSDataManager::Instance()->setData("Sigma", loadSigma());
//    RSDataManager::Instance()->setData("NoiseFactor", loadNoiseFactor());
//}

////[RexSensors]
////-------------------------------------------------------------------

////[RexSensors]
//QVariant RSSettingsManager::loadG6DatabaseFile()
//{
//    QString m_id = "RexSensors";
//    QString m_key = "G6DatabaseFile";
//    QVariant m_default = REX::DEFAULT_REX_G6_DB_FILE;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RexSensors]
//QVariant RSSettingsManager::loadG7DatabaseFile()
//{
//    QString m_id = "RexSensors";
//    QString m_key = "G7DatabaseFile";
//    QVariant m_default = REX::DEFAULT_REX_G7_DB_FILE;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSTimesManager]
////-------------------------------------------------------------------

////[RSTimesManager]
//QVariant RSSettingsManager::loadStartDate()
//{
//    QString m_id = "RSTimesManager";
//    QString m_key = "StartDate";
//    QVariant m_default = REX::DEFAULT_START_DATE;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSTimesManager]
//QVariant RSSettingsManager::loadEndDate()
//{
//    QString m_id = "RSTimesManager";
//    QString m_key = "EndDate";
//    QVariant m_default = REX::DEFAULT_END_DATE;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSTimesManager]
//QVariant RSSettingsManager::loadStepDate()
//{
//    QString m_id = "RSTimesManager";
//    QString m_key = "StepDate";
//    QVariant m_default = REX::DEFAULT_STEP_DATE_DAY;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSTimesManager]
//QVariant RSSettingsManager::loadStepView()
//{
//    QString m_id = "RSTimesManager";
//    QString m_key = "StepView";
//    QVariant m_default = REX::DEFAULT_STEP_VIEW_IDX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSTimesManager]
//QVariant RSSettingsManager::loadStepViewMax()
//{
//    QString m_id = "RSTimesManager";
//    QString m_key = "TimesManager.StepViewMax";
//    QVariant m_default = REX::DEFAULT_STEP_VIEW_IDX_MAX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSFiltersManager]
////-------------------------------------------------------------------

////[RSFiltersManager]
//QVariant RSSettingsManager::loadBrandIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "BrandIndex";
//    QVariant m_default = REX::DEFAULT_FILTER_FIELD_INDEX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSFiltersManager]
//QVariant RSSettingsManager::loadModelIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "ModelIndex";
//    QVariant m_default = REX::DEFAULT_FILTER_FIELD_INDEX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSFiltersManager]
//QVariant RSSettingsManager::loadTechnologyIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "TechnologyIndex";
//    QVariant m_default = REX::DEFAULT_FILTER_FIELD_INDEX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSFiltersManager]
//QVariant RSSettingsManager::loadPhysicalMeasureIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "PhysicalMeasureIndex";
//    QVariant m_default = REX::DEFAULT_FILTER_FIELD_INDEX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSFiltersManager]
//QVariant RSSettingsManager::loadOutputSignalIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "OutputSignalIndex";
//    QVariant m_default = REX::DEFAULT_FILTER_FIELD_INDEX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSFiltersManager]
//QVariant RSSettingsManager::loadMeasureRangeIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "MeasureRangeIndex";
//    QVariant m_default = REX::DEFAULT_FILTER_FIELD_INDEX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSFiltersManager]
//QVariant RSSettingsManager::loadTheoricalAccuracyIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "TheoricalAccuracyIndex";
//    QVariant m_default = REX::DEFAULT_FILTER_FIELD_INDEX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSFiltersManager]
//QVariant RSSettingsManager::loadUnitIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "UnitIndex";
//    QVariant m_default = REX::DEFAULT_FILTER_FIELD_INDEX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSFiltersManager]
//QVariant RSSettingsManager::loadExperimentationIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "ExperimentationIndex";
//    QVariant m_default = REX::DEFAULT_FILTER_FIELD_INDEX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSFiltersManager]
//QVariant RSSettingsManager::loadFilterQuery()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "FilterQuery";
//    QVariant m_default = REX::DEFAULT_FILTER_QUERY;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSFiltersManager]
//QVariant RSSettingsManager::loadSensorNameIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "SensorNameIndex";
//    QVariant m_default = REX::DEFAULT_SENSOR_NAME_INDEX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
////-------------------------------------------------------------------

////[RSOptionsManager]
//QVariant RSSettingsManager::loadFidelity()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "Fidelity";
//    QVariant m_default = REX::DEFAULT_FIDELITY;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
//QVariant RSSettingsManager::loadFidelityMin()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "FidelityMin";
//    QVariant m_default = REX::DEFAULT_FIDELITY_MIN;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
//QVariant RSSettingsManager::loadFidelityMax()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "FidelityMax";
//    QVariant m_default = REX::DEFAULT_FIDELITY_MAX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
//QVariant RSSettingsManager::loadFidelityAvg()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "FidelityAvg";
//    QVariant m_default = REX::DEFAULT_FIDELITY_AVG;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
//QVariant RSSettingsManager::loadNoise()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "Noise";
//    QVariant m_default = REX::DEFAULT_NOISE;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
//QVariant RSSettingsManager::loadNoiseMin()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "NoiseMin";
//    QVariant m_default = REX::DEFAULT_NOISE_MIN;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
//QVariant RSSettingsManager::loadNoiseMax()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "NoiseMax";
//    QVariant m_default = REX::DEFAULT_NOISE_MAX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
//QVariant RSSettingsManager::loadNoiseAvg()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "NoiseAvg";
//    QVariant m_default = REX::DEFAULT_NOISE_AVG;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
//QVariant RSSettingsManager::loadFailure()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "Failure";
//    QVariant m_default = REX::DEFAULT_FAILURE;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
//QVariant RSSettingsManager::loadFailureMin()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "FailureMin";
//    QVariant m_default = REX::DEFAULT_FAILURE_MIN;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
//QVariant RSSettingsManager::loadFailureMax()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "FailureMax";
//    QVariant m_default = REX::DEFAULT_FAILURE_MAX;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
//QVariant RSSettingsManager::loadFailureAvg()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "FailureAvg";
//    QVariant m_default = REX::DEFAULT_FAILURE_AVG;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
//QVariant RSSettingsManager::loadTrend()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "Trend";
//    QVariant m_default = REX::DEFAULT_TREND_DEGREE;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
//QVariant RSSettingsManager::loadSigma()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "Sigma";
//    QVariant m_default = REX::DEFAULT_SIGMA_PERCENTAGE;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////[RSOptionsManager]
//QVariant RSSettingsManager::loadNoiseFactor()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "NoiseFactor";
//    QVariant m_default = REX::DEFAULT_NOISE_FACTOR;
//    return RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);
//}

////-------------------------------------------------------------------
//// save settings
////-------------------------------------------------------------------

//void RSSettingsManager::save()
//{
//    //[RexSensors]
//    saveG6DatabaseFile();
//    saveG7DatabaseFile();

//    //[RSTimesManager]
//    saveStartDate();
//    saveEndDate();
//    saveStepDate();
//    saveStepView();
//    saveStepViewMax();

//    //[RSFiltersManager]
//    saveBrandIndex();
//    saveModelIndex();
//    saveTechnologyIndex();
//    savePhysicalMeasureIndex();
//    saveOutputSignalIndex();
//    saveMeasureRangeIndex();
//    saveTheoricalAccuracyIndex();
//    saveUnitIndex();
//    saveExperimentationIndex();

//    saveFilterQuery();
//    saveSensorNameIndex();

//    //[RSOptionsManager]
//    saveFidelity();
//    saveFidelityMin();
//    saveFidelityMax();
//    saveFidelityAvg();

//    saveNoise();
//    saveNoiseMin();
//    saveNoiseMax();
//    saveNoiseAvg();

//    saveFailure();
//    saveFailureMin();
//    saveFailureMax();
//    saveFailureAvg();

//    saveTrend();
//    saveSigma();
//    saveNoiseFactor();
//}

////[RexSensors]
////-------------------------------------------------------------------

////[RexSensors]
//void RSSettingsManager::saveG6DatabaseFile()
//{
//    QString m_id = "RexSensors";
//    QString m_key = "G6DatabaseFile";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RexSensors]
//void RSSettingsManager::saveG7DatabaseFile()
//{
//    QString m_id = "RexSensors";
//    QString m_key = "G7DatabaseFile";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSTimesManager]
////-------------------------------------------------------------------

////[RSTimesManager]
//void RSSettingsManager::saveStartDate()
//{
//    QString m_id = "RSTimesManager";
//    QString m_key = "StartDate";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSTimesManager]
//void RSSettingsManager::saveEndDate()
//{
//    QString m_id = "RSTimesManager";
//    QString m_key = "EndDate";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSTimesManager]
//void RSSettingsManager::saveStepDate()
//{
//    QString m_id = "RSTimesManager";
//    QString m_key = "StepDate";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSTimesManager]
//void RSSettingsManager::saveStepView()
//{
//    QString m_id = "RSTimesManager";
//    QString m_key = "StepView";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSTimesManager]
//void RSSettingsManager::saveStepViewMax()
//{
//    QString m_id = "RSTimesManager";
//    QString m_key = "StepViewMax";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSFiltersManager]
////-------------------------------------------------------------------

////[RSFiltersManager]
//void RSSettingsManager::saveBrandIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "BrandIndex";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSFiltersManager]
//void RSSettingsManager::saveModelIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "ModelIndex";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSFiltersManager]
//void RSSettingsManager::saveTechnologyIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "TechnologyIndex";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSFiltersManager]
//void RSSettingsManager::savePhysicalMeasureIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "PhysicalMeasureIndex";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSFiltersManager]
//void RSSettingsManager::saveOutputSignalIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "OutputSignalIndex";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSFiltersManager]
//void RSSettingsManager::saveMeasureRangeIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "MeasureRangeIndex";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSFiltersManager]
//void RSSettingsManager::saveTheoricalAccuracyIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "TheoricalAccuracyIndex";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSFiltersManager]
//void RSSettingsManager::saveUnitIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "UnitIndex";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSFiltersManager]
//void RSSettingsManager::saveExperimentationIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "ExperimentationIndex";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSFiltersManager]
//void RSSettingsManager::saveFilterQuery()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "FilterQuery";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSFiltersManager]
//void RSSettingsManager::saveSensorNameIndex()
//{
//    QString m_id = "RSFiltersManager";
//    QString m_key = "SensorNameIndex";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSOptionsManager]
////-------------------------------------------------------------------

////[RSOptionsManager]
//void RSSettingsManager::saveFidelity()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "Fidelity";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSOptionsManager]
//void RSSettingsManager::saveFidelityMin()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "FidelityMin";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSOptionsManager]
//void RSSettingsManager::saveFidelityMax()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "FidelityMax";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSOptionsManager]
//void RSSettingsManager::saveFidelityAvg()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "FidelityAvg";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSOptionsManager]
//void RSSettingsManager::saveNoise()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "Noise";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSOptionsManager]
//void RSSettingsManager::saveNoiseMin()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "NoiseMin";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSOptionsManager]
//void RSSettingsManager::saveNoiseMax()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "NoiseMax";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSOptionsManager]
//void RSSettingsManager::saveNoiseAvg()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "NoiseAvg";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}


////[RSOptionsManager]
//void RSSettingsManager::saveFailure()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "Failure";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSOptionsManager]
//void RSSettingsManager::saveFailureMin()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "FailureMin";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSOptionsManager]
//void RSSettingsManager::saveFailureMax()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "FailureMax";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSOptionsManager]
//void RSSettingsManager::saveFailureAvg()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "FailureAvg";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSOptionsManager]
//void RSSettingsManager::saveTrend()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "Trend";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSOptionsManager]
//void RSSettingsManager::saveSigma()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "Sigma";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}

////[RSOptionsManager]
//void RSSettingsManager::saveNoiseFactor()
//{
//    QString m_id = "RSOptionsManager";
//    QString m_key = "NoiseFactor";
//    QVariant m_data = RSDataManager::Instance()->getData(m_key);
//    RSGlobalMethods::Instance()->saveData(m_id, m_key, m_data);
//}
