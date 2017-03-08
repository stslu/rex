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

#include <QFile>
#include <QApplication>
#include <QBitmap>
#include <QMessageBox>
#include <QStyleOptionFrameV3>
#include <QListView>
#include <QGroupBox>
#include <QToolTip>
#include <QGridLayout>
#include <QMimeData>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QTableView>

#include "QtnGridDef.h"
#include "QtnGridTableView.h"
#include "QtnGridViewGraphics.h"
#include "QtnGridFilterWindow.h"

QTITAN_USE_NAMESPACE


/*!
    \typedef GridTableView::GridTableColumns
    \internal
*/

/*!
    \typedef GridTableView::GridColumnSet
    \internal
*/


/*!
\enum CellButtonActivatePolicy
\inmodule QtitanDataGrid
\relates GridTableColumn
Grid cell can contain, in addition to the cell editor, a set of custom buttons. The buttons are not associated with the editor at all and should be used for programming an additional behavior for cell.
For example reset the cell value, or cause arbitrary dialogue and etc. Enumirator CellButtonActivatePolicy contains a policy is how to show this buttons. 

\value FixedPolicy Button is always displayed.
\value FluentFixedPolicy  Button is always displayed but at first time fluent effect is used.
\value MouseOverPolicy Button instantly displayed when mouse is over the cell.
\value FluentMouseOverPolicy Button instantly displayed when mouse is over the cell and fluent effect is used.
\value FocusPolicy Button is displayed if cell has focus.
\value FluentFocusPolicy Button is displayed if cell has focus and fluent effect is used.
*/

/*!
\enum CellButtonKind
\inmodule QtitanDataGrid
\relates GridDataTableColumn
Enumerator describes the predefined kinds of cell buttons.
\value ChoiceButton Button with predefined choice icon.
\value SelectButton Button with predefined select icon.
\value ClearButton Button with predefined clear icon.
\value TextButton Button with predefined text icon.
\value IconButton Button with user-defined custom icon.
*/

/*!
\enum CellButtonPosition
\inmodule QtitanDataGrid
\relates GridDataTableColumn
Enumerator describes the position of the button inside cell.
\value LeftPosition Button at the left side of the cell.
\value RightPosition Button at the right side of the cell.
*/


QTITAN_BEGIN_NAMESPACE

class GridTableViewEx: public GridTableView
{
    friend class GridTableGroupSummaryGraphic;
    GridTableViewEx() : GridTableView(0) {}
};


class GraphicControlEx: public GraphicControl
{
    friend class GridCellDragObject;
    GraphicControlEx() : GraphicControl(0) {}
};

class GridTableSummaryLayoutEngine : public GridSummaryLayoutEngine
{
public:
    GridTableSummaryLayoutEngine(GridTableView* view, GridRowLayoutItem* row = Q_NULL);
    virtual ~GridTableSummaryLayoutEngine();
    virtual void updateCellsLayout();
};

class GridTableFooterSummaryPrimitive : public GridFooterSummaryPrimitive
{
public:
    GridTableFooterSummaryPrimitive(GraphicPrimitive* parent, GridTableView* view);
    virtual ~GridTableFooterSummaryPrimitive();
};

QTITAN_END_NAMESPACE

class GridColumnBasePublic: public GridColumnBase
{
    GridColumnBasePublic() : GridColumnBase(0, "") {}
    friend class Qtitan::GridTableView;
};

int Qtitan::get_group_height(QStyle* style, const GridViewOptions* options)
{
    QSize s = get_column_sizeHint(Q_NULL, style, get_group_font(options), options->zoomFactor());
    return s.height();
}

QFont Qtitan::get_group_font(const GridViewOptions* options)
{
    if (options->groupSummaryPlace() == GroupSummaryRowPlus)
        return qtn_font_apply_scale(options->groupRowFont(), 0.75);
    return options->groupRowFont();
}

int Qtitan::get_cell_height(QStyle* style, const GridViewOptions* options)
{    
    int base = options->cellHeight();
    if (base == -1)
        base = get_column_sizeHint(Q_NULL, style, options->cellFont(), options->zoomFactor()).height();
    else
        base = qRound(base * options->zoomFactor());
    return base;
}

int Qtitan::get_cell_width(QStyle* style, const GridViewOptions* options)
{
    Q_UNUSED(style);
    int base = options->cellWidth();
    if (base != -1)
        base = qRound(base * options->zoomFactor());
    return base;
}

/* GridTableRowsLayoutEngine */
GridTableRowsLayoutEngine::GridTableRowsLayoutEngine(GridTableView* view, GraphicPrimitive* parent)
    : GridRowsLayoutEngine(view, parent)
{
}

GridTableRowsLayoutEngine::~GridTableRowsLayoutEngine()
{
}

GridTableView* GridTableRowsLayoutEngine::view() const
{
    return static_cast<GridTableView *>(m_view);
}

GridRowLayoutItem* GridTableRowsLayoutEngine::createItem()
{
    return  new GridTableRowLayoutItem(this);
}

/*!
\internal
*/
void GridTableRowsLayoutEngine::updateCellButtons(const GridDataTableColumn* column)
{
    const GridTableColumn* tableColumn = static_cast<const GridTableColumn *>(column);
    GridTableColumnPrimitive* gp = tableColumn->columnGP();
    for (QVector<GridRowLayoutItem *>::iterator it =
        m_virtualRows.begin(); it != m_virtualRows.end(); ++it)
    {
        (*it)->updateCellButtons(gp);
    }
    m_focusedRowLayoutItem->updateCellButtons(gp);
}

void GridTableRowsLayoutEngine::editorChanging(const GridDataTableColumn* column, bool reset)
{
    const GridTableColumn* tableColumn = static_cast<const GridTableColumn *>(column);
    GridTableColumnPrimitive* gp = tableColumn->columnGP();
    for (QVector<GridRowLayoutItem *>::iterator it =
        m_virtualRows.begin(); it != m_virtualRows.end(); ++it)
    {
        (*it)->editorChanging(gp, reset);
    }
    m_focusedRowLayoutItem->editorChanging(gp, reset);
}

bool VirtualRowYPosLessThan(const GridRowLayoutItem* first, const GridRowLayoutItem* second)
{
    QRect r1 = first->mapToSite();
    QRect r2 = second->mapToSite();
    return r1.y() < r2.y();
}

GridRowLayoutItem* GridTableRowsLayoutEngine::getVirtualRowByPosY(int y)
{
    GridRowLayoutItem* retval = Q_NULL;

    QVector<GridRowLayoutItem *> allRows;
    if (m_focusedRowLayoutItem->type() != GridTableRowLayoutItem::Undefined)
        allRows.append(m_focusedRowLayoutItem);

    for (QVector<GridRowLayoutItem *>::iterator it = m_virtualRows.begin();
        it != m_virtualRows.end(); ++it)
    {
        if ((*it)->type() != GridTableRowLayoutItem::Undefined)
            allRows.append(*it);
    }

    qSort(allRows.begin(), allRows.end(), VirtualRowYPosLessThan);
    
    for (QVector<GridRowLayoutItem *>::iterator it = allRows.begin();
        it != allRows.end(); ++it)
    {
        if (retval == Q_NULL)
            retval = *it;
        QRect r = (*it)->mapToSite();
        if (y < r.y())
            break;
        if  (y - r.y() < y - retval->mapToSite().y())
            retval = *it;
    }

    return retval;
}

bool GridTableRowsLayoutEngine::appendNextGridRowRect(GridRow* gridRow, bool summary, GridRow::FrozenPlace frozenPlace, QRect& newRect)
{
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    int rowLength = 0;
    if (summary)
        rowLength = m_view->layout()->calculateGridSummaryRowLength(gridRow);
    else
        rowLength = m_view->layout()->calculateGridRowLength(gridRow, false);


    if (frozenPlace == GridRow::FrozenTop)
    {
        if (verticalLayout)
        {
            m_currentTopFrozenRowRect.setHeight(rowLength);
            newRect = m_currentTopFrozenRowRect;
            m_currentTopFrozenRowRect.translate(0, rowLength);
        }
        else
        {
            m_currentTopFrozenRowRect.setWidth(rowLength);
            newRect = m_currentTopFrozenRowRect;
            m_currentTopFrozenRowRect.translate(rowLength, 0);
        }
    }
    else if (frozenPlace == GridRow::FrozenBottom)
    {
        if (verticalLayout)
        {
            m_currentBottomFrozenRowRect.setHeight(rowLength);
            newRect = m_currentBottomFrozenRowRect;
            m_currentBottomFrozenRowRect.translate(0, rowLength);
        }
        else
        {
            m_currentBottomFrozenRowRect.setWidth(rowLength);
            newRect = m_currentBottomFrozenRowRect;
            m_currentBottomFrozenRowRect.translate(rowLength, 0);
        }
    }
    else
    {
        if (verticalLayout)
        {
            m_currentRowRect.setHeight(rowLength);
            newRect = m_currentRowRect;
            m_currentRowRect.translate(0, rowLength);
        }
        else
        {
            m_currentRowRect.setWidth(rowLength);
            newRect = m_currentRowRect;
            m_currentRowRect.translate(rowLength, 0);
        }
    }

    bool retval = true;

    if (frozenPlace == GridRow::FrozenNone)
    {
        //int bottomOffset = m_viewRowsLength + m_offsetY;
        //if (m_view->optionsref()->scrollRowStyle() == ScrollByItem)
        //    bottomOffset -= rowLength;

        retval = m_viewRowsLength + m_offsetY > 0;
        m_viewRowsLength -= rowLength;
    }

    return retval;
}

void GridTableRowsLayoutEngine::beginUpdate(int columnsLength, int rowsLength, int offsetY, bool forceUpdateLayout)
{
    GridRowsLayoutEngine::beginUpdate(columnsLength, rowsLength, offsetY, forceUpdateLayout);
 
    QRect scrollAreaRect = m_view->layout()->rowsScrollAreaRect();
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;

    if (verticalLayout)
    {
        if (isPrintLayout())
            scrollAreaRect.setTop(0);
        m_currentRowRect = QRect(0, scrollAreaRect.top() - offsetY, columnsLength, 0);
        m_currentTopFrozenRowRect = QRect(0, 0, columnsLength, 0);
        if (!isPrintLayout() && view()->options().newRowPlace() == NewRowTop)
            m_currentTopFrozenRowRect.translate(0, view()->layout()->columnHeader()->calculateNewRowLength() + kGridNewRowIndent);
        m_currentBottomFrozenRowRect = QRect(0, scrollAreaRect.top() + scrollAreaRect.height() + kGridNewRowIndent, columnsLength, 0);
    }
    else
    {
        if (isPrintLayout())
            scrollAreaRect.setLeft(0);
        m_currentRowRect = QRect(scrollAreaRect.left() - offsetY, 0, 0, columnsLength);
        m_currentTopFrozenRowRect = QRect(0, 0, 0, columnsLength);
        if (!isPrintLayout() && view()->options().newRowPlace() == NewRowTop)
            m_currentTopFrozenRowRect.translate(view()->layout()->columnHeader()->calculateNewRowLength() + kGridNewRowIndent, 0);
        m_currentBottomFrozenRowRect = QRect(scrollAreaRect.left() + scrollAreaRect.width() + kGridNewRowIndent, 0, 0, columnsLength);
    }
}

void GridTableRowsLayoutEngine::endUpdate()
{
    GridRowsLayoutEngine::endUpdate();
}

/* GridTableView */
GridTableView* GridTableView::create(Grid* grid)
{
    GridTableView* retval = new GridTableView(grid);
    retval->initialize();
    return retval;
}

GridTableView::GridTableView(Grid* grid)
: GridViewBase(grid)
{
}

void GridTableView::initialize()
{
    m_fieldProviders.push_front(new GridFieldChooserColumnProvider(this));
    GridViewBase::initialize();
}

GridTableView::~GridTableView()
{
    m_viewStates = ViewDestroying;
}

GridTableColumn* GridTableView::addColumn(const QString& modelColumnName, const QString& caption)
{
    return static_cast<GridTableColumn *>(GridViewBase::addColumn(modelColumnName, caption));
}

GridTableColumn* GridTableView::addColumn(int modelColumn, const QString& caption)
{
    return static_cast<GridTableColumn *>(GridViewBase::addColumn(modelColumn, caption));
}

int GridTableView::getMaximumVisualIndex(const GridTableColumnBase* anchor) const
{
    Q_UNUSED(anchor);
    return m_columnVisualIndexies.size() - 1;
}

GridColumnBase* GridTableView::createColumn(int modelColumn, const QString& caption)
{
    return new GridTableColumn(this, modelColumn, caption);
}

class GridModelControllerPublic: public GridModelController
{
    friend class GridTableView;
}; 

#if 0
void GridTableView::controllerChanged(GridControllerChanges changes)
{
//    m_changeFlags |= ControllerChanged;

    GridViewBase::controllerChanged(changes);

    
//    GridRow* fRow = modelController()->focusedRow();
    /*
    if (fRow == 0 && modelController()->getGridRowCount() > 0)
    {
        //TODO try to move this block after layout changed.
        GridTableColumnBase* next = getFirstColumn();
        int cellIndex = -1;
        if (next != Q_NULL)
            cellIndex = next->index();
        setCellFocus(0,  cellIndex); 
    }
    
    if (m_changeFlags & ColumnChanged)
        columnsChanged();
    else
        controllerLayoutChanged();
        */

    //updateFocusedGP();

    /*
    if (fRow != Q_NULL)
        showRowOnScreen(fRow->rowIndex(), fRow->rowIndex(), false);
    else
        showRowOnScreen(-1, -1, false);
        */

 //   if (fRow != Q_NULL && !(m_changeFlags & GroupExpanded) && !(m_changeFlags & GroupCollapsed))
    {
        //selectRange(
        //    fRow->rowIndex(),
        //    fRow->rowIndex(),
        //    ((GridModelControllerPublic *)modelController())->focusedCell(),
        //    ((GridModelControllerPublic *)modelController())->focusedCell());
    }

    //m_changeFlags &= ~ControllerChanged;
    //m_changeFlags &= ~GroupExpanded;
    //m_changeFlags &= ~GroupCollapsed;
}
#endif

GridNativeDragObject* GridTableView::createNativeDragDropObject(const QPoint& pos)
{
    return new GridCellDragObject(this, Q_NULL, pos);
}
GridTableColumnBase* GridTableView::getPreviousResizeColumn(const GridTableColumnBase* column) const
{
    GridTableColumnBase* retval = Q_NULL;
    if (layout()->orientation() == Qt::Vertical)
    {
        retval = static_cast<GridTableColumnBase *>(getPrevious(column));
        if (column != Q_NULL && column->fixedKind() == FixedRight && retval != Q_NULL && retval->fixedKind() != FixedRight)
            return Q_NULL;
    }
    return retval;
}

