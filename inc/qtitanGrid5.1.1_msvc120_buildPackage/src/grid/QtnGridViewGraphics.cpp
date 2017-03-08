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

#include <QtCore/qmath.h>
#include "QtnGridViewGraphics.h"

class GridRowPublic : public GridRow
{
    friend class Qtitan::GridViewPrimitiveBase;
    friend class Qtitan::GridTableViewPrimitive;
    friend class Qtitan::GridCardViewPrimitive;
};

/* GridViewLayoutItemPrimitive */
GridViewLayoutItemPrimitive::GridViewLayoutItemPrimitive(GraphicPrimitive* parent)
    : GraphicPrimitive(parent)
{
}

void GridViewLayoutItemPrimitive::updateLayout()
{
    GraphicPrimitive::updateLayout();
}

void GridViewLayoutItemPrimitive::updateChildren()
{
    QSize bounded = boundedRect().size();
    int top = 0;
    for (GraphicPrimitive::GraphicChildren::const_iterator it = childrenGP().constBegin(); it != childrenGP().constEnd(); ++it)
    {
        if (!(*it)->visibility())
            continue;
        QSize sizeHint = (*it)->sizeHint();
        if (sizeHint.width() == 0)
            sizeHint.rwidth() = bounded.width();
        if (sizeHint.height() == 0)
            sizeHint.rheight() = bounded.height();
        (*it)->setRect(QRect(QPoint(0, top), sizeHint));
        top += sizeHint.height();
    }
}

QSize GridViewLayoutItemPrimitive::sizeHint()
{
    QSize ret = QSize(0, 0);
    for (GraphicPrimitive::GraphicChildren::const_iterator it = childrenGP().constBegin(); it != childrenGP().constEnd(); ++it)
    {
        (*it)->updateLayout();
        QSize sizeHint = (*it)->sizeHint() + QSize(borderLeft() + borderRight(), borderTop() + borderBottom());
        ret.rwidth() = qMax(ret.width(), sizeHint.width());
        ret.rheight() += sizeHint.height();
          //return childrenGP()[0]->sizeHint() + QSize(borderLeft() + borderRight(), borderTop() + borderBottom());
    }
    return ret;
}

/* GridViewColumnsPrimitive */
GridViewColumnsPrimitive::GridViewColumnsPrimitive(GraphicPrimitive* parent)
    : GridViewLayoutItemPrimitive(parent)
{
    setBorder(0);
    setBorderColor(Qt::magenta);
}

QSize GridViewColumnsPrimitive::sizeHint()
{
    return GridViewLayoutItemPrimitive::sizeHint();
}


/* GridViewHeaderPrimitive */
GridViewHeaderPrimitive::GridViewHeaderPrimitive(GraphicPrimitive* parent)
    : GridViewLayoutItemPrimitive(parent)
{
    setBorder(0);
    setBorderColor(Qt::green);
}

QSize GridViewHeaderPrimitive::sizeHint()
{
    return GridViewLayoutItemPrimitive::sizeHint();
}

void GridViewHeaderPrimitive::paintEvent(QPainter& painter)
{
    static_cast<GridViewLayoutPrimitive *>(parentGP())->paintHeaderBackground(painter, boundedRect());
}

/* GridViewFooterPrimitive */
GridViewFooterPrimitive::GridViewFooterPrimitive(GraphicPrimitive* parent)
    : GridViewLayoutItemPrimitive(parent)
{
    setBorderTop(1);
}

void GridViewFooterPrimitive::resizeEvent(const QRect& rect)
{
    GridViewLayoutItemPrimitive::resizeEvent(rect);
}

void GridViewFooterPrimitive::paintEvent(QPainter& painter)
{
    static_cast<GridViewLayoutPrimitive *>(parentGP())->paintHeaderBackground(painter, boundedRect());
}

QSize GridViewFooterPrimitive::sizeHint()
{
    return GridViewLayoutItemPrimitive::sizeHint();
}

void GridViewFooterPrimitive::mouseOverEvent(const QPoint& pos)
{
    static_cast<GridViewPrimitiveBase *>(parentGP())->showFooter();
    GraphicPrimitive::mouseOverEvent(pos);
}

void GridViewFooterPrimitive::mouseOutEvent()
{
    static_cast<GridViewPrimitiveBase *>(parentGP())->hideFooter();
    GraphicPrimitive::mouseOutEvent();
}


/* GridViewTopRowsPrimitive */
GridViewTopRowsPrimitive::GridViewTopRowsPrimitive(GraphicPrimitive* parent)
    : GridViewLayoutItemPrimitive(parent)
{
    setBorder(2);
    setBorderColor(Qt::darkCyan);
}

QSize GridViewTopRowsPrimitive::sizeHint()
{
    return QSize(0, 0); //TODO
}

/* GridViewBottomRowsPrimitive */
GridViewBottomRowsPrimitive::GridViewBottomRowsPrimitive(GraphicPrimitive* parent)
    : GridViewLayoutItemPrimitive(parent)
{
    setBorder(0);
    setBorderColor(Qt::red);
}

QSize GridViewBottomRowsPrimitive::sizeHint()
{
    return GridViewLayoutItemPrimitive::sizeHint();
}

/* GridViewCentralRowsPrimitive */
GridViewCentralRowsPrimitive::GridViewCentralRowsPrimitive(GraphicPrimitive* parent)
    : GridViewLayoutItemPrimitive(parent)
{
    setBorder(0);
    setBorderColor(Qt::red);
}

void GridViewCentralRowsPrimitive::updateChildren()
{
    Q_ASSERT(childrenGP().size() == 1);
    childrenGP()[0]->setRect(QRect(QPoint(0, 0), boundedRect().size()));
}

void GridViewCentralRowsPrimitive::updateLayout()
{
    GridViewLayoutItemPrimitive::updateLayout();
}

/* GridViewPrimitive */
GridViewLayoutPrimitive::GridViewLayoutPrimitive(GraphicControl* control)
    : ScrollableGraphicPrimitive(control, 0),
    m_footerTop(-1), m_footerBottomHeight(-1), m_orientation(Qt::Vertical), m_footerAutoHide(true), m_footerShowing(false)
{
#if 0
    setBorder(3);
    setBorderColor(Qt::blue);
#endif
    m_footerPrimitive = new GridViewFooterPrimitive(this);
    m_footerPrimitive->setZIndex(10);
    m_footerPrimitive->setVisibility(false);
    setFixedGPX(m_footerPrimitive, true);
    setFixedGPY(m_footerPrimitive, true);

    m_headerPrimitive = new GridViewHeaderPrimitive(this);
    setFixedGPX(m_headerPrimitive, true);
    setFixedGPY(m_headerPrimitive, true);

    m_columnsPrimitive = new GridViewColumnsPrimitive(this);
    setFixedGPX(m_columnsPrimitive, true);
    setFixedGPY(m_columnsPrimitive, true);

    m_topRowsPrimitive = new GridViewTopRowsPrimitive(this);
    setFixedGPX(m_topRowsPrimitive, true);
    setFixedGPY(m_topRowsPrimitive, true);

    m_bottomRowsPrimitive = new GridViewBottomRowsPrimitive(this);
    setFixedGPX(m_bottomRowsPrimitive, true);
    setFixedGPY(m_bottomRowsPrimitive, true);

    m_centralRowsPrimitive = new GridViewCentralRowsPrimitive(this);
    setFixedGPX(m_centralRowsPrimitive, true);
    setFixedGPY(m_centralRowsPrimitive, true);

    m_footerUpdateTimer = new QTimer(this);
    m_footerUpdateTimer->setSingleShot(true);
    connect(m_footerUpdateTimer, SIGNAL(timeout()), this, SLOT(footerUpdateTimeout()));
}

