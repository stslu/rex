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

#include <QtCore/qmath.h>

#include "QtnGridCardView.h"
#include "QtnGridViewGraphics.h"


QTITAN_BEGIN_NAMESPACE

class GridTableViewEx : public GridTableView
{
    friend class GridTableGroupSummaryGraphic;
    GridTableViewEx() : GridTableView(0) {}
};


class GraphicControlEx : public GraphicControl
{
    friend class GridCellDragObject;
    GraphicControlEx() : GraphicControl(0) {}
};

class GridCardSummaryLayoutEngine : public GridSummaryLayoutEngine
{
public:
    GridCardSummaryLayoutEngine(GridCardView* view, GridRowLayoutItem* row = Q_NULL);
    virtual ~GridCardSummaryLayoutEngine();
    virtual void updateCellsLayout();
};

class GridCardFooterSummaryPrimitive : public GridFooterSummaryPrimitive
{
public:
    GridCardFooterSummaryPrimitive(GraphicPrimitive* parent, GridCardView* view);
    virtual ~GridCardFooterSummaryPrimitive();
};

QTITAN_END_NAMESPACE

static GridCardViewPrimitive* get_view_layout(GridViewBase* view)
{
    return static_cast<GridCardViewPrimitive *>(view->layout());
}

/*!
\class GridCardViewOptions
GridCardViewOptions contains a set of settings that allow to configure CardView layout.
\inmodule QtitanDataGrid
*/
GridCardViewOptions::GridCardViewOptions(GridCardView* view)
    : GridViewOptions(view), m_minimumItemCount(2), m_maximumItemCount(10),
    m_itemAlignment(Qt::AlignLeft), m_itemMargin(10), m_itemPadding(10), m_itemWidth(250), m_itemTitleHeight(30)
{
    m_cellWidth = 100;
    m_itemTitleText = Grid::tr_compatible(QtnGridCardItemTitleRecordString);
}

/*!
Returns the minimum count of cards per line in card view layout.
*/
int GridCardViewOptions::minimumItemCount() const
{
    return m_minimumItemCount;
}

/*!
Sets the minimum \a count of cards per line in card view layout. By default 2 cards.
*/
void GridCardViewOptions::setMinimumItemCount(int count)
{
    if (m_minimumItemCount == count)
        return;
    m_minimumItemCount = count;
    if (m_minimumItemCount < 1)
        m_minimumItemCount = 1;
    view()->requestUpdateLayout();
}

/*!
Returns the maximum count of cards per line in card view layout.
*/
int GridCardViewOptions::maximumItemCount() const
{
    return m_maximumItemCount;
}

/*!
Sets the maximum \a count of cards per line in card view layout. By default 10 cards.
*/
void GridCardViewOptions::setMaximumItemCount(int count)
{
    if (m_maximumItemCount == count)
        return;
    m_maximumItemCount = count;
    if (m_maximumItemCount < 0)
        m_maximumItemCount = -1;
    view()->requestUpdateLayout();
}

/*!
Returns the alignment of the cards if card view layout has a free space.
*/
Qt::Alignment GridCardViewOptions::itemAlignment() const
{
    return m_itemAlignment;
}

/*!
Sets the \a alignment of the cards if card view layout has a free space. By default Left alignment is used for vertical orientation and Top alignment for horizontal.
*/
void GridCardViewOptions::setItemAlignment(Qt::Alignment alignment)
{
    if (m_itemAlignment == alignment)
        return;
    m_itemAlignment = alignment;
    view()->requestUpdateLayout();
}

/*!
Returns the margin in pixels between neighboring cards.
*/
int GridCardViewOptions::itemMargin() const
{
    return m_itemMargin;
}

/*!
Sets the \a margin in pixels between neighboring cards.
*/
void GridCardViewOptions::setItemMargin(int margin)
{
    if (m_itemMargin == margin)
        return;
    m_itemMargin = margin;
    view()->requestUpdateLayout();
}

/*!
Returns the padding in pixels between edge of the cards and its content.
*/
int GridCardViewOptions::itemPadding() const
{
    return m_itemPadding;
}

/*!
Sets the \a padding in pixels between edge of the cards and its content.
*/
void GridCardViewOptions::setItemPadding(int padding)
{
    if (m_itemPadding == padding)
        return;
    m_itemPadding = padding;
    view()->requestUpdateLayout();
}

/*!
Returns the width of the cards in pixels.
*/
int GridCardViewOptions::itemWidth() const
{
    return m_itemWidth;
}

/*!
Sets the \a width of the cards in pixels.
*/
void GridCardViewOptions::setItemWidth(int width)
{
    if (m_itemWidth == width)
        return;
    m_itemWidth = width;
    view()->requestUpdateLayout();
}

