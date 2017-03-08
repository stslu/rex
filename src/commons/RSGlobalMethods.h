#ifndef RSGLOBALMETHODS_H
#define RSGLOBALMETHODS_H

// not save data

#include <QString>
#include <QDate>
#include <QTime>
#include <QSettings>
#include <QVariant>
#include <QList>
#include <QtMath>
//#include "RSSettingsManager.h}

namespace REX
{

//[RexSensors]
const QString  strPathBase = "data";
const QString   DEFAULT_REX_VERSION = "0.0.8";
const QString   DEFAULT_REX_NAME = "RexSensor";
const QString   DEFAULT_REX_CONFIG_FILE = "RexSensors.ini";
const QString   DEFAULT_REX_APP_DB_FILE =  "RexSensors.dat";
const QString   DEFAULT_REX_G6_DB_FILE = strPathBase +"/GWDBS.fdb";
const QString   DEFAULT_REX_G7_DB_FILE = strPathBase +"/G7MainDB.fdb";
}

class RSGlobalMethods
{
private:
    RSGlobalMethods();

public:
    ~RSGlobalMethods();
    static RSGlobalMethods* Instance();
    static void deleteInstance();
    int logLevel() const;

public:
     double* maxtrix(const QList<double>& x, int rows, int cols);
     double* vandermonde(const QList<double>& x, int degree);
     QList<double> interpolation(const QList<double>& x, const QList<double>& y, int degree);

    QString rexNameAndVersion();

    double min(const QList<double>& data) const;
    double max(const QList<double>& data) const;
    double avg(const QList<double>& data) const;

    QList<double> getStdList(const QList<double>& data, double factor) const;

    QString getDateFilename(const QString& filename) const;

    int getStepNumber(int size, int count) const;
    int getStepNumber(QDate min, QDate max, int step) const;
    QDate getLowerBoundDateOfInterval(QDate min, QDate max, qint64 step, qint64 stepView) const;
    QDate getUpperBoundDateOfInterval(QDate min, QDate max, qint64 step, qint64 stepView) const;

    void saveData(const QString& id, const QString& key, const QVariant& value, QSettings::Format format = QSettings::IniFormat, const QString& filename = REX::DEFAULT_REX_CONFIG_FILE) const;
    QVariant loadData(const QString& id, const QString& key, const QVariant& defaultValue, QSettings::Format format = QSettings::IniFormat, const QString& filename = REX::DEFAULT_REX_CONFIG_FILE);

    QList<QString> getRealTime(QList<double> msecs, QDateTime startDateTime);
    QList<double> getRealTime(QList<double> msecs, int step);
    QList<QString> getRealTime(QList<double> msecs, QDateTime startDateTime, int step);

    void removeFile(const QString& filename);
    bool quitConfirmation();

private:
    static RSGlobalMethods* m_instance;
    int m_logLevel;

};

#endif // RSGLOBALMETHODS_H
