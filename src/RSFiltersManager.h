#ifndef RSFILTERSMANAGER_H
#define RSFILTERSMANAGER_H

#include <QWidget>
#include <QComboBox>
#include <QMessageBox>
#include <Signaler.h>
#include "IRSFiltersManager.h"

namespace Ui
{
class RSFiltersManager;
}
class RSDatabaseAccess;

class RSFiltersManager : public QWidget, public IRSFiltersManager
{
    Q_OBJECT

public:


    RSFiltersManager(RSDatabaseAccess*,QWidget *parent = 0);
    ~RSFiltersManager();

    //void updateDataManager();

    void loadSettings(const QString& fileName) ;
    void saveSettings(const QString& fileName) const;

    void setFilterFields();
    QString getFilterQuery(QComboBox *comboBox, const QString &filter) const;

    const QString& initFiltersQuery();

    void  initQueryAndRexFilterTable();
    QStringList getExperimentationsList() const;

    void displayNbSensors(int);

     bool isDynamic() const ;



protected slots:
    void slotCurrentIndexChanged(const QString& text);

    /**
     * @brief slotResetButtonClicked : reset the filter widget. Disconnect slots, fill the combo, connect slots
     */
    void slotResetButtonClicked();


    void onSetDynamicFilterClicked();

protected:
    void blockFiltersSignals(bool);

    QVariant loadBrand();
    QVariant loadModel();
    QVariant loadTechnology();
    QVariant loadPhysicalMeasure();
    QVariant loadOutputSignal();
    QVariant loadMeasureRange();
    QVariant loadTheoricalAccuracy();
    QVariant loadUnit();
    QVariant loadExperimentation();
    QVariant loadDynamicOption();

    //[RSFiltersManager]
    void saveBrand() const;
    void saveModel() const;
    void saveTechnology() const;
    void savePhysicalMeasure() const;
    void saveOutputSignal() const;
    void saveMeasureRange() const;
    void saveTheoricalAccuracy() const;
    void saveUnit() const;
    void saveExperimentation() const;
    void saveDynamicOption() const;

    void setFilterValue(QComboBox*,const QString& value);
    void createObjects();

    /**
      * @brief createConnections : connect the slot slotResetButtonClicked to the signal m_resetButton
      */
    void createConnections();

    /**
      * @brief setData: fill the maps Combo/Label
      * @param comboBox: the combobox
      * @param label : the combo label
      * @param lastIndex : the combo index
      */
    void setData(QComboBox* comboBox, QString label, int lastIndex);
    //void updateDataManager(const QComboBox* comboBox);


    /**
      * @brief setFilterFieldAllData : fill comboBox with a QStringList items (database values)
      */
    void setFilterFieldAllData();

    /**
      * @brief setFiltersFieldsFromRexFilter : fill comboBox with a QStringList items (filtered values)
      */
    void setFiltersFieldsFromRexFilter(QComboBox* activeComboBox);

    //void setFilterFieldIndex();

    /**
      * @brief connectFilters : connect all signals/slots of the filter widget
      */
    void connectFilters();

private:
    Ui::RSFiltersManager *ui;

    QMap<QComboBox*, QString> m_comboTextMap;
    QHash<QComboBox*, QStringList> m_comboOriginalList;

    QMap<FiltersEnum,QPair<QString,QString>> m_filterNameValuePairMap;

    QMap<QComboBox*,FiltersEnum> m_comboFilterEnumMap;

    FiltersEnum getFilterEnum(const QComboBox*) const ;

    void updateFiltersCount();

};


#endif // RSFILTERSMANAGER_H
