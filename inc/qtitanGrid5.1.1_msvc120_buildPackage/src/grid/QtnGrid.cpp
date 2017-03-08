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
#include <QPrintPreviewDialog>
#include <QBitmap>
#include <QMessageBox>
#include <QTranslator>
#include <QAbstractTextDocumentLayout>

#include "QtnGrid.h"
#include "QtnGridTableView.h"
#include "QtnGridBandedTableView.h"
#include "QtnGridCardView.h"

#include "QtnGridViewGraphics.h"
#include "QtnGridTableViewBase.h"
#include "QtnGridFilterWindow.h"

#ifdef QTITAN_DEMO
#include "QtnDemo.h"
#endif

QTITAN_USE_NAMESPACE

/*!
    \enum GridActionType
    \internal
*/

/*!
\class GridViewBase::EditContext
\internal
*/



QTITAN_BEGIN_NAMESPACE

class GridViewInterfaceImpl;
class GridViewBaseEx: public GridViewBase
{
public:
    GridViewBaseEx() : GridViewBase(0) {}
    friend class Qtitan::GridViewInterfaceImpl;
};

class GridViewInterfaceImpl : public GridViewInterface
{
public:
    GridViewInterfaceImpl(GridViewBase* base)
        : m_base((GridViewBaseEx *)base)
    {
    }
        
    virtual void controllerChanged(GridControllerChanges changes)
    {
        m_base->controllerChanged(changes);
    }

    virtual void raiseQueryRelation(RelationEventArgs* args)
    {
        emit m_base->relationConnecting(args);
    }

    virtual void raiseGroupExpanding(GridModelGroup* group, bool& allow)
    {
        emit m_base->groupExpanding(group, allow);
    }
        
    virtual void raiseGroupExpanded(GridModelGroup* group)
    {
        emit m_base->groupExpanded(group);
    }

    virtual void raiseGroupCollapsing(GridModelGroup* group, bool& allow)
    {
        emit m_base->groupCollapsing(group, allow);
    }

    virtual void raiseGroupCollapsed(GridModelGroup* group)
    {
        emit m_base->groupCollapsed(group);
    }

    virtual void raiseFilterActivated()
    {
        emit m_base->filterActivated();
    }

    virtual void raiseFilterDeactivated()
    {
        emit m_base->filterDeactivated();
    }

    virtual void raiseSelectionChanged(GridSelection* selection, GridSelection* oldSelection)
    {
       emit m_base->selectionChanged(selection, oldSelection); 
    }

    virtual GridModelDataBinding* dataBindingByColumnIndex(int columnIndex)
    {
        GridColumnBase* column = m_base->getColumn(columnIndex);
        if (column == Q_NULL)
            return Q_NULL;
        return m_base->getDataBinding(column);
    }
   
    virtual void resetModel()
    {
        if (m_base->optionsref()->autoCreateColumns() && 
            m_base->getColumnCount() != m_base->modelController()->model()->columnCount(m_base->modelController()->parentIndex()))
        {
            m_base->hideFilterPanel();
            m_base->removeColumns();
            m_base->addColumns();
        }
        m_base->requestUpdateLayout();
    }
        
    virtual void modelDestroyed()
    {
        m_base->setModel(Q_NULL);
    }

    virtual void columnInserted(int modelColumn)
    {
        for (GridViewBase::GridColumns::const_iterator it = m_base->m_columnslist.constBegin(); it != m_base->m_columnslist.constEnd(); ++it)
        {
            const GridDataTableColumn* tableColumn = qobject_cast<const GridDataTableColumn *>(*it);
            Q_ASSERT(tableColumn != Q_NULL);
            if (tableColumn->dataBinding()->column() >= modelColumn)
                tableColumn->dataBinding()->setColumn(tableColumn->dataBinding()->column() + 1);
        }
        m_base->addColumn(modelColumn);
    }


    virtual void columnRemoved(int modelColumn)
    {
        GridDataTableColumn* removed = Q_NULL;
        for (GridViewBase::GridColumns::const_iterator it = m_base->m_columnslist.constBegin(); it != m_base->m_columnslist.constEnd(); ++it)
        {
            GridDataTableColumn* tableColumn = qobject_cast<GridDataTableColumn *>(*it);
            Q_ASSERT(tableColumn != Q_NULL);
            if (removed != Q_NULL)
            {
                tableColumn->dataBinding()->setColumn(tableColumn->dataBinding()->column() - 1);
            }
            else if (tableColumn->dataBinding()->column() == modelColumn)
                removed = tableColumn;
        }
        if (removed != Q_NULL)
            m_base->removeColumn(m_base->m_columnslist.indexOf(removed));
    }

    virtual bool isDestroying()
    {
        return m_base->m_viewStates & ViewDestroying;
    }

private:
    GridViewBaseEx* m_base;
};

QTITAN_END_NAMESPACE

/* GridHitInfo */
GridHitInfo::GridHitInfo(Info info, GridViewBase* viewBase, GridRow* gridRow, int columnIndex)
    : m_info(info), m_view(viewBase), m_cell(Q_NULL)
{
    m_cell = new GridCell(gridRow, columnIndex);
}

GridHitInfo::~GridHitInfo()
{
    Q_DELETE_AND_NULL(m_cell);
}

GridHitInfo::Info GridHitInfo::info() const
{
    return m_info;
}

GridViewBase* GridHitInfo::view() const
{
    return m_view;
}

GridRow* GridHitInfo::row() const
{
    return m_cell->row();
}

int GridHitInfo::columnIndex() const
{
    return m_cell->columnIndex();
}

QModelIndex GridHitInfo::modelIndex() const
{
    return m_cell->modelIndex();
}

/*!
\class GridThemeManager
\inmodule QtitanDataGrid
GridThemeManager is used to customize rendering of the text, icons, background of the main parts of the grid. To get GridThemeManager from your code use the following snippet:
\code
Grid* grid = new Grid(parent);
GridThemeManager* themeManager = grid->themeManager();
themeManager->setShadeColor(color);
\endcode
*/

/*!
\enum GridThemeManager::IconKind
\inmodule QtitanDataGrid
The enumerator describe a kind for an icons.
\value QuickButtonIcon quick button icon
\value FocusRowIndicatorIcon icon of row that has a focus
\value FilterToolButtonIcon icon of a filter tool button
\value FilterToolButtonBoldIcon icon of a filter tool button if mouse is over button
\value ColumnMenuToolButtonIcon icon of menu tool button
\value SortIndicatorASCIcon ascending sort indicator
\value SortIndicatorDESCIcon descending sort indicator
\value FrozenPinOnIcon pin-on icon for row
\value FrozenPinOffIcon pin-off icon for row
\value FrozenPlaceTopIcon icon on button to select place for rows freezing on top
\value FrozenPlaceBottomIcon icon on button to select place for rows freezing on bottom
\value DragDropBoxIcon icon for drag-drop operation
\value FocusRowEditIcon icon designates a row in edit mode
\value FocusRowEditModifyIcon icon designates a modified row in edit mode
\value FocusRowEditValidIcon icon designates a modified and validated row in edit mode
\value ZoomInIcon zoom-in icon for context menu
\value ZoomOutIcon zoom-out icon for context menu
\value NewRowIcon icon designates a new row
\value TextButtonIcon icon for cell's button - Text
\value ClearButtonIcon icon for cell's button - Clear
\value ChoiceButtonIcon icon for cell's button - Choice 
\value SettingsButtonIcon icon for cell's button - Settings
\value SelectButtonIcon icon for cell's button - Select
\value FieldChooserIcon field chooser icon for context menu
\value GroupByBoxIcon group by box icon for context menu
\value GroupByColumnIcon group by column icon for context menu
\value AlignLeftIcon align left icon for popup menu
\value AlignCenterIcon align center icon for context menu
\value AlignRightIcon align right icon for context menu
\value FixedLeftIcon fixed left icon for column's context menu
\value FixedRightIcon fixed right icon for column's context menu
*/

/*!
Contructor of GridThemeManager. You can't create a class from your code directly. 
*/
GridThemeManager::GridThemeManager(Grid* grid)
    : m_grid(grid)
{
    setShadeColor(Qt::darkGray);
}

/*!
Allows to specify a shade \a color for all items of the grid. This is effective when you want to adjust quickly the colors of grid's elements to the brand-color of your company.
Shade color is used to generate a colored shade in icons. 
\sa shadeColor(), getIcon(), setIcon(), shadePalette()
*/
void GridThemeManager::setShadeColor(const QColor& color)
{
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    palette.setColor(QPalette::Dark, color.darker());
    palette.setColor(QPalette::Light, color.lighter(350));
    palette.setColor(QPalette::Background, color.darker(300));
    palette.setColor(QPalette::Foreground, color.lighter());
    palette.setColor(QPalette::Mid, color);
    palette.setColor(QPalette::BrightText, color.lighter(380));
    palette.resolve();
    setShadePalette(palette);
}

/*!
Returns a current shade color that used for rendering grid's elements.
*/
const QColor& GridThemeManager::shadeColor() const
{
    return m_shadePalette.mid().color();
}

/*!
Returns a current shade palette that used for rendering grid's elements.
*/
const QPalette& GridThemeManager::shadePalette() const
{
    return m_shadePalette;
}

/*!
Allows you to fine-tune \a palette of different parts of the grid in order to better match your corporate style.
*/
void GridThemeManager::setShadePalette(const QPalette& palette)
{
    m_shadePalette = palette;

    m_shadePen.setColor(palette.base().color().darker(180));

    m_shadeBackground = qtn_create_background(palette.base().color());

    m_iconCache.clear();

    if (m_grid->rootView() != Q_NULL)
    {
        m_grid->rootView()->requestUpdateLayout();
        m_grid->rootView()->setActionIcons();
    }

}

/*!
Returns QPen that is based on shade color.
*/
const QPen& GridThemeManager::shadePen() const
{
    return m_shadePen;
}


#ifdef _DEBUG
static void check_icon(const QImage& image)
{
    for (int x = 0; x < image.width(); ++x)
    {
        for (int y = 0; y < image.height(); ++y)
        {
            QRgb rgb = image.pixel(x, y);
            int alpha = qAlpha(rgb);
            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);
            if (r != 0 || g != 0 || b != 0)
            {
                //Q_ASSERT(false);
            }
            alpha = alpha;
        }
    }
}
#endif

void Qtitan::qtn_set_shadecolor(QImage& image, const QColor& color)
{
#ifdef _DEBUG
    check_icon(image);
#endif
    for (int x = 0; x < image.width(); ++x)
    {
        for (int y = 0; y < image.height(); ++y)
        {
            QRgb rgb = image.pixel(x, y);
            int alpha = qAlpha(rgb);
            if (alpha > 0 && qRed(rgb) == 0 && qGreen(rgb) == 0 && qBlue(rgb) == 0)
            {
                rgb = qRgba(color.red(), color.green(), color.blue(), alpha);
                image.setPixel(x, y, rgb);
            }
        }
    }
}

/*!
Returns QPalette based on shade color.
\internal
*/
QPalette GridThemeManager::createGroupPalette(int depth)
{
    QPalette ret = m_grid->rootView()->optionsref()->cellPalette();
    ret.setColor(QPalette::BrightText, m_shadePalette.brightText().color());
    ret.setColor(QPalette::Dark, m_shadePalette.base().color().darker(100 + (depth % 10) * 20));
    return ret;
}

/*!
Returns QPixmap that is based on shade color and used for rendering backgroud of group panel, field chooser, filter panel and etc.
*/
const QPixmap& GridThemeManager::shadeBackground() const
{
    return m_shadeBackground;
}

/*!
Sets shade \a background for grid's panels. Please note it will be reset to the default value after setShadeColor() call. 
*/
void GridThemeManager::setShadeBackground(const QPixmap& background)
{
    m_shadeBackground = background;
    if (m_grid->rootView() != Q_NULL)
    {
        m_grid->rootView()->requestUpdateLayout();
        m_grid->rootView()->setActionIcons();
    }
}

/*!
Returns an icon by \a kind. Icon is processed before return, all of the pixels that have the value RED=0, GREEN=0, BLUE=0 are replaced by the shade color and then icon is scaled to \a size.
\sa IconKind
*/
QPixmap GridThemeManager::getIcon(IconKind kind, const QSize& size)
{
    QString resname;
    switch (kind)
    {
    case QuickButtonIcon:
    {
        resname = ":res/grid_quickbutton.png";
    }
    break;
    case FocusRowIndicatorIcon:
    {
        resname = ":res/grid_focusrowselect.png";
    }
    break;
    case FilterToolButtonIcon:
    {
        resname = ":res/grid_filter.png";
    }
    break;
    case FilterToolButtonBoldIcon:
    {
        resname = ":res/grid_filter_over.png";
    }
    break;
    case ColumnMenuToolButtonIcon:
    {
        resname = ":res/grid_menu_button_icon.png";
    }
    break;
    case SortIndicatorASCIcon:
    {
        resname = ":res/grid_sort_indicator_asc.png";
    }
    break;
    case SortIndicatorDESCIcon:
    {
        resname = ":res/grid_sort_indicator_desc.png";
    }
    break;
    case FrozenPinOnIcon:
    {
        resname = ":res/grid_frozen_pin_on.png";
    }
    break;
    case FrozenPinOffIcon:
    {
        resname = ":res/grid_frozen_pin_off.png";
    }
    break;
    case FrozenPlaceTopIcon:
    {
        resname = ":res/grid_frozen_place_top.png";
    }
    break;
    case FrozenPlaceBottomIcon:
    {
        resname = ":res/grid_frozen_place_bottom.png";
    }
    break;
    case DragDropBoxIcon:
    {
        resname = ":res/grid_dragdrop_icon.png";
    }
    break;
    case NewRowIcon:
    {
        resname = ":res/grid_newrow_icon.png";
    }
    break;
    case FocusRowEditIcon:
    {
        resname = ":res/grid_rowedit_icon.png";
    }
    break;
    case FocusRowEditModifyIcon:
    {
        resname = ":res/grid_rowedit_modify_icon.png";
    }
    break;
    case FocusRowEditValidIcon:
    {
        resname = ":res/grid_rowedit_valid_icon.png";
    }
    break;
    case ZoomInIcon:
    {
        resname = ":res/grid_zoom_in_icon.png";
    }
    break;
    case ZoomOutIcon:
    {
        resname = ":res/grid_zoom_out_icon.png";
    }
    break;
    case TextButtonIcon:
    {
        resname = ":res/grid_text_btn.png";
    }
    break;
    case ClearButtonIcon:
    {
        resname = ":res/grid_clear_btn.png";
    }
    break;
    case ChoiceButtonIcon:
    {
        resname = ":res/grid_choice_btn.png";
    }
    break;
    case SelectButtonIcon:
    {
        resname = ":res/grid_select_btn.png";
    }
    break;
    case SettingsButtonIcon:
    {
        resname = ":res/grid_settings_btn.png";
    }
    break;
    case FieldChooserIcon:
    {
        resname = ":res/grid_fieldchooser_icon.png";
    }
    break;
    case GroupByBoxIcon:
    {
        resname = ":res/grid_groupbybox_icon.png";
    }
    break;
    case GroupByColumnIcon:
    {
        resname = ":res/grid_groupbyfield_icon.png";
    }
    break;
    case AlignLeftIcon:
    {
        resname = ":res/grid_align_left_icon.png";
    }
    break;

    case AlignCenterIcon:
    {
        resname = ":res/grid_align_center_icon.png";
    }
    break;
    case AlignRightIcon:
    {
        resname = ":res/grid_align_right_icon.png";
    }
    break;
    case FixedLeftIcon:
    {
        resname = ":res/grid_fixed_left_icon.png";
    }
    break;
    case FixedRightIcon:
    {
        resname = ":res/grid_fixed_right_icon.png";
    }
    break;

    default:
        resname = "";
    }
    Q_ASSERT(resname != "");

    IconCache::iterator name_it = m_iconCache.find(resname);
    if (name_it == m_iconCache.end())
        name_it = m_iconCache.insert(resname, IconSizeMap());

    IconSizeMap::iterator size_it = name_it.value().find(size);
    if (size_it == name_it->end())
    {
        QPixmap pixmap;
        CustomIconMap::const_iterator custom_icon_it = m_customIconMap.constFind(kind);
        if (custom_icon_it == m_customIconMap.constEnd())
        {
            QImage image = QImage(resname);
            qtn_set_shadecolor(image, shadeColor());
            image = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            pixmap = QPixmap::fromImage(image);
        }
        else
        {
            pixmap = custom_icon_it.value().scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        size_it = name_it->insert(size, pixmap);
    }
    return size_it.value();
}

/*!
Sets a custom \a pixmap icon by \a kind. It is used to specify icon in run-time for various parts of the grid.
\sa IconKind
*/
void GridThemeManager::setIcon(IconKind kind, const QPixmap& pixmap)
{
    if (pixmap.isNull())
        m_customIconMap.remove(kind);
    else
        m_customIconMap.insert(kind, pixmap);
}

/*!
    \class Grid
    \inmodule QtitanDataGrid
    \brief Class is used to add QtitanDataGrid to the form. 
    It is descendant of QWidget so you can use any QWidget's pattern to locate grid into the layout of other widget.
    After class was created you need to specify the type of view wich will be used to show data. 
    The Qt-model's should be attached to this view. Not to this class directly.
*/
/*!
Constructor of the class Qtitan::Grid. The parameter \c parent is a widget wtich will be parent and owner for the grid.
*/
Grid::Grid(QWidget* parent)
: GraphicControl(parent), m_view(Q_NULL), m_viewType(UndefinedView)
{
    setAutoFillBackground(false);
    setBackgroundRole(QPalette::Base);
    ensurePolished();
    m_themeManager = new GridThemeManager(this);
#ifdef QTITAN_DEMO
START_QTITAN_DEMO
#endif
}

/*!
Destuctor of the class Qtitan::Grid.
*/
Grid::~Grid()
{
    Q_DELETE_AND_NULL(m_view);
    Q_DELETE_AND_NULL(m_themeManager);
}

/*!
\fn ViewType* Grid::view() const
Returns the instance of the view for the current grid. The function is a C++ template. It's allow you to get the view desired type.

\code
Qtitan::Grid* grid = new Qtitan::Grid();
grid->setViewType(Qtitan::TableView);
GridTableView* view = grid->view<GridTableView>();
\endcode
 
 Alternatively, there is a method rootViev() which returns a reference to a base class of the view. This reference must be converted into the desired type of view by youself.
 If the viewType is UndefinedView then this function return NULL.
\sa setViewType(), viewType()
*/

/*!
Returns the root view type for the current grid.
\sa setViewType()
*/
Grid::GridViewType Grid::viewType() const
{
    return (Grid::GridViewType)m_viewType;
}

/*!
\internal
*/
GridViewBase* Grid::createView(Grid::GridViewType type)
{
    switch (type)
    {
    case TableView:
        {
            return GridTableView::create(this);
        }
        break;
    case BandedTableView:
        {
            return GridBandedTableView::create(this);
        }
        break;
    case CardView:
    {
        return GridCardView::create(this);
    }
    break;
    default:
        Q_ASSERT(false);
        break;
    }
    return Q_NULL;
}

/*!
Sets the root view type for the current grid. The view might be table view or banded table view. 
Banded table view allows to grouping columns in the logical structure called bands. 
The band looks like ordinary column but has not the cell. After the type of the column has been specified the 
instance of the view can be obtained using view() function.
\sa viewType(), view()
*/
void Grid::setViewType(Grid::GridViewType type)
{
    Q_DELETE_AND_NULL(m_view);
    m_viewType = type;
    setRootView(createView(type));
}

void Grid::paintEvent(QPaintEvent* e)
{
    if (m_rootGP == Q_NULL)
    {
        QPainter painter(this);
        painter.setPen(Qt::gray);
        painter.setFont(QFont("Arial"));
        QStyleOptionHeader option;
        option.rect = rect();
        option.state = QStyle::State_Enabled;
        option.palette = palette();
        option.palette.setColor(QPalette::Light, Qt::gray);
        painter.setPen(Qt::gray);
        QPixmap icon(":res/qtitanlogo32x32.png");
        QString text = Grid::tr_compatible(QtnGridIsNotConfiguredString);
        int w = painter.fontMetrics().width(text);
        w = qMin(width(), w + icon.width() + 10);
        QRect r = QStyle::alignedRect(layoutDirection(), Qt::AlignCenter, QSize(w, height()), rect());
        paint_icon_and_text(painter, r, icon, text, Qt::AlignCenter);
        painter.drawRect(rect().adjusted(0, 0, -1, -1));
    }
    else
        GraphicControl::paintEvent(e);
}

/*!
Returns a reference to a base class of the view. This reference must be converted into the desired type of view by youself.
If the viewType is UndefinedView then this function return NULL.
\code
Qtitan::Grid* grid = new Qtitan::Grid();
grid->setViewType(Qtitan::TableView);
GridTableView* view = qobject_cast<GridTableView *>(grid->rootView());
\endcode

\sa viewType(), setViewType(), view()
*/
GridViewBase* Grid::rootView() const
{
    return m_view;
}

/*!
Sets the view instance to the grid. For internal purpose only.
*/
void Grid::setRootView(GridViewBase* view)
{
    if (m_view != Q_NULL)
    {
        // Delete old view and finalize all its parts here.
        delete m_view;
    }
    
    m_view = view;

    if (m_view != Q_NULL)
    {
        setRoot(m_view->layout());
        m_view->optionsref()->setScrollBars(ScrollAuto);
    }
    else
    {
        setRoot(Q_NULL);
    }

    update();
}

/*!
\internal
*/
GridNativeDragObject* Grid::createNativeDragDropObject(const QPoint& pos)
{
    if (m_view == Q_NULL)
        return GraphicControl::createNativeDragDropObject(pos);
    return m_view->createNativeDragDropObject(pos);
}

bool Grid::isLocked() const
{
    if (m_view == Q_NULL)
        return false;
    return m_view->isUpdating();
}

#ifndef QTN_NOUSE_XML_MODULE
/*!
Save layout of the internals view to the file given by filename.
The layout includes column's or band's sizing, order of grouping, visual position of the elements and etc.
\sa saveLayoutToDevice()
*/
bool Grid::saveLayoutToFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    return saveLayoutToDevice(&file);
}

/*!
Load layout of the internals view from the given device.
\sa loadLayoutFromDevice()
*/
bool Grid::loadLayoutFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    return loadLayoutFromDevice(&file);
}

/*!
 Save layout of the internals view to the given device. 
The layout includes column's or band's sizing, order of grouping, visual positions of the elements and etc.
\sa saveLayoutToFile()
*/
bool Grid::saveLayoutToDevice(QIODevice* device)
{
    if (rootView() == Q_NULL)
        return false;
    QXmlStreamWriter xmlwriter(device);
    return rootView()->saveLayoutToXML(&xmlwriter);
}

/*!
Load layout of the internals view from the given device.
\sa loadLayoutFromFile()
*/
bool Grid::loadLayoutFromDevice(QIODevice* device)
{
    if (rootView() == Q_NULL)
        return false;
    QXmlStreamReader xmlreader(device);
    return rootView()->loadLayoutFromXML(&xmlreader);
}
#endif

/*!
Returns an optimal size of the grid.
*/
QSize Grid::sizeHint() const
{
    return QSize(200, 200);
}

/*!
Returns a hint info by given point. The method is used to determine the type of element which is located 
at the given point of the grid.
*/
GridHitInfo* Grid::hitInfo(const QPoint& pos)
{
    GraphicPrimitive* p = hitTest(pos);
    if (p == Q_NULL)
        return new GridHitInfo(GridHitInfo::Unknown, m_view, 0, -1);

    switch ((GridHitInfo::Info)p->id())
    {
    case GridHitInfo::Column:
        {
            GridColumnPrimitive* columnGP = static_cast<GridColumnPrimitive *>(p);
            return new GridHitInfo(GridHitInfo::Column, m_view, 0, 
                columnGP->m_column != Q_NULL ? columnGP->m_column->index() : 0);
        }
        break;
    case GridHitInfo::ColumnFilterButton:
        {
            GridTableColumnFilterButton* filterButtonGP = static_cast<GridTableColumnFilterButton *>(p);
            return new GridHitInfo(GridHitInfo::ColumnFilterButton, m_view, 0, 
                filterButtonGP->column() != Q_NULL ? filterButtonGP->column()->index() : 0);
        }
        break;
    case GridHitInfo::Band:
        {
            GridColumnPrimitive* bandGP = static_cast<GridColumnPrimitive *>(p);
            return new GridHitInfo(GridHitInfo::Band, m_view, 0, 
                bandGP->m_column != Q_NULL ? bandGP->m_column->index() : 0);
        }
        break;
    case GridHitInfo::Cell:
        {
            GridTableCellGraphic* cellGP = static_cast<GridTableCellGraphic *>(p);
            return new GridHitInfo(GridHitInfo::Cell, m_view, cellGP->gridRow(), cellGP->columnIndex());
        }
        break;
    case GridHitInfo::Row:
        {
            GridTableDataRowPrimitive* rowGP = static_cast<GridTableDataRowPrimitive *>(p);
            return new GridHitInfo(GridHitInfo::Row, m_view, rowGP->rowLayoutItem()->gridRow(), -1);
        }
        break;
    case GridHitInfo::CellButton:
        {
            GridTableCellButtonGraphic* cellBtnGP = static_cast<GridTableCellButtonGraphic *>(p);
            return new GridHitInfo(GridHitInfo::CellButton, m_view, 
                cellBtnGP->m_cell != 0 ? cellBtnGP->m_cell->gridRow() : Q_NULL,
                cellBtnGP->m_cell != 0 ? cellBtnGP->m_cell->columnIndex() : -1);
        }
        break;
    case GridHitInfo::GroupSummaryCell:
        {
            GridTableGroupSummaryCellGraphic* summaryGroupCellGP = static_cast<GridTableGroupSummaryCellGraphic *>(p);
            return new GridHitInfo(GridHitInfo::GroupSummaryCell, m_view,
                summaryGroupCellGP->groupRow(), 
                summaryGroupCellGP->tableColumn() != Q_NULL ? summaryGroupCellGP->tableColumn()->index() : -1);

        }
        break;
    case GridHitInfo::FooterSummaryCell:
        {
            GridTableFooterSummaryCellGraphic* summaryFooterCellGP = static_cast<GridTableFooterSummaryCellGraphic *>(p);
            return new GridHitInfo(GridHitInfo::FooterSummaryCell, m_view, Q_NULL, 
                summaryFooterCellGP->tableColumn() != Q_NULL ? summaryFooterCellGP->tableColumn()->index() : -1);
        }
        break;
    case GridHitInfo::GroupRow:
        {
            GridGroupRowPrimitive* rowGP = static_cast<GridGroupRowPrimitive *>(p);
            return new GridHitInfo(GridHitInfo::GroupRow, m_view, rowGP->rowLayoutItem()->gridRow(), -1);
        }
        break;
    default:
        break;
    }

    
    return new GridHitInfo((GridHitInfo::Info)p->id(), m_view, Q_NULL, -1);
}

GridThemeManager* Grid::themeManager() const
{
    return m_themeManager;
}

/*!
Installs translator to QApplication based on \a country variable. Format variable is de_de, ru_ru, fr_fr. If \a country is empty then loaded QTranslator based on current locale setting. 
*/
bool Grid::loadTranslation(const QString& country)
{
    static bool loaded = false;
    if (loaded)
        return true;

    const QString directory = QLatin1String(":/translations/grid");

    QTranslator* translator = new QTranslator(qApp);
    if (country.isEmpty())
    {
        foreach(const QLocale locale, QLocale().uiLanguages())
        {
            if (translator->load(locale, QLatin1String(""), QLatin1String(""), directory))
            {
                qApp->installTranslator(translator);
                loaded = true;
                return true;
            }
        }
    }
    else
    {
        if (translator->load(country, directory))
        {
            qApp->installTranslator(translator);
            loaded = true;
            return true;
        }
    }

    delete translator;
    return false;
}

QString Grid::tr_compatible(const char *s, const char *c)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return QCoreApplication::translate("Qtitan::Grid", s, c);
#else
    return QCoreApplication::translate("Qtitan::Grid", s, c, QCoreApplication::CodecForTr);
#endif
}

/*!
  \class DragDropArgs
  \inmodule QtitanDataGrid
*/
DragDropArgs::DragDropArgs()
: BaseEventArgs(), m_mimeData(Q_NULL), m_supportedDropActions(Qt::MoveAction | Qt::CopyAction),
  m_defaultDropAction(Qt::IgnoreAction),
  m_dropAction(Qt::IgnoreAction), m_accepted(true)
{
}

QMimeData* DragDropArgs::mimeData() const
{
    return m_mimeData;
}

void DragDropArgs::setMimeData(QMimeData* data)
{
    m_mimeData = data;
}

Qt::DropActions DragDropArgs::supportedDropActions() const
{
    return m_supportedDropActions;
}

void DragDropArgs::setSupportedDropActions(Qt::DropActions actions)
{
    m_supportedDropActions = actions;
}

Qt::DropAction DragDropArgs::defaultDropAction() const
{
    return m_defaultDropAction;
}

void DragDropArgs::setDefaultDropAction(Qt::DropAction action)
{
    m_defaultDropAction = action;
}