GridTableColumnBase* GridTableView::getNextResizeColumn(const GridTableColumnBase* column) const
{
    return (GridTableColumnBase *)column;
}

bool GridTableView::isVerticalResizable(const GridTableColumnBase* columnBase) const
{
    Q_UNUSED(columnBase);
    return false;
}

bool GridTableView::isHorizontalResizable(const GridTableColumnBase* columnBase) const
{
    if (!checkHorSizing(columnBase))
        return false;
    if (columnBase->isShowingLast() && layout()->orientation() == Qt::Vertical && options().columnAutoWidth())
        return false;
    return true;
}

ColumnFixedKind GridTableView::getColumnFixedKind(const GridTableColumnBase* columnBase) const
{
    return columnBase->m_fixedKind;
}

void GridTableView::setColumnFixedKind(GridTableColumnBase* columnBase, ColumnFixedKind kind)
{
    if (columnBase->m_fixedKind == kind)
        return;
    GridViewAutoUpdater autoUpdater(this);
    GridTableColumnBase* fixedLeftColumn; 
    GridTableColumnBase* fixedRightColumn;
    getFixedColumns(fixedLeftColumn, fixedRightColumn);
    switch(kind)
    {
    case FixedLeft:
        if (fixedLeftColumn != Q_NULL)
            columnBase->setVisualIndex(fixedLeftColumn->visualIndex() + 1);
        else
            columnBase->setVisualIndex(0);
        break;
    case FixedRight:
        if (fixedRightColumn != Q_NULL)
            columnBase->setVisualIndex(fixedRightColumn->visualIndex());
        else
            columnBase->setVisualIndex(getMaximumVisualIndex(columnBase));
        break;
    default:
        if (columnBase->m_fixedKind == FixedRight)
        {
            if (fixedRightColumn != Q_NULL)
                columnBase->setVisualIndex(fixedRightColumn->visualIndex());
            else
                Q_ASSERT(false);
        }
        if (columnBase->m_fixedKind == FixedLeft)
        {
            if (fixedLeftColumn != Q_NULL)
                columnBase->setVisualIndex(fixedLeftColumn->visualIndex());
            else
                Q_ASSERT(false);
        }
    }
    columnBase->m_fixedKind = kind;
}

GridTableViewOptions& GridTableView::options() const
{
    return *(static_cast<GridTableViewOptions *>(optionsref()));
}

void GridTableView::navigateUp(Qt::KeyboardModifiers modifiers)
{
    if (layout()->orientation() == Qt::Vertical)
        doNavigatePrevRow(modifiers);
    else
        doNavigatePrevColumn(modifiers);
}

void GridTableView::navigateDown(Qt::KeyboardModifiers modifiers)
{
    if (layout()->orientation() == Qt::Vertical)
        doNavigateNextRow(modifiers);
    else
        doNavigateNextColumn(modifiers);
}

void GridTableView::navigateLeft(Qt::KeyboardModifiers modifiers)
{
    if (layout()->orientation() == Qt::Vertical)
        doNavigatePrevColumn(modifiers);
    else
        doNavigatePrevRow(modifiers);
}

void GridTableView::navigateRight(Qt::KeyboardModifiers modifiers)
{
    if (layout()->orientation() == Qt::Vertical)
        doNavigateNextColumn(modifiers);
    else
        doNavigateNextRow(modifiers);
}

void GridTableView::collectColumsForSelect(int leftXPos, int rightXPos, QList<GridDataTableColumn *>& columns)
{
    if (leftXPos > rightXPos)
        exchange_int(leftXPos, rightXPos);
    QRect r = QRect(leftXPos, 0, rightXPos - leftXPos, 100000);

    for (GridViewBase::GridColumns::const_iterator it = m_columnVisualIndexies.constBegin();
        it != m_columnVisualIndexies.constEnd(); ++it)
    {
        if (!isColumnVisible(*it))
            continue;
        GridColumnPrimitive* temp = static_cast<GridTableColumn *>(*it)->columnGP();
        if (r.intersects(temp->rect()))
            columns.push_back(static_cast<GridTableColumn *>(*it));
    }
}

int GridTableView::getColumnViewWidth(const GridTableColumnBase* column)
{
    Q_UNUSED(column);
    if (options().columnAutoWidth())
        return actualWidth();
    else
        return -1;
}

GridViewOptions* GridTableView::createOptions()
{
    return new GridTableViewOptions(this);
}

GridViewPrimitiveBase* GridTableView::createLayout()
{
    return new GridTableViewPrimitive(grid());
}

GridTableHeaderGraphic* GridTableView::createHeaderPanel(GraphicPrimitive* parent)
{
    return new GridTableHeaderGraphic(parent, this);
}

GridRowsPrimitiveAbstract* GridTableView::createRowsGraphic(GraphicPrimitive* parent)
{
    return new GridTableRowsPrimitive(this, parent);
}

GridFooterSummaryPrimitive* GridTableView::createFooterSummaryPanel(GraphicPrimitive* parent)
{
    return new GridTableFooterSummaryPrimitive(parent, this);
}

void GridTableView::getFixedColumns(GridTableColumnBase*& left, 
                                       GridTableColumnBase*& right) const
{
    left = Q_NULL;
    right = Q_NULL;
    for (GridColumns::const_iterator it = m_columnVisualIndexies.constBegin(); 
        it != m_columnVisualIndexies.constEnd(); ++it)
    {
        GridTableColumn* column = static_cast<GridTableColumn *>(*it);
        if (!isColumnVisible(column))
            continue;
        if (column->fixedKind() == FixedLeft)
            left = column;
        if (column->fixedKind() == FixedRight)
        {
            right = column;
            break;
        }
    }
}

bool GridTableView::checkHorSizing(const GridColumnBase* columnBase) const
{
    const GridTableColumn* column = qobject_cast<const GridTableColumn *>(columnBase);
    if (column == Q_NULL)
        return false;
    return options().isColumnHorSizingEnabled() && column->isHorSizingEnabled();
}

void GridTableView::scrollToColumn(int columnIndex, bool leftPosition)
{
    GridTableColumn* column = static_cast<GridTableColumn *>(getColumn(columnIndex));
    
    if (column == Q_NULL)
        return;

    int sliderPos = 0;
    if (layout()->orientation() == Qt::Vertical)
    {
        int leftPos = layout()->columnsRect().left();

        if (leftPosition)
            sliderPos = column->columnGP()->left() - leftPos;
        else
            sliderPos = (column->columnGP()->left() + column->columnGP()->width()) - layout()->boundedRect().width();

        layout()->scrollHor()->setSliderPosition(sliderPos);
    }
    else
    {
        int topPos = layout()->columnsRect().top();
        if (leftPosition)
            sliderPos = column->columnGP()->top() - topPos;
        else
            sliderPos = (column->columnGP()->top() + column->columnGP()->height()) - layout()->boundedRect().height();
        layout()->scrollVer()->setSliderPosition(sliderPos);
    }

}

bool GridTableView::isColumnOnScreen(GridTableColumnBase* column) const
{
    if (column == Q_NULL)
        return false;

    GridTableColumnPrimitive* columnGP = layout()->columnHeader()->getPrimitiveByColumn(column);

    if (layout()->orientation() == Qt::Vertical)
    {
        int leftPos = layout()->columnsRect().left();
        int rightPos = layout()->columnsRect().left() + layout()->columnsRect().width();

        int columnLeftPos = columnGP->left();
        int columnRightPos = columnGP->left() + columnGP->width();

        if (!layout()->isGPFixedX(columnGP))
        {
            columnLeftPos -= layout()->scrollHor()->sliderPosition();
            columnRightPos -= layout()->scrollHor()->sliderPosition();
        }
        return columnLeftPos >= leftPos && columnRightPos <= rightPos;
    }
    else
    {
        int topPos = layout()->columnsRect().top();
        int bottomPos = layout()->columnsRect().top() + layout()->columnsRect().height();

        int columnTopPos = columnGP->top();
        int columnBottomPos = columnGP->top() + columnGP->height();

        if (!layout()->isGPFixedY(columnGP))
        {
            columnTopPos -= layout()->scrollVer()->sliderPosition();
            columnBottomPos -= layout()->scrollVer()->sliderPosition();
        }
        return columnTopPos >= topPos && columnBottomPos <= bottomPos;
    }
}

bool GridTableView::printHeader(QPainter& painter, int& pageLength, int& itemIndex)
{
    bool verticalLayout = layout()->orientation() == Qt::Vertical;
    painter.save();
    if (itemIndex == -1)
    {
        int quickButtonIndent = layout()->getQuickButtonIndent();

        if (layout()->columnHeader()->quickButton()->visibility())
            layout()->columnHeader()->quickButton()->print(painter);

        if (verticalLayout)
        {
            painter.translate(quickButtonIndent, 0);
            pageLength -= quickButtonIndent;
        }
        else
        {
            painter.translate(0, quickButtonIndent);
            pageLength -= quickButtonIndent;

        }
    }
    int origPageLength = pageLength;

    GridTableColumn* column = itemIndex == -1 ? static_cast<GridTableColumn *>(getFirstColumn()) : static_cast<GridTableColumn *>(getColumn(itemIndex));

    bool retval = true;

    while (column != Q_NULL)
    {
        column->columnGP()->print(painter);
        int len = verticalLayout ? column->columnGP()->width() : column->columnGP()->height();

        if (verticalLayout)
            painter.translate(len, 0);
        else
            painter.translate(0, len);

        pageLength -= len;

        if (pageLength < 0)
        {
            pageLength += len;
            retval = false;
            if (origPageLength < len)
                itemIndex = getNextColumn(column)->index();
            else
                itemIndex = column->index();
            break;
        }
        column = static_cast<GridTableColumn *>(getNextColumn(column));
    }
    painter.restore();

    if (verticalLayout)
        painter.translate(0, layout()->getViewHeight());
    else
        painter.translate(layout()->getViewWidth(), 0);

    return retval;
}

void GridTableView::bestFitForView()
{
    for (GridViewBase::GridColumns::const_iterator it = m_columnslist.constBegin(); it != m_columnslist.constEnd(); ++it)
    {
        if (isColumnVisible(*it))
            static_cast<GridTableColumn *>(*it)->bestFit();
    }
}

GridTableViewPrimitive* GridTableView::layout() const
{
    return static_cast<GridTableViewPrimitive *>(GridViewBase::layout());
}

void GridTableView::showColumnOnScreen(int oldColumnIndex, int columnIndex)
{
    GridTableColumn* column = static_cast<GridTableColumn *>(getColumn(columnIndex));
    if (column == Q_NULL || isColumnOnScreen(column))
        return;

    GridTableColumn* oldColumn = static_cast<GridTableColumn *>(getColumn(oldColumnIndex));

    if (layout()->orientation() == Qt::Vertical)
    {
        scrollToColumn(columnIndex, oldColumn == Q_NULL || oldColumn->columnGP()->left() > column->columnGP()->left());
    }
    else
    {
        scrollToColumn(columnIndex, oldColumn == Q_NULL || oldColumn->columnGP()->top() > column->columnGP()->top());
    }
}

/* GridColumnEditorGraphic */
GridColumnEditorGraphic::GridColumnEditorGraphic(GridColumnPrimitive* parent, GridColumnBase* column)
: EmbedGraphicPrimitive(parent->control(), parent), m_column(column)
{
    setZIndex(parent->zIndex() + 1);
    setBorder(1);
    setBorderColor(Qt::white);
}

GridColumnEditorGraphic::~GridColumnEditorGraphic()
{
}

void GridColumnEditorGraphic::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    setFocus();
    EmbedGraphicPrimitive::mousePressEvent(point, e);
}

void GridColumnEditorGraphic::mouseOverEvent(const QPoint& pos)
{
    EmbedGraphicPrimitive::mouseOverEvent(pos);
    setBorderColor(QColor(41, 153, 255));
    update();
}

void GridColumnEditorGraphic::mouseOutEvent()
{
    EmbedGraphicPrimitive::mouseOutEvent();
    setBorderColor(Qt::white);
    update();
}

/*!
\class GridDataTableColumn
GridDataTableColumn represents the base class of the grid column that has a link to the data via dataBinding property. 
\inmodule QtitanDataGrid
*/
GridDataTableColumn::GridDataTableColumn(GridViewBase* view, int modelColumn, const QString& caption)
    : GridTableColumnBase(view, caption), m_dataBinding(Q_NULL), m_editorRepository(Q_NULL), m_editorType(AutoEditorType), m_filterEnabled(true),
    m_filterCondition(Q_NULL), m_filterPopup(Q_NULL),
m_isSortLocaleAware(false), m_sortCaseSensitivity(false),
m_isSortEnabled(true), m_isGroupSortEnabled(true), m_isGroupingEnabled(true),
m_isHidingOnGroupingEnabled(true), m_rowSpan(1)
{
    m_dataBinding = new GridModelDataBinding(view->modelController(), modelColumn);
    viewBase()->m_columnVisualIndexies.push_back(this);
}

GridDataTableColumn::~GridDataTableColumn()
{
    viewBase()->destroyFilterDialog(); //Remove the filter dialog first. So it can holds the column's reference.
    Q_DELETE_AND_NULL(m_editorRepository);

    viewBase()->m_groups.removeOne(this);

    while (m_buttons.size() > 0)
        removeButton(0);

    Q_DELETE_AND_NULL(m_filterCondition);
    Q_DELETE_AND_NULL(m_filterPopup);
    viewBase()->m_columnVisualIndexies.removeAll(this);

    viewBase()->modelController()->removeDataBinding(dataBinding());
    viewBase()->modelController()->setSortOrder(m_dataBinding, -1, SortNone);
    viewBase()->modelController()->setGroupOrder(m_dataBinding, -1, SortNone);
    viewBase()->modelController()->summary()->removeForDataBinding(m_dataBinding);
    Q_DELETE_AND_NULL(m_dataBinding);
}

GridModelDataBinding* GridDataTableColumn::dataBinding() const
{
    return m_dataBinding;
}


GridEditorRepository* GridDataTableColumn::editorRepository()
{
    if (m_editorRepository == Q_NULL)
        setEditorRepository(viewBase()->createEditorRepository(this));
    return m_editorRepository;
}

void GridDataTableColumn::setEditorRepository(GridEditorRepository* repository)
{
    viewBase()->editorChanging(this, true);
    if (m_editorRepository != Q_NULL)
        delete m_editorRepository;
    m_editorRepository = repository;
    viewBase()->modelController()->removeDefaultValues(dataBinding()->column());
    if (m_editorRepository != Q_NULL)
    {
        m_editorRepository->initialize(viewBase(), dataBinding());
    }
}

GridEditorType GridDataTableColumn::editorType() const
{
    return m_editorType;
}

void GridDataTableColumn::setEditorType(GridEditorType type)
{
    m_editorType = type;
    setEditorRepository(Q_NULL);
    editorRepository();
}

