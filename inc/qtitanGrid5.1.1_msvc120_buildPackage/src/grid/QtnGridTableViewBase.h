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

#ifndef QTN_GRIDTABLEHEADERGP_H
#define QTN_GRIDTABLEHEADERGP_H

#include <QCheckBox>
#include <QScrollArea>
#include "QtnGridGraphics.h"
#include "QtnGridModelController.h"
#include "QtnGridTableView.h"
#include "QtnGridBandedTableView.h"

QTITAN_BEGIN_NAMESPACE

class GridTableCellGraphic;
class GridCardColumn;
class GridCardColumnPrimitive;
class GridCardView;
class GridColumnEditorGraphic;
class GridTableColumnQuickButtonPrimitive;
class GridFooterSummaryPrimitive;
class GridTableGroupPanelPrimitive;
class GridSummaryCellPrimitive;
class GridRowSplitterPrimitive;
class FilterHistoryPopupPrimitive;
class GridTableGroupBranchButton;
class GridTablePreviewRowPrimitive;
class GridBandedTableHeaderGraphic;
class GridCardRowLayoutItem;
class GridCheckBoxPrimitive;
class GridFieldChooserPrimitive;
class GridNewRowPrimitive;


class GridRowsPrimitiveAbstract : public ScrollableGraphicPrimitive
{
public:
    GridRowsPrimitiveAbstract(GridViewBase* view, GraphicPrimitive* parent);
    virtual ~GridRowsPrimitiveAbstract();
    GridRowsLayoutEngine* layoutEngine() const;
    virtual void updateLayout();
    virtual void calculateFixedRows();

    int topFixedIndent() const;
    int bottomFixedIndent() const;
    GridNewRowPrimitive* newRowPanel() const;
    void updateFixedLayout();
    virtual GridRowsLayoutEngine* createPrinterLayoutEngine() const = 0;
protected:
    virtual bool checkDragDropEvent(const QPoint& pos, GridDragDropObject* object);
protected:
    GridViewBase* m_view;
    GridRowsLayoutEngine* m_layoutEngine;
    GridNewRowPrimitive* m_newRowPanelGP;
    GridRowSplitterPrimitive* m_newRowSplitter;
    GridRowSplitterPrimitive* m_topFrozenRowsSplitter;
    GridRowSplitterPrimitive* m_bottomFrozenRowsSplitter;
    int m_topFixedIndent;
    int m_bottomFixedIndent;
private:
};

class GridTableRowsPrimitive : public GridRowsPrimitiveAbstract
{
public:
    GridTableRowsPrimitive(GridViewBase* view, GraphicPrimitive* parent);
    virtual ~GridTableRowsPrimitive();
    virtual void updateLayout();
    virtual void calculateFixedRows();
    virtual GridRowsLayoutEngine* createPrinterLayoutEngine() const;
private:
};

class GridCloseButtonPrimitive : public GraphicPrimitive
{
public:
    GridCloseButtonPrimitive(GraphicPrimitive* parent);
    virtual QSize sizeHint();
protected:
    virtual void mouseMoveEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
    virtual void paintEvent(QPainter& painter);
    virtual void clicked() = 0;
private:
};

class GridHeaderColumnTabCloseButtonGraphic : public GridCloseButtonPrimitive
{
public:
    GridHeaderColumnTabCloseButtonGraphic(GraphicPrimitive* parent);
protected:
    virtual void clicked();
private:
};

class GridShadeButtonPrimitive : public GraphicPrimitive
{
    Q_OBJECT
public:
    GridShadeButtonPrimitive(GraphicPrimitive* parent);
    void setText(const QString& text);
    const QString& text() const;
    void setIcon(const QIcon& icon);
    const QIcon& icon() const;
    virtual QSize sizeHint();
    bool autoRise() const;
    void setAutoRaise(bool autoRaise);
protected:
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e);
    virtual void paintEvent(QPainter& painter);
    virtual void doClick(QMouseEvent* e);
    virtual void paintTexture(QPainter& painter, const QColor& accentColor, const QRect& rect);
protected:
    virtual QPalette getButtonPalette() const;
private:
    QString m_text;
    QIcon m_icon;
    bool m_autoRise;
Q_SIGNALS:
    void clicked();
};

class GridTabColumnPrimitiveBase : public GridShadeButtonPrimitive
{
    Q_OBJECT
public:
    GridTabColumnPrimitiveBase(GraphicPrimitive* parent, GridColumnBase* columnBase);
    GridColumnBase* column() const;
    virtual QSize sizeHint();
protected:
    virtual int extraSizeHintIndent(int margin) = 0;
    virtual int extraLeftIndent(int margin) = 0;
    virtual int extraRightIndent(int margin) = 0;

    virtual void doClick(QMouseEvent* e);
    virtual bool checkDragDropEvent(const QPoint& pos, GridDragDropObject* object);
    virtual GridDragDropObject* createDragDropObject(const QPoint& point);
    virtual void paintEvent(QPainter& painter);
    virtual void paintTexture(QPainter& painter, const QColor& accentColor, const QRect& rect);
protected:
    virtual QPalette getButtonPalette() const;
private:
    GridColumnBase* m_column;
};

class GridTabColumnPrimitive : public GridTabColumnPrimitiveBase
{
public:
    GridTabColumnPrimitive(GraphicPrimitive* parent, GridColumnBase* columnBase);
    GridHeaderColumnTabCloseButtonGraphic* closeButtonGP();
    virtual void updateLayout();
protected:
    virtual int extraSizeHintIndent(int margin);
    virtual int extraLeftIndent(int margin);
    virtual int extraRightIndent(int margin);
private:
    GridHeaderColumnTabCloseButtonGraphic* m_closeButtonGP;
};

