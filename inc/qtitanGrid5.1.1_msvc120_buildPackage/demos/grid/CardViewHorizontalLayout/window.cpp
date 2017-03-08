
#include <QtGui>
#include <QDirModel>
#include <QtSql>
#include <QMessageBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QtitanGrid.h>

#include "window.h"

Window::Window()
    : DemoMainWindow("QtitanDataGrid", QTN_VERSION_DATAGRID_STR)
{
    setWindowTitle(tr("QtitanDataGrid - CardViewHorizontalLayout"));
    setGeometry(150, 150, 1200, 700);

    Grid::loadTranslation();

    m_grid = new Qtitan::Grid();

    QString prefix = QApplication::applicationDirPath();
    prefix += "/..";

    QSqlError err;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", QString("database_demo"));

    db.setDatabaseName(prefix + "/demos/data/database_yachts.sqlite");
    db.setHostName("");
    db.setPort(-1);
    if (!db.open("", ""))
    {
        err = db.lastError();
        QSqlDatabase::removeDatabase(QString("database_demo"));
        QMessageBox::critical(0, "Demo Error", QString("Error: Can't open database - %1, error - %2").arg(prefix + "/demos/data/database.sqlite").arg(err.text()));
        QApplication::exit(1);
        return;
    }

    QSqlTableModel* model = new QSqlTableModel(m_grid, db);
    model->setTable("data");
    model->select();
    if (model->lastError().type() != QSqlError::NoError)
    {
        QMessageBox::critical(0, "Demo Error", "Error: SQL data base is not valid.");
        QApplication::exit(1);
        return;
    }


    // Configure grid view
    m_grid->setViewType(Qtitan::Grid::CardView);
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setScrollRowStyle(ScrollByPixel);
    view->options().setCellWidth(250);

    view->options().setViewOrientation(Qt::Horizontal);

    //Connect Grid's context menu handler.
    connect(view, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs*)));

    view->setModel(model);

    Qtitan::GridCardColumn* column = (Qtitan::GridCardColumn *)view->getColumnByModelColumnName("Photo");
    column->setRowSpan(5);
    column->setEditorType(Qtitan::PictureEditorType);

    column = (Qtitan::GridCardColumn *)view->getColumnByModelColumnName("Description");
    column->setEditorType(Qtitan::MemoEditorType);
    column->setRowSpan(4);
    //Add cell button to the column.
    column->addButton(Qtitan::ChoiceButton, Qtitan::RightPosition);
    connect(column, SIGNAL(buttonClicked(CellButtonClickEventArgs*)), this, SLOT(cellButtonClicked(CellButtonClickEventArgs*)));

    column = (Qtitan::GridCardColumn *)view->getColumnByModelColumnName("Built Year");
    column->setEditorType(Qtitan::DateEditorType);
    static_cast<GridDateEditorRepository *>(column->editorRepository())->setDisplayFormat("yyyy");

    column = (Qtitan::GridCardColumn *)view->getColumnByModelColumnName("id");
    view->removeColumn(column->index());

    //Show button menu for all column headers.
    for (int i = 0; i < view->getColumnCount(); ++i)
        static_cast<GridTableColumn *>(view->getColumn(i))->setMenuButtonVisible(true);

    setDemoWidget(m_grid, createSettingsWidget());


    column = (Qtitan::GridCardColumn *)view->getColumnByModelColumnName("Registry");
    column->setGroupIndex(0);

    view->bestFit();
    view->expandAll();    
}

