#include "RSDatabaseAccess.h" // big data // not save data

#include <QApplication>
#include "QtCore"
#include "RSDataManager.h"
#include "RSGlobalMethods.h"
#include "RSMessageView.h"
#include "RSSettingsManager.h"
#include "Signaler.h"
#include <RSLogger.h>

#include <QDebug>

namespace RexDatabaseSettings {
const QString DEFAULT_REX_VERSION = "0.0.1";
const QString DEFAULT_REX_NAME    = "RexSensors";

const QString DEFAULT_REX_CONFIG_FILE = "./RexSensors.ini";
const QString DEFAULT_REX_APP_DB_FILE = "./RexSensors.dat";
} // namespace RexDatabaseSettings

//! Database test : G6 and G7. user = SYSDBA, pwd = masterkey
RSDatabaseAccess* RSDatabaseAccess::m_instance = 0;

RSDatabaseAccess::RSDatabaseAccess(QObject* parent)
    : QObject(parent)
    , m_g7dbStructureIsOk(true)
    , m_g6dbStructureIsOk(true)
    , m_alwaysDisplayDatabaseConfigOnStart(false)
    , m_g7Driver("QFIREBIRD")
    , m_g6Driver("QFIREBIRD")
    , m_g7Port("3050")
    , m_g6Port("3050")
    , m_siCode(1)
    , m_dbCode(33813554)
    , m_g7SourceCode("1")
    , m_g7TagCategoryCode("2")
    , m_g7ContainerTypeCode("10")
    , m_experienceBySensorMap(0)
    , m_loadNodesWithNoAst(false)
    , m_loadDeadEntities(true)
{
    m_tempDir.setAutoRemove(true);
    loadSettings(QString());

    createObjects();
}

RSDatabaseAccess::~RSDatabaseAccess()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");
    RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
    saveSettings(QString());

    m_experienceBySensorMap->clear();
    delete m_experienceBySensorMap;

    close();
    RSLogger::instance()->info(Q_FUNC_INFO, "End");

    if(m_tempDir.remove()){
        RSLogger::instance()->info("Remove:", m_tempDir.path());
    } else {
        RSLogger::instance()->info("Fail to remove:", m_tempDir.path());
    }
}

RSDatabaseAccess* RSDatabaseAccess::Instance(QObject* parent)
{
    if(m_instance == 0) {
        m_instance = new RSDatabaseAccess(parent);
    }
    return m_instance;
}

void RSDatabaseAccess::deleteInstance()
{
    if(m_instance == 0) {
        delete m_instance;
        m_instance = 0;
    }
}

void RSDatabaseAccess::createObjects()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");
    m_databaseConfig.reset(new RSDatabaseConfig);

    m_alwaysDisplayDatabaseConfigOnStart = loadAlwaysDisplayDatabaseConfigOnStart().value<bool>();

    m_g6DatabaseFile = loadG6DatabaseFile().value<QString>();

    m_g7DatabaseFile = loadG7DatabaseFile().value<QString>();

    {
        QVariant data      = loadDeadEntitiesOption();
        m_loadDeadEntities = data.isValid() ? data.toBool() : false;
    }

    {
        QVariant data        = loadNodesWithNoAst();
        m_loadNodesWithNoAst = data.isValid() ? data.toBool() : false;
    }


    m_rexDatabaseFile = m_tempDir.path() + "/" + REX::DEFAULT_REX_APP_DB_FILE;
    qDebug().noquote() << "tempSqlite:" << m_rexDatabaseFile;

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSDatabaseAccess::dbQuery(const QString& query, const QString& databaseName)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);
    QSqlQuery querySql(databaseSql);
    bool execOk = false;

    RSLogger::instance()->info(Q_FUNC_INFO, "m_querySql = " + querySql.executedQuery());

    execOk = querySql.exec(query);

    if(execOk == false) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute dbQuery().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type()));

        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail");
        return;
    }

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSDatabaseAccess::addDatabaseDriver(const QString& databaseName, const QString& databaseDriver)
{
    RSLogger::instance()->info(Q_FUNC_INFO, databaseName + "\t : databaseDriver = " + databaseDriver);
    m_databaseDriverMap.insert(databaseName, databaseDriver);
}

void RSDatabaseAccess::addDatabaseFullName(const QString& databaseName, const QString& databaseFullName)
{
    RSLogger::instance()->info(Q_FUNC_INFO, databaseName + "\t : databaseFullName = " + databaseFullName);
    m_databaseFullNameMap.insert(databaseName, databaseFullName);
}

void RSDatabaseAccess::addDatabaseHostName(const QString& databaseName, const QString& hostName)
{
    RSLogger::instance()->info(Q_FUNC_INFO, databaseName + "\t : hostName = " + hostName);
    m_databaseHostNameMap.insert(databaseName, hostName);
}

void RSDatabaseAccess::addDatabaseUserName(const QString& databaseName, const QString& userName)
{
    RSLogger::instance()->info(Q_FUNC_INFO, databaseName + "\t : userName = " + userName);
    m_databaseUserNameMap.insert(databaseName, userName);
}

void RSDatabaseAccess::addDatabasePassword(const QString& databaseName, const QString& password)
{
    RSLogger::instance()->info(Q_FUNC_INFO, databaseName + "\t : password = " + password);
    m_databasePasswordMap.insert(databaseName, password);
}

void RSDatabaseAccess::addDatabasePort(const QString& databaseName, const QString& port)
{
    RSLogger::instance()->info(Q_FUNC_INFO, databaseName + "\t : port = " + port);
    m_databasePortMap.insert(databaseName, port);
}

void RSDatabaseAccess::addDatabaseSql(const QString& databaseName, bool utf8)
{
    QString m_databaseDriver   = m_databaseDriverMap.value(databaseName);
    QString m_databaseFullName = m_databaseFullNameMap.value(databaseName);
    QString m_databaseUserName = m_databaseUserNameMap.value(databaseName);
    QString m_databasePassword = m_databasePasswordMap.value(databaseName);
    QString databasePort       = m_databasePortMap.value(databaseName);

    RSLogger::instance()->info(Q_FUNC_INFO, "Add Database Driver :  " + m_databaseDriver);

    QSqlDatabase databaseSql = QSqlDatabase::addDatabase(m_databaseDriver, databaseName);

    RSLogger::instance()->info(Q_FUNC_INFO, "setDatabaseName. :  " + m_databaseFullName);
    databaseSql.setDatabaseName(m_databaseFullName);

    RSLogger::instance()->info(Q_FUNC_INFO, "m_databaseUserName. :  " + m_databaseUserName);
    databaseSql.setUserName(m_databaseUserName);

    RSLogger::instance()->info(Q_FUNC_INFO, "m_databasePassword. :  " + m_databasePassword);
    databaseSql.setPassword(m_databasePassword);

    RSLogger::instance()->info(Q_FUNC_INFO, QString("databasePort. %1").arg(databasePort));
    databaseSql.setPort(databasePort.toInt());
    if(!databaseName.contains("REX")){ //"REX", SQLITE ne supporte pas la suite. Mettre un paramètre additionnel
        if(utf8) {
            qDebug().noquote() << "    ----- ISC_DPB_LC_CTYPE=UTF8";
            databaseSql.setConnectOptions("ISC_DPB_LC_CTYPE=UTF8"); // base G7 par exemple, supporte UTF8
        } else {
            // qDebug().noquote() << "----- ISC_DPB_LC_CTYPE=ISO8859_1";
            // databaseSql.setConnectOptions("REX_ENCODING_MODE=LEGACY"); //Ancienne DB firebird
            // databaseSql.setConnectOptions("ISC_DPB_LC_CTYPE=ISO8859_1"); //Ancienne DB firebird


#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            databaseSql.setConnectOptions("ISC_DPB_LC_CTYPE=ISO8859_1"); //Ancienne DB firebird
#else
            databaseSql.setConnectOptions("REX_ENCODING_MODE=LEGACY"); //Ancienne DB firebird ATTENTION, nécessite le driver QFirebird pour 6.10 et +
            // databaseSql.setConnectOptions("ISC_DPB_LC_CTYPE=ISO8859_1"); //Ancienne DB firebird
#endif
            qDebug().noquote() << ">>> Connection options:" << databaseSql.connectOptions();
        }
    }

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSDatabaseAccess::saveDatabaseFullName(const QString& databaseName)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");
    QString m_databaseFullName = m_databaseFullNameMap.value(databaseName);

    if(databaseName == "G6") {
        RSDataManager::Instance()->setData("G6Database", m_databaseFullName);
    } else if(databaseName == "G7") {
        RSDataManager::Instance()->setData("G7Database", m_databaseFullName);
    }
    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

bool RSDatabaseAccess::open(const QString& databaseName, bool utf8)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start. db = " + databaseName);
    qDebug().noquote() << "Start. db = " << databaseName;

    bool isOpen        = false;
    QString dbFullName = m_databaseFullNameMap.value(databaseName);
    if(dbFullName.isEmpty()) {
        RSMessageView::Instance()->showData(QString("Please, set a valid : database, user name, password. (Case sensitive)"));
        return false;
    }

    RSMessageView::Instance()->showData(QString("Try to open database : %1").arg(databaseName));

    QString m_databaseDriver = m_databaseDriverMap.value(databaseName);

    isOpen = !dbFullName.isEmpty();

    if(!isOpen)
        RSMessageView::Instance()->showData(QString(tr("Empty database name : %1")).arg(databaseName));

    // Check drivers
    if(isOpen) {
        isOpen &= QSqlDatabase::drivers().contains(m_databaseDriver);
    }

    if(isOpen) {
        RSMessageView::Instance()->showData(QString(tr("Driver found : %1")).arg(m_databaseDriver));
    } else {
        RSMessageView::Instance()->showData(QString(tr("Driver NOT found : %1")).arg(m_databaseDriver));
    }

    // Open the database
    if(isOpen) {
        QSqlDatabase sqlDatabase;

        RSLogger::instance()->info(Q_FUNC_INFO, "\t init  QSqlDatabase::database() : " + databaseName);
        sqlDatabase = QSqlDatabase::database(databaseName);

        if(!sqlDatabase.isOpen()) {
            addDatabaseSql(databaseName, utf8);
            RSLogger::instance()->info(Q_FUNC_INFO, "saveDatabaseFullName");
            saveDatabaseFullName(databaseName);
        }

        sqlDatabase = QSqlDatabase::database(databaseName);

        if(!sqlDatabase.isOpen()) {
            qDebug().noquote() << ">> RSDatabaseAccess::open -" << databaseName << "- ConnectOptions:" << sqlDatabase.connectOptions();
            isOpen = sqlDatabase.open();
        }

        if(isOpen) {
            RSMessageView::Instance()->showData(QString("Succeeded to open database: %1.\t %2").arg(databaseName).arg(dbFullName));

        } else {
            RSMessageView::Instance()->showData(QString("Failed to open database: %1. \t%2 \n\t "
                                                        "Please, set a valid : database, user name, password. (Case sensitive)\n"
                                                        "%3")
                                                    .arg(databaseName)
                                                    .arg(dbFullName)
                                                    .arg(sqlDatabase.lastError().text()));
        }
    }

    return isOpen;
}

bool RSDatabaseAccess::open()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");
    bool isOpen = true;

    RSMessageView::Instance()->showData("======== DATABASES INITIALIZATION ============\n \tPlease patient...");
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    isOpen = true;
    addDatabaseDriver("G6", m_g6Driver);
    addDatabaseFullName("G6", m_g6DatabaseFile);
    addDatabaseUserName("G6", m_g6UserName);
    addDatabasePassword("G6", m_g6Password);
    addDatabasePort("G6", m_g6Port);
    isOpen &= open("G6", false);

    addDatabaseDriver("G7", m_g7Driver);
    addDatabaseFullName("G7", m_g7DatabaseFile);
    addDatabaseUserName("G7", m_g7UserName);
    addDatabasePassword("G7", m_g7Password);
    addDatabasePort("G7", m_g7Port);
    isOpen &= open("G7", true);

    addDatabaseDriver("REX", "QSQLITE");
    addDatabaseFullName("REX", m_rexDatabaseFile);
    addDatabaseHostName("REX", "");
    addDatabaseUserName("REX", "");
    addDatabasePassword("REX", "");
    addDatabasePort("REX", "");
    isOpen &= open("REX", false); //utf8 sera ignoré

    // Get the database paths
    if(!isOpen) {
        m_databaseConfig.data()->setMessage(tr("Please, set a valid  configration.."));

        m_databaseConfig.data()->setG6Login(m_g6DatabaseFile, m_g6UserName, m_g6Password);
        m_databaseConfig.data()->setG7Login(m_g7DatabaseFile, m_g7UserName, m_g7Password);
        m_databaseConfig.data()->setLoadDeadEntitiesOption(m_loadDeadEntities);
        m_databaseConfig.data()->setLoadNodeswithNoAstOption(m_loadNodesWithNoAst);
        m_databaseConfig.data()->setEnabledOptions(m_displayOptions);

        int m_result = m_databaseConfig->exec();

        if(m_result == QDialog::Accepted) {
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            m_g6DatabaseFile = m_databaseConfig->getG6DatabaseFile();
            m_g7DatabaseFile = m_databaseConfig->getG7DatabaseFile();

            m_g6UserName = m_databaseConfig->getG6UserName();
            m_g7UserName = m_databaseConfig->getG7UserName();

            m_g6Password = m_databaseConfig->getG6Pwd();
            m_g7Password = m_databaseConfig->getG7Pwd();

            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            // open();
        } else if(m_result == QDialog::Rejected) {
            emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error Database Config"),
                                                          tr("RexSensors cannot open database.<br/>"
                                                             "<font style='color: gray'>"
                                                             "1 - Check Firebird DB Driver<br/>"
                                                             "2 - Check SQLite DB Driver <br/>"
                                                             "3 - Check G6 & G7 database locations <br/>"
                                                             "4 - Check G6 & G7 user name and password"
                                                             "</font>"));
            emit Signaler::instance()->signal_closeAppli();
        }
    }

    RSDataManager::Instance()->setData("G6DatabaseFile", m_g6DatabaseFile);
    RSDataManager::Instance()->setData("G7DatabaseFile", m_g7DatabaseFile);

    m_databaseConfig.reset();

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
    return isOpen;
}

