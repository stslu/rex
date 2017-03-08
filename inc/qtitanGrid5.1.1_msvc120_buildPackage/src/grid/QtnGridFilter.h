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

#ifndef QTN_GRID_FILTER_H
#define QTN_GRID_FILTER_H

#include <QVector>
#include <QSet>
#include <QVariant>
#include "QtnGridDef.h"

QTITAN_BEGIN_NAMESPACE

class GridFilterCondition;
class GridFilterValueCondition;
class GridFilterGroupCondition;
class GridFilter;
class GridModelController;
class GridModelDataBinding;

class QTITAN_EXPORT GridFilterHistory: public QObject
{
    Q_OBJECT
public:
    GridFilterHistory(GridFilter* filter);
    virtual ~GridFilterHistory();
    int count() const;
    GridFilterGroupCondition* getCondition(int index) const;
    int currentIndex();
    void setCurrentIndex(int index);
    void setCondition(GridFilterGroupCondition* condition, bool update);
    GridFilter* filter() const { return m_filter; }
    void clear();
Q_SIGNALS:
    void changed();
private:
    GridFilter* m_filter;
    QList<GridFilterGroupCondition *> m_conditions;
    int m_currentIndex;
};

class QTITAN_EXPORT GridFilter: public QObject
{
public:
    friend class GridFilterValueCondition;
    friend class GridFilterGroupCondition;
    friend class GridFilterHistory;
    friend class GridModelController;
    GridFilter(GridModelController* controller);
    virtual ~GridFilter();
    GridFilterHistory* history() const;
    GridFilterGroupCondition* condition() const;
    void setCondition(GridFilterGroupCondition* groupCondition, bool addToHistory = true);
    bool validateRow(int modelRow);
    QString getName();
    void setName(const QString& name);
    bool isActive() const;
    void setActive(bool flag);
    void clear();
    QString getPresentation(GridFilterCondition* condition);
    int conditionCount();
private:
    GridModelController* m_controller;
    GridFilterHistory* m_history;
    GridFilterGroupCondition* m_condition;
    QString m_name;
    bool m_active;
    QSet<const GridFilterCondition *> m_brackets;
};

class QTITAN_EXPORT GridFilterCondition: public QObject
{
public:
    enum ValueOperator
    {
        FilterUndefined,
        FilterEqual,
        FilterNotEqual,
        FilterLess,
        FilterLessEqual,
        FilterGreater,
        FilterGreaterEqual,
        FilterLike,
        FilterNotLike
    };

    enum GroupOperator
    {
        FilterGroupUndefined,
        FilterOr,
        FilterAnd,
        FilterNot
    };

    GridFilterCondition(GridFilter* filter);
    virtual bool isTrue(int modelRow) = 0;
    virtual GridFilterCondition* clone() const = 0;
    virtual QString createPresentation() const = 0;
    virtual int conditionCount() const = 0;
protected:
    GridFilter* m_filter;
};

class QTITAN_EXPORT GridFilterValueCondition: public GridFilterCondition
{
    Q_OBJECT
public:
    GridFilterValueCondition(GridFilter* filter);
    int modelColumn() const;
    void setModelColumn(int column);
    GridModelDataBinding* dataBinding() const;
    void setDataBinding(GridModelDataBinding* dataBinding);
    Qt::ItemDataRole modelRole() const;
    void setModelRole(Qt::ItemDataRole role);
    ValueOperator valueOperator() const;
    void setValueOperator(ValueOperator op);
    void addValue(const QVariant& v);
    void clearValues();
    virtual bool isTrue(int modelRow);
    virtual GridFilterCondition* clone() const;
    virtual QString createPresentation() const;
    virtual int conditionCount() const;
private:
    int m_modelColumn;
    GridModelDataBinding* m_dataBinding;
    Qt::ItemDataRole m_role;
    ValueOperator m_op;
    typedef QHash<QVariant, unsigned char> ConditionValues;
    ConditionValues m_values;
};

class QTITAN_EXPORT GridFilterFrozenValueCondition: public GridFilterCondition
{
public:
    GridFilterFrozenValueCondition(GridFilter* filter);
    virtual bool isTrue(int modelRow);
    virtual GridFilterCondition* clone() const;
    virtual QString createPresentation() const;
    virtual int conditionCount() const;
public:
    bool getFrozenPosition(int modelRow) const;
    void addFrozenModelRow(int modelRowIndex, bool topPosition);
    void removeFrozenModelRow(int modelRowIndex);
private:
    QHash<int, bool> m_modelRows;
};

class QTITAN_EXPORT GridFilterGroupCondition: public GridFilterCondition
{
    Q_OBJECT
public:
    GridFilterGroupCondition(GridFilter* filter);
    virtual ~GridFilterGroupCondition();
    GridFilterGroupCondition* addGroup(GroupOperator groupOperator = FilterAnd);
    GridFilterValueCondition* addValue(int modelColumn, Qt::ItemDataRole role,
                                        const QVariant& value, 
                                        ValueOperator valueOperator = FilterEqual,
                                        GroupOperator groupOperator = FilterAnd);
    void addValue(GridFilterValueCondition* valueCondition, GroupOperator groupOperator = FilterAnd);
    void addCondition(GridFilterCondition* condition, GroupOperator groupOperator = FilterAnd);
    void removeCondition(int index);
    int count() const;
    GridFilterCondition* getCondition(int index, GroupOperator& op) const;
    void clear();
    virtual bool isTrue(int modelRow);
    virtual GridFilterCondition* clone() const;
    virtual QString createPresentation() const;
    virtual int conditionCount() const;
private:
    struct Item
    {
        GridFilterCondition* c;
        GroupOperator op;
    };
    QList<GridFilterGroupCondition::Item> m_items;
};

QTITAN_END_NAMESPACE

#endif //QTN_GRID_FILTER_H