/*!
Returns the height of the card's title in pixels.
*/
int GridCardViewOptions::itemTitleHeight() const
{
    return m_itemTitleHeight;
}

/*!
Sets the \a height of the card's title in pixels.
*/
void GridCardViewOptions::setItemTitleHeight(int height)
{
    if (m_itemTitleHeight == height)
        return;
    m_itemTitleHeight = height;
    view()->requestUpdateLayout();
}

/*!
Returns the text of the card's title used before card number.
*/
const QString& GridCardViewOptions::itemTitleText() const
{
    return m_itemTitleText;
}

/*!
Sets the \a text of the card's title used before card number.
*/
void GridCardViewOptions::setItemTitleText(const QString& text)
{
    m_itemTitleText = text;
}

/*!
\class GridCardView
GridCardView represents the CardView layout used in grid.
\inmodule QtitanDataGrid
*/
GridCardView* GridCardView::create(Grid* grid)
{
    GridCardView* retval = new GridCardView(grid);
    retval->initialize();
    return retval;
}
 
GridCardView::GridCardView(Grid* grid)
  : GridViewBase(grid)
{
}

GridCardView::~GridCardView()
{
    m_viewStates = ViewDestroying;
}

GridNativeDragObject* GridCardView::createNativeDragDropObject(const QPoint& pos)
{
    Q_ASSERT(false);
    Q_UNUSED(pos);
    return Q_NULL;
}

GridCardViewOptions& GridCardView::options() const
{
    return *(static_cast<GridCardViewOptions *>(optionsref()));
}

void GridCardView::navigateUp(Qt::KeyboardModifiers modifiers)
{
    doNavigatePrevColumn(modifiers);
}

void GridCardView::navigateDown(Qt::KeyboardModifiers modifiers)
{
    doNavigateNextColumn(modifiers);
}

void GridCardView::navigateLeft(Qt::KeyboardModifiers modifiers)
{
    doNavigatePrevRow(modifiers);
}

void GridCardView::navigateRight(Qt::KeyboardModifiers modifiers)
{
    doNavigateNextRow(modifiers);
}

void GridCardView::initialize()
{
    m_fieldProviders.push_front(new GridFieldChooserCardColumnProvider(this));
    GridViewBase::initialize();
}

GridViewOptions* GridCardView::createOptions()
{
    return new GridCardViewOptions(this);
}

GridViewPrimitiveBase* GridCardView::createLayout()
{
    return new GridCardViewPrimitive(grid());
}

GridColumnBase* GridCardView::createColumn(int modelColumn, const QString& caption)
{
    return new GridCardColumn(this, modelColumn, caption);
}

GridTableHeaderGraphic* GridCardView::createHeaderPanel(GraphicPrimitive* parent)
{
    Q_UNUSED(parent);
    return Q_NULL;
}

GridRowsPrimitiveAbstract* GridCardView::createRowsGraphic(GraphicPrimitive* parent)
{
    return new GridCardRowsPrimitive(this, parent);
}

GridFooterSummaryPrimitive* GridCardView::createFooterSummaryPanel(GraphicPrimitive* parent)
{
    return new GridCardFooterSummaryPrimitive(parent, this);
}

void GridCardView::bestFitForView()
{
    for (GridViewBase::GridColumns::const_iterator it = m_columnslist.constBegin(); it != m_columnslist.constEnd(); ++it)
    {
        const GridDataTableColumn* column = static_cast<const GridDataTableColumn *>(*it);
        if (!isColumnVisible(column))
            continue;
        GridCardColumnPrimitive* primitive = static_cast<GridCardRowLayoutItem *>(layout()->rowsLayoutEngine()->focusedRowLayoutItem())->primitiveForColumn(column);
        primitive->bestFit();
    }
}

bool GridCardView::printHeader(QPainter& painter, int& pageLength, int& itemIndex)
{
    Q_UNUSED(painter);
    GridCardViewPrimitive* l = static_cast<GridCardViewPrimitive *>(layout());
    int groupIndent = getRowGroupIndent(Q_NULL);
    int rowSize = l->calculateGridRowSize(Q_NULL);
    pageLength -= groupIndent;
    int count = qFloor(pageLength / rowSize);
    int itemsPerLine = l->getItemsPerLine();
    if (itemIndex == -1)
        itemIndex = 0;
    itemsPerLine -= itemIndex;

    count = qMin(count, itemsPerLine);
    pageLength -= rowSize * count;
    itemIndex += count;
    return count >= itemsPerLine;
}

void GridCardView::showColumnOnScreen(int oldColumnIndex, int columnIndex)
{
    Q_UNUSED(oldColumnIndex);
    Q_UNUSED(columnIndex);
}