void RSDatabaseAccess::close()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");
    foreach(QString m_connectionName, QSqlDatabase::connectionNames()) {
        QSqlDatabase m_databaseSql = QSqlDatabase::database(m_connectionName);
        bool m_isOpen              = m_databaseSql.isOpen();

        if(m_isOpen == true) {
            m_databaseSql.close();
        }
    }

    RSGlobalMethods::Instance()->removeFile(m_rexDatabaseFile);

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

QList<double> RSDatabaseAccess::getAcquisitionTimeList(const QDate& startDate, const QDate& endDate, int apNdCode, MeasPointType mpType,
                                                       const QString& order)
{
    QString m_databaseName     = "G6";
    QSqlDatabase m_databaseSql = QSqlDatabase::database(m_databaseName);
    QSqlQuery querySql(m_databaseSql);
    QString field;
    QString strQuery;
    bool execOk = false;

    //TODO: binding
    QString startFormat = startDate.toString("MM-dd-yyyy");
    QString endFormat   = endDate.toString("MM-dd-yyyy");

    if(mpType == MeasPointType::AcqPoint) {
        strQuery = QString("select AV_ACQUISITIONDT as IDATA from ACQVALUE av "
                           "where av.SI_CODE = :si_code "
                           "and av.DB_CODE = :db_code "
                           "and av.AP_CODE = :apnd_code "
                           "and av.AV_ACQUISITIONDT >= :dt_start "
                           "and av.AV_ACQUISITIONDT < :dt_end "
                           "and av.AV_STATUS = 0 "
                           "order by av.SI_CODE %1, av.DB_CODE %1, av.AP_CODE %1, av.AV_ACQUISITIONDT %1").arg(order);
    } else if(mpType == MeasPointType::Node) {
        strQuery = QString("select NR_NODEDT as IDATA from NODERESULT NR "
                           "where NR.SI_CODE = :si_code "
                           "and NR.DB_CODE = :db_code "
                           "and NR.ND_CODE = :apnd_code "
                           "and NR.NR_NODEDT >= :dt_start "
                           "and NR.NR_NODEDT < :dt_end "
                           "and NR.NR_STATUS = 0 "
                           "order by NR.SI_CODE %1, NR.DB_CODE %1, NR.ND_CODE %1, NR.NR_NODEDT %1").arg(order);
    } else {
        //TODO: ajouter message dans le logguer + QMessageBox
        return QList<double>();
    }

    qDebug().noquote() << "QList<double> RSDatabaseAccess::getAcquisitionTimeList\n" << strQuery;

    querySql.prepare(strQuery);
    querySql.bindValue(":si_code", m_siCode);
    querySql.bindValue(":db_code", m_dbCode);
    querySql.bindValue(":apnd_code", apNdCode);
    querySql.bindValue(":dt_start", startDate);
    querySql.bindValue(":dt_end", endDate);

    execOk = querySql.exec();

    if(!execOk) {
        RSLogger::instance()->info(Q_FUNC_INFO, "Failed to execute query : \n " + strQuery);
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(m_databaseName),
                                                      tr("%1 database cannot execute getAcquisitionTimeList().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(m_databaseName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type()));

        return QList<double>();
    }

    int fieldIndex = querySql.record().indexOf("IDATA");
    QList<double> dataList;
    while(querySql.next()) {
        // The date of acquisition
        // QString acqDateTime = querySql.value(fieldIndex).toString();
        // QDateTime stepDataTime = QDateTime::fromString(acqDateTime, "yyyy-MM-ddThh:mm:ss");

        //new
        QDateTime stepDataTime = querySql.value(fieldIndex).toDateTime();

        // First value
        if(querySql.at() == 0) {
            m_startDateTime = stepDataTime;
            dataList.push_back(0);
        } else {
            // We get the elapsed time from m_startDateTime to acqDateTime
            double elapsedTimeMsec = (double) m_startDateTime.msecsTo(stepDataTime);

            // We push gapTimeMsec in the list (relative time)
            dataList.push_back(elapsedTimeMsec);
        }
    }

    return dataList;
}

QList<double> RSDatabaseAccess::getAcquisitionValueList(const QDate& startDate, const QDate& endDate, int apNdCode, MeasPointType mpType,
                                                        const QString& order)
{
    QString m_databaseName     = "G6";
    QSqlDatabase m_databaseSql = QSqlDatabase::database(m_databaseName);
    QSqlQuery querySql(m_databaseSql);
    QList<double> dataList;
    QString field;
    bool execOk           = true;

    //TODO: binding
    QString startFormat = startDate.toString("MM-dd-yyyy");
    QString endFormat   = endDate.toString("MM-dd-yyyy");
    QString strQuery;

    //TODO: virer SI_, DB_ codé en dur
    if(mpType == MeasPointType::AcqPoint) {
        strQuery = QString("select AV_INGVALUE as IDATA from ACQVALUE av "
                           "where av.SI_CODE = :si_code "
                           "and av.DB_CODE = :db_code "
                           "and av.AP_CODE = :apnd_code "
                           "and av.AV_ACQUISITIONDT >= :dt_start "
                           "and av.AV_ACQUISITIONDT < :dt_end "
                           "and av.AV_STATUS = 0 "
                           "order by av.SI_CODE %1, av.DB_CODE %1, av.AP_CODE %1, av.AV_ACQUISITIONDT %1").arg(order);

    } else if(mpType == MeasPointType::Node) {
        strQuery = QString("select NR_CALCVALUE as IDATA from NODERESULT NR "
                           "where NR.SI_CODE = :si_code "
                           "and NR.DB_CODE = :db_code "
                           "and NR.ND_CODE = :apnd_code "
                           "and NR.NR_NODEDT >= :dt_start "
                           "and NR.NR_NODEDT < :dt_end "
                           "and NR.NR_STATUS = 0 "
                           "order by NR.SI_CODE %1, NR.DB_CODE %1, NR.ND_CODE %1, NR.NR_NODEDT %1").arg(order);
    } else {
        //TODO: logger + message
        return QList<double>();
    }

    querySql.prepare(strQuery);
    querySql.bindValue(":si_code", m_siCode);
    querySql.bindValue(":db_code", m_dbCode);
    querySql.bindValue(":apnd_code", apNdCode);
    querySql.bindValue(":dt_start", startDate);
    querySql.bindValue(":dt_end", endDate);

    RSLogger::instance()->info(Q_FUNC_INFO, "Try to execute query : \n" + strQuery);
    execOk = querySql.exec();

    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(m_databaseName),
                                                      tr("%1 database cannot execute getAcquisitionValueList().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(m_databaseName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type()));

        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail to execute query");
        return QList<double>();
    }

    int dataNo = querySql.record().indexOf("IDATA");

    bool isOk = false;
    while(querySql.next()) {
        // QString stepData = querySql.value(dataNo).toString();
        // double data      = stepData.toDouble();
        double stepValue = querySql.value(dataNo).toDouble(&isOk);
        if(!isOk){
            //TODO: mettre 0, NAN ?
        }
        dataList.push_back(stepValue);
    }

    RSLogger::instance()->info(Q_FUNC_INFO, QString("End. Succeed. dataListSize = %1").arg(dataList.count()));
    return dataList;
}

int RSDatabaseAccess::getAcquisitionValueSize(const QDate& startDate, const QDate& endDate, int apNdCode, MeasPointType mpType)
{
    QString databaseName     = "G6";
    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);
    QSqlQuery querySql(databaseSql);
    int data  = 0;
    bool execOk  = false;

    QString strQuery;

    if(mpType == MeasPointType::AcqPoint) {
        strQuery = QString("select count(*) as IDATA from ACQVALUE av "
                           "where av.SI_CODE = :si_code "
                           "and av.DB_CODE = :db_code "
                           "and av.AP_CODE = :apnd_code "
                           "and av.AV_ACQUISITIONDT >= :dt_start "
                           "and av.AV_ACQUISITIONDT < :dt_end "
                           "and av.AV_STATUS = 0");
    } else if(mpType == MeasPointType::Node) {
        strQuery = QString("select count(*) as IDATA from NODERESULT NR "
                           "where NR.SI_CODE = :si_code "
                           "and NR.DB_CODE =  :db_code "
                           "and NR.ND_CODE = :apnd_code "
                           "and NR.NR_NODEDT >= :dt_start "
                           "and NR.NR_NODEDT < :dt_end "
                           "and NR.NR_STATUS = 0");
    } else {
        //TODO: logger
        return 0;
    }

    querySql.prepare(strQuery);
    querySql.bindValue(":si_code", m_siCode);
    querySql.bindValue(":db_code", m_dbCode);
    querySql.bindValue(":apnd_code", apNdCode);
    querySql.bindValue(":dt_start", startDate);
    querySql.bindValue(":dt_end", endDate);

    execOk = querySql.exec();

    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute getAcquisitionValueSize().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type()));
        return 0;
    }

    bool isOk = false;
    int m_dataNo = querySql.record().indexOf("IDATA");
    while(querySql.next()) {
        // QString m_format = m_querySql.value(m_dataNo).toString();
        // data           = m_format.toInt();
        data = querySql.value(m_dataNo).toInt(&isOk);
        if(!isOk){
            //TODO: mettre 0, NAN ?
        }
    }

    return data;
}

QDateTime RSDatabaseAccess::getAcquisitionRelativeFirstTime(const QDate& startDate, const QDate& endDate, int apNdCode,
                                                            MeasPointType mpType, const QString& order)
{
    QDateTime dateTime;

    execQueryForLimitDateTime(startDate, endDate, apNdCode, mpType, order, dateTime);

    return dateTime;
}

void RSDatabaseAccess::execQueryForLimitDateTime(const QDate& startDate, const QDate& endDate, int apNdCode, MeasPointType mpType,
                                                 const QString& order, QDateTime& dateTimeLimit)
{
    RSLogger::instance()->info(Q_FUNC_INFO, QString("Start"));

    //! Run Query on G6 Database
    QString m_databaseName     = "G6";
    QSqlDatabase m_databaseSql = QSqlDatabase::database(m_databaseName);
    QSqlQuery querySql(m_databaseSql);

    // int dbCode     = 33813554;

    //TODO: binding
    QString format = "dd.MM.yyyy";
    QString strQuery;

    QString strStartDate = startDate.toString(format);
    QString strEndDate   = endDate.toString(format);
    RSLogger::instance()->info(Q_FUNC_INFO, QString("startDate = %1 endDate = %2 apNdCode = %3 MeasPointType = %4")
                                                .arg(strStartDate)
                                                .arg(strEndDate)
                                                .arg(apNdCode)
                                                .arg(mpType));

    QString field;

    if(mpType == MeasPointType::AcqPoint) {
        field = "AV_ACQUISITIONDT"; // beoin plus tard
        strQuery = QString("select AV_ACQUISITIONDT from ACQVALUE av "
                           "where av.SI_CODE = :si_code "
                           "and av.DB_CODE = :db_code "
                           "and av.AP_CODE = :apnd_code "
                           "and av.AV_ACQUISITIONDT >= :dt_start "
                           "and av.AV_ACQUISITIONDT < :dt_end "
                           "and av.AV_STATUS = 0 order by AV_ACQUISITIONDT %1 ")
                       .arg(order);
    } else if(mpType == MeasPointType::Node) {
        field = "NR_NODEDT";
        strQuery = QString("select NR_NODEDT from NODERESULT NR "
                           "where NR.SI_CODE = :si_code "
                           "and NR.DB_CODE = :db_code "
                           "and NR.ND_CODE = :apnd_code "
                           "and NR.NR_NODEDT >= :dt_start "
                           "and NR.NR_NODEDT < :dt_end "
                           "and NR.NR_STATUS = 0 order by NR_NODEDT %1 ")
                       .arg(order);
    } else {
        QString msg = QString("Unknown .MeasPointType: %1").arg(static_cast<int>(mpType));
        RSLogger::instance()->info(Q_FUNC_INFO, msg);
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(m_databaseName), msg);
    }


    // querySql.prepare(strQuery);
    RSLogger::instance()->info(Q_FUNC_INFO, "Try to prepare Query ");
    if(!querySql.prepare(strQuery)) {
        RSLogger::instance()->info(Q_FUNC_INFO, QString("End. Failed to prepare query : %1").arg(querySql.executedQuery()));
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(m_databaseName),
                                                      tr("%1 prepare Query Failed\n"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3 \n %4")
                                                          .arg(m_databaseName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type())
                                                          .arg(strQuery));
        dateTimeLimit = QDateTime();
    }

    querySql.bindValue(":si_code", m_siCode);
    querySql.bindValue(":db_code", m_dbCode);
    querySql.bindValue(":apnd_code", apNdCode);
    querySql.bindValue(":dt_start", startDate);
    querySql.bindValue(":dt_end", endDate);

    RSLogger::instance()->info(Q_FUNC_INFO, "Try to execute Query : " + strQuery);
    if(!querySql.exec()) {
        RSLogger::instance()->info(Q_FUNC_INFO, QString("End. m_exec == false Executed query  = %1").arg(strQuery));
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(m_databaseName),
                                                      tr("%1 database cannot execute getAcquisitionRelativeFirstTime().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(m_databaseName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type()));

        dateTimeLimit = QDateTime();
    }

    RSLogger::instance()->info(Q_FUNC_INFO, QString("Executed Query : \n %1").arg(querySql.executedQuery()));

    int dataNo = querySql.record().indexOf(field);

    QDateTime minDateTime;
    if(querySql.next()){
        minDateTime = querySql.value(dataNo).value<QDateTime>();
    }

    RSLogger::instance()->info(Q_FUNC_INFO, QString("End \t dateTime = %1").arg(minDateTime.toString()));

    dateTimeLimit = minDateTime;
}

