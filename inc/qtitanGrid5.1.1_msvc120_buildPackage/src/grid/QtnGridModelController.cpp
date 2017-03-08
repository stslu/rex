/****************************************************************************
**
** Qtitan Library by Developer Machines (DataGrid component for Qt.C++)
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/

#include <QStack>
#include <QHash>
#include <QDate>
#include <QtCore/qmath.h>

#include "QtnGrid.h"
#include "QtnGridModelController.h"
#include "QtnGridFilter.h"

QTITAN_USE_NAMESPACE

QTITAN_BEGIN_NAMESPACE
//
QTITAN_END_NAMESPACE

/*!
\class GridModelGroup
\inmodule QtitanDataGrid
*/

/*!
\enum GridEditStrategy
Enumerator GridEditStrategy describes the behaviour how the data should be submitted to the model.
\inmodule QtitanDataGrid

\value OnFieldChange - data will be submited to the model immediately once the value has changed. 
\value OnRowChange - data will be submited to the model if row focus has changed.
*/

/*!
\enum GridRow::RowType
\inmodule QtitanDataGrid
Enumirator RowType describes the type of the grid row. At the moment there are two types of rows - Data row, related to the model data and Group row, related to the group of data rows after grouping.
\value DataRow Data row
\value GroupRow Group row
*/

/*!
\enum GridRow::FrozenPlace
\inmodule QtitanDataGrid
Enumirator FrozenPlace describes the place for frozen rows. 
\value FrozenNone Row does not frozen. It can be vertical scrolled. This value by default.
\value FrozenTop Row frozen at the top of the view and can't be vertical scrolled (always displayed on view).
\value FrozenBottom Row frozen at the bottom of the view and can't be vertical scrolled (always displayed on view).
*/

GridSortColumnRules::GridSortColumnRules()
{
}

void GridSortColumnRules::clear()
{
    clearGrouping();
    clearSorting();
}

void GridSortColumnRules::clearSorting()
{
    m_sortRulesList.clear();
}

void GridSortColumnRules::clearGrouping()
{
    m_groupRulesList.clear();
}

/*!
\internal
*/
static const GridSortColumnRule* get_rule_by_dataBinding(const QList<GridSortColumnRule>& rules, 
                                                         GridModelDataBinding* dataBinding)
{
    for (QList<GridSortColumnRule>::const_iterator it = rules.constBegin(); it != rules.constEnd(); ++it)
    {
        if (it->dataBinding == dataBinding)
            return &(*it);
    }
    return Q_NULL;
}

/*!
\internal
*/
static void set_sort_rule(QList<GridSortColumnRule>& rules, GridModelDataBinding* dataBinding, int index, SortOrder order, bool caseSensitivity, bool localeAware)
{
    for (QList<GridSortColumnRule>::iterator it = rules.begin(); it != rules.end(); ++it)
    {
        if (it->dataBinding == dataBinding)
        {
            if (index == -1)
                index = it - rules.begin();
            rules.erase(it);
            break;
        }
    }

    if (order == SortNone)
        return;

    GridSortColumnRule rule;
    rule.dataBinding = dataBinding;
    rule.order = order;
    rule.caseSensitivity = caseSensitivity;
    rule.localeAware = localeAware;
    if (index == -1)
        rules.push_back(rule);
    else
        rules.insert(index, rule); 
}

void GridSortColumnRules::setSortRule(GridModelDataBinding* dataBinding, int index, SortOrder order, bool caseSensitivity, bool localeAware)
{
    set_sort_rule(m_sortRulesList, dataBinding, index, order, caseSensitivity, localeAware);
}

void GridSortColumnRules::setGroupRule(GridModelDataBinding* dataBinding, int index, SortOrder order, 
                                       bool caseSensitivity, bool localeAware)
{
    set_sort_rule(m_groupRulesList, dataBinding, index, order, caseSensitivity, localeAware);
}

int GridSortColumnRules::count() const
{
    return m_groupRulesList.size() + m_sortRulesList.size();
}

const GridSortColumnRule& GridSortColumnRules::rule(int index) const
{
    if (index < m_groupRulesList.size())
        return m_groupRulesList[index];
    else
        return m_sortRulesList[index - m_groupRulesList.size()];
}

int GridSortColumnRules::groupRulesCount() const
{
    return m_groupRulesList.size();
}

const GridSortColumnRule* GridSortColumnRules::groupRule(int index) const
{
    if (index >= 0 && index < m_groupRulesList.size())
        return &m_groupRulesList[index];
    else
        return Q_NULL;
}

const GridSortColumnRule* GridSortColumnRules::groupRuleByColumn(GridModelDataBinding* dataBinding) const
{
    return get_rule_by_dataBinding(m_groupRulesList, dataBinding);
}

int GridSortColumnRules::sortRulesCount() const
{
    return m_sortRulesList.size();
}

const GridSortColumnRule* GridSortColumnRules::sortRule(int index) const
{
    if (index >= 0 && index < m_sortRulesList.size())
        return &m_sortRulesList[index];
    else
        return Q_NULL;
}

const GridSortColumnRule* GridSortColumnRules::sortRuleByColumn(GridModelDataBinding* dataBinding) const
{
    return get_rule_by_dataBinding(m_sortRulesList, dataBinding);
}

/*!
  \class GridPersistentRow
  \inmodule QtitanDataGrid
*/
GridPersistentRow::GridPersistentRow(GridModelController* controller) 
    : m_controller(controller), m_columnIndex(-1), m_record(-1), m_modelRowIndex(-1), m_groupColumn(-1), m_gridRow(Q_NULL)
{
    Q_ASSERT(m_controller != Q_NULL);
    m_controller->m_persistRows.insert(this);
}

GridPersistentRow::GridPersistentRow(const GridPersistentRow& persistentRow)
{
    m_controller = persistentRow.m_controller;
    m_groupColumn = persistentRow.m_groupColumn;
    m_record = persistentRow.m_record;
    m_modelRowIndex = persistentRow.m_modelRowIndex;
    m_columnIndex = persistentRow.m_columnIndex;
    m_gridRow = persistentRow.m_gridRow;

    if (m_controller != Q_NULL)
        m_controller->m_persistRows.insert(this);
}

GridPersistentRow::~GridPersistentRow()
{
    if (m_controller != Q_NULL)
        m_controller->m_persistRows.remove(this);
}

bool GridPersistentRow::isValid() const
{
    return m_modelRowIndex >= 0;
}

void GridPersistentRow::clear()
{
    m_groupColumn = -1;
    m_record = -1;
    m_modelRowIndex = -1;
    m_columnIndex = -1;
    m_gridRow = Q_NULL;
}

void GridPersistentRow::set(GridRow* gridRow)
{
    if (gridRow == Q_NULL)
    {
        m_groupColumn = -1;
        m_record = -1;
        m_modelRowIndex = -1;
        return;
    }
    else if (gridRow->type() == GridRow::GroupRow)
    {
        m_groupColumn = gridRow->modelGroup()->dataBinding->column();
    }
    else
    {
        m_groupColumn = -1;
    }

    m_record = gridRow->record();
    m_modelRowIndex = gridRow->modelRowIndex();
    m_gridRow = gridRow;
}

bool GridPersistentRow::equal(GridRow* gridRow) const
{
    if (m_modelRowIndex == -1 && gridRow == Q_NULL)
        return true;

    if (gridRow == Q_NULL || m_modelRowIndex != gridRow->modelRowIndex())
        return false;

    if (gridRow->type() == GridRow::GroupRow)
    {
         if (gridRow->modelGroup()->dataBinding->column() != m_groupColumn)
             return false;
    }
    else if (m_groupColumn != -1)
    {
        return false;
    }

    return true;
}

void GridPersistentRow::setColumnIndex(int index)
{
    m_columnIndex = index;
}

int GridPersistentRow::columnIndex() const
{
    return m_columnIndex;
}

int GridPersistentRow::groupModelColumn() const
{
    return m_groupColumn;
}

int GridPersistentRow::modelRowIndex() const
{
    return m_modelRowIndex;
}

GridRow* GridPersistentRow::row() const
{
    if (!isValid())
        return Q_NULL;

    if (m_gridRow != Q_NULL)
        return m_gridRow;
    int index = m_controller->getRowIndexByPersistentRow(this);
    if (index < 0)
        const_cast<GridPersistentRow*>(this)->m_gridRow = Q_NULL;
    else
        const_cast<GridPersistentRow*>(this)->m_gridRow = m_controller->getGridRow(index);
    return m_gridRow;
}

int GridPersistentRow::record() const
{
    return m_record;
}

void GridPersistentRow::setRecord(int record)
{
    m_record = record;
    m_gridRow = Q_NULL; //Query grid row for new record.
}

GridPersistentRow& GridPersistentRow::operator=(const GridPersistentRow& persistentRow)
{
    m_controller = persistentRow.m_controller;
    m_groupColumn = persistentRow.m_groupColumn;
    m_record = persistentRow.m_record;
    m_modelRowIndex = persistentRow.m_modelRowIndex;
    m_columnIndex = persistentRow.m_columnIndex;
    m_gridRow = persistentRow.m_gridRow;
    if (m_controller != Q_NULL)
        m_controller->m_persistRows.insert(this);
    return *this;
}

/*!
    \class GridModelDataBinding
    \inmodule QtitanDataGrid
    \brief GridModelDataBinding class contains the set of functions and methods for binding QtitanDataGrid columns to the data model.
*/
GridModelDataBinding::GridModelDataBinding(GridModelController* controller, int column)
: m_modelController(controller),
  m_column(column),
  m_displayRole(Qt::DisplayRole),
  m_editRole(Qt::EditRole),
  m_sortRole(Qt::DisplayRole),
  m_summaryRole(Qt::DisplayRole),
  m_supportedSummaryTypes(0),
  m_relationMode(NoRelation),
  m_relationModel(Q_NULL),
  m_foreignKeyColumn(-1),
  m_foreignKeyMasterRole(Qt::EditRole),
  m_foreignKeyRole(Qt::DisplayRole)
{
}

GridModelDataBinding::~GridModelDataBinding()
{
    m_modelController = Q_NULL;
}

int GridModelDataBinding::column() const
{
    return m_column;
}

void GridModelDataBinding::setColumn(int column)
{
    m_column = column;
}

QString GridModelDataBinding::columnName() const
{
    if (m_column == -1 || m_modelController->model() == Q_NULL)
        return "";
    return m_modelController->model()->headerData(m_column, Qt::Horizontal).toString();
}

Qt::ItemDataRole GridModelDataBinding::displayRole() const
{
    return m_displayRole;
}

void GridModelDataBinding::setDisplayRole(Qt::ItemDataRole role)
{
    m_displayRole = role;
}

Qt::ItemDataRole GridModelDataBinding::editRole() const
{
    return m_editRole;
}

void GridModelDataBinding::setEditRole(Qt::ItemDataRole role)
{
    m_editRole = role;
}

Qt::ItemDataRole GridModelDataBinding::sortRole() const
{
    return m_sortRole;
}

void GridModelDataBinding::setSortRole(Qt::ItemDataRole role)
{
    m_sortRole = role;
}

Qt::ItemDataRole GridModelDataBinding::summaryRole() const
{
    return m_summaryRole;
}

void GridModelDataBinding::setSummaryRole(Qt::ItemDataRole role)
{
    m_summaryRole = role;
}

Qt::ItemDataRole GridModelDataBinding::foreignKeyMasterRole() const
{
    return m_foreignKeyMasterRole;
}

SummaryTypes GridModelDataBinding::supportedSummaryTypes() const
{
    return m_supportedSummaryTypes;
}

void GridModelDataBinding::setSupportedSummaryTypes(SummaryTypes types)
{
    m_supportedSummaryTypes = types;
}

GridModelDataBinding::RelationMode GridModelDataBinding::relationMode() const
{
    return m_relationMode;
}

void GridModelDataBinding::setRelationMode(GridModelDataBinding::RelationMode mode)
{
    m_relationMode = mode;
}

QAbstractItemModel* GridModelDataBinding::relationModel()
{
    if (m_relationModel == Q_NULL)
    {
        RelationEventArgs args(m_modelController, this);
        m_modelController->m_viewInterface->raiseQueryRelation(&args);
    }

    return m_relationModel;
}

void GridModelDataBinding::setRelationModel(QAbstractItemModel* model, QModelIndex parentIndex)
{
    m_relationModel = model;
    m_relationParentIndex = parentIndex;
    setRoleMapping(Qt::DisplayRole, 0, Qt::DisplayRole); //By default the first colunn mapped for Qt::DisplayRole role. 
    if (m_relationMode == NoRelation)
        m_relationMode = Relation;
}

const QModelIndex& GridModelDataBinding::relationParentIndex() const
{
    return m_relationParentIndex;
}

void GridModelDataBinding::setForeignKey(const QString& foreignColumnName, Qt::ItemDataRole foreignRole, Qt::ItemDataRole masterRole)
{
    setForeignKey(columnByColumnName(foreignColumnName), foreignRole, masterRole);
}

void GridModelDataBinding::setForeignKey(int foreignColumn, Qt::ItemDataRole foreignRole, Qt::ItemDataRole masterRole)
{
    m_foreignKeyColumn = foreignColumn;
    m_foreignKeyRole = foreignRole;
    m_foreignKeyMasterRole = masterRole;
}

QString GridModelDataBinding::foreignKeyColumnName() const
{
    return m_relationModel->headerData(m_foreignKeyColumn, Qt::Horizontal).toString();
}

int GridModelDataBinding::columnByColumnName(const QString& columnName) const
{
    for (int i = 0; i < m_relationModel->columnCount(); ++i)
    {
        if (m_relationModel->headerData(i, Qt::Horizontal).toString() == columnName)
            return i;
    }
    return -1;
}

int GridModelDataBinding::foreignKeyColumn() const
{
    return m_foreignKeyColumn;
}

Qt::ItemDataRole GridModelDataBinding::foreignKeyRole() const
{
    return m_foreignKeyRole;
}

QString GridModelDataBinding::getForeignColumnNameMapping(Qt::ItemDataRole role) const
{
    if (!isRelationModelColumnRoleSupported(role))
        return "";
    return m_relationModel->headerData(m_foreignColumnRoleMap[role].column, Qt::Horizontal).toString();
}

