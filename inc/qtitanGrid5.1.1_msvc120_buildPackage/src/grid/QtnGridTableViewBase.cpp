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

#include <QToolTip>
#include <QBitmap>
#include <QDesktopWidget>
#include <QAbstractTextDocumentLayout>
#include <QListView>
#include "QtnGridDef.h"
#include "QtnGridTableViewBase.h"
#include "QtnGridViewGraphics.h"
#include "QtnGridTableView.h"
#include "QtnGridFilterWindow.h"
#include "QtnGridBandedTableView.h"

QTITAN_USE_NAMESPACE

const int filterPanelIndent = 3;

namespace Qtitan
{
    class GridTableViewEx : public GridTableView
    {
        friend class GridFieldChooserColumnProviderImpl;
        GridTableViewEx() : GridTableView(0) {}
    };

    class GridStretchWidthLayout
    {
    public:
        struct StretchWidthInfo
        {
            int minWidth;
            int maxWidth;
            int width;
            int newWidth;
            bool ignore;
            StretchWidthInfo(int min, int max, int w, int ign)
                : minWidth(min), maxWidth(max), width(w), newWidth(-1), ignore(ign) { }
        };
        StretchWidthInfo& getInfoByIndex(int i);
        int infoCount();
        int widthSum();
        void addColumnInfo(int min, int max, int width, bool ignore);
        void execute();
        GridStretchWidthLayout();
        void setViewWidth(int viewWidth);
    private:
        QList<StretchWidthInfo> m_infolist;
        int m_viewWidth;
    };

    /* GridStretchWidthLayout */
    int GridStretchWidthLayout::widthSum()
    {
        int retval = 0;
        for (QList<StretchWidthInfo>::const_iterator it = m_infolist.constBegin(); it != m_infolist.constEnd(); ++it)
            retval += it->width;
        return retval;
    }

    GridStretchWidthLayout::StretchWidthInfo&
        GridStretchWidthLayout::getInfoByIndex(int i)
    {
        return m_infolist[i];
    }

    int GridStretchWidthLayout::infoCount()
    {
        return m_infolist.size();
    }

    void GridStretchWidthLayout::addColumnInfo(int min, int max, int width, bool ignore)
    {
        m_infolist.push_back(StretchWidthInfo(min, max, width, ignore));
    }

    GridStretchWidthLayout::GridStretchWidthLayout()
        : m_viewWidth(0)
    {
    }

    void GridStretchWidthLayout::setViewWidth(int viewWidth)
    {
        m_viewWidth = viewWidth;
    }

    void GridStretchWidthLayout::execute()
    {
        int width;
        int curWidth;
        int viewWidth = m_viewWidth;
        curWidth = width = widthSum();
        int widthRests = viewWidth;

        for (QList<StretchWidthInfo>::iterator it = m_infolist.begin(); it != m_infolist.end(); ++it)
        {
            if (it->ignore)
            {
                it->newWidth = it->width;
                if (it->newWidth < it->minWidth)
                    it->newWidth = it->minWidth;
                if (it->newWidth > it->maxWidth)
                    it->newWidth = it->maxWidth;
                widthRests -= it->newWidth;
                curWidth -= it->width;
            }
        }

        bool breakProcess;
        bool found = false;
        do
        {
            breakProcess = false;
            int delta = 0;
            viewWidth = widthRests;
            width = curWidth;

            for (QList<StretchWidthInfo>::iterator it = m_infolist.begin(); it != m_infolist.end(); ++it)
            {
                if (it->newWidth == -1)
                {
                    int itemWidth =
                        mul_div(it->width + delta, viewWidth, width) -
                        mul_div(delta, viewWidth, width);

                    if (found)
                        it->newWidth = itemWidth;
                    else
                    {
                        if (itemWidth <= it->minWidth)
                        {
                            it->newWidth = it->minWidth;
                            widthRests -= it->newWidth;
                            curWidth -= it->width;
                            breakProcess = true;
                        }
                        else if (itemWidth >= it->maxWidth)
                        {
                            it->newWidth = it->maxWidth;
                            widthRests -= it->newWidth;
                            curWidth -= it->width;
                            breakProcess = true;
                        }
                    }
                    delta += it->width;
                }
            }
            found = breakProcess ? found : !found;
        } while (!((curWidth == 0) || (!breakProcess && !found)));
    }
};

/*!
\internal
*/
QFont Qtitan::qtn_font_apply_scale(const QFont& font, double factor)
{
    if (factor == 1.0)
        return font;
    QFont f = font;
    if (f.pixelSize() != -1)
        f.setPixelSize(qRound(f.pixelSize() * factor));
    else
        f.setPointSize(qRound(f.pointSize() * factor));
    return f;
}

QSize qtn_size_apply_scale(const QSize& size, double factor)
{
    if (factor == 1.0)
        return size;
    return QSize(qRound(size.width() * factor), qRound(size.height() * factor));
}
/*!
    \internal
    Returns rectangle of text.
*/ 
QRect Qtitan::paint_icon_and_text(QPainter& painter,
                                const QRect& rect,
                                const QPixmap& icon,
                                const QString& text,
                                Qt::Alignment textAlignment,
                                Qt::TextElideMode textElideMode,
                                IconApperance iconApperance,
                                Qt::LayoutDirection direction,
                                Qt::Orientation orientation,
                                bool air,
                                int margin,
                                bool* textFullyVisible,
                                const QString& selectedText,
                                Qt::CaseSensitivity selectCaseSensitivity)
{
    Q_UNUSED(air);
    if (textFullyVisible)
        *textFullyVisible = false;

    QRect iconRect = rect;
    QRect textRect = rect;
    if (orientation == Qt::Vertical)
    {
        painter.save();
        painter.rotate(-90);
        iconRect = QRect(-iconRect.top() - iconRect.height(), iconRect.left(), iconRect.height(), iconRect.width());
        textRect = QRect(-textRect.top() - textRect.height(), textRect.left(), textRect.height(), textRect.width());
    }

    if (!icon.isNull() && (iconApperance != IconInvisible))
    {
        int iconSize = icon.width();
  
        if (iconApperance == IconAlignLeft)
        {
            iconRect.setWidth(iconSize);
            textRect.adjust(iconSize + margin, 0, 0, 0);
        }
        else if (iconApperance == IconAlignRight)
        {
            iconRect.adjust(iconRect.width() - iconSize, 0, 0, 0);
            textRect.setWidth(textRect.width() - (iconSize + margin));
        }

        QRect r = QStyle::alignedRect(direction,
            Qt::AlignHCenter | Qt::AlignVCenter,
            icon.size(),
            iconRect);
        
        painter.drawPixmap(r, icon);

        if (iconApperance == IconOnly)
        {
            textRect = QRect();
        }
    }
    if (textRect.isEmpty())
        return textRect;

    QPalette palette;// = control()->palette();
    QVector<QTextLayout::FormatRange> selections;
    if (selectedText.size() > 0)
    {
        int i = 0;
        while ((i = text.indexOf(selectedText, i, selectCaseSensitivity)) != -1)
        {
            QTextLayout::FormatRange range;
            range.start = i;
            range.length = selectedText.size();
            i += selectedText.size();

            range.format.setBackground(palette.brush(QPalette::Highlight));
            range.format.setForeground(palette.brush(QPalette::HighlightedText));
            selections.append(range);
        }
    }

    QString newText = text;
    QRect textBoundingRect = painter.fontMetrics().boundingRect(newText);
    textBoundingRect.adjust(0, 0, 1, 0);
    if (textBoundingRect.width() > textRect.width())
        newText = painter.fontMetrics().elidedText(newText, textElideMode, textRect.width());
    else
    {
        if (textFullyVisible)
            *textFullyVisible = true;
    }

    QRect newTextRect = textRect;
    if (!newText.isEmpty())
    {
#ifdef Q_OS_WIN            
        if (air)
            drawDwmCaptionText(&painter, newTextRect, newText, QStyle::State_Enabled);
        else
#endif
        {
            Qt::Alignment temp = textAlignment | Qt::AlignVCenter;
            //Error rendering text under WinXP
            //temp &= ~Qt::AlignRight;
            //temp &= ~Qt::AlignHCenter;
            //temp |= Qt::AlignLeft;
            QTextOption textOption(temp);
            textOption.setWrapMode(QTextOption::NoWrap);

            if (selections.size() == 0)
            {
                painter.drawText(newTextRect, newText, textOption);
            }
            else
            {
                QTextLayout textLayout(newText, painter.font());
                textLayout.setTextOption(textOption);

                int leading = painter.fontMetrics().leading();
                qreal height = 0;
                qreal width = 0;
                textLayout.beginLayout();
                while (1) {
                    QTextLine line = textLayout.createLine();
                    if (!line.isValid())
                        break;

                    line.setLineWidth(newTextRect.width());
                    height += leading;
                    line.setPosition(QPointF(0, height));
                    height += line.height();
                    width = qMax(width, line.naturalTextWidth());
                }
                textLayout.endLayout();


                qreal yoffset = 0;
                qreal xoffset = 0;
                if (textOption.alignment() & Qt::AlignBottom)
                    yoffset = newTextRect.height() - height;
                else if (textOption.alignment() & Qt::AlignVCenter)
                    yoffset = (newTextRect.height() - height) / 2;
#if 0
                if (textOption.alignment() & Qt::AlignRight)
                    xoffset = newTextRect.width() - width;
                else if (textOption.alignment() & Qt::AlignHCenter)
                    xoffset = (newTextRect.width() - width) / 2;
#endif                
                textLayout.draw(&painter, newTextRect.topLeft() + QPoint(xoffset, yoffset), selections);
            }
        }
    }

    if (orientation == Qt::Vertical)
    {
        painter.restore();
    }

    return textRect;
}

/* GridRowsPrimitiveAbstract */
GridRowsPrimitiveAbstract::GridRowsPrimitiveAbstract(GridViewBase* view, GraphicPrimitive* parent)
    : ScrollableGraphicPrimitive(parent), m_view(view), m_layoutEngine(Q_NULL),
    m_newRowPanelGP(Q_NULL), m_newRowSplitter(Q_NULL), m_topFrozenRowsSplitter(Q_NULL), m_bottomFrozenRowsSplitter(Q_NULL), m_topFixedIndent(0), m_bottomFixedIndent(0)
{
    m_newRowSplitter = new GridRowSplitterPrimitive(view, this);
    m_topFrozenRowsSplitter = new GridRowSplitterPrimitive(view, this);
    m_bottomFrozenRowsSplitter = new GridRowSplitterPrimitive(view, this);

    m_newRowSplitter->setVisibility(false);
    m_topFrozenRowsSplitter->setVisibility(false);
    m_bottomFrozenRowsSplitter->setVisibility(false);
}

GridRowsPrimitiveAbstract::~GridRowsPrimitiveAbstract()
{
    Q_DELETE_AND_NULL(m_layoutEngine);
}

GridRowsLayoutEngine* GridRowsPrimitiveAbstract::layoutEngine() const
{
    return m_layoutEngine;
}

GridNewRowPrimitive* GridRowsPrimitiveAbstract::newRowPanel() const
{ 
    return m_newRowPanelGP;
}

void GridRowsPrimitiveAbstract::updateFixedLayout()
{
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    switch (m_view->optionsref()->newRowPlace())
    {
    case NewRowTop:
    {
        m_newRowPanelGP->setVisibility(!m_view->isNewRowEditing());
        m_newRowSplitter->setVisibility(true);
        m_newRowSplitter->setBorderBrush(m_view->optionsref()->gridLineBrush());
        if (verticalLayout)
        {
            int height = m_view->layout()->calculateNewRowLength();
            QRect rowRect(0, 0, m_view->layout()->getViewWidth(), height);
            m_newRowPanelGP->setRect(rowRect);
            m_newRowSplitter->setRect(QRect(0, rowRect.top() + rowRect.height(), rowRect.left() + rowRect.width(), kGridNewRowIndent));
        }
        else
        {
            int width = m_view->layout()->calculateNewRowLength();
            QRect rowRect(0, 0, width, m_view->layout()->getViewHeight());
            m_newRowPanelGP->setRect(rowRect);
            m_newRowSplitter->setRect(QRect(rowRect.left() + rowRect.width(), 0, kGridNewRowIndent, rowRect.top() + rowRect.height()));
        }
    }
    break;
    case NewRowBottom:
    {
        m_newRowPanelGP->setVisibility(!m_view->isNewRowEditing());
        m_newRowSplitter->setVisibility(true);
        m_newRowSplitter->setBorderBrush(m_view->optionsref()->gridLineBrush());

        QRect scrollAreaRect = m_view->layout()->rowsScrollAreaRect();
        if (verticalLayout)
        {
            int top = scrollAreaRect.top() + scrollAreaRect.height();
            int height = m_view->layout()->calculateNewRowLength();
            top = top + m_bottomFixedIndent - height;
            QRect rowRect(0, top, m_view->layout()->getViewWidth(), height);
            m_newRowPanelGP->setRect(rowRect);
            m_newRowSplitter->setRect(QRect(0, rowRect.top() - kGridNewRowIndent, rowRect.width(), kGridNewRowIndent));
        }
        else
        {
            int left = scrollAreaRect.left() + scrollAreaRect.width();
            int width = m_view->layout()->calculateNewRowLength();
            left = left + m_bottomFixedIndent - width;
            QRect rowRect(left, 0, width, m_view->layout()->getViewHeight());
            m_newRowPanelGP->setRect(rowRect);
            m_newRowSplitter->setRect(QRect(rowRect.left() - kGridNewRowIndent, 0, kGridNewRowIndent, rowRect.height()));
        }
    }
    break;
    default:
    {
        m_newRowPanelGP->setVisibility(false);
        m_newRowPanelGP->setRect(QRect(0, 0, 0, 0));
        m_newRowSplitter->setVisibility(false);
        m_newRowSplitter->setRect(QRect(0, 0, 0, 0));
    }
    }

    if (verticalLayout)
    {
        m_topFrozenRowsSplitter->setVisibility(m_view->modelController()->getFrozenTopGridRowCount() > 0);
        if (m_topFrozenRowsSplitter->visibility())
            m_topFrozenRowsSplitter->setRect(QRect(0, m_topFixedIndent - kGridNewRowIndent, m_view->layout()->getViewWidth(), kGridNewRowIndent));

        m_bottomFrozenRowsSplitter->setVisibility(m_view->modelController()->getFrozenBottomGridRowCount() > 0);
        if (m_bottomFrozenRowsSplitter->visibility())
        {
            QRect scrollAreaRect = m_view->layout()->rowsScrollAreaRect();
            m_bottomFrozenRowsSplitter->setRect(QRect(0, scrollAreaRect.top() + scrollAreaRect.height(), m_view->layout()->getViewWidth(), kGridNewRowIndent));
        }
    }
    else
    {
        m_topFrozenRowsSplitter->setVisibility(m_view->modelController()->getFrozenTopGridRowCount() > 0);
        if (m_topFrozenRowsSplitter->visibility())
            m_topFrozenRowsSplitter->setRect(QRect(m_topFixedIndent - kGridNewRowIndent, 0, kGridNewRowIndent, m_view->layout()->getViewHeight()));

        m_bottomFrozenRowsSplitter->setVisibility(m_view->modelController()->getFrozenBottomGridRowCount() > 0);
        if (m_bottomFrozenRowsSplitter->visibility())
        {
            QRect scrollAreaRect = m_view->layout()->rowsScrollAreaRect();
            m_bottomFrozenRowsSplitter->setRect(QRect(scrollAreaRect.left() + scrollAreaRect.width(), 0, kGridNewRowIndent, m_view->layout()->getViewHeight()));
        }
    }
}


void GridRowsPrimitiveAbstract::updateLayout()
{
    m_view->layout()->updateRows(true);
}

void GridRowsPrimitiveAbstract::calculateFixedRows()
{
    m_topFixedIndent = 0;
    m_bottomFixedIndent = 0;
}

int GridRowsPrimitiveAbstract::topFixedIndent() const
{
    return m_topFixedIndent;
}

int GridRowsPrimitiveAbstract::bottomFixedIndent() const
{
    return m_bottomFixedIndent;
}

bool GridRowsPrimitiveAbstract::checkDragDropEvent(const QPoint& pos, GridDragDropObject* object)
{
    return static_cast<GridRowsPrimitiveAbstract *>(parentGP())->checkDragDropEvent(pos, object);
}

/* GridTableRowsPrimitive */
GridTableRowsPrimitive::GridTableRowsPrimitive(GridViewBase* view, GraphicPrimitive* parent)
    : GridRowsPrimitiveAbstract(view, parent)
{
    m_newRowPanelGP = new GridNewRowPrimitive(view, this);
    m_newRowPanelGP->setVisibility(false);

    GridTableView* tableView = static_cast<GridTableView *>(view);
    m_layoutEngine = new GridTableRowsLayoutEngine(tableView, this);
    m_layoutEngine->initialize();
}

GridTableRowsPrimitive::~GridTableRowsPrimitive()
{
    Q_DELETE_AND_NULL(m_newRowPanelGP);
    Q_DELETE_AND_NULL(m_newRowSplitter);
    Q_DELETE_AND_NULL(m_topFrozenRowsSplitter);
    Q_DELETE_AND_NULL(m_bottomFrozenRowsSplitter);
}

void GridTableRowsPrimitive::updateLayout()
{
    updateFixedLayout();
    static_cast<GridTableView *>(m_view)->layout()->columnHeader()->prepareCellsLayout();
    GridRowsPrimitiveAbstract::updateLayout();
}

void GridTableRowsPrimitive::calculateFixedRows()
{
    GridRowsPrimitiveAbstract::calculateFixedRows();

    GridTableView* view = static_cast<GridTableView *>(m_view);

    if (view->options().newRowPlace() == NewRowTop)
    {
        m_topFixedIndent += view->layout()->columnHeader()->calculateNewRowLength();
        m_topFixedIndent += kGridNewRowIndent;
    }

    for (int i = 0; i < view->modelController()->getFrozenTopGridRowCount(); ++i)
        m_topFixedIndent += view->layout()->calculateGridRowLength(view->modelController()->getGridRow(i), false);

    if (view->modelController()->getFrozenTopGridRowCount() > 0)
        m_topFixedIndent += kGridNewRowIndent;

    if (view->options().newRowPlace() == NewRowBottom)
    {
        m_bottomFixedIndent += view->layout()->columnHeader()->calculateNewRowLength();
        m_bottomFixedIndent += kGridNewRowIndent;
    }

    for (int i = view->modelController()->getGridRowCount() - view->modelController()->getFrozenBottomGridRowCount(); i < view->modelController()->getGridRowCount(); ++i)
        m_bottomFixedIndent += view->layout()->calculateGridRowLength(view->modelController()->getGridRow(i), false);

    if (view->modelController()->getFrozenBottomGridRowCount() > 0)
        m_bottomFixedIndent += kGridNewRowIndent;
}

GridRowsLayoutEngine* GridTableRowsPrimitive::createPrinterLayoutEngine() const
{
    GridRowsLayoutEngine* ret = new GridTableRowsLayoutEngine(qobject_cast<GridTableView *>(m_view), Q_NULL);
    ret->initialize();
    return ret;
}

/* GridCloseButtonPrimitive */
GridCloseButtonPrimitive::GridCloseButtonPrimitive(GraphicPrimitive* parent)
    : GraphicPrimitive(parent->control(), parent)
{
}

QSize GridCloseButtonPrimitive::sizeHint()
{
    return QSize(12, 12);
}

void GridCloseButtonPrimitive::mouseMoveEvent(const QPoint& point, QMouseEvent* e)
{
    GraphicPrimitive::mouseMoveEvent(point, e);
    parentGP()->update();
}

void GridCloseButtonPrimitive::mouseOverEvent(const QPoint& pos)
{
    GraphicPrimitive::mouseOverEvent(pos);
    parentGP()->update();
}

void GridCloseButtonPrimitive::mouseOutEvent()
{
    GraphicPrimitive::mouseOutEvent();
    parentGP()->update();
}

void GridCloseButtonPrimitive::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
    parentGP()->update();
}

void GridCloseButtonPrimitive::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
    if (isLeftButtonPressed() && isMouseOver())
        clicked();
    else
        parentGP()->update();
}

void GridCloseButtonPrimitive::paintEvent(QPainter& painter)
{
    QPixmap texture;
    if (isLeftButtonPressed() && isMouseOver())
        texture = QPixmap(":res/grid_header_tab_close_press.png");
    else if (isMouseOver())
        texture = QPixmap(":res/grid_header_tab_close_over.png");
    else
        texture = QPixmap(":res/grid_header_tab_close.png");
    painter.drawPixmap(0, 0, texture);
}

/* GridHeaderColumnTabCloseButtonGraphic */
GridHeaderColumnTabCloseButtonGraphic::GridHeaderColumnTabCloseButtonGraphic(GraphicPrimitive* parent)
    : GridCloseButtonPrimitive(parent)
{
}

void GridHeaderColumnTabCloseButtonGraphic::clicked()
{
    GridTableColumn* column = (GridTableColumn *)(static_cast<GridTabColumnPrimitive *>(parentGP())->column());
    {
        GridViewAutoUpdater autoUpdater(column->view());
        column->setGroupIndex(-1);
        column->setVisible(true);
    }
}

/* GridShadeButtonPrimitive */
GridShadeButtonPrimitive::GridShadeButtonPrimitive(GraphicPrimitive* parent)
    : GraphicPrimitive(parent), m_autoRise(false)
{
}

void GridShadeButtonPrimitive::setText(const QString& text)
{
    m_text = text;
}

const QString& GridShadeButtonPrimitive::text() const
{
    return m_text;
}

void GridShadeButtonPrimitive::setIcon(const QIcon& icon)
{
    m_icon = icon;
}

const QIcon& GridShadeButtonPrimitive::icon() const
{
    return m_icon;
}

QSize GridShadeButtonPrimitive::sizeHint()
{
    QSize s = get_column_sizeHint(Q_NULL, control()->style(), control()->font(), 1.0, text());
    if (!icon().isNull())
        s.rwidth() += s.height() + 6;
    return s;
}

bool GridShadeButtonPrimitive::autoRise() const
{
    return m_autoRise;
}

void GridShadeButtonPrimitive::setAutoRaise(bool autoRaise)
{
    m_autoRise = autoRaise;
}

void GridShadeButtonPrimitive::mouseOverEvent(const QPoint& pos)
{
    static_cast<GridShadeButtonPrimitive *>(parentGP())->mouseOverEvent(pos);
    update();
}

void GridShadeButtonPrimitive::mouseOutEvent()
{
    static_cast<GridShadeButtonPrimitive *>(parentGP())->mouseOutEvent();
    update();
}

void GridShadeButtonPrimitive::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(e);
    Q_UNUSED(point);
    update();
}

void GridShadeButtonPrimitive::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    if (checkClickEvent(e, true))
        doClick(e);
    update();
}

void GridShadeButtonPrimitive::mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(e);
    Q_UNUSED(point);
    update();
}

void GridShadeButtonPrimitive::paintTexture(QPainter& painter, const QColor& accentColor, const QRect& rect)
{
    QPixmap texture(":res/grid_glass_bg.png");
    QPixmap temp(texture.width(), texture.height());
    temp.fill(accentColor);
    temp.setMask(texture.createHeuristicMask());
    {
        QPainter p(&temp);
        p.drawPixmap(0, 0, texture);
    }
    draw_pixmap_texture(painter, rect, temp, 6, 6, 5, 5);
}

void GridShadeButtonPrimitive::paintEvent(QPainter& painter)
{
    QPalette pal = getButtonPalette();
    painter.save();
    QColor accentColor;
    if (isLeftButtonPressed() && isMouseOverRecursive())
    {
        accentColor = pal.color(QPalette::Dark);
    }
    else
    {
        if (isMouseOverRecursive())
            accentColor = pal.color(QPalette::Light);
        else if (!autoRise())
            accentColor = pal.color(QPalette::Mid);
        else
            accentColor = Qt::transparent;
    }
    QRect r = QRect(0, 0, width(), height());
    if (accentColor != Qt::transparent)
    {
//        painter.setOpacity(0.5);
        paintTexture(painter, accentColor, r);
    }

    r.adjust(5, 0, -5, 0);

    painter.setFont(control()->font());
    painter.setPen(pal.color(QPalette::Text));
    painter.setOpacity(1.0);
    paint_icon_and_text(painter, r, icon().pixmap(width()), text(), Qt::AlignCenter, Qt::ElideRight, text().isEmpty() ? IconOnly : IconAlignLeft);
    painter.restore();
}

void GridShadeButtonPrimitive::doClick(QMouseEvent* e)
{
    Q_UNUSED(e);
    emit clicked();
}

QPalette GridShadeButtonPrimitive::getButtonPalette() const
{
    QPalette retval = control()->palette();
    retval.setColor(QPalette::Mid, qtn_get_thememanager(control())->shadePalette().mid().color());
    retval.setColor(QPalette::Light, qtn_get_thememanager(control())->shadePalette().light().color());
    retval.setColor(QPalette::Dark, qtn_get_thememanager(control())->shadePalette().dark().color());
    return retval;
}


/* GridTabColumnPrimitive */
GridTabColumnPrimitiveBase::GridTabColumnPrimitiveBase(GraphicPrimitive* parent, GridColumnBase* column)
    : GridShadeButtonPrimitive(parent), m_column(column)
{
    Q_ASSERT(m_column != Q_NULL);
}

GridColumnBase* GridTabColumnPrimitiveBase::column() const
{
    return m_column;
}

QPalette GridTabColumnPrimitiveBase::getButtonPalette() const
{
    QPalette retval = GridShadeButtonPrimitive::getButtonPalette();
    retval.setColor(QPalette::Text, Qt::white);
    return retval;
}

QSize GridTabColumnPrimitiveBase::sizeHint()
{
    QSize s = get_column_sizeHint(m_column, control()->style(), m_column->viewBase()->getColumnFont(m_column), 1.0, m_column->caption());
  
    QStyleOptionHeader opt;
    opt.initFrom(control());
    int margin = control()->style()->pixelMetric(QStyle::PM_HeaderMargin, &opt, control());
    s.rwidth() += extraSizeHintIndent(margin);
   
    s.rwidth() = qMin(250, s.width());
    s.rwidth() = qMax(40, s.width());

    return s;
}

void GridTabColumnPrimitiveBase::paintTexture(QPainter& painter, const QColor& accentColor, const QRect& rect)
{
    QPixmap texture(":res/grid_glass_column_bg.png");
    QPixmap temp(texture.width(), texture.height());
    temp.fill(accentColor);
    temp.setMask(texture.createHeuristicMask());
    {
        QPainter p(&temp);
        p.drawPixmap(0, 0, texture);
    }
    draw_pixmap_texture(painter, rect, temp, 2, 2, 2, 2);
}

void GridTabColumnPrimitiveBase::paintEvent(QPainter& painter)
{
    GridShadeButtonPrimitive::paintEvent(painter);

    QStyleOptionHeader styleOption;
    column()->initStyleOption(styleOption);
    QFont f = column()->viewBase()->getColumnFont(column());
    styleOption.fontMetrics = QFontMetrics(f);
    painter.setFont(f);

    styleOption.selectedPosition = QStyleOptionHeader::NotAdjacent;
    styleOption.state &= ~QStyle::State_Sunken;
    styleOption.state &= ~QStyle::State_Raised;
    if (isLeftButtonPressed())
        styleOption.state |= QStyle::State_Sunken;
    else
        styleOption.state |= QStyle::State_Raised;

    if (isMouseOver())
    {
        styleOption.state |= QStyle::State_MouseOver;
    }
    else
        styleOption.state &= ~QStyle::State_MouseOver;


    styleOption.textAlignment = Qt::AlignRight | Qt::AlignVCenter;
    styleOption.rect = QRect(0, 0, width(), height());

    QPalette pal = getButtonPalette();
    int margin = control()->style()->pixelMetric(QStyle::PM_HeaderMargin);
    int leftIndent = extraLeftIndent(margin);
    int rightIndent = extraRightIndent(margin);

#ifdef Q_OS_MAC
    /* The implementation of the OS Mac native style has a bug - do not take into account the
    offset specified in the painter. All primitives are drawn in the position of 0,0.
    */
    QPixmap pm(styleOption.rect.size());
    pm.fill(Qt::transparent);
    {
        QPainter pixmapPainter(&pm);
        pixmapPainter.setFont(painter.font());
        pixmapPainter.setPen(pal.color(QPalette::Text));
        GridTableColumnPrimitive::paint_column_header_background(Q_NULL, &pixmapPainter, styleOption, column()->decorationColor());
        GridTableColumnPrimitive::paint_column_header_contents(Q_NULL, &pixmapPainter, styleOption,
            column()->textElideMode(), column()->iconApperance(), false, leftIndent, rightIndent, Qt::Horizontal, false, 1.0);
    }
    painter.drawPixmap(styleOption.rect, pm);
#else
    painter.setPen(pal.color(QPalette::Text));

    GridTableColumnPrimitive::paint_column_header_background(Q_NULL, &painter, styleOption, column()->decorationColor());
    GridTableColumnPrimitive::paint_column_header_contents(Q_NULL, &painter, styleOption,
        column()->textElideMode(), column()->iconApperance(), false, leftIndent, rightIndent,
        Qt::Horizontal, false, 1.0);
#endif
}

void GridTabColumnPrimitiveBase::doClick(QMouseEvent* e)
{
    column()->click(e);
}

bool GridTabColumnPrimitiveBase::checkDragDropEvent(const QPoint& pos, GridDragDropObject* object)
{
    return ((GridTabColumnPrimitive *)parentGP())->checkDragDropEvent(parentGP()->mapFromSite(mapToSite(pos)), object);
}

GridDragDropObject* GridTabColumnPrimitiveBase::createDragDropObject(const QPoint& point)
{
    Q_ASSERT(column() != Q_NULL);
    GridDragDropObject* retval = static_cast<GridTableColumnBase *>(column())->createDragDropColumnObject(point, this);
    return retval;
}

/* GridTabColumnPrimitive */
GridTabColumnPrimitive::GridTabColumnPrimitive(GraphicPrimitive* parent, GridColumnBase* columnBase)
    : GridTabColumnPrimitiveBase(parent, columnBase)
{
    m_closeButtonGP = new GridHeaderColumnTabCloseButtonGraphic(this);
}

GridHeaderColumnTabCloseButtonGraphic* GridTabColumnPrimitive::closeButtonGP() 
{ 
    return m_closeButtonGP;
}

void GridTabColumnPrimitive::updateLayout()
{
    QSize sh = m_closeButtonGP->sizeHint();
    QRect r = QStyle::alignedRect(control()->layoutDirection(), Qt::AlignVCenter | Qt::AlignRight, sh, boundedRect().adjusted(0, 0, -5, 0));
    m_closeButtonGP->setRect(r);
}

int GridTabColumnPrimitive::extraSizeHintIndent(int margin)
{
    return m_closeButtonGP->visibility() ? margin + m_closeButtonGP->sizeHint().width() : 0;
}

int GridTabColumnPrimitive::extraLeftIndent(int margin)
{
    Q_UNUSED(margin);
    return 0;
}

int GridTabColumnPrimitive::extraRightIndent(int margin)
{
    return m_closeButtonGP->visibility() ? margin + m_closeButtonGP->width() : 0;
}

/* GridCardColumnTabPrimitive */
GridCardColumnTabPrimitive::GridCardColumnTabPrimitive(GraphicPrimitive* parent, GridColumnBase* columnBase)
    : GridTabColumnPrimitiveBase(parent, columnBase), m_filterButtonGP(Q_NULL)
{
    m_checkBoxGP = new GridCheckBoxPrimitive(this, false);
    connect(m_checkBoxGP->widget(), SIGNAL(clicked(bool)), this, SLOT(checkBoxClicked(bool)));
}

void GridCardColumnTabPrimitive::createFilterButton()
{
    if (GridColumnPrimitive::checkFilterButtonVisible(column()))
    {
        if (m_filterButtonGP != Q_NULL)
            return;
        m_filterButtonGP = new GridTableColumnFilterButton(static_cast<GridDataTableColumn *>(column()), this);
    }
    else
    {
        Q_DELETE_AND_NULL(m_filterButtonGP);
    }
}
void GridCardColumnTabPrimitive::updateChecked()
{
    m_checkBoxGP->setChecked(column()->isVisible());
}

void GridCardColumnTabPrimitive::updateLayout()
{
    createFilterButton();
    QSize sh = m_checkBoxGP->sizeHint();
    QRect r = QStyle::alignedRect(control()->layoutDirection(), Qt::AlignVCenter | Qt::AlignRight, sh, boundedRect().adjusted(0, 0, -5, 0));
    m_checkBoxGP->setRect(r);

    if (m_filterButtonGP == Q_NULL)
        return;

    sh = m_filterButtonGP->sizeHint();
    r = QStyle::alignedRect(control()->layoutDirection(), Qt::AlignVCenter | Qt::AlignLeft, sh, boundedRect().adjusted(5, 0, 0, 0));
    m_filterButtonGP->setRect(r);
}

int GridCardColumnTabPrimitive::extraSizeHintIndent(int margin)
{
    int ret = m_checkBoxGP->visibility() ? margin + m_checkBoxGP->sizeHint().width() : 0;
    if (m_filterButtonGP != Q_NULL)
        ret += (margin + m_filterButtonGP->sizeHint().width());
    return ret;
}

int GridCardColumnTabPrimitive::extraLeftIndent(int margin)
{
    return m_filterButtonGP != Q_NULL ? margin + m_checkBoxGP->width() : 0;
}

int GridCardColumnTabPrimitive::extraRightIndent(int margin)
{
    return m_checkBoxGP->visibility() ? margin + m_checkBoxGP->width() : 0;
}

void GridCardColumnTabPrimitive::checkBoxClicked(bool checked)
{
    column()->setVisible(checked);
}

/* GridCardRowRemoveButtonPrimitive */
GridCardRowRemoveButtonPrimitive::GridCardRowRemoveButtonPrimitive(GraphicPrimitive* parent)
    : GridCloseButtonPrimitive(parent)
{
}

void GridCardRowRemoveButtonPrimitive::clicked()
{
    GridCardDataRowPrimitive* dataRowPrimitive = static_cast<GridCardDataRowPrimitive *>(parentGP());
    dataRowPrimitive->view()->removeRow(dataRowPrimitive->rowLayoutItem()->gridRow());
}

/* GridZoomIndicatorGraphic */
GridZoomIndicatorGraphic::GridZoomIndicatorGraphic(GraphicPrimitive* parent, GridViewBase* view)
    : GraphicPrimitive(parent->control(), parent), m_view(view), m_active(false)
{
    setZIndex(1000);
    ScrollableGraphicPrimitive::setFixedGPX(this, true);
    ScrollableGraphicPrimitive::setFixedGPY(this, true);
}

QFont GridZoomIndicatorGraphic::font() const
{
    QFont f = control()->font();
    f.setBold(true);
    int pixsize = 0;
    if (parentGP()->rect().height() > parentGP()->rect().width())
    {
        pixsize = parentGP()->rect().width() / 4;
    }
    else
        pixsize = parentGP()->rect().height() / 4;

    if (pixsize < 10)
        pixsize = 10;
    if (pixsize > 100)
        pixsize = 100;

    f.setPixelSize(pixsize);
    return f;
}

void GridZoomIndicatorGraphic::paintEvent(QPainter& painter)
{
    SmoothEffect* eff = effects()->findEffect(this);
    if (m_active && eff == Q_NULL)
    {
        eff = new SmoothEffect();
        eff->intialize(this);
        eff->setDuration(kGridFastScrollEffectDuration);
        effects()->run(eff);
    }

    QRect bounded = boundedRect();

    if (m_active)
    {
        QPixmap textPixmap(bounded.size());
        textPixmap.fill(Qt::transparent);
        {
            QPainter imgPainter;
            imgPainter.begin(&textPixmap);
            imgPainter.setFont(font());
            imgPainter.setPen(Qt::darkGray);
            imgPainter.drawText(bounded, QString("%1%").arg(m_view->optionsref()->zoomFactor() * 100));
        }
        eff->setInitialState(textPixmap.toImage());
        eff->setDuration(kGridFastScrollEffectDuration);
        m_active = false;
    }
    if (eff != Q_NULL)
    {
        eff->paintToInitial(&painter);
    }
    else
    {
        setVisibility(false);
    }
}

void GridZoomIndicatorGraphic::start()
{
    QFont f = font();
    QFontMetrics m(f);
    QRect r = m.boundingRect("000%");

    r.moveLeft(parentGP()->width() / 2 - r.width() / 2);
    if (r.left() < 0)
        r.setLeft(0);
    r.moveTop(parentGP()->height() / 2 - r.height() / 2);
    if (r.top() < 0)
        r.setTop(0);
    setRect(r);
    setOpacity(1.0);
    m_active = true;
    setVisibility(true);
    update();
}

/* GridTableHeaderGraphic */
GridTableHeaderGraphic::GridTableHeaderGraphic(GraphicPrimitive* parent, GridViewBase* view)
    : ScrollableGraphicPrimitive(parent), m_quickColumnButton(Q_NULL),
    m_leftFixedIndent(-1), m_rightOrBottomFixedIndent(-1), m_ignoreColumn(Q_NULL),
    m_view((GridTableView *)view)
{
    m_quickColumnButton = new GridTableColumnQuickButtonPrimitive(this, m_view->m_fieldProviders[0], false);
    m_quickColumnButton->setDropDownCount(m_view->options().columnsQuickCustomizationMaxDropDownCount());
    setFixedGPX(m_quickColumnButton, true);
    setFixedGPY(m_quickColumnButton, true);
    setOverflow(gsoNone);
}

GridTableHeaderGraphic::~GridTableHeaderGraphic()
{
    Q_DELETE_AND_NULL(m_quickColumnButton);
}

GridTableView* GridTableHeaderGraphic::view() const
{
    return m_view;
}

GridTableColumnPrimitive* GridTableHeaderGraphic::createColumnGP2(GridTableColumnBase* columnBase)
{
    GridTableColumnPrimitive* gp = new GridTableColumnPrimitive(this, columnBase);
    gp->setId(GridHitInfo::Column);
    gp->createEditorGraphic();
    gp->bestFit();
    return gp;
}

void GridTableHeaderGraphic::addColumn(GridTableColumnBase* columnBase)
{
    GridTableColumnPrimitive* gp = createColumnGP2(columnBase);
    m_columnGPsHash.insert(static_cast<GridTableColumn *>(columnBase), gp);
}

void GridTableHeaderGraphic::removeColumn(GridTableColumnBase* columnBase)
{
    QHash <const GridTableColumn *, GridTableColumnPrimitive *>::iterator it = m_columnGPsHash.find(static_cast<GridTableColumn *>(columnBase));
    if (it != m_columnGPsHash.end())
    {
        delete it.value();
        m_columnGPsHash.erase(it);
    }
    else
        Q_ASSERT(false);
}

GridTableColumnPrimitive* GridTableHeaderGraphic::getPrimitiveByColumn(const GridTableColumnBase* columnBase)
{
    QHash<const GridTableColumn *, GridTableColumnPrimitive *>::const_iterator it = m_columnGPsHash.constFind(static_cast<const GridTableColumn *>(columnBase));
    if (it == m_columnGPsHash.constEnd())
        return Q_NULL;
    else
        return it.value();
}

