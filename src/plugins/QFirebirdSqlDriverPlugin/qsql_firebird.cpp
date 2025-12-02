/****************************************************************************
**
** Qt Firebird SQL Driver Plugin with proper charset handling
**
** This is a modified version of the Qt Firebird/Interbase driver that properly
** handles the ISC_DPB_LC_CTYPE connection option to support legacy databases
** with non-UTF8 encodings (Latin1/CP1252/WIN1252).
**
** Problem: The standard Qt QFIREBIRD driver always uses QString::fromUtf8()
** for text data, which corrupts accented characters in databases using
** charset "NONE" or Latin1-compatible encodings.
**
** Solution: Store the charset setting from ISC_DPB_LC_CTYPE and use the
** appropriate decoder (Latin1 or UTF8) when reading string data.
**
****************************************************************************/

#include "qsql_firebird_p.h"

#include <qcoreapplication.h>
#include <qdatetime.h>
#include <qvariant.h>
#include <qsqlerror.h>
#include <qsqlfield.h>
#include <qsqlindex.h>
#include <qsqlquery.h>
#include <qlist.h>
#include <qvector.h>
#include <qtextcodec.h>
#include <qmutex.h>
#include <QtSql/private/qsqlcachedresult_p.h>
#include <QtSql/private/qsqldriver_p.h>

#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include <ibase.h>

QT_BEGIN_NAMESPACE

using namespace Qt::StringLiterals;

#define FBVERSION SQL_DIALECT_V6

#ifndef SQLDA_CURRENT_VERSION
#define SQLDA_CURRENT_VERSION SQLDA_VERSION1
#endif

// Firebird type codes
#define SQL_VARYING 448
#define SQL_TEXT 452
#define SQL_DOUBLE 480
#define SQL_FLOAT 482
#define SQL_LONG 496
#define SQL_SHORT 500
#define SQL_TIMESTAMP 510
#define SQL_BLOB 520
#define SQL_ARRAY 540
#define SQL_QUAD 550
#define SQL_TYPE_TIME 560
#define SQL_TYPE_DATE 570
#define SQL_INT64 580
#define SQL_BOOLEAN 32764

// Firebird uses ISC status array for error handling
static void initDA(XSQLDA *sqlda)
{
    sqlda->sqln = 0;
    sqlda->sqld = 0;
    sqlda->version = SQLDA_CURRENT_VERSION;
}

static void enlargeDA(XSQLDA *&sqlda, int n)
{
    if (n <= 0) {
        return;
    }
    sqlda = reinterpret_cast<XSQLDA *>(realloc(sqlda, XSQLDA_LENGTH(n)));
    sqlda->sqln = n;
}

static void delDA(XSQLDA *&sqlda)
{
    if (sqlda) {
        free(sqlda);
        sqlda = nullptr;
    }
}

static QVariant::Type qFirebirdTypeName(int iType, bool hasScale)
{
    switch (iType) {
    case SQL_VARYING:
    case SQL_TEXT:
        return QVariant::String;
    case SQL_LONG:
    case SQL_SHORT:
    case SQL_INT64:
        return (hasScale ? QVariant::Double : QVariant::Int);
    case SQL_FLOAT:
    case SQL_DOUBLE:
        return QVariant::Double;
    case SQL_TIMESTAMP:
        return QVariant::DateTime;
    case SQL_TYPE_TIME:
        return QVariant::Time;
    case SQL_TYPE_DATE:
        return QVariant::Date;
    case SQL_ARRAY:
        return QVariant::List;
    case SQL_BLOB:
        return QVariant::ByteArray;
    case SQL_BOOLEAN:
        return QVariant::Bool;
    default:
        return QVariant::Invalid;
    }
}

static ISC_TIMESTAMP toTimeStamp(const QDateTime &dt)
{
    ISC_TIMESTAMP ts;
    static const QDate basedate(1858, 11, 17);
    ts.timestamp_date = basedate.daysTo(dt.date());
    ts.timestamp_time = QTime(0, 0).msecsTo(dt.time()) * 10;
    return ts;
}

static QDateTime fromTimeStamp(const ISC_TIMESTAMP *ts)
{
    QDate d = QDate(1858, 11, 17).addDays(ts->timestamp_date);
    QTime t = QTime(0, 0).addMSecs(ts->timestamp_time / 10);
    return QDateTime(d, t);
}

static ISC_TIME toTime(const QTime &t)
{
    return QTime(0, 0).msecsTo(t) * 10;
}

static QTime fromTime(const ISC_TIME *time)
{
    return QTime(0, 0).addMSecs(*time / 10);
}

static ISC_DATE toDate(const QDate &d)
{
    static const QDate basedate(1858, 11, 17);
    return basedate.daysTo(d);
}

static QDate fromDate(const ISC_DATE *date)
{
    return QDate(1858, 11, 17).addDays(*date);
}

static QByteArray encodeString(QFirebirdDriverPrivate::TextCodec codec, const QString &str)
{
    if (codec == QFirebirdDriverPrivate::TextCodec::Latin1) {
        return str.toLatin1();
    }
    return str.toUtf8();
}

