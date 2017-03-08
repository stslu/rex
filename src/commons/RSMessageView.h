#ifndef RSMESSAGEVIEW_H
#define RSMESSAGEVIEW_H

#include <QWidget>
#include <QDate>
#include <QDateTime>
#include "RSDatabaseConfig.h"
#include "IRSMessageView.h"

namespace Ui
{
    class RSMessageView;
}

class RSMessageView : public QWidget, public IRSMessageView
{
    Q_OBJECT

private:
    RSMessageView(QWidget *parent = 0);

public:
    ~RSMessageView();
    static RSMessageView* Instance(QWidget *parent = 0);
    static void deleteInstance();

    virtual void separate();
    virtual void showData(const QString& data);
    virtual void showData(const QString& data, const QString& name);
    virtual void showData(int data, const QString &name);
    virtual void showData(qint64 data, const QString &name);
    virtual void showData(double data, const QString &name);
    virtual void showData(double data, const QString& name, const QString& unit);
    virtual void showData(const QDate& data, const QString& name, const QString& format);
    virtual void showData(const QDateTime& data, const QString& name, const QString& format);
    virtual void showData(const QDate& date1, const QDate& date2, const QString& name, const QString& name1, const QString& name2, const QString& format);
    virtual void showData(const QDateTime& date1, const QDateTime& date2, const QString& name, const QString& name1, const QString& name2, const QString& format);
    virtual void showData(const QList<double>& data, const QString& name);
    virtual void showData(const QList<double>& data, const QString& name, int size);
    virtual void showData(const QStringList& data);
    void clear();

protected slots:
    void slotRunButtonClicked();
    void slotClearButtonClicked();

protected:
    void position();
    void createObjects();
    void createConnections();

private:
    static RSMessageView *m_instance;
    quint64 m_nbLines;

    Ui::RSMessageView *ui;


};

#endif // RSMESSAGEVIEW_H
