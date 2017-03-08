
#include <QtGui>
#include <QDirModel>
#include <QDesktopServices>
#include <QMessageBox>

#include "window.h"

static void add_model_item(QAbstractItemModel* model, const QPixmap& photo, const QString& name, int titleIndex, 
    int countryIndex, const QString& city, const QString& code, const QString& geoLocation)
{
    int row = model->rowCount();
    model->insertRow(row);

    model->setData(model->index(row, 0), photo);
    model->setData(model->index(row, 1), name);
    model->setData(model->index(row, 2), titleIndex);
    model->setData(model->index(row, 3), countryIndex);
    model->setData(model->index(row, 4), city);
    model->setData(model->index(row, 5), code);
    QString link = QString("<a href='http://www.geonames.org/maps/google_%1.html'>%2</a>").arg(geoLocation).arg(geoLocation);
    model->setData(model->index(row, 6), link);

    const QString previewText = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam sodales, \
                                libero at hendrerit commodo, urna velit vestibulum purus, cursus dignissim tellus ante non leo. \
                                Donec sed neque sit amet mauris iaculis varius ac eu ipsum. <b>Proin vel hendrerit magna</b>. \
                                Curabitur posuere ligula eu turpis porttitor faucibus rhoncus neque pretium. Duis quam lorem, bibendum id luctus eu, consequat at purus. \
                                Aenean ut urna felis.";
    //model->setData(model->index(0, 0), previewText, Qt::WhatsThisRole);
}

static QAbstractItemModel* create_model(QObject *parent)
{
    QStandardItemModel *model = new QStandardItemModel(0, 7, parent);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Photo"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Name"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Title"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Country"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("City"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Postal Code"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Location"));

    add_model_item(model, QPixmap(":res/photo2.png"), "Dietrich Wolf", 0/*Founder*/, 0/*Germany*/, "Berlin", "10245", "52.517_13.4");
    add_model_item(model, QPixmap(":res/photo1.png"), "Albert Schneider", 1/*President*/, 0/*Germany*/, "Berlin", "10178", "52.523_13.414");
    add_model_item(model, QPixmap(":res/photo3.png"), "Adelina Adams", 3/*Sales Manager*/, 0/*Germany*/, "Berlin", "10707", "52.517_13.4");
    add_model_item(model, QPixmap(":res/photo4.png"), "Bertrand Maison", 4/*Software Engineer*/, 1/*France*/, "Paris", "75003", "48.864_2.361");
    add_model_item(model, QPixmap(":res/photo5.png"), "Anna Laroche", 3/*Sales Manager*/, 1/*France*/, "Saint-Bernard", "68720", "47.667_7.2");
    add_model_item(model, QPixmap(":res/photo6.png"), "Dominik Malet", 4/*Software Engineer*/, 1/*France*/, "Pulversheim", "68840", "47.838_7.301");
    add_model_item(model, QPixmap(":res/photo7.png"), "Berta Padovano", 2/*Vice President*/, 2/*Italy*/, "Milano", "20121", "45.464_9.19");
    add_model_item(model, QPixmap(":res/photo8.png"), "Andrew Armstrong", 5/*Software Developer*/, 2/*Italy*/, "Modena", "41100", "44.648_10.925");
    add_model_item(model, QPixmap(":res/photo9.png"), "Caterina Venezia", 6/*Head of Sales*/, 2/*Italy*/, "Roma", "00136", "41.892_12.511");
    add_model_item(model, QPixmap(":res/photo10.png"), "Sophia Smith", 7/*Technical Support*/, 3/*United States*/, "Boston", "02115", "42.343_-71.092");
    add_model_item(model, QPixmap(":res/photo11.png"), "Jacob Morris", 3/*Software Engineer*/, 3/*United States*/, "New York", "10001", "40.748_-73.997");
    add_model_item(model, QPixmap(":res/photo12.png"), "Andrew Fisher", 5/*Software Developer*/, 3/*United States*/, "Miami Beach", "33140", "25.82_-80.134");
    add_model_item(model, QPixmap(":res/photo13.png"), "Isabella Henderson", 7/*Technical Support*/, 4/*United Kingdom*/, "Manchester", "M60", "53.481_-2.237");
    add_model_item(model, QPixmap(":res/photo14.png"), "Olivia Hewitt", 7/*Technical Support*/, 4/*United Kingdom*/, "Richmond", "TW9", "51.46_-0.301");
    add_model_item(model, QPixmap(":res/photo15.png"), "Daniel Fuller", 8/*Administration Manager*/, 4/*United Kingdom*/, "Notting Hill", "W11", "51.511_-0.206");

    
    return model;
}