GridTableColumnQuickButtonPrimitive* GridTableHeaderGraphic::quickButton() const
{
    return m_quickColumnButton;
}

int GridTableHeaderGraphic::leftFixedIndent() const
{
    return m_leftFixedIndent;
}

int GridTableHeaderGraphic::rightFixedIndent() const
{
    return m_rightOrBottomFixedIndent;
}

int GridTableHeaderGraphic::getCellRowCount() const
{
    int ret = m_columnHeightTable.size();
    if (ret == 0)
        ret = 1;
    return ret;
}

void GridTableHeaderGraphic::updateLayout()
{
    m_sizeHint = updateColumnsLayout();
    setViewSize(m_sizeHint);
    m_rect = QRect(0, 0, 0, 0);
    setIgnoreColumn(Q_NULL);
    ScrollableGraphicPrimitive::updateLayout();
}

int GridTableHeaderGraphic::calculateColumnWidths(const GridViewBase::GridColumns& cols, int viewWidth, bool calculateFixed)
{
    if (cols.size() == 0)
        return 0;
    GridStretchWidthLayout stretchLayout;
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    bool ignoreAll = viewWidth == -1;
    for (GridViewBase::GridColumns::const_iterator it = cols.constBegin(); it != cols.constEnd(); ++it)
    {
        GridTableColumnPrimitive* columnGP = getPrimitiveByColumn(static_cast<GridTableColumnBase *>(*it));

        int columnSize = 0;
        int maxColumnSize = 0;
        int minColumnSize = 0;
        if (verticalLayout)
        {
            columnSize = columnGP->column()->exactWidth();
            maxColumnSize = columnGP->maxSize().width();
            minColumnSize = columnGP->minSize().width();
        }
        else
        {
            Q_ASSERT(false);
            columnSize = columnGP->column()->exactHeight();
            maxColumnSize = columnGP->maxSize().height();
            minColumnSize = columnGP->minSize().height();
        }

        stretchLayout.addColumnInfo(
            minColumnSize,
            maxColumnSize,
            columnSize,
            ignoreAll || isColumnIgnore(columnGP->column()));
    }
    stretchLayout.setViewWidth(viewWidth);
    stretchLayout.execute();
    int index = 0;
    int ret = 0;
    bool firstcolumn = true;
    for (GridViewBase::GridColumns::const_iterator it = cols.constBegin(); it != cols.constEnd(); ++it)
    {
        GridStretchWidthLayout::StretchWidthInfo& info = stretchLayout.getInfoByIndex(index);

        GridTableColumnPrimitive* columnGP = getPrimitiveByColumn(static_cast<GridTableColumnBase *>(*it));

        int oldColumnSize = verticalLayout ? columnGP->column()->exactWidth() : columnGP->column()->exactHeight();
        ret += info.newWidth;
        bool force = (m_view->m_contextStates & RequestUpdateLayout) || (m_view->m_contextStates & ColumnsChanged);
        if (oldColumnSize != info.newWidth || force)
        {
            if (verticalLayout)
                columnGP->column()->setExactWidth(info.newWidth);
            else
                columnGP->column()->setExactHeight(info.newWidth);
        }

        if (calculateFixed)
        {
            if (columnGP->column()->fixedKind() == FixedLeft)
            {
                if (firstcolumn)
                    m_leftFixedIndent += m_view->getRowGroupIndent(Q_NULL);
                firstcolumn = false;
                m_leftFixedIndent += columnGP->column()->exactWidth();
            }
            if (columnGP->column()->fixedKind() == FixedRight)
                m_rightOrBottomFixedIndent += columnGP->column()->exactWidth();
        }

        index++;
    }
    return ret;
}

void GridTableHeaderGraphic::setIgnoreColumn(GridTableColumnBase* column)
{
    m_ignoreColumn = column;
}

bool GridTableHeaderGraphic::isColumnIgnore(GridTableColumnBase* column) const
{
    if (m_ignoreColumn == Q_NULL)
        return false;
    if (m_ignoreColumn == column)
        return true;
    return m_ignoreColumn->visualIndex() > column->visualIndex();
}

void GridTableHeaderGraphic::bestFit()
{
    for (QHash<const GridTableColumn *, GridTableColumnPrimitive *>::const_iterator it = m_columnGPsHash.constBegin(); it != m_columnGPsHash.constEnd(); ++it)
        it.value()->bestFit();
}

int GridTableHeaderGraphic::calculateGridSummaryRowLength(GridRow* row)
{
    bool layoutWithBorder = row != Q_NULL && m_view->options().groupSummaryPlace() == GroupSummaryRow;
    int borderRight = 0, borderBottom = 0;
    bool _2d = false;
    if (row != Q_NULL)
        get_cell_border_lines(m_view, borderRight, borderBottom, _2d);

    if (m_view->layout()->orientation() == Qt::Vertical)
    {
        int defCellHeight = get_cell_height(m_view->grid()->style(), m_view->optionsref());
        if (layoutWithBorder)
            return (borderBottom + defCellHeight) * getCellRowCount();
        else
            return (kGridSummaryCellBorder + defCellHeight) * getCellRowCount() + kGridSummaryCellBorder + borderBottom;
    }
    else
    {
        int rowWidth = calculateRowWidthTableItem();
        if (layoutWithBorder)
            return rowWidth + (m_columnWidthTable.size() * borderRight);
        else
            return kGridSummaryCellBorder + rowWidth + (m_columnWidthTable.size() * kGridSummaryCellBorder) + borderRight;
    }
}

#if 0
int GridTableHeaderGraphic::calculateGridRowLength(GridRow* row, bool groupSummary)
{
    int ret = 0;
    if (m_view->layout()->orientation() == Qt::Vertical)
        ret = calculateGridRowHeightEx(row);
    else
        ret = calculateGridRowWidthEx(row);

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
#endif

int GridTableHeaderGraphic::calculateNewRowLength()
{
    if (m_view->layout()->orientation() == Qt::Vertical)
        return calculateNewRowHeightEx();
    else
        return calculateNewRowWidthEx();
}

int GridTableHeaderGraphic::calculateRowWidthTableItem()
{
    int width = m_view->optionsref()->cellWidth();
    if (width > 0)
        width = qRound(width * m_view->optionsref()->zoomFactor());
    else
    {
        width = 0;
        for (QMap<int, int>::const_iterator it = m_columnWidthTable.constBegin(); it != m_columnWidthTable.constEnd(); ++it)
            width += it.value();
    }
    return width;
}

int GridTableHeaderGraphic::calculateGridRowWidthEx(GridRow* row)
{
    int gridLinesWidth, gridLinesHeight;
    bool _2d;
    get_cell_border_lines(m_view, gridLinesWidth, gridLinesHeight, _2d);

    if (row == Q_NULL || row->type() == GridRow::DataRow)
    {
        int rowWidth = calculateRowWidthTableItem() + m_columnWidthTable.size() * gridLinesWidth;
        if (row != Q_NULL && m_view->options().isPreviewRowEnabled())
        {
            int previewWidth = kGridPreviewExpandButtonHeight;
            if (!m_view->options().isPreviewExpandButtonVisible() || row->isPreviewExpanded())
                previewWidth = m_view->options().previewRowHeight();

            rowWidth += qRound(previewWidth * m_view->options().zoomFactor()) + gridLinesWidth;
        }

        if (_2d)
            rowWidth += kGridDoubleLinesIndent + gridLinesWidth;
        return rowWidth;
    }

    Q_ASSERT(row->type() == GridRow::GroupRow);
    bool layoutWithBorder = m_view->options().groupSummaryPlace() == GroupSummaryRow;
    int rowWidth = layoutWithBorder ? 0 : get_group_height(control()->style(), m_view->optionsref()); //Will be rotated. 
    if (!layoutWithBorder && m_view->options().groupRowStyle() == GridGroupOfficeOutlook)
        rowWidth += kGridOffice11GroupRowHeihgtIncrement;

    int min = m_quickColumnButton->quickRowButtonSizeHint(row).width();

    if (m_view->options().groupSummaryPlace() == GroupSummaryRow || m_view->options().groupSummaryPlace() == GroupSummaryRowPlus)
    {
        rowWidth = rowWidth + calculateGridSummaryRowLength(row);
        if (m_view->options().groupSummaryPlace() == GroupSummaryRowPlus)
            rowWidth += gridLinesWidth;
    }
    return qMax(min, rowWidth + gridLinesWidth);
}

int GridTableHeaderGraphic::calculateGridRowHeightEx(GridRow* row)
{
    int gridLinesWidth, gridLinesHeight;
    bool _2d;
    get_cell_border_lines(m_view, gridLinesWidth, gridLinesHeight, _2d);

    if (row == Q_NULL || row->type() == GridRow::DataRow)
    {
        int rowHeight = 0;
        int autoCount = m_view->layout()->autoHeightRowCount();
        if (row != Q_NULL && autoCount > 0)
        {
            rowHeight = m_view->layout()->getGridRowAutoHeight(row);
            rowHeight = rowHeight + autoCount * gridLinesHeight;
        }
        else
        {
            rowHeight = get_cell_height(m_view->grid()->style(), m_view->optionsref());
            rowHeight = getCellRowCount() * (rowHeight + gridLinesHeight);
        }

        if (row != Q_NULL && m_view->options().isPreviewRowEnabled())
        {
            int previewHeight = kGridPreviewExpandButtonHeight;
            if (!m_view->options().isPreviewExpandButtonVisible() || row->isPreviewExpanded())
                previewHeight = m_view->options().previewRowHeight();
 
            rowHeight += qRound(previewHeight * m_view->options().zoomFactor()) + gridLinesHeight;
        }

        if (_2d)
            rowHeight += 1/*px*/ + gridLinesHeight;
        return rowHeight;
    }

    Q_ASSERT(row->type() == GridRow::GroupRow);
    bool layoutWithBorder = m_view->options().groupSummaryPlace() == GroupSummaryRow;

    int defCellHeight = layoutWithBorder ? 0 : get_group_height(control()->style(), m_view->optionsref());

    if (!layoutWithBorder && m_view->options().groupRowStyle() == GridGroupOfficeOutlook)
        defCellHeight += kGridOffice11GroupRowHeihgtIncrement;

    if (m_view->options().groupSummaryPlace() == GroupSummaryRow || m_view->options().groupSummaryPlace() == GroupSummaryRowPlus)
    {
        defCellHeight += calculateGridSummaryRowLength(row);
        if (m_view->options().groupSummaryPlace() == GroupSummaryRowPlus)
            defCellHeight += gridLinesWidth;
        return defCellHeight;
    }

    return defCellHeight + gridLinesHeight;
}

int GridTableHeaderGraphic::calculateNewRowWidthEx()
{
    int gridLinesWidth, gridLinesHeight;
    bool _2d;
    get_cell_border_lines(m_view, gridLinesWidth, gridLinesHeight, _2d);

    int rowWidth = calculateRowWidthTableItem() + m_columnWidthTable.size() * gridLinesWidth;

    return rowWidth;// quickButton()->width();
}

int GridTableHeaderGraphic::calculateNewRowHeightEx()
{
    int ret = 0;
    int borderRight, borderBottom;
    bool _2d;
    get_cell_border_lines(m_view, borderRight, borderBottom, _2d);

    for (QMap<int, int>::const_iterator it = m_columnHeightTable.constBegin(); it != m_columnHeightTable.constEnd(); ++it)
        ret += it.value() + borderBottom;
    if (_2d)
        ret += 1/*px*/ + borderBottom;
    return ret;
}

void GridTableHeaderGraphic::createTable()
{
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    m_visibleColumns.clear();
    GridTableColumn* ignoreColumn = qobject_cast<GridTableColumn *>(m_ignoreColumn);

    m_columnWidthTable.clear();
    m_columnHeightTable.clear();
    m_leftFixedIndent = 0;
    m_rightOrBottomFixedIndent = 0;
    int visualRowIndex = 0;
    int visualColumnIndex = 0;
    bool firstcolumn = true;

    for (GridViewBase::GridColumns::const_iterator it = m_view->m_columnVisualIndexies.constBegin();
        it != m_view->m_columnVisualIndexies.constEnd(); ++it)
    {
        GridTableColumnPrimitive* columnGP = getPrimitiveByColumn(static_cast<GridTableColumnBase *>(*it));

        if (!columnGP->visibility())
            continue;
        m_visibleColumns.push_back(columnGP->column());

        QSize s = columnGP->sizeHint();

        if (ignoreColumn != Q_NULL)
        {
            s.rwidth() = ignoreColumn->exactWidth();
        }
        else
            s.rwidth() = columnGP->column()->exactWidth();

        if (!m_columnWidthTable.contains(visualColumnIndex) || s.width() > m_columnWidthTable[visualColumnIndex])
            m_columnWidthTable[visualColumnIndex] = s.width();

        if (!m_columnHeightTable.contains(visualRowIndex) || s.height() > m_columnHeightTable[visualRowIndex])
            m_columnHeightTable[visualRowIndex] = s.height();

        if (verticalLayout)
        {
            visualRowIndex = 0;
        }
        else
        {
            if (columnGP->column()->fixedKind() == FixedLeft)
            {
                if (firstcolumn)
                    m_leftFixedIndent += m_view->getRowGroupIndent(Q_NULL);
                firstcolumn = false;
                m_leftFixedIndent += m_columnHeightTable[visualRowIndex];
            }
            if (columnGP->column()->fixedKind() == FixedRight)
            {
                m_rightOrBottomFixedIndent += m_columnHeightTable[visualRowIndex];
            }
            visualRowIndex++;
        }
    }
}

QSize GridTableHeaderGraphic::sizeHint()
{
    return m_sizeHint;
}

void GridTableHeaderGraphic::exactSizeChanged(GridTableColumnBase* column)
{
    Q_UNUSED(column);
}

bool GridTableHeaderGraphic::checkColumnsScrollBar(int& rangeMin, int& rangeMax, int& page, int& singleStep)
{
    bool ret = false;
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    if (verticalLayout)
    {
        m_leftFixedIndent = 0;
        m_rightOrBottomFixedIndent = 0;

        int width = m_view->layout()->centralAreaSize().width() - quickButton()->width();
        int groupingIndent = m_view->getRowGroupIndent(Q_NULL);
        int newWidth = calculateColumnWidths(m_visibleColumns, m_view->options().columnAutoWidth() ? (width - groupingIndent) : -1, true) + groupingIndent;
        width = qMax(0, width - (m_leftFixedIndent + m_rightOrBottomFixedIndent));
        newWidth = qMax(0, newWidth - (m_leftFixedIndent + m_rightOrBottomFixedIndent));
        ret = newWidth > width;
            //m_view->layout()->scrollHor()->setVisible(true);
        rangeMin = 0;
        rangeMax = qMax(0, newWidth - width);
        page = width;
        singleStep = width; /* pixels */

        //m_view->layout()->scrollHor()->setRange(0, qMax(0, newWidth - width));
        //m_view->layout()->scrollHor()->setPageStep(width);

    }
    else
    {
        int height = m_view->layout()->centralAreaSize().height() - quickButton()->sizeHint().height();
        int newHeihgt = m_view->getRowGroupIndent(Q_NULL);
        for (QMap<int, int>::const_iterator it = m_columnHeightTable.constBegin(); it != m_columnHeightTable.constEnd(); ++it)
            newHeihgt += it.value();
        height = qMax(0, height - (m_leftFixedIndent + m_rightOrBottomFixedIndent));
        newHeihgt = qMax(0, newHeihgt - (m_leftFixedIndent + m_rightOrBottomFixedIndent));
        ret = newHeihgt > height;
            //m_view->layout()->scrollVer()->setVisible(true);
        rangeMin = 0;
        rangeMax = qMax(0, newHeihgt - height);
        page = height;
        singleStep = 1; /* pixels */
        //m_view->layout()->scrollVer()->setRange(0, qMax(0, newHeihgt - height));
        //m_view->layout()->scrollVer()->setPageStep(height);
    }
    return ret;
}

void GridTableHeaderGraphic::updateQuickButtonLayout()
{
    QSize quickColumnButtonSize = m_quickColumnButton->sizeHint();
    m_quickColumnButton->setWidth(quickColumnButtonSize.width());
    m_quickColumnButton->setHeight(quickColumnButtonSize.height());
}

int GridTableHeaderGraphic::scrollX() const
{
    if (m_view->layout()->orientation() == Qt::Vertical)
        return m_view->layout()->scrollX();
    else
        return 0;
}

int GridTableHeaderGraphic::scrollY() const
{
    if (m_view->layout()->orientation() != Qt::Vertical)
        return m_view->layout()->scrollY();
    else
        return 0;
}

void GridTableHeaderGraphic::prepareCellsLayout()
{
    int gridLinesWidth, gridLinesHeight;
    bool _2d;
    get_cell_border_lines(m_view, gridLinesWidth, gridLinesHeight, _2d);

    int gindent = m_view->getRowGroupIndent(Q_NULL);
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    int columnXOffset = verticalLayout ? quickButton()->width() : quickButton()->left();
    int columnYOffset = verticalLayout ? quickButton()->top() : quickButton()->height();

    //TODO don't use banded view
    GridBandedTableView* bandedView = qobject_cast<GridBandedTableView *>(m_view);
    bool ignoreRowSpanForCells = bandedView != Q_NULL && bandedView->options().ignoreRowSpanForCells();
    //TODO

    int defCellLength = verticalLayout ? get_cell_height(m_view->grid()->style(), m_view->optionsref()) : get_cell_width(m_view->grid()->style(), m_view->optionsref());

    //if (cellHeight == 0 && ignoreRowSpanForCells)
    //    cellHeight = m_view->getColumnHeight(0);

    for (QHash <const GridTableColumn *, GridTableColumnPrimitive *>::const_iterator it = m_columnGPsHash.constBegin(); it != m_columnGPsHash.constEnd(); ++it)
    {
        GridTableColumnPrimitive* columnGP = it.value();
        if (!columnGP->visibility())
            continue;

        const GridTableColumn* column = it.key();

        int columnRowSpan = 1;
        if (!ignoreRowSpanForCells)
            columnRowSpan = column->rowSpan();

        QRect columnRect = columnGP->rect();
        columnRect.translate(-columnXOffset, -columnYOffset);
        if (verticalLayout)
        {
            if (m_view->hasGroupingIndent(column))
                columnRect.adjust(gindent, 0, 0, 0);
        }
        else
        {
            if (m_view->hasGroupingIndent(column))
                columnRect.adjust(0, gindent, 0, 0);
        }

        if (verticalLayout)
        {
            columnRect.setHeight((defCellLength + gridLinesHeight) * columnRowSpan);
            int y = columnGP->layouterRow() * (defCellLength + gridLinesHeight);
            columnRect.moveTop(y);
        }
        else
        {
            if (defCellLength != -1)
            {
                columnRect.setWidth(defCellLength + gridLinesWidth);
                int x = columnGP->layouterRow() * (defCellLength + gridLinesWidth);
                columnRect.moveLeft(x);
            }
            else
            {
                columnRect.setWidth(columnRect.width() + gridLinesWidth);
                int x = columnRect.left() + columnGP->layouterRow() * gridLinesWidth;
                columnRect.moveLeft(x);
            }
        }
        columnGP->setCellLayoutRect(columnRect);
    }
}


int GridTableHeaderGraphic::calculateMaxColumnWidthForView(GridTableColumnPrimitive* columnGP, int viewWidth)
{
    if (viewWidth < 0)
        return columnGP->maxSize().width();

    int retval = viewWidth;
    GridViewBase::GridColumns cols;
    view()->getColumnLine(columnGP->column(), cols);
    int min = columnGP->minSize().width();
    for (GridViewBase::GridColumns::const_iterator it = cols.constBegin(); it != cols.constEnd(); ++it)
    {
        GridTableColumnPrimitive* tableColumnGP = getPrimitiveByColumn(static_cast<GridTableColumnBase *>(*it));

        if (tableColumnGP->column()->visualIndex() < columnGP->column()->visualIndex())
            retval -= tableColumnGP->column()->exactWidth();
        if (tableColumnGP->column()->visualIndex() > columnGP->column()->visualIndex())
            retval -= tableColumnGP->minSize().width();
        if (retval < min)
        {
            retval = min;
            break;
        }
    }

    int max = columnGP->maxSize().width();
    if (retval > max)
        retval = max;
    return retval;
}

int GridTableHeaderGraphic::checkColumnWidth(GridTableColumnPrimitive* columnGP, int width)
{
    int retval = width;
    int min = columnGP->minSize().width();
    int max = 0;

    if (view()->layout()->orientation() == Qt::Vertical)
        max = calculateMaxColumnWidthForView(columnGP, view()->getColumnViewWidth(columnGP->column()));
    else
        max = columnGP->maxSize().width();

    if (retval < min)
        retval = min;
    if (retval > max)
        retval = max;
    return retval;

}

QSize GridTableHeaderGraphic::updateColumnsLayout()
{
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    int leftOffset = verticalLayout ? m_quickColumnButton->width() : 0;
    int topOffset = verticalLayout ? 0 : m_quickColumnButton->height();
    QSize columnHeaderSize(0, 0);

    int xpos = leftOffset;
    int ypos = topOffset;
    int frameWidth = xpos;
    int frameHeight = ypos;

    int rangeColumnScrollPixels = verticalLayout ? m_view->layout()->scrollHor()->maximum() : m_view->layout()->scrollVer()->maximum();
    int visualRowIndex = 0;
    bool fistFixedColumn = true;

    for (GridViewBase::GridColumns::const_iterator it = m_visibleColumns.constBegin();
        it != m_visibleColumns.constEnd(); ++it)
    {
        GridTableColumnPrimitive* columnGP = getPrimitiveByColumn(static_cast<GridTableColumnBase *>(*it));

        int columnGroupingIndent = m_visibleColumns.constBegin() == it && m_view->hasGroupingIndent(columnGP->column()) ? m_view->getRowGroupIndent(Q_NULL) : 0;


        if (verticalLayout)
        {
            if (columnGP->column()->fixedKind() == FixedRight && fistFixedColumn)
            {
                fistFixedColumn = false;
                xpos -= rangeColumnScrollPixels;
                frameWidth -= rangeColumnScrollPixels;
                columnHeaderSize.rwidth() -= rangeColumnScrollPixels;
            }

            int columnHeight = m_columnHeightTable[visualRowIndex];
            columnGP->setWidth(columnGroupingIndent + columnGP->column()->exactWidth());
            columnGP->setHeight(columnHeight);
            frameHeight = qMax(columnHeight, frameHeight);
        }
        else
        {
            if (columnGP->column()->fixedKind() == FixedRight && fistFixedColumn)
            {
                fistFixedColumn = false;
                ypos -= rangeColumnScrollPixels;
                frameHeight -= rangeColumnScrollPixels;
                columnHeaderSize.rheight() -= rangeColumnScrollPixels;
            }

            int columnWidth = m_columnWidthTable[0];
            int columnHeight = m_columnHeightTable[visualRowIndex];
            columnGP->column()->setExactWidth(columnWidth);
            columnGP->setWidth(columnGP->column()->exactWidth());
            columnGP->setHeight(columnGroupingIndent + columnHeight);
            frameWidth = qMax(columnWidth, frameWidth);
        }

        columnGP->updateFixed();
        columnGP->setLeft(xpos);
        columnGP->setTop(ypos);
        columnGP->setLayouterRow(0);

        if (verticalLayout)
        {
            xpos += columnGP->width();
            frameWidth += columnGP->width();
        }
        else
        {
            ypos += columnGP->height();
            frameHeight += columnGP->height();
            visualRowIndex++;
        }
    }

    if (verticalLayout)
    {
        columnHeaderSize.rwidth() = frameWidth;
        columnHeaderSize.rheight() = qMax(m_quickColumnButton->height(), frameHeight);
        m_quickColumnButton->setRect(QRect(0, 0, m_quickColumnButton->width(), columnHeaderSize.height()));
    }
    else
    {
        columnHeaderSize.rwidth() = qMax(m_quickColumnButton->width(), frameWidth);
        columnHeaderSize.rheight() = frameHeight;
        m_quickColumnButton->setRect(QRect(0, 0, columnHeaderSize.width(), m_quickColumnButton->height()));
    }

    if (!m_view->options().columnsHeader())
    {
        if (verticalLayout)
            columnHeaderSize.setHeight(0);
        else
            columnHeaderSize.setWidth(0);
    }

    return columnHeaderSize;
}

/* GridNewRowPrimitive */
GridNewRowPrimitive::GridNewRowPrimitive(GridViewBase* view, GraphicPrimitive* parent)
    : GraphicPrimitive(parent), m_view(view)
{
    ScrollableGraphicPrimitive::setFixedGPX(this, true);
    ScrollableGraphicPrimitive::setFixedGPY(this, true);
}

void GridNewRowPrimitive::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
    if (control()->dragDropState() != dragActive &&
        e->button() == Qt::LeftButton &&
        !isLeftButtonDblClicked() && isMouseOver())
    {
        m_view->showNewRowEditor();
    }
    update();
}

QFont GridNewRowPrimitive::getTitleFont()
{
    QFont f = control()->font();
    f.setBold(true);
    return qtn_font_apply_scale(f, m_view->optionsref()->zoomFactor());
}

QRect GridNewRowPrimitive::getLayoutItemRect()
{
    return rect();
}

void GridNewRowPrimitive::paintEvent(QPainter& painter)
{
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;

    QStyleOptionHeader styleOption;
    styleOption.state = QStyle::State_Enabled | QStyle::State_Active;
    styleOption.orientation = Qt::Horizontal;
    QRect bounded = boundedRect();
    painter.fillRect(bounded, Qt::white);

    styleOption.rect = bounded;
    if (verticalLayout)
    {
        styleOption.rect.setWidth(m_view->layout()->getQuickButtonIndent());
    }
    else
    {
        styleOption.rect.setHeight(m_view->layout()->getQuickButtonIndent());
    }
    styleOption.selectedPosition = QStyleOptionHeader::NotAdjacent;
    if (isMouseOver())
    {
        //styleOption.state |= QStyle::State_MouseOver;
    }

    QRect r = styleOption.rect;
    if (m_view->layout()->getQuickButtonIndent() != 0)
    {
        control()->style()->drawControl(QStyle::CE_HeaderSection, &styleOption, &painter, control());
        double factor = m_view->optionsref()->zoomFactor();
        QSize iconSize = QSize(qRound(factor * QtnGridNewRowIconSize.width()), qRound(factor * QtnGridNewRowIconSize.height()));
        QPixmap icon = qtn_get_thememanager(control())->getIcon(GridThemeManager::NewRowIcon, iconSize);
        r = QStyle::alignedRect(control()->layoutDirection(), Qt::AlignVCenter | Qt::AlignHCenter, icon.size(), r);
        painter.drawPixmap(r, icon);
    }
    else
    {
    }

    painter.setFont(getTitleFont());
    painter.setPen(qtn_get_thememanager(control())->shadePalette().foreground().color());
    QTextOption opt;
    opt.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    r = bounded;

    if (verticalLayout)
    {
        r.setLeft(m_view->layout()->getQuickButtonIndent());
    }
    else
    {
        r.setTop(m_view->layout()->getQuickButtonIndent());
        painter.rotate(-90);
        r = QRect(-r.top() - r.height(), r.left(), r.height(), r.width());
    }
    painter.drawText(r, m_view->optionsref()->newRowText(), opt);
}

/* GridRowSplitterPrimitive */
GridRowSplitterPrimitive::GridRowSplitterPrimitive(GridViewBase* view, GraphicPrimitive* parent)
    : GraphicPrimitive(parent), m_view(view)
{
    ScrollableGraphicPrimitive::setFixedGPX(this, true);
    ScrollableGraphicPrimitive::setFixedGPY(this, true);
}

void GridRowSplitterPrimitive::paintEvent(QPainter& painter)
{
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    QRect bounded = boundedRect();

    QRect r1 = bounded;
    if (verticalLayout)
        r1.setHeight(20);
    else
        r1.setWidth(20);

    QPixmap pixmap(r1.size());
    pixmap.fill(Qt::transparent);

    {
        QPainter pixmapPainter;
        pixmapPainter.begin(&pixmap);

        QPalette palette = control()->palette();
        QStyleOptionHeader styleOption;
        styleOption.state = QStyle::State_Enabled | QStyle::State_Active | QStyle::State_MouseOver;// | QStyle::State_Sunken;
        styleOption.orientation = Qt::Horizontal;
        styleOption.selectedPosition = QStyleOptionHeader::NotAdjacent;
        styleOption.rect = r1;
        styleOption.palette = palette;

        control()->style()->drawControl(QStyle::CE_HeaderSection, &styleOption, &pixmapPainter, control());
        pixmapPainter.end();
    }

    painter.drawPixmap(bounded, pixmap);

    if (verticalLayout)
    {
        /*
        QRect r2 = bounded;
        r2.setHeight(r2.height() - 2);
        painter.drawPixmap(r2, pixmap, r2);
        r2.setHeight(bounded.height());

        r1.setTop(r1.top() + r1.height() - 2);
        r2.setTop(r2.top() + r2.height() - 2);
        painter.drawPixmap(r2, pixmap, r1);
        */
    }
}


/* GridTableQuickRowFrozenButtonGraphic */
GridTableQuickRowFrozenButtonGraphic::GridTableQuickRowFrozenButtonGraphic(GraphicPrimitive* parentGraphic, GridRowLayoutItem* rowLayoutItem)
    : GraphicPrimitive(parentGraphic), m_rowLayoutItem(rowLayoutItem)
{
}

void GridTableQuickRowFrozenButtonGraphic::paintEvent(QPainter& painter)
{
    QRect r = boundedRect();
    if (isMouseOver() && isLeftButtonPressed())
    {
        painter.fillRect(r, Qt::lightGray);
    }

    QPixmap icon;
    double factor = m_rowLayoutItem->viewBase()->optionsref()->zoomFactor();
    QSize iconSize = QSize(qRound(factor * QtnGridSmallToolButtonSize.width()), qRound(factor * QtnGridSmallToolButtonSize.height()));

    if (m_rowLayoutItem->type() == GridTableRowLayoutItem::FrozenData)
        icon = qtn_get_thememanager(control())->getIcon(GridThemeManager::FrozenPinOnIcon, iconSize);
    else
    {
        icon = qtn_get_thememanager(control())->getIcon(GridThemeManager::FrozenPinOffIcon, iconSize);
        if (!m_rowLayoutItem->quickRowButtonGP()->isMouseOverRecursive())
            return;
    }
    QRect iconRect = QStyle::alignedRect(control()->layoutDirection(), Qt::AlignVCenter | Qt::AlignHCenter, iconSize, r);
    painter.drawPixmap(iconRect, icon);

    if (isMouseOver())
    {
        painter.setPen(QColor(220, 220, 220));
        painter.drawRect(r.adjusted(0, 0, -1, -1));
    }
}

void GridTableQuickRowFrozenButtonGraphic::mouseOverEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
    parentGP()->update();
}

void GridTableQuickRowFrozenButtonGraphic::mouseOutEvent()
{
    parentGP()->update();
}

void GridTableQuickRowFrozenButtonGraphic::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e); 
    update();
}

void GridTableQuickRowFrozenButtonGraphic::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
    if (isLeftButtonPressed() && isMouseOver())
    {
        if (m_rowLayoutItem->type() == GridTableRowLayoutItem::Data)
            m_rowLayoutItem->gridRow()->setFrozen(true);
        else if (m_rowLayoutItem->type() == GridTableRowLayoutItem::FrozenData)
            m_rowLayoutItem->gridRow()->setFrozen(false);
    }
    parentGP()->update();
}

/* GridTableCellRow */
/* GridTableQuickRowButtonGraphic */
GridTableQuickRowButtonGraphic::GridTableQuickRowButtonGraphic(
    GraphicPrimitive* parentGraphic, GridRowLayoutItem* rowLayoutItem)
    : GraphicPrimitive(parentGraphic->control(), parentGraphic), m_rowLayoutItem(rowLayoutItem)
{
    m_frozenButtonGP = new GridTableQuickRowFrozenButtonGraphic(this, rowLayoutItem);
    m_quickSelectGP = new GridTableQuickSelectButtonPrimitive(this, m_rowLayoutItem->viewBase(), m_rowLayoutItem);
    setVisibility(false);
}

void GridTableQuickRowButtonGraphic::paintEvent(QPainter& painter)
{
    QStyleOptionHeader styleOption;
    styleOption.state = QStyle::State_Enabled | QStyle::State_Active;
    styleOption.orientation = Qt::Horizontal;
    QRect bounded = boundedRect();
    styleOption.rect = bounded;
    styleOption.selectedPosition = QStyleOptionHeader::NotAdjacent;
    if (isMouseOverRecursive() || m_frozenButtonGP->isMouseOver() || m_quickSelectGP->isMouseOver())
    {
        styleOption.state |= QStyle::State_MouseOver;
    }
    control()->style()->drawControl(QStyle::CE_HeaderSection, &styleOption, &painter, control());

    GridTableView* table = qobject_cast<GridTableView *>(m_rowLayoutItem->viewBase()); //TODO remove GridTableView from here. 
    if (table != Q_NULL && !table->options().columnsQuickCustomization())
        return;

    double scaleFactor = m_rowLayoutItem->viewBase()->optionsref()->zoomFactor();
    bool isnewrow = m_rowLayoutItem->type() == GridTableRowLayoutItem::NewData;
    if ((!m_rowLayoutItem->viewBase()->isNewRowEditing() && m_rowLayoutItem->isRowFocused()) || isnewrow)
    {
        QPixmap icon;

        if (!m_rowLayoutItem->viewBase()->isEditing())
            icon = qtn_get_thememanager(control())->getIcon(GridThemeManager::FocusRowIndicatorIcon, QtnGridNewRowIconSize * scaleFactor);
        else
        {
            if (m_rowLayoutItem->viewBase()->isModified())
            {
                if (m_rowLayoutItem->viewBase()->activeEditor()->isValidated())
                    icon = qtn_get_thememanager(control())->getIcon(GridThemeManager::FocusRowEditValidIcon, QtnGridNewRowIconSize * scaleFactor);
                else
                    icon = qtn_get_thememanager(control())->getIcon(GridThemeManager::FocusRowEditModifyIcon, QtnGridNewRowIconSize * scaleFactor);
            }
            else
                icon = qtn_get_thememanager(control())->getIcon(GridThemeManager::FocusRowEditIcon, QtnGridNewRowIconSize * scaleFactor);
        }

        if (m_rowLayoutItem->viewBase()->layout()->orientation() == Qt::Horizontal)
        {
            QMatrix matrix;
            matrix.rotate(90);
            icon = icon.transformed(matrix);
        }
        int margin = qRound(control()->style()->pixelMetric(QStyle::PM_HeaderMargin) * scaleFactor);

        bounded.adjust(margin, 0, 0, 0);
        if (m_frozenButtonGP->visibility())
            bounded.adjust(m_frozenButtonGP->width() + margin, 0, 0, 0);
        if (m_quickSelectGP->visibility())
            bounded.adjust(m_quickSelectGP->width() + margin, 0, 0, 0);
   
        QRect r = QStyle::alignedRect(control()->layoutDirection(), Qt::AlignVCenter | Qt::AlignHCenter, icon.size(), bounded);
        painter.drawPixmap(r, icon);
    }
}

void GridTableQuickRowButtonGraphic::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    if (m_rowLayoutItem == Q_NULL || m_rowLayoutItem->gridRow() == Q_NULL)
        return;

    if (checkClickEvent(e))
    {
        m_rowLayoutItem->viewBase()->setFocusedRowIndex(m_rowLayoutItem->gridRow()->rowIndex(), e->modifiers(), NoPolicy);
    }
}

void GridTableQuickRowButtonGraphic::mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(e);
    Q_UNUSED(point);
}

void GridTableQuickRowButtonGraphic::mouseOverEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
    update();
}

void GridTableQuickRowButtonGraphic::mouseOutEvent()
{
    update();
}

void GridTableQuickRowButtonGraphic::updateLayout()
{
    QRect bounded = boundedRect();

    GridTableView* table = qobject_cast<GridTableView *>(m_rowLayoutItem->viewBase()); //TODO remove GridTableView from here. 

    m_quickSelectGP->setVisibility(table != Q_NULL && table->options().rowsQuickSelection());
    m_frozenButtonGP->setVisibility(table != Q_NULL && table->options().isRowFrozenButtonVisible() &&
        (m_rowLayoutItem->type() == GridTableRowLayoutItem::FrozenData || m_rowLayoutItem->type() == GridTableRowLayoutItem::Data));

    bool verticalLayout = true;// m_row->view()->layout()->orientation() == Qt::Vertical;
    double factor = m_rowLayoutItem->viewBase()->optionsref()->zoomFactor();
    int margin = qRound(control()->style()->pixelMetric(QStyle::PM_HeaderMargin) * factor);

    if (m_frozenButtonGP->visibility())
    {
        int buttonSize = qRound((QtnGridSmallToolButtonSize.width() + 8)  * factor);
        QRect r;
        if (verticalLayout)
        {
            bounded.adjust(margin, 0, 0, 0);
            r = bounded;
            r.setWidth(buttonSize);
            bounded.adjust(buttonSize, 0, 0, 0);
        }
        else
        {
            bounded.adjust(0, margin, 0, 0);
            r = bounded;
            r.setHeight(buttonSize);
            bounded.adjust(0, buttonSize, 0, 0);
        }
        r = QStyle::alignedRect(control()->layoutDirection(), Qt::AlignVCenter | Qt::AlignHCenter, QSize(buttonSize, buttonSize), r);

        m_frozenButtonGP->setRect(r);
        m_frozenButtonGP->setZIndex(zIndex() + 1);
    }

    if (m_quickSelectGP->visibility())
    {
        QRect r;
        QSize buttonSize = m_quickSelectGP->sizeHint();
        if (factor < 1)
            buttonSize = buttonSize * factor;
        if (verticalLayout)
        {
            bounded.adjust(margin, 0, 0, 0);
            r = bounded;
            r.setWidth(buttonSize.width());
        }
        else
        {
            bounded.adjust(0, margin, 0, 0);
            r = bounded;
            r.setHeight(buttonSize.height());
        }
        r = QStyle::alignedRect(control()->layoutDirection(), Qt::AlignVCenter | Qt::AlignHCenter, buttonSize, r);

        m_quickSelectGP->setRect(r);
        m_quickSelectGP->setZIndex(zIndex() + 1);
    }
}

/* GridRowPrimitiveAbstract */
GridRowPrimitiveAbstract::GridRowPrimitiveAbstract(GraphicPrimitive* parent, GridRowLayoutItem* rowLayoutItem)
    : ScrollableGraphicPrimitive(parent), m_rowLayoutItem(rowLayoutItem)
{
    setVisibility(false);
}

GridRowPrimitiveAbstract::~GridRowPrimitiveAbstract()
{
}

void Qtitan::qtn_do_update_fixed(bool verticalLayout, bool fixed, GraphicPrimitive* primitive)
{
    if (verticalLayout)
    {
        ScrollableGraphicPrimitive::setFixedGPX(primitive, fixed);
        ScrollableGraphicPrimitive::setFixedGPY(primitive, true);
    }
    else
    {
        ScrollableGraphicPrimitive::setFixedGPX(primitive, true);
        ScrollableGraphicPrimitive::setFixedGPY(primitive, fixed);
    }

    if (!fixed)
        primitive->parentGP()->mask().addGP(primitive);
    else
        primitive->parentGP()->mask().removeGP(primitive);
}