/**
 * Helper function to decode strings using the appropriate codec
 * 
 * This function is the core of the charset fix. It uses Latin1 decoding
 * for legacy database charsets (NONE, WIN1252, ISO8859_1, etc.) and 
 * UTF8 decoding for modern UTF8 databases.
 */
static QString decodeString(const char *data, int length, QFirebirdDriverPrivate::TextCodec codec)
{
    if (codec == QFirebirdDriverPrivate::TextCodec::Latin1) {
        return QString::fromLatin1(data, length);
    }
    return QString::fromUtf8(data, length);
}

static QByteArray createArrayBuffer(const QList<QVariant> &list, QVariant::Type type,
                                    short curDim, ISC_ARRAY_DESC *arrayDesc,
                                    QString *error, QFirebirdDriverPrivate::TextCodec codec)
{
    ISC_ARRAY_BOUND *bounds = arrayDesc->array_desc_bounds;
    short dim = arrayDesc->array_desc_dimensions - 1;

    int arrayItemSize = arrayDesc->array_desc_length;
    if (arrayDesc->array_desc_dtype == blr_varying ||
        arrayDesc->array_desc_dtype == blr_varying2) {
        arrayItemSize += 2;
    }

    int elemCount = 1;
    for (short d = 0; d <= dim; ++d) {
        elemCount *= bounds[d].array_bound_upper - bounds[d].array_bound_lower + 1;
    }

    QByteArray buffer;
    buffer.resize(elemCount * arrayItemSize);

    if (curDim != dim) {
        // Multi-dimensional array - not implemented
        if (error)
            *error = QCoreApplication::translate("QFirebirdDriver", "Nested arrays are not supported.");
        return QByteArray();
    }

    // Single dimensional array
    int i = 0;
    for (const QVariant &v : list) {
        char *ptr = buffer.data() + (i * arrayItemSize);
        switch (type) {
        case QVariant::String: {
            QByteArray strData = encodeString(codec, v.toString());
            short len = strData.length();
            if (arrayDesc->array_desc_dtype == blr_varying ||
                arrayDesc->array_desc_dtype == blr_varying2) {
                memcpy(ptr, &len, sizeof(short));
                memcpy(ptr + sizeof(short), strData.constData(), len);
            } else {
                memset(ptr, ' ', arrayItemSize);
                memcpy(ptr, strData.constData(), qMin(len, (short)(arrayItemSize)));
            }
            break;
        }
        default:
            // Handle other types...
            break;
        }
        ++i;
    }

    return buffer;
}

static QList<QVariant> readArrayBuffer(const char *buffer, const ISC_ARRAY_DESC *arrayDesc,
                                       QFirebirdDriverPrivate::TextCodec textCodec,
                                       QString *error)
{
    QList<QVariant> list;
    ISC_ARRAY_BOUND *bounds = const_cast<ISC_ARRAY_BOUND*>(arrayDesc->array_desc_bounds);
    short dim = arrayDesc->array_desc_dimensions - 1;

    int arrayItemSize = arrayDesc->array_desc_length;
    if (arrayDesc->array_desc_dtype == blr_varying ||
        arrayDesc->array_desc_dtype == blr_varying2) {
        arrayItemSize += 2;
    }

    int lower = bounds[dim].array_bound_lower;
    int upper = bounds[dim].array_bound_upper;

    for (int i = lower; i <= upper; ++i) {
        int idx = i - lower;
        const char *ptr = buffer + (idx * arrayItemSize);

        switch (arrayDesc->array_desc_dtype) {
        case blr_varying:
        case blr_varying2: {
            short len = *reinterpret_cast<const short*>(ptr);
            list.append(decodeString(ptr + sizeof(short), len, textCodec));
            break;
        }
        case blr_text:
        case blr_text2: {
            int len = arrayDesc->array_desc_length;
            // Trim trailing spaces
            while (len > 0 && ptr[len - 1] == ' ')
                --len;
            list.append(decodeString(ptr, len, textCodec));
            break;
        }
        case blr_long:
            list.append(*reinterpret_cast<const int*>(ptr));
            break;
        case blr_short:
            list.append(*reinterpret_cast<const short*>(ptr));
            break;
        case blr_int64:
            list.append(*reinterpret_cast<const qint64*>(ptr));
            break;
        case blr_float:
            list.append(*reinterpret_cast<const float*>(ptr));
            break;
        case blr_double:
            list.append(*reinterpret_cast<const double*>(ptr));
            break;
        case blr_timestamp:
            list.append(fromTimeStamp(reinterpret_cast<const ISC_TIMESTAMP*>(ptr)));
            break;
        case blr_sql_date:
            list.append(fromDate(reinterpret_cast<const ISC_DATE*>(ptr)));
            break;
        case blr_sql_time:
            list.append(fromTime(reinterpret_cast<const ISC_TIME*>(ptr)));
            break;
        default:
            if (error)
                *error = QCoreApplication::translate("QFirebirdDriver",
                    "Unsupported array element type: %1").arg(arrayDesc->array_desc_dtype);
            break;
        }
    }

    return list;
}

/*****************************************************************************
** QFirebirdDriverPrivate - Implementation
*****************************************************************************/

