#ifndef IRSMESSAGEVIEW
#define IRSMESSAGEVIEW

#include <QDateTime>
#include <QString>

class IRSMessageView
{
public:
    virtual ~IRSMessageView(){}

    virtual void separate() = 0;
    virtual void showData(const QString& data) = 0;
    virtual  void showData(const QString& data, const QString& name) = 0;
    virtual void showData(int data, const QString &name) = 0;
    virtual void showData(qint64 data, const QString &name) = 0;
    virtual  void showData(double data, const QString &name) = 0;
    virtual void showData(double data, const QString& name, const QString& unit) = 0;
    virtual void showData(const QDate& data, const QString& name, const QString& format) = 0;
    //virtual void showData(const QDateTime& data, const QString& name, const QString& format);
    virtual void showData(const QDate& date1, const QDate& date2, const QString& name, const QString& name1, const QString& name2, const QString& format) = 0;
    virtual void showData(const QDateTime& date1, const QDateTime& date2, const QString& name, const QString& name1, const QString& name2, const QString& format) = 0;
    virtual void showData(const QList<double>& data, const QString& name) = 0;
    virtual void showData(const QList<double>& data, const QString& name, int size) = 0;
    virtual void showData(const QStringList& data) = 0;
};

#endif // IRSMESSAGEVIEW