void GridRowPrimitiveAbstract::updateBorder()
{
    int right, bottom;
    bool _2d;
    get_cell_border_lines(view(), right, bottom, _2d);

    if (view()->optionsref()->groupRowStyle() == GridGroupOfficeOutlook)
    {
        if (view()->layout()->orientation() == Qt::Vertical)
        {
            bottom += kGridOffice11GroupRowBorderIncrement;
        }
        else
        {
            right += kGridOffice11GroupRowBorderIncrement;
        }
    }

    setBorderRight(right);
    setBorderBottom(bottom);
    setBorderPen(view()->optionsref()->gridLinePen());
}

void GridRowPrimitiveAbstract::mouseOverEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
    update();
}

void GridRowPrimitiveAbstract::mouseOutEvent()
{
    update();
}

void GridRowPrimitiveAbstract::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    if (checkClickEvent(e))
    {
        rowLayoutItem()->viewBase()->setFocusedRowIndex(rowLayoutItem()->gridRow()->rowIndex(), e->modifiers(), NoPolicy);
    }
}

void GridRowPrimitiveAbstract::paintSelectedRect(QPainter& painter, const QPalette& palette, const QRect& rect, bool mouseOverFlag)
{
    if (rowLayoutItem()->viewBase()->optionsref()->showFocusDecoration())
    {
        painter.fillRect(rect, Qt::white);
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
        QStyleOptionViewItemV4 option;
#else
        QStyleOptionViewItem option;
#endif
        option.initFrom(control());
        option.rect = rect;
        option.state = QStyle::State_Active | QStyle::State_Selected | QStyle::State_Enabled | QStyle::State_HasFocus;
        if (!mouseOverFlag)
            option.state |= QStyle::State_MouseOver;
        option.palette = palette;
        option.showDecorationSelected = true;
        static QListView fake;
        control()->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, &painter, &fake);
    }
    else
    {
        painter.fillRect(rect, selected_bgBrush());
        QPixmap texture(":res/grid_glass_column_bg.png");
        draw_pixmap_texture(painter, rect, texture, 2, 2, 2, 2);
    }
}

QRect GridRowPrimitiveAbstract::paintContentRect(bool withBorder) const
{
    QRect bounded = boundedRect();
    bool verticalLayout = view()->layout()->orientation() == Qt::Vertical;


    if (verticalLayout)
    {
        bounded.setLeft(bounded.left() + rowLayoutItem()->indentRect().right() + 1);
    }
    else
    {
        bounded.setTop(bounded.top() + rowLayoutItem()->indentRect().bottom() + 1);
    }

    if (view()->layout()->leftFixedIndent() > 0)
    {
        bounded.setWidth(bounded.width() - scrollX());
        bounded.setHeight(bounded.height() - scrollY());
    }
    if (view()->layout()->rightFixedIndent() > 0)
    {
        bounded.setWidth(bounded.width() + scrollX());
        bounded.setHeight(bounded.height() + scrollY());
    }

    if (withBorder)
    {
        if (verticalLayout)
        {
            
            if (view()->optionsref()->gridLines() == LinesBoth2D || view()->optionsref()->gridLines() == LinesHorizontal2D)
            {
                int offset = borderBottom();
                if (offset > 0)
                    offset += kGridDoubleLinesIndent; //For CardView the border is 0
                bounded.adjust(0, 0, 0, -offset);
            }
        }
        else
        {
            if (view()->optionsref()->gridLines() == LinesBoth2D || view()->optionsref()->gridLines() == LinesVertical2D)
            {
                int offset = borderRight();
                if (offset > 0)
                    offset += kGridDoubleLinesIndent; //For CardView the border is 0
                bounded.adjust(0, 0, -offset, 0);
            }
        }
    }

    return bounded;
}

void GridRowPrimitiveAbstract::paintSrollableBorder(QPainter& painter)
{
    bool verticalLayout = view()->layout()->orientation() == Qt::Vertical;
    GridViewOptions* viewOptions = view()->optionsref();

    //Draw group row border
    const QPen& pen = borderPen();
    int line_width = width();
    int line_height = height();
    if (rowLayoutItem()->viewBase()->layout()->leftFixedIndent() > 0)
    {
        line_width -= scrollX();
        line_height -= scrollY();
    }
    if (rowLayoutItem()->viewBase()->layout()->rightFixedIndent() > 0)
    {
        line_width += scrollX();
        line_height += scrollY();
    }

    const QColor& penbordercolor = qtn_get_thememanager(control())->shadeColor();

    if (borderRight() > 0)
    {
        int penWidth = borderRight();
        //pen.setWidth(penWidth);
        int indent = 0;

        if (!verticalLayout)
            indent = rowLayoutItem()->nextRowLayoutItem() ? rowLayoutItem()->nextRowLayoutItem()->indentRect().height() : 0;

        if (indent > 0)
            indent -= borderBottom();

        if (viewOptions->groupRowStyle() == GridGroupOfficeOutlook)
        {
            QPen p = pen;
            p.setColor(penbordercolor);
            painter.setPen(p);
            indent = 0;
        }
        else
            painter.setPen(pen);

        painter.drawLine(QLineF(line_width - 1 - penWidth / 2,
            indent + penWidth / 2,
            line_width - 1 - penWidth / 2,
            line_height - 1 - penWidth / 2));
    }


    if (borderBottom() > 0)
    {
        int penWidth = borderBottom();
        //pen.setWidth(penWidth);
        int indent = 0;
        if (verticalLayout)
            indent = rowLayoutItem()->nextRowLayoutItem() ? rowLayoutItem()->nextRowLayoutItem()->indentRect().width() : 0;
        
        if (indent > 0)
            indent -= borderRight();
 
        if (viewOptions->groupRowStyle() == GridGroupOfficeOutlook)
        {
            QPen p = pen;
            p.setColor(penbordercolor);
            painter.setPen(p);
            indent = 0;
        }
        else
            painter.setPen(pen);

        painter.drawLine(QLineF(indent + penWidth / 2,
            line_height - 1 - penWidth / 2,
            line_width - 1 - penWidth / 2,
            line_height - 1 - penWidth / 2));
    }
}

void GridRowPrimitiveAbstract::drawBorder(QPainter& painter)
{
    Q_UNUSED(painter);
}

void GridRowPrimitiveAbstract::paintGroupIndent(QPainter& painter)
{
    GridViewOptions* opt = rowLayoutItem()->viewBase()->optionsref();
    QPen pen = opt->gridLinePen();
    pen.setWidth(opt->gridLineWidth());
    painter.setPen(pen);
    int lineRight, lineBottom;
    bool _2d;
    get_cell_border_lines(view(), lineRight, lineBottom, _2d);

    bool verticalLayout = rowLayoutItem()->viewBase()->layout()->orientation() == Qt::Vertical;
    QRect r = verticalLayout ? QRect(borderLeft(), 0, width() - (borderLeft() + borderRight()), height()) : QRect(0, borderTop(), width(), height() - (borderTop() + borderBottom()));
    QLineF line;

    if (verticalLayout)
    {
        line = QLineF(r.left() - (opt->gridLineWidth() + 1) / 2,
            r.top() + opt->gridLineWidth() / 2,
            r.left() - (opt->gridLineWidth() + 1) / 2,
            r.top() + r.height() - (opt->gridLineWidth() + 1) / 2);
        r.setWidth(kGridGroupRowIndent);
    }
    else
    {
        line = QLineF(r.left() + opt->gridLineWidth() / 2,
            r.top() - (opt->gridLineWidth() + 1) / 2,
            r.left() + r.width() - (opt->gridLineWidth() + 1) / 2,
            r.top() - (opt->gridLineWidth() + 1) / 2);
        r.setHeight(kGridGroupRowIndent);
    }

    int count = 0;
    if (rowLayoutItem()->type() != GridTableRowLayoutItem::NewData && rowLayoutItem()->type() != GridTableRowLayoutItem::FrozenData)
        count = rowLayoutItem()->gridRow()->depth() + 1;

    QRect nextIndent = rowLayoutItem()->nextRowLayoutItem() ? rowLayoutItem()->nextRowLayoutItem()->indentRect() : QRect(0, 0, 0, 0);

    for (int i = 0; i < count; ++i)
    {
        QPalette palette = opt->groupPalette(i);
        bool lastindent = (i == count - 1);

        QRect fillRect = verticalLayout ? r.adjusted(0, 0, -lineRight, 0) : r.adjusted(0, 0, 0, -lineBottom);

        if (verticalLayout)
        {
            if (nextIndent.width() == 0)
                fillRect.adjust(0, 0, 0, -borderBottom());
        }
        else
        {
            if (nextIndent.height() == 0)
                fillRect.adjust(0, 0, -borderRight(), 0);
        }

        if (opt->groupRowStyle() == GridGroupOfficeOutlook)
        {
            if (!lastindent)
                painter.fillRect(fillRect, palette.color(QPalette::Dark));
            else
                break;
        }
        else
            painter.fillRect(fillRect, palette.color(QPalette::Dark));

        if (rowLayoutItem()->gridRow()->type() == GridRow::GroupRow  && lastindent)
            break;

        if (verticalLayout)
        {
            r.translate(kGridGroupRowIndent, 0);
            nextIndent.setWidth(qMax(0, nextIndent.width() - kGridGroupRowIndent));
        }
        else
        {
            r.translate(0, kGridGroupRowIndent);
            nextIndent.setHeight(qMax(0, nextIndent.height() - kGridGroupRowIndent));
        }


        if (verticalLayout)
        {
            if (opt->gridLines() == LinesBoth || opt->gridLines() == LinesVertical || opt->gridLines() == LinesVertical2D || opt->gridLines() == LinesBoth2D)
            {
                line.translate(kGridGroupRowIndent, 0);
                painter.drawLine(line);
            }
        }
        else
        {
            if (opt->gridLines() == LinesBoth || opt->gridLines() == LinesHorizontal || opt->gridLines() == LinesHorizontal2D || opt->gridLines() == LinesBoth2D)
            {
                line.translate(0, kGridGroupRowIndent);
                painter.drawLine(line);
            }
        }
    }
}

void GridRowPrimitiveAbstract::paintEvent(QPainter& painter)
{
    paintSrollableBorder(painter);
    if (rowLayoutItem()->isFirstItem())
        paintGroupIndent(painter);
}

/* GridSummaryRowPrimitive */
GridSummaryRowPrimitive::GridSummaryRowPrimitive(GraphicPrimitive* parent, GridRowLayoutItem* rowLayoutItem)
    : GridRowPrimitiveAbstract(parent, rowLayoutItem)
{
    setOverflow(gsoNone);
}

GridSummaryRowPrimitive::~GridSummaryRowPrimitive()
{
}

void GridSummaryRowPrimitive::paintEvent(QPainter& painter)
{
    GridRowPrimitiveAbstract::paintEvent(painter);
    QPalette p = initPalette();

    QRect contentRect = paintContentRect(false);
    rowLayoutItem()->summarylayoutEngine()->paintBackgroud(painter, p, contentRect);
}

QPalette GridSummaryRowPrimitive::initPalette()
{
    const GridViewOptions* viewOptions = view()->optionsref();
    QPalette palette = viewOptions->cellPalette();
    palette.setCurrentColorGroup(control()->hasFocus() ? QPalette::Active : QPalette::Inactive);

    m_selected_bgbrush = palette.brush(QPalette::Highlight);
    m_selected_fgbrush = palette.brush(QPalette::HighlightedText);

    if (viewOptions->isTransparentBackground())
        m_bgbrush = Qt::transparent;
    else
        m_bgbrush = palette.brush(QPalette::Base);

    m_fgbrush = palette.brush(QPalette::Text);

    return palette;
}

/* GridTableSummaryRowPrimitive */
GridTableSummaryRowPrimitive::GridTableSummaryRowPrimitive(GraphicPrimitive* parent, GridTableRowLayoutItem* row)
    : GridSummaryRowPrimitive(parent, row)
{
}

int GridTableSummaryRowPrimitive::scrollX() const
{
    if (view()->layout()->orientation() == Qt::Vertical)
        return view()->layout()->scrollX();
    else
        return 0;
}

int GridTableSummaryRowPrimitive::scrollY() const
{
    if (view()->layout()->orientation() != Qt::Vertical)
        return view()->layout()->scrollY();
    else
        return 0;
}

void GridTableSummaryRowPrimitive::paintEvent(QPainter& painter)
{
    if (rowLayoutItem()->viewBase()->layout()->leftFixedIndent() == 0)
        painter.translate(-scrollX(), -scrollY());
    GridSummaryRowPrimitive::paintEvent(painter);
}

/* GridCardSummaryRowPrimitive */
GridCardSummaryRowPrimitive::GridCardSummaryRowPrimitive(GraphicPrimitive* parent, GridCardRowLayoutItem* row)
    : GridSummaryRowPrimitive(parent, row)
{
}

void GridCardSummaryRowPrimitive::updateBorder()
{
    GridSummaryRowPrimitive::updateBorder();
}

void GridCardSummaryRowPrimitive::paintEvent(QPainter& painter)
{
    GridSummaryRowPrimitive::paintEvent(painter);
}

/* GridDataRowPrimitive */
GridDataRowPrimitive::GridDataRowPrimitive(GraphicPrimitive* parent, GridRowLayoutItem* rowLayoutItem)
    : GridRowPrimitiveAbstract(parent, rowLayoutItem)
{
    setOverflow(gsoNone);
    setId(GridHitInfo::Row);
    m_previewGP = new GridTablePreviewRowPrimitive(rowLayoutItem->viewBase(), this, rowLayoutItem);
}

GridDataRowPrimitive::~GridDataRowPrimitive()
{
    destroyCells();
    Q_DELETE_AND_NULL(m_previewGP);
}

void GridDataRowPrimitive::paintEvent(QPainter& painter)
{
    QPalette palette = initPalette();
    GridRowPrimitiveAbstract::paintEvent(painter);
    bool rowSelectionPolicy = (view()->optionsref()->selectionPolicy() == SelectSingleRow ||
        view()->optionsref()->selectionPolicy() == SelectMultiRow);

    QRect contentRect = paintContentRect(rowSelectionPolicy);

    paintBackground(painter, contentRect, palette, rowSelectionPolicy && rowLayoutItem()->isRowSelected());
}

void GridDataRowPrimitive::paintBackground(QPainter& painter, const QRect& rect, const QPalette& palette, bool selected)
{
    if (selected)
        paintSelectedRect(painter, palette, rect, isMouseOverRecursive());
    else
        painter.fillRect(rect, bgBrush());
}

QPalette GridDataRowPrimitive::initPalette()
{
    const GridViewOptions* viewOptions = view()->optionsref();
    QPalette palette = viewOptions->cellPalette();
    palette.setCurrentColorGroup(control()->hasFocus() ? QPalette::Active : QPalette::Inactive);

    m_selected_bgbrush = palette.brush(QPalette::Highlight);
    m_selected_fgbrush = palette.brush(QPalette::HighlightedText);

    if (view()->layout()->alternatingRowColors(rowLayoutItem()->gridRow()))
        m_bgbrush = palette.brush(QPalette::AlternateBase);
    else
    {
        if (viewOptions->isTransparentBackground())
            m_bgbrush = Qt::transparent;
        else
            m_bgbrush = palette.brush(QPalette::Base);
    }
    m_fgbrush = palette.brush(QPalette::Text);

    return palette;
}

bool GridDataRowPrimitive::checkDragDropEvent(const QPoint& pos, GridDragDropObject* object)
{
    Q_UNUSED(pos);
    GridCellDragObject* obj = qobject_cast<GridCellDragObject *>(object);
    if (obj != Q_NULL)
        return obj->checkDragDropEvent(NextRowPosition, false, this);
    return false;
}

void GridDataRowPrimitive::updateBorder()
{
    int right, bottom;
    bool _2d;
    get_cell_border_lines(view(), right, bottom, _2d);
    setBorderRight(right);
    setBorderBottom(bottom);
    setBorderPen(view()->optionsref()->gridLinePen());
}

const QHash<const GridColumnPrimitive *, GridTableCellGraphic *>& GridDataRowPrimitive::dataCells() const
{
    return m_dataCells;
}

GridTablePreviewRowPrimitive* GridDataRowPrimitive::previewGP() const
{
    return m_previewGP;
}

void GridDataRowPrimitive::registrColumnGraphic(const GridColumnPrimitive* columnPrimitive)
{
    Q_ASSERT(m_dataCells.find(columnPrimitive) == m_dataCells.end());
    
    GridDataTableColumn* column = static_cast<GridDataTableColumn *>(columnPrimitive->column());

    GridTableCellGraphic* dataCell = new GridTableCellGraphic(rowLayoutItem()->viewBase(), this, rowLayoutItem(), column);
    
    m_dataCells.insert(columnPrimitive, dataCell);

    for (QList<GridCellButtonInfo *>::const_iterator it = column->m_buttons.constBegin(); it != column->m_buttons.constEnd(); ++it)
        dataCell->createButton(*it);
}

void GridDataRowPrimitive::unregistrColumnGraphic(const GridColumnPrimitive* columnPrimitive)
{
    QHash<const GridColumnPrimitive *, GridTableCellGraphic *>::iterator data_it = m_dataCells.find(columnPrimitive);
    if (data_it == m_dataCells.end())
        return;
    data_it.value()->removeButtons();
    delete data_it.value();
    m_dataCells.erase(data_it);
}

void GridDataRowPrimitive::editorChanging(const GridColumnPrimitive* columnPrimitive, bool reset)
{
    QHash<const GridColumnPrimitive *, GridTableCellGraphic *>::iterator data_it = m_dataCells.find(columnPrimitive);
    if (data_it != m_dataCells.end())
    {
        if (reset)
            data_it.value()->resetEditor();
        else
        {
            GridEditor* editor = data_it.value()->editor();
            if (editor != Q_NULL)
                editor->repositoryUpdated();
        }
    }
}

void GridDataRowPrimitive::updateCellsData()
{
    for (QHash<const GridColumnPrimitive *, GridTableCellGraphic *>::iterator it = m_dataCells.begin();
        it != m_dataCells.end(); ++it)
    {
        it.value()->setDirtyData(true);
    }
    m_previewGP->setDirtyData(true);
}

void GridDataRowPrimitive::destroyCells()
{
    for (QHash<const GridColumnPrimitive *, GridTableCellGraphic *>::iterator it = m_dataCells.begin();
        it != m_dataCells.end(); ++it)
    {
        delete it.value();
    }
}

/* GridTableDataRowPrimitive */
GridTableDataRowPrimitive::GridTableDataRowPrimitive(GraphicPrimitive* parent, GridTableRowLayoutItem* row)
    : GridDataRowPrimitive(parent, row)
{
}

int GridTableDataRowPrimitive::scrollX() const
{
    if (view()->layout()->orientation() == Qt::Vertical)
        return view()->layout()->scrollX();
    else
        return 0;
}

int GridTableDataRowPrimitive::scrollY() const
{
    if (view()->layout()->orientation() != Qt::Vertical)
        return view()->layout()->scrollY();
    else
        return 0;
}

void GridTableDataRowPrimitive::paintEvent(QPainter& painter)
{
    if (rowLayoutItem()->viewBase()->layout()->leftFixedIndent() == 0)
        painter.translate(-scrollX(), -scrollY());

    GridDataRowPrimitive::paintEvent(painter);
}

/* GridCardDataRowPrimitive */
GridCardDataRowPrimitive::GridCardDataRowPrimitive(GraphicPrimitive* parent, GridCardRowLayoutItem* row)
    : GridDataRowPrimitive(parent, row)
{
    m_removeButtonPrimitive = new GridCardRowRemoveButtonPrimitive(this);
}

GridCardDataRowPrimitive::~GridCardDataRowPrimitive()
{
    Q_DELETE_AND_NULL(m_removeButtonPrimitive);
}

void GridCardDataRowPrimitive::updateBorder()
{
    //do nothing.
    //GridDataRowPrimitive::updateBorder();
}

QRect GridCardDataRowPrimitive::paintContentRect(bool withBorder) const
{
    bool verticalLayout = view()->layout()->orientation() == Qt::Vertical;

    QRect paintRect = GridDataRowPrimitive::paintContentRect(withBorder);
    const int margin = static_cast<GridCardView *>(view())->options().itemMargin();
    if (verticalLayout)
        paintRect.adjust(static_cast<GridCardRowLayoutItem *>(rowLayoutItem())->alignOffset() + margin, margin, -margin, -margin);
    else
        paintRect.adjust(margin, static_cast<GridCardRowLayoutItem *>(rowLayoutItem())->alignOffset() + margin, -margin, -margin);

    paintRect.adjust(0, 0, 0, -static_cast<GridCardRowLayoutItem *>(rowLayoutItem())->autoRowOffset());

    return paintRect;
}

QPalette GridCardDataRowPrimitive::initPalette()
{
    QPalette palette = GridDataRowPrimitive::initPalette();
    m_bgbrush = palette.brush(QPalette::AlternateBase);
    return palette;
}

GridCardView* GridCardDataRowPrimitive::view() const
{
    return static_cast<GridCardView *>(GridRowPrimitiveAbstract::view());
}

void GridCardDataRowPrimitive::paintEvent(QPainter& painter)
{
    GridDataRowPrimitive::paintEvent(painter);
   
    QRect contentRect = paintContentRect(false);
    const int padding = view()->options().itemPadding();

    {
        QPen p;
        p.setWidth(1);
        p.setColor(Qt::black);
        painter.setPen(p);

        QRect titleRect = contentRect.adjusted(padding, padding, -padding, -padding);
        titleRect.setHeight(view()->options().itemTitleHeight() * view()->options().zoomFactor());
        
        QPixmap texture(":res/grid_glass_column_bg.png");
        QPixmap temp(texture.width(), texture.height());
        temp.fill(qtn_get_thememanager(control())->shadePalette().light().color());
        temp.setMask(texture.createHeuristicMask());
        {
            QPainter p(&temp);
            p.drawPixmap(0, 0, texture);
        }
        draw_pixmap_texture(painter, titleRect, temp, 2, 2, 2, 2);
        
        painter.drawText(titleRect.adjusted(3, 0, -3, 0), 
            QString("%1 %2").arg(view()->options().itemTitleText()).arg(rowLayoutItem()->gridRow()->rowIndex()), Qt::AlignVCenter | Qt::AlignLeft);
    }
}

void GridCardDataRowPrimitive::paintBackground(QPainter& painter, const QRect& rect, const QPalette& palette, bool selected)
{
    const int margin = view()->options().itemMargin();
    QRect bgrect = rect.adjusted(-margin, -margin, margin, margin);
    QPixmap background;
    if (!static_cast<GridCardViewPrimitive *>(view()->layout())->getItemBackgroundPixmap(bgrect, bgBrush(), background))
    {
        background = QPixmap(bgrect.size());
        background.fill(Qt::transparent);
        {
            QPainter pixmapPainter(&background);
            pixmapPainter.translate(margin, margin);
            pixmapPainter.fillRect(QRect(0, 0, rect.width(), rect.height()), bgBrush());
        }


        {
            QGraphicsDropShadowEffect* e = new QGraphicsDropShadowEffect;
            e->setColor(QColor(40, 40, 40, 245));
            e->setOffset(0, 3);
            e->setBlurRadius(15);
            background = apply_effect_to_pixmap(background, e, 1);
        }
        static_cast<GridCardViewPrimitive *>(view()->layout())->addItemBackgroundPixmap(bgrect, bgBrush(), background);
    }

    if (selected)
    {
        QPainter pixmapPainter(&background);
        pixmapPainter.translate(margin, margin);
        paintSelectedRect(pixmapPainter, palette, QRect(0, 0, rect.width(), rect.height()), isMouseOverRecursive());
    }

    painter.drawPixmap(bgrect, background);
}

/* GridTableGroupBranchButton */
GridTableGroupBranchButton::GridTableGroupBranchButton(GridGroupRowPrimitive* parent,
    GridRowLayoutItem* rowLayoutItem)
    : GraphicPrimitive(parent->control(), parent), m_rowLayoutItem(rowLayoutItem)
{
    setId(GridHitInfo::GroupBranchButton);
}

void GridTableGroupBranchButton::paintEvent(QPainter& painter)
{
    if (m_rowLayoutItem->gridRow() == Q_NULL)
        return;

    if (m_rowLayoutItem->viewBase()->optionsref()->groupRowStyle() == GridGroupOfficeOutlook)
    {
        QString res = m_rowLayoutItem->gridRow()->isExpanded() ? ":res/gridoffice11minus.png" : ":res/gridoffice11plus.png";
        QPixmap button(res);
        QSize buttonSize = QSize(11, 11);
        QRect rect = QRect(QPoint(width() / 2 - buttonSize.width() / 2,
            kGridOffice11GroupRowHeihgtIncrement - 2 + (height() - kGridOffice11GroupRowHeihgtIncrement) / 2 - buttonSize.height() / 2),
            buttonSize);

        painter.drawPixmap(rect, button);
    }
    else
    {
        QStyleOptionViewItem styleOption;
        styleOption.initFrom(control());
        styleOption.palette.setBrush(QPalette::Dark, Qt::white);
        styleOption.palette.setCurrentColorGroup(QPalette::Active);
        QSize buttonSize = QSize(9, 9);
        styleOption.rect = QRect(QPoint(width() / 2 - buttonSize.width() / 2,
            height() / 2 - buttonSize.height() / 2), buttonSize);

        if (m_rowLayoutItem->gridRow()->isExpanded())
            styleOption.state = QStyle::State_Children | QStyle::State_Open;
        else
            styleOption.state = QStyle::State_Children;

        if (isMouseOver())
            styleOption.state |= QStyle::State_MouseOver;
        else
            styleOption.state &= ~QStyle::State_MouseOver;

#ifdef Q_OS_MAC
        painter.setClipping(false);
        if (painter.device()->devType() == QInternal::Widget)
            control()->style()->drawPrimitive(QStyle::PE_IndicatorBranch, &styleOption, &painter, 0);
#else
        control()->style()->drawPrimitive(QStyle::PE_IndicatorBranch, &styleOption, &painter, 0);
#endif
    }
}

void GridTableGroupBranchButton::mouseOverEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
    update();
}

void GridTableGroupBranchButton::mouseOutEvent()
{
    update();
}

void GridTableGroupBranchButton::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    if (checkClickEvent(e))
    {
        if (m_rowLayoutItem->gridRow() != Q_NULL)
            m_rowLayoutItem->gridRow()->setExpanded(!m_rowLayoutItem->gridRow()->isExpanded());
    }
}



/*
GridTableGroupRowButton::GridTableGroupRowButton(GridGroupRowPrimitive* parent)
    : GraphicPrimitive(parent)
{
    setId(GridHitInfo::GroupRow);
}

GridTableGroupRowButton::~GridTableGroupRowButton()
{
}

GridGroupRowPrimitive* GridTableGroupRowButton::groupRowPrimitive() const
{
    return static_cast<GridGroupRowPrimitive *>(parentGP());
}

void GridTableGroupRowButton::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    static_cast<GridTableGroupRowButton *>(parentGP())->mousePressEvent(point, e);
}

void GridTableGroupRowButton::mouseOverEvent(const QPoint& pos)
{
    GraphicPrimitive::mouseOverEvent(pos);
    if (groupRowPrimitive()->view()->options().showFocusDecoration())
        update();
}

void GridTableGroupRowButton::mouseOutEvent()
{
    GraphicPrimitive::mouseOutEvent();
    if (groupRowPrimitive()->view()->options().showFocusDecoration())
        update();
}

void GridTableGroupRowButton::keyPressEvent(QKeyEvent* e)
{
    GraphicPrimitive::keyPressEvent(e);
}

void GridTableGroupRowButton::paintEvent(QPainter& painter)
{
    painter.setPen(Qt::red);
    painter.drawRect(boundedRect().adjusted(0, 0, -1, -1));
    return;
}

void GridTableGroupRowButton::resizeEvent(const QRect& rect)
{
    //QSize buttonSize = m_tableRow->view()->options().groupRowStyle() == GridGroupOffice11 ? QSize(11, 11) : QSize(9, 9);
    bool verticalLayout = groupRowPrimitive()->view()->layout()->orientation() == Qt::Vertical;
    QRect r;
    if (verticalLayout)
        r = QStyle::alignedRect(control()->layoutDirection(), Qt::AlignVCenter | Qt::AlignHCenter, QSize(kGridGroupRowIndent, kGridGroupRowIndent),
        QRect(0, 0, kGridGroupRowIndent, height()));
    else
        r = QStyle::alignedRect(control()->layoutDirection(), Qt::AlignVCenter | Qt::AlignHCenter, QSize(kGridGroupRowIndent, kGridGroupRowIndent),
        QRect(0, 0, width(), kGridGroupRowIndent));

    m_branchButon->setRect(r);

    GraphicPrimitive::resizeEvent(rect);
}
*/

/* GridGroupRowPrimitive */
GridGroupRowPrimitive::GridGroupRowPrimitive(GraphicPrimitive* parent, GridRowLayoutItem* rowLayoutItem)
    : GridRowPrimitiveAbstract(parent, rowLayoutItem)
{
    m_branchButton = new GridTableGroupBranchButton(this, rowLayoutItem);
}

GridGroupRowPrimitive::~GridGroupRowPrimitive()
{
    Q_DELETE_AND_NULL(m_branchButton);
}

GridTableGroupBranchButton* GridGroupRowPrimitive::branchButon() const
{
    return m_branchButton;
}

QPalette GridGroupRowPrimitive::initPalette()
{
    const GridViewOptions* viewOptions = view()->optionsref();
    QPalette palette = viewOptions->groupPalette(rowLayoutItem()->gridRow()->depth());
    palette.setCurrentColorGroup(control()->hasFocus() ? QPalette::Active : QPalette::Inactive);

    m_selected_bgbrush = palette.brush(QPalette::Highlight);
    m_selected_fgbrush = palette.brush(QPalette::HighlightedText);

    if (viewOptions->groupRowStyle() == GridGroupOfficeOutlook)
    {
        m_bgbrush = Qt::white;
        m_fgbrush = qtn_get_thememanager(control())->shadeColor().darker(150);
    }
    else
    {
        m_bgbrush = palette.brush(QPalette::Dark);
        m_fgbrush = palette.brush(QPalette::BrightText);
    }
    return palette;
}

static QString grid_group_row_plain_text(GridRowLayoutItem* rowLayoutItem)
{
    QString retval;

    if (rowLayoutItem->gridRow() == Q_NULL)
        return retval;

    GridColumnBase* column = rowLayoutItem->viewBase()->getColumnByModelColumn(rowLayoutItem->gridRow()->groupColumn());
    Q_ASSERT(column != Q_NULL);
    if (column != Q_NULL)
    {
        QVariant v;
        rowLayoutItem->gridRow()->groupValue(v);
        retval = QString(" %1: %2").arg(column->caption()).arg(v.toString());
    }
    return retval;
}

void GridGroupRowPrimitive::paintEvent(QPainter& painter)
{
    GridRowPrimitiveAbstract::paintEvent(painter);
    QPalette palette = initPalette();

    QRect contentRect = paintContentRect(false);

    bool selected = rowLayoutItem()->isRowSelected();
    if (selected)
        paintSelectedRect(painter, palette, contentRect, isMouseOverRecursive());
    else
        painter.fillRect(contentRect, bgBrush());

    bool verticalLayout = view()->layout()->orientation() == Qt::Vertical;
    GridViewOptions* viewOptions = view()->optionsref();
 
    if (rowLayoutItem()->summarylayoutEngine()->needPaintGroupText()) //For GroupSummaryRow text being printed in a first summary-cell.
    {
        painter.setPen(selected ? selected_fgBrush().color() : fgBrush().color());
        QFont font = qtn_font_apply_scale(get_group_font(viewOptions), viewOptions->zoomFactor());
        painter.setFont(font);
        QString groupRowText = grid_group_row_plain_text(rowLayoutItem());

        int margin = qRound(control()->style()->pixelMetric(QStyle::PM_HeaderMargin) * viewOptions->zoomFactor());
        QRect r = contentRect.adjusted(margin, margin, -margin, -margin);

        if (verticalLayout)
        {
            r.setLeft(m_branchButton->left() + m_branchButton->width());
            groupRowText = painter.fontMetrics().elidedText(groupRowText, Qt::ElideRight, r.width());

            if (viewOptions->groupRowStyle() == GridGroupOfficeOutlook)
                r.setTop(r.top() + kGridOffice11GroupRowHeihgtIncrement);
            draw_text(painter, groupRowText, r, Qt::AlignTop | Qt::AlignLeft);
        }
        else
        {
            r.setTop(m_branchButton->top() + m_branchButton->height());
            groupRowText = painter.fontMetrics().elidedText(groupRowText, Qt::ElideRight, r.height());

            if (viewOptions->groupRowStyle() == GridGroupOfficeOutlook)
                r.setLeft(r.left() + kGridOffice11GroupRowHeihgtIncrement);

            painter.rotate(-90);
            r = QRect(-r.top() - r.height(), r.left(), r.height(), r.width());
            draw_text(painter, groupRowText, r, Qt::AlignTop | Qt::AlignRight);
            painter.rotate(90);
        }
    }

    if (viewOptions->focusFrameEnabled() && rowLayoutItem()->isRowFocused() && control()->hasFocus())
        draw_focus_frame(painter, selected ? selected_fgBrush().color() : fgBrush().color(), contentRect);
}

void GridGroupRowPrimitive::mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    if (control()->dragDropState() != dragActive &&
        e->button() == Qt::LeftButton)
    {
        if (rowLayoutItem()->gridRow() != Q_NULL)
            rowLayoutItem()->gridRow()->setExpanded(!rowLayoutItem()->gridRow()->isExpanded());
    }
}

void GridGroupRowPrimitive::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    GraphicPrimitive::mouseReleaseEvent(point, e);
    if (isLeftButtonDblClicked() && isMouseOverRecursive())
    {
        RowClickEventArgs rowArgs(rowLayoutItem()->gridRow());
        view()->rowDblClicked(&rowArgs);
    }
    else if (isLeftButtonPressed() && isMouseOverRecursive())
    {
        RowClickEventArgs rowArgs(rowLayoutItem()->gridRow());
        view()->rowClicked(&rowArgs);
    }
}

void GridGroupRowPrimitive::mouseMoveEvent(const QPoint& point, QMouseEvent* e)
{
    GraphicPrimitive::mouseMoveEvent(point, e);
    if (view()->optionsref()->isFocusFollowsMouse() &&
        view()->focusedRow() != rowLayoutItem()->gridRow())
    {
        if (!view()->isNewRowEditing())
            view()->setFocusedRowIndex(rowLayoutItem()->gridRow()->rowIndex());
    }
}

GridDragDropObject* GridGroupRowPrimitive::createDragDropObject(const QPoint& pos)
{
#if 0
    return (m_tableRow->view()->options().isMultiSelect() || (m_tableRow->view()->options().dragEnabled() && m_tableRow->isRowSelected()));
#endif

    if (view()->isEditing() || view()->isNewRowEditing())
        return Q_NULL;
    if (view()->optionsref()->isMultiSelect())
        return new GridTableViewSelectionObject(static_cast<GridTableView *>(view()), GridCell(rowLayoutItem()->gridRow(), -1), pos);
    return Q_NULL;
}

/* GridTableGroupRowPrimitive */
GridTableGroupRowPrimitive::GridTableGroupRowPrimitive(GraphicPrimitive* parent, GridTableRowLayoutItem* row)
    : GridGroupRowPrimitive(parent, row)
{
}

int GridTableGroupRowPrimitive::scrollX() const
{
    if (view()->layout()->orientation() == Qt::Vertical)
        return view()->layout()->scrollX();
    else
        return 0;
}

int GridTableGroupRowPrimitive::scrollY() const
{
    if (view()->layout()->orientation() != Qt::Vertical)
        return view()->layout()->scrollY();
    else
        return 0;
}

void GridTableGroupRowPrimitive::paintEvent(QPainter& painter)
{
    if (rowLayoutItem()->viewBase()->layout()->leftFixedIndent() == 0)
        painter.translate(-scrollX(), -scrollY());

    GridGroupRowPrimitive::paintEvent(painter);
}

/* GridCardGroupRowPrimitive */
GridCardGroupRowPrimitive::GridCardGroupRowPrimitive(GraphicPrimitive* parent, GridCardRowLayoutItem* row)
    : GridGroupRowPrimitive(parent, row)
{
}

void GridCardGroupRowPrimitive::paintEvent(QPainter& painter)
{
    GridGroupRowPrimitive::paintEvent(painter);
}


/* GridTableCellPrimitiveAbstract */
GridTableCellPrimitiveAbstract::GridTableCellPrimitiveAbstract(GridViewBase* view, GraphicPrimitive* parent,
    GridRowLayoutItem* rowLayoutItem, const GridDataTableColumn* column)
    : EmbedGraphicPrimitive(parent->control(), parent),
    m_view(view), m_column(column), m_rowLayoutItem(rowLayoutItem),
    m_dirtyData(true), m_effectLoadData(false), m_useEffect(true),
    m_leftIndent(0), m_rightIndent(0)
{
}

GridRow* GridTableCellPrimitiveAbstract::gridRow() const
{
    return m_rowLayoutItem != Q_NULL ? m_rowLayoutItem->gridRow() : Q_NULL;
}

void GridTableCellPrimitiveAbstract::setDirtyData(bool flag)
{
    m_dirtyData = flag;
    m_useEffect = m_view->m_contextStates == NoChanges && m_rowLayoutItem != m_rowLayoutItem->layoutEngine()->focusedRowLayoutItem();
}

int GridTableCellPrimitiveAbstract::columnIndex() const
{
    if (m_column != Q_NULL)
        return m_column->index();
    else
        return m_view->modelController()->focusedColumnIndex();
}

bool GridTableCellPrimitiveAbstract::setNewPressedCellPrimitive(QMouseEvent* e, QPoint& newPos)
{
    GridTableCellPrimitiveAbstract* new_pressed = qobject_cast<GridTableCellPrimitiveAbstract *>(control()->hitTest(e->pos()));
    if (new_pressed == Q_NULL)
        return false;
    if (control()->pressedGP() == new_pressed)
        return false;
    control()->setPressedGP(new_pressed);

    QRect r = new_pressed->translateToSiteRect(new_pressed->rect());
    newPos = QPoint(e->pos().x() - r.x(), e->pos().y() - r.y());
    return true;
}

GridViewBase* GridTableCellPrimitiveAbstract::view() const
{
    return m_view;
}

GridDataTableColumn* GridTableCellPrimitiveAbstract::column() const
{
    return const_cast<GridDataTableColumn *>(m_column);
}

void GridTableCellPrimitiveAbstract::handler_mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    EmbedGraphicPrimitive::mousePressEvent(point, e);
    setFocus();
}

void GridTableCellPrimitiveAbstract::handler_mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e)
{
    EmbedGraphicPrimitive::mouseDoubleClickEvent(point, e);
    setFocus();
}

void GridTableCellPrimitiveAbstract::handler_mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    EmbedGraphicPrimitive::mouseReleaseEvent(point, e);
}