SortOrder GridDataTableColumn::sortOrder() const
{
    return viewBase()->getColumnSortOrder(this);
}

void GridDataTableColumn::setSortOrder(SortOrder order)
{
    viewBase()->setColumnSortOrder(this, order, sortCaseSensitivity(), isSortLocaleAware());
}

bool GridDataTableColumn::isSortLocaleAware() const
{
    return m_isSortLocaleAware;
}

void GridDataTableColumn::setSortLocaleAware(bool value)
{
    m_isSortLocaleAware = value;
}

bool GridDataTableColumn::sortCaseSensitivity() const
{
    return m_sortCaseSensitivity;
}

void GridDataTableColumn::setSortCaseSensitivity(bool value)
{
    m_sortCaseSensitivity = value;
}

int GridDataTableColumn::sortIndex() const
{
    return viewBase()->getColumnSortIndex(this);
}

void GridDataTableColumn::setSortIndex(int sortIndex)
{
    viewBase()->setColumnSortIndex(this, sortIndex);
}


void GridDataTableColumn::click(QMouseEvent* e)
{
    GridViewAutoUpdater autoUpdater(viewBase());

    SortOrder order = sortOrder();

    if (e->modifiers() & Qt::ControlModifier)
        order = SortNone;
    else
    {
        if (e->modifiers() ^ Qt::ShiftModifier)
            viewBase()->modelController()->clearSorting();

        if (order == SortNone || order == SortDescending)
            order = SortAscending;
        else
            order = SortDescending;
    }

    if (viewBase()->optionsref()->groupRowStyle() == GridGroupOfficeOutlook)
    {
        if (groupIndex() == -1)
        {
            viewBase()->clearGrouping();
            setGroupIndex(0);
        }
    }

    if (order != SortNone)
    {
        bool grouping = groupIndex() != -1;
        if (!grouping)
        {
            if (!viewBase()->optionsref()->isSortEnabled() || !isSortEnabled())
                return;
        }
        else
        {
            if (!viewBase()->optionsref()->isGroupSortEnabled() || !isGroupSortEnabled())
                return;
        }
    }
    setSortOrder(order);
}

void GridDataTableColumn::initStyleOption(QStyleOptionHeader& styleOption)
{
    styleOption.init(viewBase()->grid());
    styleOption.state = QStyle::State_None | QStyle::State_Active;
    if (viewBase()->grid()->isEnabled())
        styleOption.state |= QStyle::State_Enabled;
    styleOption.orientation = Qt::Horizontal;
    if (styleOption.orientation == Qt::Horizontal)
        styleOption.state |= QStyle::State_Horizontal;

    styleOption.icon = icon();
    styleOption.textAlignment = textAlignment();

    switch (sortOrder())
    {
    case SortAscending:
        styleOption.sortIndicator = QStyleOptionHeader::SortUp;
        break;
    case SortDescending:
        styleOption.sortIndicator = QStyleOptionHeader::SortDown;
        break;
    default:
        styleOption.sortIndicator = QStyleOptionHeader::None;
        break;
    }

    bool first = isShowingFirst();
    bool last = isShowingLast();
    if (first && last)
    {
        styleOption.position = QStyleOptionHeader::OnlyOneSection;
    }
    else if (first)
        styleOption.position = QStyleOptionHeader::Beginning;
    else if (last)
        styleOption.position = QStyleOptionHeader::End;
    else
        styleOption.position = QStyleOptionHeader::Middle;
#ifdef Q_OS_MAC
    styleOption.position = QStyleOptionHeader::Beginning;
#endif

    styleOption.selectedPosition = QStyleOptionHeader::NotAdjacent;

    if (columnEditor() == Q_NULL)
        styleOption.text = caption();

    styleOption.rect = QRect(0, 0, 30, 24);
}

void GridDataTableColumn::setActionIcons()
{
}

bool GridDataTableColumn::isSortEnabled() const
{
    return m_isSortEnabled;
}

void GridDataTableColumn::setSortEnabled(bool enabled)
{
    m_isSortEnabled = enabled;
    if (!m_isSortEnabled)
        setSortOrder(SortNone);
}

bool GridDataTableColumn::isGroupSortEnabled() const
{
    return m_isGroupSortEnabled;
}

void GridDataTableColumn::setGroupSortEnabled(bool enabled)
{
    m_isGroupSortEnabled = enabled;
    if (!m_isGroupSortEnabled)
        setSortOrder(SortNone);
}

bool GridDataTableColumn::isGroupingEnabled() const
{
    return m_isGroupingEnabled;
}

void GridDataTableColumn::setGroupingEnabled(bool enabled)
{
    m_isGroupingEnabled = enabled;
    if (!m_isGroupingEnabled)
        setGroupIndex(-1);
}

bool GridDataTableColumn::isHidingOnGroupingEnabled() const
{
    return m_isHidingOnGroupingEnabled;
}

void GridDataTableColumn::setHidingOnGroupingEnabled(bool enabled)
{
    m_isHidingOnGroupingEnabled = enabled;
}

int GridDataTableColumn::groupIndex() const
{
    return viewBase()->columnGroupIndex(this);
}

void GridDataTableColumn::setGroupIndex(int groupIndex)
{
    viewBase()->setColumnGroupIndex(this, groupIndex, sortCaseSensitivity(), isSortLocaleAware());
}

bool GridDataTableColumn::isFilterButtonVisible() const
{
    return viewBase()->optionsref()->isFilterEnabled() && m_filterEnabled;
}

void GridDataTableColumn::setFilterButtonVisible(bool visible)
{
    m_filterEnabled = visible;
    viewBase()->requestUpdateLayout();
}

void GridDataTableColumn::setFooterSummary(GridSummaryType type)
{
    viewBase()->modelController()->summary()->setFooterSetting(dataBinding(), dataBinding()->summaryRole(), type);
}

void GridDataTableColumn::setGroupSummary(GridTableColumn* groupColumn, GridSummaryType type)
{
    viewBase()->modelController()->summary()->setGroupSetting(groupColumn == Q_NULL ? Q_NULL : groupColumn->dataBinding(),
        dataBinding(), dataBinding()->summaryRole(), type);
}

void GridDataTableColumn::setDefaultGroupSummary(GridSummaryType type)
{
    viewBase()->modelController()->summary()->setDefaultGroupSetting(dataBinding(), dataBinding()->summaryRole(), type);
}

int GridDataTableColumn::addButton(CellButtonKind kind,
    CellButtonPosition position,
    CellButtonActivatePolicy policy,
    const QIcon& icon, const QString& title, int buttonWidth)
{
    GridCellButtonInfo* b = new GridCellButtonInfo(this, kind, position, policy, icon, buttonWidth, title);
    m_buttons.push_back(b);
    viewBase()->layout()->rowsLayoutEngine()->updateCellButtons(this);
    return m_buttons.size() - 1;
}

bool GridDataTableColumn::removeButton(int index)
{
    if (index >= 0 && index < m_buttons.size())
    {
        GridCellButtonInfo* b = m_buttons.takeAt(index);
        delete b;
        viewBase()->layout()->rowsLayoutEngine()->updateCellButtons(this);
        return true;
    }
    else
        return false;
}

int GridDataTableColumn::buttonCount() const
{
    return m_buttons.size();
}

int GridDataTableColumn::rowSpan() const
{
    return m_rowSpan;
}

void GridDataTableColumn::setRowSpan(int span)
{
    Q_ASSERT(span > 0);
    if (span <= 0)
        span = 1;
    GridViewAutoUpdater autoUpdater(viewBase());
    m_rowSpan = span;
    viewBase()->requestUpdateLayout();
}

/*!
\class GridTableColumn
GridTableColumn represents the class of grid column for GridTableView.
\inmodule QtitanDataGrid
*/
GridTableColumn::GridTableColumn(GridTableView* view, int modelColumn, const QString& caption)
    : GridDataTableColumn(view, modelColumn, caption)
{
//    view->m_columnVisualIndexies.push_back(this);
    m_fixedLeftAction = new QAction(view->layout()->orientation() == Qt::Vertical ? Grid::tr_compatible(QtnGridFixedLeftString) : Grid::tr_compatible(QtnGridFixedTopString), this);
    m_fixedLeftAction->setCheckable(true);
    m_fixedRightAction = new QAction(view->layout()->orientation() == Qt::Vertical ? Grid::tr_compatible(QtnGridFixedRightString) : Grid::tr_compatible(QtnGridFixedBottomString), this);
    m_fixedRightAction->setCheckable(true);

    actionGroup().append(m_fixedLeftAction);
    actionGroup().append(m_fixedRightAction);
    connect(m_fixedLeftAction, SIGNAL(triggered(bool)), this, SLOT(fixedLeftChecked(bool)));
    connect(m_fixedRightAction, SIGNAL(triggered(bool)), this, SLOT(fixedRightChecked(bool)));
    setActionIcons();
}

GridTableColumn::~GridTableColumn()
{
}

void GridTableColumn::setActionIcons()
{
    QPixmap icon = view()->grid()->themeManager()->getIcon(GridThemeManager::FixedLeftIcon, QSize(64, 64));
    m_fixedLeftAction->setIcon(icon);
    icon = view()->grid()->themeManager()->getIcon(GridThemeManager::FixedRightIcon, QSize(64, 64));
    m_fixedRightAction->setIcon(icon);
}


GridTableColumnPrimitive* GridTableColumn::columnGP() const
{
    return view()->layout()->columnHeader()->getPrimitiveByColumn(this);
}

GridTableView* GridTableColumn::view() const
{
    return static_cast<GridTableView *>(viewBase());
}

int GridCellButtonInfo::getButtonWidth()
{
    if (buttonWidth != -1)
        return buttonWidth;

    GridViewBase* view = dataColumn->viewBase();
    int ret = view->optionsref()->cellButtonWidth();
    if (ret == -1)
        ret = get_column_sizeHint(Q_NULL, view->grid()->style(), view->optionsref()->cellFont(), 1).height();

    if (!titleText.isEmpty())
    {
        QRect rect = view->grid()->fontMetrics().boundingRect(titleText);
        ret += rect.width() + 3;
    }
    return ret;
}

void GridTableColumn::fixedLeftChecked(bool checked)
{
    m_fixedLeftAction->setChecked(false);
    m_fixedRightAction->setChecked(false);
    if (checked)
    {
        setFixedKind(FixedLeft);
        m_fixedLeftAction->setChecked(true);
    }
    else
        setFixedKind(FixedNone);
}

void GridTableColumn::fixedRightChecked(bool checked)
{
    m_fixedLeftAction->setChecked(false);
    m_fixedRightAction->setChecked(false);
    if (checked)
    {
        setFixedKind(FixedRight);
        m_fixedRightAction->setChecked(true);
    }
    else
        setFixedKind(FixedNone);
}

void GridTableColumn::bestFit()
{
    columnGP()->bestFit();
}

GridTableDragColumnObject* GridTableColumn::createDragDropColumnObject(const QPoint& point, GraphicPrimitive* graphic)
{
    return new GridTableDragColumnObject(point, this, graphic);
}

bool GridTableColumn::needHeaderHints() const
{
    return view()->options().columnHeaderHints();
}

/* GridTableColumnBase */
GridTableColumnBase::GridTableColumnBase(GridViewBase* view,
                                         const QString& caption)
: GridColumnBase(view, caption), m_fixedKind(FixedNone),
    m_isHidingEnabled(true), m_isMovingEnabled(true), m_isHorSizingEnabled(true),
    m_exactWidth(kGridDefColumnWidth), m_exactHeight(kGridDefColumnHeight),
    m_minWidth(kGridMinTableColumnWidth), m_maxWidth(kGridMaxTableColumnWidth),
    m_menuButtonVisible(false)
{
}

GridTableColumnBase::~GridTableColumnBase()
{
}

int GridTableColumnBase::visualIndex() const
{
    Q_ASSERT(viewBase() != Q_NULL);
    return viewBase()->getVisualIndex(this);
}

void GridTableColumnBase::setVisualIndex(int visualIndex)
{
    Q_ASSERT(viewBase() != Q_NULL);
    return viewBase()->setVisualIndex(this, visualIndex);
}

bool GridTableColumnBase::isShowingFirst() const
{
    return viewBase()->getPrevious(this) == Q_NULL;
}

bool GridTableColumnBase::isShowingLast() const
{
    return viewBase()->getNext(this) == Q_NULL;
}
 
int GridTableColumnBase::width() const
{
    return qRound(m_exactWidth);
}

void GridTableColumnBase::setWidth(int width)
{
    if (this->width() != width)// && width >= calculateMinWidth(this) && width <= calculateMaxWidth(this))
    {
        GridViewAutoUpdater autoUpdater(viewBase());

        setExactWidth(qRound(width * viewBase()->optionsref()->zoomFactor()));
        viewBase()->requestUpdateLayout();
        viewBase()->columnWidthChanged(this);
    }
}

int GridTableColumnBase::minWidth() const
{
    return m_minWidth;
}

void GridTableColumnBase::setMinWidth(int min)
{
    m_minWidth = min;
}

int GridTableColumnBase::maxWidth() const
{
    return m_maxWidth;
}

void GridTableColumnBase::setMaxWidth(int max)
{
    m_maxWidth = max;
}

int GridTableColumnBase::exactWidth()
{
    return qRound(m_exactWidth * viewBase()->optionsref()->zoomFactor());
}

void GridTableColumnBase::setExactWidth(int width)
{
    bool changed = qRound(m_exactWidth * viewBase()->optionsref()->zoomFactor()) != width;
    if (changed)
        m_exactWidth = width / viewBase()->optionsref()->zoomFactor();

    viewBase()->layout()->columnSizeChanged(this);
}

int GridTableColumnBase::exactHeight()
{
    return m_exactHeight * viewBase()->optionsref()->zoomFactor();
}

void GridTableColumnBase::setExactHeight(int height)
{
    if (m_exactHeight * viewBase()->optionsref()->zoomFactor() == height)
        return;
    m_exactHeight = height / viewBase()->optionsref()->zoomFactor();
}

/*!
Returns a visibility of culumn header menu button. By default it is not visible.
*/
bool GridTableColumnBase::isMenuButtonVisible() const
{
    return m_menuButtonVisible;
}

/*!
Sets a visibility of culumn header menu button to \a visible. Normally the button is on the top left of column header. By default it is not visible.
*/
void GridTableColumnBase::setMenuButtonVisible(bool visible)
{
    if (m_menuButtonVisible == visible)
        return;
    m_menuButtonVisible = visible;
    viewBase()->requestUpdateLayout();
}

/*!
Returns the fixed kind of column on the view. Column can be fixed on the left side or on the right side. Fixed column can't be scrolled horizontally. 
\sa ColumnFixedKind
*/
ColumnFixedKind GridTableColumnBase::fixedKind() const
{
    GridTableView* tableView = qobject_cast<GridTableView *>(viewBase());
    if (tableView != Q_NULL)
        return tableView->getColumnFixedKind(this);
    return FixedNone;
}