class GridCardColumnTabPrimitive : public GridTabColumnPrimitiveBase
{
    Q_OBJECT
public:
    GridCardColumnTabPrimitive(GraphicPrimitive* parent, GridColumnBase* columnBase);
    void updateChecked();
    virtual void updateLayout();
    void createFilterButton();
protected:
    virtual int extraSizeHintIndent(int margin);
    virtual int extraLeftIndent(int margin);
    virtual int extraRightIndent(int margin);
private Q_SLOTS:
    void checkBoxClicked(bool checked);
private:
    GridTableColumnFilterButton* m_filterButtonGP;
    GridCheckBoxPrimitive* m_checkBoxGP;
};

class GridCardRowRemoveButtonPrimitive : public GridCloseButtonPrimitive
{
public:
    GridCardRowRemoveButtonPrimitive(GraphicPrimitive* parent);
protected:
    virtual void clicked();
private:
};

class GridZoomIndicatorGraphic : public GraphicPrimitive
{
public:
    GridZoomIndicatorGraphic(GraphicPrimitive* parent, GridViewBase* view);
    void start();
protected:
    QFont font() const;
    virtual void paintEvent(QPainter& painter);
private:
    GridViewBase* m_view;
    bool m_active;
};

class GridTableHeaderGraphic : public ScrollableGraphicPrimitive
{
public:
    friend class GridTableViewOptions;
    friend class GridTableRowLayoutItem;
    friend class GridTableColumnPrimitive;
    GridTableHeaderGraphic(GraphicPrimitive* parent, GridViewBase* view);
    virtual ~GridTableHeaderGraphic();
    GridTableView* view() const;
    virtual void addColumn(GridTableColumnBase* column);
    virtual void removeColumn(GridTableColumnBase* column);
    virtual GridTableColumnPrimitive* getPrimitiveByColumn(const GridTableColumnBase* columnBase);

    GridTableColumnQuickButtonPrimitive* quickButton() const;
    int leftFixedIndent() const;
    int rightFixedIndent() const;
    int getCellRowCount() const;
    virtual void updateLayout();
    int calculateColumnWidths(const GridViewBase::GridColumns& cols, int viewWidth, bool calculateFixed);
    void setIgnoreColumn(GridTableColumnBase* column);
    virtual bool isColumnIgnore(GridTableColumnBase* column) const;
    virtual void bestFit();
    virtual void createTable();
    virtual void exactSizeChanged(GridTableColumnBase* column);
    bool checkColumnsScrollBar(int& rangeMin, int& rangeMax, int& page, int& singleStep);
//    int calculateGridRowLength(GridRow* row, bool groupSummary);
    int calculateGridSummaryRowLength(GridRow* row);
    int calculateNewRowLength();
    virtual QSize updateColumnsLayout();
    virtual void updateQuickButtonLayout();
    virtual int scrollX() const;
    virtual int scrollY() const;
    void prepareCellsLayout();
    int checkColumnWidth(GridTableColumnPrimitive* columnGP, int width);
public:
    int calculateNewRowWidthEx();
    int calculateNewRowHeightEx();
    int calculateGridRowWidthEx(GridRow* row);
    int calculateGridRowHeightEx(GridRow* row);

protected:
    int calculateRowWidthTableItem();
    virtual QSize sizeHint();
    virtual int calculateMaxColumnWidthForView(GridTableColumnPrimitive* columnGP, int viewWidth);
    GridTableColumnQuickButtonPrimitive* m_quickColumnButton;
    int m_leftFixedIndent;
    int m_rightOrBottomFixedIndent;
    QSize m_sizeHint;
    QMap<int, int> m_columnWidthTable;
    QMap<int, int> m_columnHeightTable;
    GridTableColumnBase* m_ignoreColumn;
    GridViewBase::GridColumns m_visibleColumns;

private:
    GridTableColumnPrimitive* createColumnGP2(GridTableColumnBase* columnBase);
    GridTableView* m_view;
    QHash<const GridTableColumn *, GridTableColumnPrimitive *> m_columnGPsHash;
};

class GridNewRowPrimitive : public GraphicPrimitive
{
public:
    GridNewRowPrimitive(GridViewBase* view, GraphicPrimitive* parent);
    QFont getTitleFont();
    virtual QRect getLayoutItemRect();
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
protected:
    GridViewBase* m_view;
};

class GridRowSplitterPrimitive: public GraphicPrimitive
{
public:
    GridRowSplitterPrimitive(GridViewBase* view, GraphicPrimitive* parent);
protected:
    virtual void paintEvent(QPainter& painter);
private:
    GridViewBase* m_view;
};

class GridTableQuickRowFrozenButtonGraphic : public GraphicPrimitive
{
public:
    GridTableQuickRowFrozenButtonGraphic(GraphicPrimitive* parentGraphic, GridRowLayoutItem* rowLayoutItem);
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
private:
    GridRowLayoutItem* m_rowLayoutItem;
};

class GridTableQuickRowButtonGraphic : public GraphicPrimitive
{
public:
    GridTableQuickRowButtonGraphic(GraphicPrimitive* parentGraphic, GridRowLayoutItem* rowLayoutItem);
    virtual void updateLayout();
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
private:
    GridRowLayoutItem* m_rowLayoutItem;
    GridTableQuickRowFrozenButtonGraphic* m_frozenButtonGP;
    GridTableQuickSelectButtonPrimitive* m_quickSelectGP;
};

class GridRowPrimitiveAbstract : public ScrollableGraphicPrimitive
{
public:
    GridRowPrimitiveAbstract(GraphicPrimitive* parent, GridRowLayoutItem* rowLayoutItem);
    virtual ~GridRowPrimitiveAbstract();
    inline GridRowLayoutItem* rowLayoutItem() const { return m_rowLayoutItem; }
    inline GridViewBase* view() const { return m_rowLayoutItem->viewBase(); }
    
    virtual void updateBorder();
    virtual QPalette initPalette() = 0;
    virtual QRect paintContentRect(bool withBorder) const;