void GridTableCellPrimitiveAbstract::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    e->ignore();

    Q_DELETE_AND_NULL(m_view->m_clickHandler);

    m_view->m_clickHandler = new CellMouseClickHandler(m_view, gridRow(), columnIndex());

    m_view->m_editContext.changed = false;

    if (!m_view->m_clickHandler->mousePress(e, m_column == Q_NULL || e->modifiers() & Qt::ControlModifier))
        return;

    QPoint newPos = point;
    setNewPressedCellPrimitive(e, newPos);

    //Check if editing context was changed and then check eating flag.
    if (m_view->m_editContext.changed)
    {
        if (!m_view->checkActivationEditorPolicy(EatingMouseEvents))
            return;
    }

    GridTableCellPrimitiveAbstract* cell_primitive = qobject_cast<GridTableCellPrimitiveAbstract *>(control()->pressedGP());
    if (cell_primitive != Q_NULL)
        cell_primitive->handler_mousePressEvent(newPos, e);
}

void GridTableCellPrimitiveAbstract::mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e)
{
    e->ignore();

    if (!m_view->m_clickHandler)
        return;

    m_view->m_editContext.changed = false;

    if (!m_view->m_clickHandler->mouseDblClick(e, m_column == Q_NULL || e->modifiers() & Qt::ControlModifier))
        return;

    QPoint newPos = point;
    setNewPressedCellPrimitive(e, newPos);

    //Check if editing context was changed and then check eating flag.
    if (m_view->m_editContext.changed)
    {
        if (!m_view->checkActivationEditorPolicy(EatingMouseEvents))
            return;
    }
    GridTableCellPrimitiveAbstract* cell_primitive = qobject_cast<GridTableCellPrimitiveAbstract *>(control()->pressedGP());
    if (cell_primitive != Q_NULL)
        cell_primitive->handler_mouseDoubleClickEvent(newPos, e);
}

void GridTableCellPrimitiveAbstract::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    e->ignore();

    //m_view->selectCurrentRanges(e->modifiers()); TODO

    if (!m_view->m_clickHandler)
        return;

    m_view->m_editContext.changed = false;

    m_view->m_clickHandler->mouseRelease(e, m_column == Q_NULL || e->modifiers() & Qt::ControlModifier);

    QPoint newPos = point;
    setNewPressedCellPrimitive(e, newPos);

    //Check if editing context was turn-on and then check mouse eating flag.
    if (m_view->m_editContext.changed)
    {
        if (!m_view->checkActivationEditorPolicy(EatingMouseEvents))
            return;
    }
    GridTableCellPrimitiveAbstract* cell_primitive = qobject_cast<GridTableCellPrimitiveAbstract *>(control()->pressedGP());
    if (cell_primitive != Q_NULL)
        cell_primitive->handler_mouseReleaseEvent(newPos, e);
}

void GridTableCellPrimitiveAbstract::paintEvent(QPainter& painter)
{
    bool hasEffect = false;
    SmoothEffect* eff = effects()->findEffect(this);
    QRect bounded = boundedRect();
    if (m_dirtyData)
    {
        if (eff)
        {
            if (m_useEffect)
            {
                eff->setDuration(kGridFastScrollEffectDuration);
                eff->setInitialStateToEmpty(width(), height());
                m_effectLoadData = true;
                m_dirtyData = false;
                return;
            }
            effects()->stop(eff);
            eff = Q_NULL;
        }
        loadData();
        m_effectLoadData = false;
        hasEffect = m_useEffect && m_rowLayoutItem->layoutEngine()->fastScrollEffect();
    }
    else if (m_effectLoadData)
    {
        m_dirtyData = true; //To avoid assert in the loadData() function.
        loadData();
        m_effectLoadData = false;
    }

    GridEditorStyleOption option;
    initEditorOption(option);
    GridDataRowPrimitive* parentDataRowGP = static_cast<GridDataRowPrimitive *>(parentGP());

    QRect zoomedRect = bounded.adjusted(-m_leftIndent * m_view->optionsref()->zoomFactor(), 0,
        m_rightIndent * m_view->optionsref()->zoomFactor(), 0);

    bool rowSelectionPolicy = (m_view->optionsref()->selectionPolicy() == SelectSingleRow || m_view->optionsref()->selectionPolicy() == SelectMultiRow);
    if (option.state & QStyle::State_Selected)
    {
        if (!rowSelectionPolicy)
            parentDataRowGP->paintSelectedRect(painter, option.palette, zoomedRect, isMouseOver());
    }
    else
    {
        QBrush bg = option.state & QStyle::State_On ? option.palette.brush(QPalette::AlternateBase) : option.palette.brush(QPalette::Base);
        if (bg != parentDataRowGP->bgBrush())
            painter.fillRect(zoomedRect, bg);
    }
    
    {
        bool optimization = false;
        if (eff == Q_NULL && hasEffect/* && !breakEffect*/)
        {
            eff = new SmoothEffect();
            eff->intialize(this);
            eff->setInitialStateToEmpty(width(), height());
            eff->setDuration(kGridFastScrollEffectDuration);
            effects()->run(eff);
            optimization = true;
        }

        if (eff != Q_NULL)
        {
            if (!optimization)
            {
                double saveOpacity, currentOpacity;
                eff->paintToCurrent(&painter, currentOpacity);

                saveOpacity = painter.opacity();
                painter.setOpacity(saveOpacity * currentOpacity);
                doPaintContent(painter, option);
                painter.setOpacity(saveOpacity);
            }
        }
        else
        {
            doPaintContent(painter, option);
        }
    }

    bool isNewRow = m_rowLayoutItem != Q_NULL && m_rowLayoutItem->type() == GridTableRowLayoutItem::NewData;

    if (
        m_view->optionsref()->focusFrameEnabled() &&
        option.state & QStyle::State_HasFocus &&
        control()->hasFocus() &&
        (!m_view->isEditing()) && // || (editorRepository() != Q_NULL && editorRepository()->editorActivationPolicy() & Always)) &&
        !(m_view->isNewRowEditing() && !isNewRow)
        )
    {
        QRect bounded = boundedRect().adjusted(-m_leftIndent * m_view->optionsref()->zoomFactor(), 0,
            m_rightIndent * m_view->optionsref()->zoomFactor(), 0);
        draw_focus_frame(painter, option.state & QStyle::State_Selected ? parentDataRowGP->selected_fgBrush().color() : parentDataRowGP->fgBrush().color(), bounded);
    }
}

/* GridTableCellGraphic */
GridTableCellGraphic::GridTableCellGraphic(GridViewBase* view,
    GraphicPrimitive* parent,
    GridRowLayoutItem* rowLayoutItem,
    const GridDataTableColumn* column)
    : GridTableCellPrimitiveAbstract(view, parent, rowLayoutItem, column),
    m_editor(Q_NULL)
{
    Q_ASSERT(column != Q_NULL);
    setId(GridHitInfo::Cell);
    setVisibility(false);
    m_activateButtonsTimer = new QTimer(this);
    m_activateButtonsTimer->setSingleShot(true);
    connect(m_activateButtonsTimer, SIGNAL(timeout()), this, SLOT(activateButtonsTimeout()));
}

GridTableCellGraphic::~GridTableCellGraphic()
{
    resetEditor();
    Q_ASSERT(m_editor == Q_NULL);
    removeButtons();
}

GridModelDataBinding* GridTableCellGraphic::dataBinding() const
{
    return m_column->dataBinding();
}

QModelIndex GridTableCellGraphic::modelIndex() const
{
    if (m_view->model() == Q_NULL || dataBinding() == Q_NULL || gridRow() == Q_NULL)
        return QModelIndex();
    return m_view->model()->index(
        gridRow()->modelRowIndex(), dataBinding()->column(), m_view->modelController()->parentIndex());
}

void GridTableCellGraphic::initEditorOption(GridEditorStyleOption& option) const
{
    const int columnIndex = column()->index();
    bool isNewRow = m_rowLayoutItem->type() == GridRowLayoutItem::NewData;
    bool selected = (m_view->optionsref()->selectionPolicy() == SelectSingleCell ||
        m_view->optionsref()->selectionPolicy() == SelectMultiCell) ? m_rowLayoutItem->isCellSelected(columnIndex) :
        m_rowLayoutItem->isRowSelected();
    
    GridEditorRepository* repository = column()->editorRepository();
 
    init_editor_option(m_view, &option, dataBinding(), gridRow(),
        columnIndex, m_view->layout()->alternatingRowColors(gridRow()), isNewRow, selected,
        repository != Q_NULL ? repository->isEnabled() : false, rowLayoutItem()->isRowFocused(), boundedRect(),
        repository != Q_NULL ? repository->alignment() : Qt::AlignCenter);
}

void GridTableCellGraphic::doPaintContent(QPainter& painter, GridEditorStyleOption& option)
{
    if (widget() != Q_NULL)
    {
        EmbedGraphicPrimitive::paintEvent(painter); //Do not use GridTableCellPrimitiveAbstract::paintEvent(...)
    }
    else if (editor() != Q_NULL)
    {
        editor()->paintEvent(painter, option);
    }
}


void GridTableCellGraphic::keyPressEvent(QKeyEvent* e)
{
    GridTableCellPrimitiveAbstract::keyPressEvent(e);
}

void GridTableCellGraphic::mouseMoveEvent(const QPoint& point, QMouseEvent* e)
{
    GridTableCellPrimitiveAbstract::mouseMoveEvent(point, e);

    if (widget() == Q_NULL && editor() != Q_NULL)
        editor()->mouseMoveEvent(point);

    if (m_view->optionsref()->isFocusFollowsMouse() &&
        (m_view->focusedRow() != gridRow() || m_view->focusedColumnIndex() != columnIndex()))
    {
        if (m_view->isNewRowEditing())
        {
            m_view->setFocusedColumnIndex(columnIndex());
        }
        else
        {
            m_view->setCellFocus(gridRow()->rowIndex(), columnIndex());
        }
    }
}

/* CellMouseClickHandler */
CellMouseClickHandler::CellMouseClickHandler(GridViewBase* view, GridRow* row, int columnIndex)
    : QObject(Q_NULL), m_view(view), m_columnIndex(columnIndex),
    m_isPressed(false), m_isDblClicked(false), m_focusByMousePress(false)
{
    Q_ASSERT(row != Q_NULL);
    m_persistentRow = new GridPersistentRow(view->modelController());
    m_persistentRow->set(row);
}

CellMouseClickHandler::~CellMouseClickHandler()
{
    /*
    This assert means that cell does not get back mouse release event so the endUpdate() has not been invoked
    and grid view is in the not working state.
    */
    Q_ASSERT(!m_view->isUpdating());

    Q_DELETE_AND_NULL(m_persistentRow);
    m_view = Q_NULL;
}

bool CellMouseClickHandler::mousePress(QMouseEvent* e, bool select)
{
    m_isPressed = true;
    bool ret = false;

    if (qobject_cast<GridCellDragObject *>(m_view->grid()->getDragDropObject()) == Q_NULL && e->button() == Qt::LeftButton)
    {
        //m_view->beginUpdate();

        Q_ASSERT(m_persistentRow->row() != Q_NULL);
        m_focusByMousePress =
            (m_persistentRow->equal(m_view->modelController()->focusedRow()) &&
            m_view->focusedColumnIndex() == m_columnIndex);

        if (select)
            ret = m_view->setCellFocus(m_persistentRow->row()->rowIndex(), m_columnIndex, e->modifiers(), SelectCurrentEvent);
        else
            ret = m_view->setCellFocus(m_persistentRow->row()->rowIndex(), m_columnIndex, e->modifiers(), m_focusByMousePress ? MousePressFocused : MousePressNoFocused);
    }

    return ret;
}

bool CellMouseClickHandler::mouseDblClick(QMouseEvent* e, bool select)
{
    m_isDblClicked = true;

    bool ret = false;


    if (qobject_cast<GridCellDragObject *>(m_view->grid()->getDragDropObject()) == Q_NULL && e->button() == Qt::LeftButton)
    {
        GridRow* row = m_persistentRow->isValid() ? m_persistentRow->row() : Q_NULL;

        if (row != Q_NULL)
        {
            if (select) {}
            else
                ret = m_view->setCellFocus(row->rowIndex(), m_columnIndex, e->modifiers(), m_focusByMousePress ? DblClickFocused : DblClickNoFocused);
        }
    }

    return ret;
}

void CellMouseClickHandler::mouseRelease(QMouseEvent* e, bool select)
{
    if (!isPressed() && !isDblClicked())
        return;

    GridRow* row = m_persistentRow->isValid() ? m_persistentRow->row() : Q_NULL;
    if (row != Q_NULL)
    {
        if (m_view->grid()->dragDropState() != dragActive && e->button() == Qt::LeftButton)
        {
            if (!isDblClicked())
            {
                if (select) {}
                else
                    m_view->setCellFocus(row->rowIndex(), m_columnIndex, e->modifiers(), m_focusByMousePress ? ClickFocused : ClickNoFocused);
            }
        }

        if (isDblClicked())
        {
            RowClickEventArgs rowArgs(m_persistentRow->row());
            emit m_view->rowDblClicked(&rowArgs);

            GridCell cell(m_persistentRow->row(), m_columnIndex);
            CellClickEventArgs cellArgs(&cell);
            emit m_view->cellDblClicked(&cellArgs);
        }
        else
        {
            RowClickEventArgs rowArgs(m_persistentRow->row());
            emit m_view->rowClicked(&rowArgs);

            GridCell cell(m_persistentRow->row(), m_columnIndex);
            CellClickEventArgs cellArgs(&cell);
            emit m_view->cellClicked(&cellArgs);
        }
    }

    m_view->layout()->rowsPanel()->update();

    m_isPressed = false;
    m_isDblClicked = false;
}

bool CellMouseClickHandler::isDblClicked() const
{
    return m_isDblClicked;
}

bool CellMouseClickHandler::isPressed() const
{
    return m_isPressed;
}

void GridTableCellGraphic::handler_mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    if (editor() != Q_NULL)
        editor()->mousePressEvent(point);
    GridTableCellPrimitiveAbstract::handler_mousePressEvent(point, e);
}

void GridTableCellGraphic::handler_mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e)
{
    if (editor() != Q_NULL)
        editor()->mouseDblClickEvent(point);
    GridTableCellPrimitiveAbstract::handler_mouseDoubleClickEvent(point, e);
}

void GridTableCellGraphic::handler_mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    if (editor() != Q_NULL)
    {
        if (checkClickEvent(e))
            editor()->mouseClickEvent(point);
        editor()->mouseReleaseEvent(point);
    }
    GridTableCellPrimitiveAbstract::handler_mouseReleaseEvent(point, e);
}

void GridTableCellGraphic::mouseOverEvent(const QPoint& pos)
{
    updateButtonLayout();

    if (widget() == Q_NULL && editor() != Q_NULL)
        editor()->mouseOverEvent(pos);

    EmbedGraphicPrimitive::mouseOverEvent(pos);
    if (m_rowLayoutItem != Q_NULL)
    {
        m_rowLayoutItem->update();
    }
}

void GridTableCellGraphic::mouseOutEvent()
{
    updateButtonLayout();
 
    if (widget() == Q_NULL && editor() != Q_NULL)
        editor()->mouseOutEvent();

    EmbedGraphicPrimitive::mouseOutEvent();
    if (m_rowLayoutItem != Q_NULL)
        m_rowLayoutItem->update();
}

GridDragDropObject* GridTableCellGraphic::createDragDropObject(const QPoint& pos)
{
    if (m_view->isEditing() || m_view->isNewRowEditing())
        return Q_NULL;

    bool startDrag = m_view->optionsref()->dragEnabled();
    if (m_view->optionsref()->selectionPolicy() == SelectSingleRow || m_view->optionsref()->selectionPolicy() == SelectMultiRow)
        startDrag = startDrag && m_rowLayoutItem->isRowSelected();
    else
        startDrag = startDrag && m_rowLayoutItem->isCellSelected(columnIndex());

    if (startDrag)
    {
        GridCellDragObject* obj = new GridCellDragObject(m_view, this, pos);
        if (obj->startDrag())
            return obj;
        delete obj;
    }

    if (m_view->optionsref()->isMultiSelect())
        return new GridTableViewSelectionObject(static_cast<GridTableView *>(m_view), GridCell(m_rowLayoutItem->gridRow(), columnIndex()), pos);

    return Q_NULL;
}

bool GridTableCellGraphic::checkDragDropEvent(const QPoint& pos, GridDragDropObject* object)
{
    GridCellDragObject* obj = qobject_cast<GridCellDragObject *>(object);
    if (obj != Q_NULL)
        return obj->checkDragDropEvent(qtn_get_layout_zone(this, pos, m_view->layout()->orientation(), true), true, this);
    return false;
}

void GridTableCellGraphic::drawBorder(QPainter& painter)
{
    QRect r(QPoint(0, 0), rect().size());
    drawBorderPrimitive(painter, m_borderPen, r, r,
        m_borderLeft - qRound(m_leftIndent * m_view->optionsref()->zoomFactor()),
        m_borderRight - qRound(m_rightIndent * m_view->optionsref()->zoomFactor()), m_borderTop, m_borderBottom);
}

void GridTableCellGraphic::activateButtonsTimeout()
{
    updateButtonLayout();
    invalidate();
}

void GridTableCellGraphic::updateButtonLayout()
{
    m_leftIndent = 0;
    m_rightIndent = 0;
    bool continueUpdate = false;

    QRect oldBoundedRect = boundedRect();

    if (m_buttons.size() > 0)
    {
        bool over = isMouseOver();
        for (QList<GridTableCellButtonGraphic *>::const_iterator it =
            m_buttons.constBegin(); !over && it != m_buttons.constEnd(); ++it)
        {
            GridTableCellButtonGraphic* btn = *it;
            over |= btn->isMouseOver();
        }

        for (QList<GridTableCellButtonGraphic *>::const_iterator it =
            m_buttons.constBegin(); it != m_buttons.constEnd(); ++it)
        {
            GridTableCellButtonGraphic* btn = *it;
            if (btn->info()->policy == FluentFixedPolicy)
                continueUpdate |= btn->setActivate(true, false);
            else if (btn->info()->policy == FixedPolicy)
                continueUpdate |= btn->setActivate(true, true);
            else if (btn->info()->policy == FluentMouseOverPolicy && over)
                continueUpdate |= btn->setActivate(true, false);
            else if (btn->info()->policy == MouseOverPolicy && over)
                continueUpdate |= btn->setActivate(true, true);
            else if (btn->info()->policy == FluentFocusPolicy &&
                m_view->modelController()->isCellFocused(gridRow(), columnIndex()))
                continueUpdate |= btn->setActivate(true, false);
            else if (btn->info()->policy == FocusPolicy &&
                m_view->modelController()->isCellFocused(gridRow(), columnIndex()))
                continueUpdate |= btn->setActivate(true, true);
            else
                continueUpdate |= btn->setActivate(false, false);

            if (btn->info()->position == LeftPosition)
                m_leftIndent += btn->buttonWidth();
            else
                m_rightIndent += btn->buttonWidth();
        }
    }

    setBorderLeft(qRound(m_leftIndent * m_view->optionsref()->zoomFactor()));
    setBorderTop(0);
    int br = 0, bb = 0;
    bool _2d;
    get_cell_border_lines(m_view, br, bb, _2d);
    if (qobject_cast<GridCardView *>(m_view) != Q_NULL)
    {
        //For card view border is 0.
        br = 0;
        bb = 0;
    }

    setBorderRight(br + qRound(m_rightIndent * m_view->optionsref()->zoomFactor()));
    setBorderBottom(bb);
    setBorderPen(m_view->optionsref()->gridLinePen());

    if (boundedRect() != oldBoundedRect)
        updateWidget(true);

    if (m_buttons.size() > 0)
    {
        QRect bounded = boundedRect();

        int leftIndent = borderLeft() - qRound(m_leftIndent * m_view->optionsref()->zoomFactor());
        int rightIndent = borderLeft() + bounded.width();

        for (QList<GridTableCellButtonGraphic *>::const_iterator it =
            m_buttons.constBegin(); it != m_buttons.constEnd(); ++it)
        {
            GridTableCellButtonGraphic* btn = *it;
            QRect r;
            if (btn->info()->position == LeftPosition)
            {
                r.setLeft(left() + leftIndent);
                leftIndent += btn->buttonWidth() * m_view->optionsref()->zoomFactor();
            }
            else
            {
                r.setLeft(left() + rightIndent);
                rightIndent += qRound(btn->buttonWidth() * m_view->optionsref()->zoomFactor());
            }
            r.setTop(top() + borderTop());
            r.setWidth(qRound(btn->info()->getButtonWidth() * m_view->optionsref()->zoomFactor()));
            r.setHeight(bounded.height());
            btn->setRect(r);
        }
    }

    if (continueUpdate)
        m_activateButtonsTimer->start(5);
}

void GridTableCellGraphic::update(const QRect& rect)
{
    m_rowLayoutItem->quickRowButtonGP()->update();

    EmbedGraphicPrimitive::update(rect);
}

void GridTableCellGraphic::invalidate(bool fullyViewUpdate)
{
    if (fullyViewUpdate)
        m_view->layout()->update();
    else
        GraphicPrimitive::update();
}

QWidget* GridTableCellGraphic::parent() const
{
    return Q_NULL;
}

QRect GridTableCellGraphic::geometry() const
{
    return boundedRect();
}

void GridTableCellGraphic::showHint(const QString& text) const
{
    QPoint p = mapToGlobal(boundedRect().topLeft());
    QToolTip::showText(p, text);
}

void GridTableCellGraphic::updateCursor(const QCursor& cursor)
{
    setCursor(cursor);
}

void GridTableCellGraphic::linkActivated(const QString& anchor)
{
    EditorLinkEventArgs args(m_column, gridRow(), anchor);
    m_view->editorLinkActivated(&args);
}

GridEditorStyleOption GridTableCellGraphic::editorOption()
{
    GridEditorStyleOption retval;
    initEditorOption(retval);
    return retval;
}

void Qtitan::get_cell_border_lines(GridViewBase* view, int& borderRight, int& borderBottom, bool& _2d)
{
    GridLinesStyle s = view->optionsref()->gridLines();

    bool verticalLayout = view->layout()->orientation() == Qt::Vertical;
    if (verticalLayout)
        _2d = (s == LinesBoth2D || s == LinesHorizontal2D);
    else
        _2d = (s == LinesBoth2D || s == LinesVertical2D);

    if (s == LinesBoth || s == LinesVertical || s == LinesVertical2D || s == LinesBoth2D)
        borderRight = view->optionsref()->gridLineWidth();
    else
        borderRight = 0;

    if (s == LinesBoth || s == LinesHorizontal || s == LinesBoth2D || s == LinesHorizontal2D)
        borderBottom = view->optionsref()->gridLineWidth();
    else
        borderBottom = 0;
}

GridEditor* GridTableCellGraphic::createEditor()
{
    if (m_editor == Q_NULL)
    {
        GridEditorRepository* repository = column()->editorRepository();
        if (repository == Q_NULL)
            return Q_NULL;
        m_editor = repository->createEditor();
        Q_ASSERT(m_editor != Q_NULL);
        m_editor->initEditor(repository, this);
        m_editor->setEditMode(false);
        setWidget(m_editor->getCellWidget());
        m_view->raiseEditorCreated(m_editor);
    }
    return m_editor;
}

void GridTableCellGraphic::resetEditor()
{
    setWidget(Q_NULL);
    if (m_editor == Q_NULL)
        return;

    Q_ASSERT(m_view->m_editContext.editor != m_editor);

    m_view->raiseEditorDestroying(m_editor);
    Q_DELETE_AND_NULL(m_editor);
}

GridEditor* GridTableCellGraphic::editor() const
{
    return m_editor;
}

void GridTableCellGraphic::updateFont()
{
    if (widget() != Q_NULL)
    {
        GridEditorStyleOption option;
        initEditorOption(option);
        if (widget()->font() != option.font)
            widget()->setFont(option.font);
    }
}

void GridTableCellGraphic::createButton(GridCellButtonInfo* info)
{
    GridTableCellButtonGraphic* gp = new GridTableCellButtonGraphic(this, info);
    m_buttons.push_back(gp);
}

void GridTableCellGraphic::removeButtons()
{
    while (m_buttons.size() > 0)
        delete m_buttons.takeAt(0);
}

void GridTableCellGraphic::loadData()
{
    Q_ASSERT(m_dirtyData);
    m_dirtyData = false;

    if (gridRow() == Q_NULL)
        return;

    createEditor();
    Q_ASSERT(editor() != Q_NULL);

    if (editor()->isEditMode())
    {
        if (!editor()->isContextModified())
        {
            QVariant v;
            dataBinding()->getValue(gridRow()->modelRowIndex(), dataBinding()->editRole(), v);
            editor()->updateContext(v);
        }
    }
    else
    {
        QVariant v;
        dataBinding()->getValue(gridRow()->modelRowIndex(), dataBinding()->displayRole(), v);
        editor()->updateContext(v);
    }

    Q_ASSERT(m_column != Q_NULL);

    int buttonIndex = 0;
    if (m_buttons.size() > 0)
    {
        const int columnIndex = m_column->index();
        for (QList<GridTableCellButtonGraphic *>::const_iterator bnt_it =
            m_buttons.constBegin(); bnt_it != m_buttons.constEnd(); ++bnt_it)
        {
            CellButtonEventArgs args(gridRow(), columnIndex, buttonIndex);
            emit column()->buttonStateChanged(&args);
            (*bnt_it)->m_enabled = args.isEnadled();
            buttonIndex++;
        }
    }
}

/* GridPreviewExpandButtonPrimitive */
GridPreviewExpandButtonPrimitive::GridPreviewExpandButtonPrimitive(GraphicPrimitive* parent)
    : GridShadeButtonPrimitive(parent)
{
    setCursor(Qt::PointingHandCursor);
}

QSize GridPreviewExpandButtonPrimitive::sizeHint()
{
    const QString text = Grid::tr_compatible(QtnGridPreviewRowString);
    GridTablePreviewRowPrimitive* preview = static_cast<GridTablePreviewRowPrimitive *>(parentGP());
    int h = qRound(kGridPreviewExpandButtonHeight * preview->rowLayoutItem()->viewBase()->optionsref()->zoomFactor());
    QFont f = control()->font();
    f.setPixelSize(h / 1.5);
    f.setBold(true);
    QFontMetrics metrics(f);
    int w = metrics.width(text) + qRound(30 * preview->rowLayoutItem()->viewBase()->optionsref()->zoomFactor());
    if (preview->view()->layout()->orientation() == Qt::Vertical)
        return QSize(w, h);
    else
        return QSize(h, w);
}

void GridPreviewExpandButtonPrimitive::paintEvent(QPainter& painter)
{
    GridTablePreviewRowPrimitive* preview = static_cast<GridTablePreviewRowPrimitive *>(parentGP());
    bool verticalLayout = preview->view()->layout()->orientation() == Qt::Vertical;

    GridShadeButtonPrimitive::paintEvent(painter);
    QTextOption opt;
    opt.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    QRect rect = boundedRect();

    if (!verticalLayout)
    {
        painter.rotate(-90);
        rect = QRect(-rect.top() - rect.height(), rect.left(), rect.height(), rect.width());
    }

    QFont f = painter.font();
    f.setPixelSize(rect.height() / 1.5);
    f.setBold(true);
    painter.setFont(f);
    painter.setPen(Qt::white);
    painter.setOpacity(0.7);
    painter.drawText(rect, Grid::tr_compatible(QtnGridPreviewRowString), opt);
}

void GridPreviewExpandButtonPrimitive::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    if (checkClickEvent(e))
    {
        GridTablePreviewRowPrimitive* preview = static_cast<GridTablePreviewRowPrimitive *>(parentGP());
        preview->gridRow()->setPreviewExpanded(!preview->gridRow()->isPreviewExpanded());
    }
}

QPalette GridPreviewExpandButtonPrimitive::getButtonPalette() const
{
    return GridShadeButtonPrimitive::getButtonPalette();
}

void GridPreviewExpandButtonPrimitive::paintTexture(QPainter& painter, const QColor& accentColor, const QRect& rect)
{
    GridTablePreviewRowPrimitive* preview = static_cast<GridTablePreviewRowPrimitive *>(parentGP());
    bool verticalLayout = preview->rowLayoutItem()->viewBase()->layout()->orientation() == Qt::Vertical;

    QPixmap texture(":res/grid_preview_row_button.png");
    QPixmap temp(texture.width(), texture.height());
    temp.fill(accentColor);
    temp.setMask(texture.createHeuristicMask());
    {
        QPainter p(&temp);
        p.drawPixmap(0, 0, texture);
    }
 
    QRect r = rect;
    if (!verticalLayout)
    {
        painter.rotate(-90);
        r = QRect(-r.top() - r.height(), r.left(), r.height(), r.width());
    }
    QImage image = temp.toImage();
    image = image.scaled(r.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawImage(r, image);
}

/* GridTablePreviewRowPrimitive */
GridTablePreviewRowPrimitive::GridTablePreviewRowPrimitive(GridViewBase* view, GraphicPrimitive* parent,
    GridRowLayoutItem* rowLayoutItem)
    : GridTableCellPrimitiveAbstract(view, parent, rowLayoutItem, Q_NULL), m_textDocument(Q_NULL)
{
    m_expandButton = new GridPreviewExpandButtonPrimitive(this);
    m_textDocument = new QTextDocument(this);
    m_textDocument->setDocumentMargin(0);
}

GridTablePreviewRowPrimitive::~GridTablePreviewRowPrimitive()
{
    Q_DELETE_AND_NULL(m_expandButton);
    Q_DELETE_AND_NULL(m_textDocument);
}

void GridTablePreviewRowPrimitive::loadData()
{
    Q_ASSERT(m_dirtyData);
    m_dirtyData = false;

    GridModelDataBinding* dataBinding = m_view->modelController()->previewRowDataBinding();
    QVariant v;
    if (!dataBinding->getValue(gridRow()->modelRowIndex(), dataBinding->displayRole(), v) || !v.isValid())
    {
        v = "<b><font color='#FF0000'>Preview is empty.</font></b>";
    }
    m_textDocument->setHtml(v.toString());
}

void GridTablePreviewRowPrimitive::initEditorOption(GridEditorStyleOption& option) const
{
    bool isNewRow = m_rowLayoutItem->type() == GridTableRowLayoutItem::NewData;
    bool selected = (m_view->optionsref()->selectionPolicy() == SelectSingleCell ||
        m_view->optionsref()->selectionPolicy() == SelectMultiCell) ? false : m_rowLayoutItem->isRowSelected();

    init_editor_option(m_view, &option, m_view->modelController()->previewRowDataBinding(), gridRow(),
        -1, m_view->layout()->alternatingRowColors(gridRow()), isNewRow, selected,
        true, m_rowLayoutItem->isRowFocused(), boundedRect(), Qt::Alignment());
}

void GridTablePreviewRowPrimitive::updateLayout()
{
    m_expandButton->setVisibility(m_view->optionsref()->isPreviewExpandButtonVisible());
    if (!m_expandButton->visibility())
    {
        m_expandButton->setRect(QRect(0, 0, 0, 0));
        return;
    }
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;

    if (verticalLayout)
    {
        const QSize size = m_expandButton->sizeHint();
        QRect r = QStyle::alignedRect(control()->layoutDirection(), Qt::AlignBottom | Qt::AlignHCenter, size, boundedRect());
        m_expandButton->setRect(r);
    }
    else
    {
        const QSize size = m_expandButton->sizeHint();
        QRect r = QStyle::alignedRect(control()->layoutDirection(), Qt::AlignRight | Qt::AlignVCenter, size, boundedRect());
        m_expandButton->setRect(r);
    }
}

QPoint GridTablePreviewRowPrimitive::translateMouseToTextDocument(const QPoint& pos)
{
    QPoint ret = pos - m_textDocumentRect.topLeft();
//    if (m_column->textOrientation() == Qt::Vertical)
//        ret = QPoint(m_textDocumentRect.height() - ret.y(), ret.x());
    return ret;
}

void GridTablePreviewRowPrimitive::handler_mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    if (checkClickEvent(e))
    {
        QString anchor = m_textDocument->documentLayout()->anchorAt(translateMouseToTextDocument(point));
        if (!anchor.isEmpty())
        {
            m_view->previewLinkActivated(gridRow(), anchor);
            e->accept();
            return;
        }
    }
    GridTableCellPrimitiveAbstract::handler_mouseReleaseEvent(point, e);
}

void GridTablePreviewRowPrimitive::mouseMoveEvent(const QPoint& point, QMouseEvent* e)
{
    QString anchor = m_textDocument->documentLayout()->anchorAt(translateMouseToTextDocument(point));
    if (anchor != "")
        setCursor(Qt::PointingHandCursor);
    else
        setCursor(QCursor());
    static_cast<GridTablePreviewRowPrimitive *>(parentGP())->mouseMoveEvent(point, e);
}

void GridTablePreviewRowPrimitive::doPaintContent(QPainter& painter, GridEditorStyleOption& option)
{
    Q_UNUSED(option);
    if (!gridRow()->isPreviewExpanded() && m_view->optionsref()->isPreviewExpandButtonVisible())
        return;

    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;

    m_textDocumentRect = verticalLayout ? boundedRect().adjusted(0, 0, 0, -m_expandButton->height()) : boundedRect().adjusted(0, 0, -m_expandButton->width(), 0);
    QRect r = m_textDocumentRect.adjusted(3, 3, -3, -3);
    QFont font = qtn_font_apply_scale(painter.font(), m_view->optionsref()->zoomFactor());
    if (font != m_textDocument->defaultFont())
        m_textDocument->setDefaultFont(font);
    if (r.width() != m_textDocument->textWidth())
        m_textDocument->setTextWidth(r.width());
    //if (m_textDocument->defaultTextOption().alignment() != option->textAlignment)
    //    m_textDocument->setDefaultTextOption(QTextOption(option->textAlignment));

    qtn_QTextDocument_draw(painter, m_textDocument, option.palette, r);
}


/* GridSummaryLayoutEngine */
GridSummaryLayoutEngine::GridSummaryLayoutEngine(GridViewBase* view, GridRowLayoutItem* rowLayoutItem)
    : m_paintGroupText(true), m_view(view), m_rowLayoutItem(rowLayoutItem), m_parentGP(Q_NULL)
{
}

GridSummaryLayoutEngine::~GridSummaryLayoutEngine()
{
    destroySummaryCells();
}

void GridSummaryLayoutEngine::addColumn(const GridDataTableColumn* column)
{
    Q_ASSERT(m_summaryCells.find(column) == m_summaryCells.end());
    GridSummaryCellPrimitive* summaryCell = createSummaryCell(column);
    m_summaryCells.insert(column, summaryCell);
}

void GridSummaryLayoutEngine::removeColumn(const GridDataTableColumn* column)
{
    QHash<const GridDataTableColumn *, GridSummaryCellPrimitive *>::iterator summary_it = m_summaryCells.find(column);
    Q_ASSERT(summary_it != m_summaryCells.end());
    delete summary_it.value();
    m_summaryCells.erase(summary_it);
}

void GridSummaryLayoutEngine::destroySummaryCells()
{
    for (QHash<const GridDataTableColumn *, GridSummaryCellPrimitive *>::iterator it = m_summaryCells.begin();
        it != m_summaryCells.end(); ++it)
    {
        delete it.value();
    }
}

void GridSummaryLayoutEngine::updateSummaryCellsData()
{
    for (QHash<const GridDataTableColumn *, GridSummaryCellPrimitive *>::iterator it = m_summaryCells.begin();
        it != m_summaryCells.end(); ++it)
    {
        it.value()->setDirtyData();
    }
}

void GridSummaryLayoutEngine::paintBackgroud(QPainter& painter, const QPalette& palette, const QRect& rect)
{
    QStyle* style = view()->grid()->style();
    QStyleOptionFrame opt;
    opt.rect = rect;
    opt.palette = palette;
    opt.state = QStyle::State_None;
    opt.lineWidth = style->pixelMetric(QStyle::PM_MenuBarPanelWidth);
    opt.midLineWidth = 0;
    style->drawPrimitive(QStyle::PE_PanelMenuBar, &opt, &painter, view()->grid());
    //painter.fillRect(rect, palette.window());
    
    QStyleOptionMenuItem menuOpt;
    menuOpt.palette = palette;
    menuOpt.state = QStyle::State_None;
    menuOpt.menuItemType = QStyleOptionMenuItem::EmptyArea;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.rect = rect;//.adjusted(opt.lineWidth, opt.lineWidth, -opt.lineWidth, -opt.lineWidth);
    menuOpt.menuRect = rect;//.adjusted(opt.lineWidth, opt.lineWidth, -opt.lineWidth, -opt.lineWidth);
    style->drawControl(QStyle::CE_MenuBarEmptyArea, &menuOpt, &painter, view()->grid());
    
}

int GridSummaryLayoutEngine::getGroupingIndent()
{
    if (m_rowLayoutItem != Q_NULL)
    {
        if (m_view->optionsref()->groupSummaryPlace() == GroupSummaryRowPlus)
            return m_view->getRowGroupIndent(Q_NULL);
        else
            return m_view->getRowGroupIndent(m_rowLayoutItem->gridRow());
    }
    else
        return 0;
}

int GridSummaryLayoutEngine::getGroupRowIndent()
{
    if (m_rowLayoutItem != Q_NULL)
    {
        if (m_view->optionsref()->groupSummaryPlace() == GroupSummaryRowPlus)
        {
            int ret = get_group_height(m_view->grid()->style(), m_view->optionsref());
            return ret;
        }
    }
    return 0;
}

GridViewBase* GridSummaryLayoutEngine::view() const
{
    return m_view;
}

GridRowLayoutItem* GridSummaryLayoutEngine::rowLayoutItem() const
{
    return m_rowLayoutItem;
}

GraphicPrimitive* GridSummaryLayoutEngine::parentGP() const
{
    return m_parentGP;
}

void GridSummaryLayoutEngine::setParentPrimitive(GraphicPrimitive* parent)
{
    m_parentGP = parent;
    for (QHash<const GridDataTableColumn *, GridSummaryCellPrimitive *>::const_iterator it =
        m_summaryCells.constBegin(); it != m_summaryCells.constEnd(); ++it)
    {
        it.value()->setParentGP(parent);
    }
}

bool GridSummaryLayoutEngine::isGroupRow() const
{
    return m_rowLayoutItem != Q_NULL;
}

GridSummaryCellPrimitive* GridSummaryLayoutEngine::createSummaryCell(const GridDataTableColumn* column)
{
    if (m_rowLayoutItem != Q_NULL)
        return new GridTableGroupSummaryCellGraphic(this);
    else
        return new GridTableFooterSummaryCellGraphic(this, column);
}

bool GridSummaryLayoutEngine::needPaintGroupText() const
{
    return m_paintGroupText;
}

/* GridFooterSummaryPrimitive */
GridFooterSummaryPrimitive::GridFooterSummaryPrimitive(GraphicPrimitive* parent)
    : ScrollableGraphicPrimitive(parent)
{
}

GridFooterSummaryPrimitive::~GridFooterSummaryPrimitive()
{
    Q_DELETE_AND_NULL(m_layoutEngine);
}

GridSummaryLayoutEngine* GridFooterSummaryPrimitive::layoutEngine() const
{
    return m_layoutEngine;
}

