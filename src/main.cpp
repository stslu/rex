#include <QApplication>
#include "RSMainWindow.h"
// #include "RSStylesManager.h"
#include "RSSettingsManager.h"
#include "RSLogger.h"
#include "RSDatabaseAccess.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    RSMainWindow m_mainWindow;

    m_mainWindow.showMaximized();

    return app.exec();
}
