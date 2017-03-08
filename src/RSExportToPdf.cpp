#include "RSExportToPdf.h"

RSExportToPdf::RSExportToPdf(QObject *parent) : QObject(parent)
{

}

//bool Plot::doExport(const QString &fileName, qreal resolutionFactor)
//{
//    bool isOk = false;

//    if (!fileName.isEmpty())
//    {
//        QSizeF sizeMM = QSizeF(width() / resolutionFactor,
//                               height() / resolutionFactor);
//        QwtPlotRenderer renderer;
//        LegendSetting* pLegendSetting = this->legendSetting();
//        if (pLegendSetting != NULL)
//        {
//            const int resolution = 600;
//            const double mmToInch = 1.0 / 25.4;
//            const QSizeF size = sizeMM * mmToInch * resolution;

//            QRectF documentRect( 0.0, 0.0, size.width(), size.height());
//            QPrinter printer;
//            printer.setColorMode(QPrinter::Color);
//            printer.setFullPage(true);
//            printer.setPaperSize(sizeMM, QPrinter::Millimeter );
//            printer.setDocName(title().text() );
//            printer.setOutputFileName(fileName);
//            printer.setResolution(resolution);

//            QString fileFormat = QFileInfo(fileName).suffix();
//            if(fileFormat == "pdf")
//                printer.setOutputFormat( QPrinter::PdfFormat );
//#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
//            else if(fileFormat == "ps")
//                printer.setOutputFormat( QPrinter::PostScriptFormat );
//#endif
//            else
//            {
//                if ( QImageWriter::supportedImageFormats().indexOf(
//                         fileFormat.toLatin1() ) >= 0 )
//                {
//                    QRectF imageRect = documentRect;
//                    const int dotsPerMeter = qRound( resolution * mmToInch * 1000.0 );

//                    QImage image( imageRect.size().toSize(), QImage::Format_ARGB32 );
//                    image.setDotsPerMeterX( dotsPerMeter );
//                    image.setDotsPerMeterY( dotsPerMeter );
//                    image.fill( QColor( Qt::white ).rgb() );

//                    QPainter painter( &image );
//                    this->print(&painter, imageRect, false);
//                    image.save( fileName, fileFormat.toLatin1() );
//                    return true;
//                }
//            }

//            QPainter painter(&printer);
//            this->print(&painter, documentRect, false);
//            isOk = true;
//        }
//    }

//    return isOk;
//}