QFirebirdDriverPrivate::QFirebirdDriverPrivate()
    : QSqlDriverPrivate()
    , ibase(0)
    , trans(0)
    , textCodec(TextCodec::UTF8)  // Default to UTF8
{
    dbmsType = QSqlDriver::Interbase;
    memset(status, 0, sizeof(status));
}

bool QFirebirdDriverPrivate::isError(const char *msg, QSqlError::ErrorType errorType) const
{
    if (status[0] != 1 || status[1] == 0)
        return false;

    QString imsg;
    ISC_LONG sqlcode;
    char ibmsg[512];
    int length;

    sqlcode = isc_sqlcode(status);
    isc_sql_interprete(static_cast<short>(sqlcode), ibmsg, sizeof(ibmsg));
    imsg = QString::fromLatin1(ibmsg);

    isc_interprete(ibmsg, const_cast<ISC_STATUS**>(&status));
    imsg += " "_L1 + QString::fromLatin1(ibmsg);

    while (isc_interprete(ibmsg, const_cast<ISC_STATUS**>(&status)))
        imsg += " "_L1 + QString::fromLatin1(ibmsg);

    Q_Q(const QFirebirdDriver);
    q->setLastError(QSqlError(QString::fromLatin1(msg), imsg, errorType,
                              QString::number(sqlcode)));
    return true;
}

/*****************************************************************************
** QFirebirdResultPrivate - Implementation
*****************************************************************************/

class QFirebirdResultPrivate : public QSqlCachedResultPrivate
{
    Q_DECLARE_PUBLIC(QFirebirdResult)
public:
    QFirebirdResultPrivate(QFirebirdResult *q, const QFirebirdDriver *drv);
    ~QFirebirdResultPrivate();

    bool transaction();
    bool commit();

    bool isError(const char *msg, QSqlError::ErrorType type = QSqlError::UnknownError) const
    {
        return drv_d_func()->isError(msg, type);
    }

    bool prepare(const QString &query);
    bool exec();
    void cleanup();

    QFirebirdDriverPrivate *drv_d_func() const
    {
        Q_Q(const QFirebirdResult);
        return reinterpret_cast<QFirebirdDriverPrivate *>(q->driver()->d_func());
    }

    isc_stmt_handle stmt;
    XSQLDA *sqlda;
    XSQLDA *inda;
    ISC_STATUS status[20];
    int queryType;
};

QFirebirdResultPrivate::QFirebirdResultPrivate(QFirebirdResult *q, const QFirebirdDriver *drv)
    : QSqlCachedResultPrivate(q, drv)
    , stmt(0)
    , sqlda(nullptr)
    , inda(nullptr)
    , queryType(-1)
{
    memset(status, 0, sizeof(status));
}

QFirebirdResultPrivate::~QFirebirdResultPrivate()
{
    cleanup();
}

void QFirebirdResultPrivate::cleanup()
{
    if (stmt) {
        isc_dsql_free_statement(status, &stmt, DSQL_drop);
        stmt = 0;
    }

    delDA(sqlda);
    delDA(inda);

    queryType = -1;
}

bool QFirebirdResultPrivate::prepare(const QString &query)
{
    Q_Q(QFirebirdResult);
    cleanup();

    QByteArray ba;
    const auto textCodec = drv_d_func()->textCodec;
    ba = encodeString(textCodec, query);

    isc_dsql_allocate_statement(status, &drv_d_func()->ibase, &stmt);
    if (isError("Unable to allocate statement", QSqlError::StatementError))
        return false;

    sqlda = reinterpret_cast<XSQLDA *>(malloc(XSQLDA_LENGTH(1)));
    initDA(sqlda);
    enlargeDA(sqlda, 1);

    isc_dsql_prepare(status, &drv_d_func()->trans, &stmt, 0,
                     ba.constData(), FBVERSION, sqlda);
    if (isError("Unable to prepare statement", QSqlError::StatementError))
        return false;

    isc_dsql_describe_bind(status, &stmt, FBVERSION, inda ? inda : (inda = reinterpret_cast<XSQLDA *>(malloc(XSQLDA_LENGTH(1)))));
    if (isError("Unable to describe input statement", QSqlError::StatementError))
        return false;

    if (inda->sqld > inda->sqln) {
        enlargeDA(inda, inda->sqld);
        isc_dsql_describe_bind(status, &stmt, FBVERSION, inda);
        if (isError("Unable to describe input statement", QSqlError::StatementError))
            return false;
    }

    if (sqlda->sqld > sqlda->sqln) {
        enlargeDA(sqlda, sqlda->sqld);
        isc_dsql_describe(status, &stmt, FBVERSION, sqlda);
        if (isError("Unable to describe statement", QSqlError::StatementError))
            return false;
    }

    // Get the query type
    char acBuffer[9];
    isc_info_sql_stmt_type;
    char qType = isc_info_sql_stmt_type;
    isc_dsql_sql_info(status, &stmt, 1, &qType, sizeof(acBuffer), acBuffer);
    if (isError("Unable to get query type", QSqlError::StatementError))
        return false;
    int qTypeIndex = acBuffer[0] == isc_info_sql_stmt_type ? 1 : 0;
    queryType = static_cast<int>(isc_vax_integer(&acBuffer[qTypeIndex + 2], 
                                                  static_cast<short>(isc_vax_integer(&acBuffer[qTypeIndex], 2))));

    q->setSelect(queryType == isc_info_sql_stmt_select || queryType == isc_info_sql_stmt_exec_procedure);

    return true;
}

