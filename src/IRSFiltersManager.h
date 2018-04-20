#ifndef IRSFILTERSMANAGER
#define IRSFILTERSMANAGER
#include <QStringList>

class IRSFiltersManager
{
public:

    virtual void loadSettings(const QString& fileName) = 0;
    virtual void saveSettings(const QString& fileName) const = 0;
    virtual void  initQueryAndRexFilterTable() = 0;
    virtual QStringList getExperimentationsList() const = 0;
    virtual void displayNbSensors(int) = 0;
    virtual bool isDynamic() const = 0;

    virtual ~IRSFiltersManager(){}
};

#endif // IRSFILTERSMANAGER

