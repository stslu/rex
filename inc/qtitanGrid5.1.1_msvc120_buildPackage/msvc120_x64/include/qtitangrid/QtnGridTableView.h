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

#ifndef QTN_GRIDTABLEVIEW_H
#define QTN_GRIDTABLEVIEW_H

#include <QTimer>
#include <QMenu>
#include <QStyleOption>
#include <QDrag>

#include "QtnGrid.h"
#include "QtnGridFilter.h"
#include "QtnGridEditors.h"

QTITAN_BEGIN_NAMESPACE

class GridTableView;
class GridTableColumnBase;
class GridTableColumn;
class GridTableDragColumnObject;
class GridTableViewFixedDecorGraphic;
class QuickCustomizationPopup;
class GridTableViewOptions;
class GridTableRowsPrimitive;
class GridTableCellButtonGraphic;
class GridTableRowsLayoutEngine;
class GridTableRowLayoutItem;
class GridDataRowPrimitive;
class GridGroupRowPrimitive;
class GridTableCellGraphic;
class GridTableGroupSummaryPrimitive;
class GridContextMenu;
class GridSummaryContextMenu;
class GridTableColumnEditor;
class GridColumnEditorGraphic;
class GridTableGroupPanelPrimitive;
class GridTableColumnFilterButton;
class GridFilterColumnPopup;
class GridTableQuickSelectButtonPrimitive;
class GridFieldChooserPanel;
class GridTableViewPrimitive;
class GridTableColumnPrimitive;

enum ColumnFixedKind
{
    FixedNone,
    FixedLeft,
    FixedRight
};


class QTITAN_EXPORT GridCellButtonStyleOption: public QStyleOption
{
public:
    GridCellButtonStyleOption(int version = QStyleOption::Version, int type = QStyleOption::SO_Default);
};

class QTITAN_EXPORT GridCustomDrawEventArgs: public BaseEventArgs
{
public:
    GridCustomDrawEventArgs(QPainter* p, GridCellButtonStyleOption* opt);
    QPainter* painter() const;
    GridCellButtonStyleOption* option() const;
private:
    QPainter* m_painter;
    GridCellButtonStyleOption* m_option;
};

class QTITAN_EXPORT CellButtonEventArgs: public BaseEventArgs
{
public:
    CellButtonEventArgs(GridRow* row, int columnIndex, int buttonIndex);
    virtual ~CellButtonEventArgs();
    GridRow* row() const;
    int columnIndex() const;
    QModelIndex modelIndex() const;
    int buttonIndex() const;
    bool isEnadled() const;
    void setEnadled(bool enabled);
private:
    GridCell* m_cell;
    int m_buttonIndex;
    bool m_enabled;
};

class QTITAN_EXPORT CellButtonClickEventArgs: public BaseEventArgs
{
public:
    CellButtonClickEventArgs(int index, GridTableColumn* c, GridRow* row);
    int buttonIndex() const;
    GridTableColumn* column() const;
    GridRow* row() const;
private:
    int m_buttonIndex;
    GridTableColumn* m_column;
    GridRow* m_row;
};

class QTITAN_EXPORT GridTableView: public GridViewBase
{
    Q_OBJECT
public:
    friend class Grid;
    friend class GridRowPrimitiveAbstract;
    friend class GridTableRowsPrimitive;
    friend class GridTableViewPrimitive;
    friend class GridTableColumnBase;
    friend class GridTableColumn;
    friend class GridTableColumnPrimitive;
    friend class GridTableGroupPanelPrimitive;
    friend class GridTableHeaderGraphic;
    friend class GridBandedTableHeaderGraphic;
    friend class GridTableDragColumnObject;
    friend class GridTableColumnSizingObject;
    friend class GridTableViewOptions;
    friend class GridTableViewFixedDecorGraphic;
    friend class QuickCustomizationPopup;
    friend class GridTableRowLayoutItem;
    friend class GridDataRowPrimitive;
    friend class GridTableGroupSummaryCellGraphic;
    friend class GridTableFooterSummaryCellGraphic;
    friend class GridTableCellPrimitiveAbstract;
    friend class GridTableRowsLayoutEngine;
    friend class GridNewRowGraphic;
    friend class GridTableQuickRowButtonGraphic;
    friend class GridGroupRowPrimitive;
    friend class GridFilterColumnPopup;
    friend class FilterPopupWidget;
    friend class DragColumnWidget;
    friend class GridBandedTableDragColumnObject;
    friend class GridPreviewExpandButtonPrimitive;
    friend class GridViewSelectionObject;
    friend class GridTableViewSelectionObject;

