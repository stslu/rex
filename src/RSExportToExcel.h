#ifndef RSEXPORTTOEXCEL_H
#define RSEXPORTTOEXCEL_H

#include <QObject>
#include <libxl.h>
#include <RSLogger.h>

using namespace libxl;

class RSExportToExcel : public QObject
{
public:

    RSExportToExcel(int trend, double sigma, double noiseFactor,int stepDays, int nbSteps,QObject *parent = 0);

    ~RSExportToExcel();

    void exportData();
    void addSheet(const QStringList& fields,const QList<QVariantList>& data, const QString& sheetName, const QString& pageTitle, const QDate& start, const QDate& end, QString& info = QString());
    bool saveFile();
    void setParameters(int trend, double sigma, double noiseFactor);

protected:
    void createDataSheets();

    void createDataSheet(const QString& sheetName,const QString&  pageTitle,const QStringList& fields, const QList<QVariantList>& data, const QDate& start, const QDate& end, QString& info = QString());

    int getRow(const QString& sheetName);

    void addData(const QString& sheetName, int row, int col, const QString& data);

    void createFormat();

    void createDataLineGroup(Sheet *sheet, int row, int col, int size, const QString& data, Format *format = 0);

    void adjustSheetSize(Sheet *sheet);

private:

    Book *m_book;
    QString m_rexReportPath;
    QString m_rexReportFile;

    Format *m_sheetTitleFormat;
    Format *m_tableTitleFormat;
    Format *m_tableHeaderFormat;
    Format *m_tableBodyFormat;
    Format *m_tableColumnDataFormat;
    QString m_filePath;

    int m_trend;
    double m_sigma;
    double m_noiseFactor;
    int m_stepsDay;
    int m_nbSteps;

};

#endif // RSEXPORTTOEXCEL_H