bool QFirebirdResultPrivate::exec()
{
    Q_Q(QFirebirdResult);
    
    if (!stmt)
        return false;

    // Allocate storage for output variables
    for (int i = 0; i < sqlda->sqld; ++i) {
        XSQLVAR &v = sqlda->sqlvar[i];
        v.sqlind = new short;
        switch (v.sqltype & ~1) {
        case SQL_VARYING:
            v.sqldata = new char[v.sqllen + sizeof(short)];
            break;
        case SQL_TEXT:
            v.sqldata = new char[v.sqllen];
            break;
        case SQL_LONG:
            v.sqldata = new char[sizeof(int)];
            break;
        case SQL_SHORT:
            v.sqldata = new char[sizeof(short)];
            break;
        case SQL_INT64:
            v.sqldata = new char[sizeof(qint64)];
            break;
        case SQL_FLOAT:
            v.sqldata = new char[sizeof(float)];
            break;
        case SQL_DOUBLE:
            v.sqldata = new char[sizeof(double)];
            break;
        case SQL_TIMESTAMP:
            v.sqldata = new char[sizeof(ISC_TIMESTAMP)];
            break;
        case SQL_TYPE_DATE:
            v.sqldata = new char[sizeof(ISC_DATE)];
            break;
        case SQL_TYPE_TIME:
            v.sqldata = new char[sizeof(ISC_TIME)];
            break;
        case SQL_BLOB:
        case SQL_ARRAY:
            v.sqldata = new char[sizeof(ISC_QUAD)];
            break;
        case SQL_BOOLEAN:
            v.sqldata = new char[sizeof(char)];
            break;
        default:
            v.sqldata = new char[v.sqllen];
            break;
        }
    }

    if (q->isSelect()) {
        isc_dsql_execute(status, &drv_d_func()->trans, &stmt, FBVERSION, inda);
    } else {
        isc_dsql_execute2(status, &drv_d_func()->trans, &stmt, FBVERSION, inda, sqlda);
    }

    if (isError("Unable to execute query", QSqlError::StatementError)) {
        q->setActive(false);
        return false;
    }

    q->setActive(true);
    return true;
}

/*****************************************************************************
** QFirebirdResult - Implementation
*****************************************************************************/

QFirebirdResult::QFirebirdResult(const QFirebirdDriver *db)
    : QSqlCachedResult(*new QFirebirdResultPrivate(this, db))
{
}

QFirebirdResult::~QFirebirdResult()
{
}

bool QFirebirdResult::prepare(const QString &query)
{
    Q_D(QFirebirdResult);
    return d->prepare(query);
}

bool QFirebirdResult::exec()
{
    Q_D(QFirebirdResult);
    
    if (!d->exec()) {
        return false;
    }
    
    return QSqlCachedResult::exec();
}