/*!
\class GridCardColumn
\inmodule QtitanDataGrid
*/
GridCardColumn::GridCardColumn(GridCardView* view, int modelColumn, const QString& caption)
: GridDataTableColumn(view, modelColumn, caption)
{
}

GridCardColumn::~GridCardColumn()
{
}

GridTableDragColumnObject* GridCardColumn::createDragDropColumnObject(const QPoint& point, GraphicPrimitive* graphic)
{
    return new GridCardColumnDragObject(point, this, graphic);
}

bool GridCardColumn::needHeaderHints() const
{
    return true;
}

void GridCardColumn::initStyleOption(QStyleOptionHeader& styleOption)
{
    GridDataTableColumn::initStyleOption(styleOption);
}

/* GridCardColumnDragObject */
GridCardColumnDragObject::GridCardColumnDragObject(const QPoint& point, GridTableColumnBase* column, GraphicPrimitive* source)
    : GridTableDragColumnObject(point, column, source)
{
}

bool GridCardColumnDragObject::columnsHasVerticalLayout() const
{
    return false;
}

void GridCardColumnDragObject::checkScrollToCursor(const QPoint& curPos)
{
    Q_UNUSED(curPos);
    //Do nothing.
}

/* GridFieldChooserCardColumnProvider */
GridFieldChooserCardColumnProvider::GridFieldChooserCardColumnProvider(GridViewBase* view)
    : GridFieldChooserColumnProvider(view)
{
}

GridViewBase::GridColumns GridFieldChooserCardColumnProvider::fields() const
{
    return view()->m_columnVisualIndexies;
}

/* GridCardRowsPrimitive */
GridCardRowsPrimitive::GridCardRowsPrimitive(GridViewBase* view, GraphicPrimitive* parent)
    : GridRowsPrimitiveAbstract(view, parent)
{
    m_newRowPanelGP = new GridCardNewRowPrimitive(view, this);
    m_newRowPanelGP->setVisibility(false);

    m_layoutEngine = new GridCardRowsLayoutEngine(static_cast<GridCardView *>(view), this);
    m_layoutEngine->initialize();
}

GridCardRowsPrimitive::~GridCardRowsPrimitive()
{
}

void GridCardRowsPrimitive::updateLayout()
{
    updateFixedLayout();
    GridRowsPrimitiveAbstract::updateLayout();
}

void GridCardRowsPrimitive::calculateFixedRows()
{
    GridRowsPrimitiveAbstract::calculateFixedRows();
    if (m_view->optionsref()->newRowPlace() == NewRowTop)
    {
        m_topFixedIndent += m_view->layout()->calculateNewRowLength();
        m_topFixedIndent += kGridNewRowIndent;
    }

    if (m_view->optionsref()->newRowPlace() == NewRowBottom)
    {
        m_bottomFixedIndent += m_view->layout()->calculateNewRowLength();
        m_bottomFixedIndent += kGridNewRowIndent;
    }
}

int GridCardRowsPrimitive::scrollX() const
{
    if (m_view->layout()->orientation() == Qt::Vertical)
        return m_view->layout()->scrollX();
    else
        return 0;
}

int GridCardRowsPrimitive::scrollY() const
{
    if (m_view->layout()->orientation() != Qt::Vertical)
        return m_view->layout()->scrollY();
    else
        return 0;
}

GridRowsLayoutEngine* GridCardRowsPrimitive::createPrinterLayoutEngine() const
{
    GridRowsLayoutEngine* ret = new GridCardRowsLayoutEngine(qobject_cast<GridCardView *>(m_view), Q_NULL);
    ret->initialize();
    return ret;
}

/* GridCardNewRowPrimitive */
GridCardNewRowPrimitive::GridCardNewRowPrimitive(GridViewBase* view, GraphicPrimitive* parent)
    : GridNewRowPrimitive(view, parent)
{
}

QRect GridCardNewRowPrimitive::getLayoutItemRect()
{
    QSize s(get_view_layout(m_view)->calculateGridDataRowWidth(Q_NULL),
        get_view_layout(m_view)->calculateGridDataRowHeight(Q_NULL));
    return QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, s, rect());
}

/* GridCardRowLayoutItem */
GridCardRowLayoutItem::GridCardRowLayoutItem(GridCardRowsLayoutEngine* rowsLayout)
    : GridRowLayoutItem(rowsLayout), m_alignOffset(0), m_autoRowOffset(0)
{
}

GridCardRowLayoutItem::~GridCardRowLayoutItem()
{
}

void GridCardRowLayoutItem::create()
{
    GridRowLayoutItem::create();
}

GridCardView* GridCardRowLayoutItem::view() const
{
    return static_cast<GridCardView *>(m_view);
}

