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

#include <QVBoxLayout>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QToolTip>
#include <QBitmap>

#include "QtnGridFilterWindow.h"
#include "QtnGridViewGraphics.h"

QTITAN_USE_NAMESPACE

QTITAN_BEGIN_NAMESPACE

class FilterValueModel: public QAbstractItemModel
{
public:
    friend class FilterPopupWidget;
    friend class GridFilterColumnPopup;
    FilterValueModel(QObject *parent) 
        : QAbstractItemModel(parent)
    {
    } 

    QVariant headerData(int section, Qt::Orientation orientation, int role) const
    {
        Q_UNUSED(orientation);
        Q_UNUSED(role);
        switch (section)
        {
        case 0:
            return QString("Boolean");
            break;
        case 1:
            return QString("Integer");
            break;
        default:
            break;
        }
        return QVariant();
    }

    QModelIndex parent(const QModelIndex & /*child*/) const 
    {
        return QModelIndex();
    } 

    int rowCount(const QModelIndex &parent) const 
    {
        if (parent.isValid())
            return 0;
        return m_values.size();
    } 

    int columnCount(const QModelIndex &parent) const 
    {
        if (parent.isValid())
            return 0;
        return 2;
    }

    QModelIndex index(int row, int column, const QModelIndex &parent) const 
    {
        if (parent.isValid())                                   
            return QModelIndex();

        if (row < 0 || row >= rowCount(parent))
            return QModelIndex();

        if (column < 0 || column >= columnCount(parent)) 
            return QModelIndex();

        return createIndex(row, column, (void*)NULL);
    }

    QVariant data(const QModelIndex &index, int role) const 
    {  
        if (!index.isValid())
            return QVariant();
            
        if (index.row() < 0 || index.row() >= rowCount(index.parent()))
            return QVariant();
            
        if (index.column() < 0 || index.column() >= columnCount(index.parent()))
            return QVariant();
            
        if (role == Qt::DisplayRole || role == Qt::EditRole)        
        {    
            switch (index.column())
            {
            case 0:
                return m_values[index.row()].active;
                break;
            case 1:
                return m_values[index.row()].title;
                break;
            }
        }
        else if (role == Qt::CheckStateRole)
        {
            switch (index.column())
            {
            case 0:
                return m_values[index.row()].active;
                break;
            }
        }
        return QVariant();
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (!index.isValid())
            return false;
            
        if (index.row() < 0 || index.row() >= rowCount(index.parent()))
            return false;
            
        if (index.column() < 0 || index.column() >= columnCount(index.parent()))
            return false;

        if (role != Qt::EditRole)
            return false;

        switch (index.column())
        {
        case 0:
            {
                m_values[index.row()].active = value.toBool();

                FilterPopupWidget* widget = qobject_cast<FilterPopupWidget *>(((QObject *)this)->parent());
                
                if (m_values[index.row()].active)
                    widget->getGridView()->selectRow(index.row());
                else
                    widget->getGridView()->selectRow(index.row(), Unselect);
            }
            break;
        case 1:
            m_values[index.row()].title = value.toString();
            break;
        default:
            break;
        }

        emit dataChanged(index, index);
        return true;
    }

    Qt::ItemFlags flags(const QModelIndex &index) const 
    {  
        if (!index.isValid())
            return 0;
        if (index.column() == 0)
            return Qt::ItemIsEnabled | Qt::ItemIsEditable;
        return Qt::ItemIsEnabled;
    }

    private:
    struct DataItem
    {
        bool active;
        QString title;
        QVariant value;
        DataItem() : active(false) {}
    };
    QVector<DataItem> m_values;
};

QTITAN_END_NAMESPACE

/*!
\class GridFilterWindow
\internal
*/
GridFilterWindow::GridFilterWindow(QWidget* parent, GridViewBase* view)
: GraphicControl(parent), m_viewBase(view)
{
    GridFilterArea* area = new GridFilterArea(this, view->modelController()->filter());
    setRoot(area);
}

GridFilterWindow::~GridFilterWindow()
{
    m_viewBase = Q_NULL;
}

/*!
\class GridFilterArea
\internal
*/
GridFilterArea::GridFilterArea(GridFilterWindow* window, GridFilter* filter)
: ScrollableGraphicPrimitive(window, Q_NULL), m_filter(filter)
{
}

GridFilterArea::~GridFilterArea()
{
    m_filter = Q_NULL;
}

/*!
\class GridFilterColumnPopup
\internal
*/
GridFilterColumnPopup::GridFilterColumnPopup(QWidget* parent, GridDataTableColumn* column)
: EditorPopup(parent, true, true, false), m_view(column->viewBase()), m_column(column)
{
    FilterPopupWidget* w = new FilterPopupWidget(this, column);
    setCentralWidget(w);
    layout()->setContentsMargins(5, 10, 5, 5);
    connect(w->getGridView(), SIGNAL(selectionChanged(GridSelection *, GridSelection *)), this, SLOT(selectionChanged(GridSelection *, GridSelection *)));
}

void GridFilterColumnPopup::selectionChanged(GridSelection* selection, GridSelection* oldSelection)
{
    Q_UNUSED(oldSelection);
    okButton()->setEnabled(!selection->end());
}