void GridViewLayoutPrimitive::updateLayout()
{
    int headerHeight = m_headerPrimitive->visibility() ? m_headerPrimitive->sizeHint().height() : 0;
    int footerHeight = m_footerPrimitive->visibility() ? m_footerPrimitive->sizeHint().height() : 0;

    m_footerBottomHeight = (footerHeight > 0 && isFooterAutoHide()) ? kGridFilterPanelPeer : footerHeight;

    m_headerPrimitive->setHeight(headerHeight);
    m_footerPrimitive->setHeight(footerHeight);

    if (orientation() == Qt::Vertical)
    {
        const int topRowsHeight = m_topRowsPrimitive->visibility() ? m_topRowsPrimitive->sizeHint().height() : 0;
        const int bottomRowsHeight = m_bottomRowsPrimitive->visibility() ? m_bottomRowsPrimitive->sizeHint().height() : 0;
        m_topRowsPrimitive->setHeight(topRowsHeight);
        m_bottomRowsPrimitive->setHeight(bottomRowsHeight);
    }
    else
    {
        const int topRowsWidth = m_topRowsPrimitive->visibility() ? m_topRowsPrimitive->sizeHint().width() : 0;
        const int bottomRowsWidth = m_bottomRowsPrimitive->visibility() ? m_bottomRowsPrimitive->sizeHint().width() : 0;
        m_topRowsPrimitive->setWidth(topRowsWidth);
        m_bottomRowsPrimitive->setWidth(bottomRowsWidth);
    }

    updateScrollBars();
    QRect viewRect = boundedRect();

    m_headerPrimitive->setRect(QRect(0, 0, viewRect.width(), m_headerPrimitive->height()));

    m_footerTop = viewRect.height() - m_footerBottomHeight;
    if (isFooterAutoHide())
        m_footerPrimitive->setRect(QRect(0, m_footerPrimitive->top(), viewRect.width(), m_footerPrimitive->height()));
    else
        m_footerPrimitive->setRect(QRect(0, viewRect.height() - m_footerBottomHeight, viewRect.width(), m_footerPrimitive->height()));

    


    if (orientation() == Qt::Vertical)
    {
        m_columnsPrimitive->setWidth(viewRect.width());
        const QSize columnPanelSize = m_columnsPrimitive->visibility() ? m_columnsPrimitive->sizeHint() : QSize(0, 0);
        m_columnsPrimitive->setRect(QRect(0, m_headerPrimitive->height(), viewRect.width(), columnPanelSize.height()));

        m_topRowsPrimitive->setRect(QRect(0, m_headerPrimitive->height() + m_columnsPrimitive->height(), viewRect.width(), m_topRowsPrimitive->height()));
        m_bottomRowsPrimitive->setRect(QRect(0, viewRect.height() - m_footerBottomHeight - m_bottomRowsPrimitive->height(), viewRect.width(), m_bottomRowsPrimitive->height()));

        int centralRowsHeight = m_bottomRowsPrimitive->top() - m_topRowsPrimitive->rect().bottom() - 1;
        if (centralRowsHeight < 0)
            centralRowsHeight = 0;
        m_centralRowsPrimitive->setRect(QRect(0, m_headerPrimitive->height() + m_columnsPrimitive->height() + m_topRowsPrimitive->height(),
            viewRect.width(), centralRowsHeight));
    }
    else
    {
        int viewHeight = viewRect.height() - m_footerBottomHeight - m_headerPrimitive->rect().bottom() - 1;
        if (viewHeight < 0)
            viewHeight = 0;

        m_columnsPrimitive->setHeight(viewHeight);
        const QSize columnPanelSize = m_columnsPrimitive->visibility() ? m_columnsPrimitive->sizeHint() : QSize(0, 0);
        m_columnsPrimitive->setRect(QRect(0, m_headerPrimitive->height(), columnPanelSize.width(), viewHeight));

        m_topRowsPrimitive->setRect(QRect(m_columnsPrimitive->width(), m_headerPrimitive->height(), m_topRowsPrimitive->width(), viewHeight));
        m_bottomRowsPrimitive->setRect(QRect(viewRect.width() - m_bottomRowsPrimitive->width(), m_headerPrimitive->height(), m_bottomRowsPrimitive->width(), viewHeight));

        int centralRowsWidth = m_bottomRowsPrimitive->left() - m_topRowsPrimitive->rect().right();
        if (centralRowsWidth < 0)
            centralRowsWidth = 0;
        m_centralRowsPrimitive->setRect(QRect(m_columnsPrimitive->width() + m_topRowsPrimitive->width(), m_headerPrimitive->height(), centralRowsWidth, viewHeight));
    }
    m_headerPrimitive->updateChildren();
    m_columnsPrimitive->updateChildren();
    m_footerPrimitive->updateChildren();
    m_centralRowsPrimitive->updateChildren();
    m_topRowsPrimitive->updateChildren();
    m_bottomRowsPrimitive->updateChildren();
    updateFooterAutoHideRect(false);
    control()->geometryChanged();
}

void GridViewLayoutPrimitive::resizeEvent(const QRect& rect)
{
    Q_UNUSED(rect);
    updateLayout();
}

Qt::Orientation GridViewLayoutPrimitive::orientation() const
{
    return m_orientation;
}

void GridViewLayoutPrimitive::setOrientation(Qt::Orientation orientation)
{
    if (m_orientation == orientation)
        return;
    m_orientation = orientation;
    orientationChanged();
    update();
}

bool GridViewLayoutPrimitive::isFooterAutoHide() const
{
    return m_footerAutoHide;
}

void GridViewLayoutPrimitive::setFooterAutoHide(bool autoHide)
{
    m_footerUpdateTimer->stop();
    m_footerAutoHide = autoHide;
}

void GridViewLayoutPrimitive::showFooter()
{
    if (m_footerShowing || !isFooterAutoHide())
        return;
    m_footerShowing = true;
    if (!m_footerUpdateTimer->isActive())
        m_effectStartTime = QTime();
    else if (m_effectStartTime == QTime())
    {
        m_effectStartTime = QTime::currentTime();
        m_footerUpdateTimer->stop();
        return;
    }

    m_effectDuration = 100;
    m_footerUpdateTimer->start(10);
}

void GridViewLayoutPrimitive::hideFooter()
{
    if (!m_footerShowing || !isFooterAutoHide())
        return;
    m_footerShowing = false;

    m_effectDuration = 200;
    if (!m_footerUpdateTimer->isActive())
    {
        m_effectStartTime = QTime();
        m_footerUpdateTimer->start(500);
    }
    else
    {
        m_footerUpdateTimer->start(10);
    }
}

void GridViewLayoutPrimitive::updateFooterAutoHideRect(bool repaint)
{
    if (!isFooterAutoHide())
        return;

    if (m_effectStartTime == QTime())
        m_effectStartTime = QTime::currentTime();
    int top = m_footerTop;
    int peer = m_footerPrimitive->height() - kGridFilterPanelPeer;
    int diff = m_effectStartTime.msecsTo(QTime::currentTime());
    if (m_effectDuration == 0 || diff > m_effectDuration)
    {
        if (m_footerShowing)
            top -= peer;
    }
    else
    {
        m_footerUpdateTimer->start(10);
        double offset = ((double)diff / (double)m_effectDuration) * peer;
        if (m_footerShowing)
            top -= offset;
        else
            top -= (peer - offset);
    }

    QRect r = m_footerPrimitive->rect();
    r.moveTop(top);
    m_footerPrimitive->setRect(r);

    if (repaint)
    {
        QPoint p = mapToSite(QPoint(0, m_footerTop - m_footerPrimitive->height()));
        control()->update(QRect(p.x(), p.y(), r.width(), r.height() + kGridFilterPanelPeer));
    }
}

void GridViewLayoutPrimitive::footerUpdateTimeout()
{
    updateFooterAutoHideRect(true);
}

/* GridViewPrimitiveBase */
GridViewPrimitiveBase::GridViewPrimitiveBase(GraphicControl* control)
    : GridViewLayoutPrimitive(control), m_view(Q_NULL), m_groupingPanel(Q_NULL), m_rowsPanel(Q_NULL), m_filterPanel(Q_NULL), 
    m_summaryPanel(Q_NULL), m_zoomIndicator(Q_NULL), m_modelViewHeight(-1),
    m_autoHeightColumnRowCount(0)
{
    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(updateRowsScrollTimeout()));
}

GridViewPrimitiveBase::~GridViewPrimitiveBase()
{
}

void GridViewPrimitiveBase::updateLayout()
{
    if (m_view->optionsref()->groupRowStyle() == GridGroupOfficeOutlook)
    {
        QColor color = qtn_get_thememanager(control())->shadeColor();
        color.setAlpha(78);
        static_cast<GridTableView *>(m_view)->options().setGridLineColor(color);
    }
    footer()->setBorderPen(static_cast<GridTableView *>(m_view)->options().gridLinePen());

    m_groupingPanel->setVisibility(m_view->optionsref()->groupsHeader());
}

void GridViewPrimitiveBase::setFooterAutoHide(bool autoHide)
{
    GridViewLayoutPrimitive::setFooterAutoHide(autoHide);
    filterPanel()->updateAutoHideIcon();
}

void GridViewPrimitiveBase::orientationChanged()
{
    GridViewAutoUpdater autoUpdater(m_view);
    m_view->requestUpdateLayout();
}

void GridViewPrimitiveBase::paintHeaderBackground(QPainter& painter, const QRect& rect)
{
    QBrush brush = m_view->optionsref()->groupsHeaderBrush();
    QRect boundedRectangle = rect;

    if (brush == QBrush() && m_view->optionsref()->isWindowsAirSupported())
    {
        QLinearGradient linearGrad(QPointF(10, -boundedRectangle.height()), QPointF(10, boundedRectangle.height() + 15));
        linearGrad.setColorAt(0, Qt::transparent);
        linearGrad.setColorAt(1, Qt::black);
        brush = linearGrad;
        painter.fillRect(boundedRectangle, brush);

        //brush = create_background(Qt::transparent);
    }
    else
    {
        if (brush != QBrush())
            painter.fillRect(boundedRectangle, brush);
        else
            painter.fillRect(boundedRectangle, m_view->optionsref()->groupsHeaderColor());

        if (control()->style()->inherits("Qtitan::OfficeStyle"))
        {
            QStyleOptionSlider opt;
            opt.initFrom(control());
            opt.subControls = QStyle::SC_None;
            opt.state = QStyle::State_Horizontal;
            opt.rect = boundedRectangle;
            control()->style()->drawComplexControl(QStyle::CC_ScrollBar, &opt, &painter, control());
        }
    }
}

void GridViewPrimitiveBase::setScrollAreaClipping()
{
    bool verticalLayout = orientation() == Qt::Vertical;

    QRect columnsBoundedRect = columns()->boundedRect();
    QRect rowsBoundedRect = rowsPanel()->boundedRect();

    int leftIndent = leftFixedIndent();
    int rightIndent = rightFixedIndent();

    int topIndent = rowsPanel()->topFixedIndent();
    int bottomIndent = rowsPanel()->bottomFixedIndent();

    int quickButtonIndent = getQuickButtonIndent();

    leftIndent += quickButtonIndent;

    if (verticalLayout)
    {
        QRect maskRect = QRect(leftIndent, 0, qMax(0, columnsBoundedRect.width() - leftIndent - rightIndent), columnsBoundedRect.height());
        setColumnMask(maskRect);
        m_summaryPanel->mask().setRect(maskRect);

        maskRect.translate(-quickButtonIndent, 0);
        maskRect.setHeight(10000);
        rowsLayoutEngine()->setRowMask(maskRect);

        maskRect = rowsBoundedRect.adjusted(0, topIndent, 0, -bottomIndent);
        maskRect.setHeight(qMax(0, maskRect.height()));
        rowsPanel()->mask().setRect(maskRect);
    }
    else
    {

        QRect maskRect = QRect(0, leftIndent, columnsBoundedRect.width(), qMax(0, columnsBoundedRect.height() - leftIndent - rightIndent));
        setColumnMask(maskRect);
        m_summaryPanel->mask().setRect(maskRect);

        maskRect.translate(0, -quickButtonIndent);
        maskRect.setWidth(10000);
        rowsLayoutEngine()->setRowMask(maskRect);

        maskRect = rowsBoundedRect.adjusted(topIndent, 0, -bottomIndent, 0);
        maskRect.setWidth(qMax(0, maskRect.width()));
        rowsPanel()->mask().setRect(maskRect);
    }
}