QDateTime RSDatabaseAccess::getAcquisitionRelativeLastTime(const QDate& startDate, const QDate& endDate, int apNdCode, MeasPointType mpType)
{
    RSLogger::instance()->info(Q_FUNC_INFO, QString("Start"));

    QDateTime dateTime;

    RSLogger::instance()->info(Q_FUNC_INFO, QString("startDate = %1 endDate = %2 apNdCode = %3 MeasPointType = %4")
                                                .arg(startDate.toString())
                                                .arg(endDate.toString())
                                                .arg(apNdCode)
                                                .arg(mpType));

    execQueryForLimitDateTime(startDate, endDate, apNdCode, mpType, "DESC", dateTime);

    RSLogger::instance()->info(Q_FUNC_INFO, QString("End \t dateTime = %1").arg(dateTime.toString()));

    return dateTime;
}

QDateTime RSDatabaseAccess::getStartDateTime() const
{
    return m_startDateTime;
}

void RSDatabaseAccess::loadSettings(const QString& fileName)
{
    Q_UNUSED(fileName)

    RSLogger::instance()->info(Q_FUNC_INFO, "Start");
    RSDataManager::Instance()->setData("RexDatabase.G6Database", loadG6DatabaseFile());
    RSDataManager::Instance()->setData("RexDatabase.G7Database", loadG7DatabaseFile());

    m_alwaysDisplayDatabaseConfigOnStart  = loadAlwaysDisplayDatabaseConfigOnStart().toBool();

    m_g7DatabaseFile = loadG7DatabaseFile().toString();

    m_g7Port = loadG7Port().toString();

    m_g7UserName = loadG7UserName().toString();

    m_g7Password = loadG7Password().toString();

    m_g6DatabaseFile = loadG6DatabaseFile().toString();

    m_g6Port = loadG6Port().toString();

    m_g6UserName = loadG6UserName().toString();

    m_g6Password = loadG6Password().toString();

    m_displayOptions = loadDisplayOptions().toBool();

    m_loadNodesWithNoAst = loadNodesWithNoAst().toBool();

    m_loadDeadEntities = loadDeadEntitiesOption().toBool();

    m_siCode = loadSICode();

    m_dbCode = loadDBCode();

    m_g7SourceCode = loadG7SourceCode().toString();

    m_g7TagCategoryCode = loadG7TagCategoryCode().toString();

    m_g7ContainerTypeCode = loadG7ContainerTypeCode().toString();


    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSDatabaseAccess::saveSettings(const QString& fileName)
{
    Q_UNUSED(fileName)
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    saveG6DatabaseFile();
    saveG6Port();
    saveG6UserName();
    saveG6Password();

    saveG7DatabaseFile();
    saveG7Port();
    saveG7UserName();
    saveG7Password();
    saveDeadEntitiesOption();
    saveNodesWithNoSensorOption();

    saveSICode();
    saveDBCode();
    saveG7SourceCode();
    saveG7TagCategoryCode();
    saveG7ContainerTypeCode();

    //! We do not save for the moment. Use the file
    saveDisplayOptions();

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

QVariant RSDatabaseAccess::loadAlwaysDisplayDatabaseConfigOnStart() {
    QString id       = "RexDatabase";
    QString key      = "RexDatabase.AlwaysDisplayDatabaseConfigOnStart";
    QVariant defaultVal = QVariant(false);

    QVariant data = RSGlobalMethods::Instance()->loadData(id, key, defaultVal);

    RSLogger::instance()->info(Q_FUNC_INFO, "AlwaysDisplayDatabaseConfigOnStart = " + data.toString());

    return data;
}

QVariant RSDatabaseAccess::loadG6DatabaseFile()
{
    QString id       = "RexDatabase";
    QString key      = "RexDatabase.G6Database";
    QVariant defaultVal = "";

    QVariant data = RSGlobalMethods::Instance()->loadData(id, key, defaultVal);

    RSLogger::instance()->info(Q_FUNC_INFO, "G6 Path = " + data.value<QString>());

    return data;
}

QVariant RSDatabaseAccess::loadG7DatabaseFile()
{
    QString id       = "RexDatabase";
    QString key      = "RexDatabase.G7Database";
    QVariant defaultVal = "";

    QVariant data = RSGlobalMethods::Instance()->loadData(id, key, defaultVal);
    RSLogger::instance()->info(Q_FUNC_INFO, "G7 Path = " + data.value<QString>());

    return data;
}

QVariant RSDatabaseAccess::loadDisplayOptions()
{
    QString id       = "RexDatabase";
    QString key      = "RexDatabase.DisplayOptions";
    QVariant defaultVal = QVariant(false);

    QVariant data = RSGlobalMethods::Instance()->loadData(id, key, defaultVal);
    RSLogger::instance()->info(Q_FUNC_INFO, "DisplayOptions = " + data.toString());

    // return data; //On ignore la config
    return QVariant(true);
}

void RSDatabaseAccess::saveDisplayOptions() const
{
    QString m_id  = "RexDatabase";
    QString m_key = "RexDatabase.DisplayOptions";

    //! For to FALSE at the closing
    QVariant data = QVariant(m_displayOptions);
    RSLogger::instance()->info(Q_FUNC_INFO, "Save DisplayOptions = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

QVariant RSDatabaseAccess::loadDeadEntitiesOption()
{
    QString id       = "RexDatabase";
    QString key      = "RexDatabase.LoadDeadEntities";
    QVariant defaultVal = QVariant(true);

    QVariant data = RSGlobalMethods::Instance()->loadData(id, key, defaultVal);
    RSLogger::instance()->info(Q_FUNC_INFO, "LoadDeadEntities = " + data.toString());

    return data;
}

QVariant RSDatabaseAccess::loadNodesWithNoAst()
{
    QString id       = "RexDatabase";
    QString key      = "RexDatabase.loadNodesWithNoAst";
    QVariant defaultVal = QVariant(false);

    QVariant data = RSGlobalMethods::Instance()->loadData(id, key, defaultVal);
    RSLogger::instance()->info(Q_FUNC_INFO, "loadNodesWithNoAst = " + data.toString());

    return data;
}
void RSDatabaseAccess::saveDeadEntitiesOption() const
{
    QString id  = "RexDatabase";
    QString key = "RexDatabase.LoadDeadEntities";
    QVariant data = QVariant(m_loadDeadEntities);
    RSLogger::instance()->info(Q_FUNC_INFO, "Save G7 Path = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(id, key, data);
}

void RSDatabaseAccess::saveNodesWithNoSensorOption() const
{
    QString id  = "RexDatabase";
    QString key = "RexDatabase.loadNodesWithNoAst";
    QVariant data = QVariant(m_loadNodesWithNoAst);

    RSLogger::instance()->info(Q_FUNC_INFO, "Save G7 Path = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(id, key, data);
}

void RSDatabaseAccess::saveG6DatabaseFile()
{
    QString id  = "RexDatabase";
    QString key = "RexDatabase.G6Database";
    QVariant data = m_g6DatabaseFile; // RSDataManager::Instance()->getData(m_key);

    RSLogger::instance()->info(Q_FUNC_INFO, "Save G6 Path = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(id, key, data);
}

//[RexSensors]
void RSDatabaseAccess::saveG7DatabaseFile()
{
    QString id  = "RexDatabase";
    QString key = "RexDatabase.G7Database";
    QVariant data = m_g7DatabaseFile; // RSDataManager::Instance()->getData(m_key);

    RSLogger::instance()->info(Q_FUNC_INFO, "Save G7 Path = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(id, key, data);
}

bool RSDatabaseAccess::initSensorFailureList(int mpCode, const QDate& start, const QDate& end, int evtCode,
                                             QList<quint64>& failuresList) const
{
    RSLogger::instance()->info(Q_FUNC_INFO, QString("Start"));
    failuresList.clear();
    //! Run Query on G7 Database
    QString dbName     = "G7";
    QSqlDatabase dbSql = QSqlDatabase::database(dbName);
    QSqlQuery* querySql;

    querySql = new QSqlQuery(dbSql);

    // Pour le logger
    QString format = "dd.MM.yyyy";
    QString strStart = start.toString(format);
    QString strEnd   = end.toString(format);
    RSLogger::instance()->info(Q_FUNC_INFO, QString("start = %1 end = %2 mpCode = %3").arg(strStart).arg(strEnd).arg(mpCode));

    QString strQuery = QString("SELECT startdate,enddate,datediff(second, cast(startdate as date),  cast(enddate as date)) as dT FROM"
                               "("
                               " SELECT "
                               " CASE   "
                               "   WHEN T_JOURNAL.JRN_ENDDATETIME > :dt_end OR T_JOURNAL.JRN_ENDDATETIME IS NULL THEN :dt_end "
                               "   ELSE T_JOURNAL.JRN_ENDDATETIME  "
                               " END   as endDate,"
                               " CASE"
                               "   WHEN T_JOURNAL.JRN_CREATEDATETIME < :dt_start OR T_JOURNAL.JRN_CREATEDATETIME IS NULL THEN :dt_start "
                               "   ELSE T_JOURNAL.JRN_CREATEDATETIME  "
                               " END   as startDate "
                               " FROM  T_ENTITIES INNER JOIN  T_ENTITYJOURNAL  ON T_ENTITIES.ENT_CODE = T_ENTITYJOURNAL.ENT_CODE  "
                               " LEFT JOIN T_JOURNAL on T_JOURNAL.JRN_CODE = T_ENTITYJOURNAL.JRN_CODE "
                               " LEFT JOIN T_EVENT on T_JOURNAL.EVT_CODE = T_EVENT.EVT_CODE "
                               " WHERE (T_ENTITIES.ENTITY_ID = :mp_Code "
                               "   AND  T_JOURNAL.EVT_CODE = :evt_code "
                               "   AND (T_JOURNAL.JRN_ENDDATETIME >  :dt_start OR T_JOURNAL.JRN_ENDDATETIME IS NULL)"
                               "   AND  (T_JOURNAL.JRN_CREATEDATETIME < :dt_end  OR T_JOURNAL.JRN_CREATEDATETIME IS NULL)"
                               "   AND  T_ENTITIES.CNT_CODE = :cnt_code))");

    querySql->prepare(strQuery);
    querySql->bindValue(":dt_start", start);
    querySql->bindValue(":dt_end", end);
    querySql->bindValue(":mp_code", mpCode);
    querySql->bindValue(":cnt_code", m_g7ContainerTypeCode);
    querySql->bindValue(":evt_code", evtCode);


    RSLogger::instance()->info(Q_FUNC_INFO, "Try to execute Query : \n" + strQuery);
    if(!querySql->exec()) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(dbName),
                                                      tr("%1 initSensorFailureList() Failed to execute Query.<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3 \n query : %4")
                                                          .arg(dbName)
                                                          .arg(querySql->lastError().databaseText())
                                                          .arg(querySql->lastError().type())
                                                          .arg(strQuery));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Query failed");
        return false;
    }

    int dTCol = querySql->record().indexOf("dT");

    // QList<double> failuresList;
    while(querySql->next()) {
        qint64 dT = querySql->value(dTCol).toInt();
        failuresList.push_back(dT);
    }

    if(failuresList.isEmpty()) {
        RSLogger::instance()->info(Q_FUNC_INFO, QString("failuresList.isEmpty for mpCode = %1").arg(mpCode));
    }

    querySql->clear();
    querySql->finish();
    delete querySql;
    dbSql.close();

    RSLogger::instance()->info(Q_FUNC_INFO, "End. Query suceeded");

    return true;
}

void RSDatabaseAccess::initMapsOfSensorNameCodeTechnology()
{
    RSLogger::instance()->info(Q_FUNC_INFO, QString("Start"));
    m_technologyBySensorMap.clear();
    m_sensorNameOfSensorCodeMap.clear();

    QString dbName     = "REX";
    QSqlDatabase dbSql = QSqlDatabase::database(dbName);
    QSqlQuery querySql(dbSql);

    QString strQuery = "select MP_CODE,MP_NAME, AST_TECHNOLOGY FROM REXFILTER";

    RSLogger::instance()->info(Q_FUNC_INFO, "Try to execute Query : " + strQuery);
    if(!querySql.exec(strQuery)) {
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Failed to execute Query");
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(dbName),
                                                      tr("%1 database cannot execute initTechnologyBySensorMap().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(dbName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type()));
    }

    int sensorField = querySql.record().indexOf("MP_CODE");
    int technoField = querySql.record().indexOf("AST_TECHNOLOGY");
    int nameField   = querySql.record().indexOf("MP_NAME");
    while(querySql.next()) {
        int mpCode                          = querySql.value(sensorField).value<int>();
        m_technologyBySensorMap[mpCode]     = querySql.value(technoField).toString();
        m_sensorNameOfSensorCodeMap[mpCode] = querySql.value(nameField).toString();
    }

    RSLogger::instance()->info(Q_FUNC_INFO, QString("End"));
}

const QMap<int /*sensor code*/, QString /*Technology*/>& RSDatabaseAccess::getTechnologyBySensorMap()
{
    if(m_technologyBySensorMap.isEmpty())
        initMapsOfSensorNameCodeTechnology();
    return m_technologyBySensorMap;
}

const QMap<int /*sensor code*/, QString /*Sensor name*/> RSDatabaseAccess::getSensorNameOfSensorCodeMap()
{
    // if(m_sensorNameOfSensorCodeMap.isEmpty())
    initMapsOfSensorNameCodeTechnology();
    return m_sensorNameOfSensorCodeMap;
}

QList<int> RSDatabaseAccess::getSensorCodeList(const QString& field, const QString& name)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName     = "REX";
    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);
    QSqlQuery querySql(databaseSql);
    QList<int> dataList;

    QString strQuery = QString("select distinct MP_CODE IDATA "
                               "from REXFILTER "
                               "where MP_CODE <> '' "
                               "and MP_CODE is not null "
                               "and :field = :name "
                               "order by MP_NAME");
    //TODO: vérifier prepare, c'est du SqLite
    // .arg(field)
    // .arg(name);
    querySql.prepare(strQuery);
    querySql.bindValue(":field", field);
    querySql.bindValue(":name", name);

    bool exec = querySql.exec();

    RSLogger::instance()->info(Q_FUNC_INFO, QString("Exec query : %1 ").arg(strQuery));
    if(!exec) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute getSensorNameList().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type()));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail to execute Query : " + strQuery);
        return QList<int>();
    }

    int m_dataNo = querySql.record().indexOf("IDATA");

    RSLogger::instance()->info(Q_FUNC_INFO, QString("Gathe the data"));
    while(querySql.next()) {
        int data = querySql.value(m_dataNo).value<int>();
        dataList.push_back(data);
    }

    RSLogger::instance()->info(Q_FUNC_INFO, QString("End. Found %1 sensors").arg(dataList.count()));

    return dataList;
}

QString RSDatabaseAccess::getSensorUnicProperty(int mpCode, const QString& field)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName     = "REX";
    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);
    QSqlQuery querySql(databaseSql);

    QString strQuery = QString("select distinct %1 from REXFILTER where MP_CODE = :mp_code ").arg(field);

    querySql.prepare(strQuery);
    querySql.bindValue(":mp_code", mpCode);

    bool execOk = querySql.exec();

    RSLogger::instance()->info(Q_FUNC_INFO, QString("Exec query : %1 ").arg(strQuery));
    if(execOk == false) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute getSensorUnicProperty().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type()));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail to execute Query");
        return QString();
    }

    QString techno;
    if(querySql.first()) {
        int val = querySql.value(0).toInt();
        techno  = QString::number(val);
    }

    RSLogger::instance()->info(Q_FUNC_INFO, QString("End. Found  techno : %1").arg(techno));

    return techno;
}

