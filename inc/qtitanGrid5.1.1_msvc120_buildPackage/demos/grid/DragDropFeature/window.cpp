
#include <QSplitter>
#include <QComboBox>
#include <QCheckBox>
#include <QUrl>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QMessageBox>
#include <qmath.h>

#include "window.h"

Window::Window()
: DemoMainWindow("QtitanDataGrid", QTN_VERSION_DATAGRID_STR)
{
    setWindowTitle(tr("QtitanDataGrid Drag & Drop Demo"));
    setGeometry(200, 250, 1000, 500);

    Grid::loadTranslation();

    m_grid1 = new Qtitan::Grid();
    m_grid1->setViewType(Qtitan::Grid::TableView);
    Qtitan::GridTableView* view1 = m_grid1->view<Qtitan::GridTableView>();
    view1->options().setDragEnabled(true);
    view1->options().setDropEnabled(true);

    //Connect Grid's context menu handler.
    connect(view1, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs* )));

    QStandardItemModel* model1 =  new QStandardItemModel(4, 4);
    for (int row = 0; row < 4; ++row)
    {
        for (int column = 0; column < 4; ++column)
        {
            QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
            model1->setItem(row, column, item);
        }
    }

     view1->setModel(model1);
     //m_grid1->setDragEnabled(true);

    m_grid2 = new Qtitan::Grid();
    m_grid2->setViewType(Qtitan::Grid::TableView);
    Qtitan::GridTableView* view2 = m_grid2->view<Qtitan::GridTableView>();
    view2->options().setDragEnabled(true);
    
    //Connect Grid's context menu handler.
    connect(view2, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs* )));

    QStandardItemModel* model2 =  new QStandardItemModel(4, 4);
    for (int row = 0; row < 4; ++row)
    {
        for (int column = 0; column < 4; ++column)
        {
            QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
            model2->setItem(row, column, item);
        }
    }
    view2->setModel(model2);

     m_grid3 = new QTreeView();

    QStandardItemModel* model3 =  new QStandardItemModel(4, 4);
    for (int row = 0; row < 4; ++row)
    {
        for (int column = 0; column < 4; ++column)
        {
            QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
            model3->setItem(row, column, item);
        }
    }

    m_grid3->setModel(model3);
    m_grid3->setDragDropMode(QAbstractItemView::DragDrop);
    m_grid3->setDropIndicatorShown(true);
    m_grid3->setDragDropOverwriteMode(false);

    QSplitter* s1 = new QSplitter(Qt::Horizontal, this);
    QSplitter* s2 = new QSplitter(Qt::Vertical, this);
    s1->addWidget(m_grid1);
    s1->addWidget(m_grid2);
    s2->addWidget(s1);
    s2->addWidget(m_grid3);

    //Create settings widget
    QWidget* settings = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(settings);

    QHBoxLayout* hl = new QHBoxLayout(0);
    QLabel* label = new QLabel(this);
    label->setText("Multi-select mode:");
    QComboBox* policyBox = new QComboBox(this);
    policyBox->addItem("Single Row");
    policyBox->addItem("Single Cell");
    policyBox->addItem("Multi Row");
    policyBox->addItem("Multi Row (Using RubberBand)");
    policyBox->addItem("Multi Cell");
    policyBox->addItem("Multi Cell (Using RubberBand)");
    connect(policyBox, SIGNAL(activated(int)), this, SLOT(newPolicyActivated(int)));
    hl->addWidget(label);
    hl->addWidget(policyBox);
    l->addLayout(hl);

    QCheckBox* fastScrollCheck = new QCheckBox(settings);
    fastScrollCheck->setText("Fast scroll effect");
    connect(fastScrollCheck, SIGNAL(stateChanged(int)), this, SLOT(fastScrollChanged(int)));
    l->addWidget(fastScrollCheck);
    fastScrollCheck->setChecked(true);

    QCheckBox* dottedLineCheck = new QCheckBox(settings);
    dottedLineCheck->setText("Dotted grid line");
    connect(dottedLineCheck, SIGNAL(stateChanged(int)), this, SLOT(dottedLineChanged(int)));
    l->addWidget(dottedLineCheck);
    dottedLineCheck->setChecked(false);

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
    connect(m_grid1->view<Qtitan::GridTableView>(), SIGNAL(zoomChanged(int)), zoomSlider, SLOT(setValue(int)));
    connect(m_grid2->view<Qtitan::GridTableView>(), SIGNAL(zoomChanged(int)), zoomSlider, SLOT(setValue(int)));
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
    frozenRowsBox->setText(tr("Frozen Rows (new)"));
    connect(frozenRowsBox, SIGNAL(stateChanged(int)), this, SLOT(frozenRowsEnabled(int)));
    frozenRowsBox->setChecked(true);
    l->addWidget(frozenRowsBox);

    QPushButton* printButton = new QPushButton(settings);
    printButton->setText(tr("Print Preview"));
    connect(printButton, SIGNAL(clicked()), this, SLOT(printPreview()));
    l->addWidget(printButton);

    setDemoWidget(s2, settings);
}

