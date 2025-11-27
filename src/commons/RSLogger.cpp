#include "RSLogger.h"
#include <QDateTime>
#include <QTextStream>
#include <QDebug>
#include <QDir>

#include <QStringConverter>

#include "RSGlobalMethods.h"

#define FORMAT  "yyyy_MM_dd hh_mm_ss"

RSLogger* RSLogger::s_instance = 0;

RSLogger::RSLogger(const QString& fileName,QObject* parent ) : QObject(parent),m_file(fileName),m_logLevel(0)
{
    if(getenv ("REXLOGLEVEL"))
        m_logLevel = atoi(getenv("REXLOGLEVEL"));
}


RSLogger::~RSLogger()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
}

RSLogger* RSLogger::instance(QObject* parent)
{
    if(!RSLogger::s_instance)
    {
        const QString pathLog =  "../RexLogs";
        const QString fileName = QString("%1/RexLog-%2.log").arg(pathLog).arg(QDateTime::currentDateTime().toString(FORMAT));
        if(!QDir(pathLog).exists())
            QDir().mkdir(pathLog);

        RSLogger::s_instance = new RSLogger( fileName,parent);
    }
    return RSLogger::s_instance;
}


void RSLogger::deleteInstance()
{
    if(RSLogger::s_instance)
    {
        delete  RSLogger::s_instance ;
        RSLogger::s_instance = 0;
    }
}

void RSLogger::info(const QString& function,const QString& msg, int logLevel)
{    

    if(logLevel > m_logLevel)
        return;

    if(QFile(m_file.fileName()).size() >= 6e+8)
        remove (m_file.fileName().toStdString().c_str());

    m_file.open(QIODevice::Append);

    QTextStream out(&m_file);
    QString strLine (QDateTime::currentDateTime().toString(FORMAT) + " " + function.mid(0, function.indexOf("(")) + "\t => " + msg + "\n");

    // out.setCodec("UTF-8"); // Qt 5
    // out.setEncoding(QStringConverter::Utf8); // Qt 6
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#else
    out.setEncoding(QStringConverter::Utf8);
#endif

    if (m_file.isOpen())
        out << strLine;
    out.flush();

    m_file.close();
}