// Get the measure points associated to the sensors
//INUTILISE ?
bool RSDatabaseAccess::initSensorsByExperimentationMap(const QStringList& mpCodeList, const QStringList& expList,
                                                       QMap<QString, QStringList>& map)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start. mpCodeList : \n" + mpCodeList.join(','));

    map.clear();

    QString dbName     = "G7";
    QSqlDatabase dbSql = QSqlDatabase::database(dbName);
    QSqlQuery sqlQuery(dbSql);

    // Ancienne version
    // // TAG_NAME = nom expérience
    // QString strQuery = QString("SELECT ENT_NAME, T_ENTITIES.ENT_CODE, TAG_NAME, T_ENTITYTAG.TAG_CODE"
    //                            "  FROM T_ENTITIES"
    //                            "  RIGHT  JOIN T_ENTITYTAG ON T_ENTITYTAG.ENT_CODE  = T_ENTITIES.ENT_CODE"
    //                            "  LEFT   JOIN T_TAG ON T_ENTITYTAG.TAG_CODE  = T_TAG.TAG_CODE"
    //                            "  LEFT   JOIN T_TAGCATEGORIES ON T_TAG.TCT_CODE  = T_TAGCATEGORIES.TCT_CODE"
    //                            "  WHERE T_ENTITIES.CNT_CODE = %3"
    //                            "  AND T_ENTITIES.SRC_CODE = %4"
    //                            "  AND T_TAGCATEGORIES.TCT_CODE = %5"
    //                            "  AND T_ENTITIES.ENTITY_ID IN(%1)"
    //                            "  AND T_TAG.TAG_NAME IN('%2')")
    //                        .arg(mpCodeList.join(","))
    //                        .arg(expList.join("','"))
    //                        .arg(m_g7ContainerTypeCode)
    //                        .arg(m_g7SourceCode)
    //                        .arg(m_g7TagCategoryCode);


    // bool execOk = sqlQuery.exec(strQuery);



    //Nouvelle version avec prepare
    // Génère '?,?,?,...' (autant que de codes)

    QStringList ent_placeholders_list;
    for (int i = 0; i < mpCodeList.size(); ++i)
        ent_placeholders_list << "?";
    QString ent_placeholders = ent_placeholders_list.join(",");

    QStringList tag_placeholders_list;
    for (int i = 0; i < expList.size(); ++i)
        tag_placeholders_list << "?";
    QString tag_placeholders = tag_placeholders_list.join(",");

    QString strQuery =
        "SELECT ENT_NAME, T_ENTITIES.ENT_CODE, TAG_NAME, T_ENTITYTAG.TAG_CODE "
        "FROM T_ENTITIES "
        "RIGHT JOIN T_ENTITYTAG ON T_ENTITYTAG.ENT_CODE = T_ENTITIES.ENT_CODE "
        "LEFT JOIN T_TAG ON T_ENTITYTAG.TAG_CODE = T_TAG.TAG_CODE "
        "LEFT JOIN T_TAGCATEGORIES ON T_TAG.TCT_CODE = T_TAGCATEGORIES.TCT_CODE "
        "WHERE T_ENTITIES.CNT_CODE = ? "
        "AND T_ENTITIES.SRC_CODE = ? "
        "AND T_TAGCATEGORIES.TCT_CODE = ? "
        "AND T_ENTITIES.ENTITY_ID IN (" + ent_placeholders + ") "
        "AND T_TAG.TAG_NAME IN (" + tag_placeholders + ")";

    sqlQuery.prepare(strQuery);
    sqlQuery.addBindValue(m_g7ContainerTypeCode);
    sqlQuery.addBindValue(m_g7SourceCode);
    sqlQuery.addBindValue(m_g7TagCategoryCode);
    for (const QString &code : mpCodeList)
        sqlQuery.addBindValue(code);
    for (const QString &tag : expList)
        sqlQuery.addBindValue(tag);

    bool execOk = sqlQuery.exec();

    RSLogger::instance()->info(Q_FUNC_INFO, QString("Try to execute query : \n %1 ").arg(strQuery));
    if(execOk == false) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(dbName),
                                                      tr("%1 database cannot execute initSensorsByExperimentationMap().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(dbName)
                                                          .arg(sqlQuery.lastError().databaseText())
                                                          .arg(sqlQuery.lastError().type()));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail to execute Query");
        return false;
    }

    int expCol        = sqlQuery.record().indexOf("TAG_NAME");
    int sensorNameCol = sqlQuery.record().indexOf("ENT_NAME");
    while(sqlQuery.next()) {
        QString expName    = sqlQuery.value(expCol).toString();
        QString sensorName = sqlQuery.value(sensorNameCol).toString();
        map[expName].append(sensorName);
    }

    QString msg;
    Q_FOREACH(const QString& key, map.keys())
        msg.append("%1 = %2 sensors \t").arg(key).arg(map[key].count());

    RSLogger::instance()->info(Q_FUNC_INFO, "End. .Query succeeded. \n" + msg);
    return true;
}

bool RSDatabaseAccess::isG6StructureOK() const
{
    return m_g6dbStructureIsOk;
}

bool RSDatabaseAccess::isG7StructureOK() const
{
    return m_g7dbStructureIsOk;
}

void RSDatabaseAccess::showDatabaseConfig()
{
    if(!m_alwaysDisplayDatabaseConfigOnStart){
        return;
    }

    m_databaseConfig.data()->setG6Path(m_g6DatabaseFile);
    m_databaseConfig.data()->setG7Path(m_g7DatabaseFile);

    if(m_g7DatabaseFile.isEmpty() || m_g6DatabaseFile.isEmpty())
        m_databaseConfig.data()->setMessage(tr("Please, set a valid : database, user name, password. (Case sensitive)"));
    else
        m_databaseConfig.data()->setMessage(tr("Press Ok to continue"));

    m_databaseConfig.data()->setG6Login(m_g6DatabaseFile, m_g6UserName, m_g6Password);
    m_databaseConfig.data()->setG7Login(m_g7DatabaseFile, m_g7UserName, m_g7Password);
    m_databaseConfig.data()->adjustSize();
    m_databaseConfig.data()->setLoadDeadEntitiesOption(m_loadDeadEntities);
    m_databaseConfig.data()->setLoadNodeswithNoAstOption(m_loadNodesWithNoAst);
    m_databaseConfig.data()->setEnabledOptions(m_displayOptions);

    if(m_databaseConfig->exec() == QDialog::Accepted) {
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        m_g6DatabaseFile = m_databaseConfig->getG6DatabaseFile();
        m_g7DatabaseFile = m_databaseConfig->getG7DatabaseFile();

        m_g6UserName = m_databaseConfig->getG6UserName();
        m_g7UserName = m_databaseConfig->getG7UserName();

        m_g6Password = m_databaseConfig->getG6Pwd();
        m_g7Password = m_databaseConfig->getG7Pwd();

        m_loadDeadEntities   = m_databaseConfig.data()->loadDeadEntities();
        m_loadNodesWithNoAst = m_databaseConfig.data()->loadNodesWithNoAst();

        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    } else {
        exitRex();
    }
}

void RSDatabaseAccess::exitRex()
{
    emit Signaler::instance()->signal_closeAppli();
}