int GridModelDataBinding::getForeignColumnMapping(Qt::ItemDataRole role) const
{
    if (!isRelationModelColumnRoleSupported(role))
        return -1;
    return m_foreignColumnRoleMap[role].column;
}

Qt::ItemDataRole GridModelDataBinding::getForeignRoleMapping(Qt::ItemDataRole role) const
{
    if (!isRelationModelColumnRoleSupported(role))
        return Qt::UserRole;
    return m_foreignColumnRoleMap[role].role;
}

void GridModelDataBinding::setRoleMapping(Qt::ItemDataRole role, const QString& columnName, Qt::ItemDataRole indexRole)
{
    m_foreignColumnRoleMap[role] = IndexColumn(columnByColumnName(columnName), indexRole);
}

void GridModelDataBinding::setRoleMapping(Qt::ItemDataRole role, int column, Qt::ItemDataRole indexRole)
{
    m_foreignColumnRoleMap[role] = IndexColumn(column, indexRole);
}

bool GridModelDataBinding::isRelationModelColumnRoleSupported(Qt::ItemDataRole role) const
{
    return m_foreignColumnRoleMap.constFind(role) != m_foreignColumnRoleMap.constEnd();
}

QModelIndex GridModelDataBinding::findRelationModelIndex(const QVariant& foreignValue) const
{
    if (m_relationModel == Q_NULL)
        return QModelIndex();

    int fkColumn = foreignKeyColumn();
    if (fkColumn == -1)
    {
        return m_relationModel->index(foreignValue.toInt(), 0, m_relationParentIndex);
    }

    QModelIndex start = m_relationModel->index(0, fkColumn, m_relationParentIndex);
    QModelIndexList list = m_relationModel->match(start, foreignKeyRole(), foreignValue, 1, Qt::MatchExactly);
    if (list.size() == 0)
        return QModelIndex(); //Row is not exsists for current foreign key.
    else
        return list[0];
}

QModelIndex GridModelDataBinding::getRelationModelIndex(int modelRowIndex, Qt::ItemDataRole role, Qt::ItemDataRole& foreignRole) const
{
    //Get the row index for the relation model
    QVariant foreignValue;

    if (!m_modelController->getModelValue(m_column, modelRowIndex, foreignKeyMasterRole(), foreignValue))
        return QModelIndex();

    QModelIndex foreignIndex = findRelationModelIndex(foreignValue);
    if (!foreignIndex.isValid())
        return QModelIndex();

    int column = 0; //If no mapping exists the first column in the relation model will be used for index construction.
    foreignRole = role;

    ForeignColumnRoleMap::const_iterator it = m_foreignColumnRoleMap.constFind(role);
    if (it != m_foreignColumnRoleMap.constEnd())
    {
        column = it.value().column;
        foreignRole = it.value().role;
    }

    return m_relationModel->index(foreignIndex.row(), column, foreignIndex.parent());
}

GridModelController* GridModelDataBinding::controller() const
{
    return m_modelController;
}

bool GridModelDataBinding::getValue(int modelRowIndex, Qt::ItemDataRole role, QVariant& retval) const
{
    if (modelRowIndex == -1)
        return false;

    if (m_relationMode == GridModelDataBinding::Relation && role != foreignKeyMasterRole())
    {
        if (m_relationModel == Q_NULL)
            return false;
        Qt::ItemDataRole foreignRole;
        QModelIndex index = getRelationModelIndex(modelRowIndex, role, foreignRole);
        if (!index.isValid())
            return false;
        retval = m_relationModel->data(index, foreignRole);
        return true;
    }
    return m_modelController->getModelValue(m_column, modelRowIndex, role, retval);
}

bool GridModelDataBinding::setValue(int modelRowIndex, Qt::ItemDataRole role, const QVariant& value)
{
    if (modelRowIndex == -1)
        return false;

    if (m_relationMode == GridModelDataBinding::Relation && role != foreignKeyMasterRole())
    {
        if (m_relationModel == Q_NULL)
            return false;
        if (m_relationModel == Q_NULL)
            return false;
        Qt::ItemDataRole foreignRole;
        QModelIndex index = getRelationModelIndex(modelRowIndex, role, foreignRole);
        if (!index.isValid())
            return false;
        return m_relationModel->setData(index, value, foreignRole);
    }

    return m_modelController->setModelValue(m_column, modelRowIndex, role, value);
}

/*!
  \class RelationEventArgs
  \inmodule QtitanDataGrid
*/
RelationEventArgs::RelationEventArgs(GridModelController* controller, GridModelDataBinding* binding)
: BaseEventArgs(), m_controller(controller), m_dataBinding(binding)
{
}

GridModelDataBinding* RelationEventArgs::dataBinding() const
{
    return m_dataBinding;
}

GridModelController* RelationEventArgs::controller() const
{
    return m_controller;
}

/*! 
  \class GridSelection
  \inmodule QtitanDataGrid
*/
GridSelection::GridSelection(GridModelController* controller)
: m_controller(controller), m_dirty(true)
{
    clear();
}

bool GridCellLessThan(const GridCell& first, const GridCell& second)
{
     int delta = first.rowIndex() - second.rowIndex();
     if (delta == 0)
         return first.columnIndex() < second.columnIndex();
     return delta < 0;
}

static void fill_selected_set(GridModelController* controller, 
                              const SelectionRanges& ranges, QSet<GridCell>& cells)
{
    for (SelectionRanges::const_iterator it = ranges.constBegin();
        it != ranges.constEnd(); ++it)
    {
        for (int i = it->topLeft().rowIndex(); i <= it->bottomRight().rowIndex(); ++i)
            for (int j = it->topLeft().columnIndex(); j <= it->bottomRight().columnIndex(); ++j)
                cells.insert(GridCell(controller->getGridRow(i), j));
    }
}

void GridSelection::clear()
{
    m_ranges.clear();
    m_select.clear();
    m_current = m_select.begin();
    m_dirty = true;
}

const SelectionRanges& GridSelection::ranges() const
{
    return m_ranges;
}

void GridSelection::reset()
{
    QSet<GridCell> cells;
    fill_selected_set(m_controller, m_ranges, cells);
    m_select = cells.toList();
    qSort(m_select.begin(), m_select.end(), GridCellLessThan);
    m_current = m_select.begin();
    m_dirty = false;
}

bool GridSelection::next()
{
    if (m_dirty)
        reset();

    m_current++;
    return !end();
}

bool GridSelection::end()
{
    if (m_dirty)
        reset();

    return m_current == m_select.end();
}

GridRow* GridSelection::row() const
{
    return m_current->row();
}

GridCell* GridSelection::cell() const
{
    return &(*m_current);
}

static void selection_range_subtract(GridModelController* controller,
                              const GridCellSelectionRange &source,
                              const GridCellSelectionRange &target,
                              SelectionRanges& retval)
{
    Q_ASSERT(controller != Q_NULL);

    int top = source.top();
    int left = source.left();
    int bottom = source.bottom();
    int right = source.right();

    int target_top = target.top();
    int target_left = target.left();
    int target_bottom = target.bottom();
    int target_right = target.right();

    if (target_top > top)
    {
        GridCell tl = GridCell(controller, top, left);
        GridCell br = GridCell(controller, target_top - 1, right);
        retval.append(GridCellSelectionRange(controller, tl, br));
        top = target_top;
    }

    if (target_bottom < bottom)
    {
        GridCell tl = GridCell(controller, target_bottom + 1, left);
        GridCell br = GridCell(controller, bottom, right);
        retval.append(GridCellSelectionRange(controller, tl, br));
        bottom = target_bottom;
    }

    if (target_left > left)
    {
        GridCell tl = GridCell(controller, top, left);
        GridCell br = GridCell(controller, bottom, target_left - 1);
        retval.append(GridCellSelectionRange(controller, tl, br));
        left = target_left;
    }
    
    if (target_right < right)
    {
        GridCell tl = GridCell(controller, top, target_right + 1);
        GridCell br = GridCell(controller, bottom, right);
        retval.append(GridCellSelectionRange(controller, tl, br));
        right = target_right;
    }
}

bool GridCellSelectionRangeLessThan(const GridCellSelectionRange& first, const GridCellSelectionRange& second)
{
    /*
    int delta = first.top() - second.top();
    if (delta == 0)
        return first.left() < second.left();
     */
    int delta = first.left() - second.left();
    if (delta == 0)
        return first.top() < second.top();

    return delta < 0;
}

static void selection_range_compact(SelectionRanges& target)
{
    if (target.size() > 1)
    {
        SelectionRanges result;

        qSort(target.begin(), target.end(), GridCellSelectionRangeLessThan);

        SelectionRanges::iterator it = target.begin();

        GridCellSelectionRange current = *(it)++;
        while (it != target.end())
        {
            if (current.bottom() + 1 >= it->top() && current.left() == it->left() && current.right() == it->right())
            {
                current = current.unite(*it);// GridCellSelectionRange. second = std::max(current.second, it->second); 
            }
            else
            {
                result.push_back(current);
                current = *(it);
            }
            it++;
        }
        result.push_back(current);
        target = result;
    }
}

static void selection_range_merge(GridModelController* controller, const SelectionRanges& source, SelectionRanges& target, bool invert, bool unselect)
{
    SelectionRanges newRanges = source;
    SelectionRanges intersect;

    SelectionRanges::iterator it = newRanges.begin();
    while (it != newRanges.end())
    {
        if (!(*it).isValid())
        {
            it = newRanges.erase(it);
            continue;
        }

        for (SelectionRanges::const_iterator target_it = target.constBegin(); target_it != target.constEnd(); ++target_it)
        {
            if (target_it->intersects(*it))
                intersect.append((*target_it).intersected(*it));
        }
        ++it;
    }

    for (SelectionRanges::const_iterator it = intersect.constBegin(); it != intersect.constEnd(); ++it)
    {
        for (int i = 0; i < target.count();)
        {
            if (target.at(i).intersects(*it))
            {
                selection_range_subtract(controller, target.at(i), *it, target);
                target.removeAt(i);
                continue;
            }
            else
                ++i;
        }

        if (invert)
        {
            for (int i = 0; i < newRanges.count();)
            {
                if (newRanges.at(i).intersects(*it))
                {
                    selection_range_subtract(controller, newRanges.at(i), *it, newRanges);
                    newRanges.removeAt(i);
                    continue;
                }
                else
                     ++i;
            }
        }
    }

    if (!unselect)
        target += newRanges;

    selection_range_compact(target);
}


struct FilterProgressContext
{
    FilterProgressContext(GridModelController* mc)
        : controller(mc), count(0), value(0)
    {
    }

    ~FilterProgressContext()
    {
        setValue(100);
    }

    void setValue(int progressValue)
    {
        if (progressValue == value)
            return;
        value = progressValue;
        emit controller->filterProgress(value);
    }

    void nextValue()
    {
        ++count;
        int allCount = qMax(1, controller->getPureRowCount());
        int progressValue = qMin(count * 100 / allCount, 100);
        if (progressValue > value)
            setValue(progressValue);
    }

    GridModelController* controller;
    int count;
    int value;
};

struct SortProgressContext
{
    SortProgressContext(GridModelController* mc) : controller(mc), averageCount(0), count(0), value(0)
    {
        int size = controller->getRecordCount();
        //averageCount = qRound((size * log(size)));
        averageCount = qMax(1, qRound((size * qLn(size)) / 3) + (size / 3));
    }
    ~SortProgressContext()
    {
        setValue(100);
    }

    void setValue(int progressValue)
    {
        if (progressValue == value)
            return;
        value = progressValue;
        emit controller->sortProgress(value);
    }

    void nextValue()
    {
        ++count;
        int progressValue = qMin(count * 100 / averageCount, 100);
        if (progressValue > value)
            setValue(progressValue);
    }

    GridModelController* controller;
    int averageCount;
    int count;
    int value;
};


struct SummaryProgressContext
{
    SummaryProgressContext(GridModelController* mc) : controller(mc), allCount(0), count(0), value(0)
    {
        allCount = controller->getRecordCount() * controller->summary()->footerSettingsCount();

        for (int i = 0; i < controller->sortRules().groupRulesCount(); ++i)
        {
            const GridSortColumnRule* rule = controller->sortRules().groupRule(i);
            QVector<const GridSummaryGroupSetting *> settings;
            controller->summary()->getGroupSummarySettings(rule->dataBinding, settings);
            allCount += settings.size() * controller->getRecordCount();
        }
        allCount = qMax(1, allCount);
    }

    ~SummaryProgressContext()
    {
        setValue(100);
    }

    void setValue(int progressValue)
    {
        if (progressValue == value)
            return;
        value = progressValue;
        emit controller->summaryProgress(value);
    }

    void nextValue()
    {
        ++count;
        int progressValue = qMin(count * 100 / allCount, 100);
        if (progressValue > value)
            setValue(progressValue);
    }

    GridModelController* controller;
    int allCount;
    int count;
    int value;
};

/*!
    \class GridSummaryValue
    \inmodule QtitanDataGrid
*/

/*!
    \enum SortOrder
    This enum describes sort order for column.
    \inmodule QtitanDataGrid
    \value SortNone No sorting.
    \value SortAscending Low to high sorting.
    \value SortDescending High to low sorting.
*/

/*!
    \class GridModelController
    \inmodule QtitanDataGrid
    \brief GridModelController class processes the tabular data from Qt models and provides them in a convenient way to display in the view.
    Inside of it are performed grouping and sorting operations, focus handling, column's data binding and other important data-related operations.
*/
GridModelController::GridModelController(GridViewInterface* viewInterface)
    : m_model(Q_NULL), m_selectionModel(Q_NULL),
  m_frozenRowsTopCount(0), m_frozenRowsBottomCount(0),
  m_focusedRowIndex(Q_NULL),
  m_oldFocusCell(),
  m_allSelected(false),
  m_selection(Q_NULL),
  m_selectStack(0),
  m_updateDataStack(0), m_addingModelRowIndex(-1),
  m_viewInterface(viewInterface), m_pureRowCount(0),
  m_recordSortingDirty(true), m_recordGroupingDirty(true), m_needSubmit(false),
  m_showWaitCursor(true), m_editStrategy(Qtitan::OnRowChange), m_frozenPlace(GridRow::FrozenTop), m_changes(ControllerNoChanges)
{
    m_focusedRowIndex = new GridPersistentRow(this);
    m_summary = new GridSummary(this);
    m_filter = new GridFilter(this);
    m_relationFilter = new GridFilter(this);
    m_frozenRowsFilter = new GridFilter(this);
    m_findFilter = new GridFilter(this);
    m_selection = new GridSelection(this);
    m_previewRowDataBinding = new GridModelDataBinding(this, 0);
    m_previewRowDataBinding->setDisplayRole(Qt::WhatsThisRole);
}

