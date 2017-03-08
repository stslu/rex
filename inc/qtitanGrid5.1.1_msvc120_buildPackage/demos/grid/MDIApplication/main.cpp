#include <QApplication>

#include "window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("QtitanDataGrid MDI");
    app.setOrganizationName("Developer Machines");

    Window window;
    window.show();
    return app.exec();
}
