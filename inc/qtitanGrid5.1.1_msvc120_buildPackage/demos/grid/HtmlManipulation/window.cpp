
#include <QtGui>
#include <QDirModel>
#include <QDesktopServices>
#include <QMessageBox>

#include "window.h"

/* Window */
Window::Window()
: DemoMainWindow("QtitanDataGrid", QTN_VERSION_DATAGRID_STR)
{
    setWindowTitle(tr("QtitanDataGrid - HTML Manipulation"));
    setGeometry(150, 150, 1100, 900);

    Grid::loadTranslation();

    m_grid = new Qtitan::Grid();
    HthmlManipulationModel* model = new HthmlManipulationModel(m_grid);

    // Configure grid view
    m_grid->setViewType(Qtitan::Grid::BandedTableView);
    Qtitan::GridBandedTableView* view = m_grid->view<Qtitan::GridBandedTableView>();
    view->addBand("Section 1");
    view->addBand("Section 2");
    view->addBand("Section 3");
    view->options().setFooterSummaryVisible(false);
    view->options().setGroupSummaryPlace(GroupSummaryRow);
    view->options().setPreviewRowEnabled(true);
    view->options().setPreviewExpandButtonVisible(false);
    view->modelController()->previewRowDataBinding()->setDisplayRole(Qt::WhatsThisRole);
    view->options().setPreviewRowHeight(150);
    view->options().setScrollRowStyle(ScrollByPixel);

    GridTableBand* band = view->getBand(0);
    band->setHtmlCaption("<i>Section</i> <b>One</b> <a href='#test1'>Link 1</a>");
    band = view->getBand(1);
    band->setHtmlCaption("<i>Section</i> <b>Two</b> <a href='#test2'>Link 2</a>");
    band = view->getBand(2);
    band->setHtmlCaption("<i>Section</i> <b>Three</b> <a href='#test3'>Link 3</a>");

    
    //Connect Grid's context menu handler.
    connect(view, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs* )));

    //Connect link activation handlers.
    connect(view, SIGNAL(previewLinkActivated(const GridRow*, const QString&)), this, SLOT(previewLinkActivated(const GridRow*, const QString&)));
    connect(view, SIGNAL(columnLinkActivated(const GridColumnBase*, const QString&)), this, SLOT(columnLinkActivated(const GridColumnBase*, const QString&)));
    connect(view, SIGNAL(editorLinkActivated(EditorLinkEventArgs*)), this, SLOT(editorLinkActivated(EditorLinkEventArgs*)));
    connect(view, SIGNAL(summaryGroupTextChanging(SummaryGroupTextEventArgs*)), this, SLOT(summaryGroupTextChanging(SummaryGroupTextEventArgs*)));
    view->setModel(model);

    Qtitan::GridBandedTableColumn* column = (Qtitan::GridBandedTableColumn *)view->getColumn(0);
    column->setBandIndex(0);
    //Configure the summary
    column->setDefaultGroupSummary(Qtitan::SummaryMax);
    column->setDecorationColor(Qt::yellow);

    column = (Qtitan::GridBandedTableColumn *)view->getColumn(1);
    column->setBandIndex(1);
    column->setRowIndex(0);
    //Configure the summary
    column->setDefaultGroupSummary(Qtitan::SummaryAvg);
    column->setFooterSummary(Qtitan::SummaryAvg);

    column = (Qtitan::GridBandedTableColumn *)view->getColumn(2);
    column->setBandIndex(1);
    column->setRowIndex(1);
    column->setHtmlCaption("<font color='red'>Column</font> with <a href='#string_column1'>HTML Caption</a>");
    //Configure the summary
    column->setDefaultGroupSummary(Qtitan::SummaryCount);
    column->setFooterSummary(Qtitan::SummaryCount);

    column = (Qtitan::GridBandedTableColumn *)view->getColumn(3);
    column->setBandIndex(1);
    column->setRowIndex(2);
    //Add cell button to the column.
    column->addButton();
    connect(column, SIGNAL(buttonClicked(CellButtonClickEventArgs*)), this, SLOT(cellButtonClicked(CellButtonClickEventArgs*)));

    column = (Qtitan::GridBandedTableColumn *)view->getColumn(4);
    column->setBandIndex(2);
    column->setEditorType(MemoEditorType);
    ((GridMemoEditorRepository *)column->editorRepository())->setHTML(true);
    column->setRowSpan(3);

    column = (Qtitan::GridBandedTableColumn *)view->getColumn(5);
    column->setBandIndex(0);
    column->setRowSpan(2);
    column->setRowIndex(1);
    column->setGroupIndex(0);
    //Configure the summary
    column->setDefaultGroupSummary(Qtitan::SummaryMin);
    column->setFooterSummary(Qtitan::SummaryMax);
    view->expandAll();

    //Show button menu for all column headers.
    for (int i = 0; i < view->getColumnCount(); ++i)
        static_cast<GridTableColumn *>(view->getColumn(i))->setMenuButtonVisible(true);

    setDemoWidget(m_grid, createSettingsWidget());
}

