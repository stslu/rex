#include "RSExportToExcel.h"
#include "RSDataComputation.h"
#include "RSDataManager.h"
#include "RSDatabaseAccess.h"
#include "RSFiltersManager.h"
#include "RSGlobalMethods.h"
#include "Signaler.h"

#include <QDir>
#include <QFileDialog>
#include <windows.h>

RSExportToExcel::RSExportToExcel(int trend, double sigma, double noiseFactor, int stepDays, int nbSteps, QObject* parent)
    : QObject(parent)
    , m_trend(trend)
    , m_sigma(sigma)
    , m_noiseFactor(noiseFactor)
    , m_stepsDay(stepDays)
    , m_nbSteps(nbSteps)
{
    createFormat();
}

RSExportToExcel::~RSExportToExcel()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
}

void RSExportToExcel::addSheet(const QStringList& fields, const QList<QVariantList>& data, const QString& sheetName,
                               const QString& pageTitle, const QDate& start, const QDate& end, QString& info)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "start. Sheet = " + sheetName);

    // createFormat();

    //--- -- Get data then Add the datasheet Experimentation in the xls book
    createDataSheet(sheetName, pageTitle, fields, data, start, end, info);

    //Remove the first line

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}


bool RSExportToExcel::saveFile()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");
    //Save the xls book
    bool saved = false;

    //new qxlsx
    saved = m_xlsx.saveAs(m_filePath);

    if(saved == false) {
        emit Signaler::instance()->signal_emitMessage(QMessageBox::Warning,
                                                      "yellow",
                                                      tr("Error Export Data"),
                                                      tr("The data are not exported to excel format."));
    }
    RSLogger::instance()->info(Q_FUNC_INFO, "End");

    return saved;
}


void RSExportToExcel::createDataSheet(const QString& sheetName, const QString& title, const QStringList& fieldsList,
                                      const QList<QVariantList>& data, const QDate& start, const QDate& end, QString& info)
{
    RSLogger::instance()->info(Q_FUNC_INFO, QString("Start . Sheet = " + sheetName + " Title = " + title));

    // Utiliser toStdWString() au lieu de toStdString()
    m_xlsx.addSheet(sheetName);
    m_xlsx.selectSheet(sheetName);

    QString format     = "yyyy/MM/dd";
    QString titleLabel = QString("%3 : From %1 To %2").arg(start.toString(format)).arg(end.toString(format)).arg(title);

    //! Create the title on a merged cells
    int row      = 2;
    int firstCol = 2;
    //TODO: gérer le format : createDataLineGroup(sheet, row, firstCol, 15, titleLabel, m_sheetTitleFormat);
    createDataLineGroup(row, firstCol, 15, titleLabel);
    createDataLineGroup(row + 1, firstCol, 15, titleLabel);

    // Any information ?
    m_xlsx.write(row + 1, firstCol + 0, info);

    //Write the parameters
    m_xlsx.write(row + 3, firstCol + 0, tr("Trend: ") + QString::number(m_trend)); //The amount of sensors
    m_xlsx.write(row + 3, firstCol + 2, tr("Sigma: ") + QString::number(m_sigma)); //The amount of sensors
    m_xlsx.write(row + 3, firstCol + 4, tr("Noise factor: ") + QString::number(m_trend)); //The amount of sensors
    m_xlsx.write(row + 3, firstCol + 8, tr("step(days): ") + QString::number(m_stepsDay)); //The amount of sensors
    m_xlsx.write(row + 3, firstCol + 10, tr("Nb. steps: ") + QString::number(m_nbSteps)); //The amount of sensors

    //Create the title on a merged celles
    int firstRow = row + 7;

    //Fill the Titles
    int line = firstRow;
    for(int col = 0; col < fieldsList.count(); col++)
        m_xlsx.write(line, firstCol + col, fieldsList.at(col)); //The amount of sensors

    //Fill the datas
    for(int row = 0; row < data.size(); row++) {
        line++;
        for(int col = 0; col < data.at(row).size(); col++)
            m_xlsx.write(line, firstCol + col, data.at(row).at(col).toString()); //The amount of sensors
    }

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}



// Modifie le sheet courant (m_xlsx.selectSheet(sheetName); fait avant)
void RSExportToExcel::createDataLineGroup(int row, int col, int nbCol, const QString& data, const QXlsx::Format& format)
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    int m_rowFirst = row;
    int m_rowLast  = row;
    int m_colFirst = col;
    int m_colLast  = col + nbCol - 1;

    //--- -- Merge the cells from TopLeftCorner to BottomRightCorner
    QXlsx::CellRange cellRange;
    cellRange.setFirstRow(m_rowFirst);
    cellRange.setLastRow(m_rowLast);
    cellRange.setFirstColumn(m_colFirst);
    cellRange.setLastColumn(m_colLast);

    // Merge cells & apply the format to merged cells
    m_xlsx.mergeCells(cellRange, format);

    //Write in the merged cells
    QXlsx::CellReference cellRef;
    cellRef.setColumn(col);
    cellRef.setRow(row);
    m_xlsx.write(cellRef, data);

    // QString temp("");
    // sheet->writeStr(1, 1, temp.toStdWString().c_str());

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