Qt::DropAction DragDropArgs::dropAction() const
{
    return m_dropAction;
}

void DragDropArgs::setDropAction(Qt::DropAction action)
{
    m_dropAction = action;
}

bool DragDropArgs::isAccepted() const
{
    return m_accepted;
}

void DragDropArgs::setAccepted(bool accepted)
{
    m_accepted = accepted;
}

/*!
  \class DragArgs
  \inmodule QtitanDataGrid
*/
DragArgs::DragArgs()
: DragDropArgs()
{
}

const QModelIndexList& DragArgs::indexes() const
{
    return m_indexes;
}

void DragArgs::setIndexes(const QModelIndexList& indexes)
{
    m_indexes = indexes;
}

/*!
  \class NewRowEventArgs
  \inmodule QtitanDataGrid
*/
NewRowEventArgs::NewRowEventArgs()
    : BaseEventArgs(), m_modified(false)
{
}

bool NewRowEventArgs::isModified() const
{
    return m_modified;
}

void NewRowEventArgs::setModified(bool modified)
{
    m_modified = modified;
}

/*!
  \class ColumnGroupArgs
  \inmodule QtitanDataGrid
*/
ColumnGroupArgs::ColumnGroupArgs(GridColumnBase* column, int groupIndex)
    : BaseEventArgs(), m_column(column), m_groupIndex(groupIndex)
{
}

GridColumnBase* ColumnGroupArgs::column() const
{
    return m_column;
}

int ColumnGroupArgs::groupIndex() const
{
    return m_groupIndex;
}

/* GridRowLayoutItem */
GridRowLayoutItem::GridRowLayoutItem(GridRowsLayoutEngine* rowsLayout)
    : m_rowsLayout(rowsLayout), m_view(rowsLayout->m_view),
    m_gridRow(Q_NULL),
    m_hidden(true),
    m_opacity(1.0),
    m_quickRowButtonGP(Q_NULL),
    m_dataRowGP(Q_NULL),
    m_groupRowGP(Q_NULL),
    m_summaryRowGP(Q_NULL),
    m_summarylayoutEngine(Q_NULL)
{
}

GridRowLayoutItem::~GridRowLayoutItem()
{
    m_view = Q_NULL;
    m_rowsLayout = Q_NULL;
    Q_DELETE_AND_NULL(m_summarylayoutEngine);
    Q_DELETE_AND_NULL(m_summaryRowGP);
    Q_DELETE_AND_NULL(m_quickRowButtonGP);
    Q_DELETE_AND_NULL(m_dataRowGP);
    Q_DELETE_AND_NULL(m_groupRowGP);
}

void GridRowLayoutItem::create()
{
    createQuickRowButtonGraphic();
    createDataRowGraphic();
    createGroupGraphic();
    createSummaryPanelGraphic();
    updateSummaryLayoutParent();
    registrColumns();
}


void GridRowLayoutItem::updateSummaryLayoutParent()
{
    GraphicPrimitive* p = Q_NULL;
    if (m_view->optionsref()->groupSummaryPlace() == GroupSummaryFooter)
        p = summaryRowGP();
    else if (m_view->optionsref()->groupSummaryPlace() == GroupSummaryRow || m_view->optionsref()->groupSummaryPlace() == GroupSummaryRowPlus)
        p = groupRowGP();
    summarylayoutEngine()->setParentPrimitive(p);
}

GridRowLayoutItem::Type GridRowLayoutItem::type() const
{
    return m_type;
}

GridViewBase* GridRowLayoutItem::viewBase() const
{
    return m_view;
}

void GridRowLayoutItem::initialize(GridRow* row, GridTableRowLayoutItem::Type type)
{
    m_nextRowLayoutItem = Q_NULL;

    if (m_gridRow == row && type == m_type)
        return;

    m_gridRow = row;
    m_type = type;

    if (type == Undefined)
    {
        m_rect = QRect();
        quickRowButtonGP()->setVisibility(false);
        updateGroupRowLayout(false);
        updateDataRowLayout(false);
        updateGroupSummaryLayout(false);
    }
}

GridRow* GridRowLayoutItem::gridRow() const
{
    return m_gridRow;
}

bool GridRowLayoutItem::isRowFocused() const
{
    return this == m_rowsLayout->focusedRowLayoutItem();
    /*
    if (m_gridRow == Q_NULL)
    return false;
    bool ret2 = m_view->modelController()->isRowFocused(m_gridRow) &&
    (m_type == GridTableRowLayoutItem::Data || m_type == GridTableRowLayoutItem::FrozenData ||  m_type == GridTableRowLayoutItem::Group);
    Q_ASSERT(ret == ret2);
    return ret2;
    */
}

bool GridRowLayoutItem::isRowSelected() const
{
    if (m_gridRow == Q_NULL)
        return false;
    return m_view->modelController()->isRowSelected(m_gridRow) &&
        (m_type == GridTableRowLayoutItem::Data || m_type == GridTableRowLayoutItem::FrozenData || m_type == GridTableRowLayoutItem::Group);
}

bool GridRowLayoutItem::isCellSelected(int cellIndex) const
{
    if (m_gridRow == Q_NULL)
        return false;
    return m_view->modelController()->isCellSelected(m_gridRow->cell(cellIndex)) &&
        (m_type == GridTableRowLayoutItem::Data || m_type == GridTableRowLayoutItem::FrozenData || m_type == GridTableRowLayoutItem::Group);
}

bool GridRowLayoutItem::isHidden() const
{
    return m_hidden;
}

void GridRowLayoutItem::setHidden(bool hidden)
{
    m_hidden = hidden;
}

void GridRowLayoutItem::setOpacity(double value)
{
    if (m_opacity == value)
        return;

    m_opacity = value;
    dataRowGP()->setOpacity(m_opacity);
}

void GridRowLayoutItem::setRowMask(const QRect& maskRect)
{
    dataRowGP()->mask().setRect(maskRect);
    groupRowGP()->mask().setRect(maskRect);
    summaryRowGP()->mask().setRect(maskRect);
}

double GridRowLayoutItem::opacity() const
{
    return m_opacity;
}

const QRect& GridRowLayoutItem::indentRect() const
{
    return m_indentRect;
}

bool GridRowLayoutItem::isFirstItem()
{
    return true;
}

void GridRowLayoutItem::updateGroupIndentRect()
{
    int groupIndent = 0;
    if (isFirstItem() && type() != GridTableRowLayoutItem::NewData && type() != GridTableRowLayoutItem::FrozenData)
        groupIndent = m_view->getRowGroupIndent(gridRow());
    if (groupIndent <= 0)
    {
        m_indentRect = QRect(0, 0, 0, 0);
        return;
    }

    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    if (verticalLayout)
        m_indentRect = QRect(0, 0, groupIndent, rect().height());
    else
        m_indentRect = QRect(0, 0, rect().width(), groupIndent);
}


void GridRowLayoutItem::registrColumns()
{
    for (GridViewBase::GridColumns::const_iterator column_it = m_view->m_columnslist.constBegin();
        column_it != m_view->m_columnslist.constEnd(); ++column_it)
    {
        const GridDataTableColumn* column = qobject_cast<const GridDataTableColumn *>(*column_it);
        if (column == Q_NULL)
            continue;
        addColumn(column);
    }
}

void GridRowLayoutItem::updateDataRowLayout(bool visible)
{
    if (!visible && !m_dataRowGP->visibility())
        return;
    m_dataRowGP->setVisibility(visible);
    if (!visible)
        return;

    checkMasked(m_dataRowGP);
    m_dataRowGP->updateBorder();
}

void GridRowLayoutItem::updateGroupRowLayout(bool visible)
{
    if (!visible && !m_groupRowGP->visibility())
        return;
    m_groupRowGP->setVisibility(visible);
    if (!visible)
        return;

    m_groupRowGP->updateBorder();
}

void GridRowLayoutItem::updateGroupSummaryLayout(bool visible)
{
    if (!visible && !m_summaryRowGP->visibility())
        return;
    m_summaryRowGP->setVisibility(visible);
    if (!visible)
        return;

    m_summaryRowGP->updateBorder();
}

GridTableCellGraphic* GridRowLayoutItem::getCellGraphic(int cellIndex)
{
    for (QHash<const GridColumnPrimitive *, GridTableCellGraphic *>::iterator it =
        dataRowGP()->m_dataCells.begin(); it != dataRowGP()->m_dataCells.end(); ++it)
    {
        if (it.key()->column()->index() == cellIndex)
        {
            return it.value();
        }
    }
    return Q_NULL;
}

GridTableQuickRowButtonGraphic* GridRowLayoutItem::quickRowButtonGP() const
{
    return m_quickRowButtonGP;
}

GridDataRowPrimitive* GridRowLayoutItem::dataRowGP() const
{
    return m_dataRowGP;
}

GridGroupRowPrimitive* GridRowLayoutItem::groupRowGP() const
{
    return m_groupRowGP;
}

GridSummaryRowPrimitive* GridRowLayoutItem::summaryRowGP() const
{
    return m_summaryRowGP;
}

GridSummaryLayoutEngine* GridRowLayoutItem::summarylayoutEngine() const
{
    return m_summarylayoutEngine;
}

void GridRowLayoutItem::setRect(const QRect& rect, bool forceUpdate)
{
    if (m_rect == rect && !forceUpdate)
        return;
    m_rect = rect;
    setRectLayout();
}

const QRect& GridRowLayoutItem::rect() const
{
    return m_rect;
}

void GridRowLayoutItem::createQuickRowButtonGraphic()
{
    Q_ASSERT(m_quickRowButtonGP == Q_NULL);
    GraphicPrimitive* p = m_rowsLayout->getRowsParent();
    m_quickRowButtonGP = new GridTableQuickRowButtonGraphic(p, this);
}

void GridRowLayoutItem::setRectLayout()
{
    QRect rowRect = rect();
    bool verticalLayout = m_view->layout()->orientation() == Qt::Vertical;
    int quickButtonLength = m_view->layout()->getQuickButtonIndent();
    bool quickRowButtonVisibility = quickButtonLength > 0;
    quickRowButtonGP()->setVisibility(quickRowButtonVisibility);
    updateGroupIndentRect();
    if (quickRowButtonVisibility)
    {
        QRect r = verticalLayout ? QRect(0, rowRect.top(), quickButtonLength, rowRect.height()) : QRect(rowRect.left(), 0, rowRect.width(), quickButtonLength);
        quickRowButtonGP()->setRect(r);
        quickRowButtonGP()->updateLayout();
        checkMasked(quickRowButtonGP());
    }

    switch (m_type)
    {
    case Data:
    case NewData:
    case FrozenData:
    {
        updateGroupRowLayout(false);
        updateDataRowLayout(true);
        updateGroupSummaryLayout(false);
    }
    break;
    case Group:
    {
        Q_ASSERT(m_gridRow != Q_NULL);
        dataRowGP()->setVisibility(false);
        updateDataRowLayout(false);
        updateGroupSummaryLayout(false);
        updateGroupRowLayout(true);
        if (m_view->optionsref()->groupSummaryPlace() == GroupSummaryRow || 
            m_view->optionsref()->groupSummaryPlace() == GroupSummaryRowPlus)
            summarylayoutEngine()->updateCellsLayout();
    }
    break;
    case Summary:
    {
        updateGroupRowLayout(false);
        updateDataRowLayout(false);
        updateGroupSummaryLayout(true);
    }
    break;

    default:
        updateGroupRowLayout(false);
        updateDataRowLayout(false);
        updateGroupSummaryLayout(false);
        return;
    }
}

void GridRowLayoutItem::updateModelData()
{
    switch (m_type)
    {
    case Data:
    case NewData:
    case FrozenData:
    {
        dataRowGP()->updateCellsData();
    }
    break;
    case Summary:
    {
        summarylayoutEngine()->updateSummaryCellsData();
    }
    break;

    default:
    {
    }
    }
}

GridRowLayoutItem* GridRowLayoutItem::nextRowLayoutItem() const
{
    return m_nextRowLayoutItem;
}

void GridRowLayoutItem::setNextRowLayoutItem(GridRowLayoutItem* row)
{
    m_nextRowLayoutItem = row;
}

void GridRowLayoutItem::update()
{
    if (quickRowButtonGP()->visibility())
        quickRowButtonGP()->update();
    if (dataRowGP()->visibility())
        dataRowGP()->update();
    if (groupRowGP()->visibility())
        groupRowGP()->update();
    if (summaryRowGP()->visibility())
        summaryRowGP()->update();
}

void GridRowLayoutItem::updateCellButtons(const GridColumnPrimitive* columnPrimitive)
{
    QHash<const GridColumnPrimitive *, GridTableCellGraphic *>::iterator data_it = m_dataRowGP->m_dataCells.find(columnPrimitive);
    if (data_it == m_dataRowGP->m_dataCells.end())
        return;
    GridTableCellGraphic* cell = data_it.value();
    cell->removeButtons();

    const GridDataTableColumn* dataColumn = static_cast<const GridDataTableColumn *>(columnPrimitive->column());
    for (QList<GridCellButtonInfo *>::const_iterator it = dataColumn->m_buttons.constBegin(); it != dataColumn->m_buttons.constEnd(); ++it)
        cell->createButton(*it);
}

void GridRowLayoutItem::editorChanging(const GridColumnPrimitive* columnPrimitive, bool reset)
{
    m_dataRowGP->editorChanging(columnPrimitive, reset);
}

void GridRowLayoutItem::setParentGP(GraphicPrimitive* parent)
{
    m_quickRowButtonGP->setParentGP(parent);
    m_dataRowGP->setParentGP(parent);
    m_groupRowGP->setParentGP(parent);
    m_summaryRowGP->setParentGP(parent);
}

QRect GridRowLayoutItem::mapToSite() const
{
    return QRect(layoutEngine()->getRowsParent()->mapToSite(rect().topLeft()), rect().size());
}

void GridRowLayoutItem::checkMasked(GraphicPrimitive* rowPrimitive)
{
    GridRowsPrimitiveAbstract* rowsPanel = m_view->layout()->rowsPanel();
    if (m_type == NewData || m_type == FrozenData)
    {
        rowsPanel->mask().removeGP(rowPrimitive);
    }
    else
    {
        //Scroll-primitives should be masked.
        rowsPanel->mask().addGP(rowPrimitive);
    }
}

/* GridRowsLayoutEngine */
GridRowsLayoutEngine::GridRowsLayoutEngine(GridViewBase* view, GraphicPrimitive* parent)
    : m_view(view), m_rowsParent(parent), m_focusedRowLayoutItem(Q_NULL), m_transparentRow(Q_NULL), m_firstRowLayoutItem(Q_NULL), m_lastRowLayoutItem(Q_NULL),
    m_viewColumnsLength(0), m_viewRowsLength(0), m_offsetY(0), m_forceUpdateLayout(false),
    m_newRowAnimationTimer(Q_NULL), m_focusedGridRow(Q_NULL), m_transparentRowOpacity(false), m_transparentRowAnimationOrder(false)
{
    m_printerLayout = (m_rowsParent == Q_NULL);
    if (m_printerLayout)
        m_rowsParent = new ScrollableGraphicPrimitive(view->grid(), 0);

    m_newRowAnimationTimer = new QTimer(this);
    m_newRowAnimationTimer->setSingleShot(true);
    connect(m_newRowAnimationTimer, SIGNAL(timeout()), this, SLOT(updateNewRowOpacity()));
}

void GridRowsLayoutEngine::initialize()
{
    m_focusedRowLayoutItem = createItem();
    m_focusedRowLayoutItem->create();
    m_focusedRowLayoutItem->initialize(Q_NULL, GridRowLayoutItem::Undefined);
}

GridRowsLayoutEngine::~GridRowsLayoutEngine()
{
    Q_DELETE_AND_NULL(m_focusedRowLayoutItem);
    deleteVirtualRows();
    if (m_printerLayout)
        delete m_rowsParent;
    m_rowsParent = Q_NULL;
}

void GridRowsLayoutEngine::beginUpdate(int columnsLength, int rowsLength, int offsetY, bool forceUpdateLayout)
{
    Q_UNUSED(columnsLength);

    if (rowsLength > 0)
    {
        int smallestHeight = get_group_height(m_view->grid()->style(), m_view->optionsref());
        createVirtualRows(rowsLength / smallestHeight + 1);
    }

    m_offsetY = offsetY;

    m_forceUpdateLayout = forceUpdateLayout;

    m_firstRowLayoutItem = m_lastRowLayoutItem = Q_NULL;
    m_waitingIndex = 0;

    m_viewColumnsLength = columnsLength;
    m_viewRowsLength = rowsLength;

    if (m_transparentRow != Q_NULL)
    {
        m_transparentRowOpacity = m_transparentRow->opacity();
        m_transparentRow->setOpacity(1.0);
        m_transparentRow = Q_NULL;
    }
    else
        m_transparentRowOpacity = 1.0;

    m_focusedGridRow = m_view->modelController()->focusedRow();

    for (QVector<GridRowLayoutItem *>::iterator it = m_virtualRows.begin();
        it != m_virtualRows.end();  ++it)
    {
        (*it)->setHidden(true);
    }

    m_focusedRowLayoutItem->setHidden(true);
}

void GridRowsLayoutEngine::endUpdate()
{
    GridRowLayoutItem* flashRow = Q_NULL;
    GridRowLayoutItem* previousRow = Q_NULL;
    int count = 0;
    for (int i = 0; i < m_waitingIndex; ++i)
    {
        WaitingRowParam& param = m_waiting[i];
        GridRowLayoutItem* currentRow = param.row;
        if (currentRow == Q_NULL)
        {
            while (count < m_virtualRows.size())
            {
                GridRowLayoutItem* temp = m_virtualRows[count];
                count++;
                if (temp->isHidden())
                {
                    currentRow = temp;
                    currentRow->setHidden(false);
                    param.row = currentRow;
                    break;
                }
            }

            Q_ASSERT(currentRow != Q_NULL);
            if (currentRow == Q_NULL)
                break;

            if (param.summary)
                currentRow->initialize(param.gridRow, GridRowLayoutItem::Summary);
            else if (param.gridRow->type() == GridRow::DataRow)
            {
                if (param.frozenPlace == GridRow::FrozenNone)
                    currentRow->initialize(param.gridRow, GridRowLayoutItem::Data);
                else
                    currentRow->initialize(param.gridRow, GridRowLayoutItem::FrozenData);
            }
            else
                currentRow->initialize(param.gridRow, GridRowLayoutItem::Group);
            currentRow->setRowMask(m_maskRect);
            currentRow->setRect(param.rect, true);
            currentRow->updateModelData();
        }
        else
        {
            if (currentRow == m_focusedRowLayoutItem)
            {
                Q_ASSERT(m_focusedGridRow != Q_NULL);
                if (m_focusedGridRow->type() == GridRow::DataRow)
                {
                    if (param.frozenPlace == GridRow::FrozenNone)
                        m_focusedRowLayoutItem->initialize(m_focusedGridRow, GridRowLayoutItem::Data);
                    else
                        m_focusedRowLayoutItem->initialize(m_focusedGridRow, GridRowLayoutItem::FrozenData);
                }
                else
                    m_focusedRowLayoutItem->initialize(m_focusedGridRow, GridRowLayoutItem::Group);

                m_focusedRowLayoutItem->updateModelData();
            }

            currentRow->setRowMask(m_maskRect);
            currentRow->setRect(param.rect, m_forceUpdateLayout);
        }

        if (currentRow->gridRow()->frozenPlace() == GridRow::FrozenNone)
        {
            if (m_firstRowLayoutItem == Q_NULL)
                m_firstRowLayoutItem = currentRow;
            m_lastRowLayoutItem = currentRow;
        }

        if (previousRow != Q_NULL)
        {
            int currentRowIndex = m_view->layout()->alignGridRowIndex(currentRow->gridRow()->rowIndex());
            if (currentRow->type() == GridRowLayoutItem::Summary || currentRowIndex > previousRow->gridRow()->rowIndex())
            {
                previousRow->setNextRowLayoutItem(currentRow);
                previousRow = currentRow;
            }
        }
        else
            previousRow = currentRow;

        if (flashRow == Q_NULL)
        {
            if (m_view->modelController()->isRowFocused(currentRow->gridRow()))
                flashRow = currentRow;
        }
    }

    //If the focused row is not visible on screen then hide it or set it as a new row editor.
    if (m_focusedRowLayoutItem->isHidden())
    {
        if (m_view->isNewRowEditing())
        {
            m_focusedRowLayoutItem->initialize(m_focusedGridRow, GridRowLayoutItem::NewData);
            m_focusedRowLayoutItem->updateModelData();
            QRect r = m_view->layout()->rowsPanel()->newRowPanel()->getLayoutItemRect();
            m_focusedRowLayoutItem->setRect(r, true);
            m_focusedRowLayoutItem->setRowMask(m_maskRect);

        }
        else
        {
            m_focusedRowLayoutItem->setHidden(false);
            m_focusedRowLayoutItem->initialize(Q_NULL, GridRowLayoutItem::Undefined);
        }
    }

    while (count < m_virtualRows.size())
    {
        GridRowLayoutItem* temp = m_virtualRows[count];
        count++;
        if (temp->isHidden())
        {
            temp->setHidden(false);
            temp->initialize(Q_NULL, GridRowLayoutItem::Undefined);
        }
    }

    m_forceUpdateLayout = false;

    if (m_view->isNewRowEditing())
    {
        m_focusedRowLayoutItem->updateModelData(); //????

        m_transparentRow = flashRow;
        if (m_transparentRow != Q_NULL)
            m_transparentRow->setOpacity(m_transparentRowOpacity);
        updateNewRowOpacity();
    }
    m_view->grid()->geometryChanged();
}


bool GridRowsLayoutEngine::isRowVisible(GridRow* row)
{
    const GridRowLayoutItem* virtualRow = findVirtualRow(row);

    if (virtualRow != Q_NULL)
    {
        QRect r = m_view->layout()->rowsScrollAreaRect();
        if (m_view->layout()->orientation() == Qt::Vertical)
        {
            r.setLeft(virtualRow->rect().left());
            r.setWidth(virtualRow->rect().width());
        }
        else
        {
            r.setTop(virtualRow->rect().top());
            r.setHeight(virtualRow->rect().height());
        }
        return r.contains(virtualRow->rect());
    }
    return false;
}

void GridRowsLayoutEngine::updateSummaryLayoutParent()
{
    for (QVector<GridRowLayoutItem *>::iterator
        it = m_virtualRows.begin(); it != m_virtualRows.end(); ++it)
    {
        (*it)->updateSummaryLayoutParent();
    }
    m_focusedRowLayoutItem->updateSummaryLayoutParent();
}


void GridRowsLayoutEngine::addColumn(const GridDataTableColumn* column)
{
    for (QVector<GridRowLayoutItem *>::iterator it =
        m_virtualRows.begin(); it != m_virtualRows.end(); ++it)
    {
        GridRowLayoutItem* row = (*it);
        row->addColumn(column);
    }
    m_focusedRowLayoutItem->addColumn(column);
}

void GridRowsLayoutEngine::removeColumn(const GridDataTableColumn* column)
{
    for (QVector<GridRowLayoutItem *>::iterator it =
        m_virtualRows.begin(); it != m_virtualRows.end(); ++it)
    {
        GridRowLayoutItem* row = (*it);
        row->removeColumn(column);
    }
    m_focusedRowLayoutItem->removeColumn(column);
}

GraphicPrimitive* GridRowsLayoutEngine::getRowsParent() const
{
    return m_rowsParent;
}

bool GridRowsLayoutEngine::appendNextGridRow(GridRow* gridRow, bool summary, GridRow::FrozenPlace frozenPlace)
{
    Q_ASSERT(gridRow != Q_NULL);
    if (gridRow == Q_NULL)
        return false;

    if (m_waitingIndex >= m_waiting.size())
        return false;

    WaitingRowParam& param = m_waiting[m_waitingIndex];

    if (!appendNextGridRowRect(gridRow, summary, frozenPlace, param.rect))
        return false;

    m_waitingIndex++;
    param.summary = summary;
    param.gridRow = gridRow;
    param.frozenPlace = frozenPlace;

    GridRowLayoutItem::Type rowType = GridRowLayoutItem::Undefined;
    if (summary)
        rowType = GridRowLayoutItem::Summary;
    else if (gridRow->type() == GridRow::GroupRow)
        rowType = GridRowLayoutItem::Group;
    else
    {
        Q_ASSERT(gridRow->type() == GridRow::DataRow);
        if (frozenPlace == GridRow::FrozenNone)
            rowType = GridRowLayoutItem::Data;
        else
            rowType = GridRowLayoutItem::FrozenData;
    }

    if (!m_view->isNewRowEditing() && m_focusedGridRow == gridRow && 
        (rowType == GridRowLayoutItem::Data || rowType == GridRowLayoutItem::FrozenData || rowType == GridRowLayoutItem::Group))
        param.row = m_focusedRowLayoutItem;
    else
        param.row = getVirtualRow(gridRow, rowType == GridRowLayoutItem::Summary);

    if (param.row != Q_NULL)
    {
        Q_ASSERT(param.row->isHidden());
        //if (!param.row->isHidden())
        param.row->setHidden(false);
    }
    return true;
}

bool GridRowsLayoutEngine::appendGridRow(GridRow* gridRow)
{
    if (!appendNextGridRow(gridRow, false, GridRow::FrozenNone))
        return false;

    GridRow* summaryModelRow =
        m_view->optionsref()->groupSummaryPlace() == GroupSummaryFooter &&
        is_next_summary(gridRow) ? gridRow : Q_NULL;
    while (summaryModelRow != Q_NULL)
    {
        if (!appendNextGridRow(summaryModelRow, true, GridRow::FrozenNone))
            return false;
        if (!summaryModelRow->modelGroup()->last)
            break;
        summaryModelRow = summaryModelRow->parentGroupRow();
    }
    return true;
}

bool Qtitan::is_next_summary(GridRow* modelRow)
{
    return ((modelRow->type() == GridRow::DataRow && modelRow->last()) || (modelRow->type() == GridRow::GroupRow && !modelRow->isExpanded()));
}

bool GridRowsLayoutEngine::appendFrozenGridRow(GridRow* gridRow, bool topPosition)
{
    return appendNextGridRow(gridRow, false, topPosition ? GridRow::FrozenTop : GridRow::FrozenBottom);
}


bool GridRowsLayoutEngine::isPrintLayout() const
{
    return m_printerLayout;
}

void GridRowsLayoutEngine::printer_setRowsPageSize(int w, int h)
{
    m_rowsParent->setRect(QRect(0, 0, w, h));
}


QList<GridRowLayoutItem *> GridRowsLayoutEngine::printer_rows() const
{
    QList<GridRowLayoutItem *> retval;
    for (int i = 0; i < m_waitingIndex; ++i)
    {
        const WaitingRowParam& param = m_waiting[i];
        Q_ASSERT(param.row != Q_NULL);
        retval.append(param.row);
    }
    return retval;
}

GridRowLayoutItem* GridRowsLayoutEngine::focusedRowLayoutItem() const
{
    return m_focusedRowLayoutItem;
}


GridRow* GridRowsLayoutEngine::firstGridRow() const
{
    if (m_firstRowLayoutItem == Q_NULL)
        return Q_NULL;
    return m_firstRowLayoutItem->gridRow();
}

GridRow* GridRowsLayoutEngine::lastGridRow() const
{
    if (m_lastRowLayoutItem == Q_NULL)
        return Q_NULL;
    return m_lastRowLayoutItem->gridRow();
}

GridTableCellGraphic* GridRowsLayoutEngine::getCellGraphic(GridRow* gridRow, int cellIndex)
{
    if (gridRow == Q_NULL || cellIndex == -1)
        return Q_NULL;
    for (QVector<GridRowLayoutItem *>::iterator
        it = m_virtualRows.begin(); it != m_virtualRows.end(); ++it)
    {
        GridRowLayoutItem* virtualRow = (*it);
        if (virtualRow->gridRow() == gridRow)
        {
            if (virtualRow->type() == GridTableRowLayoutItem::Data ||
                virtualRow->type() == GridTableRowLayoutItem::FrozenData)
                return virtualRow->getCellGraphic(cellIndex);
        }
    }
    return Q_NULL;
}

bool GridRowsLayoutEngine::fastScrollEffect() const
{
    return !isPrintLayout() && m_view->optionsref()->fastScrollEffect();
}

void GridRowsLayoutEngine::createVirtualRows(int rowsCount)
{
    if (rowsCount > 250)
    {
        rowsCount = 250;
#ifdef _DEBUG
        Q_ASSERT(false); //Error in view rows calculation.
#endif
    }

    if (m_virtualRows.size() >= rowsCount)
        return;

    m_virtualRows.reserve(rowsCount);
    for (int i = m_virtualRows.size(); i < rowsCount; ++i)
    {
        GridRowLayoutItem* vrow = createItem();
        vrow->create();
        m_virtualRows.push_back(vrow);
    }
    m_waiting.resize(rowsCount);

    //m_view->layout()->columnHeader()->prepareCellsLayout();
}

void GridRowsLayoutEngine::deleteVirtualRows()
{
    for (int i = 0; i < m_virtualRows.size(); ++i)
    {
        GridRowLayoutItem* vrow = m_virtualRows[i];
        delete vrow;
    }
    m_virtualRows.clear();
    m_waiting.clear();
}

