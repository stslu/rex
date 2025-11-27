#ifndef RexDataGrid_H
#define RexDataGrid_H

#include <DevMachines/QtitanGrid>
#include <QWidget>
#include "IRexDataGrid.h"
#include "ui_RexDataGrid.h"

class QAbstractItemModel;
struct RexDataGridData;
class IRSGraphView;
class DataGridModel;
class QTableView;

class RexDataGrid : public QWidget,public IRexDataGrid
{
    Q_OBJECT

public:
    //!explicit constructor
    //! @param signaler a pointer to the signaler class
    //! @param the parent object
    explicit RexDataGrid(bool useTableView = true,QWidget *parent = 0);

    virtual ~RexDataGrid();

    //! return a pointer on the grid
    const Qtitan::Grid* grid()  const;
    const DataGridModel* qTitanGridModel()  const;
    bool setDataModelMatrix(const QStringList& fields, const QList<QVariantList>& data);

    //!Export the Grid in Csv or Excel  if the user presses "Export"
    virtual void setProgressBarValue(int) ;
    QProgressBar* progressBar();

    const QStringList& getFields() const ;
    const QList<QVariantList>& getData() const ;
    const DataGridModel* model() const ;
    void clearData();
    void clearGrid();
    void setFieldDecimals(const QString& field, int prec);
    Ui::RexDataGrid& ui() const;
    Qtitan::GridTableView* view()const;


signals:
    void currentRowChanged(QModelIndex);

public slots:
    void selectMultiCellSelection(bool val);
    void onFocusRowChanged(int oldRow,int newRow);
    void slotTitanGridRowClicked(RowClickEventArgs* args);



protected:
    bool applySortingOrderForModel(QAbstractItemModel* model);

    bool eventFilter(QObject *obj, QEvent *e);
    //!Create the  Qtitan::Grid* pointer,
    //! @parame parent the parent widget
    Qtitan::Grid* initQtitanDataGrid(QWidget* parent);

    //!Add the model to the QTitanDataGridn and set the options
    //! @param Qtitan::Grid* a pointer to the Grid
    //! @param ,DataGridModel* a pointer to the model
    //! @return the result  of the operation true (succeeded)/false(failes)
    bool setQTitanModel( Qtitan::Grid*,DataGridModel*);
    bool setTableViewModel( QTableView* pGrid, DataGridModel* model);





private:
    RexDataGridData* d;
};


#endif // RexDataGrid_H