/*!
Sets the fixed \a kind of column on the view.
\sa ColumnFixedKind
*/
void GridTableColumnBase::setFixedKind(ColumnFixedKind kind)
{
    GridTableView* tableView = qobject_cast<GridTableView *>(viewBase());
    if (tableView != Q_NULL)
        return tableView->setColumnFixedKind(this, kind);
}

bool GridTableColumnBase::isHidingEnabled() const
{
    return m_isHidingEnabled;
}

void GridTableColumnBase::setHidingEnabled(bool enabled)
{
    m_isHidingEnabled = enabled;
}

bool GridTableColumnBase::isMovingEnabled() const
{
    return m_isMovingEnabled;
}

void GridTableColumnBase::setMovingEnabled(bool enabled)
{
    m_isMovingEnabled = enabled;
}

bool GridTableColumnBase::isHorSizingEnabled() const
{
    return m_isHorSizingEnabled;
}

void GridTableColumnBase::setHorSizingEnabled(bool enabled)
{
    m_isHorSizingEnabled = enabled;
}

/* GridTableColumnSizingObject */
GridTableColumnSizingObject::GridTableColumnSizingObject(
    GridTableColumnPrimitive* columnGP, Qt::Orientation orient)
    : GridDragDropObject(columnGP), m_columnGP(columnGP), m_deltaX(0), m_deltaY(0), m_orient(orient)
{
    setImmediateStart(true);
    m_rightTop = columnGP->mapToSite(QPoint(columnGP->width() - 1, 0));
    m_screenPos = QCursor::pos();
    if (orient == Qt::Horizontal)
        setAcceptedCursor(QCursor(Qt::SplitHCursor));
    else
        setAcceptedCursor(QCursor(Qt::SplitVCursor));
}

void GridTableColumnSizingObject::beginEvent()
{
    GridDragDropObject::beginEvent();
    source()->control()->update();
}

void GridTableColumnSizingObject::moveEvent(const QPoint& pos)
{
    GridDragDropObject::moveEvent(pos);

    int dx = m_deltaX;
    int dy = m_deltaY;
    if (m_orient == Qt::Horizontal)
    {
        int currentWidth = m_columnGP->column()->exactWidth();
        //if (m_column->view()->hasGroupingIndent(m_column))
        //    changed -= m_column->view()->modelController()->groupingDepth() * kGridGroupRowIndent;

        int newWidth = static_cast<GridTableViewPrimitive *>(m_columnGP->view()->layout())->columnHeader()->checkColumnWidth(
            m_columnGP, currentWidth + QCursor::pos().x() - m_screenPos.x());

        m_deltaX = newWidth - currentWidth;
    }
    else
    {
        m_deltaY = QCursor::pos().y() - m_screenPos.y();
    }
    if (dx != m_deltaX || dy != m_deltaY)
    {
        m_columnGP->view()->layout()->update();;//calculatePaintRect().united(m_paintRect));
    }
}

bool GridTableColumnSizingObject::isAccepted() const
{
    return true;
}

void GridTableColumnSizingObject::acceptEvent()
{
    GridDragDropObject::acceptEvent();

    if (m_orient == Qt::Horizontal)
    {
        if (m_deltaX != 0)
        {
            GridTableHeaderGraphic* header = static_cast<GridTableHeaderGraphic *>(m_columnGP->parentGP());
            header->setIgnoreColumn(m_columnGP->column());
            m_columnGP->column()->setWidth(qRound((m_columnGP->column()->exactWidth() + m_deltaX) / m_columnGP->view()->optionsref()->zoomFactor()));
        } 
        else
            m_columnGP->view()->layout()->update();
    }
    else
    {
        GridTableColumn* tableColumn = qobject_cast<GridTableColumn *>(m_columnGP->column());
        Q_ASSERT(tableColumn != Q_NULL);
        int columnRowSpan = tableColumn->rowSpan();
        int colWidth = m_columnGP->height() / columnRowSpan;
        int rowSpan = m_deltaY / colWidth;
        rowSpan += columnRowSpan;
        if (rowSpan < 1)
            rowSpan = 1;
        tableColumn->setRowSpan(rowSpan);
    }    
}

void GridTableColumnSizingObject::endEvent()
{
    GridDragDropObject::endEvent();
}

QRect GridTableColumnSizingObject::calculatePaintRect()
{
    QPoint parentPos = m_columnGP->view()->layout()->mapToSite(QPoint(0, 0));
    if (m_orient == Qt::Horizontal)
    {
        return QRect(m_rightTop.x() + m_deltaX, parentPos.y(), 1, m_columnGP->view()->layout()->height());
    }
    else
    {
        GridTableColumn* tableColumn = qobject_cast<GridTableColumn *>(m_columnGP->column());
        Q_ASSERT(tableColumn != Q_NULL);
        int top = m_rightTop.y();
        int columnRowSpan = tableColumn->rowSpan();
        int colWidth = m_columnGP->height() / columnRowSpan;
        int rowSpan = m_deltaY / colWidth;
        rowSpan += columnRowSpan;
        if (rowSpan < 1)
            rowSpan = 1;
        top += colWidth * rowSpan;
        return QRect(parentPos.x(), top, tableColumn->view()->layout()->viewRect().width(), 1);
    }
}

void GridTableColumnSizingObject::paintEvent(QPainter& painter)
{
    m_paintRect = calculatePaintRect();
    painter.fillRect(m_paintRect, Qt::Dense4Pattern);
}

/* GridTableDragColumnObject */
GridTableDragColumnObject::GridTableDragColumnObject(const QPoint& point, GridTableColumnBase* column, GraphicPrimitive* source)
    : GridDragDropObject(source), m_view(column->viewBase()), m_column(column), m_startPos(point),
    m_sourceGroupIndex(-1), m_sourceVisualIndex(-1), m_hideColumnFlag(false),
    m_upArrow(Q_NULL), m_downArrow(Q_NULL), m_leftArrow(Q_NULL), m_rightArrow(Q_NULL)
{
    QPixmap cursorPixmap(":res/grid_removecursor.png");
    cursorPixmap.setMask(cursorPixmap.createMaskFromColor(Qt::magenta));
    m_removeCursor = QCursor(cursorPixmap);

    cursorPixmap = QPixmap(":res/grid_scrollleftcursor.png");
    cursorPixmap.setMask(cursorPixmap.createMaskFromColor(Qt::magenta));
    m_scrLeftCursor = QCursor(cursorPixmap);
    QMatrix matrix;
    matrix.rotate(90);
    cursorPixmap = cursorPixmap.transformed(matrix);
    m_scrTopCursor = QCursor(cursorPixmap);

    cursorPixmap = QPixmap(":res/grid_scrollrightcursor.png");
    cursorPixmap.setMask(cursorPixmap.createMaskFromColor(Qt::magenta));
    m_scrRightCursor = QCursor(cursorPixmap);

    cursorPixmap = cursorPixmap.transformed(matrix);
    m_scrBottomCursor = QCursor(cursorPixmap);

    m_scrollTimer = new QTimer(this);
    m_scrollTimer->setSingleShot(true);
    connect(m_scrollTimer, SIGNAL(timeout()), this, SLOT(scrollToCursor()));

    QSize s = source->rect().size();

    m_dragColumnWidget = new DragColumnWidget(column, s);

    m_startPos.setX(((double)m_startPos.x() / (double)source->width()) * (double)s.width());

    m_downArrow = new ColumnArrowWidget(m_view->grid()->themeManager()->shadeColor(), NextRowPosition);
    m_upArrow = new ColumnArrowWidget(m_view->grid()->themeManager()->shadeColor(), PreviousRowPosition);
    m_leftArrow = new ColumnArrowWidget(m_view->grid()->themeManager()->shadeColor(), PreviousColumnPosition);
    m_rightArrow = new ColumnArrowWidget(m_view->grid()->themeManager()->shadeColor(), NextColumnPosition);
}

GridTableDragColumnObject::~GridTableDragColumnObject()
{
    updateFieldChooser(Q_NULL);
    Q_DELETE_AND_NULL(m_dragColumnWidget);
    Q_DELETE_AND_NULL(m_downArrow);
    Q_DELETE_AND_NULL(m_upArrow);
    Q_DELETE_AND_NULL(m_leftArrow);
    Q_DELETE_AND_NULL(m_rightArrow);
    Q_DELETE_AND_NULL(m_scrollTimer);
}

void GridTableDragColumnObject::setSourceGroupIndex(int groupIndex)
{
    m_sourceGroupIndex = groupIndex;
}

int GridTableDragColumnObject::sourceGroupIndex() const
{
    return m_sourceGroupIndex;
}

void GridTableDragColumnObject::setSourceVisualIndex(int visualIndex)
{
    m_sourceVisualIndex = visualIndex;
}

int GridTableDragColumnObject::sourceVisualIndex() const
{
    return m_sourceVisualIndex;
}

bool GridTableDragColumnObject::hideColumnFlag() const
{
    return m_hideColumnFlag;
}

void GridTableDragColumnObject::setHideColumnFlag(bool flag)
{
    m_hideColumnFlag = flag;
}

void GridTableDragColumnObject::beginEvent()
{
    GridDragDropObject::beginEvent();
    m_dragColumnWidget->show();
}

bool GridTableDragColumnObject::updateFieldChooser(GraphicPrimitive* overGP)
{
    bool ret = false;
    while (overGP != Q_NULL)
    {
        if (qobject_cast<GridFieldChooserPrimitive *>(overGP) != Q_NULL)
        {
            ret = true;
            break;
        }
        overGP = overGP->parentGP();
    }
    m_view->layout()->fieldChooser()->showDropTargetArea(ret);

    return ret;
}

void GridTableDragColumnObject::checkScrollToCursor(const QPoint& curPos)
{
    QRect scrollRect;
    m_view->layout()->calcScreenScrollRect(scrollRect);

    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    bool scrollBarVisible = verticalLayout ? m_view->layout()->isHorScrollBarVisible() : m_view->layout()->isVerScrollBarVisible();
    if (m_view->optionsref()->autoScroll() && !m_scrollTimer->isActive() && scrollBarVisible)
    {
        if (scrollRect.contains(curPos))
        {
            if (verticalLayout)
            {
                if (QCursor::pos().x() - scrollRect.x() < (int)kGridColumnAutoScrollIndent)
                {
                    m_scrollTimer->start(1000);
                }
                else if (scrollRect.x() + scrollRect.width() - curPos.x() < (int)kGridColumnAutoScrollIndent)
                {
                    m_scrollTimer->start(1000);
                }
                else
                    m_scrollTimer->stop();
            }
            else
            {
                if (QCursor::pos().y() - scrollRect.y() < (int)kGridColumnAutoScrollIndent)
                {
                    m_scrollTimer->start(1000);
                }
                else if (scrollRect.y() + scrollRect.height() - curPos.y() < (int)kGridColumnAutoScrollIndent)
                {
                    m_scrollTimer->start(1000);
                }
                else
                    m_scrollTimer->stop();
            }
        }
    }
}

void GridTableDragColumnObject::moveEvent(const QPoint& pos)
{
    bool overfcw = updateFieldChooser(m_view->layout()->control()->hitTest(pos));
    QPoint curPos = cursorPos();
    m_dragColumnWidget->update();
    m_dragColumnWidget->move(curPos.x() - m_startPos.x(), curPos.y() - m_startPos.y());

    Q_ASSERT(m_column != Q_NULL);

    if (target() == Q_NULL)
    {
        clear();
        if (m_downArrow != Q_NULL)
            m_downArrow->hide();
        if (m_upArrow != Q_NULL)
            m_upArrow->hide();
        if (m_leftArrow != Q_NULL)
            m_leftArrow->hide();
        if (m_rightArrow != Q_NULL)
            m_rightArrow->hide();
    }

    QRect mapedViewRect;
    if (m_view->layout()->parentGP() != Q_NULL)
    {
        Q_ASSERT(false);
        mapedViewRect = QRect(m_view->layout()->parentGP()->mapViewportToGlobal(
            m_view->layout()->rect().topLeft()), m_view->layout()->rect().size());
    }
    else
        mapedViewRect = QRect(m_view->layout()->mapToGlobal(QPoint(0, 0)), m_view->layout()->rect().size());

//        mapedViewRect = QRect(source()->control()->mapToGlobal( TODO
//        tableColumn->view()->layout()->rect().topLeft()), tableColumn->view()->layout()->rect().size());

    bool removeFlag = !mapedViewRect.contains(curPos);
    if (overfcw && m_view->layout()->fieldChooser()->isVisualOrderControlEnabled())
        removeFlag = false;
    else 
        removeFlag = removeFlag || overfcw;

    if (m_column->viewBase()->checkRemove(m_column) && removeFlag)
    {
        if (m_column->isVisible() && !hideColumnFlag())
        {
            setCursor(m_removeCursor);
            setHideColumnFlag(true);
        }
    }
    else
    {
        if (m_column->isVisible() && hideColumnFlag())
        {
            setHideColumnFlag(false);
            setDefaultCursor();
        }
    }

    checkScrollToCursor(curPos);
    GridDragDropObject::moveEvent(pos);
}

void GridTableDragColumnObject::acceptEvent()
{
    GridViewAutoUpdater autoUpdater(m_view);

    GridDataTableColumn* tableColumn = qobject_cast<GridDataTableColumn *>(m_column);
    Q_ASSERT(tableColumn != Q_NULL);
    if (sourceVisualIndex() != -1)
    {
        tableColumn->setVisualIndex(sourceVisualIndex());
        if (!m_view->layout()->fieldChooser()->isVisualOrderControlEnabled())
            tableColumn->setVisible(true);
        return;
    }

    if (sourceGroupIndex() != -1 || sourceGroupIndex() != tableColumn->groupIndex())
    {
        if (sourceGroupIndex() != -1 && (m_view->optionsref()->isColumnHidingOnGroupingEnabled() && tableColumn->isHidingOnGroupingEnabled()))
            tableColumn->setVisible(false);
        int groupIndex = tableColumn->groupIndex();
        if (groupIndex != -1 && groupIndex < sourceGroupIndex())
            tableColumn->setGroupIndex(sourceGroupIndex() -1);
        else
            tableColumn->setGroupIndex(sourceGroupIndex());
        return;
    }
    else
        tableColumn->setVisible(true);
}

void GridTableDragColumnObject::endEvent()
{
    GridTableColumnBase* columnBase = m_column;
    GridDragDropObject::endEvent();
    if (hideColumnFlag() && columnBase != Q_NULL)
        columnBase->setVisible(false);
}

bool GridTableDragColumnObject::columnsHasVerticalLayout() const
{
    return m_view->layout()->orientation() == Qt::Vertical;
}