QWidget* Window::createSettingsWidget()
{
    QWidget* settings = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(settings);
    QCheckBox* autoWidthCheck = new QCheckBox(settings);
    l->addWidget(autoWidthCheck);
    autoWidthCheck->setText("Column auto width");
    connect(autoWidthCheck, SIGNAL(stateChanged(int)), this, SLOT(autoWidthStateChanged(int)));
    autoWidthCheck->setChecked(true);

    QCheckBox* fastScrollCheck = new QCheckBox(settings);
    fastScrollCheck->setText("Fast scroll effect");
    connect(fastScrollCheck, SIGNAL(stateChanged(int)), this, SLOT(fastScrollChanged(int)));
    l->addWidget(fastScrollCheck);
    fastScrollCheck->setChecked(true);

    QCheckBox* dottedLineCheck = new QCheckBox(settings);
    dottedLineCheck->setText("Dotted grid line");
    connect(dottedLineCheck, SIGNAL(stateChanged(int)), this, SLOT(dottedLineChanged(int)));
    l->addWidget(dottedLineCheck);
    dottedLineCheck->setChecked(false);

    QLabel* label = new QLabel(settings);
    QHBoxLayout* hl = new QHBoxLayout(0);
    label->setText("Grid line style:");
    QComboBox* lineStylesSelect = new QComboBox(settings);

    lineStylesSelect->addItem("None");
    lineStylesSelect->addItem("Both");
    lineStylesSelect->addItem("Both2D");
    lineStylesSelect->addItem("Horizontal");
    lineStylesSelect->addItem("Horizontal2D");
    lineStylesSelect->addItem("Vertical");
    lineStylesSelect->addItem("Vertical2D");
    connect(lineStylesSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(selectGridLineStyles(int)));
    hl->addWidget(label);
    hl->addWidget(lineStylesSelect);
    l->addLayout(hl);
    lineStylesSelect->setCurrentIndex(2);

    QCheckBox* zoomEnable = new QCheckBox(settings);
    zoomEnable->setText(tr("Zoom enabled"));
    zoomEnable->setChecked(true);
    connect(zoomEnable, SIGNAL(stateChanged(int)), this, SLOT(zoomEnabledChanged(int)));
    l->addWidget(zoomEnable);

    QCheckBox* zoomIndicator = new QCheckBox(settings);
    zoomIndicator->setText(tr("Show zoom indicator"));
    zoomIndicator->setChecked(true);
    connect(zoomIndicator, SIGNAL(stateChanged(int)), this, SLOT(zoomIndicatorChanged(int)));
    l->addWidget(zoomIndicator);

    QSlider* zoomSlider = new QSlider(settings);
    zoomSlider->setOrientation(Qt::Horizontal);
    zoomSlider->setTickPosition(QSlider::TicksBothSides);
    zoomSlider->setMinimum(25);
    zoomSlider->setMaximum(300);
    zoomSlider->setTickInterval(25);
    zoomSlider->setSingleStep(25);
    zoomSlider->setValue(100);
    connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(zoomValueChanged(int)));
    connect(m_grid->view<Qtitan::GridTableView>(), SIGNAL(zoomChanged(int)), zoomSlider, SLOT(setValue(int)));
    l->addWidget(zoomSlider);

    QCheckBox* airCheckBox = new QCheckBox(settings);
    airCheckBox->setText(tr("Windows Air Support"));
    connect(airCheckBox, SIGNAL(stateChanged(int)), this, SLOT(WindowsAirSupportChanged(int)));
    airCheckBox->setChecked(false);
    l->addWidget(airCheckBox);

    QCheckBox* cellAutoRaise = new QCheckBox(settings);
    cellAutoRaise->setText(tr("Auto raise cell button"));
    connect(cellAutoRaise, SIGNAL(stateChanged(int)), this, SLOT(cellButtonAutoRaiseEnabled(int)));
    cellAutoRaise->setChecked(true);
    l->addWidget(cellAutoRaise);

    QCheckBox* frozenRowsBox = new QCheckBox(settings);
    frozenRowsBox->setText(tr("Frozen Rows"));
    connect(frozenRowsBox, SIGNAL(stateChanged(int)), this, SLOT(frozenRowsEnabled(int)));
    frozenRowsBox->setChecked(true);
    l->addWidget(frozenRowsBox);

    QCheckBox* transparentBox = new QCheckBox(settings);
    transparentBox->setText(tr("Transparent Background"));
    connect(transparentBox, SIGNAL(stateChanged(int)), this, SLOT(transparentBackgroundEnabled(int)));
    transparentBox->setChecked(false);
    l->addWidget(transparentBox);

    QPushButton* printButton = new QPushButton(settings);
    printButton->setText(tr("Print Preview"));
    connect(printButton, SIGNAL(clicked()), this, SLOT(printPreview()));
    l->addWidget(printButton);

    return settings;
}