void Window::newPolicyActivated(int index)
{
    if (m_grid1 == Q_NULL || m_grid2 == Q_NULL)
        return;

    Qtitan::GridTableView* view1 = m_grid1->view<Qtitan::GridTableView>();
    Qtitan::GridTableView* view2 = m_grid2->view<Qtitan::GridTableView>();
    switch (index)
    {
    case 0:
        view1->options().setSelectionPolicy(Qtitan::SelectSingleRow);
        view2->options().setSelectionPolicy(Qtitan::SelectSingleRow);
        break;
    case 1:
        view1->options().setSelectionPolicy(Qtitan::SelectSingleCell);
        view2->options().setSelectionPolicy(Qtitan::SelectSingleCell);
        break;
    case 2:
        view1->options().setSelectionPolicy(Qtitan::SelectMultiRow);
        view1->options().setRubberBandSelection(false);
        view2->options().setSelectionPolicy(Qtitan::SelectMultiRow);
        view2->options().setRubberBandSelection(false);
        break;
    case 3:
        view1->options().setSelectionPolicy(Qtitan::SelectMultiRow);
        view1->options().setRubberBandSelection(true);
        view2->options().setSelectionPolicy(Qtitan::SelectMultiRow);
        view2->options().setRubberBandSelection(true);
        break;
    case 4:
        view1->options().setSelectionPolicy(Qtitan::SelectMultiCell);
        view1->options().setRubberBandSelection(false);
        view2->options().setSelectionPolicy(Qtitan::SelectMultiCell);
        view2->options().setRubberBandSelection(false);
        break;
    case 5:
        view1->options().setSelectionPolicy(Qtitan::SelectMultiCell);
        view1->options().setRubberBandSelection(true);
        view2->options().setSelectionPolicy(Qtitan::SelectMultiCell);
        view2->options().setRubberBandSelection(true);
        break;
    default:
        view1->options().setSelectionPolicy(Qtitan::SelectSingleRow);
        view2->options().setSelectionPolicy(Qtitan::SelectSingleRow);
    }
}

void Window::fastScrollChanged(int state)
{
    if (m_grid1 == Q_NULL || m_grid2 == Q_NULL)
        return;

    Qtitan::GridTableView* view1 = m_grid1->view<Qtitan::GridTableView>();
    Qtitan::GridTableView* view2 = m_grid2->view<Qtitan::GridTableView>();
    view1->options().setFastScrollEffect(state == Qt::Checked);
    view2->options().setFastScrollEffect(state == Qt::Checked);
}

void Window::dottedLineChanged(int state)
{
    if (m_grid1 == Q_NULL || m_grid2 == Q_NULL)
        return;

    Qtitan::GridTableView* view1 = m_grid1->view<Qtitan::GridTableView>();
    Qtitan::GridTableView* view2 = m_grid2->view<Qtitan::GridTableView>();
    QPen pen = view1->options().gridLinePen();
    pen.setStyle(state == Qt::Checked ? Qt::DotLine : Qt::SolidLine);
    view1->options().setGridLinePen(pen);
    view2->options().setGridLinePen(pen);
}

