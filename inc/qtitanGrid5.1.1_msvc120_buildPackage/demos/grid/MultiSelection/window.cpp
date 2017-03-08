
#include <QtGui>
#include <QDirModel>
#include <QtSql>
#include <QDesktopServices>
#include <QMessageBox>

#include "window.h"

Window::Window()
: DemoMainWindow("QtitanDataGrid", QTN_VERSION_DATAGRID_STR)
{
    setWindowTitle(tr("QtitanDataGrid Demo (Multi-selection)"));
    setGeometry(150, 150, 1000, 800);

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
    model->setEditStrategy(QSqlTableModel::OnFieldChange);

    // Configure grid view
    m_grid->setViewType(Qtitan::Grid::BandedTableView);
    Qtitan::GridBandedTableView* view = m_grid->view<Qtitan::GridBandedTableView>();
    view->options().setShowFocusDecoration(true);
    view->options().setColumnAutoWidth(true);

    //Connect Grid's context menu handler.
    connect(view, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs* )));

    Qtitan::GridTableBand* characteristicsBand = view->addBand("Characteristics");
    Qtitan::GridTableBand* engineeringBand = view->addBand("Engineering");
    
    view->setModel(model);
    
    Qtitan::GridBandedTableColumn* column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("id");
    column->setBandIndex(characteristicsBand->index());
    column->setRowSpan(2);
    column->addButton(Qtitan::ClearButton, Qtitan::LeftPosition, FluentFixedPolicy);

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Airline");
    column->setBandIndex(characteristicsBand->index());
    column->setRowSpan(2);

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Registration");
    column->setBandIndex(characteristicsBand->index());
    column->setRowSpan(2);

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Aircraft");
    column->setBandIndex(characteristicsBand->index());
    column->setRowSpan(2);

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Location");
    column->setBandIndex(characteristicsBand->index());
    column->setRowSpan(2);

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Date");
    column->setBandIndex(characteristicsBand->index());
    column->setRowSpan(2);

    //Add cell button to the column.
    column->addButton(Qtitan::ClearButton, Qtitan::RightPosition, MouseOverPolicy);
    connect(column, SIGNAL(buttonClicked(CellButtonClickEventArgs*)), this, SLOT(cellButtonClicked(CellButtonClickEventArgs*)));

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Photo");
    column->setEditorType(Qtitan::ComboBoxPictureEditorType);
    column->setBandIndex(engineeringBand->index());
    column->setRowSpan(2);

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("History");
    column->setEditorType(Qtitan::MemoEditorType);
    column->setBandIndex(engineeringBand->index());
    column->setRowSpan(2);

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Info");
    column->setEditorType(Qtitan::MemoEditorType);
    column->setBandIndex(engineeringBand->index());
    column->setRowSpan(2);
    
    //Show button menu for all column headers.
    for (int i = 0; i < view->getColumnCount(); ++i)
        static_cast<GridTableColumn *>(view->getColumn(i))->setMenuButtonVisible(true);

    setDemoWidget(m_grid, createSettingsWidget());
}