void GridFooterSummaryPrimitive::paintEvent(QPainter& painter)
{
    QPalette palette = control()->palette();
    //QStyleOptionToolBar opt;
    //opt.initFrom(control());
    //opt.rect = boundedRect();
    //opt.state |= QStyle::State_Horizontal;
    //opt.features = QStyleOptionToolBar::None;
    //opt.toolBarArea = Qt::NoToolBarArea;
    //painter.fillRect(opt.rect, palette.background());
    //style->drawControl(QStyle::CE_ToolBar, &opt, &painter, control());
    //style->drawPrimitive(QStyle::PE_FrameMenu, &opt, &painter, control());
    //stpyle->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &painter, control());
    m_layoutEngine->paintBackgroud(painter, palette, boundedRect());
}

void GridFooterSummaryPrimitive::updateLayout()
{
    m_rect = QRect(0, 0, 0, 0);
}

void GridFooterSummaryPrimitive::resizeEvent(const QRect& rect)
{
    Q_UNUSED(rect);
    m_layoutEngine->updateCellsLayout();
}

QSize GridFooterSummaryPrimitive::sizeHint()
{
    if (!m_layoutEngine->view()->optionsref()->isFooterSummaryVisible())
        return QSize(0, 0);
    
    if (m_layoutEngine->view()->layout()->orientation() == Qt::Vertical)
        return QSize(0, m_layoutEngine->view()->layout()->calculateGridSummaryRowLength(Q_NULL));
    else
        return QSize(m_layoutEngine->view()->layout()->calculateGridSummaryRowLength(Q_NULL), 0);
}

int GridFooterSummaryPrimitive::scrollX() const
{
    if (m_layoutEngine->view()->layout()->orientation() == Qt::Vertical)
        return m_layoutEngine->view()->layout()->scrollX();
    else
    return 0;
}

int GridFooterSummaryPrimitive::scrollY() const
{
    if (m_layoutEngine->view()->layout()->orientation() != Qt::Vertical)
        return m_layoutEngine->view()->layout()->scrollY();
    else
        return 0;
}

/* GridSummaryCellPrimitive */
GridSummaryCellPrimitive::GridSummaryCellPrimitive(GridSummaryLayoutEngine* layoutEngine)
    : GraphicPrimitive(layoutEngine->view()->grid(), layoutEngine->parentGP()),
    m_textColor(Qt::transparent), m_layoutEngine(layoutEngine),
    m_column(Q_NULL), m_dirtyData(true)
{
}

void GridSummaryCellPrimitive::setTableColumn(const GridDataTableColumn* column)
{
    m_column = column;
}

const GridDataTableColumn* GridSummaryCellPrimitive::tableColumn() const
{
    return m_column;
}

void GridSummaryCellPrimitive::mousePressEvent(const QPoint& pos, QMouseEvent* event)
{
    static_cast<GridSummaryCellPrimitive *>(parentGP())->mousePressEvent(pos, event);
}

void GridSummaryCellPrimitive::mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e)
{
    static_cast<GridSummaryCellPrimitive *>(parentGP())->mouseDoubleClickEvent(point, e);
}

void GridSummaryCellPrimitive::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    static_cast<GridSummaryCellPrimitive *>(parentGP())->mouseReleaseEvent(point, e);
}

void GridSummaryCellPrimitive::mouseMoveEvent(const QPoint& point, QMouseEvent* e)
{
    static_cast<GridSummaryCellPrimitive *>(parentGP())->mouseMoveEvent(point, e);
}

void GridSummaryCellPrimitive::paintEvent(QPainter& painter)
{
    if (m_dirtyData)
        loadData();
    QString text = getText();

    bool verticalLayout = m_layoutEngine->view()->layout()->orientation() == Qt::Vertical;
    bool layoutWithBorder = m_layoutEngine->isGroupRow() && 
        m_layoutEngine->view()->optionsref()->groupSummaryPlace() == GroupSummaryRow;

    if (layoutWithBorder)
    {
        QPalette palette = m_layoutEngine->view()->optionsref()->groupPalette(m_layoutEngine->rowLayoutItem()->gridRow()->depth());
        palette.setCurrentColorGroup(control()->hasFocus() ? QPalette::Active : QPalette::Inactive);

        if (getTextColor() != Qt::transparent)
            painter.setPen(getTextColor());
        else
        {
            bool selected = m_layoutEngine->rowLayoutItem()->isRowSelected();
            if (selected)
                painter.setPen(palette.color(QPalette::HighlightedText));
            else if (m_layoutEngine->view()->optionsref()->groupRowStyle() == GridGroupOfficeOutlook)
                painter.setPen("#3764A0");
            else
                painter.setPen(palette.color(QPalette::BrightText));
        }

        QFont font = qtn_font_apply_scale(m_layoutEngine->view()->optionsref()->groupRowFont(), m_layoutEngine->view()->optionsref()->zoomFactor());
        painter.setFont(font);

        QRect r = boundedRect();
        r = r.adjusted(1, 1, -5, -1);

        if (m_hasGroupText)
        {
            QFontMetrics metrics(painter.font());
            QRect summaryRect = metrics.boundingRect(text);
            if (r.width() > summaryRect.width())
            {
                r.setWidth(r.width() - summaryRect.width());
                QString groupRowText = metrics.elidedText(grid_group_row_plain_text(m_layoutEngine->rowLayoutItem()), Qt::ElideRight, r.width());
                draw_text(painter, groupRowText, r, Qt::AlignLeft | Qt::AlignVCenter);
                r.adjust(r.width(), 0, summaryRect.width(), 0);
            }
        }
        draw_text(painter, text, r, Qt::AlignRight | Qt::AlignVCenter);
    }
    else if (!text.isEmpty())
    {
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
        QStyleOptionFrameV3 opt;
#else
        QStyleOptionFrame opt;
#endif
        opt.init(control());
        painter.setFont(m_layoutEngine->view()->getColumnFont(tableColumn()));

        if (getTextColor() != Qt::transparent)
            painter.setPen(getTextColor());
        else
            painter.setPen(m_layoutEngine->isGroupRow() ? qtn_get_thememanager(control())->shadePalette().foreground().color() : qtn_get_thememanager(control())->shadePalette().dark().color());

        if (verticalLayout)
            opt.rect = boundedRect().adjusted(kGridSummaryCellBorder, kGridSummaryCellBorder, -kGridSummaryCellBorder, 0);
        else
            opt.rect = boundedRect().adjusted(kGridSummaryCellBorder, kGridSummaryCellBorder, 0, -kGridSummaryCellBorder);

        if (true)//m_layoutEngine->isGroupRow())
        {
            painter.setOpacity(0.1);
            painter.fillRect(opt.rect, qtn_get_thememanager(control())->shadePalette().foreground().color());
            painter.setOpacity(1.0);
        }
        else
        {
            opt.frameShape = QFrame::StyledPanel;
            opt.state = QStyle::State_Sunken;
            opt.lineWidth = 1;
            opt.midLineWidth = 1;
            opt.palette = control()->palette();
            painter.fillRect(opt.rect, opt.palette.background());
            control()->style()->drawControl(QStyle::CE_ShapedFrame, &opt, &painter, control());
        }
        QRect r = opt.rect.adjusted(4, 2, -4, -2);
        draw_text(painter, text, r, Qt::AlignRight | Qt::AlignVCenter);
        painter.drawRect(boundedRect().adjusted(1, 0, -2, -1));
    }
}

const GridSummaryValue& GridSummaryCellPrimitive::getValue()
{
    return m_value;
}

const QString& GridSummaryCellPrimitive::getText() const
{
    return m_text;
}


const QColor& GridSummaryCellPrimitive::getTextColor() const
{
    return m_textColor;
}

void GridSummaryCellPrimitive::setDirtyData()
{
    m_dirtyData = true;
}

void GridSummaryCellPrimitive::showContextMenuEvent(const QPoint& pos, QContextMenuEvent* event)
{
    Q_UNUSED(pos);
    if (m_layoutEngine->view()->optionsref()->isSummaryMenuDisabled())
        return;
    m_layoutEngine->view()->showSummaryContextMenu(event->pos());
}

/* GridTableGroupSummaryCellGraphic */
GridTableGroupSummaryCellGraphic::GridTableGroupSummaryCellGraphic(GridSummaryLayoutEngine* layoutEngine)
    : GridSummaryCellPrimitive(layoutEngine)
{
    setId(GridHitInfo::GroupSummaryCell);
    ScrollableGraphicPrimitive::setFixedGPY(this, true);
}

void GridTableGroupSummaryCellGraphic::loadData()
{
    if (groupRow() == Q_NULL || tableColumn() == Q_NULL || tableColumn()->dataBinding() == Q_NULL)
        return;

    m_value = m_layoutEngine->view()->modelController()->summary()->getGroupSummaryValue(
        groupRow()->modelGroup(), tableColumn()->dataBinding());

    QString text = m_layoutEngine->view()->modelController()->summary()->getGroupSummaryDisplayText(
        groupRow()->modelGroup(), tableColumn()->dataBinding());

    QVariant v = m_value.value;
    if (m_value.type == SummaryAvg)
        v = v.toDouble() / m_value.count;

    SummaryGroupTextEventArgs args(tableColumn()->dataBinding(), m_value.type, v, groupRow()->modelGroup()->dataBinding->column(), text);

    emit m_layoutEngine->view()->summaryGroupTextChanging(&args);
    m_text = args.text();
    m_textColor = args.textColor();
}

void GridTableGroupSummaryCellGraphic::setSummaryType(GridSummaryType type)
{
    if (groupRow() == Q_NULL ||
        groupRow()->modelGroup() == Q_NULL || tableColumn()->dataBinding() == Q_NULL)
        return;

    m_layoutEngine->view()->modelController()->summary()->setGroupSetting(
        groupRow()->modelGroup()->dataBinding,
        tableColumn()->dataBinding(),
        tableColumn()->dataBinding()->summaryRole(), type);
}

GridRow* GridTableGroupSummaryCellGraphic::groupRow() const
{
    return m_layoutEngine->rowLayoutItem() != Q_NULL ? m_layoutEngine->rowLayoutItem()->gridRow() : Q_NULL;
}

void GridTableGroupSummaryCellGraphic::mouseOverEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
    if (m_layoutEngine->view()->optionsref()->groupSummaryPlace() == GroupSummaryRow)
        parentGP()->parentGP()->update();
}

void GridTableGroupSummaryCellGraphic::mouseOutEvent()
{
    if (m_layoutEngine->view()->optionsref()->groupSummaryPlace() == GroupSummaryRow)
        parentGP()->parentGP()->update();
}

/* GridTableFooterSummaryCellGraphic */
GridTableFooterSummaryCellGraphic::GridTableFooterSummaryCellGraphic(GridSummaryLayoutEngine* layoutEngine, const GridDataTableColumn* column)
    : GridSummaryCellPrimitive(layoutEngine)
{
    setId(GridHitInfo::FooterSummaryCell);
    setTableColumn(column);
    ScrollableGraphicPrimitive::setFixedGPY(this, true);
}

void GridTableFooterSummaryCellGraphic::loadData()
{
    if (tableColumn()->dataBinding() == Q_NULL)
        return;
    m_value = m_layoutEngine->view()->modelController()->summary()->getFooterSummaryValue(tableColumn()->dataBinding());

    QString text = m_layoutEngine->view()->modelController()->summary()->getFooterSummaryDisplayText(tableColumn()->dataBinding());
    QVariant v = m_value.value;
    if (m_value.type == SummaryAvg)
        v = v.toDouble() / m_value.count;
    SummaryTextEventArgs args(tableColumn()->dataBinding(), m_value.type, v, text);
    emit m_layoutEngine->view()->summaryFooterTextChanging(&args);
    m_text = args.text();
    m_textColor = args.textColor();
}

void GridTableFooterSummaryCellGraphic::setSummaryType(GridSummaryType type)
{
    if (tableColumn()->dataBinding() == Q_NULL)
        return;
    m_layoutEngine->view()->modelController()->summary()->setFooterSetting(tableColumn()->dataBinding(), tableColumn()->dataBinding()->summaryRole(), type);
}

/*!
\class GridTableCellButtonGraphic
\inmodule QtitanDataGrid
\internal
*/
GridTableCellButtonGraphic::GridTableCellButtonGraphic(GridTableCellGraphic* cell, GridCellButtonInfo* info)
    : GraphicPrimitive(cell->control(), cell->parentGP()), m_cell(cell), m_info(info),
    m_activate(false), m_buttonWidth(0), m_enabled(true)
{
    setZIndex(cell->zIndex() + 1);
    setId(GridHitInfo::CellButton);
    setVisibility(false);
}

GridCellButtonInfo* GridTableCellButtonGraphic::info() const
{
    return m_info;
}

QIcon GridTableCellButtonGraphic::getIcon() const
{
    QIcon ret;
    int iconSize = qMin(boundedRect().width(), boundedRect().height());
    iconSize = qRound(iconSize * 0.5);
    switch (info()->kind)
    {
    case ChoiceButton:
    {
        QPixmap pixmap = qtn_get_thememanager(control())->getIcon(GridThemeManager::ChoiceButtonIcon, QSize(iconSize, iconSize));
        ret.addPixmap(pixmap);
    }
    break;
    case SelectButton:
    {
        QPixmap pixmap = qtn_get_thememanager(control())->getIcon(GridThemeManager::SelectButtonIcon, QSize(iconSize, iconSize));
        ret.addPixmap(pixmap);
    }
    break;
    case ClearButton:
    {
        QPixmap pixmap = qtn_get_thememanager(control())->getIcon(GridThemeManager::ClearButtonIcon, QSize(iconSize, iconSize));
        ret.addPixmap(pixmap);
    }
    break;
    case TextButton:
    {
        QPixmap pixmap = qtn_get_thememanager(control())->getIcon(GridThemeManager::TextButtonIcon, QSize(iconSize, iconSize));
        ret.addPixmap(pixmap);
    }
    break;
    default:
        ret = info()->icon;
        break;
    }
    return ret;
}

void GridTableCellButtonGraphic::paintEvent(QPainter& painter)
{
    QStyleOptionToolButton opt;
    opt.initFrom(control());
    opt.direction = control()->layoutDirection();
    opt.rect = boundedRect();
    opt.palette = control()->palette();
    opt.font = qtn_font_apply_scale(m_cell->m_view->optionsref()->cellFont(), m_cell->m_view->optionsref()->zoomFactor());
    opt.fontMetrics = QFontMetrics(opt.font);

    opt.iconSize = boundedRect().adjusted(2, 2, -2, 2).size();
    opt.icon = getIcon();

    opt.state = QStyle::State_None;
    if (m_enabled)
    {
        opt.state |= QStyle::State_Enabled;
        opt.state |= QStyle::State_Active;
    }

    opt.features = QStyleOptionToolButton::None;
    opt.subControls = QStyle::SC_ToolButton;
    opt.activeSubControls = QStyle::SC_None;
    opt.text = info()->titleText;
    if (opt.text.isEmpty())
        opt.toolButtonStyle = Qt::ToolButtonIconOnly;
    else
        opt.toolButtonStyle = Qt::ToolButtonTextBesideIcon;

    if (m_cell->m_view->optionsref()->cellButtonAutoRaise())
        opt.state |= QStyle::State_AutoRaise;

    if (isLeftButtonPressed())
    {
        opt.state |= QStyle::State_Sunken;
        opt.activeSubControls |= QStyle::SC_ToolButton;
    }
    else
        opt.state |= QStyle::State_Raised;

    if (isMouseOver())
        opt.state |= QStyle::State_MouseOver;
    else
        opt.state &= ~QStyle::State_MouseOver;

    control()->style()->drawComplexControl(QStyle::CC_ToolButton, &opt, &painter, 0);
}

void GridTableCellButtonGraphic::mousePressEvent(const QPoint& pos, QMouseEvent* event)
{
    Q_UNUSED(pos);
    Q_UNUSED(event);
    update();
}

void GridTableCellButtonGraphic::mouseReleaseEvent(const QPoint& pos, QMouseEvent* event)
{
    Q_UNUSED(pos);
    Q_UNUSED(event);

    if (m_enabled && isLeftButtonPressed() && isMouseOver())
    {
        CellButtonClickEventArgs args(
            m_cell->m_buttons.indexOf(this),
            static_cast<GridTableColumn *>(m_cell->m_view->getColumn(m_cell->columnIndex())),
            m_cell->gridRow());
        emit args.column()->buttonClicked(&args);
        m_cell->updateButtonLayout();
    }
    update();
}

void GridTableCellButtonGraphic::mouseMoveEvent(const QPoint& /*pos*/, QMouseEvent* /*event*/)
{
}

void GridTableCellButtonGraphic::mouseOverEvent(const QPoint& /*pos*/)
{
    m_cell->updateButtonLayout();
    parentGP()->update();
}

void GridTableCellButtonGraphic::mouseOutEvent()
{
    m_cell->updateButtonLayout();
    parentGP()->update();
}

bool GridTableCellButtonGraphic::setActivate(bool b, bool immediate)
{
    m_activate = b;

    if (m_activate)
    {
        m_buttonWidth = immediate ? info()->getButtonWidth() : qMin(info()->getButtonWidth(), m_buttonWidth + 2);
        if (m_buttonWidth == info()->getButtonWidth())
        {
            ScrollableGraphicPrimitive* sp = (ScrollableGraphicPrimitive *)m_cell->parentGP();
            sp->setFixedGPX(this, sp->isGPFixedX(m_cell));
            if (sp->mask().isGPMasked(m_cell))
                sp->mask().addGP(this);
            else
                mask().removeGP(this);
            setVisibility(m_cell->visibility());
        }
        return m_buttonWidth != info()->getButtonWidth();
    }
    else
    {
        setVisibility(false);
        m_buttonWidth = immediate ? 0 : qMax(0, m_buttonWidth - 2);
        return m_buttonWidth != 0;
    }
}

bool GridTableCellButtonGraphic::isActivate() const
{
    return m_activate;
}

int GridTableCellButtonGraphic::buttonWidth() const
{
    return m_buttonWidth;
}

/* GridFieldChooserPanel */
GridFieldChooserPanel::GridFieldChooserPanel(GridFieldChooserPrimitive* parent, GridFieldChooserProvider* provider)
    : ScrollableGraphicPrimitive(parent), m_provider(provider)
{
    setUpdateOnScrolling(true);
    setOverflowY(gsoAuto);
}

void GridFieldChooserPanel::paintEvent(QPainter& painter)
{
    Q_ASSERT(m_provider != Q_NULL);
    QFontMetrics metrics(getTitleFont());

    QColor textColor = qtn_get_thememanager(control())->shadePalette().light().color();
    painter.setPen(textColor);

    if (m_showEmptyMessage)
    {
        painter.setFont(getDescriptionFont());
        QTextOption opt(Qt::AlignTop | Qt::AlignLeft);
        opt.setWrapMode(QTextOption::WordWrap);
        painter.drawText(boundedRect().adjusted(QtnFieldChooserPanelIndent + metrics.height(), QtnFieldChooserPanelIndent, -QtnFieldChooserPanelIndent, -QtnFieldChooserPanelIndent), m_provider->description(), opt);
    }
    painter.setFont(getTitleFont());
    painter.rotate(-90);
    QTextOption opt;
    opt.setWrapMode(QTextOption::NoWrap);
    opt.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    QRect r = QRect(0, 0, -height(), QtnFieldChooserPanelIndent + metrics.height());
    painter.drawText(r, m_provider->title(), opt);
}

bool GridFieldChooserPanel::checkDragDropEvent(const QPoint& pos, GridDragDropObject* object)
{
    if (!fieldChooser()->isVisualOrderControlEnabled())
        return false;

    GridTableDragColumnObject* dragObject = qobject_cast<GridTableDragColumnObject *>(object);
    if (dragObject == Q_NULL)
        return false;
    GridDataTableColumn* column = Q_NULL;

    if (qobject_cast<GridTabColumnPrimitiveBase *>(dragObject->source()))
        column = qobject_cast<GridDataTableColumn *>(qobject_cast<GridTabColumnPrimitiveBase *>(dragObject->source())->column());

    if (column == Q_NULL)
        return false;

    return dragObject->checkDropFieldChooserTarget(pos, this, column);
}

QFont GridFieldChooserPanel::getTitleFont()
{
    QFont f = control()->font();
    f.setBold(true);
    return f;
}

QFont GridFieldChooserPanel::getDescriptionFont()
{
    QFont f = control()->font();
    return f;
}

GridFieldChooserProvider* GridFieldChooserPanel::provider() const
{
    return m_provider;
}

GridFieldChooserPrimitive* GridFieldChooserPanel::fieldChooser() const
{
    return static_cast<GridFieldChooserPrimitive *>(parentGP());
}

QSize GridFieldChooserPanel::updateColumnViewLayout()
{
    QSize max;
    int posY = 0;
    int posX = 20;
    m_showEmptyMessage = true;
    for (QHash<GridColumnBase *, GridTabColumnPrimitiveBase *>::const_iterator it = m_colGroups.constBegin(); it != m_colGroups.constEnd(); ++it)
    {
        (*it)->setVisibility(fieldChooser()->isVisualOrderControlEnabled() || !m_provider->view()->isColumnVisible((*it)->column()));
        
        if (fieldChooser()->isVisualOrderControlEnabled())
            static_cast<GridCardColumnTabPrimitive *>(it.value())->updateChecked();

        if (!(*it)->visibility())
            continue;
        m_showEmptyMessage = false;
        QSize s = (*it)->sizeHint();
        if (s.width() > max.width())
            max.rwidth() = s.width();
        if (s.height() > max.height())
            max.rheight() = s.height();
    }

    GridViewBase::GridColumns cols = m_provider->fields();

    //for (QSet<GridTabColumnPrimitiveBase *>::const_iterator it = m_colGroups.constBegin(); it != m_colGroups.constEnd(); ++it)
    for (GridViewBase::GridColumns::const_iterator it = cols.constBegin(); it != cols.constEnd(); ++it)
    {
        QHash<GridColumnBase *, GridTabColumnPrimitiveBase *>::iterator col_it = m_colGroups.find(*it);
        if (col_it == m_colGroups.end())
        {
            Q_ASSERT(false);
            continue;
        }

        GridTabColumnPrimitiveBase* columnPrimitive = col_it.value();
        if (!columnPrimitive->visibility())
            continue;

        columnPrimitive->setRect(QRect(posX, posY, max.width(), max.height()));
        columnPrimitive->updateLayout();
        posY += max.height();
    }

    max.rheight() = posY;
    setViewSize(max);

    QFontMetrics metrics0(getTitleFont());
    QFontMetrics metrics1(getDescriptionFont());
    max.rheight() = qMax(metrics0.width(m_provider->title()), max.height());
    QRect descrRect = metrics1.boundingRect(QRect(0, 0, 0, max.height()), Qt::TextWordWrap, m_provider->description()).adjusted(0, 0, 6, 4);
    max.rheight() = qMax(max.height(), descrRect.height());
    max.rwidth() = qMax(descrRect.width(), max.width());
    max.rwidth() += posX;

    QSize layoutSize = m_provider->view()->layout()->rect().size();
    int layoutHeihgt = layoutSize.height() - QtnFieldChooserButtonSize.height() - QtnFieldChooserPanelIndent * 5;
    if (max.height() > layoutHeihgt)
    {
        max.rheight() = layoutHeihgt;
        if (viewSize().height() > layoutHeihgt)
            max.rwidth() += scrollVer()->sizeHint().width();
    }

    return max;
}

void GridFieldChooserPanel::columnsUpdated()
{
    QHash<GridColumnBase *, GridTabColumnPrimitiveBase *> temp = m_colGroups;
    m_colGroups.clear();
    GridViewBase::GridColumns cols = m_provider->fields();

    while (temp.size() > 0)
    {
        QHash<GridColumnBase *, GridTabColumnPrimitiveBase *>::iterator it = temp.begin();
        GridTabColumnPrimitiveBase* gp = it.value();
        temp.erase(it);

        if (cols.contains(gp->column()))
        {
            m_colGroups.insert(gp->column(), gp);
            cols.removeOne(gp->column());
        }
        else
        {
            delete gp;
        }
    }

    for (GridViewBase::GridColumns::const_iterator it = cols.constBegin(); it != cols.constEnd(); ++it)
    {
        if (fieldChooser()->isVisualOrderControlEnabled())
            m_colGroups.insert(*it, new GridCardColumnTabPrimitive(this, *it));
        else
            m_colGroups.insert(*it, new GridTabColumnPrimitive(this, *it));
    }
}

void GridFieldChooserPanel::updateLayout()
{
    //setViewSize(QSize(0, viewSize().height()));
    ScrollableGraphicPrimitive::updateLayout();
    //for (QVector<GridTabColumnPrimitive *>::const_iterator it = m_colGroups.constBegin(); it != m_colGroups.constEnd(); ++it)
    //    (*it)->setWidth(boundedRect().width());
    //ScrollableGraphicPrimitive::updateLayout();
    //setViewSize(QSize(boundedRect().width(), viewSize().height()));
    //control()->geometryChanged();
}

/* GridShadowPrimitive */
GridShadowPrimitive::GridShadowPrimitive(GridWinShadowedPrimitive* primitive, GraphicPrimitive* parent)
    : GraphicPrimitive(parent), m_primitive(primitive)
{
    setTransparent(true);
    setBorder(50);
}

void GridShadowPrimitive::drawBorder(QPainter& painter)
{
    Q_UNUSED(painter);
}

void GridShadowPrimitive::paintEvent(QPainter& painter)
{
    QPixmap background = QPixmap(rect().size());
    background.fill(Qt::transparent);
    {
        QPainter pixmapPainter(&background);
        pixmapPainter.translate(borderLeft(), borderRight());
        m_primitive->paintBackground(pixmapPainter);
    }
    QGraphicsDropShadowEffect* e = new QGraphicsDropShadowEffect;
    e->setColor(QColor(40, 40, 40, 245));
    e->setOffset(0, 5);
    e->setBlurRadius(30);
    background = apply_effect_to_pixmap(background, e, 1);

    painter.drawPixmap(0, 0, background);
}

/* GridWinShadowedPrimitive */
GridWinShadowedPrimitive::GridWinShadowedPrimitive(GraphicPrimitive* parent)
    : GraphicPrimitive(parent)
{
    m_shadow = new GridShadowPrimitive(this, parent);
    ScrollableGraphicPrimitive::setFixedGPX(this, true);
    ScrollableGraphicPrimitive::setFixedGPY(this, true);
    ScrollableGraphicPrimitive::setFixedGPX(m_shadow, true);
    ScrollableGraphicPrimitive::setFixedGPY(m_shadow, true);
}

void GridWinShadowedPrimitive::resizeEvent(const QRect& rect)
{
    m_shadow->setRect(rect.adjusted(-m_shadow->borderLeft(), -m_shadow->borderTop(), m_shadow->borderRight(), m_shadow->borderBottom()));
    m_shadow->setZIndex(zIndex() - 1);
    m_shadow->setVisibility(visibility());
}

void GridWinShadowedPrimitive::setWinVisible(bool visible)
{
    setVisibility(visible);
    m_shadow->setVisibility(visible);
    m_shadow->update();
}

/* GridFieldChooserPrimitive */
GridFieldChooserPrimitive::GridFieldChooserPrimitive(GridViewBase* view, GraphicPrimitive* parent)
    : GridWinShadowedPrimitive(parent), m_paintSelected(false), m_visualOrderControlEnabled(false)
{
    m_panel0 = new GridFieldChooserPanel(this, view->m_fieldProviders[0]);
    if (view->m_fieldProviders.size() > 1)
        m_panel1 = new GridFieldChooserPanel(this, view->m_fieldProviders[1]);
    else
        m_panel1 = Q_NULL;
}

void GridFieldChooserPrimitive::columnsUpdated()
{
    m_panel0->columnsUpdated();
    if (m_panel1 != Q_NULL)
        m_panel1->columnsUpdated();
}

QSize GridFieldChooserPrimitive::sizeHint()
{
    updateSizeHint();
    return m_sizeHint;
}

void GridFieldChooserPrimitive::setFieldChooserVisible(bool visible)
{
    if (!m_panel0->provider()->view()->optionsref()->isFieldChooserEnabled())
        return;

    setWinVisible(visible);
    updateLayout();
}

bool GridFieldChooserPrimitive::isFieldChooserVisible() const
{
    return visibility();
}

void GridFieldChooserPrimitive::paintBackground(QPainter& painter)
{
    Q_ASSERT(visibility());
    QRect r = boundedRect();

    painter.setBrush(QBrush(qtn_get_thememanager(control())->shadeBackground()));
    if (m_paintSelected)
        painter.setPen(qtn_get_thememanager(control())->shadePalette().light().color());
    else
        painter.setPen(qtn_get_thememanager(control())->shadePalette().background().color());

    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath roundRectPath;
    const int radius = 5;

    int tabdecorationheight = QtnFieldChooserButtonSize.height() + QtnFieldChooserPanelIndent * 2;
    roundRectPath.moveTo(r.left() + r.width() - radius, r.top()); //Right-top
    roundRectPath.arcTo(r.left() + r.width() - radius * 2, r.top(), radius * 2, radius * 2, 90, -90);
    roundRectPath.lineTo(r.left() + r.width(), r.top() + r.height() - radius);
    roundRectPath.arcTo(r.left() + r.width() - radius * 2, r.top() + r.height() - radius * 2, radius * 2, radius * 2, 0, -90);

    roundRectPath.lineTo(r.left() + radius, r.top() + r.height()); //Left-bottom
    roundRectPath.arcTo(r.left(), r.top() + r.height() - radius * 2, radius * 2, radius * 2, -90, -90);

    roundRectPath.lineTo(r.left(), r.top() + tabdecorationheight + radius);
    roundRectPath.arcTo(r.left(), r.top() + tabdecorationheight, radius * 2, radius * 2, -180, -90);

    roundRectPath.lineTo(r.left() + r.width() - QtnFieldChooserButtonSize.width() - QtnFieldChooserPanelIndent * 2 - radius, r.top() + tabdecorationheight);

    roundRectPath.arcTo(r.left() + r.width() - QtnFieldChooserButtonSize.width() - QtnFieldChooserPanelIndent * 2 - radius * 2,
        r.top() + tabdecorationheight - 2 * radius,
        radius * 2, radius * 2, -90, 90);

    roundRectPath.lineTo(r.left() + r.width() - QtnFieldChooserButtonSize.width() - QtnFieldChooserPanelIndent * 2, r.top() + radius);
    roundRectPath.arcTo(r.left() + r.width() - QtnFieldChooserButtonSize.width() - QtnFieldChooserPanelIndent * 2,
        r.top(), radius * 2, radius * 2, -180, -90);

    roundRectPath.lineTo(r.left() + r.width() - radius, r.top());
    roundRectPath.closeSubpath();
    painter.drawPath(roundRectPath);
}

bool GridFieldChooserPrimitive::isVisualOrderControlEnabled() const
{
    return m_visualOrderControlEnabled;
}

void GridFieldChooserPrimitive::setVisualOrderControlEnabled(bool enabled)
{
    m_visualOrderControlEnabled = enabled;
}

void GridFieldChooserPrimitive::showDropTargetArea(bool show)
{
    if (m_paintSelected == show)
        return;
    m_paintSelected = show;
    m_shadow->update();
}

void GridFieldChooserPrimitive::updateLayout()
{
    GridWinShadowedPrimitive::updateLayout();
}

void GridFieldChooserPrimitive::updateSizeHint()
{
    QSize viewSize0 = m_panel0->updateColumnViewLayout();
    if (m_panel1 == Q_NULL)
    {
        m_panel0->setRect(QRect(QtnFieldChooserPanelIndent, QtnFieldChooserButtonSize.height() + QtnFieldChooserPanelIndent * 3, viewSize0.width(), viewSize0.height()));
        m_sizeHint.rwidth() = m_panel0->width();
        m_sizeHint.rheight() = m_panel0->height();
    }
    else
    {
        QSize viewSize1 = m_panel1->updateColumnViewLayout();

        m_panel1->setRect(QRect(QtnFieldChooserPanelIndent, QtnFieldChooserButtonSize.height() + QtnFieldChooserPanelIndent * 3, viewSize1.width(), qMax(viewSize0.height(), viewSize1.height())));
        m_panel0->setRect(QRect(QtnFieldChooserPanelIndent + viewSize1.width(), QtnFieldChooserButtonSize.height() + QtnFieldChooserPanelIndent * 3, viewSize0.width(), qMax(viewSize0.height(), viewSize1.height())));
        m_sizeHint.rwidth() = m_panel0->width() + m_panel1->width();
        m_sizeHint.rheight() = qMax(m_panel0->height(), m_panel1->height());
    }
}

/* GridFieldChooserButtonPrimitive */
GridFieldChooserButtonPrimitive::GridFieldChooserButtonPrimitive(GridViewBase* view, GraphicPrimitive* parent)
    : GridShadeButtonPrimitive(parent), m_view(view), m_fieldChooser(Q_NULL)
{
    setZIndex(m_view->layout()->header()->zIndex() + 100);
    m_fieldChooser = new GridFieldChooserPrimitive(m_view, m_view->layout());
    m_fieldChooser->setVisibility(false);
    m_fieldChooser->setZIndex(zIndex() - 1);
    setIcon(QPixmap(":res/grid_fieldchooser_button_icon.png"));
}

GridFieldChooserPrimitive* GridFieldChooserButtonPrimitive::fieldChooser()
{
    return m_fieldChooser;
}

void GridFieldChooserButtonPrimitive::paintEvent(QPainter& painter)
{
    GridShadeButtonPrimitive::paintEvent(painter);
}

void GridFieldChooserButtonPrimitive::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    if (checkClickEvent(e))
    {
        if (m_view->isFieldChooserVisible())
            m_view->hideFieldChooser();
        else
            m_view->showFieldChooser();
    }
    update();
}

QPalette GridFieldChooserButtonPrimitive::getButtonPalette() const
{
    QPalette retval = GridShadeButtonPrimitive::getButtonPalette();
    retval.setColor(QPalette::Text, Qt::black);
    return retval;
}


/* GridTableGroupPanelPrimitive */
GridTableGroupPanelPrimitive::GridTableGroupPanelPrimitive(GraphicPrimitive* parent, GridViewBase* view)
    : GraphicPrimitive(parent), m_view(view)
{
    setId(GridHitInfo::GroupPanel);
    m_fieldChooserButton = new GridFieldChooserButtonPrimitive(view, this);
}

GridFieldChooserPrimitive* GridTableGroupPanelPrimitive::fieldChooser() const
{
    return m_fieldChooserButton->fieldChooser();
}

GridViewBase* GridTableGroupPanelPrimitive::view() const
{
    return m_view;
}

bool GridTableGroupPanelPrimitive::startDragEvent(const QPoint& /*p*/)
{
    return true;
}

void GridTableGroupPanelPrimitive::insertColumn(int groupIndex, GridColumnBase* columnBase)
{
    for (QList<GridTabColumnPrimitive *>::iterator it = m_columnsGPs.begin(); it != m_columnsGPs.end(); ++it)
    {
        if ((*it)->column() == columnBase)
        {
            Q_ASSERT(false);
            return;
        }
    }

    GridTabColumnPrimitive* cgp = new GridTabColumnPrimitive(this, columnBase);
    m_columnsGPs.insert(groupIndex, cgp);
}

void GridTableGroupPanelPrimitive::removeColumnAt(int groupIndex)
{
    if (groupIndex < 0 || groupIndex >= m_columnsGPs.size())
        return;
    GridTabColumnPrimitive* p = m_columnsGPs.takeAt(groupIndex);
    delete p;
}

void GridTableGroupPanelPrimitive::moveColumn(int oldGroupIndex, int groupIndex)
{
    m_columnsGPs.move(oldGroupIndex, groupIndex);
}

GridTabColumnPrimitive* GridTableGroupPanelPrimitive::getGraphicPrimitiveForColumn(GridColumnBase* columnBase)
{
    for (QList<GridTabColumnPrimitive *>::iterator it = m_columnsGPs.begin(); it != m_columnsGPs.end(); ++it)
    {
        if ((*it)->column() == columnBase)
            return (*it);
    }
    return Q_NULL;
}

void GridTableGroupPanelPrimitive::paintEvent(QPainter& painter)
{
#if 0
    QBrush brush = view()->optionsref()->groupsHeaderBrush();
    QRect boundedRectangle = boundedRect();

    if (brush == QBrush() && view()->optionsref()->isWindowsAirSupported())
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
#endif

    if (m_columnsGPs.size() == 0)
    {
        painter.setPen(view()->optionsref()->groupsHeaderTextColor());
        QRect trect = QRect(5, 0, rect().width() - 5, rect().height());
        QString text = painter.fontMetrics().elidedText(view()->optionsref()->groupsHeaderText(), Qt::ElideRight, trect.width());
        //if (airSupported())
        //    drawDwmCaptionText(&painter, trect, text, false);
        //else
        draw_text(painter, text, trect, Qt::AlignLeft | Qt::AlignVCenter);
    }

    if (m_columnsGPs.size() > 0)
    {
        painter.setPen(Qt::black);
        for (QList<GridTabColumnPrimitive *>::const_iterator it = m_columnsGPs.constBegin();
            it != m_columnsGPs.constEnd(); ++it)
        {
            if (it + 1 == m_columnsGPs.end())
                break;
            GridTabColumnPrimitive* element = *it;
            painter.drawLine(
                element->left() + element->width() - 5, element->top() + element->height(), 
                element->left() + element->width() - 5, element->top() + element->height() + 3);
            painter.drawLine(
                element->left() + element->width() - 5, element->top() + element->height() + 3, 
                element->left() + element->width() + 5, element->top() + element->height() + 3);
        }
    }
}

bool GridTableGroupPanelPrimitive::checkDragDropEvent(const QPoint& pos, GridDragDropObject* object)
{
    GridTableDragColumnObject* dragObject = qobject_cast<GridTableDragColumnObject *>(object);
    if (dragObject == Q_NULL)
        return false;
    GridDataTableColumn* column = Q_NULL;
    
    if (qobject_cast<GridColumnPrimitive *>(dragObject->source()))
        column = qobject_cast<GridDataTableColumn *>(qobject_cast<GridColumnPrimitive *>(dragObject->source())->column());

    if (qobject_cast<GridTabColumnPrimitiveBase *>(dragObject->source()))
        column = qobject_cast<GridDataTableColumn *>(qobject_cast<GridTabColumnPrimitiveBase *>(dragObject->source())->column());
    
    if (column == Q_NULL)
        return false;

    if (!static_cast<GridTableView *>(view())->checkGrouping(column))
        return false;
    return dragObject->checkDropHeaderTarget(pos, this, column);
}

void GridTableGroupPanelPrimitive::resizeEvent(const QRect& rect)
{
    Q_UNUSED(rect);
    updateFieldChooserLayout();
}

