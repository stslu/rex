
#include <QtGui>
#include <QDirModel>
#include <QDesktopServices>
#include <QMessageBox>

#include "window.h"

/* Window */
Window::Window()
: DemoMainWindow("QtitanDataGrid", QTN_VERSION_DATAGRID_STR)
{
    setWindowTitle(tr("QtitanDataGrid Demo (Summaries)"));
    setGeometry(150, 150, 1000, 800);

    Grid::loadTranslation();

    m_grid = new Qtitan::Grid();
    SummaryDemoModel* model = new SummaryDemoModel(m_grid);

    // Configure grid view
    m_grid->setViewType(Qtitan::Grid::CardView);
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();


    view->options().setFooterSummaryVisible(true);
    view->options().setGroupSummaryPlace(GroupSummaryFooter);
    //view->options().setShowFocusDecoration(true);
    QPen pen = view->options().gridLinePen();
    pen.setWidth(7);
    pen.setColor(Qt::red);
    view->options().setGridLinePen(pen);
    view->options().setBorderWidth(9);

    //Connect Grid's context menu handler.
    connect(view, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs* )));

    view->setModel(model);

    Qtitan::GridCardColumn* column = (Qtitan::GridCardColumn *)view->getColumn(0);

    //Configure the summary
    column->setDefaultGroupSummary(Qtitan::SummaryMax);
    column = (Qtitan::GridCardColumn *)view->getColumn(1);

    //Configure the summary
    column->setDefaultGroupSummary(Qtitan::SummaryAvg);
    column->setFooterSummary(Qtitan::SummaryAvg);
    column = (Qtitan::GridCardColumn *)view->getColumn(2);

    //Configure the summary
    column->setDefaultGroupSummary(Qtitan::SummaryCount);
    column->setFooterSummary(Qtitan::SummaryCount);
    column = (Qtitan::GridCardColumn *)view->getColumn(3);

    //Add cell button to the column.
    column->addButton();
    connect(column, SIGNAL(buttonClicked(CellButtonClickEventArgs*)), this, SLOT(cellButtonClicked(CellButtonClickEventArgs*)));

    column = (Qtitan::GridCardColumn *)view->getColumn(4);

    //Add cell button to the column.
    column->addButton(Qtitan::ClearButton, Qtitan::RightPosition, MouseOverPolicy);
    connect(column, SIGNAL(buttonClicked(CellButtonClickEventArgs*)), this, SLOT(cellButtonClicked(CellButtonClickEventArgs*)));

    column = (Qtitan::GridCardColumn *)view->getColumn(4);
    column->setGroupIndex(0);

    //Configure the summary
    column->setDefaultGroupSummary(Qtitan::SummaryMin);
    column->setFooterSummary(Qtitan::SummaryMax);

    //Show button menu for all column headers.
    for (int i = 0; i < view->getColumnCount(); ++i)
        static_cast<GridTableColumn *>(view->getColumn(i))->setMenuButtonVisible(true);

    view->expandAll();

    setDemoWidget(m_grid, createSettingsWidget());
}

QWidget* Window::createSettingsWidget()
{
    QWidget* settings = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(settings);
    QCheckBox* autoWidthCheck = new QCheckBox(settings);
    l->addWidget(autoWidthCheck);
    autoWidthCheck->setText("Column auto width");
    connect(autoWidthCheck, SIGNAL(stateChanged(int)), this, SLOT(autoWidthStateChanged(int)));

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

    QLabel* label = new QLabel(settings);
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
    lineStylesSelect->setCurrentIndex(4);

    label = new QLabel(settings);
    hl = new QHBoxLayout(settings);
    label->setText("Group Summary Place:");
    QComboBox* groupSummarySelect = new QComboBox(settings);

    groupSummarySelect->addItem("Footer");
    groupSummarySelect->addItem("Group");
    groupSummarySelect->addItem("Group Plus");
    connect(groupSummarySelect, SIGNAL(currentIndexChanged(int)), this, SLOT(selectGroupSummaryPlace(int)));
    hl->addWidget(label);
    hl->addWidget(groupSummarySelect);
    l->addLayout(hl);
    groupSummarySelect->setCurrentIndex(1);

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

    return settings;
}

void Window::setShadeColor(const QColor& color)
{
    m_grid->themeManager()->setShadeColor(color);
}