bool GridCardRowLayoutItem::isFirstItem()
{
    if (m_view->layout()->orientation() == Qt::Vertical)
        return rect().left() == 0;
    else
        return rect().top() == 0;
}

int GridCardRowLayoutItem::alignOffset() const
{
    return m_alignOffset;
}

int GridCardRowLayoutItem::autoRowOffset() const
{
    return m_autoRowOffset;
}

GridCardColumnPrimitive* GridCardRowLayoutItem::primitiveForColumn(const GridDataTableColumn* column)
{
    Q_ASSERT(static_cast<GridCardDataRowPrimitive *>(m_dataRowGP)->m_columnGPs.contains(column));
    return static_cast<GridCardDataRowPrimitive *>(m_dataRowGP)->m_columnGPs[column];
}

void GridCardRowLayoutItem::addColumn(const GridDataTableColumn* column)
{
    GridCardColumnPrimitive* columnPrimitive = new GridCardColumnPrimitive(m_dataRowGP, const_cast<GridDataTableColumn *>(column));
    static_cast<GridCardDataRowPrimitive *>(m_dataRowGP)->m_columnGPs.insert(column, columnPrimitive);
    m_dataRowGP->registrColumnGraphic(columnPrimitive);
    m_summarylayoutEngine->addColumn(column);
}

void GridCardRowLayoutItem::removeColumn(const GridDataTableColumn* column)
{
    QHash<const GridDataTableColumn *, GridCardColumnPrimitive *>::iterator it = static_cast<GridCardDataRowPrimitive *>(m_dataRowGP)->m_columnGPs.find(column);
    Q_ASSERT(it != static_cast<GridCardDataRowPrimitive *>(m_dataRowGP)->m_columnGPs.end());
    if (it != static_cast<GridCardDataRowPrimitive *>(m_dataRowGP)->m_columnGPs.end())
    {
        m_dataRowGP->unregistrColumnGraphic(it.value());
        delete it.value();
        static_cast<GridCardDataRowPrimitive *>(m_dataRowGP)->m_columnGPs.erase(it);
    }
    m_summarylayoutEngine->removeColumn(column);
}


void GridCardRowLayoutItem::updateDataRowLayout(bool visible)
{
    m_alignOffset = 0;
    m_autoRowOffset = 0;
    GridRowLayoutItem::updateDataRowLayout(visible);
    if (!visible)
        return;

    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    int quickButtonIndent = 0;// m_view->layout()->getQuickButtonIndent();

    if (isFirstItem())
    {
        int groupIndent = view()->getRowGroupIndent(gridRow());
        int rowColumnLength = get_view_layout(m_view)->calculateGridRowSize(gridRow());
        if (verticalLayout)
            m_alignOffset = rect().width() - groupIndent - rowColumnLength;
        else
            m_alignOffset = rect().height() - groupIndent - rowColumnLength;
    }

    if (verticalLayout)
        m_dataRowGP->setRect(rect().adjusted(quickButtonIndent, 0, 0, 0));
    else
        m_dataRowGP->setRect(rect().adjusted(0, quickButtonIndent, 0, 0));

    m_dataRowGP->previewGP()->setVisibility(false);// view()->options().isPreviewRowEnabled() && (type() == Data || type() == FrozenData));

    GridCardDataRowPrimitive* cardDataRowPrimitive = static_cast<GridCardDataRowPrimitive *>(m_dataRowGP);

    QRect rect = cardDataRowPrimitive->paintContentRect(false);

    const int padding = view()->options().itemPadding();
    rect.adjust(padding, padding, -padding, -padding);
    QRect removeButtonRect = rect.adjusted(0, 0, -5, 0);
    removeButtonRect.setHeight(view()->options().itemTitleHeight());

    removeButtonRect = QStyle::alignedRect(m_view->grid()->layoutDirection(), Qt::AlignVCenter | Qt::AlignRight, 
        cardDataRowPrimitive->m_removeButtonPrimitive->sizeHint(), removeButtonRect);
    cardDataRowPrimitive->m_removeButtonPrimitive->setRect(removeButtonRect);

    int offset = view()->options().itemTitleHeight() * view()->options().zoomFactor();
    int defCellHeight = get_cell_height(m_view->grid()->style(), m_view->optionsref());

    const bool autoHeightEnabled = (view()->layout()->autoHeightRowCount() > 0) && (type() == Data || type() == FrozenData);
    const int* autoHeightPtr = autoHeightEnabled ? &view()->layout()->autoHeightInfo()[view()->layout()->autoHeightRowCount() * gridRow()->rowIndex()] : Q_NULL;
    int rowIndex = 0;
    for (GridViewBase::GridColumns::const_iterator it = view()->m_columnVisualIndexies.constBegin(); it != view()->m_columnVisualIndexies.constEnd(); ++it)
    {
        GridDataTableColumn* column = static_cast<GridDataTableColumn *>(*it);
        Q_ASSERT(static_cast<GridCardDataRowPrimitive *>(m_dataRowGP)->m_columnGPs.contains(column));
        GridCardColumnPrimitive* columnPrimitive = static_cast<GridCardDataRowPrimitive *>(m_dataRowGP)->m_columnGPs[column];

        Q_ASSERT(cardDataRowPrimitive->m_dataCells.contains(columnPrimitive));
        GridTableCellGraphic* cellPrimitive = cardDataRowPrimitive->m_dataCells[columnPrimitive];

        columnPrimitive->setVisibility(view()->isColumnVisible(column));
        cellPrimitive->setVisibility(columnPrimitive->visibility());
        if (!cellPrimitive->visibility())
        {
            cellPrimitive->updateButtonLayout();
            continue;
        }


        QRect columnRect = QRect(rect.left(), rect.top() + offset, static_cast<GridCardViewPrimitive *>(view()->layout())->itemColumnWidth(), defCellHeight * columnPrimitive->column()->rowSpan());
        if (autoHeightPtr)
        {
            columnRect.setHeight(autoHeightPtr[rowIndex]);
        }

        columnPrimitive->setRect(columnRect);
        columnRect.translate(columnRect.width(), 0);
        columnRect.setWidth(static_cast<GridCardViewPrimitive *>(view()->layout())->itemCellWidth());
        cellPrimitive->setRect(columnRect);
        cellPrimitive->updateButtonLayout();
        cellPrimitive->updateFont();

        offset += columnRect.height();
        rowIndex++;
    }
    m_autoRowOffset = rect.height() - offset;
}