void GridTableGroupPanelPrimitive::updateFieldChooserLayout()
{
    QRect bounded = boundedRect();
    m_fieldChooserButton->setRect(QRect(QPoint(bounded.width() - QtnFieldChooserButtonSize.width() - 10, QtnFieldChooserPanelIndent + 2), QtnFieldChooserButtonSize));
    m_fieldChooserButton->setVisibility(view()->optionsref()->isFieldChooserEnabled());
    if (m_fieldChooserButton->fieldChooser())
    {
        QPoint p = m_fieldChooserButton->mapToSite(QPoint(0, 0));
        p = view()->layout()->mapFromSite(p);

        QSize fieldChooserSize = m_fieldChooserButton->fieldChooser()->sizeHint();

        fieldChooserSize.rwidth() += QtnFieldChooserPanelIndent * 2;
        fieldChooserSize.rheight() += QtnFieldChooserPanelIndent * 2;

        m_fieldChooserButton->fieldChooser()->setRect(QRect(
            p.x() - fieldChooserSize.width() + QtnFieldChooserButtonSize.width() + QtnFieldChooserPanelIndent - m_fieldChooserButton->fieldChooser()->borderLeft(),
            p.y() - QtnFieldChooserPanelIndent - m_fieldChooserButton->fieldChooser()->borderTop(),
            fieldChooserSize.width() + m_fieldChooserButton->fieldChooser()->borderLeft() + m_fieldChooserButton->fieldChooser()->borderRight(),
            QtnFieldChooserButtonSize.width() + QtnFieldChooserPanelIndent * 2 +
            fieldChooserSize.height() + m_fieldChooserButton->fieldChooser()->borderTop() + m_fieldChooserButton->fieldChooser()->borderBottom()));
        m_fieldChooserButton->fieldChooser()->updateLayout();
    }
}

QSize GridTableGroupPanelPrimitive::sizeHint()
{ 
    return m_sizeHint;
}

void GridTableGroupPanelPrimitive::updateLayout()
{
    unsigned int headerHeight = 0;
    unsigned int headerLeft = 0;
    if (visibility())
    {
        const GridViewBase::GridColumns& cols = view()->groups();
        for (GridViewBase::GridColumns::const_iterator it = cols.constBegin(); it != cols.constEnd(); ++it)
        {
            GridTabColumnPrimitive* cgp = getGraphicPrimitiveForColumn(*it);
            Q_ASSERT(cgp != Q_NULL);
            if (cgp == Q_NULL)
                continue;
            cgp->setRect(QRect(QPoint(0, 0), cgp->sizeHint()));
            headerHeight += headerHeight == 0 ? cgp->height() : kGridDefHeaderHeightIndent;
        }

        // update glyphs layout
        int ypos = kGridDefHeaderHeightIndent;

        //for (QList<GridHeaderColumnTabGraphic *>::const_iterator it = m_groupPanelGP->colElements().constBegin(); 
        //    it != m_groupPanelGP->colElements().constEnd(); ++it)
        //{
        //    headerHeight += headerHeight == 0 ? (*it)->height() : kGridDefHeaderHeightIndent;
        //}

        if (((int)kGridDefHeaderHeight - (int)headerHeight) / 2 >= (int)kGridDefHeaderHeightIndent)
        {
            ypos = (kGridDefHeaderHeight - headerHeight) / 2;
            headerHeight = kGridDefHeaderHeight;
        }
        else
            headerHeight += kGridDefHeaderHeightIndent * 2;

        headerLeft = kGridDefHeaderWidthIndent;

        for (GridTableView::GridColumns::const_iterator it = cols.constBegin(); it != cols.constEnd(); ++it)
        {
            GridTabColumnPrimitive* cgp = getGraphicPrimitiveForColumn(*it);
            Q_ASSERT(cgp != Q_NULL);
            if (cgp == Q_NULL)
                continue;
            QRect r(headerLeft, ypos, cgp->width(), cgp->height());
            cgp->setRect(r);
            cgp->updateLayout();
            ypos += kGridDefHeaderHeightIndent;
            headerLeft += cgp->width() + kGridDefHeaderWidthIndent;
        }
    }
    m_sizeHint.setWidth(0); // 0 - means that primitive will be aligned to all width of the parent in void GridViewLayoutItemPrimitive::updateChildren().
    m_sizeHint.setHeight(headerHeight);
    m_rect = QRect(0, 0, 0, 0);
}

/* GridFindPanelCloseButtonPrimitive */
GridFindPanelCloseButtonPrimitive::GridFindPanelCloseButtonPrimitive(GraphicPrimitive* parent, GridViewBase* view)
    : GridCloseButtonPrimitive(parent), m_view(view)
{
}

void GridFindPanelCloseButtonPrimitive::clicked()
{
    m_view->hideFindPanel();
}

/* FindFilterCondition */
FindFilterCondition::FindFilterCondition(GridFilter* filter, GridFindPanelPrimitive* panel)
    : GridFilterCondition(filter), m_panel(panel) {}

bool FindFilterCondition::isTrue(int modelRow)
{
    if (m_panel->selectedText().isEmpty())
        return true;

    for (int i = 0; i < m_panel->m_columnList.size(); ++i)
    {
        QVariant v;
        GridModelDataBinding* binding = m_panel->m_view->getDataBinding(m_panel->m_columnList[i]);
        Q_ASSERT(binding != Q_NULL);
        if (!binding->getValue(modelRow, Qt::DisplayRole, v))
            continue;

        if (m_panel->checkValue(v))
            return true;
    }

    return false;
}

GridFilterCondition* FindFilterCondition::clone() const
{
    FindFilterCondition* retval = new FindFilterCondition(m_filter, m_panel);
    return retval;
}

QString FindFilterCondition::createPresentation() const
{
    QString retval = "find filter: row in ()";
    return retval;
}

int FindFilterCondition::conditionCount() const
{
    return m_panel->m_columnList.size();
}

/* GridFindPanelPrimitive */
GridFindPanelPrimitive::GridFindPanelPrimitive(GraphicPrimitive* parent, GridViewBase* view)
    : GraphicPrimitive(parent), m_view(view), m_panelVisible(false), m_oldRow(view->modelController()), m_caseSensitivity(Qt::CaseInsensitive)
{
    m_textEditPrimitive = new GridLineEditPrimitive(this);
    connect(m_textEditPrimitive->widget(), SIGNAL(textEdited(const QString&)), this, SLOT(textEdited(const QString&)));
    m_textEditPrimitive->setPlaceholderText(Grid::tr_compatible(QtnGridFindPlaceholderTextString));
   
    m_backButton = new GridHistoryItemButton(this);
    m_backButton->setText(Grid::tr_compatible(QtnGridBackString));
    connect(m_backButton, SIGNAL(clicked()), this, SLOT(backClicked()));

    m_forwardButton = new GridHistoryItemButton(this);
    m_forwardButton->setText(Grid::tr_compatible(QtnGridForwardString));
    connect(m_forwardButton, SIGNAL(clicked()), this, SLOT(forwardClicked()));

    m_caseSensitivityCheckBox = new GridCheckBoxPrimitive(this);
    m_caseSensitivityCheckBox->setText(Grid::tr_compatible(QtnGridCaseSensitiveString));
    connect(m_caseSensitivityCheckBox->widget(), SIGNAL(clicked(bool)), this, SLOT(caseSensitiveClicked(bool)));
    
    m_caseSensitivityCheckBox->setChecked(m_caseSensitivity == Qt::CaseSensitive);
    
    m_filterCheckBox = new GridCheckBoxPrimitive(this); //filter unnecessary
    m_filterCheckBox->setText(Grid::tr_compatible(QtnGridFiltrationString));
    connect(m_filterCheckBox->widget(), SIGNAL(clicked(bool)), this, SLOT(filterClicked(bool)));

    m_closeButtonPrimitive = new GridFindPanelCloseButtonPrimitive(this, view);
    m_effectTimer = new QTimer(this);
    m_effectTimer->setSingleShot(true);
    connect(m_effectTimer, SIGNAL(timeout()), this, SLOT(updateLayoutTimeout()));
    m_effectHeight = 0;

    updateOptions();
}

GridFindPanelPrimitive::~GridFindPanelPrimitive()
{
    m_oldRow.clear();
}

void GridFindPanelPrimitive::updateOptions()
{
    m_caseSensitivityCheckBox->widget()->setFont(control()->font());
    m_filterCheckBox->widget()->setFont(control()->font());

    QPalette p = m_caseSensitivityCheckBox->widget()->palette();
    p.setColor(QPalette::WindowText, m_view->optionsref()->groupsHeaderTextColor());
    m_caseSensitivityCheckBox->widget()->setPalette(p);
    m_filterCheckBox->widget()->setPalette(p);
}

bool GridFindPanelPrimitive::isPanelVisible() const
{
    return m_panelVisible;
}

void GridFindPanelPrimitive::setPanelVisible(bool visible, bool cancel)
{
    if (visible && !m_view->optionsref()->isFindEnabled())
        return;

    if (visible)
        m_textEditPrimitive->setFocus();

    if (m_panelVisible == visible)
        return;

    m_panelVisible = visible;
    if (m_panelVisible)
    {
        if (!m_effectTimer->isActive())
            m_effectStartTime = QTime();
        else if (m_effectStartTime == QTime())
        {
            m_effectStartTime = QTime::currentTime();
            m_effectTimer->stop();
            return;
        }

        m_effectDuration = 150;
        m_effectTimer->start(10);
    }
    else
    {
        m_effectDuration = 150;
        if (!m_effectTimer->isActive())
            m_effectStartTime = QTime();
        m_effectTimer->start(10);
    }

    if (m_panelVisible)
    {
        m_oldRow.set(m_view->focusedRow());
        m_oldRow.setColumnIndex(m_view->focusedColumnIndex());
    }
    else
    {
        clear();
        if (cancel)
        {
            GridRow* focusRow = m_oldRow.row();
            if (focusRow != Q_NULL)
                m_view->setCellFocus(focusRow->rowIndex(), m_oldRow.columnIndex());
        }
    }
}

int GridFindPanelPrimitive::sizeHintForHeight()
{
    int ret = qMax(m_backButton->sizeHint().height(), m_textEditPrimitive->sizeHint().height());
    return ret + filterPanelIndent * 2;
}

const QString& GridFindPanelPrimitive::selectedText() const
{
    return m_selectedText;
}

Qt::CaseSensitivity GridFindPanelPrimitive::selectCaseSensitivity() const
{
    return m_caseSensitivity;
}

bool GridFindPanelPrimitive::hasEditFocus() const
{
    return isPanelVisible() && m_textEditPrimitive->hasFocus();
}

void GridFindPanelPrimitive::clear()
{
    m_textEditPrimitive->setText("");
    m_selectedText = "";
    m_filterCheckBox->setChecked(false);
    m_caseSensitivityCheckBox->setChecked(false);
    m_view->modelController()->findFilter()->clear();
    m_view->modelController()->findFilter()->setActive(false);
}

void GridFindPanelPrimitive::filterClicked(bool checked)
{
    if (checked)
    {
        if (!m_view->modelController()->findFilter()->isActive())
        {
            GridFilterGroupCondition* groupCondition = new GridFilterGroupCondition(m_view->modelController()->findFilter());
            FindFilterCondition* condition = new FindFilterCondition(m_view->modelController()->findFilter(), this);
            groupCondition->addCondition(condition);
            m_view->modelController()->findFilter()->setCondition(groupCondition, false);
            m_view->modelController()->findFilter()->setActive(true);
        }
    }
    else
    {
        m_view->modelController()->findFilter()->clear();
        m_view->modelController()->findFilter()->setActive(false);
    }
}

QVector<GridColumnBase *> GridFindPanelPrimitive::createFindColumnList()
{
    QVector<GridColumnBase *> ret;
    GridDataTableColumn* column = m_view->getFirstColumn();
    while (column != Q_NULL)
    {
        if (m_view->optionsref()->findColumnList().isEmpty() || 
            m_view->optionsref()->findColumnList().contains(column->dataBinding()->columnName()))
        {
            ret.append(column);
        }
        column = static_cast<GridTableView *>(m_view)->getNextColumn(column);

    }
    return ret;

}

bool GridFindPanelPrimitive::getNextCell(int& rowIndex, int& columnIndex, bool forward)
{
    GridRow* row = m_view->modelController()->getGridRow(rowIndex);
    if (row == Q_NULL)
        return false;

    if (forward)
    {
        ++columnIndex;

        if (row->type() != GridRow::DataRow || columnIndex > m_columnList.size() - 1)
        {
            columnIndex = 0;
            ++rowIndex;
            if (rowIndex > m_view->modelController()->getGridRowCount() - 1)
                rowIndex = 0;
        }
    }
    else
    {
        --columnIndex;
        if (row->type() != GridRow::DataRow || columnIndex < 0)
        {
            columnIndex = m_columnList.size() - 1;
            --rowIndex;
            if (rowIndex < 0)
                rowIndex = m_view->modelController()->getGridRowCount() - 1;
        }
    }

    if (row->type() != GridRow::DataRow)
        columnIndex = 0;

    return true;
}

int GridFindPanelPrimitive::getColumnListIndex(int columnIndex)
{
    for (int i = 0; i < m_columnList.size(); ++i)
    {
        if (m_columnList[i]->index() == columnIndex)
            return i;
    }
    return -1;
}

bool GridFindPanelPrimitive::checkValue(const QVariant& value)
{
    return value.toString().indexOf(m_selectedText, 0, m_caseSensitivity) != -1;
}

void GridFindPanelPrimitive::findNext(const QString& text, bool forward)
{
    if (!isPanelVisible())
        return;

    m_columnList = createFindColumnList();

    bool newsearch = m_selectedText != text;
    if (newsearch)
        forward = true;

    m_selectedText = text;

    if (m_view->modelController()->getGridRowCount() == 0 || m_columnList.size() == 0)
        return;

    int startRowIndex = 0;
    int startColumnIndex = 0;
    int rowIndex = 0;
    int columnIndex = 0;

    if (!newsearch)
    {
        if (m_view->modelController()->focusedRow() == Q_NULL)
            return;
        startRowIndex = m_view->modelController()->focusedRow()->rowIndex();
        startColumnIndex = getColumnListIndex(m_view->modelController()->focusedColumnIndex());
        rowIndex = startRowIndex;
        columnIndex = startColumnIndex;
        if (!getNextCell(rowIndex, columnIndex, forward))
            return;

    }

    while (1)
    {
        GridRow* row = m_view->modelController()->getGridRow(rowIndex);
        QVariant v;
        if (row->type() == GridRow::DataRow)
        {
            GridModelDataBinding* binding = m_view->getDataBinding(m_columnList[columnIndex]);
            Q_ASSERT(binding != Q_NULL);
            if (!binding->getValue(row->modelRowIndex(), Qt::DisplayRole, v))
                v = QVariant();
        }
        else
        {
            row->groupValue(v);
        }

        if (checkValue(v))
        {
            static_cast<GridTableView *>(m_view)->setCellFocus(row->rowIndex(), m_columnList[columnIndex]->index());
            return;
        }

        if (!getNextCell(rowIndex, columnIndex, forward))
            break;

        if (rowIndex == startRowIndex && columnIndex == startColumnIndex)
            break;
    }
}

void GridFindPanelPrimitive::caseSensitiveClicked(bool checked)
{
    m_caseSensitivity = checked ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QString text = m_selectedText;
    m_selectedText = "";
    textEdited(text);
}


void GridFindPanelPrimitive::textEdited(const QString& text)
{
    findNext(text, true);
    if (m_view->modelController()->findFilter()->isActive())
        m_view->modelController()->refresh();
    m_view->layout()->rowsPanel()->update();
}

void GridFindPanelPrimitive::backClicked()
{
    findNext(m_selectedText, false);
}

void GridFindPanelPrimitive::forwardClicked()
{
    findNext(m_selectedText, true);
}

void GridFindPanelPrimitive::updateLayoutTimeout()
{
    if (m_effectStartTime == QTime())
        m_effectStartTime = QTime::currentTime();

    int height = sizeHintForHeight();
    int diff = m_effectStartTime.msecsTo(QTime::currentTime());
    if (m_effectDuration == 0 || diff > m_effectDuration)
    {
        if (m_panelVisible)
        {
            m_effectHeight = sizeHintForHeight();
            setOpacity(1);
        }
        else
        {
            m_effectHeight = 0;
            setOpacity(0);
        }
    }
    else
    {
        m_effectTimer->start(10);

        double f = ((double)diff / (double)m_effectDuration);
        ;
        if (m_panelVisible)
        {
            m_effectHeight = f * height;
            setOpacity(f * 0.7);
        }
        else
        {
            m_effectHeight = height - f * height;
            setOpacity((1 - f) * 0.7);
        }
    }
    m_view->requestUpdateLayout();
}

void GridFindPanelPrimitive::paintEvent(QPainter& painter)
{
    GraphicPrimitive::paintEvent(painter);
}

QSize GridFindPanelPrimitive::sizeHint()
{
    return QSize(0, m_effectHeight);
}

void GridFindPanelPrimitive::resizeEvent(const QRect& rect)
{
    Q_UNUSED(rect);
    int h = sizeHintForHeight();

    QSize textSize = m_textEditPrimitive->sizeHint();
    QFontMetrics metrics(m_textEditPrimitive->widget()->font());
    textSize.rwidth() = qMin(350, metrics.width(m_textEditPrimitive->placeholderText()) + 10);

    QSize caseSensitiveSize = m_caseSensitivityCheckBox->sizeHint();
    QSize checkSize = m_filterCheckBox->sizeHint();
    QSize buttonSize = m_closeButtonPrimitive->sizeHint();
    QSize backSize = m_backButton->sizeHint();
    QSize forwardSize = m_forwardButton->sizeHint();
    backSize.rwidth() = qMax(backSize.width(), forwardSize.width());
    forwardSize.rwidth() = backSize.width();

    int xoffset = filterPanelIndent;
    
    m_textEditPrimitive->setRect(QRect(QPoint(xoffset, h / 2 - textSize.height() / 2), textSize));
    xoffset += textSize.width();
    xoffset += filterPanelIndent;
    
    m_backButton->setRect(QRect(QPoint(xoffset, h / 2 - backSize.height() / 2), backSize));
    xoffset += backSize.width();
    xoffset += filterPanelIndent;
    
    m_forwardButton->setRect(QRect(QPoint(xoffset, h / 2 - forwardSize.height() / 2), forwardSize));
    xoffset += forwardSize.width();
    xoffset += filterPanelIndent;

    m_caseSensitivityCheckBox->setRect(QRect(QPoint(xoffset, h / 2 - caseSensitiveSize.height() / 2), caseSensitiveSize));
    xoffset += caseSensitiveSize.width();
    xoffset += filterPanelIndent;

    m_filterCheckBox->setRect(QRect(QPoint(xoffset, h / 2 - checkSize.height() / 2), checkSize));

    m_closeButtonPrimitive->setRect(QRect(QPoint(width() - buttonSize.width() - filterPanelIndent, h / 2 - buttonSize.height() / 2), buttonSize));
}

/* GridCheckBoxPrimitive */
GridCheckBoxPrimitive::GridCheckBoxPrimitive(GraphicPrimitive* parent, bool nativeStyleUsed)
    : EmbedGraphicPrimitive(parent->control(), parent)
{
    CheckBoxWidget* box = new CheckBoxWidget();
    if (!nativeStyleUsed)
    {
        box->setNativeStyleEnabled(false);
        box->setMouseOverPaintEnabled(false);
    }
    box->setAttribute(Qt::WA_DontShowOnScreen);
    box->setAttribute(Qt::WA_OpaquePaintEvent);
    setWidget(box);
}

void GridCheckBoxPrimitive::setText(const QString& text)
{
    static_cast<QCheckBox *>(widget())->setText(text);
}

void GridCheckBoxPrimitive::setChecked(bool checked)
{
    static_cast<QCheckBox *>(widget())->setChecked(checked);
}

void GridCheckBoxPrimitive::mouseOverEvent(const QPoint& pos)
{
    EmbedGraphicPrimitive::mouseOverEvent(pos);
    static_cast<GridCheckBoxPrimitive *>(parentGP())->mouseOverEvent(pos);
}

void GridCheckBoxPrimitive::mouseOutEvent()
{
    EmbedGraphicPrimitive::mouseOutEvent();
    static_cast<GridCheckBoxPrimitive *>(parentGP())->mouseOutEvent();
}

#if 0
class QLineEdit2 : public QLineEdit
{
public:
    QLineEdit2(QWidget* parent = 0)
        : QLineEdit(parent)
    {
    }
protected:
    virtual void paintEvent(QPaintEvent* e)
    {
        QLineEdit::paintEvent(e);
    }
};
#endif

/* GridLineEditPrimitive */
GridLineEditPrimitive::GridLineEditPrimitive(GraphicPrimitive* parent)
    : EmbedGraphicPrimitive(parent->control(), parent)
{
    QLineEdit* edit = new QLineEdit();
    edit->setAttribute(Qt::WA_DontShowOnScreen);
    edit->setAttribute(Qt::WA_OpaquePaintEvent);
    setWidget(edit);
}

QString GridLineEditPrimitive::placeholderText() const
{
    return static_cast<QLineEdit *>(widget())->placeholderText();
}

void GridLineEditPrimitive::setPlaceholderText(const QString& text)
{
    static_cast<QLineEdit *>(widget())->setPlaceholderText(text);
}

QString GridLineEditPrimitive::text() const
{
    return static_cast<QLineEdit *>(widget())->text();
}

void GridLineEditPrimitive::setText(const QString& text)
{
    static_cast<QLineEdit *>(widget())->setText(text);
}

void GridLineEditPrimitive::mouseOverEvent(const QPoint& pos)
{
    EmbedGraphicPrimitive::mouseOverEvent(pos);
    static_cast<GridLineEditPrimitive *>(parentGP())->mouseOverEvent(pos);
}

void GridLineEditPrimitive::mouseOutEvent()
{
    EmbedGraphicPrimitive::mouseOutEvent();
    static_cast<GridLineEditPrimitive *>(parentGP())->mouseOutEvent();
}

void GridLineEditPrimitive::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    setFocus();
    EmbedGraphicPrimitive::mousePressEvent(point, e);
}

/* GridFilterPanelPrimitive */
GridFilterPanelPrimitive::GridFilterPanelPrimitive(GraphicPrimitive* parent, GridViewBase* view)
    : GraphicPrimitive(parent->control(), parent), m_view(view),
    m_activeCheckBox(Q_NULL), m_textButton(Q_NULL), m_autoHideButton(Q_NULL), m_closeButton(Q_NULL), m_historyPopup(Q_NULL)
{
    createPanel();
    m_historyPopup = new FilterHistoryPopupPrimitive(view->layout(), view->filter()->history());
    m_historyPopup->setZIndex(m_view->layout()->header()->zIndex() + 200);
    m_historyPopup->setWinVisible(false);
    ScrollableGraphicPrimitive::setFixedGPX(m_historyPopup, true);
    ScrollableGraphicPrimitive::setFixedGPY(m_historyPopup, true);

}

GridFilterPanelPrimitive::~GridFilterPanelPrimitive()
{
    Q_DELETE_AND_NULL(m_historyPopup);
}

void GridFilterPanelPrimitive::setFilterText(const QString& text)
{
    m_textButton->setText(text);
}

void GridFilterPanelPrimitive::setFilterChecked(bool checked)
{
    static_cast<QCheckBox *>(m_activeCheckBox->widget())->setChecked(checked);
}

void GridFilterPanelPrimitive::setFilterStatus(int rowCount, int filteredRowCount, int conditionCount)
{
    m_statusText = QString(Grid::tr_compatible(QtnGridFilterStatusText)).arg(rowCount).arg(filteredRowCount).arg(conditionCount);
}

void GridFilterPanelPrimitive::updateAutoHideIcon()
{
    QPixmap pixmap;
    if (m_view->optionsref()->filterAutoHide())
        pixmap = QPixmap(":res/grid_filter_autohide_on.png");
    else
        pixmap = QPixmap(":res/grid_filter_autohide_off.png");
    pixmap.setMask(pixmap.createMaskFromColor(Qt::magenta));
    m_autoHideButton->setIcon(pixmap);
}

void GridFilterPanelPrimitive::resizeEvent(const QRect& rect)
{
    Q_UNUSED(rect);
    QRect r = boundedRect();
    QSize textSize = m_textButton->sizeHint();
    QSize size = m_activeCheckBox->sizeHint();
    m_activeCheckBox->setRect(QRect(QPoint(filterPanelIndent, filterPanelIndent + (textSize.height() / 2 - size.height() / 2)), size));
    m_closeButton->setRect(QRect(QPoint(r.width() - filterPanelIndent - m_closeButton->width(), filterPanelIndent), QSize(textSize.height(), textSize.height())));
    m_autoHideButton->setRect(QRect(QPoint(m_closeButton->left() - filterPanelIndent - m_autoHideButton->width(), filterPanelIndent), QSize(textSize.height(), textSize.height())));
    m_textButton->setRect(QRect(QPoint(filterPanelIndent * 2 + m_activeCheckBox->width(), filterPanelIndent),
        QSize(m_autoHideButton->left() - m_activeCheckBox->rect().right() - 1 - filterPanelIndent * 2, textSize.height())));

    if (m_historyPopup->visibility())
    {
        const int indent = 5;
        QPoint pos = m_view->layout()->rect().bottomLeft();
        pos.ry() -= height();
 
        QSize sh = m_historyPopup->sizeHint();
        m_historyPopup->setRect(QRect(pos.x() + indent, pos.y() - sh.height() - indent * 2, qMin(qMin(900, width()), sh.width()) - indent * 2, sh.height()));
        m_historyPopup->updateLayout();
    }

    GridFilter* filter = m_view->modelController()->filter();
    setFilterText(filter->getPresentation(filter->condition()));
    setFilterChecked(filter->isActive());
    setFilterStatus(
        m_view->modelController()->getPureRowCount(),
        m_view->modelController()->getRecordCount(),
        filter->conditionCount());
}

void GridFilterPanelPrimitive::updateLayout()
{
    m_rect = QRect(0, 0, 0, 0);
}

QSize GridFilterPanelPrimitive::sizeHint()
{
    QSize row1size = m_textButton->sizeHint();
    QSize row2size = get_column_sizeHint(Q_NULL, control()->style(), control()->font(), 0.9);
    return QSize(0, filterPanelIndent * 3 + row1size.height() + row2size.height());
}

void GridFilterPanelPrimitive::createPanel()
{
    m_activeCheckBox = new GridCheckBoxPrimitive(this);
    connect(m_activeCheckBox->widget(), SIGNAL(clicked(bool)), this, SLOT(checkFilterActive(bool)));

    m_textButton = new GridShadeButtonPrimitive(this);
    m_textButton->setAutoRaise(true);
    m_autoHideButton = new GridShadeButtonPrimitive(this);
    m_closeButton = new GridShadeButtonPrimitive(this);
    QPixmap pixmap = QPixmap(":res/gridcross.png");
    pixmap.setMask(pixmap.createMaskFromColor(Qt::magenta));
    m_closeButton->setIcon(pixmap);
    connect(m_textButton, SIGNAL(clicked()), this, SLOT(filterTextClicked()));
    connect(m_autoHideButton, SIGNAL(clicked()), this, SLOT(autoHideClicked()));
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(closeFilterClicked()));
    updateAutoHideIcon();
}

void GridFilterPanelPrimitive::paintEvent(QPainter& painter)
{
#if 0
    QBrush brush = m_view->optionsref()->groupsHeaderBrush();
    QRect boundedRectangle = boundedRect();

    if (brush == QBrush() && m_view->optionsref()->isWindowsAirSupported())
    {
        QLinearGradient linearGrad(QPointF(10, -boundedRectangle.height()), QPointF(10, boundedRectangle.height() + 15));
        linearGrad.setColorAt(0, Qt::transparent);
        linearGrad.setColorAt(1, Qt::black);
        brush = linearGrad;

//        brush = create_background(Qt::transparent);
        painter.fillRect(boundedRectangle, brush);
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
            opt.rect = boundedRect();
            control()->style()->drawComplexControl(QStyle::CC_ScrollBar, &opt, &painter, control());
        }
    }
#endif

    QRect r = boundedRect();
    r.setTop(r.top() + m_textButton->height() + filterPanelIndent * 2);

    painter.setFont(qtn_font_apply_scale(control()->font(), 0.9));
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    QTextDocument doc;
    doc.setUndoRedoEnabled(false);
    doc.setHtml(m_statusText);
    doc.setTextWidth(r.width());
    doc.setUseDesignMetrics(true);
    doc.setDefaultTextOption(QTextOption(Qt::AlignRight));
    doc.setDefaultFont(qtn_font_apply_scale(control()->font(), 0.9));
    QPalette palette = control()->palette();

    QColor textColor = m_view->optionsref()->groupsHeaderTextColor();

    palette.setColor(QPalette::Text, textColor);
    qtn_QTextDocument_draw(painter, &doc, palette, r);
}

void GridFilterPanelPrimitive::mouseOverEvent(const QPoint& pos)
{
    m_view->layout()->showFooter();
    GraphicPrimitive::mouseOverEvent(pos);
}

void GridFilterPanelPrimitive::mouseOutEvent()
{
    m_view->layout()->hideFooter();
    GraphicPrimitive::mouseOutEvent();
}

void GridFilterPanelPrimitive::checkFilterActive(bool checked)
{
    if (checked == m_view->modelController()->filter()->isActive())
        return;
    GridViewAutoUpdater autoUpdater(m_view);
    m_view->modelController()->filter()->setActive(checked);
}

void GridFilterPanelPrimitive::setHistoryWindowVisible(bool visible)
{
    m_historyPopup->setWinVisible(visible);
    if (visible)
    {
        m_view->layout()->updateLayout();
        m_view->layout()->update();
    }
}

void GridFilterPanelPrimitive::filterTextClicked()
{
    setHistoryWindowVisible(!m_historyPopup->visibility());
}

void GridFilterPanelPrimitive::closeFilterClicked()
{
    GridViewAutoUpdater autoUpdater(m_view);
    m_view->hideFilterPanel();
    m_view->modelController()->filter()->clear();
    m_view->modelController()->filter()->setActive(false);
    m_view->requestUpdateLayout();
}

void GridFilterPanelPrimitive::autoHideClicked()
{
    //m_view->layout()->m_footerUpdateTimer->stop();
    m_view->optionsref()->setFilterAutoHide(!m_view->optionsref()->filterAutoHide());
}

/* GridTableColumnFilterButton */
GridTableColumnFilterButton::GridTableColumnFilterButton(GridDataTableColumn* column, GraphicPrimitive* parent)
: GraphicPrimitive(parent->control(), parent),
m_column(column)
{
    setId(GridHitInfo::ColumnFilterButton);
    setCursor(Qt::PointingHandCursor);
}

QSize GridTableColumnFilterButton::sizeHint()
{
    return QtnGridSmallToolButtonSize;
}

void GridTableColumnFilterButton::paintEvent(QPainter& painter)
{
    if (isMouseOver())
    {
        QPixmap over = qtn_get_thememanager(control())->getIcon(GridThemeManager::FilterToolButtonBoldIcon, boundedRect().size());
        painter.drawPixmap(boundedRect().topLeft(), over);
    }
    else if (parentGP()->isMouseOver() || m_column->m_filterCondition != Q_NULL)
    {
        QPixmap normal = qtn_get_thememanager(control())->getIcon(GridThemeManager::FilterToolButtonIcon, boundedRect().size());
        painter.drawPixmap(boundedRect().topLeft(), normal);
    }
}

void GridTableColumnFilterButton::mouseOverEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
    update();
}

void GridTableColumnFilterButton::mouseOutEvent()
{
    update();
}

void GridTableColumnFilterButton::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
    if (m_column->m_filterPopup == Q_NULL)
        m_column->m_filterPopup = new GridFilterColumnPopup(control(), m_column);
    QPoint pos = mapToGlobal(QPoint(0, height()));
#ifndef QT_NO_EFFECTS
    qFadeEffect(m_column->m_filterPopup);
#endif

    m_column->m_filterPopup->raise();
    m_column->m_filterPopup->show();

    int w = m_column->m_filterPopup->minimumWidth();
    if (w < 100)
        w = 100;

    pos.rx() -= w / 2;
    m_column->m_filterPopup->move(pos);

    m_column->m_filterPopup->resize(w, m_column->m_filterPopup->heightHint());
    m_column->m_filterPopup->updateMask();

    if (m_column->m_filterPopup->geometry().right() > QApplication::desktop()->width())
        m_column->m_filterPopup->move(QApplication::desktop()->width() - m_column->m_filterPopup->width(),
        m_column->m_filterPopup->y());

    if (m_column->m_filterPopup->geometry().bottom() > QApplication::desktop()->height())
        m_column->m_filterPopup->move(m_column->m_filterPopup->x(),
        QApplication::desktop()->height() - m_column->m_filterPopup->height());

    if (m_column->m_filterPopup->x() < 0)
        m_column->m_filterPopup->move(0, m_column->m_filterPopup->y());
    if (m_column->m_filterPopup->y() < 0)
        m_column->m_filterPopup->move(m_column->m_filterPopup->x(), 0);
}

/* GridTableColumnMenuButton */
GridTableColumnMenuButton::GridTableColumnMenuButton(GridColumnBase* column, GraphicPrimitive* parent)
    : GraphicPrimitive(parent), m_column(column)
{
    setCursor(Qt::PointingHandCursor);
}

void GridTableColumnMenuButton::paintEvent(QPainter& painter)
{
    double opacity = 0.0;
    if (parentGP()->isMouseOver())
        opacity = 0.6;
    else if (isMouseOver())
        opacity = 1.0;
    painter.setOpacity(opacity);
    QPixmap pixmap = qtn_get_thememanager(control())->getIcon(GridThemeManager::ColumnMenuToolButtonIcon, boundedRect().size());
    painter.drawPixmap(boundedRect().topLeft(), pixmap);
}

void GridTableColumnMenuButton::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(e);
    GridDataTableColumn* tableColumn = qobject_cast<GridDataTableColumn *>(m_column);
    if (tableColumn == Q_NULL)
        return;
    QMenu* menu = new QMenu(control());
    menu->setTitle(Grid::tr_compatible(QtnGridColumnMenuString));
    menu->addActions(tableColumn->actionGroup());
    QAction* act = menu->exec(QPoint(mapToGlobal(point)));
    Q_UNUSED(act);
    delete menu;
}

/* GridColumnPrimitive */
GridColumnPrimitive::GridColumnPrimitive(GraphicPrimitive* parent, GridColumnBase* column)
    : GraphicPrimitive(parent), m_columnEditorGP(Q_NULL), m_filterButtonGP(Q_NULL), m_menuGP(Q_NULL), m_column(column), m_textDocument(Q_NULL)
{
}

GridColumnPrimitive::~GridColumnPrimitive()
{
    Q_DELETE_AND_NULL(m_textDocument);
}

GridViewBase* GridColumnPrimitive::view() const
{
    return m_column->viewBase();
}

GridColumnBase* GridColumnPrimitive::column() const
{
    return m_column;
}

void GridColumnPrimitive::paintEvent(QPainter& painter)
{
    QStyleOptionHeader opt;
    initStyleOption(opt);
    QFont f = view()->getColumnFont(m_column);
    painter.setFont(f);
    opt.fontMetrics = QFontMetrics(f);

    if (editorGraphic() && editorGraphic()->isMouseOver())
        opt.state |= QStyle::State_MouseOver;

    if ((m_menuGP != Q_NULL && m_menuGP->isMouseOver()) || (m_filterButtonGP != Q_NULL && m_filterButtonGP->isMouseOver()))
        opt.state |= QStyle::State_MouseOver;

    QRect bounded = boundedRect();

    const QStyleOptionHeader* oldOptPtr = static_cast<const QStyleOptionHeader *>(&m_effSettings.styleOption());
    if (oldOptPtr == Q_NULL)
        oldOptPtr = &opt;

    bool breakEffect = m_effSettings.size() != opt.rect.size() ||
        (opt.state & QStyle::State_Enabled) != (oldOptPtr->state & QStyle::State_Enabled) ||
        (opt.state & QStyle::State_Active) != (oldOptPtr->state & QStyle::State_Active);

    bool hasEffect = ((opt.sortIndicator != oldOptPtr->sortIndicator) ||
        (opt.state & QStyle::State_Sunken) != (oldOptPtr->state & QStyle::State_Sunken) ||
        (opt.state & QStyle::State_On) != (oldOptPtr->state & QStyle::State_On) ||
        (opt.state & QStyle::State_MouseOver) != (oldOptPtr->state & QStyle::State_MouseOver));

    m_effSettings.setStyleOption(opt);

    SmoothEffect* eff = static_cast<GridGraphicSitePublic *>(control())->effects()->findEffect(this);

    if (breakEffect && eff != Q_NULL)
    {
        static_cast<GridGraphicSitePublic *>(control())->effects()->stop(eff);
        eff = Q_NULL;
    }

    if (eff != Q_NULL)
    {
        if (hasEffect)
        {
            eff->setInitialState(m_effSettings.bg());
            eff->setDuration(200);
        }
    }
    else if (hasEffect && !breakEffect)
    {
        eff = new SmoothEffect();
        eff->intialize(this);
        eff->setInitialState(m_effSettings.bg());
        eff->setDuration(200);
        static_cast<GridGraphicSitePublic *>(control())->effects()->run(eff);
    }

    if (eff != Q_NULL)
    {
        double currentOpacity;
        QPixmap currentBg(bounded.size());
        currentBg.fill(Qt::transparent);
        QPainter imgPainter(&currentBg);
        imgPainter.setFont(painter.font());
        eff->paintToCurrent(&imgPainter, currentOpacity);
        imgPainter.setOpacity(currentOpacity);
        doPaintContent(imgPainter, opt);
        imgPainter.end();

        painter.drawPixmap(bounded, currentBg);
        m_effSettings.setBg(currentBg.toImage());
    }

    if (eff == Q_NULL || !eff->isActive())
    {
        QPixmap currentBg(bounded.size());
        currentBg.fill(Qt::transparent);
        QPainter imgPainter(&currentBg);
        imgPainter.setFont(painter.font());
        doPaintContent(imgPainter, opt);
        imgPainter.end();
        m_effSettings.setBg(currentBg.toImage());

        if (eff == Q_NULL)
            painter.drawImage(bounded, m_effSettings.bg());
    }
}

void GridColumnPrimitive::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    if (checkClickEvent(e))
    {
        GridColumnBase* column = m_column;
        QString anchor;
        if (m_textDocument != Q_NULL)
            anchor = m_textDocument->documentLayout()->anchorAt(translateMouseToTextDocument(point));
        if (!anchor.isEmpty())
            emit view()->columnLinkActivated(column, anchor);
        else
        {
            doColumnClick(column, e);
        }
    }
    update();
}

void GridColumnPrimitive::doColumnClick(GridColumnBase* column, QMouseEvent* e)
{
    column->click(e);
}

void GridColumnPrimitive::mouseMoveEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(e);
    if (m_textDocument != Q_NULL)
    {
        QString anchor = m_textDocument->documentLayout()->anchorAt(translateMouseToTextDocument(point));
        if (anchor != "")
            setCursor(Qt::PointingHandCursor);
        else
            setCursor(QCursor());
    }
    else
        setCursor(QCursor());
}

void GridColumnPrimitive::mouseOverEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
    if (m_column->needHeaderHints())
        control()->setToolTip(m_column->caption());
    update();
}

void GridColumnPrimitive::mouseOutEvent()
{
    control()->setToolTip("");
    QToolTip::hideText();
    update();
}


void GridColumnPrimitive::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
    update();
}

void GridColumnPrimitive::mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
    update();
}

void GridColumnPrimitive::initStyleOption(QStyleOptionHeader& styleOption)
{
    m_column->initStyleOption(styleOption);
    if (control()->dragDropState() == dragActive)
    {
        styleOption.state |= QStyle::State_Raised;
    }
    else
    {
        if (isLeftButtonPressed())
            styleOption.state |= QStyle::State_Sunken;
        else
            styleOption.state |= QStyle::State_Raised;
    }

    if (isMouseOver())
        styleOption.state |= QStyle::State_MouseOver;

    styleOption.rect = QRect(0, 0, width(), height());
}