GridModelController::~GridModelController()
{
    Q_DELETE_AND_NULL(m_focusedRowIndex);
    setModel(Q_NULL);
    resizeModelRows(0);
    clearPersistRows();
    Q_DELETE_AND_NULL(m_summary);
    Q_DELETE_AND_NULL(m_filter);
    Q_DELETE_AND_NULL(m_relationFilter);
    Q_DELETE_AND_NULL(m_frozenRowsFilter);
    Q_DELETE_AND_NULL(m_findFilter);
    Q_DELETE_AND_NULL(m_viewInterface);
    Q_DELETE_AND_NULL(m_selection);
    Q_DELETE_AND_NULL(m_previewRowDataBinding);
}

void GridModelController::clearPersistRows()
{
    for (QSet<GridPersistentRow *>::const_iterator it = m_persistRows.constBegin(); it != m_persistRows.constEnd(); ++it)
        (*it)->m_controller = Q_NULL;
    m_persistRows.clear();
}

int Qtitan::variant_compare(const QVariant &v1, const QVariant& v2, bool caseSensitivity, bool localeAware)
{
    switch (v1.userType()) {
    case QVariant::Invalid:
        if (v2.type() == QVariant::Invalid)
            return 0;
        else
            return -1;
    case QVariant::Bool:
        return v1.toBool() - v2.toBool();
    case QVariant::Int:
        return v1.toInt() - v2.toInt();
    case QVariant::UInt:
        return v1.toUInt() - v2.toUInt();
    case QVariant::LongLong:
        return v1.toLongLong() - v2.toLongLong();
    case QVariant::ULongLong:
        return v1.toULongLong() - v2.toULongLong();
    case QVariant::Double:
        {
            double val = v1.toDouble() - v2.toDouble();
            if (val == 0.0)
                return 0;
            return val < 0.0 ? -1 : 1;
        }
    case QVariant::Char:
        return v1.toChar().unicode() - v2.toChar().unicode();
    case QVariant::Date:
        return v1.toDate().toJulianDay() - v2.toDate().toJulianDay();
    case QVariant::Time:
        return v1.toTime().msec() - v2.toTime().msec();
    case QVariant::DateTime:
        if (v1.toString() < v2.toString())
            return -1;
        else if (v1 == v2)
            return 0;
        else
            return 1;
    case QVariant::String:
    default:
        if (localeAware)
            return v1.toString().localeAwareCompare(v2.toString());
        else
            return v1.toString().compare(v2.toString(), caseSensitivity ? Qt::CaseSensitive : Qt::CaseInsensitive);
    }
}


template <class ContextClass, typename T>
static 
void quick_sort(ContextClass* context, QVector<T>& data, int l, int r, 
                int(*func)(ContextClass* context, const T& val1, const T& val2), SortProgressContext* progressContext = Q_NULL)
{
    int i, j;
    T cur, t;
    do
    {
        i = l;
        j = r;
        cur = data[(l + r) >> 1];
        do
        {
            while (func(context, data[i], cur) < 0)
            {
                i++;
                if (progressContext != Q_NULL)
                    progressContext->nextValue();
            }

            while (func(context, data[j], cur) > 0)
            {
                j--;
                if (progressContext != Q_NULL)
                    progressContext->nextValue();
            }

            if (i <= j)
            {
                t = data[i];
                data[i] = data[j];
                data[j] = t;
                ++i;
                --j;
            }
        }
        while (i <= j);

        if (l < j)
            quick_sort(context, data, l, j, func, progressContext);
        l = i;
    }
    while (i < r);
}

bool GridModelController::getModelValue(int modelColumn, int modelRow, Qt::ItemDataRole role, QVariant& retval) const
{
    Q_ASSERT(m_model != Q_NULL);
    QModelIndex index = m_model->index(modelRow, modelColumn, m_parentIndex);
    if (!index.isValid())
        return false;
    retval = m_model->data(index, role);
    return true;
}

bool GridModelController::setModelValue(int modelColumn, int modelRow, Qt::ItemDataRole role, const QVariant& value)
{
    Q_ASSERT(m_model != Q_NULL);
    QModelIndex index = m_model->index(modelRow, modelColumn, m_parentIndex);
    if (!index.isValid())
        return false;
    bool ret = m_model->setData(index, value, role);
    
    if (ret)
        m_needSubmit = true;
    return ret;
}

bool GridModelController::getPreviewModelValue(int modelRow, QVariant& retval)
{
    return getModelValue(m_previewRowDataBinding->column(), modelRow, m_previewRowDataBinding->displayRole(), retval);
}

bool GridModelController::submitData()
{
    if (!m_needSubmit)
        return true;

    if (model() == Q_NULL || model()->submit())
        m_needSubmit = false;

    return !m_needSubmit;
}

int GridModelController::focusedRowIndex()
{
    GridRow* row = m_focusedRowIndex->row();
    return row != Q_NULL ? row->rowIndex() : -1;
}

int GridModelController::oldFocusedRowIndex() const
{
    return m_oldFocusCell.rowIndex();
}

int GridModelController::oldFocusedColumnIndex() const
{
    return m_oldFocusCell.columnIndex();
}

GridRow* GridModelController::focusedRow()
{
    return m_focusedRowIndex->row();
}

bool GridModelController::isRowFocused(GridRow* row)
{
    return m_focusedRowIndex->equal(row);
}

int GridModelController::focusedColumnIndex() const
{
    return m_focusedRowIndex->columnIndex();
}

bool GridModelController::setCellFocus(GridRow* row, int columnIndex)
{
    bool ret = false;
    m_oldFocusCell = GridCell(m_focusedRowIndex->row(), m_focusedRowIndex->columnIndex());
    if (!m_focusedRowIndex->equal(row))
    {
        m_focusedRowIndex->set(row);
        ret = true;
    }

    int oldFocusColumnIndex = m_focusedRowIndex->columnIndex();
    if (oldFocusColumnIndex != columnIndex)
    {
        m_focusedRowIndex->setColumnIndex(columnIndex);
        ret = true;
    }

    if (ret)
    {
        m_changes |= ControllerFocusChanged;
        applyChanges();
    }
    return ret;
}

bool GridModelController::isCellFocused(GridRow* row, int columnIndex) const
{
    return m_focusedRowIndex->equal(row) && m_focusedRowIndex->columnIndex() == columnIndex;
}

GridSelection* GridModelController::selection() const
{
    m_selection->reset();
    return m_selection;
}

void GridModelController::beginUpdate()
{
    if (m_updateDataStack == 0)
    {
    //    m_insertModelRows.clear();
    //    m_removeModelRowsFlag = false;
    }
    m_updateDataStack++;
}

void GridModelController::endUpdate()
{
    m_updateDataStack--;
    applyChanges();
}

void GridModelController::applyChanges()
{
    if (isDataUpdating() || m_viewInterface->isDestroying())
        return;

    emit aboutToBeChanged();

    bool clearCache = false;


    if (m_recordSortingDirty)
    {
        fillRecords();
        createModelGroups(); clearCache = true;
        m_recordSortingDirty = false;
        m_recordGroupingDirty = false;
    }

    if (m_recordGroupingDirty)
    {
        createModelGroups(); clearCache = true;
        m_recordGroupingDirty = false;
    }

    if ((m_changes & ControllerSortingChanged) || (m_changes & ControllerGroupingChanged))
    {
        if (!clearCache)
        {
            for (QSet<GridPersistentRow *>::const_iterator it = m_persistRows.constBegin(); it != m_persistRows.constEnd(); ++it)
                (*it)->m_gridRow = Q_NULL;
        }
        createGridRows();
    }

    if ((m_changes & ControllerRowsChanged) || (m_changes & ControllerDataChanged))
        m_summary->markChanged();

    m_summary->calculate();

    GridControllerChanges temp = m_changes;
    m_changes = ControllerNoChanges;
    if (temp != ControllerNoChanges)
        m_viewInterface->controllerChanged(temp);

    emit changed();
}

bool GridModelController::isDataUpdating() const
{
    return m_updateDataStack > 0;
}

bool GridModelController::hasChanges() const
{
    return m_changes != ControllerNoChanges;
}

bool GridModelController::insertModelRow(int row)
{
    if (m_model == Q_NULL)
        return false;
    if (!m_model->insertRow(row, m_parentIndex))
        return false;

    for (QHash<int, DefaultValueMap>::const_iterator it = m_defValues.constBegin(); it != m_defValues.constEnd(); ++it)
    {
        for (DefaultValueMap::const_iterator jt = it.value().constBegin(); jt != it.value().constEnd(); ++jt)
        {
            setModelValue(it.key(), row, jt.key(), jt.value());
        }
    }
    return true;
}

bool GridModelController::removeModelRow(int modelRow)
{
    if (m_model == Q_NULL)
        return false;

    if (!m_model->removeRow(modelRow, m_parentIndex))
        return false;

    m_needSubmit = true;

    return true;
}

void GridModelController::beginAddRow(int row)
{
    m_addingModelRowIndex = row;
}

void GridModelController::endAddRow()
{
    m_addingModelRowIndex = -1;
}

bool GridModelController::isNewRowMode() const
{
    return m_addingModelRowIndex != -1;
}

int GridModelController::addingModelRowIndex() const
{
    return m_addingModelRowIndex;
}

QVariant GridModelController::defaultValue(int modelColumn, Qt::ItemDataRole role) const
{
    QHash<int, DefaultValueMap>::const_iterator it = m_defValues.constFind(modelColumn);
    if (it == m_defValues.constEnd())
        return QVariant();
    DefaultValueMap::const_iterator jt = it.value().constFind(role);
    if (jt == it.value().constEnd())
        return QVariant();
    return jt.value();
}

void GridModelController::setDefaultValue(int modelColumn, const QVariant& value, Qt::ItemDataRole role)
{
    m_defValues[modelColumn][role] = value;
}

void GridModelController::removeDefaultValue(int modelColumn, Qt::ItemDataRole role)
{
    QHash<int, DefaultValueMap>::iterator it = m_defValues.find(modelColumn);
    if (it == m_defValues.end())
        return;
    it.value().remove(role);
    if (it.value().size() == 0)
        m_defValues.erase(it);
}

void GridModelController::removeDefaultValues(int modelColumn)
{
    m_defValues.remove(modelColumn);
}

void GridModelController::removeDataBinding(GridModelDataBinding* dataBinding)
{
    Q_ASSERT(isDataUpdating());
    Q_ASSERT(dataBinding != Q_NULL);
    filter()->clear();
    filter()->m_active = false;
    
    m_currentSelectionRanges.clear();
    m_selectionRanges.clear();
    m_selection->clear();
    clearModelGroups();
    m_focusedRowIndex->clear();

    removeDefaultValues(dataBinding->column());
}

/*!
Indicates whether or not to show the wait cursor when the controller performs lengthy operations with data like sorting or grouping.
*/
bool GridModelController::showWaitCursor()
{
    return m_showWaitCursor;
}

/*!
Enables or disables wait cursor if controller is busy for lengthy operations with data like sorting or grouping.
*/
void GridModelController::setShowWaitCursor(bool show)
{
    m_showWaitCursor = show;
}

/*!
Returns the strategy for data submiting to the model after the cell's value has changed.
*/
GridEditStrategy GridModelController::editStrategy() const
{
    return m_editStrategy;
}

/*!
Sets the strategy for data submiting to the model after the cell's value has changed. Please note what the setting does not has effect if QSqlTableModel is used with 
edit strategy equal to QSqlTableModel::OnFieldChange.
*/
void GridModelController::setEditStrategy(GridEditStrategy strategy)
{
    m_editStrategy = strategy;
}

GridRow::FrozenPlace GridModelController::frozenPlace() const
{
    return m_frozenPlace;
}

void GridModelController::setFrozenPlace(GridRow::FrozenPlace place)
{
    m_frozenPlace = place;
    if (place == GridRow::FrozenNone)
        m_frozenPlace = GridRow::FrozenTop;
}

void GridModelController::addModelRowToFrozenFilter(int modelRowIndex, GridRow::FrozenPlace place)
{
    ModelControllerAutoUpdater updater(this);

    GridFilterGroupCondition* groupCondition = m_frozenRowsFilter->condition();
    if (groupCondition == Q_NULL)
    {
        groupCondition = new GridFilterGroupCondition(m_frozenRowsFilter);
        groupCondition->addCondition(new GridFilterFrozenValueCondition(m_frozenRowsFilter));
        m_frozenRowsFilter->setCondition(groupCondition);
    }
    GridFilterCondition::GroupOperator op;
    GridFilterFrozenValueCondition* condition = static_cast<GridFilterFrozenValueCondition *>(groupCondition->getCondition(0, op));
    if (place != GridRow::FrozenNone)
        condition->addFrozenModelRow(modelRowIndex, place == GridRow::FrozenTop);
    else
        condition->removeFrozenModelRow(modelRowIndex);
    if (!m_frozenRowsFilter->isActive())
        m_frozenRowsFilter->setActive(true);

    m_recordSortingDirty = true;
    m_summary->markChanged();
    modelLayoutChanged();
}

void GridModelController::removeModelRowFromFrozenFilter(int modelRowIndex)
{
    ModelControllerAutoUpdater updater(this);

    GridFilterGroupCondition* groupCondition = m_frozenRowsFilter->condition();
    if (groupCondition == Q_NULL)
        return;

    GridFilterCondition::GroupOperator op;
    GridFilterFrozenValueCondition* condition = static_cast<GridFilterFrozenValueCondition *>(groupCondition->getCondition(0, op));
    condition->removeFrozenModelRow(modelRowIndex);

    if (condition->conditionCount() == 0)
        m_frozenRowsFilter->setActive(false);

    m_recordSortingDirty = true;
    m_summary->markChanged();
    modelLayoutChanged();
}

GridModelDataBinding* GridModelController::previewRowDataBinding() const
{
    return m_previewRowDataBinding;
}

const GridSortColumnRules& GridModelController::sortRules() const
{
    return m_sortRules;
}