int GridViewPrimitiveBase::getModelHeight()
{
    if (m_modelViewHeight == -1)
        calculateModelViewHeight();
    return m_modelViewHeight;
}

int GridViewPrimitiveBase::getGridRowAutoHeight(GridRow* gridRow) const
{
    int retval = 0;
    Q_ASSERT(gridRow->type() == GridRow::DataRow && autoHeightRowCount() > 0);
    int* ahptr = const_cast<int *>(&autoHeightInfo()[autoHeightRowCount() * gridRow->rowIndex()]);

    if (ahptr[0] == -1)
        calculateGridRowAutoHeight(gridRow, ahptr);

    for (int i = 0; i < autoHeightRowCount(); ++i)
    {
        retval += ahptr[i];
    }

    return retval;
}

int GridViewPrimitiveBase::calculateGridRowLength(GridRow* row, bool groupSummary)
{
    int ret = 0;
    GridRow* summaryGridRow = groupSummary && is_next_summary(row) ? row : Q_NULL;
    while (summaryGridRow != Q_NULL)
    {
        ret += calculateGridSummaryRowLength(summaryGridRow);
        if (!summaryGridRow->modelGroup()->last)
            break;
        summaryGridRow = summaryGridRow->parentGroupRow();
    }
    return ret;
}

void GridViewPrimitiveBase::setColumnMask(const QRect& maskRect)
{
    Q_UNUSED(maskRect);
}

void GridViewPrimitiveBase::resetAutoHeightMode()
{
    m_modelViewHeight = -1;
    m_autoHeightColumnRowCount = 0;
    getAutoHeightRowCount(m_autoHeightColumnRowCount);

    int gridRowCount = m_view->modelController()->getGridRowCount();

    m_autoHeightInfo.resize(m_autoHeightColumnRowCount * gridRowCount);
    if (m_autoHeightInfo.size() == 0)
    {
        m_autoHeightColumnRowCount = 0;
        return;
    }
    memset(m_autoHeightInfo.data(), 0xFF, sizeof(int) * m_autoHeightInfo.size());

}

int GridViewPrimitiveBase::autoHeightRowCount() const
{ 
    return m_autoHeightColumnRowCount;
}

const QVector<int>& GridViewPrimitiveBase::autoHeightInfo() const
{
    return m_autoHeightInfo;
}

/*!
\internal
*/
void GridViewPrimitiveBase::updateRows(bool forceUpdateLayout)
{
    if (m_view->model() == Q_NULL || rowsLayoutEngine() == Q_NULL || m_view->isUpdating())
        return;

    int offset = 0;
    int rowIndex = calculateRowIndexByScrollPos(offset);
    if (rowIndex == -1)
        return;

    bool verticalLayout = orientation() == Qt::Vertical;
    QRect rowsRect = rowsScrollAreaRect();
    int rowsLength = verticalLayout ? rowsRect.height() : rowsRect.width();
    int columnsLength = verticalLayout ? getViewWidth() : getViewHeight();

    rowsLayoutEngine()->beginUpdate(columnsLength, rowsLength, offset, forceUpdateLayout);

    //Process top frozen rows
    for (int i = 0; i < m_view->modelController()->getFrozenTopGridRowCount(); ++i)
    {
        GridRow* gridRow = m_view->modelController()->getGridRow(i);
        if (!rowsLayoutEngine()->appendFrozenGridRow(gridRow, true))
            break;
    }

    int lastGridRowIndex = m_view->modelController()->getGridRowCount() - m_view->modelController()->getFrozenBottomGridRowCount() - 1;

    //Process bottom frozen rows
    for (int i = lastGridRowIndex + 1; i < m_view->modelController()->getGridRowCount(); ++i)
    {
        GridRow* gridRow = m_view->modelController()->getGridRow(i);
        if (!rowsLayoutEngine()->appendFrozenGridRow(gridRow, false))
            break;
    }

    for (;;)
    {
        if (rowIndex <= lastGridRowIndex)
        {
            GridRow* gridRow = m_view->modelController()->getGridRow(rowIndex);
            if (!rowsLayoutEngine()->appendGridRow(gridRow))
                break;
        }
        else
            break;
        rowIndex++;
    }
    rowsLayoutEngine()->endUpdate();
}

int GridViewPrimitiveBase::alignGridRowIndex(int rowIndex, int* itemsCount)
{
    if (itemsCount != Q_NULL)
        *itemsCount = 1;
    return rowIndex;
}

bool GridViewPrimitiveBase::alternatingRowColors(GridRow* row) const
{
    if (!m_view->optionsref()->alternatingRowColors())
        return false;
    if (row == Q_NULL || row->type() != GridRow::DataRow)
        return false;
    double rowIndex = row->rowIndex();
    if (rowIndex == -1)
        return false;
    return rowIndex != ((int)rowIndex / 2) * 2;
}

bool GridViewPrimitiveBase::iterateToNextLine(int rowIndex, bool back, int& out_nextRowIndex, int& out_rowLength, int& out_itemsCount)
{
    Q_ASSERT(rowIndex >= 0 && rowIndex < m_view->getRowCount());

    GridRow* row = m_view->getRow(rowIndex);
    const GridRow::FrozenPlace frozenPlace = row->frozenPlace();

    out_nextRowIndex = alignGridRowIndex(rowIndex, &out_itemsCount);
    out_rowLength = 0;
    
  
    for (int i = 0; i < out_itemsCount; ++i)
    {
        GridRow* row = m_view->getRow(rowIndex + i);
        out_rowLength = qMax(out_rowLength, calculateGridRowLength(row, m_view->optionsref()->groupSummaryPlace() == GroupSummaryFooter));
        if (!m_view->optionsref()->rowAutoHeight())
            break;
    }

    if (back)
        out_nextRowIndex--;
    else
        out_nextRowIndex += out_itemsCount;

    if (out_nextRowIndex < 0 || out_nextRowIndex >= m_view->getRowCount())
        return false;
  
    row = m_view->getRow(out_nextRowIndex);
    if (row->frozenPlace() != frozenPlace)
        return false;

    return true;
}

void GridViewPrimitiveBase::drawBorder(QPainter& painter)
{
    #if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
    QStyleOptionFrameV3 opt;
    #else
    QStyleOptionFrame opt;
   #endif
    opt.init(control());
    opt.palette.setColor(QPalette::WindowText, m_view->optionsref()->borderColor());
    int frameShape = m_view->optionsref()->borderStyled() ? QFrame::StyledPanel : QFrame::Panel;
    int frameShadow = 0;
    opt.frameShape = QFrame::Shape(int(opt.frameShape) | frameShape);
    opt.rect = QRect(0, 0, width(), height());
    switch (frameShape) {
    case QFrame::StyledPanel:
    case QFrame::Panel:
        opt.lineWidth = borderLeft();
        opt.midLineWidth = 0;
        break;
    default:
        opt.lineWidth = borderLeft();
        break;
    }

    if (frameShadow == QFrame::Sunken)
        opt.state |= QStyle::State_Sunken;
    else if (frameShadow == QFrame::Raised)
        opt.state |= QStyle::State_Raised;

    control()->style()->drawControl(QStyle::CE_ShapedFrame, &opt, &painter, control());
}

void GridViewPrimitiveBase::updateScrollBars()
{
    setColumnsScrollBarVisible(false);
    setRowsScrollBarVisible(false);

    int columnsRangeMin, columnsRangeMax, columnsPage, columnsSingleStep;
    int rowsRangeMin, rowsRangeMax, rowsPage, rowsSingleStep;

    bool columnsScrollVisible = checkColumnsScrollBar(columnsRangeMin, columnsRangeMax, columnsPage, columnsSingleStep);
    setColumnsScrollBarVisible(columnsScrollVisible);

    bool rowsScrollVisible = checkRowsScrollBar(rowsRangeMin, rowsRangeMax, rowsPage, rowsSingleStep);
    if (setRowsScrollBarVisible(rowsScrollVisible))
    {
        int oldColumnPage = columnsPage;
        columnsScrollVisible = checkColumnsScrollBar(columnsRangeMin, columnsRangeMax, columnsPage, columnsSingleStep);
        setColumnsScrollBarVisible(columnsScrollVisible);

        bool rowAutoHeight = m_view->optionsref()->rowAutoHeight();
        bool isCard = qobject_cast<GridCardView *>(m_view);
        if (isCard || (oldColumnPage != columnsPage && rowAutoHeight))
        {
            rowsScrollVisible = checkRowsScrollBar(rowsRangeMin, rowsRangeMax, rowsPage, rowsSingleStep);
            setRowsScrollBarVisible(rowsScrollVisible);
        }
    }
    setColumnsScrollBarRange(columnsRangeMin, columnsRangeMax, columnsPage, columnsSingleStep);
    setRowsScrollBarRange(rowsRangeMin, rowsRangeMax, rowsPage, rowsSingleStep);

    setScrollBarsPosition();
}