bool QFirebirdResult::gotoNext(QSqlCachedResult::ValueCache &row, int rowIdx)
{
    Q_D(QFirebirdResult);
    Q_UNUSED(rowIdx);

    ISC_STATUS stat = isc_dsql_fetch(d->status, &d->stmt, FBVERSION, d->sqlda);

    if (stat == 100) {
        // No more rows
        return false;
    }

    if (d->isError("Error fetching row", QSqlError::StatementError)) {
        return false;
    }

    // Get the text codec from the driver for proper charset handling
    const auto textCodec = d->drv_d_func()->textCodec;

    // Process each column
    for (int i = 0; i < d->sqlda->sqld; ++i) {
        int idx = rowIdx + i;
        XSQLVAR &v = d->sqlda->sqlvar[i];

        if ((v.sqltype & 1) && *v.sqlind < 0) {
            // NULL value
            row[idx] = QVariant(qFirebirdTypeName(v.sqltype & ~1, v.sqlscale < 0));
            continue;
        }

        char *buf = v.sqldata;
        int size = v.sqllen;
        int sqltype = v.sqltype & ~1;

        switch (sqltype) {
        case SQL_VARYING:
            // VARCHAR - first 2 bytes are the length
            // Use the appropriate codec based on connection settings
            row[idx] = decodeString(buf + sizeof(short), *reinterpret_cast<short*>(buf), textCodec);
            break;

        case SQL_TEXT:
            // CHAR - fixed length, trim trailing spaces
            while (size > 0 && buf[size - 1] == ' ')
                --size;
            // Use the appropriate codec based on connection settings
            row[idx] = decodeString(buf, size, textCodec);
            break;

        case SQL_SHORT:
            if (v.sqlscale < 0) {
                row[idx] = static_cast<double>(*reinterpret_cast<short*>(buf)) / 
                          pow(10.0, -v.sqlscale);
            } else {
                row[idx] = static_cast<int>(*reinterpret_cast<short*>(buf));
            }
            break;

        case SQL_LONG:
            if (v.sqlscale < 0) {
                row[idx] = static_cast<double>(*reinterpret_cast<int*>(buf)) / 
                          pow(10.0, -v.sqlscale);
            } else {
                row[idx] = *reinterpret_cast<int*>(buf);
            }
            break;

        case SQL_INT64:
            if (v.sqlscale < 0) {
                row[idx] = static_cast<double>(*reinterpret_cast<qint64*>(buf)) / 
                          pow(10.0, -v.sqlscale);
            } else {
                row[idx] = *reinterpret_cast<qint64*>(buf);
            }
            break;

        case SQL_FLOAT:
            row[idx] = static_cast<double>(*reinterpret_cast<float*>(buf));
            break;

        case SQL_DOUBLE:
            row[idx] = *reinterpret_cast<double*>(buf);
            break;

        case SQL_TIMESTAMP:
            row[idx] = fromTimeStamp(reinterpret_cast<ISC_TIMESTAMP*>(buf));
            break;

        case SQL_TYPE_DATE:
            row[idx] = fromDate(reinterpret_cast<ISC_DATE*>(buf));
            break;

        case SQL_TYPE_TIME:
            row[idx] = fromTime(reinterpret_cast<ISC_TIME*>(buf));
            break;

        case SQL_BLOB: {
            // Read BLOB data
            ISC_QUAD *blobId = reinterpret_cast<ISC_QUAD*>(buf);
            isc_blob_handle blobHandle = 0;
            isc_open_blob2(d->status, &d->drv_d_func()->ibase, &d->drv_d_func()->trans,
                          &blobHandle, blobId, 0, nullptr);
            if (d->isError("Error opening BLOB", QSqlError::StatementError)) {
                row[idx] = QVariant(QVariant::ByteArray);
                break;
            }

            QByteArray blobData;
            char blobBuffer[1024];
            unsigned short segLen;
            while (isc_get_segment(d->status, &blobHandle, &segLen, sizeof(blobBuffer), blobBuffer) == 0) {
                blobData.append(blobBuffer, segLen);
            }
            isc_close_blob(d->status, &blobHandle);
            row[idx] = blobData;
            break;
        }

        case SQL_ARRAY: {
            ISC_QUAD *arrayId = reinterpret_cast<ISC_QUAD*>(buf);
            ISC_ARRAY_DESC arrayDesc;
            
            // Get array descriptor
            char relName[32], fieldName[32];
            memset(relName, 0, sizeof(relName));
            memset(fieldName, 0, sizeof(fieldName));
            memcpy(relName, v.relname, v.relname_length);
            memcpy(fieldName, v.sqlname, v.sqlname_length);
            
            isc_array_lookup_bounds(d->status, &d->drv_d_func()->ibase, &d->drv_d_func()->trans,
                                   relName, fieldName, &arrayDesc);
            if (d->isError("Error looking up array bounds", QSqlError::StatementError)) {
                row[idx] = QVariant(QVariant::List);
                break;
            }

            // Calculate array buffer size
            ISC_LONG bufferLen = arrayDesc.array_desc_length;
            if (arrayDesc.array_desc_dtype == blr_varying || arrayDesc.array_desc_dtype == blr_varying2)
                bufferLen += 2;

            int elemCount = 1;
            for (short dim = 0; dim < arrayDesc.array_desc_dimensions; ++dim) {
                elemCount *= arrayDesc.array_desc_bounds[dim].array_bound_upper -
                            arrayDesc.array_desc_bounds[dim].array_bound_lower + 1;
            }
            bufferLen *= elemCount;

            QByteArray arrayBuffer;
            arrayBuffer.resize(bufferLen);
            
            isc_array_get_slice(d->status, &d->drv_d_func()->ibase, &d->drv_d_func()->trans,
                               arrayId, &arrayDesc, arrayBuffer.data(), &bufferLen);
            if (d->isError("Error getting array slice", QSqlError::StatementError)) {
                row[idx] = QVariant(QVariant::List);
                break;
            }

            QString error;
            // Pass the textCodec to readArrayBuffer for proper charset handling
            row[idx] = readArrayBuffer(arrayBuffer.constData(), &arrayDesc, textCodec, &error);
            if (!error.isEmpty()) {
                setLastError(QSqlError(QCoreApplication::translate("QFirebirdResult", 
                            "Error reading array data"), error, QSqlError::StatementError));
            }
            break;
        }

        case SQL_BOOLEAN:
            row[idx] = static_cast<bool>(*buf);
            break;

        default:
            row[idx] = QVariant();
            break;
        }
    }

    return true;
}

int QFirebirdResult::size()
{
    return -1;
}

int QFirebirdResult::numRowsAffected()
{
    Q_D(QFirebirdResult);
    
    static char acBuffer[64];
    char qType = isc_info_sql_records;
    isc_dsql_sql_info(d->status, &d->stmt, 1, &qType, sizeof(acBuffer), acBuffer);

    for (char *p = acBuffer + 3; *p != isc_info_end;) {
        char type = *p++;
        short len = static_cast<short>(isc_vax_integer(p, 2));
        p += 2;
        if (type == isc_info_req_update_count || type == isc_info_req_delete_count ||
            type == isc_info_req_insert_count) {
            return static_cast<int>(isc_vax_integer(p, len));
        }
        p += len;
    }
    return -1;
}