void GridFilterColumnPopup::setCustomFilter()
{
    GridViewAutoUpdater autoUpdater(m_view);
    bool init = false;

    FilterValueModel* model = static_cast<FilterValueModel *>(static_cast<FilterPopupWidget *>(centralWidget())->getGridView()->model());
    for (QVector<FilterValueModel::DataItem>::iterator it = model->m_values.begin(); it != model->m_values.end(); ++it)
    {
        if (it->active == true)
        {
            if (!init)
            {
                m_view->filter()->clear();
                if (m_column->m_filterCondition == Q_NULL)
                    m_column->m_filterCondition = new GridFilterValueCondition(m_view->filter());
                m_column->m_filterCondition->clearValues();
                m_column->m_filterCondition->setDataBinding(m_column->dataBinding());
                m_column->m_filterCondition->setModelRole(m_column->dataBinding()->displayRole());
                m_column->m_filterCondition->setValueOperator(GridFilterCondition::FilterEqual);
                init = true;
            }
            m_column->m_filterCondition->addValue((*it).value);
        }
    }
    
    if (!init)
        return;

    m_view->showFilterPanel();
    m_view->buildColumnFilters();
    if (!m_view->filter()->isActive())
        m_view->filter()->setActive(true);
    else
        m_view->modelController()->refresh();
}

void GridFilterColumnPopup::setBlanksFilter()
{
    GridViewAutoUpdater autoUpdater(m_view);
    m_view->filter()->clear();
    if (m_column->m_filterCondition == Q_NULL)
        m_column->m_filterCondition = new GridFilterValueCondition(m_view->filter());
    m_column->m_filterCondition->clearValues();
    m_column->m_filterCondition->setDataBinding(m_column->dataBinding());
    m_column->m_filterCondition->setModelRole(m_column->dataBinding()->displayRole());
    m_column->m_filterCondition->setValueOperator(GridFilterCondition::FilterEqual);
    m_column->m_filterCondition->addValue(QVariant());
    m_view->showFilterPanel();
    m_view->buildColumnFilters();
    m_view->filter()->setActive(true);
}

void GridFilterColumnPopup::setNoBlanksFilter()
{
    GridViewAutoUpdater autoUpdater(m_view);
    m_view->filter()->clear();
    if (m_column->m_filterCondition == Q_NULL)
        m_column->m_filterCondition = new GridFilterValueCondition(m_view->filter());
    m_column->m_filterCondition->clearValues();
    m_column->m_filterCondition->setDataBinding(m_column->dataBinding());
    m_column->m_filterCondition->setModelRole(m_column->dataBinding()->displayRole());
    m_column->m_filterCondition->setValueOperator(GridFilterCondition::FilterNotEqual);
    m_column->m_filterCondition->addValue(QVariant());
    m_view->showFilterPanel();
    m_view->buildColumnFilters();
    m_view->filter()->setActive(true);
}

int GridFilterColumnPopup::heightHint()
{
    Qtitan::GridViewBase* view = static_cast<FilterPopupWidget *>(centralWidget())->getGridView();
    return qMin(view->layout()->getModelHeight() + 180, 350);
}

void GridFilterColumnPopup::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_bg);
}

void GridFilterColumnPopup::resizeEvent(QResizeEvent *)
{
    updateMask();
}

void GridFilterColumnPopup::updateMask()
{
    m_bg = QPixmap(width(), height());
    m_bg.fill(Qt::transparent);
    {
        QPainter painter(&m_bg);
        painter.setBrush(QBrush(m_view->grid()->themeManager()->shadeBackground()));
        painter.setPen(m_view->grid()->themeManager()->shadePalette().light().color());
        painter.setRenderHint(QPainter::Antialiasing, true);

        QRect r = QRect(3, 3, width() - 6, height() - 6);
        QPainterPath roundRectPath;
        const int radius = 3;
        const int indent = 10;

        roundRectPath.moveTo(r.left() + r.width() - radius, r.top() + indent); //Right-top
        roundRectPath.arcTo(r.left() + r.width() - radius * 2, r.top() + indent, radius * 2, radius * 2, 90, -90);
        roundRectPath.lineTo(r.left() + r.width(), r.top() + r.height() - radius); //Right-bottom
        roundRectPath.arcTo(r.left() + r.width() - radius * 2, r.top() + r.height() - radius * 2, radius * 2, radius * 2, 0, -90);

        roundRectPath.lineTo(r.left() + radius, r.top() + r.height()); //Left-bottom
        roundRectPath.arcTo(r.left(), r.top() + r.height() - radius * 2, radius * 2, radius * 2, -90, -90);

        roundRectPath.lineTo(r.left(), r.top() + indent + radius); //Left-top
        roundRectPath.arcTo(r.left(), r.top() + indent, radius * 2, radius * 2, -180, -90);


        roundRectPath.lineTo(r.left() + radius + (r.width() - 2 * radius) / 2 - indent * 0.75, r.top() + indent);//Indent
        roundRectPath.lineTo(r.left() + radius + (r.width() - 2 * radius) / 2, r.top()); //Indent
        roundRectPath.lineTo(r.left() + radius + (r.width() - 2 * radius) / 2 + indent * 0.75, r.top() + indent);
        roundRectPath.lineTo(r.left() + r.width() - radius, r.top() + indent);

        roundRectPath.closeSubpath();
        painter.drawPath(roundRectPath);
    }
    setMask(m_bg.createMaskFromColor(Qt::transparent));
}