bool GridViewPrimitiveBase::checkRowsScrollBar(int& rangeMin, int& rangeMax, int& page, int& singleStep)
{
    bool verticalLayout = orientation() == Qt::Vertical;
    rowsPanel()->calculateFixedRows();

    QSize scrollSize = rowsScrollAreaRect().size();

    const int firstRowIndex = m_view->modelController()->getFrozenTopGridRowCount();
    const int lastRowIndex = m_view->modelController()->getGridRowCount() - m_view->modelController()->getFrozenBottomGridRowCount() - 1;

    int rowsLength = 0;
    int maxRowsLength = verticalLayout ? scrollSize.height() : scrollSize.width();
    int visibleRowCount = 0;

    while (lastRowIndex - visibleRowCount >= firstRowIndex)
    {
        int rowIndex = lastRowIndex - visibleRowCount;
        int alignedRowIndex = alignGridRowIndex(rowIndex);
        GridRow* gridRow = m_view->modelController()->getGridRow(alignedRowIndex);

        int length = 0;
        
        if (m_view->optionsref()->rowAutoHeight())
        {
            for (int i = alignedRowIndex; i <= rowIndex; ++i)
            {
                length = qMax(length, calculateGridRowLength(m_view->modelController()->getGridRow(i), m_view->optionsref()->groupSummaryPlace() == GroupSummaryFooter));
            }
        }
        else
            length = calculateGridRowLength(gridRow, m_view->optionsref()->groupSummaryPlace() == GroupSummaryFooter);

        rowsLength += length;

        if (rowsLength > maxRowsLength)
        {
            rangeMin = 0;
            if (m_view->optionsref()->scrollRowStyle() == ScrollByPixel)
            {
                int height = getModelHeight();
                rangeMax = height - maxRowsLength;
                page = maxRowsLength;
                singleStep = kGridWebScrollStep;

            }
            else
            {
                rangeMax = (lastRowIndex - firstRowIndex + 1) * kGridNormalScrollStep - visibleRowCount * kGridNormalScrollStep;
                page = maxRowsLength;
                singleStep = kGridNormalScrollStep;
            }
            return true;
        }
        visibleRowCount += (rowIndex - alignedRowIndex) + 1;
    }
    rangeMin = 0;
    rangeMax = 0;
    page = 0;
    singleStep = kGridNormalScrollStep;
    return false;
}

/*!
\internal
*/
int GridViewPrimitiveBase::calculateRowIndexByScrollPos(int& offset)
{
    offset = 0;

    bool verticalLayout = orientation() == Qt::Vertical;
    int rowIndex = 0;

    int scrollPos = verticalLayout ? scrollY() : scrollX();

    if (m_view->optionsref()->scrollRowStyle() == ScrollByPixel)
    {
        getModelHeight();
        rowIndex = find_modelRowIndex_by_view(m_view->modelController(), scrollPos);
        if (rowIndex == -1)
            return -1;

        rowIndex = alignGridRowIndex(rowIndex);

        GridRowPublic* nearestRow =
            static_cast<GridRowPublic *>(m_view->modelController()->getGridRow(rowIndex));

        if (nearestRow != Q_NULL)
        {
            scrollPos = scrollPos - nearestRow->m_scrollPosition;
            const int rowLength = calculateGridRowLength(nearestRow, m_view->optionsref()->groupSummaryPlace() == GroupSummaryFooter);
            if (scrollPos <= rowLength)
            {
                offset = scrollPos;
            }
            else
            {
                scrollPos -= rowLength;
                rowIndex = scrollPos / rowLength;
                offset = scrollPos - nearestRow->m_scrollPosition;
                rowIndex++;
            }
        }
    }
    else
    {
        rowIndex = alignGridRowIndex(m_view->modelController()->getFrozenTopGridRowCount() + (scrollPos / kGridNormalScrollStep));
    }
    return rowIndex;
}

void GridViewPrimitiveBase::calculateModelViewHeight()
{
    if (m_view->model() == Q_NULL)
    {
        m_modelViewHeight = 0;
        return;
    }

    //int rowCount = m_view->modelController()->getGridRowCount() - m_view->modelController()->getFrozenTopGridRowCount() - m_view->modelController()->getFrozenBottomGridRowCount();
    //int currentRowIndex = m_view->modelController()->getFrozenTopGridRowCount();
    //int maxLength = -1;

    int length = 0;
    int rowIndex = m_view->modelController()->getFrozenTopGridRowCount();
    int nextRowIndex = -1;
    int rowLength = -1;
    int itemsCount = -1;

    bool hasNext = true;
    while (hasNext && rowIndex >= 0 && rowIndex < m_view->modelController()->getGridRowCount())
    {
        hasNext = iterateToNextLine(rowIndex, false, nextRowIndex, rowLength, itemsCount);
        for (int i = 0; i < itemsCount; ++i)
        {
            GridRowPublic* gridRow = static_cast<GridRowPublic *>(m_view->modelController()->getGridRow(rowIndex + i));
            gridRow->m_scrollPosition = length;
        }
        length += rowLength;
        rowIndex = nextRowIndex;
    }
    m_modelViewHeight = length;

    //for (int rowIndex = m_view->modelController()->getFrozenTopGridRowCount(); rowIndex < rowCount; ++rowIndex)
    //{
    //    int alignedRowIndex = alignGridRowIndex(rowIndex);
    //    if (alignedRowIndex > currentRowIndex)
    //    {
    //        currentRowIndex = alignedRowIndex;
    //        length += maxLength;
    //        maxLength = -1;
    //    }
    //    GridRowPublic* gridRow = static_cast<GridRowPublic *>(m_view->modelController()->getGridRow(rowIndex));
    //    gridRow->m_scrollPosition = length;
    //    maxLength = qMax(calculateGridRowLength(gridRow, m_view->optionsref()->groupSummaryPlace() == GroupSummaryFooter), maxLength);
    //}

    //m_modelViewHeight = length + maxLength;
}

#if 0
void GridTableViewPrimitive::calculateModelViewHeight()
{
    if (m_view->model() == Q_NULL)
    {
        m_modelViewHeight = 0;
        return;
    }

    int length = 0;
    int rowCount = m_view->modelController()->getGridRowCount() - m_view->modelController()->getFrozenTopGridRowCount() - m_view->modelController()->getFrozenBottomGridRowCount();
    for (int i = m_view->modelController()->getFrozenTopGridRowCount(); i < rowCount; ++i)
    {
        GridRowPublic* gridRow = static_cast<GridRowPublic *>(m_view->modelController()->getGridRow(i));
        gridRow->m_scrollPosition = length;
        length += calculateGridRowLength(gridRow, m_view->optionsref()->groupSummaryPlace() == GroupSummaryFooter);
    }
    m_modelViewHeight = length;
}
#endif

int GridViewPrimitiveBase::calculateNextPageRowIndex(int rowIndex, bool up, int* offsetPos)
{
    if (rowIndex < m_view->modelController()->getFrozenTopGridRowCount() || 
        rowIndex > m_view->modelController()->getGridRowCount() - m_view->modelController()->getFrozenBottomGridRowCount() - 1)
        return -1;

    int rowsLen = orientation() == Qt::Vertical ? rowsScrollAreaRect().height() : rowsScrollAreaRect().width();

    GridRow* retval = Q_NULL;
     int nextRowIndex = -1;
    int rowLength = -1;
    int itemsCount = -1;
    retval = m_view->getRow(rowIndex);

    while (rowIndex >= m_view->modelController()->getFrozenTopGridRowCount() && 
        rowIndex < m_view->modelController()->getGridRowCount() - m_view->modelController()->getFrozenBottomGridRowCount())
    {
        bool hasNext = iterateToNextLine(rowIndex, up, nextRowIndex, rowLength, itemsCount);

        rowsLen -= rowLength;
        if (rowsLen < 0)
        {
            hasNext = false;
        }

        if (!hasNext)
            break;

        rowIndex = nextRowIndex;
        retval = m_view->getRow(rowIndex);
        Q_ASSERT(retval->frozenPlace() == GridRow::FrozenNone);
    }

    if (offsetPos != Q_NULL)
        *offsetPos = rowsLen;

    return alignGridRowIndex(retval->rowIndex());
}

void GridViewPrimitiveBase::updateZooming()
{
    updateLayout();
    /*
    if (view()->options().columnAutoWidth())
    return;
    for (GridViewBase::GridColumns::const_iterator it = view()->m_columnslist.constBegin(); it != view()->m_columnslist.constEnd(); ++it)
    {
    GridTableColumnBase* col = (GridTableColumnBase *)(*it);
    if (!view()->isColumnVisible(col))
    continue;
    //col->ensureWidth();
    }
    */
}

void GridViewPrimitiveBase::startZooming()
{
    if (!m_view->optionsref()->isZoomIndicatorActive())
        return;
    if (m_zoomIndicator == Q_NULL)
        m_zoomIndicator = new GridZoomIndicatorGraphic(this, m_view);
    m_zoomIndicator->start();
}

void GridViewPrimitiveBase::columnCreated(GridColumnBase* column)
{
    GridDataTableColumn* tableColumn = qobject_cast<GridDataTableColumn *>(column);
    if (tableColumn != Q_NULL)
    {
        rowsLayoutEngine()->addColumn(tableColumn);
        summaryPanel()->layoutEngine()->addColumn(tableColumn);
    }
}

void GridViewPrimitiveBase::columnDestroying(GridColumnBase* column)
{
    GridDataTableColumn* tableColumn = qobject_cast<GridDataTableColumn *>(column);
    if (tableColumn != Q_NULL)
    {
        rowsLayoutEngine()->removeColumn(tableColumn);
        summaryPanel()->layoutEngine()->removeColumn(tableColumn);
    }
    groupingPanel()->removeColumnAt(m_view->columnGroupIndex(column));
}

QRect GridViewPrimitiveBase::columnsRect() const
{
    int indent = getQuickButtonIndent();
    if (orientation() == Qt::Vertical)
        return QRect(indent, 0, columns()->boundedRect().width() - indent, 0);
    else
        return QRect(0, indent, 0, columns()->boundedRect().height() - indent);
}