void GridRowsLayoutEngine::resetVirtualRows()
{
    for (QVector<GridRowLayoutItem *>::iterator
        it = m_virtualRows.begin(); it != m_virtualRows.end(); ++it)
    {
        GridRowLayoutItem* cur = (*it);
        cur->initialize(Q_NULL, GridRowLayoutItem::Undefined);
    }
    m_focusedRowLayoutItem->initialize(Q_NULL, GridRowLayoutItem::Undefined);
}

int GridRowsLayoutEngine::getVirtualRowsCount() const
{
    return m_virtualRows.size();
}

GridRowLayoutItem* GridRowsLayoutEngine::getVirtualRowByIndex(int index)
{
    return m_virtualRows[index];
}

GridRowLayoutItem* GridRowsLayoutEngine::findVirtualRow(GridRow* gridRow)
{
    GridRowLayoutItem* retval = getVirtualRow(gridRow, false);
    if (retval == Q_NULL)
    {
        // Maybe it is a focused row?
        if (m_focusedRowLayoutItem->gridRow() == gridRow)
            retval = m_focusedRowLayoutItem;
    }
    return retval;
}

GridRowLayoutItem* GridRowsLayoutEngine::getVirtualRow(GridRow* gridRow, bool symmaryRow)
{
    if (gridRow == Q_NULL)
    {
        Q_ASSERT(false);
        return Q_NULL;
    }
    for (QVector<GridRowLayoutItem *>::const_iterator it = m_virtualRows.constBegin();
        it != m_virtualRows.constEnd(); ++it)
    {
        GridRowLayoutItem* vrow = (*it);
        if (vrow->gridRow() == gridRow && vrow->type() != GridRowLayoutItem::Undefined)
        {
            if (symmaryRow)
            {
                if (vrow->type() == GridRowLayoutItem::Summary)
                    return vrow;
            }
            else
            {
                if (vrow->type() != GridRowLayoutItem::Summary)
                    return vrow;
            }
        }
    }
    return Q_NULL;
}

void GridRowsLayoutEngine::setRowMask(const QRect& maskRect)
{
    m_maskRect = maskRect;
}

void GridRowsLayoutEngine::updateNewRowOpacity()
{
    if (m_transparentRow == Q_NULL)
        return;

    double o = m_transparentRow->opacity();
    if (m_view->optionsref()->newRowHighlightEffect() == RowFlashEffect)
    {
        if (o >= 0.8)
            m_transparentRowAnimationOrder = true;

        if (m_transparentRowAnimationOrder)
        {
            o -= 0.07;
            if (o <= m_view->optionsref()->newRowOpacity())
                m_transparentRowAnimationOrder = false;
        }
        else
        {
            o += 0.07;
            if (o >= 0.8)
                m_transparentRowAnimationOrder = true;
        }
    }
    else
    {
        o = m_view->optionsref()->newRowOpacity();
    }
    m_transparentRow->setOpacity(o);
    m_transparentRow->dataRowGP()->update();

    if (m_view->optionsref()->newRowHighlightEffect() == RowFlashEffect && !m_newRowAnimationTimer->isActive())
        m_newRowAnimationTimer->start(m_view->optionsref()->newRowFlashInterval());
}


/* GridViewBase */
GridViewBase::GridViewBase(Grid* grid)
    : QObject(grid)
    , m_viewStates(NoChanges),
    m_contextStates(NoChanges)
    , m_clickHandler(Q_NULL)
    , m_isNewRowModify(false)
    , m_visibleColumnCount(0)
    , m_shiftColumnIndex(-1)
    , m_shiftRowIndex(-1)
    , m_shiftSelect(false)
#ifndef QTN_NOUSE_XML_MODULE
    , m_layoutLoading(false)
#endif
    , m_grid(grid)
    , m_modelController(Q_NULL)
    , m_options(Q_NULL)
    , m_viewPrimitive(Q_NULL)
    , m_filterDlg(Q_NULL)
    , m_contextHit(Q_NULL)
{
    m_modelController = new GridModelController(new GridViewInterfaceImpl(this));
}

GridViewBase::~GridViewBase()
{
    Q_ASSERT(m_viewStates & ViewDestroying);

    removeActions();
    {
        GridViewAutoUpdater autoUpdater(this);
        hideEditor();
        removeColumns();
        modelController()->setModel(Q_NULL);
    }

    Q_DELETE_AND_NULL(m_clickHandler);
    Q_DELETE_AND_NULL(m_options);
    Q_DELETE_AND_NULL(m_viewPrimitive);
    Q_DELETE_AND_NULL(m_modelController);
    Q_DELETE_AND_NULL(m_filterDlg);
    for (int i = 0; i < m_fieldProviders.size(); ++i)
        delete m_fieldProviders[i];
    m_fieldProviders.clear();
}

/*!
\internal
*/
void Qtitan::init_editor_option(GridViewBase* view,
                                    GridEditorStyleOption* option,
                                    GridModelDataBinding* dataBinding,
                                    GridRow* gridRow,
                                    int columnIndex,
                                    bool alternatingRowColors,
                                    bool isNewRow,
                                    bool isSelected,
                                    bool isEnabled,
                                    bool isFocused,
                                    const QRect& boundedRect,
                                    Qt::Alignment alignment)
{
    option->init(view->grid());
    option->palette = view->optionsref()->cellPalette();

    option->palette.setColor(QPalette::Base, Qt::transparent);
    option->palette.setColor(QPalette::AlternateBase, Qt::transparent);

    if (view->grid()->hasFocus())
        option->state |= QStyle::State_Active;
    else
        option->state &= ~QStyle::State_Active;

    if (alternatingRowColors && !isNewRow)
        option->state |= QStyle::State_On;

    if (isEnabled)
        option->state |= QStyle::State_Enabled;
    else
        option->state &= ~QStyle::State_Enabled;

    option->state &= ~QStyle::State_HasFocus;
    if (isNewRow)
    {
        if (view->modelController()->focusedColumnIndex() == columnIndex)
            option->state |= QStyle::State_HasFocus;
    }
    else
    {
        if (view->modelController()->isCellFocused(gridRow, columnIndex))
            option->state |= QStyle::State_HasFocus;
    }

    if (isSelected)
    {
        if (!(view->isNewRowEditing() && isFocused))
            option->state |= QStyle::State_Selected;
    }

    option->rect = boundedRect;

    option->palette.setCurrentColorGroup(option->state & QStyle::State_Active ? QPalette::Active : QPalette::Inactive);

    if (gridRow != Q_NULL && view->optionsref()->modelDecoration())
    {
        QVariant variant;
        if (dataBinding->getValue(gridRow->modelRowIndex(), Qt::ForegroundRole, variant) &&
            variant.isValid())
        {
            QColor color;
            if (variant.type() == QVariant::Color)
                color = variant.value<QColor>();
            else if (variant.type() == QVariant::Brush)
                color = variant.value<QBrush>().color();
            option->palette.setColor(QPalette::Text, color);
            option->palette.setColor(QPalette::WindowText, color);
            option->palette.setColor(QPalette::HighlightedText,
                qcolor_mix_cmy(option->palette.color(QPalette::HighlightedText), color));
        }

        if (dataBinding->getValue(gridRow->modelRowIndex(), Qt::DecorationRole, variant))
        {
            if (variant.isValid())
            {
                QPixmap pixmap;
                if (qvariant_to_pixmap(variant, pixmap) != QVariant::Invalid && !pixmap.isNull())
                {
                    option->icon = pixmap;
                    int iconWidth = view->grid()->style()->pixelMetric(QStyle::PM_SmallIconSize, 0, 0);
                    option->iconSize = QSize(iconWidth, iconWidth);
                }

            }
            variant = QVariant();
        }

        if (dataBinding->getValue(gridRow->modelRowIndex(), Qt::BackgroundRole, variant))
        {
            if (variant.isValid())
            {
                QBrush bgbrush;
                if (variant.type() == QVariant::Color)
                    bgbrush = QBrush(variant.value<QColor>());
                else if (variant.type() == QVariant::Brush)
                    bgbrush = variant.value<QBrush>();

                QBrush mix = bgbrush;
                mix.setColor(qcolor_mix_cmy(option->palette.color(QPalette::Highlight), bgbrush.color()));
                option->palette.setBrush(QPalette::Highlight, mix);
                option->palette.setBrush(QPalette::Base, bgbrush);
                option->palette.setBrush(QPalette::AlternateBase, bgbrush);
                option->palette.setBrush(QPalette::Window, bgbrush);
            }
        }

        if (dataBinding->getValue(gridRow->modelRowIndex(), Qt::TextAlignmentRole, variant)
            && variant.isValid())
        {
            option->decorationAlignment = (Qt::Alignment)variant.toInt();
        }
        else
            option->decorationAlignment = alignment;

        if (gridRow != Q_NULL && dataBinding->getValue(gridRow->modelRowIndex(), Qt::FontRole, variant) && variant.isValid())
        {
            option->font = qtn_font_apply_scale(variant.value<QFont>(), view->optionsref()->zoomFactor());
        }
        else
            option->font = qtn_font_apply_scale(view->optionsref()->cellFont(), view->optionsref()->zoomFactor());
    }
    else
    {
        option->font = qtn_font_apply_scale(view->optionsref()->cellFont(), view->optionsref()->zoomFactor());
        option->decorationAlignment = alignment;
    }
    option->fontMetrics = QFontMetrics(option->font);
    option->selectedText = view->layout()->findPanel()->selectedText();
    option->selectCaseSensitivity = view->layout()->findPanel()->selectCaseSensitivity();
}

void GridViewBase::changeGrouping(GridColumnBase* columnBase, int groupIndex, bool caseSensitivity, bool localeAware)
{
    GridViewAutoUpdater autoUpdater(this);

    SortOrder order = SortNone;
    if (groupIndex != -1)
    {
        order = getColumnSortOrder(columnBase);
        if (order == SortNone)
            order = SortAscending;
    }
    modelController()->setGroupOrder(getDataBinding(columnBase), groupIndex, order, caseSensitivity, localeAware);
}

bool GridViewBase::isUpdating() const
{
    return modelController()->isDataUpdating() || modelController()->hasChanges();
}

void GridViewBase::zoomIn()
{
    if (!optionsref()->zoomEnabled())
        return;

    double factor = optionsref()->zoomFactor() + 0.25;
    if (factor > 3.0)
        factor = 3.0;
    optionsref()->setZoomFactor(factor);
}

void GridViewBase::zoomOut()
{
    if (!optionsref()->zoomEnabled())
        return;

    double factor = optionsref()->zoomFactor() - 0.25;
    if (factor < 0.25)
        factor = 0.25;
    optionsref()->setZoomFactor(factor);
}

void GridViewBase::requestUpdateLayout(bool showZoomIndicator)
{
    if (showZoomIndicator)
        m_viewPrimitive->startZooming();

    if (isUpdating())
    {
        m_viewStates |= RequestUpdateLayout;
        return;
    }

    layout()->resetAutoHeightMode();

    layout()->updateLayout();
    updateFocusedGP();
    layout()->update();
}

int GridViewBase::getColumnCount() const
{
    return m_columnslist.size();
}

GridColumnBase* GridViewBase::getColumn(int index) const
{
    if (index >= 0 && index < m_columnslist.size())
        return m_columnslist.at(index);
    else return Q_NULL;
}

GridColumnBase* GridViewBase::getColumnByModelColumn(int modelColumn) const
{
    for (GridColumns::const_iterator it = m_columnslist.constBegin(); it != m_columnslist.constEnd(); ++it)
    {
        if (getDataBinding(*it)->column() == modelColumn)
            return *it;
    }
    return Q_NULL;
}

GridColumnBase* GridViewBase::getColumnByModelColumnName(const QString& modelColumnName) const
{
    if (modelController()->model() == Q_NULL)
        return Q_NULL;
    for (GridColumns::const_iterator it = m_columnslist.constBegin(); it != m_columnslist.constEnd(); ++it)
    {
        if (getDataBinding(*it)->columnName() == modelColumnName)
            return *it;
    }
    return Q_NULL;
}

GridColumnBase* GridViewBase::getColumnByCaption(const QString& caption) const
{
    for (GridColumns::const_iterator it = m_columnslist.constBegin(); it != m_columnslist.constEnd(); ++it)
    {
        if ((*it)->caption() == caption)
            return *it;
    }
    return Q_NULL;
}


GridColumnBase* GridViewBase::getPrevious(const GridColumnBase* columnBase) const
{
    Q_ASSERT(columnBase != Q_NULL);
    GridColumns cols;
    getColumnLine(columnBase, cols);
    GridColumnBase* retval = Q_NULL;
    for (GridColumns::const_iterator it = cols.constBegin(); it != cols.constEnd(); ++it)
    {
        GridColumnBase* column = (*it);
        if (columnBase == column)
            return retval;
        retval = column;
    }
    return Q_NULL;
}

GridColumnBase* GridViewBase::getNext(const GridColumnBase* columnBase) const
{
    Q_ASSERT(columnBase != Q_NULL);
    GridColumns cols;
    getColumnLine(columnBase, cols);
    bool found = false;
    for (GridColumns::const_iterator it = cols.constBegin(); it != cols.constEnd(); ++it)
    {
        GridColumnBase* column = (*it);
        if (columnBase == column)
        {
            found = true;
            continue;
        }

        if (found)
        {
            return column;
        }
    }
    return Q_NULL;
}

void GridViewBase::showFieldChooser()
{
    layout()->groupingPanel()->fieldChooser()->setFieldChooserVisible(true);
}

void GridViewBase::hideFieldChooser()
{
    layout()->groupingPanel()->fieldChooser()->setFieldChooserVisible(false);
}

bool GridViewBase::isFieldChooserVisible() const
{
    return layout()->groupingPanel()->fieldChooser()->isFieldChooserVisible();
}

void GridViewBase::showFilterPanel()
{
    layout()->showFooter();
    layout()->footer()->setVisibility(true);
    requestUpdateLayout();
}

void GridViewBase::hideFilterPanel()
{
    Q_DELETE_AND_NULL(m_filterDlg);
    destroyColumnFilters();
    
    layout()->hideFooter();
    layout()->footer()->setVisibility(false);
    layout()->filterPanel()->setHistoryWindowVisible(false);

}

void GridViewBase::showFindPanel()
{
    layout()->findPanel()->setPanelVisible(true, false);
}

void GridViewBase::hideFindPanel(bool cancel)
{
    layout()->findPanel()->setPanelVisible(false, cancel);
}

bool GridViewBase::isFindPanelVisible() const
{
    return layout()->findPanel()->isPanelVisible();
}

void GridViewBase::findNext()
{
    layout()->findPanel()->forwardClicked();
}

void GridViewBase::findPrevious()
{
    layout()->findPanel()->backClicked();
}

void GridViewBase::executeFilterDialog()
{
    if (m_filterDlg == Q_NULL)
        m_filterDlg = new FilterDialog(this, filter()->condition(), grid());

    QPoint gp = layout()->mapToGlobal(QPoint(0, 0));
    int left = gp.x() + (layout()->width() - m_filterDlg->sizeHint().width()) / 2;
    int top = gp.y() + (layout()->height() - m_filterDlg->sizeHint().height()) / 2;
    m_filterDlg->move(left, top);

    if (m_filterDlg->exec() == QDialog::Accepted)
    {
        GridViewAutoUpdater autoUpdater(this);
        destroyColumnFilters();
        showFilterPanel();
        filter()->setCondition((GridFilterGroupCondition *)m_filterDlg->condition()->clone());
        if (!filter()->isActive())
            filter()->setActive(true);
        else
            modelController()->refresh();
    }
    else if (!filter()->isActive())
    {
        Q_DELETE_AND_NULL(m_filterDlg);
    }
}

void GridViewBase::beginUpdate()
{
    modelController()->beginUpdate();
}

void GridViewBase::endUpdate()
{
    modelController()->endUpdate();
    if (isUpdating() || m_viewStates & ViewDestroying)
        return;

    GridViewStates states = m_viewStates;
    m_viewStates = NoChanges;
    applyChanges(ControllerNoChanges, states);
}

Grid* GridViewBase::grid() const
{
    return m_grid;
}

GridModelController* GridViewBase::modelController() const
{
    return m_modelController;
}

/*!
Method is intended to connect model with the data to the view. The model should be derived from class QAbstractItemModel. 
Parameter \a parent used if the data is not at the first level of the model. \a createColumns parameter allows automatically create a columns in the view which will correspond to 
the structure of the model. If this parameter is false then the columns will not be created at all. 
Later they can be created manually in the required composition. To create a column please use createColumn() function. 
Note that for banded view, automatic creation of columns works as well. Columns will be created and inserted into the first 
available band column. If there is no band column in the view it will be created first.
*/ 
void GridViewBase::setModel(QAbstractItemModel* model,
                            QModelIndex parent)
{
    hideFilterPanel();
    setModelToController(model, parent);
}

/*!
\internal
*/
void GridViewBase::setModelToController(QAbstractItemModel* model, const QModelIndex& parent)
{
    GridViewAutoUpdater autoUpdater(this);

    hideEditor();
    /*
    m_viewGP->filterGraphic()->closeFilterClicked();
    */
    removeColumns();
    modelController()->setModel(model, parent);                        
    if (optionsref()->autoCreateColumns())
        addColumns();
}

GridFilter* GridViewBase::filter() const
{
    return modelController()->filter();
}

void GridViewBase::addColumns()
{
    QAbstractItemModel* model = modelController()->model();
    if (model != Q_NULL)
    {
        int count = model->columnCount(modelController()->parentIndex());
        
        for (int i = 0; i < count; ++i)
        {
            addColumn(i);
        }
    }
}

/*!
Returns the row that has the focus at the moment.

\sa focusedRowIndex()
*/
GridRow* GridViewBase::focusedRow()
{
    return modelController()->focusedRow();
}

/*!
Returns the index of the row that has the focus at the moment.

\sa focusedRow()
*/
int GridViewBase::focusedRowIndex()
{
    return modelController()->focusedRowIndex();
}

/*!
Returns the index of the column that has the focus at the moment.

\sa focusedRowIndex()
*/
int GridViewBase::focusedColumnIndex()
{
    return modelController()->focusedColumnIndex();
}

/*!
\internal
*/
void GridViewBase::updateFocusedGP()
{
    if (layout()->findPanel()->hasEditFocus())
        return;

    GridRowLayoutItem* virtualRow = layout()->rowsLayoutEngine()->focusedRowLayoutItem();
    if (virtualRow == Q_NULL)
        return;

    GraphicPrimitive* gp = Q_NULL;

    if (virtualRow->type() == GridRowLayoutItem::Data ||
        virtualRow->type() == GridRowLayoutItem::NewData ||
        virtualRow->type() == GridRowLayoutItem::FrozenData)
    {
        gp = virtualRow->getCellGraphic(modelController()->focusedColumnIndex());
    }
    else if (virtualRow->type() == GridRowLayoutItem::Group)
    {
        gp = virtualRow->groupRowGP();
    }

    if (gp == Q_NULL)
        gp = layout();

    if (gp != Q_NULL)
        gp->setFocus();
}

void GridViewBase::applyChanges(GridControllerChanges controllerChanges, GridViewStates viewStates)
{
    if (controllerChanges == ControllerNoChanges && viewStates == NoChanges)
        return;

    if (controllerChanges == ControllerLayoutChanged)
    {
        layout()->updateLayout();
        return;
    }

    m_contextStates = viewStates;

    if (controllerChanges & ControllerRowsChanged || controllerChanges & ControllerDataChanged)
    {
        layout()->rowsLayoutEngine()->resetVirtualRows();
    }

    bool needUpdateLayout = (viewStates != ControllerChanged) ||
                            (controllerChanges & ControllerRowsChanged) ||
                            (controllerChanges & ControllerLayoutChanged);

    if (controllerChanges & ControllerRowsChanged || viewStates & ColumnsChanged || viewStates & RequestUpdateLayout)
    {
        layout()->resetAutoHeightMode();
        needUpdateLayout = true;
    }
    else if (controllerChanges & ControllerDataChanged)
    {
        if (qobject_cast<GridTableView *>(this) != Q_NULL)
        {
            if (static_cast<GridTableView *>(this)->options().rowAutoHeight())
            {
                layout()->resetAutoHeightMode();
                needUpdateLayout = true;
            }
        }
    }

    if (viewStates & ColumnsChanged)
    {
        layout()->fieldChooser()->columnsUpdated();
        emit columnsUpdated();
    }

    if (!needUpdateLayout)
    {
        layout()->updateRows(true);
        layout()->rowsPanel()->update();
    }
    else
        layout()->updateLayout();

    if (controllerChanges & ControllerFocusChanged)
    {
        if (modelController()->oldFocusedRowIndex() != modelController()->focusedRowIndex())
        {
            showRowOnScreen(modelController()->oldFocusedRowIndex(), modelController()->focusedRowIndex());
            emit focusRowChanged(modelController()->oldFocusedRowIndex(), modelController()->focusedRowIndex());
        }

        if (modelController()->oldFocusedColumnIndex() != modelController()->focusedColumnIndex())
        {
            showColumnOnScreen(modelController()->oldFocusedColumnIndex(), modelController()->focusedColumnIndex());
            emit focusColumnChanged(modelController()->oldFocusedColumnIndex(), modelController()->focusedColumnIndex());
        }
        updateFocusedGP();
    }
    else if (modelController()->focusedRowIndex() == -1 || modelController()->focusedColumnIndex() == -1)
    {
        initViewFocus();
    }
    m_contextStates = NoChanges;
}

void GridViewBase::markColumnsChanged()
{
    m_viewStates |= ColumnsChanged;
}

void GridViewBase::editorChanging(const GridDataTableColumn* column, bool reset)
{
    if (layout()->rowsLayoutEngine() != Q_NULL)
        layout()->rowsLayoutEngine()->editorChanging(column, reset);

    if (reset)
        markColumnsChanged();
}

void GridViewBase::editorRepositoryChanged(GridEditorRepository* repository)
{
    if (layout()->rowsLayoutEngine() == Q_NULL || repository == Q_NULL)
        return;

    for (GridViewBase::GridColumns::const_iterator column_it = m_columnslist.constBegin();
        column_it != m_columnslist.constEnd(); ++column_it)
    {
        if (!isColumnVisible(*column_it))
            continue;
        GridDataTableColumn* column = qobject_cast<GridDataTableColumn *>(*column_it);
        if (column == Q_NULL || column->m_editorRepository != repository)
            continue;
        layout()->rowsLayoutEngine()->editorChanging(column, false);
    }
}

void GridViewBase::controllerChanged(GridControllerChanges changes)
{
    Q_ASSERT(!(m_viewStates & ViewDestroying));

    if (changes != ControllerFocusChanged)
        m_viewStates |= ControllerChanged;

    GridViewStates states = m_viewStates;
    m_viewStates = NoChanges;
    applyChanges(changes, states);
}

bool GridViewBase::setCellFocus(int rowIndex, int columnIndex, Qt::KeyboardModifiers modifiers, const GridEditorActivationPolicyFlag& activationFlag)
{
    if (isNewRowEditing())
    {
        GraphicPrimitive* gp = grid()->pressedGP();
        while (gp != Q_NULL)
        {
            if (gp->id() == GridHitInfo::Row)
            {
                GridDataRowPrimitive* rowPrimitive = static_cast<GridDataRowPrimitive *>(gp);
                if (rowPrimitive->rowLayoutItem() != layout()->rowsLayoutEngine()->focusedRowLayoutItem())
                {
                    closeNewRowEditor();
                }
                break;
            }
            gp = gp->parentGP();
        }
    }

    GridViewAutoUpdater autoUpdater(this);

    int oldRowIndex = modelController()->focusedRowIndex();
    int oldColumnIndex = modelController()->focusedColumnIndex();

    bool rowChanged = rowIndex != oldRowIndex;
    bool columnChanged = columnIndex != oldColumnIndex;

    GridRow* newFocusedRow = modelController()->getGridRow(rowIndex);
    if (newFocusedRow == Q_NULL)
        return false;

    GridPersistentRow persistentRow(modelController());
    persistentRow.set(newFocusedRow);

    bool needSubmitData = modelController()->addingModelRowIndex() != newFocusedRow->modelRowIndex();

    {
        if (isNewRowEditing() && needSubmitData)
        {
            if (!closeNewRowEditor())
                return false;
        }

        if (isEditing() && (columnChanged || rowChanged))
        {
            if (!closeEditorInternal())
                return false;
        }
    }

    if (needSubmitData)
    {
        if (columnChanged && modelController()->editStrategy() == Qtitan::OnFieldChange)
        {
            if (!modelController()->submitData())
            {
                raiseError(Grid::tr_compatible(QtnGridDataErrorString), Grid::tr_compatible(QtnGridDataErrorDescription_004_String));
                return false;
            }
        }
        else if (rowChanged)
        {
            if (!modelController()->submitData())
            {
                raiseError(Grid::tr_compatible(QtnGridDataErrorString), Grid::tr_compatible(QtnGridDataErrorDescription_004_String));
                return false;
            }
        }
    }

    //Controller may be changed at this point.
    newFocusedRow = persistentRow.row();
    if (newFocusedRow == Q_NULL)
        return false;
    rowIndex = newFocusedRow->rowIndex();


    if (optionsref()->selectionPolicy() != SelectIgnore)
    {
        updateSelection(rowIndex, columnIndex, rowChanged || columnChanged, modifiers, activationFlag);
    }

    modelController()->setCellFocus(newFocusedRow, columnIndex);

    /*
    if (rowChanged || columnChanged)
    {
        showRowOnScreen(oldRowIndex, newFocusedRow->rowIndex(), true);
    }
    */

    GridColumnBase* column =getColumn(columnIndex);
    if (!isColumnVisible(column))
        column = Q_NULL;

//    Q_ASSERT(column == Q_NULL || column->m_gp != Q_NULL);

    //if (!isUpdating() && columnChanged && column != Q_NULL)
    //{
    //    showColumnOnScreen(oldColumnIndex, column->index());
    //}

    m_editContext.changed = false;

    if (column != Q_NULL && newFocusedRow->type() == GridRow::DataRow)
    {
        if (checkActivationEditorPolicy(activationFlag))
        {
            if (!showEditorInternal(activationFlag))
            {
                Q_ASSERT(false); //rollback
                //modelController()->setFocusedRowIndex(oldRowIndex);
                //modelController()->setFocusedCell(oldColumnIndex);
                //return false;
            }
        }
    }

#if 0
    if (columnChanged || rowChanged || m_editContext.changed)
    {
        GridTableCellGraphic* cellGP = layout()->rowsLayoutEngine()->getCellGraphic(modelController()->getGridRow(oldRowIndex), oldColumnIndex);
        if (cellGP)
            cellGP->updateButtonLayout();

        cellGP = layout()->rowsLayoutEngine()->getCellGraphic(newFocusedRow, columnIndex);
        if (cellGP)
            cellGP->updateButtonLayout();

        if (!isUpdating())
            layout()->rowsPanel()->update();
    }
#endif
    return true;
}

bool GridViewBase::showEditorInternal(GridEditorActivationPolicyFlag policyFlag)
{
    if (isEditing())
        return true;

    int rowIndex = modelController()->focusedRowIndex();
    int columnIndex = modelController()->focusedColumnIndex();

    GridRow* row = modelController()->getGridRow(rowIndex);
    if (row == Q_NULL || row->type() != GridRow::DataRow)
        return false;

    GridDataTableColumn* column = qobject_cast<GridDataTableColumn *>(getColumn(columnIndex));
    if (column == Q_NULL)
        return false;
    Q_ASSERT(column->editorRepository() != Q_NULL);

    {
        EditorEventArgs args(column, row, policyFlag);
        emit editorStarting(&args);
        if (args.isHandled())
            return true;
    }

    QModelIndex index = column->dataBinding()->controller()->model()->index(
        row->modelRowIndex(), column->dataBinding()->column(), column->dataBinding()->controller()->parentIndex());

    if (!column->editorRepository()->isEditable() || !index.flags().testFlag(Qt::ItemIsEditable))
        return true;

    QVariant value;
    if (row == Q_NULL || !column->dataBinding()->getValue(
        row->modelRowIndex(),
        column->dataBinding()->editRole(),
        value))
    {
        return false;
    }

    //if (column->editorRepository()->defaultValue(Qt::EditRole).type() != value.type())
    //{
    //    if (!value.convert(column->editorRepository()->defaultValue(Qt::EditRole).type()))
    //    {
    //        Q_ASSERT(false);
    //        value = column->editorRepository()->defaultValue(Qt::EditRole);
    //    }
    //}

    m_editContext.cellGP = layout()->rowsLayoutEngine()->focusedRowLayoutItem()->getCellGraphic(columnIndex);
    Q_ASSERT(m_editContext.cellGP != Q_NULL);
    if (m_editContext.cellGP == Q_NULL)
        return false;

    Q_ASSERT(m_editContext.persistentRow == Q_NULL);
    m_editContext.persistentRow = new GridPersistentRow(modelController());
    m_editContext.persistentRow->set(row);
    m_editContext.cellGP->rowLayoutItem()->initialize(row, isNewRowEditing() ? GridRowLayoutItem::NewData : GridRowLayoutItem::Data);
    m_editContext.editor = m_editContext.cellGP->createEditor();
    Q_ASSERT(m_editContext.editor != Q_NULL);

    m_editContext.cellGP->setWidget(Q_NULL);
    m_editContext.editor->setEditMode(true);
    m_editContext.cellGP->setDirtyData(true);
    //m_editContext.cellGP->loadData();
    m_editContext.cellGP->setWidget(m_editContext.editor->getCellWidget());
    m_editContext.cellGP->updateFont();
    m_editContext.cellGP->setFocus();
    m_editContext.cellGP->invalidate();

    raiseEditorEditModeChanged(m_editContext.editor);

    m_editContext.changed = true;

    {
        EditorEventArgs args(column, row, policyFlag);
        emit editorStarted(&args);
    }

    return true;
}