    const QBrush& bgBrush() const { return m_bgbrush; }
    const QBrush& fgBrush() const { return m_fgbrush; }
    const QBrush& selected_bgBrush() const { return m_selected_bgbrush; }
    const QBrush& selected_fgBrush() const { return m_selected_fgbrush; }
protected:
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void paintEvent(QPainter& painter);
    virtual void drawBorder(QPainter& painter);
    void paintSrollableBorder(QPainter& painter);
    void paintGroupIndent(QPainter& painter);
    void paintSelectedRect(QPainter& painter, const QPalette& palette, const QRect& rect, bool mouseOverFlag);
    QBrush m_bgbrush;
    QBrush m_fgbrush;
    QBrush m_selected_bgbrush;
    QBrush m_selected_fgbrush;
private:
    GridRowLayoutItem* m_rowLayoutItem;
};

class GridDataRowPrimitive : public GridRowPrimitiveAbstract
{
    friend class GridRowsLayoutEngine;
    friend class GridRowLayoutItem;
    friend class GridTableView;
    friend class GridTableCellPrimitiveAbstract;
public:
    GridDataRowPrimitive(GraphicPrimitive* parent, GridRowLayoutItem* rowLayoutItem);
    virtual ~GridDataRowPrimitive();
    void updateCellsData();
    void destroyCells();
    void registrColumnGraphic(const GridColumnPrimitive* columnPrimitive);
    void unregistrColumnGraphic(const GridColumnPrimitive* columnPrimitive);
    void editorChanging(const GridColumnPrimitive* columnPrimitive, bool reset);
    virtual QPalette initPalette();
    virtual void updateBorder();
    const QHash<const GridColumnPrimitive *, GridTableCellGraphic *>& dataCells() const;
    GridTablePreviewRowPrimitive* previewGP() const;
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void paintBackground(QPainter& painter, const QRect& rect, const QPalette& palette, bool selected);
    virtual bool checkDragDropEvent(const QPoint& pos, GridDragDropObject* object);
protected:
    QHash<const GridColumnPrimitive *, GridTableCellGraphic *> m_dataCells;
    GridTablePreviewRowPrimitive* m_previewGP;
};

class GridTableDataRowPrimitive : public GridDataRowPrimitive
{
public:
    GridTableDataRowPrimitive(GraphicPrimitive* parent, GridTableRowLayoutItem* row);
    virtual int scrollX() const;
    virtual int scrollY() const;
protected:
    virtual void paintEvent(QPainter& painter);
private:
};

class GridCardDataRowPrimitive : public GridDataRowPrimitive
{
public:
    friend class GridCardRowLayoutItem;
    friend class GridCardRowsLayoutEngine;
    friend class GridCardColumnPrimitive;
    friend class GridCardViewPrimitive;
    GridCardDataRowPrimitive(GraphicPrimitive* parent, GridCardRowLayoutItem* row);
    virtual ~GridCardDataRowPrimitive();
    virtual void updateBorder();
    virtual QRect paintContentRect(bool withBorder) const;
    virtual QPalette initPalette();
    GridCardView* view() const;
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void paintBackground(QPainter& painter, const QRect& rect, const QPalette& palette, bool selected);
private:
    QHash<const GridDataTableColumn *, GridCardColumnPrimitive *> m_columnGPs;
    GridCardRowRemoveButtonPrimitive* m_removeButtonPrimitive;
};

class GridGroupRowPrimitive : public GridRowPrimitiveAbstract
{
    friend class GridRowsLayoutEngine;
    friend class GridTableRowLayoutItem;
    friend class GridTableView;
    friend class GridTableCellGraphic;
public:
    GridGroupRowPrimitive(GraphicPrimitive* parent, GridRowLayoutItem* rowLayoutItem);
    virtual ~GridGroupRowPrimitive();
    virtual QPalette initPalette();
    GridTableGroupBranchButton* branchButon() const;
protected:
    virtual void mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseMoveEvent(const QPoint& point, QMouseEvent* e);
    virtual void paintEvent(QPainter& painter);
    virtual GridDragDropObject* createDragDropObject(const QPoint& pos);
private:
    GridTableGroupBranchButton* m_branchButton;
};

class GridTableGroupRowPrimitive : public GridGroupRowPrimitive
{
public:
    GridTableGroupRowPrimitive(GraphicPrimitive* parent, GridTableRowLayoutItem* row);
    virtual int scrollX() const;
    virtual int scrollY() const;
protected:
    virtual void paintEvent(QPainter& painter);
private:
};

class GridCardGroupRowPrimitive : public GridGroupRowPrimitive
{
public:
    GridCardGroupRowPrimitive(GraphicPrimitive* parent, GridCardRowLayoutItem* row);
protected:
    virtual void paintEvent(QPainter& painter);
private:
};

class GridTableCellPrimitiveAbstract : public EmbedGraphicPrimitive
{
    Q_OBJECT
public:
    GridTableCellPrimitiveAbstract(GridViewBase* view, GraphicPrimitive* parent,
        GridRowLayoutItem* rowLayoutItem, const GridDataTableColumn* column);
    GridRow* gridRow() const;
    GridRowLayoutItem* rowLayoutItem() const { return m_rowLayoutItem; }
    void setVirtualRow(GridRowLayoutItem* rowLayoutItem) { m_rowLayoutItem = rowLayoutItem; }
    virtual void loadData() = 0;
    virtual void initEditorOption(GridEditorStyleOption& option) const = 0;
    void setDirtyData(bool flag);
    bool isDirtyData() const { return m_dirtyData; }
    int columnIndex() const;
    bool setNewPressedCellPrimitive(QMouseEvent* e, QPoint& newPos);
    GridViewBase* view() const;
    GridDataTableColumn* column() const;
protected:
    virtual void handler_mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void handler_mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e);
    virtual void handler_mouseReleaseEvent(const QPoint& point, QMouseEvent* e);

    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
    virtual void paintEvent(QPainter& painter);
