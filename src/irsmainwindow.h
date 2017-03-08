#ifndef IRSMAINWINDOW
#define IRSMAINWINDOW

#include <QMessageBox>

class IRSMainWindow
{
public:

    virtual ~IRSMainWindow(){}
    virtual void treatMessage(QMessageBox::Icon icon, const QString& color, const QString& title, const QString& msg) = 0;
    virtual void loadSettings(const QString& fileName) = 0;
    virtual void saveSettings(const QString& fileName) const = 0;

};

#endif // IRSMAINWINDOW

