/****************************************************************************
**
** Qtitan Library by Developer Machines (DataGrid component for Qt.C++)
** 
** Copyright (c) 2009-2016 Developer Machines (http://www.devmachines.com)
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

#ifndef QTN_GRID_CARDVIEW_H
#define QTN_GRID_CARDVIEW_H

#include "QtnGrid.h"
#include "QtnGridFilter.h"
#include "QtnGridEditors.h"
#include "QtnGridTableViewBase.h"

QTITAN_BEGIN_NAMESPACE
class GridCardView;
class GridCardRowsLayoutEngine;

class QTITAN_EXPORT GridCardViewOptions : public GridViewOptions
{
    Q_OBJECT
protected:
    friend class GridCardView;
    GridCardViewOptions(GridCardView* view);
public:
    int minimumItemCount() const;
    void setMinimumItemCount(int count);
    int maximumItemCount() const;
    void setMaximumItemCount(int count);
    Qt::Alignment itemAlignment() const;
    void setItemAlignment(Qt::Alignment alignment);
    int itemMargin() const;
    void setItemMargin(int margin);
    int itemPadding() const;
    void setItemPadding(int padding);
    int itemWidth() const;
    void setItemWidth(int width);
    int itemTitleHeight() const;
    void setItemTitleHeight(int height);
    const QString& itemTitleText() const;
    void setItemTitleText(const QString& text);
private:
    int m_minimumItemCount;
    int m_maximumItemCount;
    Qt::Alignment m_itemAlignment;
    int m_itemMargin;
    int m_itemPadding;
    int m_itemWidth;
    int m_itemTitleHeight;
    QString m_itemTitleText;
};

class QTITAN_EXPORT GridCardView : public GridViewBase
{
    Q_OBJECT
public:
    friend class GridCardRowsLayoutEngine;
    friend class GridCardViewPrimitive;
    static GridCardView* create(Grid* grid);
    GridCardView(Grid* grid);
    virtual ~GridCardView();
    virtual GridNativeDragObject* createNativeDragDropObject(const QPoint& pos);
    GridCardViewOptions& options() const;

    virtual void navigateUp(Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    virtual void navigateDown(Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    virtual void navigateLeft(Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    virtual void navigateRight(Qt::KeyboardModifiers modifiers = Qt::NoModifier);
protected:
    virtual void initialize();
    virtual GridViewOptions* createOptions();
    virtual GridViewPrimitiveBase* createLayout();
    virtual GridColumnBase* createColumn(int modelColumn, const QString& caption);
    virtual GridTableHeaderGraphic* createHeaderPanel(GraphicPrimitive* parent);
    virtual GridRowsPrimitiveAbstract* createRowsGraphic(GraphicPrimitive* parent);
    virtual GridFooterSummaryPrimitive* createFooterSummaryPanel(GraphicPrimitive* parent);
    virtual void bestFitForView();
    virtual bool printHeader(QPainter& painter, int& pageLength, int& itemIndex);
    virtual void showColumnOnScreen(int oldColumnIndex, int columnIndex);
};

class GridCardColumn : public GridDataTableColumn
{
    Q_OBJECT
public:
    friend class GridCardView;
protected:
    GridCardColumn(GridCardView* view, int modelColumn, const QString& caption);
    virtual ~GridCardColumn();
    virtual GridTableDragColumnObject* createDragDropColumnObject(const QPoint& point, GraphicPrimitive* graphic);
    virtual bool needHeaderHints() const;
    virtual void initStyleOption(QStyleOptionHeader& styleOption);
private:
};

class GridCardColumnDragObject : public GridTableDragColumnObject
{
public:
    GridCardColumnDragObject(const QPoint& point, GridTableColumnBase* column, GraphicPrimitive* source);
protected:
    virtual bool columnsHasVerticalLayout() const;
    virtual void checkScrollToCursor(const QPoint& curPos);
private:
};

class GridFieldChooserCardColumnProvider : public GridFieldChooserColumnProvider
{
public:
    GridFieldChooserCardColumnProvider(GridViewBase* view);
    virtual GridViewBase::GridColumns fields() const;
};

class GridCardRowsPrimitive : public GridRowsPrimitiveAbstract
{
public:
    GridCardRowsPrimitive(GridViewBase* view, GraphicPrimitive* parent);
    virtual ~GridCardRowsPrimitive();
    virtual void updateLayout();
    virtual void calculateFixedRows();
    virtual int scrollX() const;
    virtual int scrollY() const;
    virtual GridRowsLayoutEngine* createPrinterLayoutEngine() const;
private:
};

class GridCardNewRowPrimitive : public GridNewRowPrimitive
{
public:
    GridCardNewRowPrimitive(GridViewBase* view, GraphicPrimitive* parent);
    virtual QRect getLayoutItemRect();
};

class GridCardRowLayoutItem : public GridRowLayoutItem
{
public:
    GridCardRowLayoutItem(GridCardRowsLayoutEngine* rowsLayout);
    virtual ~GridCardRowLayoutItem();
    virtual void create();
    GridCardView* view() const;
    virtual bool isFirstItem();
    int alignOffset() const;
    int autoRowOffset() const;
    GridCardColumnPrimitive* primitiveForColumn(const GridDataTableColumn* column);
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
    int m_alignOffset;
    int m_autoRowOffset;
};

class GridCardRowsLayoutEngine : public GridRowsLayoutEngine
{
    Q_OBJECT
public:
    GridCardRowsLayoutEngine(GridCardView* view, GraphicPrimitive* parent);
    virtual ~GridCardRowsLayoutEngine();
    GridCardView* view() const;
    virtual void beginUpdate(int columnsLength, int rowsLength, int offsetY, bool forceUpdateLayout);
    virtual void endUpdate();

    virtual void editorChanging(const GridDataTableColumn* column, bool reset);
    virtual void updateCellButtons(const GridDataTableColumn* column);
    virtual GridRowLayoutItem* createItem();
    virtual bool appendNextGridRowRect(GridRow* gridRow, bool summary, GridRow::FrozenPlace frozenPlace, QRect& newRect);
protected:
    void alignItems();
private:
    int m_startNewItemIndex;
    int m_currentMaxRowLength;
};


QTITAN_END_NAMESPACE

#endif //QTN_GRID_CARDVIEW_H