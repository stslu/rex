#include <QApplication>
#include "RSMainWindow.h"
#include "RSStylesManager.h"
#include "RSSettingsManager.h"
#include "RSLogger.h"
#include "RSDatabaseAccess.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//    RSStylesManager::Instance(qApp);

    RSMainWindow *m_mainWindow = new RSMainWindow;

    m_mainWindow->show();



    return app.exec();
}