QSqlRecord QFirebirdResult::record() const
{
    Q_D(const QFirebirdResult);
    
    QSqlRecord rec;
    if (!isActive() || !d->sqlda)
        return rec;

    for (int i = 0; i < d->sqlda->sqld; ++i) {
        XSQLVAR &v = d->sqlda->sqlvar[i];
        QSqlField f(QString::fromLatin1(v.aliasname, v.aliasname_length).simplified(),
                   qFirebirdTypeName(v.sqltype & ~1, v.sqlscale < 0));
        f.setLength(v.sqllen);
        f.setPrecision(qAbs(v.sqlscale));
        f.setRequired((v.sqltype & 1) == 0);
        f.setSqlType(v.sqltype);
        rec.append(f);
    }
    return rec;
}

QVariant QFirebirdResult::handle() const
{
    Q_D(const QFirebirdResult);
    return QVariant::fromValue(d->stmt);
}

/*****************************************************************************
** QFirebirdDriver - Implementation
*****************************************************************************/

QFirebirdDriver::QFirebirdDriver(QObject *parent)
    : QSqlDriver(*new QFirebirdDriverPrivate, parent)
{
}

QFirebirdDriver::QFirebirdDriver(isc_db_handle connection, QObject *parent)
    : QSqlDriver(*new QFirebirdDriverPrivate, parent)
{
    Q_D(QFirebirdDriver);
    d->ibase = connection;
    setOpen(true);
    setOpenError(false);
}

QFirebirdDriver::~QFirebirdDriver()
{
    close();
}

bool QFirebirdDriver::hasFeature(DriverFeature f) const
{
    switch (f) {
    case Transactions:
    case PreparedQueries:
    case PositionalPlaceholders:
    case Unicode:
    case BLOB:
    case EventNotifications:
    case MultipleResultSets:
        return true;
    case QuerySize:
    case NamedPlaceholders:
    case BatchOperations:
    case SimpleLocking:
    case LowPrecisionNumbers:
    case FinishQuery:
    case CancelQuery:
        return false;
    }
    return false;
}

bool QFirebirdDriver::open(const QString &db, const QString &user, const QString &password,
                          const QString &host, int port, const QString &connOpts)
{
    Q_D(QFirebirdDriver);

    if (isOpen())
        close();

    const QStringList opts(connOpts.split(u';', Qt::SkipEmptyParts));

    QByteArray dpb;
    dpb.reserve(256);
    dpb.append(isc_dpb_version1);
    dpb.append(isc_dpb_user_name);
    QByteArray usr = user.toLocal8Bit();
    dpb.append(static_cast<char>(usr.length()));
    dpb.append(usr);
    dpb.append(isc_dpb_password);
    QByteArray pwd = password.toLocal8Bit();
    dpb.append(static_cast<char>(pwd.length()));
    dpb.append(pwd);

    QByteArray enc;
    
    // Process connection options
    for (const QString &opt : opts) {
        const QString tmp(opt.simplified());
        qsizetype idx = tmp.indexOf(u'=');
        if (idx == -1) {
            qWarning("Firebird: Illegal connect option value '%s'", opt.toLocal8Bit().constData());
            continue;
        }
        
        const QString optName = tmp.left(idx).simplified();
        const QString val = tmp.mid(idx + 1).simplified();

        if (optName == "ISC_DPB_LC_CTYPE"_L1) {
            enc = val.toUtf8();
            enc.truncate(255);
            
            // Determine the text codec to use based on the charset
            // This is the core fix: store the codec for later use when reading strings
            QString encUpper = val.toUpper();
            if (encUpper == "NONE"_L1 || encUpper == "OCTETS"_L1 ||
                encUpper == "WIN1252"_L1 || encUpper == "ISO8859_1"_L1 || 
                encUpper == "ISO8859_15"_L1 || encUpper == "LATIN1"_L1 ||
                encUpper == "ASCII"_L1 || encUpper == "ANSI"_L1 ||
                encUpper == "CP1252"_L1 || encUpper == "ISO-8859-1"_L1 ||
                encUpper == "ISO-8859-15"_L1) {
                d->textCodec = QFirebirdDriverPrivate::TextCodec::Latin1;
            } else {
                d->textCodec = QFirebirdDriverPrivate::TextCodec::UTF8;
            }
            
            dpb.append(isc_dpb_lc_ctype);
            dpb.append(static_cast<char>(enc.length()));
            dpb.append(enc);
        } else if (optName == "ISC_DPB_SQL_ROLE_NAME"_L1) {
            QByteArray role = val.toUtf8();
            role.truncate(255);
            dpb.append(isc_dpb_sql_role_name);
            dpb.append(static_cast<char>(role.length()));
            dpb.append(role);
        }
    }

    QString dbPath;
    if (!host.isEmpty())
        dbPath = host + (port ? u':' + QString::number(port) : QString()) + u':';
    dbPath += db;
    QByteArray ba = dbPath.toLocal8Bit();

    isc_attach_database(d->status, static_cast<short>(ba.size()), ba.constData(),
                       &d->ibase, static_cast<short>(dpb.size()), dpb.constData());

    if (d->isError("Error opening database", QSqlError::ConnectionError)) {
        setOpenError(true);
        return false;
    }

    setOpen(true);
    setOpenError(false);
    return true;
}