void RSExportToExcel::createFormat()
{
    QString impactFont("Impact");
    QString arialFont("Arial");

    RSLogger::instance()->info(Q_FUNC_INFO, "start");
    // m_sheetTitleFormat      = m_book->addFormat();
    // Font* sheetTitleFont    = m_book->addFont();
    // Color sheetTitleBgColor = m_book->colorPack(0, 128, 128);
    // Color sheetTitleFgColor = m_book->colorPack(200, 200, 200);

    // sheetTitleFont->setBold(false);
    // sheetTitleFont->setItalic(false);
    // // sheetTitleFont->setSize(16);
    // sheetTitleFont->setName(impactFont.toStdWString().c_str());
    // sheetTitleFont->setColor(sheetTitleFgColor);
    // m_sheetTitleFormat->setFont(sheetTitleFont);
    // m_sheetTitleFormat->setBorder(BORDERSTYLE_MEDIUM);
    // RSLogger::instance()->info(Q_FUNC_INFO, "h3");
    // m_sheetTitleFormat->setFillPattern(FILLPATTERN_SOLID);
    // m_sheetTitleFormat->setPatternForegroundColor(sheetTitleBgColor);
    // RSLogger::instance()->info(Q_FUNC_INFO, "h");
    // m_sheetTitleFormat->setAlignH(ALIGNH_CENTER);
    // m_sheetTitleFormat->setAlignV(ALIGNV_CENTER);

    // m_tableTitleFormat      = m_book->addFormat();
    // Font* tableTitleFont    = m_book->addFont();
    // Color tableTitleBgColor = m_book->colorPack(0, 128, 128);
    // Color tableTitleFgColor = m_book->colorPack(200, 200, 200);
    // tableTitleFont->setBold(false);
    // tableTitleFont->setItalic(false);
    // // tableTitleFont->setSize(16);
    // tableTitleFont->setName(impactFont.toStdWString().c_str());
    // tableTitleFont->setColor(tableTitleFgColor);
    // m_tableTitleFormat->setFont(tableTitleFont);
    // m_tableTitleFormat->setBorder(BORDERSTYLE_MEDIUM);
    // m_tableTitleFormat->setFillPattern(FILLPATTERN_SOLID);
    // m_tableTitleFormat->setPatternForegroundColor(tableTitleBgColor);
    // m_tableTitleFormat->setAlignH(ALIGNH_CENTER);
    // m_tableTitleFormat->setAlignV(ALIGNV_CENTER);

    // m_tableHeaderFormat   = m_book->addFormat();
    // Font* tableHeaderFont = m_book->addFont();
    // tableHeaderFont->setBold(true);
    // tableHeaderFont->setColor(COLOR_BLUE);
    // tableHeaderFont->setItalic(true);
    // // tableHeaderFont->setSize(12);
    // tableHeaderFont->setName(arialFont.toStdWString().c_str());
    // m_tableHeaderFormat->setFont(tableHeaderFont);
    // m_tableHeaderFormat->setBorder(BORDERSTYLE_MEDIUM);
    // m_tableHeaderFormat->setFillPattern(FILLPATTERN_SOLID);
    // m_tableHeaderFormat->setPatternForegroundColor(COLOR_GOLD);
    // m_tableHeaderFormat->setAlignH(ALIGNH_CENTER);
    // m_tableHeaderFormat->setAlignV(ALIGNV_CENTER);

    // m_tableBodyFormat      = m_book->addFormat();
    // Font* tableBodyFont    = m_book->addFont();
    // Color tableBodyBgColor = m_book->colorPack(173, 216, 230);
    // tableBodyFont->setBold(false);
    // tableBodyFont->setItalic(false);
    // // tableBodyFont->setSize(12);
    // tableBodyFont->setName(arialFont.toStdWString().c_str());
    // m_tableBodyFormat->setFont(tableBodyFont);
    // m_tableBodyFormat->setBorder(BORDERSTYLE_MEDIUM);
    // m_tableBodyFormat->setFillPattern(FILLPATTERN_SOLID);
    // m_tableBodyFormat->setPatternForegroundColor(tableBodyBgColor);
    // m_tableBodyFormat->setAlignH(ALIGNH_LEFT);
    // m_tableBodyFormat->setAlignV(ALIGNV_CENTER);
    // m_tableBodyFormat->setShrinkToFit(false);

    // m_tableColumnDataFormat      = m_book->addFormat();
    // Font* tableColumnDataFont    = m_book->addFont();
    // Color tableColumnDataBgColor = m_book->colorPack(0, 128, 128);
    // tableColumnDataFont->setBold(true);
    // tableColumnDataFont->setColor(COLOR_BLUE);
    // tableColumnDataFont->setItalic(true);
    // // tableColumnDataFont->setSize(12);
    // tableColumnDataFont->setName(arialFont.toStdWString().c_str());
    // m_tableColumnDataFormat->setFont(tableColumnDataFont);
    // m_tableColumnDataFormat->setBorder(BORDERSTYLE_MEDIUM);
    // m_tableColumnDataFormat->setFillPattern(FILLPATTERN_SOLID);
    // m_tableColumnDataFormat->setPatternForegroundColor(tableColumnDataBgColor);
    // m_tableColumnDataFormat->setRotation(90);
    // m_tableColumnDataFormat->setAlignH(ALIGNH_CENTER);
    // m_tableColumnDataFormat->setAlignV(ALIGNV_CENTER);

    RSLogger::instance()->info(Q_FUNC_INFO, "End");
}

// void RSExportToExcel::adjustSheetSize(Sheet* sheet)
// {
//     sheet->setCol(0, 255, -1);
// }

QString RSExportToExcel::filePath() const
{
    return m_filePath;
}

void RSExportToExcel::setFilePath(const QString& newFilePath)
{
    m_filePath = newFilePath;
}
