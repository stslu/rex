
#include <QtGui>
#include <QDirModel>
#include <QtSql>
#include <QDesktopServices>
#include <QMessageBox>

#include "window.h"


Window::Window()
: DemoMainWindow("QtitanDataGrid", QTN_VERSION_DATAGRID_STR)
{
    setWindowTitle(tr("QtitanDataGrid - Relation/MasterDetails modes"));
    setGeometry(250, 250, 800, 500);

    Grid::loadTranslation();

    m_tabs = new QTabWidget(this);
    createRelationGrid();
    createMasterDetailsGrid();

    setDemoWidget(m_tabs, 0);
}

void Window::createRelationGrid()
{
    m_relationGrid = new Qtitan::Grid(this);

    QSqlTableModel* model = new QSqlTableModel(m_relationGrid);
    model->setTable("employee");
    model->select();
    model->setEditStrategy(QSqlTableModel::OnFieldChange);

    // Configure grid view
    m_relationGrid->setViewType(Qtitan::Grid::TableView);
    Qtitan::GridTableView* view = m_relationGrid->view<Qtitan::GridTableView>();
    view->setModel(model);
    view->options().setNewRowPlace(Qtitan::NewRowBottom);
    Qtitan::GridTableColumn* column = (Qtitan::GridTableColumn *)view->getColumnByCaption("id");
    column->editorRepository()->setEditable(true);

    //Connect Grid's context menu handler.
    connect(view, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs* )));

    QSqlTableModel* city = new QSqlTableModel(m_relationGrid);
    city->setTable("city");
    city->select();

    QSqlTableModel* country = new QSqlTableModel(m_relationGrid);
    country->setTable("country");
    country->select();

    column = (Qtitan::GridTableColumn *)view->getColumnByCaption("city");
    column->setEditorType(Qtitan::ComboBoxEditorType);
    column->dataBinding()->setRelationMode(GridModelDataBinding::Relation);
    column->dataBinding()->setRelationModel(city);
    column->dataBinding()->setForeignKey("id");
    column->dataBinding()->setRoleMapping(Qt::DisplayRole, "name");

    column = (Qtitan::GridTableColumn *)view->getColumnByCaption("country");
    column->setEditorType(Qtitan::ComboBoxEditorType);
    column->dataBinding()->setRelationMode(GridModelDataBinding::Relation);
    column->dataBinding()->setRelationModel(country);
    column->dataBinding()->setForeignKey("id");
    column->dataBinding()->setRoleMapping(Qt::DisplayRole, "name");

    m_tabs->insertTab(0, m_relationGrid, tr("Relation Mode"));
}