protected:
    virtual void doPaintContent(QPainter& painter, GridEditorStyleOption& option) = 0;
    GridViewBase* m_view;
    const GridDataTableColumn* m_column;
    GridRowLayoutItem* m_rowLayoutItem;
    bool m_dirtyData;
    bool m_effectLoadData;
    bool m_useEffect;
    QRect m_layoutRect;
    int m_leftIndent;
    int m_rightIndent;
};

class CellMouseClickHandler : public QObject
{
public:
    CellMouseClickHandler(GridViewBase* view, GridRow* row, int columnIndex);
    virtual ~CellMouseClickHandler();
    bool mousePress(QMouseEvent* e, bool select);
    bool mouseDblClick(QMouseEvent* e, bool select);
    void mouseRelease(QMouseEvent* e, bool select);
    bool isDblClicked() const;
    bool isPressed() const;
private:
    GridViewBase* m_view;
    GridPersistentRow* m_persistentRow;
    int m_columnIndex;
    bool m_isPressed;
    bool m_isDblClicked;
    bool m_focusByMousePress;
};

class GridTableCellGraphic : public GridTableCellPrimitiveAbstract, public GridEditorSite
{
    Q_OBJECT
public:
    friend class GridDataRowPrimitive;
    friend class GridRowsLayoutEngine;
    friend class GridViewPrimitiveBase;
    friend class GridTableRowLayoutItem;
    friend class GridCardRowLayoutItem;
    friend class GridTableCellButtonGraphic;
    GridTableCellGraphic(GridViewBase* view, GraphicPrimitive* parent,
        GridRowLayoutItem* rowLayoutItem, const GridDataTableColumn* column);
    virtual ~GridTableCellGraphic();
    virtual void loadData();
    virtual void initEditorOption(GridEditorStyleOption& option) const;
    GridEditor* createEditor();
    void resetEditor();
    GridEditor* editor() const;
    void updateFont();
    void createButton(GridCellButtonInfo* info);
    void removeButtons();
    void updateButtonLayout();
    virtual void update(const QRect& rect);

    /* GridEditorSite */
    virtual void invalidate(bool fullyViewUpdate = false);
    virtual GridModelDataBinding* dataBinding() const;
    virtual QModelIndex modelIndex() const;
    virtual QWidget* parent() const;
    virtual QRect geometry() const;
    virtual GridEditorStyleOption editorOption();
    virtual void showHint(const QString& text) const;
    virtual void updateCursor(const QCursor& cursor);  
    virtual void linkActivated(const QString& anchor);
protected:
    virtual void handler_mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void handler_mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e);
    virtual void handler_mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
protected:
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void mouseMoveEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
    virtual GridDragDropObject* createDragDropObject(const QPoint& pos);
    virtual bool checkDragDropEvent(const QPoint& pos, GridDragDropObject* object);
protected:
    virtual void drawBorder(QPainter& painter);
    virtual void doPaintContent(QPainter& painter, GridEditorStyleOption& option);
private:
    GridEditor* m_editor;
    EffectSettings m_effSettings;
    QList<GridTableCellButtonGraphic *> m_buttons;
    QTimer* m_activateButtonsTimer;
    private Q_SLOTS:
    void activateButtonsTimeout();
};

class GridPreviewExpandButtonPrimitive : public GridShadeButtonPrimitive
{
public:
    GridPreviewExpandButtonPrimitive(GraphicPrimitive* parent);
    virtual QSize sizeHint();
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
    virtual QPalette getButtonPalette() const;
    virtual void paintTexture(QPainter& painter, const QColor& accentColor, const QRect& rect);
private:
};

class GridTablePreviewRowPrimitive : public GridTableCellPrimitiveAbstract
{
public:
    GridTablePreviewRowPrimitive(GridViewBase* view, GraphicPrimitive* parent, GridRowLayoutItem* rowLayoutItem);
    virtual ~GridTablePreviewRowPrimitive();
    virtual void loadData();
    virtual void initEditorOption(GridEditorStyleOption& option) const;
    virtual void updateLayout();
protected:
    virtual void doPaintContent(QPainter& painter, GridEditorStyleOption& option);
    QPoint translateMouseToTextDocument(const QPoint& pos);
    virtual void mouseMoveEvent(const QPoint& point, QMouseEvent* e);
protected:
    virtual void handler_mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
private:
    GridPreviewExpandButtonPrimitive* m_expandButton;
    QTextDocument* m_textDocument;
    QRect m_textDocumentRect;
};

class GridSummaryRowPrimitive : public GridRowPrimitiveAbstract
{
    friend class GridRowsLayoutEngine;
    friend class GridTableRowLayoutItem;
    friend class GridTableView;
public:
    GridSummaryRowPrimitive(GraphicPrimitive* parent, GridRowLayoutItem* rowLayoutItem);
    virtual ~GridSummaryRowPrimitive();
    virtual QPalette initPalette();
 protected:
     virtual void paintEvent(QPainter& painter);
 private:
};

class GridTableSummaryRowPrimitive : public GridSummaryRowPrimitive
{
public:
    GridTableSummaryRowPrimitive(GraphicPrimitive* parent, GridTableRowLayoutItem* row);
    virtual int scrollX() const;
    virtual int scrollY() const;
protected:
    virtual void paintEvent(QPainter& painter);
private:
};

class GridCardSummaryRowPrimitive : public GridSummaryRowPrimitive
{
public:
    GridCardSummaryRowPrimitive(GraphicPrimitive* parent, GridCardRowLayoutItem* row);
    virtual void updateBorder();
protected:
    virtual void paintEvent(QPainter& painter);
private:
};


class GridTableGroupBranchButton : public GraphicPrimitive
{
public:
    friend class GridGroupRowPrimitive;
    GridTableGroupBranchButton(GridGroupRowPrimitive* parent, GridRowLayoutItem* rowLayoutItem);
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
private:
    GridRowLayoutItem* m_rowLayoutItem;
};

