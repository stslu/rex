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

#include "QtnGridDef.h"
#include "QtnGridBandedTableView.h"
#include "QtnGridViewGraphics.h"
#include "QtnGridTableViewBase.h"

QTITAN_USE_NAMESPACE

/* GridBandedTableViewEx */
class GridBandedTableViewEx: public GridBandedTableView
{
    GridBandedTableViewEx() : GridBandedTableView(0) {}
    friend class GridFieldChooserBandColumnProvider;
};

/* GridFieldChooserBandProviderImpl */
class GridFieldChooserBandColumnProvider: public GridFieldChooserProvider
{
public:
    GridFieldChooserBandColumnProvider(GridBandedTableView* view)
    : GridFieldChooserProvider(view)
    {
    }

    virtual QString title() const
    {
        return Grid::tr_compatible(QtnGridBandsString);
    }

    virtual QString description() const
    {
        return Grid::tr_compatible(QtnGridEmptyFieldChooserBandString);
    }

    virtual GridViewBase::GridColumns fields() const
    {
        GridBandedTableViewEx* bandedView = static_cast<GridBandedTableViewEx *>(view());
        GridViewBase::GridColumns fields;
        for (GridTableBands::const_iterator it = bandedView->bands().constBegin();
            it != bandedView->bands().constEnd(); ++it)
            fields.push_back(*it);
        return fields;
    }
    
    virtual bool isHiddenColumnsEnabled() const
    {
        return false;
    }
private:
};

/* GridBandedTableDragColumnObject */
GridBandedTableDragColumnObject::GridBandedTableDragColumnObject(const QPoint& point,
                                                                 GridTableColumnBase* column,
                                                                 GraphicPrimitive* source)
                                                                 : GridTableDragColumnObject(point, column, source),
  m_sourceBandIndex(-1), m_sourceRowIndex(-1), m_insertedRowIndex(-1), m_bandMovePosition(UndefinedPosition)
{
}