QWidget* Window::createSettingsWidget()
{
    //Create settings widget
    QWidget* settings = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(settings);

    QHBoxLayout* hl = new QHBoxLayout(0);
    QLabel* label = new QLabel(this);
    label->setText("Multi-select mode:");
    QComboBox* policyBox = new QComboBox(this);
    policyBox->addItem("Single Row");
    policyBox->addItem("Single Cell");
    policyBox->addItem("Multi Rows");
    policyBox->addItem("Multi Rows (Using RubberBand)");
    policyBox->addItem("Multi Cells");
    policyBox->addItem("Multi Cells (Using RubberBand)");
    connect(policyBox, SIGNAL(currentIndexChanged(int)), this, SLOT(newPolicyActivated(int)));
    hl->addWidget(label);
    hl->addWidget(policyBox);
    l->addLayout(hl);

    QPushButton* showSelectedButton = new QPushButton(settings);
    showSelectedButton->setText("Show Selected");
    connect(showSelectedButton, SIGNAL(clicked()), this, SLOT(showSelected()));
    l->addWidget(showSelectedButton);

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

    label = new QLabel(this);
    hl = new QHBoxLayout(0);
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
    lineStylesSelect->setCurrentIndex(5);

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

    QCheckBox* rowsQuickSelectBox = new QCheckBox(settings);
    rowsQuickSelectBox->setText(tr("Rows Quick Selection"));
    connect(rowsQuickSelectBox, SIGNAL(stateChanged(int)), this, SLOT(rowsQuickSelectEnabled(int)));
    rowsQuickSelectBox->setChecked(true);
    l->addWidget(rowsQuickSelectBox);

    QCheckBox* transparentBox = new QCheckBox(settings);
    transparentBox->setText(tr("Transparent Background"));
    connect(transparentBox, SIGNAL(stateChanged(int)), this, SLOT(transparentBackgroundEnabled(int)));
    transparentBox->setChecked(false);
    l->addWidget(transparentBox);
#if 0
    label = new QLabel(this);
    hl = new QHBoxLayout(0);
    label->setText(tr("View Orientation (new)"));
    QComboBox* viewOrientationSelect = new QComboBox(settings);
    viewOrientationSelect->addItem("Vertical");
    viewOrientationSelect->addItem("Horizontal");
    connect(viewOrientationSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(selectViewOrientation(int)));
    hl->addWidget(label);
    hl->addWidget(viewOrientationSelect);
    l->addLayout(hl);
    viewOrientationSelect->setCurrentIndex(0);
#endif
    QPushButton* printButton = new QPushButton(settings);
    printButton->setText(tr("Print Preview"));
    connect(printButton, SIGNAL(clicked()), this, SLOT(printPreview()));
    l->addWidget(printButton);

    policyBox->setCurrentIndex(3);

    return settings;
}

void Window::newPolicyActivated(int index)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    switch (index)
    {
    case 0:
        view->options().setSelectionPolicy(Qtitan::SelectSingleRow);
        view->options().setRowsQuickSelection(true);
        break;
    case 1:
        view->options().setSelectionPolicy(Qtitan::SelectSingleCell);
        view->options().setRowsQuickSelection(true);
        break;
    case 2:
        view->options().setSelectionPolicy(Qtitan::SelectMultiRow);
        view->options().setRubberBandSelection(false);
        view->options().setRowsQuickSelection(true);
        
        break;
    case 3:
        view->options().setSelectionPolicy(Qtitan::SelectMultiRow);
        view->options().setRubberBandSelection(true);
        view->options().setRowsQuickSelection(true);
        break;
    case 4:
        view->options().setSelectionPolicy(Qtitan::SelectMultiCell);
        view->options().setRubberBandSelection(false);
        view->options().setRowsQuickSelection(true);
        break;
    case 5:
        view->options().setSelectionPolicy(Qtitan::SelectMultiCell);
        view->options().setRubberBandSelection(true);
        view->options().setRowsQuickSelection(true);
        break;
    default:
        view->options().setSelectionPolicy(Qtitan::SelectSingleRow);
        view->options().setRowsQuickSelection(false);
    }
}

void Window::showSelected()
{
    QString text;
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    GridSelection* selection = view->selection();
    // Iteration by Rows
    while (!selection->end())
    {
        if (selection->row()->type() == GridRow::GroupRow)
        {
            QVariant v;
            selection->row()->groupValue(v);
            text += QString("[Group %1]\r\n").arg( v.toString());
            selection->next();
        }
        else
        {
            QString cellstext;
            GridRow* current = selection->row();
            // Iteration by Cells
            while (!selection->end() && current == selection->row())
            {
                if (!cellstext.isEmpty())
                    cellstext += ", ";
                cellstext += QString("%1").arg(selection->cell()->columnIndex());
                selection->next();
            }
            text += QString("[Row %1] cells(%2)\r\n").arg( current->rowIndex()).arg(cellstext);
        }
    }

    QTextEdit* edit = new QTextEdit();
    edit->setText(text);
    QDialog* dlg = new QDialog(this);
    QVBoxLayout* vbl = new QVBoxLayout(dlg);
    vbl->addWidget(edit);
    dlg->exec();
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
    }
    break;
    case 1:
    {
        view->options().setViewOrientation(Qt::Horizontal);
    }
    break;
    default:
        break;
    }
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

void Window::rowsQuickSelectEnabled(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setRowsQuickSelection(state == Qt::Checked);
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

void Window::setShadeColor(const QColor& color)
{
    m_grid->themeManager()->setShadeColor(color);
}

void Window::printPreview()
{
    m_grid->view<Qtitan::GridTableView>()->printPreview();
}

void Window::showCompanyWebSite()
{
    QDesktopServices::openUrl(QUrl("http://www.devmachines.com"));
}