/*
class GridTableGroupRowButton : public GraphicPrimitive
{
public:
    friend class GridTableRowLayoutItem;
    friend class GridTableView;
    GridTableGroupRowButton(GridGroupRowPrimitive* parent);
    virtual ~GridTableGroupRowButton();
    GridGroupRowPrimitive* groupRowPrimitive() const;
    //GridTableGroupBranchButton* branchButon() const { return m_branchButon; }
protected:
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseMoveEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void paintEvent(QPainter& painter);
    virtual void resizeEvent(const QRect& rect);
private:
};
*/

class GridSummaryLayoutEngine
{
public:
    GridSummaryLayoutEngine(GridViewBase* view, GridRowLayoutItem* rowLayoutItem = Q_NULL);
    virtual ~GridSummaryLayoutEngine();
    void addColumn(const GridDataTableColumn* column);
    void removeColumn(const GridDataTableColumn* column);
    void destroySummaryCells();
    void updateSummaryCellsData();
    virtual void updateCellsLayout() = 0;
    bool isGroupRow() const;
    int getGroupingIndent();
    int getGroupRowIndent();
    GridViewBase* view() const;
    GridRowLayoutItem* rowLayoutItem() const;
    GraphicPrimitive* parentGP() const;
    void setParentPrimitive(GraphicPrimitive* parent);
    void paintBackgroud(QPainter& painter, const QPalette& palette, const QRect& rect);
    GridSummaryCellPrimitive* createSummaryCell(const GridDataTableColumn* column);
    bool needPaintGroupText() const;
protected:
    QHash<const GridDataTableColumn *, GridSummaryCellPrimitive *> m_summaryCells;
    bool m_paintGroupText;
private:
    GridViewBase* m_view;
    GridRowLayoutItem* m_rowLayoutItem;
    GraphicPrimitive* m_parentGP;
};

class GridFooterSummaryPrimitive : public ScrollableGraphicPrimitive
{
public:
    GridFooterSummaryPrimitive(GraphicPrimitive* parent);
    virtual ~GridFooterSummaryPrimitive();
    GridSummaryLayoutEngine* layoutEngine() const;
    virtual int scrollX() const;
    virtual int scrollY() const;
protected:
    virtual void updateLayout();
    void resizeEvent(const QRect& rect);
    virtual void paintEvent(QPainter& painter);
    virtual QSize sizeHint();
protected:
    GridSummaryLayoutEngine* m_layoutEngine;
};

class GridSummaryCellPrimitive : public GraphicPrimitive
{
public:
    friend class GridTableSummaryLayoutEngine;
    friend class GridCardSummaryLayoutEngine;
    GridSummaryCellPrimitive(GridSummaryLayoutEngine* layoutEngine);
    void setTableColumn(const GridDataTableColumn* column);
    const GridDataTableColumn* tableColumn() const;
    void setDirtyData();
    virtual void setSummaryType(GridSummaryType type) = 0;
    virtual const GridSummaryValue& getValue();
    const QString& getText() const;
    const QColor& getTextColor() const;
protected:
    virtual void mousePressEvent(const QPoint& pos, QMouseEvent* event);
    virtual void mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseMoveEvent(const QPoint& point, QMouseEvent* e);
    virtual void paintEvent(QPainter& painter);
    virtual void showContextMenuEvent(const QPoint& pos, QContextMenuEvent* event);
    virtual void loadData() = 0;
    GridSummaryValue m_value;
    QString m_text;
    QColor m_textColor;
    GridSummaryLayoutEngine* m_layoutEngine;
private:
    const GridDataTableColumn* m_column;
    bool m_dirtyData;
    bool m_hasGroupText;
};

class GridTableGroupSummaryCellGraphic : public GridSummaryCellPrimitive
{
public:
    GridTableGroupSummaryCellGraphic(GridSummaryLayoutEngine* layoutEngine);
    virtual void setSummaryType(GridSummaryType type);
    GridRow* groupRow() const;
protected:
    virtual void loadData();
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
private:
};

class GridTableFooterSummaryCellGraphic : public GridSummaryCellPrimitive
{
public:
    GridTableFooterSummaryCellGraphic(GridSummaryLayoutEngine* layoutEngine, const GridDataTableColumn* column);
    virtual void setSummaryType(GridSummaryType type);
protected:
    virtual void loadData();
private:
};

class GridTableCellButtonGraphic : public GraphicPrimitive
{
public:
    friend class Grid;
    friend class GridTableCellGraphic;
    GridTableCellButtonGraphic(GridTableCellGraphic* cell, GridCellButtonInfo* info);
    GridCellButtonInfo* info() const;
    bool setActivate(bool b, bool immediate);
    bool isActivate() const;
    int buttonWidth() const;
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void mouseMoveEvent(const QPoint& pos, QMouseEvent* event);
    virtual void mousePressEvent(const QPoint& pos, QMouseEvent* event);
    virtual void mouseReleaseEvent(const QPoint& pos, QMouseEvent* event);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
private:
    QIcon getIcon() const;
    GridTableCellGraphic* m_cell;
    GridCellButtonInfo* m_info;
    bool m_activate;
    int m_buttonWidth;
    bool m_enabled;
};


class GridFieldChooserPanel : public ScrollableGraphicPrimitive
{
public:
    friend class GridFieldChooserPrimitive;
    friend class GridTableDragColumnObject;
    GridFieldChooserPanel(GridFieldChooserPrimitive* parent, GridFieldChooserProvider* provider);
    virtual void updateLayout();
    void columnsUpdated();
    QSize updateColumnViewLayout();
    QFont getTitleFont();
    QFont getDescriptionFont();
    GridFieldChooserProvider* provider() const;
    GridFieldChooserPrimitive* fieldChooser() const;
protected:
    virtual void paintEvent(QPainter& painter);
    virtual bool checkDragDropEvent(const QPoint& pos, GridDragDropObject* object);
private:
    QHash<GridColumnBase *, GridTabColumnPrimitiveBase *> m_colGroups;
    GridFieldChooserProvider* m_provider;
    bool m_showEmptyMessage;
};

