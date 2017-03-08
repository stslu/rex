/****************************************************************************
**
** Qtitan    Library by Developer Machines (Advanced Grid for Qt)
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

#ifndef QTN_MODELCONTROLLER_H
#define QTN_MODELCONTROLLER_H

#include <QApplication>
#include <QAbstractItemModel>
#include <QItemSelectionModel>

#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class GridModelController;
class GridRow;
class GridCell;
class GridPersistentRow;
class GridSummary;
class GridSelection;
class GridModelDataBinding;
class RelationEventArgs;
class GridFilter;

enum SortOrder
{
    SortNone,
    SortAscending,
    SortDescending
};

struct GridSortColumnRule
{
    SortOrder order;
    bool caseSensitivity;
    bool localeAware;
    GridModelDataBinding* dataBinding;
};

enum GridEditStrategy
{
    OnFieldChange,
    OnRowChange
};

class QTITAN_EXPORT GridSortColumnRules
{
public:
    GridSortColumnRules();
    void clear();
    void clearSorting();
    void clearGrouping();
    void setSortRule(GridModelDataBinding* dataBinding, int index, SortOrder order, bool caseSensitivity = false, bool localeAware = false);
    void setGroupRule(GridModelDataBinding* dataBinding, int index, SortOrder order, bool caseSensitivity = false, bool localeAware = false);
    int count() const;
    const GridSortColumnRule& rule(int index) const;
    int groupRulesCount() const;
    const GridSortColumnRule* groupRule(int index) const;
    const GridSortColumnRule* groupRuleByColumn(GridModelDataBinding* dataBinding) const;
    int sortRulesCount() const;
    const GridSortColumnRule* sortRule(int index) const;
    const GridSortColumnRule* sortRuleByColumn(GridModelDataBinding* dataBinding) const;
protected:
private:
    QList<GridSortColumnRule> m_sortRulesList;
    QList<GridSortColumnRule> m_groupRulesList;
};

class GridModelGroup
{
public:
    GridModelDataBinding* dataBinding;
    int depth;
    int rowIndex;
    int index;       // Index of group structure in the controller group list.
    int parentIndex; // Index of parent group structure in the controller group list.
    bool last;
    int firstRecord;
    int lastRecord;
    bool expanded;
    QVariant groupValue; // The base value of which the group was generated.
};

struct RecordMapKey
{
    RecordMapKey(const QVariant& v, int c)
    {
        value = v;
        column = c;
    };
    QVariant value;
    int column;
};

inline uint qHash(const Qtitan::RecordMapKey &key, uint seed = 0) Q_DECL_NOTHROW
{
    return qHash(key.value.toString()) + (key.column ^ seed);
}

inline bool operator==(const RecordMapKey& first, const RecordMapKey& second)
{
    return first.value == second.value && first.column == second.column;
}

typedef QHash<RecordMapKey, bool> GroupingSavedState;

class QTITAN_EXPORT GridPersistentRow
{
public:
    friend class GridCellSelectionRange;
    friend class GridModelController;
    GridPersistentRow(GridModelController* controller);
    GridPersistentRow(const GridPersistentRow& index);
    virtual ~GridPersistentRow();
    void clear();
    bool isValid() const;
    void set(GridRow* gridRow);
    bool equal(GridRow* gridRow) const;
    void setColumnIndex(int index);
    int columnIndex() const;
    int groupModelColumn() const;
    int modelRowIndex() const;
    GridRow* row() const;
    int record() const;
    void setRecord(int record);
    GridPersistentRow& operator=(const GridPersistentRow& index);
private:
    GridModelController* m_controller;
    int m_columnIndex;
    int m_record;
    int m_modelRowIndex;
    int m_groupColumn;
    GridRow* m_gridRow;
};

enum SelectionOperator
{
    Empty       = 0x0000,
    Clear          = 0x0001,
    Select         = 0x0002,
    Unselect       = 0x0004,
    Invert         = 0x0008,
    StartCurrent   = 0x0010
};

Q_DECLARE_FLAGS(SelectionOperation, SelectionOperator)

enum GridSummaryType
{
    SummaryNone = 0,
    SummarySum = 0x0001,
    SummaryMin = 0x0002,
    SummaryMax = 0x0004,
    SummaryCount = 0x0008,
    SummaryAvg = 0x0010
};
Q_DECLARE_FLAGS(SummaryTypes, GridSummaryType)

enum GridControllerChangeFlag
{
    ControllerNoChanges = 0x0000,
    ControllerSortingChanged = 0x0001,
    ControllerGroupingChanged = 0x0002,
    ControllerRowsChanged = 0x0004,
    ControllerDataChanged = 0x0008,
    ControllerLayoutChanged = 0x0010,
    ControllerFocusChanged = 0x0020,
    ControllerSelectionChanged = 0x0040,
};
Q_DECLARE_FLAGS(GridControllerChanges, GridControllerChangeFlag)

class GridViewInterface
{
public:
    virtual ~GridViewInterface() {}
    virtual void controllerChanged(GridControllerChanges changes) = 0;
    virtual void raiseQueryRelation(RelationEventArgs* args) = 0;
    virtual void raiseGroupExpanding(GridModelGroup* group, bool& allow) = 0;
    virtual void raiseGroupExpanded(GridModelGroup* group) = 0;
    virtual void raiseGroupCollapsing(GridModelGroup* group, bool& allow) = 0;
    virtual void raiseGroupCollapsed(GridModelGroup* group) = 0;
    virtual void raiseFilterActivated() = 0;
    virtual void raiseFilterDeactivated() = 0;
    virtual void raiseSelectionChanged(GridSelection* oldSelection, GridSelection* selection) = 0;
    virtual GridModelDataBinding* dataBindingByColumnIndex(int columnIndex) = 0;
    virtual void resetModel() = 0;
    virtual void modelDestroyed() = 0;
    virtual void columnInserted(int modelColumn) = 0;
    virtual void columnRemoved(int modelColumn) = 0;
    virtual bool isDestroying() = 0;
};

typedef QHash<Qt::ItemDataRole, QVariant> DefaultValueMap;

class QTITAN_EXPORT GridModelDataBinding: public QObject
{
    Q_OBJECT
public:
    enum RelationMode
    {
        NoRelation = 0,
        Relation = 1,
        MasterDetailsRelation = 2
    };

public:
    GridModelDataBinding(GridModelController* modelController, int column);
    virtual ~GridModelDataBinding();
    int column() const;
    void setColumn(int column);
    QString columnName() const;
    Qt::ItemDataRole displayRole() const;
    void setDisplayRole(Qt::ItemDataRole role = Qt::DisplayRole);
    Qt::ItemDataRole editRole() const;
    void setEditRole(Qt::ItemDataRole role = Qt::EditRole);
    Qt::ItemDataRole sortRole() const;
    void setSortRole(Qt::ItemDataRole role = Qt::DisplayRole);
    Qt::ItemDataRole summaryRole() const;
    void setSummaryRole(Qt::ItemDataRole role = Qt::DisplayRole);
    SummaryTypes supportedSummaryTypes() const;
    void setSupportedSummaryTypes(SummaryTypes types);
    RelationMode relationMode() const;
    void setRelationMode(RelationMode mode);
    QAbstractItemModel* relationModel();
    void setRelationModel(QAbstractItemModel* model, QModelIndex parentIndex = QModelIndex());
    const QModelIndex& relationParentIndex() const;
    void setForeignKey(int foreignColumn, Qt::ItemDataRole foreignRole = Qt::DisplayRole, Qt::ItemDataRole masterRole = Qt::EditRole);
    void setForeignKey(const QString& foreignColumnName, Qt::ItemDataRole foreignRole = Qt::DisplayRole, Qt::ItemDataRole masterRole = Qt::EditRole);
    int foreignKeyColumn() const;
    QString foreignKeyColumnName() const;
    Qt::ItemDataRole foreignKeyRole() const;
    Qt::ItemDataRole foreignKeyMasterRole() const;
    void setRoleMapping(Qt::ItemDataRole role, const QString& foreignColumnName, Qt::ItemDataRole foreignRole = Qt::DisplayRole);
    void setRoleMapping(Qt::ItemDataRole role, int foreignColumn, Qt::ItemDataRole foreignRole = Qt::DisplayRole);
    int getForeignColumnMapping(Qt::ItemDataRole role) const;
    QString getForeignColumnNameMapping(Qt::ItemDataRole role) const;
    Qt::ItemDataRole getForeignRoleMapping(Qt::ItemDataRole role) const;
public:
    GridModelController* controller() const;
    bool getValue(int modelRowIndex, Qt::ItemDataRole role, QVariant& retval) const;
    bool setValue(int modelRowIndex, Qt::ItemDataRole role, const QVariant& value);
    QModelIndex findRelationModelIndex(const QVariant& foreignKeyValue) const;
private:
    int columnByColumnName(const QString& columnName) const;
    bool isRelationModelColumnRoleSupported(Qt::ItemDataRole role) const;
    QModelIndex getRelationModelIndex(int modelRowIndex, Qt::ItemDataRole role, Qt::ItemDataRole& foreignRole) const;
    GridModelController* m_modelController;
    int m_column;
    Qt::ItemDataRole m_displayRole;
    Qt::ItemDataRole m_editRole;
    Qt::ItemDataRole m_sortRole;
    Qt::ItemDataRole m_summaryRole;
    SummaryTypes m_supportedSummaryTypes;
    RelationMode m_relationMode;
    QAbstractItemModel* m_relationModel;
    QModelIndex m_relationParentIndex;
    int m_foreignKeyColumn;
    Qt::ItemDataRole m_foreignKeyMasterRole;
    Qt::ItemDataRole m_foreignKeyRole;
    struct IndexColumn
    {
        IndexColumn()
            : role(Qt::UserRole)
        {
        }
        IndexColumn(int c, Qt::ItemDataRole r)
            : column(c), role(r)
        {
        }
        int column;
        Qt::ItemDataRole role;
    };
    typedef QHash<Qt::ItemDataRole, IndexColumn> ForeignColumnRoleMap;
    ForeignColumnRoleMap m_foreignColumnRoleMap;
};

class QTITAN_EXPORT RelationEventArgs: public BaseEventArgs
{
public:
    RelationEventArgs(GridModelController* controller, GridModelDataBinding* binding);
    GridModelDataBinding* dataBinding() const;
    GridModelController* controller() const;
private:
    GridModelController* m_controller;
    GridModelDataBinding* m_dataBinding;
};

class QTITAN_EXPORT GridCell
{
public:
    GridCell();
    GridCell(GridModelController* controller, int rowIndex, int columnIndex);
    GridCell(GridRow* gridRow, int columnIndex);
    GridRow* row() const;
    int rowIndex() const;
    int columnIndex() const;
    QModelIndex modelIndex() const;
private:
    GridRow* m_row;
    int m_columnIndex;
};

inline uint qHash(const GridCell& cell)
{ 
    return uint((cell.rowIndex() << 4) + cell.columnIndex());
}
inline bool operator==(const GridCell& first, const GridCell& second)
{
    return first.row() == second.row() && first.columnIndex() == second.columnIndex();
}


class QTITAN_EXPORT GridRow
{
public:
    enum RowType
    {
        DataRow,
        GroupRow
    };

    enum FrozenPlace
    {
        FrozenNone,
        FrozenTop,
        FrozenBottom
    };
public:
    friend class GridPersistentRow;
    friend class GridModelController;
    GridRow();
    RowType type() const;
    int depth() const;
    int rowIndex() const;
    int modelRowIndex() const;
    GridRow* parentGroupRow() const;
    GridRow* groupRow() const;
    GridModelGroup* modelGroup() const;
    bool last() const;
    bool isExpanded() const;
    void setExpanded(bool expanded);
    bool isPreviewExpanded() const;
    void setPreviewExpanded(bool expanded);
    int groupColumn() const;
    bool isFrozen() const;
    void setFrozen(bool b);
    FrozenPlace frozenPlace() const;
    GridCell cell(int index) const;
    bool groupValue(QVariant& retval) const;
    GridModelController* controller() const;
public:
    static bool compareByScrollPosition(GridRow* modelRow, const int& ypos);
    static bool compareByRecord(GridRow* modelRow, const int& record);
protected:
    void initialize(GridModelController* controller, RowType type, GridModelGroup* group, int rowIndex);
    void reset();
    bool isInitialized() const;
    int record() const;
    int m_scrollPosition;
private:
    GridModelController* m_controller;
    RowType m_type;
    int m_rowIndex;
    GridModelGroup* m_group;
    bool m_initialized;
    bool m_previewExpanded;
};

class QTITAN_EXPORT GridCellSelectionRange
{
public:
    GridCellSelectionRange(GridModelController* controller, const GridCell& topLeft, const GridCell& bottomRight);
    GridCell topLeft() const;
    GridCell bottomRight() const;
    bool contains(const GridCell& cell) const;
    bool contains(const GridRow* row) const;
    bool contains(const GridCellSelectionRange& range) const;
    bool isValid() const;
    bool intersects(const GridCellSelectionRange& range) const;
    GridCellSelectionRange intersected(const GridCellSelectionRange& range) const;
    GridCellSelectionRange unite(const GridCellSelectionRange& range) const;
    inline int top() const { return topLeft().rowIndex(); }
    inline int left() const { return topLeft().columnIndex(); }
    inline int bottom() const { return bottomRight().rowIndex(); }
    inline int right() const { return bottomRight().columnIndex(); }
    inline int width() const { return bottomRight().columnIndex() - topLeft().columnIndex() + 1; }
    inline int height() const { return bottomRight().rowIndex() - topLeft().rowIndex() + 1; }
private:
    GridModelController* m_controller;
    GridPersistentRow m_persistTopLeft;
    GridPersistentRow m_persistBottomRight;
};

   
typedef QList<GridCellSelectionRange> SelectionRanges;

class QTITAN_EXPORT GridSelection
{
public:
    void reset();
    bool next();
    bool end();
    GridRow* row() const;
    GridCell* cell() const;
    const SelectionRanges& ranges() const;
protected:
    GridSelection(GridModelController* controller);
    friend class GridModelController;
private:
    void clear();
    GridModelController* m_controller;
    SelectionRanges m_ranges;
    QList<GridCell>::iterator m_current;
    QList<GridCell> m_select;
    bool m_dirty;
};

#define GridSelectionIterator GridSelection

class QTITAN_EXPORT GridModelController: public QObject
{
    Q_OBJECT
public:
    friend class GridModelDataBinding;
    friend class GridRow;
    friend class GridCell;
    friend class GridSummary;
    friend class GridSummaryCalculator;
    friend class GridSelection;
    friend class GridPersistentRow;
    friend class GridFilter;
    friend class GridFilterValueCondition;
    typedef QVector<GridModelGroup> ModelGroupList;
    GridModelController(GridViewInterface* viewInterface);
    virtual ~GridModelController();
    void setModel(QAbstractItemModel* model, QModelIndex parent = QModelIndex());
    QAbstractItemModel* model() const;
    void setSelectionModel(QItemSelectionModel* selectionModel);
    QItemSelectionModel* selectionModel() const;
    const QModelIndex& parentIndex() const;
    void setSortOrder(GridModelDataBinding* dataBinding, int sortIndex, SortOrder order, bool caseSensitivity = false, bool localeAware = false);
    SortOrder getSortOrder(GridModelDataBinding* dataBinding) const;
    void setGroupOrder(GridModelDataBinding* dataBinding, int groupIndex, SortOrder order, bool caseSensitivity = false, bool localeAware = false);
    SortOrder getGroupOrder(GridModelDataBinding* dataBinding) const;
    void clearSorting();
    void clearGrouping();
    bool hasGrouping() const;
    int groupingDepth() const;
    int getModelColumnCount() const;
    int getGridRowCount() const;
    GridRow* getGridRow(int index);
    GridRow* getGridRowByModelRow(int modelRow);
    int getFrozenTopGridRowCount() const;
    int getFrozenBottomGridRowCount() const;
    int modelGroupCount() const;
    GridModelGroup* modelGroup(int index);
    ModelGroupList& groups();
    void expandAllGroup();
    void collapseAllGroup();
    bool expandGroup(GridModelGroup* group, bool disableChanged = false);
    bool collapseGroup(GridModelGroup* group, bool disableChanged = false);
    void refresh();
    GridSummary* summary() const;
    GridFilter* filter() const;
    GridFilter* relationFilter() const;
    GridFilter* frozenRowsFilter() const;
    GridFilter* findFilter() const;
    bool getModelValue(int modelColumn, int modelRow, Qt::ItemDataRole role, QVariant& retval) const;
    bool setModelValue(int modelColumn, int modelRow, Qt::ItemDataRole role, const QVariant& retval);
    bool getPreviewModelValue(int modelRow, QVariant& retval);
    int ensureVisible(int modelRow);
    int focusedRowIndex();
    GridRow* focusedRow();
    int oldFocusedRowIndex() const;
    int oldFocusedColumnIndex() const;
    int focusedColumnIndex() const;
    bool setCellFocus(GridRow* row, int columnIndex);
    bool isRowFocused(GridRow* row);
    bool isCellFocused(GridRow* row, int columnIndex) const;
    GridSelection* selection() const;
    void beginUpdate();
    void endUpdate();
    bool isDataUpdating() const;
    bool hasChanges() const;
    bool insertModelRow(int modelRow);
    bool removeModelRow(int modelRow);
    void beginAddRow(int row);
    void endAddRow();
    bool isNewRowMode() const;
    int addingModelRowIndex() const;
    QVariant defaultValue(int modelColumn, Qt::ItemDataRole role) const;
    void setDefaultValue(int modelColumn, const QVariant& value, Qt::ItemDataRole role);
    void removeDefaultValue(int modelColumn, Qt::ItemDataRole role);
    void removeDefaultValues(int modelColumn);
    void removeDataBinding(GridModelDataBinding* dataBinding);
    bool showWaitCursor();
    void setShowWaitCursor(bool show);
    GridEditStrategy editStrategy() const;
    void setEditStrategy(GridEditStrategy strategy);
    GridRow::FrozenPlace frozenPlace() const;
    void setFrozenPlace(GridRow::FrozenPlace place);
    void addModelRowToFrozenFilter(int modelRowIndex, GridRow::FrozenPlace);
    void removeModelRowFromFrozenFilter(int modelRowIndex);
    void clearFilters(bool clearHistory);
public:
    GridModelDataBinding* previewRowDataBinding() const;
    const GridSortColumnRules& sortRules() const;
public:
    void beginSelect();
    void endSelect();
    void clearSelect();
    //void selectAll();
    void selectRanges(const SelectionRanges& ranges, const SelectionOperation& operation = Qtitan::Select);
    bool isRowSelected(const GridRow* row) const;
    bool isCellSelected(const GridCell& cell) const;
    bool isAllSelected() const;
    int getPureRowCount() const;
    int getRecordCount() const;
    int getModelRowIndexByRecord(int record) const;
    bool submitData();
Q_SIGNALS:
    void selectChanged();

    void aboutToBeSorted(bool& cancel);
    void sortCompleted();
    void sortProgress(int value);

    void aboutToBeFiltered(bool& cancel);
    void filterCompleted();
    void filterProgress(int value);

    void aboutToBeSummaryCalculated(bool& cancel);
    void summaryCompleted();
    void summaryProgress(int value);

    void aboutToBeChanged();
    void changed();

protected:
    int getRowIndexByPersistentRow(const GridPersistentRow* persistentRow);
    void commitCurrentSelect();
    void clearModelGroups();
    void createModelGroups();
    void clearPersistRows();
    void resizeModelRows(int size);
    GridModelGroup* appendModelGroup();
    bool addModelRowToMap(int modelRowIndex);
    void fillRecords();
    void createGridRows();
    GridModelGroup* findGroup(int row, int& groupIndex);
    int findRowIndexByRecord(int record);
    int findRecordByModelRow(int modelRow);
    static int compareModelRows(GridModelController* controller, const int& firstRow, const int& secondRow);

    void saveGroupingState();
    void restoreSelectState();
    void clearGroupingState();
    void applyChanges();
protected Q_SLOTS:
    void modelColumnsInserted(const QModelIndex & parent, int start, int end);
    void modelColumnsMoved(const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationColumn);
    void modelColumnsRemoved(const QModelIndex & parent, int start, int end);
    void modelLayoutChanged();
    void modelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void modelRowsAboutToBeInserted(const QModelIndex& parent, int start, int end);
    void modelRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);
    void modelRowsInserted(const QModelIndex& parent, int start, int end);
    void modelRowsRemoved(const QModelIndex& parent, int start, int end);
    void modelAboutToBeReset();
    void modelReset();
    void modelDestroyed();
private:
    void modelRowsInsertedInt(int start, int end, bool change);
    void modelRowsRemovedInt(int start, int end, bool change);
    void addFrozenModelRow(int modelRow, bool topPosition);
    void removeFrozenModelRow(int modelRow);
    QAbstractItemModel* m_model;
    QItemSelectionModel* m_selectionModel;
    QModelIndex m_parentIndex;
    QVector<int> m_recordsMap;
    QVector<GridRow> m_gridRows;
    QHash<int, bool> m_frozenRows;
    int m_frozenRowsTopCount;
    int m_frozenRowsBottomCount;
    ModelGroupList m_groups;
    GridSortColumnRules m_sortRules;
    GroupingSavedState m_groupingSavedState;
    GridSummary* m_summary;
    GridFilter* m_filter;
    GridFilter* m_relationFilter;
    GridFilter* m_frozenRowsFilter;
    GridFilter* m_findFilter;
    GridPersistentRow* m_focusedRowIndex;
    GridCell m_oldFocusCell;
    SelectionRanges m_selectionRanges;
    SelectionRanges m_currentSelectionRanges;
    SelectionOperation m_currentSelectionOperation;
    bool m_allSelected;
    GridSelection* m_selection;
    int m_selectStack;
    int m_updateDataStack;
    QSet<GridPersistentRow *> m_persistRows;
    int m_addingModelRowIndex;
    GridViewInterface* m_viewInterface;
    int m_pureRowCount;
    bool m_recordSortingDirty;
    bool m_recordGroupingDirty;
    bool m_needSubmit;
    QHash<int, DefaultValueMap> m_defValues;
    bool m_showWaitCursor;
    GridEditStrategy m_editStrategy;
    GridRow::FrozenPlace m_frozenPlace;
    GridModelDataBinding* m_previewRowDataBinding;
    GridControllerChanges m_changes;
};

class AutoOverrideCursor
{
public:
    AutoOverrideCursor(const QCursor& cursor, bool show)
        : m_show(show)
    {
        if (m_show)
            QApplication::setOverrideCursor(cursor);
    };
    virtual ~AutoOverrideCursor()
    {
        if (m_show)
            QApplication::restoreOverrideCursor();
    };
private:
    bool m_show;
};

class SelectAutoUpdater
{
public:
    SelectAutoUpdater(GridModelController* controller)
        : m_controller(controller)
    {
        m_controller->beginSelect();
    }

    ~SelectAutoUpdater()
    {
        m_controller->endSelect();
    }
private:
    GridModelController* m_controller;
};


class ModelControllerAutoUpdater
{
public:
    ModelControllerAutoUpdater(GridModelController* controller)
        : m_controller(controller)
    {
        m_controller->beginUpdate();
    }

    ~ModelControllerAutoUpdater()
    {
        m_controller->endUpdate();
    }
private:
    GridModelController* m_controller;
};

extern int find_modelRowIndex_by_view(GridModelController* controller, int ypos);


struct GridSummaryValue
{
    GridSummaryType type;
    Qt::ItemDataRole role;
    int count;
    QVariant value;
    GridSummaryValue() : type(SummaryNone), role(Qt::DisplayRole), count(0) {}
};

typedef QHash<GridModelDataBinding *, GridSummaryValue> ColumnSummaryValues;

class QTITAN_EXPORT GridSummarySetting
{
public:
    GridSummarySetting(GridModelDataBinding* dataBinding, Qt::ItemDataRole role,  GridSummaryType type);
    GridModelDataBinding* dataBinding() const;
    Qt::ItemDataRole role() const;
    GridSummaryType type() const;
private:
    GridModelDataBinding* m_dataBinding;
    Qt::ItemDataRole m_role;
        GridSummaryType m_type;
};

class QTITAN_EXPORT GridSummaryGroupSetting : public GridSummarySetting
{
public:
    enum ShowingPlace
    {
        FooterPlace,
        GroupPlace
    };
    GridSummaryGroupSetting(GridModelDataBinding* groupDataBinding,
                            GridModelDataBinding* dataBinding,
                            Qt::ItemDataRole role,
                            GridSummaryType type,
                            ShowingPlace place = FooterPlace);
    GridModelDataBinding* groupDataBinding() const;
    bool sorted() const;
    void setSorted(bool sorted);
    ShowingPlace showingPlace() const;
    void setShowingPlace(ShowingPlace place);
private:
    GridModelDataBinding* m_groupDataBinding;
    bool m_sorted;
    ShowingPlace m_showingPlace;
};

class QTITAN_EXPORT GridSummary: public QObject
{
public:
    friend class GridModelController;
    GridSummary(GridModelController* controller);
    GridModelController* controller() const;
    void setFooterSetting(GridModelDataBinding* dataBinding, Qt::ItemDataRole role,  GridSummaryType type);
    void setDefaultGroupSetting(GridModelDataBinding* dataBinding, Qt::ItemDataRole role,  GridSummaryType type);
    void setGroupSetting(GridModelDataBinding* groupDataBinding, GridModelDataBinding* dataBinding, Qt::ItemDataRole role, GridSummaryType type);

    const GridSummarySetting& footerSetting(int index) const;
    int footerSettingsCount() const;
    bool footerSettingsExists(GridModelDataBinding* dataBinding) const;

    const GridSummaryGroupSetting& defaultGroupSetting(int index) const;
    int defaultGroupSettingsCount() const;

    const GridSummaryGroupSetting& groupSetting(GridModelDataBinding* groupDataBinding, int index) const;
    int groupSettingsCount(GridModelDataBinding* groupDataBinding) const;
    void getGroupSummarySettings(GridModelDataBinding* groupDataBinding, QVector<const GridSummaryGroupSetting *>& settings) const;
    bool groupSettingsExists(GridModelDataBinding* dataBinding) const;

    const GridSummaryValue& getFooterSummaryValue(GridModelDataBinding* dataBinding) const;
    QString getFooterSummaryDisplayText(GridModelDataBinding* dataBinding) const;
    const GridSummaryValue& getGroupSummaryValue(GridModelGroup* group, GridModelDataBinding* dataBinding) const;
    QString getGroupSummaryDisplayText(GridModelGroup* group, GridModelDataBinding* dataBinding) const;
    bool supportSummaryType(GridModelDataBinding* dataBinding, GridSummaryType type) const;
    void removeForDataBinding(GridModelDataBinding* dataBinding);
private:
    void calculate();

    void markChanged(int modelColumn = -1);
    bool do_calculate_summary(GridModelDataBinding* groupDataBinding, void* progressContext);
    void createDisplayText(const GridSummaryValue& value, bool isFooterSummary, QString& retval) const;
    GridModelController* m_controller;
    QHash<GridModelDataBinding *, GridSummarySetting> m_footerSummaryMap;
    QHash<GridModelDataBinding *, QHash<GridModelDataBinding *, GridSummaryGroupSetting> > m_groupSummaryMap;
    QVector<GridSummaryValue> m_footerResults;
    QVector<GridSummaryValue> m_groupResults;
    bool m_changed;
};

int variant_compare(const QVariant &v1, const QVariant& v2, bool caseSensitivity = false, bool localeAware = false);

QTITAN_END_NAMESPACE

Q_DECLARE_OPERATORS_FOR_FLAGS(Qtitan::SummaryTypes)
Q_DECLARE_OPERATORS_FOR_FLAGS(Qtitan::SelectionOperation)


#endif //QTN_MODELCONTROLLER_H

