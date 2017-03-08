
#include <QtGui>
#include <QDirModel>
#include <QDesktopServices>
#include <QMessageBox>

#include "window.h"

void addMail(QAbstractItemModel* model, const QString& subject,
             const QString& from, const QDateTime &date, bool attachment, bool read, int status)
{
    model->insertRow(0);
    QPixmap pm;

    model->setData(model->index(0, 0), pm);
    if (attachment)
    {
        pm = QPixmap(":res/attachmentcell.png");
        pm.setMask(pm.createMaskFromColor(Qt::magenta));
    }
    model->setData(model->index(0, 1), pm);
    pm = QPixmap();
    model->setData(model->index(0, 2), pm);

    if (read)
    {
        pm = QPixmap(":res/mail.png");
        pm.setMask(pm.createMaskFromColor(Qt::magenta));
    }
    else
    {
        pm = QPixmap(":res/mailandarrow.png");
        pm.setMask(pm.createMaskFromColor(Qt::magenta));
    }
    model->setData(model->index(0, 3), pm);

    model->setData(model->index(0, 4), date);
    model->setData(model->index(0, 5), subject);
    model->setData(model->index(0, 6), from);

    if (status == 0)
    {
        pm = QPixmap(":res/flag.png");
        pm.setMask(pm.createMaskFromColor(Qt::magenta));
    }
    else
    {
        pm = QPixmap(":res/flag_red.png");
        pm.setMask(pm.createMaskFromColor(Qt::magenta));
    }

    model->setData(model->index(0, 7), pm);
}