void GridColumnPrimitive::doPaintContent(QPainter& painter, QStyleOptionHeader& styleOption)
{
    QStyleOptionHeader option = styleOption;
    painter.setPen(m_column->viewBase()->getColumnPen(m_column));

    double scaleFactor = m_column->viewBase()->optionsref()->zoomFactor();
    int margin = qRound(control()->style()->pixelMetric(QStyle::PM_HeaderMargin) * scaleFactor);
    int rightIndent = m_filterButtonGP != Q_NULL ? margin + m_filterButtonGP->width() : 0;
    int leftIndent = 0;
    if (m_menuGP != Q_NULL)
        leftIndent += m_menuGP->left() + m_menuGP->width();

    if (m_textDocument != Q_NULL)
        option.text = "";

    GridTableColumnPrimitive::paint_column_header_background(this, &painter, option, m_column->decorationColor());
    GridTableColumnPrimitive::paint_column_header_contents(this, &painter, option, m_column->textElideMode(), m_column->iconApperance(),
        false, leftIndent, rightIndent, m_column->textOrientation(), false, scaleFactor);
}


int GridColumnPrimitive::getCellElementsWidth() const
{
    return 0;
}

GridDragDropObject* GridColumnPrimitive::createDragDropObject(const QPoint& point)
{
    update();
    if (view()->checkMove(column()))
        return column()->createDragDropColumnObject(point, this);
    return Q_NULL;
}

bool GridColumnPrimitive::checkDragDropEvent(const QPoint& pos, GridDragDropObject* object)
{
    GridTableDragColumnObject* dragObject = qobject_cast<GridTableDragColumnObject *>(object);
    if (dragObject != Q_NULL)
        return dragObject->checkDropColumnTarget(pos, this);
    return false;
}

void GridColumnPrimitive::updateHTMLDocument()
{
    if (!m_column->htmlCaption().isEmpty())
    {
        if (m_textDocument == Q_NULL)
        {
            m_textDocument = new QTextDocument(this);
            m_textDocument->setDocumentMargin(0);
        }
        m_textDocument->setHtml(m_column->htmlCaption());
    }
    else
    {
        Q_DELETE_AND_NULL(m_textDocument);
    }
}

QPoint GridColumnPrimitive::translateMouseToTextDocument(const QPoint& pos)
{
    QPoint ret = pos - m_textDocumentRect2.topLeft();
    if (m_column->textOrientation() == Qt::Vertical)
        ret = QPoint(m_textDocumentRect2.height() - ret.y(), ret.x());
    return ret;
}

QFont GridColumnPrimitive::getFont() const
{
    return qtn_font_apply_scale(m_column->font(), m_column->viewBase()->optionsref()->zoomFactor());
}

void GridColumnPrimitive::createEditorGraphic()
{
    if (m_column->columnEditor() == Q_NULL)
        return;
    m_columnEditorGP = new GridColumnEditorGraphic(this, m_column);
    QWidget* w = m_column->columnEditor()->createEditorWidget(m_column);
    w->setAttribute(Qt::WA_DontShowOnScreen);
    w->setAttribute(Qt::WA_OpaquePaintEvent);
    w->show();
    m_columnEditorGP->setWidget(w);
}

void GridColumnPrimitive::deleteEditorGraphic()
{
    Q_DELETE_AND_NULL(m_columnEditorGP);
}

GridColumnEditorGraphic* GridColumnPrimitive::editorGraphic() const
{
    return m_columnEditorGP;
}

void GridColumnPrimitive::updateLayout()
{
    createFilterButton();
    createMenuButton();

    double scaleFactor = m_column->viewBase()->optionsref()->zoomFactor();
    int margin = qRound(control()->style()->pixelMetric(QStyle::PM_HeaderMargin) * scaleFactor);

    if (m_menuGP != Q_NULL)
    {
        const int topOffset = 2;
        const int leftOffset = 2;

        QSize s = QSize(qRound(QtnGridSmallToolButtonSize.width() * scaleFactor), qRound(QtnGridSmallToolButtonSize.height() * scaleFactor));
        m_menuGP->setRect(QRect(qRound(leftOffset * scaleFactor), qRound(topOffset * scaleFactor), s.width(), s.height()));
    }

    if (m_filterButtonGP != Q_NULL)
    {
        QSize s = QSize(qRound(QtnGridSmallToolButtonSize.width() * scaleFactor), qRound(QtnGridSmallToolButtonSize.height() * scaleFactor));
        m_filterButtonGP->setRect(QRect(width() - s.width() - margin, margin, s.width(), s.height()));
    }

    if (m_column != Q_NULL && m_columnEditorGP != Q_NULL)
    {
        QStyleOptionHeader headerOption;
        initStyleOption(headerOption);

        if (m_filterButtonGP != Q_NULL)
            headerOption.rect.adjust(0, 0, m_filterButtonGP->width(), 0);

        QRect labelRectNormal = control()->style()->subElementRect(QStyle::SE_HeaderLabel, &headerOption, control());

        //headerOption.rect.setHeight(r.height());// column()->view()->getColumnHeight(column()));
        QRect labelRect = control()->style()->subElementRect(QStyle::SE_HeaderLabel, &headerOption, control());
        labelRect.setHeight(labelRectNormal.height());

        QRect arrowRect = control()->style()->subElementRect(QStyle::SE_HeaderArrow, &headerOption, control());
        if (arrowRect.top() < labelRect.top())
            labelRect.adjust(0, 3, 0, 0);
        m_columnEditorGP->setRect(labelRect);
    }
}

QSize GridColumnPrimitive::sizeHint()
{
    double factor = m_column->viewBase()->optionsref()->zoomFactor();
    QSize s = get_column_sizeHint(m_column, control()->style(), m_column->viewBase()->getColumnFont(m_column), factor, m_column->caption(), m_textDocument);

    int defHeight = m_column->viewBase()->getColumnHeight(m_column);
    if (defHeight > 0)
        s.rheight() = qRound(defHeight * factor);

    if (m_column->textOrientation() == Qt::Vertical)
        s = QSize(s.height(), s.width());

    int cellElementsWidth = 0;
    if (m_column->viewBase()->layout()->orientation() == Qt::Vertical)
        cellElementsWidth = getCellElementsWidth();
    int columnElementsWidth = 0;

    if (isFilterButton())
    {
        int margin = control()->style()->pixelMetric(QStyle::PM_HeaderMargin);
        columnElementsWidth += qRound((margin + QtnGridSmallToolButtonSize.width()) * factor);
    }

    if (isMenuButton())
    {
        int margin = control()->style()->pixelMetric(QStyle::PM_HeaderMargin);
        columnElementsWidth += qRound((margin + QtnGridSmallToolButtonSize.width()) * factor);
    }

    s.rwidth() += qMax(columnElementsWidth, cellElementsWidth);

    return s;
}

void GridColumnPrimitive::createFilterButton()
{
    if (isFilterButton())
    {
        if (m_filterButtonGP != Q_NULL)
            return;
        m_filterButtonGP = new GridTableColumnFilterButton(static_cast<GridDataTableColumn *>(column()), this);
    }
    else
    {
        Q_DELETE_AND_NULL(m_filterButtonGP);
    }
}

void GridColumnPrimitive::createMenuButton()
{
    if (isMenuButton())
    {
        if (m_menuGP != Q_NULL)
            return;
        m_menuGP = new GridTableColumnMenuButton(m_column, this);
    }
    else
    {
        Q_DELETE_AND_NULL(m_menuGP);
    }
}

ColumnMovePosition GridColumnPrimitive::getMovePositionByPoint(const QPoint& point, bool canRowChange) const
{
    return qtn_get_layout_zone(this, point, view()->layout()->orientation(), canRowChange);
}

void GridColumnPrimitive::paint_column_header_background(GridColumnPrimitive* columnPrimitive, QPainter* painter, const QStyleOptionHeader& option, const QColor& decorColor)
{
    if (columnPrimitive != Q_NULL)
        columnPrimitive->control()->style()->drawControl(QStyle::CE_HeaderSection, &option, painter, columnPrimitive->control());

    if (decorColor != Qt::transparent)
    {
        qreal o = painter->opacity();
        painter->setOpacity(0.3);
        painter->fillRect(option.rect, decorColor);
        painter->setOpacity(o);
    }
}

void GridColumnPrimitive::paint_column_header_contents(GridColumnPrimitive* columnPrimitive, QPainter* painter, const QStyleOptionHeader& option, Qt::TextElideMode textElideMode,
    IconApperance iconApperance, bool drawEditor, int leftTextIndent, int rightTextIndent,
    Qt::Orientation textOrientation, bool air, double scaleFactor)
{
    Grid* grid = qobject_cast<Grid *>(option.styleObject);
    Q_ASSERT(grid != Q_NULL);
    if (grid == Q_NULL)
        return;

    int margin = qRound(grid->style()->pixelMetric(QStyle::PM_HeaderMargin) * scaleFactor);
    int iconSize = qRound(grid->style()->pixelMetric(QStyle::PM_SmallIconSize) * scaleFactor);

    QRect paintRect = option.rect.adjusted(margin + leftTextIndent, margin, -(margin + rightTextIndent), -margin);

    if (option.sortIndicator != QStyleOptionHeader::None)
    {
        QPixmap sortPixmap = qtn_get_thememanager(grid)->getIcon(option.sortIndicator == QStyleOptionHeader::SortUp ? GridThemeManager::SortIndicatorASCIcon :
            GridThemeManager::SortIndicatorDESCIcon, QSize(iconSize, iconSize));

        QRect r;
        if (textOrientation == Qt::Vertical)
        {
            r = QStyle::alignedRect(grid->layoutDirection(), Qt::AlignTop | Qt::AlignHCenter, sortPixmap.size(), paintRect);
            paintRect.adjust(0, iconSize + margin, 0, 0);
        }
        else
        {
            r = QStyle::alignedRect(grid->layoutDirection(), Qt::AlignVCenter | Qt::AlignLeft, sortPixmap.size(), paintRect);
            paintRect.adjust(iconSize + margin, 0, 0, 0);
        }
        painter->drawPixmap(r.topLeft(), sortPixmap);
    }

    if (paintRect.isValid())
    {
        QIcon::Mode mode = option.state & QStyle::State_Enabled ? QIcon::Normal : QIcon::Disabled;
        QPixmap icon = option.icon.pixmap(iconSize, mode);
        if (scaleFactor != 1.0)
            painter->setFont(qtn_font_apply_scale(painter->font(), scaleFactor));
        QRect textRect = paint_icon_and_text(*painter, paintRect,
            icon, option.text, option.textAlignment, textElideMode, iconApperance, option.direction,
            textOrientation, air, margin);
        //painter->drawRect(documentRect);

        QTextDocument* textDocument = columnPrimitive != Q_NULL ? columnPrimitive->m_textDocument : Q_NULL;
        if (option.text.isEmpty() && textDocument != Q_NULL)
        {
            columnPrimitive->m_textDocumentRect2 = textRect;
            painter->save();

            if (textOrientation == Qt::Vertical)
            {
                painter->rotate(-90);
                textRect = QRect(-textRect.top() - textRect.height(), textRect.left(), textRect.height(), textRect.width());
            }

            if (painter->font() != textDocument->defaultFont())
                textDocument->setDefaultFont(painter->font());
            if (textRect.width() != textDocument->textWidth())
                textDocument->setTextWidth(textRect.width());
            if (textDocument->defaultTextOption().alignment() != option.textAlignment)
                textDocument->setDefaultTextOption(QTextOption(option.textAlignment));

            painter->translate(textRect.left(), textRect.top());
            textDocument->drawContents(painter, QRect(QPoint(0, 0), textRect.size()));
            painter->restore();
        }
    }

    if (drawEditor)
    {
        GridColumnEditorGraphic* editorGP = columnPrimitive != Q_NULL ? columnPrimitive->editorGraphic() : Q_NULL;
        if (editorGP != Q_NULL)
        {
            painter->setOpacity(0.7);
            painter->translate(editorGP->left(), editorGP->top());
            editorGP->paintEvent(*painter);
            editorGP->drawBorder(*painter);
        }
    }
}

bool GridColumnPrimitive::checkFilterButtonVisible(const GridColumnBase* columnBase)
{
    const GridDataTableColumn* col = qobject_cast<const GridDataTableColumn *>(columnBase);
    return col != Q_NULL && col->isFilterButtonVisible();
}

bool GridColumnPrimitive::checkMenuButtonVisible(const GridColumnBase* columnBase)
{
    const GridDataTableColumn* col = qobject_cast<const GridDataTableColumn *>(columnBase);
    return col != Q_NULL && col->isMenuButtonVisible();
}

/* GridTableColumnPrimitive */
GridTableColumnPrimitive::GridTableColumnPrimitive(GraphicPrimitive* parent, GridTableColumnBase* column)
    : GridColumnPrimitive(parent, column), m_layouterRow(-1)
{
}

GridTableColumnPrimitive::~GridTableColumnPrimitive()
{
    //if (qobject_cast<const GridTableColumn *>(m_column))
    //    static_cast<GridTableHeaderGraphic *>(parentGP())->m_columnGPs.removeOne(this);
}

GridTableColumnBase* GridTableColumnPrimitive::column() const
{
    return static_cast<GridTableColumnBase *>(m_column);
}

GridTableView* GridTableColumnPrimitive::view() const
{
    return static_cast<GridTableView *>(m_column->viewBase());
}

void GridTableColumnPrimitive::updateFixed()
{
    qtn_do_update_fixed(view()->layout()->orientation() == Qt::Vertical, column()->fixedKind() != FixedNone, this);
}

QSize GridTableColumnPrimitive::minSize()
{
    double factor = m_column->viewBase()->optionsref()->zoomFactor();

    int width = qRound(column()->minWidth() * factor) + getCellElementsWidth();
    if (isMenuButton())
    {
        int margin = control()->style()->pixelMetric(QStyle::PM_HeaderMargin);
        width += qRound((margin + QtnGridSmallToolButtonSize.width()) * factor);
    }

    if (view()->layout()->columnHeader()->isColumnIgnore(column()))
        width = column()->exactWidth();

    GridTableColumn* tableColumn = qobject_cast<GridTableColumn *>(column());
    return QSize(width, 
        tableColumn != Q_NULL ? tableColumn->rowSpan() * sizeHint().height() : sizeHint().height());
}

QSize GridTableColumnPrimitive::maxSize()
{
    double factor = m_column->viewBase()->optionsref()->zoomFactor();
    int width = qRound(column()->maxWidth() * factor) + getCellElementsWidth();
    if (isMenuButton())
    {
        int margin  = control()->style()->pixelMetric(QStyle::PM_HeaderMargin);
        width += qRound((margin + QtnGridSmallToolButtonSize.width()) * factor);
    }

    if (view()->layout()->columnHeader()->isColumnIgnore(column()))
        width = column()->exactWidth();

    return QSize(width, 1000);
}

void GridTableColumnPrimitive::bestFit()
{
    GridViewAutoUpdater autoUpdater(view());

    QSize s = sizeHint();
    QSize min = minSize();
    QSize max = maxSize();
    if (s.width() < min.width())
        s.rwidth() = min.width();
    if (s.height() < min.height())
        s.rheight() = min.height();

    if (s.width() > max.width())
        s.rwidth() = max.width();
    if (s.height() > max.height())
        s.rheight() = max.height();

    column()->setExactWidth(s.width());
    column()->setExactHeight(s.height());
    view()->requestUpdateLayout();
}

bool GridTableColumnPrimitive::isMenuButton() const
{
    return checkMenuButtonVisible(m_column);
}

bool GridTableColumnPrimitive::isFilterButton() const
{
    return checkFilterButtonVisible(m_column);
}

void GridTableColumnPrimitive::mouseOverEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
    if (!view()->isColumnOnScreen(column()))
        GridColumnPrimitive::mouseOverEvent(pos);
    else
        update();
}

void GridTableColumnPrimitive::setLayouterRow(int row)
{
    m_layouterRow = row;
}

int GridTableColumnPrimitive::layouterRow() const
{
    return m_layouterRow;
}

void GridTableColumnPrimitive::setCellLayoutRect(const QRect& rect)
{
    m_cellLayoutRect = rect;
}

const QRect& GridTableColumnPrimitive::cellLayoutRect() const
{
    return m_cellLayoutRect;
}


int GridTableColumnPrimitive::getCellElementsWidth() const
{
    GridDataTableColumn* dataColumn = qobject_cast<GridDataTableColumn *>(m_column);
    if (dataColumn == Q_NULL)
        return 0;
    int ret = 0;
    for (QList<GridCellButtonInfo *>::const_iterator it = dataColumn->m_buttons.constBegin(); it != dataColumn->m_buttons.constEnd(); ++it)
        ret += (*it)->getButtonWidth();
    //    if (view()->options().rowsQuickSelection() && graphic() != Q_NULL && columnGP()->m_quickSelectGP != Q_NULL)
    //        ret += kGridQuickSelectButtonWidth;
    return ret;
}

void GridTableColumnPrimitive::mouseMoveEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(e);
    GridTableColumnBase* resizeColumn;
    Qt::Orientation orient;
    if (getResizeDragDropParams(point, resizeColumn, orient))
    {
        if (orient == Qt::Horizontal)
            setCursor(QCursor(Qt::SplitHCursor));
        else
            setCursor(QCursor(Qt::SplitVCursor));
    }
    else
    {
        GridColumnPrimitive::mouseMoveEvent(point, e);
    }
}

GridDragDropObject* GridTableColumnPrimitive::createDragDropObject(const QPoint& point)
{
    GridTableColumnBase* resizeColumn;
    Qt::Orientation orient;
    if (getResizeDragDropParams(point, resizeColumn, orient))
    {
        update();
        GridTableColumnPrimitive* resizeColumnGP = view()->layout()->columnHeader()->getPrimitiveByColumn(resizeColumn);
        return new GridTableColumnSizingObject(resizeColumnGP, orient);
    }
    else
        return GridColumnPrimitive::createDragDropObject(point);
   
    return Q_NULL;
}

bool GridTableColumnPrimitive::getResizeDragDropParams(const QPoint& point,
                                                     GridTableColumnBase*& columnBase, 
                                                     Qt::Orientation& orient)
{
    if (point.y() > (height() - kGridColumnResizerIndent) && point.y() <= height())
    {
        if (view()->isVerticalResizable(column()))
        {
            columnBase = column();
            orient = Qt::Vertical;
            return true;
        }
    }
    
    if (point.x() >= 0 && point.x() < kGridColumnResizerIndent)
    {
        GridTableColumnBase* leftColumn = view()->getPreviousResizeColumn(column());
        if (leftColumn != Q_NULL && view()->isHorizontalResizable(leftColumn))
        {
            columnBase = leftColumn;
            orient = Qt::Horizontal;
            return true;
        }
    }
    
    if (point.x() >= width() - kGridColumnResizerIndent && point.x() < width())
    {
        GridTableColumnBase* resizeColumn = view()->getNextResizeColumn(column());
        if (resizeColumn != Q_NULL && view()->isHorizontalResizable(resizeColumn))
        {
            columnBase = resizeColumn;
            orient = Qt::Horizontal;
            return true;
        }
    }
    return false;
}

/* GridCardColumnPrimitive */
GridCardColumnPrimitive::GridCardColumnPrimitive(GraphicPrimitive* parent, GridDataTableColumn* column)
    : GridColumnPrimitive(parent, column)
{
}

GridCardColumnPrimitive::~GridCardColumnPrimitive()
{
}

GridCardColumn* GridCardColumnPrimitive::column() const
{
    return static_cast<GridCardColumn *>(m_column);
}

GridCardView* GridCardColumnPrimitive::view() const
{
    return static_cast<GridCardView *>(m_column->viewBase());
}

void GridCardColumnPrimitive::bestFit()
{
    GridViewAutoUpdater autoUpdater(view());

    QSize s = sizeHint();
    column()->setExactWidth(s.width());
    column()->setExactHeight(s.height());
    view()->requestUpdateLayout();
}

ColumnMovePosition GridCardColumnPrimitive::getMovePositionByPoint(const QPoint& point, bool canRowChange) const
{
    return qtn_get_layout_zone(this, point, Qt::Horizontal, canRowChange);
}

bool GridCardColumnPrimitive::isMenuButton() const
{
    return false;
}

bool GridCardColumnPrimitive::isFilterButton() const
{
    return false;
}

void GridCardColumnPrimitive::paintEvent(QPainter& painter)
{
    QStyleOptionHeader option;
    initStyleOption(option);
    QFont f = view()->getColumnFont(m_column);
    painter.setFont(f);
    option.fontMetrics = QFontMetrics(f);
    option.textAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    option.sortIndicator = QStyleOptionHeader::None;
    option.text = option.text + ':';

    if (editorGraphic() && editorGraphic()->isMouseOver())
        option.state |= QStyle::State_MouseOver;

    if ((m_menuGP != Q_NULL && m_menuGP->isMouseOver()) || (m_filterButtonGP != Q_NULL && m_filterButtonGP->isMouseOver()))
        option.state |= QStyle::State_MouseOver;

    painter.setPen(m_column->viewBase()->getColumnPen(m_column));

    double scaleFactor = m_column->viewBase()->optionsref()->zoomFactor();
    int margin = qRound(control()->style()->pixelMetric(QStyle::PM_HeaderMargin) * scaleFactor);
    int rightIndent = m_filterButtonGP != Q_NULL ? margin + m_filterButtonGP->width() : 0;
    int leftIndent = 0;
    if (m_menuGP != Q_NULL)
        leftIndent += m_menuGP->left() + m_menuGP->width();

    if (m_textDocument != Q_NULL)
        option.text = "";

    GridTableColumnPrimitive::paint_column_header_contents(this, &painter, option, m_column->textElideMode(), m_column->iconApperance(),
        false, leftIndent, rightIndent, m_column->textOrientation(), false, scaleFactor);
}

void GridCardColumnPrimitive::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    e->ignore();
    GridColumnPrimitive::mousePressEvent(point, e);
    if (!e->isAccepted())
        static_cast<GridCardDataRowPrimitive *>(parentGP())->mousePressEvent(point, e);
}

void GridCardColumnPrimitive::doColumnClick(GridColumnBase* column, QMouseEvent* e)
{
    Q_UNUSED(column);
    Q_UNUSED(e);
}

/* GridTableBandPrimitive */
GridTableBandPrimitive::GridTableBandPrimitive(GraphicPrimitive* parent, GridTableBand* band)
    : GridTableColumnPrimitive(parent, band)
{
    //parent->m_bandGPs.push_back(this);
}

GridTableBandPrimitive::~GridTableBandPrimitive()
{
    //static_cast<GridBandedTableHeaderGraphic *>(parentGP())->m_bandGPs.removeAll(this);
}

QSize GridTableBandPrimitive::minSize()
{
    bool verticalLayout = view()->layout()->orientation() == Qt::Vertical;

    GridTableBand* band = static_cast<GridTableBand *>(m_column);
    QSize min = GridTableColumnPrimitive::minSize();

    if (band->isBottomBand())
    {
        int row = 0;
        int columnCount = band->m_nestedCols.size();
        while (columnCount > 0)
        {
            QSize curSize = QSize(0, 0);
            for (GridBandedTableColumns::const_iterator it = band->m_nestedCols.constBegin();
                it != band->m_nestedCols.constEnd(); ++it)
            {
                if ((*it)->rowIndex() == row)
                {
                    columnCount--;
                    if (!(*it)->columnGP()->visibility())
                        continue;
                    QSize s = (*it)->columnGP()->minSize();
                    if (verticalLayout)
                        curSize.rwidth() += s.width();
                    else
                        curSize.rheight() += s.height();
                }
            }

            if (curSize.width() > min.rwidth())
                min.rwidth() = curSize.width();
            if (curSize.height() > min.height())
                min.rheight() = curSize.height();
            row++;
        }
    }
    else
    {
        QSize curSize = QSize(0, 0);
        for (GridTableBands::const_iterator it = band->m_nestedBands.constBegin();
            it != band->m_nestedBands.constEnd(); ++it)
        {
            if (!(*it)->columnGP()->visibility())
                continue;
            QSize s = (*it)->columnGP()->minSize();
            if (verticalLayout)
                curSize.rwidth() += s.width();
            else
                curSize.rheight() += s.height();
        }

        if (curSize.width() > min.rwidth())
            min.rwidth() = curSize.width();
        if (curSize.height() > min.height())
            min.rheight() = curSize.height();
    }

    return min;
}

QSize GridTableBandPrimitive::maxSize()
{
    bool verticalLayout = view()->layout()->orientation() == Qt::Vertical;

    GridTableBand* band = static_cast<GridTableBand *>(m_column);
    QSize max = GridTableColumnPrimitive::maxSize();
    if (band->isBottomBand())
    {
        int row = 0;
        int columnCount = band->m_nestedCols.size();
        while (columnCount > 0)
        {
            QSize curSize = QSize(0, 0);
            for (GridBandedTableColumns::const_iterator it = band->m_nestedCols.constBegin();
                it != band->m_nestedCols.constEnd(); ++it)
            {
                if ((*it)->rowIndex() == row)
                {
                    columnCount--;
                    if (!(*it)->columnGP()->visibility())
                        continue;

                    QSize s = (*it)->columnGP()->maxSize();
                    if (verticalLayout)
                        curSize.rwidth() += s.width();
                    else
                        curSize.rheight() += s.height();
                }
            }
            if (curSize.width() > 0/* && curSize.width() > max.width()*/)
                max.rwidth() = qMin(max.width(), curSize.width());
            if (curSize.height() > 0/* && curSize.height() > max.height()*/)
                max.rheight() = qMin(max.height(), curSize.height());
            row++;
        }
    }
    else
    {
        QSize curSize = QSize(0, 0);
        for (GridTableBands::const_iterator it = band->m_nestedBands.constBegin();
            it != band->m_nestedBands.constEnd(); ++it)
        {
            if (!(*it)->columnGP()->visibility())
                continue;
            QSize s = (*it)->columnGP()->maxSize();
            if (verticalLayout)
                curSize.rwidth() += s.width();
            else
                curSize.rheight() += s.height();
        }

        if (curSize.width() > 0/* && curSize.width() > max.width()*/)
            max.rwidth() = qMin(max.rwidth(), curSize.width());
        if (curSize.height() > 0/* && curSize.height() > max.height()*/)
            max.rheight() = qMin(max.height(), curSize.height());
    }
    return max;
}

void GridTableBandPrimitive::bestFit()
{
    GridTableColumnPrimitive::bestFit();
    GridTableBand* band = static_cast<GridTableBand *>(m_column);
    for (GridBandedTableColumns::const_iterator it = band->m_nestedCols.constBegin();
        it != band->m_nestedCols.constEnd(); ++it)
    {
        (*it)->columnGP()->bestFit();
    }

    for (GridTableBands::const_iterator it = band->m_nestedBands.constBegin();
        it != band->m_nestedBands.constEnd(); ++it)
    {
        (*it)->columnGP()->bestFit();
    }
}

GridBandedTableView* GridTableBandPrimitive::view() const
{
    return static_cast<GridBandedTableView *>(m_column->viewBase());
}

void GridTableBandPrimitive::updateColumnsLayout(QSize& columnHeaderSize, const int bandDepth, int& visualRowIndex)
{
    GridTableBand* band = static_cast<GridTableBand *>(m_column);
    GridBandedTableHeaderGraphic* header = static_cast<GridBandedTableHeaderGraphic *>(parentGP());

    GridBandedTableView* bandedView = static_cast<GridBandedTableView *>(view());

    int bandGroupingIndent = bandedView->hasGroupingIndent(band) ? bandedView->getRowGroupIndent(Q_NULL) : 0;
    const bool verticalLayout = bandedView->layout()->orientation() == Qt::Vertical;

    int topOffset = verticalLayout ? band->columnGP()->rect().bottom() + 1 : band->columnGP()->rect().top();
    int leftOffset = verticalLayout ? band->columnGP()->rect().left() : band->columnGP()->rect().right() + 1;
    int frameWidth = 0;
    int frameHeight = 0;


    if (band->isBottomBand())
    {
        int row = 0;
        int layouterRowCount = 0;

        int columnCount = band->m_nestedCols.size();
        int xpos = 0;
        int ypos = 0;
        int maxVisualRowIndex = 1;

        while (columnCount > 0)
        {
            int maxWidth = 0;
            int maxHeight = 0;
            int maxRowSpan = 0;
            int currentVisualRowIndex = 0;

            GridViewBase::GridColumns columnsRow;
            for (GridBandedTableColumns::const_iterator it = band->m_nestedCols.constBegin();
                it != band->m_nestedCols.constEnd(); ++it)
            {
                if ((*it)->rowIndex() == row)
                {
                    columnCount--;
                    if ((*it)->columnGP()->visibility())
                        columnsRow.push_back(*it);
                }
            }


            for (GridViewBase::GridColumns::const_iterator it = columnsRow.constBegin();
                it != columnsRow.constEnd(); ++it)
            {
                GridBandedTableColumn* bandedColumn = static_cast<GridBandedTableColumn *>(*it);
                int span = bandedColumn->rowSpan();
                if (maxRowSpan < span)
                    maxRowSpan = span;

                GridBandedTableView* bandedView = static_cast<GridBandedTableView *>(view());
                int groupingIndent = ((it == columnsRow.constBegin()) && bandedView->hasGroupingIndent(bandedColumn)) ? bandedView->getRowGroupIndent(Q_NULL) : 0;

                if (verticalLayout)
                {
                    int columnHeight = 0;
                    for (int i = visualRowIndex + currentVisualRowIndex; i < visualRowIndex + currentVisualRowIndex + span; ++i)
                    {
                        if (i < header->m_columnHeightTable.count())
                            columnHeight += header->m_columnHeightTable[i];
                        else
                            Q_ASSERT(false);
                            //columnHeight += header->m_columnHeightTable[row]; //default value should be here
                    }
                    bandedColumn->columnGP()->setRect(QRect(band->columnGP()->left() + xpos, topOffset + ypos, bandedColumn->exactWidth() + groupingIndent, columnHeight));
                    xpos += bandedColumn->columnGP()->width();

                    if (bandedColumn->columnGP()->height() > maxHeight)
                        maxHeight = bandedColumn->columnGP()->height();
                }
                else
                {
                    Q_ASSERT(row == bandedColumn->rowIndex());
                    int columnWidth = header->m_columnWidthTable[row];
                    int columnHeight = 0;
                    for (int i = visualRowIndex + currentVisualRowIndex; i < visualRowIndex + currentVisualRowIndex + span; ++i)
                    {
                        if (header->m_columnHeightTable.contains(i))
                            columnHeight += header->m_columnHeightTable[i];
                        else
                            Q_ASSERT(false);
                    }

                    bandedColumn->setExactWidth(columnWidth);

                    bandedColumn->columnGP()->setRect(QRect(leftOffset + xpos, band->columnGP()->top() + ypos, columnWidth, columnHeight + groupingIndent));
                    ypos += bandedColumn->columnGP()->height();

                    if (bandedColumn->columnGP()->width() > maxWidth)
                        maxWidth = bandedColumn->columnGP()->width();
                }

                if (!verticalLayout)
                    currentVisualRowIndex += span;

//                bandedColumn->columnGP()->updateQuickSelectButtonLayout(false);//TODO
                //    m_bandedView->options().rowsQuickSelection() && firstband && firstcolumn);

                bandedColumn->columnGP()->setLayouterRow(layouterRowCount);

                bandedColumn->columnGP()->updateFixed();
            }
            if (verticalLayout)
                visualRowIndex += maxRowSpan;
            else
            {
                if (currentVisualRowIndex > maxVisualRowIndex)
                    maxVisualRowIndex = currentVisualRowIndex;
            }

            row++;
            if (maxRowSpan > 0)
                layouterRowCount += static_cast<GridBandedTableView *>(view())->options().ignoreRowSpanForCells() ? 1 : maxRowSpan;
            maxRowSpan = 0;

            if (verticalLayout)
            {
                if (xpos > frameWidth)
                    frameWidth = xpos;
                ypos += maxHeight;
                if (ypos > frameHeight)
                    frameHeight = ypos;
                xpos = 0;
            }
            else
            {
                if (ypos > frameHeight)
                    frameHeight = ypos;
                xpos += maxWidth;
                if (xpos > frameWidth)
                    frameWidth = xpos;
                ypos = 0;
            }

        }
     
        if (verticalLayout)
        {
            visualRowIndex = 0;
        }
        else
        {
            int bandHeight = 0;
            for (int i = visualRowIndex; i < visualRowIndex + maxVisualRowIndex; ++i)
            {
                if (header->m_columnHeightTable.contains(i))
                    bandHeight += header->m_columnHeightTable[i];
                else
                    Q_ASSERT(false);
            }
            band->columnGP()->setHeight(bandGroupingIndent + bandHeight);
            visualRowIndex += maxVisualRowIndex;
        }
    } 
    else
    {
        GridViewBase::GridColumns columns;
        for (GridTableBands::const_iterator it = band->m_nestedBands.constBegin();
            it != band->m_nestedBands.constEnd(); ++it)
        {
            if ((*it)->columnGP()->visibility())
                columns.push_back(*it);
        }

        int xpos = 0;
        int ypos = 0;
 
        for (GridViewBase::GridColumns::const_iterator it = columns.constBegin();
            it != columns.constEnd(); ++it)
        {
            GridTableBand* childBand = static_cast<GridTableBand *>(*it);
            int childBandGroupingIndent = it == columns.constBegin() ? bandGroupingIndent : 0;// && static_cast<GridBandedTableView *>(view())->hasGroupingIndent(childBand) ? header->groupingIndent() : 0;

            if (verticalLayout)
            {
                
                int bandColumnHeight = header->m_bandItemSizes[bandDepth].height();
                if (childBand->isBottomBand())
                {
                    for (int i = bandDepth + 1; i < header->m_bandItemSizes.size(); i++)
                        bandColumnHeight += header->m_bandItemSizes[i].height();
                }
                //Check if band header should be hidden.
                if (!static_cast<GridBandedTableView *>(view())->options().bandsHeader())
                    bandColumnHeight = 0;

                childBand->columnGP()->setRect(QRect(band->columnGP()->left() + xpos, topOffset + ypos, childBand->exactWidth() + childBandGroupingIndent, bandColumnHeight));
                static_cast<GridTableBandPrimitive *>(childBand->columnGP())->updateColumnsLayout(columnHeaderSize, bandDepth + 1, visualRowIndex);
                xpos += childBand->columnGP()->width();
            }
            else
            {
                int bandColumnWidth = header->m_bandItemSizes[bandDepth].width();
                childBand->setExactWidth(bandColumnWidth);
                if (childBand->isBottomBand())
                {
                    for (int i = bandDepth + 1; i < header->m_bandItemSizes.size(); ++i)
                        bandColumnWidth += header->m_bandItemSizes[i].width();
                }
 
                //Check if band header should be hidden.
                if (!static_cast<GridBandedTableView *>(view())->options().bandsHeader())
                    bandColumnWidth = 0;

                childBand->columnGP()->setRect(QRect(leftOffset + xpos, band->columnGP()->top() + ypos, bandColumnWidth, 0 /* for horizontal layout height will be calculated in updateColumnsLayout(...)*/));
                static_cast<GridTableBandPrimitive *>(childBand->columnGP())->updateColumnsLayout(columnHeaderSize, bandDepth + 1, visualRowIndex);
                band->columnGP()->setHeight(band->columnGP()->height() + childBand->columnGP()->height());
                ypos += childBand->columnGP()->height();
            }
            childBand->columnGP()->updateFixed();
        }
    }

    if (verticalLayout)
    {
        columnHeaderSize.rwidth() += frameWidth;
        columnHeaderSize.rheight() = qMax(frameHeight, columnHeaderSize.height());
    }
    else
    {
        columnHeaderSize.rheight() += frameHeight;
        columnHeaderSize.rwidth() = qMax(frameWidth, columnHeaderSize.width());
    }
}

/* GridTableColumnFrozenPlaceButtonGraphic */
GridTableColumnFrozenPlaceButtonGraphic::GridTableColumnFrozenPlaceButtonGraphic(GraphicPrimitive* parentGraphic, GridViewBase* view)
    : GraphicPrimitive(parentGraphic->control(), parentGraphic), m_view(view)
{
}

QSize GridTableColumnFrozenPlaceButtonGraphic::sizeHint()
{
    return QtnRowsQuickFrozenButtonSize;
}

void GridTableColumnFrozenPlaceButtonGraphic::paintEvent(QPainter& painter)
{
    QRect bounded = boundedRect();
    double scaleFactor = m_view->optionsref()->zoomFactor();
    int iconSize = width() - qRound(6 * scaleFactor);

    if (isMouseOver() && isLeftButtonPressed())
    {
        painter.fillRect(bounded, Qt::lightGray);
    }

    QPixmap icon;
    if (m_view->modelController()->frozenPlace() == GridRow::FrozenTop)
        icon = qtn_get_thememanager(control())->getIcon(GridThemeManager::FrozenPlaceTopIcon, QSize(iconSize, iconSize));
    else
        icon = qtn_get_thememanager(control())->getIcon(GridThemeManager::FrozenPlaceBottomIcon, QSize(iconSize, iconSize));

    QRect r = QStyle::alignedRect(control()->layoutDirection(), Qt::AlignVCenter | Qt::AlignHCenter, icon.size(), bounded);
 
    painter.drawPixmap(r, icon);

    if (isMouseOver())
    {
        painter.setPen(QColor(220, 220, 220));
        painter.drawRect(bounded.adjusted(0, 0, -1, -1));
    }
}

void GridTableColumnFrozenPlaceButtonGraphic::mouseOverEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
    update();
}

void GridTableColumnFrozenPlaceButtonGraphic::mouseOutEvent()
{
    update();
}

void GridTableColumnFrozenPlaceButtonGraphic::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
    update();
}

void GridTableColumnFrozenPlaceButtonGraphic::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
    if (isLeftButtonPressed() && isMouseOver())
    {
        if (m_view->modelController()->frozenPlace() == GridRow::FrozenTop)
            m_view->modelController()->setFrozenPlace(GridRow::FrozenBottom);
        else
            m_view->modelController()->setFrozenPlace(GridRow::FrozenTop);
    }
    parentGP()->update();
}

/* GridTableQuickSelectButtonPrimitive */
GridTableQuickSelectButtonPrimitive::GridTableQuickSelectButtonPrimitive(GraphicPrimitive* parent, GridViewBase* view, GridRowLayoutItem* rowLayoutItem)
    : GraphicPrimitive(parent), m_view(view), m_rowLayoutItem(rowLayoutItem)
{
}

QSize GridTableQuickSelectButtonPrimitive::sizeHint()
{
    QStyleOptionButton styleOption;
    styleOption.state = QStyle::State_Enabled | QStyle::State_Active;
    styleOption.direction = Qt::LeftToRight;
    QStyle::ContentsType ct = m_view->optionsref()->isMultiSelect() ? QStyle::CT_CheckBox : QStyle::CT_RadioButton;
    return control()->style()->sizeFromContents(ct, &styleOption, QSize(0, 0), control()).expandedTo(QApplication::globalStrut());
}

