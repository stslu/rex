
#include <QtGui>
#include <QDirModel>
#include <QDesktopServices>
#include <QMessageBox>

#include "window.h"

/* Window */
Window::Window()
: DemoMainWindow("QtitanDataGrid", QTN_VERSION_DATAGRID_STR)
{
    setWindowTitle(tr("QtitanDataGrid - Large Model with 100.000 rows"));
    setGeometry(150, 150, 1000, 800);

    Grid::loadTranslation();

    m_grid = new Qtitan::Grid();
    LargeDataSetModel* model = new LargeDataSetModel(m_grid);

    // Configure grid view
    m_grid->setViewType(Qtitan::Grid::TableView);
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setGridLineWidth(1);

    //Connect Grid's context menu handler.
    connect(view, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs* )));

    connect(view, SIGNAL(editorModifying(GridEditor *)), this, SLOT(editorModifying(GridEditor *)));
    connect(view, SIGNAL(editorValidating(EditorValidationEventArgs*)), this, SLOT(editorValidating(EditorValidationEventArgs*)));


    view->setModel(model);

    Qtitan::GridTableColumn* column = (Qtitan::GridTableColumn *)view->getColumn(0);
    column->setEditorType(Qtitan::CheckBoxEditorType);
    column->editorRepository()->setImmediatePost(true);

    column = (Qtitan::GridTableColumn *)view->getColumn(1);
    column->setEditorType(Qtitan::NumericEditorType);
    ((Qtitan::GridNumericEditorRepository *)column->editorRepository())->setMinimum(-10000);
    ((Qtitan::GridNumericEditorRepository *)column->editorRepository())->setMaximum(10000);
    

    column = (Qtitan::GridTableColumn *)view->getColumn(2);
    column->setEditorType(Qtitan::StringEditorType);
    column->editorRepository()->setValidateOnEnter(false);
    column->editorRepository()->setEditorActivationPolicy(GridEditorActivationPolicy(KeyPress_WithEating | Enter));

    //Add cell button to the column.
    column->addButton(Qtitan::TextButton, Qtitan::RightPosition);
    column->addButton(Qtitan::ChoiceButton, Qtitan::RightPosition);
    connect(column, SIGNAL(buttonClicked(CellButtonClickEventArgs*)), this, SLOT(cellButtonClicked(CellButtonClickEventArgs*)));

    column = (Qtitan::GridTableColumn *)view->getColumn(3);
    column->setEditorType(Qtitan::DateEditorType);
    //Add cell button to the column.
    column->addButton(Qtitan::ClearButton, Qtitan::RightPosition, Qtitan::MouseOverPolicy);
    connect(column, SIGNAL(buttonClicked(CellButtonClickEventArgs*)), this, SLOT(cellButtonClicked(CellButtonClickEventArgs*)));
    column->editorRepository()->setAutoSelect(true);

    column = (Qtitan::GridTableColumn *)view->getColumn(4);
    column->setEditorType(Qtitan::TimeEditorType);

     //Show button menu for all column headers.
    for (int i = 0; i < view->getColumnCount(); ++i)
        static_cast<GridTableColumn *>(view->getColumn(i))->setMenuButtonVisible(true);

    setDemoWidget(m_grid, createSettingsWidget());

    setMinimumHeight(10);
}

QWidget* Window::createSettingsWidget()
{
    //Create settings widget
    QWidget* settings = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(settings);
    QCheckBox* autoWidthCheck = new QCheckBox(settings);
    autoWidthCheck->setText("Column auto width");
    connect(autoWidthCheck, SIGNAL(stateChanged(int)), this, SLOT(autoWidthStateChanged(int)));
    autoWidthCheck->setChecked(true);
    l->addWidget(autoWidthCheck);

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

void Window::editorModifying(GridEditor* editor)
{
    Q_UNUSED(editor);
}

void Window::editorValidating(EditorValidationEventArgs* args)
{
    args->setValueValid(true);
    //args->setEditingValue(args->editValue());
    args->setHandled(true);
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
    m_grid->view<Qtitan::GridTableView>()->printPreview();
}

void Window::showCompanyWebSite()
{
    QDesktopServices::openUrl(QUrl("http://www.devmachines.com"));
}


/* LargeDataSetModel */
LargeDataSetModel::LargeDataSetModel(QObject *parent) 
: QAbstractItemModel(parent)
{
    m_values.resize(100000);
    int y = 2009;
    int m = 1;
    int d = 1;
    for (int i = 0; i < m_values.size(); ++i)
    {
        m_values[i].v0 = true;
        m_values[i].v1 = i;
        m_values[i].v2 = QString("String = %1").arg(i);
        m_values[i].v4 = QTime(12, 0, 0);
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
        m_values[i].v3 = QDate(y, m, d);
        if (!(i % 10))
            d++;
    }
} 

LargeDataSetModel::~LargeDataSetModel()
{
} 

QVariant LargeDataSetModel::headerData(int section, Qt::Orientation orientation,
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
            return QString("Date");
        }
        break;
    case 4:
        {
            return QString("Time");
        }
        break;
    }
    return QVariant();
}

QModelIndex LargeDataSetModel::parent(const QModelIndex & /*child*/) const 
{
    return QModelIndex();
} 

bool LargeDataSetModel::hasChildren(const QModelIndex &parent) const 
{  
    if (parent.model() == this || !parent.isValid()) 
    {
        return rowCount(parent) > 0 && columnCount(parent) > 0;
    }
    return false;
} 

int LargeDataSetModel::rowCount(const QModelIndex &parent) const 
{
    if (parent.isValid())
        return 0;
    return m_values.size();
} 

int LargeDataSetModel::columnCount(const QModelIndex &parent) const 
{
    if (parent.isValid())
        return 0;
    return 5;
}

QModelIndex LargeDataSetModel::index(int row, int column, const QModelIndex &parent) const 
{
    if (parent.isValid())                                   
        return QModelIndex();

    if (row < 0 || row >= rowCount(parent))
        return QModelIndex();

    if (column < 0 || column >= columnCount(parent)) 
        return QModelIndex();

    return createIndex(row, column, (void*)NULL);
}

QVariant LargeDataSetModel::data(const QModelIndex &index, int role) const 
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

bool LargeDataSetModel::setData(const QModelIndex &index, const QVariant &value, int role)
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
            m_values[index.row()].v3 = value.toDate();
        }
        break;
    case 4:
        {
            m_values[index.row()].v4 = value.toTime();
        }
        break;
    }

    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags LargeDataSetModel::flags(const QModelIndex &index) const 
{  
    if (!index.isValid())
        return Qt::ItemFlags();
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}                

