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

#ifndef QTN_GRIDBANDEDTABLEVIEW_H
#define QTN_GRIDBANDEDTABLEVIEW_H

#include "QtnGrid.h"
#include "QtnGridTableView.h"

QTITAN_BEGIN_NAMESPACE

class GridBandedTableView;
class GridTableBand;
class GridBandedTableColumn;
class GridBandedTableViewOptions;


class GridBandedTableDragColumnObject: public GridTableDragColumnObject
{
public:
    GridBandedTableDragColumnObject(const QPoint& point, GridTableColumnBase* column, GraphicPrimitive* source);
    virtual bool checkDropColumnTarget(const QPoint& pos, GridColumnPrimitive* target);
    virtual void clear();
    int sourceBandIndex() const;
    void setSourceBandIndex(int bandIndex);
    int sourceRowIndex() const;
    void setSourceRowIndex(int rowIndex);
    int insertedRowIndex() const;
    void setInsertedRowIndex(int index);
    ColumnMovePosition bandMovePosition() const;
    void setBandMovePosition(ColumnMovePosition position);
protected:
    virtual void acceptEvent();
    void showMoveColumnRowArrows(GridColumnPrimitive* target, ColumnMovePosition position);
private:
    int m_sourceBandIndex;
    int m_sourceRowIndex;
    int m_insertedRowIndex;
    ColumnMovePosition m_bandMovePosition;
};

typedef QList<GridTableBand *> GridTableBands;
typedef QList<GridBandedTableColumn *> GridBandedTableColumns;

