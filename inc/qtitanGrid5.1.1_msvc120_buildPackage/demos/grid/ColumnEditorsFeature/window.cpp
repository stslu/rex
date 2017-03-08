
#include <QtGui>
#include <QDirModel>
#include <QDesktopServices>
#include <QMessageBox>

#include "window.h"


Window::Window()
: DemoMainWindow("QtitanDataGrid", QTN_VERSION_DATAGRID_STR)
{
    setWindowTitle(tr("QtitanDataGrid Column Editor Demo (Banded table view)"));
    setGeometry(150, 150, 1000, 800);

    Grid::loadTranslation();

    m_grid = new Qtitan::Grid();
    ColumnEditorDemoModel* model = new ColumnEditorDemoModel(m_grid);

    // Configure grid view
    m_grid->setViewType(Qtitan::Grid::BandedTableView);
    Qtitan::GridBandedTableView* view = m_grid->view<Qtitan::GridBandedTableView>();
    view->options().setBandHeight(300);
    view->options().setModelDecoration(true);

    //Connect Grid's context menu handler.
    connect(view, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs* )));

    Qtitan::GridTableBand* engineeringBand = view->addBand("Engineering");
    Qtitan::GridTableBand* characteristics1 = view->addBand("Characteristics 1");
    Qtitan::GridTableBand* characteristics2 = view->addBand("Characteristics 2");
    Qtitan::GridTableBand* characteristics3 = view->addBand("Characteristics 3");
    GridChartColumnEditor* chartEditor = new GridChartColumnEditor();
    characteristics1->setColumnEditor(chartEditor);
    characteristics2->setColumnEditor(chartEditor);
    characteristics3->setColumnEditor(chartEditor);
    engineeringBand->setFixedKind(Qtitan::FixedLeft);
    view->setModel(model);

    Qtitan::GridBandedTableColumn* column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 0");
    column->setBandIndex(engineeringBand->index());
    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 1");
    column->setBandIndex(engineeringBand->index());
    //Add cell button to the column.
    column->addButton(Qtitan::ClearButton);
    connect(column, SIGNAL(buttonClicked(CellButtonClickEventArgs*)), this, SLOT(cellButtonClicked(CellButtonClickEventArgs*)));

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 2");
    column->setBandIndex(engineeringBand->index());
    //Add cell button to the column.
    column->addButton(Qtitan::ChoiceButton);
    connect(column, SIGNAL(buttonClicked(CellButtonClickEventArgs*)), this, SLOT(cellButtonClicked(CellButtonClickEventArgs*)));

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 3");
    column->setBandIndex(characteristics1->index());
    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 4");
    column->setBandIndex(characteristics1->index());
    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 5");
    column->setBandIndex(characteristics1->index());

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 6");
    column->setBandIndex(characteristics2->index());
    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 7");
    column->setBandIndex(characteristics2->index());
    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 8");
    column->setBandIndex(characteristics2->index());

    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 9");
    column->setBandIndex(characteristics3->index());
    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 10");
    column->setBandIndex(characteristics3->index());
    column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 11");
    column->setBandIndex(characteristics3->index());

    setDemoWidget(m_grid, createSettingsWidget());
}

