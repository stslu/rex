#ifndef IRSTIMESMANAGER
#define IRSTIMESMANAGER

#include <QDateTime>

class IRSTimesManager
{
public:
    virtual QDateTime getStartDateInView() const = 0;
    virtual  QDateTime getEndDateInView() const = 0;
    virtual qint64 getStepDate() const = 0;
    virtual void setStepViewValue(int data) = 0;

    virtual void loadSettings(const QString& fileName) = 0;
    virtual void saveSettings(const QString& fileName) const = 0;

    virtual int stepViewMax() const = 0;
    virtual int stepDays()  const = 0;

    virtual ~IRSTimesManager(){}
};

#endif // IRSTIMESMANAGER