    typedef QList<GridTableColumn *> GridTableColumns;
    typedef QSet<GridTableColumn *> GridColumnSet;

    static GridTableView* create(Grid* grid);
    GridTableColumn* addColumn(const QString& modelColumnName, const QString& caption = QString());
    GridTableColumn* addColumn(int modelColumn, const QString& caption = QString());
    virtual GridNativeDragObject* createNativeDragDropObject(const QPoint& pos);
    GridTableViewOptions& options() const;
    virtual void navigateUp(Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    virtual void navigateDown(Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    virtual void navigateLeft(Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    virtual void navigateRight(Qt::KeyboardModifiers modifiers = Qt::NoModifier);

    virtual void scrollToColumn(int columnIndex, bool firstPosition = true);
public:
    GridTableView(Grid* grid);
    virtual ~GridTableView();
protected:
    virtual void initialize();
    virtual GridTableColumnBase* getPreviousResizeColumn(const GridTableColumnBase* columnBase) const;
    virtual GridTableColumnBase* getNextResizeColumn(const GridTableColumnBase* columnBase) const;
    virtual bool isVerticalResizable(const GridTableColumnBase* columnBase) const;
    virtual bool isHorizontalResizable(const GridTableColumnBase* columnBase) const;
    virtual ColumnFixedKind getColumnFixedKind(const GridTableColumnBase* columnBase) const;
    virtual void setColumnFixedKind(GridTableColumnBase* columnBase, ColumnFixedKind kind);
    virtual GridColumnBase* createColumn(int modelColumn, const QString& caption);
    virtual int getMaximumVisualIndex(const GridTableColumnBase* anchor) const;
    virtual int getColumnViewWidth(const GridTableColumnBase* column);
    virtual GridViewOptions* createOptions();
    virtual GridViewPrimitiveBase* createLayout();
    virtual GridTableHeaderGraphic* createHeaderPanel(GraphicPrimitive* parent);
    virtual GridRowsPrimitiveAbstract* createRowsGraphic(GraphicPrimitive* parent);
    virtual GridFooterSummaryPrimitive* createFooterSummaryPanel(GraphicPrimitive* parent);
    virtual void getFixedColumns(GridTableColumnBase*& left, GridTableColumnBase*& right) const;
    virtual bool checkHorSizing(const GridColumnBase* columnBase) const;

    bool isColumnOnScreen(GridTableColumnBase* column) const;
    virtual void bestFitForView();
    GridTableViewPrimitive* layout() const;
    virtual bool printHeader(QPainter& painter, int& pageLength, int& itemIndex);
private:
    void collectColumsForSelect(int leftXPos, int rightXPos, QList<GridDataTableColumn *>& columns);
    virtual void showColumnOnScreen(int oldColumnIndex, int columnIndex);
};

class QTITAN_EXPORT GridTableColumnBase: public GridColumnBase
{
    Q_OBJECT
public:
    friend class GridViewBase;
    friend class GridTableView;
    friend class GridBandedTableView;
    friend class GridTableColumnSizingObject;
    friend class GridTableDragColumnObject;
    //friend class GridTableSelectionObject;
    friend class GridTableViewGraphic;
    friend class GridTableColumnPrimitive;
    friend class GridCardColumnPrimitive;
    friend class GridTableHeaderGraphic;
    friend class GridBandedTableHeaderGraphic;
    friend class GridTabColumnPrimitive;
    friend class GridTableViewFixedDecorGraphic;
    friend class GridDataRowPrimitive;
    virtual int visualIndex() const;
    virtual void setVisualIndex(int visualIndex);
    virtual bool isShowingFirst() const;
    virtual bool isShowingLast() const;
    ColumnFixedKind fixedKind() const;
    void setFixedKind(ColumnFixedKind kind);
    bool isHidingEnabled() const;
    void setHidingEnabled(bool enabled);
    bool isMovingEnabled() const;
    void setMovingEnabled(bool enabled);
    bool isHorSizingEnabled() const;
    void setHorSizingEnabled(bool enabled);
    int width() const;
    void setWidth(int width);
    int minWidth() const;
    void setMinWidth(int min);
    int maxWidth() const;
    void setMaxWidth(int max);
    bool isMenuButtonVisible() const;
    void setMenuButtonVisible(bool visible);
protected:
    GridTableColumnBase(GridViewBase* view, const QString& caption);
    virtual ~GridTableColumnBase();    
    ColumnFixedKind m_fixedKind;
    int exactWidth();
    void setExactWidth(int width);
    int exactHeight();
    void setExactHeight(int height);
private:
    bool m_isHidingEnabled;
    bool m_isMovingEnabled;
    bool m_isHorSizingEnabled;
    double m_exactWidth, m_exactHeight; //In Pixels, values without group indent and zoomfactor.
    int m_minWidth; //In Pixels, without zoom factor
    int m_maxWidth; //In Pixels, without zoom factor
    bool m_menuButtonVisible;
};

class GridColumnEditorGraphic: public EmbedGraphicPrimitive
{
public:
    friend class GridColumnPrimitive;
    GridColumnEditorGraphic(GridColumnPrimitive* parent, GridColumnBase* column);
    virtual ~GridColumnEditorGraphic();
protected:
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
private:
    GridColumnBase* m_column;
};

enum CellButtonKind
{
    ChoiceButton,
    SelectButton,
    ClearButton,
    TextButton,
    IconButton
};
    
enum CellButtonPosition
{
    LeftPosition,
    RightPosition
};

enum CellButtonActivatePolicy
{
    FixedPolicy,
    FluentFixedPolicy,
    MouseOverPolicy,
    FluentMouseOverPolicy,
    FocusPolicy,
    FluentFocusPolicy
};

class GridCellButtonInfo
{
public:
    GridCellButtonInfo(GridDataTableColumn* column,
        CellButtonKind k, CellButtonPosition pos, CellButtonActivatePolicy p, const QIcon& i, int w, const QString& title)
        : dataColumn(column), kind(k), position(pos), policy(p), icon(i), titleText(title), buttonWidth(w)
    {
    }
    GridDataTableColumn* dataColumn;
    CellButtonKind kind;
    CellButtonPosition position;
    CellButtonActivatePolicy policy;
    QIcon icon;
    int getButtonWidth();
    QString titleText;
private:
    const int buttonWidth;
};

class QTITAN_EXPORT GridDataTableColumn : public GridTableColumnBase
{
    Q_OBJECT
public:
    friend class GridViewBase;
    friend class GridDataRowPrimitive;
    friend class GridTableColumnFilterButton;
    friend class GridRowLayoutItem;
    friend class GridTableCellGraphic;
    friend class GridFilterColumnPopup;
    friend class GridTableColumnPrimitive;

    GridModelDataBinding* dataBinding() const;
    GridEditorRepository* editorRepository();
    void setEditorRepository(GridEditorRepository* repository);
    GridEditorType editorType() const;
    void setEditorType(GridEditorType type);
    SortOrder sortOrder() const;
    void setSortOrder(SortOrder order);
    bool isSortLocaleAware() const;
    void setSortLocaleAware(bool value);
    bool sortCaseSensitivity() const;
    void setSortCaseSensitivity(bool value);
    bool isSortEnabled() const;
    void setSortEnabled(bool enabled);
    bool isGroupSortEnabled() const;
    void setGroupSortEnabled(bool enabled);
    bool isGroupingEnabled() const;
    void setGroupingEnabled(bool enabled);
    bool isHidingOnGroupingEnabled() const;
    void setHidingOnGroupingEnabled(bool enabled);
    int groupIndex() const;
    void setGroupIndex(int groupIndex);
    bool isFilterButtonVisible() const;
    void setFilterButtonVisible(bool visible);
    void setFooterSummary(GridSummaryType type);
    void setGroupSummary(GridTableColumn* groupColumn, GridSummaryType type);
    void setDefaultGroupSummary(GridSummaryType type);
    int addButton(CellButtonKind kind = SelectButton,
        CellButtonPosition position = RightPosition,
        CellButtonActivatePolicy policy = FixedPolicy,
        const QIcon& icon = QIcon(),
        const QString& title = QString(),
        int buttonWidth = -1);
    bool removeButton(int index);
    int buttonCount() const;
    int rowSpan() const;
    void setRowSpan(int span);
Q_SIGNALS:
    void buttonClicked(CellButtonClickEventArgs* args);
    void buttonStateChanged(CellButtonEventArgs* args);
protected:
    GridDataTableColumn(GridViewBase* view, int modelColumn, const QString& caption);
    virtual ~GridDataTableColumn();
    int sortIndex() const;
    void setSortIndex(int sortIndex);
    virtual void click(QMouseEvent* e);
    virtual void initStyleOption(QStyleOptionHeader& styleOption);
    virtual void setActionIcons();
private:
    GridModelDataBinding* m_dataBinding;
    GridEditorRepository* m_editorRepository;
    GridEditorType m_editorType;
    bool m_filterEnabled;
    GridFilterValueCondition* m_filterCondition;
    GridFilterColumnPopup* m_filterPopup;
    QList<GridCellButtonInfo *> m_buttons;
    bool m_isSortLocaleAware;
    bool m_sortCaseSensitivity;
    bool m_isSortEnabled;
    bool m_isGroupSortEnabled;
    bool m_isGroupingEnabled;
    bool m_isHidingOnGroupingEnabled;
    int m_rowSpan;
};

class QTITAN_EXPORT GridTableColumn : public GridDataTableColumn
{
    Q_OBJECT
public:
    friend class GridTableDragColumnObject;
    friend class GridViewBase;
    friend class GridTableView;
    friend class GridTableRowLayoutItem;
    friend class GridTableSummaryLayoutEngine;
    friend class GridTableRowsLayoutEngine;
    friend class FilterPopupWidget;
    friend class GridTableCellButtonGraphic;
    friend class GridBandedTableView;
    GridTableView* view() const;
public Q_SLOTS:
    void bestFit();
private Q_SLOTS:
    void fixedLeftChecked(bool checked);
    void fixedRightChecked(bool checked);
protected:
    GridTableColumn(GridTableView* view, int modelColumn, const QString& caption);
    virtual ~GridTableColumn();
    virtual GridTableDragColumnObject* createDragDropColumnObject(const QPoint& point, GraphicPrimitive* graphic);
    virtual bool needHeaderHints() const;
    virtual void setActionIcons();
    GridTableColumnPrimitive* columnGP() const;
private:
    QAction* m_fixedLeftAction;
    QAction* m_fixedRightAction;
};

class GridTableColumnSizingObject: public GridDragDropObject
{
public:
    GridTableColumnSizingObject(GridTableColumnPrimitive* columnGP, Qt::Orientation orient);
protected:
    virtual void beginEvent();
    virtual void moveEvent(const QPoint& pos);
    virtual void endEvent();
    virtual void paintEvent(QPainter& painter);
    virtual void acceptEvent();
    virtual bool isAccepted() const;
private:
    QRect calculatePaintRect();
    QRect m_paintRect;
    QPoint m_rightTop;
    QPoint m_screenPos;
    GridTableColumnPrimitive* m_columnGP;
    int m_deltaX;
    int m_deltaY;
    Qt::Orientation m_orient;
};
    
class GridTableDragColumnObject: public GridDragDropObject
{
    Q_OBJECT
public:
    GridTableDragColumnObject(const QPoint& point, GridTableColumnBase* column, GraphicPrimitive* source);
    virtual ~GridTableDragColumnObject();
    void setSourceGroupIndex(int groupIndex);
    int sourceGroupIndex() const;
    void setSourceVisualIndex(int visualIndex);
    int sourceVisualIndex() const;
    bool hideColumnFlag() const;
    void setHideColumnFlag(bool flag);
    virtual bool checkDropColumnTarget(const QPoint& pos, GridColumnPrimitive* target);
    virtual bool checkDropHeaderTarget(const QPoint& pos, GridTableGroupPanelPrimitive* target, GridDataTableColumn* tableColumn);
    virtual bool checkDropFieldChooserTarget(const QPoint& pos, GridFieldChooserPanel* target, GridDataTableColumn* tableColumn);
    virtual void clear();
    DragColumnWidget* dragColumnWidget() const;
protected:
    virtual void beginEvent();
    virtual void moveEvent(const QPoint& pos);
    virtual void acceptEvent();
    virtual void endEvent();
    virtual bool columnsHasVerticalLayout() const;
    bool updateFieldChooser(GraphicPrimitive* overGP);
protected:
    GridViewBase* m_view;
    GridTableColumnBase* m_column;
private:
    QPoint m_startPos;
    DragColumnWidget* m_dragColumnWidget;
    QTimer* m_scrollTimer;
    QCursor m_removeCursor;
    QCursor m_scrLeftCursor;
    QCursor m_scrRightCursor;
    QCursor m_scrTopCursor;
    QCursor m_scrBottomCursor;
    int m_sourceGroupIndex;
    int m_sourceVisualIndex;
    bool m_hideColumnFlag;
    bool m_columnsHasVerticalLayout;
protected:
    void showMoveColumnArrows(GraphicPrimitive* columnPrimitive, ColumnMovePosition position);
    virtual void checkScrollToCursor(const QPoint& curPos);
    ColumnArrowWidget* m_upArrow;
    ColumnArrowWidget* m_downArrow;
    ColumnArrowWidget* m_leftArrow;
    ColumnArrowWidget* m_rightArrow;
private Q_SLOTS:
    virtual void scrollToCursor();
};

class GridTableSelectionRubberBand: public GraphicPrimitive
{
public:
    GridTableSelectionRubberBand(GraphicPrimitive* parent);
protected:
    virtual void paintEvent(QPainter& painter);
};

class GridViewSelectionObject: public GridDragDropObject
{
public:
    GridViewSelectionObject(GridViewBase* view, const GridCell& cell, const QPoint& cellPos);
    virtual ~GridViewSelectionObject();

    virtual void selectTo(const QPoint& pos) = 0;

    void setRubberBandRect(const QPoint& pos);
    void scrollTo(const QPoint& pos);
protected:
    virtual bool event(QEvent* e);
    virtual void beginEvent();
    virtual void moveEvent(const QPoint& pos);
    virtual void endEvent();
protected:
    GridViewBase* m_view;
    GridTableSelectionRubberBand* m_rubberBandGP;
    GridCell m_startCell;
    GridCell m_endCell;
    QPoint m_cellPos;
    QPoint m_lastPos;
    QBasicTimer m_scrollTimer;
};

class GridTableViewSelectionObject : public GridViewSelectionObject
{
public:
    GridTableViewSelectionObject(GridTableView* view, const GridCell& cell, const QPoint& cellPos);
    virtual void selectTo(const QPoint& pos);
    GridTableView* view() const;
private:
};

class GridCellDragObject: public GridNativeDragObject
{
    Q_OBJECT
public:
    GridCellDragObject(GridViewBase* view, GridTableCellGraphic* gp, const QPoint& cellPos);
    virtual ~GridCellDragObject();
    bool startDrag();
    QModelIndexList draggableIndexes();
    bool checkDragDropEvent(ColumnMovePosition position, bool cell, GraphicPrimitive* gp);
    bool checkRow(ColumnMovePosition position, GridDataRowPrimitive* gp);
    bool checkCell(ColumnMovePosition position, GridTableCellGraphic* gp);
    QModelIndex parentModelIndex();
    void setParentModelIndex(const QModelIndex& index);
    int modelRow();
    void setModelRow(int row);
    int modelColumn();
    void setModelColumn(int column);
protected:
    virtual void beginEvent();
    virtual void moveEvent(const QPoint& pos);
    virtual void acceptEvent();
    virtual void endEvent();
private:
    void completeMoveAction();
    GridViewBase* m_view;
    GridTableCellGraphic* m_cellGP;
    QPoint m_pos;
    QModelIndexList m_indexes;
    ColumnArrowWidget* m_upArrow;
    ColumnArrowWidget* m_downArrow;
    ColumnArrowWidget* m_leftArrow;
    ColumnArrowWidget* m_rightArrow;
    QModelIndex m_parentModelIndex;
    int m_modelRow;
    int m_modelColumn;
    QDrag* m_drag;
    Qt::DropAction m_defaultDropAction;
    Qt::DropActions m_supportedActions;
};

class QTITAN_EXPORT GridTableViewOptions: public GridViewOptions
{
    Q_OBJECT
protected:
    friend class GridTableView;
    GridTableViewOptions(GridTableView* view);
public:
    bool columnAutoWidth() const;
    void setColumnAutoWidth(bool autoWidth);
    bool columnsHeader() const;
    void setColumnsHeader(bool columnsHeader);
    bool columnsQuickMenuVisible() const;
    void setColumnsQuickMenuVisible(bool quickMenuVisible);
    bool columnsQuickCustomization() const;
    void setColumnsQuickCustomization(bool quickCustomization);
    int columnsQuickCustomizationMaxDropDownCount() const;
    void setColumnsQuickCustomizationMaxDropDownCount(int dropDownCount);
    bool rowsQuickSelection() const;
    void setRowsQuickSelection(bool enabled);
    bool frozenPlaceQuickSelection() const;
    void setFrozenPlaceQuickSelection(bool visible);
    bool isRowFrozenButtonVisible() const;
    void setRowFrozenButtonVisible(bool visible);
    bool columnHeaderHints() const;
    void setColumnHeaderHints(bool headerHints);
    const QColor& fixedSeparatorColor() const;
    void setFixedSeparatorColor(const QColor& color);
    int fixedSeparatorWidth() const;
    void setFixedSeparatorWidth(int width);

    bool isColumnHorSizingEnabled() const;
    void setColumnHorSizingEnabled(bool enabled);
private:
    bool m_columnAutoWidth;
    bool m_columnsHeader;
    bool m_columnsQuickMenuVisible;
    bool m_columnsQuickCustomization;
    bool m_rowsQuickSelection;
    bool m_rowFrozenButtonVisible;
    bool m_frozenPlaceQuickSelection;
    bool m_columnHeaderHints;
    int m_columnsDropDownCount;
    QColor m_fixedSeparatorColor;
    int m_fixedSeparatorWidth;
    bool m_isColumnHorSizingEnabled;
};


class GridTableRowLayoutItem : public GridRowLayoutItem
{
public:
    friend class GridTableRowsLayoutEngine;
    friend class GridDataRowPrimitive;
    friend class GridTableGroupSummaryPrimitive;
    friend class GridTableGroupSummaryCellGraphic;
    friend class GridTableQuickRowButtonGraphic;
    friend class GridTableCellGraphic;

    GridTableRowLayoutItem(GridTableRowsLayoutEngine* rowsLayout);
    virtual ~GridTableRowLayoutItem();
    virtual void create();
    GridTableView* view() const;
protected:
    virtual void addColumn(const GridDataTableColumn* column);
    virtual void removeColumn(const GridDataTableColumn* column);

    virtual void updateDataRowLayout(bool visible);
    virtual void updateGroupRowLayout(bool visible);
    virtual void updateGroupSummaryLayout(bool visible);

    virtual void createDataRowGraphic();
    virtual void createGroupGraphic();
    virtual void createSummaryPanelGraphic();
private:
};

class GridTableRowsLayoutEngine : public GridRowsLayoutEngine
{
    Q_OBJECT
public:
    friend class GridTableView;
    friend class GridTableRowLayoutItem;
    friend class GridTableCellGraphic;
    friend class GridTableSummaryLayoutEngine;
    GridTableRowsLayoutEngine(GridTableView* view, GraphicPrimitive* parent);
    virtual ~GridTableRowsLayoutEngine();
    GridTableView* view() const;
    virtual GridRowLayoutItem* createItem();
    void beginUpdate(int columnsLength, int rowsLength, int offsetY, bool forceUpdateLayout);
    void endUpdate();
    void updateCellButtons(const GridDataTableColumn* column);

    void editorChanging(const GridDataTableColumn* column, bool reset);
    GridRow* topRow() const;
    GridRow* bottomRow() const;
    GridRowLayoutItem* getVirtualRowByPosY(int y);
    GridTableCellGraphic* getCellGraphic(GridRow* modelRow, int cellIndex);
private: 
    virtual bool appendNextGridRowRect(GridRow* gridRow, bool summary, GridRow::FrozenPlace frozenPlace, QRect& newRect);
public:
};

class QTITAN_EXPORT GridContextMenu: public QMenu
{
public:
    GridContextMenu(QWidget *parent);
    void show(int gx, int gy, GraphicPrimitive* hit);
    GraphicPrimitive* hit() const;
private:
    GraphicPrimitive* m_hit;
};

class QTITAN_EXPORT GridTableColumnEditor: public QObject
{
public:
    GridTableColumnEditor();
    virtual ~GridTableColumnEditor();
    virtual QWidget* createEditorWidget(GridColumnBase* column) = 0;
};

class GridGraphicSitePublic: public GraphicControl
{
    GridGraphicSitePublic() : GraphicControl(0) {}
    friend class Qtitan::GridTableView;
    //friend class Qtitan::GridTableRowsLayoutEngine;
    friend class Qtitan::GridTableDragColumnObject;
    //friend class Qtitan::GridViewSelectionObject;
    friend class Qtitan::GridColumnPrimitive;
    friend class Qtitan::GridTableCellGraphic;
    friend class Qtitan::GridTableColumnFilterButton;
};

extern void get_cell_border_lines(GridViewBase* view, int& borderRight, int& borderBottom, bool& _2d);
extern int get_group_height(QStyle* style, const GridViewOptions* options);
extern QFont get_group_font(const GridViewOptions* options);
extern int get_cell_width(QStyle* style, const GridViewOptions* options);
extern int get_cell_height(QStyle* style, const GridViewOptions* options);
extern bool is_next_summary(GridRow* modelRow);

QTITAN_END_NAMESPACE

#endif //QTN_GRIDTABLEVIEW_H