void RSDatabaseAccess::saveG7UserName()
{
    QString m_id  = "RexDatabase";
    QString m_key = "RexDatabase.G7UserName";
    QVariant data = m_g7UserName;

    RSLogger::instance()->info(Q_FUNC_INFO, "Save m_g7UserName = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

void RSDatabaseAccess::saveG6UserName()
{
    QString m_id  = "RexDatabase";
    QString m_key = "RexDatabase.G6UserName";
    QVariant data = m_g6UserName;

    RSLogger::instance()->info(Q_FUNC_INFO, "Save m_g6UserName = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

void RSDatabaseAccess::saveG7Password()
{
    QString m_id  = "RexDatabase";
    QString m_key = "RexDatabase.G7Password";
    QVariant data = m_g7Password;

    RSLogger::instance()->info(Q_FUNC_INFO, "Save m_g7Password = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

void RSDatabaseAccess::saveG6Password()
{
    QString m_id  = "RexDatabase";
    QString m_key = "RexDatabase.G6Password";
    QVariant data = m_g6Password;

    RSLogger::instance()->info(Q_FUNC_INFO, "Save m_g6Password = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

QVariant RSDatabaseAccess::loadG7UserName()
{
    QString m_id       = "RexDatabase";
    QString m_key      = "RexDatabase.G7UserName";
    QVariant m_default = ""; // "SYSDBA";

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);

    RSLogger::instance()->info(Q_FUNC_INFO, "G7UserName = " + data.value<QString>());

    return data;
}

QVariant RSDatabaseAccess::loadG6UserName()
{
    QString m_id       = "RexDatabase";
    QString m_key      = "RexDatabase.G6UserName";
    QVariant m_default = ""; // COSMOSUSER";

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);

    RSLogger::instance()->info(Q_FUNC_INFO, "G6UserName = " + data.value<QString>());

    return data;
}

QVariant RSDatabaseAccess::loadG7Password()
{
    QString m_id       = "RexDatabase";
    QString m_key      = "RexDatabase.G7Password";
    QVariant m_default = ""; // "masterkey";

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);

    RSLogger::instance()->info(Q_FUNC_INFO, "G7Password = " + data.value<QString>());

    return data;
}

QVariant RSDatabaseAccess::loadG6Password()
{
    QString m_id       = "RexDatabase";
    QString m_key      = "RexDatabase.G6Password";
    QVariant m_default = ""; // cosmosus";

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);

    RSLogger::instance()->info(Q_FUNC_INFO, "G6Password = " + data.value<QString>());

    return data;
}

QVariant RSDatabaseAccess::loadG7Port()
{
    QString m_id       = "RexDatabase";
    QString m_key      = "RexDatabase.G7Port";
    QVariant m_default = "3050";

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);

    RSLogger::instance()->info(Q_FUNC_INFO, "G7Port = " + data.value<QString>());

    return data;
}

QVariant RSDatabaseAccess::loadG6Port()
{
    QString m_id       = "RexDatabase";
    QString m_key      = "RexDatabase.G6Port";
    QVariant m_default = "3050";

    QVariant data = RSGlobalMethods::Instance()->loadData(m_id, m_key, m_default);

    RSLogger::instance()->info(Q_FUNC_INFO, "G6Port = " + data.value<QString>());

    return data;
}

void RSDatabaseAccess::saveG7Port()
{
    QString m_id  = "RexDatabase";
    QString m_key = "RexDatabase.G7Port";
    QVariant data = m_g7Port;

    RSLogger::instance()->info(Q_FUNC_INFO, "Save G7Port = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

void RSDatabaseAccess::saveG6Port()
{
    QString m_id  = "RexDatabase";
    QString m_key = "RexDatabase.G6Port";
    QVariant data = m_g6Port;

    RSLogger::instance()->info(Q_FUNC_INFO, "Save m_g6Port = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(m_id, m_key, data);
}

int RSDatabaseAccess::loadSICode() {
    QString id       = "RexDatabase";
    QString key      = "RexDatabase.SICode";
    int defaultVal = 1;

    QVariant data = RSGlobalMethods::Instance()->loadData(id, key, defaultVal);
    bool isOk = false;
    int val = data.toInt(&isOk);
    if(!isOk){
        val = -1;
    }

    RSLogger::instance()->info(Q_FUNC_INFO, QString("SICode = %1").arg(val));
    return val;
}

void RSDatabaseAccess::saveSICode() {
    QString id  = "RexDatabase";
    QString key = "RexDatabase.SICode";
    QVariant data = m_siCode;

    RSLogger::instance()->info(Q_FUNC_INFO, "Save m_siCode = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(id, key, data);
}

int RSDatabaseAccess::loadDBCode()
{
    QString id       = "RexDatabase";
    QString key      = "RexDatabase.DBCode";
    int defaultVal = 33813554;

    QVariant data = RSGlobalMethods::Instance()->loadData(id, key, defaultVal);
    bool isOk = false;
    int val = data.toInt(&isOk);
    if(!isOk){
        val = -1;
    }

    RSLogger::instance()->info(Q_FUNC_INFO, QString("DBCode = %1").arg(val));

    return val;
}

void RSDatabaseAccess::saveDBCode() {
    QString id  = "RexDatabase";
    QString key = "RexDatabase.DBCode";
    QVariant data = m_dbCode;

    RSLogger::instance()->info(Q_FUNC_INFO, "Save m_dbCode = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(id, key, data);
}

// SRC_CODE (table T_SOURCE, 1 = GW)
QVariant RSDatabaseAccess::loadG7SourceCode() {
    QString id       = "RexDatabase";
    QString key      = "RexDatabase.G7SourceCode";
    QVariant defaultVal = "1";

    QVariant data = RSGlobalMethods::Instance()->loadData(id, key, defaultVal);

    RSLogger::instance()->info(Q_FUNC_INFO, "G7SourceCode = " + data.value<QString>());

    return data;
}

void RSDatabaseAccess::saveG7SourceCode() {
    QString id  = "RexDatabase";
    QString key = "RexDatabase.G7SourceCode";
    QVariant data = m_g7SourceCode;

    RSLogger::instance()->info(Q_FUNC_INFO, "Save m_g7SourceCode = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(id, key, data);
}

// TCT_CODE: (Tag category code) Experimentation = 2
QVariant RSDatabaseAccess::loadG7TagCategoryCode() {
    QString id       = "RexDatabase";
    QString key      = "RexDatabase.G7TagCategoryCode";
    QVariant defaultVal = "2";

    QVariant data = RSGlobalMethods::Instance()->loadData(id, key, defaultVal);

    RSLogger::instance()->info(Q_FUNC_INFO, "G7TagCategoryCode = " + data.value<QString>());

    return data;
}

void RSDatabaseAccess::saveG7TagCategoryCode() {
    QString id  = "RexDatabase";
    QString key = "RexDatabase.G7TagCategoryCode";
    QVariant data = m_g7TagCategoryCode;

    RSLogger::instance()->info(Q_FUNC_INFO, "Save m_g7TagCategoryCode = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(id, key, data);
}

// CNT_CODE : (container type code) MeasurePoint = 10
QVariant RSDatabaseAccess::loadG7ContainerTypeCode() {
    QString id       = "RexDatabase";
    QString key      = "RexDatabase.G7ContainerTypeCode";
    QVariant defaultVal = "10";

    QVariant data = RSGlobalMethods::Instance()->loadData(id, key, defaultVal);

    RSLogger::instance()->info(Q_FUNC_INFO, "G7ContainerTypeCode = " + data.value<QString>());

    return data;
}

void RSDatabaseAccess::saveG7ContainerTypeCode() {
    QString id  = "RexDatabase";
    QString key = "RexDatabase.G7ContainerTypeCode";
    QVariant data = m_g7ContainerTypeCode;

    RSLogger::instance()->info(Q_FUNC_INFO, "Save m_g7ContainerTypeCode = " + data.value<QString>());
    RSGlobalMethods::Instance()->saveData(id, key, data);
}




const QMap<int, QStringList>* RSDatabaseAccess::experienceBySensorMap()
{
    if(!m_experienceBySensorMap || m_experienceBySensorMap->isEmpty())
        initExperienceBySensorMap();

    return m_experienceBySensorMap;
}

void RSDatabaseAccess::initExperienceBySensorMap()
{
    if(!m_experienceBySensorMap)
        m_experienceBySensorMap = new QMap<int, QStringList>();

    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName = "G7";
    bool execOk = false;

    // Get data from G7
    QSqlQuery querySqlG7(QSqlDatabase::database("G7"));

    //TODO: virer ce qui est codé en dur : container type (CNT_CODE 10 = MPs) et SRC_CODE (1 = GW) et T_TAGCATEGORIES
    //! brief DB_CODE = 33813554 is to be transfered in a config file
    QString strQuery = QString(" SELECT distinct  T_ENTITIES.ENTITY_ID,T_ENTITIES.ENT_CODE, TAG_NAME"
                               " FROM T_ENTITIES"
                               " RIGHT  JOIN T_ENTITYTAG ON T_ENTITYTAG.ENT_CODE  = T_ENTITIES.ENT_CODE"
                               " LEFT   JOIN T_TAG ON T_ENTITYTAG.TAG_CODE  = T_TAG.TAG_CODE"
                               " LEFT   JOIN T_TAGCATEGORIES ON T_TAG.TCT_CODE  = T_TAGCATEGORIES.TCT_CODE"
                               " WHERE T_ENTITIES.CNT_CODE = %1"
                               "   AND T_ENTITIES.SRC_CODE = %2"
                               "   AND T_TAGCATEGORIES.TCT_CODE = %3").arg(m_g7ContainerTypeCode).arg(m_g7SourceCode).arg(m_g7TagCategoryCode);
    execOk = querySqlG7.exec(strQuery);
    if(!execOk)
        RSMessageView::Instance()->showData("REX: failed select on G7");

    RSLogger::instance()->info(Q_FUNC_INFO, "Try to execute query:\n " + strQuery);
    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute setSensorByExpDatasetTable().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3 \n query : %4")
                                                          .arg(databaseName)
                                                          .arg(querySqlG7.lastError().databaseText())
                                                          .arg(querySqlG7.lastError().type())
                                                          .arg(strQuery));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail to execute query");
        return;
    }

    int entCodeCol = querySqlG7.record().indexOf("ENTITY_ID");
    int tagNameCol = querySqlG7.record().indexOf("TAG_NAME");

    QMap<int, QStringList>::iterator it;
    while(querySqlG7.next()) {
        QVariant entCode = querySqlG7.value(entCodeCol);
        QVariant tagName = querySqlG7.value(tagNameCol).toString();
        if(entCode.isNull() || tagName.isNull())
            continue;

        int mpCode = entCode.toInt();

        if((it = m_experienceBySensorMap->find(mpCode)) != m_experienceBySensorMap->end()) {
            it.value().append(tagName.toString());
        } else {
            m_experienceBySensorMap->insert(mpCode, QStringList() << tagName.toString());
        }
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSDatabaseAccess::setG6DatasetTable_acqPoints()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName     = "G6";
    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);

    if(!checkG6DatabaseStructure(databaseSql)) {
        m_g6dbStructureIsOk = false;
        return;
    }

    QSqlQuery querySql(databaseSql);
    bool isExecuted = false;

    QSqlDatabase databaseSqlRex = QSqlDatabase::database("REX");
    QSqlQuery querySqlRex(databaseSqlRex);

    bool queryRexOneOnly = true;

    QString strQuery = QString(" select distinct "
                               " mp.MP_CODE, "
                               " mp.MP_NAME, "
                               " ap.AP_CODE, "
                               " ap.AP_NAME, "
                               " ast.AST_BRAND, "
                               " ast.AST_MODEL, "
                               " ast.AST_TECHNOLOGY, "
                               " ast.AST_RANGE, "
                               " ast.AST_THEORETICALACCURACY, "
                               " ast.AST_UNIT,"
                               " ast.AST_PHYSICALMEASUREMENT,"
                               " ast.AST_OUTPUTSIGNAL   "
                               " from MEASUREPOINT mp "
                               " left join ACQPOINT ap "
                               " on ap.SI_CODE = mp.SI_CODE "
                               " and ap.DB_CODE = mp.DB_CODE "
                               " and ap.AP_CODE = mp.AP_CODE "
                               " left join ACQSENSORTYPE ast "
                               " on ast.SI_CODE = ap.SI_CODE "
                               " and ast.AST_CODE = ap.AST_CODE "
                               " where mp.SI_CODE = %1 "
                               " and mp.DB_CODE = %2 "
                               " and mp.MP_CODE is not null"
                               " and mp.AP_CODE is not null").arg(m_siCode).arg(m_dbCode);

    // qDebug().noquote() << "RSDatabaseAccess::setG6DatasetTable_acqPoints\n";

    isExecuted  = querySql.exec(strQuery);



    RSLogger::instance()->info(Q_FUNC_INFO, "Try to execute query:\n " + strQuery);
    if(!isExecuted) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute setG6DatasetTable_acqPoints().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3 \n query : %4")
                                                          .arg(databaseName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type())
                                                          .arg(strQuery));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail to execute query");
        return;
    }

    int mpCodeNo               = querySql.record().indexOf("MP_CODE");
    int mpNameNo               = querySql.record().indexOf("MP_NAME");
    int apCodeNo               = querySql.record().indexOf("AP_CODE");
    int apNameNo               = querySql.record().indexOf("AP_NAME");
    int astBrandNo             = querySql.record().indexOf("AST_BRAND");
    int astModelNo             = querySql.record().indexOf("AST_MODEL");
    int astTechnologyNo        = querySql.record().indexOf("AST_TECHNOLOGY");
    int astRangeNo             = querySql.record().indexOf("AST_RANGE");
    int astTheoricalAccuracyNo = querySql.record().indexOf("AST_THEORETICALACCURACY");
    int astUnitNo              = querySql.record().indexOf("AST_UNIT");
    int physicalMeasurementCol   = querySql.record().indexOf("AST_PHYSICALMEASUREMENT");
    int outputSignalCol          = querySql.record().indexOf("AST_OUTPUTSIGNAL");

    QString queryStringRex = "insert into G6DATASET ("
                               "MP_CODE, "
                               "MP_NAME, "
                               "AP_CODE, "
                               "AP_NAME, "
                               "AST_BRAND, "
                               "AST_MODEL, "
                               "AST_TECHNOLOGY, "
                               "AST_RANGE, "
                               "AST_THEORICALACCURACY, "
                               "AST_UNIT, "
                               "AST_PHYSICALMEASUREMENT,"
                               "AST_OUTPUTSIGNAL"
                               ") values ";

    while(querySql.next()) {
        QString mpCodeData               = querySql.value(mpCodeNo).toString();
        QString mpNameData               = querySql.value(mpNameNo).toString();
        QString apCodeData               = querySql.value(apCodeNo).toString();
        QString apNameData               = querySql.value(apNameNo).toString();
        QString astBrandData             = querySql.value(astBrandNo).toString();
        QString astModelData             = querySql.value(astModelNo).toString();

        // // 1. Récupérer les octets bruts
        // QByteArray rawData = querySql.value(astModelNo).toByteArray();
        // qDebug() << "\n---->Octets bruts avec OCTETS:" << rawData.toHex(' ');
        // QString textLatin1 = QString::fromLatin1(rawData);
        // qDebug() << "---->Texte Latin1:" << textLatin1;
        // qDebug() << "---->Texte direct:" << astModelData;
        QString astTechnologyData        = querySql.value(astTechnologyNo).toString();
        QString astRangeData             = querySql.value(astRangeNo).toString();
        QString astTheoricalAccuracyData = querySql.value(astTheoricalAccuracyNo).toString();
        QString astUnitData              = querySql.value(astUnitNo).toString();
        QString physicalMeasurement        = querySql.value(physicalMeasurementCol).toString();
        QString outputSignal               = querySql.value(outputSignalCol).toString();

        if(queryRexOneOnly == true) {
            queryStringRex += QString("('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10','%11','%12')")
                                    .arg(mpCodeData)
                                    .arg(mpNameData)
                                    .arg(apCodeData)
                                    .arg(apNameData)
                                    .arg(astBrandData)
                                    .arg(astModelData)
                                    .arg(astTechnologyData)
                                    .arg(astRangeData)
                                    .arg(astTheoricalAccuracyData)
                                    .arg(astUnitData)
                                    .arg(physicalMeasurement)
                                    .arg(outputSignal);

            queryRexOneOnly = false;
        } else {
            queryStringRex += QString(",('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10','%11','%12')")
                                    .arg(mpCodeData)
                                    .arg(mpNameData)
                                    .arg(apCodeData)
                                    .arg(apNameData)
                                    .arg(astBrandData)
                                    .arg(astModelData)
                                    .arg(astTechnologyData)
                                    .arg(astRangeData)
                                    .arg(astTheoricalAccuracyData)
                                    .arg(astUnitData)
                                    .arg(physicalMeasurement)
                                    .arg(outputSignal);
        }
    }

    isExecuted = querySqlRex.exec(queryStringRex);

    RSLogger::instance()->info(Q_FUNC_INFO, "Fill REX Database");
    if(!isExecuted) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("setG6DatasetTable_acqPoints Failed to insert data in G6DATASET table ().<br/>"
                                                         "ErrorText : %1<br/>"
                                                         "ErrorType : %2")
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type()));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail m_querySqlRex : ");
        return;
    }
    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSDatabaseAccess::setG6DatasetTable_nodes()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName     = "G6";
    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);

    if(!checkG6DatabaseStructure(databaseSql)) {
        m_g6dbStructureIsOk = false;
        return;
    }

    QSqlQuery querySql(databaseSql);
    bool execOk = false;

    QSqlDatabase databaseSqlRex = QSqlDatabase::database("REX");
    QSqlQuery querySqlRex(databaseSqlRex);

    bool queryRexOneOnly = true;

    QString strQuery = QString(" select distinct "
                               " mp.MP_CODE, "
                               " mp.MP_NAME, "
                               " MP.ND_CODE, "
                               " ND.ND_NAME, "
                               " ast.AST_BRAND, "
                               " ast.AST_MODEL, "
                               " ast.AST_TECHNOLOGY, "
                               " ast.AST_RANGE, "
                               " ast.AST_THEORETICALACCURACY, "
                               " ast.AST_UNIT,"
                               " ast.AST_PHYSICALMEASUREMENT,"
                               " ast.AST_OUTPUTSIGNAL   "
                               " from MEASUREPOINT mp "
                               " left join NODE ND "
                               " on MP.SI_CODE = ND.SI_CODE "
                               " and MP.DB_CODE = ND.DB_CODE "
                               " and MP.ND_CODE = ND.ND_CODE "
                               " left join ACQSENSORTYPE AST "
                               " on ND.SI_CODE = AST.SI_CODE "
                               " and ND.AST_CODE = AST.AST_CODE "
                               " where mp.SI_CODE = %1 "
                               " and mp.DB_CODE = %2 "
                               " and mp.MP_CODE is NOT NULL"
                               " and mp.ND_CODE is NOT NULL"
                               " and mp.AP_CODE is NULL").arg(m_siCode).arg(m_dbCode);

    //! Case where we want to skip nodes without any sensor type
    if(m_loadNodesWithNoAst == false) {
        strQuery.append(" AND ND.AST_CODE IS NOT NULL ");
    }

    execOk = querySql.exec(strQuery);

    RSLogger::instance()->info(Q_FUNC_INFO, "Try to execute query:\n " + strQuery);
    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute setG6DatasetTable_nodes().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3 \n query : %4")
                                                          .arg(databaseName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type())
                                                          .arg(strQuery));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail to execute query");
        return;
    }

    int mpCodeNo               = querySql.record().indexOf("MP_CODE");
    int mpNameNo               = querySql.record().indexOf("MP_NAME");
    int apCodeNo               = querySql.record().indexOf("ND_CODE");
    int apNameNo               = querySql.record().indexOf("ND_NAME");
    int astBrandNo             = querySql.record().indexOf("AST_BRAND");
    int astModelNo             = querySql.record().indexOf("AST_MODEL");
    int astTechnologyNo        = querySql.record().indexOf("AST_TECHNOLOGY");
    int astRangeNo             = querySql.record().indexOf("AST_RANGE");
    int astTheoricalAccuracyNo = querySql.record().indexOf("AST_THEORETICALACCURACY");
    int astUnitNo              = querySql.record().indexOf("AST_UNIT");
    int physicalMeasurementCol   = querySql.record().indexOf("AST_PHYSICALMEASUREMENT");
    int outputSignalCol          = querySql.record().indexOf("AST_OUTPUTSIGNAL");

    QString queryStringRex = "insert into G6DATASET ("
                               "MP_CODE, "
                               "MP_NAME, "
                               "ND_CODE, "
                               "ND_NAME, "
                               "AST_BRAND, "
                               "AST_MODEL, "
                               "AST_TECHNOLOGY, "
                               "AST_RANGE, "
                               "AST_THEORICALACCURACY, "
                               "AST_UNIT, "
                               "AST_PHYSICALMEASUREMENT,"
                               "AST_OUTPUTSIGNAL"
                               ") values ";

    while(querySql.next()) {
        QString mpCodeData               = querySql.value(mpCodeNo).toString();
        QString mpNameData               = querySql.value(mpNameNo).toString();
        QString apCodeData               = querySql.value(apCodeNo).toString();
        QString apNameData               = querySql.value(apNameNo).toString();
        QString astBrandData             = querySql.value(astBrandNo).toString();
        QString astModelData             = querySql.value(astModelNo).toString();
        QString astTechnologyData        = querySql.value(astTechnologyNo).toString();
        QString astRangeData             = querySql.value(astRangeNo).toString();
        QString astTheoricalAccuracyData = querySql.value(astTheoricalAccuracyNo).toString();
        QString astUnitData              = querySql.value(astUnitNo).toString();
        QString physicalMeasurement        = querySql.value(physicalMeasurementCol).toString();
        QString outputSignal               = querySql.value(outputSignalCol).toString();

        if(queryRexOneOnly == true) {
            queryStringRex += QString("('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10','%11','%12')")
                                    .arg(mpCodeData)
                                    .arg(mpNameData)
                                    .arg(apCodeData)
                                    .arg(apNameData)
                                    .arg(astBrandData)
                                    .arg(astModelData)
                                    .arg(astTechnologyData)
                                    .arg(astRangeData)
                                    .arg(astTheoricalAccuracyData)
                                    .arg(astUnitData)
                                    .arg(physicalMeasurement)
                                    .arg(outputSignal);

            queryRexOneOnly = false;
        } else {
            queryStringRex += QString(",('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10','%11','%12')")
                                    .arg(mpCodeData)
                                    .arg(mpNameData)
                                    .arg(apCodeData)
                                    .arg(apNameData)
                                    .arg(astBrandData)
                                    .arg(astModelData)
                                    .arg(astTechnologyData)
                                    .arg(astRangeData)
                                    .arg(astTheoricalAccuracyData)
                                    .arg(astUnitData)
                                    .arg(physicalMeasurement)
                                    .arg(outputSignal);
        }
    }

    execOk = querySqlRex.exec(queryStringRex);

    RSLogger::instance()->info(Q_FUNC_INFO, "Fill REX Database");
    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("setG6DatasetTable_acqPoints Failed to insert data in G6DATASET table ().<br/>"
                                                         "ErrorText : %1<br/>"
                                                         "ErrorType : %2")
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type()));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail m_querySqlRex : ");
        return;
    }
    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSDatabaseAccess::setG6DatasetTable_deadPoints()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName     = "G6";
    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);

    if(!checkG6DatabaseStructure(databaseSql)) {
        m_g6dbStructureIsOk = false;
        return;
    }

    QSqlQuery querySql(databaseSql);
    bool execOk = false;

    QSqlDatabase databaseSqlRex = QSqlDatabase::database("REX");
    QSqlQuery querySqlRex(databaseSqlRex);

    bool queryRexOneOnly = true;

    //! brief DB_CODE = 33813554 is to be transfered in a config file
    QString strQuery = QString(" SELECT distinct  MP.MP_CODE, MP.MP_NAME FROM MEASUREPOINT MP "
                               " where MP.SI_CODE = %1  "
                               " and MP.DB_CODE = %2  "
                               " and MP.AP_CODE  IS null "
                               " and mp.ND_CODE  IS null").arg(m_siCode).arg(m_dbCode);

    execOk = querySql.exec(strQuery);

    RSLogger::instance()->info(Q_FUNC_INFO, "Try to execute query:\n " + strQuery);
    if(!execOk) {
        emit Signaler::instance()
            ->signal_emitMessage(QMessageBox::Critical, "red", tr("setG6DatasetTable_deadPoints(). Error %1 Database").arg(databaseName),
                                 tr("%1 Failed to execute ().<br/> ErrorText : %2<br/> ErrorType : %3 \n Query : %4")
                                     .arg(databaseName)
                                     .arg(querySql.lastError().databaseText())
                                     .arg(querySql.lastError().type())
                                     .arg(strQuery));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail to execute query");
        return;
    }

    int mpCodeNo = querySql.record().indexOf("MP_CODE");
    int mpNameNo = querySql.record().indexOf("MP_NAME");

    QString queryStringRex = "insert into G6DATASET (MP_CODE, MP_NAME ) VALUES ";

    while(querySql.next()) {
        QString m_mpCodeData = querySql.value(mpCodeNo).toString();
        QString m_mpNameData = querySql.value(mpNameNo).toString();

        if(queryRexOneOnly == true) {
            queryStringRex += QString("('%1', '%2')").arg(m_mpCodeData).arg(m_mpNameData);

            queryRexOneOnly = false;
        } else {
            queryStringRex += QString(",('%1', '%2')").arg(m_mpCodeData).arg(m_mpNameData);
        }
    }

    execOk = querySqlRex.exec(queryStringRex);

    RSLogger::instance()->info(Q_FUNC_INFO, "Fill REX Database");
    if(!execOk) {
        emit Signaler::instance()
            ->signal_emitMessage(QMessageBox::Critical, "red", tr("setG6DatasetTable_deadPoints(). Error %1 Database").arg(databaseName),
                                 tr("%1 Failed to execute ().<br/> ErrorText : %2<br/> ErrorType : %3 \n Query : %4")
                                     .arg(databaseName)
                                     .arg(querySql.lastError().databaseText())
                                     .arg(querySql.lastError().type())
                                     .arg(strQuery));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail m_querySqlRex : ");
        return;
    }
    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSDatabaseAccess::setG7DatasetTable()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName     = "G7";
    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);

    if(!checkG7DatabaseStructure(databaseSql)) {
        return;
    }

    QSqlQuery querySql(databaseSql);
    bool execOk = true;

    QSqlDatabase databaseSqlRex = QSqlDatabase::database("REX");
    QSqlQuery querySqlRex(databaseSqlRex);

    QString queryStringRex = "insert into G7DATASET ("
                               "ENT_CODE, "
                               "ENTITY_ID, "
                               "ENT_NAME, "
                               "TAG_NAME"
                               ") values ";

    bool queryRexOneOnly = true;


    QString strQuery = QString("select distinct e.ENT_CODE, e.ENTITY_ID, e.ENT_NAME, "
                               "t.TAG_NAME from T_ENTITIES e "
                               "left join T_ENTITYTAG et "
                               "on et.ENT_CODE = e.ENT_CODE "
                               "left join T_TAG t "
                               "on t.TAG_CODE = et.TAG_CODE "
                               "left join T_TAGCATEGORIES tc "
                               "on tc.TCT_CODE = t.TCT_CODE "
                               "where e.CNT_CODE = %1 "
                               "and e.SRC_CODE = %2 "
                               "and tc.TCT_CODE = %3").arg(m_g7ContainerTypeCode).arg(m_g7SourceCode).arg(m_g7TagCategoryCode);

    execOk = querySql.exec(strQuery);

    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute setG7DatasetTable().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type()));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. m_exec == false Fail.. Query is : " + querySqlRex.executedQuery());
        return;
    }

    int entCodeNo  = querySql.record().indexOf("ENT_CODE");
    int entityIdNo = querySql.record().indexOf("ENTITY_ID");
    int entNameNo  = querySql.record().indexOf("ENT_NAME");
    int tagNameNo  = querySql.record().indexOf("TAG_NAME");

    while(querySql.next()) {
        QString entCodeData  = querySql.value(entCodeNo).toString();
        QString entityIdData = querySql.value(entityIdNo).toString();
        QString entNameData  = querySql.value(entNameNo).toString();
        QString tagNameData  = querySql.value(tagNameNo).toString();

        if(queryRexOneOnly == true) {
            queryStringRex += QString("('%1', '%2', '%3', '%4')")
                                    .arg(entCodeData)
                                    .arg(entityIdData)
                                    .arg(entNameData)
                                    .arg(tagNameData);
            queryRexOneOnly = false;
        } else {
            queryStringRex += QString(",('%1', '%2', '%3', '%4')")
                                    .arg(entCodeData)
                                    .arg(entityIdData)
                                    .arg(entNameData)
                                    .arg(tagNameData);
        }
    }

    execOk = querySqlRex.exec(queryStringRex);

    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute setG7DatasetTable().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(querySqlRex.lastError().databaseText())
                                                          .arg(querySqlRex.lastError().type()));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. m_exec == false Fail.. Query is : " + querySqlRex.executedQuery());
        return;
    }

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSDatabaseAccess::setSensorByExpDatasetTable()
{
    initExperienceBySensorMap();

    RSMessageView::Instance()->showData("setSensorByExpDatasetTable");
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName = "G7";

    bool execOk = true;

    // Get data from G7
    RSMessageView::Instance()->showData("Get data from G7");
    QSqlQuery querySqlG7(QSqlDatabase::database("G7"));

    QString strQuery = QString(" SELECT distinct ENT_NAME, T_ENTITIES.ENT_CODE, TAG_NAME"
                               " FROM T_ENTITIES"
                               " RIGHT  JOIN T_ENTITYTAG ON T_ENTITYTAG.ENT_CODE  = T_ENTITIES.ENT_CODE"
                               " LEFT   JOIN T_TAG ON T_ENTITYTAG.TAG_CODE  = T_TAG.TAG_CODE"
                               " LEFT   JOIN T_TAGCATEGORIES ON T_TAG.TCT_CODE  = T_TAGCATEGORIES.TCT_CODE"
                               " WHERE T_ENTITIES.CNT_CODE = %1"
                               "   AND T_ENTITIES.SRC_CODE = %2"
                               "   AND T_TAGCATEGORIES.TCT_CODE = %3").arg(m_g7ContainerTypeCode).arg(m_g7SourceCode).arg(m_g7TagCategoryCode);

    execOk  = querySqlG7.exec(strQuery);
    if(!execOk)
        RSMessageView::Instance()->showData("REX: failed select on G7");

    RSLogger::instance()->info(Q_FUNC_INFO, "Try to execute query:\n " + strQuery);
    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute setSensorByExpDatasetTable().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3 \n query : %4")
                                                          .arg(databaseName)
                                                          .arg(querySqlG7.lastError().databaseText())
                                                          .arg(querySqlG7.lastError().type())
                                                          .arg(strQuery));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail to execute query");
        return;
    }

    int entNameCol = querySqlG7.record().indexOf("ENT_NAME");
    int entCodeCol = querySqlG7.record().indexOf("ENT_CODE");
    int tagNameCol = querySqlG7.record().indexOf("TAG_NAME");

    QString queryStringRex = "insert into SENSORBYEXPDATASET (ENT_NAME, TAG_NAME, TAG_NAME ) values ";
    while(querySqlG7.next()) {
        QString entName = querySqlG7.value(entNameCol).toString();
        QString entCode = querySqlG7.value(entCodeCol).toString();
        QString tagName = querySqlG7.value(tagNameCol).toString();
        if(querySqlG7.at() == 0)
            queryStringRex += QString("('%1', '%2', '%3')").arg(entName).arg(entCode).arg(tagName);
        else
            queryStringRex += QString(",('%1', '%2', '%3')").arg(entName).arg(entCode).arg(tagName);
    }

    // Put data in REX
    RSMessageView::Instance()->showData("SENSORBYEXPDATASET");
    {
        QSqlQuery m_querySqlREX(QSqlDatabase::database("REX"));
        execOk = m_querySqlREX.exec(queryStringRex);

        if(!execOk) {
            emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                     tr("setSensorByExpDatasetTable Failed to insert data in SENSORBYEXPDATASET table ().<br/>"
                                        "ErrorText : %1<br/>"
                                        "ErrorType : %2")
                                         .arg(m_querySqlREX.lastError().databaseText())
                                         .arg(m_querySqlREX.lastError().type()));
            RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail m_querySqlRex : ");
            return;
        }
    }
    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