void GridCardRowLayoutItem::updateGroupRowLayout(bool visible)
{
    GridRowLayoutItem::updateGroupRowLayout(visible);
    if (!visible)
        return;
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    groupRowGP()->setRect(rect());

    if (verticalLayout)
    {
        int height = get_group_height(m_view->grid()->style(), m_view->optionsref());
        QRect r = QStyle::alignedRect(m_view->grid()->layoutDirection(), Qt::AlignVCenter | Qt::AlignHCenter, QSize(kGridGroupRowIndent, kGridGroupRowIndent),
            QRect(indentRect().width(), 0, kGridGroupRowIndent, height));
        groupRowGP()->branchButon()->setRect(r);
    }
    else
    {
        int width = get_group_height(m_view->grid()->style(), m_view->optionsref());
        QRect r = QStyle::alignedRect(m_view->grid()->layoutDirection(), Qt::AlignVCenter | Qt::AlignVCenter, QSize(kGridGroupRowIndent, kGridGroupRowIndent),
            QRect(0, indentRect().height(), width, kGridGroupRowIndent));
        groupRowGP()->branchButon()->setRect(r);
    }
}

void GridCardRowLayoutItem::updateGroupSummaryLayout(bool visible)
{
    GridRowLayoutItem::updateGroupSummaryLayout(visible);
    if (!visible)
        return;

    Q_ASSERT(type() == Summary);

    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;

    int quickButtonIndent = 0;// m_view->layout()->getQuickButtonIndent();

    if (verticalLayout)
        summaryRowGP()->setRect(rect().adjusted(quickButtonIndent, 0, 0, 0));
    else
        summaryRowGP()->setRect(rect().adjusted(0, quickButtonIndent, 0, 0));

    m_summarylayoutEngine->updateCellsLayout();
}

void GridCardRowLayoutItem::createDataRowGraphic()
{
    Q_ASSERT(m_dataRowGP == Q_NULL);
    GraphicPrimitive* p = m_rowsLayout->getRowsParent();
    m_dataRowGP = new GridCardDataRowPrimitive(p, this);
}

void GridCardRowLayoutItem::createGroupGraphic()
{
    Q_ASSERT(m_groupRowGP == Q_NULL);
    GraphicPrimitive* p = m_rowsLayout->getRowsParent();
    m_groupRowGP = new GridCardGroupRowPrimitive(p, this);
}

void GridCardRowLayoutItem::createSummaryPanelGraphic()
{
    Q_ASSERT(m_summarylayoutEngine == Q_NULL);
    m_summarylayoutEngine = new GridCardSummaryLayoutEngine(view(), this);

    Q_ASSERT(m_summaryRowGP == Q_NULL);
    GraphicPrimitive* p = m_rowsLayout->getRowsParent();
    m_summaryRowGP = new GridCardSummaryRowPrimitive(p, this);
}

/* GridCardRowsLayoutEngine */
GridCardRowsLayoutEngine::GridCardRowsLayoutEngine(GridCardView* view, GraphicPrimitive* parent)
    : GridRowsLayoutEngine(view, parent), m_startNewItemIndex(-1), m_currentMaxRowLength(0)
{
}