void GridModelController::beginSelect()
{
    m_selectStack++;
}

void GridModelController::endSelect()
{
    Q_ASSERT(m_selectStack != 0);
    if (m_selectStack > 0)
        m_selectStack--;
    if (m_selectStack == 0)
    {
        emit selectChanged();
    }
}

void GridModelController::clearSelect()
{
    selectRanges(SelectionRanges(), Qtitan::Clear);
}

//void GridModelController::selectAll()
//{
//    int rowCount = getGridRowCount();
//    if (rowCount == 0)
//        return;
//    SelectionRanges r;`
//    r.append(GridCellSelectionRange(getGridRow(0), getGridRow(rowCount -1)));
//    selectRanges(r);
//}

void GridModelController::selectRanges(const SelectionRanges& ranges, const SelectionOperation& operation)
{
     if (operation == Qtitan::Empty)
        return;

    SelectAutoUpdater updater(this);

    //Check existed selection ranges
    SelectionRanges::iterator it = m_selectionRanges.begin();
    while (it != m_selectionRanges.end()) {
      if (!it->isValid())
        it = m_selectionRanges.erase(it);
      else
        ++it;
    }

    // store old selection to emit signal
    GridSelection oldSelection(this);

    oldSelection.m_ranges = m_selectionRanges;
    selection_range_merge(this, m_currentSelectionRanges, oldSelection.m_ranges, operation & Qtitan::Invert, operation & Qtitan::Unselect);

    SelectionRanges sel = ranges;

    if (operation & Qtitan::Clear) {
        m_selectionRanges.clear();
        m_currentSelectionRanges.clear();
    }

    // commit old selection to start current selection
    if (!(operation & Qtitan::StartCurrent))
        commitCurrentSelect();

    // Set new current selection
    if (operation & Qtitan::Invert || operation & Qtitan::Select || operation & Qtitan::Unselect) {
        m_currentSelectionOperation = operation;
        m_currentSelectionRanges = sel;
    }
    m_selection->clear();
    m_selection->m_ranges = m_selectionRanges;
    selection_range_merge(this, m_currentSelectionRanges, m_selection->m_ranges, m_currentSelectionOperation & Qtitan::Invert, m_currentSelectionOperation & Qtitan::Unselect);

    m_allSelected = false;
    if (m_selection->ranges().size() > 0)
    {
       int leftColumnInRange = m_selection->ranges()[0].left();
       
        GridCellSelectionRange allrange(this, GridCell(this, 0, leftColumnInRange), GridCell(this, getGridRowCount() - 1, leftColumnInRange));
        m_allSelected = m_selection->ranges()[0].contains(allrange);
        
        //for ( SelectionRanges::const_iterator it = m_selection->ranges().constBegin(); it != m_selection->ranges().constEnd(); ++it)
        {
            //if (it->contains(allrange))
            {
         //       m_allSelected = true;
         //       break;
            }
        }
        
    }
    m_viewInterface->raiseSelectionChanged(m_selection, &oldSelection);
}

void GridModelController::commitCurrentSelect()
{
    SelectAutoUpdater updater(this);
    selection_range_merge(this, m_currentSelectionRanges, m_selectionRanges, m_currentSelectionOperation & Qtitan::Invert, m_currentSelectionOperation & Qtitan::Unselect);
    m_currentSelectionRanges.clear();
}

static bool is_row_selected(const SelectionRanges& ranges, const GridRow* row)
{
    for (SelectionRanges::const_iterator it = ranges.constBegin(); it != ranges.constEnd(); ++it)
    {
        if (it->contains(row))
            return true;
    }
    return false;
}

bool GridModelController::isRowSelected(const GridRow* row) const
{
    bool selected = is_row_selected(m_selectionRanges, row);
    bool selectedInCurrent = is_row_selected(m_currentSelectionRanges, row);

    if ((m_currentSelectionOperation & Qtitan::Unselect) && selected)
        selected = !selectedInCurrent;
    else if (m_currentSelectionOperation & Qtitan::Invert)
        selected ^= selectedInCurrent;
    else if ((m_currentSelectionOperation & Qtitan::Select) && !selected)
        selected = selectedInCurrent;

    return selected;
}

static bool is_cell_selected(const SelectionRanges& ranges, const GridCell& cell)
{
    for (SelectionRanges::const_iterator it = ranges.constBegin(); it != ranges.constEnd(); ++it)
    {
        if (it->contains(cell))
            return true;
    }
    return false;
}

bool GridModelController::isCellSelected(const GridCell& cell) const
{
    bool selected = is_cell_selected(m_selectionRanges, cell);
    bool selectedInCurrent = is_cell_selected(m_currentSelectionRanges, cell);
    if ((m_currentSelectionOperation & Qtitan::Unselect) && selected)
        selected = !selectedInCurrent;
    else if (m_currentSelectionOperation & Qtitan::Invert)
        selected ^= selectedInCurrent;
    else if ((m_currentSelectionOperation & Qtitan::Select) && !selected)
        selected = selectedInCurrent;
    return selected;
}

bool GridModelController::isAllSelected() const
{
    return m_allSelected;
}

int GridModelController::getGridRowCount() const
{
    return m_gridRows.size();
}

int GridModelController::getRecordCount() const
{
    return m_recordsMap.size();
}

int GridModelController::getModelRowIndexByRecord(int record) const
{
#if 0
    if (record >= m_recordsMap.size())
    {
        int retval = m_recordsMap[record];
        Q_ASSERT(false);

    }
#endif
    return m_recordsMap[record];
}

int GridModelController::getModelColumnCount() const
{
    if (m_model == Q_NULL)
        return 0;
    return m_model->columnCount(m_parentIndex);
}

GridRow* GridModelController::getGridRow(int index)
{
    if (index < 0 || index >= m_gridRows.size())
        return Q_NULL;

    GridRow* retval = &m_gridRows[index];
    if (!retval->isInitialized())
    {
        if (!hasGrouping() || index < m_frozenRowsTopCount || index >= m_gridRows.size() - m_frozenRowsBottomCount)
        {
            retval->initialize(this, GridRow::DataRow, Q_NULL, index);
        }
        else
        {
            int groupIndex;
            GridModelGroup* group = findGroup(index, groupIndex);
            Q_ASSERT(group != Q_NULL);
            if (group->rowIndex == index)
                retval->initialize(this, GridRow::GroupRow, group, index);
            else
                retval->initialize(this, GridRow::DataRow, group, index);
        }
    }
    return retval;
}

GridRow* GridModelController::getGridRowByModelRow(int modelRow)
{
    return getGridRow(findRowIndexByRecord(findRecordByModelRow(modelRow)));
}

int GridModelController::getFrozenTopGridRowCount() const
{
    return m_frozenRowsTopCount;
}

int GridModelController::getFrozenBottomGridRowCount() const
{
    return m_frozenRowsBottomCount;
}

void GridModelController::expandAllGroup()
{
    ModelControllerAutoUpdater updater(this);

    for (ModelGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        GridModelGroup& group = *it;
        expandGroup(&group, true);
    }
}

void GridModelController::collapseAllGroup()
{
    ModelControllerAutoUpdater updater(this);

    for (ModelGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        GridModelGroup& group = *it;
        collapseGroup(&group, true);
    }
}

bool GridModelController::expandGroup(GridModelGroup* group, bool disableChanged)
{
    if (group->expanded)
        return true;
    bool allow = true;
    m_viewInterface->raiseGroupExpanding(group, allow);
    if (!allow)
        return false;
    
    group->expanded = true;
    m_changes |= ControllerGroupingChanged;

    m_viewInterface->raiseGroupExpanded(group);
    if (disableChanged)
        return true;
    
    applyChanges();
    return true;
}

bool GridModelController::collapseGroup(GridModelGroup* group, bool disableChanged)
{
    if (!group->expanded)
        return true;
    bool allow = true;
    m_viewInterface->raiseGroupCollapsing(group, allow);
    if (!allow)
        return false;

    group->expanded = false;
    m_changes |= ControllerGroupingChanged;

    m_viewInterface->raiseGroupCollapsed(group);
    if (disableChanged)
        return true;
    
    applyChanges();
    return true;
}

void GridModelController::resizeModelRows(int size)
{
    int oldsize = m_gridRows.size();
    if (size != oldsize)
    {
#ifdef _DEBUG
        for (int i = size; i < oldsize; ++i)
            m_gridRows[i].m_controller = Q_NULL;
#endif
        m_gridRows.resize(size);
    }

    if (size > oldsize)
    {
        for (QVector<GridRow>::iterator it = m_gridRows.begin() + oldsize; it != m_gridRows.end(); ++it)
            it->reset();
    }
}

int GridModelController::compareModelRows(GridModelController* controller,
                                          const int& firstRow, const int& secondRow)
{
    if (firstRow == secondRow)
        return 0;

    if (controller->m_frozenRows.size() > 0)
    {
        int firstval = 0;
        int secondval = 0;
        QHash<int, bool>::const_iterator first_it = controller->m_frozenRows.find(firstRow);
        if (first_it != controller->m_frozenRows.constEnd())
        {
            if (first_it.value())
                firstval = -1;
            else
                firstval = 1;
        }

        QHash<int, bool>::const_iterator second_it = controller->m_frozenRows.find(secondRow);
        if (second_it != controller->m_frozenRows.constEnd())
        {
            if (second_it.value())
                secondval = -1;
            else
                secondval = 1;
        }

        if (firstval != secondval)
            return firstval - secondval;
    }

    for (int i = 0; i < controller->m_sortRules.count(); ++i)
    {
        const GridSortColumnRule& rule = controller->m_sortRules.rule(i);
        QVariant v1;
        rule.dataBinding->getValue(firstRow, rule.dataBinding->sortRole(), v1);
        QVariant v2;
        rule.dataBinding->getValue(secondRow, rule.dataBinding->sortRole(), v2);

        int result = variant_compare(v1, v2);
        if (result != 0)
            return rule.order == SortAscending ? result : -result;
    }
    return firstRow - secondRow;
}

template <typename T>
static GridModelGroup* do_find_group(GridModelController* controller, 
                                 T value, 
                                 bool (*compare_func)(GridModelGroup* group, const T& value), 
                                 int& groupIndex)
{
    GridModelGroup* retval = Q_NULL;
    int hi = controller->modelGroupCount();
    int low = -1;
    while (hi - low > 1)
    {
        groupIndex = (low + hi) / 2;
        GridModelGroup* temp = controller->modelGroup(groupIndex);
        if (compare_func(temp, value))
            hi = groupIndex;
        else
            low = groupIndex;
    }

    groupIndex = low;

    retval = controller->modelGroup(groupIndex);
    while (groupIndex > 0)
    {
        GridModelGroup* temp = controller->modelGroup(groupIndex - 1);
        if (temp->rowIndex == retval->rowIndex)
        {
            retval = temp;
            groupIndex--;
            continue;
        }
        else
            break;
    }
    return retval;
}

static int do_find_modelRow(GridModelController* controller, 
                            int value, 
                            bool (*compare_func)(GridRow* modelRow, const int& value))
{
    int rowIndex = -1;
    int hi = controller->getGridRowCount();
    int low = -1;
    while (hi - low > 1)
    {
        rowIndex = (low + hi) / 2;
        GridRow* temp = controller->getGridRow(rowIndex);
        if (compare_func(temp, value))
            hi = rowIndex;
        else
            low = rowIndex;
    }
    return low;
}

static bool group_compare_by_record(GridModelGroup* group, const int& record)
{
    return group->firstRecord > record;
}

int GridModelController::ensureVisible(int modelRow)
{
    int record = findRecordByModelRow(modelRow);
    Q_ASSERT(record != -1);
    int groupIndex = 0;
    if (hasGrouping())
    {
        GridModelGroup* group = do_find_group<int>(this, record, group_compare_by_record, groupIndex);
        while (group != Q_NULL && !group->expanded)
        {
            if (!expandGroup(group, false))
                return -1;
            groupIndex++;
            if (groupIndex >= modelGroupCount())
                break;
            group = modelGroup(groupIndex);
            if (group == Q_NULL || (group->firstRecord < record || record > group->lastRecord))
                break;
        }
    }
    return findRowIndexByRecord(record);
}

static bool group_compare_by_row(GridModelGroup* group, const int& row)
{
    return group->rowIndex > row;
}

GridModelGroup* GridModelController::findGroup(int rowIndex, int& groupIndex)
{
    return do_find_group<int>(this, rowIndex, group_compare_by_row, groupIndex);
}

int GridModelController::findRowIndexByRecord(int record)
{
    return do_find_modelRow(this, record, GridRow::compareByRecord);
}

int GridModelController::findRecordByModelRow(int modelRow)
{
    int rowCount = m_recordsMap.size();
    int* mapPtr = m_recordsMap.data();
    for (int i = 0; i < rowCount; ++i)
        if (mapPtr[i] == modelRow)
            return i;
    return -1;
}

bool GridModelController::addModelRowToMap(int modelRowIndex)
{
    if (m_addingModelRowIndex == -1 || m_addingModelRowIndex != modelRowIndex)
    {
        if ((filter()->isActive() && !filter()->validateRow(modelRowIndex)) || 
            (relationFilter()->isActive() && !relationFilter()->validateRow(modelRowIndex)) ||
            (findFilter()->isActive() && !findFilter()->validateRow(modelRowIndex)))
        {
            return false;
        }
    }
    
    if (frozenRowsFilter()->isActive() && frozenRowsFilter()->validateRow(modelRowIndex))
    {
        GridFilterCondition::GroupOperator op;
        GridFilterFrozenValueCondition* condition = static_cast<GridFilterFrozenValueCondition *>(frozenRowsFilter()->condition()->getCondition(0, op));
        addFrozenModelRow(modelRowIndex, condition->getFrozenPosition(modelRowIndex));
    }

    int insertIndex = -1;

    int l = -1;
    int r = m_recordsMap.size();

    while (r - l > 1)
    {
        int cur = (r + l) >> 1;
        if (compareModelRows(this, getModelRowIndexByRecord(cur), modelRowIndex) > 0)
            r = cur;
        else
            l = cur;
    }
    insertIndex = l + 1;

    m_recordsMap.insert(insertIndex, modelRowIndex);
    m_recordGroupingDirty = true;
    return true;
}