bool GridTableDragColumnObject::checkDropColumnTarget(const QPoint& pos, GridColumnPrimitive* target)
{
    clear();

    if (target == Q_NULL && m_view->m_visibleColumnCount == 0)
    {
        setSourceVisualIndex(0);
        QPoint viewPos;
        int viewWidth = m_view->actualWidth();
        viewPos = m_view->rowsGraphic()->mapToGlobal(
            QPoint(m_view->layout()->borderLeft() + m_view->layout()->columnsRect().left(), -m_view->layout()->getViewHeight()));

        int xpos = viewPos.x();
        if (xpos < viewPos.x())
            xpos = viewPos.x();
        xpos -= m_rightArrow->width();
        int ypos = viewPos.y() - m_leftArrow->height() / 2 - 1;
        m_rightArrow->move(xpos, ypos);
        m_rightArrow->show();

        xpos = viewPos.x() + viewWidth;
        if (xpos > viewPos.x() + viewWidth)
            xpos = viewPos.x() + viewWidth;

        ypos = viewPos.y() - m_rightArrow->height() / 2 - 1;
        m_leftArrow->move(xpos, ypos);
        m_leftArrow->show();
        return true;
    }

    GridDataTableColumn* sourceColumn = qobject_cast<GridDataTableColumn *>(m_column);
    GridDataTableColumn* targetColumn = qobject_cast<GridDataTableColumn *>(target->column());

    if (sourceColumn != Q_NULL && targetColumn != Q_NULL)
    {
        if (sourceColumn->fixedKind() != targetColumn->fixedKind())
            return false;
        if (sourceColumn == targetColumn)
            return false;

        ColumnMovePosition position = target->getMovePositionByPoint(pos, false);

        switch (position)
        {
        case PreviousColumnPosition:
        case NextColumnPosition:
            {
                if (position == NextColumnPosition)
                    setSourceVisualIndex(targetColumn->visualIndex() + 1);
                else
                    setSourceVisualIndex(targetColumn->visualIndex());

                if (sourceColumn->visualIndex() < targetColumn->visualIndex())
                    setSourceVisualIndex(sourceVisualIndex() - 1);

                if (sourceVisualIndex() == sourceColumn->visualIndex() && 
                    sourceColumn->isVisible() == target->column()->isVisible())
                    return false;

                showMoveColumnArrows(target, position);
                return true;
            }
            break;
        default:
            return false;
        }
    }
    return false;
}

bool GridTableDragColumnObject::checkDropHeaderTarget(const QPoint& pos, GridTableGroupPanelPrimitive* target, GridDataTableColumn* tableColumn)
{
    clear();
    int posX = kGridDefHeaderWidthIndent;
    setSourceGroupIndex(0);

    const GridTableView::GridColumns& cols = m_view->groups();
    for (GridTableView::GridColumns::const_iterator it = cols.constBegin(); it != cols.constEnd(); ++it)
    {
        GridTabColumnPrimitive* cgp = target->getGraphicPrimitiveForColumn(*it);
        Q_ASSERT(cgp != Q_NULL);
        if (cgp == Q_NULL)
            continue;

        if (pos.x() >= posX - (int)kGridDefHeaderWidthIndent)
            setSourceGroupIndex(static_cast<GridDataTableColumn *>(cgp->column())->groupIndex());
        if (pos.x() > posX + cgp->width() / 2)
            setSourceGroupIndex(static_cast<GridDataTableColumn *>(cgp->column())->groupIndex() + 1);
        posX += cgp->width() + kGridDefHeaderWidthIndent;
    }

    int groupIndex = tableColumn->groupIndex();
    if (groupIndex != -1)
    {
        if (groupIndex == sourceGroupIndex() || groupIndex == sourceGroupIndex() - 1)
            return false;
    }

    if (sourceGroupIndex() != -1)
    {
        QPoint gp = target->mapToGlobal(QPoint(0, 0));
        GridTabColumnPrimitive* columnElement = target->getGraphicPrimitiveForColumn(m_view->groupColumnByIndex(sourceGroupIndex()));
        if (columnElement != Q_NULL)
        {
            gp = columnElement->mapToGlobal(QPoint(-1, 0));
            if (sourceGroupIndex() != 0)
                gp.setX(gp.x() - kGridDefHeaderWidthIndent / 2);
        }
        else
        {
            if (sourceGroupIndex() > 0)
            {
                columnElement = target->getGraphicPrimitiveForColumn(m_view->groupColumnByIndex(sourceGroupIndex() - 1));
                gp = columnElement->mapToGlobal(QPoint(columnElement->width() - 1, 0));
            }
        }

        m_downArrow->move(gp.x() - m_downArrow->width() / 2 + 1, gp.y() - m_downArrow->height());
        m_downArrow->show();
        m_upArrow->move(gp.x() - m_upArrow->width() / 2 + 1, gp.y() + ((columnElement != Q_NULL) ? columnElement->height() : target->height()));
        m_upArrow->show();
        m_leftArrow->hide();
        m_rightArrow->hide();
        return true;
    }
    else
        return false;
}

bool GridTableDragColumnObject::checkDropFieldChooserTarget(const QPoint& pos, GridFieldChooserPanel* target, GridDataTableColumn* tableColumn)
{
    for (QHash<GridColumnBase *, GridTabColumnPrimitiveBase *>::const_iterator it = target->m_colGroups.constBegin(); it != target->m_colGroups.constEnd(); ++it)
    {
        GridTabColumnPrimitiveBase* primitive = it.value();
        
        if (pos.y() >= primitive->top() && pos.y() <= primitive->top() + primitive->height() / 2)
        {
            setSourceVisualIndex(static_cast<GridTableColumnBase *>(primitive->column())->visualIndex());
            showMoveColumnArrows(primitive, PreviousColumnPosition);
        }
        else if (pos.y() >= primitive->top() + primitive->height() / 2 && pos.y() <= primitive->top() + primitive->height())
        {
            setSourceVisualIndex(static_cast<GridTableColumnBase *>(primitive->column())->visualIndex() + 1);
            showMoveColumnArrows(primitive, NextColumnPosition);
        }
        else
            primitive = Q_NULL;

        if (primitive != Q_NULL)
        {
            if (tableColumn->visualIndex() < static_cast<GridTableColumnBase *>(primitive->column())->visualIndex())
                setSourceVisualIndex(sourceVisualIndex() - 1);
            return true;
        }
    }
    return false;
}

void GridTableDragColumnObject::clear()
{
    m_sourceGroupIndex = -1;
    m_sourceVisualIndex = -1;
}

DragColumnWidget* GridTableDragColumnObject::dragColumnWidget() const
{
    return m_dragColumnWidget;
}

void GridTableDragColumnObject::showMoveColumnArrows(GraphicPrimitive* columnPrimitive, ColumnMovePosition position)
{
    QPoint targetPos = columnPrimitive->mapViewportToGlobal(QPoint(0, 0));

    if (position == NextColumnPosition)
    {
        if (columnsHasVerticalLayout())
            targetPos.rx() += columnPrimitive->width();
        else
            targetPos.ry() += columnPrimitive->height();
    }

    if (position == NextRowPosition)
    {
        //if (m_view->layout()->orientation() == Qt::Vertical)
            //targetPos.rx() += columnPrimitive->height();
        //else
        //    targetPos.ry() += columnPrimitive->width();
    }

    QRect scrollRect;
    m_view->layout()->calcScreenScrollRect(scrollRect);

    if (columnsHasVerticalLayout())
    {
        if (targetPos.x() < scrollRect.x())
            targetPos.setX(scrollRect.x());
        if (targetPos.x() > scrollRect.x() + scrollRect.width())
            targetPos.setX(scrollRect.x() + scrollRect.width());

        m_downArrow->move(targetPos.x() - m_downArrow->width() / 2 - 1, targetPos.y() - m_downArrow->height());
        m_downArrow->show();
        m_upArrow->move(targetPos.x() - m_upArrow->width() / 2 - 1, targetPos.y() + columnPrimitive->height());
        m_upArrow->show();

        m_leftArrow->hide();
        m_rightArrow->hide();
    }
    else
    {
        if (targetPos.y() < scrollRect.y())
            targetPos.setY(scrollRect.y());
        if (targetPos.y() > scrollRect.y() + scrollRect.height())
            targetPos.setY(scrollRect.y() + scrollRect.height());

        m_rightArrow->move(targetPos.x() - m_downArrow->width(), targetPos.y() - m_downArrow->height() / 2 - 1);
        m_rightArrow->show();
        m_leftArrow->move(targetPos.x() + columnPrimitive->width(), targetPos.y() - m_downArrow->height() / 2 - 1);
        m_leftArrow->show();

        m_downArrow->hide();
        m_upArrow->hide();
    }
}

void GridTableDragColumnObject::scrollToCursor()
{
    GridTableColumnBase* columnBase = m_column;
    if (source() == Q_NULL)
    {
        Q_ASSERT(false);
        return;
    }

    QRect scrollRect;
    m_view->layout()->calcScreenScrollRect(scrollRect);

    bool verticalLayout = columnBase->viewBase()->layout()->orientation() == Qt::Vertical;
    QScrollBar* scrollBar = verticalLayout ? columnBase->viewBase()->layout()->scrollHor() : columnBase->viewBase()->layout()->scrollVer();
    if (scrollRect.contains(QCursor::pos()))
    {
        setDefaultCursor();
        setDefaultAcceptedCursor();

        if (verticalLayout)
        {
            if (QCursor::pos().x() - scrollRect.x() < (int)kGridColumnAutoScrollIndent)
            {
                if (scrollBar->value() > scrollBar->minimum())
                {
                    int step = 6 - (QCursor::pos().x() - scrollRect.x()) / 6;
                    scrollBar->setValue(scrollBar->value() - step);
                    m_scrollTimer->start(10);
                    setCursor(m_scrLeftCursor);
                    setAcceptedCursor(m_scrLeftCursor);
                }
            }
            else if (scrollRect.x() + scrollRect.width() - QCursor::pos().x() < (int)kGridColumnAutoScrollIndent)
            {
                if (scrollBar->value() < scrollBar->maximum())
                {
                    int step = 6 - (scrollRect.x() + scrollRect.width() - QCursor::pos().x()) / 6;
                    scrollBar->setValue(scrollBar->value() + step);
                    m_scrollTimer->start(10);
                    setCursor(m_scrRightCursor);
                    setAcceptedCursor(m_scrRightCursor);
                }
            }
        }
        else
        {
            if (QCursor::pos().y() - scrollRect.y() < (int)kGridColumnAutoScrollIndent)
            {
                if (scrollBar->value() > scrollBar->minimum())
                {
                    int step = 6 - (QCursor::pos().y() - scrollRect.y()) / 6;
                    scrollBar->setValue(scrollBar->value() - step);
                    m_scrollTimer->start(10);
                    setCursor(m_scrTopCursor);
                    setAcceptedCursor(m_scrTopCursor);
                }
            }
            else if (scrollRect.y() + scrollRect.height() - QCursor::pos().y() < (int)kGridColumnAutoScrollIndent)
            {
                if (scrollBar->value() < scrollBar->maximum())
                {
                    int step = 6 - (scrollRect.y() + scrollRect.height() - QCursor::pos().y()) / 6;
                    scrollBar->setValue(scrollBar->value() + step);
                    m_scrollTimer->start(10);
                    setCursor(m_scrBottomCursor);
                    setAcceptedCursor(m_scrBottomCursor);
                }
            }
        }

        QMouseEvent event(QEvent::MouseMove, 
            static_cast<GridGraphicSitePublic *>(source()->control())->mapFromGlobal(QCursor::pos()),
            Qt::NoButton,
            Qt::MouseButtons(Qt::MouseButtonMask),
            Qt::KeyboardModifiers(Qt::KeyboardModifierMask));

        static_cast<GridGraphicSitePublic *>(source()->control())->mouseMoveEvent(&event);
    }
}

/* GridTableSelectionRubberBand */
GridTableSelectionRubberBand::GridTableSelectionRubberBand(GraphicPrimitive* parent)
: GraphicPrimitive(parent->control(), parent)
{
    setTransparent(true);
}

void GridTableSelectionRubberBand::paintEvent(QPainter& painter)
{
    QStyleOptionRubberBand option;
    option.rect = boundedRect();
    option.shape = QRubberBand::Rectangle;
    option.opaque = true;
    control()->style()->drawControl(QStyle::CE_RubberBand, &option, &painter, control());
}

/* GridViewSelectionObject */
GridViewSelectionObject::GridViewSelectionObject(GridViewBase* view, const GridCell& startCell, const QPoint& cellPos)
    : GridDragDropObject(view->layout()->rowsPanel()), m_view(view), m_rubberBandGP(Q_NULL),
  m_startCell(startCell), m_endCell(startCell), m_cellPos(cellPos)
{
    m_rubberBandGP = new GridTableSelectionRubberBand(view->layout());
    ScrollableGraphicPrimitive::setFixedGPX(m_rubberBandGP, true);
    ScrollableGraphicPrimitive::setFixedGPY(m_rubberBandGP, true);
    m_rubberBandGP->setZIndex(180);
    m_rubberBandGP->setVisibility(false);
}

GridViewSelectionObject::~GridViewSelectionObject()
{
    Q_DELETE_AND_NULL(m_rubberBandGP);
}

bool GridViewSelectionObject::event(QEvent* e)
{
    switch (e->type())
    {
    case QEvent::Timer:
        {
            if (static_cast<QTimerEvent *>(e)->timerId() == m_scrollTimer.timerId())
            {
                moveEvent(m_lastPos);
                e->accept();
                return true;
            }
        }
        break;
    default:
        break;
    }
    return QObject::event(e); 
}

void GridViewSelectionObject::beginEvent()
{
    m_rubberBandGP->setVisibility(m_view->optionsref()->rubberBandSelection());
    m_rubberBandGP->update();
}

void GridViewSelectionObject::scrollTo(const QPoint& pos)
{
    QPoint p = m_view->rowsGraphic()->mapFromSite(pos);
    int xscroll = m_view->layout()->scrollHor()->value();
    int yscroll = m_view->layout()->scrollVer()->value();
    if (p.x() < 0)
    {
        m_view->layout()->scrollHor()->setValue(m_view->layout()->scrollHor()->value() - m_view->layout()->scrollHor()->singleStep());
    }
    else if (p.x() > m_view->layout()->boundedRect().width())
    {
        m_view->layout()->scrollHor()->setValue(m_view->layout()->scrollHor()->value() + m_view->layout()->scrollHor()->singleStep());
    }

    if (p.y() < 0)
    {
        m_view->layout()->scrollVer()->setValue(m_view->layout()->scrollVer()->value() - m_view->layout()->scrollVer()->singleStep());
    }
    else if (p.y() > m_view->rowsGraphic()->boundedRect().height())
    {
        m_view->layout()->scrollVer()->setValue(m_view->layout()->scrollVer()->value() + m_view->layout()->scrollVer()->singleStep());
    }

    if ((xscroll == m_view->layout()->scrollHor()->value() && yscroll == m_view->layout()->scrollVer()->value()))
        m_scrollTimer.stop();
    else
    {
        if (!m_scrollTimer.isActive())
            m_scrollTimer.start(25, this);        
    }
}