class GridWinShadowedPrimitive;
class GridShadowPrimitive : public GraphicPrimitive
{
public:
    friend class GridFieldChooserPrimitive;
    GridShadowPrimitive(GridWinShadowedPrimitive* primitive, GraphicPrimitive* parent);
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void drawBorder(QPainter& painter);
private:
    GridWinShadowedPrimitive* m_primitive;
};


class GridWinShadowedPrimitive : public GraphicPrimitive
{
public:
    GridWinShadowedPrimitive(GraphicPrimitive* parent);
    virtual void resizeEvent(const QRect& rect);
    void setWinVisible(bool visible);
    virtual void paintBackground(QPainter& painter) = 0;
protected:
    GridShadowPrimitive* m_shadow;
};

class GridCheckBoxPrimitive : public EmbedGraphicPrimitive
{
public:
    GridCheckBoxPrimitive(GraphicPrimitive* parent, bool nativeStyleUsed = true);
    void setText(const QString& text);
    void setChecked(bool checked);
protected:
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
};

class GridLineEditPrimitive : public EmbedGraphicPrimitive
{
public:
    GridLineEditPrimitive(GraphicPrimitive* parent);
    QString placeholderText() const;
    void setPlaceholderText(const QString& text);
    QString text() const;
    void setText(const QString& text);
protected:
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
};

class GridFieldChooserPrimitive : public GridWinShadowedPrimitive
{
    Q_OBJECT
public:
    GridFieldChooserPrimitive(GridViewBase* view, GraphicPrimitive* parent);
    void columnsUpdated();
    virtual void updateLayout();
    virtual QSize sizeHint();
    void updateSizeHint();
    void setFieldChooserVisible(bool visible);
    bool isFieldChooserVisible() const;
    void showDropTargetArea(bool show);
    virtual void paintBackground(QPainter& painter);
    bool isVisualOrderControlEnabled() const;
    void setVisualOrderControlEnabled(bool enabled);
protected:
    GridViewBase* m_view;
    GridFieldChooserPanel* m_panel0;
    GridFieldChooserPanel* m_panel1;
    QSize m_sizeHint;
    bool m_paintSelected;
    bool m_visualOrderControlEnabled;
};

class GridFieldChooserButtonPrimitive : public GridShadeButtonPrimitive
{
public:
    GridFieldChooserButtonPrimitive(GridViewBase* view, GraphicPrimitive* parent);
    GridFieldChooserPrimitive* fieldChooser();
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
    virtual QPalette getButtonPalette() const;
private:
    GridViewBase* m_view;
    GridFieldChooserPrimitive* m_fieldChooser;
};


class GridTableGroupPanelPrimitive: public GraphicPrimitive
{
public:
    friend class GridViewOptions;
    friend class GridTableDragColumnObject;
    friend class GridTableView;
    friend class GridTableColumn;
    friend class GridViewPrimitiveBase;
    GridViewBase* view() const;
    void insertColumn(int groupIndex, GridColumnBase* columnBase);
    void removeColumnAt(int groupIndex);
    void moveColumn(int oldGroupIndex, int groupIndex);
    GridTabColumnPrimitive* getGraphicPrimitiveForColumn(GridColumnBase* columnBase);
    GridTableGroupPanelPrimitive(GraphicPrimitive* parent, GridViewBase* view);
    GridFieldChooserPrimitive* fieldChooser() const;
protected:
    virtual void paintEvent(QPainter& painter);
    virtual bool startDragEvent(const QPoint& p);
    virtual bool checkDragDropEvent(const QPoint& pos, GridDragDropObject* object);
    virtual void updateLayout();
    virtual QSize sizeHint();
    virtual void resizeEvent(const QRect& rect);
    void updateFieldChooserLayout();
private:
    GridViewBase* m_view;
    QList<GridTabColumnPrimitive *> m_columnsGPs;
    GridFieldChooserButtonPrimitive* m_fieldChooserButton;
    QSize m_sizeHint;
};

class GridFindPanelCloseButtonPrimitive : public GridCloseButtonPrimitive
{
public:
    GridFindPanelCloseButtonPrimitive(GraphicPrimitive* parent, GridViewBase* view);
protected:
    virtual void clicked();
private:
    GridViewBase* m_view;
};

class GridFindPanelPrimitive : public GraphicPrimitive
{
    Q_OBJECT
public:
    friend class FindFilterCondition;
    GridFindPanelPrimitive(GraphicPrimitive* parent, GridViewBase* view);
    virtual ~GridFindPanelPrimitive();
    bool isPanelVisible() const;
    void setPanelVisible(bool visible, bool cancel);
    const QString& selectedText() const;
    Qt::CaseSensitivity selectCaseSensitivity() const;
    bool hasEditFocus() const;
    void clear();
    void updateOptions();
protected:
    virtual void paintEvent(QPainter& painter);
    virtual QSize sizeHint();
    virtual void resizeEvent(const QRect& rect);
    int sizeHintForHeight();
    QVector<GridColumnBase *> createFindColumnList();
    void findNext(const QString& text, bool forward);
    bool getNextCell(int& rowIndex, int& columnIndex, bool forward);
    int getColumnListIndex(int columnIndex);
    bool checkValue(const QVariant& value);
private Q_SLOTS:
    void updateLayoutTimeout();
    void caseSensitiveClicked(bool checked);
    void filterClicked(bool checked);
    void textEdited(const QString& text);
public Q_SLOTS:
    void backClicked();
    void forwardClicked();
private:
    GridViewBase* m_view;
    QString m_selectedText;
    GridLineEditPrimitive* m_textEditPrimitive;
    GridShadeButtonPrimitive* m_backButton;
    GridShadeButtonPrimitive* m_forwardButton;
    GridCheckBoxPrimitive* m_caseSensitivityCheckBox;
    GridCheckBoxPrimitive* m_filterCheckBox;

    GridFindPanelCloseButtonPrimitive* m_closeButtonPrimitive;
    bool m_panelVisible;
    QTimer* m_effectTimer;
    QTime m_effectStartTime;
    int m_effectDuration;
    int m_effectHeight;
    QVector<GridColumnBase *> m_columnList;
    GridPersistentRow m_oldRow;
    Qt::CaseSensitivity m_caseSensitivity;
};