Window::Window()
: DemoMainWindow("QtitanDataGrid", QTN_VERSION_DATAGRID_STR)
{
    setWindowTitle(tr("QtitanDataGrid - CardView add new row demo"));
    setGeometry(150, 150, 1000, 800);

    Grid::loadTranslation();

    m_grid = new Qtitan::Grid();
    QAbstractItemModel* model = create_model(m_grid);

    // Configure grid view
    m_grid->setViewType(Qtitan::Grid::CardView);
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setNewRowPlace(Qtitan::NewRowTop);

    //Connect Grid's context menu handler.
    connect(view, SIGNAL(contextMenu(ContextMenuEventArgs*)), this, SLOT(contextMenu(ContextMenuEventArgs* )));
    connect(view, SIGNAL(editorLinkActivated(EditorLinkEventArgs*)), this, SLOT(editorLinkActivated(EditorLinkEventArgs*)));

    view->setModel(model);

    GridCardColumn* column = static_cast<GridCardColumn *>(view->getColumnByModelColumnName("Photo"));
    column->setEditorType(PictureEditorType);
    column->setRowSpan(4);
    QVariant defValue = GridPictureEditorRepository::convertToVariant(QVariant::ByteArray, QPixmap(":res/new_user.png"));
    column->editorRepository()->setDefaultValue(defValue, Qt::DisplayRole);
    column->editorRepository()->setDefaultValue(defValue, Qt::EditRole);

    column = static_cast<GridCardColumn *>(view->getColumnByModelColumnName("Title"));
    column->setEditorType(ComboBoxEditorType);
    QStringListModel* titleModel = new QStringListModel(column);
    QStringList options;
    options << "Founder" << "President" << "Vice President" << "Sales Manager" << "Software Engineer" << "Software Developer" << "Head of Sales" << "Technical Support" << "Administration Manager";
    titleModel->setStringList(options);
    column->dataBinding()->setRelationModel(titleModel);

    column = static_cast<GridCardColumn *>(view->getColumnByModelColumnName("Country"));
    column->setEditorType(ComboBoxEditorType);
    QStringListModel* countryModel = new QStringListModel(column);
    options.clear();
    options << "Germany" << "France" << "Italy" << "United States" << "United Kingdom" << "Russia" << "China" << "Canada";
    countryModel->setStringList(options);
    column->dataBinding()->setRelationModel(countryModel);

    column = static_cast<GridCardColumn *>(view->getColumnByModelColumnName("Location"));
    column->setEditorType(MemoEditorType);
    static_cast<GridMemoEditorRepository *>(column->editorRepository())->setHTML(true);
    
    //Show button menu for all column headers.
    for (int i = 0; i < view->getColumnCount(); ++i)
        static_cast<GridTableColumn *>(view->getColumn(i))->setMenuButtonVisible(true);

    setDemoWidget(m_grid, createSettingsWidget());

    view->bestFit();
#if 0
    addPreviewRow();
#endif
}

QWidget* Window::createSettingsWidget()
{
    //Create settings widget
    QWidget* settings = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(settings);

    QLabel* placeLabel = new QLabel(settings);
    placeLabel->setText(tr("New Card pane place:"));
    QComboBox* newRowPosition = new QComboBox(settings);
    newRowPosition->addItem("Top");
    newRowPosition->addItem("Bottom");
    newRowPosition->addItem("Hide");
    connect(newRowPosition, SIGNAL(activated(int)), this, SLOT(newRowPositionActivated(int)));
    placeLabel->setBuddy(newRowPosition);
    QHBoxLayout* hl = new QHBoxLayout(0);
    hl->addWidget(placeLabel);
    hl->addWidget(newRowPosition);
    l->addLayout(hl);

    QLabel* effectLabel = new QLabel(settings);
    effectLabel->setText(tr("Highlight effect:"));
    QComboBox* newRowEffect = new QComboBox(settings);
    newRowEffect->addItem("Flash");
    newRowEffect->addItem("Alpha");
    connect(newRowEffect, SIGNAL(activated(int)), this, SLOT(newRowEffectActivated(int)));
    effectLabel->setBuddy(newRowEffect);
    hl = new QHBoxLayout(0);
    hl->addWidget(effectLabel);
    hl->addWidget(newRowEffect);
    l->addLayout(hl);

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
    hl = new QHBoxLayout(0);
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
    lineStylesSelect->setCurrentIndex(3);

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
    connect(m_grid->view<Qtitan::GridCardView>(), SIGNAL(zoomChanged(int)), zoomSlider, SLOT(setValue(int)));
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
#if 0
    QCheckBox* previewExpandButtonBox = new QCheckBox(settings);
    previewExpandButtonBox->setText(tr("Preview Expand Button (new)"));
    connect(previewExpandButtonBox, SIGNAL(stateChanged(int)), this, SLOT(previewExpandButtonEnabled(int)));
    previewExpandButtonBox->setChecked(true);
    l->addWidget(previewExpandButtonBox);
#endif
    QCheckBox* transparentBox = new QCheckBox(settings);
    transparentBox->setText(tr("Transparent Background"));
    connect(transparentBox, SIGNAL(stateChanged(int)), this, SLOT(transparentBackgroundEnabled(int)));
    transparentBox->setChecked(false);
    l->addWidget(transparentBox);

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

    QPushButton* printButton = new QPushButton(settings);
    printButton->setText(tr("Print Preview"));
    connect(printButton, SIGNAL(clicked()), this, SLOT(printPreview()));
    l->addWidget(printButton);
    return settings;
}


