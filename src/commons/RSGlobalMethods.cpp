#include "RSGlobalMethods.h"
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_vector_double.h>
#include <QDir>
#include <RSLogger.h>
#include "RSMessageView.h"
#include <QMessageBox>
#include <QSpacerItem>
#include <QGridLayout>
#include <QTranslator>

#include <QDebug>

RSGlobalMethods* RSGlobalMethods::m_instance = 0;

RSGlobalMethods::RSGlobalMethods()
{


}

RSGlobalMethods::~RSGlobalMethods()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
}

RSGlobalMethods* RSGlobalMethods::Instance()
{
    if(m_instance == 0)
    {
        m_instance = new RSGlobalMethods();
    }

    return m_instance;
}

void  RSGlobalMethods::deleteInstance()
{
    if(m_instance)
    {
        delete m_instance;
        m_instance = 0;
    }
}

// dij = xi
double* RSGlobalMethods::maxtrix(const QList<double>& x, int rows, int cols)
{
    double* m_data = new double[rows*cols];

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            double m_xi = x.at(i);

            int k = i*cols + j;
            m_data[k] = m_xi;
        }
    }

    return m_data;
}


// dij = xi^j
double* RSGlobalMethods::vandermonde(const QList<double>& x, int degree)
{
    int m_rows = x.size();
    int m_cols = degree + 1;
    double* m_data = new double[m_rows*m_cols];

    for(int i = 0; i < m_rows; i++)
    {
        for(int j = 0; j < m_cols; j++)
        {
            double m_xi = x.at(i);
            double m_dij = qPow(m_xi, (double)j);

            int k = i*m_cols + j;
            m_data[k] = m_dij;
        }
    }

    return m_data;
}

// Ax = b // A(m x n)
QList<double> RSGlobalMethods::interpolation(const QList<double>& x, const QList<double>& y, int degree)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start",1);


    QList<double> m_data;
    double* m_dV = vandermonde(x, degree);
    int m_M = x.size();
    int m_N = degree + 1;

    double* m_dB = maxtrix(y, m_M, 1);

    gsl_matrix_view m_A = gsl_matrix_view_array (m_dV, m_M, m_N);
    gsl_matrix *m_V = gsl_matrix_alloc(m_N, m_N);
    gsl_vector *m_S = gsl_vector_alloc(m_N);
    gsl_vector *m_Work = gsl_vector_alloc(m_N);

    if( m_A.matrix.size1 <=1 )
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"matrix size11 is equal to 1, cannot compute");
        gsl_matrix_free(m_V);
        gsl_vector_free(m_S);
        gsl_vector_free(m_Work);
        delete[] m_dV;
        delete[] m_dB;
        return m_data;
    }

    gsl_linalg_SV_decomp(&m_A.matrix, m_V, m_S, m_Work);

    gsl_vector_view m_B = gsl_vector_view_array(m_dB, m_M);
    gsl_vector *m_X = gsl_vector_alloc(m_N);

    gsl_linalg_SV_solve(&m_A.matrix, m_V, m_S, &m_B.vector, m_X);

    for(int i = 0; i < m_N; i++)
    {
        m_data.push_back(m_X->data[i]);
    }

    gsl_matrix_free(m_V);
    gsl_vector_free(m_S);
    gsl_vector_free(m_Work);
    gsl_vector_free(m_X);

    delete[] m_dV;
    delete[] m_dB;

    RSLogger::instance()->info(Q_FUNC_INFO,"End",1);
    return m_data;
}

// d = min(x)
double RSGlobalMethods::min(const QList<double>& data) const
{
    Q_ASSERT(data.size() != 0);

    double m_min = data.at(0);
    int m_size = data.size();

    for(int i = 1; i < m_size; i++)
    {
        double m_data = data.at(i);

        if(m_min > m_data)
        {
            m_min = m_data;
        }
    }

    return m_min;
}

// d = max(x)
double RSGlobalMethods::max(const QList<double> &data) const
{
    Q_ASSERT(data.size() != 0);

    double m_max = data.at(0);
    int m_size = data.size();

    for(int i = 1; i < m_size; i++)
    {
        double m_data = data.at(i);

        if(m_max < m_data)
        {
            m_max = m_data;
        }
    }

    return m_max;
}

// d = Sum(xi)/Size(x)
double RSGlobalMethods::avg(const QList<double> &data) const
{
    /*double m_avg = 0.0;
    int m_size = data.size();

    Q_ASSERT(m_size != 0);

    for(int i = 0; i < m_size; i++)
    {
        double m_data = data.at(i);
        m_avg += m_data;
    }

    m_avg /= m_size;

    return m_avg;*/

    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    double mean = sum / data.size();
    return mean;
}

// di = xi/f
QList<double> RSGlobalMethods::getStdList(const QList<double>& dataList, double factor) const
{
    Q_ASSERT(dataList.size() != 0);

    QList<double> dividedDataList;

    for(int i = 0; i < dataList.size(); i++)
    {
        double m_di = dataList.at(i) / factor;
        dividedDataList.push_back(m_di);
    }

    return dividedDataList;
}

QString RSGlobalMethods::getDateFilename(const QString& filename) const
{
    QDateTime m_dateTime = QDateTime::currentDateTime();
    QString m_format = m_dateTime.toString("yyyy_MM_dd_hh_mm_ss_zzz");
    QString m_newFilename = filename + "_" + m_format + ".xls";

    return m_newFilename;
}

// d = s/c
int RSGlobalMethods::getStepNumber(int size, int count) const
{
    Q_ASSERT(size >= 2 && count >= 2);

    int m_data = (size - 1)/(count - 1);

    return m_data;
}

