#include <QApplication>
#include <QMdiSubWindow>
#include <QDesktopWidget>
#include <QMdiArea>
#include <QCheckBox>
#include <QSettings>

#include "window.h"


/* CustomDataModel */
CustomDataModel::CustomDataModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_values.resize(100);
    int y = 2012;
    int m = 1;
    int d = 1;
    for (int i = 0; i < m_values.size(); ++i)
    {
        m_values[i].v0 = i;
        m_values[i].v1 = QString("String = %1").arg(i);
        m_values[i].v3 = QTime(12, 0, 0);
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
        m_values[i].v2 = QDate(y, m, d);
        if (!(i % 10))
            d++;
    }
}

QVariant CustomDataModel::headerData(int section, Qt::Orientation orientation,
    int role) const
{
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    switch (section)
    {
    case 0:
    {
        return QString("Integer");
    }
    break;
    case 1:
    {
        return QString("String");
    }
    break;
    case 2:
    {
        return QString("Date");
    }
    break;
    case 3:
    {
        return QString("Time");
    }
    break;
    }
    return QVariant();
}

QModelIndex CustomDataModel::parent(const QModelIndex & /*child*/) const
{
    return QModelIndex();
}

bool CustomDataModel::hasChildren(const QModelIndex &parent) const
{
    if (parent.model() == this || !parent.isValid())
    {
        return rowCount(parent) > 0 && columnCount(parent) > 0;
    }
    return false;
}

int CustomDataModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_values.size();
}

int CustomDataModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 4;
}

QModelIndex CustomDataModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
        return QModelIndex();

    if (row < 0 || row >= rowCount(parent))
        return QModelIndex();

    if (column < 0 || column >= columnCount(parent))
        return QModelIndex();

    return createIndex(row, column, (void*)NULL);
}

QVariant CustomDataModel::data(const QModelIndex &index, int role) const
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

bool CustomDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
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
        m_values[index.row()].v0 = value.toInt();
    }
    break;
    case 1:
    {
        m_values[index.row()].v1 = value.toString();
    }
    break;
    case 2:
    {
        m_values[index.row()].v2 = value.toDate();
    }
    case 3:
    {
        m_values[index.row()].v3 = value.toTime();
    }
    break;
    }

    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags CustomDataModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemFlags();
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}


/* Window */
Window::Window()
    : DemoMainWindow("QtitanDataGrid", QTN_VERSION_DATAGRID_STR)
{
    setWindowTitle("QtitanDataGrid - MDI Application");
    setGeometry(50, 50, 800, 500);

    m_mdiArea = new QMdiArea(this);

    m_mdiArea->setViewMode(QMdiArea::TabbedView);

    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setDemoWidget(m_mdiArea, createSettingsWidget());

    QRect geom = QApplication::desktop()->availableGeometry();
    resize( 2 * geom.width() / 3, 2 * geom.height() / 3 );
}

Window::~Window()
{
}

QWidget* Window::createSettingsWidget()
{
    QWidget* settings = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(settings);

    QHBoxLayout* hl = new QHBoxLayout(0);
    QLabel* label = new QLabel(settings);
    label->setText("MDI view mode:");
    QComboBox* mdiViewModeComboBox = new QComboBox(settings);
    mdiViewModeComboBox->addItem("TabbedView");
    mdiViewModeComboBox->addItem("SubWindowView");
    connect(mdiViewModeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setMDIViewMode(int)));
    hl->addWidget(label);
    hl->addWidget(mdiViewModeComboBox);
    l->addLayout(hl);
    mdiViewModeComboBox->setCurrentIndex(0);

    QPushButton* addGridButton = new QPushButton(settings);
    addGridButton->setText(tr("Add grid"));
    connect(addGridButton, SIGNAL(clicked()), this, SLOT(addGrid()));
    l->addWidget(addGridButton);

    QPushButton* removeGridButton = new QPushButton(settings);
    removeGridButton->setText(tr("Remove grid"));
    connect(removeGridButton, SIGNAL(clicked()), this, SLOT(removeGrid()));
    l->addWidget(removeGridButton);

    return settings;
}

void Window::setShadeColor(const QColor& color)
{
    QList<QMdiSubWindow *> windowList = m_mdiArea->subWindowList();
    for (int i = 0; i < windowList.size(); ++i)
    {
        QMdiSubWindow* subWindow = windowList[i];
        Grid* grid = static_cast<Grid *>(subWindow->widget());
        grid->themeManager()->setShadeColor(color);
    }
}

void Window::addGrid()
{
    Grid* grid = new Grid();

    // Configure grid view
    grid->setViewType(Qtitan::Grid::TableView);
    Qtitan::GridTableView* view = grid->view<Qtitan::GridTableView>();
    view->options().setGridLineWidth(0);
    view->options().setColumnAutoWidth(true);
    view->options().setSelectionPolicy(Qtitan::SelectMultiRow);

    CustomDataModel* model = new CustomDataModel(grid);
    view->setModel(model);
    QString title = QString("Window %1").arg(m_mdiArea->subWindowList().size());
    QMdiSubWindow* subWindow = m_mdiArea->addSubWindow(grid);
    subWindow->setWindowTitle(title);
    subWindow->show();
}

void Window::removeGrid()
{
    QList<QMdiSubWindow *> windowList = m_mdiArea->subWindowList();
    if (windowList.size() == 0)
        return;
    QMdiSubWindow* subWindow = windowList.last();
    subWindow->close();
    subWindow->deleteLater();
}

void Window::setMDIViewMode(int index)
{
    m_mdiArea->setViewMode(index == 0 ? QMdiArea::TabbedView : QMdiArea::SubWindowView);
}

void Window::subWindowActivated(QMdiSubWindow* subWindow)
{
    Q_UNUSED(subWindow);
}