void QFirebirdDriver::close()
{
    Q_D(QFirebirdDriver);

    if (isOpen()) {
        if (d->trans) {
            isc_rollback_transaction(d->status, &d->trans);
            d->trans = 0;
        }
        isc_detach_database(d->status, &d->ibase);
        d->ibase = 0;
        setOpen(false);
        setOpenError(false);
    }
}

QSqlResult *QFirebirdDriver::createResult() const
{
    return new QFirebirdResult(this);
}

bool QFirebirdDriver::beginTransaction()
{
    Q_D(QFirebirdDriver);

    if (!isOpen() || isOpenError())
        return false;
    if (d->trans)
        return false;

    isc_start_transaction(d->status, &d->trans, 1, &d->ibase, 0, nullptr);
    return !d->isError("Error starting transaction", QSqlError::TransactionError);
}

bool QFirebirdDriver::commitTransaction()
{
    Q_D(QFirebirdDriver);

    if (!isOpen() || isOpenError())
        return false;
    if (!d->trans)
        return false;

    isc_commit_transaction(d->status, &d->trans);
    d->trans = 0;
    return !d->isError("Error committing transaction", QSqlError::TransactionError);
}

bool QFirebirdDriver::rollbackTransaction()
{
    Q_D(QFirebirdDriver);

    if (!isOpen() || isOpenError())
        return false;
    if (!d->trans)
        return false;

    isc_rollback_transaction(d->status, &d->trans);
    d->trans = 0;
    return !d->isError("Error rolling back transaction", QSqlError::TransactionError);
}

QStringList QFirebirdDriver::tables(QSql::TableType type) const
{
    Q_D(const QFirebirdDriver);
    
    QStringList res;
    if (!isOpen())
        return res;

    QString typeFilter;
    if (type & QSql::SystemTables)
        typeFilter += "RDB$SYSTEM_FLAG = 1"_L1;
    else if (type & QSql::Tables)
        typeFilter += "RDB$SYSTEM_FLAG = 0 OR RDB$SYSTEM_FLAG IS NULL"_L1;
    if (type & QSql::Views)
        typeFilter += (typeFilter.isEmpty() ? QString() : " OR "_L1) + 
                     "RDB$VIEW_SOURCE IS NOT NULL"_L1;

    if (typeFilter.isEmpty())
        return res;

    QSqlQuery q(createResult());
    q.setForwardOnly(true);
    if (!q.exec("SELECT RDB$RELATION_NAME FROM RDB$RELATIONS WHERE "_L1 + typeFilter))
        return res;

    while (q.next())
        res << q.value(0).toString().simplified();

    return res;
}

QSqlRecord QFirebirdDriver::record(const QString &tablename) const
{
    QSqlRecord rec;
    if (!isOpen())
        return rec;

    QSqlQuery q(createResult());
    q.setForwardOnly(true);
    QString table = tablename.toUpper();

    q.exec("SELECT a.RDB$FIELD_NAME, a.RDB$NULL_FLAG, a.RDB$FIELD_SOURCE, "
           "f.RDB$FIELD_LENGTH, f.RDB$FIELD_SCALE, f.RDB$FIELD_TYPE "
           "FROM RDB$RELATION_FIELDS a "
           "JOIN RDB$FIELDS f ON f.RDB$FIELD_NAME = a.RDB$FIELD_SOURCE "
           "WHERE a.RDB$RELATION_NAME = '"_L1 + table + "' "
           "ORDER BY a.RDB$FIELD_POSITION"_L1);

    while (q.next()) {
        QSqlField f(q.value(0).toString().simplified(),
                   qFirebirdTypeName(q.value(5).toInt(), q.value(4).toInt() < 0));
        f.setRequired(q.value(1).toInt() > 0);
        f.setLength(q.value(3).toInt());
        f.setPrecision(qAbs(q.value(4).toInt()));
        rec.append(f);
    }

    return rec;
}

QSqlIndex QFirebirdDriver::primaryIndex(const QString &tablename) const
{
    QSqlIndex idx;
    if (!isOpen())
        return idx;

    QString table = tablename.toUpper();
    QSqlQuery q(createResult());
    q.setForwardOnly(true);
    q.exec("SELECT a.RDB$INDEX_NAME, b.RDB$FIELD_NAME, d.RDB$FIELD_TYPE "
           "FROM RDB$RELATION_CONSTRAINTS a "
           "JOIN RDB$INDEX_SEGMENTS b ON a.RDB$INDEX_NAME = b.RDB$INDEX_NAME "
           "JOIN RDB$RELATION_FIELDS c ON a.RDB$RELATION_NAME = c.RDB$RELATION_NAME "
           "AND b.RDB$FIELD_NAME = c.RDB$FIELD_NAME "
           "JOIN RDB$FIELDS d ON c.RDB$FIELD_SOURCE = d.RDB$FIELD_NAME "
           "WHERE a.RDB$RELATION_NAME = '"_L1 + table + "' "
           "AND a.RDB$CONSTRAINT_TYPE = 'PRIMARY KEY' "_L1
           "ORDER BY b.RDB$FIELD_POSITION"_L1);

    while (q.next()) {
        if (idx.name().isEmpty())
            idx.setName(q.value(0).toString().simplified());
        QSqlField f(q.value(1).toString().simplified(),
                   qFirebirdTypeName(q.value(2).toInt(), false));
        idx.append(f);
    }

    return idx;
}