void Window::setShadeColor(const QColor& color)
{
    m_grid->themeManager()->setShadeColor(color);
}

void Window::previewLinkActivated(const GridRow* row, const QString& anchor)
{
    QMessageBox::information(this, tr("Preview link clicked"),
        tr("Clicked: RowIndex  %1, Link %2").arg(row->rowIndex()).arg(anchor));
}

void Window::columnLinkActivated(const GridColumnBase* column, const QString& anchor)
{
    if (qobject_cast<const GridTableBand *>(column) != Q_NULL)
        QMessageBox::information(this, tr("Band Column HTML link clicked"), tr("Clicked: Band %1, Link %2").arg(column->caption()).arg(anchor));
    else
        QMessageBox::information(this, tr("Column HTML link clicked"), tr("Clicked: Column %1, Link %2").arg(column->caption()).arg(anchor));
}

void Window::editorLinkActivated(EditorLinkEventArgs* args)
{
    QMessageBox::information(this, tr("Cell Editor link clicked"), tr("Column %1, RowIndex %2, Link %3").arg(args->column()->caption()).arg(args->row()->rowIndex()).arg(args->anchor()));
}

void Window::summaryGroupTextChanging(SummaryGroupTextEventArgs* args)
{
    if (args->dataBinding()->column() == 1)
        args->setTextColor(Qt::red);
}

void Window::autoWidthStateChanged(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setColumnAutoWidth(state == Qt::Checked);
}

void Window::fastScrollChanged(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setFastScrollEffect(state == Qt::Checked);
}

void Window::dottedLineChanged(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    QPen pen = view->options().gridLinePen();
    pen.setStyle(state == Qt::Checked ? Qt::DotLine : Qt::SolidLine);
    view->options().setGridLinePen(pen);
}