void GridFilterColumnPopup::okClick()
{
    static_cast<FilterPopupWidget *>(centralWidget())->getGridView()->closeEditor();
    setCustomFilter();
    EditorPopup::okClick();
}

/*!
\class FilterPopupWidget
\internal
*/
FilterPopupWidget::FilterPopupWidget(QWidget* parent, GridDataTableColumn* column)
: QWidget(parent), m_column(column)
{
    m_header = new QLabel(this);
    m_header->sizeHint();
    m_header->resize(10, 100);
    m_valuesGrid = new Qtitan::Grid(this);
    m_valuesGrid->setViewType(Qtitan::Grid::TableView);
    Qtitan::GridTableView* view = m_valuesGrid->view<Qtitan::GridTableView>();
    view->options().setColumnAutoWidth(true);
    view->options().setColumnsQuickCustomization(false);
    view->options().setGroupingEnabled(false);
    view->options().setGroupsHeader(false);
    view->options().setColumnsHeader(false);
    view->options().setFocusFollowsMouse(true);
    view->options().setColumnHidingEnabled(false);
    view->options().setGridLines(LinesNone);
    view->options().setFilterEnabled(false);
    view->options().setBorderWidth(0);
    view->options().setBorderColor(Qt::gray);
    view->options().setAlternatingRowColors(false);
    view->options().setTransparentBackground(true);
    view->options().setBackgroundColor(Qt::transparent);
    view->options().setSelectionPolicy(SelectMultiRow);
    view->options().setMainMenuDisabled(true);
    view->options().setSelectionPolicy(SelectIgnore);
    view->options().setFindEnabled(false);
    QPalette p = view->options().cellPalette();
    p.setColor(QPalette::Highlight, Qt::transparent);
    QColor fc = qtn_get_thememanager(column->viewBase()->grid())->shadePalette().light().color();
    p.setColor(QPalette::Text, fc);
    
    view->options().setCellPalette(p);

 //   view->options().setCellHeight(view->getColumnHeight(0) - 4);
 
    view->setModel(createModel());

    GridTableColumn* tableColumn = (GridTableColumn *)view->getColumn(0);
    tableColumn->setHorSizingEnabled(false);
    tableColumn->setMinWidth(18);
    tableColumn->setMaxWidth(18);
    tableColumn->setEditorType(Qtitan::CheckBoxEditorType);
    static_cast<GridCheckBoxEditorRepository *>(tableColumn->editorRepository())->setNativeStyle(false);
    static_cast<GridCheckBoxEditorRepository *>(tableColumn->editorRepository())->setImmediatePost(true);
    tableColumn->editorRepository()->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    tableColumn = (GridTableColumn *)view->getColumn(1);
    tableColumn->setCaption(Grid::tr_compatible(QtnGridSetFilterTitleString));
    tableColumn->editorRepository()->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QVBoxLayout* l = new QVBoxLayout(this);

    l->setContentsMargins(10, 10, 10, 10);
    
    QLabel* label = new QLabel(this);
    QColor linkColor = fc;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QString linkColorName = linkColor.name(QColor::HexArgb);
#else
    QString linkColorName = "white";
#endif
    label->setText(QString("<a href=\"#\" style=\"color: %1;\">(%2)</a>").arg(linkColorName).arg(Grid::tr_compatible(QtnGridFilterBlanksString)));
    connect(label, SIGNAL(linkActivated(const QString&)), this, SLOT(selectBlanksClicked(const QString&)));   
    l->addWidget(label, 0);

    label = new QLabel(this);
    label->setText(QString("<a href=\"#\" style=\"color: %1;\">(%2)</a>").arg(linkColorName).arg(Grid::tr_compatible(QtnGridFilterNoBlanksString)));
    connect(label, SIGNAL(linkActivated(const QString&)), this, SLOT(selectNoBlanksClicked(const QString&)));
    l->addWidget(label, 0);

    label = new QLabel(this);
    label->setText(QString("<a href=\"#\" style=\"color: %1;\">(%2)</a>").arg(linkColorName).arg(Grid::tr_compatible(QtnGridFilterSelectAllString)));
    connect(label, SIGNAL(linkActivated(const QString&)), this, SLOT(selectClicked(const QString&)));
    l->addWidget(label, 0);

    label = new QLabel(this);
    label->setText(QString("<a href=\"#\" style=\"color: %1;\">(%2)</a>").arg(linkColorName).arg(Grid::tr_compatible(QtnGridFilterUnselectAllString)));
    connect(label, SIGNAL(linkActivated(const QString&)), this, SLOT(unselectClicked(const QString&)));
    l->addWidget(label, 0);

    l->addWidget(m_valuesGrid, 1);
    m_valuesGrid->setFocus();
}

void FilterPopupWidget::selectBlanksClicked(const QString& link)
{
    Q_UNUSED(link);
    static_cast<GridFilterColumnPopup *>(parent())->setBlanksFilter();
    static_cast<GridFilterColumnPopup *>(parent())->close();
}

void FilterPopupWidget::selectNoBlanksClicked(const QString& link)
{
    Q_UNUSED(link);
    static_cast<GridFilterColumnPopup *>(parent())->setNoBlanksFilter();
    static_cast<GridFilterColumnPopup *>(parent())->close();
}