void GridViewSelectionObject::setRubberBandRect(const QPoint& pos)
{
    QPoint startPos;
    GraphicPrimitive* gp = Q_NULL;
    GridRowLayoutItem* virtualRow = m_view->layout()->rowsLayoutEngine()->findVirtualRow(m_startCell.row());
    if (virtualRow != Q_NULL)
    {
        if (m_startCell.row()->type() == GridRow::DataRow)
            gp = virtualRow->getCellGraphic(m_startCell.columnIndex());
        else
        {
            gp = virtualRow->groupRowGP();
        }
    }
    
    if (gp == Q_NULL)
    {
        GridTableColumn* startColumn = static_cast<GridTableColumn *>(m_view->getColumn(m_startCell.columnIndex()));
        if (startColumn == Q_NULL)
            startPos = m_cellPos;
        else
            startPos = m_view->layout()->mapFromSite(static_cast<GridTableView *>(m_view)->layout()->columnHeader()->getPrimitiveByColumn(startColumn)->mapToSite(m_cellPos));

        GridRowLayoutItem* virtualRow = m_view->layout()->rowsLayoutEngine()->findVirtualRow(m_startCell.row());
        if (virtualRow != Q_NULL)
        {
            QPoint p = m_view->layout()->mapFromSite(virtualRow->dataRowGP()->mapToSite(m_cellPos));
            startPos.setY(p.y());
        }
        else
        {
            if (m_startCell.rowIndex() < m_view->layout()->rowsLayoutEngine()->firstGridRow()->rowIndex())
                startPos.setY(-1);
            else
                startPos.setY(m_view->layout()->height() + 1);
        }
    }
    else
    {
        startPos = m_view->layout()->mapFromSite(gp->mapToSite(m_cellPos));
    }

    QPoint curPos = m_view->layout()->mapFromSite(pos);
    QRect rbRect = QRect();
    if (curPos.x() >= startPos.x())
    {
        rbRect.setLeft(startPos.x());   
        rbRect.setWidth(curPos.x() - startPos.x());
    }
    else
    {
        rbRect.setLeft(curPos.x());
        rbRect.setWidth(startPos.x() - curPos.x());
    }

    if (curPos.y() >= startPos.y())
    {
        rbRect.setTop(startPos.y());
        rbRect.setHeight(curPos.y() - startPos.y());
    }
    else
    {
        rbRect.setTop(curPos.y());
        rbRect.setHeight(startPos.y() - curPos.y());
    }
    QRect updateRect = m_rubberBandGP->rect();
    updateRect = updateRect.united(rbRect);
    m_rubberBandGP->setRect(rbRect);
    m_view->layout()->update(updateRect.translated(m_view->layout()->borderLeft(), m_view->layout()->borderTop()));
    //m_rubberBandGP->update(updateRect);
}

void GridViewSelectionObject::moveEvent(const QPoint& pos)
{
    m_lastPos = pos;
    scrollTo(pos);
    setRubberBandRect(pos);
    if (!m_view->optionsref()->rubberBandSelection())
        selectTo(pos);
}

void GridViewSelectionObject::endEvent()
{
    selectTo(m_lastPos);
}

/* GridTableViewSelectionObject */
GridTableViewSelectionObject::GridTableViewSelectionObject(GridTableView* view, const GridCell& cell, const QPoint& cellPos)
    : GridViewSelectionObject(view, cell, cellPos)
{
}

void GridTableViewSelectionObject::selectTo(const QPoint& pos)
{
    GridRowLayoutItem* vr = static_cast<GridTableRowsLayoutEngine *>(m_view->layout()->rowsLayoutEngine())->getVirtualRowByPosY(pos.y());
    if (vr == Q_NULL)
    {
        Q_ASSERT(false);
        return;
    }

    m_endCell = GridCell(vr->gridRow(), -1);
    QPoint startPos = m_view->layout()->mapViewportFromSite(m_rubberBandGP->mapToSite(QPoint(0, 0)));
    QList<GridDataTableColumn *> columns;
    int leftXPos = startPos.x();
    int rightXPos = startPos.x() + m_rubberBandGP->rect().width();
    bool rowSelectionPolicy = (m_view->optionsref()->selectionPolicy() == SelectSingleRow ||
        m_view->optionsref()->selectionPolicy() == SelectMultiRow);
    if (rowSelectionPolicy)
    {
        leftXPos = 0;
        rightXPos = 1000000;
    }

    view()->collectColumsForSelect(leftXPos, rightXPos, columns);

    SelectionOperation operation;
    operation |= Qtitan::Clear;
    operation |= Qtitan::Select;

    m_view->selectColumnRange(m_startCell.row(), m_endCell.row(), columns, operation);
    m_view->layout()->update();
}

GridTableView* GridTableViewSelectionObject::view() const
{
    return static_cast<GridTableView *>(m_view);
}
/*!
\class GridCellDragObject
\internal
*/
GridCellDragObject::GridCellDragObject(GridViewBase* view, GridTableCellGraphic* cellGP, const QPoint& pos)
: GridNativeDragObject(view->grid(), cellGP), m_view(view), m_cellGP(cellGP), m_pos(pos),
  m_modelRow(-1), m_modelColumn(-1)
{
    m_downArrow = new ColumnArrowWidget(view->grid()->themeManager()->shadeColor(), NextRowPosition);
    m_upArrow = new ColumnArrowWidget(view->grid()->themeManager()->shadeColor(), PreviousRowPosition);
    m_leftArrow = new ColumnArrowWidget(view->grid()->themeManager()->shadeColor(), PreviousColumnPosition);
    m_rightArrow = new ColumnArrowWidget(view->grid()->themeManager()->shadeColor(), NextColumnPosition);
}

GridCellDragObject::~GridCellDragObject()
{
    Q_DELETE_AND_NULL(m_downArrow);
    Q_DELETE_AND_NULL(m_upArrow);
    Q_DELETE_AND_NULL(m_leftArrow);
    Q_DELETE_AND_NULL(m_rightArrow);
}

QModelIndexList GridCellDragObject::draggableIndexes()
{
    QModelIndexList indexes;
    GridSelection* selection = m_view->selection();
    while (!selection->end())
    {
        if (selection->row()->type() == GridRow::GroupRow)
        {
            selection->next();
            continue;
        }

        GridRow* current = selection->row();
        while (!selection->end() && current == selection->row())
        {
            indexes.append(selection->cell()->modelIndex());
            selection->next();
        }
    }
    return indexes;
}

bool GridCellDragObject::checkDragDropEvent(ColumnMovePosition position, bool cell, GraphicPrimitive* gp)
{
    DragDropArgs args;
    args.setMimeData(mimeData());
    args.setDropAction(dropAction());
    emit m_view->checkDragDrop(&args);
    if (args.isHandled())
        return args.isAccepted();

    QStringList modelTypes = m_view->modelController()->model()->mimeTypes();
    QMimeData *mime = mimeData();
    if (mime == Q_NULL)
        return false;
    bool hasFormat = false;
    for (int i = 0; i < modelTypes.count(); ++i)
    {
        if (mime->hasFormat(modelTypes.at(i)))
        {

            hasFormat = true;
            break;
        }
    }

    if (!hasFormat)
        return false;

    if (!(args.dropAction() & m_view->modelController()->model()->supportedDropActions()))
        return false;

    setDropAction(args.dropAction());

    if (qobject_cast<GridTableViewPrimitive *>(gp))
    {
        return checkRow(position, Q_NULL);
    }

    if (cell)
        return checkCell(position, static_cast<GridTableCellGraphic *>(gp));
    else
        return checkRow(position, static_cast<GridDataRowPrimitive *>(gp));
}

bool GridCellDragObject::checkRow(ColumnMovePosition position, GridDataRowPrimitive* gp1)
{
    if (gp1 == Q_NULL && m_view->getRowCount() > 0)
        return false;

    int row = gp1 != Q_NULL && gp1->rowLayoutItem()->gridRow() ? gp1->rowLayoutItem()->gridRow()->modelRowIndex() : -1;
    if (row != -1)
    {
        if (position == PreviousRowPosition)
            setModelRow(row);
        else
            setModelRow(row + 1);
    }
    else
        setModelRow(-1);

    setParentModelIndex(m_view->modelController()->parentIndex());

    QPoint viewPos = m_view->rowsGraphic()->mapToGlobal(m_view->layout()->columnsRect().topLeft());

    QPoint gpos = gp1 != Q_NULL ? gp1->mapToGlobal(QPoint(0, position == NextRowPosition ? gp1->height() : 0)) : viewPos;
    int viewWidth = m_view->actualWidth();

    int xpos = gpos.x() - m_rightArrow->width();
    int ypos = gpos.y() - m_rightArrow->height() / 2 - 1;
    m_rightArrow->move(xpos, ypos);
    m_rightArrow->show();

    if (gp1 == Q_NULL)
    {
        if (m_view->rowsGraphic()->width() > viewWidth)
            xpos = gpos.x() + viewWidth;
        else
            xpos = gpos.x() + m_view->rowsGraphic()->width() - m_view->layout()->columnsRect().left();
    }
    else if (gp1->width() > viewWidth)
        xpos = gpos.x() + viewWidth;
    else
        xpos = gpos.x() + gp1->width();
    ypos = gpos.y() - m_leftArrow->height() / 2 - 1;
    m_leftArrow->move(xpos, ypos);
    m_leftArrow->show();
    m_upArrow->hide();
    m_downArrow->hide();
    return true;
}

bool GridCellDragObject::checkCell(ColumnMovePosition position, GridTableCellGraphic* gp)
{
    int col = gp->dataBinding() ? gp->dataBinding()->column() : -1;

    if (position == PreviousRowPosition || position == NextRowPosition)
    {
        bool retval = checkRow(position, static_cast<GridDataRowPrimitive *>(gp->parentGP()));
        setModelColumn(col);
        return retval;
    }

    int row = gp->gridRow() ? gp->gridRow()->modelRowIndex() : -1;

    setParentModelIndex(m_view->modelController()->model()->index(row, col, m_view->modelController()->parentIndex()));
    setModelRow(-1);
    setModelColumn(-1);

    QPoint gpos = gp->mapToGlobal(QPoint(0, 0));
    int xpos = gpos.x() + gp->width() /2 - m_downArrow->width() / 2;
    int ypos = gpos.y() - m_downArrow->height();
    m_downArrow->move(xpos, ypos);
    m_downArrow->show();
    xpos = gpos.x() + gp->width() /2 - m_upArrow->width() / 2;
    ypos = gpos.y() + gp->height();
    m_upArrow->move(xpos, ypos);
    m_upArrow->show();
    m_leftArrow->hide();
    m_rightArrow->hide();
    return true;
}

QModelIndex GridCellDragObject::parentModelIndex()
{
    return m_parentModelIndex;
}

void GridCellDragObject::setParentModelIndex(const QModelIndex& index)
{
    m_parentModelIndex = index;
}

int GridCellDragObject::modelRow()
{
    return m_modelRow;
}

void GridCellDragObject::setModelRow(int row)
{
    m_modelRow = row;
}

int GridCellDragObject::modelColumn()
{
    return m_modelColumn;
}

void GridCellDragObject::setModelColumn(int column)
{
    m_modelColumn = column;
}

void GridCellDragObject::beginEvent()
{
    if (m_cellGP == Q_NULL)
        return; //This case for Drop operation.

    GridTableCellGraphic* cellGP = m_cellGP; //Drag operation may destroy instance of this object - GridCellDragObject().
    Qt::DropAction action = m_drag->exec(m_supportedActions, m_defaultDropAction);

    if (cellGP->control()->getDragDropObject() != Q_NULL)
        static_cast<GridCellDragObject *>(cellGP->control()->getDragDropObject())->setDropAction(action);

    QMouseEvent event(QEvent::MouseButtonRelease, cellGP->mapFromGlobal(QCursor::pos()), 
        QCursor::pos(), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    ((GraphicControlEx *)cellGP->control())->mouseReleaseEvent(&event);
}

bool GridCellDragObject::startDrag()
{
    if (m_cellGP == Q_NULL)
        return false;

    QModelIndexList indexes = draggableIndexes();

    DragArgs startArgs;
    startArgs.setIndexes(indexes);
    emit m_view->startDragDrop(&startArgs);

    if (startArgs.isHandled())
    {
        if (!startArgs.isAccepted())
            return false;

        indexes = startArgs.indexes();
    }
    else
    {
        //Default handler for startDragDrop operaion.
        for(int i = indexes.count() - 1 ; i >= 0; --i)
        {
            if (!(indexes.at(i).flags() & Qt::ItemIsDragEnabled))
                indexes.removeAt(i);
        }
    }

    if (indexes.count() == 0)
        return false;

    QMimeData* data = startArgs.mimeData();
    if (data == Q_NULL)
    {
        data = m_view->modelController()->model()->mimeData(indexes);
    }
    
    if (data == Q_NULL)
        return false;

    QRect rect;
    m_drag = new QDrag(m_cellGP->control());
    QGraphicsDropShadowEffect* e = new QGraphicsDropShadowEffect;
    e->setColor(QColor(40, 40, 40, 245));
    e->setOffset(0, 10);
    e->setBlurRadius(50);
    QPixmap icon = qtn_get_thememanager(control())->getIcon(GridThemeManager::DragDropBoxIcon,  QSize(64, 64));
    QPixmap pixmap(icon.width() + 60, icon.height() + 30);
    pixmap.fill(Qt::transparent);
    {
        QPainter p(&pixmap);
        p.setOpacity(0.5);
        p.drawPixmap(pixmap.width() / 2 - icon.width() / 2, 0, icon);
        QFont f = p.font();
        f.setBold(true);
        p.setFont(f);
        p.setOpacity(0.7);
        QString text = QString::number(indexes.size()) + " ";
        if (indexes.size() == 1)
            text += Grid::tr_compatible(QtnGridCellString);
        else
            text += Grid::tr_compatible(QtnGridCellsString);
        int w = p.fontMetrics().width(text);
        p.drawText(pixmap.width() / 2 - w / 2, icon.height() + 15, text);
    }
    pixmap = apply_effect_to_pixmap(pixmap, e, 1);
    m_drag->setPixmap(pixmap);

    m_drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height()));

    m_drag->setMimeData(data);
    m_defaultDropAction = Qt::IgnoreAction;
    m_supportedActions = startArgs.supportedDropActions();
    if (startArgs.defaultDropAction() != Qt::IgnoreAction && (m_supportedActions & startArgs.defaultDropAction()))
        m_defaultDropAction = startArgs.defaultDropAction();
    return true;
}

