#ifndef RSMAINWINDOW_H
#define RSMAINWINDOW_H

#include <QWidget>
#include <QLayout>
#include <QMessageBox>


#include "RSTimesManager.h"
#include "RSFiltersManager.h"
#include "RSOptionsManager.h"
#include "RSExportWidget.h"
#include "rslogicactionsmanager.h"
#include "RSGraphView.h"
#include "RSMessageView.h"
#include "IRSMainwindow.h"

namespace Ui
{
    class RSMainWindow;
}

class RSMainWindow : public QWidget, public IRSMainWindow
{
    Q_OBJECT

public:
    RSMainWindow(QWidget *parent = 0);
    ~RSMainWindow();
    void treatMessage(QMessageBox::Icon icon, const QString& color, const QString& title, const QString& msg);

    void loadSettings(const QString& fileName) ;
    void saveSettings(const QString& fileName) const;

private:
    void createObjects();


    void position();
    int quitConfirmation();

protected:
    void closeEvent(QCloseEvent *event);
     virtual void keyPressEvent(QKeyEvent *);

    void setupUi();

public slots:
    void slotInitializeSystem();
    /**
     * @brief slotTreatMessage : display a message on the panel
     * @param icon the msg icon
     * @param color : the color
     * @param title : the msg title
     * @param msg : the text to diplay
     */
    void slotTreatMessage(QMessageBox::Icon icon, const QString& color, const QString& title, const QString& msg);


    void slotConfigureDatabase();
      void createConnections();

     void slot_closeAppli();

private:
    Ui::RSMainWindow *ui;

    bool m_closeApplication;

    RSLogicActionsManager* m_logic;
    RSTimesManager* m_timesManager;
    RSFiltersManager* m_filtersManager;
    RSOptionsManager* m_optionsManager;
    RSGraphView* m_graphView;
    RSMessageView* m_messageView;
    RSDatabaseAccess* m_databaseAccess;
};

#endif // RSMAINWINDOW_H
