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

#include "QtnGridFilter.h"
#include "QtnGridModelController.h"
#include "QtnGrid.h"

QTITAN_USE_NAMESPACE

inline uint qHash(const QVariant& key)
{
    return qHash(key.toString());
}

/* GridFilterHistory */
GridFilterHistory::GridFilterHistory(GridFilter* filter)
    : m_filter(filter), m_currentIndex(-1)
{
}

GridFilterHistory::~GridFilterHistory()
{
    clear();
    m_filter = Q_NULL;
}

void GridFilterHistory::clear()
{
    while (m_conditions.size() > 0)
    {
        delete m_conditions[0];
        m_conditions.removeAt(0);
    }
    emit changed();
}

int GridFilterHistory::count() const
{
    return m_conditions.size();
}

GridFilterGroupCondition* GridFilterHistory::getCondition(int index) const
{
    return m_conditions[index];
}

void GridFilterHistory::setCondition(GridFilterGroupCondition* condition, bool update)
{
    if (update)
    {
        delete m_conditions[m_currentIndex];
        m_conditions[m_currentIndex] = condition;
        emit changed();
        return;
    }

    while (m_conditions.size() -1 > m_currentIndex)
    {
        delete m_conditions.takeLast();
    }

    m_conditions.push_back(condition);
    if (m_conditions.size() > 5)
        delete m_conditions.takeFirst();
    m_currentIndex = m_conditions.size() - 1;
    emit changed();
}

int GridFilterHistory::currentIndex()
{
    return m_currentIndex;
}

void GridFilterHistory::setCurrentIndex(int index)
{
    if (index < 0 || index >= m_conditions.size())
        return;
    m_currentIndex = index;
    m_filter->setActive(true);
    m_filter->setCondition((GridFilterGroupCondition *)m_conditions[m_currentIndex]->clone(), false);
    m_filter->m_controller->refresh();
}

/* GridFilter */
GridFilter::GridFilter(GridModelController* controller)
: m_controller(controller), m_history(Q_NULL),
  m_condition(Q_NULL), m_active(false)
{
    m_history = new GridFilterHistory(this);
}

GridFilter::~GridFilter()
{
    Q_DELETE_AND_NULL(m_condition);
    Q_DELETE_AND_NULL(m_history);
}

GridFilterHistory* GridFilter::history() const
{
    return m_history;
}

GridFilterGroupCondition* GridFilter::condition() const
{
    return m_condition;
}

void GridFilter::setCondition(GridFilterGroupCondition* groupCondition, bool addToHistory)
{
    Q_DELETE_AND_NULL(m_condition);
    m_condition = groupCondition;
    if (groupCondition != Q_NULL && addToHistory)
        m_history->setCondition((GridFilterGroupCondition *)groupCondition->clone(), false);
}

bool GridFilter::validateRow(int modelRow)
{
    if (m_condition == 0)
        return true;
    return m_condition->isTrue(modelRow);
}

QString GridFilter::getName()
{
    if (m_name.isEmpty())
        m_name = getPresentation(m_condition);
    return m_name;
}

void GridFilter::setName(const QString& name)
{
    m_name = name;
}

bool GridFilter::isActive() const
{
    return m_active;
}

void GridFilter::setActive(bool flag)
{    
    if (m_active == flag)
        return;

    m_active = flag;

    m_controller->refresh();

    if (flag)
        m_controller->m_viewInterface->raiseFilterActivated();
    else
        m_controller->m_viewInterface->raiseFilterDeactivated();
}

void GridFilter::clear()
{
    Q_DELETE_AND_NULL(m_condition);
}

QString GridFilter::getPresentation(GridFilterCondition* condition)
{
    if (condition == Q_NULL)
        return "<" + Grid::tr_compatible(QtnGridEmptyFilterString) + ">";

    QString retval;
    m_brackets.insert(condition);
    retval = condition->createPresentation();
    m_brackets.remove(condition);

    return retval;
}

int GridFilter::conditionCount()
{
    if (m_condition == Q_NULL)
        return 0;

    int retval = m_condition->conditionCount();
    if (retval > 0)
        retval--; // Root group should be excluded.
    return retval;
}

/* GridFilterCondition */
GridFilterCondition::GridFilterCondition(GridFilter* filter)
: m_filter(filter)
{
}

