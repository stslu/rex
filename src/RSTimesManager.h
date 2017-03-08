#ifndef RSTIMESMANAGER_H
#define RSTIMESMANAGER_H

#include <QWidget>
#include "IRSTimesManager.h"
class RSDatabaseAccess;
class QDateEdit;

namespace Ui
{
class RSTimesManager;
}

class RSTimesManager : public QWidget, public IRSTimesManager
{
    Q_OBJECT

public:
    RSTimesManager(RSDatabaseAccess*,QWidget *parent = 0);
    ~RSTimesManager();

    /**
     * @brief getStartDateInView :
     * @return the start date  on the widget
     */
    virtual QDateTime getStartDateInView() const;

    /**
     * @brief getEndDateInView :
     * @return the end date  on the widget
     */
    virtual QDateTime getEndDateInView() const;

    /**
     * @brief getStepDate
     * @return the step date in days. From 1 to 360
     */
    virtual qint64 getStepDate() const;

    //!brief : assign the maximum step value
    void setStepViewValue(int data);

    //!brief : load last times settings from setting file
    void loadSettings(const QString& fileName) ;

    //!brief : save last times settings to  setting file
    void saveSettings(const QString& fileName) const;

    //!brief : return the greatest step
    int stepViewMax() const ;

    int stepDays() const ;

protected:

    //! Load the settings and update the interface
    void createObjects();

    //!Connect slots to signals
    void createConnections();

    QVariant loadStartDate();
    QVariant loadEndDate();
    QVariant loadStepDate();

    void saveStartDate()const;
    void saveEndDate()const;
    void saveStepDate()const;

    void setDateEditValue(QDateEdit*, QDate& value);
    void updateStepViewMax(bool emitSignal = true);

public slots:
    //!Catch the start date changement and update the steps
    void slotStartDateValueChanged(const QDate& data);

    //!Catch the end date changement and update the steps
    void slotEndDateValueChanged(const QDate& data);

    //!Catch the step dayse changement and emit a signal, so the graphView can be updated
    void slotStepDateValueChanged(int data);

private:
    Ui::RSTimesManager *ui;
    RSDatabaseAccess* m_databaseAccess;
    QString m_id;
    int m_stepViewMax;
};

#endif // RSTIMESMANAGER_H