GridCardRowsLayoutEngine::~GridCardRowsLayoutEngine()
{
}

GridCardView* GridCardRowsLayoutEngine::view() const
{
    return static_cast<GridCardView *>(m_view);
}

void GridCardRowsLayoutEngine::beginUpdate(int columnsLength, int rowsLength, int offsetY, bool forceUpdateLayout)
{
    GridRowsLayoutEngine::beginUpdate(columnsLength, rowsLength, offsetY, forceUpdateLayout);

    QRect scrollAreaRect = m_view->layout()->rowsScrollAreaRect();
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    //m_currentTopFrozenRowRect = QRect(0, 0, 0, 0);
    //m_currentBottomFrozenRowRect = QRect(0, 0, 0, 0);

    if (verticalLayout)
    {
        if (isPrintLayout())
            scrollAreaRect.setTop(0);
        m_currentRowRect = QRect(0, scrollAreaRect.top() - offsetY, columnsLength, 0);

        m_currentTopFrozenRowRect = QRect(0, 0, columnsLength, 0);
        if (!isPrintLayout() && view()->options().newRowPlace() == NewRowTop)
            m_currentTopFrozenRowRect.translate(0, view()->layout()->calculateNewRowLength() + kGridNewRowIndent);
        m_currentBottomFrozenRowRect = QRect(0, scrollAreaRect.top() + scrollAreaRect.height() + kGridNewRowIndent, columnsLength, 0);
    }
    else
    {
        if (isPrintLayout())
            scrollAreaRect.setLeft(0);
        m_currentRowRect = QRect(scrollAreaRect.left() - offsetY, 0, 0, columnsLength);

        m_currentTopFrozenRowRect = QRect(0, 0, 0, columnsLength);
        if (!isPrintLayout() && view()->options().newRowPlace() == NewRowTop)
            m_currentTopFrozenRowRect.translate(view()->layout()->calculateNewRowLength() + kGridNewRowIndent, 0);
        m_currentBottomFrozenRowRect = QRect(scrollAreaRect.left() + scrollAreaRect.width() + kGridNewRowIndent, 0, 0, columnsLength);
    }
 
    m_startNewItemIndex = 0;
    m_currentMaxRowLength = 0;
}

void GridCardRowsLayoutEngine::endUpdate()
{
    alignItems();
    GridRowsLayoutEngine::endUpdate();
}

void GridCardRowsLayoutEngine::editorChanging(const GridDataTableColumn* column, bool reset)
{
    for (QVector<GridRowLayoutItem *>::iterator it =
        m_virtualRows.begin(); it != m_virtualRows.end(); ++it)
    {
        GridCardDataRowPrimitive* cardDataRowPrimitive = static_cast<GridCardDataRowPrimitive *>((*it)->dataRowGP());
        QHash<const GridDataTableColumn *, GridCardColumnPrimitive *>::iterator column_it = cardDataRowPrimitive->m_columnGPs.find(column);
        if (column_it == cardDataRowPrimitive->m_columnGPs.end())
            continue;
        (*it)->editorChanging(column_it.value(), reset);
    }

    GridCardDataRowPrimitive* cardDataRowPrimitive = static_cast<GridCardDataRowPrimitive *>((m_focusedRowLayoutItem)->dataRowGP());
    QHash<const GridDataTableColumn *, GridCardColumnPrimitive *>::iterator column_it = cardDataRowPrimitive->m_columnGPs.find(column);
    if (column_it == cardDataRowPrimitive->m_columnGPs.end())
        return;
    m_focusedRowLayoutItem->editorChanging(column_it.value(), reset);

}

void GridCardRowsLayoutEngine::updateCellButtons(const GridDataTableColumn* column)
{
    for (QVector<GridRowLayoutItem *>::iterator it =
        m_virtualRows.begin(); it != m_virtualRows.end(); ++it)
    {
        GridCardDataRowPrimitive* cardDataRowPrimitive = static_cast<GridCardDataRowPrimitive *>((*it)->dataRowGP());
        QHash<const GridDataTableColumn *, GridCardColumnPrimitive *>::iterator column_it = cardDataRowPrimitive->m_columnGPs.find(column);
        if (column_it == cardDataRowPrimitive->m_columnGPs.end())
            continue;
        (*it)->updateCellButtons(column_it.value());
    }

    GridCardDataRowPrimitive* cardDataRowPrimitive = static_cast<GridCardDataRowPrimitive *>((m_focusedRowLayoutItem)->dataRowGP());
    QHash<const GridDataTableColumn *, GridCardColumnPrimitive *>::iterator column_it = cardDataRowPrimitive->m_columnGPs.find(column);
    if (column_it == cardDataRowPrimitive->m_columnGPs.end())
        return;
    m_focusedRowLayoutItem->updateCellButtons(column_it.value());
}