void FilterPopupWidget::selectClicked(const QString& link)
{
    Q_UNUSED(link);
    FilterValueModel* model = static_cast<FilterValueModel *>(getGridView()->model());
    for (QVector<FilterValueModel::DataItem>::iterator it = model->m_values.begin(); it != model->m_values.end(); ++it)
        it->active = true;
    getGridView()->selectAll();
    getGridView()->modelController()->refresh();
}

void FilterPopupWidget::unselectClicked(const QString& link)
{
    Q_UNUSED(link);
    FilterValueModel* model = static_cast<FilterValueModel *>(getGridView()->model());
    for (QVector<FilterValueModel::DataItem>::iterator it = model->m_values.begin(); it != model->m_values.end(); ++it)
        it->active = false; 
    getGridView()->deselectAll();
    getGridView()->modelController()->refresh();
}

GridViewBase* FilterPopupWidget::getGridView()
{
    return m_valuesGrid->view<GridViewBase>();
}

QAbstractItemModel* FilterPopupWidget::createModel()
{
    AutoOverrideCursor autoCursor(Qt::WaitCursor, true);

    FilterValueModel* model = new FilterValueModel(this);
    QMap<QString, QVariant> strings;
 
    int count = m_column->dataBinding()->controller()->getRecordCount();
    for (int i = 0; i < count; ++i)
    {
        int modelIndex = m_column->dataBinding()->controller()->getModelRowIndexByRecord(i);
        QVariant value;
        if (!m_column->dataBinding()->getValue(modelIndex, m_column->dataBinding()->displayRole(), value))
            continue;

        strings.insert(value.toString(), value);
    }

    model->m_values.resize(strings.size());
    count = 0;
    for (QMap<QString, QVariant>::const_iterator it = strings.constBegin(); it != strings.constEnd(); ++it)
    {
        FilterValueModel::DataItem& item = model->m_values[count];
        item.title = it.key(); 
        item.value = it.value();
        item.active = false;
        count++;
    }

    return model;
}

QTITAN_BEGIN_NAMESPACE

class FindWhatContainer: public QWidget
{
public:
    FindWhatContainer(QWidget* parent)
        : QWidget(parent), m_editorWidget(Q_NULL)
    {
    }
    void setEditorWidget(QWidget* widget)
    {
        m_editorWidget = widget;
        if (m_editorWidget != Q_NULL)
        {
            m_editorWidget->setGeometry(QRect(QPoint(0, 0), size()));
            setMinimumSize(m_editorWidget->minimumSize());
            setMaximumSize(m_editorWidget->maximumSize());
            updateGeometry();
        }
    }
    virtual QSize sizeHint() const
    {
        if (m_editorWidget != Q_NULL)
            return m_editorWidget->sizeHint();
        QLineEdit e;
        return e.sizeHint();
    }
protected:
    virtual void resizeEvent(QResizeEvent* e)
    {
        Q_UNUSED(e);
        if (m_editorWidget != Q_NULL)
            m_editorWidget->setGeometry(QRect(QPoint(0, 0), size()));
    }
private:
    QWidget* m_editorWidget;
};

/*!
\class GridEditorSiteImpl
\internal
*/
class GridEditorSiteImpl: public GridEditorSite
{
public:
    GridEditorSiteImpl(FilterDialog* dlg, QWidget* containerWidget)
        : m_dlg(dlg), m_containerWidget(containerWidget), m_column(Q_NULL), m_editor(Q_NULL)
    {
    }

    virtual ~GridEditorSiteImpl()
    {
        if (m_editor != Q_NULL && m_editor->isEditMode())
            m_editor->setEditMode(false);
        Q_DELETE_AND_NULL(m_editor);
    }

    bool getEditorValue(QVariant& value)
    {
        if (m_editor == Q_NULL)
            return true;
        if (!m_editor->validate())
            return false;
        value = m_editor->getContextValue();
        return true;
    }

    GridTableColumn* getColumn() const
    {
        return m_column;
    }

    void setColumn(GridTableColumn* column)
    {
        if (m_editor != Q_NULL)
        {
            m_editor->setEditMode(false);
            Q_DELETE_AND_NULL(m_editor);
        }
        m_column = column;
        if (m_column == Q_NULL || m_column->editorRepository() == Q_NULL)
            return;
        m_editor = m_column->editorRepository()->createEditor();
        m_editor->initEditor(m_column->editorRepository(), this);
        m_editor->updateContext(m_column->dataBinding()->controller()->defaultValue(
            m_column->dataBinding()->column(), m_column->dataBinding()->displayRole()));
        m_editor->setEditMode(true);
        m_editor->updateContext(m_column->dataBinding()->controller()->defaultValue(
            m_column->dataBinding()->column(), m_column->dataBinding()->editRole()));
        QWidget* widget = m_editor->getCellWidget();
        widget->setAttribute(Qt::WA_NoSystemBackground, false);
        widget->setEnabled(true);
        widget->show();
        static_cast<FindWhatContainer *>(m_containerWidget)->setEditorWidget(widget);
    }

    virtual void invalidate(bool fullyViewUpdate = false)
    {
        Q_UNUSED(fullyViewUpdate);
        m_dlg->update();
    }

    virtual GridModelDataBinding* dataBinding() const
    {
        if (m_column == Q_NULL)
            return 0;
        return m_column->dataBinding();
    }

    virtual QModelIndex modelIndex() const
    {
        return QModelIndex();
    }

