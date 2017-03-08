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

#ifndef QTN_GRIDVIEWGRAPTHICS_H
#define QTN_GRIDVIEWGRAPTHICS_H

#include <QTimer>
#include <QPixmapCache>
#include "QtnGridGraphics.h"
#include "QtnGridTableViewBase.h"
#include "QtnGridCardView.h"

QTITAN_BEGIN_NAMESPACE

class GridViewLayoutItemPrimitive : public GraphicPrimitive
{
public:
    GridViewLayoutItemPrimitive(GraphicPrimitive* parent);
    virtual void updateLayout();
    virtual QSize sizeHint();
    virtual void updateChildren();
};

class GridViewColumnsPrimitive : public GridViewLayoutItemPrimitive
{
public:
    GridViewColumnsPrimitive(GraphicPrimitive* parent);
    virtual QSize sizeHint();
};

class GridViewHeaderPrimitive : public GridViewLayoutItemPrimitive
{
public:
    GridViewHeaderPrimitive(GraphicPrimitive* parent);
    virtual QSize sizeHint();
protected:
    virtual void paintEvent(QPainter& painter);
};

class GridViewFooterPrimitive : public GridViewLayoutItemPrimitive
{
public:
    GridViewFooterPrimitive(GraphicPrimitive* parent);
    virtual QSize sizeHint();
    void mouseOverEvent(const QPoint& pos);
    void mouseOutEvent();
    virtual void resizeEvent(const QRect& rect);
protected:
    virtual void paintEvent(QPainter& painter);
};

class GridViewTopRowsPrimitive : public GridViewLayoutItemPrimitive
{
public:
    GridViewTopRowsPrimitive(GraphicPrimitive* parent);
    virtual QSize sizeHint();
};

class GridViewBottomRowsPrimitive : public GridViewLayoutItemPrimitive
{
public:
    GridViewBottomRowsPrimitive(GraphicPrimitive* parent);
    virtual QSize sizeHint();
};

class GridViewCentralRowsPrimitive : public GridViewLayoutItemPrimitive
{
public:
    GridViewCentralRowsPrimitive(GraphicPrimitive* parent);
    virtual void updateLayout();
    virtual void updateChildren();
protected:
};

class GridViewLayoutPrimitive : public ScrollableGraphicPrimitive
{
    Q_OBJECT
public:
    GridViewLayoutPrimitive(GraphicControl* control);
    virtual void updateLayout();
    virtual void resizeEvent(const QRect& rect);
    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);
    bool isFooterAutoHide() const;
    virtual void setFooterAutoHide(bool autoHide);
    void showFooter();
    void hideFooter();
    GridViewHeaderPrimitive* header() const { return m_headerPrimitive; }
    GridViewFooterPrimitive* footer() const { return m_footerPrimitive; }
    GridViewColumnsPrimitive* columns() const { return m_columnsPrimitive; }
    GridViewCentralRowsPrimitive* central() const { return m_centralRowsPrimitive; }
    GridViewTopRowsPrimitive* topRows() const { return m_topRowsPrimitive;  }
    GridViewBottomRowsPrimitive* bottomRows() const { return m_bottomRowsPrimitive; }
public:
    virtual void paintHeaderBackground(QPainter& painter, const QRect& rect) = 0;
protected:
    virtual void orientationChanged() = 0;
    int m_footerTop;
    int m_footerBottomHeight;
private:
    void updateFooterAutoHideRect(bool repaint);
    GridViewHeaderPrimitive* m_headerPrimitive;
    GridViewColumnsPrimitive* m_columnsPrimitive;
    GridViewFooterPrimitive* m_footerPrimitive;
    GridViewTopRowsPrimitive* m_topRowsPrimitive;
    GridViewBottomRowsPrimitive* m_bottomRowsPrimitive;
    GridViewCentralRowsPrimitive* m_centralRowsPrimitive;
    Qt::Orientation m_orientation;
    bool m_footerAutoHide;
    QTimer* m_footerUpdateTimer;
    bool m_footerShowing;
    QTime m_effectStartTime;
    int m_effectDuration;
private Q_SLOTS:
    void footerUpdateTimeout();
};