void GridModelController::modelColumnsInserted(const QModelIndex & parent, int start, int end)
{
    if (parent != m_parentIndex || m_viewInterface == Q_NULL)
        return;
    for (int i = start; i <= end; ++i)
        m_viewInterface->columnInserted(i);
}

void GridModelController::modelColumnsMoved(const QModelIndex & sourceParent, int sourceStart, int sourceEnd, 
                                            const QModelIndex & destinationParent, int destinationColumn)
{
    Q_UNUSED(sourceParent);
    Q_UNUSED(sourceStart);
    Q_UNUSED(sourceEnd);
    Q_UNUSED(destinationColumn);
    if (destinationParent != m_parentIndex)
        return;
}

void GridModelController::modelColumnsRemoved(const QModelIndex & parent, int start, int end)
{
    if (parent != m_parentIndex || m_viewInterface == Q_NULL)
        return;
    for (int i = start; i <= end; ++i)
        m_viewInterface->columnRemoved(i);
}

void GridModelController::modelLayoutChanged()
{
    m_changes |= ControllerLayoutChanged;
    applyChanges();
}

void GridModelController::modelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    if (topLeft.parent() != m_parentIndex)
        return;

    m_changes |= ControllerDataChanged;

    bool sortneeded = false;
    for (int i = topLeft.column(); i <= bottomRight.column(); ++i)
    {
        m_summary->markChanged(i);
        if (sortneeded)
            continue;

        for (int j = 0; j < m_sortRules.count(); ++j)
        {
            const GridSortColumnRule& rule = m_sortRules.rule(j);
            if (rule.dataBinding->column() == i)
            {
                sortneeded = true;
                break;
            }
        }
    }


    if (sortneeded || filter()->isActive() || relationFilter()->isActive() || findFilter()->isActive())
    {
        modelRowsRemovedInt(topLeft.row(), bottomRight.row(), true);
        modelRowsInsertedInt(topLeft.row(), bottomRight.row(), true);
    }

    applyChanges();
}

void GridModelController::modelRowsAboutToBeInserted(const QModelIndex& parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
}

void GridModelController::modelRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
}

void GridModelController::modelRowsInsertedInt(int start, int end, bool change)
{
    //Fixup indexies of the model's rows.
    int rowCount = m_recordsMap.size();
    int* mapPtr = m_recordsMap.data();
    if (!change)
    {
        m_summary->markChanged();

        m_pureRowCount += ((end - start) + 1);

        for (int i = 0; i < rowCount; ++i)
        {
            if (mapPtr[i] >= start)
                mapPtr[i] += (end - start + 1);
        }

        for (QSet<GridPersistentRow *>::const_iterator it = m_persistRows.constBegin(); it != m_persistRows.constEnd(); ++it)
        {
            GridPersistentRow* persistentRow = *it;
            if (persistentRow->modelRowIndex() >= start)
            {
                persistentRow->m_modelRowIndex += (end - start + 1);
                persistentRow->setRecord(-1);
            }
        }
        if (m_addingModelRowIndex != -1 && !isDataUpdating())
        {
            if (m_addingModelRowIndex >= start)
                m_addingModelRowIndex += (end - start + 1);
        }
    }
 
    for (int index = start; index <= end; ++index)
        addModelRowToMap(index);
}

void GridModelController::modelRowsInserted(const QModelIndex& parent, int start, int end)
{
    if (parent != m_parentIndex)
        return;

    modelRowsInsertedInt(start, end, false);

    applyChanges();
}

void GridModelController::modelRowsRemovedInt(int start, int end, bool change)
{
    int rowCount = m_recordsMap.size();
    for (int i = rowCount -1; i >= 0; --i)
    {
        // Fixup indexies of the model's rows. 
        if (m_recordsMap[i] >= start && m_recordsMap[i] <= end)
        {
            removeFrozenModelRow(m_recordsMap[i]);
            m_recordsMap.remove(i);
            m_recordGroupingDirty = true;
        }
        else if (m_recordsMap[i] > end && !change)
        {
            m_recordsMap[i] -= (end - start + 1);
        }
    }

    //Reset persistent rows and remove fixed and hidden rows.
    if (!change)
    {
        m_summary->markChanged();

        m_pureRowCount -= ((end - start) + 1);

        for (QSet<GridPersistentRow *>::const_iterator it = m_persistRows.constBegin(); it != m_persistRows.constEnd(); ++it)
        {
            GridPersistentRow* persistentRow = *it;
            if (persistentRow->modelRowIndex() >= start && persistentRow->modelRowIndex() <= end)
            {
                persistentRow->clear(); //Row is not valid anymore. 
            } 
            else if (persistentRow->modelRowIndex() > end)
            {
                persistentRow->m_modelRowIndex -= (end - start + 1);
                persistentRow->setRecord(-1);
            }
        }

        if (m_addingModelRowIndex != -1 && !isDataUpdating())
        {
            if (m_addingModelRowIndex >= start && m_addingModelRowIndex <= end)
            {
                //Model wishes to remove adding row. In this case we should to raise NewRowCancel event to the view. 
                Q_ASSERT(false);
            }
            else if (m_addingModelRowIndex > end)
                m_addingModelRowIndex -= (end - start + 1);
        }
    }
}

void GridModelController::modelRowsRemoved(const QModelIndex& parent, int start, int end)
{
    if (parent != m_parentIndex)
        return;

    modelRowsRemovedInt(start, end, false);

    applyChanges();
}

void GridModelController::addFrozenModelRow(int modelRow, bool topPosition)
{
    if (m_frozenRows.contains(modelRow))
        removeFrozenModelRow(modelRow);
    m_frozenRows.insert(modelRow, topPosition);
    if (topPosition)
        m_frozenRowsTopCount++;
    else
        m_frozenRowsBottomCount++;
}

void GridModelController::removeFrozenModelRow(int modelRow)
{
    QHash<int, bool>::iterator it = m_frozenRows.find(modelRow);
    if (it == m_frozenRows.constEnd())
        return;
    if (it.value())
        m_frozenRowsTopCount--;
    else
        m_frozenRowsBottomCount--;

    m_frozenRows.erase(it);
}

void GridModelController::modelAboutToBeReset()
{
    beginUpdate();
}

void GridModelController::clearFilters(bool clearHistory)
{
    filter()->clear();
    filter()->m_active = false;
    relationFilter()->clear();
    relationFilter()->m_active = false;
    findFilter()->clear();
    findFilter()->m_active = false;
    if (clearHistory)
    {
        filter()->history()->clear();
        relationFilter()->history()->clear();
        findFilter()->history()->clear();
    }
}

void GridModelController::modelReset()
{
    m_recordSortingDirty = true;

    m_summary->markChanged();
    m_viewInterface->resetModel();
    endUpdate();
}

/*!
\internal
*/

bool GridRow::compareByScrollPosition(GridRow* modelRow, const int& ypos)
{
    return modelRow->m_scrollPosition > ypos;
}

/*!
\internal
*/
bool GridRow::compareByRecord(GridRow* modelRow, const int& record)
{
    return modelRow->record() > record;
}

/*!
\internal
*/
int Qtitan::find_modelRowIndex_by_view(GridModelController* controller, int ypos)
{
    return do_find_modelRow(controller, ypos, GridRow::compareByScrollPosition);
}

/*!
\internal
*/
void GridModelController::createGridRows()
{
    m_changes |= ControllerRowsChanged;
    m_oldFocusCell = GridCell();

    if (!hasGrouping())
    {
        resizeModelRows(0);
        resizeModelRows(m_recordsMap.size());
    }
    else
    {
        resizeModelRows(0);
        resizeModelRows(m_recordsMap.size());

        int currentDepth = 0;
        int currentRowIndex = m_frozenRowsTopCount - 1;

        for (ModelGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
        {
            GridModelGroup& group = *it;
            if (group.depth != currentDepth)
            {
                Q_ASSERT(group.depth < currentDepth);
                currentDepth = group.depth;
            }

            currentRowIndex++;
            group.rowIndex = currentRowIndex;

            if (!group.expanded)
            {
#if 0
                for (QSet<GridPersistentRow *>::const_iterator p_it = m_persistRows.constBegin();
                    p_it != m_persistRows.constEnd(); ++p_it)
                {
                    GridPersistentRow* p = *p_it;
                    if (p->groupColumn != -1)
                        continue;

                    if (group.firstRecord <= p->record && p->record <= group.lastRecord)
                    {
                        p->groupColumn = group.dataBinding->column();
                        p->record = group.firstRecord;
                        p->modelRowIndex = getModelRowIndexByRecord(p->record);
                    }
                }
#endif
                while (it + 1 != m_groups.end())
                {
                    GridModelGroup& next = *(it + 1);
                    if (next.depth > group.depth)
                    {
                        next.rowIndex = currentRowIndex;
                        it++;
                        continue;
                    }
                    break;
                }
            }
            else if (currentDepth == m_sortRules.groupRulesCount() - 1)
            {
                currentRowIndex += group.lastRecord - group.firstRecord + 1;
            }
            else
            {
                currentDepth++;
            }
        }
        resizeModelRows(currentRowIndex + m_frozenRowsBottomCount + 1);
        restoreSelectState();
    }
}

/*!
\internal
*/
void GridModelController::fillRecords()
{
    Q_ASSERT(!isDataUpdating());

    m_changes |= ControllerSortingChanged;

    AutoOverrideCursor autoCursor(Qt::WaitCursor, showWaitCursor());

    int rowCount = m_model != 0 ? m_model->rowCount(m_parentIndex) : 0;
    m_recordsMap.resize(rowCount);
    m_pureRowCount = rowCount;

    m_frozenRows.clear();
    m_frozenRowsTopCount = 0;
    m_frozenRowsBottomCount = 0;

    int filteredRowCount = 0;
    FilterProgressContext* filterProgress = Q_NULL;
    if (filter()->isActive())
    {
        bool cancel = false;
        emit aboutToBeFiltered(cancel);
        if (!cancel)
            filterProgress = new FilterProgressContext(this);
    }

    int* mapPtr = m_recordsMap.data();
    for (int i = 0; i < rowCount; ++i)
    {
        if (m_addingModelRowIndex == -1 || m_addingModelRowIndex != i)
        {
            if (relationFilter()->isActive() && !relationFilter()->validateRow(i))
            {
                m_pureRowCount--;
                continue;
            }

            if (filterProgress != Q_NULL)
            {
                filterProgress->nextValue();
                if (!filter()->validateRow(i))
                    continue;
            }

            if (findFilter()->isActive() && !findFilter()->validateRow(i))
                continue;

            if (frozenRowsFilter()->isActive() && frozenRowsFilter()->validateRow(i))
            {
                GridFilterCondition::GroupOperator op;
                GridFilterFrozenValueCondition* condition = static_cast<GridFilterFrozenValueCondition *>(frozenRowsFilter()->condition()->getCondition(0, op));
                addFrozenModelRow(i, condition->getFrozenPosition(i));
            }
        }

        mapPtr[filteredRowCount] = i;
        filteredRowCount++;
    }

    m_recordsMap.resize(filteredRowCount);

    if (filterProgress != Q_NULL)
    {
        Q_DELETE_AND_NULL(filterProgress);
        emit filterCompleted();
    }

    if ((m_frozenRows.size() > 0 || m_sortRules.count() > 0) && m_recordsMap.size() > 0)
    {
        bool cancel = false;
        emit aboutToBeSorted(cancel);
        if (!cancel)
        {
            {
                SortProgressContext progressContext(this);
                quick_sort<GridModelController, int>(this, m_recordsMap, 0, m_recordsMap.size() - 1, compareModelRows, &progressContext);
            }
            emit sortCompleted();
        }
    }
}

/*!
\internal
*/
void GridModelController::modelDestroyed()
{
    m_model = Q_NULL;
    beginUpdate();
    m_viewInterface->modelDestroyed();
    endUpdate();
}

void GridModelController::setModel(QAbstractItemModel* model, QModelIndex parent)
{
    if (m_model == model && parent == m_parentIndex)
        return;

    if (m_model != Q_NULL)
    {
        QObject::disconnect(m_model, SIGNAL(destroyed()), this, SLOT(modelDestroyed())); 
        QObject::disconnect(m_model, SIGNAL(columnsInserted(const QModelIndex&, int, int)), 
            this, SLOT(modelColumnsInserted(const QModelIndex&, int, int)));
        QObject::disconnect(m_model, SIGNAL(columnsMoved(const QModelIndex&, int, int, const QModelIndex&, int)), 
            this, SLOT(modelColumnsMoved(const QModelIndex&, int, int, const QModelIndex&, int)));
        QObject::disconnect(m_model, SIGNAL(columnsRemoved(const QModelIndex&, int, int)),
            this, SLOT(modelColumnsRemoved(const QModelIndex&, int, int)));
        QObject::disconnect(m_model, SIGNAL(layoutChanged()), this, SLOT(modelLayoutChanged()));
        QObject::disconnect(m_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), 
        this, SLOT(modelDataChanged(const QModelIndex&, const QModelIndex&)));
        QObject::disconnect(m_model, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)), 
            this, SLOT(modelRowsAboutToBeInserted(const QModelIndex&, int, int)));
        QObject::disconnect(m_model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), 
            this, SLOT(modelRowsAboutToBeRemoved(const QModelIndex&, int, int)));
        QObject::disconnect(m_model, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            this, SLOT(modelRowsInserted(const QModelIndex&, int, int)));
        QObject::disconnect(m_model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
            this, SLOT(modelRowsRemoved(const QModelIndex&, int, int)));
        QObject::disconnect(m_model, SIGNAL(modelAboutToBeReset()),
            this, SLOT(modelAboutToBeReset()));
        QObject::disconnect(m_model, SIGNAL(modelReset()),
            this, SLOT(modelReset()));

        submitData();
    }

    for (QSet<GridPersistentRow *>::const_iterator it = m_persistRows.constBegin(); it != m_persistRows.constEnd(); ++it)
        (*it)->clear(); //Old rows is not valid anymore.

    m_currentSelectionRanges.clear();
    m_selectionRanges.clear();
    m_selection->clear();
    clearModelGroups();
    clearFilters(true);

    m_model = model;
    
    if (m_model != Q_NULL)
    {
        QObject::connect(m_model, SIGNAL(destroyed()), this, SLOT(modelDestroyed())); 
        QObject::connect(m_model, SIGNAL(columnsInserted(const QModelIndex&, int, int)), 
            this, SLOT(modelColumnsInserted(const QModelIndex&, int, int)));
        QObject::connect(m_model, SIGNAL(columnsMoved(const QModelIndex&, int, int, const QModelIndex&, int)), 
            this, SLOT(modelColumnsMoved(const QModelIndex&, int, int, const QModelIndex&, int)));
        QObject::connect(m_model, SIGNAL(columnsRemoved(const QModelIndex&, int, int)),
            this, SLOT(modelColumnsRemoved(const QModelIndex&, int, int)));
        QObject::connect(m_model, SIGNAL(layoutChanged()), this, SLOT(modelLayoutChanged()));
        QObject::connect(m_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), 
            this, SLOT(modelDataChanged(const QModelIndex&, const QModelIndex&)));
        QObject::connect(m_model, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)), 
            this, SLOT(modelRowsAboutToBeInserted(const QModelIndex&, int, int)));
        QObject::connect(m_model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), 
            this, SLOT(modelRowsAboutToBeRemoved(const QModelIndex&, int, int)));
        QObject::connect(m_model, SIGNAL(rowsInserted(const QModelIndex&, int, int)), 
            this, SLOT(modelRowsInserted(const QModelIndex&, int, int)));
        QObject::connect(m_model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), 
            this, SLOT(modelRowsRemoved(const QModelIndex&, int, int)));
        QObject::connect(m_model, SIGNAL(modelAboutToBeReset()),
            this, SLOT(modelAboutToBeReset()));
        QObject::connect(m_model, SIGNAL(modelReset()),
            this, SLOT(modelReset()));
    }

    m_parentIndex = parent;
    refresh();
}