GridRowLayoutItem* GridCardRowsLayoutEngine::createItem()
{
    return new GridCardRowLayoutItem(this);
}

bool GridCardRowsLayoutEngine::appendNextGridRowRect(GridRow* gridRow, bool summary, GridRow::FrozenPlace frozenPlace, QRect& newRect)
{
    Q_UNUSED(frozenPlace);
    Q_ASSERT(frozenPlace == GridRow::FrozenNone);
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    int rowLength = 0;
    int rowColumnLength = 0;
    int groupIndent = 0;
    if (summary)
    {
        rowColumnLength = m_viewColumnsLength;
        rowLength = m_view->layout()->calculateGridSummaryRowLength(gridRow);
    }
    else
    {
        if (gridRow->type() == GridRow::DataRow)
        {
            groupIndent = view()->getRowGroupIndent(gridRow);
            rowColumnLength = get_view_layout(m_view)->calculateGridRowSize(gridRow);
        }
        else
            rowColumnLength = m_viewColumnsLength;
        rowLength = m_view->layout()->calculateGridRowLength(gridRow, false);
    }

    //int viewColumnsLength = m_viewColumnsLength - groupIndent;
    bool nextRow = false;
    if (verticalLayout)
    {
        nextRow = m_currentRowRect.left() + rowColumnLength > m_viewColumnsLength;
        if (nextRow)
        {
            m_currentRowRect.setLeft(0);
            m_currentRowRect.translate(0, m_currentMaxRowLength);// m_currentRowRect.height());
            m_viewRowsLength -= m_currentMaxRowLength;
            m_currentMaxRowLength = 0;
        }

        if (gridRow->type() == GridRow::DataRow && m_currentRowRect.left() == 0)
        {
            rowColumnLength += groupIndent;
        }

        m_currentRowRect.setHeight(rowLength);
        m_currentRowRect.setWidth(rowColumnLength);

        newRect = m_currentRowRect;
        m_currentRowRect.translate(rowColumnLength, 0);
    }
    else
    {
        nextRow = m_currentRowRect.top() + rowColumnLength > m_viewColumnsLength;
        if (nextRow)
        {
            m_currentRowRect.setTop(0);
            m_currentRowRect.translate(m_currentMaxRowLength /*m_currentRowRect.width()*/, 0);
            m_viewRowsLength -= m_currentMaxRowLength;
            m_currentMaxRowLength = 0;
        }

        if (gridRow->type() == GridRow::DataRow && m_currentRowRect.top() == 0)
        {
            rowColumnLength += groupIndent;
        }

        m_currentRowRect.setWidth(rowLength);
        m_currentRowRect.setHeight(rowColumnLength);

        newRect = m_currentRowRect;
        m_currentRowRect.translate(0, rowColumnLength);
    }
    m_currentMaxRowLength = qMax(m_currentMaxRowLength, rowLength);

    bool retval = m_viewRowsLength + m_offsetY > 0;
    if (nextRow)
    {
        alignItems();
        m_startNewItemIndex = m_waitingIndex;
    }

    return retval;
}

void GridCardRowsLayoutEngine::alignItems()
{
    if (m_waitingIndex == m_startNewItemIndex)
        return; //Already aligned.

    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;

    bool alignRight = verticalLayout ? view()->options().itemAlignment().testFlag(Qt::AlignRight) : view()->options().itemAlignment().testFlag(Qt::AlignBottom);
    bool alignCenter = verticalLayout ? view()->options().itemAlignment().testFlag(Qt::AlignHCenter) : view()->options().itemAlignment().testFlag(Qt::AlignVCenter);

    if (m_waiting[m_startNewItemIndex].gridRow->type() != GridRow::DataRow)
        return;

    if (!alignCenter && !alignRight && !view()->options().rowAutoHeight())
        return;
 
    int count = m_waitingIndex - m_startNewItemIndex;
    int space = -1;
    int delta = -1;
    int maxRowLength = -1;
    if (view()->options().rowAutoHeight())
    {
        for (int i = m_waitingIndex - 1; i >= m_startNewItemIndex; --i)
        {
            WaitingRowParam& param = m_waiting[i];
            if (verticalLayout)
                maxRowLength = qMax(maxRowLength, param.rect.height());
            else
                maxRowLength = qMax(maxRowLength, param.rect.width());
        }
    }

    for (int i = m_waitingIndex - 1; i >= m_startNewItemIndex; --i)
    {
        WaitingRowParam& param = m_waiting[i];
        if (param.gridRow->type() != GridRow::DataRow)
            return;
        if (i == m_waitingIndex - 1)
        {
            space = verticalLayout ? 
                m_viewColumnsLength - (param.rect.left() + param.rect.width()) : 
                m_viewColumnsLength - (param.rect.top() + param.rect.height());

            if (alignCenter)
                delta = space / (count + 1);
            else if (alignRight)
                delta = space;
            else
                delta = 0;
        }

        if (maxRowLength > 0)
        {
            if (verticalLayout)
                param.rect.setHeight(maxRowLength);
            else
                param.rect.setWidth(maxRowLength);
        }

        if (i == m_startNewItemIndex)
        {
            if (verticalLayout)
                param.rect.setWidth(param.rect.width() + delta);
            else
                param.rect.setHeight(param.rect.height() + delta);
        }
        else
        {
            if (verticalLayout)
            {
                if (alignCenter)
                    param.rect.translate(delta * (i - m_startNewItemIndex + 1), 0);
                else
                    param.rect.translate(delta, 0);
            }
            else
            {
                if (alignCenter)
                    param.rect.translate(0, delta * (i - m_startNewItemIndex + 1));
                else
                    param.rect.translate(0, delta);
            }
        }
    }
}

