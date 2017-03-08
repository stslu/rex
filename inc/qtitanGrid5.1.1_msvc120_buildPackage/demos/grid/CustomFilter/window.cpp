
#include <QtGui>
#include <QDirModel>
#include <QDesktopServices>
#include <QMessageBox>
#include "QtnGridFilter.h"

#include "window.h"

/* Window */
Window::Window()
: DemoMainWindow("QtitanDataGrid", QTN_VERSION_DATAGRID_STR)
{
    setWindowTitle(tr("QtitanDataGrid - Custom Filters"));
    setGeometry(50, 50, 800, 500);

    Grid::loadTranslation();

    m_grid = new Qtitan::Grid();
    CustomFilterModel* model = new CustomFilterModel(m_grid);

    // Configure grid view
    m_grid->setViewType(Qtitan::Grid::TableView);
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setGridLineWidth(0);
    view->options().setColumnAutoWidth(true);
    view->options().setSelectionPolicy(Qtitan::SelectMultiRow);
    view->options().setFastScrollEffect(true);

    //Connect Grid's context menu handler.
    connect(view, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs* )));

    view->setModel(model);

    Qtitan::GridTableColumn* column = (Qtitan::GridTableColumn *)view->getColumn(0);
    column->setEditorType(Qtitan::NumericEditorType);
    ((Qtitan::GridNumericEditorRepository *)column->editorRepository())->setMinimum(-10000);
    ((Qtitan::GridNumericEditorRepository *)column->editorRepository())->setMaximum(10000);
    column->editorRepository()->setEditorActivationPolicy(GridEditorActivationPolicy(DblClick | EditPress));

    column = (Qtitan::GridTableColumn *)view->getColumn(1);
    column->setEditorType(Qtitan::StringEditorType);
    column->editorRepository()->setValidateOnEnter(false);
    column->editorRepository()->setEditorActivationPolicy(GridEditorActivationPolicy(DblClick | EditPress));

    column = (Qtitan::GridTableColumn *)view->getColumn(2);
    column->setEditorType(Qtitan::DateEditorType);
    column->editorRepository()->setAutoSelect(true);
    column->editorRepository()->setEditorActivationPolicy(GridEditorActivationPolicy(DblClick | EditPress));

    column = (Qtitan::GridTableColumn *)view->getColumn(3);
    column->setEditorType(Qtitan::TimeEditorType);
    column->editorRepository()->setAutoSelect(true);
    column->editorRepository()->setEditorActivationPolicy(GridEditorActivationPolicy(DblClick | EditPress));

    //Create settings widget
    QWidget* settings = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(settings);

    QLabel* text = new QLabel(settings);
    text->setText("Select several rows and press 'Set Filter' button to apply filter to these rows.");
    text->setWordWrap(true);
    l->addWidget(text);

    QPushButton* filterButton = new QPushButton(settings);
    filterButton->setText(tr("Set Filter"));
    connect(filterButton, SIGNAL(clicked()), this, SLOT(applyFilter()));
    l->addWidget(filterButton);

    setDemoWidget(m_grid, settings);

    setMinimumHeight(10);
}

void Window::contextMenu(ContextMenuEventArgs* args)
{
    args->contextMenu()->addAction("Print Preview", this, SLOT(printPreview()));
    args->contextMenu()->addSeparator();
    args->contextMenu()->addAction("Developer Machines on the Web", this, SLOT(showCompanyWebSite()));
}

void Window::applyFilter()
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    GridFilterGroupCondition* groupCondition = new GridFilterGroupCondition(view->filter());
    CustomFilterCondition* condition = new CustomFilterCondition(view->filter());
    groupCondition->addCondition(condition);

    GridSelection* selection = view->selection();
    while (!selection->end())
    {
        GridRow* row = selection->row();
        condition->addRow(row->modelRowIndex());
        selection->next();
    }
    
    view->filter()->setCondition(groupCondition, true /* add to history or not */);
    view->filter()->setActive(true);

    view->showFilterPanel();
}