QWidget* Window::createSettingsWidget()
{
    //Create settings widget
    QWidget* settings = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(settings);
    QCheckBox* autoWidthCheck = new QCheckBox(settings);
    autoWidthCheck->setText("Column auto width");
    connect(autoWidthCheck, SIGNAL(stateChanged(int)), this, SLOT(autoWidthStateChanged(int)));
    l->addWidget(autoWidthCheck);
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
    dottedLineCheck->setChecked(true);

    QLabel* label = new QLabel(this);
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
#if 0
    label = new QLabel(this);
    hl = new QHBoxLayout(0);
    label->setText(tr("View Orientation (new)"));
    QComboBox* viewOrientationSelect = new QComboBox(settings);
    viewOrientationSelect->addItem("Vertical");
    viewOrientationSelect->addItem("Horizontal");
    connect(viewOrientationSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(selectViewOrientation(int)));
    hl->addWidget(label);
    hl->addWidget(viewOrientationSelect);
    l->addLayout(hl);
    viewOrientationSelect->setCurrentIndex(0);
#endif
    QPushButton* printButton = new QPushButton(settings);
    printButton->setText(tr("Print Preview"));
    connect(printButton, SIGNAL(clicked()), this, SLOT(printPreview()));
    l->addWidget(printButton);
    return settings;
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

        Qtitan::GridBandedTableColumn* column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 3");
        column->setRowSpan(1);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 4");
        column->setRowSpan(1);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 5");
        column->setRowSpan(1);

        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 6");
        column->setRowSpan(1);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 7");
        column->setRowSpan(1);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 8");
        column->setRowSpan(1);

        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 9");
        column->setRowSpan(1);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 10");
        column->setRowSpan(1);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 11");
        column->setRowSpan(1);
    }
    break;
    case 1:
    {
        view->options().setViewOrientation(Qt::Horizontal);

        Qtitan::GridBandedTableColumn* column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 3");
        column->setRowSpan(2);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 4");
        column->setRowSpan(2);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 5");
        column->setRowSpan(2);

        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 6");
        column->setRowSpan(2);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 7");
        column->setRowSpan(2);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 8");
        column->setRowSpan(2);

        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 9");
        column->setRowSpan(2);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 10");
        column->setRowSpan(2);
        column = (Qtitan::GridBandedTableColumn *)view->getColumnByModelColumnName("Column 11");
        column->setRowSpan(2);
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

void Window::setShadeColor(const QColor& color)
{
    m_grid->themeManager()->setShadeColor(color);
}

/* GridChartColumnEditor */
GridChartColumnEditor::GridChartColumnEditor()
: Qtitan::GridTableColumnEditor(), m_model(0), m_selectionModel(0)
{
    m_model = new QStandardItemModel(8, 2, this);
    m_model->setHeaderData(0, Qt::Horizontal, tr("Label"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Quantity"));
    m_selectionModel = new QItemSelectionModel(m_model);
}

QWidget* GridChartColumnEditor::createEditorWidget(GridColumnBase* column)
{
    Q_UNUSED(column);
    QAbstractItemView* pieChart = new PieView();
    pieChart->setFrameShape(QFrame::NoFrame);
    pieChart->setModel(m_model);
    loadModel(":/Charts/qtdata.cht");
    pieChart->setSelectionModel(m_selectionModel);
    return pieChart;
}

void GridChartColumnEditor::loadModel(const QString &fileName)
{
    if (!fileName.isEmpty())
    {
        QFile file(fileName);

        if (file.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream stream(&file);
            QString line;

            m_model->removeRows(0, m_model->rowCount(QModelIndex()), QModelIndex());

            int row = 0;
            do {
                line = stream.readLine();
                if (!line.isEmpty()) {

                    m_model->insertRows(row, 1, QModelIndex());

                    QStringList pieces = line.split(",", QString::SkipEmptyParts);
                    m_model->setData(m_model->index(row, 0, QModelIndex()),
                                   pieces.value(0));
                    m_model->setData(m_model->index(row, 1, QModelIndex()),
                                   pieces.value(1));
                    m_model->setData(m_model->index(row, 0, QModelIndex()),
                                   QColor(pieces.value(2)), Qt::DecorationRole);
                    row++;
                }
            } while (!line.isEmpty());

            file.close();
        }
    }
}


/* ColumnEditorDemoModel */
ColumnEditorDemoModel::ColumnEditorDemoModel(QObject *parent) 
: QAbstractItemModel(parent)
{
} 

QVariant ColumnEditorDemoModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const
{
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    return QString("Column %1").arg(section);
}

QModelIndex ColumnEditorDemoModel::parent(const QModelIndex & /*child*/) const 
{
    return QModelIndex();
} 

bool ColumnEditorDemoModel::hasChildren(const QModelIndex &parent) const 
{  
    if (parent.model() == this || !parent.isValid()) 
    {
        return rowCount(parent) > 0 && columnCount(parent) > 0;
    }
    return false;
} 

int ColumnEditorDemoModel::rowCount(const QModelIndex &parent) const 
{
    if (parent.isValid())
        return 0;
    return 1000;
} 

int ColumnEditorDemoModel::columnCount(const QModelIndex &parent) const 
{
    if (parent.isValid())
        return 0;
    return 12;
}

QModelIndex ColumnEditorDemoModel::index(int row, int column, const QModelIndex &parent) const 
{
    if (parent.isValid())                                   
        return QModelIndex();

    if (row < 0 || row >= rowCount(parent))
        return QModelIndex();

    if (column < 0 || column >= columnCount(parent)) 
        return QModelIndex();

    return createIndex(row, column, (void*)NULL);
}

QVariant ColumnEditorDemoModel::data(const QModelIndex &index, int role) const 
{  
    if (!index.isValid())
        return QVariant();
    
    if (index.row() < 0 || index.row() >= rowCount(index.parent()))
        return QVariant();
    
    if (index.column() < 0 || index.column() >= columnCount(index.parent()))
        return QVariant();
    
    if (role == Qt::DisplayRole || role == Qt::EditRole)        
    {    
        return index.row();
    }

    if (role == Qt::DecorationRole)
    {
        if (index.column() == 5 && index.row() >= 3 && index.row() < 10)
        {
            return QColor(Qt::red);
        }
        if (index.column() == 7 && index.row() >= 5 && index.row() < 15)
        {
            return QColor(Qt::gray);
        }
    }
    return QVariant();
}

Qt::ItemFlags ColumnEditorDemoModel::flags(const QModelIndex &index) const 
{  
    if (!index.isValid())
        return Qt::ItemFlags();
    return Qt::ItemIsEditable; 
}                