void Window::createMasterDetailsGrid()
{
    m_masterDetailsGrid = new Qtitan::Grid();

    QString prefix = QApplication::applicationDirPath();
    prefix += "/..";

    QSqlError err;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", QString("database_demo"));

    db.setDatabaseName(prefix + "/demos/data/films.sqlite");
    db.setHostName("");
    db.setPort(-1);
    if (!db.open("", ""))
    {
        err = db.lastError();
        QSqlDatabase::removeDatabase(QString("database_demo"));
        QMessageBox::critical(0, "Demo Error", QString("Error: Can't open database - %1, error - %2").arg(prefix + "/demos/data/films.sqlite").arg(err.text()));
        QApplication::exit(1);
        return;
    }

    QSqlTableModel* model = new QSqlTableModel(m_masterDetailsGrid, db);
    model->setTable("Films");
    model->select();
    if (model->lastError().type() != QSqlError::NoError)
    {
        QMessageBox::critical(0, "Demo Error", "Error: SQL data base is not valid.");
        QApplication::exit(1);
        return;
    }

    QSqlTableModel* countriesModel = new CountriesDemoModel(m_masterDetailsGrid, db);
    countriesModel->setTable("Countries");
    countriesModel->select();
    if (countriesModel->lastError().type() != QSqlError::NoError)
    {
        QMessageBox::critical(0, "Demo Error", "Error: SQL data base is not valid.");
        QApplication::exit(1);
        return;
    }

    // Configure grid view
    m_masterDetailsGrid->setViewType(Qtitan::Grid::TableView);
    Qtitan::GridTableView* view = m_masterDetailsGrid->view<Qtitan::GridTableView>();
    view->options().setBorderWidth(0);
    view->options().setRowAutoHeight(true);
    view->options().setScrollRowStyle(Qtitan::ScrollByPixel);

    connect(view, SIGNAL(editorEditModeChanged(GridEditor *)), this, SLOT(editorModeChanged(GridEditor *)));

    view->setModel(model);

    Qtitan::GridTableColumn* column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Goofs");
    column->setEditorType(Qtitan::MemoEditorType);

    column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Plot");
    column->setEditorType(Qtitan::MemoEditorType);

    column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Photo");
    column->setEditorType(Qtitan::PictureEditorType);

    column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Country");
    column->setEditorType(Qtitan::ComboBoxEditorType);
    column->editorRepository()->setEditorActivationPolicy(Qtitan::AnyEvents);
    column->dataBinding()->setRelationMode(GridModelDataBinding::Relation);
    column->dataBinding()->setRelationModel(countriesModel);
    column->dataBinding()->setForeignKey("id");
    column->dataBinding()->setRoleMapping(Qt::DisplayRole, "Name");
    column->dataBinding()->setRoleMapping(Qt::DecorationRole, "Flag");

    QSqlTableModel* actorsModel = new QSqlTableModel(0, db);
    actorsModel->setTable("Actors");
    actorsModel->select();
    column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Cast");
    column->setEditorType(Qtitan::TableEditorType);
    column->dataBinding()->setRelationMode(GridModelDataBinding::MasterDetailsRelation);
    column->dataBinding()->setRelationModel(actorsModel);
    column->dataBinding()->setForeignKey("film_id");
    view->bestFit();

    m_tabs->insertTab(1, m_masterDetailsGrid, tr("MasterDetails Mode"));
}

void Window::setShadeColor(const QColor& color)
{
    m_relationGrid->themeManager()->setShadeColor(color);
    m_masterDetailsGrid->themeManager()->setShadeColor(color);
}

void Window::editorModeChanged(GridEditor* editor)
{
    if (editor->site()->dataBinding()->columnName() != "Cast")
        return;
    //Customize grid editor here
    Qtitan::TableComboBox* box = qobject_cast<Qtitan::TableComboBox *>(editor->getCellWidget());
    if (box == 0)
        return;
    Qtitan::Grid* grid = box->grid();
    Qtitan::GridTableView* view = grid->view<Qtitan::GridTableView>();
    view->options().setRowAutoHeight(true);
    view->options().setNewRowText(tr("Click here to add new actor"));
    //view->options().setFocusFollowsMouse(true);

    Qtitan::GridTableColumn* column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Actor photo");
    if (column)
        column->setEditorType(Qtitan::PictureEditorType);
}

void Window::contextMenu(ContextMenuEventArgs* args)
{
    args->contextMenu()->addAction("Print Preview", this, SLOT(printPreview()));
    args->contextMenu()->addSeparator();
    args->contextMenu()->addAction("Developer Machines on the Web", this, SLOT(showCompanyWebSite()));
}

void Window::printPreview()
{
    m_masterDetailsGrid->view<Qtitan::GridTableView>()->printPreview();
}

void Window::showCompanyWebSite()
{
    QDesktopServices::openUrl(QUrl("http://www.devmachines.com"));
}

/* CountriesDemoModel */
CountriesDemoModel::CountriesDemoModel(QObject *parent, QSqlDatabase db)
: QSqlTableModel(parent, db)
{
}

QVariant CountriesDemoModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DecorationRole)
    {
        QModelIndex pixmapindex = QSqlTableModel::index(index.row(), 1, index.parent());
        QPixmap pixmap;
        qvariant_to_pixmap(QSqlTableModel::data(pixmapindex, Qt::DisplayRole), pixmap);
        return pixmap;
    }
    return QSqlTableModel::data(index, role);
}