    virtual QWidget* parent() const
    {
        return m_containerWidget;
    }

    virtual QRect geometry() const
    {
        return QRect(0, 0, m_containerWidget->width(), m_containerWidget->height());
    }
    
    virtual GridEditorStyleOption editorOption()
    {
        GridEditorStyleOption option;
        init_editor_option(m_dlg->view(), &option, dataBinding(), Q_NULL, 
            m_column->index(), false, false, false,
            m_column->editorRepository() != Q_NULL ? m_column->editorRepository()->isEnabled() : false, false, geometry(),
            m_column->editorRepository() != Q_NULL ? m_column->editorRepository()->alignment() : Qt::AlignCenter);
        return option;
    }

    virtual void showHint(const QString& text) const
    {
        QPoint p = m_containerWidget->mapToGlobal(QPoint(0, 0));
        QToolTip::showText(p, text);
    }

    virtual void updateCursor(const QCursor& cursor)
    {
        m_containerWidget->setCursor(cursor);
    }

    virtual void linkActivated(const QString& anchor)
    {
        Q_UNUSED(anchor);
    }
private:
    FilterDialog* m_dlg;
    QWidget* m_containerWidget;
    GridTableColumn* m_column;
    GridEditor* m_editor;
};

QTITAN_END_NAMESPACE

/*!
\class FilterDialog
\internal
*/
FilterDialog::FilterDialog(GridViewBase* view, GridFilterGroupCondition* condition, QWidget *parent)
    : QDialog(parent), m_view(view), m_condition(condition)
{
    if (m_condition == Q_NULL)
        m_condition = new GridFilterGroupCondition(m_view->filter());
    else
        m_condition = (GridFilterGroupCondition *)condition->clone();

    m_tab = new QTabWidget;
    m_filterWidget = new QWidget(m_tab);
    m_tab->addTab(m_filterWidget, Grid::tr_compatible(QtnGridFilterString));
    m_formLayout = new QFormLayout(m_filterWidget);

    m_columns1 = new QComboBox(m_filterWidget);
    connect(m_columns1, SIGNAL(currentIndexChanged(int)), this, SLOT(column1IndexChanged(int)));
    m_findWhatContainer1 = new FindWhatContainer(m_filterWidget);
    m_operator1 = new QComboBox(m_filterWidget);
    m_operator1->addItem(Grid::tr_compatible(QtnGridFilterEqualString), GridFilterCondition::FilterEqual);
    m_operator1->addItem(Grid::tr_compatible(QtnGridFilterNotEqualString), GridFilterCondition::FilterNotEqual);
    m_operator1->addItem(Grid::tr_compatible(QtnGridFilterLessString), GridFilterCondition::FilterLess);
    m_operator1->addItem(Grid::tr_compatible(QtnGridFilterLessorOrEqualString), GridFilterCondition::FilterLessEqual);
    m_operator1->addItem(Grid::tr_compatible(QtnGridFilterGreaterString), GridFilterCondition::FilterGreater);
    m_operator1->addItem(Grid::tr_compatible(QtnGridFilterGreaterOrEqualString), GridFilterCondition::FilterGreaterEqual);

    m_columns2 = new QComboBox(m_filterWidget);
    connect(m_columns2, SIGNAL(currentIndexChanged(int)), this, SLOT(column2IndexChanged(int)));
    m_findWhatContainer2 = new FindWhatContainer(m_filterWidget);
    m_operator2 = new QComboBox(m_filterWidget);
    m_operator2->addItem(Grid::tr_compatible(QtnGridFilterEqualString), GridFilterCondition::FilterEqual);
    m_operator2->addItem(Grid::tr_compatible(QtnGridFilterNotEqualString), GridFilterCondition::FilterNotEqual);
    m_operator2->addItem(Grid::tr_compatible(QtnGridFilterLessString), GridFilterCondition::FilterLess);
    m_operator2->addItem(Grid::tr_compatible(QtnGridFilterLessorOrEqualString), GridFilterCondition::FilterLessEqual);
    m_operator2->addItem(Grid::tr_compatible(QtnGridFilterGreaterString), GridFilterCondition::FilterGreater);
    m_operator2->addItem(Grid::tr_compatible(QtnGridFilterGreaterOrEqualString), GridFilterCondition::FilterGreaterEqual);

    m_findwhatlayout1 = new QHBoxLayout();
    m_findwhatlayout2 = new QHBoxLayout();
    m_operator1->setMaximumWidth(100);
    m_operator2->setMaximumWidth(100);
    m_findwhatlayout1->addWidget(m_operator1);
    m_findwhatlayout1->addWidget(m_findWhatContainer1);
    m_findwhatlayout2->addWidget(m_operator2);
    m_findwhatlayout2->addWidget(m_findWhatContainer2);

    m_formLayout->addRow(Grid::tr_compatible(QtnGridFilterColumnString), m_columns1);
    m_formLayout->addRow(Grid::tr_compatible(QtnGridFilterFindWhatString), m_findwhatlayout1);

    QHBoxLayout* hblt = new QHBoxLayout();
    m_or = new QRadioButton(Grid::tr_compatible(QtnGridFilteORString));
    m_and = new QRadioButton(Grid::tr_compatible(QtnGridFilterANDString));
    m_andnot = new QRadioButton(Grid::tr_compatible(QtnGridFilterANDNOTString));
    connect(m_or, SIGNAL(clicked(bool)), this, SLOT(clickedOR(bool)));
    connect(m_and, SIGNAL(clicked(bool)), this, SLOT(clickedAND(bool)));
    connect(m_andnot, SIGNAL(clicked(bool)), this, SLOT(clickedANDNOT(bool)));
    hblt->addWidget(m_or);
    hblt->addWidget(m_and);
    hblt->addWidget(m_andnot);
    m_formLayout->addRow(hblt);
    m_formLayout->addRow(Grid::tr_compatible(QtnGridFilterColumnString), m_columns2);
    m_formLayout->addRow(Grid::tr_compatible(QtnGridFilterFindWhatString), m_findwhatlayout2);
    m_addToFilter = new QCheckBox(Grid::tr_compatible(QtnGridFilterSearchInResultsString), m_filterWidget);
    m_formLayout->addRow(m_addToFilter);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok |
                                     QDialogButtonBox::RestoreDefaults |
                                     QDialogButtonBox::Cancel);
    
    m_buttons->button(QDialogButtonBox::RestoreDefaults)->setText(Grid::tr_compatible(QtnGridResetString));

    connect(m_buttons, SIGNAL(accepted()), this, SLOT(dialogOk()));
    connect(m_buttons, SIGNAL(rejected()), this, SLOT(dialogCancel()));
    connect(m_buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogClicked(QAbstractButton*)));

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_tab);
    mainLayout->addWidget(m_buttons);
    setLayout(mainLayout);
    setWindowTitle(Grid::tr_compatible(QtnGridFilterDialogString));
    setEnabledSecondField(false);

    m_columnSite1 = new GridEditorSiteImpl(this, m_findWhatContainer1);
    m_columnSite2 = new GridEditorSiteImpl(this, m_findWhatContainer2);
    fillColumns(m_columns1);
    fillColumns(m_columns2);
}

