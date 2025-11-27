#include "RSFiltersManager.h"
#include "ui_RSFiltersManager.h"
#include "RSDataManager.h"
#include "RSDatabaseAccess.h"
#include "RSMessageView.h"
#include "RSSettingsManager.h"
#include "RSPictoManager.h"
#include "Signaler.h"
#include "RSGlobalMethods.h"

#include <RSLogger.h>



namespace RexFiltersDefaultSettings
{
const QString   DEFAULT_FILTER_FIELD_DATA = "All";
const int       DEFAULT_FILTER_FIELD_INDEX = 0;
const QString   DEFAULT_FILTER_QUERY = "";
const int       DEFAULT_SENSOR_NAME_INDEX = 10;
} // REX_NAMESPACE


RSFiltersManager::RSFiltersManager(RSDatabaseAccess* dbAcess,QWidget *parent) : QWidget(parent)
  , ui(new Ui::RSFiltersManager)
  , IRSFiltersManager()
{
    ui->setupUi(this);

    createObjects();

    createConnections();

    //updateDataManager();

}

RSFiltersManager::~RSFiltersManager()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
    saveSettings(QString());

    delete ui;
}


void RSFiltersManager::createObjects()
{
    ui->m_resetButton->setCursor(Qt::PointingHandCursor);
    ui->m_resetButton->setIcon(RSPictoManager::Instance()->getIcon(fa::fa_refresh, "black"));

    ui->setDynamicFilter->setIcon(RSPictoManager::Instance()->getIcon(fa::fa_bolt, "black"));
    ui->setDynamicFilter->setCursor(Qt::PointingHandCursor);
    ui->setDynamicFilter->setCheckable(true);
    ui->setDynamicFilter->setChecked(true);

    loadSettings(QString());

    setData(ui->m_brandEdit, "Brand", ui->m_brandEdit->currentIndex());
    setData(ui->m_modelEdit, "Model",  ui->m_modelEdit->currentIndex());
    setData(ui->m_technologyEdit, "Technology",  ui->m_technologyEdit->currentIndex());
    setData(ui->m_physicalMeasurementEdit, "PhysicalMeasure",  ui->m_physicalMeasurementEdit->currentIndex());
    setData(ui->m_outputSignalEdit, "OutputSignal",  ui->m_outputSignalEdit->currentIndex());
    setData(ui->m_measurementRangeEdit, "MeasureRange",  ui->m_measurementRangeEdit->currentIndex());
    setData(ui->m_theoricalAccuracyEdit, "TheoricalAccuracy",  ui->m_theoricalAccuracyEdit->currentIndex());
    setData(ui->m_unitEdit, "Unit",  ui->m_unitEdit->currentIndex());
    setData(ui->m_experimentationEdit, "Experimentation",  ui->m_experimentationEdit->currentIndex());


}

void RSFiltersManager::createConnections()
{
    connect(ui->m_resetButton,  &QToolButton::clicked, this, &RSFiltersManager::slotResetButtonClicked);
    connect(ui->setDynamicFilter, &QToolButton::clicked, this, &RSFiltersManager::onSetDynamicFilterClicked);
}

 void RSFiltersManager::onSetDynamicFilterClicked()
 {
     updateFiltersCount();
 }

void RSFiltersManager::setData(QComboBox* comboBox, QString label, int lastIndex)
{
    m_comboTextMap.insert(comboBox, label);
}


void RSFiltersManager::setFilterFieldAllData()
{
    //! block signals and clear
    foreach (QComboBox *comboBox, m_comboTextMap.keys())
    {
        comboBox->blockSignals(true);
        comboBox->clear();
        comboBox->addItem(RexFiltersDefaultSettings::DEFAULT_FILTER_FIELD_DATA);
    }

    ui->m_brandEdit->addItems(RSDatabaseAccess::Instance()->getBrandNameList());

    ui->m_modelEdit->addItems(RSDatabaseAccess::Instance()->getModelNameList());

    ui->m_technologyEdit->addItems(RSDatabaseAccess::Instance()->getTechnologyNameList());

    ui->m_physicalMeasurementEdit->addItems(RSDatabaseAccess::Instance()->getPhysicalMeasurementNameList());

    ui->m_outputSignalEdit->addItems(RSDatabaseAccess::Instance()->getOutputSignalNameList());

    ui->m_measurementRangeEdit->addItems(RSDatabaseAccess::Instance()->getMeasurementRangeNameList());

    ui->m_theoricalAccuracyEdit->addItems(RSDatabaseAccess::Instance()->getTheoricalAccuracyNameList());

    ui->m_unitEdit->addItems(RSDatabaseAccess::Instance()->getUnitNameList());

    ui->m_experimentationEdit->addItems(RSDatabaseAccess::Instance()->getExperimentationNameList());

    //! enable signals and set ALL
    foreach (QComboBox *comboBox, m_comboTextMap.keys())
    {
        comboBox->setCurrentIndex(0);
        comboBox->blockSignals(false);
    }

    updateFiltersCount();
}