/*!
\internal
*/
bool GridViewBase::closeEditorInternal()
{
    if (!isEditing())
        return true;

    if (!postEditor())
        return false;

    cancelEditing();
    return true;
}


bool GridViewBase::isNewRowEditing() const
{
    return modelController()->isNewRowMode();
}

bool GridViewBase::validateEditor()
{
    GridEditor* editor = activeEditor();
    if (editor == Q_NULL)
        return false;
    return editor->validate();
}

bool GridViewBase::isEditing() const
{
    return m_editContext.editor != Q_NULL;
}

GridEditor* GridViewBase::activeEditor() const
{
    return m_editContext.editor;
}

bool GridViewBase::isModified() const
{
    return activeEditor() != Q_NULL &&
        activeEditor()->isContextModified();
}

bool GridViewBase::showEditor(GridEditorActivationPolicyFlag policyFlag)
{
    GridViewAutoUpdater autoUpdater(this);

    if (showEditorInternal(policyFlag))
    {
        if (!isNewRowEditing())
            showRowOnScreen(modelController()->focusedRowIndex(), modelController()->focusedRowIndex());
        return true;
    }
    return false;
}

void GridViewBase::hideEditor()
{
    GridViewAutoUpdater autoUpdater(this);

    if (!isEditing())
        return;

    cancelEditing();
}

void GridViewBase::cancelEditing()
{
    Q_ASSERT(isEditing());

    Q_ASSERT(m_editContext.editor->isEditMode());

    m_editContext.cellGP->setWidget(Q_NULL);
    m_editContext.editor->setEditMode(false);
    m_editContext.cellGP->setDirtyData(true);
    //m_editContext.cellGP->loadData();
    m_editContext.cellGP->setWidget(m_editContext.editor->getCellWidget());
    m_editContext.cellGP->invalidate();

    raiseEditorEditModeChanged(m_editContext.editor);

    {
        GridColumnBase* column = getColumn(m_editContext.cellGP->columnIndex());
        EditorEventArgs args(column, m_editContext.persistentRow->row(), NoPolicy);
        emit editorStoped(&args);
    }

    m_editContext.cellGP = Q_NULL;
    Q_DELETE_AND_NULL(m_editContext.persistentRow);
    m_editContext.editor = Q_NULL;
    m_editContext.changed = true;
}

bool GridViewBase::closeEditor()
{
    GridViewAutoUpdater autoUpdater(this);

    if (!closeEditorInternal())
        return false;

    return true;
}

bool GridViewBase::postEditor()
{
    if (!isEditing())
        return false;

    Q_ASSERT(m_editContext.persistentRow != Q_NULL);

    GridDataTableColumn* column = m_editContext.cellGP->column();
    if (column == Q_NULL)
        return false;

    Q_ASSERT(column->editorRepository() != Q_NULL);

    {
        EditorEventArgs args(column, m_editContext.persistentRow->row(), NoPolicy);
        emit editorStoping(&args);
        if (args.isHandled())
            return false;
    }

    if (m_editContext.editor->isContextModified() && m_editContext.persistentRow != Q_NULL && m_editContext.persistentRow->isValid())
    {
        if (static_cast<GridDataTableColumn *>(column)->editorRepository()->isValidationEnabled() || !m_editContext.editor->isValidated())
        {
            if (!m_editContext.editor->validate())
            {
                return false;
            }
        }

        {
            QVariant value = m_editContext.editor->getContextValue();

            GridModelDataBinding* dataBinding = column->dataBinding();
            Q_ASSERT(dataBinding != Q_NULL);

            if (!dataBinding->setValue(m_editContext.persistentRow->modelRowIndex(), dataBinding->editRole(), value))
            {
                m_editContext.cellGP->showHint(Grid::tr_compatible(QtnGridDataErrorDescription_001_String));
                return false;
            }

            //Reset isContextModified flag (we still in cell edit mode).
            dataBinding->getValue(m_editContext.persistentRow->modelRowIndex(), dataBinding->editRole(), value);
            m_editContext.editor->updateContext(value);

        }

        //updateRows(true); TODO remove it!

        if (isNewRowEditing())
        {
            m_isNewRowModify = true;
            GridTableCellGraphic* gp = layout()->rowsLayoutEngine()->getCellGraphic(m_editContext.persistentRow->row(), column->index());
            if (gp != Q_NULL)
            {
                gp->setDirtyData(true);
                gp->invalidate();
            }
        }
    }
    return true;
}

bool GridViewBase::showNewRowEditor()
{
    Q_DELETE_AND_NULL(m_clickHandler); //Destoy click handler to avoid mouseDblClick on cell when the new row banner wiil be hided.

    if (modelController()->model() == Q_NULL)
        return false;

    if (isNewRowEditing())
        return true;

    if (optionsref()->newRowPlace() == NewRowNone)
        return false;

    NewRowEventArgs args;
    emit newRowEditorClicked(&args);
    if (args.isHandled())
        return true;

    int modelRowIndex = modelController()->model()->rowCount();
    int rowIndex = -1;
    {
        bool error = false;
        {
            GridViewAutoUpdater autoUpdater(this);
            modelController()->beginAddRow(modelRowIndex); // To be here to value does not be catched by the filter.
            if (!modelController()->insertModelRow(modelRowIndex))
            {
                modelController()->endAddRow();
                error = true;
            }
        }

        if (error)
        {
            raiseError(Grid::tr_compatible(QtnGridDataErrorString), Grid::tr_compatible(QtnGridDataErrorDescription_002_String));
            return false;
        }

        rowIndex = modelController()->ensureVisible(modelRowIndex);

        if (rowIndex == -1)
        {
            modelController()->endAddRow();
            modelController()->model()->removeRow(modelRowIndex);
            raiseError(Grid::tr_compatible(QtnGridDataErrorString), Grid::tr_compatible(QtnGridDataErrorDescription_003_String));
            return false;
        }
    }

    m_isNewRowModify = args.isModified();
    GridDataTableColumn* fc = getFirstColumn();
    if (fc == Q_NULL || !setCellFocus(rowIndex, fc->index(), Qt::NoModifier, NoPolicy)) //TODO setCellFocus
    {
        closeNewRowEditor();
        return false;
    }

    requestUpdateLayout();
    return true;
}

void GridViewBase::hideNewRowEditor()
{
    if (!isNewRowEditing())
        return;

    {
        hideEditor();

        modelController()->endAddRow();

        //In the new row editing mode the focused row is equal to last added row. 
        if (modelController()->focusedRow() != Q_NULL)
            modelController()->removeModelRow(modelController()->focusedRow()->modelRowIndex());
        else
            Q_ASSERT(false);

        //m_rowsLayout->setNewRowContext(Q_NULL);
    }
    requestUpdateLayout();
}

bool GridViewBase::closeNewRowEditor()
{
    if (modelController()->model() == Q_NULL)
        return false;

    if (!isNewRowEditing())
        return true;

    if (!closeEditorInternal())
        return false;

    if (!m_isNewRowModify)
    {
        hideNewRowEditor();
        return true;
    }
    else
    {
        GridViewAutoUpdater autoUpdater(this);
        if (modelController()->submitData())
        {
            modelController()->endAddRow();
            requestUpdateLayout();
            return true;
        }
    }

    raiseError(Grid::tr_compatible(QtnGridDataErrorString), Grid::tr_compatible(QtnGridDataErrorDescription_004_String));
    return false;
}

void GridViewBase::print(QPrinter* printer)
{
    /* Sets the attribute Qt::WA_WState_InPaintEvent to properly render the QWidgets
    which has been embeded into the grid's cell.  For more info please look at the
    EmbedGraphicPrimitive::eventFilter(...) function.
    */
    grid()->setAttribute(Qt::WA_WState_InPaintEvent);

    int sx = layout()->scrollX();
    int sy = layout()->scrollY();
    layout()->setScrollX(0);
    layout()->setScrollY(0);

    QPainter painter(printer);
    QList<PrinterColumnLayoutItem> items;

    bool verticalLayout = layout()->orientation() == Qt::Vertical;

    GridRowsLayoutEngine* rowsLayoutEngine = layout()->rowsPanel()->createPrinterLayoutEngine();
    int headerWidth = layout()->getViewWidth();
    int headerHeight = layout()->getViewHeight();

    int rowIndex = 0;
    bool first = true;
    while (rowIndex < modelController()->getGridRowCount())
    {
        int rowsLength = verticalLayout ? printer->pageRect().height() : printer->pageRect().width();

        if (first)
        {
            if (verticalLayout)
                rowsLength -= headerHeight;
            else
                rowsLength -= headerWidth;
        }

        if (rowIndex > 0) //Skip if it is a first page to print.
            printer->newPage();

        if (verticalLayout)
        {
            rowsLayoutEngine->setRowMask(QRect(QPoint(0, 0), QSize(headerWidth, printer->pageRect().height() - headerHeight)));
            rowsLayoutEngine->beginUpdate(headerWidth, rowsLength, 0, true);
            rowsLayoutEngine->printer_setRowsPageSize(headerWidth, rowsLength);
        }
        else
        {
            rowsLayoutEngine->setRowMask(QRect(QPoint(0, 0), QSize(printer->pageRect().width() - headerWidth, headerHeight)));
            rowsLayoutEngine->beginUpdate(headerHeight, rowsLength, 0, true);
            rowsLayoutEngine->printer_setRowsPageSize(rowsLength, headerHeight);
        }

        for (int i = rowIndex; i < modelController()->getGridRowCount(); ++i)
        {
            GridRow* gridRow = modelController()->getGridRow(i);
            if (!rowsLayoutEngine->appendGridRow(gridRow))
                break;
            rowIndex++;
        }
        rowsLayoutEngine->endUpdate();

        bool isLastRowPrinted = true;
        bool isFooterPrinted = false;
        bool isFooter = rowIndex == modelController()->getGridRowCount();

        printPage(rowsLayoutEngine, rowsLength, painter, printer, first, isFooter, items, isLastRowPrinted, isFooterPrinted);

        if (!isLastRowPrinted)
        {
            rowIndex = layout()->alignGridRowIndex(rowIndex - 1);
            isFooter = false;
        }

        first = false;
        if (verticalLayout)
            headerHeight = 0;
        else
            headerWidth = 0;

        if (isFooter && !isFooterPrinted)
        {
            printer->newPage();
            printPage(Q_NULL, rowsLength, painter, printer, first, isFooter, items, isLastRowPrinted, isFooterPrinted);
        }
    }

    delete rowsLayoutEngine;
    grid()->setAttribute(Qt::WA_WState_InPaintEvent, false);

    layout()->setScrollX(sx);
    layout()->setScrollY(sy);
}

static void print_gp(QPainter& painter, GraphicPrimitive* gp)
{
    painter.save();
    painter.translate(gp->left(), gp->top());
    gp->print(painter);
    painter.restore();
}

void GridViewBase::printPage(GridRowsLayoutEngine* rowsLayoutEngine,
    const int rowsHeight,
    QPainter& painter,
    QPrinter* printer,
    const bool header,
    const bool footer,
    QList<PrinterColumnLayoutItem>& items,
    bool& isLastRowPrinted,
    bool& isFooterPrinted)
{
    int itemIndex = -1;
    int multpageindex = -1;
    PrinterColumnLayoutItem item;
 
    item.offset = 0;
    
    isLastRowPrinted = true;
    isFooterPrinted = false;
    bool verticalLayout = layout()->orientation() == Qt::Vertical;

    for (;;)
    {
        painter.save();

        painter.setClipRect(QRect(QPoint(0, 0), printer->pageRect().size()));

        bool cont = false;
        int rowsLengthUsed = 0;
        if (header)
        {
            int pageLength = verticalLayout ? printer->pageRect().width() : printer->pageRect().height();
            if (!printHeader(painter, pageLength, itemIndex))
            {
                cont = true;
                PrinterColumnLayoutItem newItem;
                if (verticalLayout)
                    newItem.offset = item.offset + printer->pageRect().width() - pageLength;
                else
                    newItem.offset = item.offset + printer->pageRect().height() - pageLength;

                items.push_back(newItem);
            }
            //rowsLengthUsed -= ;
        }

        if (verticalLayout)
            painter.translate(-item.offset, 0);
        else
            painter.translate(0, -item.offset);

      

        QList<GridRowLayoutItem *> rows;
        if (rowsLayoutEngine != Q_NULL)
            rows = rowsLayoutEngine->printer_rows();
        GridRowLayoutItem* rowLayoutItem = Q_NULL;


        for (QList<GridRowLayoutItem *>::const_iterator it = rows.constBegin(); it != rows.constEnd(); ++it)
        {
            rowLayoutItem = *it;
            painter.save();

            if (verticalLayout)
            {
                //painter.translate(0, rowsLengthUsed);
                if (rowLayoutItem->isFirstItem())
                    rowsLengthUsed += rowLayoutItem->rect().height();
            }
            else
            {
                //painter.translate(rowsLengthUsed, 0);
                if (rowLayoutItem->isFirstItem())
                    rowsLengthUsed += rowLayoutItem->rect().width();
            }


            if (rowLayoutItem->quickRowButtonGP()->visibility())
            {
                print_gp(painter, rowLayoutItem->quickRowButtonGP());
            }

            switch (rowLayoutItem->type())
            {
            case GridTableRowLayoutItem::Data:
            {
                print_gp(painter, rowLayoutItem->dataRowGP());
            }
            break;
            case GridTableRowLayoutItem::Group:
            {
                print_gp(painter, rowLayoutItem->groupRowGP());
            }
            break;
            case GridTableRowLayoutItem::Summary:
            {
                print_gp(painter, rowLayoutItem->summaryRowGP());
            }
            break;
            default:
                break;
            }

            painter.restore();

            if (rowsLengthUsed > rowsHeight)
            {
                isLastRowPrinted = false;
            }
        }

        /*if (rowLayoutItem != Q_NULL)
        {
            if (verticalLayout)
                isLastRowPrinted = rowsHeight > rowLayoutItem->rect().top() + rowLayoutItem->rect().height();
            else
                isLastRowPrinted = rowsHeight > rowLayoutItem->rect().left() + rowLayoutItem->rect().width();
        }*/

        if (multpageindex == -1)
            multpageindex = 0;

        cont = cont || multpageindex < items.size();
        if (cont)
            item = items[multpageindex];
        multpageindex++;


        if (footer && isLastRowPrinted)
        {
            if (!layout()->bottomRows()->visibility())
            {
                isFooterPrinted = true;
            }
            else
            {
                if (verticalLayout)
                {
                    painter.translate(0, rowsLengthUsed);
                    rowsLengthUsed += layout()->summaryPanel()->height();
                }
                else
                {
                    painter.translate(rowsLengthUsed, 0);
                    rowsLengthUsed += layout()->summaryPanel()->width();
                }

 
                if (rowsLengthUsed <= rowsHeight)
                {
                    isFooterPrinted = true;
                    layout()->summaryPanel()->print(painter);
                }
            }
        }

        painter.restore();

        if (!cont)
            break;
        printer->newPage();
    }
}

bool GridViewBase::setFocusedRowIndex(int rowIndex, Qt::KeyboardModifiers modifiers, GridEditorActivationPolicyFlag flag)
{
    return setCellFocus(rowIndex, modelController()->focusedColumnIndex(), modifiers, flag);
}

bool GridViewBase::setFocusedColumnIndex(int columnIndex, Qt::KeyboardModifiers modifiers, GridEditorActivationPolicyFlag flag)
{
    return setCellFocus(modelController()->focusedRowIndex(), columnIndex, modifiers, flag);
}

GridRow* GridViewBase::getRow(const QModelIndex& index) const
{
    return modelController()->getGridRowByModelRow(index.row());
}

GridRow* GridViewBase::getRow(int rowIndex) const
{
    return modelController()->getGridRow(rowIndex);
}

int GridViewBase::getRowCount() const
{
    return modelController()->getGridRowCount();
}

/*!
Removes \a count rows starting with the given \a row from the view and model. Before rows will be removing, the signal rowRemoving() has raised.
In the handler of the rowRemoving() signal you may process some action before. For instance show agree message box or
disable the rows deletion by set the RowRemovingEventArgs::setHandled() to true of the \a args.
If the lastRow is NULL then the deletion will be continued till the last row in the view.

\sa RowRemovingEventArgs, rowRemoving()
*/
bool GridViewBase::removeRows(GridRow* row, int count)
{
    if (!isNewRowEditing() && row != Q_NULL)
    {
        RowRemovingEventArgs args(row, count);
        emit rowRemoving(&args);
        if (args.isHandled())
            return false;

        QList<int> rows;
        while (count > 0)
        {
            rows.push_back(row->modelRowIndex());
            if (row->rowIndex() == modelController()->getGridRowCount() - 1)
                break;
            row = modelController()->getGridRow(row->rowIndex() + 1);
            count--;
        }

        bool error = false;
        {
            GridViewAutoUpdater autoUpdater(this);

            for (QList<int>::const_iterator it = rows.constBegin(); it != rows.constEnd(); ++it)
            {
                if (!modelController()->removeModelRow(*it))
                {
                    error = true;
                    break;
                }
            }
        }

        if (error)
        {
            raiseError(Grid::tr_compatible(QtnGridDataErrorString), Grid::tr_compatible(QtnGridDataErrorDescription_005_String));
            return false;
        }

        if (!modelController()->submitData())
        {
            raiseError(Grid::tr_compatible(QtnGridDataErrorString), Grid::tr_compatible(QtnGridDataErrorDescription_004_String));
            return false;
        }

        return true;
    }
    return false;
}

/*!
Removes the row from the view and model. Before row will be removing, the signal rowRemoving() has raised. In the handler of the rowRemoving() signal you may process some action before.
For instance show agree message box or disable the row deletion by set the RowRemovingEventArgs::setHandled() to true of the \a args.

\sa RowRemovingEventArgs, rowRemoving()
*/
bool GridViewBase::removeRow(GridRow* row)
{
    return removeRows(row, 1);
}


void GridViewBase::selectRow(int rowIndex, const SelectionOperation& operation)
{
    selectRowRange(rowIndex, rowIndex, operation);
}

void GridViewBase::selectRowRange(int firstRowIndex, int lastRowIndex, const SelectionOperation& operation)
{
    GridDataTableColumn* f = getFirstColumn();
    GridDataTableColumn* l = getLastColumn();
    if (f == Q_NULL || l == Q_NULL)
        return;

    if (optionsref()->isMultiSelect())
        selectRange(firstRowIndex, lastRowIndex, f->index(), l->index(), operation);
    else
        selectRange(firstRowIndex, firstRowIndex, f->index(), l->index(), operation);
}

void GridViewBase::selectRange(int firstRowIndex, int lastRowIndex,
    int firstColumnIndex, int lastColumnIndex,
    const SelectionOperation& operation)
{
    if (firstRowIndex > lastRowIndex)
        exchange_int(firstRowIndex, lastRowIndex);

    Q_UNUSED(lastRowIndex);
    GridRow* firstRow = modelController()->getGridRow(firstRowIndex);
    if (firstRow == Q_NULL)
    {
        //error
        Q_ASSERT(false);
        modelController()->clearSelect();
        return;
    }

    GridDataTableColumn* currentColumn = static_cast<GridDataTableColumn *>(getColumn(firstColumnIndex));
    GridDataTableColumn* lastColumn = static_cast<GridDataTableColumn *>(getColumn(lastColumnIndex));
    if (!isColumnVisible(currentColumn) || !isColumnVisible(lastColumn))
        return;

    GridRow* lastRow = modelController()->getGridRow(lastRowIndex);

    if (lastRow == Q_NULL)
    {
        //error
        Q_ASSERT(false);
        modelController()->clearSelect();
        return;
    }

    if (currentColumn != Q_NULL && lastColumn != Q_NULL)
    {
        bool lessOrder = layout()->compareColumnOrder(currentColumn, lastColumn);
        
        if (lessOrder)
        {
            GridDataTableColumn* temp = currentColumn;
            currentColumn = lastColumn;
            lastColumn = temp;
        }
    }

    QList<GridDataTableColumn *> columns;
    while (currentColumn != Q_NULL)
    {
        if (isColumnVisible(currentColumn))
            columns.push_back(currentColumn);

        if (currentColumn == lastColumn)
            break;
        currentColumn = getNextColumn(currentColumn);
    }

    selectColumnRange(firstRow, lastRow, columns, operation);
}

void GridViewBase::selectColumnRange(GridRow* firstRow, GridRow* lastRow, const QList<GridDataTableColumn *>& columns, const SelectionOperation& operation)
{
    SelectionRanges ranges;
    for (QList<GridDataTableColumn *>::const_iterator it = columns.constBegin(); it != columns.constEnd(); ++it)
    {
        GridCell firstCell(firstRow, (*it)->index());
        GridCell lastCell(lastRow, (*it)->index());
        ranges.append(GridCellSelectionRange(modelController(), firstCell, lastCell));
    }

    if (ranges.isEmpty())
    {
        GridCell firstCell(firstRow, -1);
        GridCell lastCell(lastRow, -1);
        ranges.append(GridCellSelectionRange(modelController(), firstCell, lastCell));
    }

    selectCurrentRanges(ranges, operation);
}


void GridViewBase::getColumnLine(const GridColumnBase* /*column*/, GridColumns& retval) const
{
    for (GridColumns::const_iterator it = m_columnVisualIndexies.constBegin(); it != m_columnVisualIndexies.constEnd(); ++it)
    {
        if (!isColumnVisible(*it))
            continue;
        retval.push_back(*it);
    }
}

void GridViewBase::selectCurrentRanges(const SelectionRanges& ranges, const SelectionOperation& operation)
{
    SelectAutoUpdater updater(modelController());
    modelController()->selectRanges(ranges, operation);
}

/*!
Select all cells or rows in a view in multi-select mode.
\sa selection(), deselectAll(), selectRange(), selectRow(), selectRowRange()
*/
void GridViewBase::selectAll()
{
    GridDataTableColumn* f = getFirstColumn();
    GridDataTableColumn* l = getLastColumn();
    if (f == Q_NULL || l == Q_NULL)
        return;

    if (optionsref()->isMultiSelect())
        selectRange(0, modelController()->getGridRowCount() - 1, f->index(), l->index());
    else if (optionsref()->selectionPolicy() == SelectSingleCell)
        selectRange(modelController()->focusedRowIndex(), modelController()->focusedRowIndex(), modelController()->focusedColumnIndex(), modelController()->focusedColumnIndex());
    else
        selectRange(modelController()->focusedRowIndex(), modelController()->focusedRowIndex(), f->index(), l->index());

    layout()->update();
}

/*!
Undo selection for all cells or rows in a view in a multi-select mode.
\sa selection(), selectAll(), selectRange(), selectRow(), selectRowRange()
*/
void GridViewBase::deselectAll()
{
    SelectAutoUpdater updater(modelController());
    modelController()->selectRanges(SelectionRanges(), Qtitan::Clear);
    layout()->update();
}

/*!
Returns GridSelection object to iterate by all selected cells or rows.
\sa selectAll(), deselectAll(), selectRange(), selectRow(), selectRowRange()
*/
GridSelection* GridViewBase::selection() const
{
    if (modelController() == Q_NULL)
        return Q_NULL;
    return modelController()->selection();
}


/*!
\internal
*/
void GridViewBase::initViewFocus()
{
    int rowIndex = -1;
    GridRow* row = modelController()->focusedRow();
    if (row != Q_NULL)
        rowIndex = row->rowIndex();
    else if (modelController()->getGridRowCount() > 0)
        rowIndex = 0;

    int columnIndex = modelController()->focusedColumnIndex();
    if (columnIndex == -1)
    {
        GridTableColumnBase* column = getFirstColumn();
        if (column != Q_NULL)
            columnIndex = column->index();
    }
    setCellFocus(rowIndex, columnIndex);
}


void GridViewBase::updateSelection(int rowIndex, int columnIndex, bool focusChanged, Qt::KeyboardModifiers modifiers, const GridEditorActivationPolicyFlag& activationFlag)
{
    SelectionOperation selectOperation;

    if (!optionsref()->isMultiSelect())
    {
        modifiers &= ~Qt::ShiftModifier;
        selectOperation |= Qtitan::Clear;
    }

    if (!(modifiers & Qt::ShiftModifier) && !(modifiers & Qt::ControlModifier))
    {
        selectOperation |= Qtitan::Clear;
        selectOperation |= Qtitan::Select;
        m_shiftColumnIndex = columnIndex;
        m_shiftRowIndex = rowIndex;
    }

    if (focusChanged || modifiers & Qt::ControlModifier)
    {
        if (modifiers & Qt::ControlModifier)
        {
            if (activationFlag & SelectCurrentEvent)
            {
                if (!(modifiers & Qt::ShiftModifier))
                {
                    m_shiftSelect = false;
                    GridRow* newFocusedRow = modelController()->getGridRow(rowIndex);
                    if (newFocusedRow != Q_NULL)
                    {
                        if (optionsref()->selectionPolicy() == SelectMultiRow || optionsref()->selectionPolicy() == SelectSingleRow)
                            m_shiftSelect = !modelController()->isRowSelected(newFocusedRow);
                        else if (optionsref()->selectionPolicy() == SelectMultiCell || optionsref()->selectionPolicy() == SelectSingleCell)
                            m_shiftSelect = !modelController()->isCellSelected(GridCell(newFocusedRow, columnIndex));
                    }

                    if (m_shiftSelect)
                        selectOperation |= Qtitan::Select;
                    else
                        selectOperation |= Qtitan::Unselect;

                    m_shiftColumnIndex = columnIndex;
                    m_shiftRowIndex = rowIndex;
                }
            }
        }
        else
            m_shiftSelect = true;


        if (modifiers & Qt::ShiftModifier)
        {
            selectOperation |= Qtitan::StartCurrent;
            if (m_shiftSelect)
                selectOperation |= Qtitan::Select;
            else
                selectOperation |= Qtitan::Unselect;
        }

        int startRowIndex = m_shiftRowIndex != -1 ? m_shiftRowIndex : rowIndex;
        int endRowIndex = rowIndex;

        int startColumnIndex = m_shiftColumnIndex != -1 ? m_shiftColumnIndex : columnIndex;
        int endColumnIndex = columnIndex;

        if (optionsref()->selectionPolicy() == SelectSingleRow ||
            optionsref()->selectionPolicy() == SelectMultiRow)
        {
            GridDataTableColumn* c1 = getFirstColumn();
            GridDataTableColumn* c2 = getLastColumn();
            if (c1 != Q_NULL)
                startColumnIndex = c1->index();
            if (c2 != Q_NULL)
                endColumnIndex = c2->index();
        }


        selectRange(startRowIndex, endRowIndex, startColumnIndex, endColumnIndex, selectOperation);
    }
}

void GridViewBase::slot_removeRow()
{
    if (m_contextHit == 0)
    {
        if (focusedRow() != Q_NULL)
            removeRow(focusedRow());
    }
    else
    {
        GridTableCellGraphic* cell = qobject_cast<GridTableCellGraphic *>(m_contextHit);
        if (cell == Q_NULL || cell->gridRow() == Q_NULL)
            return;
        removeRow(cell->gridRow());
    }
}

static GridColumnBase* cast_hitToTableColumn(GridViewBase* view, GraphicPrimitive* gp, bool resolveCell = false)
{
    GridColumnPrimitive* graphic = qobject_cast<GridColumnPrimitive *>(gp);
    if (graphic == Q_NULL)
    {
        if (!resolveCell)
            return Q_NULL;
        GridTableCellGraphic* cell = qobject_cast<GridTableCellGraphic *>(gp);
        if (cell == Q_NULL)
            return Q_NULL;
        return view->getColumn(cell->columnIndex());
    }
    return graphic->column();
}

void GridViewBase::sortNone()
{
    GridColumnBase* column = cast_hitToTableColumn(this, m_contextHit);
    if (column == Q_NULL || qobject_cast<GridDataTableColumn *>(column) == 0)
        return;
    qobject_cast<GridDataTableColumn *>(column)->setSortOrder(SortNone);
}

void GridViewBase::sortAscending()
{
    GridColumnBase* column = cast_hitToTableColumn(this, m_contextHit);
    if (column == Q_NULL || qobject_cast<GridDataTableColumn *>(column) == 0)
        return;
    qobject_cast<GridDataTableColumn *>(column)->setSortOrder(SortAscending);
}

void GridViewBase::sortDescending()
{
    GridColumnBase* column = cast_hitToTableColumn(this, m_contextHit);
    if (column == Q_NULL || qobject_cast<GridDataTableColumn *>(column) == 0)
        return;
    qobject_cast<GridDataTableColumn *>(column)->setSortOrder(SortDescending);
}