FilterDialog::~FilterDialog()
{
    Q_DELETE_AND_NULL(m_condition);
    delete static_cast<GridEditorSiteImpl *>(m_columnSite1);
    delete static_cast<GridEditorSiteImpl *>(m_columnSite2);
    m_columnSite1 = Q_NULL;
    m_columnSite2 = Q_NULL;
}

void FilterDialog::fillColumns(QComboBox* box)
{
    for (int i = 0; i< m_view->getColumnCount(); ++i)
    {
        GridDataTableColumn* column = qobject_cast<GridDataTableColumn *>(m_view->getColumn(i));
        if (column == Q_NULL || !column->isFilterButtonVisible())
            continue;
        box->addItem(column->caption(), i);
    }
    if (m_view->modelController()->focusedColumnIndex() >= 0)
        box->setCurrentIndex(m_view->modelController()->focusedColumnIndex());

}

GridFilterGroupCondition* FilterDialog::condition() const
{
    return m_condition;
}

void FilterDialog::setEnabledSecondField(bool enabled)
{
    m_columns2->setEnabled(enabled);
    m_formLayout->labelForField(m_columns2)->setEnabled(enabled);
    m_findWhatContainer2->setEnabled(enabled);
    m_operator2->setEnabled(enabled);
    m_formLayout->labelForField(m_findwhatlayout2)->setEnabled(enabled);
}

void FilterDialog::dialogOk()
{
    if (!m_addToFilter->isChecked())
        m_condition->clear();

    GridTableColumn* c1 = static_cast<GridEditorSiteImpl *>(m_columnSite1)->getColumn();
    GridTableColumn* c2 = static_cast<GridEditorSiteImpl *>(m_columnSite2)->getColumn();

    QVariant v;
    if (!static_cast<GridEditorSiteImpl *>(m_columnSite1)->getEditorValue(v))
        return;

    QStandardItemModel *m = qobject_cast<QStandardItemModel*>(m_operator1->model());
    QStandardItem* item = m->item(m_operator1->currentIndex(), 0);
    GridFilterCondition::ValueOperator op = (GridFilterCondition::ValueOperator)item->data(Qt::UserRole).toInt();

    m_condition->addValue(c1->dataBinding()->column(),
                          c1->dataBinding()->displayRole(),
                          v,
                          v.type() == QVariant::String ?
                          GridFilterCondition::FilterLike : op,
                          GridFilterCondition::FilterAnd);
    if (c2 != Q_NULL && m_columns2->isEnabled())
    {
        if (!static_cast<GridEditorSiteImpl *>(m_columnSite2)->getEditorValue(v))
            return;

        GridFilterCondition::GroupOperator groupOperator = GridFilterCondition::FilterAnd;
        if (m_or->isChecked())
            groupOperator = GridFilterCondition::FilterOr;
        else if (m_andnot->isChecked())
            groupOperator = GridFilterCondition::FilterNot;

        m = qobject_cast<QStandardItemModel*>(m_operator2->model());
        item = m->item(m_operator2->currentIndex(), 0);
        op = (GridFilterCondition::ValueOperator)item->data(Qt::UserRole).toInt();

        m_condition->addValue(c2->dataBinding()->column(),
                              c2->dataBinding()->displayRole(),
                              v,
                              v.type() == QVariant::String ?
                              GridFilterCondition::FilterLike : op, groupOperator);
    }
    accept();
}