QWidget* Window::createSettingsWidget()
{
    //Create settings widget
    QWidget* settings = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(settings);
 
    QCheckBox* fastScrollCheck = new QCheckBox(settings);
    fastScrollCheck->setText("Fast scroll effect");
    connect(fastScrollCheck, SIGNAL(stateChanged(int)), this, SLOT(fastScrollChanged(int)));
    l->addWidget(fastScrollCheck);
    fastScrollCheck->setChecked(true);

    QCheckBox* dottedLineCheck = new QCheckBox(settings);
    dottedLineCheck->setText("Dotted grid line");
    connect(dottedLineCheck, SIGNAL(stateChanged(int)), this, SLOT(dottedLineChanged(int)));
    l->addWidget(dottedLineCheck);
    dottedLineCheck->setChecked(true);

    QLabel* label = new QLabel(this);
    QHBoxLayout* hl = new QHBoxLayout(0);
    label->setText("Grid line style:");
    QComboBox* lineStylesSelect = new QComboBox(settings);

    lineStylesSelect->addItem("None");
    lineStylesSelect->addItem("Both");
    lineStylesSelect->addItem("Both2D");
    lineStylesSelect->addItem("Horizontal");
    lineStylesSelect->addItem("Horizontal2D");
    lineStylesSelect->addItem("Vertical");
    lineStylesSelect->addItem("Vertical2D");
    connect(lineStylesSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(selectGridLineStyles(int)));
    hl->addWidget(label);
    hl->addWidget(lineStylesSelect);
    l->addLayout(hl);
    lineStylesSelect->setCurrentIndex(2);

    QLabel* alignLabel = new QLabel(this);
    QHBoxLayout* alignLayout = new QHBoxLayout(0);
    alignLabel->setText("Items alignement:");
    QComboBox* alignSelect = new QComboBox(settings);

    alignSelect->addItem("Top");
    alignSelect->addItem("Center");
    alignSelect->addItem("Bottom");
    connect(alignSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(alignSelectChanged(int)));
    alignLayout->addWidget(alignLabel);
    alignLayout->addWidget(alignSelect);
    l->addLayout(alignLayout);
    alignSelect->setCurrentIndex(0);


    QHBoxLayout* l2 = new QHBoxLayout(0);
    QPushButton* button1 = new QPushButton(settings);
    button1->setText(tr("Save Layout"));
    connect(button1, SIGNAL(clicked()), this, SLOT(saveLayout()));
    l2->addWidget(button1);
    QPushButton* button2 = new QPushButton(settings);
    button2->setText(tr("Load Layout"));
    connect(button2, SIGNAL(clicked()), this, SLOT(loadLayout()));
    l2->addWidget(button2);
    l->addLayout(l2);

    QCheckBox* zoomEnable = new QCheckBox(settings);
    zoomEnable->setText(tr("Zoom enabled"));
    zoomEnable->setChecked(true);
    connect(zoomEnable, SIGNAL(stateChanged(int)), this, SLOT(zoomEnabledChanged(int)));
    l->addWidget(zoomEnable);

    QCheckBox* zoomIndicator = new QCheckBox(settings);
    zoomIndicator->setText(tr("Show zoom indicator"));
    zoomIndicator->setChecked(true);
    connect(zoomIndicator, SIGNAL(stateChanged(int)), this, SLOT(zoomIndicatorChanged(int)));
    l->addWidget(zoomIndicator);
    
    QSlider* zoomSlider = new QSlider(settings);
    zoomSlider->setOrientation(Qt::Horizontal);
    zoomSlider->setTickPosition(QSlider::TicksBothSides);
    zoomSlider->setMinimum(25);
    zoomSlider->setMaximum(300);
    zoomSlider->setTickInterval(25);
    zoomSlider->setSingleStep(25);
    zoomSlider->setValue(100);
    connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(zoomValueChanged(int)));
    connect(m_grid->view<Qtitan::GridCardView>(), SIGNAL(zoomChanged(int)), zoomSlider, SLOT(setValue(int)));
    l->addWidget(zoomSlider);

    QCheckBox* airCheckBox = new QCheckBox(settings);
    airCheckBox->setText(tr("Windows Air Support"));
    connect(airCheckBox, SIGNAL(stateChanged(int)), this, SLOT(WindowsAirSupportChanged(int)));
    airCheckBox->setChecked(false);
    l->addWidget(airCheckBox);

    QCheckBox* cellAutoRaise = new QCheckBox(settings);
    cellAutoRaise->setText(tr("Auto raise cell button"));
    connect(cellAutoRaise, SIGNAL(stateChanged(int)), this, SLOT(cellButtonAutoRaiseEnabled(int)));
    cellAutoRaise->setChecked(true);
    l->addWidget(cellAutoRaise);

    QCheckBox* transparentBox = new QCheckBox(settings);
    transparentBox->setText(tr("Transparent Background"));
    connect(transparentBox, SIGNAL(stateChanged(int)), this, SLOT(transparentBackgroundEnabled(int)));
    transparentBox->setChecked(false);
    l->addWidget(transparentBox);

    QPushButton* printButton = new QPushButton(settings);
    printButton->setText(tr("Print Preview"));
    connect(printButton, SIGNAL(clicked()), this, SLOT(printPreview()));
    l->addWidget(printButton);

    return settings;
}

void Window::setShadeColor(const QColor& color)
{
    m_grid->themeManager()->setShadeColor(color);
}

void Window::fastScrollChanged(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setFastScrollEffect(state == Qt::Checked);
}