void RSFiltersManager::updateFiltersCount()
{
    ui->nbBrand->setText(QString("(%1)").arg(ui->m_brandEdit->count() - 1));
    ui->nbModel->setText(QString("(%1)").arg(ui->m_modelEdit->count() - 1));
    ui->nbTechno->setText(QString("(%1)").arg(ui->m_technologyEdit->count() - 1));
    ui->nbPhysicalMeas->setText(QString("(%1)").arg(ui->m_physicalMeasurementEdit->count() - 1));
    ui->nbOutputSignal->setText(QString("(%1)").arg(ui->m_outputSignalEdit->count() - 1));
    ui->nbMeasRange->setText(QString("(%1)").arg(ui->m_measurementRangeEdit->count() - 1));
    ui->nbTheoricalAccuracy->setText(QString("(%1)").arg(ui->m_theoricalAccuracyEdit->count() - 1));
    ui->nbUnit->setText(QString("(%1)").arg(ui->m_unitEdit->count() - 1));
    ui->nbExperimentations->setText(QString("(%1)").arg(ui->m_experimentationEdit->count() - 1));
}


void RSFiltersManager::setFiltersFieldsFromRexFilter(QComboBox* activeCombo)
{

    //! block signals and clear
   const QString strAll = RexFiltersDefaultSettings::DEFAULT_FILTER_FIELD_DATA;
    foreach (QComboBox *combo, m_comboTextMap.keys())
    {
        if((combo != activeCombo) || (activeCombo->currentText() == strAll))
        {
            combo->blockSignals(true);
            combo->clear();
            combo->addItem(strAll);
        }
    }

    if((activeCombo != ui->m_brandEdit) ||(activeCombo->currentText() == strAll) )
        ui->m_brandEdit->addItems(RSDatabaseAccess::Instance()->getFilteredBrandNameList());

    if((activeCombo != ui->m_modelEdit) ||(activeCombo->currentText() == strAll))
        ui->m_modelEdit->addItems(RSDatabaseAccess::Instance()->getFilteredModelNameList());

    if((activeCombo != ui->m_technologyEdit) ||(activeCombo->currentText() == strAll))
        ui->m_technologyEdit->addItems(RSDatabaseAccess::Instance()->getFilteredTechnologyNameList());

    if((activeCombo != ui->m_physicalMeasurementEdit) ||(activeCombo->currentText() == strAll))
        ui->m_physicalMeasurementEdit->addItems(RSDatabaseAccess::Instance()->getFilteredPhysicalMeasurementNameList());

    if((activeCombo != ui->m_outputSignalEdit) ||(activeCombo->currentText() == strAll))
        ui->m_outputSignalEdit->addItems(RSDatabaseAccess::Instance()->getFilteredOutputSignalNameList());

    if((activeCombo != ui->m_measurementRangeEdit) ||(activeCombo->currentText() == strAll))
        ui->m_measurementRangeEdit->addItems(RSDatabaseAccess::Instance()->getFilteredMeasurementRangeNameList());

    if((activeCombo != ui->m_theoricalAccuracyEdit) ||(activeCombo->currentText() == strAll))
        ui->m_theoricalAccuracyEdit->addItems(RSDatabaseAccess::Instance()->getFilteredTheoricalAccuracyNameList());

    if((activeCombo != ui->m_unitEdit) ||(activeCombo->currentText() == strAll))
        ui->m_unitEdit->addItems(RSDatabaseAccess::Instance()->getFilteredUnitNameList());

    if((activeCombo != ui->m_experimentationEdit) ||(activeCombo->currentText() == strAll))
        ui->m_experimentationEdit->addItems(RSDatabaseAccess::Instance()->getFilteredExperimentationNameList());

    //!Display the last values
    Q_FOREACH(QComboBox* combo,m_comboTextMap.keys())
    {
        if((combo != activeCombo)  ||(activeCombo->currentText() == strAll))
        {
            combo->setCurrentText(m_comboTextMap.value(combo));
            combo->blockSignals(false);
        }
    }

    updateFiltersCount();
}

