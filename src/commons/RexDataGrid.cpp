
#include <QStandardItemModel>
#include "RexDataGrid.h"
#include "RexDataGridModel.h"
#include "RSLogger.h"
#include "IRSGraphView.h"
#include "Signaler.h"
#include <QLayout>
#include <QFileDialog>
#include <QTableView>


struct RexDataGridData
{
    RexDataGridData() : grid(0) ,dataGridModel(0),displayBand(true)
    {}
    QModelIndex selectedRowIndex;
    Qtitan::Grid* grid;
    QTableView* tableView;
    DataGridModel* dataGridModel;
    QHash<QAbstractItemModel*, QPair<int, Qtitan::SortOrder> > modelSortingHash;
    QWidget* graphView;
    Ui::RexDataGrid ui;
    bool displayBand;

};

RexDataGrid::RexDataGrid(bool displayBand,QWidget *parent): QWidget(parent) ,d(new RexDataGridData())
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    //d = new RexDataGridData();
    d->dataGridModel = new DataGridModel(this);

    d->displayBand = displayBand;

    d->ui.setupUi(this);

    d->graphView = d->ui.dataGrid;
    d->ui.progressBar->setStyleSheet("QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #78d,stop: 0.4999 #46a,stop: 0.5 #45a,stop: 1 lightblue );}");

    QVBoxLayout* gridLayout = new QVBoxLayout();
    d->graphView->setLayout(gridLayout);


    d->grid = initQtitanDataGrid(d->graphView);
    gridLayout->addWidget(d->grid);

    if(d->grid && d->dataGridModel)
        setQTitanModel(d->grid,d->dataGridModel);


    QObject::connect(d->ui.m_runReportsCalculations,SIGNAL(clicked(bool)),Signaler::instance(), SIGNAL(signal_runReportsCalculations()));

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

RexDataGrid::~RexDataGrid()
{
     RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
    if(d)
        delete d;
}

Ui::RexDataGrid& RexDataGrid::ui() const
{
    return d->ui;
}

const Qtitan::Grid* RexDataGrid::grid() const
{
    return d->grid;
}

const DataGridModel*  RexDataGrid::qTitanGridModel()  const
{
    DataGridModel* qTitanModel = 0;
    if(d->grid)
    {
        Qtitan::GridTableView* view = d->grid->view<Qtitan::GridTableView>();
        qTitanModel = dynamic_cast<DataGridModel*>(view->model());
    }
    return qTitanModel;
}

void RexDataGrid::onFocusRowChanged(int oldRow, int newRow)
{
    Q_UNUSED(oldRow)
    QModelIndex  idx = d->dataGridModel->index(newRow, 0);
    if(idx.isValid())
    {
        d->selectedRowIndex = idx;
        emit currentRowChanged(idx);
    }
}

void RexDataGrid::slotTitanGridRowClicked(RowClickEventArgs* args)
{
    if(!d->dataGridModel)
    {
        return;
    }

    // int rowModelIndex = args->row().modelRowIndex();
    // QModelIndex idx =  d->dataGridModel->index(rowModelIndex, 0);

    QModelIndex idx = args->row().modelIndex(0); // QTitanDatagrid 9.0
    if (!idx.isValid())
        return;

    d->selectedRowIndex = idx;
    emit currentRowChanged(idx);
}


Qtitan::GridTableView* RexDataGrid::view()const
{
    Qtitan::GridTableView* view = 0;
    if(d->grid)
        view = d->grid->view<Qtitan::GridTableView>();
    return view;
}

bool RexDataGrid::setQTitanModel(Qtitan::Grid *pGrid, DataGridModel* model)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    if(!pGrid)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End. pGrid = NULL");
        return false;
    }

    if(!model)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End. model = NULL");
        return false;
    }

    Qtitan::GridTableView* view = pGrid->view<Qtitan::GridTableView>();
    if(!view)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End Failed to initialize   Qtitan::GridTableView");
        return false;
    }

    RSLogger::instance()->info(Q_FUNC_INFO,"set model");
    view->setModel(model);

    RSLogger::instance()->info(Q_FUNC_INFO,"set options");
    view->options().setGridLines((Qtitan::LinesBoth));

    // view->options().setShowFocusDecoration(true); // ancien
    Qtitan::GridViewOptions& opt = view->options(); // Qtitandatagrid 9
    opt.setFocusFrameEnabled(true);      // affiche le cadre de focus
    opt.setKeepFocusedRow(true);         // (optionnel) garde la ligne focus après refresh
    pGrid->setStyleSheet("Qtitan--Grid:focus { outline: 3px dotted blue; }");

    view->options().setZoomEnabled(true);
    // view->options().setColumnAutoWidth(true);
    view->tableOptions().setColumnAutoWidth(true);



    if(d->displayBand)
        view->options().setGroupsHeader(true);
    else
        view->options().setGroupsHeader(false);

    RSLogger::instance()->info(Q_FUNC_INFO,"connect signals");
    disconnect(view, &Qtitan::GridTableView::focusRowChanged, this, &RexDataGrid::onFocusRowChanged);
    disconnect(view, &Qtitan::GridTableView::rowClicked,this, &RexDataGrid::slotTitanGridRowClicked);
    connect(view, &Qtitan::GridTableView::focusRowChanged, this, &RexDataGrid::onFocusRowChanged);
    connect(view, &Qtitan::GridTableView::rowClicked,this, &RexDataGrid::slotTitanGridRowClicked);

    RSLogger::instance()->info(Q_FUNC_INFO,"set alignments");
    for (int col = 0; col < model->columnCount(); col++)
    {
        Qtitan::GridTableColumn* column = (Qtitan::GridTableColumn *)view->getColumn(col);
        if(col <= 1)
            column->setTextAlignment(Qt::AlignLeft);
        else
            column->setTextAlignment(Qt::AlignCenter);
    }



    RSLogger::instance()->info(Q_FUNC_INFO,"End");
    return true;
}


