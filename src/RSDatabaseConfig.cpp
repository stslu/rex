#include "RSDatabaseConfig.h"
#include "ui_RSDatabaseConfig.h"
#include "RSPictoManager.h"
#include "RSDataManager.h"
#include <QFileDialog>
#include <RSLogger.h>
#include "RSGlobalMethods.h"

RSDatabaseConfig::RSDatabaseConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RSDatabaseConfig)
{
    ui->setupUi(this);

    ui->m_g6PasswordEdit->setEchoMode(QLineEdit::Password);
    ui->m_g7PasswordEdit->setEchoMode(QLineEdit::Password);

    createObjects();

    createConnections();
}

RSDatabaseConfig::~RSDatabaseConfig()
{

    delete ui;
}

void RSDatabaseConfig::createConnections()
{
    connect(ui->m_g6DatabaseButton, SIGNAL(clicked()), this, SLOT(slotG6DatabaseButtonClicked()));
    connect(ui->m_g7DatabaseButton, SIGNAL(clicked()), this, SLOT(slotG7DatabaseButtonClicked()));
}


void RSDatabaseConfig::createObjects()
{

    //!G6 database
    ui->m_g6DatabaseEdit->setText(RSDataManager::Instance()->getData("G6DatabaseFile").toString());
    ui->m_g6DatabaseButton->setIcon(RSPictoManager::Instance()->getIcon(fa::file, "black"));

    //!G7 database
    ui->m_g7DatabaseEdit->setText(RSDataManager::Instance()->getData("G7DatabaseFile").toString());
    ui->m_g7DatabaseButton->setIcon(RSPictoManager::Instance()->getIcon(fa::file, "black"));

    //!Load loadNodesWithNoAst
    if(RSDataManager::Instance()->getData("loadNodesWithNoAst").toBool())
    {
        RSLogger::instance()->info(Q_FUNC_INFO, "loadNodesWithNoAst = TRUE ");
        ui->loadNodeWithNoAstYes->setChecked(true);
    }
    else
    {
         RSLogger::instance()->info(Q_FUNC_INFO, "loadNodesWithNoAst = FALSE ");
        ui->loadNodeWithNoAstYes->setChecked(false);
    }

    //!Load LoadDeadEntities
    if(RSDataManager::Instance()->getData("LoadDeadEntities").toBool())
    {
        RSLogger::instance()->info(Q_FUNC_INFO, "LoadDeadEntities = TRUE ");
        ui->loadDeadEntitiesYes->setChecked(true);
    }
    else
    {
        RSLogger::instance()->info(Q_FUNC_INFO, "LoadDeadEntities = FALSE ");
        ui->loadDeadEntitiesYes->setChecked(false);
    }

}

bool RSDatabaseConfig::loadDeadEntities() const
{

    return ui->loadDeadEntitiesYes->isChecked();
}

bool RSDatabaseConfig::loadNodesWithNoAst() const
{
    return ui->loadNodeWithNoAstYes->isChecked();
}


QString RSDatabaseConfig::getG6DatabaseFile() const
{
    return ui->m_g6DatabaseEdit->text();
}

QString RSDatabaseConfig::getG7DatabaseFile() const
{
    return ui->m_g7DatabaseEdit->text();
}

void RSDatabaseConfig::slotG6DatabaseButtonClicked()
{
    QString m_filter = "Firebird DB Files (*.FDB)";

    QString m_path = QDir::homePath();

    if(ui->m_g6DatabaseEdit->text().isEmpty() == false)
    {
        m_path = ui->m_g6DatabaseEdit->text();
    }

    QString m_filename = QFileDialog::getOpenFileName(this, "Select G6 Database File...", m_path, m_filter);

    if(m_filename.isEmpty() == false)
    {
        ui->m_g6DatabaseEdit->setText(m_filename);
    }
}

void RSDatabaseConfig::slotG7DatabaseButtonClicked()
{
    QString m_filter = "Firebird DB Files (*.FDB)";
    QString m_path = QDir::homePath();

    if(ui->m_g7DatabaseEdit->text().isEmpty() == false)
    {
        m_path = ui->m_g7DatabaseEdit->text();
    }

    QString m_filename = QFileDialog::getOpenFileName(this, "Select G7 Database File...", m_path, m_filter);

    if(m_filename.isEmpty() == false)
    {
        ui->m_g7DatabaseEdit->setText(m_filename);
    }
}

void RSDatabaseConfig::setMessage(const QString& msg)
{
    ui->m_title->setText(msg);
}

void RSDatabaseConfig::setG7Path(const QString& path)
{
    ui->m_g7DatabaseEdit->setText(path);
}

void RSDatabaseConfig::setG6Path(const QString& path)
{
    ui->m_g6DatabaseEdit->setText(path);
}

void RSDatabaseConfig::setG7Login(const QString& path,const QString& userName,const QString& pwd )
{
    ui->m_g7DatabaseEdit->setText(path);
    ui->m_g7PasswordEdit->setText(pwd);
    ui->m_g7UserNameEdit->setText(userName);
}

void RSDatabaseConfig::setG6Login(const QString& path,const QString& userName,const QString& pwd )
{
    ui->m_g6DatabaseEdit->setText(path);
    ui->m_g6PasswordEdit->setText(pwd);
    ui->m_g6UserNameEdit->setText(userName);
}

void RSDatabaseConfig::setLoadDeadEntitiesOption(bool checked)
{

    if(checked)
    {
        ui->loadDeadEntitiesYes->setChecked(true);
        ui->loadDeadEntitiesNo->setChecked(false);
    }
    else
    {
        ui->loadDeadEntitiesYes->setChecked(false);
        ui->loadDeadEntitiesNo->setChecked(true);
    }
}

void RSDatabaseConfig::setLoadNodeswithNoAstOption(bool checked)
{
    if(checked)
    {
        ui->loadNodeWithNoAstYes->setChecked(true);
        ui->loadNodeWithNoAstNo->setChecked(false);
    }
    else
    {
        ui->loadNodeWithNoAstYes->setChecked(false);
        ui->loadNodeWithNoAstNo->setChecked(true);
    }
}

QString RSDatabaseConfig::getG6UserName() const
{
    return ui->m_g6UserNameEdit->text();
}

QString RSDatabaseConfig::getG7UserName() const
{
    RSLogger::instance()->info(Q_FUNC_INFO,"ui->m_g7UserNameEdit->text() = " + ui->m_g7UserNameEdit->text());
    return ui->m_g7UserNameEdit->text();
}

QString RSDatabaseConfig::getG6Pwd() const
{
    return  ui->m_g6PasswordEdit->text();
}

QString RSDatabaseConfig::getG7Pwd() const
{
    return  ui->m_g7PasswordEdit->text();
}