QAbstractItemModel* GridModelController::model() const
{
    return m_model;
}

void GridModelController::setSelectionModel(QItemSelectionModel* selectionModel)
{
    Q_ASSERT(false);
    m_selectionModel = selectionModel;
}

QItemSelectionModel* GridModelController::selectionModel() const
{
    return m_selectionModel;
}

const QModelIndex& GridModelController::parentIndex() const
{
    return m_parentIndex;
}

void GridModelController::setSortOrder(GridModelDataBinding* dataBinding, int sortIndex, SortOrder order, bool caseSensitivity, bool localeAware)
{
    m_sortRules.setSortRule(dataBinding, sortIndex, order, caseSensitivity, localeAware);
    GridSortColumnRule* rule = (GridSortColumnRule *)m_sortRules.groupRuleByColumn(dataBinding);
    if (rule != Q_NULL)
    {
        if (order == SortNone)
            order = SortAscending;
        rule->order = order;
    }
    m_recordSortingDirty = true;
    applyChanges();
}

SortOrder GridModelController::getSortOrder(GridModelDataBinding* dataBinding) const
{
    const GridSortColumnRule* rule = m_sortRules.groupRuleByColumn(dataBinding);
    if (rule == Q_NULL)
        rule = m_sortRules.sortRuleByColumn(dataBinding);
    if (rule != Q_NULL)
        return rule->order;
    else
        return SortNone;
}

void GridModelController::setGroupOrder(GridModelDataBinding* dataBinding, int groupIndex, SortOrder order, bool caseSensitivity, bool localeAware)
{
    m_sortRules.setGroupRule(dataBinding, groupIndex, order, caseSensitivity, localeAware);
    m_recordSortingDirty = true;
    applyChanges();
}

SortOrder GridModelController::getGroupOrder(GridModelDataBinding* dataBinding) const
{
    const GridSortColumnRule* rule = m_sortRules.groupRuleByColumn(dataBinding);
    if (rule != Q_NULL)
        return rule->order;
    else
        return SortNone;
}

void GridModelController::clearSorting()
{
    m_sortRules.clearSorting();
    m_recordSortingDirty = true;
    applyChanges();
}

void GridModelController::clearGrouping()
{
    m_sortRules.clearGrouping();
    m_recordSortingDirty = true;
    applyChanges();
}

void GridModelController::refresh()
{
    ModelControllerAutoUpdater updater(this);

    m_recordSortingDirty = true;
    m_summary->markChanged();
}

GridSummary* GridModelController::summary() const
{
    return m_summary;
}

GridFilter* GridModelController::filter() const
{
    return m_filter;
}

GridFilter* GridModelController::relationFilter() const
{
    return m_relationFilter;
}

GridFilter* GridModelController::frozenRowsFilter() const
{
    return m_frozenRowsFilter;
}

GridFilter* GridModelController::findFilter() const
{
    return m_findFilter;
}

bool GridModelController::hasGrouping() const
{
    return m_groups.size() > 0;
}

int GridModelController::groupingDepth() const
{
    return m_sortRules.groupRulesCount();
}

void GridModelController::clearModelGroups()
{
    m_groups.resize(0);
}

void GridModelController::saveGroupingState()
{
    clearGroupingState();

    m_groupingSavedState.reserve(m_groups.size());
    for (ModelGroupList::const_iterator it = m_groups.constBegin(); it != m_groups.constEnd(); ++it)
    {
        const GridModelGroup& group = *it;
        if (group.expanded)
        {
            RecordMapKey key(group.groupValue, group.dataBinding->column());
            m_groupingSavedState.insert(key, true);
        }
        else
        {
            while (it + 1 != m_groups.end())
            {
                const GridModelGroup& next = *(it + 1);
                if (next.depth > group.depth)
                {
                    it++;
                    continue;
                }
                break;
            }
        }
    }

    m_groupingSavedState.squeeze();
}

int GridModelController::getRowIndexByPersistentRow(const GridPersistentRow* persistentRow)
{
    Q_ASSERT(persistentRow != Q_NULL);
    int rowIndex = do_find_modelRow(this, persistentRow->record(), GridRow::compareByRecord);
    if (rowIndex == -1)
        return -1;

    if (persistentRow->groupModelColumn() == -1)
    {
        GridRow* modelRow = getGridRow(rowIndex);
        if (modelRow->type() == GridRow::DataRow)
            return rowIndex;
        else
            return -1; //Row hidden in the collapsed group.
    }

    while (rowIndex >= 0)
    {
        GridRow* modelRow = getGridRow(rowIndex);
        if (modelRow->record() != persistentRow->record())
        {
            rowIndex = -1;
            break;
        }

        if (modelRow->type() == GridRow::GroupRow && modelRow->modelGroup()->dataBinding->column() == persistentRow->groupModelColumn())
            break;

        rowIndex--;
    }
    return rowIndex;
}

int GridModelController::getPureRowCount() const
{
    return m_pureRowCount;
}

void GridModelController::restoreSelectState()
{
    //do nothing
}

void GridModelController::clearGroupingState()
{
    m_groupingSavedState.clear();
}

void GridModelController::createModelGroups()
{    
    Q_ASSERT(!isDataUpdating());

    AutoOverrideCursor autoCursor(Qt::WaitCursor, showWaitCursor());

    saveGroupingState();
    clearModelGroups();

    QMultiHash<int, GridPersistentRow *> persistRowsHash;
    persistRowsHash.reserve(m_persistRows.size());
    for (QSet<GridPersistentRow *>::const_iterator it = m_persistRows.constBegin(); it != m_persistRows.constEnd(); ++it)
    {
        // Reset persistent record and save it to hash
        (*it)->setRecord(-1);

        //Check grouping persistent rows
        if ((*it)->groupModelColumn() != -1)
        {
            bool exists = false;
            for (int i = 0; i < m_sortRules.groupRulesCount(); ++i)
            {
                const GridSortColumnRule* rule = m_sortRules.groupRule(i);
                if (rule->dataBinding->column() == (*it)->groupModelColumn())
                {
                    exists = true;
                    break;
                }
            }

            if (!exists)
            {
                (*it)->clear();
                continue;
            }
        }
        persistRowsHash.insert((*it)->modelRowIndex(), *it);
    }
    
    bool restoreState = m_groupingSavedState.size() > 0;

    int* mapPtr = m_recordsMap.data();
    int rowCount = m_recordsMap.size();

    QStack<int> groupStack;

    for (int i = 1; i <= rowCount; ++i)
    {
        // restore current persistent record
        QMultiHash<int, GridPersistentRow *>::const_iterator it = persistRowsHash.constFind(mapPtr[i - 1]);
        while (it != persistRowsHash.constEnd() && it.key() == mapPtr[i - 1])
        {
            it.value()->setRecord(i - 1);
            it++;
        }

        if (i <= m_frozenRowsTopCount || i > rowCount - m_frozenRowsBottomCount)
            continue;

        int depth = -1;
        for (int j = 0; j < m_sortRules.groupRulesCount(); ++j)
        {
            const GridSortColumnRule* rule = m_sortRules.groupRule(j);
            QVariant value1;
            rule->dataBinding->getValue(mapPtr[i - 1], rule->dataBinding->displayRole(), value1);

            if (groupStack.size() < j + 1)
            {
                /////////////////////
                bool expandFlag = false;
                if (restoreState)
                {
                    GroupingSavedState::const_iterator data_it = 
                        m_groupingSavedState.constFind(RecordMapKey(value1, rule->dataBinding->column()));
                    expandFlag = data_it != m_groupingSavedState.constEnd();
                }
                /////////////////////

                GridModelGroup* group = appendModelGroup();
                group->dataBinding = rule->dataBinding;
                group->depth = j;
                group->last = false;
                group->parentIndex = groupStack.size() == 0 ? -1 : groupStack.last();
                group->firstRecord = i - 1;
                group->lastRecord = -1;
                group->expanded = expandFlag;
                group->groupValue = value1;
                group->rowIndex = 0xFFFFFFFF;
                groupStack.push(m_groups.size() - 1);
            }

            if (i == rowCount)
                continue;

            QVariant value2;

            rule->dataBinding->getValue(mapPtr[i], rule->dataBinding->displayRole(), value2);

            if (depth == -1 && value1 != value2)
                depth = j;
        }

        //if (groupStack.size() > 0 && !m_hiddenRowsHash.contains(mapPtr[i - 1]))
        //{
        //    GridModelGroup* group = &m_groups[groupStack.last()];
        //    while (group->hidden == true)
        //    {
        //        group->hidden = false;
        //        if (group->parentIndex == -1)
        //            break;
        //        group = &m_groups[group->parentIndex];
        //    }
        //}

        if (i == rowCount)
            depth = 0;

        if (depth == -1)
            continue;

        while (groupStack.size() > depth)
        {
            GridModelGroup& group = m_groups[groupStack.pop()];
            group.lastRecord = i - 1;
            group.last = groupStack.size() > depth || groupStack.size() == 0;
            
            //Open group for focused row
            if (group.firstRecord <= m_focusedRowIndex->record() &&
                m_focusedRowIndex->record() <= group.lastRecord &&
                group.dataBinding->column() != m_focusedRowIndex->groupModelColumn())
            {
                group.expanded = true;
            }
        }
    }

    clearGroupingState();

    m_changes |= ControllerGroupingChanged;
}

int GridModelController::modelGroupCount() const
{
    return m_groups.size();
}

GridModelGroup* GridModelController::modelGroup(int index)
{
    return &m_groups[index];
}

QVector<GridModelGroup>& GridModelController::groups()
{
    return m_groups;
}

GridModelGroup* GridModelController::appendModelGroup()
{
    if (m_groups.capacity() <= m_groups.size() + 1)
        m_groups.reserve(m_groups.capacity() * 2);

    GridModelGroup g;
    g.index = m_groups.size();
    m_groups.push_back(g);
    return &m_groups.last();
}

/*!
    \class GridRow
    \inmodule QtitanDataGrid
    \brief GridRow class introduce a row to display in a grid view. The row can be of two types - data row and a row that represents the group - group row. 
    See GridRow::type() for more details. Number of rows does not always match the number of rows that are available in the Qt model.
    For example, if the filter has been imposed or grid has grouping then the number of grid rows will be different than Qt model rows.
    \image grid_rows_scheme.png
*/
GridRow::GridRow()
    : m_controller(Q_NULL), m_type(DataRow), m_rowIndex(-1), m_group(Q_NULL), m_initialized(false), m_previewExpanded(false)
{
}

/*!
Returns the type of the row. It can be DataRow or GroupRow.
*/
GridRow::RowType GridRow::type() const
{
    Q_ASSERT(m_controller != Q_NULL);
    return m_type;
}

/*!
Returns index number for row.
*/
int GridRow::rowIndex() const
{
    Q_ASSERT(m_controller != Q_NULL);
    return m_rowIndex;
}

/*!
Returns depth for GroupRow. For DataRow this function does not has effect and always returns -1.
*/
int GridRow::depth() const
{
    Q_ASSERT(m_controller != Q_NULL);
    if (m_group != Q_NULL)
        return m_group->depth;
    else
        return -1;
}

/*!
Returns expanded state for row if row is a GroupRow. For DataRow this function does not has effect and always returns true.
*/
bool GridRow::isExpanded() const
{
    Q_ASSERT(m_controller != Q_NULL);
    if (m_group != Q_NULL)
        return m_group->expanded;
    else
        return true;
}

/*!
\internal
*/
bool GridRow::last() const
{
    Q_ASSERT(m_controller != Q_NULL);
    if (m_group == Q_NULL)
        return false;

    if (type() == GridRow::DataRow)
    {
        return m_rowIndex == m_group->rowIndex + m_group->lastRecord - m_group->firstRecord + 1;
    }
    else
    {
        Q_ASSERT(type() == GridRow::GroupRow);
        return m_group->last;
    }
}

/*!
\internal
*/
GridRow* GridRow::parentGroupRow() const
{
    Q_ASSERT(m_controller != Q_NULL);
    if (m_group == Q_NULL || m_group->parentIndex == -1)
        return Q_NULL;
    return m_controller->getGridRow(m_controller->modelGroup(m_group->parentIndex)->rowIndex);
}

GridRow* GridRow::groupRow() const
{
    Q_ASSERT(m_controller != Q_NULL);
    if (m_group == Q_NULL || m_group->index == -1)
        return Q_NULL;
    return m_controller->getGridRow(m_controller->modelGroup(m_group->index)->rowIndex);
}

/*!
\internal
*/
GridModelGroup* GridRow::modelGroup() const
{ 
    return m_group;
}

