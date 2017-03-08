#include "RSExportToExcel.h"
#include "RSGlobalMethods.h"
#include "RSDatabaseAccess.h"
#include "RSDataManager.h"
#include "RSDataComputation.h"
#include "RSFiltersManager.h"
#include "Signaler.h"

#include <windows.h>
#include <QDir>
#include <QFileDialog>

RSExportToExcel::RSExportToExcel(int trend, double sigma, double noiseFactor,int stepDays, int nbSteps,QObject *parent) : QObject(parent)
  , m_book(0)
  , m_trend(trend)
  , m_sigma(sigma)
  , m_noiseFactor(noiseFactor)
  , m_stepsDay(stepDays)
  , m_nbSteps(nbSteps)
{

}

RSExportToExcel::~RSExportToExcel()
{
 RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
}

void RSExportToExcel::createDataSheets()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"start");

    m_book = xlCreateBook();

    createFormat();

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSExportToExcel::addSheet(const QStringList& fields
                               , const QList<QVariantList>& data
                               , const QString& sheetName
                               , const QString& pageTitle
                               , const QDate& start
                               , const QDate& end)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"start. Sheet = " + sheetName);
    if(!m_book)
        m_book = xlCreateBook();

    if(m_filePath.isEmpty())
    {
        QString fileName = QString("RexExport_%1").arg(QDateTime::currentDateTime().toString("yyyy_MM_dd hh.mm.ss"));

        //--- -- Get the path
        m_filePath = QFileDialog::getSaveFileName(NULL,
                                                  tr("Select file"),fileName
                                                  ,"Microsoft Excel (*.xls);;Microsoft Excel (*.xlsx);;Csv (*.csv)");
    }

    if(m_filePath.isEmpty())
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End. m_filePath.isEmpty() ");
        return;
    }
    RSLogger::instance()->info(Q_FUNC_INFO,"m_filePath= " + m_filePath);
    QFile fileCheck( m_filePath );
    bool canOpen = fileCheck.open( QFile::WriteOnly );
    if (!canOpen )
    {
        QMessageBox::warning( NULL, QObject::tr("Failed to save excel file"),
                              QObject::tr("Failed to write excel data to file %1: %2.").arg( m_filePath ).arg( fileCheck.errorString() ) );
        fileCheck.close();
        RSLogger::instance()->info(Q_FUNC_INFO,"End. canOpen = fileCheck.open( QFile::WriteOnly ) = FALSE " );
        return;
    }

    createFormat();

    //--- -- Get data then Add the datasheet Experimentation in the xls book
    createDataSheet(sheetName, pageTitle,fields,data,start,end);

    //Remove the first line


    RSLogger::instance()->info(Q_FUNC_INFO,"End" );

}

bool RSExportToExcel::saveFile()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");
    //Save the xls book
    bool saved = m_book->save(m_filePath.toStdString().c_str());
    m_book->release();

    if(saved == false)
    {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Warning, "yellow", tr("Error Export Data"),tr("The data are not exported to excel format."));

    }
    RSLogger::instance()->info(Q_FUNC_INFO,"End");
    return saved;
}

void RSExportToExcel::createDataSheet(const QString& sheetName,const QString&  title,const QStringList& fieldsList, const QList<QVariantList>& data, const QDate& start, const QDate& end)
{
    RSLogger::instance()->info(Q_FUNC_INFO,QString("Start . Sheet = " + sheetName + " Title = " + title));

    Sheet *sheet = m_book->addSheet(sheetName.toStdString().c_str());

    QString format = "yyyy/MM/dd";
    QString m_titleLabel = QString("%3 : From %1 To %2").arg(start.toString(format)).arg(end.toString(format)).arg(title);
    //Create the title on a merged celles
    int row = 2;
    int firstCol = 2;
    createDataLineGroup(sheet, row, firstCol, 15, m_titleLabel, m_sheetTitleFormat);

    //Write the parameters
    sheet->writeStr(row +1,firstCol + 0,QString(tr("Trend: ") + QString::number(m_trend)).toStdString().c_str());//The amount of sensors
    sheet->writeStr(row +1,firstCol + 2,QString(tr("Sigma: ")+ QString::number(m_sigma)).toStdString().c_str());//The amount of sensors
    sheet->writeStr(row +1,firstCol + 4,QString(tr("Noise factor: ")+ QString::number(m_trend)).toStdString().c_str());//The amount of sensors
    sheet->writeStr(row +1,firstCol + 8,QString(tr("step(days): ")+ QString::number(m_stepsDay)).toStdString().c_str());//The amount of sensors
    sheet->writeStr(row +1,firstCol + 10,QString(tr("Nb. steps: ")+ QString::number(m_nbSteps)).toStdString().c_str());//The amount of sensors


    //Create the title on a merged celles
    int firstRow = row + 4;

    //Fill the Titles
    int line = firstRow;
    for(int col = 0; col < fieldsList.count(); col++)
        sheet->writeStr(line,firstCol + col,fieldsList.at(col).toStdString().c_str());//The amount of sensors

    //Fill the datas
    for(int row = 0; row < data.size(); row++)
    {
        line++;
        for(int col = 0; col < data.at(row).size(); col++)
            sheet->writeStr(line,firstCol + col,data.at(row).at(col).toString().toStdString().c_str());//The amount of sensors
    }

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}