void Window::selectGridLineStyles(int index)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    switch (index)
    {
    case 0:
        view->options().setGridLines(Qtitan::LinesNone);
        break;
    case 1:
        view->options().setGridLines(Qtitan::LinesBoth);
        break;
    case 2:
        view->options().setGridLines(Qtitan::LinesBoth2D);
        break;
    case 3:
        view->options().setGridLines(Qtitan::LinesHorizontal);
        break;
    case 4:
        view->options().setGridLines(Qtitan::LinesHorizontal2D);
        break;
    case 5:
        view->options().setGridLines(Qtitan::LinesVertical);
        break;
    case 6:
        view->options().setGridLines(Qtitan::LinesVertical2D);
        break;
    default:
        view->options().setGridLines(Qtitan::LinesBoth);
    }
 }

void Window::selectViewOrientation(int index)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    switch (index)
    {
    case 0:
    {
        view->options().setViewOrientation(Qt::Vertical);
    }
    break;
    case 1:
    {
        view->options().setViewOrientation(Qt::Horizontal);
    }
    break;
    default:
        break;
    }
}

void Window::zoomEnabledChanged(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setZoomEnabled(state == Qt::Checked);
}

void Window::zoomIndicatorChanged(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setZoomIndicatorActive(state == Qt::Checked);
}

void Window::zoomValueChanged(int value)
{
    double factor = qCeil((double)value / 25) * 25;
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setZoomFactor(factor / 100);
}

void Window::WindowsAirSupportChanged(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setWindowsAirSupported(state == Qt::Checked);
}

void Window::cellButtonAutoRaiseEnabled(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setCellButtonAutoRaise(state == Qt::Checked);
}

void Window::frozenRowsEnabled(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setRowFrozenButtonVisible(state == Qt::Checked);
    view->options().setFrozenPlaceQuickSelection(state == Qt::Checked);
}

void Window::transparentBackgroundEnabled(int state)
{
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setTransparentBackground(state == Qt::Checked);

    view->options().setAlternatingRowColors(!view->options().alternatingRowColors());
}

void Window::contextMenu(ContextMenuEventArgs* args)
{
    args->contextMenu()->addAction("Print Preview", this, SLOT(printPreview()));
    args->contextMenu()->addSeparator();
    args->contextMenu()->addAction("Developer Machines on the Web", this, SLOT(showCompanyWebSite()));
}

void Window::cellButtonClicked(CellButtonClickEventArgs* args)
{
    QMessageBox::information(this, tr("Cell button clicked"), 
        tr("Clicked: Button - %1, Column Title - %2, RowIndex - %3").arg(args->buttonIndex()).arg(args->column()->caption()).arg(args->row()->rowIndex()));
}

void Window::printPreview()
{
    m_grid->view<Qtitan::GridTableView>()->printPreview();
}

void Window::showCompanyWebSite()
{
    QDesktopServices::openUrl(QUrl("http://www.devmachines.com"));
}

/* HthmlManipulationModel */
HthmlManipulationModel::HthmlManipulationModel(QObject *parent)
: QAbstractItemModel(parent)
{
    const QString memoText = "<a href='#link_%1'>Lorem ipsum</a> dolor sit amet, consectetur adipiscing elit. Nullam sodales, \
                                libero at hendrerit commodo, urna velit vestibulum purus, cursus dignissim tellus ante non leo.";
    m_values.resize(100);
    int y = 2000;
    int m = 1;
    int d = 1;
    for (int i = 0; i < m_values.size(); ++i)
    {
        m_values[i].v0 = true;
        m_values[i].v1 = i;
        m_values[i].v2 = QString("String-1 = %1").arg(i);
        m_values[i].v3 = QString("String-2 = %1").arg(i);
        if (d > 28)
        {
            d = 1;
            m++;
            if (m > 12)
            {
                m = 1;
                y++;
            }
        }
        m_values[i].v4memo = memoText.arg(i);
        m_values[i].v5 = QDate(y, m, d);
        if (!(i % 10))
            d++;
    }
} 