/*!
Expands or collapses the row if row is a GroupRow. For DataRow functions does nothing.
*/
void GridRow::setExpanded(bool expanded)
{
    Q_ASSERT(m_controller != Q_NULL);
    if (m_group != Q_NULL && m_group->expanded != expanded)
        expanded ? m_controller->expandGroup(m_group) : m_controller->collapseGroup(m_group);
}

bool GridRow::isPreviewExpanded() const
{
    return m_previewExpanded;
}

void GridRow::setPreviewExpanded(bool expanded)
{
    Q_ASSERT(m_controller != Q_NULL);
    if (m_previewExpanded == expanded)
        return;
    m_previewExpanded = expanded;
    m_controller->modelLayoutChanged();
}

/*!
If the row is a GroupRow returns model column index by which the group is formed. If row is a DataRow the result is -1.
*/
int GridRow::groupColumn() const
{
    Q_ASSERT(m_controller != Q_NULL);
    return m_group != Q_NULL ? m_group->dataBinding->column() : -1;
}

/*!
Returns the frozen state for the row. 
*/
bool GridRow::isFrozen() const
{
    Q_ASSERT(m_controller != Q_NULL);
    return m_controller->m_frozenRows.contains(modelRowIndex());
}

/*!
Sets the frozen state for the row. if the value is true then the row will be freeze at the GridModelController::frozenPlace() place. 
Please note, function does not has effect if GridRow::type() is not equal a GridRow::DataRow.
*/
void GridRow::setFrozen(bool b)
{
    if (isFrozen() == b)
        return;
    if (type() == DataRow)
    {
        if (b)
            m_controller->addModelRowToFrozenFilter(modelRowIndex(), m_controller->frozenPlace());
        else
            m_controller->removeModelRowFromFrozenFilter(modelRowIndex());
    }
}

/*!
Returns the frozen place for the row. 
*/
GridRow::FrozenPlace GridRow::frozenPlace() const
{
    Q_ASSERT(m_controller != Q_NULL);
    QHash<int, bool>::const_iterator it = m_controller->m_frozenRows.constFind(modelRowIndex());
    if (it == m_controller->m_frozenRows.constEnd())
        return FrozenNone;
    else if (it.value())
        return FrozenTop;
    else
        return FrozenBottom;
}

/*!
Creates and returns GridCell structure for current row and cell \a index.
*/
GridCell GridRow::cell(int index) const
{
    Q_ASSERT(m_controller != Q_NULL);
    return GridCell((GridRow *)this, index);
}

/*!
\internal
*/
bool GridRow::groupValue(QVariant& retval) const
{
    Q_ASSERT(m_controller != Q_NULL);
    if (m_group == Q_NULL)
    {
        retval = QVariant();
        return false;
    }
    return m_group->dataBinding->getValue(m_controller->getModelRowIndexByRecord(m_group->firstRecord), Qt::DisplayRole, retval);
}

/*!
Returns the controller owner for this row.
*/
GridModelController* GridRow::controller() const
{
    return m_controller;
}

/*!
\internal
*/
void GridRow::initialize(GridModelController* controller, RowType type, GridModelGroup* group, int rowIndex)
{
    m_controller = controller;
    m_type = type;
    m_group = group;
    m_rowIndex = rowIndex;
    m_initialized = true;
    m_scrollPosition = -1;
    m_previewExpanded = false;
}

/*!
\internal
*/
void GridRow::reset()
{
    m_initialized = false;
}

/*!
\internal
*/
bool GridRow::isInitialized() const
{
    return m_initialized;
}

/*!
\internal
*/
int GridRow::record() const
{
    Q_ASSERT(m_controller != Q_NULL);
    int ret = -1;
    if (m_group == Q_NULL)
    {
        if (m_controller->m_frozenRowsBottomCount == 0)
            ret = m_rowIndex;
        else
        {
            //Calculate record index for frozen row at bottom.
            if (m_rowIndex >= m_controller->m_gridRows.size() - m_controller->m_frozenRowsBottomCount)
                ret = m_controller->m_recordsMap.size() - (m_controller->m_gridRows.size() - m_rowIndex);
            else
                ret = m_rowIndex;
        }
    }
    else if (m_group->rowIndex == m_rowIndex)
        ret = m_group->firstRecord;
    else
        ret = m_group->firstRecord + m_rowIndex - m_group->rowIndex - 1;
    return ret;
}

/*!
Returns row index related to the model. It should be used to contstruct QModelIndex.
*/
int GridRow::modelRowIndex() const
{
    int r = record();
    if (r == -1)
        return -1;
    return m_controller->getModelRowIndexByRecord(r);
}

/* GridCell */ 
GridCell::GridCell()
: m_row(Q_NULL), m_columnIndex(-1)
{
}

GridCell::GridCell(GridModelController* controller, int rowIndex, int columnIndex)
: m_row(controller->getGridRow(rowIndex)), m_columnIndex(columnIndex)
{
}

GridCell::GridCell(GridRow* row, int columnIndex)
: m_row(row), m_columnIndex(columnIndex)
{
}

GridRow* GridCell::row() const
{ 
    return m_row;
}

int GridCell::rowIndex() const
{
    if (m_row == Q_NULL)
        return -1;
    return m_row->rowIndex();
}

int GridCell::columnIndex() const
{ 
    return m_columnIndex;
}

QModelIndex GridCell::modelIndex() const
{
    if (m_row == Q_NULL || m_row->controller() == 0)
        return QModelIndex();
    GridModelDataBinding* binding = m_row->controller()->m_viewInterface->dataBindingByColumnIndex(m_columnIndex);
    Q_ASSERT(binding != Q_NULL);
    if (binding == Q_NULL)
        return QModelIndex();
    return m_row->controller()->model()->index(m_row->modelRowIndex(), binding->column(), m_row->controller()->parentIndex());
}

/* GridCellSelectionRange */
GridCellSelectionRange::GridCellSelectionRange(GridModelController* controller, const GridCell& topLeft, const GridCell& bottomRight)
: m_controller(controller), m_persistTopLeft(controller), m_persistBottomRight(controller)
{
    GridRow* topRow = topLeft.row()->rowIndex() < bottomRight.row()->rowIndex() ? topLeft.row() : bottomRight.row();
    GridRow* bottomRow = topLeft.row()->rowIndex() < bottomRight.row()->rowIndex() ? bottomRight.row() : topLeft.row();
    int leftIndex = topLeft.columnIndex() < bottomRight.columnIndex() ? topLeft.columnIndex() : bottomRight.columnIndex();
    int rightIndex = topLeft.columnIndex() < bottomRight.columnIndex() ? bottomRight.columnIndex() : topLeft.columnIndex();

    m_persistTopLeft.set(topRow);
    m_persistTopLeft.setColumnIndex(leftIndex);
    m_persistBottomRight.set(bottomRow);
    m_persistBottomRight.setColumnIndex(rightIndex);
}

GridCell GridCellSelectionRange::topLeft() const
{
    return GridCell(m_persistTopLeft.row(), m_persistTopLeft.columnIndex());
}

GridCell GridCellSelectionRange::bottomRight() const
{
    return GridCell(m_persistBottomRight.row(), m_persistBottomRight.columnIndex());
}

bool GridCellSelectionRange::contains(const GridCell& cell) const
{
    GridCell tl = topLeft();
    GridCell br = bottomRight();
    return tl.rowIndex() <= cell.rowIndex() && tl.columnIndex() <= cell.columnIndex()
                && br.rowIndex() >= cell.rowIndex() && br.columnIndex() >= cell.columnIndex();
}

bool GridCellSelectionRange::contains(const GridRow* row) const
{
    return topLeft().rowIndex() <= row->rowIndex() && bottomRight().rowIndex() >= row->rowIndex();
}

bool GridCellSelectionRange::isValid() const
{
    GridCell tl = topLeft();
    GridCell br = bottomRight();

    return tl.rowIndex() != -1 &&
        tl.rowIndex() <= br.rowIndex() &&
        tl.columnIndex() != -1 &&
        tl.columnIndex() <= br.columnIndex();
}

bool GridCellSelectionRange::intersects(const GridCellSelectionRange& range) const
{
    return (isValid() && range.isValid() &&
        ((top() <= range.top() && bottom() >= range.top())
                || (top() >= range.top() && top() <= range.bottom()))
            && ((left() <= range.left() && right() >= range.left())
                || (left() >= range.left() && left() <= range.right())));
}

bool GridCellSelectionRange::contains(const GridCellSelectionRange& range) const
{
    return (isValid() && range.isValid() &&
        ((top() <= range.top() && bottom() >= range.bottom()))
         && 
        ((left() <= range.left() && right() >= range.right())));
}

GridCellSelectionRange GridCellSelectionRange::unite(const GridCellSelectionRange& range) const
{
 
    GridCell first = GridCell(m_controller, qMin(top(), range.top()), qMin(left(), range.left()));
    GridCell second = GridCell(m_controller, qMax(bottom(), range.bottom()), qMax(right(), range.right()));
    return GridCellSelectionRange(m_persistTopLeft.m_controller , first, second);
}

GridCellSelectionRange GridCellSelectionRange::intersected(const GridCellSelectionRange& range) const
{
 
    GridCell first = GridCell(m_controller, qMax(top(), range.top()), qMax(left(), range.left()));
    GridCell second = GridCell(m_controller, qMin(bottom(), range.bottom()), qMin(right(), range.right()));
    return GridCellSelectionRange(m_persistTopLeft.m_controller , first, second);
}

/*!
  \class GridSummarySetting
  \inmodule QtitanDataGrid
  \internal
*/
GridSummarySetting::GridSummarySetting(GridModelDataBinding* dataBinding, Qt::ItemDataRole role,  GridSummaryType type)
: m_dataBinding(dataBinding), m_role(role), m_type(type)
{
}

GridModelDataBinding* GridSummarySetting::dataBinding() const
{
    return m_dataBinding;
}

 GridSummaryType GridSummarySetting::type() const
{
    return m_type;
}

Qt::ItemDataRole GridSummarySetting::role() const
{
    return m_role;
}

/*!
  \class GridSummaryGroupSetting
  \inmodule QtitanDataGrid
  \internal
*/
GridSummaryGroupSetting::GridSummaryGroupSetting(GridModelDataBinding* groupDataBinding,
                                                 GridModelDataBinding* dataBinding, Qt::ItemDataRole role,
                                                 GridSummaryType type, ShowingPlace place)
: GridSummarySetting(dataBinding, role, type),m_groupDataBinding(groupDataBinding), m_showingPlace(place)
{
}

GridModelDataBinding* GridSummaryGroupSetting::groupDataBinding() const
{
    return m_groupDataBinding;
}

bool GridSummaryGroupSetting::sorted() const
{
    return m_sorted;
}

void GridSummaryGroupSetting::setSorted(bool sorted)
{
    m_sorted = sorted;
}

GridSummaryGroupSetting::ShowingPlace GridSummaryGroupSetting::showingPlace() const
{
    return m_showingPlace;
}

void GridSummaryGroupSetting::setShowingPlace(ShowingPlace place)
{
    m_showingPlace = place;
}

QTITAN_BEGIN_NAMESPACE
class GridSummaryCalculator
{
public:
    GridSummaryCalculator(GridSummary* summary, SummaryProgressContext* progressContext);
    void start(GridModelDataBinding* dataBinding, Qt::ItemDataRole role, GridSummaryType type);
    GridSummaryValue stop(GridModelDataBinding* dataBinding);
    void calculate(int firstRecord, int secondRecord);
    void clear();
private:
    void do_calculate(int firstRecord, int secondRecord, GridModelDataBinding* dataBinding, GridSummaryValue& values);
private:
    void startCalculation(ColumnSummaryValues& hash, GridModelDataBinding* dataBinding, Qt::ItemDataRole role, GridSummaryType type);
    GridSummaryValue stopCalculation(ColumnSummaryValues& hash, GridModelDataBinding* dataBinding);
    ColumnSummaryValues m_columnValuesHash;
    GridSummary* m_summary;
    GridModelController* m_controller;
    SummaryProgressContext* m_progressContext;
};
QTITAN_END_NAMESPACE

/*!
  \class GridSummary
  \inmodule QtitanDataGrid
*/
GridSummary::GridSummary(GridModelController* controller)
: QObject(Q_NULL), m_controller(controller), m_changed(false)
{
}

GridModelController* GridSummary::controller() const
{
    return m_controller;
}

void GridSummary::setFooterSetting(GridModelDataBinding* dataBinding, Qt::ItemDataRole role,  GridSummaryType type)
{
    ModelControllerAutoUpdater updater(m_controller);

    if (type == SummaryNone)
    {
        //Remove the summary setting
        QHash<GridModelDataBinding *, GridSummarySetting>::iterator it = m_footerSummaryMap.find(dataBinding);
        if (it != m_footerSummaryMap.end())
        {
            m_footerSummaryMap.erase(it);
            markChanged();
        }
    }
    else
    {
        m_footerSummaryMap.insert(dataBinding, GridSummarySetting(dataBinding, role, type));
        markChanged();
    }
}

void GridSummary::setDefaultGroupSetting(GridModelDataBinding* dataBinding, Qt::ItemDataRole role, GridSummaryType type)
{
    setGroupSetting(Q_NULL, dataBinding, role, type);
}

void GridSummary::setGroupSetting(GridModelDataBinding* groupDataBinding, GridModelDataBinding* dataBinding, Qt::ItemDataRole role,  GridSummaryType type)
{
    ModelControllerAutoUpdater updater(m_controller);

    QHash<GridModelDataBinding *, QHash<GridModelDataBinding *, GridSummaryGroupSetting> >::iterator
        groupit = m_groupSummaryMap.find(groupDataBinding);

    if (groupDataBinding == Q_NULL && type == SummaryNone)
    {
        //Remove the summary setting for default group only (== Q_NULL)
        if (groupit != m_groupSummaryMap.end())
        {
            QHash<GridModelDataBinding *, GridSummaryGroupSetting>::iterator it = groupit.value().find(dataBinding);
            if (it != groupit.value().end())
            {
                groupit.value().erase(it);
                if (groupit.value().size() == 0)
                    m_groupSummaryMap.erase(groupit);
                markChanged();
            }
        }
    }
    else
    {
        if (groupit != m_groupSummaryMap.end())
            groupit->insert(dataBinding, GridSummaryGroupSetting(groupDataBinding, dataBinding, role, type));
        else
        {
            QHash<GridModelDataBinding *, GridSummaryGroupSetting> map;
            map.insert(dataBinding, GridSummaryGroupSetting(groupDataBinding, dataBinding, role, type));
            m_groupSummaryMap.insert(groupDataBinding, map);
        }
        markChanged();
    }
}