void Window::showCompanyWebSite()
{
    QDesktopServices::openUrl(QUrl("http://www.devmachines.com"));
}

void Window::setShadeColor(const QColor& color)
{
    m_grid->themeManager()->setShadeColor(color);
}

/* CustomFilterCondition */
CustomFilterCondition::CustomFilterCondition(GridFilter* filter)
    : GridFilterCondition(filter) { }

bool CustomFilterCondition::isTrue(int modelRow)
{
    return m_modelRows.contains(modelRow);
}

GridFilterCondition* CustomFilterCondition::clone() const
{
    CustomFilterCondition* retval = new CustomFilterCondition(m_filter);
    retval->m_modelRows = m_modelRows;
    return retval;
}

QString CustomFilterCondition::createPresentation() const
{
    QString retval = "custom filter: row in (";
 
    QList<int> list = m_modelRows.toList();
    qSort(list);
    for (QList<int>::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        if (it != list.constBegin())
            retval += ", ";
        retval += QString::number(*it);
    }
    retval += ")";
    return retval;
}

int CustomFilterCondition::conditionCount() const
{
    return m_modelRows.size();
}

void CustomFilterCondition::addRow(int modelRowIndex)
{
    m_modelRows.insert(modelRowIndex);
}

void CustomFilterCondition::removeRow(int modelRowIndex)
{
    m_modelRows.remove(modelRowIndex);
}

/* CustomFilterModel */
CustomFilterModel::CustomFilterModel(QObject *parent) 
: QAbstractItemModel(parent)
{
    m_values.resize(100);
    int y = 2012;
    int m = 1;
    int d = 1;
    for (int i = 0; i < m_values.size(); ++i)
    {
        m_values[i].v0 = i;
        m_values[i].v1 = QString("String = %1").arg(i);
        m_values[i].v3 = QTime(12, 0, 0);
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
        m_values[i].v2 = QDate(y, m, d);
        if (!(i % 10))
            d++;
    }
} 

QVariant CustomFilterModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const
{
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    switch (section)
    {
    case 0:
        {
            return QString("Integer");
        }
        break;
    case 1:
        {
            return QString("String");
        }
        break;
    case 2:
        {
            return QString("Date");
        }
        break;
    case 3:
        {
            return QString("Time");
        }
        break;
    }
    return QVariant();
}

QModelIndex CustomFilterModel::parent(const QModelIndex & /*child*/) const 
{
    return QModelIndex();
} 

bool CustomFilterModel::hasChildren(const QModelIndex &parent) const 
{  
    if (parent.model() == this || !parent.isValid()) 
    {
        return rowCount(parent) > 0 && columnCount(parent) > 0;
    }
    return false;
} 

int CustomFilterModel::rowCount(const QModelIndex &parent) const 
{
    if (parent.isValid())
        return 0;
    return m_values.size();
} 

int CustomFilterModel::columnCount(const QModelIndex &parent) const 
{
    if (parent.isValid())
        return 0;
    return 4;
}

QModelIndex CustomFilterModel::index(int row, int column, const QModelIndex &parent) const 
{
    if (parent.isValid())                                   
        return QModelIndex();

    if (row < 0 || row >= rowCount(parent))
        return QModelIndex();

    if (column < 0 || column >= columnCount(parent)) 
        return QModelIndex();

    return createIndex(row, column, (void*)NULL);
}

QVariant CustomFilterModel::data(const QModelIndex &index, int role) const 
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

bool CustomFilterModel::setData(const QModelIndex &index, const QVariant &value, int role)
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
            m_values[index.row()].v0 = value.toInt();
        }
        break;
    case 1:
        {
            m_values[index.row()].v1 = value.toString();
        }
        break;
    case 2:
        {
            m_values[index.row()].v2 = value.toDate();
        }
    case 3:
        {
            m_values[index.row()].v3 = value.toTime();
        }
        break;
    }

    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags CustomFilterModel::flags(const QModelIndex &index) const 
{  
    if (!index.isValid())
        return Qt::ItemFlags();
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}                