void GridViewBase::hideColumn()
{
    GridColumnBase* column = cast_hitToTableColumn(this, m_contextHit);
    if (column == Q_NULL)
        return;
    column->setVisible(false);
}

void GridViewBase::groupByColumn()
{
    GridColumnBase* column = cast_hitToTableColumn(this, m_contextHit);
    if (column == Q_NULL || qobject_cast<GridDataTableColumn *>(column) == 0)
        return;
    qobject_cast<GridDataTableColumn *>(column)->setGroupIndex(m_groups.size());
}

void GridViewBase::alignLeftAct()
{
    GridColumnBase* column = cast_hitToTableColumn(this, m_contextHit, true);
    if (column == Q_NULL)
        return;

    Qt::Alignment alignment = column->textAlignment();
    alignment &= ~(Qt::AlignRight | Qt::AlignHCenter);
    column->setTextAlignment(alignment | Qt::AlignLeft);

    layout()->updateRows(true);
}

void GridViewBase::alignRightAct()
{
    GridColumnBase* column = cast_hitToTableColumn(this, m_contextHit, true);
    if (column == Q_NULL)
        return;

    Qt::Alignment alignment = column->textAlignment();
    alignment &= ~(Qt::AlignLeft | Qt::AlignHCenter);
    column->setTextAlignment(alignment | Qt::AlignRight);

    layout()->updateRows(true);
}

void GridViewBase::alignCenterAct()
{
    GridColumnBase* column = cast_hitToTableColumn(this, m_contextHit, true);
    if (column == Q_NULL)
        return;

    Qt::Alignment alignment = column->textAlignment();
    alignment &= ~(Qt::AlignLeft | Qt::AlignRight);
    column->setTextAlignment(alignment | Qt::AlignHCenter);

    layout()->updateRows(true);
}

void GridViewBase::groupByBox()
{
    optionsref()->setGroupsHeader(!optionsref()->groupsHeader());
}

void GridViewBase::fieldChooser()
{
    showFieldChooser();
}

void GridViewBase::showHideFooter()
{
    optionsref()->setFooterSummaryVisible(!optionsref()->isFooterSummaryVisible());
}

void GridViewBase::showHideGroupFooters()
{
    if (optionsref()->groupSummaryPlace() == GroupSummaryHide)
        optionsref()->setGroupSummaryPlace(GroupSummaryFooter);
    else
        optionsref()->setGroupSummaryPlace(GroupSummaryHide);
}

/*!
Expands all the groups in the view.
*/
void GridViewBase::expandAll()
{
    modelController()->expandAllGroup();
}

/*!
Collapses all the groups in the view.
*/
void GridViewBase::collapseAll()
{
    modelController()->collapseAllGroup();
}

bool GridViewBase::checkActivationEditorPolicy(GridEditorActivationPolicyFlag policyFlag)
{
    GridDataTableColumn* column = static_cast<GridDataTableColumn *>(getColumn(modelController()->focusedColumnIndex()));
    if (column == Q_NULL || column->editorRepository() == Q_NULL || column->editorRepository()->editorActivationPolicy() == NoPolicy)
        return false;
    return column->editorRepository()->editorActivationPolicy().testFlag(policyFlag);
}

void GridViewBase::bestFit()
{
    GridViewAutoUpdater autoUpdater(this);
    bestFitForView();
}

void GridViewBase::showContextMenu(const QPoint& pos)
{
    GridContextMenu* menu = new GridContextMenu(grid());
    menu->addAction(m_actions[FilterAction]);
    menu->addAction(m_actions[FindAction]);
    menu->addSeparator();

    menu->addAction(m_actions[SortAscendingAction]);
    menu->addAction(m_actions[SortDescendingAction]);
    menu->addAction(m_actions[SortNoneAction]);
    menu->addSeparator();

    menu->addAction(m_actions[BestFitAction]);
    menu->addAction(m_actions[ZoomInAction]);
    menu->addAction(m_actions[ZoomOutAction]);
    menu->addSeparator();

    menu->addAction(m_actions[ExpandAllAction]);
    menu->addAction(m_actions[CollapseAllAction]);
    menu->addAction(m_actions[GroupByColumnAction]);
    menu->addAction(m_actions[GroupByBoxAction]);
    menu->addSeparator();

    menu->addAction(m_actions[HideColumnAction]);
    menu->addAction(m_actions[FieldChooserAction]);
    menu->addSeparator();

    menu->addAction(m_actions[FooterAction]);
    menu->addAction(m_actions[GroupFootersAction]);

    QMenu* alignmentMenu = menu->addMenu(Grid::tr_compatible(QtnGridActionAlignmentString));
    alignmentMenu->addAction(m_actions[AlignLeftAction]);
    alignmentMenu->addAction(m_actions[AlignRightAction]);
    alignmentMenu->addAction(m_actions[AlignCenterAction]);
    menu->addSeparator();
    menu->addAction(m_actions[DeleteRowAction]);

    m_contextHit = ((GridGraphicSitePublic *)grid())->hitTest(pos);
    Q_ASSERT(m_contextHit != Q_NULL);
    GridDataTableColumn* tableColumn = qobject_cast<GridDataTableColumn *>(cast_hitToTableColumn(this, m_contextHit));

    m_actions[ZoomInAction]->setVisible(optionsref()->zoomEnabled());
    m_actions[ZoomOutAction]->setVisible(optionsref()->zoomEnabled());

    m_actions[ZoomInAction]->setEnabled(optionsref()->zoomEnabled() && optionsref()->zoomFactor() < 5.0);
    m_actions[ZoomOutAction]->setEnabled(optionsref()->zoomEnabled() && optionsref()->zoomFactor() > 0.25);

    m_actions[ExpandAllAction]->setEnabled(modelController()->hasGrouping());
    m_actions[CollapseAllAction]->setEnabled(modelController()->hasGrouping());

    m_actions[SortNoneAction]->setEnabled(tableColumn != Q_NULL &&
        optionsref()->isSortEnabled() && tableColumn->isSortEnabled());
    m_actions[SortAscendingAction]->setEnabled(tableColumn != Q_NULL &&
        optionsref()->isSortEnabled() && tableColumn->isSortEnabled());
    m_actions[SortDescendingAction]->setEnabled(tableColumn != Q_NULL &&
        optionsref()->isSortEnabled() && tableColumn->isSortEnabled());
    m_actions[HideColumnAction]->setEnabled(tableColumn != Q_NULL &&
        checkRemove(tableColumn));
    m_actions[GroupByColumnAction]->setEnabled(tableColumn != Q_NULL &&
        checkGrouping(tableColumn));

    alignmentMenu->setEnabled(tableColumn != Q_NULL);

    m_actions[AlignLeftAction]->setCheckable(true);
    m_actions[AlignLeftAction]->setChecked(false);

    m_actions[AlignRightAction]->setCheckable(true);
    m_actions[AlignRightAction]->setChecked(false);

    m_actions[AlignCenterAction]->setCheckable(true);
    m_actions[AlignCenterAction]->setChecked(false);

    if (tableColumn != Q_NULL)
    {
        m_actions[AlignLeftAction]->setChecked(tableColumn->textAlignment() & Qt::AlignLeft);
        m_actions[AlignRightAction]->setChecked(tableColumn->textAlignment() & Qt::AlignRight);
        m_actions[AlignCenterAction]->setChecked(tableColumn->textAlignment() & Qt::AlignHCenter);
    }

    ContextMenuEventArgs args(this, menu, grid()->hitInfo(pos));
    emit contextMenu(&args);
    if (args.isHandled())
    {
        delete menu;
        return;
    }
    QAction* action = menu->exec(layout()->mapToGlobal(pos));
    delete menu;
    Q_UNUSED(action);
}

static void set_summary_type(GridModelController* controller, GridSummaryCellPrimitive* cell, GridSummaryType type)
{
    if (cell == Q_NULL)
        return;

    controller->beginUpdate();
    cell->setSummaryType(type);
    controller->endUpdate();
}

void GridViewBase::summaryNone(GridSummaryCellPrimitive* cell)
{
    set_summary_type(modelController(), cell, SummaryNone);
}

void GridViewBase::summarySum(GridSummaryCellPrimitive* cell)
{
    set_summary_type(modelController(), cell, SummarySum);
}

void GridViewBase::summaryMin(GridSummaryCellPrimitive* cell)
{
    set_summary_type(modelController(), cell, SummaryMin);
}

void GridViewBase::summaryMax(GridSummaryCellPrimitive* cell)
{
    set_summary_type(modelController(), cell, SummaryMax);
}

void GridViewBase::summaryCount(GridSummaryCellPrimitive* cell)
{
    set_summary_type(modelController(), cell, SummaryCount);
}

void GridViewBase::summaryAvg(GridSummaryCellPrimitive* cell)
{
    set_summary_type(modelController(), cell, SummaryAvg);
}


void GridViewBase::printAreaRequested(QPrinter *printer)
{
    print(printer);
}

void GridViewBase::printPreview()
{
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setOrientation(layout()->orientation() == Qt::Vertical ? QPrinter::Portrait : QPrinter::Landscape);
    QPrintPreviewDialog preview(&printer);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printAreaRequested(QPrinter*)));
    preview.exec();
}

QAbstractItemModel* GridViewBase::model() const
{
    return modelController()->model();
}

GridViewOptions* GridViewBase::optionsref() const
{
    return m_options;
}

void GridViewBase::columnVisibleChanged(GridColumnBase* columnBase, bool visible)
{
    GridViewAutoUpdater autoUpdater(this);
    layout()->columnVisibleChanged(columnBase, visible);
 
    if (visible)
    {
        if (modelController()->focusedColumnIndex() == -1)
            setFocusedColumnIndex(columnBase->index());
    }
    else
    {
        if (modelController()->focusedColumnIndex() == columnBase->index())
        {
            if (isEditing())
            {
                if (!closeEditor())
                    hideEditor();
            }

            GridTableColumn* column = static_cast<GridTableColumn *>(columnBase);
            GridTableColumnBase* next = getNextColumn(column);
            if (next == Q_NULL)
                next = getFirstColumn();
            if (next != Q_NULL && next != column)
                setFocusedColumnIndex(next->index());
            else
                setFocusedColumnIndex(-1);
        }
    }
    markColumnsChanged();
}

void GridViewBase::columnCreated(GridColumnBase* column)
{
    layout()->columnCreated(column);
}

void GridViewBase::columnDestroying(GridColumnBase* column)
{
    layout()->columnDestroying(column);
}

void GridViewBase::columnWidthChanged(GridColumnBase* column)
{
    Q_UNUSED(column);
    markColumnsChanged();
}

void GridViewBase::raiseError(const QString& error, const QString& description)
{
    ErrorEventArgs args(this, error, description);
    emit this->error(&args);
    if (args.isHandled())
        return;
   QMessageBox::critical(grid(), args.error(), args.description());
}

void GridViewBase::focusInEvent(QFocusEvent *)
{
    layout()->rowsPanel()->update();
}

void GridViewBase::focusOutEvent(QFocusEvent *)
{
    layout()->rowsPanel()->update();
}

QFont GridViewBase::getColumnFont(const GridColumnBase* column) const
{
    if (column == Q_NULL)
        return optionsref()->columnFont();
    QFont ret = column->font().resolve(optionsref()->columnFont());
    ret.resolve(ret.resolve() | optionsref()->columnFont().resolve());
    return ret;
}

int GridViewBase::getColumnHeight(const GridColumnBase* column) const
{
    Q_UNUSED(column);
    return optionsref()->columnHeight();
}

QPen GridViewBase::getColumnPen(const GridColumnBase* column) const
{
    if (column->pen() != QPen())
        return column->pen();
    else if (optionsref()->modelDecoration())
    {
        const GridDataTableColumn* tableColumn = qobject_cast<const GridDataTableColumn *>(column);
        if (tableColumn != Q_NULL)
        {
            QVariant variant;
            variant = model()->headerData(tableColumn->dataBinding()->column(), Qt::Vertical, Qt::ForegroundRole);
            QPen pen;
            if (variant.type() == QVariant::Color)
                pen.setColor(variant.value<QColor>());
            else if (variant.type() == QVariant::Brush)
                pen.setBrush(variant.value<QBrush>());
            else if (variant.type() == QVariant::Pen)
                pen = variant.value<QPen>();

            if (pen != QPen())
                return pen;
        }
    }

    return QPen(m_options->columnPen());
}

GridViewPrimitiveBase* GridViewBase::layout() const
{
    return m_viewPrimitive;
}

QHash<GridActionType, QAction*>& GridViewBase::actions()
{
    return m_actions;
}

/*!
\internal
*/
void GridViewBase::initialize()
{
    m_options = createOptions();
    m_viewPrimitive = createLayout();
    m_viewPrimitive->init(this);
    updateBorder();
    m_options->setWindowsAirSupported(false);
    createActions();
}

void GridViewBase::finalize()
{
}

void GridViewBase::destroyFilterDialog()
{
    Q_DELETE_AND_NULL(m_filterDlg);
}

int GridViewBase::getRowGroupIndent(GridRow* gridRow) const
{
    int indent = 1;
    if (optionsref()->groupRowStyle() == GridGroupOfficeOutlook)
        indent = 0;

    if (gridRow == Q_NULL)
        return (modelController()->groupingDepth() + indent - 1) * kGridGroupRowIndent;

    if (gridRow->type() == GridRow::DataRow)
        return (gridRow->depth() + indent) * kGridGroupRowIndent;
    else
        return gridRow->depth() * kGridGroupRowIndent;
}

bool GridViewBase::hasGroupingIndent(const GridColumnBase* column) const
{
    if (column == Q_NULL)
        return false;
    return modelController()->hasGrouping() && column->isShowingFirst();
}

void GridViewBase::textAlignmentChanged(GridColumnBase* columnBase)
{
    GridDataTableColumn* column = qobject_cast<GridDataTableColumn *>(columnBase);
    if (column != Q_NULL && column->editorRepository() != Q_NULL)
        column->editorRepository()->setAlignment(columnBase->textAlignment());
    layout()->update();
}

static QIcon create_menu_icon(const QString& fileName)
{
    QPixmap pixmap(fileName);
    pixmap.setMask(pixmap.createMaskFromColor(Qt::magenta));
    return QIcon(pixmap);
}

QMenu* GridViewBase::createSummaryContextMenu(QHash<GridSummaryType, QAction*>& summaryActions) const
{
    QMenu* summaryContextMenu = new QMenu(layout()->control());
    summaryActions.insert(SummarySum,
        summaryContextMenu->addAction(
        create_menu_icon(":res/summaryMenuSum.png"), Grid::tr_compatible(QtnGridActionSumString)));

    summaryActions.insert(SummaryMin,
        summaryContextMenu->addAction(
        create_menu_icon(":res/summaryMenuMin.png"), Grid::tr_compatible(QtnGridActionMinString)));
    summaryActions.insert(SummaryMax,
        summaryContextMenu->addAction(
        create_menu_icon(":res/summaryMenuMax.png"), Grid::tr_compatible(QtnGridActionMaxString)));
    summaryActions.insert(SummaryCount,
        summaryContextMenu->addAction(
        create_menu_icon(":res/summaryMenuCount.png"), Grid::tr_compatible(QtnGridActionCountString)));
    summaryActions.insert(SummaryAvg,
        summaryContextMenu->addAction(
        create_menu_icon(":res/summaryMenuAvg.png"), Grid::tr_compatible(QtnGridActionAverageString)));
    summaryContextMenu->addSeparator();
    summaryActions.insert(SummaryNone,
        summaryContextMenu->addAction(Grid::tr_compatible(QtnGridActionNoneString)));

    summaryActions[SummaryNone]->setCheckable(true);
    summaryActions[SummarySum]->setCheckable(true);
    summaryActions[SummaryMin]->setCheckable(true);
    summaryActions[SummaryMax]->setCheckable(true);
    summaryActions[SummaryCount]->setCheckable(true);
    summaryActions[SummaryAvg]->setCheckable(true);
    return summaryContextMenu;
}

void GridViewBase::showSummaryContextMenu(const QPoint& pos)
{
    GraphicPrimitive* gp = ((GridGraphicSitePublic *)grid())->hitTest(pos);
    if (gp->id() != GridHitInfo::GroupSummaryCell && gp->id() != GridHitInfo::FooterSummaryCell)
        return;

    GridSummaryCellPrimitive* cell = (GridSummaryCellPrimitive *)gp;

    const GridSummaryValue& value = cell->getValue();
    QHash<GridSummaryType, QAction*> summaryActions;
    QMenu* menu = createSummaryContextMenu(summaryActions);

    summaryActions[SummaryNone]->setChecked(value.type == SummaryNone);
    summaryActions[SummarySum]->setChecked(value.type == SummarySum);
    summaryActions[SummaryMax]->setChecked(value.type == SummaryMax);
    summaryActions[SummaryMin]->setChecked(value.type == SummaryMin);
    summaryActions[SummaryCount]->setChecked(value.type == SummaryCount);
    summaryActions[SummaryAvg]->setChecked(value.type == SummaryAvg);

    summaryActions[SummarySum]->setEnabled(cell->tableColumn()->dataBinding()->supportedSummaryTypes() & SummarySum);
    summaryActions[SummaryMax]->setEnabled(cell->tableColumn()->dataBinding()->supportedSummaryTypes() & SummaryMax);
    summaryActions[SummaryMin]->setEnabled(cell->tableColumn()->dataBinding()->supportedSummaryTypes() & SummaryMin);
    summaryActions[SummaryCount]->setEnabled(cell->tableColumn()->dataBinding()->supportedSummaryTypes() & SummaryCount);
    summaryActions[SummaryAvg]->setEnabled(cell->tableColumn()->dataBinding()->supportedSummaryTypes() & SummaryAvg);

    ContextMenuEventArgs args(this, menu, grid()->hitInfo(pos));
    emit contextMenu(&args);
    if (args.isHandled())
    {
        delete menu;
        return;
    }

    QAction* act = menu->exec(QPoint(layout()->mapToGlobal(pos)));
    delete menu;

    if (summaryActions[SummaryNone] == act)
        summaryNone(cell);
    if (summaryActions[SummarySum] == act)
        summarySum(cell);
    if (summaryActions[SummaryMax] == act)
        summaryMax(cell);
    if (summaryActions[SummaryMin] == act)
        summaryMin(cell);
    if (summaryActions[SummaryCount] == act)
        summaryCount(cell);
    if (summaryActions[SummaryAvg] == act)
        summaryAvg(cell);
    layout()->update();
}

void GridViewBase::doNavigatePrevColumn(Qt::KeyboardModifiers modifiers)
{
    if (focusedRow() != Q_NULL)
    {
        if (focusedRow()->type() == GridRow::GroupRow)
        {
            focusedRow()->setExpanded(false);
        }
        else
        {
            int index = modelController()->focusedColumnIndex();
            if (index == -1)
                return;
            const GridDataTableColumn* column =
                static_cast<const GridDataTableColumn *>(getColumn(index));
            column = getPrevColumn(column);
            if (column == Q_NULL)
                column = getLastColumn();
            if (column == Q_NULL)
                return;
            setFocusedColumnIndex(column->index(), modifiers, KeyboardFocus);
            layout()->update();
        }
    }
}

void GridViewBase::doNavigateNextColumn(Qt::KeyboardModifiers modifiers)
{
    if (focusedRow() != Q_NULL)
    {
        if (focusedRow()->type() == GridRow::GroupRow)
        {
            focusedRow()->setExpanded(true);
        }
        else
        {
            int index = modelController()->focusedColumnIndex();
            if (index == -1)
                return;
            const GridDataTableColumn* column =
                static_cast<const GridDataTableColumn *>(getColumn(index));
            column = getNextColumn(column);
            if (column == Q_NULL)
                column = getFirstColumn();
            if (column == Q_NULL)
                return;
            setFocusedColumnIndex(column->index(), modifiers, KeyboardFocus);
            layout()->update();
        }
    }
}

void GridViewBase::doNavigatePrevRow(Qt::KeyboardModifiers modifiers)
{
    int index = modelController()->focusedRowIndex();
    if (index == 0 && optionsref()->newRowPlace() == NewRowTop)
    {
        if (!(modifiers & Qt::ShiftModifier) && !(modifiers & Qt::ControlModifier))
            showNewRowEditor();
        return;
    }
    else if (isNewRowEditing())
    {
        if (optionsref()->newRowPlace() != NewRowBottom)
            return;
        closeNewRowEditor();
        return;
    }
    else
        index--;

    if (index >= 0 && index < modelController()->getGridRowCount())
    {
        setFocusedRowIndex(index, modifiers, KeyboardFocus);
    }
}

void GridViewBase::doNavigateNextRow(Qt::KeyboardModifiers modifiers)
{
    int index = modelController()->focusedRowIndex();
    if (index == modelController()->getGridRowCount() - 1 && optionsref()->newRowPlace() == NewRowBottom)
    {
        if (!(modifiers & Qt::ShiftModifier) && !(modifiers & Qt::ControlModifier))
            showNewRowEditor();
        return;
    }
    else if (isNewRowEditing())
    {
        if (optionsref()->newRowPlace() != NewRowTop)
            return;
        closeNewRowEditor();
        return;
    }
    else
        index++;

    if (index >= 0 && index < modelController()->getGridRowCount())
    {
        setFocusedRowIndex(index, modifiers, KeyboardFocus);
    }
}

GridEditorRepository* GridViewBase::createEditorRepository(const GridDataTableColumn* column)
{
    //By default the repository is selectable, editable and enabled.
    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;

    GridRow* first = modelController()->getGridRow(0);
    if (first != Q_NULL)
    {
        QModelIndex index = modelController()->model()->index(
            first->modelRowIndex(), column->dataBinding()->column(), modelController()->parentIndex());
        flags = modelController()->model()->flags(index);
    }

    GridEditorRepository* retval = Q_NULL;
    QVariant firstDisplayValue;
    QVariant firstEditValue;
    if (first != Q_NULL)
    {
        column->dataBinding()->getValue(first->modelRowIndex(), column->dataBinding()->displayRole(), firstDisplayValue);
        column->dataBinding()->getValue(first->modelRowIndex(), column->dataBinding()->editRole(), firstEditValue);
        if (firstEditValue == QVariant())
            firstEditValue = firstDisplayValue;
    }
    bool checkBoxEditorBool = false;

    if (column->editorType() == AutoEditorType)
    {
        switch (firstDisplayValue.type())
        {
        case QVariant::Invalid:
            retval = new GridStringEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummaryCount);
            break;
        case QVariant::Bool:
            retval = new GridCheckBoxEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummarySum |
                SummaryMin |
                SummaryMax |
                SummaryCount);
            if (flags & Qt::ItemIsTristate)
            {
                column->dataBinding()->setDisplayRole(Qt::CheckStateRole);
                column->dataBinding()->setEditRole(Qt::CheckStateRole);
            }
            checkBoxEditorBool = true;
            break;
        case QVariant::Int:
            retval = new GridNumericEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(
                SummarySum |
                SummaryMin |
                SummaryMax |
                SummaryCount |
                SummaryAvg);
            break;
        case QVariant::UInt:
            retval = new GridNumericEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(
                SummarySum |
                SummaryMin |
                SummaryMax |
                SummaryCount |
                SummaryAvg);
            break;
        case QVariant::LongLong:
            retval = new GridNumericEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(
                SummarySum |
                SummaryMin |
                SummaryMax |
                SummaryCount |
                SummaryAvg);
            break;
        case QVariant::ULongLong:
            retval = new GridNumericEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(
                SummarySum |
                SummaryMin |
                SummaryMax |
                SummaryCount |
                SummaryAvg);
            break;
        case QVariant::Double:
            retval = new GridNumericEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(
                SummarySum |
                SummaryMin |
                SummaryMax |
                SummaryCount |
                SummaryAvg);
            break;
        case QVariant::Char:
            retval = new GridStringEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(
                SummaryMin | SummaryMax | SummaryCount);
            break;
        case QVariant::Date:
            retval = new GridDateEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(
                SummaryMin | SummaryMax | SummaryCount);
            break;
        case QVariant::Time:
            retval = new GridTimeEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(
                SummarySum |
                SummaryMin |
                SummaryMax |
                SummaryCount |
                SummaryAvg);
            break;
        case QVariant::DateTime:
            retval = new GridDateTimeEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(
                SummaryMin | SummaryMax | SummaryCount);
            break;
        case QVariant::String:
            retval = new GridStringEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummaryCount);
            break;
        case QVariant::List:
        case QVariant::Map:
        case QVariant::StringList:
            retval = new GridComboBoxEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummaryCount);
            break;
        case QVariant::Icon:
        case QVariant::Image:
        case QVariant::Pixmap:
        case QVariant::Bitmap:
            retval = new GridPictureComboBoxEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummaryCount);
            break;
        default:
            retval = new GridStringEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummaryCount);
        }
    }
    else
    {
        switch (column->editorType())
        {
        case StringEditorType:
        {
            retval = new GridStringEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummaryCount);
        }
        break;
        case MemoEditorType:
        {
            retval = new GridMemoEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummaryCount);
        }
        break;
        case ComboBoxBlobEditorType:
        {
            retval = new GridBlobEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummaryCount);
        }
        break;
        case NumericEditorType:
        {
            retval = new GridNumericEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(
                SummarySum | SummaryMin | SummaryMax | SummaryCount | SummaryAvg);
        }
        break;
        case ProgressEditorType:
        {
            retval = new GridProgressEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(
                SummarySum | SummaryMin | SummaryMax | SummaryCount | SummaryAvg);
        }
        break;
        case CheckBoxEditorType:
        {
            retval = new GridCheckBoxEditorRepository();
            if (flags & Qt::ItemIsTristate)
            {
                column->dataBinding()->setDisplayRole(Qt::CheckStateRole);
                column->dataBinding()->setEditRole(Qt::CheckStateRole);
            }
            checkBoxEditorBool = true;
        }
        break;

        case ComboBoxEditorType:
        {
            retval = new GridComboBoxEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummaryCount);
        }
        break;

        case DateTimeEditorType:
        {
            retval = new GridDateTimeEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(
                SummaryMin | SummaryMax | SummaryCount);
        }
        break;
        case TimeEditorType:
        {
            retval = new GridTimeEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(
                SummarySum | SummaryMin | SummaryMax | SummaryCount | SummaryAvg);
        }
        break;
        case DateEditorType:
        {
            retval = new GridDateEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(
                SummaryMin | SummaryMax | SummaryCount);
        }
        break;
        case PictureEditorType:
        {
            retval = new GridPictureEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummaryCount);
        }
        break;
        case ComboBoxPictureEditorType:
        {
            retval = new GridPictureComboBoxEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummaryCount);
        }
        break;
        case DelegateAdapterEditorType:
        {
            retval = new GridDelegateAdapterEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummaryCount);
        }
        break;
        case TableEditorType:
        {
            retval = new GridTableEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummaryCount);
        }
        break;
        default:
            retval = new GridStringEditorRepository();
            column->dataBinding()->setSupportedSummaryTypes(SummaryCount);
        }
    }

    retval->initialize(this, column->dataBinding()); //Initialize the repository. Set view to it.

    if (firstDisplayValue != QVariant())
        retval->setDefaultValue(QVariant((int)firstDisplayValue.type(), (void *)0), Qt::DisplayRole);
    if (firstEditValue != QVariant())
        retval->setDefaultValue(QVariant((int)firstEditValue.type(), (void *)0), Qt::EditRole);

    if (checkBoxEditorBool)
        ((GridCheckBoxEditorRepository *)retval)->setTristate(flags & Qt::ItemIsTristate);
    retval->setEnabled(flags & Qt::ItemIsEnabled);
    retval->setEditable(flags & Qt::ItemIsEditable);
    retval->setAlignment(column->textAlignment());
    emit editorRepositoryCreated(column, retval);
    return retval;
}

/*!
\internal
*/
void GridViewBase::updateBorder()
{
    layout()->setBorder(optionsref()->borderWidth());
    layout()->setBorderColor(optionsref()->borderColor());
}


bool GridViewBase::checkMove(const GridColumnBase* columnBase) const
{
    const GridDataTableColumn* column = qobject_cast<const GridDataTableColumn *>(columnBase);
    if (column == Q_NULL)
        return false;

    if (!optionsref()->isLastColumnMovingEnabled() && m_visibleColumnCount == 1 && columnBase->isVisible())
        return false;

    return optionsref()->isColumnMovingEnabled() && column->isMovingEnabled();
}

bool GridViewBase::checkRemove(const GridColumnBase* columnBase) const
{
    const GridDataTableColumn* column = qobject_cast<const GridDataTableColumn *>(columnBase);
    if (column == Q_NULL)
        return false;
    return optionsref()->isColumnHidingEnabled() && column->isHidingEnabled();
}

bool GridViewBase::checkGrouping(const GridColumnBase* columnBase) const
{
    const GridDataTableColumn* column = qobject_cast<const GridDataTableColumn *>(columnBase);
    if (column == Q_NULL)
        return false;
    return optionsref()->isGroupingEnabled() && column->isGroupingEnabled();
}