void Window::selectGridLineStyles(int index)
{
    if (m_grid1 == Q_NULL || m_grid2 == Q_NULL)
        return;

    Qtitan::GridTableView* view1 = m_grid1->view<Qtitan::GridTableView>();
    Qtitan::GridTableView* view2 = m_grid2->view<Qtitan::GridTableView>();
    switch (index)
    {
    case 0:
        view1->options().setGridLines(Qtitan::LinesNone);
        view2->options().setGridLines(Qtitan::LinesNone);
        break;
    case 1:
        view1->options().setGridLines(Qtitan::LinesBoth);
        view2->options().setGridLines(Qtitan::LinesBoth);
        break;
    case 2:
        view1->options().setGridLines(Qtitan::LinesBoth2D);
        view2->options().setGridLines(Qtitan::LinesBoth2D);
        break;
    case 3:
        view1->options().setGridLines(Qtitan::LinesHorizontal);
        view2->options().setGridLines(Qtitan::LinesHorizontal);
        break;
    case 4:
        view1->options().setGridLines(Qtitan::LinesHorizontal2D);
        view2->options().setGridLines(Qtitan::LinesHorizontal2D);
        break;
    case 5:
        view1->options().setGridLines(Qtitan::LinesVertical);
        view2->options().setGridLines(Qtitan::LinesVertical);
        break;
    case 6:
        view1->options().setGridLines(Qtitan::LinesVertical2D);
        view2->options().setGridLines(Qtitan::LinesVertical2D);
        break;
    default:
        view1->options().setGridLines(Qtitan::LinesBoth);
        view2->options().setGridLines(Qtitan::LinesBoth);
    }
 }

void Window::zoomEnabledChanged(int state)
{
    Qtitan::GridTableView* view1 = m_grid1->view<Qtitan::GridTableView>();
    Qtitan::GridTableView* view2 = m_grid2->view<Qtitan::GridTableView>();
    view1->options().setZoomEnabled(state == Qt::Checked);
    view2->options().setZoomEnabled(state == Qt::Checked);
}

void Window::zoomIndicatorChanged(int state)
{
    Qtitan::GridTableView* view1 = m_grid1->view<Qtitan::GridTableView>();
    Qtitan::GridTableView* view2 = m_grid2->view<Qtitan::GridTableView>();
    view1->options().setZoomIndicatorActive(state == Qt::Checked);
    view2->options().setZoomIndicatorActive(state == Qt::Checked);
}

void Window::zoomValueChanged(int value)
{
    double factor = qCeil((double)value / 25) * 25;
    Qtitan::GridTableView* view1 = m_grid1->view<Qtitan::GridTableView>();
    Qtitan::GridTableView* view2 = m_grid2->view<Qtitan::GridTableView>();
    view1->options().setZoomFactor(factor / 100);
    view2->options().setZoomFactor(factor / 100);
}

void Window::WindowsAirSupportChanged(int state)
{
    Qtitan::GridTableView* view1 = m_grid1->view<Qtitan::GridTableView>();
    Qtitan::GridTableView* view2 = m_grid2->view<Qtitan::GridTableView>();
    view1->options().setWindowsAirSupported(state == Qt::Checked);
    view2->options().setWindowsAirSupported(state == Qt::Checked);
}

void Window::cellButtonAutoRaiseEnabled(int state)
{
    Qtitan::GridTableView* view1 = m_grid1->view<Qtitan::GridTableView>();
    Qtitan::GridTableView* view2 = m_grid2->view<Qtitan::GridTableView>();
    view1->options().setCellButtonAutoRaise(state == Qt::Checked);
    view2->options().setCellButtonAutoRaise(state == Qt::Checked);
}

void Window::frozenRowsEnabled(int state)
{
    Qtitan::GridTableView* view1 = m_grid1->view<Qtitan::GridTableView>();
    Qtitan::GridTableView* view2 = m_grid2->view<Qtitan::GridTableView>();
    view1->options().setRowFrozenButtonVisible(state == Qt::Checked);
    view1->options().setFrozenPlaceQuickSelection(state == Qt::Checked);
    view2->options().setRowFrozenButtonVisible(state == Qt::Checked);
    view2->options().setFrozenPlaceQuickSelection(state == Qt::Checked);
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

void Window::printPreview()
{
    m_grid1->view<Qtitan::GridTableView>()->printPreview();
}

void Window::showCompanyWebSite()
{
    QDesktopServices::openUrl(QUrl("http://www.devmachines.com"));
}

void Window::setShadeColor(const QColor& color)
{
    m_grid1->themeManager()->setShadeColor(color);
    m_grid2->themeManager()->setShadeColor(color);
}