void RSExportToExcel::createDataLineGroup(Sheet *sheet, int row, int col, int nbCol, const QString& data, Format *format)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    int m_rowFirst = row;
    int m_rowLast = row;
    int m_colFirst = col;
    int m_colLast = col + nbCol - 1;

    //--- -- Merge the cells from TopLeftCorner to BottomRightCorner
    sheet->setMerge(m_rowFirst, m_rowLast, m_colFirst, m_colLast);

    //Write in the merged cells
    sheet->writeStr(row, col, data.toStdString().c_str());

    //Apply the format to merged cells
    for(int i = 0; i < nbCol; i++)
    {
        sheet->setCellFormat(row,  m_colFirst + i, format);
    }

    sheet->writeStr(1, 1, "");

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSExportToExcel::createFormat()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"start");
    m_sheetTitleFormat = m_book->addFormat();
    Font *m_sheetTitleFont = m_book->addFont();
    Color sheetTitleBgColor = m_book->colorPack(0, 128, 128);
    Color m_sheetTitleFgColor = m_book->colorPack(200, 200, 200);
    m_sheetTitleFont->setBold(false);
    m_sheetTitleFont->setItalic(false);
    m_sheetTitleFont->setSize(16);
    m_sheetTitleFont->setName("Impact");
    m_sheetTitleFont->setColor(m_sheetTitleFgColor);
    m_sheetTitleFormat->setFont(m_sheetTitleFont);
    m_sheetTitleFormat->setBorder(BORDERSTYLE_MEDIUM);
    RSLogger::instance()->info(Q_FUNC_INFO,"h3");
    m_sheetTitleFormat->setFillPattern(FILLPATTERN_SOLID);
    m_sheetTitleFormat->setPatternForegroundColor(sheetTitleBgColor);
    RSLogger::instance()->info(Q_FUNC_INFO,"h");
    m_sheetTitleFormat->setAlignH(ALIGNH_CENTER);
    m_sheetTitleFormat->setAlignV(ALIGNV_CENTER);

    m_tableTitleFormat = m_book->addFormat();
    Font *m_tableTitleFont = m_book->addFont();
    Color m_tableTitleBgColor = m_book->colorPack(0, 128, 128);
    Color m_tableTitleFgColor = m_book->colorPack(200, 200, 200);
    m_tableTitleFont->setBold(false);
    m_tableTitleFont->setItalic(false);
    m_tableTitleFont->setSize(16);
    m_tableTitleFont->setName("Impact");
    m_tableTitleFont->setColor(m_tableTitleFgColor);
    m_tableTitleFormat->setFont(m_tableTitleFont);
    m_tableTitleFormat->setBorder(BORDERSTYLE_MEDIUM);
    m_tableTitleFormat->setFillPattern(FILLPATTERN_SOLID);
    m_tableTitleFormat->setPatternForegroundColor(m_tableTitleBgColor);
    m_tableTitleFormat->setAlignH(ALIGNH_CENTER);
    m_tableTitleFormat->setAlignV(ALIGNV_CENTER);

    m_tableHeaderFormat = m_book->addFormat();
    Font *m_tableHeaderFont = m_book->addFont();
    m_tableHeaderFont->setBold(true);
    m_tableHeaderFont->setColor(COLOR_BLUE);
    m_tableHeaderFont->setItalic(true);
    m_tableHeaderFont->setSize(12);
    m_tableHeaderFont->setName("Arial");
    m_tableHeaderFormat->setFont(m_tableHeaderFont);
    m_tableHeaderFormat->setBorder(BORDERSTYLE_MEDIUM);
    m_tableHeaderFormat->setFillPattern(FILLPATTERN_SOLID);
    m_tableHeaderFormat->setPatternForegroundColor(COLOR_GOLD);
    m_tableHeaderFormat->setAlignH(ALIGNH_CENTER);
    m_tableHeaderFormat->setAlignV(ALIGNV_CENTER);

    m_tableBodyFormat = m_book->addFormat();
    Font *m_tableBodyFont = m_book->addFont();
    Color m_tableBodyBgColor = m_book->colorPack(173, 216, 230);
    m_tableBodyFont->setBold(false);
    m_tableBodyFont->setItalic(false);
    m_tableBodyFont->setSize(12);
    m_tableBodyFont->setName("Arial");
    m_tableBodyFormat->setFont(m_tableBodyFont);
    m_tableBodyFormat->setBorder(BORDERSTYLE_MEDIUM);
    m_tableBodyFormat->setFillPattern(FILLPATTERN_SOLID);
    m_tableBodyFormat->setPatternForegroundColor(m_tableBodyBgColor);
    m_tableBodyFormat->setAlignH(ALIGNH_LEFT);
    m_tableBodyFormat->setAlignV(ALIGNV_CENTER);
    m_tableBodyFormat->setShrinkToFit(false);

    m_tableColumnDataFormat = m_book->addFormat();
    Font *m_tableColumnDataFont = m_book->addFont();
    Color m_tableColumnDataBgColor = m_book->colorPack(0, 128, 128);
    m_tableColumnDataFont->setBold(true);
    m_tableColumnDataFont->setColor(COLOR_BLUE);
    m_tableColumnDataFont->setItalic(true);
    m_tableColumnDataFont->setSize(12);
    m_tableColumnDataFont->setName("Arial");
    m_tableColumnDataFormat->setFont(m_tableColumnDataFont);
    m_tableColumnDataFormat->setBorder(BORDERSTYLE_MEDIUM);
    m_tableColumnDataFormat->setFillPattern(FILLPATTERN_SOLID);
    m_tableColumnDataFormat->setPatternForegroundColor(m_tableColumnDataBgColor);
    m_tableColumnDataFormat->setRotation(90);
    m_tableColumnDataFormat->setAlignH(ALIGNH_CENTER);
    m_tableColumnDataFormat->setAlignV(ALIGNV_CENTER);

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSExportToExcel::adjustSheetSize(Sheet *sheet)
{
    sheet->setCol(0, 255, -1);
}