void RSFiltersManager::connectFilters()
{
    foreach (QComboBox *comboBox, m_comboTextMap.keys())
    {
        connect(comboBox, SIGNAL(currentIndexChanged(QString)), this,SLOT(slotCurrentIndexChanged(QString)), Qt::UniqueConnection);
    }
}

void RSFiltersManager::blockFiltersSignals(bool block)
{
    foreach (QComboBox *comboBox, m_comboTextMap.keys())
        comboBox->blockSignals(block);
}

void RSFiltersManager::setFilterFields()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");
    blockFiltersSignals(true);

    if(!RSDatabaseAccess::Instance()->isG6StructureOK())
    {
        RSMessageView::Instance()->clear();
        RSMessageView::Instance()->showData(tr("G6 datatabase : INVALID STRUCTURE. Close and restart REX"));
        return ;
    }

    if(!RSDatabaseAccess::Instance()->isG7StructureOK())
    {
        RSMessageView::Instance()->clear();
        RSMessageView::Instance()->showData(tr("G7 datatabase : INVALID STRUCTURE.Close and restart REX"));
        return ;
    }

    //Build a listcof values for each filter
    setFilterFieldAllData();

    connectFilters();

    blockFiltersSignals(false);

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

QString RSFiltersManager::getFilterQuery(QComboBox *comboBox, const QString& filterName) const
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString currentText = comboBox->currentText();
    QString filterQuery = "";

    if(currentText.toUpper() != "ALL")
    {
        filterQuery += QString("AND %1 = '%2' ")
                .arg(filterName)
                .arg(currentText);
        RSLogger::instance()->info(Q_FUNC_INFO, "Query is : " + filterQuery);
    }
    else
        RSLogger::instance()->info(Q_FUNC_INFO, "Query is : All");

    RSLogger::instance()->info(Q_FUNC_INFO, "End");

    return filterQuery;
}


const QString& RSFiltersManager::initFiltersQuery()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString filterQuery = "";
    QString query;

    //--- --Create the Query
    filterQuery =  getFilterQuery(ui->m_brandEdit, "AST_BRAND");
    filterQuery += getFilterQuery(ui->m_brandEdit, "AST_BRAND");
    filterQuery += getFilterQuery(ui->m_modelEdit, "AST_MODEL");
    filterQuery += getFilterQuery(ui->m_technologyEdit, "AST_TECHNOLOGY");
    filterQuery += getFilterQuery(ui->m_physicalMeasurementEdit, "AST_PHYSICALMEASUREMENT");
    filterQuery += getFilterQuery(ui->m_outputSignalEdit, "AST_OUTPUTSIGNAL");
    filterQuery += getFilterQuery(ui->m_measurementRangeEdit, "AST_RANGE");
    filterQuery += getFilterQuery(ui->m_theoricalAccuracyEdit, "AST_THEORICALACCURACY");
    filterQuery += getFilterQuery(ui->m_unitEdit, "AST_UNIT");
    filterQuery += getFilterQuery(ui->m_experimentationEdit, "TAG_NAME");

    if(filterQuery.toUpper().startsWith("AND"))
        filterQuery =  filterQuery.remove(0,strlen("AND")).trimmed();

    //--- --Add the query in the Map
    RSDataManager::Instance()->setData("FilterQuery", filterQuery);

    RSLogger::instance()->info(Q_FUNC_INFO, "End. m_filterQuery = " + filterQuery);

    return filterQuery;
}