void FilterDialog::dialogCancel()
{
    reject();
}

void FilterDialog::dialogClicked(QAbstractButton * button)
{
    if (m_buttons->button(QDialogButtonBox::RestoreDefaults) == button)
    {
        setEnabledSecondField(false);
        
        m_and->setAutoExclusive(false);
        m_or->setAutoExclusive(false);
        m_andnot->setAutoExclusive(false);

        m_and->setChecked(false);
        m_or->setChecked(false);
        m_andnot->setChecked(false);

        m_and->setAutoExclusive(true);
        m_or->setAutoExclusive(true);
        m_andnot->setAutoExclusive(true);
    }
}

void FilterDialog::clickedOR(bool checked)
{
    Q_UNUSED(checked);
    setEnabledSecondField(true);
}

void FilterDialog::clickedAND(bool checked)
{
    Q_UNUSED(checked);
    setEnabledSecondField(true);
}

void FilterDialog::clickedANDNOT(bool checked)
{
    Q_UNUSED(checked);
    setEnabledSecondField(true);
}

void FilterDialog::column1IndexChanged(int index)
{
    int columnIndex = m_columns1->itemData(index, Qt::UserRole).toInt();
    GridTableColumn* column = static_cast<GridTableColumn *>(m_view->getColumn(columnIndex));
    static_cast<GridEditorSiteImpl *>(m_columnSite1)->setColumn(column);
}

void FilterDialog::column2IndexChanged(int index)
{
    int columnIndex = m_columns2->itemData(index, Qt::UserRole).toInt();
    GridTableColumn* column = static_cast<GridTableColumn *>(m_view->getColumn(columnIndex));
    static_cast<GridEditorSiteImpl *>(m_columnSite2)->setColumn(column);
}


/*!
\class FilterTextWidget
\internal
*/
FilterTextWidget::FilterTextWidget(QWidget* parent)
    : QWidget(parent), n_needHint(false), m_hover(false), m_glassHighlight(true)
{
     setMouseTracking(true);
     setCursor(Qt::PointingHandCursor);
}

void FilterTextWidget::setText(const QString& text)
{
    m_text = text;
    updateGeometry();
}

bool FilterTextWidget::isGlassHighlight() const
{
    return m_glassHighlight;
}

void FilterTextWidget::setGlassHighlight(bool flag)
{
    m_glassHighlight = flag;
}

bool FilterTextWidget::event(QEvent* e)
{
    switch (e->type()) {
    case QEvent::HoverEnter:
    case QEvent::Enter:
        m_hover = true;
        update();
        break;
    case QEvent::HoverLeave:
    case QEvent::Leave:
        m_hover = false;
        m_tooltipTimer.stop();
        update();
        break;
    case QEvent::Timer:
        {
            QPoint pos = QCursor::pos();
            if (static_cast<QTimerEvent *>(e)->timerId() == m_tooltipTimer.timerId())
                QToolTip::showText(pos, "<p>" + m_text + "</p>");
        }
        break;
    default:
        break;
    }
    return QWidget::event(e);
}

QSize FilterTextWidget::sizeHint() const
{
    return fontMetrics().boundingRect(m_text).size() + QSize(10, 10);
}

void FilterTextWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(palette().color(foregroundRole()));
    QFont f = painter.font();
    f.setUnderline(!isGlassHighlight() && m_hover);
    painter.setFont(f);
    QTextOption op(Qt::AlignVCenter);
    QRect r(QPoint(0, 0), size());
    QRect r2 = r.adjusted(5, 1, -3, -5);
    QString elidedText = painter.fontMetrics().elidedText(m_text, Qt::ElideRight, r2.width());
    if (m_hover && isGlassHighlight())
    {
        QPixmap bgglass(":res/grid_glass_bg.png");
        r.setWidth(painter.fontMetrics().boundingRect(elidedText).width() + 10);
        draw_pixmap_texture(painter, r, bgglass, 3, 4, 3, 4);
        painter.save();
        painter.setOpacity(0.1);
        painter.setBrush(palette().color(foregroundRole()));
        painter.drawRoundedRect(r.adjusted(1, 2, -1, -2), 3, 3);
        painter.restore();

    }
    n_needHint = elidedText.size() != m_text.size();  
    painter.drawText(r2, elidedText, op);
}


void FilterTextWidget::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    if (!n_needHint)
        return;
    m_tooltipTimer.stop();
    m_tooltipTimer.start(1000, this);
}

void FilterTextWidget::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    emit clicked();
}

/* GridFilterHistoryCloseButtonPrimitive */
GridFilterHistoryCloseButtonPrimitive::GridFilterHistoryCloseButtonPrimitive(GraphicPrimitive* parent)
    : GridCloseButtonPrimitive(parent)
{
}

void GridFilterHistoryCloseButtonPrimitive::clicked()
{
    static_cast<FilterHistoryPopupPrimitive *>(parentGP())->setWinVisible(false);
}
/* GridHistoryItemButton */
GridHistoryItemButton::GridHistoryItemButton(GraphicPrimitive* parent)
    : GridShadeButtonPrimitive(parent)
{
}

QSize GridHistoryItemButton::sizeHint()
{
    return GridShadeButtonPrimitive::sizeHint();
}

void GridHistoryItemButton::paintTexture(QPainter& painter, const QColor& accentColor, const QRect& rect)
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