/* GridFilterValueCondition */
GridFilterValueCondition::GridFilterValueCondition(GridFilter* filter)
: GridFilterCondition(filter), m_modelColumn(-1), m_dataBinding(Q_NULL), m_role(Qt::DisplayRole), m_op(FilterUndefined)
{
}

int GridFilterValueCondition::modelColumn() const
{
    return m_modelColumn;
}

void GridFilterValueCondition::setModelColumn(int column)
{
    m_modelColumn = column;
    m_dataBinding = Q_NULL;
}

GridModelDataBinding* GridFilterValueCondition::dataBinding() const
{
    return m_dataBinding;
}

void GridFilterValueCondition::setDataBinding(GridModelDataBinding* binding)
{
    m_dataBinding = binding;
    m_modelColumn = m_dataBinding != Q_NULL ? m_dataBinding->column() : -1;
}

Qt::ItemDataRole GridFilterValueCondition::modelRole() const
{
    return m_role;
}

void GridFilterValueCondition::setModelRole(Qt::ItemDataRole role)
{
    m_role = role;
}

GridFilterCondition::ValueOperator GridFilterValueCondition::valueOperator() const
{
    return m_op;
}

void GridFilterValueCondition::setValueOperator(ValueOperator op)
{
    m_op = op;
}

void GridFilterValueCondition::addValue(const QVariant& v)
{
    m_values.insert(v, 0);
}

void GridFilterValueCondition::clearValues()
{
    m_values.clear();
}

bool GridFilterValueCondition::isTrue(int modelRow)
{
    QVariant currentValue;
    bool retval = true;
    if (m_dataBinding)
        m_dataBinding->getValue(modelRow, modelRole(), currentValue);
    else
        m_filter->m_controller->getModelValue(modelColumn(), modelRow, modelRole(), currentValue);

    if (valueOperator() == FilterEqual)
    {
        bool ret = m_values.find(currentValue) != m_values.end();
        if (ret || currentValue.toString() != "")
            return ret;
        return m_values.find(QVariant()) != m_values.end();
    }

    if (valueOperator()  == FilterNotEqual)
    {
        bool ret = m_values.find(currentValue) != m_values.end();
        if (ret || currentValue.toString() != "")
            return !ret;
        return m_values.find(QVariant()) == m_values.end();
    }

    for (ConditionValues::const_iterator it = m_values.constBegin(); it != m_values.constEnd(); ++it)
    {
        QVariant value = it.key();
        switch (valueOperator())
        {
        case FilterEqual:
            retval = variant_compare(currentValue, value) == 0;
            break;
        case FilterNotEqual:
            retval = variant_compare(currentValue, value) != 0;
            break;
        case FilterLess:
            retval = variant_compare(currentValue, value) < 0;
            break;
        case FilterLessEqual:
            retval = variant_compare(currentValue, value) <= 0;
            break;
        case FilterGreater:
            retval = variant_compare(currentValue, value) > 0;
            break;
        case FilterGreaterEqual:
            retval = variant_compare(currentValue, value) >= 0;
            break;
        case FilterLike:
            {
                QRegExp rx(value.toString());
                rx.setPatternSyntax(QRegExp::Wildcard);
                return rx.exactMatch(currentValue.toString());
            }
            break;
        case FilterNotLike:
            {
                QRegExp rx(value.toString());
                rx.setPatternSyntax(QRegExp::Wildcard);
                return !rx.exactMatch(currentValue.toString());
            }
            break;
        default:
            Q_ASSERT(false);
            return false;
            break;
        }
        if (!retval)
            return false;
    }
    return retval;
}

int GridFilterValueCondition::conditionCount() const
{
    return m_values.size();
}

GridFilterCondition* GridFilterValueCondition::clone() const
{
    GridFilterValueCondition* retval = new GridFilterValueCondition(m_filter);
    retval->m_modelColumn = m_modelColumn;
    retval->m_dataBinding = m_dataBinding;
    retval->m_role = m_role;
    retval->m_op = m_op;
    retval->m_values = m_values;
    return retval;
}