void RSFiltersManager::slotCurrentIndexChanged(const QString& text)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    //! --- --Get the current combo address
    QComboBox* activeCombo = qobject_cast<QComboBox*>(sender());
    if(!activeCombo)
        return;
    //! Memorize the last text
    m_comboTextMap[activeCombo] = text;

    //! --- --update the field value of this filter in the map
    RSLogger::instance()->info(Q_FUNC_INFO, "update the field value of this filter in the map");

    QString filterQuery =  initFiltersQuery();

    //! Update the REXFILTER Table, from REXDATASET
    RSLogger::instance()->info(Q_FUNC_INFO, "Update the REXFILTER  : \n" + filterQuery);
    bool m_checkFilter = RSDatabaseAccess::Instance()->checkFilterQueryAndBuildRexFilterTable(filterQuery);

    //! If this selection doens contain datas, back to the prvious
    if(m_checkFilter == false)
    {
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail.This filter don't contain data.");
        displayNbSensors(0);
        emit Signaler::instance()->signal_clearGraphsAndSensorList();
        return;
    }

    RSLogger::instance()->info(Q_FUNC_INFO, "emit signal_rsFilterIndexChanged");

    //!Reset the filters in dynamic mode
    if(isDynamic())
    {
        setFiltersFieldsFromRexFilter(activeCombo);
    }

    emit Signaler::instance()->signal_rsFilterIndexChanged();

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}


void  RSFiltersManager::initQueryAndRexFilterTable()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString filterQuery =  initFiltersQuery();

    //Update the REXFILTER Table, from REXDATASET
    RSLogger::instance()->info(Q_FUNC_INFO, "Update the REXFILTER  with query : \n" + filterQuery);
    bool m_checkFilter = RSDatabaseAccess::Instance()->checkFilterQueryAndBuildRexFilterTable(filterQuery);

    //If this selection doens contain datas, back to the prvious
    if(m_checkFilter == false)
    {
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail.This filter don't contain data.");
        displayNbSensors(0);
        emit Signaler::instance()->signal_clearGraphsAndSensorList();
        return;
    }
    ui->m_title->setText("Filters");

    RSLogger::instance()->info(Q_FUNC_INFO, "emit signal_rsFilterIndexChanged");

    emit Signaler::instance()->signal_rsFilterIndexChanged();

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSFiltersManager::slotResetButtonClicked()
{
    setFilterFieldAllData();

    QString strQuery =   initFiltersQuery();

    RSDatabaseAccess::Instance()->checkFilterQueryAndBuildRexFilterTable(strQuery);

    emit Signaler::instance()->signal_rsFilterIndexChanged();
}



FiltersEnum RSFiltersManager::getFilterEnum(const QComboBox* comboBox) const
{
    FiltersEnum filterEnum = FiltersEnum::Brand;

    if(comboBox == ui->m_brandEdit)
        filterEnum =  FiltersEnum::Brand;
    else if(comboBox == ui->m_modelEdit)
        filterEnum =  FiltersEnum::Model;
    else if(comboBox == ui->m_technologyEdit)
        filterEnum =  FiltersEnum::Technology;
    else if(comboBox == ui->m_physicalMeasurementEdit)
        filterEnum =  FiltersEnum::PhysicalMeasurement;
    else if(comboBox == ui->m_outputSignalEdit)
        filterEnum =  FiltersEnum::OutputSignal;
    else if(comboBox == ui->m_theoricalAccuracyEdit)
        filterEnum =  FiltersEnum::TheoricalAccuracy;
    else if(comboBox == ui->m_unitEdit)
        filterEnum =  FiltersEnum::Unit;
    else if(comboBox == ui->m_experimentationEdit)
        filterEnum =  FiltersEnum::experimentation;
    else if(comboBox == ui->m_brandEdit)
        filterEnum =  FiltersEnum::Technology;

    return filterEnum;
}
void RSFiltersManager::loadSettings(const QString& fileName)
{
    loadBrand();
    loadModel();
    loadTechnology();
    loadPhysicalMeasure();
    loadOutputSignal();
    loadMeasureRange();
    loadTheoricalAccuracy();
    loadUnit();
    loadExperimentation();
    loadDynamicOption();
}

void RSFiltersManager::saveSettings(const QString& fileName) const
{
    saveBrand() ;
    saveModel();
    saveTechnology();
    savePhysicalMeasure();
    saveOutputSignal();
    saveMeasureRange();
    saveTheoricalAccuracy();
    saveUnit();
    saveExperimentation();
}