void GridTableQuickSelectButtonPrimitive::paintEvent(QPainter& painter)
{
    bool needpaint = (m_rowLayoutItem == Q_NULL || m_rowLayoutItem->isRowSelected() || 
        m_rowLayoutItem->quickRowButtonGP()->isMouseOverRecursive() || m_rowLayoutItem->dataRowGP()->isMouseOverRecursive());
    if (!needpaint)
        return;

    QStyleOptionButton styleOption;
    styleOption.state = QStyle::State_Enabled | QStyle::State_Active;
    styleOption.direction = Qt::LeftToRight;

    QStyle::ContentsType ct = m_view->optionsref()->isMultiSelect() ? QStyle::CT_CheckBox : QStyle::CT_RadioButton;
    QSize sizeHint = control()->style()->sizeFromContents(ct, &styleOption, QSize(0, 0), control()).expandedTo(QApplication::globalStrut());
    styleOption.rect = QRect(QPoint(0, 0), sizeHint);
    
    QPixmap pixmap(styleOption.rect.size()); 
    pixmap.fill(Qt::transparent);
     
    if (m_rowLayoutItem != Q_NULL)
    {
        styleOption.state |= m_rowLayoutItem->isRowSelected() ? QStyle::State_On : QStyle::State_Off;

    }
    else
    {
        styleOption.state |= m_view->modelController()->isAllSelected() ? QStyle::State_On : QStyle::State_Off;
    }

    
    if (isLeftButtonPressed())
        styleOption.state |= QStyle::State_Sunken;

    if (isMouseOver())
        styleOption.state |= QStyle::State_MouseOver;
    else
        styleOption.state &= ~QStyle::State_MouseOver;

    {
        QPainter pixmapPainter;
        pixmapPainter.begin(&pixmap);
        QStyle::ControlElement ce = m_view->optionsref()->isMultiSelect() ? QStyle::CE_CheckBox : QStyle::CE_RadioButton;
        control()->style()->drawControl(ce, &styleOption, &pixmapPainter, 0);
    }
    
    if (m_view->optionsref()->zoomFactor() < 1.0)
    {
        sizeHint.setWidth(sizeHint.width() * m_view->optionsref()->zoomFactor() + 1);
        sizeHint.setHeight(sizeHint.height() * m_view->optionsref()->zoomFactor() + 1);
    }

    QRect targetRect = QStyle::alignedRect(control()->layoutDirection(), Qt::AlignVCenter | Qt::AlignHCenter, 
        sizeHint, boundedRect());

    painter.drawPixmap(targetRect, pixmap);
/*
    QStylePainter p(this);
    QStyleOptionButton opt;
    initStyleOption(&opt);
    opt.state &= ~QStyle::State_KeyboardFocusChange;
    p.drawControl(QStyle::CE_CheckBox, opt);
*/
}

void GridTableQuickSelectButtonPrimitive::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
    update();
}

void GridTableQuickSelectButtonPrimitive::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    if (checkClickEvent(e) && isMouseOver())
    {
        if (m_rowLayoutItem != Q_NULL && m_rowLayoutItem->gridRow() != Q_NULL)
        {
            GridDataTableColumn* c1 = m_view->getFirstColumn();
            GridDataTableColumn* c2 = m_view->getLastColumn();
            if (c1 != Q_NULL && c2 != Q_NULL)
                m_view->selectRange(m_rowLayoutItem->gridRow()->rowIndex(), m_rowLayoutItem->gridRow()->rowIndex(), 
                c1->index(), c2->index(), m_rowLayoutItem->isRowSelected() ? Qtitan::Unselect : Qtitan::Select);
        }
        else
        {
            if (m_view->modelController()->isAllSelected())
                m_view->deselectAll();
            else
                m_view->selectAll();
        }
    }    
    if (m_rowLayoutItem != Q_NULL)
        m_rowLayoutItem->update();
}

void GridTableQuickSelectButtonPrimitive::mouseOverEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
    update();
}

void GridTableQuickSelectButtonPrimitive::mouseOutEvent()
{
    update();
}

/* GridTableColumnQuickButtonGraphic */
GridTableColumnQuickButtonPrimitive::GridTableColumnQuickButtonPrimitive(
    GridTableHeaderGraphic* parent, GridFieldChooserProvider* provider, bool bands) 
    : GraphicPrimitive(parent->control(), parent), m_quickSelectGP(Q_NULL), m_bands(bands)
{
    m_popup = new QuickCustomizationPopup(provider);
    m_popup->hide();
    m_frozenPlaceButtonGP = new GridTableColumnFrozenPlaceButtonGraphic(this, provider->view());
    m_quickSelectGP = new GridTableQuickSelectButtonPrimitive(this, provider->view(), Q_NULL);
}

GridTableColumnQuickButtonPrimitive::~GridTableColumnQuickButtonPrimitive()
{
    Q_DELETE_AND_NULL(m_popup);
    Q_DELETE_AND_NULL(m_frozenPlaceButtonGP);
    Q_DELETE_AND_NULL(m_quickSelectGP);
}

void GridTableColumnQuickButtonPrimitive::updateLayout()
{
    GridTableView* tableView = static_cast<GridTableHeaderGraphic *>(parentGP())->view();
    m_frozenPlaceButtonGP->setVisibility(!m_bands && tableView->options().frozenPlaceQuickSelection());
    m_quickSelectGP->setVisibility(!m_bands && tableView->options().rowsQuickSelection() && tableView->options().isMultiSelect());
    double factor = tableView->optionsref()->zoomFactor();
    int margin = qRound(control()->style()->pixelMetric(QStyle::PM_HeaderMargin) * factor);
    QRect r = QRect(QPoint(0, 0), boundedRect().size());
    r.adjust(margin, 0, 0, 0);

    if (m_frozenPlaceButtonGP->visibility())
    {
        QSize sh = m_frozenPlaceButtonGP->sizeHint() * factor;
        QRect elementRect = QStyle::alignedRect(Qt::LeftToRight,
            Qt::AlignLeft | Qt::AlignVCenter, sh, r);

        m_frozenPlaceButtonGP->setRect(elementRect);
        r.adjust(m_frozenPlaceButtonGP->width() + margin, 0, 0, 0);
    }

    if (m_quickSelectGP->visibility())
    {
        QSize sh = m_quickSelectGP->sizeHint() * factor;
        QRect elementRect = QStyle::alignedRect(Qt::LeftToRight,
            Qt::AlignLeft | Qt::AlignVCenter, sh, r);
        m_quickSelectGP->setRect(elementRect);
        r.adjust(m_quickSelectGP->width() + margin, 0, 0, 0);
    }
    /*
    if (!m_bands)
    {
        
        if (tableView->options().columnsQuickMenuVisible())
        {
            r.adjust(margin, 0, 0, 0);
            m_menuIconLeftPos = r.left();
            r.adjust(qRound(kQuickCustomizeButtonWidth * factor), 0, 0, 0);
        }
    }
    else
    {
        if (qobject_cast<GridBandedTableView *>(tableView)->options().bandsQuickMenuVisible())
        {
            r.adjust(margin, 0, 0, 0);
            m_menuIconLeftPos = r.left();
            r.adjust(qRound(kQuickCustomizeButtonWidth * factor), 0, 0, 0);
        }
    }
    */
}

QSize GridTableColumnQuickButtonPrimitive::sizeHint()
{
    if (!m_bands)
    {
        return quickRowButtonSizeHint(Q_NULL);
    }
    else
    {
        GridTableView* tableView = static_cast<GridTableHeaderGraphic *>(parentGP())->view();
        double factor = tableView->optionsref()->zoomFactor();
        int margin = qRound(control()->style()->pixelMetric(QStyle::PM_HeaderMargin) * factor);

        GridBandedTableView* bandedTableView = qobject_cast<GridBandedTableView *>(tableView);
        QSize s = get_column_sizeHint(Q_NULL, control()->style(), bandedTableView->options().bandFont(), factor);
        s.rwidth() = 0;
        if (bandedTableView->options().bandsQuickCustomization())
            s.rwidth() += margin + qRound(kQuickCustomizeButtonWidth * factor);
        if (s.rwidth() > 0)
            s.rwidth() += margin;
        return s;
    }
}

QSize GridTableColumnQuickButtonPrimitive::quickRowButtonSizeHint(GridRow* row)
{
    GridTableView* tableView = static_cast<GridTableHeaderGraphic *>(parentGP())->view();
    double factor = tableView->optionsref()->zoomFactor();
    int margin = qRound(control()->style()->pixelMetric(QStyle::PM_HeaderMargin) * factor);

    QSize s = get_column_sizeHint(Q_NULL, control()->style(), tableView->options().columnFont(), factor);
    s.rwidth() = 0;
    bool includeFromzeButton = row == Q_NULL || row->type() == GridRow::DataRow;
    if (includeFromzeButton && (tableView->options().isRowFrozenButtonVisible() || tableView->options().frozenPlaceQuickSelection()))
        s.rwidth() += margin + qRound(m_frozenPlaceButtonGP->sizeHint().width() * factor);

    if (tableView->options().rowsQuickSelection())
        s.rwidth() += margin + qRound(m_quickSelectGP->sizeHint().width() * factor);

    if (tableView->options().columnsQuickCustomization())
        s.rwidth() += margin + qRound(kQuickCustomizeButtonWidth * factor);

    if (s.rwidth() > 0)
        s.rwidth() += margin;
    return s;
}

void GridTableColumnQuickButtonPrimitive::setDropDownCount(int count)
{
    m_popup->m_dropDownCount = count;
}

void GridTableColumnQuickButtonPrimitive::paintEvent(QPainter& painter)
{
    QStyleOptionHeader styleOption;
    styleOption.state = QStyle::State_Enabled | QStyle::State_Active;
    styleOption.orientation = Qt::Horizontal;
    styleOption.rect = QRect(0, 0, rect().width(), rect().height());

    GridTableView* tableView = static_cast<GridTableHeaderGraphic *>(parentGP())->view();
    GridBandedTableView* bandedTableView = qobject_cast<GridBandedTableView *>(tableView);

    bool buttonvisible = m_bands ? bandedTableView->options().bandsQuickCustomization() : tableView->options().columnsQuickCustomization();

    if (buttonvisible)
    {
        styleOption.selectedPosition = QStyleOptionHeader::NotAdjacent;
        if (isMouseOver())
        {
            styleOption.state |= QStyle::State_MouseOver;
        }
        if (m_popup->isVisible())
        {
            styleOption.state |= QStyle::State_Sunken;
        }
    }

    QRect bounded = boundedRect();
    QPixmap pixmap(bounded.size());
    pixmap.fill(Qt::transparent);
    
    {
        QPainter pixmapPainter;
        pixmapPainter.begin(&pixmap);
        control()->style()->drawControl(QStyle::CE_Header, &styleOption, &pixmapPainter, control());
    }
    
    painter.drawPixmap(bounded, pixmap);

    if (m_bands)
        buttonvisible = buttonvisible && bandedTableView->options().bandsQuickMenuVisible();
    else
        buttonvisible = buttonvisible && tableView->options().columnsQuickMenuVisible();

    double factor = tableView->optionsref()->zoomFactor();
    int margin = qRound(control()->style()->pixelMetric(QStyle::PM_HeaderMargin) * factor);
    QRect r = bounded;
    r.adjust(margin, 0, -margin, 0);
    if (m_frozenPlaceButtonGP->visibility())
        r.adjust(m_frozenPlaceButtonGP->width() + margin, 0, 0, 0);

    if (m_quickSelectGP->visibility())
        r.adjust(m_frozenPlaceButtonGP->width() + margin, 0, 0, 0);

    if (buttonvisible)
    {
        QPixmap icon = qtn_get_thememanager(control())->getIcon(GridThemeManager::QuickButtonIcon, 
            QSize(qRound((kQuickCustomizeButtonWidth - 4) * factor), qRound((kQuickCustomizeButtonWidth - 4) * factor)));
        QRect iconRect = QStyle::alignedRect(Qt::LeftToRight,
            Qt::AlignRight | Qt::AlignVCenter, icon.size(), r);
        painter.drawPixmap(iconRect.topLeft(), icon);
    }
}

void GridTableColumnQuickButtonPrimitive::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
    GridTableView* tableView = static_cast<GridTableHeaderGraphic *>(parentGP())->view();
    GridBandedTableView* bandedTableView = qobject_cast<GridBandedTableView *>(tableView);
    bool buttonvisible = m_bands ? bandedTableView->options().bandsQuickCustomization() : tableView->options().columnsQuickCustomization();

    if (bandedTableView != Q_NULL)
        buttonvisible = buttonvisible && bandedTableView->options().bandsQuickMenuVisible();
    else
        buttonvisible = buttonvisible && tableView->options().columnsQuickMenuVisible();

    if (buttonvisible)
    {
        m_popup->setPalette(control()->palette());
        QPoint pos = mapToGlobal(QPoint(0, height()));
        m_popup->move(pos);
        m_popup->show();
    }
    GraphicPrimitive::update();
}

void GridTableColumnQuickButtonPrimitive::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
    GraphicPrimitive::update();
}

void GridTableColumnQuickButtonPrimitive::mouseOverEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
    GraphicPrimitive::update();
}

void GridTableColumnQuickButtonPrimitive::mouseOutEvent()
{
    GraphicPrimitive::update();
}

/* GridTableViewFixedDecorGraphic */
GridTableViewFixedDecorGraphic::GridTableViewFixedDecorGraphic(GridTableView* view)
    : GraphicPrimitive(view->layout()), m_view(view)
{
    setTransparent(true);
    setZIndex(m_view->layout()->header()->zIndex() + 10);
}

void GridTableViewFixedDecorGraphic::paintEvent(QPainter& painter)
{
    QRect firstRect;
    QRect lastRect;

    if (m_view->layout()->leftFixedIndent() > 0)
    {
        if (m_view->layout()->orientation() == Qt::Vertical)
        {
            firstRect = QRect(m_view->layout()->getQuickButtonIndent() + m_view->layout()->leftFixedIndent() - 2 -
                m_view->options().fixedSeparatorWidth(), m_view->layout()->columns()->top(),
                m_view->options().fixedSeparatorWidth(), m_view->layout()->height());
        }
        else
        {
            firstRect = QRect(m_view->layout()->columns()->left(),
                m_view->layout()->columns()->top() + m_view->layout()->getQuickButtonIndent() + m_view->layout()->leftFixedIndent() - 2 - m_view->options().fixedSeparatorWidth(),
                m_view->layout()->width(), m_view->options().fixedSeparatorWidth());
        }
        painter.fillRect(firstRect, m_view->options().fixedSeparatorColor());

    }
    if (m_view->layout()->rightFixedIndent())
    {
        if (m_view->layout()->orientation() == Qt::Vertical)
        {
            lastRect = QRect(m_view->layout()->getViewWidth() - m_view->layout()->rightFixedIndent() + 1, 
                m_view->layout()->columns()->top(),
                m_view->options().fixedSeparatorWidth(), m_view->layout()->height());
            if (lastRect.x() > firstRect.x())
                painter.fillRect(lastRect, m_view->options().fixedSeparatorColor());
        }
        else
        {
            lastRect = QRect(m_view->layout()->columns()->left(),
                m_view->layout()->columns()->top() + m_view->layout()->getViewHeight() - m_view->layout()->rightFixedIndent() + 1,
                m_view->layout()->width(), m_view->options().fixedSeparatorWidth());
            if (lastRect.y() > firstRect.y())
                painter.fillRect(lastRect, m_view->options().fixedSeparatorColor());
        }
    }
}

/* QuickCustomizationPopup */
QuickCustomizationPopup::QuickCustomizationPopup(GridFieldChooserProvider* provider)
: QWidget(provider->view()->grid(), Qt::Popup | Qt::FramelessWindowHint),
  m_provider(provider), m_dropDownCount(0)
{
    setBackgroundRole(QPalette::Base);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setFrameStyle(QFrame::Panel | QFrame::Plain);
    m_scrollArea->setLineWidth(1);
    m_area = new QWidget(this);
    m_scrollArea->setWidget(m_area);
}

GridFieldChooserProvider* QuickCustomizationPopup::provider() const
{
    return m_provider;
}

class GridColumnCheckBox: public QCheckBox
{
public:
    GridColumnCheckBox(QWidget* parent, GridColumnBase* column)
        : QCheckBox(column->caption(), parent), m_column(column)
    {
    }
    inline GridColumnBase* column() const { return m_column; }
private:
    GridColumnBase* m_column;
};

void QuickCustomizationPopup::showEvent(QShowEvent* /*event*/)
{
    recreateChecks();
}

void QuickCustomizationPopup::hideEvent(QHideEvent* /*event*/)
{
    m_provider->view()->layout()->update();
}

void QuickCustomizationPopup::recreateChecks()
{
    for (QList<QCheckBox *>::iterator it = m_checks.begin(); it != m_checks.end(); ++it)
        delete *it;
    m_checks.clear();

    GridViewBase::GridColumns cols = m_provider->fields();
    int posY = 0;
    int popupWidth = 0;
    int checkBoxHeight = 0;

    for (GridViewBase::GridColumns::const_iterator it = cols.constBegin(); it != cols.constEnd(); ++it)
    {
        if (!static_cast<GridTableView *>(m_provider->view())->hasVisibleParent(*it))
            continue;

        if (static_cast<const Qtitan::GridTableColumnBase*>(*it)->isHidingEnabled())
        {
           QCheckBox* chBox = new GridColumnCheckBox(m_area, *it);
           chBox->setChecked((*it)->isVisible());
           connect(chBox, SIGNAL(stateChanged(int)), this, SLOT(columnStateChanged(int)), Qt::QueuedConnection);
           chBox->show();
           chBox->move(2, posY);
           checkBoxHeight = chBox->height();
           posY += checkBoxHeight;
           if (chBox->width() > popupWidth)
               popupWidth = chBox->width();
           m_checks.push_back(chBox);
        }
    }

    m_area->setGeometry(0, 0, popupWidth, posY);
    if (m_dropDownCount > 0)
    {
        if (checkBoxHeight * m_dropDownCount < posY)
        {
            posY = checkBoxHeight * m_dropDownCount;
            popupWidth += m_scrollArea->verticalScrollBar()->sizeHint().width();
        }
    }
    QRect g = geometry();
    g.setWidth(popupWidth + 3);
    g.setHeight(posY + 3);
    setGeometry(g);
    m_scrollArea->setGeometry(0, 0, g.width(), g.height());
}

void QuickCustomizationPopup::columnStateChanged(int state)
{
    GridColumnCheckBox* chBox = static_cast<GridColumnCheckBox *>(sender());
    chBox->column()->setVisible(state == 2);
    recreateChecks();
}

/* GridBandedTableHeaderGraphic */
GridBandedTableHeaderGraphic::GridBandedTableHeaderGraphic(GraphicPrimitive* parent, GridViewBase* view)
    : GridTableHeaderGraphic(parent, view), m_bandedView((GridBandedTableView *)view)
{
    m_quickBandButton = new GridTableColumnQuickButtonPrimitive(this, m_bandedView->m_fieldProviders[1], true);
    m_quickBandButton->setDropDownCount(m_bandedView->options().bandsQuickCustomizationMaxDropDownCount());
    setFixedGPX(m_quickBandButton, true);
    setFixedGPY(m_quickBandButton, true);
}

GridBandedTableHeaderGraphic::~GridBandedTableHeaderGraphic()
{
    Q_DELETE_AND_NULL(m_quickBandButton);
}

GridTableColumnQuickButtonPrimitive* GridBandedTableHeaderGraphic::quickBandButton() const
{ 
    return m_quickBandButton;
}

void GridBandedTableHeaderGraphic::updateBandsLayout(int leftOffset, int topOffset, QSize& bandHeaderSize, QSize& columnHeaderSize)
{
    bool verticalLayout = m_bandedView->layout()->orientation() == Qt::Vertical;
    const int bandDepth = 0;

    int xpos = leftOffset;
    int ypos = topOffset;
    int frameWidth = xpos;
    int frameHeight = ypos;
    
    int rangeColumnScrollPixels = verticalLayout ? m_bandedView->layout()->scrollHor()->maximum() : m_bandedView->layout()->scrollVer()->maximum();
    int visualRowIndex = 0;
    bool fistFixedColumn = true;
    for (GridViewBase::GridColumns::const_iterator it = m_visibleColumns.constBegin();
        it != m_visibleColumns.constEnd(); ++it)
    {
        GridTableBand* band = static_cast<GridTableBand *>(*it);
        int bandGroupingIndent = m_visibleColumns.constBegin() == it && m_bandedView->hasGroupingIndent(band) ? m_bandedView->getRowGroupIndent(Q_NULL) : 0;

        if (verticalLayout)
        {
            if (band->fixedKind() == FixedRight && fistFixedColumn)
            {
                fistFixedColumn = false;
                xpos -= rangeColumnScrollPixels;
                frameWidth -= rangeColumnScrollPixels;
                columnHeaderSize.rwidth() -= rangeColumnScrollPixels;
            }

            int bandColumnHeight = m_bandItemSizes[bandDepth].height();
            if (band->isBottomBand())
            {
                for (int i = bandDepth + 1; i < m_bandItemSizes.size(); i++)
                    bandColumnHeight += m_bandItemSizes[i].height();
            }
            //Check if band header should be hidden.
            if (!static_cast<GridBandedTableView *>(view())->options().bandsHeader())
                bandColumnHeight = 0;

            band->columnGP()->setHeight(bandColumnHeight);
            band->columnGP()->setWidth(band->exactWidth() + bandGroupingIndent);
        }
        else
        {
            if (band->fixedKind() == FixedRight && fistFixedColumn)
            {
                fistFixedColumn = false;
                ypos -= rangeColumnScrollPixels;
                frameHeight -= rangeColumnScrollPixels;
                columnHeaderSize.rheight() -= rangeColumnScrollPixels;
            }

            int bandColumnWidth = m_bandItemSizes[bandDepth].width();
            band->setExactWidth(bandColumnWidth);
            if (band->isBottomBand())
            {
                for (int i = bandDepth + 1; i < m_bandItemSizes.size(); ++i)
                    bandColumnWidth += m_bandItemSizes[i].width();
            }

            //Check if band header should be hidden.
            if (!static_cast<GridBandedTableView *>(view())->options().bandsHeader())
                bandColumnWidth = 0;

            band->columnGP()->setWidth(bandColumnWidth);// band->columnGP()->exactWidth());
            band->columnGP()->setHeight(0 /* for horizontal layout height will be calculated in updateColumnsLayout(...)*/);
        }

        band->columnGP()->updateFixed();

        band->columnGP()->setLeft(xpos);
        band->columnGP()->setTop(ypos);
        
        static_cast<GridTableBandPrimitive *>(band->columnGP())->updateColumnsLayout(columnHeaderSize, bandDepth + 1, visualRowIndex);

        if (verticalLayout)
        {
            xpos += band->columnGP()->width();
            frameWidth += band->columnGP()->width();
        }
        else
        {
            ypos += band->columnGP()->height();
            frameHeight += band->columnGP()->height();
        }
    }


    if (verticalLayout)
    {
        int height = 0;
        if (static_cast<GridBandedTableView *>(view())->options().bandsHeader())
        {
            for (QMap<int, QSize>::const_iterator it = m_bandItemSizes.constBegin(); it != m_bandItemSizes.constEnd(); it++)
                height += it.value().height();
        }

        bandHeaderSize = QSize(frameWidth, height);
    }
    else
    {
        int width = 0;
        if (static_cast<GridBandedTableView *>(view())->options().bandsHeader())
        {
            for (QMap<int, QSize>::const_iterator it = m_bandItemSizes.constBegin(); it != m_bandItemSizes.constEnd(); it++)
                width += it.value().width();
        }
        bandHeaderSize = QSize(width, frameHeight);
    }
}

void GridBandedTableHeaderGraphic::updateQuickButtonLayout()
{
    bool verticalLayout = m_bandedView->layout()->orientation() == Qt::Vertical;

    QSize quickBandButtonSize = m_quickBandButton->sizeHint();
    QSize quickColumnButtonSize = m_quickColumnButton->sizeHint();

    quickBandButtonSize.rwidth() = qMax(quickBandButtonSize.width(), quickColumnButtonSize.width());
    quickColumnButtonSize.rwidth() = quickBandButtonSize.width();
    quickBandButtonSize.rheight() = qMax(quickBandButtonSize.height(), quickColumnButtonSize.height());
    quickColumnButtonSize.rheight() = quickBandButtonSize.height();

    if (!m_bandedView->options().bandsHeader())
    {
        if (verticalLayout)
            quickBandButtonSize.setHeight(0);
        else
            quickBandButtonSize.setWidth(0);
    }

    m_quickBandButton->setWidth(quickBandButtonSize.width());
    m_quickBandButton->setHeight(quickBandButtonSize.height());
    m_quickColumnButton->setWidth(quickColumnButtonSize.width());
    m_quickColumnButton->setHeight(quickColumnButtonSize.height());
    m_quickBandButton->setVisibility(quickBandButtonSize.width() != 0);
    m_quickColumnButton->setVisibility(quickColumnButtonSize.width() != 0);
}

QSize GridBandedTableHeaderGraphic::updateColumnsLayout()
{
    bool verticalLayout = m_bandedView->layout()->orientation() == Qt::Vertical;

    int leftOffset = verticalLayout ? m_quickColumnButton->width() : 0;
    int topOffset = verticalLayout ? 0 : m_quickColumnButton->height();

    QSize bandHeaderSize(0, 0);
    QSize columnHeaderSize(0, 0);

    updateBandsLayout(leftOffset, topOffset, bandHeaderSize, columnHeaderSize);
    
    if (verticalLayout)
    {
        bandHeaderSize.rheight() = qMax(m_quickBandButton->height(), bandHeaderSize.height());
        columnHeaderSize.rheight() = qMax(m_quickColumnButton->height(), columnHeaderSize.height());
        m_quickBandButton->setRect(QRect(0, 0, m_quickBandButton->width(), bandHeaderSize.height()));
        m_quickColumnButton->setRect(QRect(0, m_quickBandButton->height(), m_quickBandButton->width(), columnHeaderSize.height()));
    }
    else
    {
        bandHeaderSize.rwidth() = qMax(m_quickBandButton->width(), bandHeaderSize.width());
        columnHeaderSize.rwidth() = qMax(m_quickColumnButton->width(), columnHeaderSize.width());
        m_quickBandButton->setRect(QRect(0, 0, bandHeaderSize.width(), m_quickBandButton->height()));
        m_quickColumnButton->setRect(QRect(m_quickBandButton->width(), 0, columnHeaderSize.width(), m_quickBandButton->height()));
    }

    if (!m_bandedView->options().columnsHeader())
        columnHeaderSize.setHeight(0);

    if (verticalLayout)
        return QSize(qMax(columnHeaderSize.width(), bandHeaderSize.width()), columnHeaderSize.height() + bandHeaderSize.height());
    else
        return QSize(columnHeaderSize.width() + bandHeaderSize.width(), qMax(columnHeaderSize.height(), bandHeaderSize.height()));
}

GridTableBandPrimitive* GridBandedTableHeaderGraphic::createBandGP2(GridTableBand* columnBase)
{
    GridTableBandPrimitive* gp = new GridTableBandPrimitive(this, columnBase);
    gp->setId(GridHitInfo::Band);
    gp->createEditorGraphic();
//    ((ScrollableGraphicPrimitive *)gp->parentGP())->setFixedGPY(gp, true);
    gp->bestFit();
    return gp;
}

void GridBandedTableHeaderGraphic::addColumn(GridTableColumnBase* columnBase)
{
    GridTableBand* band = qobject_cast<GridTableBand *>(columnBase);
    if (band != Q_NULL)
    {
        GridTableBandPrimitive* gp = createBandGP2(band);
        m_bandGPsHash.insert(band, gp);
    }
    else
        GridTableHeaderGraphic::addColumn(columnBase);
}

void GridBandedTableHeaderGraphic::removeColumn(GridTableColumnBase* columnBase)
{
    QHash<const GridTableBand *, GridTableBandPrimitive *>::iterator it = m_bandGPsHash.find(static_cast<GridTableBand *>(columnBase));
    if (it != m_bandGPsHash.end())
    {
        delete it.value();
        m_bandGPsHash.erase(it);
    }
    else
        GridTableHeaderGraphic::removeColumn(columnBase);
}

GridTableColumnPrimitive* GridBandedTableHeaderGraphic::getPrimitiveByColumn(const GridTableColumnBase* columnBase)
{
    const GridTableBand* band = qobject_cast<const GridTableBand *>(columnBase);
    if (band != Q_NULL)
    {
        QHash<const GridTableBand *, GridTableBandPrimitive *>::const_iterator it = m_bandGPsHash.constFind(band);
        if (it == m_bandGPsHash.constEnd())
            return Q_NULL;
        else
            return it.value();
    }
    else
        return GridTableHeaderGraphic::getPrimitiveByColumn(columnBase);
}

#if 0
void GridBandedTableHeaderGraphic::deleteBandGP(GridTableBandPrimitive* gp)
{
    for (QList<GridTableBandPrimitive *>::iterator it = m_bandGPs.begin(); it != m_bandGPs.end(); ++it)
    {
        if (*it == gp)
        {
            delete gp;
            m_bandGPs.erase(it);
            break;
        }
    }
}
#endif

void GridBandedTableHeaderGraphic::bestFit()
{
    for (QHash<const GridTableBand *, GridTableBandPrimitive *>::iterator it = m_bandGPsHash.begin(); it != m_bandGPsHash.end(); ++it)
        it.value()->bestFit();
}

bool GridBandedTableHeaderGraphic::isColumnIgnore(GridTableColumnBase* column) const
{
    if (m_ignoreColumn == Q_NULL)
        return false;
    
    GridTableBand* parent = Q_NULL;
    GridTableColumnBase* c1 = column;
    GridTableColumnBase* c2 = m_ignoreColumn;

    if (c1 == c2)
        return true;

    while (get_parent_band(c1) != Q_NULL)
    {
        if (qobject_cast<GridBandedTableColumn *>(m_ignoreColumn) && qobject_cast<GridBandedTableColumn *>(m_ignoreColumn)->isDescendant(get_parent_band(c1)))
        {
            parent = get_parent_band(c1);
            break;
        }

        if (qobject_cast<GridTableBand *>(m_ignoreColumn) && qobject_cast<GridTableBand *>(m_ignoreColumn)->isDescendant(get_parent_band(c1)))
        {
            parent = get_parent_band(c1);
            break;
        }
        c1 = get_parent_band(c1);
    }
    Q_ASSERT(c1 != Q_NULL);
    if (parent == m_ignoreColumn)
        return false;

    while (parent != get_parent_band(c2))
    {
        c2 = get_parent_band(c2);
        Q_ASSERT(c2 != Q_NULL);
        if (c2 == Q_NULL)
            break;
    }

    if (c1 == Q_NULL || c2 == Q_NULL)
    {
        Q_ASSERT(false);
        return false;
    }
  
    if (qobject_cast<GridTableBand *>(c1) != Q_NULL)
    {
        Q_ASSERT(qobject_cast<GridTableBand *>(c2) != Q_NULL);
        return c2->visualIndex() > c1->visualIndex();
    }
    if (qobject_cast<GridBandedTableColumn *>(c1) != Q_NULL)
    {
        Q_ASSERT(qobject_cast<GridBandedTableColumn *>(c2) != Q_NULL);
        return qobject_cast<GridBandedTableColumn *>(c2)->rowIndex() == 
            qobject_cast<GridBandedTableColumn *>(c1)->rowIndex() && c2->visualIndex() > c1->visualIndex();
    }
    return false;
}

void GridBandedTableHeaderGraphic::exactSizeChanged(GridTableColumnBase* column)
{
    GridTableHeaderGraphic::exactSizeChanged(column);

    GridTableBand* band = qobject_cast<GridTableBand *>(column);
    if (band == Q_NULL || band->columnGP() == Q_NULL || m_bandedView->layout()->orientation() != Qt::Vertical)
        return;
    int width = band->exactWidth();

    if (band->isBottomBand())
    {
        int row = 0;
        int columnCount = band->m_nestedCols.size();
        while (columnCount > 0)
        {
            GridViewBase::GridColumns columnsRow;
            for (GridBandedTableColumns::const_iterator it = band->m_nestedCols.constBegin();
                it != band->m_nestedCols.constEnd(); ++it)
            {
                if ((*it)->rowIndex() == row)
                {
                    columnCount--;
                    if (!(*it)->columnGP()->visibility())
                        continue;
                    columnsRow.push_back(*it);
                }
            }
            if (columnsRow.size() > 0)
                calculateColumnWidths(columnsRow, width, false);
            row++;
        }
    }
    else
    {
        GridViewBase::GridColumns bands;
        for (GridTableBands::const_iterator it = band->m_nestedBands.constBegin();
            it != band->m_nestedBands.constEnd(); ++it)
        {
            if ((*it)->columnGP()->visibility())
                bands.push_back(*it);
        }
        if (bands.size() > 0)
            calculateColumnWidths(bands, width, false);
    }
}

void GridBandedTableHeaderGraphic::createTable()
{
    bool verticalLayout = m_bandedView->layout()->orientation() == Qt::Vertical;
        
    createBandTable();

    GridBandedTableColumn* ignoreColumn = qobject_cast<GridBandedTableColumn *>(m_ignoreColumn);

    m_columnWidthTable.clear();
    m_columnHeightTable.clear();
    m_leftFixedIndent = 0;
    m_rightOrBottomFixedIndent = 0;
    GridTableBands bottomBands;
    int visualRowIndex = 0;
    bool firstband = true;
    m_bandedView->getBottomLevelBands(Q_NULL, bottomBands);
    for (GridTableBands::const_iterator it = bottomBands.constBegin(); it != bottomBands.constEnd(); ++it)
    {
        GridTableBand* band = (*it);
        if (!band->columnGP()->visibility())
            continue;

        int row = 0;
        int maxVisualRowIndex = 0;
        if (!m_columnHeightTable.contains(visualRowIndex))
        {
            QSize defaultSize = get_column_sizeHint(Q_NULL, control()->style(), view()->getColumnFont(band), view()->optionsref()->zoomFactor());
            m_columnHeightTable[visualRowIndex] = defaultSize.height();
        }

        int columnCount = band->m_nestedCols.size();
        while (columnCount > 0)
        {
            int currentVisualRowIndex = 0;
            for (GridBandedTableColumns::const_iterator jit = band->m_nestedCols.constBegin();
                jit != band->m_nestedCols.constEnd(); ++jit)
            {
                if ((*jit)->rowIndex() == row)
                {
                    columnCount--;
                    if (!(*jit)->columnGP()->visibility())
                        continue;

                    QSize s = (*jit)->columnGP()->sizeHint();

                    if (ignoreColumn != Q_NULL && ignoreColumn->rowIndex() == row)
                        s.rwidth() = ignoreColumn->exactWidth();
                    else
                        s.rwidth() = (*jit)->exactWidth();

                    if (!m_columnWidthTable.contains(row) || s.width() > m_columnWidthTable[row])
                        m_columnWidthTable[row] = s.width();

                    if (!m_columnHeightTable.contains(visualRowIndex + currentVisualRowIndex) || s.height() > m_columnHeightTable[visualRowIndex + currentVisualRowIndex])
                        m_columnHeightTable[visualRowIndex + currentVisualRowIndex] = s.height();

                    for (int i = 1; i < (*jit)->rowSpan(); ++i)
                    {
                        if (!m_columnHeightTable.contains(visualRowIndex + currentVisualRowIndex + i))
                        {
                            QSize defaultSize = get_column_sizeHint(Q_NULL, control()->style(), view()->getColumnFont(*jit), view()->optionsref()->zoomFactor());
                            m_columnHeightTable[visualRowIndex + currentVisualRowIndex + i] = defaultSize.height();
                        }
                    }
                    currentVisualRowIndex += (*jit)->rowSpan();

                    if (currentVisualRowIndex > maxVisualRowIndex)
                        maxVisualRowIndex = currentVisualRowIndex;

                    if (verticalLayout)
                        currentVisualRowIndex = 0;
                }
            }
            if (verticalLayout)
            {
                visualRowIndex += maxVisualRowIndex;
                maxVisualRowIndex = 0;
            }
            row++;
        }
        if (verticalLayout)
            visualRowIndex = 0;
        else
        {
            if (maxVisualRowIndex == 0)
                maxVisualRowIndex = 1;

            if (band->fixedKind() == FixedLeft)
            {
                if (firstband)
                    m_leftFixedIndent += m_bandedView->getRowGroupIndent(Q_NULL);
                firstband = false;
                for (int i = visualRowIndex; i < visualRowIndex + maxVisualRowIndex; ++i)
                    m_leftFixedIndent += m_columnHeightTable[i];
            }
            else if (band->fixedKind() == FixedRight)
            {
                for (int i = visualRowIndex; i < visualRowIndex + maxVisualRowIndex; ++i)
                    m_rightOrBottomFixedIndent += m_columnHeightTable[i];
            }

            visualRowIndex += maxVisualRowIndex;
        }
    }
}

void GridBandedTableHeaderGraphic::createBandTable()
{
    bool verticalLayout = view()->layout()->orientation() == Qt::Vertical;
    m_visibleColumns.clear();

    m_bandItemSizes.clear();

    GridTableBand* ignoreBand = qobject_cast<GridTableBand *>(m_ignoreColumn);
    for (GridTableBands::const_iterator it = m_bandedView->m_rootBands.constBegin(); it != m_bandedView->m_rootBands.constEnd(); ++it)
    {
        GridTableBand* band = *it;
        if (!band->columnGP()->visibility())
            continue;
        m_visibleColumns.push_back(band);
    }

    for (QHash<const GridTableBand *, GridTableBandPrimitive *>::const_iterator it = m_bandGPsHash.constBegin(); it != m_bandGPsHash.constEnd(); ++it)
    {
        GridTableBandPrimitive* bandGP = it.value();
        if (!bandGP->visibility())
            continue;

        int depth = static_cast<GridTableBand *>(bandGP->column())->depth();

        QSize s = bandGP->sizeHint();

        if (!verticalLayout)
        {
            if (ignoreBand != Q_NULL && ignoreBand->depth() == depth)
                s.rwidth() = ignoreBand->exactWidth();
            else
                s.rwidth() = bandGP->column()->exactWidth();
        }

        if (!m_bandItemSizes.contains(depth))
            m_bandItemSizes[depth] = QSize(0, 0);
        if (s.width() > m_bandItemSizes[depth].width())
            m_bandItemSizes[depth].rwidth() = s.width();
        if (s.height() > m_bandItemSizes[depth].height())
            m_bandItemSizes[depth].rheight() = s.height();
    }
}


int GridBandedTableHeaderGraphic::calculateMaxColumnWidthForView(GridTableColumnPrimitive* columnGP, int viewWidth)
{
    if (viewWidth < 0)
        return qRound(columnGP->maxSize().width() * view()->options().zoomFactor());

    double deltaX = 0;
    const GridTableColumnBase* cur = columnGP->column();
    while (cur != Q_NULL)
    {
        GridTableColumnPrimitive* curGP = getPrimitiveByColumn(cur);
        deltaX += GridTableHeaderGraphic::calculateMaxColumnWidthForView(curGP, view()->getColumnViewWidth(cur)) - curGP->width();

        cur = get_parent_band(cur);
    }
    int retval = columnGP->width() + deltaX;

    int max = qRound(columnGP->maxSize().width() * view()->options().zoomFactor());

    if (retval > max)
        retval = max;
    return retval;
}