HthmlManipulationModel::~HthmlManipulationModel()
{
} 

QVariant HthmlManipulationModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const
{
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    switch (section)
    {
    case 0:
        {
            return QString("Boolean");
        }
        break;
    case 1:
        {
            return QString("Integer");
        }
        break;
    case 2:
        {
            return QString("String");
        }
        break;
    case 3:
        {
            return QString("String");
        }
        break;
        case 4:
        {
            return QString("Memo");
        }
        break;
        case 5:
        {
            return QString("Date");
        }
        break;
    }
    return QVariant();
}

QModelIndex HthmlManipulationModel::parent(const QModelIndex & /*child*/) const
{
    return QModelIndex();
} 

bool HthmlManipulationModel::hasChildren(const QModelIndex &parent) const
{  
    if (parent.model() == this || !parent.isValid()) 
    {
        return rowCount(parent) > 0 && columnCount(parent) > 0;
    }
    return false;
} 

int HthmlManipulationModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_values.size();
} 

int HthmlManipulationModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 6;
}

QModelIndex HthmlManipulationModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())                                   
        return QModelIndex();

    if (row < 0 || row >= rowCount(parent))
        return QModelIndex();

    if (column < 0 || column >= columnCount(parent)) 
        return QModelIndex();

    return createIndex(row, column, (void*)NULL);
}

QVariant HthmlManipulationModel::data(const QModelIndex &index, int role) const
{  
    if (!index.isValid())
        return QVariant();
    
    if (index.row() < 0 || index.row() >= rowCount(index.parent()))
        return QVariant();
    
    if (index.column() < 0 || index.column() >= columnCount(index.parent()))
        return QVariant();
    if (role == Qt::WhatsThisRole)
    {
        const QString previewText = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam sodales, \
                                     libero at hendrerit commodo, urna velit vestibulum purus, cursus dignissim tellus ante non leo. \
                                     <h2>Duis quam lorem, bibendum id luctus eu, consequat at purus</h2>. \
                                     <a href='#preview_link'>Test Preview Link</a> \
                                     Duis quam lorem, bibendum id luctus eu, consequat at purus. Curabitur posuere ligula eu turpis porttitor faucibus rhoncus neque pretium. \
                                     Aenean ut urna felis.";
        return previewText;
    }
    else
    if (role == Qt::DisplayRole || role == Qt::EditRole)        
    {    
        switch (index.column())
        {
        case 0:
            {
                return m_values[index.row()].v0;
            }
            break;
        case 1:
            {
                return m_values[index.row()].v1;
            }
            break;
        case 2:
            {
                return m_values[index.row()].v2;
            }
            break;
        case 3:
            {
                return m_values[index.row()].v3;
            }
            break;
        case 4:
            {
                return m_values[index.row()].v4memo;
            }
            break;
            case 5:
            {
                return m_values[index.row()].v5;
            }
            break;
        }
    }
    else if (role == Qt::CheckStateRole)
    {
        switch (index.column())
        {
        case 0:
            {
                return m_values[index.row()].v0;
            }
            break;
        }
    }
    return QVariant();
}

bool HthmlManipulationModel::setData(const QModelIndex &index, const QVariant &value, int role)
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
            m_values[index.row()].v0 = value.toBool();
        }
        break;
    case 1:
        {
            m_values[index.row()].v1 = value.toInt();
        }
        break;
    case 2:
        {
            m_values[index.row()].v2 = value.toString();
        }
        break;
    case 3:
        {
            m_values[index.row()].v3 = value.toString();
        }
        break;
    case 4:
        {
            m_values[index.row()].v4memo = value.toString();
        }
        break;
    }

    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags HthmlManipulationModel::flags(const QModelIndex &index) const
{  
    if (!index.isValid())
        return Qt::ItemFlags();

    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}                