void GridCellDragObject::moveEvent(const QPoint& pos)
{
    Q_UNUSED(pos);

    if (target() == Q_NULL)
    {
        setModelRow(-1);
        setModelColumn(-1);
        setParentModelIndex(QModelIndex());
        if (m_downArrow != Q_NULL)
            m_downArrow->hide();
        if (m_upArrow != Q_NULL)
            m_upArrow->hide();
        if (m_leftArrow != Q_NULL)
            m_leftArrow->hide();
        if (m_rightArrow != Q_NULL)
            m_rightArrow->hide();
    }
}

void GridCellDragObject::acceptEvent()
{
    DragDropArgs args;
    args.setMimeData(mimeData());
    args.setDropAction(dropAction());
    emit m_view->dragDrop(&args);
    if (args.isHandled())
        return;

    m_view->modelController()->model()->dropMimeData(args.mimeData(), args.dropAction(),
                                                     modelRow(), modelColumn(),
                                                     parentModelIndex());
}

void GridCellDragObject::endEvent()
{
    if (m_cellGP == Q_NULL)
        return;

    DragArgs endArgs;
    endArgs.setDropAction(dropAction());
    emit m_view->endDragDrop(&endArgs);
    if (!endArgs.isHandled())
    {
        if (endArgs.dropAction() == Qt::MoveAction)
            completeMoveAction();
    }
}

void GridCellDragObject::completeMoveAction()
{
}

/*!
\class GridTableViewOptions
\inmodule QtitanDataGrid
GridTableViewOptions class is used for configuring the behavior and display of different parts of GridTableView.
*/

/*!
Constructor of GridTableViewOptions class. The class is constructed automatically when the table view is created.
*/
GridTableViewOptions::GridTableViewOptions(GridTableView* view)
: GridViewOptions(view), m_columnAutoWidth(false), m_columnsHeader(true), 
  m_columnsQuickMenuVisible(true), m_columnsQuickCustomization(true), m_rowsQuickSelection(false), m_rowFrozenButtonVisible(false), m_frozenPlaceQuickSelection(false), m_columnHeaderHints(true), 
  m_columnsDropDownCount(0), m_fixedSeparatorColor(Qt::black), m_fixedSeparatorWidth(2),
  m_isColumnHorSizingEnabled(true)
{
}

/*!
Returns true if the auto column width feature is on. The column width is calculated automatically and depends on the view width - if possible, all columns shoud fit within the view.
*/
bool GridTableViewOptions::columnAutoWidth() const
{
    return m_columnAutoWidth;
}

/*!
Sets the column's auto width feature. If \a autoWidth is a true then the columns will be adjasted to the entire with of the view.
For this, all the columns will be stretched or narrowed proportionally. But at the same time takes into account the settings of maximum/minimum column's width.
The option has effect if the type of view orientation is Qt::Vertical only. for Qt::Horizontal orientation of the view it is not used.
\sa columnAutoWidth()
*/
void GridTableViewOptions::setColumnAutoWidth(bool autoWidth)
{
    GridViewAutoUpdater autoUpdater(view());
    m_columnAutoWidth = autoWidth;
    view()->requestUpdateLayout();
}

/*!
Returns "true" if grid columns are displayed. Otherwise, "false" is returned.
*/
bool GridTableViewOptions::columnsHeader() const
{
    return m_columnsHeader;
}

/*!
Enables or disables the display of grid columns depending on \a columnsHeader.
*/
void GridTableViewOptions::setColumnsHeader(bool columnsHeader)
{
    m_columnsHeader = columnsHeader;
    GridTableView* tableView = static_cast<GridTableView *>(view());
    for (int i = 0; i < tableView->getColumnCount(); i++)
    {
        GridColumnBase* column = tableView->getColumn(i);
        if (tableView->isColumnVisible(column))
            column->setVisible(true);
    }
    tableView->requestUpdateLayout();
}

/*!
Returns the visibility value of a small button to the left of the columns that forms a margin on the left side of the view. If the button is shown, the funtion returns "true" (default value).
*/
bool GridTableViewOptions::columnsQuickCustomization() const
{
    return m_columnsQuickCustomization;
}

/*!
Sets the visibility to \a quickCustomization of a small button to the left of the columns that forms a margin on the left side of the view.
*/
void GridTableViewOptions::setColumnsQuickCustomization(bool quickCustomization)
{
    if (m_columnsQuickCustomization == quickCustomization)
        return;
    m_columnsQuickCustomization = quickCustomization;
    view()->requestUpdateLayout();
}

bool GridTableViewOptions::columnsQuickMenuVisible() const
{
    return m_columnsQuickMenuVisible;
}

void GridTableViewOptions::setColumnsQuickMenuVisible(bool quickMenuVisible)
{
    if (m_columnsQuickMenuVisible == quickMenuVisible)
        return;
    m_columnsQuickMenuVisible = quickMenuVisible;
    view()->requestUpdateLayout();
}

/*!
Returns the number of items in the popup window that is shown when the quick customization button (a small icon to the left of the columns) is clicked and before the vertical scrollbar is shown.
*/
int GridTableViewOptions::columnsQuickCustomizationMaxDropDownCount() const
{
    return m_columnsDropDownCount;
}

/*!
Sets the number of items in the popup window that is shown when the quick customization button (a small icon to the left of the columns) is clicked and before the vertical scrollbar is shown.
*/
void GridTableViewOptions::setColumnsQuickCustomizationMaxDropDownCount(int dropDownCount)
{
    if (m_columnsDropDownCount == dropDownCount)
        return;
    m_columnsDropDownCount = dropDownCount;
    GridTableView* tableView = static_cast<GridTableView *>(view());
    tableView->layout()->columnHeader()->m_quickColumnButton->setDropDownCount(dropDownCount);
}

/*!
Returns true if a quick rows quick selection feature is enabled. Otherwise returns false. By default it is false.
*/
bool GridTableViewOptions::rowsQuickSelection() const
{
    return m_rowsQuickSelection;
}

/*!
Enables a quick rows selection feature. If \a quickSelection is true grid will show a special check boxes for quick rows selection. Check boxes are located opposite each row at the left. 
In addition to the left of the column headers there is a separate check box that is make possible to select all or de-select all rows. For single row selection policy the radio box is used instead of check box. 
*/
void GridTableViewOptions::setRowsQuickSelection(bool quickSelection)
{
    if (m_rowsQuickSelection == quickSelection)
        return;
    m_rowsQuickSelection = quickSelection;
    view()->requestUpdateLayout();
}

/*!
Returns the visibility of quick button for selection of place for freeze. By default it is false.
*/
bool GridTableViewOptions::frozenPlaceQuickSelection() const
{
    return m_frozenPlaceQuickSelection;
}

/*!
Sets the visibility of quick button for selection of place for freeze to \a visible.
*/
void GridTableViewOptions::setFrozenPlaceQuickSelection(bool visible)
{
    m_frozenPlaceQuickSelection = visible;
    view()->requestUpdateLayout();
}

/*!
Returns the visibility of buttons that allow to freeze the row. By default it is false.
*/
bool GridTableViewOptions::isRowFrozenButtonVisible() const
{
    return m_rowFrozenButtonVisible;
}

/*!
Sets the visibility of button at the left of row to \a visible that allow to freeze the row.
*/
void GridTableViewOptions::setRowFrozenButtonVisible(bool visible)
{
    m_rowFrozenButtonVisible = visible;
    view()->requestUpdateLayout();
}

/*!
Returns "true" if the component is configured to show a popup tip over the column header if the column does not completely fit on the screen and the user cannot read the column header.
*/
bool GridTableViewOptions::columnHeaderHints() const
{
    return m_columnHeaderHints;
}

/*!
Sets the flag \a headerHints that adds a popup hint over the column header if the header does not fit on the screen and the user cannot read the column header.
*/
void GridTableViewOptions::setColumnHeaderHints(bool headerHints)
{
    m_columnHeaderHints = headerHints;
}

/*!
Returns the color of the vertical line used for delimiting fixed cells to the left or to the right of the non-fixed cells.
\sa fixedSeparatorWidth()
*/
const QColor& GridTableViewOptions::fixedSeparatorColor() const
{
    return m_fixedSeparatorColor;
}

/*!
Sets the \a color of the vertical line used for delimiting fixed cells to the left or to the right of the non-fixed cells.
\sa setFixedSeparatorWidth()
*/
void GridTableViewOptions::setFixedSeparatorColor(const QColor& color)
{
    m_fixedSeparatorColor = color;
    view()->layout()->update();
}

/*!
Returns the width of the vertical line used for delimiting fixed cells to the left or to the right of the non-fixed cells.
\sa fixedSeparatorColor()
*/
int GridTableViewOptions::fixedSeparatorWidth() const
{
    return m_fixedSeparatorWidth;
}


/*!
Sets the \a width of the vertical line used for delimiting fixed cells to the left or to the right of the non-fixed cells.
\sa setFixedSeparatorColor()
*/
void GridTableViewOptions::setFixedSeparatorWidth(int width)
{
    m_fixedSeparatorWidth = width;
    view()->layout()->update();
}

/*!
Returns "true" if the user can change the column width. The flag is applied to all columns at once.
*/
bool GridTableViewOptions::isColumnHorSizingEnabled() const
{
    return m_isColumnHorSizingEnabled;
}

/*!
Enables or disables the user's ability to change the column width. This flag is applied to all columns at once. If you need to prohibit size changes for one column only, you need to change the corresponding property of this specific column - GridTableColumn::setHorSizingEnabled() - and leave the global flag set to "true".
*/
void GridTableViewOptions::setColumnHorSizingEnabled(bool enabled)
{
    m_isColumnHorSizingEnabled = enabled;
}

/* GridTableRowLayoutItem */
GridTableRowLayoutItem::GridTableRowLayoutItem(GridTableRowsLayoutEngine* tableEngine)
    : GridRowLayoutItem(tableEngine)
{
}

GridTableRowLayoutItem::~GridTableRowLayoutItem()
{
}

void GridTableRowLayoutItem::create()
{
    GridRowLayoutItem::create();
}

GridTableView* GridTableRowLayoutItem::view() const
{
    return (GridTableView *)m_view;
}

void GridTableRowLayoutItem::addColumn(const GridDataTableColumn* column)
{
    m_dataRowGP->registrColumnGraphic(static_cast<const GridTableColumn *>(column)->columnGP());
    m_summarylayoutEngine->addColumn(column);
}

void GridTableRowLayoutItem::removeColumn(const GridDataTableColumn* column)
{
    m_dataRowGP->unregistrColumnGraphic(static_cast<const GridTableColumn *>(column)->columnGP());
    m_summarylayoutEngine->removeColumn(column);
}

/*
void GridTableRowLayoutItem::updateGroupIndentRowLayout(bool visible)
{
    visible = false;
    
    int leftGroupIndent = m_view->getRowGroupIndent(m_gridRow);
    visible = visible && (leftGroupIndent > 0);

    if (!visible && !m_groupIndentGraphic->visibility())
        return;
    m_groupIndentGraphic->setVisibility(visible);
    if (!visible)
        return;

    // Draw left row indent.
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    int quickButtonLength = verticalLayout ? m_view->layout()->columnHeader()->quickButton()->width() : m_view->layout()->columnHeader()->quickButton()->height();

    if (verticalLayout)
    {
        m_groupIndentGraphic->setRect(rect().adjusted(quickButtonLength, 0, 0, 0));
        m_groupIndentGraphic->setWidth(leftGroupIndent);
    }
    else
    {
        m_groupIndentGraphic->setRect(rect().adjusted(0, quickButtonLength, 0, 0));
        m_groupIndentGraphic->setHeight(leftGroupIndent);
    }
    checkMasked(m_groupIndentGraphic);
    
}
*/
void GridTableRowLayoutItem::updateGroupRowLayout(bool visible)
{

    GridRowLayoutItem::updateGroupRowLayout(visible);
    if (!visible)
        return;

    checkMasked(m_groupRowGP);
    qtn_do_update_fixed(m_view->layout()->orientation() == Qt::Vertical, viewBase()->layout()->leftFixedIndent() > 0, groupRowGP()->branchButon());

    int quickButtonIndent = m_view->layout()->getQuickButtonIndent();
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;

    if (verticalLayout)
    {
        m_groupRowGP->setRect(rect().adjusted(quickButtonIndent, 0, 0, 0));
        int height = get_group_height(m_view->grid()->style(), m_view->optionsref());

        QRect r = QStyle::alignedRect(m_view->grid()->layoutDirection(), Qt::AlignVCenter | Qt::AlignHCenter, QSize(kGridGroupRowIndent, kGridGroupRowIndent),
            QRect(indentRect().width(), 0, kGridGroupRowIndent, height));
        m_groupRowGP->m_branchButton->setRect(r);
    }
    else
    {
        m_groupRowGP->setRect(rect().adjusted(0, quickButtonIndent, 0, 0));
        int width = get_group_height(m_view->grid()->style(), m_view->optionsref());
        QRect r = QStyle::alignedRect(m_view->grid()->layoutDirection(), Qt::AlignVCenter | Qt::AlignVCenter, QSize(kGridGroupRowIndent, kGridGroupRowIndent),
            QRect(0, indentRect().height(), width, kGridGroupRowIndent));
        
        m_groupRowGP->m_branchButton->setRect(r);
    }
}