/* GridCardSummaryLayoutEngine */
GridCardSummaryLayoutEngine::GridCardSummaryLayoutEngine(GridCardView* view, GridRowLayoutItem* row)
    : GridSummaryLayoutEngine(view, row)
{
}

GridCardSummaryLayoutEngine::~GridCardSummaryLayoutEngine()
{
}

//#define QTN_ALIGN_SUMMARY_RIGHT
void GridCardSummaryLayoutEngine::updateCellsLayout()
{
#ifdef QTN_ALIGN_SUMMARY_RIGHT
    int groupingIndent = getGroupingIndent();
#endif
    int groupRowIndent = getGroupRowIndent();
    bool verticalLayout = view()->layout()->orientation() == Qt::Vertical;
    int defCellLength = get_cell_height(view()->grid()->style(), view()->optionsref());
    int offset = groupRowIndent + kGridSummaryCellBorder;
    m_paintGroupText = true;

    for (QHash<const GridDataTableColumn *, GridSummaryCellPrimitive *>::const_iterator it =
        m_summaryCells.constBegin();
        it != m_summaryCells.constEnd(); ++it)
    {
        const GridCardColumn* cardColumn = static_cast<const GridCardColumn *>(it.key());
        GridSummaryCellPrimitive* cell = it.value();

        if (isGroupRow())
        {
            cell->setVisibility(view()->modelController()->summary()->groupSettingsExists(cardColumn->dataBinding()));
        }
        else
        {
            cell->setVisibility(view()->modelController()->summary()->footerSettingsExists(cardColumn->dataBinding()));
        }
        if (!cell->visibility())
            continue;
        const int columnWidth = 170;

        QRect columnRect;
#ifdef QTN_ALIGN_SUMMARY_RIGHT
       if (view()->optionsref()->groupSummaryPlace() != GroupSummaryRow)
            columnRect = QRect(kGridSummaryCellBorder, offset, columnWidth, defCellLength);
        else
#endif
            columnRect = QRect(parentGP()->width() - columnWidth - kGridSummaryCellBorder, offset, columnWidth, defCellLength);
  
        if (verticalLayout)
        {
            if (view()->hasGroupingIndent(cardColumn))
            {
                cell->m_hasGroupText = false;
#ifdef QTN_ALIGN_SUMMARY_RIGHT

                if (view()->optionsref()->groupSummaryPlace() != GroupSummaryRow)
                    columnRect.adjust(groupingIndent, 0, 0, 0);
#endif
            }
            else
                cell->m_hasGroupText = false;
        }
        else
        {
            if (view()->hasGroupingIndent(cardColumn))
            {
                cell->m_hasGroupText = false;
#ifdef QTN_ALIGN_SUMMARY_RIGHT
                if (view()->optionsref()->groupSummaryPlace() != GroupSummaryRow)
                  columnRect.adjust(0, groupingIndent, 0, 0);
#endif
            }
            else
                cell->m_hasGroupText = false;
        }
        cell->setRect(columnRect);
        cell->setTableColumn(cardColumn);
        offset += defCellLength + kGridSummaryCellBorder;
    }
}

/* GridCardFooterSummaryPrimitive */
GridCardFooterSummaryPrimitive::GridCardFooterSummaryPrimitive(GraphicPrimitive* parent, GridCardView* view)
    : GridFooterSummaryPrimitive(parent)
{
    m_layoutEngine = new GridCardSummaryLayoutEngine(view);
    m_layoutEngine->setParentPrimitive(this);
}

GridCardFooterSummaryPrimitive::~GridCardFooterSummaryPrimitive()
{
}