const GridSummarySetting& GridSummary::footerSetting(int index) const
{
    return *(m_footerSummaryMap.find(m_footerSummaryMap.uniqueKeys()[index]));
}

int GridSummary::footerSettingsCount() const
{
    return m_footerSummaryMap.size();
}

bool GridSummary::footerSettingsExists(GridModelDataBinding* dataBinding) const
{
    return m_footerSummaryMap.contains(dataBinding);
}

const GridSummaryGroupSetting& GridSummary::defaultGroupSetting(int index) const
{
    return groupSetting(Q_NULL, index);
}

int GridSummary::defaultGroupSettingsCount() const
{
    return groupSettingsCount(Q_NULL);
}

const GridSummaryGroupSetting& GridSummary::groupSetting(GridModelDataBinding* groupDataBinding, int index) const
{
    QHash<GridModelDataBinding *, QHash<GridModelDataBinding *, GridSummaryGroupSetting> >::const_iterator groupit = m_groupSummaryMap.constFind(groupDataBinding);
    Q_ASSERT(groupit != m_groupSummaryMap.end());
    return *(groupit->constFind(groupit->uniqueKeys()[index]));
}

int GridSummary::groupSettingsCount(GridModelDataBinding* groupDataBinding) const
{
    QHash<GridModelDataBinding *, QHash<GridModelDataBinding *, GridSummaryGroupSetting> >::const_iterator 
        groupit = m_groupSummaryMap.constFind(groupDataBinding);
    if (groupit == m_groupSummaryMap.constEnd())
        return 0;
    else
        return groupit->size();
}

void GridSummary::getGroupSummarySettings(GridModelDataBinding* groupDataBinding, QVector<const GridSummaryGroupSetting *>& settings) const
{
    QSet<GridModelDataBinding *> usedColumns;
    for (int i = 0; i < groupSettingsCount(groupDataBinding); ++i)
    {
        const GridSummaryGroupSetting& setting = groupSetting(groupDataBinding, i);
        Q_ASSERT(usedColumns.find(setting.dataBinding()) == usedColumns.end());
        usedColumns.insert(setting.dataBinding());
        settings.push_back(&setting);
    }

    for (int i = 0; i < defaultGroupSettingsCount(); ++i)
    {
        const GridSummaryGroupSetting& setting = defaultGroupSetting(i);
        if (usedColumns.find(setting.dataBinding()) == usedColumns.end())
        {
            settings.push_back(&setting);
        }
    }

    QVector<const GridSummaryGroupSetting *> list;
    for (QVector<const GridSummaryGroupSetting *>::iterator it = settings.begin(); it != settings.end(); ++it)
        if ((*it)->type() != SummaryNone)
            list.push_back(*it);
    settings = list;
}

bool GridSummary::groupSettingsExists(GridModelDataBinding* dataBinding) const
{
    for (QHash<GridModelDataBinding *, QHash<GridModelDataBinding *, GridSummaryGroupSetting> >::const_iterator groupit = 
        m_groupSummaryMap.constBegin(); groupit != m_groupSummaryMap.constEnd(); ++groupit)
    {
        for (QHash<GridModelDataBinding *, GridSummaryGroupSetting>::const_iterator it = groupit.value().constBegin(); it != groupit.value().constEnd(); ++it)
        {
            if (it.value().dataBinding() == dataBinding)
                return true;
        }
    }
    return false;
}

bool GridSummary::do_calculate_summary(GridModelDataBinding* groupDataBinding, void* progressContext)
{
    QVector<const GridSummaryGroupSetting *> settings;
    getGroupSummarySettings(groupDataBinding, settings);
    if (settings.size() == 0)
        return false;

    GridModelController::ModelGroupList& groups = controller()->groups();

    for (int i = 0; i < groups.size(); ++i) 
    {
        const GridModelGroup& group = groups[i];
        if (group.dataBinding == groupDataBinding)
        {
            GridSummaryCalculator calculator(this, static_cast<SummaryProgressContext *>(progressContext));
            for (QVector<const GridSummaryGroupSetting *>::const_iterator setting_it = settings.constBegin(); 
                setting_it != settings.constEnd(); ++setting_it)
            {
                const GridSummaryGroupSetting* setting = *setting_it;
                calculator.start(setting->dataBinding(), setting->role(), setting->type()); 
            }

            calculator.calculate(group.firstRecord, group.lastRecord);

            for (QVector<const GridSummaryGroupSetting *>::const_iterator setting_it = settings.constBegin(); 
                setting_it != settings.constEnd(); ++setting_it)
            {
                const GridSummaryGroupSetting* setting = *setting_it;
                m_groupResults[controller()->getModelColumnCount() * group.index + setting->dataBinding()->column()] =
                    calculator.stop(setting->dataBinding());
            }
        }        
    }
    return true;
}

void GridSummary::calculate()
{
    if (!m_changed)
        return;

    m_changed = false;

    m_footerResults.clear();
    if (m_footerSummaryMap.size() > 0)
        m_footerResults.resize(m_controller->getModelColumnCount());

    m_groupResults.clear();
    if (m_groupSummaryMap.size() > 0)
        m_groupResults.resize(m_controller->getModelColumnCount() * m_controller->groups().size());

    bool cancel = false;
    emit m_controller->aboutToBeSummaryCalculated(cancel);
    if (cancel)
        return;

    AutoOverrideCursor autoCursor(Qt::WaitCursor, m_controller->showWaitCursor());

    SummaryProgressContext porgressContext(controller());

    bool flag = footerSettingsCount() > 0;
    if (flag)
    {
        GridSummaryCalculator calculator(this, &porgressContext);

        for (int i = 0; i < footerSettingsCount(); ++i)
        {
            const GridSummarySetting& setting = footerSetting(i);
            if (setting.type() == SummaryNone)
                continue;
            calculator.start(setting.dataBinding(), setting.role(), setting.type()); 
        }

        calculator.calculate(0, m_controller->m_recordsMap.size()- 1);

        for (int i = 0; i < footerSettingsCount(); ++i)
        {
            const GridSummarySetting& setting = footerSetting(i);
            if (setting.type() == SummaryNone)
                continue;
            m_footerResults[setting.dataBinding()->column()] = calculator.stop(setting.dataBinding()); 
        }
    }
     
    for (int j = 0; j < m_controller->m_sortRules.groupRulesCount(); ++j)
    {
        const GridSortColumnRule* rule = m_controller->m_sortRules.groupRule(j);
        flag |= do_calculate_summary(rule->dataBinding, &porgressContext);
    }
    emit m_controller->summaryCompleted();
}

void GridSummary::createDisplayText(const GridSummaryValue& value, bool isFooterSummary, QString& retval) const
{
    Q_UNUSED(isFooterSummary);
    switch (value.type)
    {
    case SummarySum:
        retval = QString(Grid::tr_compatible(QtnGridSumTextString) + "=%1").arg(value.value.toDouble());
        break;
    case SummaryMin:
        retval = QString(Grid::tr_compatible(QtnGridMinTextString) + "=%1").arg(value.value.toString());
        break;
    case SummaryMax:
        retval = QString(Grid::tr_compatible(QtnGridMaxTextString) + "=%1").arg(value.value.toString());
        break;
    case SummaryCount:
        retval = QString("%1").arg(value.value.toInt());
        break;
    case SummaryAvg:
        retval = QString(Grid::tr_compatible(QtnGridAverageTextString) + "=%1").arg(value.value.toDouble() / value.count);
        break;
    default:
        retval = "";
        break;
    }
}

static GridSummaryValue emptySummaryValue;

const GridSummaryValue& GridSummary::getFooterSummaryValue(GridModelDataBinding* dataBinding) const
{
    if (dataBinding->column() >= 0 && dataBinding->column() < m_footerResults.size())
        return m_footerResults[dataBinding->column()];
    else
        return emptySummaryValue;
}

QString GridSummary::getFooterSummaryDisplayText(GridModelDataBinding* dataBinding) const
{
    const GridSummaryValue& value = getFooterSummaryValue(dataBinding);
    if (value.type == SummaryNone)
        return QString();

    QString retval;
    createDisplayText(value, true, retval);
    return retval;
}

const GridSummaryValue& GridSummary::getGroupSummaryValue(GridModelGroup* group, GridModelDataBinding* dataBinding) const
{
    int index = m_controller->getModelColumnCount() * group->index + dataBinding->column();
    if (index >= 0 && index < m_groupResults.size())
        return m_groupResults[index];
    else
        return emptySummaryValue;
}

QString GridSummary::getGroupSummaryDisplayText(GridModelGroup* group, GridModelDataBinding* dataBinding) const
{
    const GridSummaryValue& value = getGroupSummaryValue(group, dataBinding);
    if (value.type == SummaryNone)
        return QString();
    QString retval;
    createDisplayText(value, false, retval);
    return retval;
}

bool GridSummary::supportSummaryType(GridModelDataBinding* dataBinding, GridSummaryType type) const
{
    Q_UNUSED(dataBinding);
    Q_UNUSED(type);
    return true;
}

void GridSummary::markChanged(int modelColumn)
{
    if (modelColumn == -1)
    {
        m_changed = true;
    }
    else
    {
        for (int i = 0; i < footerSettingsCount(); ++i)
        {
            const GridSummarySetting& setting = footerSetting(i);
            if (setting.dataBinding()->column() == modelColumn)
            {
                m_changed = true;
                return;
            }
        }

        for (QHash<GridModelDataBinding *, QHash<GridModelDataBinding *, GridSummaryGroupSetting> >::const_iterator it = m_groupSummaryMap.constBegin();
            it != m_groupSummaryMap.constEnd(); ++it)
        {
            if (it.key() != Q_NULL && it.key()->column() == modelColumn)
            {
                m_changed = true;
                return;
            }

            for (QHash<GridModelDataBinding *, GridSummaryGroupSetting>::const_iterator jt = it.value().constBegin(); jt != it.value().constEnd(); ++jt)
            {
                if (jt.key() != Q_NULL && jt.key()->column() == modelColumn)
                {
                    m_changed = true;
                    return;
                }
            }
        }
    }
}

void GridSummary::removeForDataBinding(GridModelDataBinding* dataBinding)
{
    markChanged();

    QHash<GridModelDataBinding *, GridSummarySetting>::iterator it = m_footerSummaryMap.find(dataBinding);
    if (it != m_footerSummaryMap.end())
        m_footerSummaryMap.erase(it);

    for (QHash<GridModelDataBinding *, QHash<GridModelDataBinding *, GridSummaryGroupSetting> >::iterator it = m_groupSummaryMap.begin();
        it != m_groupSummaryMap.end(); ++it)
    {
        if (it.key() == dataBinding)
        {
            it = m_groupSummaryMap.erase(it);
            if (it == m_groupSummaryMap.end())
                break;
        }
        QHash<GridModelDataBinding *, GridSummaryGroupSetting>::iterator it2 = it.value().find(dataBinding);
        if (it2 != it.value().end())
            it.value().erase(it2);
    }
}

/* GridSummaryCalculator */
GridSummaryCalculator::GridSummaryCalculator(GridSummary* summary, SummaryProgressContext* progressContext)
    : m_summary(summary), m_progressContext(progressContext)
{
    m_controller = m_summary->controller();
}

void GridSummaryCalculator::startCalculation(ColumnSummaryValues& map, GridModelDataBinding* dataBinding, Qt::ItemDataRole role,  GridSummaryType type)
{
    GridSummaryValue val;
    val.type = type;
    val.role = role;
    ColumnSummaryValues::iterator it = map.find(dataBinding);
    if (it == map.end())
        it = map.insert(dataBinding, val);
    else
        Q_ASSERT(false);
}

GridSummaryValue GridSummaryCalculator::stopCalculation(ColumnSummaryValues& map, GridModelDataBinding* dataBinding)
{
    ColumnSummaryValues::iterator it = map.find(dataBinding);
    Q_ASSERT(it != map.end());
    GridSummaryValue retval = *it;
    return retval;
}

void GridSummaryCalculator::start(GridModelDataBinding* dataBinding, Qt::ItemDataRole role, GridSummaryType type)
{
    startCalculation(m_columnValuesHash, dataBinding, role, type);
}

GridSummaryValue GridSummaryCalculator::stop(GridModelDataBinding* dataBinding)
{
    return stopCalculation(m_columnValuesHash, dataBinding);
}

void GridSummaryCalculator::calculate(int firstRecord, int secondRecord)
{
    for (ColumnSummaryValues::iterator it = m_columnValuesHash.begin(); 
        it != m_columnValuesHash.end(); ++it)
    {
        GridModelDataBinding* dataBinding = it.key();
        GridSummaryValue& value = it.value();
        do_calculate(firstRecord, secondRecord, dataBinding, value);
    }
}

void GridSummaryCalculator::clear()
{
}

void GridSummaryCalculator::do_calculate(int firstRecord, int secondRecord,
                                         GridModelDataBinding* dataBinding, GridSummaryValue& summaryValue)
{
    for (int record = firstRecord; record <= secondRecord; ++record)
    {
        QVariant v;
        if (!dataBinding->getValue(m_controller->getModelRowIndexByRecord(record), summaryValue.role, v))
            continue;
        switch (summaryValue.type)
        {
        case SummarySum:
            {
                summaryValue.value = summaryValue.value.toDouble() + v.toDouble();
            }

            break;
        case SummaryMin:
            {
                if (summaryValue.value.isNull() || variant_compare(v, summaryValue.value) < 0)
                    summaryValue.value = v;
            }
            break;
        case SummaryMax:
            {
                if (summaryValue.value.isNull() || variant_compare(v, summaryValue.value) >= 0)
                    summaryValue.value = v;
            }
            break;
        case SummaryCount:
            {
                summaryValue.value = summaryValue.value.toDouble() + 1;
            }
            break;
        case SummaryAvg:
            {
                summaryValue.value = summaryValue.value.toDouble() + v.toDouble();
            }
            break;
        default:
            Q_ASSERT(false);

        };
        summaryValue.count++;

        if (m_progressContext != Q_NULL)
            m_progressContext->nextValue();
    }
}