QString GridFilterValueCondition::createPresentation() const
{
    if (m_filter->m_controller->model() == Q_NULL)
        return QString();

    const bool brackets = !m_filter->m_brackets.contains(static_cast<const GridFilterCondition *>(this));
    QString retval = (m_values.size() > 1 && brackets) ? "(" : "";
    QString columnName = m_filter->m_controller->model()->headerData(m_modelColumn, Qt::Horizontal).toString();
    for (ConditionValues::const_iterator it = m_values.constBegin(); it != m_values.constEnd(); ++it)
    {
        if (it != m_values.constBegin())
            retval += QString(" %1 ").arg(Grid::tr_compatible(QtnGridFilteORString));
        retval += "'" + columnName + "'";
        switch (valueOperator())
        {
        case FilterEqual:
            {
                if (it.key().isValid())
                    retval += QString(" %1 ").arg(Grid::tr_compatible(QtnGridFilterEqualString));
                else
                    retval += QString(" %1").arg(Grid::tr_compatible(QtnGridFilteIsBlankString));
            }
            break;
        case FilterNotEqual:
            {
                if (it.key().isValid())
                    retval += QString(" %1 ").arg(Grid::tr_compatible(QtnGridFilterNotEqualString));
                else
                    retval += QString(" %1").arg(Grid::tr_compatible(QtnGridFilterIsNotBlankString));
            }
            break;
        case FilterLess:
            retval += QString(" %1 ").arg(Grid::tr_compatible(QtnGridFilterLessString));
            break;
        case FilterLessEqual:
            retval += QString(" %1 ").arg(Grid::tr_compatible(QtnGridFilterLessorOrEqualString));
            break;
        case FilterGreater:
            retval += QString(" %1 ").arg(Grid::tr_compatible(QtnGridFilterGreaterString));
            break;
        case FilterGreaterEqual:
            retval += QString(" %1 ").arg(Grid::tr_compatible(QtnGridFilterGreaterOrEqualString));
            break;
        case FilterLike:
            retval += QString(" %1 ").arg(Grid::tr_compatible(QtnGridFilterLikekString));
            break;
        case FilterNotLike:
            retval += QString(" %1 ").arg(Grid::tr_compatible(QtnGridFilterNotLikekString));
            break;
        default:
            Q_ASSERT(false);
            return QString();
            break;
        }
        if (it.key().isValid())
            retval += "'" + it.key().toString() + "'";
        if (retval.size() >= 1024)
            return retval;
    }
    return (m_values.size() > 1 && brackets) ? retval + ")" : retval;
}


/* GridFilterFrozenValueCondition */
GridFilterFrozenValueCondition::GridFilterFrozenValueCondition(GridFilter* filter)
    : GridFilterCondition(filter)
{
}

bool GridFilterFrozenValueCondition::isTrue(int modelRow)
{
    return m_modelRows.contains(modelRow);
}

GridFilterCondition* GridFilterFrozenValueCondition::clone() const
{
    GridFilterFrozenValueCondition* retval = new GridFilterFrozenValueCondition(m_filter);
    retval->m_modelRows = m_modelRows;
    return retval;
}

QString GridFilterFrozenValueCondition::createPresentation() const
{
    return "";
}

int GridFilterFrozenValueCondition::conditionCount() const
{
    return m_modelRows.size();
}

bool GridFilterFrozenValueCondition::getFrozenPosition(int modelRow) const
{
    QHash<int, bool>::const_iterator it = m_modelRows.find(modelRow);
    if (it == m_modelRows.constEnd())
        return false;
    return it.value();
}

void GridFilterFrozenValueCondition::addFrozenModelRow(int modelRowIndex, bool topPosition)
{
    m_modelRows.insert(modelRowIndex, topPosition);
}

void GridFilterFrozenValueCondition::removeFrozenModelRow(int modelRowIndex)
{
    m_modelRows.remove(modelRowIndex);
}

/* GridFilterGroupCondition */
GridFilterGroupCondition::GridFilterGroupCondition(GridFilter* filter)
: GridFilterCondition(filter)
{
}
GridFilterGroupCondition::~GridFilterGroupCondition()
{
    clear();
}

GridFilterGroupCondition* GridFilterGroupCondition::addGroup(GroupOperator groupOperator)
{
    GridFilterGroupCondition* condition = new GridFilterGroupCondition(m_filter);
    Item item = {condition, groupOperator};
    m_items.push_back(item);
    return condition;
}