QRect GridViewPrimitiveBase::rowsScrollAreaRect() const
{
    QRect r = QRect(QPoint(0, 0), centralAreaSize());

    if (orientation() == Qt::Vertical)
    {
        r.setTop(qMin(r.height(), rowsPanel()->topFixedIndent()));
        r.setHeight(qMax(0, r.height() - rowsPanel()->bottomFixedIndent()));
    }
    else
    {
        r.setLeft(qMin(r.width(), rowsPanel()->topFixedIndent()));
        r.setWidth(qMax(0, r.width() - rowsPanel()->bottomFixedIndent()));

    }
    return r;
}

void GridViewPrimitiveBase::calcScreenScrollRect(QRect& scrollRect) const
{
    scrollRect = this->boundedRect();
    scrollRect.moveTo(this->mapToGlobal(scrollRect.topLeft()));
}

QSize GridViewPrimitiveBase::centralAreaSize() const
{
    QSize s = boundedRect().size();
    if (orientation() == Qt::Vertical)
    {
        s.rheight() = qMax(0, s.height() - header()->height() - columns()->height() - topRows()->height() - bottomRows()->height() - m_footerBottomHeight);
    }
    else
    {
        s.rwidth() = qMax(0, s.width() - topRows()->width() - columns()->width() - bottomRows()->width());
        s.rheight() = qMax(0, s.height() - header()->height() - m_footerBottomHeight);
    }
    return s;
}

void GridViewPrimitiveBase::doScrollRows()
{
    if (m_view->optionsref()->scrollRowStyle() != ScrollByItem || m_view->optionsref()->fastScrollEffect() ||
        m_view->grid()->dragDropState() == dragActive)
    {
        updateRowsScrollTimeout();
    }
    else
    {
        if (!m_updateTimer->isActive())
            m_updateTimer->start(25);
    }
}

void GridViewPrimitiveBase::scrollXEvent(int position)
{
    Q_UNUSED(position);
    if (orientation() == Qt::Vertical)
    {
        columns()->update();
        rowsPanel()->update();
        summaryPanel()->update();
    }
    else
    {
        doScrollRows();
    }
}

void GridViewPrimitiveBase::scrollYEvent(int position)
{
    Q_UNUSED(position);
    if (orientation() == Qt::Vertical)
    {
        doScrollRows();
    }
    else
    {
        columns()->update();
        rowsPanel()->update();
        summaryPanel()->update();
    }
}

void GridViewPrimitiveBase::setColumnsScrollBarRange(int rangeMin, int rangeMax, int page, int singleStep)
{
    if (orientation() == Qt::Vertical)
    {
        scrollHor()->setRange(rangeMin, rangeMax);
        scrollHor()->setPageStep(page);
        scrollHor()->setSingleStep(singleStep);
    }
    else
    {
        scrollVer()->setRange(rangeMin, rangeMax);
        scrollVer()->setPageStep(page);
        scrollVer()->setSingleStep(singleStep);
    }
}

bool GridViewPrimitiveBase::setColumnsScrollBarVisible(bool visible)
{
    bool ret = false;
    if (orientation() == Qt::Vertical)
    {
        if (overflowX() == gsoVisible)
            visible = true;
        else if (overflowX() == gsoNone)
            visible = false;

        ret = isHorScrollBarVisible() != visible;
        setHorScrollBarVisible(visible);
    }
    else
    {
        if (overflowY() == gsoVisible)
            visible = true;
        else if (overflowY() == gsoNone)
            visible = false;

        ret = isVerScrollBarVisible() != visible;
        setVerScrollBarVisible(visible);
    }
    return ret;
}

void GridViewPrimitiveBase::setRowsScrollBarRange(int rangeMin, int rangeMax, int page, int singleStep)
{
    if (orientation() == Qt::Vertical)
    {
        scrollVer()->setRange(rangeMin, rangeMax);
        scrollVer()->setPageStep(page);
        scrollVer()->setSingleStep(singleStep);
    }
    else
    {
        scrollHor()->setRange(rangeMin, rangeMax);
        scrollHor()->setPageStep(page);
        scrollHor()->setSingleStep(singleStep);
    }
}

bool GridViewPrimitiveBase::setRowsScrollBarVisible(bool visible)
{
    bool ret = false;
    if (orientation() == Qt::Vertical)
    {
        if (overflowY() == gsoVisible)
            visible = true;
        else if (overflowY() == gsoNone)
            visible = false;

        ret = isVerScrollBarVisible() != visible;
        setVerScrollBarVisible(visible);
    }
    else
    {
        if (overflowX() == gsoVisible)
            visible = true;
        else if (overflowX() == gsoNone)
            visible = false;

        ret = isHorScrollBarVisible() != visible;
        setHorScrollBarVisible(visible);
    }
    return ret;
}


bool GridViewPrimitiveBase::checkDragDropEvent(const QPoint& pos, GridDragDropObject* object)
{
    GridTableDragColumnObject* dragObject = qobject_cast<GridTableDragColumnObject *>(object);
    if (dragObject != Q_NULL)
        return dragObject->checkDropColumnTarget(pos, 0);

    GridCellDragObject* cellDragObj = qobject_cast<GridCellDragObject *>(object);
    if (cellDragObj != Q_NULL)
        return cellDragObj->checkDragDropEvent(PreviousRowPosition, false, this);

    return false;
}

void GridViewPrimitiveBase::keyPressEvent(QKeyEvent* e)
{
    e->ignore();

//    bool verticalLayout = orientation() == Qt::Vertical;
    switch (e->key())
    {
    case Qt::Key_Home:
    {
        if (m_view->getRowCount() > 0)
        {
            if (m_view->setFocusedRowIndex(0, e->modifiers(), KeyPress))
                e->accept();
        }
    }
    break;
    case Qt::Key_End:
    {
        if (m_view->getRowCount() > 0)
        {
            if (m_view->setFocusedRowIndex(m_view->getRowCount() - 1, e->modifiers(), KeyPress))
                e->accept();
        }
    }
    break;
    case Qt::Key_PageUp:
        m_view->navigatePageUp(e->modifiers());
        e->accept();
        break;
    case Qt::Key_PageDown:
        m_view->navigatePageDown(e->modifiers());
        e->accept();
        break;
    case Qt::Key_Up:
        m_view->navigateUp(e->modifiers());

        //if (verticalLayout)
        //    m_view->navigateUp(e->modifiers());
        //else
        //    m_view->navigateLeft(e->modifiers());
        e->accept();
        break;
    case Qt::Key_Down:
        m_view->navigateDown(e->modifiers());

        //if (verticalLayout)
        //    m_view->navigateDown(e->modifiers());
        //else
        //    m_view->navigateRight(e->modifiers());
        e->accept();
        break;
    case Qt::Key_Left:
        m_view->navigateLeft(e->modifiers());

        //if (verticalLayout)
        //    m_view->navigateLeft(e->modifiers());
        //else
        //    m_view->navigateUp(e->modifiers());
        e->accept();
        break;
    case Qt::Key_Right:
        m_view->navigateRight(e->modifiers());

        //if (verticalLayout)
        //    m_view->navigateRight(e->modifiers());
        //else
        //    m_view->navigateDown(e->modifiers());
        e->accept();
        break;
    case Qt::Key_Tab:
        m_view->navigateTab(e->modifiers());
        e->accept();
        break;
    case Qt::Key_Backtab:
        m_view->navigateBacktab(e->modifiers());
        e->accept();
        break;
    case Qt::Key_F2:
    {
        if (!m_view->isEditing())
        {
            if (m_view->checkActivationEditorPolicy(KeyPress_F2))
                m_view->showEditor(KeyPress_F2);
            e->accept();
        }
    }
    break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        if (m_view->isFindPanelVisible() && e->modifiers() == Qt::ShiftModifier)
        {
            m_view->hideFindPanel(false);
            m_view->showRowOnScreen(m_view->modelController()->focusedRowIndex(), m_view->modelController()->focusedRowIndex());
        }
        else if (m_view->isFindPanelVisible() && e->modifiers() == Qt::KeyboardModifiers())
        {
            m_view->findNext();
        }
        else if (!m_view->isEditing())
        {
            if (m_view->checkActivationEditorPolicy(Enter))
                m_view->showEditor(Enter);
        }
        else
        {
            if (m_view->activeEditor() && m_view->activeEditor()->editorRepository()->validateOnEnter())
            {
                if (!m_view->activeEditor()->validate())
                    break;
            }

            m_view->closeEditor();
        }
        e->accept();
    }
    break;
    case Qt::Key_Escape:
    {
        if (m_view->isNewRowEditing())
            m_view->hideNewRowEditor();
        else if (m_view->isEditing())
            m_view->hideEditor();
        else if (m_view->isFieldChooserVisible())
            m_view->hideFieldChooser();
        else if (m_view->isFindPanelVisible())
            m_view->hideFindPanel(true);
        e->accept();
    }
    break;
    case Qt::Key_A:
    {
        if (e->modifiers() == Qt::ControlModifier)
        {
            m_view->selectAll();
            e->accept();
        }
    }
    break;
    case Qt::Key_Space:
    case Qt::Key_Select:
    {
        if (e->modifiers() == Qt::ControlModifier)
        {
            int rowIndex = m_view->modelController()->focusedRowIndex();
            int columnIndex = m_view->modelController()->focusedColumnIndex();
            m_view->setCellFocus(rowIndex, columnIndex, e->modifiers(), SelectCurrentEvent);
            e->accept();
        }
    }
    break;
    default:
        break;
    }

    if (e->isAccepted())
        return;

    if (!m_view->isEditing())
    {
        if (m_view->checkActivationEditorPolicy(KeyPress))
        {
            QChar unicodechar;
            if (e->text().size() > 0)
                unicodechar = e->text()[0];

            if (unicodechar.isLetterOrNumber() || unicodechar.isSymbol() || unicodechar.isSpace() || unicodechar.isPunct())
            {
                bool eating = m_view->checkActivationEditorPolicy(EatingKeyEvents);
                if (m_view->showEditor(eating ? KeyPress_WithEating : KeyPress))
                {
                    if (eating)
                    {
                        Q_ASSERT(m_view->m_editContext.cellGP != Q_NULL);
                        if (m_view->m_editContext.cellGP)
                        {
                            m_view->m_editContext.cellGP->keyPressEvent(e);
                        }
                    }
                    e->accept();
                    return;
                }
            }
        }
    }
}

