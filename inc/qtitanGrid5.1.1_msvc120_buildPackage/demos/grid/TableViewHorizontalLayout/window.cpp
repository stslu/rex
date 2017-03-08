
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
    setWindowTitle(tr("QtitanDataGrid - Horizontal BandedTableView"));
    setGeometry(150, 150, 1200, 700);

    Grid::loadTranslation();

    m_grid = new Qtitan::Grid();

    QString prefix = QApplication::applicationDirPath();
    prefix += "/..";

    QSqlError err;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", QString("database_demo"));

    db.setDatabaseName(prefix + "/demos/data/database.sqlite");
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
    m_grid->setViewType(Qtitan::Grid::BandedTableView);
    Qtitan::GridBandedTableView* view = m_grid->view<Qtitan::GridBandedTableView>();
    QPen pen = view->options().gridLinePen();
    pen.setStyle(Qt::DotLine);
    view->options().setGridLinePen(pen);
    view->options().setViewOrientation(Qt::Horizontal);

    //Connect Grid's context menu handler.
    connect(view, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs*)));

    Qtitan::GridTableBand* characteristicsBand = view->addBand("Characteristics");
    characteristicsBand->setIcon(QPixmap(":/res/qtitanlogo64x64.png"));
    characteristicsBand->setIconApperance(Qtitan::IconAlignLeft);

    Qtitan::GridTableBand* visualBand = view->addBand("Visual");
    Qtitan::GridTableBand* descriptionBand = view->addBand("Description");


    view->setModel(model);

    Qtitan::GridBandedTableColumn* column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Photo");
    column->setEditorType(Qtitan::PictureEditorType);
    column->setIcon(QPixmap(":/res/qtitanlogo64x64.png"));
    Qtitan::GridPictureEditorRepository* pictureEditor = (Qtitan::GridPictureEditorRepository *)column->editorRepository();
    Q_UNUSED(pictureEditor);
    column->setBandIndex(visualBand->index());
    column->setRowSpan(3);
    column->setRowIndex(0);

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("History");
    column->setEditorType(Qtitan::MemoEditorType);
    Qtitan::GridMemoEditorRepository* memoEditor = (Qtitan::GridMemoEditorRepository *)column->editorRepository();
    column->setBandIndex(descriptionBand->index());
    column->setRowSpan(3);
    column->setRowIndex(0);

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Info");
    column->setEditorType(Qtitan::MemoEditorType);
    memoEditor = (Qtitan::GridMemoEditorRepository *)column->editorRepository();
    column->setBandIndex(descriptionBand->index());
    column->setRowSpan(3);
    column->setRowIndex(0);

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Registration");
    column->setBandIndex(characteristicsBand->index());
    column->setRowIndex(0);

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Aircraft");
    column->setIcon(QPixmap(":/res/qtitanlogo64x64.png"));
    column->setBandIndex(characteristicsBand->index());
    column->setRowSpan(2);
    column->setRowIndex(0);

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Location");
    column->setIcon(QPixmap(":/res/qtitanlogo64x64.png"));
    column->setBandIndex(characteristicsBand->index());
    column->setRowIndex(0);

    //Add cell button to the column.
    column->addButton(Qtitan::ClearButton, Qtitan::RightPosition, Qtitan::MouseOverPolicy, QIcon(), "Text here");
    connect(column, SIGNAL(buttonClicked(CellButtonClickEventArgs*)), this, SLOT(cellButtonClicked(CellButtonClickEventArgs*)));

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Date");
    column->setBandIndex(characteristicsBand->index());
    column->setRowIndex(0);
    column->addButton(ClearButton, RightPosition, MouseOverPolicy);
    connect(column, SIGNAL(buttonClicked(CellButtonClickEventArgs*)), this, SLOT(cellButtonClicked(CellButtonClickEventArgs*)));
    connect(column, SIGNAL(buttonStateChanged(CellButtonEventArgs*)), this, SLOT(cellButtonStateChanged(CellButtonEventArgs*)));

    //Show button menu for all column headers.
    for (int i = 0; i < view->getColumnCount(); ++i)
        static_cast<GridTableColumn *>(view->getColumn(i))->setMenuButtonVisible(true);

    setDemoWidget(m_grid, createSettingsWidget());

    view->bestFit();
}

QWidget* Window::createSettingsWidget()
{
    //Create settings widget
    QWidget* settings = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(settings);
    QCheckBox* autoWidthCheck = new QCheckBox(settings);
    autoWidthCheck->setText("Column auto width");
    connect(autoWidthCheck, SIGNAL(stateChanged(int)), this, SLOT(autoWidthStateChanged(int)));
    l->addWidget(autoWidthCheck);
    autoWidthCheck->setChecked(true);

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

    QHBoxLayout* l2 = new QHBoxLayout(0);
    QPushButton* button1 = new QPushButton(settings);
    button1->setText(tr("Save Layout"));
    connect(button1, SIGNAL(clicked()), this, SLOT(saveLayout()));
    l2->addWidget(button1);
    QPushButton* button2 = new QPushButton(settings);
    button2->setText(tr("Load Layout"));
    connect(button2, SIGNAL(clicked()), this, SLOT(loadLayout()));
    l2->addWidget(button2);

    hl->addWidget(label);
    hl->addWidget(lineStylesSelect);
    l->addLayout(hl);
    l->addLayout(l2);
    lineStylesSelect->setCurrentIndex(2);

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
    connect(m_grid->view<Qtitan::GridTableView>(), SIGNAL(zoomChanged(int)), zoomSlider, SLOT(setValue(int)));
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

    QCheckBox* frozenRowsBox = new QCheckBox(settings);
    frozenRowsBox->setText(tr("Frozen Rows"));
    connect(frozenRowsBox, SIGNAL(stateChanged(int)), this, SLOT(frozenRowsEnabled(int)));
    frozenRowsBox->setChecked(true);
    l->addWidget(frozenRowsBox);

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

void Window::autoWidthStateChanged(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setColumnAutoWidth(state == Qt::Checked);
}

void Window::fastScrollChanged(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setFastScrollEffect(state == Qt::Checked);
}

void Window::dottedLineChanged(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    QPen pen = view->options().gridLinePen();
    pen.setStyle(state == Qt::Checked ? Qt::DotLine : Qt::SolidLine);
    view->options().setGridLinePen(pen);
}

void Window::selectGridLineStyles(int index)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
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

void Window::selectViewOrientation(int index)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
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
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setZoomEnabled(state == Qt::Checked);
}

void Window::zoomIndicatorChanged(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setZoomIndicatorActive(state == Qt::Checked);
}

void Window::zoomValueChanged(int value)
{
    double factor = qCeil((double)value / 25) * 25;
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setZoomFactor(factor / 100);
}

void Window::WindowsAirSupportChanged(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setWindowsAirSupported(state == Qt::Checked);
}

void Window::cellButtonAutoRaiseEnabled(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setCellButtonAutoRaise(state == Qt::Checked);
}

void Window::frozenRowsEnabled(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setRowFrozenButtonVisible(state == Qt::Checked);
    view->options().setFrozenPlaceQuickSelection(state == Qt::Checked);
}

void Window::transparentBackgroundEnabled(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
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
    m_grid->view<Qtitan::GridTableView>()->printPreview();
}

void Window::showCompanyWebSite()
{
    QDesktopServices::openUrl(QUrl("http://www.devmachines.com"));
}