void GridViewBase::buildColumnFilters()
{
    GridFilterGroupCondition* newcond = new GridFilterGroupCondition(filter());
    for (GridViewBase::GridColumns::const_iterator column_it = m_columnslist.constBegin();
        column_it != m_columnslist.constEnd(); ++column_it)
    {
        GridDataTableColumn* column = static_cast<GridDataTableColumn *>(*column_it);
        if (column->m_filterCondition == Q_NULL)
            continue;
        newcond->addValue(
            static_cast<GridFilterValueCondition *>(column->m_filterCondition->clone()));
    }
    filter()->setCondition(newcond);
}

void GridViewBase::destroyColumnFilters()
{
    for (GridViewBase::GridColumns::const_iterator column_it = m_columnslist.constBegin();
        column_it != m_columnslist.constEnd(); ++column_it)
    {
        GridDataTableColumn* column = static_cast<GridDataTableColumn *>(*column_it);
        Q_DELETE_AND_NULL(column->m_filterCondition);
        Q_DELETE_AND_NULL(column->m_filterPopup);
    }
}

/*!
\internal
*/
void GridViewBase::raiseEditorCreated(GridEditor* editor)
{
    emit editorCreated(editor);
}

/*!
\internal
*/
void GridViewBase::raiseEditorDestroying(GridEditor* editor)
{
    emit editorDestroying(editor);
}

/*!
\internal
*/
void GridViewBase::raiseEditorModifying(GridEditor* editor)
{
    emit editorModifying(editor);

    if (editor->editorRepository()->immediatePost())
        postEditor();
}

/*!
\internal
*/
void GridViewBase::raiseEditorValidating(EditorValidationEventArgs* args)
{
    emit editorValidating(args);
}

/*!
\internal
*/
void GridViewBase::raiseEditorEditModeChanged(GridEditor* editor)
{
    emit editorEditModeChanged(editor);
}

static QAction* addActionToWidget(QWidget* widget, const QIcon & icon, const QString & text, const QObject * receiver, const char * member,
    const QKeySequence & shortcut = QKeySequence(), const QKeySequence & second_shortcut = QKeySequence())
{
    QAction* action = new QAction(icon, text, widget);
    if (shortcut != QKeySequence())
    {
        if (second_shortcut == QKeySequence())
            action->setShortcut(shortcut);
        else
        {
            QList<QKeySequence> l;
            l.append(shortcut);
            l.append(second_shortcut);
            action->setShortcuts(l);
        }
        action->setShortcutContext(Qt::WidgetShortcut);
    }
    action->connect(action, SIGNAL(triggered()), receiver, member);
    widget->addAction(action);
    return action;
}

void GridViewBase::createActions()
{
    m_actions.insert(FilterAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionFilterString), this, SLOT(executeFilterDialog()), QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_F)));
    m_actions.insert(FindAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionFindString), this, SLOT(showFindPanel()), QKeySequence::Find));
    m_actions.insert(FindNextAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionFindString), this, SLOT(findNext()), QKeySequence::FindNext));
    m_actions.insert(FindPreviousAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionFindString), this, SLOT(findPrevious()), QKeySequence::FindPrevious));

    m_actions.insert(BestFitAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionBestFitString), this, SLOT(bestFit()), QKeySequence::Refresh));

    m_actions.insert(SortAscendingAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionSortAscendingString), this, SLOT(sortAscending())));
    m_actions.insert(SortDescendingAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionSortDescendingString), this, SLOT(sortDescending())));
    m_actions.insert(SortNoneAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionClearSortingString), this, SLOT(sortNone())));

    m_actions.insert(ZoomInAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionZoomInString), this, SLOT(zoomIn()), QKeySequence::ZoomIn));
    m_actions.insert(ZoomOutAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionZoomOutString), this, SLOT(zoomOut()), QKeySequence::ZoomOut));

    m_actions.insert(ExpandAllAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionExpandAllString), this, SLOT(expandAll())));
    m_actions.insert(CollapseAllAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionCollapseAllString), this, SLOT(collapseAll())));

    m_actions.insert(GroupByColumnAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionGroupByThisColumnString), this, SLOT(groupByColumn())));
    m_actions.insert(GroupByBoxAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionGroupByBoxString), this, SLOT(groupByBox())));

    m_actions.insert(HideColumnAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridRemoveColumnString), this, SLOT(hideColumn())));
    m_actions.insert(FieldChooserAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridFieldChooserString), this, SLOT(fieldChooser())));

    m_actions.insert(FooterAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionFooterString), this, SLOT(showHideFooter())));
    m_actions.insert(GroupFootersAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridGroupFootersString), this, SLOT(showHideGroupFooters())));

    m_actions.insert(AlignLeftAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionLeftString), this, SLOT(alignLeftAct())));
    m_actions.insert(AlignRightAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionRightString), this, SLOT(alignRightAct())));
    m_actions.insert(AlignCenterAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridActionCenterString), this, SLOT(alignCenterAct())));

    m_actions.insert(DeleteRowAction, addActionToWidget(grid(), QIcon(), Grid::tr_compatible(QtnGridRemoveRowString), this, SLOT(slot_removeRow()), QKeySequence::Delete));

    setActionIcons();
}

void GridViewBase::removeActions()
{
    for (QHash<GridActionType, QAction*>::const_iterator it = m_actions.constBegin(); it != m_actions.constEnd(); ++it)
        delete it.value();
    m_actions.clear();
}

void GridViewBase::setActionIcons()
{
    m_actions[SortAscendingAction]->setIcon(grid()->themeManager()->getIcon(GridThemeManager::SortIndicatorASCIcon, QSize(64, 64)));
    m_actions[SortDescendingAction]->setIcon(grid()->themeManager()->getIcon(GridThemeManager::SortIndicatorDESCIcon, QSize(64, 64)));
    m_actions[SortNoneAction]->setIcon(QIcon());
    m_actions[ZoomInAction]->setIcon(grid()->themeManager()->getIcon(GridThemeManager::ZoomInIcon, QSize(64, 64)));
    m_actions[ZoomOutAction]->setIcon(grid()->themeManager()->getIcon(GridThemeManager::ZoomOutIcon, QSize(64, 64)));
    m_actions[DeleteRowAction]->setIcon(grid()->themeManager()->getIcon(GridThemeManager::ClearButtonIcon, QSize(64, 64)));
    m_actions[FilterAction]->setIcon(grid()->themeManager()->getIcon(GridThemeManager::FilterToolButtonIcon, QSize(64, 64)));
    m_actions[FieldChooserAction]->setIcon(grid()->themeManager()->getIcon(GridThemeManager::FieldChooserIcon, QSize(64, 64)));
    m_actions[GroupByBoxAction]->setIcon(grid()->themeManager()->getIcon(GridThemeManager::GroupByBoxIcon, QSize(64, 64)));
    m_actions[GroupByColumnAction]->setIcon(grid()->themeManager()->getIcon(GridThemeManager::GroupByColumnIcon, QSize(64, 64)));
    m_actions[AlignLeftAction]->setIcon(grid()->themeManager()->getIcon(GridThemeManager::AlignLeftIcon, QSize(64, 64)));
    m_actions[AlignRightAction]->setIcon(grid()->themeManager()->getIcon(GridThemeManager::AlignRightIcon, QSize(64, 64)));
    m_actions[AlignCenterAction]->setIcon(grid()->themeManager()->getIcon(GridThemeManager::AlignCenterIcon, QSize(64, 64)));
    for (GridViewBase::GridColumns::const_iterator column_it = m_columnslist.constBegin();
        column_it != m_columnslist.constEnd(); ++column_it)
    {
        GridDataTableColumn* column = qobject_cast<GridDataTableColumn *>(*column_it);
        if (column != Q_NULL)
            column->setActionIcons();
    }
}

int GridViewBase::actualWidth() const
{
    return layout()->columnsRect().width();
}


#ifndef QTN_NOUSE_XML_MODULE
bool GridViewBase::saveLayoutToXML(QXmlStreamWriter* xmlwriter)
{
    xmlwriter->writeStartDocument();
    xmlwriter->writeNamespace("http://www.devmachines.com/qt/qtitan", "Qtitan");
    xmlwriter->writeStartElement("Qtitan:GridStorage");
    xmlwriter->writeAttribute("type", "layout");
    xmlwriter->writeAttribute("version", "1.2");
    saveLayoutToStream(xmlwriter);
    xmlwriter->writeEndElement();
    xmlwriter->writeEndDocument();
    return true;
}

bool GridViewBase::loadLayoutFromXML(QXmlStreamReader* xmlreader)
{
    GridViewAutoUpdater autoUpdater(this);

    if (xmlreader->readNext() != QXmlStreamReader::StartDocument)
        return false;
    if (xmlreader->readNext() != QXmlStreamReader::StartElement)
        return false;
#if 0
    QString s = xmlreader->name().toString();
    s = xmlreader->qualifiedName().toString();
#endif
    if (xmlreader->name() != "GridStorage")
        return false;
    QXmlStreamAttributes attrs = xmlreader->attributes();
    if (attrs.value("type") != "layout" || (attrs.value("version") != "1.1" && attrs.value("version") != "1.2"))
        return false;

    m_layoutLoading = true;
    if (!loadLayoutFromStream(xmlreader))
    {
        m_layoutLoading = false;
        return false;
    }
    m_layoutLoading = false;

    if (xmlreader->readNext() != QXmlStreamReader::EndElement)
        return false;
    if (xmlreader->readNext() != QXmlStreamReader::EndDocument)
        return false;
    return true;
}

bool GridViewBase::saveLayoutToStream(QXmlStreamWriter* xmlwriter)
{
    //At first, sorting the columns by visualIndex for the right restore process.
    QMultiMap<int, GridDataTableColumn *> sorted;
    for (GridColumns::const_iterator it = m_columnVisualIndexies.constBegin(); it != m_columnVisualIndexies.constEnd(); ++it)
    {
        GridDataTableColumn* base = qobject_cast<GridDataTableColumn *>(*it);
        if (base == Q_NULL)
            return false;
        sorted.insert(base->visualIndex(), base);
    }

    xmlwriter->writeStartElement("Qtitan:Columns");
    for (QMultiMap<int, GridDataTableColumn *>::const_iterator it = sorted.constBegin(); it != sorted.constEnd(); ++it)
    {
        xmlwriter->writeStartElement("Qtitan:Column");
        xmlwriter->writeAttribute("id", QString("%1").arg((*it)->dataBinding()->column()));
        saveColumnBaseLayout(*it, xmlwriter);
        saveTableColumnLayout(qobject_cast<const GridTableColumn *>(*it), xmlwriter);
        xmlwriter->writeEndElement();
    }
    xmlwriter->writeEndElement();

    xmlwriter->writeStartElement("Qtitan:Misc");
    xmlwriter->writeStartElement("Qtitan:Options");
    xmlwriter->writeAttribute("groupsHeader", QString("%1").arg(optionsref()->groupsHeader()));
    xmlwriter->writeAttribute("footerSummary", QString("%1").arg(optionsref()->isFooterSummaryVisible()));
    xmlwriter->writeAttribute("groupSummary", QString("%1").arg(optionsref()->groupSummaryPlace()));
    xmlwriter->writeEndElement();
    xmlwriter->writeEndElement();
    return true;
}

bool GridViewBase::loadLayoutFromStream(QXmlStreamReader* xmlreader)
{
    if (xmlreader->readNext() != QXmlStreamReader::StartElement)
        return false;
    if (xmlreader->name() != "Columns")
        return false;
    xmlreader->readNext();
    while (xmlreader->tokenType() == QXmlStreamReader::StartElement)
    {
        QXmlStreamAttributes attrs = xmlreader->attributes();
        if (!attrs.hasAttribute("id"))
            return false;
        int id = attrs.value("id").toString().toInt();
        GridColumnBase* column = getColumnByModelColumn(id);
        if (column == Q_NULL)
            return false;

        if (!loadColumnBaseLayout(column, xmlreader))
            return false;
        if (!loadTableColumnLayout(qobject_cast<GridDataTableColumn *>(column), xmlreader))
            return false;
        if (xmlreader->readNext() != QXmlStreamReader::EndElement)
            return false;
        xmlreader->readNext();
    }
    if (xmlreader->tokenType() != QXmlStreamReader::EndElement)
        return false;

    if (xmlreader->readNext() != QXmlStreamReader::StartElement)
        return false;
    if (xmlreader->name() != "Misc")
        return false;

    if (xmlreader->readNext() != QXmlStreamReader::StartElement)
        return false;
    if (xmlreader->name() != "Options")
        return false;

    QXmlStreamAttributes attrs = xmlreader->attributes();
    if (attrs.hasAttribute("groupsHeader"))
    {
        int groupsHeader = attrs.value("groupsHeader").toString().toInt();
        optionsref()->setGroupsHeader(groupsHeader != 0);
    }

    if (attrs.hasAttribute("footerSummary"))
    {
        int footerSummary = attrs.value("footerSummary").toString().toInt();
        optionsref()->setFooterSummaryVisible(footerSummary != 0);
    }

    if (attrs.hasAttribute("groupSummary"))
    {
        int groupSummary = attrs.value("groupSummary").toString().toInt();
        optionsref()->setGroupSummaryPlace((GridGroupSummaryPlace)groupSummary);
    }

    if (xmlreader->readNext() != QXmlStreamReader::EndElement)
        return false;
    if (xmlreader->readNext() != QXmlStreamReader::EndElement)
        return false;
    requestUpdateLayout();
    return true;
}

bool GridViewBase::saveColumnBaseLayout(const GridColumnBase* column, QXmlStreamWriter* xmlwriter)
{
    Q_ASSERT(column != Q_NULL);
    xmlwriter->writeAttribute("iconApperance", QString("%1").arg(column->iconApperance()));
    xmlwriter->writeAttribute("textAlignment", QString("%1").arg(column->textAlignment()));
    xmlwriter->writeAttribute("textElideMode", QString("%1").arg(column->textElideMode()));
    xmlwriter->writeAttribute("visible", QString("%1").arg(column->isVisible()));
    return true;
}

bool GridViewBase::saveTableColumnLayout(const GridDataTableColumn* column, QXmlStreamWriter* xmlwriter)
{
    Q_ASSERT(column != Q_NULL);
    xmlwriter->writeStartElement("Qtitan:TabularData");
    xmlwriter->writeAttribute("visual", QString("%1").arg(column->visualIndex()));
    xmlwriter->writeAttribute("width", QString("%1").arg(column->width()));
    xmlwriter->writeAttribute("sortOrder", QString("%1").arg(column->sortOrder()));
    xmlwriter->writeAttribute("group", QString("%1").arg(column->groupIndex()));
    xmlwriter->writeEndElement();
    return true;
}

bool GridViewBase::loadColumnBaseLayout(GridColumnBase* column, QXmlStreamReader* xmlreader)
{
    Q_ASSERT(column != Q_NULL);
    QXmlStreamAttributes attrs = xmlreader->attributes();

    if (attrs.hasAttribute("iconApperance"))
    {
        IconApperance apperance = (IconApperance)attrs.value("iconApperance").toString().toInt();
        column->setIconApperance(apperance);
    }
    if (attrs.hasAttribute("textAlignment"))
    {
        Qt::Alignment alignment = (Qt::Alignment)attrs.value("textAlignment").toString().toInt();
        column->setTextAlignment(alignment);
    }
    if (attrs.hasAttribute("textElideMode"))
    {
        Qt::TextElideMode mode = (Qt::TextElideMode)attrs.value("textElideMode").toString().toInt();
        column->setTextElideMode(mode);
    }
    if (attrs.hasAttribute("visible"))
    {
        bool visible = attrs.value("visible").toString().toInt();
        column->setVisible(visible);
    }
    return true;
}

bool GridViewBase::loadTableColumnLayout(GridDataTableColumn* column, QXmlStreamReader* xmlreader)
{
    Q_ASSERT(column != Q_NULL);
    if (xmlreader->readNext() != QXmlStreamReader::StartElement || xmlreader->name() != "TabularData")
        return false;

    QXmlStreamAttributes attrs = xmlreader->attributes();

    attrs = xmlreader->attributes();
    if (attrs.hasAttribute("visual"))
    {
        int visualIndex = attrs.value("visual").toString().toInt();
        if (column->visualIndex() != visualIndex)
            column->setVisualIndex(visualIndex);
    }

    if (attrs.hasAttribute("width"))
    {
        int width = attrs.value("width").toString().toInt();
        if (column->width() != width)
            column->setWidth(width);
    }

    if (attrs.hasAttribute("sortOrder"))
    {
        Qtitan::SortOrder sortOrder = (Qtitan::SortOrder)attrs.value("sortOrder").toString().toInt();
        if (column->sortOrder() != sortOrder)
            column->setSortOrder(sortOrder);
    }

    if (attrs.hasAttribute("group"))
    {
        int groupIndex = attrs.value("group").toString().toInt();
        if (column->groupIndex() != groupIndex)
            column->setGroupIndex(attrs.value("group").toString().toInt());
    }

    if (xmlreader->readNext() != QXmlStreamReader::EndElement)
        return false;
    return true;

}

bool GridViewBase::isLayoutLoading() const
{
    return m_layoutLoading;
}
#endif

bool GridViewBase::isRowShowing(GridRow* row) const
{
    return layout()->rowsLayoutEngine()->isRowVisible(row);
}


class GridRowPublic : public GridRow
{
    friend class Qtitan::GridViewBase;
};

void GridViewBase::scrollToRow(GridRow* row, bool topPosition)
{
    if (row->frozenPlace() != GridRow::FrozenNone)
        return;

    int offsetY = 0;
    GridRow* topRow = row;
    if (!topPosition)
        topRow = modelController()->getGridRow(layout()->calculateNextPageRowIndex(row->rowIndex(), true, &offsetY));

    QScrollBar* rowsScrollBar = layout()->orientation() == Qt::Vertical ? layout()->scrollVer() : layout()->scrollHor();

    if (topRow != Q_NULL)
    {
        if (optionsref()->scrollRowStyle() == ScrollByPixel)
        {
            Q_ASSERT(static_cast<GridRowPublic *>(topRow)->m_scrollPosition != -1);
            if (offsetY < 0)
                rowsScrollBar->setSliderPosition(static_cast<GridRowPublic *>(topRow)->m_scrollPosition - offsetY);
            else
                rowsScrollBar->setSliderPosition(static_cast<GridRowPublic *>(topRow)->m_scrollPosition);
        }
        else
        {
            int rowIndex = topRow->rowIndex();
            if (!topPosition && rowIndex < row->rowIndex() && rowIndex < modelController()->getGridRowCount() - modelController()->getFrozenBottomGridRowCount() - 1)
            {
                int l, c;
                layout()->iterateToNextLine(rowIndex, false, rowIndex, l, c);
            }
            rowsScrollBar->setSliderPosition((rowIndex - modelController()->getFrozenTopGridRowCount()) * kGridNormalScrollStep);
        }
    }
}

void GridViewBase::navigatePageUp(Qt::KeyboardModifiers modifiers)
{
    if (focusedRow() == Q_NULL)
        return;

    int nextRowIndex = layout()->calculateNextPageRowIndex(focusedRow()->rowIndex(), true);
    setFocusedRowIndex(nextRowIndex, modifiers, KeyboardFocus);
}

void GridViewBase::navigatePageDown(Qt::KeyboardModifiers modifiers)
{
    if (focusedRow() == Q_NULL)
        return;

    int nextRowIndex = layout()->calculateNextPageRowIndex(focusedRow()->rowIndex(), false);
    setFocusedRowIndex(nextRowIndex, modifiers, KeyboardFocus);
}


void GridViewBase::navigateTab(Qt::KeyboardModifiers modifiers)
{
    GridRow* row = focusedRow();
    if (row != Q_NULL)
    {
        int index = modelController()->focusedColumnIndex();
        int rowIndex = row->rowIndex();

        if (index == -1 || rowIndex == -1)
            return;

        const GridDataTableColumn* column = Q_NULL;
        if (row->type() == GridRow::DataRow)
        {
            column = static_cast<const GridDataTableColumn *>(getColumn(index));
            column = getNextColumn(column);
        }

        if (column == Q_NULL)
        {
            column = getFirstColumn();
            rowIndex++;
            if (rowIndex >= modelController()->getGridRowCount())
                rowIndex = 0;
        }

        if (column == Q_NULL)
            return;

        setCellFocus(rowIndex, column->index(), modifiers, KeyboardFocus);
    }
}

void GridViewBase::navigateBacktab(Qt::KeyboardModifiers modifiers)
{
    GridRow* row = focusedRow();
    if (row != Q_NULL)
    {
        int index = modelController()->focusedColumnIndex();
        int rowIndex = row->rowIndex();

        if (index == -1 || rowIndex == -1)
            return;
        const GridDataTableColumn* column = Q_NULL;
        if (row->type() == GridRow::DataRow)
        {
            column = static_cast<const GridDataTableColumn *>(getColumn(index));
            column = getPrevColumn(column);
        }

        if (column == Q_NULL)
        {
            column = getLastColumn();
            rowIndex--;
            if (rowIndex < 0)
                rowIndex = modelController()->getGridRowCount() - 1;
        }

        if (column == Q_NULL)
            return;

        setCellFocus(rowIndex, column->index(), modifiers, KeyboardFocus);
    }
}

void GridViewBase::scrollToIndex(const QModelIndex& index, bool topPosition)
{
    GridRow* row = modelController()->getGridRowByModelRow(index.row());
    if (row != Q_NULL)
        scrollToRow(row, topPosition);
}

void GridViewBase::showRowOnScreen(int oldRowIndex, int newRowIndex)
{
    GridRow* row = modelController()->getGridRow(newRowIndex);
    if (row == Q_NULL)
        return;
    if (layout()->rowsLayoutEngine()->isRowVisible(row))
    {
        layout()->rowsPanel()->update();
        return;
    }

    scrollToRow(row, oldRowIndex >= newRowIndex);

    if (layout()->updateTimer()->isActive())
    {
        layout()->updateTimer()->stop();
        layout()->updateRowsScrollTimeout();
    }
}

GridDataTableColumn* GridViewBase::getFirstColumn() const
{
    return getNextColumn(Q_NULL);
}

GridDataTableColumn* GridViewBase::getLastColumn() const
{
    return getPrevColumn(Q_NULL);
}

GridDataTableColumn* GridViewBase::getNextColumn(const GridDataTableColumn* anchor) const
{
    QListIterator<GridColumnBase *> iterator(m_columnVisualIndexies);
    iterator.toFront();
    while (iterator.hasNext())
    {
        GridTableColumn* base = static_cast<GridTableColumn *>(iterator.next());
        if (!isColumnVisible(base))
            continue;
        if (anchor == Q_NULL)
            return base;
        if (anchor == base)
        {
            while (iterator.hasNext())
            {
                base = static_cast<GridTableColumn *>(iterator.next());
                if (!isColumnVisible(base))
                    continue;
                return base;
            }
            return Q_NULL;
        }
    }
    return Q_NULL;
}

GridDataTableColumn* GridViewBase::getPrevColumn(const GridDataTableColumn* anchor) const
{
    QListIterator<GridColumnBase *> iterator(m_columnVisualIndexies);
    iterator.toBack();
    while (iterator.hasPrevious())
    {
        GridTableColumn* base = static_cast<GridTableColumn *>(iterator.previous());
        if (!isColumnVisible(base))
            continue;
        if (anchor == Q_NULL)
            return base;
        if (anchor == base)
        {
            while (iterator.hasPrevious())
            {
                base = static_cast<GridTableColumn *>(iterator.previous());
                if (!isColumnVisible(base))
                    continue;
                return base;
            }
            return Q_NULL;
        }
    }
    return Q_NULL;
}

bool GridViewBase::isColumnVisible(const GridColumnBase* column) const
{
    return column != Q_NULL && column->isVisible() && hasVisibleParent(column);
}

bool GridViewBase::hasVisibleParent(const GridColumnBase* column) const
{
    Q_UNUSED(column);
    return true;
}

/*!
\internal
*/
GridRowsPrimitiveAbstract* GridViewBase::rowsGraphic() const
{
    return layout()->rowsPanel();
}

int GridViewBase::getVisualIndex(const GridColumnBase* columnBase) const
{
    return m_columnVisualIndexies.indexOf((GridColumnBase *)columnBase);
}

void GridViewBase::setVisualIndex(const GridColumnBase* columnBase, int visualIndex)
{
    GridViewAutoUpdater autoUpdater(this);

    int index = m_columnVisualIndexies.indexOf((GridColumnBase *)columnBase);
    Q_ASSERT(index != -1);
    m_columnVisualIndexies.move(index, visualIndex);
    requestUpdateLayout();
}

int GridViewBase::getColumnIndex(const GridColumnBase* columnBase) const
{
    return m_columnslist.indexOf((GridColumnBase *)columnBase);
}

GridColumnBase* GridViewBase::addColumn(const QString& modelColumnName, const QString& caption)
{
    if (modelController()->model() == Q_NULL)
        return Q_NULL;
    for (int i = 0; i < modelController()->model()->columnCount(); ++i)
    {
        if (modelController()->model()->headerData(i, Qt::Horizontal).toString() == modelColumnName)
            return addColumn(i, caption);
    }
    return Q_NULL;
}

GridColumnBase* GridViewBase::addColumn(int modelColumn, const QString& caption)
{
    if (modelController()->model() == Q_NULL ||
        modelColumn < 0 || modelColumn >= modelController()->model()->columnCount())
        return Q_NULL;

    GridViewAutoUpdater autoUpdater(this);
    QString s = caption;
    if (s.isEmpty())
        s = modelController()->model()->headerData(modelColumn, Qt::Horizontal).toString();
    if (s.isEmpty())
        s = QString("Column %1").arg(modelColumn);
    GridColumnBase* retval = createColumn(modelColumn, s);
    m_columnslist.push_back(retval);
    retval->initialize();
    markColumnsChanged();
    return retval;
}

bool GridViewBase::removeColumn(int index)
{
    if (index < 0 || index >= m_columnslist.size())
        return false;
    GridViewAutoUpdater autoUpdater(this);

    GridColumnBase* column = m_columnslist[index];
    //setColumnGroupIndex(column, -1, false, false); //Remove from grouping
    m_columnslist.removeAt(index);
    column->finalize();
    delete column;
    markColumnsChanged();
    return true;
}

void GridViewBase::removeColumns()
{
    GridViewAutoUpdater autoUpdater(this);

    while (getColumnCount() > 0)
        removeColumn(0);
}

int GridViewBase::groupColumnCount() const
{
    return m_groups.size();
}

GridColumnBase* GridViewBase::groupColumnByIndex(int groupIndex) const
{
    if (groupIndex >= 0 && groupIndex < m_groups.size())
        return static_cast<GridColumnBase *>(m_groups.at(groupIndex));
    else return Q_NULL;
}

void GridViewBase::clearGrouping()
{
    while (m_groups.size() > 0)
        static_cast<GridDataTableColumn *>(m_groups[0])->setGroupIndex(-1);
}


GridModelDataBinding* GridViewBase::getDataBinding(const GridColumnBase* columnBase) const
{
    const GridDataTableColumn* column = qobject_cast<const GridDataTableColumn *>(columnBase);
    if (column != Q_NULL)
        return column->dataBinding();
    else
        return Q_NULL;
}

int GridViewBase::getColumnSortIndex(const GridColumnBase* columnBase) const
{
    Q_UNUSED(columnBase);
    return -1;
}

void GridViewBase::setColumnSortIndex(const GridColumnBase* columnBase, int sortIndex)
{
    Q_UNUSED(columnBase);
    Q_UNUSED(sortIndex);
}

SortOrder GridViewBase::getColumnSortOrder(const GridColumnBase* columnBase) const
{
    return modelController()->getSortOrder(getDataBinding(columnBase));
}
 
void GridViewBase::setColumnSortOrder(GridColumnBase* columnBase, SortOrder order, bool caseSensitivity, bool localeAware)
{
    SortOrder oldorder = getColumnSortOrder(columnBase);
    if (oldorder == order)
        return;

    GridViewAutoUpdater autoUpdater(this);

    modelController()->setSortOrder(getDataBinding(columnBase), -1, order, caseSensitivity, localeAware);
}

int GridViewBase::columnGroupIndex(const GridColumnBase* column) const
{
    return m_groups.indexOf((GridColumnBase *)column);
}

void GridViewBase::setColumnGroupIndex(GridColumnBase* column, int groupIndex,
    bool caseSensitivity, bool localeAware)
{
    int oldIndex = columnGroupIndex(column);
    if (oldIndex == groupIndex)
        return;

    ColumnGroupArgs args(column, groupIndex);
    emit startGrouping(&args);
    if (args.isHandled())
        return;

    GridViewAutoUpdater autoUpdater(this);

    if (groupIndex == kGridColumnIndexInvalid && oldIndex != kGridColumnIndexInvalid)
    {
        layout()->groupingPanel()->removeColumnAt(oldIndex);
        m_groups.removeAt(oldIndex);
    }
    else
    {
        if (groupIndex > m_groups.size())
            groupIndex = m_groups.size();
        if (oldIndex != kGridColumnIndexInvalid)
        {
            if (groupIndex >= m_groups.size())
                groupIndex = m_groups.size() - 1;
            m_groups.move(oldIndex, groupIndex);
            layout()->groupingPanel()->moveColumn(oldIndex, groupIndex);
        }
        else if (groupIndex != kGridColumnIndexInvalid)
        {
            layout()->groupingPanel()->insertColumn(groupIndex, column);
            m_groups.insert(groupIndex, column);
        }
    }
    changeGrouping(column, groupIndex, caseSensitivity, localeAware);
}