void GridViewPrimitiveBase::showContextMenuEvent(const QPoint& pos, QContextMenuEvent* event)
{
    Q_UNUSED(pos);
    if (m_view->optionsref()->isMainMenuDisabled())
        return;
    m_view->showContextMenu(event->pos());
}

void GridViewPrimitiveBase::wheelEvent(const QPoint& pos, QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        if (event->delta() > 0)
        {
            m_view->zoomIn();
        }
        else if (event->delta() < 0)
        {
            m_view->zoomOut();
        }
        event->accept();
        return;
    }
    ScrollableGraphicPrimitive::wheelEvent(pos, event);
}

void GridViewPrimitiveBase::paintEvent(QPainter& painter)
{
    if (!m_view->optionsref()->isTransparentBackground())
        painter.fillRect(rect(), m_view->optionsref()->backgroundColor());
}

void GridViewPrimitiveBase::updateRowsScrollTimeout()
{
    updateRows();

    if (m_view->m_contextStates != NoChanges)
        return;

    if (!rowsPanel()->isUpdating())
        rowsPanel()->update();
}

GridFindPanelPrimitive* GridViewPrimitiveBase::findPanel() const
{
    return m_findPanel;
}

GridTableGroupPanelPrimitive* GridViewPrimitiveBase::groupingPanel() const
{
    return m_groupingPanel;
}

GridFilterPanelPrimitive* GridViewPrimitiveBase::filterPanel() const
{
    return m_filterPanel;
}

GridFieldChooserPrimitive* GridViewPrimitiveBase::fieldChooser() const
{
    return m_groupingPanel->m_fieldChooserButton->fieldChooser();
}

GridRowsPrimitiveAbstract* GridViewPrimitiveBase::rowsPanel() const
{
    return m_rowsPanel;
}

GridFooterSummaryPrimitive* GridViewPrimitiveBase::summaryPanel() const
{
    return m_summaryPanel;
}

GridRowsLayoutEngine* GridViewPrimitiveBase::rowsLayoutEngine() const
{
    return m_rowsPanel->layoutEngine();
}

void GridViewPrimitiveBase::init(GridViewBase* view)
{
    m_view = view;

    m_groupingPanel = new GridTableGroupPanelPrimitive(header(), view);
    setFixedGPX(m_groupingPanel, true);
    setFixedGPY(m_groupingPanel, true);

    m_findPanel = new GridFindPanelPrimitive(header(), view);
    setFixedGPX(m_findPanel, true);
    setFixedGPY(m_findPanel, true);


    m_rowsPanel = view->createRowsGraphic(central());
    setFixedGPX(m_rowsPanel, true);
    setFixedGPY(m_rowsPanel, true);

    m_summaryPanel = view->createFooterSummaryPanel(bottomRows());
    setFixedGPX(m_summaryPanel, true);
    setFixedGPY(m_summaryPanel, true);

    m_filterPanel = new GridFilterPanelPrimitive(footer(), view);
    setFixedGPX(m_filterPanel, true);
    setFixedGPY(m_filterPanel, true);
}


/* GridTableViewPrimitive */
GridTableViewPrimitive::GridTableViewPrimitive(GraphicControl* control)
    : GridViewPrimitiveBase(control), m_columnHeader(Q_NULL), m_fixedDecoration(Q_NULL)
{
}

void GridTableViewPrimitive::init(GridViewBase* view)
{
    GridViewPrimitiveBase::init(view);

    m_columnHeader = static_cast<GridTableView *>(view)->createHeaderPanel(columns());
    setFixedGPX(m_columnHeader, true);
    setFixedGPY(m_columnHeader, true);

    m_fixedDecoration = new GridTableViewFixedDecorGraphic(static_cast<GridTableView *>(view));
    setFixedGPX(m_fixedDecoration, true);
    setFixedGPY(m_fixedDecoration, true);
}

GridTableHeaderGraphic* GridTableViewPrimitive::columnHeader() const
{
    return m_columnHeader;
}

GridTableView* GridTableViewPrimitive::view() const
{
    return static_cast<GridTableView *>(m_view);
}

void GridTableViewPrimitive::updateLayout()
{
    GridViewPrimitiveBase::updateLayout();

    if (view()->options().columnAutoWidth())
        resetAutoHeightMode();

    GridViewLayoutPrimitive::updateLayout();
    setScrollAreaClipping();
    central()->updateLayout();
    m_fixedDecoration->setRect(QRect(QPoint(0, 0), boundedRect().size()));
    update();
}

void GridTableViewPrimitive::columnCreated(GridColumnBase* column)
{
    columnHeader()->addColumn(static_cast<GridTableColumnBase *>(column));
    GridViewPrimitiveBase::columnCreated(column);
}

void GridTableViewPrimitive::columnDestroying(GridColumnBase* column)
{
    GridViewPrimitiveBase::columnDestroying(column);
    columnHeader()->removeColumn(static_cast<GridTableColumnBase *>(column));
}

int GridTableViewPrimitive::getViewWidth() const
{
    return columnHeader()->viewSize().width();
}

int GridTableViewPrimitive::getViewHeight() const
{
    return columnHeader()->viewSize().height();
}

int GridTableViewPrimitive::getQuickButtonIndent() const
{
    if (orientation() == Qt::Vertical)
    {
        Q_ASSERT(m_columnHeader->quickButton()->left() == 0);
        return m_columnHeader->quickButton()->width();
    }
    else
    {
        Q_ASSERT(m_columnHeader->quickButton()->top() == 0);
        return m_columnHeader->quickButton()->height();
    }
}

int GridTableViewPrimitive::leftFixedIndent() const
{
    return m_columnHeader->leftFixedIndent();
}

int GridTableViewPrimitive::rightFixedIndent() const
{
    return m_columnHeader->rightFixedIndent();
}

bool GridTableViewPrimitive::compareColumnOrder(GridDataTableColumn* c1, GridDataTableColumn* c2)
{
    GridTableColumnPrimitive* p1 = m_columnHeader->getPrimitiveByColumn(c1);
    GridTableColumnPrimitive* p2 = m_columnHeader->getPrimitiveByColumn(c2);

    bool lessOrder = p1->left() > p2->left();

    if (p1->top() != p2->top())
        lessOrder = p1->top() > p2->top();

    return lessOrder;
}

void GridTableViewPrimitive::calculateGridRowAutoHeight(GridRow* gridRow, int* heights) const
{
    Q_ASSERT(gridRow != Q_NULL);
    GridTableView* tableView = static_cast<GridTableView *>(m_view);

    for (GridViewBase::GridColumns::const_iterator column_it = tableView->m_columnslist.constBegin();
        column_it != tableView->m_columnslist.constEnd(); ++column_it)
    {
        if (!tableView->isColumnVisible(*column_it))
            continue;

        GridTableColumn* column = static_cast<GridTableColumn *>(*column_it);
        Q_ASSERT(column->editorRepository() != Q_NULL);

        int rowIndex = tableView->getColumnRowIndex(column);
        QVariant v;
        if (!column->dataBinding()->getValue(gridRow->modelRowIndex(), column->dataBinding()->displayRole(), v))
            continue;

        int defaultHeight = get_cell_height(tableView->grid()->style(), tableView->optionsref());
        int currentHeight = column->editorRepository()->heightHint(column->width(), v);
        if (currentHeight > 0)
            currentHeight = qRound(currentHeight * tableView->options().zoomFactor());
        if (currentHeight < defaultHeight)
            currentHeight = defaultHeight;
        if (heights[rowIndex] < currentHeight)
            heights[rowIndex] = currentHeight;
    }
}

bool GridTableViewPrimitive::getAutoHeightRowCount(int& rowCount) const
{
    rowCount = 0;

    if (!m_view->optionsref()->rowAutoHeight())
        return false;

    for (GridViewBase::GridColumns::const_iterator column_it = view()->m_columnslist.constBegin();
        column_it != view()->m_columnslist.constEnd(); ++column_it)
    {
        if (!view()->isColumnVisible(*column_it))
            continue;

        if (static_cast<const GridDataTableColumn *>(*column_it)->rowSpan() > 1)
        {
            rowCount = 0;
            return false;
        }

        if (view()->getColumnRowIndex(static_cast<const GridDataTableColumn *>(*column_it)) + 1 > rowCount)
            rowCount = view()->getColumnRowIndex(static_cast<const GridDataTableColumn *>(*column_it)) + 1;
    }
    return true;
}

int GridTableViewPrimitive::calculateGridRowLength(GridRow* row, bool groupSummary)
{
    int ret = 0;
    if (orientation() == Qt::Vertical)
        ret = m_columnHeader->calculateGridRowHeightEx(row);
    else
        ret = m_columnHeader->calculateGridRowWidthEx(row);

    ret += GridViewPrimitiveBase::calculateGridRowLength(row, groupSummary);
    return ret;
}

int GridTableViewPrimitive::calculateGridSummaryRowLength(GridRow* row)
{
    return m_columnHeader->calculateGridSummaryRowLength(row);
}

int GridTableViewPrimitive::calculateNewRowLength()
{
    return m_columnHeader->calculateNewRowLength();
}

void GridTableViewPrimitive::setColumnMask(const QRect& maskRect)
{
    m_columnHeader->mask().setRect(maskRect);
}

void GridTableViewPrimitive::calcScreenScrollRect(QRect& scrollRect) const
{
    scrollRect = m_columnHeader->mask().rect();
    scrollRect.moveTo(m_columnHeader->mapToGlobal(scrollRect.topLeft()));
}