QAbstractItemModel *createMailModel(QObject *parent)
{
    QStandardItemModel *model = new QStandardItemModel(0, 8, parent);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Importance"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Attachment"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Reminder"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Read"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Created"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Subject"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("From"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Status"));

    addMail(model, "Happy New Year!", "Grace K. <grace@software-inc.com>",
        QDateTime(QDate(2015, 12, 31), QTime(17, 03)), false, true, 0);
    addMail(model, "Radically new concept", "Grace K. <grace@software-inc.com>",
        QDateTime(QDate(2014, 12, 22), QTime(9, 44)), true, true, 0);
    addMail(model, "Accounts", "pascale@nospam.com",
        QDateTime(QDate(2014, 12, 31), QTime(12, 50)), false, true, 0);
    addMail(model, "Expenses", "Joe Bloggs <joe@bloggs.com>",
        QDateTime(QDate(2014, 12, 25), QTime(11, 39)), false, false, 0);
    addMail(model, "Re: Expenses", "Andy <andy@nospam.com>",
        QDateTime(QDate(2015, 01, 02), QTime(16, 05)), false, true, 1);
    addMail(model, "Re: Accounts", "Joe Bloggs <joe@bloggs.com>",
        QDateTime(QDate(2015, 01, 03), QTime(14, 18)), false, false, 0);
    addMail(model, "Re: Accounts", "Andy <andy@nospam.com>",
        QDateTime(QDate(2015, 01, 03), QTime(14, 26)), true, false, 0);
    addMail(model, "Sports", "Linda Smith <linda.smith@nospam.com>",
        QDateTime(QDate(2013, 01, 05), QTime(11, 33)), false, true, 1);
    addMail(model, "AW: Sports", "Rolf Newschweinstein <rolfn@nospam.com>",
        QDateTime(QDate(2014, 01, 05), QTime(12, 00)), true, true, 0);
    addMail(model, "RE: Sports", "Petra Schmidt <petras@nospam.com>",
        QDateTime(QDate(2015, 01, 05), QTime(12, 01)), false, true, 0);

    return model;
}

Window::Window()
: DemoMainWindow("QtitanDataGrid", QTN_VERSION_DATAGRID_STR)
{
    setWindowTitle(tr("QtitanDataGrid Demo (MS-Office row grouping)"));
    setGeometry(150, 150, 1000, 600);

    Grid::loadTranslation();

    m_grid = new Qtitan::Grid();
    QAbstractItemModel* model = createMailModel(m_grid);

    // Configure grid view
    m_grid->setViewType(Qtitan::Grid::TableView);
    Qtitan::GridTableView* view = m_grid->view<Qtitan::GridTableView>();
    view->options().setModelDecoration(false);
    view->options().setGroupRowStyle(GridGroupOfficeOutlook);
    view->options().setShowFocusDecoration(true);

    //Connect Grid's context menu handler.
    connect(view, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs* )));

    view->setModel(model);

    Qtitan::GridTableColumn* column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Subject");
    //Add cell button to the column.
    column->addButton(Qtitan::TextButton, Qtitan::LeftPosition);
    connect(column, SIGNAL(buttonClicked(CellButtonClickEventArgs*)), this, SLOT(cellButtonClicked(CellButtonClickEventArgs*)));

    QPixmap pm;
    pm = QPixmap(":res/attachment.png");
    pm.setMask(pm.createMaskFromColor(Qt::magenta));
    column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Attachment");
    column->setIcon(pm);
    column->setIconApperance(Qtitan::IconOnly);
    column->setMaxWidth(20);
    column->setMinWidth(20);
    column->setFilterButtonVisible(false);
    column->setMenuButtonVisible(false);
    column->setTextAlignment(Qt::AlignCenter);
    column->setHorSizingEnabled(false);
    column->setEditorType(Qtitan::PictureEditorType);
    column->editorRepository()->setEditable(false);
    column->editorRepository()->setAlignment(Qt::AlignCenter);
    ((GridPictureEditorRepository *)column->editorRepository())->setScaled(false);

    pm = QPixmap(":res/importance.png");
    pm.setMask(pm.createMaskFromColor(Qt::magenta));
    column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Importance");
    column->setIcon(pm);
    column->setIconApperance(Qtitan::IconOnly);
    column->setMaxWidth(20);
    column->setMinWidth(20);
    column->setFilterButtonVisible(false);
    column->setMenuButtonVisible(false);
    column->setTextAlignment(Qt::AlignCenter);
    column->setHorSizingEnabled(false);
    column->setEditorType(Qtitan::PictureEditorType);
    column->editorRepository()->setEditable(false);
    column->editorRepository()->setAlignment(Qt::AlignCenter);
    ((GridPictureEditorRepository *)column->editorRepository())->setScaled(false);

    pm = QPixmap(":res/reminder.png");
    pm.setMask(pm.createMaskFromColor(Qt::magenta));
    column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Reminder");
    column->setIcon(pm);
    column->setIconApperance(Qtitan::IconOnly);
    column->setMaxWidth(20);
    column->setMinWidth(20);
    column->setFilterButtonVisible(false);
    column->setMenuButtonVisible(false);
    column->setTextAlignment(Qt::AlignCenter);
    column->setHorSizingEnabled(false);
    column->setEditorType(Qtitan::PictureEditorType);
    column->editorRepository()->setEditable(false);
    column->editorRepository()->setAlignment(Qt::AlignCenter);
    ((GridPictureEditorRepository *)column->editorRepository())->setScaled(false);

    pm = QPixmap(":res/read.png");
    pm.setMask(pm.createMaskFromColor(Qt::magenta));
    column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Read");
    column->setIcon(pm);
    column->setIconApperance(Qtitan::IconOnly);
    column->setMaxWidth(20);
    column->setMinWidth(20);
    column->setFilterButtonVisible(false);
    column->setMenuButtonVisible(false);
    column->setTextAlignment(Qt::AlignCenter);
    column->setHorSizingEnabled(false);
    column->setEditorType(Qtitan::PictureEditorType);
    column->editorRepository()->setEditable(false);
    column->editorRepository()->setAlignment(Qt::AlignCenter);
    ((GridPictureEditorRepository *)column->editorRepository())->setScaled(false);

    pm = QPixmap(":res/status.png");
    pm.setMask(pm.createMaskFromColor(Qt::magenta));
    column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Status");
    column->setIcon(pm);
    column->setIconApperance(Qtitan::IconOnly);
    column->setMaxWidth(20);
    column->setMinWidth(20);
    column->setFilterButtonVisible(false);
    column->setMenuButtonVisible(false);
    column->setHorSizingEnabled(false);
    column->setEditorType(Qtitan::PictureEditorType);
    column->editorRepository()->setEditable(false);
    column->editorRepository()->setAlignment(Qt::AlignCenter);
    ((GridPictureEditorRepository *)column->editorRepository())->setScaled(false);


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
    dottedLineCheck->setChecked(false);

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
    lineStylesSelect->setCurrentIndex(4);

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

void Window::setShadeColor(const QColor& color)
{
    m_grid->themeManager()->setShadeColor(color);
}
