#include "RSMainWindow.h"
#include "ui_RSMainWindow.h"
#include "RSDatabaseAccess.h"
#include "RSExportToExcel.h"
#include "RSDataGenerator.h"
#include "RSDatabaseCreate.h"
#include "RSMessageView.h"
#include "RSGraphView.h"
#include "RSSettingsManager.h"
#include "RSPictoManager.h"
#include "RSDataManager.h"

#include <QMessageBox>
#include <QTimer>
#include <QCloseEvent>
#include <RSLogger.h>
#include <QSplitter>

RSMainWindow::RSMainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::RSMainWindow),IRSMainWindow(),
    m_timesManager(0),
    m_filtersManager(0),
    m_optionsManager(0),
    m_graphView(0)
  , m_messageView(0)
  , m_logic(0)
  , m_databaseAccess(0)
{   
    position();

    createConnections();

    createObjects();

    setupUi();

    this->adjustSize();
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());
    this->setWindowState(Qt::WindowMaximized);

    QTimer::singleShot(200, this, SLOT(slotInitializeSystem()));

}

RSMainWindow::~RSMainWindow()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
    if(m_logic)
        delete m_logic;

    if(m_timesManager)
        delete m_timesManager;

    if(m_filtersManager)
        delete m_filtersManager;

    if(m_optionsManager)
        delete m_optionsManager;

    if(m_graphView)
        delete m_graphView;

    delete ui;

    //Delete singletons
    RSLogger::instance()->info(Q_FUNC_INFO,"delete  RSDatabaseAccess");
    RSDatabaseAccess::deleteInstance();

    RSLogger::instance()->info(Q_FUNC_INFO,"delete  RSGlobalMethods");
    RSGlobalMethods::deleteInstance();

    RSLogger::instance()->info(Q_FUNC_INFO,"delete  Signaler");
    Signaler::killInstance();

    RSLogger::instance()->info(Q_FUNC_INFO,"delete  RSMessageView");
    RSMessageView::deleteInstance();
}

void RSMainWindow::setupUi()
{
    ui->setupUi(this);

    this->setWindowTitle(RSGlobalMethods::Instance()->rexNameAndVersion());

    if(!m_timesManager)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"init m_timesManager");
        m_timesManager = new RSTimesManager(m_databaseAccess,this);
    }

    if(!m_filtersManager)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"init m_filtersManager");
        m_filtersManager = new RSFiltersManager(m_databaseAccess,this);
    }

    if(!m_optionsManager)
        m_optionsManager = new RSOptionsManager(this);

    if(!m_messageView)
        m_messageView = RSMessageView::Instance();
    m_messageView->setWindowTitle(RSGlobalMethods::Instance()->rexNameAndVersion());

    if(!m_graphView)
    {
        m_graphView = new RSGraphView(m_databaseAccess,this);
        m_graphView->setStepViewMax(m_timesManager->stepViewMax());
    }

    if(!m_logic)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"init m_logic");
        m_logic = new RSLogicActionsManager(m_databaseAccess
                                            , this
                                            , m_timesManager
                                            , m_optionsManager
                                            , m_graphView
                                            , m_filtersManager
                                            , this);
    }

    //Add widgets in splitters
    QSplitter* mainSplitter = new QSplitter(this);
    QSplitter* leftSplitter = new QSplitter(this);

    leftSplitter->setOrientation(Qt::Vertical);

    leftSplitter->addWidget(m_timesManager);
    leftSplitter->addWidget(m_filtersManager);
    leftSplitter->addWidget(m_optionsManager);
    m_messageView->setVisible(true);
    leftSplitter->addWidget(m_messageView);
    leftSplitter->setFrameStyle(QFrame::StyledPanel);

    leftSplitter->setStyleSheet("handle:horizontal {background: qlineargradient(x1:0, y1:0, x2:1, y2:1,stop:0 #eee, stop:1 #ccc);"
                                "border: 1px solid #777;"
                                "width: 13px;"
                                "margin-top: 2px;"
                                "margin-bottom: 2px;"
                                "border-radius: 8px;"
                                "}");

    leftSplitter->setStretchFactor(1, 1);
    mainSplitter->setOrientation(Qt::Horizontal);
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(m_graphView);
    mainSplitter->setOpaqueResize( false );
    mainSplitter->setChildrenCollapsible( true );
    mainSplitter->setWindowState(Qt::WindowMaximized);
    mainSplitter->setFrameStyle(QFrame::StyledPanel);
    this->layout()->addWidget(mainSplitter);
    mainSplitter->setHandleWidth(1);
}


void RSMainWindow::position()
{
    QSize m_desktopSize = QApplication::desktop()->size();
    adjustSize();
    QSize m_screenSize = size();

    int m_x = (m_desktopSize.width() - m_screenSize.width()) / 2;
    int m_y = (m_desktopSize.height() - m_screenSize.height()) / 2;
    int m_w = m_screenSize.width();
    int m_h = m_screenSize.height();

    setGeometry(m_x, m_y, m_w, m_h);
}