void GridTableViewPrimitive::columnChanged(GridColumnBase* column)
{
    GridTableColumnPrimitive* gp = columnHeader()->getPrimitiveByColumn(static_cast<GridTableColumnBase *>(column));
    if (gp != Q_NULL)
        gp->updateHTMLDocument();
    else
    {
        Q_ASSERT(false);
    }
}

void GridTableViewPrimitive::columnEditorChanged(GridColumnBase* column, GridTableColumnEditor* editor)
{
    GridTableColumnPrimitive* gp = columnHeader()->getPrimitiveByColumn(static_cast<GridTableColumnBase *>(column));
    if (gp != Q_NULL)
    {
        gp->deleteEditorGraphic();
        if (editor != Q_NULL)
            gp->createEditorGraphic();
    }
    else
    {
        Q_ASSERT(false);
    }
}

void GridTableViewPrimitive::columnVisibleChanged(GridColumnBase* columnBase, bool visible)
{
    GridTableColumnPrimitive* gp = columnHeader()->getPrimitiveByColumn(static_cast<GridTableColumnBase *>(columnBase));
    if (gp != Q_NULL)
        gp->setVisibility(visible);
}

void GridTableViewPrimitive::columnSizeChanged(GridColumnBase* column)
{
    columnHeader()->exactSizeChanged(static_cast<GridTableColumnBase *>(column));
}

void GridTableViewPrimitive::updateScrollBars()
{
    m_columnHeader->createTable();
    m_columnHeader->updateQuickButtonLayout();
    GridViewPrimitiveBase::updateScrollBars();
}

bool GridTableViewPrimitive::checkColumnsScrollBar(int& rangeMin, int& rangeMax, int& page, int& singleStep)
{
    return m_columnHeader->checkColumnsScrollBar(rangeMin, rangeMax, page, singleStep);
}

void GridTableViewPrimitive::orientationChanged()
{
    GridViewAutoUpdater autoUpdater(m_view);
    GridViewPrimitiveBase::orientationChanged();
    m_columnHeader->bestFit();
}

GridTableViewPrimitive::~GridTableViewPrimitive()
{
    Q_DELETE_AND_NULL(m_fixedDecoration);
    Q_DELETE_AND_NULL(m_zoomIndicator);
}

/* GridCardViewPrimitive */
GridCardViewPrimitive::GridCardViewPrimitive(GraphicControl* control)
    : GridViewPrimitiveBase(control), m_currentItemsPerLine(0), m_visualColumnItemCount(0), m_itemColumnWidth(-1), m_itemCellWidth(-1)
{
}

GridCardViewPrimitive::~GridCardViewPrimitive()
{
}

void GridCardViewPrimitive::init(GridViewBase* view)
{
    GridViewPrimitiveBase::init(view);
    fieldChooser()->setVisualOrderControlEnabled(true);
}

GridCardView* GridCardViewPrimitive::view() const
{
    return static_cast<GridCardView *>(m_view);
}

void GridCardViewPrimitive::updateLayout()
{
    m_visualColumns.clear();
    m_itemColumnWidth = 50;
    m_itemCellWidth = qMax(m_view->optionsref()->cellWidth(), 70);
    m_visualColumnItemCount = 0;
    for (GridViewBase::GridColumns::const_iterator it = view()->m_columnVisualIndexies.constBegin(); it != view()->m_columnVisualIndexies.constEnd(); ++it)
    {
        GridDataTableColumn* column = static_cast<GridDataTableColumn *>(*it);
        if (!view()->isColumnVisible(column))
            continue;
        m_itemColumnWidth = qMax(m_itemColumnWidth, column->width());
        m_visualColumns.push_back(column);
        m_visualColumnItemCount += column->rowSpan();
    }

    checkItemsPerLine();
    GridViewPrimitiveBase::updateLayout();
    GridViewLayoutPrimitive::updateLayout();
    setScrollAreaClipping();
    central()->updateLayout();
    update();
}

void GridCardViewPrimitive::columnCreated(GridColumnBase* column)
{
    GridViewPrimitiveBase::columnCreated(column);
}

void GridCardViewPrimitive::columnDestroying(GridColumnBase* column)
{
    GridViewPrimitiveBase::columnDestroying(column);
}

void GridCardViewPrimitive::checkItemsPerLine()
{
    int count = getItemsPerLine();
    if (m_currentItemsPerLine == count)
        return;
    m_currentItemsPerLine = count;
    resetAutoHeightMode();
}

int GridCardViewPrimitive::getItemsPerLine() const
{
    int groupIndent = static_cast<GridCardView *>(view())->getRowGroupIndent(Q_NULL);

    int count = 0;
    int rowSize = calculateGridRowSize(Q_NULL);

    if (orientation() == Qt::Vertical)
    {
        int width = centralAreaSize().width() - groupIndent;
        count = qFloor(width / rowSize);
    }
    else
    {
        int height = centralAreaSize().height() - groupIndent;
        count = qFloor(height / rowSize);
    }

    count = qMax(count, view()->options().minimumItemCount());
    count = qMin(count, view()->options().maximumItemCount());
    if (count < 1)
    {
        Q_ASSERT(false);
        count = 1;
    }
    return count;
}

int GridCardViewPrimitive::alignGridRowIndex(int rowIndex, int* itemsCount)
{
    GridRow* row = view()->modelController()->getGridRow(rowIndex);
    if (row != Q_NULL && row->type() == GridRow::DataRow)
    {
        int firstRowIndex = 0;

        GridRow* group = row->groupRow();
        if (group != Q_NULL)
            firstRowIndex = group->rowIndex() + 1;

        int itemsPerLine = getItemsPerLine();
        int lines = qFloor((rowIndex - firstRowIndex) / itemsPerLine);
        rowIndex = firstRowIndex + lines * itemsPerLine;

        if (itemsCount != Q_NULL)
        {
            int allRowsCount = view()->modelController()->getGridRowCount();
            if (group != Q_NULL)
                allRowsCount = firstRowIndex + (group->modelGroup()->lastRecord - group->modelGroup()->firstRecord) + 1;
            *itemsCount = qMin(allRowsCount, rowIndex + itemsPerLine) - rowIndex;
        }
    }
    else
    {
        if (itemsCount != Q_NULL)
            *itemsCount = 1;
    }

    return rowIndex;
}

bool GridCardViewPrimitive::alternatingRowColors(GridRow* row) const
{
    Q_UNUSED(row);
    return false;
}

int GridCardViewPrimitive::itemColumnWidth() const
{
    return m_itemColumnWidth;
}

int GridCardViewPrimitive::itemCellWidth() const
{
    return m_itemCellWidth;
}

bool GridCardViewPrimitive::getItemBackgroundPixmap(const QRect& rect, const QBrush& brush, QPixmap& pixmap)
{
    Q_UNUSED(brush);
    QString key = QString("bg_key_%1_%2").arg(rect.width(), rect.height());
    return m_itemBgCache.find(key, pixmap);
}

void GridCardViewPrimitive::addItemBackgroundPixmap(const QRect& rect, const QBrush& brush, const QPixmap& pixmap)
{
    Q_UNUSED(brush);
    QString key = QString("bg_key_%1_%2").arg(rect.width(), rect.height());
    m_itemBgCache.insert(key, pixmap);
}

void GridCardViewPrimitive::clearItemBackgroundPixmapCache()
{
    m_itemBgCache.clear();
}

int GridCardViewPrimitive::getViewWidth() const
{
    if (orientation() == Qt::Horizontal)
        return 0;
    int rowWidth = calculateGridRowSize(Q_NULL);
    int groupIndent = static_cast<GridCardView *>(view())->getRowGroupIndent(Q_NULL);
    return qMax(centralAreaSize().width(), getItemsPerLine() * rowWidth + groupIndent);
}

int GridCardViewPrimitive::getViewHeight() const
{
    if (orientation() == Qt::Vertical)
        return 0;
    int rowHeight = calculateGridRowSize(Q_NULL);
    int groupIndent = static_cast<GridCardView *>(view())->getRowGroupIndent(Q_NULL);
    return qMax(centralAreaSize().height(), getItemsPerLine() * rowHeight + groupIndent);
}

int GridCardViewPrimitive::getQuickButtonIndent() const
{
    return 0;
}

int GridCardViewPrimitive::leftFixedIndent() const
{
    return 0;
}

int GridCardViewPrimitive::rightFixedIndent() const
{
    return 0;
}

bool GridCardViewPrimitive::compareColumnOrder(GridDataTableColumn* c1, GridDataTableColumn* c2)
{
    Q_UNUSED(c1);
    Q_UNUSED(c2);
    return true;
}

void GridCardViewPrimitive::calculateGridRowAutoHeight(GridRow* gridRow, int* heights) const
{
    Q_ASSERT(gridRow != Q_NULL);
    GridCardView* cardView = view();
    int rowIndex = 0;
    for (GridViewBase::GridColumns::const_iterator column_it = m_visualColumns.constBegin();
        column_it != m_visualColumns.constEnd(); ++column_it)
    {
 /*       if (!cardView->isColumnVisible(*column_it))
            continue;
*/
        GridCardColumn* column = static_cast<GridCardColumn *>(*column_it);
        Q_ASSERT(column->editorRepository() != Q_NULL);

        QVariant v;
        if (!column->dataBinding()->getValue(gridRow->modelRowIndex(), column->dataBinding()->displayRole(), v))
            continue;

        int defaultHeight = get_cell_height(m_view->grid()->style(), m_view->optionsref()) * column->rowSpan();
        int currentHeight = defaultHeight;
        if (column->rowSpan() == 1)
        {
            currentHeight = column->editorRepository()->heightHint(itemCellWidth(), v);
            if (currentHeight > 0)
                currentHeight = qRound(currentHeight * cardView->options().zoomFactor());
            if (currentHeight < defaultHeight)
                currentHeight = defaultHeight;
        }

        heights[rowIndex] = currentHeight;
        rowIndex++;
    }
}