void RSFiltersManager::setFilterValue(QComboBox* combo, const QString& value)
{
    combo->blockSignals(true);
    int index = combo->findText(value,Qt::MatchFlag::MatchExactly);

    if(index < 0)
        index = 0;
    combo->blockSignals(true);
    combo->setCurrentIndex(index);
    combo->blockSignals(false);
}


QVariant RSFiltersManager::loadBrand()
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.Brand";
    QVariant m_default = RexFiltersDefaultSettings::DEFAULT_FILTER_FIELD_INDEX;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default).value<QString>();
    setFilterValue( ui->m_brandEdit,data.value<QString>());

    RSLogger::instance()->info(Q_FUNC_INFO,"Brand = " + data.value<QString>());

    return data;
}

QVariant RSFiltersManager::loadDynamicOption()
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.DynamicFilter";
    QVariant m_default = RexFiltersDefaultSettings::DEFAULT_FILTER_FIELD_INDEX;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default).value<QString>();
    ui->setDynamicFilter->setChecked(data.toBool());

    RSLogger::instance()->info(Q_FUNC_INFO,"DynamicFilter = " + data.toBool());

    return data;
}



//[RSFiltersManager]
QVariant RSFiltersManager::loadModel()
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.Model";
    QVariant m_default = RexFiltersDefaultSettings::DEFAULT_FILTER_FIELD_INDEX;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default).value<QString>();
    setFilterValue( ui->m_modelEdit,data.value<QString>());

    RSLogger::instance()->info(Q_FUNC_INFO,"Model = " + data.value<QString>());
    return data;
}

//[RSFiltersManager]
QVariant RSFiltersManager::loadTechnology()
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.Technology";
    QVariant m_default = RexFiltersDefaultSettings::DEFAULT_FILTER_FIELD_INDEX;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default).value<QString>();
    setFilterValue( ui->m_technologyEdit,data.value<QString>());
    RSLogger::instance()->info(Q_FUNC_INFO,"Technology = " + data.value<QString>());
    return data;
}

//[RSFiltersManager]
QVariant RSFiltersManager::loadPhysicalMeasure()
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.PhysicalMeasure";
    QVariant m_default = RexFiltersDefaultSettings::DEFAULT_FILTER_FIELD_INDEX;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default).value<QString>();
    setFilterValue( ui->m_physicalMeasurementEdit,data.value<QString>());

    RSLogger::instance()->info(Q_FUNC_INFO,"PhysicalMeasure = " + data.value<QString>());
    return data;
}

//[RSFiltersManager]
QVariant RSFiltersManager::loadOutputSignal()
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.OutputSignal";
    QVariant m_default = RexFiltersDefaultSettings::DEFAULT_FILTER_FIELD_INDEX;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default).value<QString>();
    setFilterValue( ui->m_outputSignalEdit,data.value<QString>());

    RSLogger::instance()->info(Q_FUNC_INFO,"OutputSignal = " + data.value<QString>());
    return data;
}

//[RSFiltersManager]
QVariant RSFiltersManager::loadMeasureRange()
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.MeasureRange";
    QVariant m_default = RexFiltersDefaultSettings::DEFAULT_FILTER_FIELD_INDEX;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default).value<QString>();
    setFilterValue( ui->m_measurementRangeEdit,data.value<QString>());

    RSLogger::instance()->info(Q_FUNC_INFO,"MeasureRange = " + data.value<QString>());
    return data;
}

//[RSFiltersManager]
QVariant RSFiltersManager::loadTheoricalAccuracy()
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.TheoricalAccuracy";
    QVariant m_default = RexFiltersDefaultSettings::DEFAULT_FILTER_FIELD_INDEX;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default).value<QString>();
    setFilterValue( ui->m_theoricalAccuracyEdit,data.value<QString>());

    RSLogger::instance()->info(Q_FUNC_INFO,"TheoricalAccuracy = " + data.value<QString>());
    return data;
}

//[RSFiltersManager]
QVariant RSFiltersManager::loadUnit()
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.Unit";
    QVariant m_default = RexFiltersDefaultSettings::DEFAULT_FILTER_FIELD_INDEX;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default).value<QString>();
    setFilterValue( ui->m_unitEdit,data.value<QString>());

    RSLogger::instance()->info(Q_FUNC_INFO,"Unit = " + data.value<QString>());
    return data;
}