QString QFirebirdDriver::formatValue(const QSqlField &field, bool trimStrings) const
{
    switch (field.type()) {
    case QVariant::DateTime: {
        QDateTime dt = field.value().toDateTime();
        if (dt.isValid())
            return u'\'' + QString::number(dt.date().year()) + u'-' +
                   QString::number(dt.date().month()) + u'-' +
                   QString::number(dt.date().day()) + u' ' +
                   QString::number(dt.time().hour()) + u':' +
                   QString::number(dt.time().minute()) + u':' +
                   QString::number(dt.time().second()) + u'.' +
                   QString::number(dt.time().msec()).rightJustified(3, u'0', true) + u'\'';
        else
            return "NULL"_L1;
    }
    case QVariant::Time: {
        QTime tm = field.value().toTime();
        if (tm.isValid())
            return u'\'' + QString::number(tm.hour()) + u':' +
                   QString::number(tm.minute()) + u':' +
                   QString::number(tm.second()) + u'.' +
                   QString::number(tm.msec()).rightJustified(3, u'0', true) + u'\'';
        else
            return "NULL"_L1;
    }
    case QVariant::Date: {
        QDate dt = field.value().toDate();
        if (dt.isValid())
            return u'\'' + QString::number(dt.year()) + u'-' +
                   QString::number(dt.month()) + u'-' +
                   QString::number(dt.day()) + u'\'';
        else
            return "NULL"_L1;
    }
    default:
        return QSqlDriver::formatValue(field, trimStrings);
    }
}

QVariant QFirebirdDriver::handle() const
{
    Q_D(const QFirebirdDriver);
    return QVariant::fromValue(d->ibase);
}

QString QFirebirdDriver::escapeIdentifier(const QString &identifier, IdentifierType) const
{
    QString res = identifier;
    if (!identifier.isEmpty() && !identifier.startsWith(u'"') && !identifier.endsWith(u'"')) {
        res.replace(u'"', "\"\""_L1);
        res.prepend(u'"').append(u'"');
    }
    return res;
}

bool QFirebirdDriver::subscribeToNotification(const QString &name)
{
    Q_D(QFirebirdDriver);

    if (!isOpen()) {
        qWarning("QFirebirdDriver::subscribeToNotification: database not open.");
        return false;
    }

    if (d->eventBuffers.contains(name)) {
        qWarning("QFirebirdDriver::subscribeToNotification: already subscribing to '%s'.",
                 qPrintable(name));
        return false;
    }

    QFirebirdEventBuffer *eBuffer = new QFirebirdEventBuffer;
    eBuffer->bufferLength = 0;
    eBuffer->resultBuffer = nullptr;
    eBuffer->eventBuffer = nullptr;

    QByteArray ba = name.toLocal8Bit();
    eBuffer->bufferLength = isc_event_block(&eBuffer->eventBuffer, &eBuffer->resultBuffer, 1,
                                            ba.constData());
    isc_que_events(d->status, &d->ibase, &eBuffer->eventId, 
                   static_cast<short>(eBuffer->bufferLength),
                   eBuffer->eventBuffer, 
                   reinterpret_cast<ISC_EVENT_CALLBACK>(qFirebirdEventCallback),
                   eBuffer);
    if (d->isError("Error subscribing to event notification", QSqlError::UnknownError)) {
        delete eBuffer;
        return false;
    }

    d->eventBuffers.insert(name, eBuffer);
    return true;
}

bool QFirebirdDriver::unsubscribeFromNotification(const QString &name)
{
    Q_D(QFirebirdDriver);

    if (!isOpen()) {
        qWarning("QFirebirdDriver::unsubscribeFromNotification: database not open.");
        return false;
    }

    if (!d->eventBuffers.contains(name)) {
        qWarning("QFirebirdDriver::unsubscribeFromNotification: not subscribed to '%s'.",
                 qPrintable(name));
        return false;
    }

    QFirebirdEventBuffer *eBuffer = d->eventBuffers.value(name);
    isc_cancel_events(d->status, &d->ibase, &eBuffer->eventId);
    if (d->isError("Error cancelling event notification", QSqlError::UnknownError))
        return false;

    isc_free(eBuffer->eventBuffer);
    isc_free(eBuffer->resultBuffer);
    d->eventBuffers.remove(name);
    delete eBuffer;

    return true;
}

QStringList QFirebirdDriver::subscribedToNotifications() const
{
    Q_D(const QFirebirdDriver);
    return d->eventBuffers.keys();
}

void qFirebirdEventCallback(void *result, ISC_USHORT length, const ISC_UCHAR *updated)
{
    Q_UNUSED(length);
    Q_UNUSED(updated);
    QFirebirdEventBuffer *eBuffer = reinterpret_cast<QFirebirdEventBuffer *>(result);
    if (!eBuffer)
        return;

    // This callback is called from Firebird's thread, need to process in main thread
    // The actual implementation would emit a signal here
}

QT_END_NAMESPACE