/**
 * @brief setRexDatasetTable create REXDATASET  from the Left join G6DATABASE.G7DATABASE
 * REXDATASET contains THE WHOLE DATAS
 */
void RSDatabaseAccess::setRexDatasetTable()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName        = "REX";
    QSqlDatabase databaseSqlRex = QSqlDatabase::database("REX");
    QSqlQuery querySqlRex(databaseSqlRex);
    bool execOk = false;

    execOk = querySqlRex.exec(QString("drop view if exists REXDATASET"));

    execOk &= querySqlRex.exec(QString("create view if not exists REXDATASET as "
                                         "select distinct g6.*, g7.* from G6DATASET g6 "
                                         "left join G7DATASET g7 "
                                         "on g7.ENTITY_ID = g6.MP_CODE"));

    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute setRexDatasetTable().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(querySqlRex.lastError().databaseText())
                                                          .arg(querySqlRex.lastError().type()));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. m_exec == false Fail.. Query is : " + querySqlRex.executedQuery());
        return;
    }

    QString temp;
    setRexFilterTable(temp);

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSDatabaseAccess::setRexFilterTable(QString& strQuery)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName   = "REX";
    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);

    qDebug().noquote() << ">> setRexFilterTable -" << databaseName << "- ConnectOptions:" << databaseSql.connectOptions();

    databaseSql.open();
    QSqlQuery* querySql;
    querySql    = new QSqlQuery(databaseSql);
    bool execOk = false;
    QString filterQuery;

    if(strQuery.isEmpty())
        filterQuery = RSDataManager::Instance()->getData("FilterQuery").toString();
    else
        filterQuery = strQuery;

    if(!filterQuery.isEmpty() && !filterQuery.toLower().startsWith("where")) {
        filterQuery = " where " + filterQuery;
    }
    RSLogger::instance()->info(Q_FUNC_INFO, " criterias are : " + filterQuery);

    execOk = querySql->exec(QString("drop view if exists REXFILTER"));

    QString completeQuery = QString("create view if not exists REXFILTER as select DISTINCT * from REXDATASET r %1").arg(filterQuery);
    RSLogger::instance()->info(Q_FUNC_INFO, "Try to execute Query: \n" + completeQuery);
    execOk &= querySql->exec(completeQuery);

    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute setRexFilterTable().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(querySql->lastError().databaseText())
                                                          .arg(querySql->lastError().type()));

        RSLogger::instance()->info(Q_FUNC_INFO, "End. Query Fail! :\n");
        return;
    }

    querySql->finish();
    querySql->clear();
    delete querySql;
    databaseSql.close();

    RSLogger::instance()->info(Q_FUNC_INFO, "End. Query Succeeded..");
}