void GridTableRowLayoutItem::updateDataRowLayout(bool visible)
{
    GridRowLayoutItem::updateDataRowLayout(visible);

    if (!visible)
        return;

    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;

    int quickButtonIndent = m_view->layout()->getQuickButtonIndent();

    if (verticalLayout)
        m_dataRowGP->setRect(rect().adjusted(quickButtonIndent/* + m_view->getRowGroupIndent(m_gridRow)*/, 0, 0, 0));
    else
        m_dataRowGP->setRect(rect().adjusted(0, quickButtonIndent/* + m_view->getRowGroupIndent(m_gridRow)*/, 0, 0));

    const bool autoHeightEnabled = (view()->layout()->autoHeightRowCount() > 0) && (type() == Data || type() == FrozenData);

    const int* autoHeightPtr = autoHeightEnabled ? &view()->layout()->autoHeightInfo()[view()->layout()->autoHeightRowCount() * gridRow()->rowIndex()] : Q_NULL;
    int gridLinesHeight = m_dataRowGP->borderBottom();
    int gridLinesWidth = m_dataRowGP->borderRight();
    
    int previewCellTop = verticalLayout ? 0 : indentRect().height();
    int previewCellLeft = verticalLayout ? indentRect().width() : 0;

    for (QHash<const GridColumnPrimitive *, GridTableCellGraphic *>::const_iterator it =
        m_dataRowGP->dataCells().constBegin(); it != m_dataRowGP->dataCells().constEnd(); ++it)
    {
        const GridTableColumnPrimitive* columnPrimitive = static_cast<const GridTableColumnPrimitive *>(it.key());
        GridTableCellGraphic* cellPrimitive = it.value();
        const GridDataTableColumn* column = static_cast<GridDataTableColumn *>(columnPrimitive->column());

        cellPrimitive->setVisibility(columnPrimitive->visibility());
 
        if (!cellPrimitive->visibility())
        {
            cellPrimitive->updateButtonLayout();
            continue;
        }
 
        if (autoHeightPtr)
        {
            QRect r = columnPrimitive->cellLayoutRect();
            int topY = 0;
            int index = view()->getColumnRowIndex(column);
            for (int i = 0; i < index; ++i)
            {
                topY += autoHeightPtr[i] + gridLinesHeight;
            }
            r.setTop(topY);
            r.setHeight(autoHeightPtr[index] + gridLinesHeight);
            cellPrimitive->setRect(r);
        }
        else
        {
            QRect r = columnPrimitive->cellLayoutRect();
            if ((type() == FrozenData || type() == NewData) && m_view->hasGroupingIndent(column))
            {
                int gindent = m_view->getRowGroupIndent(Q_NULL);
                r.adjust(-gindent, 0, 0, 0);
            }
            cellPrimitive->setRect(r);
        }

        if (verticalLayout)
        {
            if (cellPrimitive->rect().bottom() > previewCellTop)
            {
                previewCellTop = cellPrimitive->rect().bottom() + 1;
            }
        }
        else
        {
            if (cellPrimitive->rect().right() > previewCellLeft)
            {
                previewCellLeft = cellPrimitive->rect().right() + 1;
            }
        }

        cellPrimitive->updateButtonLayout();
        cellPrimitive->updateFont();

        //Update fixed state cell and cell's buttons.
        bool fixed = column->fixedKind() != FixedNone;
        qtn_do_update_fixed(verticalLayout, fixed, cellPrimitive);
        for (QList<GridTableCellButtonGraphic *>::const_iterator btn_it = cellPrimitive->m_buttons.constBegin(); btn_it != cellPrimitive->m_buttons.constEnd(); ++btn_it)
            qtn_do_update_fixed(verticalLayout, fixed, *btn_it);
    }

    m_dataRowGP->previewGP()->setVisibility(view()->options().isPreviewRowEnabled() && (type() == Data || type() == FrozenData));
    if (m_dataRowGP->previewGP()->visibility())
    {
        if (verticalLayout)
        {
            int height = !view()->options().isPreviewExpandButtonVisible() || gridRow()->isPreviewExpanded() ? view()->options().previewRowHeight() : kGridPreviewExpandButtonHeight;
            height = qRound(height * view()->options().zoomFactor()) + gridLinesHeight;
            m_dataRowGP->previewGP()->setRect(QRect(previewCellLeft, previewCellTop, m_dataRowGP->width() - previewCellLeft, height));
            m_dataRowGP->previewGP()->setBorder(0, 0, 0, gridLinesHeight);
        }
        else
        {
            int width = !view()->options().isPreviewExpandButtonVisible() || gridRow()->isPreviewExpanded() ? view()->options().previewRowHeight() : kGridPreviewExpandButtonHeight;
            width = qRound(width * view()->options().zoomFactor()) + gridLinesWidth;
            m_dataRowGP->previewGP()->setRect(QRect(previewCellLeft, previewCellTop, width, m_dataRowGP->height() - previewCellTop));
            m_dataRowGP->previewGP()->setBorder(0, gridLinesWidth, 0, 0);
        }

        m_dataRowGP->previewGP()->setBorderPen(m_dataRowGP->borderPen());
        m_dataRowGP->previewGP()->updateLayout();
    }
}

void GridTableRowLayoutItem::updateGroupSummaryLayout(bool visible)
{
    GridRowLayoutItem::updateGroupSummaryLayout(visible);
    if (!visible)
        return;

    Q_ASSERT(type() == Summary);

    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    checkMasked(m_summaryRowGP);

    int quickButtonIndent = m_view->layout()->getQuickButtonIndent();

    if (verticalLayout)
        m_summaryRowGP->setRect(rect().adjusted(quickButtonIndent, 0, 0, 0));
    else
        m_summaryRowGP->setRect(rect().adjusted(0, quickButtonIndent, 0, 0));

    m_summarylayoutEngine->updateCellsLayout();
}

void GridTableRowLayoutItem::createDataRowGraphic()
{
    Q_ASSERT(m_dataRowGP == Q_NULL);
    GraphicPrimitive* p = m_rowsLayout->getRowsParent();
    m_dataRowGP = new GridTableDataRowPrimitive(p, this);
}

void GridTableRowLayoutItem::createGroupGraphic()
{
    Q_ASSERT(m_groupRowGP == Q_NULL);
    GraphicPrimitive* p = m_rowsLayout->getRowsParent();
    m_groupRowGP = new GridTableGroupRowPrimitive(p, this);
}

void GridTableRowLayoutItem::createSummaryPanelGraphic()
{
    Q_ASSERT(m_summarylayoutEngine == Q_NULL);
    m_summarylayoutEngine = new GridTableSummaryLayoutEngine(view(), this);

    Q_ASSERT(m_summaryRowGP == Q_NULL);
    GraphicPrimitive* p = m_rowsLayout->getRowsParent();
    m_summaryRowGP = new GridTableSummaryRowPrimitive(p, this);
}


/* GridContextMenu */
GridContextMenu::GridContextMenu(QWidget *parent)
: QMenu(parent), m_hit(Q_NULL)
{
}

void GridContextMenu::show(int gx, int gy, GraphicPrimitive* hit)
{
    m_hit = hit;
    move(gx, gy);
    QMenu::show();
}

GraphicPrimitive* GridContextMenu::hit() const
{
    return m_hit;
}

/*!
  \class GridTableColumnEditor
  \inmodule QtitanDataGrid
  Class allows to add QWidget to the column header. It used as a factory for QWidget creation. Use snippet below to add QPushButton to the column header.

  \code
  class GridCustomColumnEditor: public Qtitan::GridTableColumnEditor
  {
  public:
      GridCustomColumnEditor()
      {
      }

      virtual QWidget* GridCustomColumnEditor::createEditorWidget(Qtitan::GridTableColumnBase* column)
      {
          QPushButton* button = new QPushButton();
          button->setText("text");
          return button;
      }
  };

  ...

  void initColumnEditor()
  {
      Qtitan::GridTableColumn* column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Column");
      GridCustomColumnEditor* columnEditor = new GridCustomColumnEditor();
      column->setColumnEditor(columnEditor);
  }

  void freeColumnEditor()
  {
      Qtitan::GridTableColumn* column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Column");
      GridCustomColumnEditor* columnEditor = column->columnEditor();
      column->setColumnEditor(0);
      delete columnEditor;
  }
  \endcode
*/

/*!
Contructor of GridTableColumnEditor
*/
GridTableColumnEditor::GridTableColumnEditor()
: QObject()
{
}

/*!
Destructor of GridTableColumnEditor
*/
GridTableColumnEditor::~GridTableColumnEditor()
{
}

/*!
\fn QWidget* GridTableColumnEditor::createEditorWidget(GridColumnBase* column);
Create a widget that will be used in \a column header. You do not have to worry about deleting the widget. The owner of the widget to become a column header. 
The widget will be deleted when you remove a column or will be reset to null an column editor - column->setColumnEditor(0).
*/



/* GridCellButtonStyleOption */
GridCellButtonStyleOption::GridCellButtonStyleOption(int version, int type)
: QStyleOption(version, type)
{
}

/*!
  \class GridCustomDrawEventArgs
  \inmodule QtitanDataGrid
*/
GridCustomDrawEventArgs::GridCustomDrawEventArgs(QPainter* p, GridCellButtonStyleOption* opt)
: BaseEventArgs(), m_painter(p), m_option(opt)
{
}

QPainter* GridCustomDrawEventArgs::painter() const
{
    return m_painter;
}

GridCellButtonStyleOption* GridCustomDrawEventArgs::option() const
{
    return m_option;
}

/*!
  \class CellButtonEventArgs
  \inmodule QtitanDataGrid
*/
CellButtonEventArgs::CellButtonEventArgs(GridRow* row, int columnIndex, int buttonIndex)
    : m_buttonIndex(buttonIndex), m_enabled(true)
{
    m_cell = new GridCell(row, columnIndex);
}

CellButtonEventArgs::~CellButtonEventArgs()
{
    Q_DELETE_AND_NULL(m_cell);
}

GridRow* CellButtonEventArgs::row() const
{
    return m_cell->row();
}

int CellButtonEventArgs::columnIndex() const
{
    return m_cell->columnIndex();
}

QModelIndex CellButtonEventArgs::modelIndex() const
{
    return m_cell->modelIndex();
}

int CellButtonEventArgs::buttonIndex() const
{
    return m_buttonIndex;
}

bool CellButtonEventArgs::isEnadled() const
{
    return m_enabled;
}

void CellButtonEventArgs::setEnadled(bool enabled)
{
    m_enabled = enabled;
}

/*!
  \class CellButtonClickEventArgs
  \inmodule QtitanDataGrid
*/
CellButtonClickEventArgs::CellButtonClickEventArgs(int index, GridTableColumn* c, GridRow* row)
    : BaseEventArgs(), m_buttonIndex(index), m_column(c), m_row(row)
{
}

int CellButtonClickEventArgs::buttonIndex() const
{
    return m_buttonIndex;
}

GridTableColumn* CellButtonClickEventArgs::column() const
{
    return m_column;
}

GridRow* CellButtonClickEventArgs::row() const
{
    return m_row;
}


/* GridTableSummaryLayoutEngine */
GridTableSummaryLayoutEngine::GridTableSummaryLayoutEngine(GridTableView* view, GridRowLayoutItem* row)
    : GridSummaryLayoutEngine(view, row)
{
}
GridTableSummaryLayoutEngine::~GridTableSummaryLayoutEngine()
{
}

void GridTableSummaryLayoutEngine::updateCellsLayout()
{
    int groupingIndent = getGroupingIndent();
    int groupRowIndent = getGroupRowIndent();
    bool verticalLayout = view()->layout()->orientation() == Qt::Vertical;

    int columnXOffset = verticalLayout ? view()->layout()->getQuickButtonIndent() : 0;
    int columnYOffset = verticalLayout ? 0 : view()->layout()->getQuickButtonIndent();

    bool group_row = isGroupRow();
    if (!group_row)
    {
        if (verticalLayout)
            columnXOffset = 0;
        else
            columnYOffset = 0;
    }


    int cellBorderRight = 0, cellBorderBottom = 0;
    bool _2d = false;
    QPen cellBorderPen = view()->optionsref()->gridLinePen();
    bool layoutWithBorder = group_row && view()->optionsref()->groupSummaryPlace() == GroupSummaryRow;
    m_paintGroupText = !layoutWithBorder;

    if (layoutWithBorder)
    {
        get_cell_border_lines(view(), cellBorderRight, cellBorderBottom, _2d);
        GridGroupRowPrimitive* groupRowPrimitive = static_cast<GridGroupRowPrimitive *>(parentGP());
        groupingIndent += groupRowPrimitive->branchButon()->width();
    }

    //TODO don't use banded view
    GridBandedTableView* bandedView = qobject_cast<GridBandedTableView *>(view());
    bool ignoreRowSpanForCells = bandedView != Q_NULL && bandedView->options().ignoreRowSpanForCells();

    int defCellLength = verticalLayout ? get_cell_height(view()->grid()->style(), view()->optionsref()) : get_cell_width(view()->grid()->style(), view()->optionsref());

    for (QHash<const GridDataTableColumn *, GridSummaryCellPrimitive *>::const_iterator it =
        m_summaryCells.constBegin();
        it != m_summaryCells.constEnd(); ++it)
    {
        const GridTableColumn* tableColumn = static_cast<const GridTableColumn *>(it.key());
        GridSummaryCellPrimitive* cell = it.value();
        const GridTableColumnPrimitive* columnPrimitive = tableColumn->columnGP();

        bool visible = view()->isColumnVisible(tableColumn);
        cell->setVisibility(visible);
        if (!visible)
            continue;

        qtn_do_update_fixed(verticalLayout, columnPrimitive->column()->fixedKind() != FixedNone, cell);

        QRect columnRect = columnPrimitive->rect();
        columnRect.translate(-columnXOffset, -columnYOffset);

        if (verticalLayout)
        {
            if (view()->hasGroupingIndent(tableColumn))
            {
                cell->m_hasGroupText = layoutWithBorder;
                columnRect.adjust(groupingIndent, 0, 0, 0);
            }
            else
                cell->m_hasGroupText = false;
        }
        else
        {
            if (view()->hasGroupingIndent(tableColumn))
            {
                cell->m_hasGroupText = layoutWithBorder;
                columnRect.adjust(0, groupingIndent, 0, 0);
            }
            else
                cell->m_hasGroupText = false;
        }

        cell->setBorder(0, cellBorderRight, 0, cellBorderBottom);
        cell->setBorderPen(cellBorderPen);

        int columnRowSpan = 1;
        if (!ignoreRowSpanForCells)
            columnRowSpan = tableColumn->rowSpan();

        int gridLinesWidth = layoutWithBorder ? cellBorderRight : kGridSummaryCellBorder;
        int gridLinesHeight = layoutWithBorder ? cellBorderBottom : kGridSummaryCellBorder;

        if (verticalLayout)
        {
            columnRect.setHeight((defCellLength + gridLinesHeight) * columnRowSpan);
            int y = columnPrimitive->layouterRow() * (defCellLength + gridLinesHeight);
            columnRect.moveTop(groupRowIndent + y);
            //columnRect.setHeight(columnRect.height() + columnRowSpan * kGridSummaryCellBorder);
            //columnRect.setWidth(columnRect.width() - 1);
        }
        else
        {
            if (defCellLength != -1)
            {
                columnRect.setWidth(defCellLength + gridLinesWidth);
                int x = columnPrimitive->layouterRow() * (defCellLength + gridLinesWidth);
                columnRect.moveLeft(groupRowIndent + x);
            }
            else
            {
                columnRect.setWidth(columnRect.width() + gridLinesWidth);
                int x = columnRect.left() + columnPrimitive->layouterRow() * gridLinesWidth;
                columnRect.moveLeft(groupRowIndent + x);
            }
        }

        cell->setRect(columnRect);
        cell->setTableColumn(tableColumn);
    }
}

/* GridTableFooterSummaryPrimitive */
GridTableFooterSummaryPrimitive::GridTableFooterSummaryPrimitive(GraphicPrimitive* parent, GridTableView* view)
    : GridFooterSummaryPrimitive(parent)
{
    m_layoutEngine = new GridTableSummaryLayoutEngine(view);
    m_layoutEngine->setParentPrimitive(this);
}

GridTableFooterSummaryPrimitive::~GridTableFooterSummaryPrimitive()
{
}