class FindFilterCondition : public GridFilterCondition
{
public:
    FindFilterCondition(GridFilter* filter, GridFindPanelPrimitive* panel);
    virtual bool isTrue(int modelRow);
    virtual GridFilterCondition* clone() const;
    virtual QString createPresentation() const;
    virtual int conditionCount() const;
public:
private:
    GridFindPanelPrimitive* m_panel;
};

class GridFilterPanelPrimitive: public GraphicPrimitive
{
    Q_OBJECT
public:
    GridFilterPanelPrimitive(GraphicPrimitive* parent, GridViewBase* view);
    virtual ~GridFilterPanelPrimitive();
    void setFilterText(const QString& text);
    void setFilterChecked(bool checked);
    void setFilterStatus(int rowCount, int filteredRowCount, int conditionCount);
    void updateAutoHideIcon();
    virtual void updateLayout();
    virtual QSize sizeHint();
    void setHistoryWindowVisible(bool visible);
protected:
    void createPanel();
    virtual void paintEvent(QPainter& painter);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
    void resizeEvent(const QRect& rect);
    public Q_SLOTS:
    void checkFilterActive(bool checked);
    void filterTextClicked();
    void closeFilterClicked();
    void autoHideClicked();
private:
    GridViewBase* m_view;
    GridCheckBoxPrimitive* m_activeCheckBox;
    GridShadeButtonPrimitive* m_textButton;
    GridShadeButtonPrimitive* m_autoHideButton;
    GridShadeButtonPrimitive* m_closeButton;
    QString m_statusText;
    FilterHistoryPopupPrimitive* m_historyPopup;
};

class GridTableColumnFilterButton: public GraphicPrimitive
{
public:
    GridTableColumnFilterButton(GridDataTableColumn* column, GraphicPrimitive* parent);
    GridDataTableColumn* column() const { return m_column; }
    virtual QSize sizeHint();
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
private:
    GridDataTableColumn* m_column;
};

class GridTableColumnMenuButton : public GraphicPrimitive
{
public:
    GridTableColumnMenuButton(GridColumnBase* column, GraphicPrimitive* parent);
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
private:
    GridColumnBase* m_column;
};

class GridColumnPrimitive : public GraphicPrimitive
{
    Q_OBJECT
    friend class Grid;
    friend class GridTabColumnPrimitive;
    friend class DragColumnWidget;
public:
    GridColumnPrimitive(GraphicPrimitive* parent, GridColumnBase* column);
    virtual ~GridColumnPrimitive();
    GridViewBase* view() const;
    GridColumnBase* column() const;
    virtual void bestFit() = 0;
    void updateHTMLDocument();
    QPoint translateMouseToTextDocument(const QPoint& pos);
    QFont getFont() const;
    void createEditorGraphic();
    void deleteEditorGraphic();
    virtual GridColumnEditorGraphic* editorGraphic() const;
    virtual void updateLayout();
    virtual QSize sizeHint();

    virtual ColumnMovePosition getMovePositionByPoint(const QPoint& point, bool canRowChange = true) const;
    static void paint_column_header_background(GridColumnPrimitive* columnPrimitive, QPainter* painter, const QStyleOptionHeader& option, const QColor& decorColor);
    static void paint_column_header_contents(GridColumnPrimitive* columnPrimitive, QPainter* painter, const QStyleOptionHeader& option,
        Qt::TextElideMode textElideMode, IconApperance iconApperance, bool drawEditor, int leftTextIndent, int rightTextIndent, Qt::Orientation textOrientation,
        bool air, double scaleFactor);
    static bool checkFilterButtonVisible(const GridColumnBase* columnBase);
    static bool checkMenuButtonVisible(const GridColumnBase* columnBase);
protected:
    virtual bool isMenuButton() const = 0;
    virtual bool isFilterButton() const = 0;

    void createFilterButton();
    void createMenuButton();

    virtual void paintEvent(QPainter& painter);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseMoveEvent(const QPoint& point, QMouseEvent* e);

    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e);

    virtual void initStyleOption(QStyleOptionHeader& styleOption);
    virtual void doPaintContent(QPainter& painter, QStyleOptionHeader& styleOption);

    virtual int getCellElementsWidth() const;

    virtual GridDragDropObject* createDragDropObject(const QPoint& point);
    virtual bool checkDragDropEvent(const QPoint& pos, GridDragDropObject* object);

    virtual void doColumnClick(GridColumnBase* column, QMouseEvent* e);
protected:
    EffectSettings m_effSettings;
    GridColumnEditorGraphic* m_columnEditorGP;
    GridTableColumnFilterButton* m_filterButtonGP;
    GridTableColumnMenuButton* m_menuGP;
    GridColumnBase* m_column;
    QTextDocument* m_textDocument;
    QRect m_textDocumentRect2;
};

class GridTableColumnPrimitive: public GridColumnPrimitive
{
public:
    friend class GridTableRowLayoutItem;
    GridTableColumnPrimitive(GraphicPrimitive* parent, GridTableColumnBase* column);
    virtual ~GridTableColumnPrimitive();
    GridTableColumnBase* column() const;
    GridTableView* view() const;
    void updateFixed();
    virtual QSize minSize();
    virtual QSize maxSize();
    virtual void bestFit();
    void setLayouterRow(int row);
    int layouterRow() const;
    void setCellLayoutRect(const QRect& rect);
    const QRect& cellLayoutRect() const;
    virtual int getCellElementsWidth() const;
protected:
    virtual bool isMenuButton() const;
    virtual bool isFilterButton() const;

    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseMoveEvent(const QPoint& point, QMouseEvent* e);
    virtual GridDragDropObject* createDragDropObject(const QPoint& point);
    bool getResizeDragDropParams(const QPoint& point, GridTableColumnBase*& column, Qt::Orientation& orient);
private:
    int m_layouterRow;
    QRect m_cellLayoutRect;
public:
};