void RSMainWindow::createObjects()
{       
    m_databaseAccess =  RSDatabaseAccess::Instance(this);

    RSDatabaseCreate::Instance(this);

    RSPictoManager::Instance(this);

    m_closeApplication = false;

}

void RSMainWindow::createConnections()
{
    //! brief : must update the sensor name list when the selection changes
    bool result =  connect(Signaler::instance(),SIGNAL(signal_emitMessage(QMessageBox::Icon , const QString& , const QString& , const QString&)),
                           this,SLOT(slotTreatMessage(QMessageBox::Icon , const QString& , const QString& , const QString& )));
    if(!result)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Failed to connect slotTreatMessage");
    }

    if(!connect(Signaler::instance(),SIGNAL(signal_closeAppli()),this,SLOT(slot_closeAppli())))
        RSLogger::instance()->info(Q_FUNC_INFO,"Failed to connect slot_closeAppli");
}

int RSMainWindow::quitConfirmation()
{ 
    QString m_title = tr("Quit Confirmation");
    QString m_msgTitle = tr("<font style=\"color:lime; font-weight: bold;\">"
                            "%1").arg(m_title);
    QString m_msg = tr("<font style=\"color:%1; font-weight: bold;\">"
                       "Are you sure you want to quit ?"
                       "</font>");

    QMessageBox m_message(this);
    m_message.setWindowTitle(m_title);
    m_message.setIcon(QMessageBox::Information);
    m_message.setText(m_msgTitle);
    m_message.setInformativeText(m_msg);
    m_message.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    m_message.setCursor(Qt::PointingHandCursor);

    QSpacerItem* m_spacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* m_layout = (QGridLayout*)m_message.layout();
    m_layout->addItem(m_spacer, m_layout->rowCount(), 0, 1, m_layout->columnCount());

    int response = QMessageBox::Yes;

    if(m_closeApplication == false)
    {
        response = m_message.exec();
    }


    return response;
}

void RSMainWindow::closeEvent(QCloseEvent *event)
{
    int m_quitConfirmation = quitConfirmation();

    if(m_quitConfirmation == QMessageBox::Yes)
    {
        emit Signaler::instance()->signal_quit();

        RSDatabaseAccess::Instance()->close();
        qApp->closeAllWindows();
        exit(0);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void RSMainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Ok|QMessageBox::NoButton);

        //emit signal exit loop
        if(msgBox.exec() == QMessageBox::Yes)
            emit Signaler::instance()->signal_cancel();
    }
}

void RSMainWindow::slotInitializeSystem()
{
    RSLogger::instance()->info(Q_FUNC_INFO, "Start");

    //--- --Open the database
    RSLogger::instance()->info(Q_FUNC_INFO, "open database");

    //Show the configuration interface, so the user can modify the path
    RSDatabaseAccess::Instance()->showDatabaseConfig();

    //open the database.
    if(!RSDatabaseAccess::Instance()->open())
        slot_closeAppli() ;
    else
        RSDatabaseAccess::Instance()->saveSettings(QString());
    RSMessageView::Instance()->showData("------ Database initialization suceeded -------------\n");

    //--- --Open the database
    RSLogger::instance()->info(Q_FUNC_INFO, "createDatasetTable");
    RSDatabaseAccess::Instance()->createDatasetTable();

    RSLogger::instance()->info(Q_FUNC_INFO, "setDatasetTable");
    RSDatabaseAccess::Instance()->setDatasetTable();

    RSLogger::instance()->info(Q_FUNC_INFO, "setFilterFields");
    m_filtersManager->setFilterFields();

    RSLogger::instance()->info(Q_FUNC_INFO, "setSensorNameList");
    m_graphView->setSensorNameList();

    emit Signaler::instance()->signal_readyToStart();

    RSLogger::instance()->info(Q_FUNC_INFO, "End. Emit signal_readyToStart");
}

void RSMainWindow::slotTreatMessage(QMessageBox::Icon icon, const QString& color, const QString& title, const QString& msg)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start. title = " + title + " msg = " + msg);
    treatMessage(icon, color, title,  msg);
    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSMainWindow::treatMessage(QMessageBox::Icon icon, const QString& color, const QString& title, const QString& msg)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start. title = " + title + " msg = " + msg);
    QMessageBox m_message(this);
    m_message.setWindowTitle(title);
    m_message.setIcon(icon);
    m_message.setText(tr("<font style=\"color:%1; font-weight: bold;\">%2</font>").arg(color).arg(title));
    m_message.setInformativeText(msg);
    m_message.setStandardButtons(QMessageBox::Ok);
    m_message.setCursor(Qt::PointingHandCursor);

    QSpacerItem* m_spacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* m_layout = (QGridLayout*)m_message.layout();
    m_layout->addItem(m_spacer, m_layout->rowCount(), 0, 1, m_layout->columnCount());

    m_message.exec();

    if(icon == QMessageBox::Critical)
    {
        m_closeApplication = true;
        //close();
    }

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

void RSMainWindow::slotConfigureDatabase()
{

}

void RSMainWindow::loadSettings(const QString& fileName)
{

}

void RSMainWindow::saveSettings(const QString& fileName) const
{

}

void  RSMainWindow::slot_closeAppli()
{    
    this->close();
}
