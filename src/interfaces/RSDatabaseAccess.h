#ifndef RSDatabaseAccess_H
#define RSDatabaseAccess_H

#include <QObject>
#include <QMap>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QDateTime>
#include <QFileInfo>
#include <QVariant>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QString>

#include "RSDatabaseConfig.h"
#include "RSDataManager.h"
#include "Signaler.h"


class RSDatabaseAccess : public QObject
{

public:

    RSDatabaseAccess(QObject *parent = 0);
    virtual ~RSDatabaseAccess();
    static RSDatabaseAccess* Instance(QObject *parent = 0);
    static void deleteInstance();

    void loadSettings(const QString& fileName) ;
    void saveSettings(const QString& fileName) ;

    const QMap<int /*sensor code*/,QString /*Technology*/>& getTechnologyBySensorMap();

    const QMap<int /*sensor code*/,QString /*Sensor name*/> getSensorNameOfSensorCodeMap();
    void initMapsOfSensorNameCodeTechnology();

    virtual QList<int> getSensorCodeList(const QString& field, const QString& name);
    virtual bool initSensorFailureList(int sensorCode, const QDate& start, const QDate& end, int evtCode,QList<quint64>& failuresList) const;

    virtual QString getSensorUnicProperty(int mpCode, const QString& field);

    virtual  bool initSensorsByExperimentationMap(const QStringList& acqPointsCodeList, const QStringList& expList,QMap<QString, QStringList>& sensorsByExpMap);

    virtual bool isG6StructureOK() const;
    virtual bool isG7StructureOK() const;

    virtual bool open();
    virtual bool open(const QString& databaseName);
    virtual void close();

    virtual QList<double> getAcquisitionTimeList(const QDate& startDate, const QDate& endDate, int apNdCode,MeasPointType mpType, const QString &order = "ASC");

    virtual QList<double> getAcquisitionValueList(const QDate& startDate, const QDate& endDate, int apNdCode,MeasPointType mpType, const QString& order = "ASC");

    virtual  int getAcquisitionValueSize(const QDate& startDate, const QDate& endDate, int apNdCode,MeasPointType mpType);

    virtual QDateTime getAcquisitionRelativeFirstTime(const QDate& startDate, const QDate& endDate, int apNdCode,MeasPointType mpType, const QString& order = "ASC");

    virtual QDateTime getAcquisitionRelativeLastTime(const QDate& startDate, const QDate& endDate, int apNdCode,MeasPointType mpType);

    virtual QDateTime getStartDateTime() const;

    virtual void showDatabaseConfig();

    const QMap<int,QStringList>* experienceBySensorMap() ;


    void createDatasetTable();

    /**
     * @brief setDatasetTable call the three procedures setG6DatasetTable, setG7DatasetTable,setRexDatasetTable
     */
    void setDatasetTable();

    /**
     * @brief checkFilterQueryAndBuildRexFilterTable : check the query and  call setRexFilterTable
     * @return
     */
    bool checkFilterQueryAndBuildRexFilterTable(QString& filterQuery);

    /**
     * @brief getSensorNameList : return all the sensors in the table
     * @return
     */
    QStringList getSensorNameList();

    /**
     * @brief getSensorsDetailedInfoSet : return more detailed info (SensorInfos) about the sensors
     * @return
     */
    QList<SensorInfos> getSensorsDetailedInfoSet() const;

    /**
     * @brief getSensorNameList : return a QStringList of sensor (MP_NAME) where  the field matchs the criteria
     * @param field : the field for the criteria
     * @param Criteria : criteria on the field
     * @return QStringList of sensors
     */
    QStringList getSensorNameList(const QString& field, const QString& Criteria);

    int getSensorNameCode(const QString& name);
    QPair<int,MeasPointType> getSensorNameCodeAndType(const QString& name) const;

    /**
     * @brief getBrandNameList
     * @return :return all the brands in the table
     */
    QStringList getBrandNameList() const;
    QStringList getFilteredBrandNameList() const;

    QStringList getModelNameList() const;
    QStringList getFilteredModelNameList() const;

    QStringList getTechnologyNameList() const;
    QStringList getFilteredTechnologyNameList() const;

    QStringList getPhysicalMeasurementNameList() const ;
    QStringList getFilteredPhysicalMeasurementNameList() const ;

    QStringList getOutputSignalNameList() const ;
    QStringList getFilteredOutputSignalNameList()const ;

    QStringList getMeasurementRangeNameList() const;
    QStringList getFilteredMeasurementRangeNameList() const;

    QStringList getTheoricalAccuracyNameList() const;
    QStringList getFilteredTheoricalAccuracyNameList() const;

    QStringList getUnitNameList()const;
    QStringList getFilteredUnitNameList() const;

    QStringList getExperimentationNameList()const;
    QStringList getFilteredExperimentationNameList()const;


    bool deadEntitiesLoaded() const;
    bool nodesWithNoAstLoaded()const;


protected:
    bool m_g7dbStructureIsOk;
    bool m_g6dbStructureIsOk;