QPalette GridHistoryItemButton::getButtonPalette() const
{
    QPalette retval = GridShadeButtonPrimitive::getButtonPalette();
    retval.setColor(QPalette::Text, Qt::white);
    return retval;
}

/* FilterHistoryPopupPrimitive */
FilterHistoryPopupPrimitive::FilterHistoryPopupPrimitive(GraphicPrimitive* parent, GridFilterHistory* history)
    : GridWinShadowedPrimitive(parent), m_history(history)
{
    m_closeButtonGP = new GridFilterHistoryCloseButtonPrimitive(this);
    QObject::connect(m_history, SIGNAL(changed()), this, SLOT(fillItems()));
    fillItems();
}

FilterHistoryPopupPrimitive::~FilterHistoryPopupPrimitive()
{
    QObject::disconnect(m_history, SIGNAL(changed()), this, SLOT(fillItems()));
    m_history = Q_NULL;
}

static const int history_item_top = 30;
static const int history_item_indent = 10;

void FilterHistoryPopupPrimitive::updateLayout()
{
    QSize sh = m_closeButtonGP->sizeHint();
    m_closeButtonGP->setRect(QRect(QPoint(width() - sh.width() - 10, 10), sh));

    int top = history_item_top;
    for (QList<GridHistoryItemButton *>::const_iterator it = m_items.constBegin(); it != m_items.constEnd(); ++it)
    {
        GridHistoryItemButton* item = *it;
        QSize size = item->sizeHint();
        item->setRect(QRect(history_item_indent, top, width() - history_item_indent * 2, size.height()));
        top += size.height() + 3;
    }
}

QSize FilterHistoryPopupPrimitive::sizeHint()
{
    return m_sizeHint;
}

void FilterHistoryPopupPrimitive::fillItems()
{
    while (m_items.size() > 0)
        delete m_items.takeFirst();

    int maxWidth = 0;
    int maxHeight = history_item_top;
    for (int i = 0; i < m_history->count(); i++)
    {
        GridFilterCondition* cond = m_history->getCondition(i);
        GridHistoryItemButton* item = new GridHistoryItemButton(this);
        connect(item, SIGNAL(clicked()), this, SLOT(itemClicked()));
        item->setText(m_history->filter()->getPresentation(cond));
        QSize hint = item->sizeHint();
        if (hint.width() > maxWidth)
            maxWidth = hint.width();
        maxHeight += hint.height();
        m_items.push_back(item);
    }
    m_sizeHint = QSize(maxWidth + history_item_indent * 2, maxHeight + history_item_top);
}

void FilterHistoryPopupPrimitive::paintBackground(QPainter& painter)
{
    QRect r = boundedRect();

    painter.setBrush(QBrush(qtn_get_thememanager(control())->shadeBackground()));
    painter.setPen(qtn_get_thememanager(control())->shadePalette().background().color());

    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath roundRectPath;
    const int radius = 5;
    const int indent = 10;

    roundRectPath.moveTo(r.left() + r.width() - radius, r.top()); //Right-top
    roundRectPath.arcTo(r.left() + r.width() - radius * 2, r.top(), radius * 2, radius * 2, 90, -90);
    roundRectPath.lineTo(r.left() + r.width(), r.top() + (r.height() - indent) - radius); //Right-bottom
    roundRectPath.arcTo(r.left() + r.width() - radius * 2, r.top() + (r.height() - indent) - radius * 2, radius * 2, radius * 2, 0, -90);

    roundRectPath.lineTo(r.left() + radius + (r.width() - 2 * radius) / 2 + indent * 0.75, r.top() + (r.height() - indent));//Indent
    roundRectPath.lineTo(r.left() + radius + (r.width() - 2 * radius) / 2, r.top() + r.height()); //Indent
    roundRectPath.lineTo(r.left() + radius + (r.width() - 2 * radius) / 2 - indent * 0.75, r.top() + (r.height() - indent));

    roundRectPath.lineTo(r.left() + radius, r.top() + (r.height() - indent)); //Left-bottom
    roundRectPath.arcTo(r.left(), r.top() + (r.height() - indent) - radius * 2, radius * 2, radius * 2, -90, -90);

    roundRectPath.lineTo(r.left(), r.top() + radius); //Left-top
    roundRectPath.arcTo(r.left(), r.top(), radius * 2, radius * 2, -180, -90);

    roundRectPath.lineTo(r.left() + r.width() - radius, r.top());
    roundRectPath.closeSubpath();
    painter.drawPath(roundRectPath);

    painter.setPen(qtn_get_thememanager(control())->shadePalette().light().color());
    QFont f = painter.font();
    f.setBold(true);
    painter.setFont(f);
    QRect textRect = QRect(history_item_indent, m_closeButtonGP->top(), m_closeButtonGP->rect().right() - history_item_indent, 
        painter.fontMetrics().height());
    paint_icon_and_text(painter, textRect, QPixmap(), Grid::tr_compatible(QtnGridFilterHistoryText), Qt::AlignLeft | Qt::AlignTop, Qt::ElideRight, IconInvisible);
}

void FilterHistoryPopupPrimitive::itemClicked()
{
    if (m_history == Q_NULL)
        return;
    int index = m_items.indexOf(static_cast<GridHistoryItemButton *>(sender()));
    m_history->setCurrentIndex(index);
    setWinVisible(false);
}