void Window::dottedLineChanged(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    QPen pen = view->options().gridLinePen();
    pen.setStyle(state == Qt::Checked ? Qt::DotLine : Qt::SolidLine);
    view->options().setGridLinePen(pen);
}

void Window::selectGridLineStyles(int index)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    switch (index)
    {
    case 0:
        view->options().setGridLines(Qtitan::LinesNone);
        break;
    case 1:
        view->options().setGridLines(Qtitan::LinesBoth);
        break;
    case 2:
        view->options().setGridLines(Qtitan::LinesBoth2D);
        break;
    case 3:
        view->options().setGridLines(Qtitan::LinesHorizontal);
        break;
    case 4:
        view->options().setGridLines(Qtitan::LinesHorizontal2D);
        break;
    case 5:
        view->options().setGridLines(Qtitan::LinesVertical);
        break;
    case 6:
        view->options().setGridLines(Qtitan::LinesVertical2D);
        break;
    default:
        view->options().setGridLines(Qtitan::LinesBoth);
    }
}

void Window::alignSelectChanged(int index)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    switch (index)
    {
    case 0:
        view->options().setItemAlignment(Qt::AlignTop);
        break;
    case 1:
        view->options().setItemAlignment(Qt::AlignCenter);
        break;
    case 2:
        view->options().setItemAlignment(Qt::AlignBottom);
        break;
    default:
        view->options().setItemAlignment(Qt::Alignment());
    }
}

void Window::selectViewOrientation(int index)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    switch (index)
    {
    case 0:
    {
        view->options().setViewOrientation(Qt::Vertical);

        Qtitan::GridBandedTableColumn* column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Aircraft");
        column->setRowIndex(1);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Location");
        column->setRowIndex(1);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Date");
        column->setRowIndex(2);
    }
    break;
    case 1:
    {
        view->options().setViewOrientation(Qt::Horizontal);

        Qtitan::GridBandedTableColumn* column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Aircraft");
        column->setRowIndex(0);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Location");
        column->setRowIndex(0);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Date");
        column->setRowIndex(0);
    }
    break;
    default:
        break;
    }
}

void Window::saveLayout()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save QtitanDataGrid Layout to File"), "qtitan-layout.xml", tr("Layout-XML (*.xml)"));
    if (fileName.isEmpty())
        return;
    m_grid->saveLayoutToFile(fileName);
}

void Window::loadLayout()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Open QtitanDataGrid Layout File"), "", tr("Layout-XML (*.xml)"));
    if (files.size() != 1 || files[0].isEmpty())
        return;
    m_grid->loadLayoutFromFile(files[0]);
}


void Window::zoomEnabledChanged(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setZoomEnabled(state == Qt::Checked);
}

void Window::zoomIndicatorChanged(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setZoomIndicatorActive(state == Qt::Checked);
}

void Window::zoomValueChanged(int value)
{
    double factor = qCeil((double)value / 25) * 25;
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setZoomFactor(factor / 100);
}

void Window::WindowsAirSupportChanged(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setWindowsAirSupported(state == Qt::Checked);
}

void Window::cellButtonAutoRaiseEnabled(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setCellButtonAutoRaise(state == Qt::Checked);
}

void Window::transparentBackgroundEnabled(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setTransparentBackground(state == Qt::Checked);

    view->options().setAlternatingRowColors(!view->options().alternatingRowColors());
}

void Window::contextMenu(ContextMenuEventArgs* args)
{
    args->contextMenu()->addAction("Print Preview", this, SLOT(printPreview()));
    args->contextMenu()->addSeparator();
    args->contextMenu()->addAction("Developer Machines on the Web", this, SLOT(showCompanyWebSite()));
}

void Window::cellButtonClicked(CellButtonClickEventArgs* args)
{
    QMessageBox::information(this, tr("Cell button clicked"),
        tr("Clicked: Button - %1, Column Title - %2, RowIndex - %3").arg(args->buttonIndex()).arg(args->column()->caption()).arg(args->row()->rowIndex()));
}

void Window::cellButtonStateChanged(CellButtonEventArgs* args)
{
    if (args->row()->rowIndex() == 1 || args->row()->rowIndex() == 2)
        args->setEnadled(false);
}

void Window::printPreview()
{
    m_grid->view<Qtitan::GridCardView>()->printPreview();
}

void Window::showCompanyWebSite()
{
    QDesktopServices::openUrl(QUrl("http://www.devmachines.com"));
}

