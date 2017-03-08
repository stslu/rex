#ifndef RSEXPORTMANAGER_H
#define RSEXPORTMANAGER_H

#include <QWidget>
#include <QFrame>
#include <QLayout>
#include <QMovie>

#include <QLabel>
#include <QPushButton>
#include "irsexportmanager.h"
class IRSGraphView;

namespace Ui
{
class RSExportManager;
}

class RSExportManager : public QWidget, public IRSExportManager
{
    Q_OBJECT

public:
    RSExportManager(QWidget *parent = 0);
    ~RSExportManager();

    void setProgressBarRate(double) ;


private:
    void createObjects();
    void createConnections();

public slots:
    void slot_updateDataButtonClicked();
    void slotTestButtonClicked();
    void slotExcelButtonClicked();
    void slot_updateSensorsListButtonClicked();

private:
    Ui::RSExportManager *ui;
    QMovie* m_movie;

};

#endif // RSEXPORTMANAGER_H