class GridCardColumnPrimitive : public GridColumnPrimitive
{
public:
    GridCardColumnPrimitive(GraphicPrimitive* parent, GridDataTableColumn* column);
    virtual ~GridCardColumnPrimitive();
    GridCardColumn* column() const;
    GridCardView* view() const;
    virtual void bestFit();
    virtual ColumnMovePosition getMovePositionByPoint(const QPoint& point, bool canRowChange = true) const;
protected:
    virtual bool isMenuButton() const;
    virtual bool isFilterButton() const;

    virtual void paintEvent(QPainter& painter);
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void doColumnClick(GridColumnBase* column, QMouseEvent* e);
};

class GridTableBandPrimitive : public GridTableColumnPrimitive
{
public:
    GridTableBandPrimitive(GraphicPrimitive* parent, GridTableBand* band);
    virtual ~GridTableBandPrimitive();
    virtual QSize minSize();
    virtual QSize maxSize();
    void updateColumnsLayout(QSize& columnHeaderSize, const int bandDepth, int& visualRowIndex);
    virtual void bestFit();
    GridBandedTableView* view() const;
};

class GridTableColumnFrozenPlaceButtonGraphic: public GraphicPrimitive
{
public:
    GridTableColumnFrozenPlaceButtonGraphic(GraphicPrimitive* parentGraphic, GridViewBase* view);
    virtual QSize sizeHint();
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
private:
    GridViewBase* m_view;
};

class GridTableQuickSelectButtonPrimitive: public GraphicPrimitive
{
public:
    GridTableQuickSelectButtonPrimitive(GraphicPrimitive* parent, GridViewBase* view, GridRowLayoutItem* rowLayoutItem);
    virtual QSize sizeHint();
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
private:
    GridViewBase* m_view;
    GridRowLayoutItem* m_rowLayoutItem;
};

class GridTableColumnQuickButtonPrimitive: public GraphicPrimitive
{
public:
    GridTableColumnQuickButtonPrimitive(GridTableHeaderGraphic* parent, GridFieldChooserProvider* provider, bool bands);
    virtual ~GridTableColumnQuickButtonPrimitive();
    void setDropDownCount(int count);
    virtual void updateLayout();
    virtual QSize sizeHint();
    QSize quickRowButtonSizeHint(GridRow* row);
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
private:
    QuickCustomizationPopup* m_popup;
    GridTableColumnFrozenPlaceButtonGraphic* m_frozenPlaceButtonGP;
    GridTableQuickSelectButtonPrimitive* m_quickSelectGP;
    bool m_bands;
};

class GridTableViewFixedDecorGraphic : public GraphicPrimitive
{
public:
    GridTableViewFixedDecorGraphic(GridTableView* view);
protected:
    virtual void paintEvent(QPainter& painter);
private:
    GridTableView* m_view;
};

class QuickCustomizationPopup: public QWidget
{
    Q_OBJECT
public:
    friend class GridTableColumnQuickButtonPrimitive;
    QuickCustomizationPopup(GridFieldChooserProvider* provider);
    GridFieldChooserProvider* provider() const;
protected:
    virtual void showEvent(QShowEvent* event);
    virtual void hideEvent(QHideEvent* event);
    void recreateChecks();
private:
    GridFieldChooserProvider* m_provider;
    QList<QCheckBox *> m_checks;
    QScrollArea* m_scrollArea;
    QWidget* m_area;
    int m_dropDownCount;
private Q_SLOTS:
    void columnStateChanged(int state);
};

class GridBandedTableHeaderGraphic: public GridTableHeaderGraphic
{
public:
    friend class GridBandedTableView;
    friend class GridBandedTableViewOptions;
    friend class GridTableBandPrimitive;
    GridBandedTableHeaderGraphic(GraphicPrimitive* parent, GridViewBase* view);
    virtual ~GridBandedTableHeaderGraphic();
    GridTableColumnQuickButtonPrimitive* quickBandButton() const;
    GridTableBandPrimitive* createBandGP2(GridTableBand* columnBase);
    virtual void addColumn(GridTableColumnBase* column);
    virtual void removeColumn(GridTableColumnBase* column);
    virtual GridTableColumnPrimitive* getPrimitiveByColumn(const GridTableColumnBase* columnBase);

    virtual bool isColumnIgnore(GridTableColumnBase* column) const;
    virtual void bestFit();
    virtual void createTable();
    virtual void exactSizeChanged(GridTableColumnBase* column);
    virtual QSize updateColumnsLayout();
    virtual void updateQuickButtonLayout();
protected:
    void updateBandsLayout(int leftOffset, int topOffset, QSize& bandHeaderSize, QSize& columnHeaderSize);
    void createBandTable();
    virtual int calculateMaxColumnWidthForView(GridTableColumnPrimitive* columnGP, int viewWidth);
private:
    GridTableColumnQuickButtonPrimitive* m_quickBandButton;
    GridBandedTableView* m_bandedView;
    QHash<const GridTableBand *, GridTableBandPrimitive *> m_bandGPsHash;
    QMap<int, QSize> m_bandItemSizes;
};

QRect paint_icon_and_text(QPainter& painter, const QRect& rect, const QPixmap& icon , const QString& text,
    Qt::Alignment textAlignment, Qt::TextElideMode textElideMode = Qt::ElideRight,
    IconApperance iconApperance = Qtitan::IconAlignLeft, Qt::LayoutDirection direction = Qt::LeftToRight, 
    Qt::Orientation orientation = Qt::Horizontal, bool air = false, int margin = kGridDefHeaderWidthIndent, bool* textFullyVisible = Q_NULL, 
    const QString& selectedText = QString(), Qt::CaseSensitivity selectCaseSensitivity = Qt::CaseInsensitive);

QTITAN_END_NAMESPACE

#endif //QTN_GRIDTABLEHEADERGP_H