void Window::newRowPositionActivated(int index)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();

    switch (index)
    {
    case 0:
        view->options().setNewRowPlace(Qtitan::NewRowTop);
        break;
    case 1:
        view->options().setNewRowPlace(Qtitan::NewRowBottom);
        break;
    default:
        view->options().setNewRowPlace(Qtitan::NewRowNone);
    }
}


void Window::newRowEffectActivated(int index)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();

    switch (index)
    {
    case 0:
        view->options().setNewRowHighlightEffect(Qtitan::RowFlashEffect);
        break;
    case 1:
        view->options().setNewRowHighlightEffect(Qtitan::RowAlphaEffect);
        break;
    default:
        view->options().setNewRowHighlightEffect(Qtitan::RowAlphaEffect);
    }
}

void Window::fastScrollChanged(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setFastScrollEffect(state == Qt::Checked);
}

void Window::dottedLineChanged(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    QPen pen = view->options().gridLinePen();
    pen.setStyle(state == Qt::Checked ? Qt::DotLine : Qt::SolidLine);
    view->options().setGridLinePen(pen);
}

void Window::selectGridLineStyles(int index)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
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
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    switch (index)
    {
    case 0:
        {
            view->options().setViewOrientation(Qt::Vertical);
            view->options().setPreviewRowHeight(100);
        }
        break;
    case 1:
        {
            view->options().setViewOrientation(Qt::Horizontal);
            view->options().setPreviewRowHeight(200);
        }
        break;
    default:
        break;
    }
}

void Window::zoomEnabledChanged(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setZoomEnabled(state == Qt::Checked);
}

void Window::zoomIndicatorChanged(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setZoomIndicatorActive(state == Qt::Checked);
}

void Window::zoomValueChanged(int value)
{
    double factor = qCeil((double)value / 25) * 25;
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setZoomFactor(factor / 100);
}

void Window::WindowsAirSupportChanged(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setWindowsAirSupported(state == Qt::Checked);
}

void Window::cellButtonAutoRaiseEnabled(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setCellButtonAutoRaise(state == Qt::Checked);
}

void Window::transparentBackgroundEnabled(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setTransparentBackground(state == Qt::Checked);

    view->options().setAlternatingRowColors(!view->options().alternatingRowColors());
}

void Window::previewExpandButtonEnabled(int state)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setPreviewExpandButtonVisible(state == Qt::Checked);
}

void Window::contextMenu(ContextMenuEventArgs* args)
{
    args->contextMenu()->addAction("Print Preview", this, SLOT(printPreview()));
    args->contextMenu()->addSeparator();
    args->contextMenu()->addAction("Developer Machines on the Web", this, SLOT(showCompanyWebSite()));
}

void Window::editorLinkActivated(EditorLinkEventArgs* args)
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    if (!view->isEditing())
        QDesktopServices::openUrl(QUrl(args->anchor()));
}

void Window::cellButtonClicked(CellButtonClickEventArgs* args)
{
    QMessageBox::information(this, tr("Cell button clicked"), 
        tr("Clicked: Button - %1, Column Title - %2, RowIndex - %3").arg(args->buttonIndex()).arg(args->column()->caption()).arg(args->row()->rowIndex()));
}

void Window::printPreview()
{
    m_grid->view<Qtitan::GridCardView>()->printPreview();
}

void Window::showCompanyWebSite()
{
    QDesktopServices::openUrl(QUrl("http://www.devmachines.com"));
}

void Window::setShadeColor(const QColor& color)
{
    m_grid->themeManager()->setShadeColor(color);
}

void Window::addPreviewRow()
{
    Qtitan::GridCardView* view = m_grid->view<Qtitan::GridCardView>();
    view->options().setPreviewRowEnabled(true);
    view->modelController()->previewRowDataBinding()->setDisplayRole(Qt::WhatsThisRole);
    view->options().setPreviewRowHeight(100);
}