bool RexDataGrid::setTableViewModel( QTableView* pGrid, DataGridModel* model)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    if(!pGrid)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End. pGrid = NULL");
        return false;
    }

    if(!model)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End. model = NULL");
        return false;
    }

    d->tableView->setModel(model);

    return true;
}

Qtitan::Grid *RexDataGrid::initQtitanDataGrid(QWidget* parent)
{   
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");
    if(!parent)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End parent is NULL. Cannot to initialize  Qtitan::Grid");
        return NULL;
    }

    Qtitan::Grid* pGrid = new Qtitan::Grid(parent);
    if(!pGrid)
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End Failed to initialize  Qtitan::Grid");
        return NULL;
    }
    pGrid->setViewType(Qtitan::Grid::GridViewType::TableView);

    pGrid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pGrid->installEventFilter(this);

    RSLogger::instance()->info(Q_FUNC_INFO,"End");

    return pGrid;
}

void RexDataGrid::selectMultiCellSelection(bool val)
{
    Qtitan::GridTableView* view = d->grid->view<Qtitan::GridTableView>();
    if(view)
    {
        if(val)
            view->options().setSelectionPolicy(GridViewOptions::MultiCellSelection);
        else
            view->options().setSelectionPolicy(GridViewOptions::MultiRowSelection);
        view->modelController()->clearSelect();
        this->update();
    }
}

bool RexDataGrid::eventFilter(QObject *obj, QEvent *e)
{
    Q_UNUSED(obj)

    if(e->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = (QKeyEvent*)e;
        if(keyEvent && keyEvent->key() == Qt::Key_C && keyEvent->modifiers() == Qt::ControlModifier)
        {
            return true;
        }
        if(keyEvent && keyEvent->key() == Qt::Key_Escape)
        {
            return true;
        }
    }

    return false;
}


bool RexDataGrid::applySortingOrderForModel(QAbstractItemModel* model)
{
    if(!model)
        return false;

    Qtitan::GridTableView* view = d->grid->view<Qtitan::GridTableView>();
    if(!view)
        return false;

    QPair<int, Qtitan::SortOrder> sortingPair = d->modelSortingHash.value(model);
    Qtitan::GridTableColumn* column = (Qtitan::GridTableColumn *)view->getColumn(sortingPair.first);
    if(!column)
        return false;

    column->setSortOrder(sortingPair.second);
    return true;
}


bool RexDataGrid::setDataModelMatrix(const QStringList& fields, const QList<QVariantList>& data)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start. fields :" + fields.join(","));

    if(!d->dataGridModel)
    {
        d->dataGridModel = new DataGridModel(this);
        RSLogger::instance()->info(Q_FUNC_INFO,"init model");
    }
    d->dataGridModel->setMatrix(fields, data);


    if(!d->grid)
        d->grid = initQtitanDataGrid(d->graphView);

    setQTitanModel(d->grid,d->dataGridModel);

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
    return true;

}



void RexDataGrid::setProgressBarValue(int value)
{
    d->ui.progressBar->setValue(value);
}


QProgressBar* RexDataGrid::progressBar()
{
    return d->ui.progressBar;
}

const QStringList&  RexDataGrid::getFields() const
{
    return d->dataGridModel->fieldsList();
}

const QList<QVariantList>&  RexDataGrid::getData() const
{
    return d->dataGridModel->dataMatrix();
}

const DataGridModel*  RexDataGrid::model() const
{
    return d->dataGridModel;
}


void RexDataGrid::clearData()
{
    d->dataGridModel->clearData();
    setDataModelMatrix(d->dataGridModel->fieldsList(),QList<QVariantList>());
}

void  RexDataGrid::clearGrid()
{
    d->dataGridModel->clearMatrix();
    setDataModelMatrix(QStringList(),QList<QVariantList>());
}

void  RexDataGrid::setFieldDecimals(const QString& field, int prec)
{

}