class QTITAN_EXPORT GridBandedTableView: public GridTableView
{
    Q_OBJECT
public:
    friend class GridTableBandPrimitive;
    friend class GridBandedTableColumn;
    friend class GridTableBand;
    friend class GridBandedTableDragColumnObject;
    friend class GridBandedTableViewOptions;
    friend class GridBandedTableHeaderGraphic;
    GridBandedTableColumn* addColumn(const QString& modelColumnName, const QString& caption = QString());
    GridBandedTableColumn* addColumn(int modelColumn, const QString& caption = QString());
    GridTableBand* addBand(const QString& caption);
    int bandCount() const;
    GridTableBand* getBand(int index) const;
    void removeBand(int index);
    void removeBands();
    static GridBandedTableView* create(Grid* grid);
public:
    GridBandedTableView(Grid* grid);
    virtual ~GridBandedTableView();
    virtual void initialize();
    virtual void addColumns();
    virtual void bestFitForView();
    int columnRowCount(GridTableBand* band) const;
    void getBottomLevelBands(GridTableBand* band, GridTableBands& bands) const;
    GridTableBand* getTopLevelBand(const GridTableColumnBase* columnBase) const;
    void createColumnRow(int row, GridBandedTableColumn* column);
    void createBandRow(GridTableBand* band, int bandIndex, ColumnMovePosition position);
    GridBandedTableViewOptions& options() const;
    virtual void navigateUp(Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    virtual void navigateDown(Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    virtual GridDataTableColumn* getNextColumn(const GridDataTableColumn* anchor) const;
    virtual GridDataTableColumn* getPrevColumn(const GridDataTableColumn* anchor) const;
    virtual bool hasVisibleParent(const GridColumnBase* column) const;
protected:
    virtual void setModelToController(QAbstractItemModel* model, const QModelIndex& parent);
#ifndef QTN_NOUSE_XML_MODULE
    virtual bool saveLayoutToStream(QXmlStreamWriter* xmlwriter);
    virtual bool loadLayoutFromStream(QXmlStreamReader* xmlreader);
    virtual bool saveTableColumnLayout(const GridDataTableColumn* column, QXmlStreamWriter* xmlwriter);
    virtual bool loadTableColumnLayout(GridDataTableColumn* column, QXmlStreamReader* xmlreader);
#endif
    virtual int getColumnRowIndex(const GridDataTableColumn* column) const;
    void setColumnRowIndex(GridBandedTableColumn* column, int rowIndex);
    void ensureRowValid(GridTableBand* band, int row);
    virtual int getVisualIndex(const GridColumnBase* columnBase) const;
    virtual void setVisualIndex(const GridColumnBase* tableColumn, int visualIndex);
    virtual GridColumnBase* createColumn(int modelColumn, const QString& caption);
    virtual int getColumnIndex(const GridColumnBase* columnBase) const;
    virtual GridTableColumnBase* getPreviousResizeColumn(const GridTableColumnBase* columnBase) const;
    virtual GridTableColumnBase* getNextResizeColumn(const GridTableColumnBase* columnBase) const;
    virtual bool isVerticalResizable(const GridTableColumnBase* columnBase) const;
    virtual bool isHorizontalResizable(const GridTableColumnBase* columnBase) const;
    virtual ColumnFixedKind getColumnFixedKind(const GridTableColumnBase* columnBase) const;
    virtual void setColumnFixedKind(GridTableColumnBase* columnBase, ColumnFixedKind kind);
    virtual void getColumnLine(const GridColumnBase* column, GridColumns& retval) const;
    virtual int getMaximumVisualIndex(const GridTableColumnBase* anchor) const;
    virtual void columnWidthChanged(GridColumnBase* column);
    virtual int getColumnViewWidth(const GridTableColumnBase* column);
    virtual GridViewOptions* createOptions();
    virtual GridTableHeaderGraphic* createHeaderPanel(GraphicPrimitive* parent);
    virtual void columnVisibleChanged(GridColumnBase* columnBase, bool visible);
    virtual void getFixedColumns(GridTableColumnBase*& left, GridTableColumnBase*& right) const;
    virtual bool checkRemove(const GridColumnBase* columnBase) const;
    virtual bool checkMove(const GridColumnBase* columnBase) const;
    virtual bool checkHorSizing(const GridColumnBase* columnBase) const;
    virtual bool checkVerSizing(const GridColumnBase* columnBase) const;
    virtual QFont getColumnFont(const GridColumnBase* column) const;
    virtual QPen getColumnPen(const GridColumnBase* column) const;
    virtual int getColumnHeight(const GridColumnBase* column) const;
    int getBandIndex(const GridTableBand* band) const;
    void addNestedBand(GridTableBand* band);
    int removeNestedBand(GridTableBand* band);
    void parentBandChanged(GridTableBand* band);
    void parentBandChanged(GridBandedTableColumn* column);
    const GridTableBands& bands() const;
protected:
    virtual GridViewPrimitiveBase* createLayout();
    virtual bool hasGroupingIndent(const GridColumnBase* column) const;
protected:
    bool printHeader(QPainter& painter, int& pageLength, int& itemIndex);
    bool printBandHeader(QPainter& painter, int& pageLength, const int origPageLength, GridTableBand* band, GridBandedTableColumn* &startColumn);
private:
    void collectBandChilds(GridTableBand* band, GridColumns& cols, bool recursive) const;
    GridTableBands m_bandslist;
    GridTableBands m_rootBands;
};

class QTITAN_EXPORT GridTableBand: public GridTableColumnBase
{
    Q_OBJECT
public:
    friend class GridBandedTableView;
    friend class GridBandedTableColumn;
    friend class GridBandedTableHeaderGraphic;
    friend class GridTableHeaderGraphic;
    friend class GridBandedTableDragColumnObject;
    friend class GridBandedTableViewOptions;
    friend class GridTableBandPrimitive;
    GridTableBand(GridBandedTableView* view, const QString& caption);
    virtual ~GridTableBand();
    bool isRootBand() const;
    bool isBottomBand() const;
    bool isDescendant(GridTableBand* band) const;
    GridTableBand* parentBand() const;
    int bandIndex() const;
    void setBandIndex(int index);
    int depth() const;
    virtual GridBandedTableColumn* getNextColumn(const GridBandedTableColumn* anchor) const;
    virtual GridBandedTableColumn* getPrevColumn(const GridBandedTableColumn* anchor) const;
    GridBandedTableView* view() const;
public Q_SLOTS:
    void bestFit();
protected:
    virtual void click(QMouseEvent* e);
    virtual GridTableDragColumnObject* createDragDropColumnObject(const QPoint& point, GraphicPrimitive* graphic);
    virtual bool needHeaderHints() const;
    virtual void initStyleOption(QStyleOptionHeader& styleOption);
    void addNestedColumn(GridBandedTableColumn* column);
    void removeNestedColumn(GridBandedTableColumn* column);
    void addNestedBand(GridTableBand* band);
    int removeNestedBand(GridTableBand* band);
    bool hasChild(GridTableColumnBase* column);
    GridTableColumnPrimitive* columnGP() const;
private:
    void moveColumnsToBottmLevel();
    GridBandedTableColumns m_nestedCols;
    GridTableBands m_nestedBands;
    GridTableBand* m_parentBand;
};

class QTITAN_EXPORT GridBandedTableColumn: public GridTableColumn
{
    Q_OBJECT
    friend class GridBandedTableView;
    friend class GridTableBandPrimitive;
    friend class GridTableBand;
    friend class GridBandedTableDragColumnObject;
    friend class GridBandedTableHeaderGraphic;
public:
    int bandIndex() const;
    void setBandIndex(int index);
    int rowIndex() const;
    void setRowIndex(int index);
    GridTableBand* parentBand() const;
    bool isDescendant(GridTableBand* band) const;
    bool isVerSizingEnabled() const;
    void setVerSizingEnabled(bool enabled);
    GridBandedTableView* view() const;
protected:
    GridBandedTableColumn(GridBandedTableView* view, int modelColumn, const QString& caption);
    virtual ~GridBandedTableColumn();
    virtual GridTableDragColumnObject* createDragDropColumnObject(const QPoint& point, GraphicPrimitive* graphic);
private:
    int m_row;
    GridTableBand* m_parentBand;
    bool m_isVerSizingEnabled;
};

class QTITAN_EXPORT GridBandedTableViewOptions: public GridTableViewOptions
{
protected:
    friend class GridBandedTableView;
    GridBandedTableViewOptions(GridBandedTableView* view);
public:
    bool bandsHeader() const;
    void setBandsHeader(bool bandsHeader);
    bool bandsQuickMenuVisible() const;
    void setBandsQuickMenuVisible(bool quickMenuVisible);
    bool bandsQuickCustomization() const;
    void setBandsQuickCustomization(bool quickCustomization);
    int bandsQuickCustomizationMaxDropDownCount() const;
    void setBandsQuickCustomizationMaxDropDownCount(int dropDownCount);
    bool bandHeaderHints() const;
    void setBandHeaderHints(bool headerHints);
    bool isBandHidingEnabled() const;
    void setBandHidingEnabled(bool enabled);
    bool isBandMovingEnabled() const;
    void setBandMovingEnabled(bool enabled);
    bool changeNestedBandOnMoving() const;
    void setChangeNestedBandOnMoving(bool changeNestedBandOnMoving);
    bool changeColumnRowOnMoving() const;
    void setChangeColumnRowOnMoving(bool changeColumnRowOnMoving);
    bool isColumnVerSizingEnabled() const;
    void setColumnVerSizingEnabled(bool enabled);
    bool isBandHorSizingEnabled() const;
    void setBandHorSizingEnabled(bool enabled);
    const QFont& bandFont() const;
    void setBandFont(const QFont& font);
    const QPen& bandPen() const;
    void setBandPen(const QPen& pen);
    int bandHeight() const;
    void setBandHeight(int height);
    bool ignoreRowSpanForCells() const;
    void setIgnoreRowSpanForCells(bool ignore);
private:
    bool m_bandsHeader;
    bool m_bandsQuickMenuVisible;
    bool m_bandsQuickCustomization;
    bool m_bandHeaderHints;
    int m_bandsDropDownCount;
    bool m_isBandHidingEnabled;
    bool m_isBandMovingEnabled;
    bool m_changeNestedBandOnMoving;
    bool m_changeColumnRowOnMoving;
    bool m_isColumnVerSizingEnabled;
    bool m_isBandHorSizingEnabled;
    QPen m_bandPen;
    QFont m_bandFont;
    int m_bandHeight;
    bool m_ignoreRowSpanForCells;
};

GridTableBand* get_parent_band(const GridColumnBase* columnBase);

QTITAN_END_NAMESPACE

#endif //QTN_GRIDBANDEDTABLEVIEW_H

