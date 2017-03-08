
#include <QtGui>
#include <QDirModel>
#include <QMessageBox>
#include <QVariant>

#include "window.h"
#include "stardelegate.h"
#include "starrating.h"

Window::Window()
: DemoMainWindow("QtitanDataGrid", QTN_VERSION_DATAGRID_STR)
{
    setWindowTitle(tr("QtitanDataGrid Demo (Qt delegates)"));
    setGeometry(50, 50, 800, 500);

    Grid::loadTranslation();

    m_grid = new Qtitan::Grid();
    m_grid->setViewType(Qtitan::Grid::TableView);
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setBorderWidth(0);

    //Connect Grid's context menu handler.
    connect(view, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs* )));

    StandartDelegateDemoModel* model = new StandartDelegateDemoModel(m_grid);
    view->setModel(model);

    Qtitan::GridTableColumn* column = (Qtitan::GridTableColumn *)view->getColumn(0);
    //Add cell button to the column.
    column->addButton(Qtitan::ChoiceButton, Qtitan::RightPosition);
    connect(column, SIGNAL(buttonClicked(CellButtonClickEventArgs*)), this, SLOT(cellButtonClicked(CellButtonClickEventArgs*)));

    column = (Qtitan::GridTableColumn *)view->getColumn(3);
    column->setEditorType(Qtitan::DelegateAdapterEditorType);
    GridDelegateAdapterEditorRepository* adapter = static_cast<GridDelegateAdapterEditorRepository *>(column->editorRepository());
    adapter->setDelegate(new StarDelegate);
    adapter->setDefaultValue(qVariantFromValue(StarRating(5)), Qt::EditRole);

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
    dottedLineCheck->setChecked(false);

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

void Window::contextMenu(ContextMenuEventArgs* args)
{
    args->contextMenu()->addAction("Print Preview (new)", this, SLOT(printPreview()));
    args->contextMenu()->addSeparator();
    args->contextMenu()->addAction("Developer Machines on the Web", this, SLOT(showCompanyWebSite()));
}

void Window::cellButtonClicked(CellButtonClickEventArgs* args)
{
    QMessageBox::information(this, tr("Cell button clicked (new)"), 
        tr("Clicked: Button - %1, Column Title - %2, RowIndex - %3").arg(args->buttonIndex()).arg(args->column()->caption()).arg(args->row()->rowIndex()));
}

void Window::transparentBackgroundEnabled(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setTransparentBackground(state == Qt::Checked);

    view->options().setAlternatingRowColors(!view->options().alternatingRowColors());
}

void Window::printPreview()
{
    m_grid->view<Qtitan::GridTableView>()->printPreview();
}

void Window::showCompanyWebSite()
{
    QDesktopServices::openUrl(QUrl("http://www.devmachines.com"));
}

void Window::setShadeColor(const QColor& color)
{
    m_grid->themeManager()->setShadeColor(color);
}

/* StandartDelegateDemoModel */
StandartDelegateDemoModel::StandartDelegateDemoModel(QObject *parent) 
: QAbstractItemModel(parent)
{
    m_values.push_back(DataItem("Mass in B-Minor", "Baroque", "J.S. Bach", 5));
    m_values.push_back(DataItem("Three More Foxes", "Jazz", "Maynard Ferguson", 4));
    m_values.push_back(DataItem("Sex Bomb", "Pop", "Tom Jones", 3));
    m_values.push_back(DataItem("Barbie Girl", "Pop", "Aqua", 5));
} 

StandartDelegateDemoModel::~StandartDelegateDemoModel()
{
}

QVariant StandartDelegateDemoModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const
{
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    switch (section)
    {
    case 0:
        {
            return QString("Title");
        }
        break;
    case 1:
        {
            return QString("Genre");
        }
        break;
    case 2:
        {
            return QString("Artist");
        }
        break;
    case 3:
        {
            return QString("Rating");
        }
        break;
    default:
        return QVariant();
    }
}

QModelIndex StandartDelegateDemoModel::parent(const QModelIndex & /*child*/) const 
{
    return QModelIndex();
} 

bool StandartDelegateDemoModel::hasChildren(const QModelIndex &parent) const 
{  
    if (parent.model() == this || !parent.isValid()) 
    {
        return rowCount(parent) > 0 && columnCount(parent) > 0;
    }
    return false;
} 

int StandartDelegateDemoModel::rowCount(const QModelIndex &parent) const 
{
    if (parent.isValid())
        return 0;
    return m_values.size();
} 

int StandartDelegateDemoModel::columnCount(const QModelIndex &parent) const 
{
    if (parent.isValid())
        return 0;
    return 4;
}

QModelIndex StandartDelegateDemoModel::index(int row, int column, const QModelIndex &parent) const 
{
    if (parent.isValid())                                   
        return QModelIndex();

    if (row < 0 || row >= rowCount(parent))
        return QModelIndex();

    if (column < 0 || column >= columnCount(parent)) 
        return QModelIndex();

    return createIndex(row, column, (void*)NULL);
}

QVariant StandartDelegateDemoModel::data(const QModelIndex &index, int role) const 
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
                return m_values[index.row()].title;
            }
            break;
        case 1:
            {
                return m_values[index.row()].genre;
            }
            break;
        case 2:
            {
                return m_values[index.row()].artist;
            }
            break;
        case 3:
            {
                return qVariantFromValue(StarRating(m_values[index.row()].rating));
            }
            break;
        default:
            break;
        }
    }
    return QVariant();
}

bool StandartDelegateDemoModel::setData(const QModelIndex &index, const QVariant &value, int role)
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
            m_values[index.row()].title = value.toString();
        }
        break;
    case 1:
        {
            m_values[index.row()].genre = value.toString();
        }
        break;
    case 2:
        {
            m_values[index.row()].artist = value.toString();
        }
        break;
    case 3:
        {
            if (value.canConvert<StarRating>())
            {
                StarRating starRating = qvariant_cast<StarRating>(value);
                m_values[index.row()].rating = starRating.starCount();
            }
            else Q_ASSERT(false);
        }
        break;
    default:
        break;
    }

    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags StandartDelegateDemoModel::flags(const QModelIndex &index) const 
{  
    if (!index.isValid())
        return Qt::ItemFlags();
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}                