QStringList RSDatabaseAccess::getDataColumn(const QString& table, const QString& field) const
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName   = "REX";
    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);
    // databaseSql.open();
    QSqlQuery* querySql;
    querySql = new QSqlQuery(databaseSql);

    QStringList sensorsList;

    bool execOk = false;

    QString strQuery = QString("select distinct %1 IDATA "
                               "from %2 "
                               "where %1 <> '' "
                               "and %1 is not null "
                               "order by %1")
                           .arg(field)
                           .arg(table);

    execOk = querySql->exec(strQuery);

    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute getDataColumn().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(querySql->lastError().databaseText())
                                                          .arg(querySql->lastError().type()));
        RSLogger::instance()->info(Q_FUNC_INFO, "Failed to execute query: \n " + strQuery);
        return QStringList();
    }
    RSLogger::instance()->info(Q_FUNC_INFO, "Executed query: \n " + strQuery);

    int dataNo = querySql->record().indexOf("IDATA");

    while(querySql->next()) {
        QString data = querySql->value(dataNo).toString();
        sensorsList.push_back(data);
    }

    RSLogger::instance()->info(Q_FUNC_INFO, "clear the query");
    querySql->finish();
    querySql->clear();
    delete querySql;
    databaseSql.close();

    RSLogger::instance()->info(Q_FUNC_INFO, QString("End. nbSensors = %1  ").arg(sensorsList.count()));

    return sensorsList;
}

void RSDatabaseAccess::createDatasetTable()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName        = "REX";
    QSqlDatabase databaseSqlRex = QSqlDatabase::database("REX");
    QSqlQuery querySqlRex(databaseSqlRex);
    bool execOk = false;

    execOk = querySqlRex.exec(QString("drop table if exists G6DATASET"));

    execOk &= querySqlRex.exec(QString("create table if not exists G6DATASET ( "
                                         "MP_CODE VARCHAR(64), "
                                         "MP_NAME VARCHAR(64), "
                                         "AP_CODE VARCHAR(64), "
                                         "AP_NAME VARCHAR(64), "
                                         "ND_CODE VARCHAR(64), "
                                         "ND_NAME VARCHAR(64), "
                                         "AST_BRAND VARCHAR(64), "
                                         "AST_MODEL VARCHAR(64), "
                                         "AST_TECHNOLOGY VARCHAR(64), "
                                         "AST_RANGE VARCHAR(64), "
                                         "AST_THEORICALACCURACY VARCHAR(64), "
                                         "AST_UNIT VARCHAR(64),"
                                         "AST_PHYSICALMEASUREMENT VARCHAR(64),"
                                         "AST_OUTPUTSIGNAL VARCHAR(64)"
                                         ")"));
    if(!execOk)
        RSMessageView::Instance()->showData("REX: failed to create G6DATASET");

    execOk &= querySqlRex.exec(QString("drop table if exists G7DATASET"));

    execOk &= querySqlRex.exec(QString("create table if not exists G7DATASET ( "
                                         "ENT_CODE VARCHAR(64), "
                                         "ENTITY_ID VARCHAR(64), "
                                         "ENT_NAME VARCHAR(64), "
                                         "TAG_NAME VARCHAR(64)"
                                         ")"));
    if(!execOk)
        RSMessageView::Instance()->showData("REX: failed to create G7DATASET");

    execOk &= querySqlRex.exec(QString("drop table if exists BRAND_FIDELITY"));

    execOk &= querySqlRex.exec(QString("create table if not exists BRAND_FIDELITY ( "
                                         "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                         "BRAND VARCHAR(64), "
                                         "FIDELITY DOUBLE"
                                         ")"));
    if(!execOk)
        RSMessageView::Instance()->showData("REX: failed to create BRAND_FIDELITY");

    execOk &= querySqlRex.exec(QString("drop table if exists SENSORBYEXPDATASET"));
    execOk &= querySqlRex.exec(QString("create table if not exists SENSORBYEXPDATASET ( "
                                         "ENT_NAME VARCHAR(64), "
                                         "ENT_CODE VARCHAR(64), "
                                         "TAG_NAME VARCHAR(64)"
                                         ")"));
    if(!execOk)
        RSMessageView::Instance()->showData("REX: failed to create SENSORBYEXPDATASET");

    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute createDatasetTable().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(querySqlRex.lastError().databaseText())
                                                          .arg(querySqlRex.lastError().type()));

        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail");
        return;
    }
    RSLogger::instance()->info(Q_FUNC_INFO, "End. Succeeded");
}

void RSDatabaseAccess::setDatasetTable()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start.");

    setG6DatasetTable_acqPoints();

    setG6DatasetTable_nodes();

    if(m_loadDeadEntities)
        setG6DatasetTable_deadPoints();

    setG7DatasetTable();

    setRexDatasetTable();

    setSensorByExpDatasetTable();

    RSLogger::instance()->info(Q_FUNC_INFO, "End.");
}

bool RSDatabaseAccess::checkFilterQueryAndBuildRexFilterTable(QString& strQuery)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName   = "REX";
    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);
    QSqlQuery* querySql;
    QString filterQuery;
    querySql = new QSqlQuery(databaseSql);

    if(strQuery.isEmpty())
        filterQuery = RSDataManager::Instance()->getData("FilterQuery").toString();
    else
        filterQuery = strQuery;

    if(!filterQuery.isEmpty() && !filterQuery.toLower().startsWith("where")) {
        filterQuery = "where " + filterQuery;
    }

    QString queryString = QString("select count(*) IDATA "
                                  "from REXDATASET r %1")
                              .arg(filterQuery);

    bool execOk = querySql->exec(queryString);
    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute checkFilterQueryAndBuildRexFilterTable().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(querySql->lastError().databaseText())
                                                          .arg(querySql->lastError().type()));

        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail to execute query :\n " + queryString);
        querySql->clear();
        querySql->finish();
        delete querySql;
        databaseSql.close();
        return false;
    }

    RSLogger::instance()->info(Q_FUNC_INFO, "Executed Query :\n " + queryString);

    int dataNo    = querySql->record().indexOf("IDATA");
    int dataCount = 0;

    while(querySql->next()) {
        dataCount = querySql->value(dataNo).toInt();
    }

    bool data = (dataCount > 0) ? true : false;

    // Update the REXFILTER table.
    if(data) {
        RSLogger::instance()->info(Q_FUNC_INFO, "setRexFilterTable   ");
        setRexFilterTable(filterQuery);
    } else {
        RSLogger::instance()->info(Q_FUNC_INFO, "End. No Data!");
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName), tr("No Data!"));
    }

    querySql->clear();
    querySql->finish();
    delete querySql;
    databaseSql.close();

    RSLogger::instance()->info(Q_FUNC_INFO, "End");

    return data;
}

QStringList RSDatabaseAccess::getSensorNameList()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Query on REXFILTER.MP_NAME");
    static QStringList dataList;

    const QString table = "REXFILTER";
    const QString field = "MP_NAME";

    dataList = getDataColumn(table, field);

    return dataList;
}

QList<SensorInfos> RSDatabaseAccess::getSensorsDetailedInfoSet() const
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName     = "REX";
    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);
    QSqlQuery sqlQuery(databaseSql);
    QList<SensorInfos> dataList;

    bool execOk = false;

    QString strQuery = QString("select * from REXFILTER ");
    execOk  = sqlQuery.exec(strQuery);

    RSLogger::instance()->info(Q_FUNC_INFO, QString("Exec query : %1 ").arg(strQuery));
    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute getSensorNameList().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(sqlQuery.lastError().databaseText())
                                                          .arg(sqlQuery.lastError().type()));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail to execute Query : " + strQuery);
        return dataList;
    }

    RSLogger::instance()->info(Q_FUNC_INFO, QString("Gathe the data"));
    int colApCode = sqlQuery.record().indexOf("AP_CODE");
    int colNdCode = sqlQuery.record().indexOf("ND_CODE");
    int colName   = sqlQuery.record().indexOf("MP_NAME");
    int colMpCode = sqlQuery.record().indexOf("MP_CODE");

    int colBrand  = sqlQuery.record().indexOf("AST_BRAND");
    int colModel  = sqlQuery.record().indexOf("AST_MODEL");
    int colTechno = sqlQuery.record().indexOf("AST_TECHNOLOGY");
    int colRange  = sqlQuery.record().indexOf("AST_RANGE");

    int colAccuracy     = sqlQuery.record().indexOf("AST_THEORICALACCURACY");
    int colUnit         = sqlQuery.record().indexOf("AST_UNIT");
    int colPhysicalMeas = sqlQuery.record().indexOf("AST_PHYSICALMEASUREMENT");

    int colOutputSignal = sqlQuery.record().indexOf("AST_OUTPUTSIGNAL");

    while(sqlQuery.next()) {
        SensorInfos info;

        info.name              = sqlQuery.value(colName).value<QString>();
        info.brand             = sqlQuery.value(colBrand).value<QString>();
        info.model             = sqlQuery.value(colModel).value<QString>();
        info.technology        = sqlQuery.value(colTechno).value<QString>();
        info.measRange         = sqlQuery.value(colRange).value<QString>();
        info.mpCode            = sqlQuery.value(colMpCode).value<int>();
        info.theoricalAccuracy = sqlQuery.value(colAccuracy).value<QString>();
        info.unit              = sqlQuery.value(colUnit).value<QString>();
        info.physicalMeas      = sqlQuery.value(colPhysicalMeas).value<QString>();
        info.outputSignal      = sqlQuery.value(colOutputSignal).value<QString>();

        if(!sqlQuery.value(colApCode).isNull()) {
            info.measPointType = MeasPointType::AcqPoint;
            info.code          = sqlQuery.value(colApCode).value<int>();
        } else if(!sqlQuery.value(colNdCode).isNull()) {
            info.measPointType = MeasPointType::Node;
            info.code          = sqlQuery.value(colNdCode).value<int>();
        } else {
            RSLogger::instance()->info(Q_FUNC_INFO, QString("%1  : AP_CODE and ND_CODE are missing!! ").arg(info.name));
        }

        dataList.append(info);
    }

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
    return dataList;
}