class GridViewPrimitiveBase : public GridViewLayoutPrimitive
{
    Q_OBJECT
public:
    GridViewPrimitiveBase(GraphicControl* control);
    virtual ~GridViewPrimitiveBase();
    GridFindPanelPrimitive* findPanel() const;
    GridTableGroupPanelPrimitive* groupingPanel() const;
    GridFilterPanelPrimitive* filterPanel() const;
    GridFieldChooserPrimitive* fieldChooser() const;
    GridRowsPrimitiveAbstract* rowsPanel() const;
    GridFooterSummaryPrimitive* summaryPanel() const;
    GridRowsLayoutEngine* rowsLayoutEngine() const;
    virtual void init(GridViewBase* view);
    virtual void updateLayout();
    virtual void setFooterAutoHide(bool autoHide);
    QRect columnsRect() const;
    QSize centralAreaSize() const;
    QRect rowsScrollAreaRect() const;
    void updateZooming();
    void startZooming();
    QTimer* updateTimer() const { return m_updateTimer; }
    virtual void columnCreated(GridColumnBase* column);
    virtual void columnDestroying(GridColumnBase* column);
    virtual void columnChanged(GridColumnBase* column) = 0;
    virtual void columnEditorChanged(GridColumnBase* column, GridTableColumnEditor* editor) = 0;
    virtual void columnVisibleChanged(GridColumnBase* columnBase, bool visible) = 0;
    virtual void columnSizeChanged(GridColumnBase* column) = 0;

    virtual int getViewWidth() const = 0;
    virtual int getViewHeight() const = 0;
    virtual int getQuickButtonIndent() const = 0;

    virtual int leftFixedIndent() const = 0;
    virtual int rightFixedIndent() const = 0;

    virtual bool compareColumnOrder(GridDataTableColumn* c1, GridDataTableColumn* c2) = 0;
    virtual void calculateGridRowAutoHeight(GridRow* gridRow, int* heights) const = 0;
    virtual bool getAutoHeightRowCount(int& rowCount) const = 0;

    virtual int calculateGridRowLength(GridRow* row, bool groupSummary);
    virtual int calculateGridSummaryRowLength(GridRow* row) = 0;
    virtual int calculateNewRowLength() = 0;
    virtual void setColumnMask(const QRect& maskRect);
    virtual void calcScreenScrollRect(QRect& scrollRect) const;
    virtual int getModelHeight();
    int calculateNextPageRowIndex(int rowIndex, bool up, int* offsetPos = Q_NULL);
    int getGridRowAutoHeight(GridRow* gridRow) const;
    void resetAutoHeightMode();
    int autoHeightRowCount() const;
    const QVector<int>& autoHeightInfo() const;

    void updateRows(bool forceUpdateLayout = false);
    virtual int alignGridRowIndex(int rowIndex, int* itemsCount = Q_NULL);
    virtual bool alternatingRowColors(GridRow* row) const;
    bool iterateToNextLine(int rowIndex, bool back, int& out_nextRowIndex, int& out_rowLength, int& out_itemsCount);
protected:
    virtual void drawBorder(QPainter& painter);
    virtual void updateScrollBars();
    virtual bool checkColumnsScrollBar(int& rangeMin, int& rangeMax, int& page, int& singleStep) = 0;
    virtual bool checkRowsScrollBar(int& rangeMin, int& rangeMax, int& page, int& singleStep);
    virtual int calculateRowIndexByScrollPos(int& offset);
    virtual void calculateModelViewHeight();
    virtual void scrollXEvent(int position);
    virtual void scrollYEvent(int position);
    virtual void orientationChanged();
    virtual void paintHeaderBackground(QPainter& painter, const QRect& rect);
    void setColumnsScrollBarRange(int rangeMin, int rangeMax, int page, int singleStep);
    bool setColumnsScrollBarVisible(bool visible);
    void setRowsScrollBarRange(int rangeMin, int rangeMax, int page, int singleStep);
    bool setRowsScrollBarVisible(bool visible);
    virtual bool checkDragDropEvent(const QPoint& pos, GridDragDropObject* object);
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void showContextMenuEvent(const QPoint& pos, QContextMenuEvent* event);
    virtual void wheelEvent(const QPoint& pos, QWheelEvent* event);
    virtual void paintEvent(QPainter& painter);
protected:
    void setScrollAreaClipping();
    void doScrollRows();

    GridViewBase* m_view;
    GridFindPanelPrimitive* m_findPanel;
    GridTableGroupPanelPrimitive* m_groupingPanel;
    GridRowsPrimitiveAbstract* m_rowsPanel;
    GridFilterPanelPrimitive* m_filterPanel;
    GridFooterSummaryPrimitive* m_summaryPanel;
    GridZoomIndicatorGraphic* m_zoomIndicator;
    QTimer* m_updateTimer;
    int m_modelViewHeight;
    int m_autoHeightColumnRowCount;
    QVector<int> m_autoHeightInfo;
public Q_SLOTS:
    void updateRowsScrollTimeout();
};