bool GridCardViewPrimitive::getAutoHeightRowCount(int& rowCount) const
{
    rowCount = 0;
    if (!m_view->optionsref()->rowAutoHeight())
        return false;
    GridViewBase::GridColumns columns;
    view()->getColumnLine(Q_NULL, columns);
    rowCount = columns.size();
    return true;
}

int GridCardViewPrimitive::calculateGridDataRowWidth(GridRow* row) const
{
    Q_UNUSED(row);
    return itemColumnWidth() + itemCellWidth() + (/*view()->options().zoomFactor()*/1 * (view()->options().itemMargin() * 2 + view()->options().itemPadding() * 2));
}

int GridCardViewPrimitive::calculateGridDataRowHeight(GridRow* row) const
{
    int ret = 0;
    int autoCount = autoHeightRowCount();
    if (row != Q_NULL && autoCount > 0)
    {
        ret = getGridRowAutoHeight(row);
    }
    else
    {
        int defCellHeight = get_cell_height(m_view->grid()->style(), m_view->optionsref());
        ret = m_visualColumnItemCount * defCellHeight;
    }

    ret += (/*view()->options().zoomFactor()*/1 * (view()->options().itemMargin() * 2 + view()->options().itemPadding() * 2));
    ret += view()->options().itemTitleHeight() * view()->options().zoomFactor();
    return ret;
}

int GridCardViewPrimitive::calculateGridRowLength(GridRow* row, bool groupSummary)
{
    bool verticalLayout = orientation() == Qt::Vertical;
    int ret = 0;

    if (row->type() == GridRow::DataRow)
    {
        if (verticalLayout)
            ret = calculateGridDataRowHeight(row);
        else
            ret = calculateGridDataRowWidth(row);
    }
    else
    {
        int gridLinesWidth, gridLinesHeight;
        bool _2d;
        get_cell_border_lines(m_view, gridLinesWidth, gridLinesHeight, _2d);

        int defCellHeight = get_group_height(control()->style(), m_view->optionsref());
        int lines = verticalLayout ? gridLinesHeight : gridLinesWidth;
        ret = defCellHeight + lines;

        if (view()->options().groupSummaryPlace() == GroupSummaryRowPlus)
        {
            ret += calculateGridSummaryRowLength(row);
        }
        else if (view()->options().groupSummaryPlace() == GroupSummaryRow)
        {
            ret = qMax(ret, calculateGridSummaryRowLength(row) + lines);
        }
    }
    ret += GridViewPrimitiveBase::calculateGridRowLength(row, groupSummary);
    return ret;
}

int GridCardViewPrimitive::calculateGridSummaryRowLength(GridRow* row)
{
    bool verticalLayout = orientation() == Qt::Vertical;
    int borderRight = 0, borderBottom = 0;
    bool _2d = false;
    if (row != Q_NULL && view()->options().groupSummaryPlace() == GroupSummaryFooter)
        get_cell_border_lines(m_view, borderRight, borderBottom, _2d);

    if (verticalLayout)
    {
        int defCellHeight = get_cell_height(m_view->grid()->style(), m_view->optionsref());

        int count = 0;
        if (row == Q_NULL /* footer summary */)
        {
            count = view()->modelController()->summary()->footerSettingsCount();
        }
        else
        {
            GridModelGroup* group = row->modelGroup();
            Q_ASSERT(group != Q_NULL);
            if (group == Q_NULL)
                return 40;
            count = view()->modelController()->summary()->defaultGroupSettingsCount();
            count += view()->modelController()->summary()->groupSettingsCount(group->dataBinding);
        }
        count = qMax(count, 1);
        return kGridSummaryCellBorder + count * (defCellHeight + kGridSummaryCellBorder) + borderBottom;
    }
    else
    {
        const int width = 100;
        return kGridSummaryCellBorder + width + kGridSummaryCellBorder + borderRight;
    }
}

int GridCardViewPrimitive::calculateNewRowLength()
{
    if (m_view->isNewRowEditing())
        return calculateGridDataRowHeight(Q_NULL);
    else
        return 50;
}

int GridCardViewPrimitive::calculateGridRowSize(GridRow* row) const
{
    Q_UNUSED(row);
    bool verticalLayout = orientation() == Qt::Vertical;
    int ret = 0;
    if (verticalLayout)
        ret = calculateGridDataRowWidth(row);
    else
        ret = calculateGridDataRowHeight(row);
    return ret;
}

void GridCardViewPrimitive::columnChanged(GridColumnBase* column)
{
    GridDataTableColumn* dataColumn = static_cast<GridDataTableColumn *>(column);

    for (int i = 0; i < rowsLayoutEngine()->getVirtualRowsCount(); ++i)
    {
        GridCardDataRowPrimitive* cardDataRowPrimitive = static_cast<GridCardDataRowPrimitive *>((rowsLayoutEngine()->getVirtualRowByIndex(i))->dataRowGP());
        QHash<const GridDataTableColumn *, GridCardColumnPrimitive *>::iterator column_it = cardDataRowPrimitive->m_columnGPs.find(dataColumn);
        if (column_it == cardDataRowPrimitive->m_columnGPs.end())
            continue;
        column_it.value()->updateHTMLDocument();
    }

    GridCardDataRowPrimitive* cardDataRowPrimitive = static_cast<GridCardDataRowPrimitive *>((rowsLayoutEngine()->focusedRowLayoutItem())->dataRowGP());
    QHash<const GridDataTableColumn *, GridCardColumnPrimitive *>::iterator column_it = cardDataRowPrimitive->m_columnGPs.find(dataColumn);
    if (column_it == cardDataRowPrimitive->m_columnGPs.end())
        return;
    column_it.value()->updateHTMLDocument();
}

void GridCardViewPrimitive::columnEditorChanged(GridColumnBase* column, GridTableColumnEditor* editor)
{
    GridDataTableColumn* dataColumn = static_cast<GridDataTableColumn *>(column);

    for (int i = 0; i < rowsLayoutEngine()->getVirtualRowsCount(); ++i)
    {
        GridCardDataRowPrimitive* cardDataRowPrimitive = static_cast<GridCardDataRowPrimitive *>((rowsLayoutEngine()->getVirtualRowByIndex(i))->dataRowGP());
        QHash<const GridDataTableColumn *, GridCardColumnPrimitive *>::iterator column_it = cardDataRowPrimitive->m_columnGPs.find(dataColumn);
        if (column_it == cardDataRowPrimitive->m_columnGPs.end())
            continue;
        column_it.value()->deleteEditorGraphic();
        if (editor != Q_NULL)
            column_it.value()->createEditorGraphic();
    }

    GridCardDataRowPrimitive* cardDataRowPrimitive = static_cast<GridCardDataRowPrimitive *>((rowsLayoutEngine()->focusedRowLayoutItem())->dataRowGP());
    QHash<const GridDataTableColumn *, GridCardColumnPrimitive *>::iterator column_it = cardDataRowPrimitive->m_columnGPs.find(dataColumn);
    if (column_it == cardDataRowPrimitive->m_columnGPs.end())
        return;
    column_it.value()->deleteEditorGraphic();
    if (editor != Q_NULL)
        column_it.value()->createEditorGraphic();
}

void GridCardViewPrimitive::columnVisibleChanged(GridColumnBase* column, bool visible)
{
    Q_UNUSED(column);
    Q_UNUSED(visible);
}

void GridCardViewPrimitive::columnSizeChanged(GridColumnBase* column)
{
    Q_UNUSED(column);
}

void GridCardViewPrimitive::orientationChanged()
{
    GridViewPrimitiveBase::orientationChanged();
}

bool GridCardViewPrimitive::checkColumnsScrollBar(int& rangeMin, int& rangeMax, int& page, int& singleStep)
{
    bool ret = false;
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    if (verticalLayout)
    {
        //m_leftFixedIndent = 0;
        //m_rightOrBottomFixedIndent = 0;

        int width = m_view->layout()->centralAreaSize().width();
        //int groupingIndent = static_cast<GridCardView *>(view())->getRowGroupIndent(Q_NULL);

        int newWidth = getViewWidth();

        width = qMax(0, width - 0);
        newWidth = qMax(0, newWidth - 0);
        ret = newWidth > width;
        //m_view->layout()->scrollHor()->setVisible(true);
        rangeMin = 0;
        rangeMax = qMax(0, newWidth - width);
        page = width;
        singleStep = qCeil(width / 3); /* pixels */

        //m_view->layout()->scrollHor()->setRange(0, qMax(0, newWidth - width));
        //m_view->layout()->scrollHor()->setPageStep(width);

    }
    else
    {
        int height = m_view->layout()->centralAreaSize().height();
        int newHeihgt = getViewHeight();// static_cast<GridCardView *>(view())->getRowGroupIndent(Q_NULL);

        //for (QMap<int, int>::const_iterator it = m_columnHeightTable.constBegin(); it != m_columnHeightTable.constEnd(); ++it)
        //    newHeihgt += it.value();
        height = qMax(0, height - 0);
        newHeihgt = qMax(0, newHeihgt - 0);
        ret = newHeihgt > height;
        //m_view->layout()->scrollVer()->setVisible(true);
        rangeMin = 0;
        rangeMax = qMax(0, newHeihgt - height);
        page = height;
        singleStep = qCeil(height / 3); /* pixels */
        //m_view->layout()->scrollVer()->setRange(0, qMax(0, newHeihgt - height));
        //m_view->layout()->scrollVer()->setPageStep(height);
    }
    return ret;
}



#if 0
/* GridWindow */
GridWindow::GridWindow(GridViewBase* view)
    : GraphicControl(0)
{
 //   view->m_gw1 = this;
    m_viewPrimitive = new GridViewPrimitive(this);
    m_viewPrimitive->init(view);
    m_viewPrimitive->setBorder(3);
    setRoot(m_viewPrimitive);
}
#endif