GridFilterValueCondition* GridFilterGroupCondition::addValue(int modelColumn, Qt::ItemDataRole role,
                                                             const QVariant& value, 
                                                             ValueOperator valueOperator,
                                                             GroupOperator groupOperator)
{
    GridFilterValueCondition* condition = new GridFilterValueCondition(m_filter);
    condition->setModelColumn(modelColumn);
    condition->setModelRole(role);
    condition->addValue(value);
    condition->setValueOperator(valueOperator);
    Item item = {condition, groupOperator};
    m_items.push_back(item);
    return condition;
}

void GridFilterGroupCondition::addValue(GridFilterValueCondition* condition,
                                        GroupOperator groupOperator)
{
    Item item = {condition, groupOperator};
    m_items.push_back(item);
}

void GridFilterGroupCondition::addCondition(GridFilterCondition* condition, GroupOperator groupOperator)
{
    Item item = {condition, groupOperator};
    m_items.push_back(item);
}

int GridFilterGroupCondition::count() const
{
    return m_items.size();
}

void GridFilterGroupCondition::removeCondition(int index)
{
    if (index < 0 || index >= m_items.size())
        return;
    delete m_items[index].c;
    m_items.removeAt(index);
}

GridFilterCondition* GridFilterGroupCondition::getCondition(int index, GroupOperator& op) const
{
    if (index < 0 || index >= m_items.size())
        return 0;
    op = m_items.at(index).op;
    return m_items.at(index).c;
}

void GridFilterGroupCondition::clear()
{
    for (QList<GridFilterGroupCondition::Item>::iterator it = m_items.begin(); it != m_items.end(); ++it)
        delete (*it).c;
    m_items.clear();
}

bool GridFilterGroupCondition::isTrue(int modelRow)
{
    bool retval = true;
    int size = count();
    for (int i = 0; i < size; ++i)
    {
        GroupOperator op = FilterGroupUndefined;
        GridFilterCondition* c = getCondition(i, op);
        switch (op)
        {
        case FilterOr:
            if (i == 0) //First iteration for OR condition
                retval = c->isTrue(modelRow);
            else
                retval |= c->isTrue(modelRow);
        break;
        case FilterAnd:
            if (i == 0) //First iteration for AND condition
                retval = c->isTrue(modelRow);
            else
                retval &= c->isTrue(modelRow);
            break;
        case FilterNot:
            if (i == 0) //First iteration for NOT condition
                retval = c->isTrue(modelRow);
            else
                retval &= !c->isTrue(modelRow);
            break;
        default:
            Q_ASSERT(false);
            return false;
            break;
        };
    }
    return retval;
}

int GridFilterGroupCondition::conditionCount() const
{
    int retval = m_items.size();
    for (QList<GridFilterGroupCondition::Item>::const_iterator it = m_items.constBegin(); it != m_items.constEnd(); ++it)
        retval += (*it).c->conditionCount();
    return retval;
}

GridFilterCondition* GridFilterGroupCondition::clone() const
{
    GridFilterGroupCondition* retval = new GridFilterGroupCondition(m_filter);
    for (QList<GridFilterGroupCondition::Item>::const_iterator it = m_items.constBegin(); it != m_items.constEnd(); ++it)
    {
        Item item;
        item.op = (*it).op;
        item.c = (*it).c->clone();
        retval->m_items.push_back(item);
    }
    return retval;
}

QString GridFilterGroupCondition::createPresentation() const
{
    const bool brackets = !m_filter->m_brackets.contains(static_cast<const GridFilterCondition *>(this));
    QString retval = brackets ? "(" : "";
    for (QList<GridFilterGroupCondition::Item>::const_iterator it = m_items.constBegin(); it != m_items.constEnd(); ++it)
    {
        if (it != m_items.constBegin())
        {
            switch ((*it).op)
            {
            case FilterOr:
                retval += QString(" %1 ").arg(Grid::tr_compatible(QtnGridFilteORString));
            break;
            case FilterAnd:
                retval += QString(" %1 ").arg(Grid::tr_compatible(QtnGridFilterANDString));
                break;
            case FilterNot:
                retval += QString(" %1 ").arg(Grid::tr_compatible(QtnGridFilterNOTString));
                break;
            default:
                Q_ASSERT(false);
                return QString();
                break;
            };
        }

        if (!brackets && m_items.size() == 1)
            m_filter->m_brackets.insert((*it).c);

        retval += (*it).c->createPresentation();
        m_filter->m_brackets.remove((*it).c);

        if (retval.size() >= 1024)
            return retval;
    }
    retval += brackets ? ")" : "";
    return retval;
}
