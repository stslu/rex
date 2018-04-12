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

    //!brief :setStepViewValue assign the maximum step value
    //! @param stepMax  the step max to assign
    void setStepViewValue(int stepMax);

    //!brief : load last times settings from setting file
    void loadSettings(const QString& fileName) ;

    /**
     * @brief : save last times settings to  setting file
     * @param fileName the path of the settings
     */
    void saveSettings(const QString& fileName) const;

    //!brief : return the greatest step
    int stepViewMax() const ;

    //!@brief stepDays returns the number of day per step
    int stepDays() const ;

protected:

    //! Load the settings and update the interface
    void createObjects();

    //! Connect slots to signals
    void createConnections();

    //! @brief loadStartDate. return the start date defined by the user. The default is current date minus 5 years
    QVariant loadStartDate();

    //!@brief loadEndDate return the default end date in the settings file. Current date is the default value
    QVariant loadEndDate();

    //!@brief loadStepDate  return the step in the settings. The default value is one week (7 Days)
    QVariant loadStepDate();

    //!@brief saveStartDate add the start date in the settings fir rexSensors.ini
    void saveStartDate()const;

    //!@brief saveEndDate add the end date in the settings fir rexSensors.ini
    void saveEndDate()const;

    //!@brief saveEndDate add the step  in the settings fir rexSensors.ini
    void saveStepDate()const;

    /**
     * @brief setDateEditValue in sert a date in a QDateEdit by blocking signals
     * @param object pointer on the objectName
     * @param date the date to set
     */
    void setDateEditValue(QDateEdit* object, QDate& date);

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