int GridViewBase::getColumnRowIndex(const GridDataTableColumn* column) const
{
    Q_UNUSED(column);
    return 0;
}

/*!
    \class Qtitan::GridColumnBase
    \inmodule QtitanDataGrid
*/
GridColumnBase::GridColumnBase(GridViewBase* view, const QString& caption)
: m_visible(true), m_view(view),
  m_caption(caption), m_iconApperance(IconAlignLeft), 
  m_textAlignment(Qt::AlignVCenter | Qt::AlignRight),
  m_textElideMode(Qt::ElideRight), m_textOrientation(Qt::Horizontal), m_decorationColor(Qt::transparent), m_columnEditor(Q_NULL)
{
}

GridColumnBase::~GridColumnBase()
{
    for (QList<QAction *>::const_iterator it = m_actionGroup.constBegin(); it != m_actionGroup.constEnd(); ++it)
        delete *it;
    m_actionGroup.clear();
    m_columnEditor = Q_NULL;
}

void GridColumnBase::initialize()
{
    m_view->columnCreated(this);
}

void GridColumnBase::finalize()
{
    m_view->columnDestroying(this);
}

int GridColumnBase::index() const
{ 
    Q_ASSERT(m_view != Q_NULL); 
    return m_view->getColumnIndex(this);
}

void GridColumnBase::setCaption(const QString& caption)
{
    m_caption = caption;
}

const QString& GridColumnBase::htmlCaption() const
{
    return m_htmlCaption;
}

void GridColumnBase::setHtmlCaption(const QString& caption)
{
    if (m_htmlCaption == caption)
        return;
    m_htmlCaption = caption;
    m_view->layout()->columnChanged(this);
}

const QFont& GridColumnBase::font() const
{
    return m_font;
}

void GridColumnBase::setFont(const QFont& font)
{
    m_font = font;
}

const QString& GridColumnBase::caption() const
{ 
    return m_caption;
}

Qt::Alignment GridColumnBase::textAlignment() const
{
    return m_textAlignment;
}

void GridColumnBase::setTextAlignment(Qt::Alignment alignment)
{
    if (m_textAlignment == alignment)
        return;
    m_textAlignment = alignment;
    m_view->textAlignmentChanged(this);
}

Qt::TextElideMode GridColumnBase::textElideMode() const
{
    return m_textElideMode;
}

void GridColumnBase::setTextElideMode(Qt::TextElideMode mode)
{
    m_textElideMode = mode;
}

Qt::Orientation GridColumnBase::textOrientation() const
{
    return m_textOrientation;
}

void GridColumnBase::setTextOrientation(Qt::Orientation orientation)
{
    m_textOrientation = orientation;
}

void GridColumnBase::setIcon(const QIcon& icon)
{
    m_icon = icon;
}

const QIcon& GridColumnBase::icon() const
{
    return m_icon;
}

IconApperance GridColumnBase::iconApperance() const
{
    return m_iconApperance;
}

void GridColumnBase::setIconApperance(IconApperance apperance)
{
    m_iconApperance = apperance;
}

bool GridColumnBase::isVisible() const
{
    Q_ASSERT(m_view != Q_NULL);
    return m_visible;
}

void GridColumnBase::setVisible(bool visible)
{
    Q_ASSERT(m_view != Q_NULL); 
    if (m_visible == visible)
        return;
    m_visible = visible;
    m_view->columnVisibleChanged(this, visible);
}

const QColor& GridColumnBase::decorationColor() const
{
    return m_decorationColor;
}

void GridColumnBase::setDecorationColor(const QColor& color)
{
    m_decorationColor = color;
}

const QPen& GridColumnBase::pen() const
{
    return m_pen;
}

void GridColumnBase::setPen(const QPen& pen)
{
    m_pen = pen;
}

QList<QAction *>& GridColumnBase::actionGroup()
{
    return m_actionGroup;
}

/*!
Returns the column editor installed to the column.
*/
GridTableColumnEditor* GridColumnBase::columnEditor() const
{
    return m_columnEditor;
}

/*!
Sets the column \a editor installed to the column. Column editor allows embed a custom QWidget to any grid column or band for BandedTableView cases.
\sa GridTableColumnEditor
*/
void GridColumnBase::setColumnEditor(GridTableColumnEditor* editor)
{
    if (m_columnEditor == editor)
        return;
    m_columnEditor = editor;
    viewBase()->layout()->columnEditorChanged(this, editor);
}

/* GridFieldChooserColumnProvider */
GridFieldChooserColumnProvider::GridFieldChooserColumnProvider(GridViewBase* view)
    : GridFieldChooserProvider(view)
{
}

QString GridFieldChooserColumnProvider::title() const
{
    return Grid::tr_compatible(QtnGridColumnsString);
}

QString GridFieldChooserColumnProvider::description() const
{
    return Grid::tr_compatible(QtnGridEmptyFieldChooserColumnString);
}

GridViewBase::GridColumns GridFieldChooserColumnProvider::fields() const
{
    return view()->m_columnslist;
}

/* GridFloatToolWidget */
GridFloatToolWidget::GridFloatToolWidget()
: QWidget(Q_NULL, Qt::ToolTip | Qt::FramelessWindowHint)
{
}

/* DragColumnWidget */
DragColumnWidget::DragColumnWidget(GridColumnBase* columnBase, const QSize& size)
: GridFloatToolWidget(), m_columnBase(columnBase)
{
    setGeometry(0, 0, size.width(), size.height());
    setWindowOpacity(0.7);
}

class AutoInPaintWidget
{
public:
    AutoInPaintWidget(QWidget* w)
    {
        m_w = w;
        m_testInPaintEvent = m_w->testAttribute(Qt::WA_WState_InPaintEvent);
        m_w->setAttribute(Qt::WA_WState_InPaintEvent);
    }
    ~AutoInPaintWidget()
    {
        m_w->setAttribute(Qt::WA_WState_InPaintEvent, m_testInPaintEvent);
    }
private:
    QWidget* m_w;
    bool m_testInPaintEvent;
};

void DragColumnWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    Grid* grid = m_columnBase->viewBase()->grid();

    AutoInPaintWidget autoPaintEvent(grid);

    QStyleOptionHeader styleOption;
    m_columnBase->initStyleOption(styleOption);

    styleOption.selectedPosition = QStyleOptionHeader::NotAdjacent;
    styleOption.state &= ~QStyle::State_Sunken;
    styleOption.state &= ~QStyle::State_Raised;
    styleOption.state &= ~QStyle::State_MouseOver;
    styleOption.state |= QStyle::State_Raised;
    styleOption.rect = QRect(0, 0, rect().width(), rect().height());
    styleOption.textAlignment = Qt::AlignRight;
    styleOption.styleObject = grid;

    painter.setFont(m_columnBase->viewBase()->getColumnFont(m_columnBase));
    painter.setPen(m_columnBase->viewBase()->getColumnPen(m_columnBase));

    GridTableView* view = qobject_cast<GridTableView *>(m_columnBase->viewBase());
   
    GridTableColumnPrimitive* columnGP = view != Q_NULL ? 
        view->layout()->columnHeader()->getPrimitiveByColumn(static_cast<GridTableColumnBase *>(m_columnBase)) : Q_NULL;

    GridTableColumnPrimitive::paint_column_header_background(columnGP, &painter, styleOption, m_columnBase->decorationColor());
    GridTableColumnPrimitive::paint_column_header_contents(columnGP, &painter, styleOption, m_columnBase->textElideMode(), m_columnBase->iconApperance(),
        true, 0, 0, m_columnBase->textOrientation(), false, 1.0);
}

/* ColumnArrowWidget */
ColumnArrowWidget::ColumnArrowWidget(const QColor& shadeColor, ColumnMovePosition position)
: GridFloatToolWidget(), m_arrow(QImage(":res/grid_arrow.png")) 
{
    qtn_set_shadecolor(m_arrow, shadeColor.darker(120));

    switch (position)
    {
    case PreviousColumnPosition:
        {
            QMatrix matrix;
            matrix.rotate(90);
            m_arrow = m_arrow.transformed(matrix);
        }
        break;
    case NextColumnPosition:
        {
            QMatrix matrix;
            matrix.rotate(270);
            m_arrow = m_arrow.transformed(matrix);
        }
        break;
    case PreviousRowPosition:
        {
            QMatrix matrix;
            matrix.rotate(180);
            m_arrow = m_arrow.transformed(matrix);
        }
        break;
    default:
        break;
    }
    m_arrow = m_arrow.scaled(QSize(12, 12), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setMask(QPixmap::fromImage(m_arrow.createAlphaMask()));
    setGeometry(0, 0, m_arrow.width(), m_arrow.height());
}

void ColumnArrowWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, m_arrow);
}

/*!
  \class ErrorEventArgs
  \inmodule QtitanDataGrid
*/
ErrorEventArgs::ErrorEventArgs(GridViewBase* view, const QString& error, const QString& description)
    : BaseEventArgs(), m_view(view), m_error(error), m_description(description)
{
}

ErrorEventArgs::~ErrorEventArgs()
{
  m_view = Q_NULL;
}

GridViewBase* ErrorEventArgs::view() const
{
    return m_view;
}

const QString& ErrorEventArgs::error() const
{
    return m_error;
}

const QString& ErrorEventArgs::description() const
{
    return m_description;
}

void ErrorEventArgs::setDescription(const QString& description)
{
    m_description = description;
}

/*!
  \class ContextMenuEventArgs
  \inmodule QtitanDataGrid
*/
ContextMenuEventArgs::ContextMenuEventArgs(GridViewBase* view, QMenu* contextMenu, GridHitInfo* info)
    : BaseEventArgs(), m_view(view), m_contextMenu(contextMenu), m_hitInfo(info)
{
}

ContextMenuEventArgs::~ContextMenuEventArgs()
{
    Q_DELETE_AND_NULL(m_hitInfo);
}

GridViewBase* ContextMenuEventArgs::view() const
{
    return m_view;
}

QMenu* ContextMenuEventArgs::contextMenu() const
{
    return m_contextMenu;
}

GridHitInfo* ContextMenuEventArgs::hitInfo() const
{
    return m_hitInfo;
}

/*!
\class RowRemovingEventArgs
\inmodule QtitanDataGrid
*/
RowRemovingEventArgs::RowRemovingEventArgs(GridRow* row, int count)
    : BaseEventArgs(), m_row(row), m_count(count)
{
}

GridRow* RowRemovingEventArgs::row() const
{
    return m_row;
}

int RowRemovingEventArgs::count() const
{
    return m_count;
}

/*!
\class RowClickEventArgs
\inmodule QtitanDataGrid
*/

RowClickEventArgs::RowClickEventArgs(GridRow* row)
    : BaseEventArgs(), m_row(row)
{
}

GridRow* RowClickEventArgs::row() const
{
    return m_row;
}

/*!
\class CellClickEventArgs
\inmodule QtitanDataGrid
*/
CellClickEventArgs::CellClickEventArgs(GridCell* cell)
    : BaseEventArgs()
{
    m_cell = new GridCell(cell->row(), cell->columnIndex());
}

CellClickEventArgs::~CellClickEventArgs()
{
    Q_DELETE_AND_NULL(m_cell);
}

GridCell* CellClickEventArgs::cell() const
{
    return m_cell;
}

/*!
\class SummaryTextEventArgs
\inmodule QtitanDataGrid
*/
SummaryTextEventArgs::SummaryTextEventArgs(GridModelDataBinding* binding, GridSummaryType type, QVariant value, const QString& text)
    : BaseEventArgs(), m_binding(binding), m_text(text), m_type(type), m_value(value), m_color(Qt::transparent)
{
}

GridModelDataBinding* SummaryTextEventArgs::dataBinding() const
{
    return m_binding;
}

const QString& SummaryTextEventArgs::text() const
{
    return m_text;
}

void SummaryTextEventArgs::setText(const QString& t)
{
    m_text = t;
}

const QColor& SummaryTextEventArgs::textColor() const
{
    return m_color;
}

void SummaryTextEventArgs::setTextColor(const QColor& color)
{
    m_color = color;
}

GridSummaryType SummaryTextEventArgs::type() const
{
    return m_type;
}

const QVariant& SummaryTextEventArgs::value() const
{
    return m_value;
}

/*!
\class SummaryGroupTextEventArgs
\inmodule QtitanDataGrid
*/
SummaryGroupTextEventArgs::SummaryGroupTextEventArgs(GridModelDataBinding* binding,
    GridSummaryType type, QVariant value,
    int modelGroupColumn,
    const QString& text)
    : SummaryTextEventArgs(binding, type, value, text), m_modelGroupColumn(modelGroupColumn)
{
}

int SummaryGroupTextEventArgs::modelGroupColumn() const
{
    return m_modelGroupColumn;
}

/*!
\enum GridScrollStyle
The enumerator describes the scrolling behaviour.
\relates GridViewOptions
\inmodule QtitanDataGrid
\value ScrollByItem vertical scrolling by rows
\value ScrollByItem  vertical scrolling by pixels
*/

/*!
\enum GridNewRowPlace
The enumerator specifies the location of a special line for creating and editing the new row of the grid.
\inmodule QtitanDataGrid
\relates GridViewOptions
\value NewRowNone Line for editing the new row is hidden.
\value NewRowTop Line for editing the new row on the top.
\value NewRowBottom Line for editing the new row on the bottom.
*/

/*!
\enum ScrollBarsMode
The enumerator of the grid scrollbar behavior modes.
\inmodule QtitanDataGrid
\relates GridViewOptions

\value ScrollNone Scrollbars are not shown
\value ScrollBoth Both horizontal and vertical scrollbars are shown
\value ScrollHorizontal The horiontal scrollbar is shown
\value ScrollVertical The vertical scrollbar is shown
\value ScrollAuto The grid automatically controls the scrollbar visibility (depends on the width or height of the grid view).
*/

/*!
\enum GridGroupRowStyle
The enumerator ways of grouping the grid rows.

\inmodule QtitanDataGrid
\relates GridViewOptions
\value GridGroupStandard Standard way of grouping.
\value GridGroupOfficeOutlook The way to emulate the behavior and appearance similar to the grouping in MS-Office Outlook.
*/

/*!
\enum RowHighlightEffect
The enumerator admissible effects highlighting row in a state of inserting into grid.

\inmodule QtitanDataGrid
\relates GridViewOptions
\value RowFlashEffect The row is flashing.
\value RowAlphaEffect Row marked translucency.
*/

/*!
\enum GridLinesStyle
This enumerator specifies the style of the frame between the cells. The frame can be dual, single, or it may not be at all.

\inmodule QtitanDataGrid
\relates GridViewOptions
\value LinesNone grid does not draw lines.
\value LinesBoth grid draws a horizontal and vertical lines.
\value LinesBoth2D grid draws a dual lines horizontally and a single lines vertically for vertical view layout and a dual lines vertically and a single lines horizontally for horizontal view layout.
\value LinesHorizontal grid draws a horizontal lines only.
\value LinesHorizontal2D grid draws a dual lines horizontally only, used in vertical view layout.
\value LinesVertical grid draws a vertical lines only.
\value LinesVertical2D  grid draws a dual lines vertically only, used in horizontal view layout.
*/


/*!
\enum GridGroupSummaryPlace
\inmodule QtitanDataGrid
\relates GridViewOptions
\value GroupSummaryHide Hide group summaries. This value is used by default.
\value GroupSummaryRow Show summaries for group on the grop row direcly.
\value GroupSummaryFooter Show summaries for group on the grop footer.
*/

/*!
\enum GridSelectionPolicy
\inmodule QtitanDataGrid
\relates GridViewOptions
Enum describes policy for selection behaviour
\value SelectIgnore Ignore focus selection. If this policy is used then selection based on change focus (by keyborad and mouse for instance) will be disabled.
You can select rows by GridTableView::selectRange(), GridTableView::selectRowRange(), GridTableView::selectRow(), GridTableView::selectAll() functions only. By defaut this flag is false.
\value SelectSingleRow user can select only one row (by default).
\value SelectSingleCell user can select only one cell.
\value SelectMultiRow user can select several rows at time. Hold shift key to select multiple rows.
\value SelectMultiCell user can select several cells at time. Hold shift key to select multiple cells.
\sa GridTableView::selectRange(), GridTableView::selectRowRange(), GridTableView::selectRow(), GridTableView::selectAll()
*/

/*!
\class GridViewOptions
\inmodule QtitanDataGrid
GridViewOptions class intended for configuring the behavior and appearance of different parts of grid's view.
The settings stored in this class relate to all types of views supported by the grid.
To change the settings related to a specific view of GridTableView, GridBandedTableView or GridCardView use the GridTableViewOptions, GridBandedTableViewOptions or GridCardViewOptions classes respectively.
The GridViewOptions class is used for spawning settings classes of specific views.
*/

/*!
Constructor of GridViewOptions class. The constructor is called automatically when a grid view is created.
*/
GridViewOptions::GridViewOptions(GridViewBase* view)
: QObject(Q_NULL), m_view(view), m_scrollBarsMode(ScrollAuto),
  m_groupsHeader(true), 
  m_scrollRowStyle(ScrollByItem),
  m_autoScroll(true), m_gridLines(LinesBoth2D),
  m_columnHeight(-1), m_columnPen(Qt::black), m_cellWidth(-1), m_cellHeight(-1), m_isSortEnabled(true), m_isGroupSortEnabled(true),
  m_modelDecoration(true), m_rubberBandSelection(true), m_borderWidth(2), m_borderColor(Qt::black), m_borderStyled(true),
  m_transparent(false), m_filterEnabled(true), m_filterPanelHeight(50),
  m_dragEnabled(false), m_dropEnabled(false), m_zoomEnabled(true), m_zoomFactor(1.0), m_zoomIndicatorActive(true), m_windowsAirSupported(false),
  m_cellButtonAutoRaise(false), m_autoCreateColumns(true), m_mainMenuDisabled(false), m_summaryMenuDisabled(false), m_fieldChooserEnabled(true),
  m_findEnabled(true), m_selectionPolicy(SelectSingleRow), m_newRowPlace(NewRowNone), m_newRowText(Grid::tr_compatible(QtnAddNewRowString)),
  m_alternatingRowColors(true), m_focusFrameEnabled(true),
  m_groupRowHeight(-1), m_groupRowStyle(GridGroupStandard),
  m_footerSummaryVisible(false), m_groupSummaryPlace(GroupSummaryHide),
  m_columnHidingEnabled(true), m_columnMovingEnabled(true), m_lastColumnMovingEnabled(false),
  m_isColumnHidingOnGroupingEnabled(true), m_isGroupingEnabled(true), m_fastScrollEffect(false), m_focusFollowsMouse(false),
  m_previewRowEnabled(false), m_previewRowHeight(50), m_previewExpandButtonVisible(true),
  m_newRowHighlightEffect(RowFlashEffect), m_newRowFlashInterval(170),
  m_newRowOpacity(0.7), m_showFocusDecoration(false),
  m_rowAutoHeight(false)
{
    Q_ASSERT(m_view != Q_NULL);
    m_groupsHeaderTextColor = Qt::darkGray;
    m_groupsHeaderText = Grid::tr_compatible(QtnGridHeaderString);
    m_columnFont = QApplication::font("QHeaderView");
    m_cellButtonWidth = -1;
    m_backgroundColor = m_view->grid()->palette().color(m_view->grid()->backgroundRole());
    m_filterPanelHeight = view->grid()->fontMetrics().height() * 2 + 20;
    m_cellPalette = view->grid()->palette();
    m_cellFont = view->grid()->font();
    m_groupRowFont = view->grid()->font();
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
    QStyleOptionViewItemV4 option;
#else
    QStyleOptionViewItem option;
#endif

    option.initFrom(view->grid());
    const int gridHint = view->grid()->style()->styleHint(QStyle::SH_Table_GridLineColor, &option, view->grid());
    m_gridLinePen = QPen(QBrush(static_cast<QRgb>(gridHint)), 1);
}

/*!
Returns the view that owns this options.
*/
GridViewBase* GridViewOptions::view() const
{
    return m_view;
}

/*!
Returns the scrollbar display mode value.
\sa ScrollBarsMode
*/
ScrollBarsMode GridViewOptions::scrollBars() const
{
    return m_scrollBarsMode;
}

/*!
Sets the scroll bars display \a mode. The default value is ScrollAuto.
\sa ScrollBarsMode
*/
void GridViewOptions::setScrollBars(ScrollBarsMode mode)
{
    m_scrollBarsMode = mode;
    switch (m_scrollBarsMode)
    {
    case ScrollNone:
        m_view->layout()->setOverflow(gsoNone);
        break;
    case ScrollBoth:
        m_view->layout()->setOverflow(gsoVisible);
        break;
    case ScrollHorizontal:
        m_view->layout()->setOverflowX(gsoNone);
        m_view->layout()->setOverflowY(gsoVisible);
        break;
    case ScrollVertical:
        m_view->layout()->setOverflowX(gsoVisible);
        m_view->layout()->setOverflowY(gsoNone);
        break;
    case ScrollAuto:
        m_view->layout()->setOverflow(gsoAuto);
        break;
    }
}

/*!
Returns the visibility value of the grid grouping panel.
*/
bool GridViewOptions::groupsHeader() const
{
    return m_groupsHeader;
}

/*!
Sets the visibility status of the grid grouping panel to \a groupsHeader.
*/
void GridViewOptions::setGroupsHeader(bool groupsHeader)
{
    m_groupsHeader = groupsHeader;
    view()->requestUpdateLayout();
    //m_view->headerPanelGraphic()->setGroupPanelVisible(m_groupsHeader); TODO
}

/*!
Returns the grid row scrolling mode value. Two types of scrolling are supported: by row and by pixel.
\sa GridScrollStyle
*/
GridScrollStyle GridViewOptions::scrollRowStyle() const
{
    return m_scrollRowStyle;
}

/*!
Sets the grid row scrolling mode to \a style. Two scrolling modes are supported: by row and by pixel.
\sa GridScrollStyle
*/
void GridViewOptions::setScrollRowStyle(GridScrollStyle style)
{
    if (m_scrollRowStyle != style)
    {
        m_scrollRowStyle = style;
        //m_view->controllerChanged();
        view()->requestUpdateLayout();
    }
}

/*!
Returns true is grid automatically scrolls the view to the target column when an attempt is made to move the grid column by dragging it by the boundary of the visible part.
*/
bool GridViewOptions::autoScroll() const
{
    return m_autoScroll;
}

/*!
Sets the flag \a Scroll that specifies if the grid automatically scrolls the view to the target column when the grid is moved by the boundary of the visible part.
*/
void GridViewOptions::setAutoScroll(bool autoScroll)
{
    m_autoScroll = autoScroll;
}

/*!
Returns the style of the grid cell delimiting lines.
Possible variants:  show only horizontal, only vertical, horizontal and vertical, do not show lines.
\sa GridLinesStyle
*/
GridLinesStyle GridViewOptions::gridLines() const
{
    return m_gridLines;
}

/*!
Sets the style of the grid cell delimiting \a lines.
Possible variants:  show only horizontal, only vertical, horizontal and vertical, do not show lines.
*/
void GridViewOptions::setGridLines(GridLinesStyle lines)
{
    m_gridLines = lines;
    view()->requestUpdateLayout();
}

/*!
Returns the color of the line delimiting grid cells.
*/
QColor GridViewOptions::gridLineColor() const
{
    return m_gridLinePen.color();
}

/*!
Sets the \a color of the line delimiting grid cells.
*/
void GridViewOptions::setGridLineColor(const QColor& color)
{
    m_gridLinePen.setColor(color);
}

/*!
Returns the width of the lines delimiting grid cells.
*/
int GridViewOptions::gridLineWidth() const
{
    return m_gridLinePen.width();
}

/*!
Sets the \a width of the lines delimiting grid cells.
*/
void GridViewOptions::setGridLineWidth(int width)
{
    if (m_gridLinePen.width() == width)
        return;
    m_gridLinePen.setWidth(width);
    view()->requestUpdateLayout();
}

/*!
Returns the value of the pen that used for drawing lines delimiting grid cells.
\sa gridLineBrush
*/
const QPen& GridViewOptions::gridLinePen() const
{
    return m_gridLinePen;
}

/*!
Sets the \a pen used for drawing lines delimiting grid cells.
\sa setGridLineBrush
*/
void GridViewOptions::setGridLinePen(const QPen& pen)
{
    m_gridLinePen = pen;
    view()->requestUpdateLayout();
}

/*!
Returns the value of the brush that used for drawing lines delimiting grid cells.
\sa gridLinePen
*/
QBrush GridViewOptions::gridLineBrush() const
{
    return m_gridLinePen.brush();
}

/*!
Sets the \a brush used for drawing lines delimiting grid cells.
\sa setGridLinePen
*/
void GridViewOptions::setGridLineBrush(const QBrush& brush)
{
    m_gridLinePen.setBrush(brush);
    view()->requestUpdateLayout();
}

/*!
Returns font that used for drawing the text in the grid columns.
*/
const QFont& GridViewOptions::columnFont() const
{
    return m_columnFont;
}

/*!
Sets the \a font that used for drawing text in grid columns.
*/
void GridViewOptions::setColumnFont(const QFont& font)
{
    m_columnFont = font;
    view()->requestUpdateLayout();
}

/*!
Returns the default value of the grid height. If the returned value is -1, the height will be calculated automatically and will depend on the size of the column font used for displaying text information.
*/
int GridViewOptions::columnHeight() const
{
    return m_columnHeight;
}


/*!
Sets the default height of grid columns. If the passed value is -1, the height will be automatically calculated based on the column font used for displaying text information.
*/
void GridViewOptions::setColumnHeight(int height)
{
    m_columnHeight = height;
    view()->requestUpdateLayout();
}

/*!
Returns pen that is used for displaying text in grid columns.
*/
const QPen& GridViewOptions::columnPen() const
{
    if (m_columnPen != QPen())
        return m_columnPen;
    return view()->grid()->themeManager()->shadePen();
}


/*!
Sets the \a pen that is used for displaying text in grid columns.
*/
void GridViewOptions::setColumnPen(const QPen& pen)
{
    m_columnPen = pen;
}

/*!
Returns the height of the grid cell in pixels. By default the cellFont() value is used to calculate the cell height.
*/
int GridViewOptions::cellHeight() const
{
    return m_cellHeight;
}

/*!
Returns the width of the grid cell in pixels. By default the cellFont() value is used to calculate the cell width.
*/
int GridViewOptions::cellWidth() const
{
    return m_cellWidth;
}

/*!
Sets the exact width for the grid cell in pixels. If this function is called then the font width for cell is ignored.
*/
void GridViewOptions::setCellWidth(int width)
{
    m_cellWidth = width;
    view()->requestUpdateLayout();
}

/*!
Sets the exact height for the grid cell in pixels. If this function is called then the font height for cell is ignored.
*/
void GridViewOptions::setCellHeight(int height)
{
    m_cellHeight = height;
    view()->requestUpdateLayout();
}

bool GridViewOptions::autoCreateColumns() const
{
    return m_autoCreateColumns;
}

void GridViewOptions::setAutoCreateColumns(bool autoCreate)
{
    m_autoCreateColumns = autoCreate;
}

/*!
Returns the value of the palette that is used for rendering the grid cell. 
*/
const QPalette& GridViewOptions::cellPalette() const
{
    return m_cellPalette;
}

/*!
Sets the value of the palette that is used for rendering the grid cell. By default the grid's widget palette is used.
*/
void GridViewOptions::setCellPalette(const QPalette& palette)
{
    m_cellPalette = palette;
}

/*!
Returns the font that will be used for rendering text information in the grid cell.
*/
const QFont& GridViewOptions::cellFont() const
{
    return m_cellFont;
}

/*!
Sets the \a font to be used for rendering text in the grid cell. Font will be used for all cells of the grid view. 
If the widget is used to display the cell content, or the cell is in edit mode then this font will be applied to the embed widget.
*/
void GridViewOptions::setCellFont(const QFont& font)
{
    m_cellFont = font;
    view()->requestUpdateLayout();
}

/*!
Returns the font that will be used for rendering text information in the grid group row.
*/
const QFont& GridViewOptions::groupRowFont() const
{
    return m_groupRowFont;
}

/*!
Sets the \a font to be used for rendering text in the grid group row. Font will be used for all group rows of the grid view.
*/
void GridViewOptions::setGroupRowFont(const QFont& font)
{
    m_groupRowFont = font;
    view()->requestUpdateLayout();
}

/*!
Returns the status of the flag allowing the user to sort by a column.
*/
bool GridViewOptions::isSortEnabled() const
{
    return m_isSortEnabled;
}

/*!
Sets the status of the flag to \a enabled allowing the user to sort by a column.
*/
void GridViewOptions::setSortEnabled(bool enabled)
{
    m_isSortEnabled = enabled;
    if (!m_isSortEnabled)
        m_view->modelController()->clearSorting();
}

/*!
Returns the status of the flag enabling the user to sort by a column taking part in grouping.
*/
bool GridViewOptions::isGroupSortEnabled() const
{
    return m_isGroupSortEnabled;
}

