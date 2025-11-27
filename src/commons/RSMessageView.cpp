#include "RSMessageView.h"
#include "ui_RSMessageView.h"
#include "RSGlobalMethods.h"
#include "RSSettingsManager.h"

#include <QApplication>
#include <QSize>
#include <QGuiApplication>
#include <QScreen>



#include "RSLogger.h"
#include <QDebug>

RSMessageView* RSMessageView::m_instance = 0;

RSMessageView::RSMessageView(QWidget *parent) : QWidget(parent), ui(new Ui::RSMessageView),IRSMessageView(),m_nbLines(0)
{

    ui->setupUi(this);

    position();

    createObjects();
    createConnections();

    //this->adjustSize();
    QScreen *screen = this->screen(); // fonctionne en Qt 5.15 et Qt 6.10
    if (!screen)
        screen = QGuiApplication::primaryScreen();  // fallback au cas où

    if (screen) {
        const QRect screenRect = screen->geometry();  // ou availableGeometry() si tu veux ignorer la barre des tâches
        this->move(screenRect.center() - this->rect().center());
    }

    ui->m_runButton->setVisible(false);
}

RSMessageView::~RSMessageView()
{
     RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
    delete ui;
}

RSMessageView* RSMessageView::Instance(QWidget *parent)
{
    if(m_instance == 0)
    {
        m_instance = new RSMessageView(parent);
    }

    return m_instance;
}

void RSMessageView::deleteInstance()
{
    if(m_instance == 0)
    {
        delete m_instance;
        m_instance = 0;
    }
}

void RSMessageView::position() // Compatible Qt 5.15/6.10
{
    const QSize screenSize(600, 300);

    // Écran associé au widget si possible
    QScreen* screen = this->screen();
    if (!screen)
        screen = QGuiApplication::primaryScreen();   // fallback

    if (!screen) {
        // Cas très improbable : pas d'écran → on fait au plus simple
        setGeometry(0, 0, screenSize.width(), screenSize.height());
        show();
        return;
    }

    // Zone disponible (sans barre des tâches / dock)
    const QRect avail = screen->availableGeometry();

    const int x = avail.x() + avail.width()  - screenSize.width()  - 20;
    const int y = avail.y() + avail.height() - screenSize.height() - 50;
    const int w = screenSize.width();
    const int h = screenSize.height();

    setGeometry(x, y, w, h);
    show();
}

void RSMessageView::createObjects()
{

}

void RSMessageView::createConnections()
{
    connect(ui->m_runButton, SIGNAL(clicked()), this, SLOT(slotRunButtonClicked()));
    connect(ui->m_clearButton, SIGNAL(clicked()), this, SLOT(slotClearButtonClicked()));
}

void RSMessageView::separate()
{
    showData("--------------------------------------------------");
}

void RSMessageView::showData(const QString& data)
{
    if((m_nbLines + 1 % 1000) == 0)
        clear();
    else
        m_nbLines ++;

    m_nbLines++;
    ui->m_messageView->append(data);
    ui->m_messageView->update();

    QTextCursor c =  ui->m_messageView->textCursor();
    c.movePosition(QTextCursor::End);
    ui->m_messageView->setTextCursor(c);

    RSLogger::instance()->info(Q_FUNC_INFO,data);
}

void RSMessageView::showData(const QString& data, const QString& name)
{
    showData(tr("%1 : %2").arg(name).arg(data));

    qDebug() << tr("%1 : %2").arg(name).arg(data);
}

void RSMessageView::showData(int data, const QString& name)
{
    QString m_data = QString("%1").arg(data);
    showData(m_data, name);
}

void RSMessageView::showData(qint64 data, const QString& name)
{
    QString m_data = QString("%1").arg(data);
    showData(m_data, name);
}

void RSMessageView::showData(double data, const QString& name)
{
    QString m_data = QString("%1").arg(data, 0, 'g', 6);
    showData(m_data, name);
}

void RSMessageView::showData(double data, const QString& name, const QString& unit)
{
    QString m_data = QString("%1 %2").arg(data, 0, 'g', 6).arg(unit);
    showData(m_data, name);
}

void RSMessageView::showData(const QDate& data, const QString& name, const QString& format)
{
    QString m_format = data.toString(format);
    showData(m_format, name);
}

void RSMessageView::showData(const QDateTime& data, const QString& name, const QString& format)
{
    QString m_format = data.toString(format);
    showData(m_format, name);
}

void RSMessageView::showData(const QDate& date1, const QDate& date2, const QString& name, const QString& name1, const QString& name2, const QString& format)
{
    QString m_format1 = date1.toString(format);
    QString m_format2 = date2.toString(format);
    QString m_data = QString("%1: %2: %3 | %4: %5").arg(name).arg(name1).arg(m_format1).arg(name2).arg(m_format2);
    showData(m_data);
}

void RSMessageView::showData(const QDateTime& date1, const QDateTime& date2, const QString& name, const QString& name1, const QString& name2, const QString& format)
{
    QString m_format1 = date1.toString(format);
    QString m_format2 = date2.toString(format);
    QString m_data = QString("%1: %2: %3 | %4: %5").arg(name).arg(name1).arg(m_format1).arg(name2).arg(m_format2);
    showData(m_data);
}

void RSMessageView::showData(const QList<double>& data, const QString& name)
{

    for(int i = 0; i < data.size(); i++)
    {
        double m_xi = data.at(i);
        showData(QString("%1 [%2] : %3").arg(name).arg(i).arg(m_xi));
    }
}

void RSMessageView::showData(const QList<double>& data, const QString& name, int size)
{
    int m_size = qMin(data.size(), size);

    for(int i = data.size() - 1; i >= data.size() - m_size; i--)
    {
        double m_xi = data.at(i);
        showData(QString("%1 [%2] : %3").arg(name).arg(i).arg(m_xi));
    }
}

void RSMessageView::showData(const QStringList& data)
{
    foreach (QString m_data, data)
    {
        showData(m_data);
    }
}

void RSMessageView::slotRunButtonClicked()
{

}

void RSMessageView::slotClearButtonClicked()
{
    clear();
}
void RSMessageView::clear()
{
    ui->m_messageView->clear();
    m_nbLines = 0;
}