// d = (max - min)/step
int RSGlobalMethods::getStepNumber(QDate min, QDate max, int step) const
{
    Q_ASSERT(step >= 0);

    if(min > max)
    {
        qSwap(min, max);
    }

    qint64 m_width = min.daysTo(max);
    qint64 m_modulo = m_width % step;
    qint64 m_data = m_width / step;

    if(m_modulo == 0 && m_data != 0)
    {
        m_data -= 1;
    }

    return m_data;
}

// d = min + stepView*step
QDate RSGlobalMethods::getLowerBoundDateOfInterval(QDate minDate, QDate maxDate, qint64 stepDate, qint64 stepView) const
{
    Q_ASSERT(stepDate >= 0 && stepView >= 0);
    qint64 gapOfTimeMsec = stepView*stepDate;

    //The upper bound date is the start date of the interval, increased by the Gap in msec
    QDate uppperBoundDate = qMin(minDate.addDays(gapOfTimeMsec), maxDate);

    RSLogger::instance()->info(Q_FUNC_INFO,"minDate = "
                               + minDate.toString()
                               + "minDate = "
                               + maxDate.toString()
                               + " Return : " + uppperBoundDate.toString());

    return uppperBoundDate;
}

// d = min + (stepView + 1)*step
QDate RSGlobalMethods::getUpperBoundDateOfInterval(QDate min, QDate max, qint64 stepDate, qint64 stepView) const
{
    Q_ASSERT(stepDate >= 0 && stepView >= 0);

    qint64 step = (stepView + 1)*stepDate;

    QDate dataStep = min.addDays(step);

    QDate data = qMin(dataStep, max);

    return data;
}

void RSGlobalMethods::saveData(const QString& id, const QString& key, const QVariant& value, QSettings::Format format, const QString& filename) const
{
    QSettings m_settings(filename, format);
    QString m_keyId = id + "/" + key;
    m_settings.setValue(m_keyId, value);

    RSLogger::instance()->info(Q_FUNC_INFO,"key =  " + value.toString());
}


QVariant RSGlobalMethods::loadData(const QString& id, const QString& key, const QVariant& defaultValue, QSettings::Format format, const QString& filename)
{
    QSettings m_settings(filename, format);
    QString m_keyId = id + "/" + key;
    QVariant data = m_settings.value(m_keyId, defaultValue);

    if(data.isValid() == false)
    {
        return defaultValue;
    }

    RSLogger::instance()->info(Q_FUNC_INFO,"key =  " + data.toString());
    return data;
}

// di = a0 + ai
QList<QString> RSGlobalMethods::getRealTime(QList<double> msecs, QDateTime startDateTime)
{
    Q_ASSERT(msecs.size() != 0);

    QList<QString> m_data;

    for(int i = 0; i < msecs.size(); i++)
    {
        qint64 m_msecs = msecs.at(i);
        QDateTime m_realDateTime = startDateTime.addMSecs(m_msecs);
        QString m_format = m_realDateTime.toString("yyyy-MM-dd hh:mm:ss");
        m_data.push_back(m_format);
    }

    return m_data;
}

QList<double> RSGlobalMethods::getRealTime(QList<double> msecs, int step)
{
    Q_ASSERT(msecs.size() != 0);

    QList<double> m_data;
    if(step == 0)
        return m_data;

    for(int i = 0; i < msecs.size(); i++)
    {
        if(i % step == 0)
        {
            double m_msecs = msecs.at(i);
            m_data.push_back(m_msecs);
        }
    }

    return m_data;
}

QList<QString> RSGlobalMethods::getRealTime(QList<double> msecs, QDateTime startDateTime, int step)
{
    Q_ASSERT(msecs.size() != 0);

    QList<QString> m_data;

    if(step == 0)
        return m_data;

    for(int i = 0; i < msecs.size(); i++)
    {
        if(i % step == 0)
        {
            qint64 m_msecs = (qint64)msecs.at(i);
            QDateTime m_realDateTime = startDateTime.addMSecs(m_msecs);
            QString m_format = m_realDateTime.toString("yyyy-MM-dd hh:mm:ss");
            m_data.push_back(m_format);
        }
    }

    return m_data;
}

void RSGlobalMethods::removeFile(const QString& filename)
{
    QDir m_dir;
    m_dir.remove(filename);
}

QString RSGlobalMethods::rexNameAndVersion()
{
    return QString("%1 %2").arg(REX::DEFAULT_REX_NAME).arg(REX::DEFAULT_REX_VERSION);
}

bool RSGlobalMethods::quitConfirmation()
{
    QString m_title = QTranslator::tr("Quit Confirmation");
    QString m_msgTitle = QTranslator::tr("<font style=\"color:lime; font-weight: bold;\">"
                                         "%1").arg(m_title);
    QString m_msg = QTranslator::tr("<font style=\"color:%1; font-weight: bold;\">"
                                    "Are you sure you want to quit ?"
                                    "</font>");

    QMessageBox m_message;
    m_message.setWindowTitle(m_title);
    m_message.setIcon(QMessageBox::Information);
    m_message.setText(m_msgTitle);
    m_message.setInformativeText(m_msg);
    m_message.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    m_message.setCursor(Qt::PointingHandCursor);

    QSpacerItem* m_spacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* m_layout = (QGridLayout*)m_message.layout();
    m_layout->addItem(m_spacer, m_layout->rowCount(), 0, 1, m_layout->columnCount());

    if(m_message.exec() == QMessageBox::Yes)
        return true;
    else
        return false;
}

int RSGlobalMethods::logLevel() const
{
    return m_logLevel;
}