/*!
Sets the status of the flag to \a enabled allowing the user to sort by a column taking part in grouping.
*/
void GridViewOptions::setGroupSortEnabled(bool enabled)
{
    m_isGroupSortEnabled = enabled;
    if (!m_isGroupSortEnabled)
        m_view->modelController()->clearGrouping();
}

/*!
Returns the status of the flag allowing the use of decoration settings of the grid cells obtained from corresponding model roles, such as Qt::FontRole, Qt::DecorationRole, Qt::BackgroundRole and others.
The default value is "true".
*/
bool GridViewOptions::modelDecoration() const
{
    return m_modelDecoration;
}

/*!
Sets the status to \a enabled of the flag that allows the use of decoration settings of the grid cells obtained from corresponding model roles, such as Qt::FontRole, Qt::DecorationRole, Qt::BackgroundRole and others.
*/
void GridViewOptions::setModelDecoration(bool enabled)
{
    m_modelDecoration = enabled;
    view()->requestUpdateLayout();
}

/*!
Returns the status of the flag allowing the use of RubberBand for cell or row multi-selection.
The default value is "true".
*/
bool GridViewOptions::rubberBandSelection() const
{
    return m_rubberBandSelection;
}

/*!
Sets the status to \a enabled of the flag allowing the use of RubberBand for cell or row multi-selection.
*/
void GridViewOptions::setRubberBandSelection(bool enabled)
{
    m_rubberBandSelection = enabled;
}

/*!
Returns the grid view border width (in pixels). The default value is 1px. The border color is set by the borderColor() function.
*/
int GridViewOptions::borderWidth() const
{
    return m_borderWidth;
}

/*!
Sets the grid view border \a width in pixels. The border color is set by the setBorderColor() function.
*/
void GridViewOptions::setBorderWidth(int width)
{
    m_borderWidth = width;
    m_borderStyled = false;
    m_view->updateBorder();
}

/*!
Returns the color of the border around the grid view. The default value is Qt::black.
*/
const QColor& GridViewOptions::borderColor() const
{
    return m_borderColor;
}

/*!
Sets the \a color of the border around the grid view.
*/
void GridViewOptions::setBorderColor(const QColor& color)
{
    m_borderColor = color;
    m_view->updateBorder();
}

/*!
Returns true if grid should use QStyle to render the grid view border.
*/
bool GridViewOptions::borderStyled() const
{
    return m_borderStyled;
}

/*!
Sets the \a styled flag that specifies if the grid should use QStyle to render the grid view border. By default it is true. If the value is false then borderColor() is used to render the border.
*/
void GridViewOptions::setBorderStyled(bool styled)
{
    m_borderStyled = styled;
    m_view->updateBorder();
}

/*!
Returs the brush which is used for rendering of the group header panel's background.
*/
const QBrush& GridViewOptions::groupsHeaderBrush() const
{
    return m_groupsHeaderBrush;
}

/*!
Sets the \a brush which will be used for rendering of the group header panel's background.
*/
void GridViewOptions::setGroupsHeaderBrush(const QBrush& brush)
{
    m_groupsHeaderBrush = brush;
}

/*!
Returs the color which is used for filling the group header panel's background.
*/
const QColor& GridViewOptions::groupsHeaderColor() const
{
    return m_groupsHeaderBrush.color();
}

/*!
Sets the \a color which will be used for filling the group header panel's background.
*/
void GridViewOptions::setGroupsHeaderColor(const QColor& color)
{
    m_groupsHeaderBrush = QBrush(color, Qt::SolidPattern);
}

/*!
Returns the color of the text which is specified by groupsHeaderText() property.
*/
const QColor& GridViewOptions::groupsHeaderTextColor() const
{
    return m_groupsHeaderTextColor;
}

/*!
Sets the color of the text which is specified by groupsHeaderText() property.
*/
void GridViewOptions::setGroupsHeaderTextColor(const QColor& color)
{
    m_groupsHeaderTextColor = color;
    m_view->layout()->findPanel()->updateOptions();
}

/*!
Returns the value of the color for the background of the grid view.
*/
const QColor& GridViewOptions::backgroundColor() const
{
    const_cast<GridViewOptions *>(this)->m_backgroundColor = m_view->grid()->palette().color(m_view->grid()->backgroundRole());
    return m_backgroundColor;
}

/*!
Sets the value of the \a color for the background of the grid view.
*/
void GridViewOptions::setBackgroundColor(const QColor& color)
{
    m_backgroundColor = color;
}

/*!
Returns the value of the background transparency for the view. If true, the background is transparent. By default the value is false.
*/
bool GridViewOptions::isTransparentBackground() const
{
    return m_transparent;
}

/*!
Sets a \a transparent background for the grid view.
*/
void GridViewOptions::setTransparentBackground(bool transparent)
{
    if (m_transparent == transparent)
        return;
    m_transparent = transparent;
    m_view->requestUpdateLayout();
}

/*!
Returns a value that indicates whether or not to use any filtering mechanisms for the grid view.
*/
bool GridViewOptions::isFilterEnabled() const
{
    return m_filterEnabled;
}

/*!
Depending on \a enabled flag determines whether or not to use any filtering mechanisms for the grid view.
*/
void GridViewOptions::setFilterEnabled(bool enabled)
{
    if (m_filterEnabled == enabled)
        return;
    m_filterEnabled = enabled;
    m_view->requestUpdateLayout();
}

/*!
Returns the value that indicates whether the filter panel can automatically hide or not.
*/
bool GridViewOptions::filterAutoHide() const
{
    return m_view->layout()->isFooterAutoHide();
}

/*!
Depending on \a autoHide flag determines whether the filter panel is able to automatically hiding if the user leads away the mouse from the panel.
*/
void GridViewOptions::setFilterAutoHide(bool autoHide)
{
    m_view->layout()->setFooterAutoHide(autoHide);
    m_view->requestUpdateLayout();
}

/*!
Returns the height of filter panel in pixels.
*/
int GridViewOptions::filterPanelHeight() const
{
    return m_filterPanelHeight;
}

/*!
Sets the \a height of filter panel in pixels.
*/
void GridViewOptions::setFilterPanelHeight(int height)
{
    if (m_filterPanelHeight == height)
        return;
    m_filterPanelHeight = height;
    m_view->requestUpdateLayout();
}

/*!
Returns true if grid's view can act as a drag source.
*/
bool GridViewOptions::dragEnabled() const
{
    return m_dragEnabled;
}

/*!
Sets the grid's view as a source of the dragging to \a enabled.
*/
void GridViewOptions::setDragEnabled(bool enabled)
{
    m_dragEnabled = enabled;
}

/*!
Returns true if grid's view can act as a drag target.
*/
bool GridViewOptions::dropEnabled() const
{
    return m_dropEnabled;
}

/*!
Sets the grid's view as a target of the dropping to \a enabled.
*/
void GridViewOptions::setDropEnabled(bool enabled)
{
    m_dropEnabled = enabled;
    view()->grid()->setAcceptDrops(m_dropEnabled);
}

/*!
Returns the value that shows the availability of zooming mechanism. By default the value is true.
*/
bool GridViewOptions::zoomEnabled() const
{
    return m_zoomEnabled;
}

/*!
Enables or disables the zooming mechanism depending on flag \a enabled.
*/
void GridViewOptions::setZoomEnabled(bool enabled)
{
    if (m_zoomEnabled == enabled)
        return;
    m_zoomEnabled = enabled;
    view()->requestUpdateLayout(zoomFactor());
}

/*!
Returns the current value of the zoom factor. Value of 1.0 means that elements of the grid view have the size to 100%. Value of 0.5 means 50%, 1.5 - 150% and so on. The default value is 1.0.
*/
double GridViewOptions::zoomFactor() const
{
    if (!m_zoomEnabled)
        return 1;
    return m_zoomFactor;
}

/*!
Sets the current value of the zoom \a factor.
*/
void GridViewOptions::setZoomFactor(double factor)
{
    if (m_zoomFactor == factor)
        return;
    m_zoomFactor = factor;
    view()->requestUpdateLayout(factor);
    view()->zoomFactorChanged(factor);
    view()->zoomChanged(qRound(factor * 100));
}

/*!
Returns the flag of usage for the special blinking zoom indicator. Indicator momentarily highlights the current value of the zooming factor in percentage.
*/
bool GridViewOptions::isZoomIndicatorActive() const
{
    return m_zoomIndicatorActive;
}

/*!
Sets the flag of usage for the special blinking indicator of zooming to \a active. By default it is true.
*/
void GridViewOptions::setZoomIndicatorActive(bool active)
{
    m_zoomIndicatorActive = active;
}

bool GridViewOptions::isWindowsAirSupported() const
{
    return m_windowsAirSupported;
}

void GridViewOptions::setWindowsAirSupported(bool supported)
{
    m_windowsAirSupported = supported;
    if (supported /*&& isAirAvailable()*/)
        m_groupsHeaderBrush = QBrush();
    else if (!view()->grid()->style()->inherits("Qtitan::OfficeStyle"))
    {
        m_groupsHeaderBrush = qtn_create_background(Qt::transparent);
    }

    view()->layout()->header()->setAirSupported(supported);
    view()->layout()->footer()->setAirSupported(supported);
    view()->layout()->update();
}

bool GridViewOptions::cellButtonAutoRaise() const
{
    return m_cellButtonAutoRaise;
}

void GridViewOptions::setCellButtonAutoRaise(bool b)
{
    m_cellButtonAutoRaise = b;
    view()->layout()->update();
}

int GridViewOptions::cellButtonWidth() const
{
    return m_cellButtonWidth;
}

void GridViewOptions::setCellButtonWidth(int width)
{
    m_cellButtonWidth = width;
    view()->layout()->resizeEvent(view()->layout()->rect());
}

/*!
Returns the text to be displayed on the group header panel if the grouping is not fulfilled.
*/
const QString& GridViewOptions::groupsHeaderText() const
{
    return m_groupsHeaderText;
}

/*!
Sets the \a text to be displayed on the group header panel if the grouping is not fulfilled.
*/
void GridViewOptions::setGroupsHeaderText(const QString& text)
{
    m_groupsHeaderText = text;
}

/*!
Returns the orientation for the grid view. 
*/
Qt::Orientation GridViewOptions::viewOrientation() const
{
    return view()->layout()->orientation();
}

/*!
Returns the \a orientation for the grid view. Can be specified horizontal or vertical orientation. For the vertical orientation the scrolling of rows is carried vertically (by default). 
In the horizontal orientation the scrolling of rows is carried horizontaly and columns are placed one above the other. viewOrientation property supported by TableView, BandedTableView and CardView.
*/
void GridViewOptions::setViewOrientation(Qt::Orientation orientation)
{
    view()->layout()->setOrientation(orientation);
}

/*!
Returns the accessibility of the grid main context menu which popup on the right mouse click.
*/
bool GridViewOptions::isMainMenuDisabled() const
{
    return m_mainMenuDisabled;
}

/*!
Sets the accessibility of the grid main context menu which popup on the right mouse click to \a disabled.
*/
void GridViewOptions::setMainMenuDisabled(bool disabled)
{
    m_mainMenuDisabled = disabled;
}

/*!
Returns the accessibility of the grid summary context menu which popup on the right mouse click on the summary group or footer panel.
*/
bool GridViewOptions::isSummaryMenuDisabled() const
{
    return m_summaryMenuDisabled;
}

/*!
Sets the accessibility of the grid summary context menu which popup on the right mouse click on the summary group or footer panel to \a disabled value.
*/
void GridViewOptions::setSummaryMenuDisabled(bool disabled)
{
    m_summaryMenuDisabled = disabled;
}

/*!
Returns true if field chooser button is visible, otherwise returns false.
*/
bool GridViewOptions::isFieldChooserEnabled() const
{
    return m_fieldChooserEnabled;
}

/*!
Enables a field chooser. If \a enable is false then the field chooser button will be hided.
*/
void GridViewOptions::setFieldChooserEnabled(bool enabled)
{
    if (m_fieldChooserEnabled == enabled)
        return;
    m_fieldChooserEnabled = enabled;
    view()->requestUpdateLayout();
}

/*!
Returns true if a find feature (find panel, find shortcurs and etc.) is enabled. By default it is true.
*/
bool GridViewOptions::isFindEnabled() const
{
    return m_findEnabled;
}
/*!
Enables or disables a find feature (find panel, find shortcurs and etc.) that allows to find a text in the grid cells. Also it's enables or disables all shortcuts related to the find feature.
*/
void GridViewOptions::setFindEnabled(bool enabled)
{
    if (m_findEnabled == enabled)
        return;

    m_findEnabled = enabled;
    view()->m_actions[FindAction]->setVisible(enabled);
    view()->m_actions[FindNextAction]->setVisible(enabled);
    view()->m_actions[FindPreviousAction]->setVisible(enabled);

    if (!enabled)
        view()->hideFindPanel(true);
}

/*!
Returns a list of column names in which text will be finding.
*/
const QStringList& GridViewOptions::findColumnList() const
{
    return m_findColumnList;
}

/*!
Sets a list of column names in which text will be finding. If this list is empty then search will be executed for all columns. By default it is empty.
*/
void GridViewOptions::setFindColumnList(const QStringList& list)
{
    m_findColumnList = list;
}

/*!
Returns the selection policy used in the grid view.
*/
GridSelectionPolicy GridViewOptions::selectionPolicy() const
{
    return m_selectionPolicy;
}

/*!
Sets the selection \a policy used in the grid view. If ignoreFocusSelection() is true then selectionPolicy() does not has any effects.
\sa GridSelectionPolicy
*/
void GridViewOptions::setSelectionPolicy(GridSelectionPolicy policy)
{
    m_selectionPolicy = policy;
}

/*!
\internal
*/
bool GridViewOptions::isMultiSelect() const
{
    return selectionPolicy() == SelectMultiRow || selectionPolicy() == SelectMultiCell;
}

/*!
Returns the location of a special line for creating and editing the new row of the grid.
\sa GridNewRowPlace
*/
GridNewRowPlace GridViewOptions::newRowPlace() const
{
    return m_newRowPlace;
}

/*!
Sets the location to \a place of a special line for creating and editing the new row of the grid.
\sa GridNewRowPlace
*/
void GridViewOptions::setNewRowPlace(GridNewRowPlace place)
{
    if (m_newRowPlace == place)
        return;
    m_newRowPlace = place;
    view()->requestUpdateLayout();
}

/*!
Returns the text that is typed on a special line for creating and editing the new row of the grid.
*/
const QString& GridViewOptions::newRowText() const
{
    return m_newRowText;
}

/*!
Sets the text that is typed on a special line for creating and editing the new row of the grid.
*/
void GridViewOptions::setNewRowText(const QString& text)
{
    m_newRowText = text;
}

/*!
Returns the value of the flag that defines whether alternating colors should be used for drawing grid cells.
If the returned value is true, the item background will be drawn using QPalette::Base and QPalette::AlternateBase;
otherwise the background will be drawn using the QPalette::Base color. By default, this property is true.
*/
bool GridViewOptions::alternatingRowColors() const
{
    return m_alternatingRowColors;
}

/*!
Sets the flag indicating that alternating colors should be used for drawing grid cells.
If the set value is "true", the item background will be drawn using QPalette::Base and QPalette::AlternateBase;
otherwise the background will be drawn using the QPalette::Base color. By default, this property is set to "false".
*/
void GridViewOptions::setAlternatingRowColors(bool alternating)
{
    m_alternatingRowColors = alternating;
}

/*!
Returns true if the doted frame around the cell that has focus is drawn in at the moment.
*/
bool GridViewOptions::focusFrameEnabled() const
{
    return m_focusFrameEnabled;
}

/*!
Sets the painting the doted frame around the cell with focus to the \a enabled. By default frame is enabled.
*/
void GridViewOptions::setFocusFrameEnabled(bool enabled)
{
    if (m_focusFrameEnabled == enabled)
        return;
    m_focusFrameEnabled = enabled;
    view()->layout()->update();
}

/*!
Returns the height of group rows. If the returned value is -1, the height of the row will depend on the settings of the font used in the cells of the row. The default value is -1.
*/
int GridViewOptions::groupRowHeight() const
{
    return m_groupRowHeight;
}

/*!
Sets the height of group rows. If the returned value is -1, the height of the row will depend on the settings of the font used in the cells of the row.
*/
void GridViewOptions::setGroupRowHeight(int height)
{
    if (m_groupRowHeight == height)
        return;
    m_groupRowHeight = height;
    view()->requestUpdateLayout();
}

/*!
Returns the style of grouping for the grid rows.
\sa GridGroupRowStyle
*/
GridGroupRowStyle GridViewOptions::groupRowStyle() const
{
    return m_groupRowStyle;
}

/*!
Sets the style of grouping for the grid rows to \a rowStyle.
\sa GridGroupRowStyle
*/
void GridViewOptions::setGroupRowStyle(GridGroupRowStyle rowStyle)
{
    if (m_groupRowStyle == rowStyle)
        return;
    m_groupRowStyle = rowStyle;
    if (m_groupRowStyle == GridGroupOfficeOutlook)
    {
        //TODO
        m_gridLines = LinesHorizontal;
        setAlternatingRowColors(false);
        //setColumnsQuickCustomization(false);
        //setFrozenPlaceQuickSelection(false);
        //setRowFrozenButtonVisible(false);
        setBackgroundColor(Qt::white);
        setTransparentBackground(false);
    }
    else
    {
        setAlternatingRowColors(true);
        //setColumnsQuickCustomization(true);
        //setFrozenPlaceQuickSelection(true);
        //setRowFrozenButtonVisible(true);
        setTransparentBackground(true);
    }

    view()->requestUpdateLayout();
}

/*!
Returns the palette that used for displaying the visual elements of a row representing a group for the \a depth.
For instance, margins, background, font color. By default, the grid widget palette is returned.
*/
QPalette GridViewOptions::groupPalette(int depth) const
{
    QPalette p = view()->grid()->themeManager()->createGroupPalette(depth);
    QHash<int, QPalette>::const_iterator it = m_groupPalette.constFind(depth);
    if (it != m_groupPalette.end())
        return it.value().resolve(p);
    return p;
}


/*!
Sets the \a palette used for displaying the visual elements of a row representing a group for the \a depth.
For instance, margins, background, font color.
*/
void GridViewOptions::setGroupPalette(int depth, const QPalette& palette)
{
    m_groupPalette[depth] = palette;
}

/*!
Returns the visibility of the panel to display the summaries at the footer of the grid view.
*/
bool GridViewOptions::isFooterSummaryVisible() const
{
    return m_footerSummaryVisible;
}

/*!
Sets the visibility to \a visible of the panel to display the summaries at the footer of the grid view.
*/
void GridViewOptions::setFooterSummaryVisible(bool visible)
{
    if (m_footerSummaryVisible == visible)
        return;
    m_footerSummaryVisible = visible;
    view()->requestUpdateLayout();
}

/*!
Returns the place of a special panel to display the summaries for the group rows.
\sa GridGroupSummaryPlace
*/
GridGroupSummaryPlace GridViewOptions::groupSummaryPlace() const
{
    return m_groupSummaryPlace;
}

/*!
Sets the \a place of a special panel to display the summaries. By default summaries for group rows are hidden.
\sa GridGroupSummaryPlace
*/
void GridViewOptions::setGroupSummaryPlace(GridGroupSummaryPlace place)
{
    if (m_groupSummaryPlace == place)
        return;
    m_groupSummaryPlace = place;
    view()->layout()->rowsLayoutEngine()->updateSummaryLayoutParent();
    view()->requestUpdateLayout();
}

/*!
Returns "true" if the user can hide columns by dragging them outside the grid view. Returns "true" by default.
*/
bool GridViewOptions::isColumnHidingEnabled() const
{
    return m_columnHidingEnabled;
}

/*!
\brief Enables or disables the user's ability to hide columns by dragging them outside of the grid view depending on \a enabled flag.
*/
void GridViewOptions::setColumnHidingEnabled(bool enabled)
{
    m_columnHidingEnabled = enabled;
}

/*!
Returns "true" if the user can change the position of columns by dragging them. This flag is applied to all columns at once.
*/
bool GridViewOptions::isColumnMovingEnabled() const
{
    return m_columnMovingEnabled;
}

/*!
Enables or disables the user's ability to change column positions by dragging. This \a enabled flag is applied to all columns at once. 
If you need to prohibit dragging for one column only, you need to change the corresponding property of this specific column - GridTableColumn::setMovingEnabled() - and leave the global flag set to "true".
*/
void GridViewOptions::setColumnMovingEnabled(bool enabled)
{
    m_columnMovingEnabled = enabled;
}

bool GridViewOptions::isLastColumnMovingEnabled() const
{
    return m_lastColumnMovingEnabled;
}

void GridViewOptions::setLastColumnMovingEnabled(bool enabled)
{
    m_lastColumnMovingEnabled = enabled;
}

/*!
Returns the status of the grid column hiding function after the column was dragged onto the group panel and grouping was performed.
*/
bool GridViewOptions::isColumnHidingOnGroupingEnabled() const
{
    return m_isColumnHidingOnGroupingEnabled;
}

/*!
Enables or disables the grid column hiding function depending on \a enabled flag after the column was dragged onto the group panel and grouping was performed.
*/
void GridViewOptions::setColumnHidingOnGroupingEnabled(bool enabled)
{
    m_isColumnHidingOnGroupingEnabled = enabled;
}

/*!
Returns "true" if the user can group rows by a column by dragging the column onto the group panel. This flag is applied to all columns at once.
*/
bool GridViewOptions::isGroupingEnabled() const
{
    return m_isGroupingEnabled;
}

/*!
Enables or disables the user's ability to group rows by a column by dragging the column onto the group panel. This flag \a enabled is applied to all columns at once.
If you need to prohibit grouping for one column only, you need to change the corresponding property of this specific column - GridTableColumn::setGroupingEnabled(), and leave the global flag set to "true".
*/
void GridViewOptions::setGroupingEnabled(bool enabled)
{
    m_isGroupingEnabled = enabled;
    if (!m_isGroupingEnabled)
        view()->modelController()->clearGrouping();
}


bool GridViewOptions::fastScrollEffect() const
{
    return m_fastScrollEffect;
}

void GridViewOptions::setFastScrollEffect(bool value)
{
    m_fastScrollEffect = value;
}

/*!
Returns true if the cell's focus follow the mouse moves over the grid view.
*/
bool GridViewOptions::isFocusFollowsMouse() const
{
    return m_focusFollowsMouse;
}

/*!
Sets a flag \a enabled that the focus of cells or rows of the grid will follow the mouse moves over the grid.
*/
void GridViewOptions::setFocusFollowsMouse(bool enabled)
{
    m_focusFollowsMouse = enabled;
}


bool GridViewOptions::isPreviewRowEnabled() const
{
    return m_previewRowEnabled;
}

void GridViewOptions::setPreviewRowEnabled(bool enabled)
{
    if (m_previewRowEnabled == enabled)
        return;
    m_previewRowEnabled = enabled;
    view()->requestUpdateLayout();
}

int GridViewOptions::previewRowHeight() const
{
    return m_previewRowHeight;
}

void GridViewOptions::setPreviewRowHeight(int height)
{
    if (m_previewRowHeight == height)
        return;
    m_previewRowHeight = height;
    view()->requestUpdateLayout();
}

bool GridViewOptions::isPreviewExpandButtonVisible() const
{
    return m_previewExpandButtonVisible;
}

void GridViewOptions::setPreviewExpandButtonVisible(bool visible)
{
    if (m_previewExpandButtonVisible == visible)
        return;
    m_previewExpandButtonVisible = visible;
    view()->requestUpdateLayout();
}

/*!
Returns the effect of highlighting for the grid row while the grid view in "New Row Editing" mode.
\sa newRowOpacity(), newRowFlashInterval()
*/
RowHighlightEffect GridViewOptions::newRowHighlightEffect() const
{
    return m_newRowHighlightEffect;
}

/*!
Sets the \a effect of highlighting for the gird row while the grid view in "New Row Editing" mode.
\sa setNewRowOpacity(), setNewRowFlashInterval()
*/
void GridViewOptions::setNewRowHighlightEffect(RowHighlightEffect effect)
{
    m_newRowHighlightEffect = effect;
}

/*!
Returns interval of flashing for the grid row while the grid view in "New Row Editing" mode.
\sa newRowHighlightEffect(), newRowOpacity()
*/
int GridViewOptions::newRowFlashInterval() const
{
    return m_newRowFlashInterval;
}

/*!
Sets \a interval of flashing for the grid row while the grid view in "New Row Editing" mode.
\sa setNewRowHighlightEffect(), setNewRowOpacity()
*/
void GridViewOptions::setNewRowFlashInterval(int interval)
{
    m_newRowFlashInterval = interval;
}

/*!
Returns the value of opacity for the grid row  while the grid view in "New Row Editing" mode.
\sa newRowHighlightEffect(), newRowFlashInterval()
*/
double GridViewOptions::newRowOpacity() const
{
    return m_newRowOpacity;
}

/*!
Sets the value of \a opacity for the grid row while the grid view in "New Row Editing" mode.
\sa setNewRowHighlightEffect(), setNewRowFlashInterval()
*/
void GridViewOptions::setNewRowOpacity(double opcity)
{
    m_newRowOpacity = opcity;
}

/*!
Returns true if focus decoration adopted for the current operating system or for the current style.
*/
bool GridViewOptions::showFocusDecoration() const
{
    return m_showFocusDecoration;
}

/*!
Sets indication value of how to highlight the row that has focus. If \a value is true then the QStyle prmitive will be used for painting background of the focused row.
Otherwise, the grid will use the built-in bitmap. By default this value is false.
*/
void GridViewOptions::setShowFocusDecoration(bool value)
{
    m_showFocusDecoration = value;
}

/*!
Returns the state of row auto height feature. By default it is false.
*/
bool GridViewOptions::rowAutoHeight() const
{
    return m_rowAutoHeight;
}

/*!
Sets the state of row auto height feature to \a autoHeight. Row auto height it is feature that allows to adjust height of the row to its contents.
*/
void GridViewOptions::setRowAutoHeight(bool autoHeight)
{
    if (m_rowAutoHeight == autoHeight)
        return;
    m_rowAutoHeight = autoHeight;
    view()->requestUpdateLayout();
}

int Qtitan::get_column_width_by_font(const QFont& font, const QString& text)
{
    QFontMetrics metrics(font);
    return metrics.width(text) + 30;
}

QSize Qtitan::get_column_sizeHint(const GridColumnBase* column, QStyle* style, const QFont& font, double scaleFactor, const QString& text, QTextDocument* textDocument)
{
    QSize size;
    QString newText = (column != Q_NULL) && (column->iconApperance() == IconOnly) ? "" : text;
    QIcon icon = (column != Q_NULL) && (column->iconApperance() != IconInvisible) ? column->icon() : QIcon();

    QFont f = qtn_font_apply_scale(font, scaleFactor);
    int margin = qRound(style->pixelMetric(QStyle::PM_HeaderMargin) * scaleFactor);
    int iconSize = qRound(style->pixelMetric(QStyle::PM_SmallIconSize) * scaleFactor);

    QSize textSize = QFontMetrics(f).size(Qt::TextSingleLine, newText);
    if (textDocument != Q_NULL)
        textSize.rwidth() = textDocument->idealWidth();

    size.setHeight(margin + qMax(icon.isNull() ? 0 : iconSize, textSize.height()) + margin);
    size.setWidth(margin + (icon.isNull() ? 0 : iconSize + margin) + textSize.width() + margin);

    const GridDataTableColumn* tableColumn = qobject_cast<const GridDataTableColumn *>(column);
    if (tableColumn != Q_NULL && tableColumn->isSortEnabled())
        size.rwidth() += margin + iconSize;
    return size;
}

ColumnMovePosition Qtitan::qtn_get_layout_zone(const GraphicPrimitive* gp, const QPoint& point, Qt::Orientation layoutOrientation, bool canRowChange)
{
    if (layoutOrientation == Qt::Vertical)
    {
        if (canRowChange && point.y() >= 0 && point.y() < gp->height() / 5)
            return PreviousRowPosition;
        else if (canRowChange && point.y() > gp->height() - (gp->height() / 5) && point.y() <= gp->height())
            return NextRowPosition;
        else if (point.x() >= 0 && point.x() <= gp->width() / 2)
            return PreviousColumnPosition;
        else if (point.x() >= gp->width() / 2 && point.x() <= gp->width())
            return NextColumnPosition;
    }
    else
    {
        if (canRowChange && point.x() >= 0 && point.x() < gp->width() / 5)
            return PreviousRowPosition;
        else if (canRowChange && point.x() > gp->width() - (gp->width() / 5) && point.x() <= gp->width())
            return NextRowPosition;
        else if (point.y() >= 0 && point.y() <= gp->height() / 2)
            return PreviousColumnPosition;
        else if (point.y() >= gp->height() / 2 && point.y() <= gp->height())
            return NextColumnPosition;
    }
    return UndefinedPosition;
}

const QSize Qtitan::QtnGridSmallToolButtonSize = QSize(9, 9);
const QSize Qtitan::QtnGridNewRowIconSize = QSize(11, 11);
const QSize Qtitan::QtnFieldChooserButtonSize = QSize(32, 32);
const QSize Qtitan::QtnRowsQuickFrozenButtonSize = QSize(20, 20);
const int Qtitan::QtnFieldChooserPanelIndent = 5;