bool GridBandedTableDragColumnObject::checkDropColumnTarget(const QPoint& pos, GridColumnPrimitive* target)
{
    if (target == Q_NULL)
        return GridTableDragColumnObject::checkDropColumnTarget(pos, target);

    clear();
    GridTableBand* sourceBand = qobject_cast<GridTableBand *>(m_column);
    GridBandedTableColumn* sourceColumn = qobject_cast<GridBandedTableColumn *>(m_column);

    GridTableBand* targetBand = qobject_cast<GridTableBand *>(target->column());
    GridBandedTableColumn* targetColumn = qobject_cast<GridBandedTableColumn *>(target->column());

    if (sourceBand != Q_NULL)
    {
        ColumnMovePosition position = target->getMovePositionByPoint(pos, ((GridBandedTableView *)m_view)->options().changeNestedBandOnMoving());
        if (targetColumn != Q_NULL)
        {
            if (sourceBand->fixedKind() != targetColumn->fixedKind())
                return false;
            if (position != PreviousRowPosition)
                return false;
            setSourceBandIndex(targetColumn->bandIndex());
            setBandMovePosition(position);
            showMoveColumnRowArrows(target, position);
            return true;
        }
        if (sourceBand->fixedKind() != targetBand->fixedKind())
            return false;
        switch (position)
        {
        case PreviousRowPosition:
            {
                setSourceBandIndex(targetBand->index());
                setBandMovePosition(position);
                showMoveColumnRowArrows(target, position);
                return true;
            }
            break;
        case NextRowPosition:
            {
                setSourceBandIndex(targetBand->index());
                setBandMovePosition(position);
                showMoveColumnRowArrows(target, position);
                return true;
            }
            break;
        case NextColumnPosition:
        case PreviousColumnPosition:
            {
                if (source() == target)
                    return false;

                if (targetBand->bandIndex() != sourceBand->bandIndex())
                {
                    setSourceBandIndex(targetBand->bandIndex());
                    if (position == NextColumnPosition)
                        setSourceVisualIndex(targetBand->visualIndex() + 1);
                    else
                        setSourceVisualIndex(targetBand->visualIndex());
                }
                else
                {
                    if (position == NextColumnPosition)
                        setSourceVisualIndex(targetBand->visualIndex() + 1);
                    else
                        setSourceVisualIndex(targetBand->visualIndex());

                    if (sourceBand->visualIndex() < targetBand->visualIndex())
                        setSourceVisualIndex(sourceVisualIndex() - 1);

                    if ((sourceVisualIndex() == sourceBand->visualIndex()) && 
                        (sourceBand->isVisible() == targetBand->isVisible()))
                        return false;
                    setSourceBandIndex(sourceBand->bandIndex());
                }

                if (sourceBand->fixedKind() != targetBand->fixedKind())
                    return false;

                showMoveColumnArrows(target, position);
                return true;
            }
        default:
            return false;
        }
    }

    if (sourceColumn != Q_NULL)
    {
        ColumnMovePosition position = target->getMovePositionByPoint(pos, ((GridBandedTableView *)m_view)->options().changeColumnRowOnMoving());
        if (targetBand != Q_NULL)
        {
            if (sourceColumn->fixedKind() != targetBand->fixedKind())
                return false;
            if (!targetBand->isBottomBand())
                return false;
            if (position != NextRowPosition)
                return false;
            setInsertedRowIndex(0);
            setSourceBandIndex(targetBand->index());
            showMoveColumnRowArrows(target, position);
            return true;
        }

        if (sourceColumn->fixedKind() != targetColumn->fixedKind())
            return false;

        Q_ASSERT(targetColumn != Q_NULL);
        if (targetColumn->bandIndex() != sourceColumn->bandIndex())
            setSourceBandIndex(targetColumn->bandIndex());

        switch (position)
        {
        case PreviousRowPosition:
            {
                setInsertedRowIndex(targetColumn->rowIndex());
                showMoveColumnRowArrows(target, position);
                return true;
            }
            break;
        case NextRowPosition:
            {
                setInsertedRowIndex(targetColumn->rowIndex() + 1);
                showMoveColumnRowArrows(target, position);
                return true;
            }
            break;
        case PreviousColumnPosition:
        case NextColumnPosition:
            {
                if (sourceColumn == targetColumn)
                    return false;

                if (position == NextColumnPosition)
                    setSourceVisualIndex(targetColumn->visualIndex() + 1);
                else
                    setSourceVisualIndex(targetColumn->visualIndex());

                if (targetColumn->rowIndex() != sourceColumn->rowIndex())
                    setSourceRowIndex(targetColumn->rowIndex());

                if (sourceBandIndex() == -1)
                {
                    if (sourceColumn->visualIndex() < targetColumn->visualIndex())
                        setSourceVisualIndex(sourceVisualIndex() - 1);
                    
                    if (sourceRowIndex() == -1)
                    {
                        if ((sourceVisualIndex() == sourceColumn->visualIndex()) && 
                            (targetColumn->isVisible() == sourceColumn->isVisible()))
                            return false;
                    }
                }

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

void GridBandedTableDragColumnObject::acceptEvent()
{
    GridViewAutoUpdater autoUpdater(m_view);

    GridBandedTableColumn* sourceColumn = 
        qobject_cast<GridBandedTableColumn *>(m_column);
    GridTableBand* sourceBand = 
        qobject_cast<GridTableBand *>(m_column);

    if (sourceColumn != Q_NULL)
    {
        sourceColumn->setVisible(true);

        if (sourceBandIndex() != -1)
            sourceColumn->setBandIndex(sourceBandIndex());

        if (insertedRowIndex() != -1)
            ((GridBandedTableView *)m_view)->createColumnRow(insertedRowIndex(), sourceColumn);

        if (sourceRowIndex() != -1)
            sourceColumn->setRowIndex(sourceRowIndex());

        //grouping and visual index
        GridTableDragColumnObject::acceptEvent();
    }

    if (sourceBand != Q_NULL)
    {
        sourceBand->setVisible(true);

        if (bandMovePosition() == PreviousRowPosition || bandMovePosition() == NextRowPosition)
        {
            Q_ASSERT(sourceBandIndex() != -1);
            ((GridBandedTableView *)m_view)->createBandRow(sourceBand, sourceBandIndex(), bandMovePosition());
        }
        else
        {
            if (sourceBand->index() == sourceBandIndex())
                setSourceVisualIndex(sourceVisualIndex() + sourceBand->visualIndex());

            sourceBand->setBandIndex(sourceBandIndex());
            sourceBand->setVisualIndex(sourceVisualIndex());
        }
    }
}

void GridBandedTableDragColumnObject::clear()
{
    GridTableDragColumnObject::clear();
    m_sourceBandIndex = -1;
    m_sourceRowIndex = -1;
    m_insertedRowIndex = -1;
    m_bandMovePosition = UndefinedPosition;
}

int GridBandedTableDragColumnObject::sourceBandIndex() const
{
    return m_sourceBandIndex;
}

void GridBandedTableDragColumnObject::setSourceBandIndex(int bandIndex)
{
    m_sourceBandIndex = bandIndex;
}

int GridBandedTableDragColumnObject::sourceRowIndex() const
{
    return m_sourceRowIndex;
}

void GridBandedTableDragColumnObject::setSourceRowIndex(int rowIndex)
{
    m_sourceRowIndex = rowIndex;
}

int GridBandedTableDragColumnObject::insertedRowIndex() const
{
    return m_insertedRowIndex;
}

void GridBandedTableDragColumnObject::setInsertedRowIndex(int index)
{
    m_insertedRowIndex = index;
}

ColumnMovePosition GridBandedTableDragColumnObject::bandMovePosition() const
{
    return m_bandMovePosition;
}

void GridBandedTableDragColumnObject::setBandMovePosition(ColumnMovePosition position)
{
    m_bandMovePosition = position;
}

GridTableBand* Qtitan::get_parent_band(const GridColumnBase* columnBase)
{
    const GridTableBand* band = qobject_cast<const GridTableBand *>(columnBase);
    if (band != Q_NULL)
        return band->parentBand();
    const GridBandedTableColumn* bandedColumn = qobject_cast<const GridBandedTableColumn *>(columnBase);
    if (bandedColumn != Q_NULL)
        return bandedColumn->parentBand();
    return Q_NULL;
}

void GridBandedTableDragColumnObject::showMoveColumnRowArrows(GridColumnPrimitive* target, ColumnMovePosition position)
{
    GridTableBand* parentBand = Q_NULL;
    GridTableBand* band = qobject_cast<GridTableBand *>(target);
    if (band != Q_NULL)
    {
        if (position == NextRowPosition)
            parentBand = band;
        else
            parentBand = band->parentBand();
    }
    else
        parentBand = get_parent_band(target->column());

    QPoint targetPos = target->mapViewportToGlobal(QPoint(0, 0));
    if (position == NextRowPosition)
    {
        if (m_view->layout()->orientation() == Qt::Vertical)
            targetPos.ry() += target->height();
        else
            targetPos.rx() += target->width();
    }


    QRect scrollRect;
    m_view->layout()->calcScreenScrollRect(scrollRect);
    QPoint parentPos = parentBand != Q_NULL ? parentBand->columnGP()->mapToGlobal(QPoint(0, 0)) : scrollRect.topLeft();

    QSize parentViewSize = parentBand != Q_NULL ? parentBand->columnGP()->size() : scrollRect.size();

    if (band != Q_NULL)
    {
        parentPos = targetPos;
        parentViewSize = target->size();
    }

    if (m_view->layout()->orientation() == Qt::Vertical)
    {
        int xpos = parentPos.x();
        if (xpos < scrollRect.x())
            xpos = scrollRect.x();
        xpos -= m_rightArrow->width();

        int ypos = targetPos.y() - m_leftArrow->height() / 2 - 1;
        m_rightArrow->move(xpos, ypos);
        m_rightArrow->show();

        xpos = parentPos.x() + parentViewSize.width();
        if (xpos > scrollRect.x() + scrollRect.width())
            xpos = scrollRect.x() + scrollRect.width();

        m_leftArrow->move(xpos, ypos);
        m_leftArrow->show();

        m_downArrow->hide();
        m_upArrow->hide();
    }
    else
    {
        int xpos = targetPos.x() - m_downArrow->width() / 2 - 1;

        int ypos = parentPos.y();
        if (ypos < scrollRect.y())
            ypos = scrollRect.y();
        ypos -= m_downArrow->height();

        m_downArrow->move(xpos, ypos);
        m_downArrow->show();

        ypos = parentPos.y() + parentViewSize.height();
        if (ypos > scrollRect.y() + scrollRect.height())
            ypos = scrollRect.y() + scrollRect.height();

        m_upArrow->move(xpos, ypos);
        m_upArrow->show();

        m_leftArrow->hide();
        m_rightArrow->hide();
    }
}

/* GridBandedTableView */
GridBandedTableView* GridBandedTableView::create(Grid* grid)
{
    GridBandedTableView* retval = new GridBandedTableView(grid);
    retval->initialize();
    return retval;
}

GridBandedTableView::GridBandedTableView(Grid* grid)
: GridTableView(grid)
{
}

GridBandedTableView::~GridBandedTableView()
{
    m_viewStates = ViewDestroying;

    GridViewAutoUpdater autoUpdater(this);
    removeBands();
    Q_ASSERT(m_rootBands.size() == 0);
}

void GridBandedTableView::initialize()
{
    m_fieldProviders.push_front(new GridFieldChooserBandColumnProvider(this));
    GridTableView::initialize();
    //fieldChooserWindow()->addFiledProvider(m_bandProvider);
}

bool GridBandedTableView::hasVisibleParent(const GridColumnBase* column) const
{
    const GridTableBand* band = get_parent_band(column);

    while (band != Q_NULL)
    {
        if (!band->isVisible())
            return false;
        band = band->parentBand();
    }
    return true;
}

void GridBandedTableView::bestFitForView()
{
    for (GridTableBands::const_iterator it = m_bandslist.constBegin(); it != m_bandslist.constEnd(); ++it)
    {
        if (isColumnVisible(*it))
            (*it)->bestFit();
    }
    GridTableView::bestFitForView();
}


GridBandedTableColumn* GridBandedTableView::addColumn(const QString& modelColumnName,
                                                      const QString& caption)
{
    return static_cast<GridBandedTableColumn *>(GridViewBase::addColumn(modelColumnName, caption));
}

GridBandedTableColumn* GridBandedTableView::addColumn(int modelColumn, 
                                                      const QString& caption)
{
    return static_cast<GridBandedTableColumn *>(GridViewBase::addColumn(modelColumn, caption));
}

GridTableBand* GridBandedTableView::addBand(const QString& caption)
{
    GridViewAutoUpdater autoUpdater(this);

    GridTableBand* band = new GridTableBand(this, caption);
    m_bandslist.push_back(band);
    band->initialize();
    addNestedBand(band);
    return band;
}

void GridBandedTableView::removeBands()
{
    while (m_bandslist.size() > 0)
        removeBand(m_bandslist.size() - 1);
}

void GridBandedTableView::removeBand(int index)
{
    if (index >= 0 && index < m_bandslist.size())
    {
        GridTableBand* band = m_bandslist.at(index);
        GridTableBands bands = band->m_nestedBands;
        for (GridTableBands::const_iterator it = bands.constBegin(); 
            it != bands.constEnd(); ++it)
        {
            (*it)->setBandIndex(-1);
        }

        GridBandedTableColumns cols = band->m_nestedCols;
        for (GridBandedTableColumns::const_iterator it = cols.constBegin(); 
            it != cols.constEnd(); ++it)
        {
            (*it)->setBandIndex(-1);
        }

        GridTableBand* parentBand = band->parentBand();
        if (parentBand != Q_NULL)
            parentBand->removeNestedBand(band);
        else
            removeNestedBand(band);

        m_bandslist.removeAt(index);
        band->finalize();
        delete band;
    }
}

int GridBandedTableView::bandCount() const
{
    return m_bandslist.size();
}

int GridBandedTableView::columnRowCount(GridTableBand* band) const
{
    int retval = 0;
    if (band == Q_NULL)
        return retval;

    for (GridBandedTableColumns::const_iterator it = band->m_nestedCols.constBegin(); 
        it != band->m_nestedCols.constEnd(); ++it)
    {
        if (!isColumnVisible(*it))
            continue;
        GridBandedTableColumn* column = *it;
        if (column->rowIndex() > retval - 1)
            retval = column->rowIndex() + 1;
    }
    return retval;
}

#ifndef QTN_NOUSE_XML_MODULE
bool GridBandedTableView::saveLayoutToStream(QXmlStreamWriter* xmlwriter)
{
    //At first, sorting the bands by visualIndex for the right restore process.
    QMultiMap<int, GridTableBand *> sorted;
    for (GridTableBands::const_iterator it = m_bandslist.constBegin(); it != m_bandslist.constEnd(); ++it)
    {
        GridTableBand* band = *it;
        sorted.insert(band->visualIndex(), band);
    }

    xmlwriter->writeStartElement("Qtitan:Bands");
    for (QMultiMap<int, GridTableBand *>::const_iterator it = sorted.constBegin(); it != sorted.constEnd(); ++it)
    {
        xmlwriter->writeStartElement("Qtitan:Band");
        xmlwriter->writeAttribute("id", QString("%1").arg((*it)->index()));
        xmlwriter->writeAttribute("bid", QString("%1").arg((*it)->bandIndex()));
        xmlwriter->writeAttribute("width", QString("%1").arg((*it)->width()));
        xmlwriter->writeAttribute("visual", QString("%1").arg((*it)->visualIndex()));
        if (!saveColumnBaseLayout(*it, xmlwriter))
            return false;
        xmlwriter->writeEndElement();
    }
    xmlwriter->writeEndElement();
    return GridTableView::saveLayoutToStream(xmlwriter);
}

bool GridBandedTableView::loadLayoutFromStream(QXmlStreamReader* xmlreader)
{
    if (xmlreader->readNext() != QXmlStreamReader::StartElement)
        return false;
    if (xmlreader->name() != "Bands")
        return false;
    xmlreader->readNext();
    while (xmlreader->tokenType() == QXmlStreamReader::StartElement)
    {
        QXmlStreamAttributes attrs = xmlreader->attributes();
        if (!attrs.hasAttribute("id"))
            return false;
        int id = attrs.value("id").toString().toInt();
        if (id < 0 || id >= m_bandslist.size())
            return false;
        GridTableBand* band = m_bandslist[id];
        if (band == Q_NULL)
            return false;

        if (attrs.hasAttribute("bid"))
            band->setBandIndex(attrs.value("bid").toString().toInt());
        if (attrs.hasAttribute("width"))
            band->setWidth(attrs.value("width").toString().toInt());
        if (attrs.hasAttribute("visual"))
            band->setVisualIndex(attrs.value("visual").toString().toInt());

        if (!loadColumnBaseLayout(band, xmlreader))
            return false;
        if (xmlreader->readNext() != QXmlStreamReader::EndElement)
            return false;
        xmlreader->readNext();
    }
    if (xmlreader->tokenType() != QXmlStreamReader::EndElement)
        return false;
    return GridTableView::loadLayoutFromStream(xmlreader);
}

bool GridBandedTableView::saveTableColumnLayout(const GridDataTableColumn* column, QXmlStreamWriter* xmlwriter)
{
    if (!GridTableView::saveTableColumnLayout(column, xmlwriter))
        return false;

    const GridBandedTableColumn* bandColumn = qobject_cast<const GridBandedTableColumn *>(column);
    xmlwriter->writeStartElement("Qtitan:BandedData");
    xmlwriter->writeAttribute("bid", QString("%1").arg(bandColumn->bandIndex()));
    xmlwriter->writeAttribute("rindex", QString("%1").arg(bandColumn->rowIndex()));
    xmlwriter->writeAttribute("rspan", QString("%1").arg(bandColumn->rowSpan()));
    xmlwriter->writeEndElement();
    return true;
}

bool GridBandedTableView::loadTableColumnLayout(GridDataTableColumn* column, QXmlStreamReader* xmlreader)
{
    if (!GridTableView::loadTableColumnLayout(column, xmlreader))
        return false;
    GridBandedTableColumn* bandColumn = qobject_cast<GridBandedTableColumn *>(column);
    if (xmlreader->readNext() != QXmlStreamReader::StartElement || xmlreader->name() != "BandedData")
        return false;
    QXmlStreamAttributes attrs = xmlreader->attributes();
    if (attrs.hasAttribute("bid"))
    {
        int bandIndex = attrs.value("bid").toString().toInt();
        if (bandColumn->bandIndex() != bandIndex)
            bandColumn->setBandIndex(bandIndex);
    }
    if (attrs.hasAttribute("rindex"))
    {
        int rowIndex = attrs.value("rindex").toString().toInt();
        if (bandColumn->rowIndex() != rowIndex)
            bandColumn->setRowIndex(rowIndex);
    }
    if (attrs.hasAttribute("rspan"))
    {
        int rowSpan = attrs.value("rspan").toString().toInt();
        if (bandColumn->rowSpan() != rowSpan)
            bandColumn->setRowSpan(rowSpan);
    }
    if (xmlreader->readNext() != QXmlStreamReader::EndElement)
        return false;
    return true;
}
#endif

int GridBandedTableView::getColumnRowIndex(const GridDataTableColumn* column) const
{
    const GridBandedTableColumn* bandedColumn = static_cast<const GridBandedTableColumn *>(column);
    return bandedColumn->m_row;
}

void GridBandedTableView::setColumnRowIndex(GridBandedTableColumn* column, int rowIndex)
{
    if (column->parentBand() == Q_NULL)
        return;
    GridViewAutoUpdater autoUpdater(this);
    int oldRowIndex = column->rowIndex();

    GridTableBand* p = column->parentBand();
    int rowCount = 0;
    int rowIndexAnchor = 0;
    int index = 0;

    p->m_nestedCols.removeOne(column);
    for (GridBandedTableColumns::const_iterator it = p->m_nestedCols.constBegin(); 
        it != p->m_nestedCols.constEnd(); ++it)
    {
        if (isColumnVisible(*it))
        {
            GridBandedTableColumn* bandedColumn = *it;
            if (bandedColumn->rowIndex() > rowCount - 1)
                rowCount = bandedColumn->rowIndex() + 1;
            if (bandedColumn->rowIndex() <= rowIndex)
                rowIndexAnchor = index + 1;
        }
        index++;
    }

    if (layout()->orientation() == Qt::Horizontal)
    {
        for (int i = 0; i < p->m_nestedCols.size(); ++i)
        {
            if (isColumnVisible(p->m_nestedCols[i]))
            {
                column->setExactWidth(p->m_nestedCols[i]->exactWidth());
                break;
            }
        }
    }
    p->m_nestedCols.insert(rowIndexAnchor, column);
    

    if (rowIndex > rowCount)
        rowIndex = rowCount;

    column->m_row = rowIndex;
    ensureRowValid(p, oldRowIndex);
    requestUpdateLayout();
}

void GridBandedTableView::getBottomLevelBands(GridTableBand* band, GridTableBands& bands) const
{
    const GridTableBands& nestedBands = (band == Q_NULL ? m_rootBands : band->m_nestedBands);
    for (GridTableBands::const_iterator it = nestedBands.constBegin(); it != nestedBands.constEnd(); ++it)
    {
        if ((*it)->isBottomBand())
            bands.push_back(*it);
        else
            getBottomLevelBands(*it, bands);
    }
}

GridTableBand* GridBandedTableView::getTopLevelBand(const GridTableColumnBase* columnBase) const
{
    GridTableBand* parent = get_parent_band(columnBase);
    if (parent == Q_NULL)
        return qobject_cast<GridTableBand *>((GridTableColumnBase *)columnBase);

    while (parent->parentBand() != Q_NULL)
        parent = parent->parentBand();
    return parent;
}

void GridBandedTableView::createColumnRow(int row, GridBandedTableColumn* column)
{
    GridTableBand* parentBand = column->parentBand();
    if (parentBand == Q_NULL)
        return;
    column->setGroupIndex(kGridColumnIndexInvalid);
    //GridBandedTableColumn* anchorColumn = Q_NULL;
    for (GridBandedTableColumns::const_iterator it = parentBand->m_nestedCols.constBegin();
        it != parentBand->m_nestedCols.constEnd(); ++it)
    {
        GridBandedTableColumn* bandedColumn = *it;
        if (bandedColumn->rowIndex() >= row)
            bandedColumn->m_row++;
        //else
        //    anchorColumn = bandedColumn;
    }
    column->setRowIndex(row);
    //if (anchorColumn != Q_NULL)
    //    column->setVisualIndex(anchorColumn->visualIndex());
    //else
    //    column->setVisualIndex(0);
}

//static bool is_band_has_parent(const GridTableBand* band, const GridTableBand* parent)
//{
//    while (band->parentBand() != Q_NULL)
//    {
//        band = band->parentBand();
//        if (band == parent)
//            return true;
//    }
//    return false;
//}

void GridBandedTableView::createBandRow(GridTableBand* band, int bandIndex, ColumnMovePosition position)
{
    GridTableBand* destBand = getBand(bandIndex);
    Q_ASSERT(destBand != Q_NULL);
    if (position == NextRowPosition)
    {
        GridTableBands bands = destBand->m_nestedBands;
        for (GridTableBands::const_iterator it = bands.constBegin(); 
            it != bands.constEnd(); ++it)
            (*it)->setBandIndex(band->index());
        band->setBandIndex(bandIndex);
    }
    else if (position == PreviousRowPosition)
    {
        band->setBandIndex(destBand->bandIndex());
        band->setVisualIndex(destBand->visualIndex());
        destBand->setBandIndex(band->index());
        destBand->setVisualIndex(0);
    }
}

GridBandedTableViewOptions& GridBandedTableView::options() const
{
    return *(static_cast<GridBandedTableViewOptions *>(optionsref()));
}

void GridBandedTableView::addColumns()
{
    GridTableView::addColumns();
    int count = getColumnCount();
    for (int i = 0; i < count; ++i)
    {
        GridBandedTableColumn* column = (GridBandedTableColumn *)getColumn(i);
        if (column->bandIndex() == -1)
        {
            if (bandCount() == 0)
                addBand("Band 1");
            column->setBandIndex(0);
        }
    }
}

void GridBandedTableView::setModelToController(QAbstractItemModel* model, const QModelIndex& parent)
{
    GridViewAutoUpdater autoUpdater(this);

    if (model == Q_NULL)
        removeBands();
    GridTableView::setModelToController(model, parent);
}

void GridBandedTableView::navigateUp(Qt::KeyboardModifiers modifiers)
{
    GridTableView::navigateUp(modifiers);
}

void GridBandedTableView::navigateDown(Qt::KeyboardModifiers modifiers)
{
    GridTableView::navigateDown(modifiers);
}

GridDataTableColumn* GridBandedTableView::getNextColumn(const GridDataTableColumn* anchor) const
{
    GridTableBands bottomBands;
    getBottomLevelBands(Q_NULL, bottomBands);
    QListIterator<GridTableBand *> bandIterator(bottomBands);
    bandIterator.toFront();
    while (bandIterator.hasNext())
    {
        GridTableBand* band = bandIterator.next();
        if (!isColumnVisible(band))
            continue;
        GridBandedTableColumn* column = band->getNextColumn(static_cast<const GridBandedTableColumn *>(anchor));
        if (column != Q_NULL)
            return column;
        if (anchor != Q_NULL && 
            static_cast<const GridBandedTableColumn *>(anchor)->parentBand() == band)
            anchor = Q_NULL;
    }
    return Q_NULL;
}

GridDataTableColumn* GridBandedTableView::getPrevColumn(const GridDataTableColumn* anchor) const
{
    GridTableBands bottomBands;
    getBottomLevelBands(Q_NULL, bottomBands);
    QListIterator<GridTableBand *> bandIterator(bottomBands);
    bandIterator.toBack();
    while (bandIterator.hasPrevious())
    {
        GridTableBand* band = bandIterator.previous();
        if (!isColumnVisible(band))
            continue;
        GridBandedTableColumn* column = band->getPrevColumn(static_cast<const GridBandedTableColumn *>(anchor));
        if (column != Q_NULL)
            return column;
        if (anchor != Q_NULL && 
            static_cast<const GridBandedTableColumn *>(anchor)->parentBand() == band)
            anchor = Q_NULL;
    }
    return Q_NULL;
}

void GridBandedTableView::ensureRowValid(GridTableBand* band, int row)
{    
    Q_ASSERT(band != Q_NULL);
    for (GridBandedTableColumns::const_iterator it = band->m_nestedCols.constBegin();
        it != band->m_nestedCols.constEnd(); ++it)
    {
        GridBandedTableColumn* bandedColumn = static_cast<GridBandedTableColumn *>(*it);
        if (bandedColumn->rowIndex() == row)
            return;
    }

    for (GridBandedTableColumns::const_iterator it = band->m_nestedCols.constBegin();
        it != band->m_nestedCols.constEnd(); ++it)
    {
        GridBandedTableColumn* bandedColumn = static_cast<GridBandedTableColumn *>(*it);
        if (bandedColumn->rowIndex() > row)
            bandedColumn->m_row--;
    }
}

GridColumnBase* GridBandedTableView::createColumn(int modelColumn, const QString& caption)
{
    GridBandedTableColumn* retval = new GridBandedTableColumn(this, modelColumn, caption);
    return retval;
}

int GridBandedTableView::getColumnIndex(const GridColumnBase* columnBase) const
{
    const GridTableBand* band = qobject_cast<const GridTableBand *>(columnBase);
    if (band != Q_NULL)
        return getBandIndex(band);
    else
        return GridTableView::getColumnIndex(columnBase);
}

GridTableColumnBase* GridBandedTableView::getPreviousResizeColumn(const GridTableColumnBase* columnBase) const
{
    GridTableColumnBase* retval = Q_NULL;
    if (layout()->orientation() == Qt::Vertical)
    {
        retval = static_cast<GridTableColumnBase *>(getPrevious(columnBase));
        if (retval == Q_NULL)
        {
            GridTableBand* band = get_parent_band(columnBase);
            while (band != Q_NULL)
            {
                retval = static_cast<GridTableColumnBase *>(getPrevious(band));
                if (retval != Q_NULL)
                    break;
                band = band->parentBand();
            }
        }
        GridTableBand* br = getTopLevelBand(columnBase);
        GridTableBand* bl = getTopLevelBand(retval);
        if (br != Q_NULL && br->fixedKind() == FixedRight && bl != Q_NULL && bl->fixedKind() != FixedRight)
            return Q_NULL;
    }
    else
    {
        GridTableBand* parentBand = get_parent_band(columnBase);
        const GridBandedTableColumn* column = qobject_cast<const GridBandedTableColumn *>(columnBase);
        if (column == Q_NULL || column->rowIndex() == 0)
            retval = parentBand;
        else
        {
            for (GridBandedTableColumns::const_iterator it = parentBand->m_nestedCols.constBegin();
                it != parentBand->m_nestedCols.constEnd(); ++it)
            {
                if ((*it)->rowIndex() == column->rowIndex() - 1)
                {
                    retval = *it;
                    break;
                }
            }
        }
    }
    return retval;
}

GridTableColumnBase* GridBandedTableView::getNextResizeColumn(const GridTableColumnBase* column) const
{
    return (GridTableColumnBase *)column;
}

bool GridBandedTableView::isVerticalResizable(const GridTableColumnBase* columnBase) const
{
    return (qobject_cast<const GridTableBand *>(columnBase) == Q_NULL) && checkVerSizing(columnBase);
}

bool GridBandedTableView::isHorizontalResizable(const GridTableColumnBase* columnBase) const
{
    if (!checkHorSizing(columnBase))
        return false;
    if (!options().columnAutoWidth())
        return true;
    else if (GridTableView::isHorizontalResizable(columnBase))
        return true;
    else
    {
        GridTableColumnBase* parent = get_parent_band(columnBase);
        if (parent != Q_NULL)
            return isHorizontalResizable(parent);
    }
    return false;
}

ColumnFixedKind GridBandedTableView::getColumnFixedKind(const GridTableColumnBase* columnBase) const
{
    GridTableBand* band = getTopLevelBand(columnBase);
    if (band != Q_NULL)
        return GridTableView::getColumnFixedKind(band);
    else
        return FixedNone;
}

void GridBandedTableView::setColumnFixedKind(GridTableColumnBase* columnBase, ColumnFixedKind kind)
{
    GridTableBand* band = getTopLevelBand(columnBase);
    if (band != Q_NULL)
    {
        GridTableView::setColumnFixedKind(band, kind);
    }
}

int GridBandedTableView::getVisualIndex(const GridColumnBase* columnBase) const
{
    const GridTableBand* band = qobject_cast<const GridTableBand *>(columnBase);
    if (band != Q_NULL)
    {
        GridTableBand* parentBand = band->parentBand();
        if (parentBand != Q_NULL)
            return parentBand->m_nestedBands.indexOf((GridTableBand *)band);
        else
            return m_rootBands.indexOf((GridTableBand *)band);
    }

    const GridBandedTableColumn* bandedColumn = qobject_cast<const GridBandedTableColumn *>(columnBase);
    if (bandedColumn != Q_NULL)
    {
        GridTableBand* parentBand = bandedColumn->parentBand();
        if (parentBand != Q_NULL)
        {
            return parentBand->m_nestedCols.indexOf((GridBandedTableColumn *)columnBase);
        }
    }
    return -1;
}

void GridBandedTableView::setVisualIndex(const GridColumnBase* tableColumn, int visualIndex)
{
    GridViewAutoUpdater autoUpdater(this);

    const GridTableBand* band = qobject_cast<const GridTableBand *>(tableColumn);
    if (band != Q_NULL)
    {
        GridTableBand* parentBand = band->parentBand();
        if (parentBand != Q_NULL)
        {
            int index = parentBand->m_nestedBands.indexOf((GridTableBand *)band);
            Q_ASSERT(index != -1);
            parentBand->m_nestedBands.move(index, visualIndex);
        }
        else
        {
            int index = m_rootBands.indexOf((GridTableBand *)band);
            Q_ASSERT(index != -1);
            m_rootBands.move(index, visualIndex);
        }
        requestUpdateLayout();
    }
    const GridBandedTableColumn* bandedColumn = qobject_cast<const GridBandedTableColumn *>(tableColumn);
    if (bandedColumn != Q_NULL)
    {
        GridTableBand* parentBand = bandedColumn->parentBand();
        if (parentBand != Q_NULL)
        {
            int index = parentBand->m_nestedCols.indexOf((GridBandedTableColumn *)bandedColumn);
            Q_ASSERT(index != -1);
            parentBand->m_nestedCols.move(index, visualIndex);
            requestUpdateLayout();
            //GridTableView::setVisualIndex(tableColumn, visualIndex); TODO
            return;
        }
    }
}

void GridBandedTableView::getColumnLine(const GridColumnBase* column, GridColumns& retval) const
{
    Q_ASSERT(column != Q_NULL);
    retval.clear();
    const GridTableBand* band = qobject_cast<const GridTableBand *>(column);
    if (band)
    {
        GridTableBand* parentBand = band->parentBand();
        const GridTableBands& nestedBands = parentBand != Q_NULL ? parentBand->m_nestedBands : m_rootBands;
        for (GridTableBands::const_iterator it = nestedBands.constBegin(); 
            it != nestedBands.constEnd(); ++it)
        {
            if (!isColumnVisible(*it))
                continue;
            retval.push_back(*it);
        }
        return;
    }
    const GridBandedTableColumn* bandedColumn = qobject_cast<const GridBandedTableColumn *>(column);
    if (bandedColumn)
    {
        GridTableBand* parentBand = bandedColumn->parentBand();
        if (parentBand == Q_NULL)
            return;
        for (GridBandedTableColumns::const_iterator it = parentBand->m_nestedCols.constBegin(); 
            it != parentBand->m_nestedCols.constEnd(); ++it)
        {
            if (!isColumnVisible(*it))
                continue;
            if ((*it)->rowIndex() == bandedColumn->rowIndex())
                retval.push_back(*it);
        }
    }
}

int GridBandedTableView::getMaximumVisualIndex(const GridTableColumnBase* anchor) const
{
    const GridTableBand* band = qobject_cast<const GridTableBand *>(anchor);
    if (band != Q_NULL)
    {
        GridTableBand* parentBand = band->parentBand();
        if (parentBand != Q_NULL)
        {
            return parentBand->m_nestedBands.size() - 1;
        }
        else
        {
            return m_rootBands.size() - 1;
        }
    }

    const GridBandedTableColumn* bandedColumn = qobject_cast<const GridBandedTableColumn *>(anchor);
    if (bandedColumn != Q_NULL)
    {
        GridTableBand* parentBand = bandedColumn->parentBand();
        if (parentBand != Q_NULL)
        {
            return parentBand->m_nestedCols.size() - 1;
        }
    }
    Q_ASSERT(false);
    return -1;
}

int GridBandedTableView::getColumnViewWidth(const GridTableColumnBase* column)
{
    const GridTableBand* band = get_parent_band(column);
    if (band)
        return band->columnGP()->width() - (hasGroupingIndent(band) ? 0 : 0);
    else
        return GridTableView::getColumnViewWidth(column);
}

/*
bool GridBandedTableView::getColumnVisible(const GridColumnBase* columnBase) const
{
    const GridTableBand* band = qobject_cast<const GridTableBand *>(columnBase);
    if (band != Q_NULL)
        return GridTableView::getColumnVisible(band);
    else
    {
        const GridBandedTableColumn* column = qobject_cast<const GridBandedTableColumn *>(columnBase);
        if (column == Q_NULL)
            return false;
        band = column->parentBand();
        if (band == Q_NULL)
            return false;

        while (band != Q_NULL)
        {
            if (!GridTableView::getColumnVisible(band))
                return false;
            band = band->parentBand();
        }

        return GridTableView::getColumnVisible(column);
    }
    return false;
}
*/
void GridBandedTableView::columnVisibleChanged(GridColumnBase* columnBase, bool visible)
{
    GridViewAutoUpdater autoUpdater(this);
    GridTableBand* band = qobject_cast<GridTableBand *>(columnBase);
    if (band == Q_NULL)
        GridTableView::columnVisibleChanged(columnBase, visible);
    else
    {
        layout()->columnVisibleChanged(columnBase, visible);
 
        GridViewBase::GridColumns cols;
        collectBandChilds(band, cols, false);
        for (GridViewBase::GridColumns::const_iterator it = cols.constBegin();
            it != cols.constEnd(); ++it)
        {
            columnVisibleChanged(*it, visible);
        }
    }
}

GridViewOptions* GridBandedTableView::createOptions()
{
    return new GridBandedTableViewOptions(this);
}

GridTableHeaderGraphic* GridBandedTableView::createHeaderPanel(GraphicPrimitive* parent)
{
    return new GridBandedTableHeaderGraphic(parent, this);
}

int GridBandedTableView::getBandIndex(const GridTableBand* band) const
{
    return m_bandslist.indexOf((GridTableBand *)band);
}

GridTableBand* GridBandedTableView::getBand(int index) const
{
    if (index >= 0 && index < m_bandslist.size())
        return m_bandslist.at(index);
    else
        return Q_NULL;
}

void GridBandedTableView::addNestedBand(GridTableBand* band)
{
    Q_ASSERT(band != Q_NULL);
    if (m_rootBands.indexOf(band) == -1)
    {
        if (band->m_parentBand != Q_NULL)
            band->m_parentBand->removeNestedBand(band);
        band->m_parentBand = Q_NULL;
        m_rootBands.push_back(band);
        parentBandChanged(band);
    }
    markColumnsChanged();
}

int GridBandedTableView::removeNestedBand(GridTableBand* band)
{
    int retval = m_rootBands.indexOf(band);
    Q_ASSERT(band != Q_NULL && band->m_parentBand == Q_NULL && retval != -1);
    m_rootBands.removeAll(band);
    markColumnsChanged();
    return retval;
}

void GridBandedTableView::parentBandChanged(GridTableBand* band)
{
    Q_UNUSED(band);
}

void GridBandedTableView::parentBandChanged(GridBandedTableColumn* column)
{
    Q_UNUSED(column);
}

const GridTableBands& GridBandedTableView::bands() const
{
    return m_bandslist;
}

GridViewPrimitiveBase* GridBandedTableView::createLayout()
{
    return GridTableView::createLayout();
}

bool GridBandedTableView::hasGroupingIndent(const GridColumnBase* column) const
{
    const GridTableBand* band = qobject_cast<const GridTableBand *>(column);
    if (band == Q_NULL)
    {
        const GridBandedTableColumn* bandedColumn = qobject_cast<const GridBandedTableColumn *>(column);
        if (bandedColumn == Q_NULL || !GridTableView::hasGroupingIndent(bandedColumn))
            return false;
        band = bandedColumn->parentBand();
    }

    if (band == Q_NULL)
        return false;

    while (band != Q_NULL)
    {
        if (!GridTableView::hasGroupingIndent(band))
            return false;
        band = band->parentBand();
    }
    return true;
}

bool GridBandedTableView::printBandHeader(QPainter& painter,
    int& pageLength,
    const int origPageLength,
    GridTableBand* band, 
    GridBandedTableColumn* &startColumn)
{
    bool verticalLayout = layout()->orientation() == Qt::Vertical;

    if (startColumn != Q_NULL && !startColumn->isDescendant(band))
        return true;

    painter.save();

#if 0
    GridTableBand* toplevelBand = Q_NULL;

    if (startColumn != Q_NULL)
        toplevelBand = getTopLevelBand(startColumn);
#endif

    band->columnGP()->print(painter);
    if (verticalLayout)
        painter.translate(0, band->columnGP()->height());
    else
        painter.translate(band->columnGP()->width(), 0);


    if (band->isBottomBand())
    {
        int offset = -1;
        int length = 0;
        bool skip = startColumn != Q_NULL;
        bool exit = false;
        int exitPageLength = 0;
        int startPos = -1;

        for (GridBandedTableColumns::const_iterator it = band->m_nestedCols.constBegin();
            it != band->m_nestedCols.constEnd(); ++it)
        {
            GridBandedTableColumn* column = *it;
            if (!isColumnVisible(column))
                continue;

            int curLength = verticalLayout ? column->columnGP()->width() : column->columnGP()->height();

            if (startColumn == column)
            {
                startColumn = Q_NULL;
                skip = false;
            }

            if (!skip)
            {
                if (offset == -1)
                {
                    if (verticalLayout)
                    {
                        offset = column->columnGP()->top();
                        startPos = column->columnGP()->left();
                    }
                    else
                    {
                        offset = column->columnGP()->left();
                        startPos = column->columnGP()->top();
                    }
                }
                int curOffset = verticalLayout ? column->columnGP()->top() : column->columnGP()->left();
                if (curOffset > offset)
                {
                    pageLength += length;
                    if (verticalLayout)
                        painter.translate(-length, curOffset - offset);
                    else
                        painter.translate(curOffset - offset, -length);

                    offset = curOffset;
      
                    if (verticalLayout)
                    {
                        length = column->columnGP()->left() - startPos;
                        painter.translate(length, 0);
                    }
                    else
                    {
                        length = column->columnGP()->top() - startPos;
                        painter.translate(0, length);
                    }
                }
                
                if (pageLength > 0)
                    column->columnGP()->print(painter);

                pageLength -= curLength;

                if (!exit && pageLength < 0)
                {
                    exitPageLength = pageLength + curLength;
                    startColumn = column;
                    exit = true;
                    if (origPageLength < curLength)
                    {
                        GridDataTableColumn* next = Q_NULL;
                        for (next = getNextColumn(startColumn); next != Q_NULL; next = getNextColumn(next))
                        {
                            if (verticalLayout)
                            {
                                if (static_cast<GridBandedTableColumn *>(next)->columnGP()->top() == startColumn->columnGP()->top())
                                    break;
                            }
                            else
                            {
                                if (static_cast<GridBandedTableColumn *>(next)->columnGP()->left() == startColumn->columnGP()->left())
                                    break;
                            }
                        }
                        startColumn = static_cast<GridBandedTableColumn *>(next);
                    }
                }
            }
            if (verticalLayout)
                painter.translate(curLength, 0);
            else
                painter.translate(0, curLength);
            length += curLength;
        }

        if (exit)
        {
            pageLength = exitPageLength;
            painter.restore();
            return false;
        }
    }
    else
    {
        for (GridTableBands::const_iterator it = band->m_nestedBands.constBegin();
            it != band->m_nestedBands.constEnd(); ++it)
        {
            if (!isColumnVisible(*it))
                continue;

            if (!printBandHeader(painter, pageLength, origPageLength, *it, startColumn))
            {
                painter.restore();
                return false;
            }
            if (verticalLayout)
                painter.translate((*it)->columnGP()->width(), 0);
            else
                painter.translate(0, (*it)->columnGP()->height());
        }
    }

    painter.restore();
    return true;
}

bool GridBandedTableView::printHeader(QPainter& painter, int& pageLength, int& itemIndex)
{
    bool verticalLayout = layout()->orientation() == Qt::Vertical;

    painter.save();

    GridBandedTableHeaderGraphic* bandColumnHeader = ((GridBandedTableHeaderGraphic *)layout()->columnHeader());

    GridBandedTableColumn* bandedStartColumn = itemIndex == -1 ? Q_NULL : static_cast<GridBandedTableColumn *>(getColumn(itemIndex));
    bool retval = true;

    GridTableBand* toplevelBand = Q_NULL;
    if (bandedStartColumn != Q_NULL)
        toplevelBand = getTopLevelBand(bandedStartColumn);

    bool skip = toplevelBand != Q_NULL;

    if (bandedStartColumn == Q_NULL)
    {
        bandColumnHeader->quickBandButton()->print(painter);
        if (verticalLayout)
            painter.translate(0, bandColumnHeader->quickBandButton()->height());
        else
            painter.translate(bandColumnHeader->quickBandButton()->width(), 0);

        bandColumnHeader->quickButton()->print(painter);

        if (verticalLayout)
            painter.translate(0, -bandColumnHeader->quickBandButton()->height());
        else
            painter.translate(-bandColumnHeader->quickBandButton()->width(), 0);

        if (verticalLayout)
        {
            int length = bandColumnHeader->quickBandButton()->width();
            painter.translate(length, 0);
            pageLength -= length;
        }
        else
        {
            int length = bandColumnHeader->quickBandButton()->height();
            painter.translate(0, length);
            pageLength -= length;
        }
    }
    else
    {
        if (verticalLayout)
            painter.translate(toplevelBand->columnGP()->left() - bandedStartColumn->columnGP()->left(), 0);
        else
            painter.translate(0, toplevelBand->columnGP()->top() - bandedStartColumn->columnGP()->top());
    }
    const int origPageLength = pageLength;

    for (GridTableBands::const_iterator it = m_rootBands.constBegin(); 
        it != m_rootBands.constEnd(); ++it)
    {
        if (!isColumnVisible(*it))
            continue;

        if (toplevelBand == *it)
            skip = false;

        if (!skip)
        {
            if (!printBandHeader(painter, pageLength, origPageLength, *it, bandedStartColumn))
            {
                retval = false;
                itemIndex = bandedStartColumn->index();
                break;
            }
            if (verticalLayout)
                painter.translate((*it)->columnGP()->width(), 0);
            else
                painter.translate(0, (*it)->columnGP()->height());
        }
    }

    painter.restore();

    if (verticalLayout)
        painter.translate(0, bandColumnHeader->quickBandButton()->height() + bandColumnHeader->quickButton()->height());
    else
        painter.translate(bandColumnHeader->quickBandButton()->width() + bandColumnHeader->quickButton()->width(), 0);

    //GridTableView::printHeader(painter, pageWidth, column);
    //if (!retval && toplevelBand)
    //{
    //    if (!toplevelBand->isBottomBand())
    //    {
    //        GridTableBands bottomBands;
    //        getBottomLevelBands(toplevelBand, bottomBands);
    //        if (bottomBands.size() > 0)
    //            toplevelBand = bottomBands[0];
    //        else
    //            toplevelBand = Q_NULL;
    //    }
    //    if (toplevelBand != Q_NULL)        
    //        startColumn = toplevelBand->getNextColumn(Q_NULL);
    //    else
    //        startColumn = Q_NULL;
    //    retval = startColumn == Q_NULL;
    //}
    return retval;
}

void GridBandedTableView::collectBandChilds(GridTableBand* band, GridColumns& cols, bool recursive) const
{
    Q_ASSERT(band != Q_NULL);
    for (GridTableBands::const_iterator it = band->m_nestedBands.constBegin();
        it != band->m_nestedBands.constEnd(); ++it)
    {
        cols.push_back(*it);
        if (recursive)
            collectBandChilds(*it, cols, recursive);
    }
    for (GridBandedTableColumns::const_iterator it = band->m_nestedCols.constBegin();
        it != band->m_nestedCols.constEnd(); ++it)
    {
        cols.push_back(*it);
    }
}

void GridBandedTableView::getFixedColumns(GridTableColumnBase*& left, 
                                             GridTableColumnBase*& right) const
{
    left = Q_NULL;
    right = Q_NULL;
    for (GridTableBands::const_iterator it = m_rootBands.constBegin(); 
        it != m_rootBands.constEnd(); ++it)
    {
        GridTableBand* band = *it;
        if (!isColumnVisible(band))
            continue;

        if (band->fixedKind() == FixedLeft)
            left = band;
        if (band->fixedKind() == FixedRight)
        {
            right = band;
            break;
        }
    }
}

bool GridBandedTableView::checkRemove(const GridColumnBase* columnBase) const
{
    const GridTableBand* band = qobject_cast<const GridTableBand *>(columnBase);
    if (band != Q_NULL)
        return options().isBandHidingEnabled() && band->isHidingEnabled();
    else
        return GridTableView::checkRemove(columnBase);
}

bool GridBandedTableView::checkMove(const GridColumnBase* columnBase) const
{
    const GridTableBand* band = qobject_cast<const GridTableBand *>(columnBase);
    if (band != Q_NULL)
    {
        if (!options().isLastColumnMovingEnabled() && band->isRootBand() && band->isVisible())
        {
            GridColumns bands;
            getColumnLine(band, bands);
            if (bands.size() == 1)
                return false;
        }
        return options().isBandMovingEnabled() && band->isMovingEnabled();
    }

    bool hack = options().isLastColumnMovingEnabled();
    options().setLastColumnMovingEnabled(true);
    bool ret = GridTableView::checkMove(columnBase);
    options().setLastColumnMovingEnabled(hack);
    return ret;
}

bool GridBandedTableView::checkHorSizing(const GridColumnBase* columnBase) const
{
    const GridTableBand* band = qobject_cast<const GridTableBand *>(columnBase);
    if (band != Q_NULL)
        return options().isBandHorSizingEnabled() && band->isHorSizingEnabled();
    else
        return GridTableView::checkHorSizing(columnBase);
}

bool GridBandedTableView::checkVerSizing(const GridColumnBase* columnBase) const
{
    const GridTableBand* band = qobject_cast<const GridTableBand *>(columnBase);
    if (band != Q_NULL)
        return false;
    else
    {
        const GridBandedTableColumn* column = qobject_cast<const GridBandedTableColumn *>(columnBase);
        if (column == Q_NULL)
            return false;
        return options().isColumnVerSizingEnabled() && column->isVerSizingEnabled();
    }
}

QFont GridBandedTableView::getColumnFont(const GridColumnBase* column) const
{
    if (qobject_cast<const GridTableBand *>(column) == Q_NULL)
        return GridTableView::getColumnFont(column);
    QFont ret = column->font().resolve(options().bandFont());
    ret.resolve(ret.resolve() | options().bandFont().resolve());
    return ret;
}

QPen GridBandedTableView::getColumnPen(const GridColumnBase* column) const
{
    if (qobject_cast<const GridTableBand *>(column) != Q_NULL)
        return options().bandPen();
    return GridTableView::getColumnPen(column);
}

int GridBandedTableView::getColumnHeight(const GridColumnBase* column) const
{
    if (qobject_cast<const GridTableBand *>(column) != Q_NULL)
        return options().bandHeight();
    return GridTableView::getColumnHeight(column);
}

void GridBandedTableView::columnWidthChanged(GridColumnBase* column)
{
    GridTableView::columnWidthChanged(column);

    GridTableBand* parentBand = get_parent_band((const GridTableColumnBase*)column);
    if (parentBand != Q_NULL && layout()->orientation() == Qt::Vertical)
    {
        int parentBandWidth = 0;
        GridColumns cols;
        getColumnLine(column, cols);
//        GridBandedTableHeaderGraphic* header = static_cast<GridBandedTableHeaderGraphic *>(static_cast<GridTableColumnBase *>(column)->columnGP()->parentGP());
//        header->adaptColumnsToView(cols, getColumnViewWidth(static_cast<GridTableColumnBase *>(column)));
        for (GridColumns::const_iterator it = cols.constBegin(); it != cols.constEnd(); ++it)
            parentBandWidth += ((GridTableColumnBase *)*it)->width();
        parentBand->setWidth(parentBandWidth);
    }
}

/*!
    \class GridTableBand
    \inmodule QtitanDataGrid
*/
GridTableBand::GridTableBand(GridBandedTableView* view, const QString& caption)
: GridTableColumnBase(view, caption), m_parentBand(Q_NULL)
{
}

GridTableBand::~GridTableBand()
{
}

void GridTableBand::addNestedColumn(GridBandedTableColumn* column)
{
    Q_ASSERT(column != Q_NULL);
    if (m_nestedCols.indexOf(column) == -1)
    {
        if (column->m_parentBand != Q_NULL)
            column->m_parentBand->removeNestedColumn(column);

        column->m_parentBand = this;
        m_nestedCols.push_back(column);
        view()->parentBandChanged(column);

        if (!view()->isLayoutLoading())
        {
            if (view()->layout()->orientation() == Qt::Vertical)
            {
                int w = this->exactWidth();
                if (m_nestedCols.size() > 1)
                    w = this->exactWidth() + column->exactWidth();
                else if (m_nestedCols.size() == 1)
                    w = qMax(w, column->exactWidth());

                GridTableBand* b = this;
                int delta = w - b->exactWidth();
                b->setExactWidth(w);
                b = get_parent_band(b);
                while (b)
                {
                    b->setExactWidth(b->exactWidth() + delta);
                    b = get_parent_band(b);
                }
            }
            else
            {
                for (int i = 0; i < m_nestedCols.size(); ++i)
                {
                    if (view()->isColumnVisible(m_nestedCols[i]))
                    {
                        column->setExactWidth(m_nestedCols[i]->exactWidth());
                        break;
                    }
                }
            }
        }

        view()->markColumnsChanged();
    }
}

void GridTableBand::removeNestedColumn(GridBandedTableColumn* column)
{
    Q_ASSERT(column != Q_NULL);
    if (m_nestedCols.removeAll(column) > 0)
    {
        Q_ASSERT(column->m_parentBand == this);
        column->m_parentBand = Q_NULL;
        if (!view()->isLayoutLoading())
        {
            if (view()->layout()->orientation() == Qt::Vertical)
            {
                int w = 0;
                if (m_nestedCols.size() > 0)
                    w = this->exactWidth() - column->exactWidth();
                else
                    w = this->exactWidth();

                GridTableBand* b = this;
                int delta = w - b->exactWidth();
                b->setExactWidth(w);
                b = get_parent_band(b);
                while (b)
                {
                    b->setExactWidth(b->exactWidth() + delta);
                    b = get_parent_band(b);
                }
            }
        }
        view()->markColumnsChanged();
    }
}

static bool is_circ_parent(GridTableBand* parent)
{
    QSet<GridTableBand*> parents;
    while (parent != Q_NULL)
    {
        if (parents.find(parent) != parents.end())
            return true;
        parents.insert(parent);
        parent = parent->parentBand();
    }
    return false;
}

void GridTableBand::addNestedBand(GridTableBand* band)
{
    Q_ASSERT(band != Q_NULL);
    if (m_nestedBands.indexOf(band) == -1)
    {
        bool descendant = isDescendant(band);
        int oldBandIndex = band->bandIndex();
        int oldBandVisulaIndex; 

        if (band->m_parentBand != Q_NULL)
            oldBandVisulaIndex = band->m_parentBand->removeNestedBand(band);
        else
            oldBandVisulaIndex = view()->removeNestedBand(band);

        band->m_parentBand = this;
        m_nestedBands.push_back(band);

        if (descendant)
        {
            GridTableBands bands = band->m_nestedBands;
            for (GridTableBands::const_iterator it = bands.constBegin(); it != bands.constEnd(); ++it)
            {
                if (is_circ_parent(*it) || isDescendant(*it))
                {
                    (*it)->setBandIndex(oldBandIndex);
                    (*it)->setVisualIndex(oldBandVisulaIndex);
                    oldBandVisulaIndex++;
                }
            }
        }

        moveColumnsToBottmLevel();
        view()->parentBandChanged(band);
    }
}

int GridTableBand::removeNestedBand(GridTableBand* band)
{
    int retval = m_nestedBands.indexOf(band);
    Q_ASSERT(band != Q_NULL && retval != -1);
    if (m_nestedBands.removeAll(band) > 0)
    {
        Q_ASSERT(band->m_parentBand == this);
        band->m_parentBand = Q_NULL;
    }
    return retval;
}

bool GridTableBand::isRootBand() const
{
    return parentBand() == Q_NULL;
}

bool GridTableBand::isBottomBand() const
{
    return m_nestedBands.size() == 0;
}

bool GridTableBand::isDescendant(GridTableBand* band) const
{
    const GridTableBand* parent = this;
    while (parent != Q_NULL)
    {
        if (band == parent)
            return true;
        parent = parent->parentBand();
        Q_ASSERT(parent != this);
    }
    return false;
}

GridTableBand* GridTableBand::parentBand() const
{
    return m_parentBand;
}

int GridTableBand::bandIndex() const
{
    return view()->getBandIndex(m_parentBand);
}

void GridTableBand::setBandIndex(int index)
{
    GridViewAutoUpdater autoUpdater(view());

    if (index == -1)
    {
        view()->addNestedBand(this);
        return;
    }

    GridTableBand* band = view()->getBand(index);
    if (band != Q_NULL)
        band->addNestedBand(this);
}

int GridTableBand::depth() const
{
    int retval = 0;
    GridTableBand* parentBand = get_parent_band(this);
    while (parentBand != Q_NULL)
    {
        retval++;
        parentBand = get_parent_band(parentBand);
    }
    return retval;
}

GridBandedTableColumn* GridTableBand::getNextColumn(const GridBandedTableColumn* anchor) const
{
    QListIterator<GridBandedTableColumn *> iterator(m_nestedCols);
    iterator.toFront();
    while(iterator.hasNext())
    {
        GridBandedTableColumn* base = iterator.next();
        if (!view()->isColumnVisible(base))
            continue;
       if (anchor == Q_NULL)
            return base;
        if (anchor == base)
        {
            while (iterator.hasNext())
            {
                base = iterator.next();
                if (!view()->isColumnVisible(base))
                    continue;
                return base;
            }
            return Q_NULL;
        }
    }
    return Q_NULL;
}

void GridTableBand::bestFit()
{
    columnGP()->bestFit();
}

GridBandedTableColumn* GridTableBand::getPrevColumn(const GridBandedTableColumn* anchor) const
{
    QListIterator<GridBandedTableColumn *> iterator(m_nestedCols);
    iterator.toBack();
    while(iterator.hasPrevious())
    {
        GridBandedTableColumn* base = iterator.previous();
        if (!view()->isColumnVisible(base))
            continue;
       if (anchor == Q_NULL)
            return base;
        if (anchor == base)
        {
            while (iterator.hasPrevious())
            {
                base = iterator.previous();
                if (!view()->isColumnVisible(base))
                    continue;
                return base;
            }
            return Q_NULL;
        }
    }
    return Q_NULL;
}

GridBandedTableView* GridTableBand::view() const
{
    return static_cast<GridBandedTableView *>(viewBase());
}

void GridTableBand::click(QMouseEvent* e)
{
    Q_UNUSED(e);
}

GridTableDragColumnObject* GridTableBand::createDragDropColumnObject(const QPoint& point, GraphicPrimitive* graphic)
{
    return new GridBandedTableDragColumnObject(point, this, graphic);
}

bool GridTableBand::needHeaderHints() const
{
    return view()->options().bandHeaderHints();
}

GridTableColumnPrimitive* GridTableBand::columnGP() const
{
    return view()->layout()->columnHeader()->getPrimitiveByColumn(this);
}

void GridTableBand::initStyleOption(QStyleOptionHeader& styleOption)
{
    styleOption.init(view()->grid());
    styleOption.state = QStyle::State_Active;
    if (view()->grid()->isEnabled())
        styleOption.state |= QStyle::State_Enabled;

    styleOption.orientation = Qt::Horizontal;
    styleOption.icon = icon();
    styleOption.textAlignment = textAlignment();
    styleOption.sortIndicator = QStyleOptionHeader::None;
    if (columnGP() != Q_NULL)
    {
        //if (graphic()->isDragging())
        if (columnGP()->control()->dragDropState() == dragActive)
        {
            styleOption.position = QStyleOptionHeader::OnlyOneSection;
            styleOption.state |= QStyle::State_Raised;
        }
        else
        {
            styleOption.selectedPosition = QStyleOptionHeader::NotAdjacent;
            
            if (columnGP()->isMouseOver())
            {
                styleOption.state |= QStyle::State_MouseOver;
            }

            if (columnGP()->isLeftButtonPressed())
            {
                styleOption.state |= QStyle::State_Sunken;
            }
            else
            {
                styleOption.state |= QStyle::State_Raised;
            }
        }
        styleOption.rect = QRect(QPoint(0, 0), columnGP()->boundedRect().size());
    }
    else
        styleOption.rect = QRect(0, 0, 30, 24);

    styleOption.text = caption();
}

/*
void GridTableBand::updateFixedState()
{
    GridViewBase::GridColumns cols;
    cols.push_back(this);
    view()->collectBandChilds(this, cols, true);
    for (GridViewBase::GridColumns::const_iterator it = cols.constBegin();
        it != cols.constEnd(); ++it)
    {
        if (static_cast<GridTableColumnBasePublic *>(*it)->columnGP() != Q_NULL)
        {
            view()->m_rowsLayout->updateGraphicFixedState((GridTableColumn *)*it);
            //((GridTableFooterGraphic *)view()->footerPanelGraphic())->summaryGraphic()->updateGraphicFixedState((GridTableColumn *)*it); TODO
        }
    }
}
*/
void GridTableBand::moveColumnsToBottmLevel()
{
    if (isBottomBand())
        return;
    GridTableBands bottomLevelBands;
    view()->getBottomLevelBands(this, bottomLevelBands);
    if (bottomLevelBands.size() > 0)
    {
        GridTableBand* band = bottomLevelBands[0];
        {
            GridViewAutoUpdater autoUpdater(view());
            GridBandedTableColumns nestedCols = m_nestedCols;
            for (GridBandedTableColumns::const_iterator it = nestedCols.constBegin();
                it != nestedCols.constEnd(); ++it)
                band->addNestedColumn(*it);
        }
    }
}

/*!
    \class GridBandedTableColumn
    \inmodule QtitanDataGrid
*/
GridBandedTableColumn::GridBandedTableColumn(GridBandedTableView* view, int modelColumn, const QString& caption)
    : GridTableColumn(view, modelColumn, caption), m_row(0), m_parentBand(Q_NULL), m_isVerSizingEnabled(true)
{
}

GridBandedTableColumn::~GridBandedTableColumn()
{ 
    setBandIndex(-1);
}

int GridBandedTableColumn::bandIndex() const
{
    return view()->getBandIndex(m_parentBand);
}

void GridBandedTableColumn::setBandIndex(int index)
{
    GridTableBand* currentBand = parentBand();
    GridTableBand* band = view()->getBand(index);
    if (band != Q_NULL && !band->isBottomBand())
    {
        GridTableBands bands;
        view()->getBottomLevelBands(band, bands);
        if (bands.size() == 0)
            return;
        band = bands[0];
    }
    if (band == currentBand)
        return;
    GridViewAutoUpdater autoUpdater(view());
    if (band != Q_NULL)
        band->addNestedColumn(this);
    else if (currentBand != Q_NULL)
    {
        currentBand->removeNestedColumn(this);
        setVisible(false);
    }
    if (currentBand != Q_NULL)
        view()->ensureRowValid(currentBand, rowIndex());
}

int GridBandedTableColumn::rowIndex() const
{
    Q_ASSERT(view() != Q_NULL);
    return view()->getColumnRowIndex(this);
}

void GridBandedTableColumn::setRowIndex(int index)
{
    Q_ASSERT(view() != Q_NULL);
    view()->setColumnRowIndex(this, index);
}

GridTableBand* GridBandedTableColumn::parentBand() const
{
    return m_parentBand;
}

bool GridBandedTableColumn::isDescendant(GridTableBand* band) const
{
    const GridTableBand* parent = m_parentBand;
    while (parent != Q_NULL)
    {
        if (band == parent)
            return true;
        parent = parent->parentBand();
        Q_ASSERT(parent != m_parentBand);
    }
    return false;
}

GridTableDragColumnObject* GridBandedTableColumn::createDragDropColumnObject(const QPoint& point, GraphicPrimitive* graphic)
{
    return new GridBandedTableDragColumnObject(point, this, graphic);
}

bool GridBandedTableColumn::isVerSizingEnabled() const
{
    return m_isVerSizingEnabled;
}

void GridBandedTableColumn::setVerSizingEnabled(bool enabled)
{
    m_isVerSizingEnabled = enabled;
}

GridBandedTableView* GridBandedTableColumn::view() const
{
    return static_cast<GridBandedTableView *>(viewBase());
}

/* GridBandedTableViewOptions */
GridBandedTableViewOptions::GridBandedTableViewOptions(GridBandedTableView* view)
: GridTableViewOptions(view), m_bandsHeader(true), m_bandsQuickMenuVisible(true), m_bandsQuickCustomization(true),
  m_bandHeaderHints(true), m_bandsDropDownCount(0), m_isBandHidingEnabled(true), m_isBandMovingEnabled(true), 
  m_changeNestedBandOnMoving(true), m_changeColumnRowOnMoving(true), m_isColumnVerSizingEnabled(true),
  m_isBandHorSizingEnabled(true), m_bandHeight(-1), m_ignoreRowSpanForCells(false)
{
    QFont f = columnFont();
    f.setBold(true);
    m_bandFont = f;
}

bool GridBandedTableViewOptions::bandsHeader() const
{
    return m_bandsHeader;
}

void GridBandedTableViewOptions::setBandsHeader(bool bandsHeader)
{
    m_bandsHeader = bandsHeader;
    view()->requestUpdateLayout();
}

bool GridBandedTableViewOptions::bandsQuickMenuVisible() const
{
    return m_bandsQuickMenuVisible;
}

void GridBandedTableViewOptions::setBandsQuickMenuVisible(bool quickMenuVisible)
{
    m_bandsQuickMenuVisible = quickMenuVisible;
}

bool GridBandedTableViewOptions::bandsQuickCustomization() const
{
    return m_bandsQuickCustomization;
}

void GridBandedTableViewOptions::setBandsQuickCustomization(bool quickCustomization)
{
    m_bandsQuickCustomization = quickCustomization;
    GridBandedTableView* bandedView = static_cast<GridBandedTableView *>(view());
    bandedView->requestUpdateLayout();
}

int GridBandedTableViewOptions::bandsQuickCustomizationMaxDropDownCount() const
{
    return m_bandsDropDownCount;
}

void GridBandedTableViewOptions::setBandsQuickCustomizationMaxDropDownCount(int dropDownCount)
{
    m_bandsDropDownCount = dropDownCount;
    GridBandedTableView* bandedView = static_cast<GridBandedTableView *>(view());
    ((GridBandedTableHeaderGraphic *)bandedView->layout()->columnHeader())->quickBandButton()->setDropDownCount(dropDownCount);
}

bool GridBandedTableViewOptions::bandHeaderHints() const
{
    return m_bandHeaderHints;
}

void GridBandedTableViewOptions::setBandHeaderHints(bool headerHints)
{
    m_bandHeaderHints = headerHints;
}

bool GridBandedTableViewOptions::isBandHidingEnabled() const
{
    return m_isBandHidingEnabled;
}

void GridBandedTableViewOptions::setBandHidingEnabled(bool enabled)
{
    m_isBandHidingEnabled = enabled;
}

bool GridBandedTableViewOptions::isBandMovingEnabled() const
{
    return m_isBandMovingEnabled;
}

void GridBandedTableViewOptions::setBandMovingEnabled(bool enabled)
{
    m_isBandMovingEnabled = enabled;
}

bool GridBandedTableViewOptions::changeNestedBandOnMoving() const
{
    return m_changeNestedBandOnMoving;
}

void GridBandedTableViewOptions::setChangeNestedBandOnMoving(bool changeNestedBandOnMoving)
{
    m_changeNestedBandOnMoving = changeNestedBandOnMoving;
}

bool GridBandedTableViewOptions::changeColumnRowOnMoving() const
{
    return m_changeColumnRowOnMoving;
}

void GridBandedTableViewOptions::setChangeColumnRowOnMoving(bool changeColumnRowOnMoving)
{
    m_changeColumnRowOnMoving = changeColumnRowOnMoving;
}

bool GridBandedTableViewOptions::isColumnVerSizingEnabled() const
{
    return m_isColumnVerSizingEnabled;
}

void GridBandedTableViewOptions::setColumnVerSizingEnabled(bool enabled)
{
    m_isColumnVerSizingEnabled = enabled;
}

bool GridBandedTableViewOptions::isBandHorSizingEnabled() const
{
    return m_isBandHorSizingEnabled;
}

void GridBandedTableViewOptions::setBandHorSizingEnabled(bool enabled)
{
    m_isBandHorSizingEnabled = enabled;
}

const QFont& GridBandedTableViewOptions::bandFont() const
{
    return m_bandFont;
}

void GridBandedTableViewOptions::setBandFont(const QFont& font)
{
    m_bandFont = font;
    //m_bandHeight = get_column_sizeHint(view()->grid()->style(), m_bandFont).height();
    view()->requestUpdateLayout();
}

const QPen& GridBandedTableViewOptions::bandPen() const
{
    if (m_bandPen != QPen())
        return m_bandPen;
    return columnPen();
}

void GridBandedTableViewOptions::setBandPen(const QPen& pen)
{
    m_bandPen = pen;
}

int GridBandedTableViewOptions::bandHeight() const
{
    return m_bandHeight;
}

void GridBandedTableViewOptions::setBandHeight(int height)
{
    m_bandHeight = height;
    view()->requestUpdateLayout();
}

bool GridBandedTableViewOptions::ignoreRowSpanForCells() const
{
    return m_ignoreRowSpanForCells;
}

void GridBandedTableViewOptions::setIgnoreRowSpanForCells(bool ignore)
{
    m_ignoreRowSpanForCells = ignore;
    view()->requestUpdateLayout();
}