void Window::autoWidthStateChanged(int state)
{
    Q_UNUSED(state);
#if 0
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setColumnAutoWidth(state == Qt::Checked);
#endif
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

void Window::selectGroupSummaryPlace(int index)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    switch (index)
    {
    case 1:
        view->options().setGroupSummaryPlace(GroupSummaryRow);
        break;
    case 2:
        view->options().setGroupSummaryPlace(GroupSummaryRowPlus);
        break;
    default:
        view->options().setGroupSummaryPlace(GroupSummaryFooter);
    }
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

void Window::selectViewOrientation(int index)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
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

void Window::frozenRowsEnabled(int state)
{
    Q_UNUSED(state);
#if 0
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setRowFrozenButtonVisible(state == Qt::Checked);
    view->options().setFrozenPlaceQuickSelection(state == Qt::Checked);
#endif
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

void Window::printPreview()
{
    m_grid->view<Qtitan::GridCardView>()->printPreview();
}

void Window::showCompanyWebSite()
{
    QDesktopServices::openUrl(QUrl("http://www.devmachines.com"));
}

/* SummaryDemoModel */
SummaryDemoModel::SummaryDemoModel(QObject *parent) 
: QAbstractItemModel(parent)
{
    m_values.resize(100);
    int y = 2000;
    int m = 1;
    int d = 1;
    for (int i = 0; i < m_values.size(); ++i)
    {
        m_values[i].v0 = true;
        m_values[i].v1 = i;
        m_values[i].v2 = QString("String-1 = %1").arg(i);
        m_values[i].v3 = QString("String-2 = %1").arg(i);
        if (d > 28)
        {
            d = 1;
            m++;
            if (m > 12)
            {
                m = 1;
                y++;
            }
        }
        m_values[i].v4 = QDate(y, m, d);
        if (!(i % 10))
            d++;
    }
} 

SummaryDemoModel::~SummaryDemoModel()
{
} 

QVariant SummaryDemoModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const
{
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    switch (section)
    {
    case 0:
        {
            return QString("Boolean");
        }
        break;
    case 1:
        {
            return QString("Integer");
        }
        break;
    case 2:
        {
            return QString("String");
        }
        break;
    case 3:
        {
            return QString("String");
        }
        break;
    case 4:
        {
            return QString("Date");
        }
        break;
    }
    return QVariant();
}

QModelIndex SummaryDemoModel::parent(const QModelIndex & /*child*/) const 
{
    return QModelIndex();
} 

bool SummaryDemoModel::hasChildren(const QModelIndex &parent) const 
{  
    if (parent.model() == this || !parent.isValid()) 
    {
        return rowCount(parent) > 0 && columnCount(parent) > 0;
    }
    return false;
} 

int SummaryDemoModel::rowCount(const QModelIndex &parent) const 
{
    if (parent.isValid())
        return 0;
    return m_values.size();
} 

int SummaryDemoModel::columnCount(const QModelIndex &parent) const 
{
    if (parent.isValid())
        return 0;
    return 5;
}

QModelIndex SummaryDemoModel::index(int row, int column, const QModelIndex &parent) const 
{
    if (parent.isValid())                                   
        return QModelIndex();

    if (row < 0 || row >= rowCount(parent))
        return QModelIndex();

    if (column < 0 || column >= columnCount(parent)) 
        return QModelIndex();

    return createIndex(row, column, (void*)NULL);
}

QVariant SummaryDemoModel::data(const QModelIndex &index, int role) const 
{  
    if (!index.isValid())
        return QVariant();
    
    if (index.row() < 0 || index.row() >= rowCount(index.parent()))
        return QVariant();
    
    if (index.column() < 0 || index.column() >= columnCount(index.parent()))
        return QVariant();
    
    if (role == Qt::DisplayRole || role == Qt::EditRole)        
    {    
        switch (index.column())
        {
        case 0:
            {
                return m_values[index.row()].v0;
            }
            break;
        case 1:
            {
                return m_values[index.row()].v1;
            }
            break;
        case 2:
            {
                return m_values[index.row()].v2;
            }
            break;
        case 3:
            {
                return m_values[index.row()].v3;
            }
            break;
        case 4:
            {
                return m_values[index.row()].v4;
            }
            break;
        }
    }
    else if (role == Qt::CheckStateRole)
    {
        switch (index.column())
        {
        case 0:
            {
                return m_values[index.row()].v0;
            }
            break;
        }
    }
    return QVariant();
}

bool SummaryDemoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    
    if (index.row() < 0 || index.row() >= rowCount(index.parent()))
        return false;
    
    if (index.column() < 0 || index.column() >= columnCount(index.parent()))
        return false;

    if (role != Qt::EditRole)
        return false;

    switch (index.column())
    {
    case 0:
        {
            m_values[index.row()].v0 = value.toBool();
        }
        break;
    case 1:
        {
            m_values[index.row()].v1 = value.toInt();
        }
        break;
    case 2:
        {
            m_values[index.row()].v2 = value.toString();
        }
        break;
    case 3:
        {
            m_values[index.row()].v3 = value.toString();
        }
        break;
    case 4:
        {
            m_values[index.row()].v4 = value.toDate();
        }
        break;
    }

    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags SummaryDemoModel::flags(const QModelIndex &index) const 
{  
    if (!index.isValid())
        return Qt::ItemFlags();

    //if (index.column() == 0)
    //{
    //    return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
    //}
    //else if (index.column() == 4)
    //{
    //    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
    //}
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}                