QStringList RSDatabaseAccess::getSensorNameList(const QString& field, const QString& name)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    QString databaseName     = "REX";
    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);
    QSqlQuery querySql(databaseSql);
    QStringList dataList;
    bool execOk = false;

    QString strQuery = QString("select distinct MP_NAME IDATA "
                               "from REXFILTER "
                               "where MP_NAME <> '' "
                               "and MP_NAME is not null "
                               "and %1 = '%2' "
                               "order by MP_NAME")
                           .arg(field)
                           .arg(name);
    execOk = querySql.exec(strQuery);

    RSLogger::instance()->info(Q_FUNC_INFO, QString("Exec query : %1 ").arg(strQuery));
    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute getSensorNameList().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type()));
        RSLogger::instance()->info(Q_FUNC_INFO, "End. Fail to execute Query : " + strQuery);
        return QStringList();
    }

    int dataNo = querySql.record().indexOf("IDATA");

    RSLogger::instance()->info(Q_FUNC_INFO, QString("Gathe the data"));
    while(querySql.next()) {
        QString data = querySql.value(dataNo).toString();
        dataList.push_back(data);
    }

    RSLogger::instance()->info(Q_FUNC_INFO, QString("End. Found %1 sensors").arg(dataList.count()));
    return dataList;
}

QPair<int, MeasPointType> RSDatabaseAccess::getSensorNameCodeAndType(const QString& name) const
{
    QString databaseName     = "REX";
    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);
    QSqlQuery querySql(databaseSql);
    QPair<int, MeasPointType> data;
    bool execOk = false;

    const QString strQuery = QString("select RF.AP_CODE,RF.ND_CODE from REXFILTER RF "
                                     "   where RF.MP_NAME = '%1' "
                                     "   and RF.MP_NAME <> '' "
                                     "   and RF.MP_NAME is not null "
                                     "   limit 1")
                                 .arg(name);

    RSLogger::instance()->info(Q_FUNC_INFO, "Execute Query:\n" + strQuery);
    execOk = querySql.exec(strQuery);

    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute getSensorNameCode().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type()));
        return QPair<int, MeasPointType>();
    }

    int apCodeNo = querySql.record().indexOf("AP_CODE");
    int ndCodeNo = querySql.record().indexOf("ND_CODE");

    QVariant apCode, ndCode;

    while(querySql.next()) {
        apCode = querySql.value(apCodeNo).toString();
        ndCode = querySql.value(ndCodeNo).toString();
    }

    if(!apCode.toString().trimmed().isEmpty()) {
        data.first  = apCode.toInt();
        data.second = MeasPointType::AcqPoint;
    } else if(!ndCode.toString().trimmed().isEmpty()) {
        data.first  = ndCode.toInt();
        data.second = MeasPointType::Node;
    } else {
    }

    return data;
}

int RSDatabaseAccess::getSensorNameCode(const QString& name)
{
    QString databaseName     = "REX";
    QSqlDatabase databaseSql = QSqlDatabase::database(databaseName);
    QSqlQuery querySql(databaseSql);
    int apOrNdCode;
    bool execOk = false;

    execOk = querySql.exec(QString("select RF.AP_CODE,RF.ND_CODE from REXFILTER RF "
                                      "where RF.MP_NAME = '%1' "
                                      "and RF.MP_NAME <> '' "
                                      "and RF.MP_NAME is not null "
                                      "limit 1")
                                  .arg(name));

    if(!execOk) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Critical, "red", tr("Error %1 Database").arg(databaseName),
                                                      tr("%1 database cannot execute getSensorNameCode().<br/>"
                                                         "ErrorText : %2<br/>"
                                                         "ErrorType : %3")
                                                          .arg(databaseName)
                                                          .arg(querySql.lastError().databaseText())
                                                          .arg(querySql.lastError().type()));
        return 0;
    }

    int apCodeNo = querySql.record().indexOf("AP_CODE");
    int ndCodeNo = querySql.record().indexOf("ND_CODE");

    QVariant apCode, ndCode;

    while(querySql.next()) {
        apCode = querySql.value(apCodeNo).toString();
        ndCode = querySql.value(ndCodeNo).toString();
    }

    apOrNdCode = apCode.isValid() ? apCode.toInt() : ndCode.toInt();
    return apOrNdCode;
}

QStringList RSDatabaseAccess::getBrandNameList() const
{
    return getDataColumn("G6DATASET", "AST_BRAND");
}

QStringList RSDatabaseAccess::getFilteredBrandNameList() const
{
    return getDataColumn("REXFILTER", "AST_BRAND");
}

QStringList RSDatabaseAccess::getModelNameList() const
{
    return getDataColumn("G6DATASET", "AST_MODEL");
}

QStringList RSDatabaseAccess::getFilteredModelNameList() const
{
    return getDataColumn("REXFILTER", "AST_MODEL");
}

QStringList RSDatabaseAccess::getTechnologyNameList() const
{
    return getDataColumn("G6DATASET", "AST_TECHNOLOGY");
}
QStringList RSDatabaseAccess::getFilteredTechnologyNameList() const
{
    return getDataColumn("REXFILTER", "AST_TECHNOLOGY");
}

QStringList RSDatabaseAccess::getPhysicalMeasurementNameList() const
{
    return getDataColumn("G6DATASET", "AST_PHYSICALMEASUREMENT ");
}
QStringList RSDatabaseAccess::getFilteredPhysicalMeasurementNameList() const
{
    return getDataColumn("REXFILTER", "AST_PHYSICALMEASUREMENT ");
}

QStringList RSDatabaseAccess::getOutputSignalNameList() const
{
    return getDataColumn("G6DATASET", "AST_OUTPUTSIGNAL");
}
QStringList RSDatabaseAccess::getFilteredOutputSignalNameList() const
{
    return getDataColumn("REXFILTER", "AST_OUTPUTSIGNAL");
}

QStringList RSDatabaseAccess::getMeasurementRangeNameList() const
{
    return getDataColumn("G6DATASET", "AST_RANGE");
}
QStringList RSDatabaseAccess::getFilteredMeasurementRangeNameList() const
{
    return getDataColumn("REXFILTER", "AST_RANGE");
}

QStringList RSDatabaseAccess::getTheoricalAccuracyNameList() const
{
    return getDataColumn("G6DATASET", "AST_THEORICALACCURACY");
}
QStringList RSDatabaseAccess::getFilteredTheoricalAccuracyNameList() const
{
    return getDataColumn("REXFILTER", "AST_THEORICALACCURACY");
}

QStringList RSDatabaseAccess::getUnitNameList() const
{
    return getDataColumn("G6DATASET", "AST_UNIT");
}
QStringList RSDatabaseAccess::getFilteredUnitNameList() const
{
    return getDataColumn("REXFILTER", "AST_UNIT");
}

QStringList RSDatabaseAccess::getExperimentationNameList() const
{
    return getDataColumn("G7DATASET", "TAG_NAME");
}
QStringList RSDatabaseAccess::getFilteredExperimentationNameList() const
{
    return getDataColumn("REXFILTER", "TAG_NAME");
}

bool RSDatabaseAccess::checkG7DatabaseStructure(QSqlDatabase& db)
{
    // Map of tables and fields
    QMap<QString /*Table*/, QStringList /*Fields*/> tablesMap;

    tablesMap.insert("T_ENTITIES", QStringList()
                                       << "ENT_CODE"
                                       << "ENTITY_ID"
                                       << "ENT_NAME"
                                       << "ENT_CODE"
                                       << "SRC_CODE"
                                       << "CNT_CODE");
    tablesMap.insert("T_TAG", QStringList()
                                  << "TAG_NAME"
                                  << "TAG_CODE"
                                  << "TCT_CODE");
    tablesMap.insert("T_TAGCATEGORIES", QStringList()
                                            << "TCT_CODE"
                                            << "TCT_CODE");
    tablesMap.insert("T_ENTITYTAG", QStringList()
                                        << "ENT_CODE"
                                        << "TAG_CODE");
    tablesMap.insert("T_JOURNAL", QStringList()
                                      << "JRN_CODE"
                                      << "JRN_STARTDATETIME"
                                      << "JRN_ENDDATETIME"
                                      << "EVT_CODE");
    tablesMap.insert("T_ENTITYJOURNAL", QStringList()
                                            << "ENJ_CODE"
                                            << "ENT_CODE"
                                            << "JRN_CODE");

    m_g7dbStructureIsOk = checkDatabaseStructure(db, tablesMap);
    return m_g7dbStructureIsOk;
}

bool RSDatabaseAccess::checkG6DatabaseStructure(QSqlDatabase& db)
{
    // Map of tables and fields
    QMap<QString /*Table*/, QStringList /*Fields*/> tablesMap;

    tablesMap.insert("ACQSENSORTYPE", QStringList()
                                          << "SI_CODE"
                                          << "AST_CODE"
                                          << "AST_NAME"
                                          << "AST_PHYSICALMEASUREMENT"
                                          << "AST_TECHNOLOGY"
                                          << "AST_OUTPUTSIGNAL"
                                          << "AST_RANGE"
                                          << "AST_THEORETICALACCURACY"
                                          << "AST_UNIT"
                                          << "AST_BRAND"
                                          << "AST_MODEL"
                                          << "AST_UNIT");

    tablesMap.insert("ACQPOINT", QStringList()
                                     << "SI_CODE"
                                     << "DB_CODE"
                                     << "AP_CODE"
                                     << "AST_CODE");

    tablesMap.insert("ACQVALUE", QStringList()
                                     << "SI_CODE"
                                     << "DB_CODE"
                                     << "AP_CODE"
                                     << "AV_ACQUISITIONDAY"
                                     << "AV_PHYSICALDT"
                                     << "AV_ORIGINALVALUE"
                                     << "AV_INGVALUE"
                                     << "AV_STATUS");

    tablesMap.insert("MEASUREPOINT", QStringList()
                                         << "SI_CODE"
                                         << "DB_CODE"
                                         << "MP_CODE"
                                         << "MPT_CODE"
                                         << "MP_NAME"
                                         << "AP_CODE");

    tablesMap.insert("NODE", QStringList()
                                 << "SI_CODE"
                                 << "DB_CODE"
                                 << "ND_CODE"
                                 << "AST_CODE");

    tablesMap.insert("NODERESULT", QStringList()
                                       << "SI_CODE"
                                       << "DB_CODE"
                                       << "ND_CODE"
                                       << "NR_NODEDT"
                                       << "NR_NODEDAY"
                                       << "NR_CALCVALUE"
                                       << "NR_STATUS");

    m_g6dbStructureIsOk = checkDatabaseStructure(db, tablesMap);
    return m_g6dbStructureIsOk;
}

bool RSDatabaseAccess::checkDatabaseStructure(QSqlDatabase& db, const QMap<QString, QStringList>& tabMap) const
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Check " + db.databaseName());
    if(tabMap.isEmpty()) {
        emit Signaler::instance()
            ->signal_emitMessage(QMessageBox::Critical, "red", tr("Error on %1 Database").arg(db.databaseName()), tr("No table."));
        RSLogger::instance()->info(Q_FUNC_INFO, "No table.");
        return false;
    }

    QSqlQuery sqlQuery(db);
    Q_FOREACH(const QString& tb, tabMap.keys()) {
        if(!db.tables().contains(tb)) {
            QString msg = QString("Expected table : %1 not found").arg(tb);
            emit Signaler::instance()
                ->signal_emitMessage(QMessageBox::Critical, "red", tr("Error on %1 Database").arg(db.databaseName()), msg);
            RSLogger::instance()->info(Q_FUNC_INFO, msg);
            return false;
        }

        Q_FOREACH(const QString& field, tabMap[tb]) {
            const QString strQuery = QString(tr("Select %1 from %2")).arg(field).arg(tb);
            RSLogger::instance()->info(Q_FUNC_INFO, "Try to execute query :" + strQuery);
            if(sqlQuery.exec(strQuery) == false) {
                QString msg = QString("Table : %1. Expected field %2  not found").arg(tb).arg(field);
                emit Signaler::instance()
                    ->signal_emitMessage(QMessageBox::Critical, "red", tr("Error on %1 Database").arg(db.databaseName()), msg);
                RSLogger::instance()->info(Q_FUNC_INFO, msg);
                return false;
            }
        }
    }

    return true;
}

bool RSDatabaseAccess::deadEntitiesLoaded() const
{
    return m_loadDeadEntities;
}

bool RSDatabaseAccess::nodesWithNoAstLoaded() const
{
    return m_loadNodesWithNoAst;
}