    QVariant loadG6DatabaseFile();
    QVariant loadG7DatabaseFile();
    void saveG7UserName() ;
    void saveG6UserName() ;

    QVariant loadDisplayOptions();
    void saveDisplayOptions() const;

    QVariant loadDeadEntitiesOption();
    QVariant loadNodesWithNoAst();
    void saveDeadEntitiesOption() const;
    void saveNodesWithNoSensorOption() const;

    void saveG6DatabaseFile() ;
    void saveG7DatabaseFile() ;

    void saveG7Password() ;
    void saveG6Password() ;

    QVariant loadG7UserName() ;
    QVariant loadG6UserName() ;

    QVariant loadG7Password() ;
    QVariant loadG6Password() ;

    QVariant loadG7Port() ;
    QVariant loadG6Port() ;

    void saveG7Port() ;
    void saveG6Port() ;

    void initExperienceBySensorMap();


private:
    static RSDatabaseAccess* m_instance;
    QMap<QString, QString> m_databaseDriverMap;
    QMap<QString, QString> m_databaseFullNameMap;
    QMap<QString, QString> m_databaseHostNameMap;
    QMap<QString, QString> m_databaseUserNameMap;
    QMap<QString, QString> m_databasePasswordMap;
    QMap<QString, QString> m_databasePortMap;

    QString m_g7UserName;
    QString m_g6UserName;

    QString m_g7Password;
    QString m_g6Password;

    QString m_g7Driver;
    QString m_g6Driver;

    QString m_g7Port;
    QString m_g6Port;

    QDateTime m_startDateTime;

    QSharedPointer<RSDatabaseConfig> m_databaseConfig;
    QString m_g6DatabaseFile;
    QString m_g7DatabaseFile;
    QString m_rexDatabaseFile;

    QMap<int,QStringList>* m_experienceBySensorMap;

    QMap <int, QPair<QDateTime,QDateTime>> m_apCodeMinMaxMap;

    QMap<int /*sensor code*/,QString /*Technology*/> m_technologyBySensorMap;
    QMap<int /*sensor code*/,QString /*Sensor name*/> m_sensorNameOfSensorCodeMap;

    bool m_loadNodesWithNoAst;
    bool m_loadDeadEntities;
    bool m_displayOptions;

    virtual void execQueryForLimitDateTime(const QDate& startDate, const QDate& endDate, int apNdCode,MeasPointType mpType, const QString& order, QDateTime &dateTimeLimit);

    virtual void exitRex();

    virtual void createObjects();

    virtual void dbQuery(const QString& query, const QString& databaseName = "REX");
    virtual void addDatabaseDriver(const QString& databaseName, const QString& databaseDriver);
    virtual void addDatabaseFullName(const QString& databaseName, const QString& databaseFullName);
    virtual void addDatabaseHostName(const QString& databaseName, const QString& hostName);
    virtual void addDatabaseUserName(const QString& databaseName, const QString& userName);
    virtual void addDatabasePassword(const QString& databaseName, const QString& password);
    virtual void addDatabasePort(const QString& databaseName, const QString& port);
    virtual void addDatabaseSql(const QString& databaseName);
    virtual void saveDatabaseFullName(const QString& databaseName);

    /**
     * @brief setG6DatasetTable_acqPoints : Create REX SQLITE Database from G6DATABASE (THE WHOLE DATAS)
     */
    void setG6DatasetTable_acqPoints();

    /**
     * @brief setG6DatasetTable_nodes : Create REX SQLITE Database from G6DATABASE (THE WHOLE DATAS)
     */
    void setG6DatasetTable_nodes();

    /**
     * @brief setG6DatasetTable_deadPoints : Create REX SQLITE Database from G6DATABASE (THE WHOLE DATAS)
     */
    void setG6DatasetTable_deadPoints();

    /**
     * @brief setG7DatasetTable : Create REX SQLITE Database from G7DATABASE (THE WHOLE DATAS)
     */
    void setG7DatasetTable();

    /**
     * @brief setRexDatasetTable create REXDATASET  from the Left join G6DATABASE.G7DATABASE
     * REXDATASET contains THE WHOLE DATAS
     */
    void setRexDatasetTable();
    void setSensorByExpDatasetTable();

    /**
     * @brief setRexFilterTable: Create REXFILTER from the current Filters in the RSFilterManager
     * REXFILTER is built from REXDATASET, according to the criterias in RSFilterManager UI
     */
    void setRexFilterTable(QString& filterQuery);

    /**
     * @brief getDataColumn : return the a no duplicate array of a column
     * @param table
     * @param field
     * @return the no duplicated values in the column as a QStringList
     */
    QStringList getDataColumn(const QString& table, const QString& field) const;


    bool checkG7DatabaseStructure(QSqlDatabase& db) ;
    bool checkG6DatabaseStructure(QSqlDatabase& db) ;
    bool checkDatabaseStructure(QSqlDatabase& db, const  QMap<QString/*Table*/,QStringList/*Fields*/>& tabMap ) const;

};

#endif // RSDatabaseAccess_H
