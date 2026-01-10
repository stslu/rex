/****************************************************************************
**
** Qt Firebird SQL Driver - Private header
**
** This header contains the private implementation details for the
** Qt Firebird SQL driver with proper charset handling.
**
****************************************************************************/

#ifndef QSQL_FIREBIRD_P_H
#define QSQL_FIREBIRD_P_H

#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlresult.h>
#include <QtSql/private/qsqlcachedresult_p.h>
#include <QtSql/private/qsqldriver_p.h>
#include <qmap.h>

#include <ibase.h>

QT_BEGIN_NAMESPACE

class QFirebirdDriver;
class QFirebirdResultPrivate;
struct QFirebirdEventBuffer;

/**
 * Event callback function for Firebird event notifications
 */
void qFirebirdEventCallback(void *result, ISC_USHORT length, const ISC_UCHAR *updated);

/**
 * Structure to hold event buffer information for Firebird event notifications
 */
struct QFirebirdEventBuffer
{
    ISC_LONG eventId;
    ISC_LONG bufferLength;
    ISC_UCHAR *eventBuffer;
    ISC_UCHAR *resultBuffer;
};

/**
 * Private implementation class for QFirebirdDriver
 * 
 * Key feature: Contains TextCodec enum and textCodec member to properly
 * handle charset encoding for legacy databases using NONE, WIN1252, ISO8859_1, etc.
 */
class QFirebirdDriverPrivate : public QSqlDriverPrivate
{
    Q_DECLARE_PUBLIC(QFirebirdDriver)
    
public:
    /**
     * Enum for text codec types
     * 
     * This enum determines how string data is decoded when reading from
     * the database. UTF8 is the default for modern databases, while Latin1
     * is used for legacy databases with charsets like NONE, WIN1252, ISO8859_1.
     */
    enum class TextCodec { 
        UTF8,   ///< UTF-8 encoding (default for modern databases)
        Latin1  ///< Latin1/ISO-8859-1/Windows-1252 encoding (for legacy databases)
    };

    QFirebirdDriverPrivate();

    /**
     * Check if an error occurred and set the last error on the driver
     * @param msg Error message prefix
     * @param errorType Type of SQL error
     * @return true if an error occurred, false otherwise
     */
    bool isError(const char *msg, QSqlError::ErrorType errorType = QSqlError::UnknownError) const;

public:
    isc_db_handle ibase;              ///< Database handle
    isc_tr_handle trans;              ///< Transaction handle
    ISC_STATUS status[20];            ///< Status array for Firebird API calls
    QMap<QString, QFirebirdEventBuffer*> eventBuffers;  ///< Event notification buffers
    TextCodec textCodec;              ///< Text codec for string decoding (NEW: charset fix)
};

/**
 * QFirebirdResult - SQL result class for Firebird driver
 */
class QFirebirdResult : public QSqlCachedResult
{
    Q_DECLARE_PRIVATE(QFirebirdResult)
    friend class QFirebirdResultPrivate;

public:
    explicit QFirebirdResult(const QFirebirdDriver *db);
    ~QFirebirdResult();

    bool prepare(const QString &query) override;
    bool exec() override;
    QVariant handle() const override;

protected:
    bool gotoNext(QSqlCachedResult::ValueCache &row, int rowIdx) override;
    int size() override;
    int numRowsAffected() override;
    QSqlRecord record() const override;
};

/**
 * QFirebirdDriver - SQL driver class for Firebird/Interbase databases
 * 
 * This driver implementation properly handles the ISC_DPB_LC_CTYPE connection
 * option to support legacy databases with non-UTF8 encodings.
 * 
 * Usage example:
 * @code
 * QSqlDatabase db = QSqlDatabase::addDatabase("QFIREBIRD");
 * db.setDatabaseName("mydb.fdb");
 * db.setUserName("SYSDBA");
 * db.setPassword("masterkey");
 * 
 * // For legacy databases with Latin1/Windows-1252 encoding:
 * db.setConnectOptions("ISC_DPB_LC_CTYPE=WIN1252");
 * // or
 * db.setConnectOptions("ISC_DPB_LC_CTYPE=NONE");
 * 
 * // For modern UTF-8 databases:
 * db.setConnectOptions("ISC_DPB_LC_CTYPE=UTF8");
 * @endcode
 */
class QFirebirdDriver : public QSqlDriver
{
    Q_DECLARE_PRIVATE(QFirebirdDriver)
    Q_OBJECT
    friend class QFirebirdResultPrivate;

public:
    explicit QFirebirdDriver(QObject *parent = nullptr);
    explicit QFirebirdDriver(isc_db_handle connection, QObject *parent = nullptr);
    ~QFirebirdDriver() override;

    bool hasFeature(DriverFeature f) const override;
    bool open(const QString &db,
              const QString &user,
              const QString &password,
              const QString &host,
              int port,
              const QString &connOpts) override;
    void close() override;
    QSqlResult *createResult() const override;
    bool beginTransaction() override;
    bool commitTransaction() override;
    bool rollbackTransaction() override;
    QStringList tables(QSql::TableType type) const override;
    QSqlRecord record(const QString &tablename) const override;
    QSqlIndex primaryIndex(const QString &table) const override;
    QString formatValue(const QSqlField &field, bool trimStrings) const override;
    QVariant handle() const override;
    QString escapeIdentifier(const QString &identifier, IdentifierType type) const override;

    bool subscribeToNotification(const QString &name) override;
    bool unsubscribeFromNotification(const QString &name) override;
    QStringList subscribedToNotifications() const override;
};

QT_END_NAMESPACE

#endif // QSQL_FIREBIRD_P_H
