#ifndef RSLOGGER_H
#define RSLOGGER_H

#include <QObject>
#include <QMessageLogger>
#include <QFile>


class RSLogger: public QObject
{
public:
   static RSLogger* instance(QObject* parent = 0);
   static void  deleteInstance();
   void info(const QString& function,const QString& msg, int logLevel = 0);

protected:
    RSLogger(const QString& fileName,QObject* parent = 0);

    ~RSLogger();

private:
    static RSLogger* s_instance;
    QFile m_file;
    int m_logLevel;

};

#endif // RSLOGGER_H