class GridTableViewPrimitive : public GridViewPrimitiveBase
{
    Q_OBJECT
public:
    GridTableViewPrimitive(GraphicControl* control);
    virtual ~GridTableViewPrimitive();
    GridTableHeaderGraphic* columnHeader() const;
    virtual void init(GridViewBase* view);
    GridTableView* view() const;
    virtual void updateLayout();

    virtual void columnCreated(GridColumnBase* column);
    virtual void columnDestroying(GridColumnBase* column);

    virtual int getViewWidth() const;
    virtual int getViewHeight() const;
    virtual int getQuickButtonIndent() const;

    virtual int leftFixedIndent() const;
    virtual int rightFixedIndent() const;

    virtual bool compareColumnOrder(GridDataTableColumn* c1, GridDataTableColumn* c2);
    virtual void calculateGridRowAutoHeight(GridRow* gridRow, int* heights) const;
    virtual bool getAutoHeightRowCount(int& rowCount) const;

    virtual int calculateGridRowLength(GridRow* row, bool groupSummary);
    virtual int calculateGridSummaryRowLength(GridRow* row);
    virtual int calculateNewRowLength();
    virtual void setColumnMask(const QRect& maskRect);
    virtual void calcScreenScrollRect(QRect& scrollRect) const;

    virtual void columnChanged(GridColumnBase* column);
    virtual void columnEditorChanged(GridColumnBase* column, GridTableColumnEditor* editor);
    virtual void columnVisibleChanged(GridColumnBase* columnBase, bool visible);
    virtual void columnSizeChanged(GridColumnBase* column);
protected:
    virtual void orientationChanged();
    virtual void updateScrollBars();
    virtual bool checkColumnsScrollBar(int& rangeMin, int& rangeMax, int& page, int& singleStep);
private:
    GridTableHeaderGraphic* m_columnHeader;
    GridTableViewFixedDecorGraphic* m_fixedDecoration;
};

class GridCardViewPrimitive : public GridViewPrimitiveBase
{
    Q_OBJECT
public:
    GridCardViewPrimitive(GraphicControl* control);
    virtual ~GridCardViewPrimitive();
    virtual void init(GridViewBase* view);
    GridCardView* view() const;
    virtual void updateLayout();

    virtual void columnCreated(GridColumnBase* column);
    virtual void columnDestroying(GridColumnBase* column);

    virtual int getViewWidth() const;
    virtual int getViewHeight() const;
    virtual int getQuickButtonIndent() const;

    virtual int leftFixedIndent() const;
    virtual int rightFixedIndent() const;

    virtual bool compareColumnOrder(GridDataTableColumn* c1, GridDataTableColumn* c2);
    virtual void calculateGridRowAutoHeight(GridRow* gridRow, int* heights) const;
    virtual bool getAutoHeightRowCount(int& rowCount) const;

    virtual int calculateGridRowLength(GridRow* row, bool groupSummary);
    virtual int calculateGridSummaryRowLength(GridRow* row);
    virtual int calculateNewRowLength();

    int calculateGridRowSize(GridRow* row) const;
    int getItemsPerLine() const;
    void checkItemsPerLine();

    int calculateGridDataRowWidth(GridRow* row) const;
    int calculateGridDataRowHeight(GridRow* row) const;

    virtual void columnChanged(GridColumnBase* column);
    virtual void columnEditorChanged(GridColumnBase* column, GridTableColumnEditor* editor);
    virtual void columnVisibleChanged(GridColumnBase* column, bool visible);
    virtual void columnSizeChanged(GridColumnBase* column);
    virtual int alignGridRowIndex(int rowIndex, int* itemsCount = Q_NULL);
    virtual bool alternatingRowColors(GridRow* row) const;
    int itemColumnWidth() const;
    int itemCellWidth() const;
    bool getItemBackgroundPixmap(const QRect& rect, const QBrush& brush, QPixmap& pixmap);
    void addItemBackgroundPixmap(const QRect& rect, const QBrush& brush, const QPixmap& pixmap);
    void clearItemBackgroundPixmapCache();
protected:
    virtual void orientationChanged();
    virtual bool checkColumnsScrollBar(int& rangeMin, int& rangeMax, int& page, int& singleStep);
private:
    int m_currentItemsPerLine;
    GridViewBase::GridColumns m_visualColumns;
    int m_visualColumnItemCount;
    int m_itemColumnWidth;
    int m_itemCellWidth;
    QPixmapCache m_itemBgCache;
};

#if 0
class QTITAN_EXPORT GridWindow : public GraphicControl
{
public:
    GridWindow(GridViewBase* view);
    GridViewPrimitive* layout() { return m_viewPrimitive; }
private:
    GridViewPrimitive* m_viewPrimitive;
};
#endif

QTITAN_END_NAMESPACE

#endif //QTN_GRIDVIEWGRAPTHICS_H