//[RSFiltersManager]
QVariant RSFiltersManager::loadExperimentation()
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.Experimentation";
    QVariant m_default = RexFiltersDefaultSettings::DEFAULT_FILTER_FIELD_INDEX;

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default).value<QString>();
    setFilterValue( ui->m_experimentationEdit,data.value<QString>());

    RSLogger::instance()->info(Q_FUNC_INFO,"Experimentation = " + data.value<QString>());
    return data;
}

//[RSFiltersManager]
void RSFiltersManager::saveBrand() const
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.Brand";
    QVariant data = ui->m_brandEdit->currentText();

    RSLogger::instance()->info(Q_FUNC_INFO,"Brand = " + data.value<QString>());

    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

//[RSFiltersManager]
void RSFiltersManager::saveDynamicOption() const
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.DynamicFilter";
    QVariant data = ui->setDynamicFilter->isChecked();

    RSLogger::instance()->info(Q_FUNC_INFO,"DynamicFilter = " + data.value<QString>());

    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}


//[RSFiltersManager]
void RSFiltersManager::saveModel()const
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.Model";
    QVariant data = ui->m_modelEdit->currentText();

    RSLogger::instance()->info(Q_FUNC_INFO,"Model = " + data.value<QString>());

    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

//[RSFiltersManager]
void RSFiltersManager::saveTechnology()const
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.Technology";
    QVariant data = ui->m_technologyEdit->currentText();

    RSLogger::instance()->info(Q_FUNC_INFO,"Technology = " + data.value<QString>());

    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

//[RSFiltersManager]
void RSFiltersManager::savePhysicalMeasure()const
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.PhysicalMeasure";
    QVariant data = ui->m_physicalMeasurementEdit->currentText();

    RSLogger::instance()->info(Q_FUNC_INFO,"PhysicalMeasure = " + data.value<QString>());

    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

//[RSFiltersManager]
void RSFiltersManager::saveOutputSignal()const
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.OutputSignal";
    QVariant data = ui->m_outputSignalEdit->currentText();

    RSLogger::instance()->info(Q_FUNC_INFO,"OutputSignal = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

//[RSFiltersManager]
void RSFiltersManager::saveMeasureRange()const
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.MeasureRange";
    QVariant data = ui->m_measurementRangeEdit->currentText();

    RSLogger::instance()->info(Q_FUNC_INFO,"MeasureRange = " + data.value<QString>());

    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

//[RSFiltersManager]
void RSFiltersManager::saveTheoricalAccuracy()const
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.TheoricalAccuracy";
    QVariant data = ui->m_theoricalAccuracyEdit->currentText();

    RSLogger::instance()->info(Q_FUNC_INFO,"TheoricalAccuracy = " + data.value<QString>());

    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

//[RSFiltersManager]
void RSFiltersManager::saveUnit()const
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.Unit";
    QVariant data = ui->m_unitEdit->currentText();

    RSLogger::instance()->info(Q_FUNC_INFO,"Unit = " + data.value<QString>());

    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

//[RSFiltersManager]
void RSFiltersManager::saveExperimentation()const
{
    QString m_id = "RSFiltersManager";
    QString m_key = "RSFiltersManager.Experimentation";
    QVariant data = ui->m_experimentationEdit->currentText();

    RSLogger::instance()->info(Q_FUNC_INFO,"Experimentation = " + data.value<QString>());

    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}


QStringList RSFiltersManager::getExperimentationsList() const
{
    QStringList itemsList;
    QString currentText = ui->m_experimentationEdit->currentText();

    //return all sensors
    if(currentText.toLower().trimmed() == "all")
    {
        for(int i = 0; i < ui->m_experimentationEdit->count(); i++)
        {
            if(ui->m_experimentationEdit->itemText(i).toLower().trimmed() != "all")
                itemsList.push_back(ui->m_experimentationEdit->itemText(i));
        }
    }
    //Return the current sensor
    else
    {
        itemsList.push_back(currentText);
    }

    return itemsList;
}

void  RSFiltersManager::displayNbSensors(int nbSensors)
{
    ui->m_title->setText(QString(tr("%1 sensors for the current criterias ")).arg(nbSensors));
}

bool RSFiltersManager::isDynamic() const
{
    if(ui->setDynamicFilter)
        return ui->setDynamicFilter->isChecked();
    else
        return false;
}
