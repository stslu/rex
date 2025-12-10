#ifndef RSEXPORTTOEXCEL_H
#define RSEXPORTTOEXCEL_H

#include <RSLogger.h>
#include <QObject>


#include <xlsxcellrange.h>
#include <xlsxchart.h>
#include <xlsxchartsheet.h>
#include <xlsxdocument.h>
#include <xlsxrichstring.h>
#include <xlsxworkbook.h>


class RSExportToExcel : public QObject
{
public:
    RSExportToExcel(
        int trend, double sigma, double noiseFactor, int stepDays, int nbSteps, QObject *parent = 0);

    ~RSExportToExcel();

    void addSheet(const QStringList &fields,
                  const QList<QVariantList> &data,
                  const QString &sheetName,
                  const QString &pageTitle,
                  const QDate &start,
                  const QDate &end,
                  QString &info);
    bool saveFile();
    void setParameters(int trend, double sigma, double noiseFactor);

    QString filePath() const;
    void setFilePath(const QString& newFilePath);

protected:
    void createDataSheet(const QString &sheetName,
                         const QString &pageTitle,
                         const QStringList &fields,
                         const QList<QVariantList> &data,
                         const QDate &start,
                         const QDate &end,
                         QString &info);

    void addData(const QString &sheetName, int row, int col, const QString &data);
    void createFormat();

    // void createDataLineGroup(Sheet *sheet, int row, int col, int size, const QString &data, Format *format = 0);
    // const Format &format = Format()
    void createDataLineGroup(int row, int col, int size, const QString& data, const QXlsx::Format& format = QXlsx::Format());

private:

    QString m_rexReportPath;
    QString m_rexReportFile;

    // Format *m_sheetTitleFormat;
    // Format *m_tableTitleFormat;
    // Format *m_tableHeaderFormat;
    // Format *m_tableBodyFormat;
    // Format *m_tableColumnDataFormat;
    QString m_filePath;

    int m_trend;
    double m_sigma;
    double m_noiseFactor;
    int m_stepsDay;
    int m_nbSteps;

    // Passage à qxlsx
    QXlsx::Document m_xlsx;

};

#endif // RSEXPORTTOEXCEL_H
