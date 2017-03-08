/****************************************************************************
**
** Qtitan Library by Developer Machines (DataGrid component for Qt.C++)
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthoriized reproduction, reverse-engineering
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

#include <QApplication>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QStyleHints>
#endif
#include <QHBoxLayout>
#include <QMenu>
#include <QFileDialog>
#include <QImageWriter>
#include <QClipboard>
#include <QPrintDialog>
#include <QStylePainter>
#include <QBitmap>
#include <QBuffer>
#include <QToolTip>
#include <QMimeData>
#include <QAbstractTextDocumentLayout>
#include "QtnGridTableView.h"
#include "QtnGridTableViewBase.h"
#if defined(Q_WS_WIN)
#include "qt_windows.h"
#endif
#include <qmath.h>
#include "QtnGridEditors.h"

QTITAN_USE_NAMESPACE

/*!
\enum GridEditorActivationPolicyFlag
Enumerator GridEditorActivationPolicyFlag describes the policy of activating the edit mode of the cell editor if the cell gets the focus. You can group the policy on your own using logical OR. 
\inmodule QtitanDataGrid
\relates GridEditorRepository

\value NoPolicy Blank value. The editing mode may not be enabled. The cell can not be edited.
\value ClickNoFocused Edit mode of the cell editor is activated if you click on a cell that has no focus.
\value ClickFocused Edit mode of the cell editor is activated if you click on a cell that already has focus.
\value Click Edit mode of the cell editor is activated if you click on a cell. This policy does not depend on whether the cell has focus or not. Editor always activated when was a click on a cell.
\value MousePressNoFocused Edit mode of the cell editor is activated if you press mouse on a cell that has no focus.
\value MousePressFocused Edit mode of the cell editor is activated if you press mouse on a cell that already has focus.
\value MousePress Edit mode of the cell editor is activated if you press mouse on a cell. This policy does not depend on whether the cell has focus or not. Editor always activated when was a mouse press (mouse down) on a cell.
\value DblClickNoFocused Edit mode of the cell editor is activated if you double-click on a cell that has no focus.
\value DblClickFocused Edit mode of the cell editor is activated if you double-click on a cell that already has focus.
\value DblClick Edit mode of the cell editor is activated if you double-click on a cell. This policy does not depend on whether the cell has focus or not. Editor always activated when was a double-click on a cell.
\value KeyboardFocus The cell becomes in the edit state if the focus was acquired by the keyboard (for instance by pressing the navigation buttons Up, Down, Left, Right etc).
\value Enter The cell becomes in the edit state if the Enter key was pressed while cell already has focus and is not in the edit state. 
\value KeyPress The cell becomes in the edit state if the any key was pressed while cell has focus and is not in the edit state. You can filtering the value of the key by using the editorEditingStarting() signal.
\value KeyPress_WithEating The cell becomes in the edit state if the any key was pressed while cell has focus and is not in the edit state. 
        Unlike the KeyPress, KeyPress_WithEating eats first pressed symbol that caused the event. This first pressed symbol does not passed into the cell editor.
        You can filtering the value of the key by using the editorEditingStarting() signal.
\value SelectCurrentEvent grid should be start multi selection for this cell.
\value AnyEvents The cell switch to edit state until the cell has focus.

*/

/*!
\enum GridEditorType
This enumerator describes the possible types of grid cell editors for table view, banded table view, card view. 
\code
Qtitan::GridTableColumn* column = (Qtitan::GridTableColumn *)view->getColumnByModelColumnName("Photo");
column->setEditorType(Qtitan::PictureEditorType);
Qtitan::GridPictureEditorRepository* pictureEditor = (Qtitan::GridPictureEditorRepository *)column->editorRepository();
pictureEditor->setAspectRatioMode(Qt::KeepAspectRatio);
\endcode

\inmodule QtitanDataGrid
\relates GridDataTableColumn

\value AutoEditorType Automatic selection of the editor by edited value. By default.
\value StringEditorType The editor of an unary line. See GridStringEditorRepository for the details.
\value MemoEditorType The editor of multi-line text. See GridMemoEditorRepository for the details.
\value ComboBoxBlobEditorType The editor of multi-line text in popup window. See GridBlobEditorRepository for the details.
\value NumericEditorType The editor of numerical value. See GridNumericEditorRepository for the details.
\value CheckBoxEditorType The editor of boolean value. See GridCheckBoxEditorRepository for the details.
\value ComboBoxEditorType The editor allows to show combo boxes in the grid cell. See GridComboBoxEditorRepository for the details.
\value TableEditorType The editor allows to specify the other datagrid as details for the editing value. Details grid will be shown in the popup window by clicking on the button that embeded to the grid cell.
See GridTableEditorRepository for the details.
\value DateTimeEditorType The editor of DateTime value. See GridDateTimeEditorRepository for the details.
\value TimeEditorType The editor of Time value. See GridTimeEditorRepository for the details.
\value DateEditorType The editor of Date value. See GridDateEditorRepository for the details.
\value PictureEditorType The editor of picturies or images. See GridPictureEditorRepository for the details.
\value ComboBoxPictureEditorType The editor of picturies and images. The pucture will be edited in the popup window like combobox. See GridPictureComboBoxEditorRepository for the details.
\value DelegateAdapterEditorType The type of editor which allows you to use delegates from the regular table QTableView in the QtitanDataGrid. For more details see GridDelegateAdapterEditorRepository or follow this topic - \l{QtitanDataGrid cell editor}
\value ProgressEditorType The editor for showing and editing numeric value. Custom ProgressBar widget is used for this purposes. See GridProgressEditorRepository for the details.
\value CustomEditType The editor of value creating by cutomer. Please read this article for more details - \l{QtitanDataGrid cell editor}
*/

Q_DECLARE_METATYPE(QModelIndex)

#define QDATETIMEEDIT_TIME_MIN QTime(0, 0, 0, 0)
#define QDATETIMEEDIT_TIME_MAX QTime(23, 59, 59, 999)
#define QDATETIMEEDIT_DATE_MIN QDate(1752, 9, 14)
#define QDATETIMEEDIT_DATE_MAX QDate(7999, 12, 31)
#define QDATETIMEEDIT_DATETIME_MIN QDateTime(QDATETIMEEDIT_DATE_MIN, QDATETIMEEDIT_TIME_MIN)
#define QDATETIMEEDIT_DATETIME_MAX QDateTime(QDATETIMEEDIT_DATE_MAX, QDATETIMEEDIT_TIME_MAX)

QVariant Qtitan::qtn_pixmap_to_qvariant(QVariant::Type type, const QPixmap& pixmap)
{
    switch (type)
    {
    case QVariant::ByteArray:
        {
            QByteArray barray;
            QBuffer buffer(&barray);
            buffer.open(QIODevice::WriteOnly);
            pixmap.save(&buffer, "PNG"); 
            return barray;
        }
        break;
    case QVariant::Pixmap:
        return pixmap;
        break;
    case QVariant::Bitmap:
        return QBitmap(pixmap);
        break;
    case QVariant::Icon:
        return QIcon(pixmap);
        break;
    case QVariant::Image:
        return pixmap.toImage();
        break;
    default:
        break;
    }
    QVariant v = pixmap;
    v.convert(type);
    return v;
}

/* GridEditorStyleOption */
GridEditorStyleOption::GridEditorStyleOption(int version)
    : QStyleOptionViewItemV4(version)
{
}

/*!
  \class EditorEventArgs
  \inmodule QtitanDataGrid
*/
EditorEventArgs::EditorEventArgs(const GridColumnBase* column, GridRow* row, GridEditorActivationPolicyFlag policyFlag)
: BaseEventArgs(), m_column(column), m_row(row), m_policyFlag(policyFlag)
{
}

const GridColumnBase* EditorEventArgs::column() const
{
    return m_column;
}

GridRow* EditorEventArgs::row() const
{
    return m_row;
}

GridModelDataBinding* EditorEventArgs::dataBinding() const
{
    return m_column->viewBase()->getDataBinding(m_column);
}

GridEditorActivationPolicyFlag EditorEventArgs::activationPolicyFlag() const
{
    return m_policyFlag;
}

/*!
\class EditorLinkEventArgs
\inmodule QtitanDataGrid
*/
EditorLinkEventArgs::EditorLinkEventArgs(const GridColumnBase* column, GridRow* row, const QString& anchor)
    : EditorEventArgs(column, row, NoPolicy), m_anchor(anchor)
{
}

const QString& EditorLinkEventArgs::anchor() const
{
    return m_anchor;
}


/*!
  \class EditorValidationEventArgs
  \inmodule QtitanDataGrid
*/
EditorValidationEventArgs::EditorValidationEventArgs(GridEditor* editor, const QVariant& editingValue, const QVariant& editValue)
    : BaseEventArgs(), m_editor(editor)
{
    m_editingValue = editingValue;
    m_editValue = editValue;
    m_valueValid = true;
    m_editingValueModified = false;
    m_hint = Grid::tr_compatible(QtnGridValidationErrorString);
}

const QVariant& EditorValidationEventArgs::editingValue() const
{
    return m_editingValue;
}

void EditorValidationEventArgs::setEditingValue(const QVariant& value)
{
    m_editingValue = value;
    m_editingValueModified = true;
}

const QVariant& EditorValidationEventArgs::editValue() const
{
    return m_editValue;
}

bool EditorValidationEventArgs::isValueValid() const
{
    return m_valueValid;
}

void EditorValidationEventArgs::setErrorHint(const QString& hint)
{
    m_hint = hint;
}

const QString& EditorValidationEventArgs::errorHint() const
{
    return m_hint;
}

void EditorValidationEventArgs::setValueValid(bool valid)
{
    m_valueValid = valid;
}

GridModelDataBinding* EditorValidationEventArgs::dataBinding() const
{
    return m_editor->site()->dataBinding();
}

QModelIndex EditorValidationEventArgs::modelIndex() const
{
    return m_editor->site()->modelIndex();
}


/*!
    \class GridEditorSite
    \inmodule QtitanDataGrid
    \brief The GridEditorSite class-interface for access the grid cell internals. It may be necessary if you are coding a custom cell editor. 
    For example, it's possible to get the dimensions of the cell, the index model - QModelIndex, show a tooltip near the cell and etc.
*/

/*!
    \class GridEditor
    \inmodule QtitanDataGrid
    \brief Interface for grid editors.
*/

/*!
  \class GridEditorBase
  \inmodule QtitanDataGrid
  \brief Implementation of GridEditor interface with base functions.
*/
GridEditorBase::GridEditorBase()
    : m_editorRepository(Q_NULL), m_editMode(false), m_needToolTip(false), m_site(Q_NULL), m_contextValidated(false), m_contextModified(false), m_updateContextCount(0)
{
}

GridEditorBase::~GridEditorBase()
{
    m_editorRepository->m_editors.removeOne(this);
    m_editorRepository = Q_NULL;
    m_site = Q_NULL;
}

void GridEditorBase::beginUpdateContext()
{
    m_updateContextCount++;
}

void GridEditorBase::endUpdateContext()
{
    m_updateContextCount--;
    Q_ASSERT(m_updateContextCount >= 0);
}

bool GridEditorBase::isContextUpdating() const
{
    return m_updateContextCount > 0;
}

void GridEditorBase::setEditingValue(const QVariant& value)
{
    m_contextEditingValue = value;
    m_contextValidated = false;
}

const QVariant& GridEditorBase::getEditingValue() const
{
    return m_contextEditingValue;
}

void GridEditorBase::initEditor(GridEditorRepository* repository, GridEditorSite* site)
{
    m_editorRepository = repository;
    m_site = site;
    m_editorRepository->m_editors.push_back(this);
}

QWidget* GridEditorBase::getCellWidget()
{
    return Q_NULL;
}

QString GridEditorBase::contextValueToString()
{
    return m_contextValue.toString();
}

bool GridEditorBase::isValidated()
{
    return m_contextValidated;
}

QVariant GridEditorBase::getContextValue()
{
    return m_contextValue;
}

bool GridEditorBase::isContextModified()
{
    return isEditMode() && m_contextModified;
}

void GridEditorBase::setContextModified(bool modified)
{
    m_contextModified = modified;
    if (modified)
        m_editorRepository->view()->raiseEditorModifying(this);
}

bool GridEditorBase::validate()
{
    if (!m_editorRepository->isValidationEnabled())
    {
        m_contextValue = m_contextEditingValue;
        return true;
    }

    if (m_contextValidated)
        return true;

    EditorValidationEventArgs args(this, m_contextEditingValue, m_contextValue);
    
    m_editorRepository->view()->raiseEditorValidating(&args);

    if (!args.isHandled())
    {
        m_contextValue = m_contextEditingValue;
        return true;
    }

    if (args.m_editingValueModified)
    {
        setEditingValue(args.editingValue());
        setValueToWidget(args.editingValue());
    }

    m_contextValidated = args.isValueValid();
    
    if (!m_contextValidated)
    {
        if (!args.errorHint().isEmpty())
            site()->showHint(args.errorHint());
        return false;
    }

    m_contextValue = args.editingValue();

    return true;
}

GridEditorSite* GridEditorBase::site() const
{
    return m_site;
}

void GridEditorBase::repositoryUpdated()
{
}

bool GridEditorBase::isEditMode()
{
    return m_editMode;
}

void GridEditorBase::setEditMode(bool mode)
{
    if (m_editMode == mode)
        return;
    m_editMode = mode;
    if (m_editMode)
        createEditModeContext();
    else
        destroyEditModeContext();
}

void GridEditorBase::paintSimpleText(QPainter& painter, const GridEditorStyleOption& option, const QString& text)
{
    QPalette palette = option.palette;
    palette.setCurrentColorGroup(option.state & QStyle::State_Active ? QPalette::Active : QPalette::Inactive);
    const QColor fgcolor = palette.color(option.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text);
    painter.setPen(fgcolor);
    painter.setFont(option.font);
    QRect textRect = option.rect.adjusted(kGridTextEditorPadding, 0, -kGridTextEditorPadding, 0);
    QString s = text;

    QPixmap icon = option.icon.pixmap(option.rect.height(), option.state & QStyle::State_Enabled ? QIcon::Normal : QIcon::Disabled);


    bool textFullyVisible;
    paint_icon_and_text(painter, textRect, icon,
        s, option.decorationAlignment, Qt::ElideRight, 
        m_editorRepository->iconApperance(),
        option.direction, 
        Qt::Horizontal, false, kGridDefHeaderWidthIndent, &textFullyVisible, option.selectedText, option.selectCaseSensitivity);
    m_needToolTip = !textFullyVisible;
}

void GridEditorBase::editingValueUpdated(const QVariant& value, bool updateSite)
{
    if (isContextUpdating())
        return;

    setEditingValue(value);

    if (m_editorRepository->validateOnEnter() || validate())
        setContextModified(true);

    if (updateSite)
        site()->invalidate(true);
}

void GridEditorBase::paintEvent(QPainter& painter, const GridEditorStyleOption& option)
{
    paintSimpleText(painter, option, contextValueToString());
}

void GridEditorBase::mouseMoveEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
}

void GridEditorBase::mousePressEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
}

void GridEditorBase::mouseReleaseEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
}

void GridEditorBase::mouseClickEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
}

void GridEditorBase::mouseDblClickEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
}

void GridEditorBase::mouseOverEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
    if (m_needToolTip && m_editorRepository->isHintVisible())
        m_editorRepository->view()->grid()->setToolTip(m_contextValue.toString());
}

void GridEditorBase::mouseOutEvent()
{
    QToolTip::hideText();
    m_editorRepository->view()->grid()->setToolTip("");
}

void GridEditorBase::updateContext(const QVariant& value)
{
    m_contextValue = value;
    m_contextModified = false;
    m_contextValidated = true;

    if (isEditMode())
        m_contextEditingValue = value;
    else
        m_contextEditingValue = QVariant();
}

GridEditorRepository* GridEditorBase::editorRepository() const
{
    return m_editorRepository;
}

/*!
    \class GridEditorRepository
    \inmodule QtitanDataGrid
    \brief The GridEditorRepository class is a repository for the cell's editor.
*/
GridEditorRepository::GridEditorRepository()
: QObject(), m_view(Q_NULL), m_alignment(Qt::AlignVCenter | Qt::AlignRight),
  m_iconApperance(IconAlignLeft), m_enabled(true), m_editable(true), m_transparentBackground(true),
  m_editorActivationPolicy(DblClick | MousePressFocused | Enter),
  m_autoSelect(false), m_hintVisible(true), m_validateOnEnter(true), m_validationEnabled(true),
  m_validator(Q_NULL), m_immediatePost(false), m_autoGeneratedValue(false)
{
}

GridEditorRepository::~GridEditorRepository()
{
    Q_ASSERT(m_editors.size() == 0);
}

void GridEditorRepository::setAlignment(Qt::Alignment flag)
{
    if (m_alignment == flag)
        return;
    m_alignment = flag;
    changed();
}

Qt::Alignment GridEditorRepository::alignment() const
{
    return m_alignment;
}

const QIcon& GridEditorRepository::icon() const
{
    return m_icon;
}

void GridEditorRepository::setIcon(const QIcon& icon)
{
    m_icon = icon;
    changed();
}

IconApperance GridEditorRepository::iconApperance() const
{
    return m_iconApperance;
}

void GridEditorRepository::setIconApperance(IconApperance apperance)
{
    if (m_iconApperance == apperance)
        return;
    m_iconApperance = apperance;
    changed();
}

bool GridEditorRepository::isEnabled() const
{
    return m_enabled;
}

void GridEditorRepository::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;
    m_enabled = enabled;
    changed();
}

bool GridEditorRepository::isEditable() const
{
    return m_editable;
}

void GridEditorRepository::setEditable(bool editable)
{
    if (m_editable == editable)
        return;
    m_editable = editable;
    changed();
}

bool GridEditorRepository::transparentBackground() const
{
    return m_transparentBackground;
}

void GridEditorRepository::setTransparentBackground(bool transparent)
{
    if (m_transparentBackground == transparent)
        return;
    m_transparentBackground = transparent;
    changed();
}

const GridEditorActivationPolicy& GridEditorRepository::editorActivationPolicy() const
{
    return m_editorActivationPolicy;
}

void GridEditorRepository::setEditorActivationPolicy(const GridEditorActivationPolicy& activationPolicy)
{
    if (m_editorActivationPolicy == activationPolicy)
        return;
    m_editorActivationPolicy = activationPolicy;
    changed();
}

bool GridEditorRepository::autoSelect() const
{
    return m_autoSelect;
}

void GridEditorRepository::setAutoSelect(bool autoSelect)
{
    if (m_autoSelect == autoSelect)
        return;
    m_autoSelect = autoSelect;
    changed();
}

bool GridEditorRepository::isHintVisible() const
{
    return m_hintVisible;
}

void GridEditorRepository::setHintVisible(bool visible)
{
    if (m_hintVisible == visible)
        return;
    m_hintVisible = visible;
    changed();
}

QVariant GridEditorRepository::defaultValue(Qt::ItemDataRole role) const
{
    Q_ASSERT(dataBinding() != Q_NULL);
    return dataBinding()->controller()->defaultValue(dataBinding()->column(), role);
}

void GridEditorRepository::setDefaultValue(const QVariant& value, Qt::ItemDataRole role)
{
    Q_ASSERT(dataBinding() != Q_NULL);
    dataBinding()->controller()->setDefaultValue(dataBinding()->column(), value, role);
}

void GridEditorRepository::removeDefaultValue(Qt::ItemDataRole role)
{
    Q_ASSERT(dataBinding() != Q_NULL);
    dataBinding()->controller()->removeDefaultValue(dataBinding()->column(), role);
}

bool GridEditorRepository::validateOnEnter() const
{
    return m_validateOnEnter;
}

void GridEditorRepository::setValidateOnEnter(bool flag)
{
    if (m_validateOnEnter == flag)
        return;
    m_validateOnEnter = flag;
    changed();
}

bool GridEditorRepository::isValidationEnabled() const
{
    return m_validationEnabled;
}

void GridEditorRepository::setValidationEnabled(bool b)
{
    m_validationEnabled = b;
    changed();
}

QValidator* GridEditorRepository::validator() const
{
    return m_validator;
}

void GridEditorRepository::setValidator(QValidator* validator)
{
    if (m_validator == validator)
        return;
    m_validator = validator;
    changed();
}

int GridEditorRepository::heightHint(int width, const QVariant& v)
{
    Q_UNUSED(width);
    Q_UNUSED(v);
    return -1;
}

void GridEditorRepository::initialize(GridViewBase* view, GridModelDataBinding* binding)
{
    m_view = view;
    m_dataBinding = binding;
    if (!m_autoGeneratedValue)
    {
        setDefaultValue(m_defValueDisplay, Qt::DisplayRole);
        setDefaultValue(m_defValueEdit, Qt::EditRole);
    }
}

GridViewBase* GridEditorRepository::view() const
{
    return m_view;
}

GridModelDataBinding* GridEditorRepository::dataBinding() const
{
    return m_dataBinding;
}

void GridEditorRepository::changed()
{
    if (view() != Q_NULL)
        view()->editorRepositoryChanged(this);
}

bool GridEditorRepository::immediatePost() const
{
    return m_immediatePost;
}

void GridEditorRepository::setImmediatePost(bool b)
{
    m_immediatePost = b;
}

bool GridEditorRepository::isAutoGeneratedValue() const
{
    return m_autoGeneratedValue;
}

void GridEditorRepository::setAutoGeneratedValue(bool autoGenerated)
{
    if (autoGenerated == m_autoGeneratedValue)
        return;
    m_autoGeneratedValue = autoGenerated;
    if (m_autoGeneratedValue)
    {
        removeDefaultValue(Qt::DisplayRole);
        removeDefaultValue(Qt::EditRole);
    }
    else
    {
        setDefaultValue(m_defValueDisplay, Qt::DisplayRole);
        setDefaultValue(m_defValueEdit, Qt::EditRole);
    }
}


/* GridStringEditor */
GridStringEditor::GridStringEditor()
: GridEditorBase(), m_textEdit(Q_NULL)
{
}

GridStringEditor::~GridStringEditor()
{
    Q_ASSERT(!isEditMode());
    destroyEditModeContext();
}

void GridStringEditor::paintEvent(QPainter& painter, const GridEditorStyleOption& option)
{
    QString text = m_contextValue.toString();
    QLineEdit::EchoMode em = static_cast<GridStringEditorRepository *>(m_editorRepository)->echoMode();
    if (em == QLineEdit::NoEcho)
        text = QString::fromLatin1("");

    bool password = (em == QLineEdit::Password || em == QLineEdit::PasswordEchoOnEdit);
    if (password)
    {
        QChar passwordCharacter = m_editorRepository->view()->grid()->style()->styleHint(
            QStyle::SH_LineEdit_PasswordCharacter, &option, m_editorRepository->view()->grid());
        text.fill(passwordCharacter);
    }
    if (static_cast<GridStringEditorRepository *>(m_editorRepository)->maxLength() > 0)
        text = text.left(static_cast<GridStringEditorRepository *>(m_editorRepository)->maxLength());

    if (static_cast<GridStringEditorRepository *>(m_editorRepository)->isHLink())
    {
        GridEditorStyleOption optioncopy = option;
        optioncopy.font.setUnderline(true);
        optioncopy.palette.setColor(QPalette::Text, Qt::blue);
        paintSimpleText(painter, optioncopy, text);
    }
    else
        paintSimpleText(painter, option, text);

    if (password)
        m_needToolTip =false;
}

void GridStringEditor::mouseOverEvent(const QPoint& pos)
{
    if (!isEditMode() && static_cast<GridStringEditorRepository *>(m_editorRepository)->isHLink())
        QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
    GridEditorBase::mouseOverEvent(pos);
}

void GridStringEditor::mouseOutEvent()
{
    if (!isEditMode() && static_cast<GridStringEditorRepository *>(m_editorRepository)->isHLink())
        QApplication::restoreOverrideCursor();
    GridEditorBase::mouseOutEvent();
}

QWidget* GridStringEditor::getCellWidget()
{
    return m_textEdit;
}

void GridStringEditor::setValueToWidget(const QVariant& value)
{
    if (m_textEdit == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    if (value.toString() != m_textEdit->text())
    {
        m_textEdit->setAttribute(Qt::WA_UpdatesDisabled, true);
        m_textEdit->setText(value.toString());
        if (m_editorRepository->autoSelect())
            m_textEdit->selectAll();
        m_textEdit->setAttribute(Qt::WA_UpdatesDisabled, false);
    }
}

void GridStringEditor::updateContext(const QVariant& value)
{
    EditorAutoUpdater autoupdater(this);

    if (isEditMode())
        setValueToWidget(value);

    GridEditorBase::updateContext(value);
}

void GridStringEditor::createEditModeContext()
{
    Q_ASSERT(isEditMode());
    m_textEdit = new QLineEdit(site()->parent());
    if (site()->parent() == Q_NULL)
        m_textEdit->setAttribute(Qt::WA_DontShowOnScreen);
    connect(m_textEdit, SIGNAL(textEdited(const QString &)), this, SLOT(textEdited(const QString &)));
    repositoryUpdated();
}

void GridStringEditor::destroyEditModeContext()
{
    Q_DELETE_AND_NULL(m_textEdit);
}

void GridStringEditor::repositoryUpdated()
{
    if (m_textEdit == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    GridEditorStyleOption option = site()->editorOption();

    if (m_editorRepository->transparentBackground() != m_textEdit->testAttribute(Qt::WA_NoSystemBackground))
        m_textEdit->setAttribute(Qt::WA_NoSystemBackground, m_editorRepository->transparentBackground());

    if (m_textEdit->validator() != m_editorRepository->validator())
        m_textEdit->setValidator(m_editorRepository->validator());

    if (m_textEdit->inputMask() != static_cast<GridStringEditorRepository *>(m_editorRepository)->inputMask())
        m_textEdit->setInputMask(static_cast<GridStringEditorRepository *>(m_editorRepository)->inputMask());

    if (m_textEdit->echoMode() != static_cast<GridStringEditorRepository *>(m_editorRepository)->echoMode())
        m_textEdit->setEchoMode(static_cast<GridStringEditorRepository *>(m_editorRepository)->echoMode());

    if (m_textEdit->maxLength() != static_cast<GridStringEditorRepository *>(m_editorRepository)->maxLength())
        m_textEdit->setMaxLength(static_cast<GridStringEditorRepository *>(m_editorRepository)->maxLength());

    if (m_textEdit->isEnabled() != m_editorRepository->isEnabled())
        m_textEdit->setEnabled(m_editorRepository->isEnabled());

    if (m_textEdit->alignment() != option.decorationAlignment)
        m_textEdit->setAlignment(option.decorationAlignment);
}

void GridStringEditor::textEdited(const QString &text)
{
    editingValueUpdated(text, false);
}

/*!
\class GridStringEditorRepository
\inmodule QtitanDataGrid
*/
GridStringEditorRepository::GridStringEditorRepository()
: GridEditorRepository(),
  m_echoMode(QLineEdit::Normal), m_maxLength(0x7FFF), m_hlink(false)
{
    m_defValueDisplay = QString("");
    m_defValueEdit = QString("");
}

GridEditor* GridStringEditorRepository::createEditor()
{
    return new GridStringEditor();
}

const QString& GridStringEditorRepository::inputMask() const
{
    return m_inputMask;
}

void GridStringEditorRepository::setInputMask(const QString& mask)
{
    m_inputMask = mask;
    changed();
}

QLineEdit::EchoMode GridStringEditorRepository::echoMode() const
{
    return m_echoMode;
}

void GridStringEditorRepository::setEchoMode(QLineEdit::EchoMode mode)
{
    m_echoMode = mode;
    changed();
}

int GridStringEditorRepository::maxLength() const
{
    return m_maxLength;
}

void GridStringEditorRepository::setMaxLength(int length)
{
    m_maxLength = length;
    changed();
}

bool GridStringEditorRepository::isHLink() const
{
    return m_hlink;
}

void GridStringEditorRepository::setHLink(bool link)
{
    m_hlink = link;
    changed();
}

/*!
\class GridNumericEditorRepository
\inmodule QtitanDataGrid
*/
GridNumericEditorRepository::GridNumericEditorRepository()
: GridEditorRepository(), m_maximum(99.99), m_minimum(0), m_decimals(2), m_singleStep(1.0), m_wrapping(false)
{
    m_defValueDisplay = 0;
    m_defValueEdit = 0;
}

GridEditor* GridNumericEditorRepository::createEditor()
{
    return new GridNumericEditor();
}

double GridNumericEditorRepository::maximum() const
{
    return m_maximum;
}

void GridNumericEditorRepository::setMaximum(double max)
{
    m_maximum = max;
    changed();
}

double GridNumericEditorRepository::minimum() const
{
    return m_minimum;
}

void GridNumericEditorRepository::setMinimum(double min)
{
    m_minimum = min;
    changed();
}

void GridNumericEditorRepository::getRange(double& minimum, double& maximum) const
{
    minimum = m_minimum;
    maximum = m_maximum;
}

void GridNumericEditorRepository::setRange(double minimum, double maximum)
{
    m_minimum = minimum;
    m_maximum = maximum;
    changed();
}

const QString& GridNumericEditorRepository::prefix() const
{
    return m_prefix;
}

void GridNumericEditorRepository::setPrefix(const QString& prefix)
{
    m_prefix = prefix;
    changed();
}

const QString& GridNumericEditorRepository::suffix() const
{
    return m_suffix;
}

void GridNumericEditorRepository::setSuffix(const QString& suffix)
{
    m_suffix = suffix;
    changed();
}

int GridNumericEditorRepository::decimals() const
{
    return m_decimals;
}

void GridNumericEditorRepository::setDecimals(int prec)
{
    m_decimals = prec;
    changed();
}

void GridNumericEditorRepository::setSingleStep(double val)
{
    m_singleStep = val;
    changed();
}

double GridNumericEditorRepository::singleStep() const
{
    return m_singleStep;
}

bool GridNumericEditorRepository::wrapping() const
{
    return m_wrapping;
}

void GridNumericEditorRepository::setWrapping(bool wrapping)
{
    m_wrapping = wrapping;
    changed();
}


/* GridNumericEditor */
GridNumericEditor::GridNumericEditor()
: GridEditorBase(), m_numericEdit(Q_NULL)
{
}

GridNumericEditor::~GridNumericEditor()
{
    Q_ASSERT(!isEditMode());
    destroyEditModeContext();
}

QWidget* GridNumericEditor::getCellWidget()
{
    return m_numericEdit;
}

class QDoubleSpinBoxPublic: public QDoubleSpinBox
{
    friend class Qtitan::GridNumericEditor;
};

void GridNumericEditor::setValueToWidget(const QVariant& value)
{
    if (m_numericEdit == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    if (m_numericEdit->value() != value.toDouble())
    {
        m_numericEdit->setAttribute(Qt::WA_UpdatesDisabled, true);
        m_numericEdit->setValue(value.toDouble());
        m_numericEdit->setAttribute(Qt::WA_UpdatesDisabled, false);
        if (m_editorRepository->autoSelect())
            m_numericEdit->selectAll();
    }
}

void GridNumericEditor::updateContext(const QVariant& value)
{
    EditorAutoUpdater autoupdater(this);

    if (isEditMode())
        setValueToWidget(value);

    GridEditorBase::updateContext(value);
}

void GridNumericEditor::createEditModeContext()
{
    Q_ASSERT(isEditMode());
    m_numericEdit = new QDoubleSpinBox(site()->parent());
    if (site()->parent() == Q_NULL)
        m_numericEdit->setAttribute(Qt::WA_DontShowOnScreen);
    connect(m_numericEdit, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
    repositoryUpdated();
}

void GridNumericEditor::destroyEditModeContext()
{
    Q_DELETE_AND_NULL(m_numericEdit);
}

QString GridNumericEditor::contextValueToString()
{
    QString text;
    switch (m_contextValue.type())
    {
    case QVariant::Double:
        text = QLocale::system().toString(m_contextValue.toDouble(), 'f', static_cast<GridNumericEditorRepository *>(m_editorRepository)->decimals());
        break;
    case QVariant::Int:
        text = QLocale::system().toString(m_contextValue.toInt());
        break;
    case QVariant::UInt:
        text = QLocale::system().toString(m_contextValue.toUInt());
        break;
    case QVariant::LongLong:
        text = QLocale::system().toString(m_contextValue.toLongLong());
        break;
    case QVariant::ULongLong:
        text = QLocale::system().toString(m_contextValue.toULongLong());
        break;
    default:
        text = m_contextValue.toString();
        break;
    }

    return QString("%1%2%3").arg(static_cast<GridNumericEditorRepository *>(m_editorRepository)->prefix()).arg(
        text).arg(static_cast<GridNumericEditorRepository *>(m_editorRepository)->suffix());
}

void GridNumericEditor::repositoryUpdated()
{
    if (m_numericEdit == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    GridEditorStyleOption option = site()->editorOption();

    if (m_editorRepository->transparentBackground() != m_numericEdit->testAttribute(Qt::WA_NoSystemBackground))
        m_numericEdit->setAttribute(Qt::WA_NoSystemBackground, m_editorRepository->transparentBackground());

    if (m_numericEdit->maximum() != ((GridNumericEditorRepository *)m_editorRepository)->maximum())
        m_numericEdit->setMaximum(((GridNumericEditorRepository *)m_editorRepository)->maximum());

    if (m_numericEdit->minimum() != ((GridNumericEditorRepository *)m_editorRepository)->minimum())
        m_numericEdit->setMinimum(((GridNumericEditorRepository *)m_editorRepository)->minimum());

    if (m_numericEdit->prefix() != ((GridNumericEditorRepository *)m_editorRepository)->prefix())
        m_numericEdit->setPrefix(((GridNumericEditorRepository *)m_editorRepository)->prefix());

    if (m_numericEdit->suffix() != ((GridNumericEditorRepository *)m_editorRepository)->suffix())
        m_numericEdit->setSuffix(((GridNumericEditorRepository *)m_editorRepository)->suffix());

    if (m_numericEdit->decimals() != ((GridNumericEditorRepository *)m_editorRepository)->decimals())
        m_numericEdit->setDecimals(((GridNumericEditorRepository *)m_editorRepository)->decimals());

    if (m_numericEdit->singleStep() != ((GridNumericEditorRepository *)m_editorRepository)->singleStep())
        m_numericEdit->setSingleStep(((GridNumericEditorRepository *)m_editorRepository)->singleStep());

    if (m_numericEdit->wrapping() != ((GridNumericEditorRepository *)m_editorRepository)->wrapping())
        m_numericEdit->setWrapping(((GridNumericEditorRepository *)m_editorRepository)->wrapping());

    if (m_editorRepository->validator() != Q_NULL)
        ((QDoubleSpinBoxPublic *)m_numericEdit)->lineEdit()->setValidator(m_editorRepository->validator());

    if (m_numericEdit->isEnabled() != m_editorRepository->isEnabled())
        m_numericEdit->setEnabled(m_editorRepository->isEnabled());

    if (m_numericEdit->alignment() != option.decorationAlignment)
        m_numericEdit->setAlignment(option.decorationAlignment);
}

void GridNumericEditor::valueChanged(double d)
{
    editingValueUpdated(d, false);
}

/*!
\class GridProgressEditorRepository
\inmodule QtitanDataGrid
*/
GridProgressEditorRepository::GridProgressEditorRepository()
    : GridEditorRepository(), m_maximum(99.99), m_minimum(0), m_singleStep(1.0), m_wrapping(false), m_textVisible(true), m_dialEdit(false)
{
    m_defValueDisplay = m_minimum;
    m_defValueEdit = m_minimum;
}

GridEditor* GridProgressEditorRepository::createEditor()
{
    return new GridProgressEditor();
}

double GridProgressEditorRepository::maximum() const
{
    return m_maximum;
}

void GridProgressEditorRepository::setMaximum(double max)
{
    m_maximum = max;
    changed();
}

double GridProgressEditorRepository::minimum() const
{
    return m_minimum;
}

void GridProgressEditorRepository::setMinimum(double min)
{
    m_minimum = min;
    changed();
}

void GridProgressEditorRepository::getRange(double& minimum, double& maximum) const
{
    minimum = m_minimum;
    maximum = m_maximum;
}

void GridProgressEditorRepository::setRange(double minimum, double maximum)
{
    m_minimum = minimum;
    m_maximum = maximum;
    changed();
}

void GridProgressEditorRepository::setSingleStep(double val)
{
    m_singleStep = val;
    changed();
}

double GridProgressEditorRepository::singleStep() const
{
    return m_singleStep;
}

bool GridProgressEditorRepository::wrapping() const
{
    return m_wrapping;
}

void GridProgressEditorRepository::setWrapping(bool wrapping)
{
    m_wrapping = wrapping;
    changed();
}

void GridProgressEditorRepository::setTextVisible(bool textVisible)
{
    m_textVisible = textVisible;
    changed();
}

bool GridProgressEditorRepository::isTextVisible() const
{
    return m_textVisible;
}

bool GridProgressEditorRepository::isDialEdit() const
{
    return m_dialEdit;
}

void GridProgressEditorRepository::setDialEdit(bool dial)
{
    m_dialEdit = dial;
    changed();
}

/* ProgressEdit */
ProgressEdit::ProgressEdit(QWidget* parent)
    : QProgressBar(parent), m_editFieldKind(emNone), m_buttonState(None), m_buttonSymbols(QAbstractSpinBox::UpDownArrows), 
      m_accelerate(true), m_acceleration(0), m_effectiveSpinRepeatRate(1), m_spinBoxClickTimerId(-1), m_spinBoxClickThresholdTimerId(-1), 
      m_singleStep(1.0), m_pageStep(10), m_wrapping(false), m_notchTarget(3.7)
{
    setMouseTracking(true);
    QStyleOptionSpinBox opt;
    spin_initStyleOption(&opt);
    m_spinBoxClickTimerInterval = style()->styleHint(QStyle::SH_SpinBox_ClickAutoRepeatRate, &opt, this);
    m_spinBoxClickThresholdTimerInterval = style()->styleHint(QStyle::SH_SpinBox_ClickAutoRepeatThreshold, &opt, this);
}

ProgressEdit::EditFieldKind ProgressEdit::editFieldKind() const
{
    return m_editFieldKind;
}

void ProgressEdit::setEditFieldKind(ProgressEdit::EditFieldKind kind)
{
    m_editFieldKind = kind;
}

void ProgressEdit::keyPressEvent(QKeyEvent *event)
{
    if (editFieldKind() == emNone)
    {
        QProgressBar::keyPressEvent(event);
        return;
    }

    int steps = 1;
    bool isPgUpOrDown = false;
    switch (event->key())
    {
    case Qt::Key_PageUp:
    case Qt::Key_PageDown:
        steps *= 10;
        isPgUpOrDown = true;
    case Qt::Key_Up:
    case Qt::Key_Down:
        {
            event->accept();
            const bool up = (event->key() == Qt::Key_PageUp || event->key() == Qt::Key_Up);
            if (!(spin_stepEnabled() & (up ? QAbstractSpinBox::StepUpEnabled : QAbstractSpinBox::StepDownEnabled)))
                return;
            if (!up)
                steps *= -1;
            if (style()->styleHint(QStyle::SH_SpinBox_AnimateButton, 0, this)) {
                m_buttonState = (Keyboard | (up ? Up : Down));
            }
            if (m_spinBoxClickTimerId == -1)
                stepBy(steps);
            if(event->isAutoRepeat() && !isPgUpOrDown) {
                if(m_spinBoxClickThresholdTimerId == -1 && m_spinBoxClickTimerId == -1) {
                    spin_updateState(up, true);
                }
            }
            return;
        }
        break;
    case Qt::Key_Escape:
        event->ignore();
        break;
    default:
        QProgressBar::keyPressEvent(event);
        break;
    }
}

/*!
    \reimp
*/
void ProgressEdit::keyReleaseEvent(QKeyEvent *event)
{
    if (editFieldKind() == emNone)
    {
        QProgressBar::keyReleaseEvent(event);
        return;
    }

    if (m_buttonState & Keyboard && !event->isAutoRepeat())  {
        spin_reset();
    } else {
        QProgressBar::keyReleaseEvent(event);
    }
}

/*!
    \reimp
*/
void ProgressEdit::wheelEvent(QWheelEvent *event)
{
    const int steps = (event->delta() > 0 ? 1 : -1);
    if (spin_stepEnabled() & (steps > 0 ? QAbstractSpinBox::StepUpEnabled : QAbstractSpinBox::StepDownEnabled))
        stepBy(event->modifiers() & Qt::ControlModifier ? steps * 10 : steps);
    event->accept();
}

/*!
    \reimp
*/
void ProgressEdit::changeEvent(QEvent *event)
{
    switch (event->type())
    {
        case QEvent::StyleChange:
            m_spinBoxClickTimerInterval = style()->styleHint(QStyle::SH_SpinBox_ClickAutoRepeatRate, 0, this);
            m_spinBoxClickThresholdTimerInterval =
                style()->styleHint(QStyle::SH_SpinBox_ClickAutoRepeatThreshold, 0, this);
            spin_reset();
            //updateEditFieldGeometry();
            break;
        case QEvent::EnabledChange:
            if (!isEnabled()) {
                spin_reset();
            }
            break;
        default:
            break;
    }
    QWidget::changeEvent(event);
}


bool ProgressEdit::updateSpiBoxHoverControl(const QPoint& pos)
{
    QRect lastHoverRect = m_hoverRect;
    QStyle::SubControl lastHoverControl = m_hoverControl;
    bool doesHover = true;//testAttribute(Qt::WA_Hover);
    if (lastHoverControl != newSpinBoxHoverControl(pos) && doesHover) {
        update(lastHoverRect);
        update(m_hoverRect);
        return true;
    }
    return !doesHover;
}

QStyle::SubControl ProgressEdit::newSpinBoxHoverControl(const QPoint &pos)
{
    QStyleOptionSpinBox opt;
    spin_initStyleOption(&opt);
    opt.subControls = QStyle::SC_All;
    m_hoverControl = style()->hitTestComplexControl(QStyle::CC_SpinBox, &opt, pos, this);
    m_hoverRect = style()->subControlRect(QStyle::CC_SpinBox, &opt, m_hoverControl, this);
    return m_hoverControl;
}

void ProgressEdit::setSingleStep(double val)
{
    m_singleStep = val;
}

double ProgressEdit::singleStep() const
{
    return m_singleStep;
}

bool ProgressEdit::wrapping() const
{
    return m_wrapping;
}

void ProgressEdit::setWrapping(bool wrapping)
{
    m_wrapping = wrapping;
}

QVariant ProgressEdit::bound(const QVariant &val, const QVariant &old, int steps) const
{
    QVariant v = val;
    if (!m_wrapping || steps == 0 || old.isNull()) {
        if (variant_compare(v, minimum()) < 0) {
            v = m_wrapping ? maximum() : minimum();
        }
        if (variant_compare(v, maximum()) > 0) {
            v = m_wrapping ? minimum() : maximum();
        }
    } else {
        const bool wasMin = old == minimum();
        const bool wasMax = old == maximum();
        const int oldcmp = variant_compare(v, old);
        const int maxcmp = variant_compare(v, maximum());
        const int mincmp = variant_compare(v, minimum());
        const bool wrapped = (oldcmp > 0 && steps < 0) || (oldcmp < 0 && steps > 0);
        if (maxcmp > 0) {
            v = ((wasMax && !wrapped && steps > 0) || (steps < 0 && !wasMin && wrapped))
                ? minimum() : maximum();
        } else if (wrapped && (maxcmp > 0 || mincmp < 0)) {
            v = ((wasMax && steps > 0) || (!wasMin && steps < 0)) ? minimum() : maximum();
        } else if (mincmp < 0) {
            v = (!wasMax && !wasMin ? minimum() : maximum());
        }
    }

    return v;
}

void ProgressEdit::stepBy(int steps)
{
    int oldValue = value();
    setValue(bound(oldValue + (singleStep() * steps), oldValue, steps).toInt());
}

void ProgressEdit::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
    {
        event->ignore();
        return;
    }
    event->accept();

    if (editFieldKind() == emSpinBox)
    {
        updateSpiBoxHoverControl(event->pos());

        if (m_spinBoxClickTimerId != -1 && m_buttonSymbols != QAbstractSpinBox::NoButtons)
        {
            const QAbstractSpinBox::StepEnabled se = spin_stepEnabled();
            if ((se & QAbstractSpinBox::StepUpEnabled) && m_hoverControl == QStyle::SC_SpinBoxUp)
                spin_updateState(true);
            else if ((se & QAbstractSpinBox::StepDownEnabled) && m_hoverControl == QStyle::SC_SpinBoxDown)
                spin_updateState(false);
            else
                spin_reset();
        }
    } else if (editFieldKind() == emDial)
    {
        QPoint p = event->pos(); 
        QRect r = rect();
        p.setX(p.x() - (r.width() - r.height()));
        r = QRect(0, 0, height(), height());
        if (r.contains(p))
            setValue(dial_valueFromPoint(p));
    }
}

/*!
    \reimp
*/
void ProgressEdit::mousePressEvent(QMouseEvent *event)
{
    //if (event->button() != Qt::LeftButton || m_buttonState != None) {
    //    return;
    //}

    if (maximum() == minimum() ||
        (event->button() != Qt::LeftButton)  ||
        (event->buttons() ^ event->button())) {
        event->ignore();
        return;
    }

    event->accept();

    if (editFieldKind() == emSpinBox)
    {
        updateSpiBoxHoverControl(event->pos());

        const QAbstractSpinBox::StepEnabled se = (m_buttonSymbols == QAbstractSpinBox::NoButtons) ? QAbstractSpinBox::StepEnabled(QAbstractSpinBox::StepNone) : spin_stepEnabled();
        if ((se & QAbstractSpinBox::StepUpEnabled) && m_hoverControl == QStyle::SC_SpinBoxUp) {
            spin_updateState(true);
        } else if ((se & QAbstractSpinBox::StepDownEnabled) && m_hoverControl == QStyle::SC_SpinBoxDown) {
            spin_updateState(false);
        } else {
            event->ignore();
        }
    }
    else if (editFieldKind() == emDial)
    {
        QPoint p = event->pos(); 
        QRect r = rect();
        p.setX(p.x() - (r.width() - r.height()));
        r = QRect(0, 0, height(), height());
        if (r.contains(p))
            setValue(dial_valueFromPoint(p));
    }
}

/*!
    \reimp
*/
void ProgressEdit::mouseReleaseEvent(QMouseEvent *event)
{
    if ((m_buttonState & Mouse) != 0)
        spin_reset();
    event->accept();
}

static int getKeyboardAutoRepeatRate()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return qApp->styleHints()->keyboardAutoRepeatRate();
#else
    int ret = 30;
#if defined(Q_OS_WIN)
    DWORD time;
    if (SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &time, 0) != FALSE)
        ret = static_cast<int>(1000 / static_cast<int>(time)); // msecs
#endif
    return ret; // msecs
#endif
}

void ProgressEdit::timerEvent(QTimerEvent *event)
{
    bool doStep = false;
    if (event->timerId() == m_spinBoxClickThresholdTimerId) {
        killTimer(m_spinBoxClickThresholdTimerId);
        m_spinBoxClickThresholdTimerId = -1;
        m_effectiveSpinRepeatRate = m_buttonState & Keyboard ? getKeyboardAutoRepeatRate() : m_spinBoxClickTimerInterval;
        m_spinBoxClickTimerId = startTimer(m_effectiveSpinRepeatRate);
        doStep = true;
    } else if (event->timerId() == m_spinBoxClickTimerId) {
        if (m_accelerate) {
            m_acceleration = m_acceleration + (int)(m_effectiveSpinRepeatRate * 0.05);
            if (m_effectiveSpinRepeatRate - m_acceleration >= 10) {
                killTimer(m_spinBoxClickTimerId);
                m_spinBoxClickTimerId = startTimer(m_effectiveSpinRepeatRate - m_acceleration);
            }
        }
        doStep = true;
    }

    if (doStep) {
        const QAbstractSpinBox::StepEnabled st = spin_stepEnabled();
        if (m_buttonState & Up) {
            if (!(st & QAbstractSpinBox::StepUpEnabled)) {
                spin_reset();
            } else {
                stepBy(1);
            }
        } else if (m_buttonState & Down) {
            if (!(st & QAbstractSpinBox::StepDownEnabled)) {
                spin_reset();
            } else {
                stepBy(-1);
            }
        }
        return;
    }
    QWidget::timerEvent(event);
    return;
}

QAbstractSpinBox::StepEnabled ProgressEdit::spin_stepEnabled() const
{
    if (editFieldKind() == emNone)
        return QAbstractSpinBox::StepNone;
    
    QAbstractSpinBox::StepEnabled ret = QAbstractSpinBox::StepNone;
    if (variant_compare(value(), maximum()) < 0) {
        ret |= QAbstractSpinBox::StepUpEnabled;
    }
    if (variant_compare(value(), minimum()) > 0) {
        ret |= QAbstractSpinBox::StepDownEnabled;
    }
    return ret;
}

void ProgressEdit::spin_reset()
{
    m_buttonState = None;
    if (m_spinBoxClickTimerId != -1)
        killTimer(m_spinBoxClickTimerId);
    if (m_spinBoxClickThresholdTimerId != -1)
        killTimer(m_spinBoxClickThresholdTimerId);
    m_spinBoxClickTimerId = m_spinBoxClickThresholdTimerId = -1;
    m_acceleration = 0;
    update();
}

void ProgressEdit::spin_updateState(bool up, bool fromKeyboard /*= false*/)
{
    if ((up && (m_buttonState & Up)) || (!up && (m_buttonState & Down)))
        return;
    spin_reset();
    if (spin_stepEnabled() & (up ? QAbstractSpinBox::StepUpEnabled : QAbstractSpinBox::StepDownEnabled)) {
        m_spinBoxClickThresholdTimerId = startTimer(m_spinBoxClickThresholdTimerInterval);
        m_buttonState = (up ? Up : Down) | (fromKeyboard ? Keyboard : Mouse);
        stepBy(up ? 1 : -1);
    }
}

int ProgressEdit::bound(int val) const
{
    if (wrapping()) {
        if ((val >= minimum()) && (val <= maximum()))
            return val;
        val = minimum() + ((val - minimum()) % (maximum() - minimum()));
        if (val < minimum())
            val += maximum() - minimum();
        return val;
    } else {
        return qMax(minimum(), qMin(maximum(), val));
    }
}

int ProgressEdit::notchSize() const
{
    int width = height();
    int r = qMin(width, height())/2;
    int l = (int)(r * (wrapping() ? 6 : 5) * M_PI / 6);
    if (maximum() > minimum() + m_pageStep)
        l = (int)(0.5 + l * m_pageStep / (maximum() - minimum()));
    l = l * m_singleStep / (m_pageStep ? m_pageStep : 1);
    if (l < 1)
        l = 1;
    l = (int)(0.5 + m_notchTarget / l);
    if (!l)
        l = 1;
    return m_singleStep * l;
}

void ProgressEdit::dial_initStyleOption(QStyleOptionSlider* option)
{
   if (!option)
        return;

    option->initFrom(this);
    option->minimum = minimum();
    option->maximum = maximum();
    option->sliderPosition = value();
    option->sliderValue = value();
    option->singleStep = singleStep();
    option->pageStep = m_pageStep;
    option->upsideDown = !invertedAppearance();
    option->notchTarget = m_notchTarget;
    option->dialWrapping = wrapping();
    option->subControls = QStyle::SC_All;
    option->activeSubControls = QStyle::SC_None;
    if (true) {//!d->showNotches) {
        option->subControls &= ~QStyle::SC_DialTickmarks;
        option->tickPosition = QSlider::TicksAbove;
    } else {
        option->tickPosition = QSlider::NoTicks;
    }
    option->tickInterval = notchSize();
}

void ProgressEdit::spin_initStyleOption(QStyleOptionSpinBox* spinBoxOption)
{
        spinBoxOption->initFrom(this);
        spinBoxOption->activeSubControls = QStyle::SC_None;
        spinBoxOption->buttonSymbols = m_buttonSymbols;
        spinBoxOption->subControls = QStyle::SC_SpinBoxFrame | QStyle::SC_SpinBoxEditField;

        if (spinBoxOption->buttonSymbols != QAbstractSpinBox::NoButtons)
        {
            spinBoxOption->subControls |= QStyle::SC_SpinBoxUp | QStyle::SC_SpinBoxDown;
            if (m_buttonState & Up)
            {
                spinBoxOption->activeSubControls = QStyle::SC_SpinBoxUp;
            } 
            else if (m_buttonState & Down)
            {
                spinBoxOption->activeSubControls = QStyle::SC_SpinBoxDown;
            }
        }

        if (m_buttonState)
        {
            spinBoxOption->state |= QStyle::State_Sunken;
        }
        else
        {
            spinBoxOption->activeSubControls = m_hoverControl;
        }

        spinBoxOption->stepEnabled = 
            style()->styleHint(QStyle::SH_SpinControls_DisableOnBounds)
                      ? spin_stepEnabled() : (QAbstractSpinBox::StepDownEnabled | QAbstractSpinBox::StepUpEnabled);

        spinBoxOption->frame = 0;//d->frame;
}

int ProgressEdit::dial_valueFromPoint(const QPoint &p) const
{
    int width = height();
    double yy = (double)height()/2.0 - p.y();
    double xx = (double)p.x() - width/2.0;
    double a = (xx || yy) ? qAtan2(yy, xx) : 0;

    if (a < M_PI / -2)
        a = a + M_PI * 2;

    int dist = 0;
    int minv = minimum(), maxv = maximum();

    if (minimum() < 0) {
        dist = -minimum();
        minv = 0;
        maxv = maximum() + dist;
    }

    int r = maxv - minv;
    int v;
    if (wrapping())
        v =  (int)(0.5 + minv + r * (M_PI * 3 / 2 - a) / (2 * M_PI));
    else
        v =  (int)(0.5 + minv + r* (M_PI * 4 / 3 - a) / (M_PI * 10 / 6));

    if (dist > 0)
        v -= dist;

    return !invertedAppearance() ? bound(v) : maximum() - bound(v);
}

void ProgressEdit::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QWidget* widget = this;

    QRect spinBoxEditFieldRect = rect();
    if (editFieldKind() == emSpinBox)
    {
        QStyleOptionSpinBox spinBoxOption;
        spin_initStyleOption(&spinBoxOption);
        spinBoxEditFieldRect = style()->subControlRect(QStyle::CC_SpinBox, &spinBoxOption, QStyle::SC_SpinBoxEditField, widget);
        style()->drawComplexControl(QStyle::CC_SpinBox, &spinBoxOption, &p, widget);
    //SC_SpinBoxEditField
       //theme.rect = proxy()->subControlRect(CC_SpinBox, option, SC_SpinBoxUp, widget)

    }
    else if (editFieldKind() == emDial)
    {
        QStyleOptionSlider option;
        dial_initStyleOption(&option);
        spinBoxEditFieldRect.setWidth(spinBoxEditFieldRect.width() - spinBoxEditFieldRect.height());
        option.rect.setWidth(option.rect.height());
        p.save();
        p.translate(spinBoxEditFieldRect.width(), 0);
        style()->drawComplexControl(QStyle::CC_Dial, &option, &p, widget);
        p.restore();
    }

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
    QStyleOptionProgressBarV2 opt, subopt;
#else
    QStyleOptionProgressBar opt, subopt;
#endif

    initStyleOption(&opt);
    opt.rect = spinBoxEditFieldRect;

    subopt = opt;

    subopt.rect = style()->subElementRect(QStyle::SE_ProgressBarGroove, &opt, widget);
    style()->drawControl(QStyle::CE_ProgressBarGroove, &subopt, &p, widget);
    subopt.rect = style()->subElementRect(QStyle::SE_ProgressBarContents, &opt, widget);
    if (hasFocus())
    {
      style()->drawControl(QStyle::CE_ProgressBarContents, &subopt, &p, widget);
    }
    else
    {
        bool vertical = (subopt.orientation == Qt::Vertical);
        bool inverted = subopt.invertedAppearance;
        bool reverse = (subopt.direction == Qt::LeftToRight && inverted) || (subopt.direction == Qt::RightToLeft && !inverted);

        qint64 progress = qMax<qint64>(subopt.progress, subopt.minimum); // workaround for bug in QProgressBar

        if (vertical)
        {
            int maxHeight = subopt.rect.height();
            int minHeight = 0;
            double vc6_workaround = ((progress - qint64(subopt.minimum)) / qMax(double(1.0), double(qint64(subopt.maximum) - qint64(subopt.minimum))) * maxHeight);
            int height = false ? maxHeight: qMax(int(vc6_workaround), minHeight);
            subopt.rect.setHeight(height);
            if (!inverted)
                subopt.rect.moveTop(opt.rect.height() - subopt.rect.height());
        }
        else
        {
            int maxWidth = subopt.rect.width();
            int minWidth = 0;
            double vc6_workaround = ((progress - qint64(subopt.minimum)) / qMax(double(1.0), double(qint64(subopt.maximum) - qint64(subopt.minimum))) * maxWidth);
            int width = false ? maxWidth : qMax(int(vc6_workaround), minWidth);
            subopt.rect.setWidth(width);
            subopt.rect = QStyle::visualRect(reverse ? Qt::RightToLeft : Qt::LeftToRight,
                                                        opt.rect, subopt.rect);
        }
        subopt.rect.adjust(1, 1, -1, -1);
        style()->drawPrimitive(QStyle::PE_IndicatorProgressChunk, &subopt, &p, widget);
    }

    if (subopt.textVisible)
    {
        subopt.rect = style()->subElementRect(QStyle::SE_ProgressBarLabel, &opt, widget);
        style()->drawControl(QStyle::CE_ProgressBarLabel, &subopt, &p, widget);
    }
    //d_func()->lastPaintedValue = d_func()->value;
}

/* GridProgressEditor */
GridProgressEditor::GridProgressEditor()
    : GridEditorBase(), m_progressEdit(Q_NULL)
{
}

GridProgressEditor::~GridProgressEditor()
{
    Q_ASSERT(!isEditMode());
    destroyEditModeContext();
}

QWidget* GridProgressEditor::getCellWidget()
{
    if (m_progressEdit == Q_NULL)
        createEditModeContext();

    return m_progressEdit;
}

void GridProgressEditor::setEditMode(bool mode)
{
    m_editMode = mode;
    getCellWidget();
    if (mode)
        m_progressEdit->setEditFieldKind(((GridProgressEditorRepository *)m_editorRepository)->isDialEdit() ?  ProgressEdit::emDial : ProgressEdit::emSpinBox);
    else
        m_progressEdit->setEditFieldKind(ProgressEdit::emNone);
}

void GridProgressEditor::repositoryUpdated()
{
    if (m_progressEdit == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    GridEditorStyleOption option = site()->editorOption();

    if (m_editorRepository->transparentBackground() != m_progressEdit->testAttribute(Qt::WA_NoSystemBackground))
        m_progressEdit->setAttribute(Qt::WA_NoSystemBackground, m_editorRepository->transparentBackground());

    if (m_progressEdit->maximum() != ((GridProgressEditorRepository *)m_editorRepository)->maximum())
        m_progressEdit->setMaximum(((GridProgressEditorRepository *)m_editorRepository)->maximum());

    if (m_progressEdit->minimum() != ((GridProgressEditorRepository *)m_editorRepository)->minimum())
        m_progressEdit->setMinimum(((GridProgressEditorRepository *)m_editorRepository)->minimum());

    if (m_progressEdit->isTextVisible() != ((GridProgressEditorRepository *)m_editorRepository)->isTextVisible())
        m_progressEdit->setTextVisible(((GridProgressEditorRepository *)m_editorRepository)->isTextVisible());

    if (m_progressEdit->singleStep() != ((GridProgressEditorRepository *)m_editorRepository)->singleStep())
        m_progressEdit->setSingleStep(((GridProgressEditorRepository *)m_editorRepository)->singleStep());

    if (m_progressEdit->wrapping() != ((GridProgressEditorRepository *)m_editorRepository)->wrapping())
        m_progressEdit->setWrapping(((GridProgressEditorRepository *)m_editorRepository)->wrapping());
}


void GridProgressEditor::updateContext(const QVariant& value)
{
    EditorAutoUpdater autoupdater(this);

    setValueToWidget(value);

    GridEditorBase::updateContext(value);
}

void GridProgressEditor::createEditModeContext()
{
    m_progressEdit = new ProgressEdit(site()->parent());
    if (site()->parent() == Q_NULL)
        m_progressEdit->setAttribute(Qt::WA_DontShowOnScreen);
    m_progressEdit->setProperty("_q_no_animation", true);
    connect(m_progressEdit, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
    repositoryUpdated();
}

void GridProgressEditor::destroyEditModeContext()
{
    Q_DELETE_AND_NULL(m_progressEdit);
}

QString GridProgressEditor::contextValueToString()
{
    QString text;
    switch (m_contextValue.type())
    {
    case QVariant::Double:
        text = QLocale::system().toString(m_contextValue.toDouble(), 'f', static_cast<GridNumericEditorRepository *>(m_editorRepository)->decimals());
        break;
    case QVariant::Int:
        text = QLocale::system().toString(m_contextValue.toInt());
        break;
    case QVariant::UInt:
        text = QLocale::system().toString(m_contextValue.toUInt());
        break;
    case QVariant::LongLong:
        text = QLocale::system().toString(m_contextValue.toLongLong());
        break;
    case QVariant::ULongLong:
        text = QLocale::system().toString(m_contextValue.toULongLong());
        break;
    default:
        text = m_contextValue.toString();
        break;
    }

    return  text;
}
 
void GridProgressEditor::setValueToWidget(const QVariant& value)
{
    if (m_progressEdit == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    if (m_progressEdit->value() != value.toInt())
    {
        m_progressEdit->setAttribute(Qt::WA_UpdatesDisabled, true);
        m_progressEdit->setValue(m_progressEdit->bound(value.toInt()));
        m_progressEdit->setAttribute(Qt::WA_UpdatesDisabled, false);
    }
}
    
void GridProgressEditor::valueChanged(int value)
{
    editingValueUpdated(value, false);
}

/*!
\class GridCheckBoxEditorRepository
\inmodule QtitanDataGrid
*/
GridCheckBoxEditorRepository::GridCheckBoxEditorRepository()
: GridEditorRepository(), m_tristate(false), m_nativeStyle(true)
{
    setEditorActivationPolicy(GridEditorActivationPolicy(AnyEvents | EatingMouseEvents | EatingKeyEvents));
    setAlignment(Qt::AlignCenter);
    m_defValueDisplay = false;
    m_defValueEdit = false;
}

GridEditor* GridCheckBoxEditorRepository::createEditor()
{
    return new GridCheckBoxEditor();
}

void GridCheckBoxEditorRepository::setTristate(bool state)
{
    m_tristate = state;
    changed();
}

bool GridCheckBoxEditorRepository::isTristate() const
{
    return m_tristate;
}

bool GridCheckBoxEditorRepository::nativeStyle() const
{
    return m_nativeStyle;
}

void GridCheckBoxEditorRepository::setNativeStyle(bool style)
{
    m_nativeStyle = style;
    changed();
}

/* GridCheckBoxEditor */
GridCheckBoxEditor::GridCheckBoxEditor()
: GridEditorBase(), m_checkEdit(Q_NULL)
{
}

GridCheckBoxEditor::~GridCheckBoxEditor()
{
    Q_ASSERT(!isEditMode());
    destroyEditModeContext();
}

QWidget* GridCheckBoxEditor::getCellWidget()
{
    if (m_checkEdit == Q_NULL)
        createEditModeContext();
    return m_checkEdit;
}

void GridCheckBoxEditor::setValueToWidget(const QVariant& value)
{
    if (m_checkEdit == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    m_checkEdit->setAttribute(Qt::WA_UpdatesDisabled, true);
    int val = Qt::Unchecked;
    if (((GridCheckBoxEditorRepository *)m_editorRepository)->isTristate())
    {
        val = value.toInt();
        if (val != Qt::Checked && val != Qt::Unchecked && val != Qt::PartiallyChecked)
            val = Qt::Unchecked;
    }
    else
    {
        val = value.toBool() ? Qt::Checked : Qt::Unchecked;
    }

    if (m_checkEdit->box()->checkState() != val)
        m_checkEdit->box()->setCheckState((Qt::CheckState)val);

    m_checkEdit->setAttribute(Qt::WA_UpdatesDisabled, false);
}

void GridCheckBoxEditor::setEditMode(bool mode)
{
    m_editMode = mode;
}

void GridCheckBoxEditor::stateChanged(int state)
{
    if (isContextUpdating())
        return;

    if (!isEditMode())
    {
        setValueToWidget(m_contextValue);
        return;
    }

    QVariant value;
    if (((GridCheckBoxEditorRepository *)m_editorRepository)->isTristate())
        value = state;
    else
        value = state == Qt::Checked;

    editingValueUpdated(value, false);
}

void GridCheckBoxEditor::updateContext(const QVariant& value)
{
    EditorAutoUpdater autoupdater(this);

    setValueToWidget(value);

    GridEditorBase::updateContext(value);
}

void GridCheckBoxEditor::createEditModeContext()
{
    m_checkEdit = new CheckEdit(site()->parent());
    if (site()->parent() == Q_NULL)
        m_checkEdit->setAttribute(Qt::WA_DontShowOnScreen);
    connect(m_checkEdit->box(), SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    repositoryUpdated();
}

void GridCheckBoxEditor::destroyEditModeContext()
{
    Q_DELETE_AND_NULL(m_checkEdit);
}

QString GridCheckBoxEditor::contextValueToString()
{
    return m_contextValue.toBool() ? Grid::tr_compatible(QtnGridTrueString) : Grid::tr_compatible(QtnGridFalseString);
}

void GridCheckBoxEditor::repositoryUpdated()
{
    if (m_checkEdit == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    GridEditorStyleOption option = site()->editorOption();

    if (m_editorRepository->transparentBackground() != m_checkEdit->testAttribute(Qt::WA_NoSystemBackground))
        m_checkEdit->setAttribute(Qt::WA_NoSystemBackground, m_editorRepository->transparentBackground());

    if (m_checkEdit->box()->isTristate() != static_cast<GridCheckBoxEditorRepository *>(m_editorRepository)->isTristate())
        m_checkEdit->box()->setTristate(static_cast<GridCheckBoxEditorRepository *>(m_editorRepository)->isTristate());

    if (m_checkEdit->nativeStyle() != static_cast<GridCheckBoxEditorRepository *>(m_editorRepository)->nativeStyle())
        m_checkEdit->setNativeStyle(static_cast<GridCheckBoxEditorRepository *>(m_editorRepository)->nativeStyle());

    if (m_checkEdit->alignment() != option.decorationAlignment)
        m_checkEdit->setAlignment(option.decorationAlignment);

    if (m_checkEdit->isEnabled() != m_editorRepository->isEnabled())
        m_checkEdit->setEnabled(m_editorRepository->isEnabled());
}

/* GridComboBoxEditorRepository */
GridComboBoxEditorRepository::GridComboBoxEditorRepository()
: GridEditorRepository(), m_comboBoxEditable(false)
{
    m_defValueDisplay = QVariant((int)QVariant::Int, (const void *)0);
    m_defValueEdit = QVariant((int)QVariant::Int, (const void *)0);
}

bool GridComboBoxEditorRepository::isComboBoxEditable() const
{
    return m_comboBoxEditable;
}

void GridComboBoxEditorRepository::setComboBoxEditable(bool editable)
{
    m_comboBoxEditable = editable;
}

GridEditor* GridComboBoxEditorRepository::createEditor()
{
    return new GridComboBoxEditor();
}

/* GridComboBoxEditor */
GridComboBoxEditor::GridComboBoxEditor()
: GridEditorBase(), m_comboBox(Q_NULL)
{
}

GridComboBoxEditor::~GridComboBoxEditor()
{
    Q_ASSERT(!isEditMode());
    destroyEditModeContext();
}

QWidget* GridComboBoxEditor::getCellWidget()
{
    return m_comboBox;
}

void GridComboBoxEditor::paintEvent(QPainter& painter, const GridEditorStyleOption& option)
{
    QString currentText = m_contextValue.toString();
    paintSimpleText(painter, option, currentText);
#if 0
    QPalette palette = option.palette;
    palette.setCurrentColorGroup(option.state & QStyle::State_Active ? QPalette::Active : QPalette::Inactive);
    const QColor fgcolor = palette.color(option.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text);
    painter.setPen(fgcolor);
    painter.setFont(option.font);
    QPixmap icon = option.icon.pixmap(option.rect.height(), option.state & QStyle::State_Enabled ? QIcon::Normal : QIcon::Disabled);

    paint_icon_and_text(painter, option.rect, icon,
        currentText, m_editorRepository->alignment(), Qt::ElideRight, m_editorRepository->iconApperance(), option.direction, Qt::Horizontal);
#endif

}

void GridComboBoxEditor::resolveValue(int index, QVariant& value)
{
    if (isEditMode())
    {
        if (site()->dataBinding()->relationMode() == GridModelDataBinding::Relation &&
            site()->dataBinding()->relationModel() != Q_NULL &&
            site()->dataBinding()->foreignKeyColumn() != -1)
        {
            QModelIndex modelIndex = site()->dataBinding()->relationModel()->index(
                index,
                site()->dataBinding()->foreignKeyColumn(),
                site()->dataBinding()->relationParentIndex());
            value = site()->dataBinding()->relationModel()->data(modelIndex, site()->dataBinding()->foreignKeyRole());
        }
        else
            value = index;
    }
    else
        value = index;
}

void GridComboBoxEditor::updateContext(const QVariant& value)
{
    EditorAutoUpdater autoupdater(this);
    if (isEditMode())
        setValueToWidget(value);
    GridEditorBase::updateContext(value);
}

void GridComboBoxEditor::createEditModeContext()
{
    Q_ASSERT(isEditMode());
    m_comboBox = new QComboBox(site()->parent());
    if (site()->parent() == Q_NULL)
        m_comboBox->setAttribute(Qt::WA_DontShowOnScreen);
    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));

    if (m_editorRepository->validator() != Q_NULL)
        m_comboBox->setValidator(m_editorRepository->validator());
    repositoryUpdated();
}

void GridComboBoxEditor::destroyEditModeContext()
{
    Q_DELETE_AND_NULL(m_comboBox);
}

void GridComboBoxEditor::repositoryUpdated()
{
    if (m_comboBox == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    GridEditorStyleOption option = site()->editorOption();

    if (m_editorRepository->transparentBackground() != m_comboBox->testAttribute(Qt::WA_NoSystemBackground))
        m_comboBox->setAttribute(Qt::WA_NoSystemBackground, m_editorRepository->transparentBackground());

    if (m_comboBox->isEnabled() != m_editorRepository->isEnabled())
        m_comboBox->setEnabled(m_editorRepository->isEnabled());
    if (m_comboBox->isEditable() != ((GridComboBoxEditorRepository *)m_editorRepository)->isComboBoxEditable())
        m_comboBox->setEditable(((GridComboBoxEditorRepository *)m_editorRepository)->isComboBoxEditable());

    if (m_comboBox->lineEdit() && (m_comboBox->lineEdit()->alignment() != option.decorationAlignment))
        m_comboBox->lineEdit()->setAlignment(option.decorationAlignment);
}

void GridComboBoxEditor::setValueToWidget(const QVariant& value)
{
    if (m_comboBox == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    m_comboBox->setAttribute(Qt::WA_UpdatesDisabled, true);
    switch (value.type())
    {
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QVariant::Double:
    case QVariant::String:
    case QVariant::Uuid:
        {
            if (site()->dataBinding()->relationMode() == GridModelDataBinding::Relation &&
                site()->dataBinding()->relationModel() != Q_NULL)
            {
                m_comboBox->setModel(site()->dataBinding()->relationModel());
                m_comboBox->setModelColumn(site()->dataBinding()->getForeignColumnMapping(Qt::DisplayRole));
                QModelIndex relationIndex = site()->dataBinding()->findRelationModelIndex(value);
                if (relationIndex.isValid())
                    m_comboBox->setCurrentIndex(relationIndex.row());
                else
                    m_comboBox->setCurrentIndex(-1);
            }
        }
        break;
    case QVariant::List:
        {
            m_comboBox->clear();
            QList<QVariant> list = value.toList();
            for (QList<QVariant>::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
                m_comboBox->addItem((*it).toString());
        }
        break;
    case QVariant::Map:
        {
            m_comboBox->clear();
            QMap<QString, QVariant> map = value.toMap();
            for (QMap<QString, QVariant>::const_iterator it = map.constBegin(); it != map.constEnd(); ++it)
            {
                QPixmap pixmap;
                qvariant_to_pixmap(it.value(), pixmap);
                m_comboBox->addItem(pixmap, it.key());
            }
        }
        break;
    case QVariant::StringList:
        {
            m_comboBox->clear();
            QStringList list = value.toStringList();
            for (QStringList::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
                m_comboBox->addItem(*it);
        }
        break;
    default:
        break;
    }
    m_comboBox->setAttribute(Qt::WA_UpdatesDisabled, false);
}

void GridComboBoxEditor::currentIndexChanged(int index)
{
    if (isContextUpdating())
        return;

    QVariant value;
    resolveValue(index, value);
    editingValueUpdated(value, true);
}

/* CheckBoxWidget */
CheckBoxWidget::CheckBoxWidget(QWidget* parent)
    : QCheckBox(parent), m_nativeStyle(true), m_mouseOverPaint(true)
{
}

bool CheckBoxWidget::isNativeStyleEnabled() const
{
    return m_nativeStyle;
}

void CheckBoxWidget::setNativeStyleEnabled(bool enabled)
{
    m_nativeStyle = enabled;
}

bool CheckBoxWidget::isMouseOverPaintEnabled() const
{
    return m_mouseOverPaint;
}

void CheckBoxWidget::setMouseOverPaintEnabled(bool enabled)
{
    m_mouseOverPaint = enabled;
}

void CheckBoxWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    if (m_nativeStyle)
    {
        QStylePainter p(this);
        QStyleOptionButton opt;
        initStyleOption(&opt);
        opt.state &= ~QStyle::State_KeyboardFocusChange;
        p.drawControl(QStyle::CE_CheckBox, opt);

    }
    else
    {
        QPainter painter(this);
        QStyleOptionButton opt;
        initStyleOption(&opt);
        opt.state &= ~QStyle::State_KeyboardFocusChange;
        QIcon icon;
        QImage image;
        if (opt.state & QStyle::State_On)
        {
            image = QImage(":res/grid_checkbox_on.png");
            if (opt.state & QStyle::State_Sunken)
                image = image.scaled(image.size() + QSize(2, 2), Qt::IgnoreAspectRatio/*, Qt::SmoothTransformation*/);
        }
        else
        {
            if (m_mouseOverPaint)
            {
                if (!hasFocus() && !(opt.state & QStyle::State_MouseOver))
                    return;
            }

            image = QImage(":res/grid_checkbox_off.png");
            if (opt.state & QStyle::State_Sunken)
                image = image.scaled(image.size() + QSize(2, 2), Qt::IgnoreAspectRatio/*, Qt::SmoothTransformation*/);
        }
        qtn_set_shadecolor(image, Qt::white);
        icon = QIcon(QPixmap::fromImage(image));

        QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
        icon.paint(&painter, QRect(QPoint(0, 0), size()), Qt::AlignHCenter | Qt::AlignVCenter, mode);
    }
}

/* CheckEdit */
CheckEdit::CheckEdit(QWidget* parent)
: QWidget(parent), m_alignment(Qt::AlignVCenter | Qt::AlignHCenter)
{
    m_cb = new Qtitan::CheckBoxWidget(this);
    setFocusProxy(m_cb);
}

QCheckBox* CheckEdit::box() const
{
    return m_cb;
}

void CheckEdit::setAlignment(Qt::Alignment flag)
{
    m_alignment = flag;
    updateAlign();
}

Qt::Alignment CheckEdit::alignment() const
{
    return m_alignment;
}

void CheckEdit::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    updateAlign();
}

void CheckEdit::focusInEvent(QFocusEvent* e)
{
    QWidget::focusInEvent(e);
}

class QCheckBoxPublic: public QCheckBox
{
    friend class Qtitan::CheckEdit;
};

bool CheckEdit::event(QEvent* e)
{
    switch (e->type())
    {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
        return ((QCheckBoxPublic *)m_cb)->event(e);
    default:
        break;
    }
    return QWidget::event(e);
}

void CheckEdit::updateAlign()
{
    QRect cbrect = QStyle::alignedRect(layoutDirection(),
        alignment(), m_cb->sizeHint(), rect());
    m_cb->setGeometry(cbrect);
}

bool CheckEdit::nativeStyle() const
{
    return static_cast<Qtitan::CheckBoxWidget *>(m_cb)->m_nativeStyle;
}

void CheckEdit::setNativeStyle(bool style)
{
    static_cast<Qtitan::CheckBoxWidget *>(m_cb)->m_nativeStyle = style;
    update();
}

/*!
\class GridDateTimeEditorRepository
\inmodule QtitanDataGrid
*/
GridDateTimeEditorRepository::GridDateTimeEditorRepository()
: GridEditorRepository(), m_calendarPopup(true),
  m_maximumDateTime(QDATETIMEEDIT_DATETIME_MAX), m_maximumDate(QDATETIMEEDIT_DATE_MAX), m_maximumTime(QDATETIMEEDIT_TIME_MAX), 
  m_minimumDate(QDATETIMEEDIT_DATE_MIN), m_minimumDateTime(QDATETIMEEDIT_DATETIME_MIN), m_minimumTime(QDATETIMEEDIT_TIME_MIN),
  m_timeSpec(Qt::LocalTime)
{
    setEditorActivationPolicy(AnyEvents);
    const QLocale loc;
    setDisplayFormat(loc.dateTimeFormat(QLocale::ShortFormat));
    m_defValueDisplay = QDateTime();
    m_defValueEdit = QDateTime();
}

GridEditor* GridDateTimeEditorRepository::createEditor()
{
    return new GridDateTimeEditor();
}

bool GridDateTimeEditorRepository::calendarPopup() const
{
    return m_calendarPopup;
}

void GridDateTimeEditorRepository::setCalendarPopup(bool flag)
{
    m_calendarPopup = flag;
    changed();
}

QDateTime GridDateTimeEditorRepository::maximumDateTime() const
{
    return m_maximumDateTime;
}

void GridDateTimeEditorRepository::setMaximumDateTime(const QDateTime& dt)
{
    m_maximumDateTime = dt;
    changed();
}

QDate GridDateTimeEditorRepository::maximumDate() const
{
    return m_maximumDate;
}

void GridDateTimeEditorRepository::setMaximumDate(const QDate& max)
{
    m_maximumDate = max;
    changed();
}

QTime GridDateTimeEditorRepository::maximumTime() const
{
    return m_maximumTime;
}

void GridDateTimeEditorRepository::setMaximumTime(const QTime& max)
{
    m_maximumTime = max;
    changed();
}

QDate GridDateTimeEditorRepository::minimumDate() const
{
    return m_minimumDate;
}

void GridDateTimeEditorRepository::setMinimumDate(const QDate& min)
{
    m_minimumDate = min;
    changed();
}

QDateTime GridDateTimeEditorRepository::minimumDateTime() const
{
    return m_minimumDateTime;
}

void GridDateTimeEditorRepository::setMinimumDateTime(const QDateTime& dt)
{
    m_minimumDateTime = dt;
    changed();
}

QTime GridDateTimeEditorRepository::minimumTime() const
{
    return m_minimumTime;
}

void GridDateTimeEditorRepository::setMinimumTime(const QTime& min)
{
    m_minimumTime = min;
    changed();
}

Qt::TimeSpec GridDateTimeEditorRepository::timeSpec() const
{
    return m_timeSpec;
}

void GridDateTimeEditorRepository::setTimeSpec(Qt::TimeSpec spec)
{
    m_timeSpec = spec;
    changed();
}

const QString& GridDateTimeEditorRepository::displayFormat() const
{
    return m_displayFormat;
}

void GridDateTimeEditorRepository::setDisplayFormat(const QString& format)
{
    m_displayFormat = format;
    changed();
}

void GridDateTimeEditorRepository::getDateRange(QDate &min, QDate& max) const
{
    min = minimumDate();
    max = maximumDate();
}

void GridDateTimeEditorRepository::setDateRange(const QDate& min, const QDate& max)
{
    setMinimumDate(min);
    setMaximumDate(max);
}

void GridDateTimeEditorRepository::getDateTimeRange(QDateTime& min, QDateTime& max) const
{
    min = minimumDateTime();
    max = maximumDateTime();
}

void GridDateTimeEditorRepository::setDateTimeRange(const QDateTime& min, const QDateTime& max)
{
    setMinimumDateTime(min);
    setMaximumDateTime(max);
}

void GridDateTimeEditorRepository::getTimeRange(QTime& min, QTime& max) const
{
    min = minimumTime();
    max = maximumTime();
}

void GridDateTimeEditorRepository::setTimeRange(const QTime& min, const QTime& max)
{
    setMinimumTime(min);
    setMaximumTime(max);
}

/* GridDateTimeEditor */
GridDateTimeEditor::GridDateTimeEditor()
: GridEditorBase(), m_dtEdit(Q_NULL)
{
}

GridDateTimeEditor::~GridDateTimeEditor()
{
    Q_ASSERT(!isEditMode());
    destroyEditModeContext();
}

QWidget* GridDateTimeEditor::getCellWidget()
{
    return m_dtEdit;
}

QString GridDateTimeEditor::contextValueToString()
{
    return m_contextValue.toDateTime().toString(((Qtitan::GridDateTimeEditorRepository *)m_editorRepository)->displayFormat());
}

void GridDateTimeEditor::updateContext(const QVariant& value)
{
    EditorAutoUpdater autoupdater(this);
    if (isEditMode())
    {
        setValueToWidget(value);
        if (m_dtEdit != Q_NULL && !m_editorRepository->autoSelect())
            m_dtEdit->setSelectedSection(QDateTimeEdit::NoSection);
    }
    GridEditorBase::updateContext(value);
}

void GridDateTimeEditor::createEditModeContext()
{
    Q_ASSERT(isEditMode());
    m_dtEdit = createDateTimeWidget();
    if (site()->parent() == Q_NULL)
    m_dtEdit->setAttribute(Qt::WA_DontShowOnScreen);
    connect(m_dtEdit, SIGNAL(dateTimeChanged(const QDateTime&)), this, SLOT(dateTimeChanged(const QDateTime&)));
    repositoryUpdated();
}

void GridDateTimeEditor::destroyEditModeContext()
{
    Q_DELETE_AND_NULL(m_dtEdit);
}

QDateTimeEdit* GridDateTimeEditor::createDateTimeWidget()
{
    return new QDateTimeEdit(site()->parent());
}

class QDateTimeEditPublic: public QDateTimeEdit
{
    friend class GridDateTimeEditor;
};

void GridDateTimeEditor::repositoryUpdated()
{
    if (m_dtEdit == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    GridEditorStyleOption option = site()->editorOption();

    if (m_editorRepository->transparentBackground() != m_dtEdit->testAttribute(Qt::WA_NoSystemBackground))
        m_dtEdit->setAttribute(Qt::WA_NoSystemBackground, m_editorRepository->transparentBackground());

    if (m_dtEdit->calendarPopup() != ((GridDateTimeEditorRepository *)m_editorRepository)->calendarPopup())
        m_dtEdit->setCalendarPopup(((GridDateTimeEditorRepository *)m_editorRepository)->calendarPopup());

    if (m_dtEdit->maximumDateTime() != ((GridDateTimeEditorRepository *)m_editorRepository)->maximumDateTime())
        m_dtEdit->setMaximumDateTime(((GridDateTimeEditorRepository *)m_editorRepository)->maximumDateTime());

    if (m_dtEdit->maximumDate() != ((GridDateTimeEditorRepository *)m_editorRepository)->maximumDate())
        m_dtEdit->setMaximumDate(((GridDateTimeEditorRepository *)m_editorRepository)->maximumDate());

    if (m_dtEdit->maximumTime() != ((GridDateTimeEditorRepository *)m_editorRepository)->maximumTime())
        m_dtEdit->setMaximumTime(((GridDateTimeEditorRepository *)m_editorRepository)->maximumTime());

    if (m_dtEdit->minimumDateTime() != ((GridDateTimeEditorRepository *)m_editorRepository)->minimumDateTime())
        m_dtEdit->setMinimumDateTime(((GridDateTimeEditorRepository *)m_editorRepository)->minimumDateTime());

    if (m_dtEdit->minimumDate() != ((GridDateTimeEditorRepository *)m_editorRepository)->minimumDate())
        m_dtEdit->setMinimumDate(((GridDateTimeEditorRepository *)m_editorRepository)->minimumDate());

    if (m_dtEdit->minimumTime() != ((GridDateTimeEditorRepository *)m_editorRepository)->minimumTime())
        m_dtEdit->setMinimumTime(((GridDateTimeEditorRepository *)m_editorRepository)->minimumTime());

    if (m_dtEdit->timeSpec() != ((GridDateTimeEditorRepository *)m_editorRepository)->timeSpec())
        m_dtEdit->setTimeSpec(((GridDateTimeEditorRepository *)m_editorRepository)->timeSpec());

    if (m_dtEdit->displayFormat() != ((GridDateTimeEditorRepository *)m_editorRepository)->displayFormat())
        m_dtEdit->setDisplayFormat(((GridDateTimeEditorRepository *)m_editorRepository)->displayFormat());

    if (m_dtEdit->isEnabled() != m_editorRepository->isEnabled())
        m_dtEdit->setEnabled(m_editorRepository->isEnabled());

    if (m_dtEdit->alignment() != option.decorationAlignment)
        m_dtEdit->setAlignment(option.decorationAlignment);
}

void GridDateTimeEditor::setValueToWidget(const QVariant& value)
{
    EditorAutoUpdater autoupdater(this);

    //if (value.toDateTime() != m_dtEdit->dateTime())
    {
        m_dtEdit->setAttribute(Qt::WA_UpdatesDisabled, true);
        switch(value.type())
        {
        case QVariant::Date:
            m_dtEdit->setDate(value.toDate());
            break;
        case QVariant::Time:
            m_dtEdit->setTime(value.toTime());
            break;
        default:
            m_dtEdit->setDateTime(value.toDateTime());
        }

        if (m_editorRepository->autoSelect())
            m_dtEdit->selectAll();

        m_dtEdit->setAttribute(Qt::WA_UpdatesDisabled, false);
    }
}

void GridDateTimeEditor::dateTimeChanged(const QDateTime& datetime)
{
    editingValueUpdated(datetime, false);
}

/*!
\class GridTimeEditorRepository
\inmodule QtitanDataGrid
*/
GridTimeEditorRepository::GridTimeEditorRepository()
: GridDateTimeEditorRepository()
{
    const QLocale loc;
    setDisplayFormat(loc.timeFormat(QLocale::ShortFormat));
    m_defValueDisplay = QTime();
    m_defValueEdit = QTime();
}

GridEditor* GridTimeEditorRepository::createEditor()
{
    return new GridTimeEditor();
}

/* GridTimeEditor */
GridTimeEditor::GridTimeEditor()
: GridDateTimeEditor()
{
}

QString GridTimeEditor::contextValueToString()
{
    return m_contextValue.toTime().toString(((Qtitan::GridDateTimeEditorRepository *)m_editorRepository)->displayFormat());
}

QDateTimeEdit* GridTimeEditor::createDateTimeWidget()
{
    return new QTimeEdit(site()->parent());
}

/*!
\class GridDateEditorRepository
\inmodule QtitanDataGrid
*/
GridDateEditorRepository::GridDateEditorRepository()
: GridDateTimeEditorRepository()
{
    const QLocale loc;
    setDisplayFormat(loc.dateFormat(QLocale::ShortFormat));
    m_defValueDisplay = QDate();
    m_defValueEdit = QDate();
}

GridEditor* GridDateEditorRepository::createEditor()
{
    return new GridDateEditor();
}


/* GridDateEditor */
GridDateEditor::GridDateEditor()
: GridDateTimeEditor()
{
}

QDateTimeEdit* GridDateEditor::createDateTimeWidget()
{
    return new QDateEdit(site()->parent());
}

QString GridDateEditor::contextValueToString()
{
    return m_contextValue.toDate().toString(((Qtitan::GridDateTimeEditorRepository *)m_editorRepository)->displayFormat());
}

/*! 
\class GridPictureEditorRepository
\inmodule QtitanDataGrid
*/
GridPictureEditorRepository::GridPictureEditorRepository()
: GridEditorRepository(),
  m_transformMode(Qt::FastTransformation),
  m_aspectRatioMode(Qt::KeepAspectRatio), m_scaled(true)
{
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QPixmap pixmap(8, 8);
    pixmap.fill(Qt::transparent);
    QVariant defValue = qtn_pixmap_to_qvariant(QVariant::ByteArray, pixmap);
    m_defValueDisplay = defValue;
    m_defValueEdit = defValue;
}

GridEditor* GridPictureEditorRepository::createEditor()
{
    return new GridPictureEditor();
}

Qt::TransformationMode GridPictureEditorRepository::transformMode() const
{
    return m_transformMode;
}

void GridPictureEditorRepository::setTransformMode(Qt::TransformationMode mode)
{
    m_transformMode = mode;
    changed();
}

Qt::AspectRatioMode GridPictureEditorRepository::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void GridPictureEditorRepository::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    m_aspectRatioMode = mode;
    changed();
}

bool GridPictureEditorRepository::isScaled() const
{
    return m_scaled;
}

void GridPictureEditorRepository::setScaled(bool scaled)
{
    m_scaled = scaled;
    changed();
}

static void qvariant_to_image(const QVariant& v, QImage& image)
{
    image = QImage();
    switch (v.type())
    {
    case QVariant::ByteArray:
        {
            image.loadFromData(v.value<QByteArray>());
        }
        break;
    case QVariant::Pixmap:
    case QVariant::Bitmap:
    case QVariant::Icon:
    case QVariant::Image:
        {
            image = v.value<QImage>();
        }
        break;
    default:
        image = QImage();
    }
}

int GridPictureEditorRepository::heightHint(int width, const QVariant& v)
{
    QImage im;
    qvariant_to_image(v, im);
    double scale = (double)width / (double)im.width();
    return im.height() * scale;
}

void GridPictureEditorRepository::hidePictureEditor()
{
    if (view() != Q_NULL)
        view()->closeEditor();
}

const QString& GridPictureEditorRepository::imageEmptyText() const
{
    return m_imageEmptyText;
}

void GridPictureEditorRepository::setImageEmptyText(const QString& text)
{
    m_imageEmptyText = text;
    changed();
}

QVariant GridPictureEditorRepository::convertToVariant(QVariant::Type type, const QPixmap& pixmap)
{
    return qtn_pixmap_to_qvariant(type, pixmap);
}

/* GridPictureEditor */
GridPictureEditor::GridPictureEditor()
: GridEditorBase(), m_inplaceWidget(Q_NULL)
{
}

GridPictureEditor::~GridPictureEditor()
{
    Q_ASSERT(!isEditMode());
    destroyEditModeContext();
}

QWidget* GridPictureEditor::getCellWidget()
{
    return m_inplaceWidget;
}

void GridPictureEditor::paintEvent(QPainter& painter, const GridEditorStyleOption& option)
{
    Q_UNUSED(option);
    if (m_displayPixmap.isNull() || m_displayPixmap.width() == 0 || m_displayPixmap.height() == 0)
    {
        QPalette palette = option.palette;
        palette.setCurrentColorGroup(option.state & QStyle::State_Active ? QPalette::Active : QPalette::Inactive);
#if 0
         const QColor bgcolor = 
            option.state & QStyle::State_Selected ? 
            palette.color(QPalette::Highlight) : (option.state & QStyle::State_On ? 
            palette.color(QPalette::AlternateBase) : palette.color(QPalette::Base));
#endif
        const QColor fgcolor = palette.color(option.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text);
     
        painter.setPen(fgcolor);
        painter.setFont(option.font);
        draw_text(painter, static_cast<GridPictureEditorRepository *>(m_editorRepository)->imageEmptyText(), option.rect, 
            m_editorRepository->alignment());
        return;
    }

    QPixmap scaled;
    if (static_cast<GridPictureEditorRepository *>(m_editorRepository)->isScaled())
        scaled = m_displayPixmap.scaled(site()->geometry().size(),
            static_cast<GridPictureEditorRepository *>(m_editorRepository)->aspectRatioMode(),
            static_cast<GridPictureEditorRepository *>(m_editorRepository)->transformMode());
    else
        scaled = m_displayPixmap;

    QRect paintRect = QStyle::alignedRect(m_editorRepository->view()->grid()->layoutDirection(),
        m_editorRepository->alignment(), scaled.size(), site()->geometry());
    painter.drawPixmap(paintRect, scaled);
}

class GridViewBasePublic: public GridViewBase
{
    GridViewBasePublic(): GridViewBase(0) {}
    friend class GridPictureEditor;
};

void GridPictureEditor::setValueToWidget(const QVariant& value)
{
    if (m_inplaceWidget == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    QPixmap pixmap;
    qvariant_to_pixmap(value, pixmap);
    m_inplaceWidget->setAttribute(Qt::WA_UpdatesDisabled, true);
    m_inplaceWidget->setPixmap(pixmap);
    m_inplaceWidget->setAttribute(Qt::WA_UpdatesDisabled, false);
}

void GridPictureEditor::updateContext(const QVariant& value)
{
    EditorAutoUpdater autoupdater(this);

    if (isEditMode())
        setValueToWidget(value);
    else
        qvariant_to_pixmap(value, m_displayPixmap);

    GridEditorBase::updateContext(value);
}

void GridPictureEditor::createEditModeContext()
{
    Q_ASSERT(isEditMode());
    m_inplaceWidget = new PictureEditorWidget(site()->parent());
    if (site()->parent() == Q_NULL)
        m_inplaceWidget->setAttribute(Qt::WA_DontShowOnScreen);
    connect(m_inplaceWidget, SIGNAL(pictureChanged()), this, SLOT(pictureChanged()));
    repositoryUpdated();
}

void GridPictureEditor::destroyEditModeContext()
{
    Q_DELETE_AND_NULL(m_inplaceWidget);
}

void GridPictureEditor::repositoryUpdated()
{
    if (m_inplaceWidget == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    if (m_editorRepository->transparentBackground() != m_inplaceWidget->testAttribute(Qt::WA_NoSystemBackground))
        m_inplaceWidget->setAttribute(Qt::WA_NoSystemBackground, m_editorRepository->transparentBackground());

    if (m_inplaceWidget->isEnabled() != m_editorRepository->isEnabled())
        m_inplaceWidget->setEnabled(m_editorRepository->isEnabled());
}

void GridPictureEditor::pictureChanged()
{
    QVariant value = qtn_pixmap_to_qvariant(editorRepository()->defaultValue(Qt::EditRole).type(), *m_inplaceWidget->pixmap());
    editingValueUpdated(value, false);
}

/*!
\class Qtitan::CustomComboBox
\internal
*/
CustomComboBox::CustomComboBox(QWidget* parent)
:QWidget(parent),
 arrowState(QStyle::State_None),
 hoverControl(QStyle::SC_None), m_popup(Q_NULL), m_modified(false)
{
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);
}

CustomComboBox::~CustomComboBox()
{
    Q_DELETE_AND_NULL(m_popup);
}

bool CustomComboBox::isModified() const
{
    return m_modified;
}

void CustomComboBox::setModified(bool modified)
{
    m_modified = modified;
    if (!modified)
        saveDefaultState();
    updatePopupButtons();
}

void CustomComboBox::showPopup()
{
    if (m_popup == Q_NULL)
        return;
    updatePopupButtons();
    QRect g = geometry();
    QPoint gpos = mapToGlobal(QPoint(0, 0));
    g.moveTo(gpos.x(), gpos.y() + g.height());
    g.setWidth(m_popup->width());
    g.setHeight(m_popup->height());
    m_popup->setGeometry(g);
#ifndef QT_NO_EFFECTS
    qScrollEffect(m_popup, true ? QEffects::DownScroll : QEffects::UpScroll, 150);
#endif
    m_popup->raise();
    m_popup->show();
}

bool CustomComboBox::okButtonVisible()
{
    return m_popup->okButton()->isVisible();
}

void CustomComboBox::setOkButtonVisible(bool visible)
{
    m_popup->okButton()->setVisible(visible);
}

bool CustomComboBox::cancelButtonVisible() const
{
    return m_popup->cancelButton()->isVisible();
}

void CustomComboBox::setCancelButtonVisible(bool visible) const
{
    m_popup->cancelButton()->setVisible(visible);
}

bool CustomComboBox::crossButtonVisible() const
{
    return m_popup->crossButton()->isVisible();
}

void CustomComboBox::setCrossButtonVisible(bool visible)
{
    m_popup->crossButton()->setVisible(visible);
}

void CustomComboBox::saveDefaultState()
{
}

QStyle::SubControl CustomComboBox::newHoverControl(const QPoint &pos)
{
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    opt.subControls = QStyle::SC_All;
    hoverControl = style()->hitTestComplexControl(QStyle::CC_ComboBox, &opt, pos, this);
    hoverRect = (hoverControl != QStyle::SC_None)
                   ? style()->subControlRect(QStyle::CC_ComboBox, &opt, hoverControl, this)
                   : QRect();
    return hoverControl;
} 

bool CustomComboBox::updateHoverControl(const QPoint &pos)
{
    QRect lastHoverRect = hoverRect;
    QStyle::SubControl lastHoverControl = hoverControl;
    bool doesHover = testAttribute(Qt::WA_Hover);
    if (lastHoverControl != newHoverControl(pos) && doesHover)
    {
        update(lastHoverRect);
        update(hoverRect);
        return true;
    }
    return !doesHover;
} 

void CustomComboBox::updateArrow(QStyle::StateFlag state)
{
    if (arrowState == state)
        return;
    arrowState = state;
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    update(style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this));
} 

bool CustomComboBox::event(QEvent *event)
{
    switch(event->type())
    {
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::HoverMove:
    if (const QHoverEvent *he = static_cast<const QHoverEvent *>(event))
        updateHoverControl(he->pos());
        break; 
    default:
        break;
    }
    return QWidget::event(event); 
}

void CustomComboBox::initStyleOption(QStyleOptionComboBox *option) const
{
    if (!option)
        return;
    option->initFrom(this);
    option->editable = true;//isEditable();
    //option->frame = frame;
    if (hasFocus() && !option->editable)
        option->state |= QStyle::State_Selected;
    option->subControls = QStyle::SC_All;
    if (arrowState == QStyle::State_Sunken) {
        option->activeSubControls = QStyle::SC_ComboBoxArrow;
        option->state |= arrowState;
    } else {
        option->activeSubControls = hoverControl;
    }
} 

void CustomComboBox::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);
    if (m_icon.isNull())
        m_icon = getIcon();
    QRect editRect = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);
    QIcon::Mode mode = opt.state & QStyle::State_Enabled ? QIcon::Normal : QIcon::Disabled;
    m_icon.paint(&painter, editRect, Qt::AlignHCenter | Qt::AlignVCenter, mode);
}

void CustomComboBox::mousePressEvent(QMouseEvent *e)
{
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    QStyle::SubControl sc = style()->hitTestComplexControl(QStyle::CC_ComboBox, &opt, e->pos(),
                                                           this);
    if (e->button() == Qt::LeftButton && (sc == QStyle::SC_ComboBoxArrow))
    {
        if (sc == QStyle::SC_ComboBoxArrow)
            updateArrow(QStyle::State_Sunken);
        showPopup();
    } else {
        QWidget::mousePressEvent(e);
    }
}

void CustomComboBox::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    updateArrow(QStyle::State_None);
}

void CustomComboBox::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Down:
        if (e->modifiers() & Qt::AltModifier) {
            showPopup();
            return;
        }
        break;
    case Qt::Key_F4:
        if (!e->modifiers()) {
            showPopup();
            return;
        }
        break;
    case Qt::Key_Space:
        if (!e->modifiers()) {
            showPopup();
            return;
        }
        break;
    default:
        e->ignore();
    }
}

void CustomComboBox::updatePopupButtons()
{
    if (m_popup != Q_NULL)
        m_popup->okButton()->setDisabled(!isModified());
}

class QSizeGripEx: public QSizeGrip
{
public:
    QSizeGripEx(QWidget* parent)
        : QSizeGrip(parent), m_mousePress(false)
    {
    }

    void mouseMoveEvent(QMouseEvent* mouseEvent)
    {
        if (/*mouseEvent->buttons() != Qt::LeftButton || */!m_mousePress)
        {
            QWidget::mouseMoveEvent(mouseEvent);
            return;
        }
        QPoint curpos = mouseEvent->globalPos();
        QRect r = m_r.adjusted(0, 0, curpos.x() - m_pos.x(), curpos.y() - m_pos.y());
        parentWidget()->resize(r.size());
    }

    void mouseReleaseEvent(QMouseEvent* mouseEvent)
    {
        if (m_mousePress/* mouseEvent->button() == Qt::LeftButton*/)
        {
            m_mousePress = false;
            m_pos = QPoint();
            m_r = QRect();
            releaseMouse();
        }
        else
            QWidget::mouseReleaseEvent(mouseEvent);
    }

    void mousePressEvent(QMouseEvent* mouseEvent)
    {
        if (mouseEvent->buttons() != Qt::LeftButton)
        {
            QWidget::mouseMoveEvent(mouseEvent);
            return;
        }
        m_pos = mouseEvent->globalPos();
        m_mousePress = true;
        m_r = parentWidget()->geometry();
        grabMouse();
    }
private:
    bool m_mousePress;
    QPoint m_pos;
    QRect m_r;
};

/*!
\class Qtitan::CustomComboBoxPopup
\internal
*/
CustomComboBoxPopup::CustomComboBoxPopup(CustomComboBox* parent, 
                                         bool okButton,
                                         bool cancelButton,
                                         bool crossButton)
  : EditorPopup(parent, okButton, cancelButton, crossButton) , m_comboBox(parent)
{
}

void CustomComboBoxPopup::hideEvent(QHideEvent* e)
{
    EditorPopup::hideEvent(e);
    m_comboBox->hidePopup();
}

void CustomComboBoxPopup::cancelClick()
{
    m_comboBox->undoModified();
    m_comboBox->setModified(false);
    EditorPopup::cancelClick();
}

/* GridToolButton */
GridToolButton::GridToolButton(QWidget* parent)
    : QAbstractButton(parent), m_autoRise(false)
{
    setMouseTracking(true);
}

bool GridToolButton::autoRise() const
{
    return m_autoRise;
}

void GridToolButton::setAutoRaise(bool ar)
{
    m_autoRise = ar;
}

void GridToolButton::paintTexture(QPainter& painter, const QColor& accentColor, const QRect& rect)
{
    QPixmap texture(":res/grid_glass_bg.png");
    QPixmap temp(texture.width(), texture.height());
    temp.fill(accentColor);
    temp.setMask(texture.createHeuristicMask());
    {
        QPainter p(&temp);
        p.drawPixmap(0, 0, texture);
    }
    draw_pixmap_texture(painter, rect, temp, 6, 6, 5, 5);
}

QPalette GridToolButton::getButtonPalette() const
{
    QPalette retval = palette();
    QObject* p = ((QObject *)this)->parent();
    while (p != Q_NULL)
    {
        Grid* grid = qobject_cast<Grid *>(p);
        if (grid != Q_NULL)
        {
            retval.setColor(QPalette::Mid, qtn_get_thememanager(grid)->shadePalette().mid().color());
            retval.setColor(QPalette::Light, qtn_get_thememanager(grid)->shadePalette().light().color());
            retval.setColor(QPalette::Dark, qtn_get_thememanager(grid)->shadePalette().dark().color());
            break;
        }
        p = p->parent();
    }
    return retval;
}

void GridToolButton::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    QPalette pal = getButtonPalette();
    painter.save();
    QColor accentColor;
    if (underMouse() && isDown())
    {
        accentColor = pal.color(QPalette::Dark);
    }
    else
    {
        if (underMouse())
            accentColor = pal.color(QPalette::Light);
        else if (!autoRise())
            accentColor = pal.color(QPalette::Mid);
        else
            accentColor = Qt::transparent;
    }
    QRect r = QRect(0, 0, width(), height());
    if (accentColor != Qt::transparent)
    {
        //        painter.setOpacity(0.5);
        paintTexture(painter, accentColor, r);
    }
    painter.setFont(font());
    painter.setOpacity(1.0);
    r.adjust(5, 0, -5, 0);
    paint_icon_and_text(painter, r, icon().pixmap(width()), text(), Qt::AlignCenter, Qt::ElideRight, text().isEmpty() ? IconOnly : IconAlignLeft);
    painter.restore();
}

QSize GridToolButton::sizeHint() const
{
    QSize s = get_column_sizeHint(Q_NULL, style(), font(), 1.0, text());
    if (!icon().isNull())
        s.rwidth() += s.height() + 5;
    return s;
}


/*!
\class Qtitan::EditorPopup
\internal
*/
EditorPopup::EditorPopup(QWidget* parent, bool okButton, bool cancelButton, bool crossButton)
: QWidget(parent, Qt::Popup), m_autoHiding(false)
{
    QPixmap cancelPixmap(":res/gridcancel.png");
    cancelPixmap.setMask(cancelPixmap.createMaskFromColor(Qt::magenta));

    QPixmap crossPixmap(":res/gridcross.png");
    crossPixmap.setMask(crossPixmap.createMaskFromColor(Qt::magenta));

    m_okButton = new GridToolButton(this);
    connect(m_okButton, SIGNAL(clicked()), this, SLOT(okClick())); 
    m_okButton->setText(Grid::tr_compatible(QtnGridOkString));
    m_okButton->setDisabled(true);
    m_okButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_okButton->setVisible(okButton);

    m_cancelButton = new GridToolButton(this);
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(cancelClick())); 
    m_cancelButton ->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_cancelButton->setText(Grid::tr_compatible(QtnGridCancelString));
    m_cancelButton->setIcon(cancelPixmap);
    m_cancelButton->setVisible(cancelButton);

    m_closeButton = new GridToolButton(this);
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(closeClick())); 
    m_closeButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_closeButton->setIcon(crossPixmap);
    m_closeButton->setText(Grid::tr_compatible(QtnGridCloseString));
    m_closeButton->setVisible(crossButton);

#ifndef Q_OS_WIN
    m_grip = new QSizeGrip(this);
#else
    m_grip = new QSizeGripEx(this);
#endif
}

void EditorPopup::setCentralWidget(QWidget* w)
{
    setMaximumSize(QSize(1000, 1000));
    m_centralWidget = w;
    m_centralWidget->setParent(this);
    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->addWidget(m_centralWidget, 0, 0, 1, 5);
    QHBoxLayout* hboxlayout = new QHBoxLayout();
    hboxlayout->addWidget(m_closeButton);
    hboxlayout->addWidget(m_okButton);
    hboxlayout->addWidget(m_cancelButton);
    gridLayout->addLayout(hboxlayout, 1, 3);
    hboxlayout->setContentsMargins(10, 10, 10, 10);
    gridLayout->addWidget(m_grip, 1, 4, Qt::AlignRight | Qt::AlignBottom);
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(2, 0);
    gridLayout->setColumnStretch(3, 0);
    gridLayout->setColumnStretch(4, 0);
    gridLayout->setContentsMargins(3, 3, 3, 3);
    this->setLayout(gridLayout);
}

void EditorPopup::setAutoHiding(bool flag)
{
    m_autoHiding = flag;
}

QAbstractButton* EditorPopup::okButton() const
{
    return m_okButton;
}

QAbstractButton* EditorPopup::cancelButton() const
{
    return m_cancelButton;
}

QAbstractButton* EditorPopup::crossButton() const
{
    return m_closeButton;
}

void EditorPopup::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(Qt::gray);
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
}

void EditorPopup::setVisible(bool visible)
{
    if (!visible && m_autoHiding)
        return;
    QWidget::setVisible(visible);
}

QWidget* EditorPopup::centralWidget() const
{
    return m_centralWidget;
}

void EditorPopup::okClick()
{
    close();
}

void EditorPopup::cancelClick()
{
    close();
}

void EditorPopup::closeClick()
{
    close();
}

/*!
\class Qtitan::PictureEditorPopup
\internal
*/
PictureEditorPopup::PictureEditorPopup(PictureComboBox* parent)
: CustomComboBoxPopup(parent)
{
}

/*!
\class Qtitan::PictureEditorWidget
\internal
*/
PictureEditorWidget::PictureEditorWidget(QWidget* parent)
: QScrollArea(parent), m_zoomFactor(1), m_modified(false)
{
    create();
}

void PictureEditorWidget::setPixmapInternal(const QPixmap& pixmap, bool modified)
{
    m_image->setPixmap(pixmap);
    m_image->adjustSize();
    m_modified = modified;
    if (modified)
        emit pictureChanged();
}

void PictureEditorWidget::setPixmap(const QPixmap& pixmap)
{
    setPixmapInternal(pixmap, false);
}

const QPixmap* PictureEditorWidget::pixmap() const
{
    return m_image->pixmap();
}

bool PictureEditorWidget::isModified() const
{
    return m_modified;
}

void PictureEditorWidget::setModified(bool m)
{
    m_modified = m;
}

void PictureEditorWidget::contextMenuEvent(QContextMenuEvent* event)
{
    m_contextMenu->move(event->globalX(), event->globalY());
    m_contextMenu->show();
}

void PictureEditorWidget::create()
{
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setBackgroundRole(QPalette::Light);
    m_image = new QLabel(this);
    m_image ->setScaledContents(true); 
    m_image->adjustSize();
    setWidget(m_image);
    createActions();
}

void PictureEditorWidget::createActions()
{
    m_contextMenu = new QMenu(this);
    m_loadAction = m_contextMenu->addAction(Grid::tr_compatible(QtnGridActionLoadString), this, SLOT(load()), QKeySequence::Open);
    m_saveAction = m_contextMenu->addAction(Grid::tr_compatible(QtnGridActionSaveString), this, SLOT(save()), QKeySequence::Save);
    m_contextMenu->addSeparator();
    m_copyAction = m_contextMenu->addAction(Grid::tr_compatible(QtnGridActionCopyString), this, SLOT(copy()), QKeySequence::Copy);
    m_pasteAction = m_contextMenu->addAction(Grid::tr_compatible(QtnGridActionPasteString), this, SLOT(paste()), QKeySequence::Paste);
    m_zoomInAction = m_contextMenu->addAction(Grid::tr_compatible(QtnGridActionZoomInString), this, SLOT(zoomIn()), QKeySequence::ZoomIn);
    m_zoomOutAction = m_contextMenu->addAction(Grid::tr_compatible(QtnGridActionZoomOutString), this, SLOT(zoomOut()), QKeySequence::ZoomOut);
    m_contextMenu->addSeparator();
    m_adjustSizeAction = m_contextMenu->addAction(Grid::tr_compatible(QtnGridActionAdjustSizeString), this, SLOT(adjustSize()));
    m_actualSizeAction = m_contextMenu->addAction(Grid::tr_compatible(QtnGridActionActualSizeString), this, SLOT(actualSize()));
    m_contextMenu->addSeparator();
    m_printAction = m_contextMenu->addAction(Grid::tr_compatible(QtnGridActionPrintString), this, SLOT(print()));
    
    addAction(m_loadAction);
    addAction(m_saveAction);
    addAction(m_copyAction);
    addAction(m_pasteAction);
    addAction(m_zoomInAction);
    addAction(m_zoomOutAction);
    addAction(m_adjustSizeAction);
    addAction(m_actualSizeAction);
}

void PictureEditorWidget::loadImage(const QString& fileName)
{
    QPixmap pixmap(fileName);
    if (pixmap.isNull())
        return;
    setPixmapInternal(pixmap, true);
}

void PictureEditorWidget::saveImage(const QString& fileName, const QString& format)
{
    if (m_image->pixmap() == 0)
        return;
    m_image->pixmap()->save(fileName, format.toLatin1());
}

void PictureEditorWidget::zoomImage(double factor)
{
    if (m_image->pixmap() == 0)
        return;

    m_zoomFactor = factor;
    m_image->resize(m_zoomFactor * m_image->pixmap()->size());
    m_zoomInAction->setEnabled(m_zoomFactor < 3.0);
    m_zoomOutAction->setEnabled(m_zoomFactor > 0.333);
} 

class PictureEditorWidgetAutoDisableHide
{
public:
    PictureEditorWidgetAutoDisableHide(PictureEditorWidget* w)
        : m_w(w)
    {
        if (m_w->parentWidget() != Q_NULL)
            static_cast<PictureEditorPopup *>(m_w->parentWidget())->setAutoHiding(true);

    }
    virtual ~PictureEditorWidgetAutoDisableHide()
    {
        if (m_w->parentWidget() != Q_NULL)
            static_cast<PictureEditorPopup *>(m_w->parentWidget())->setAutoHiding(false);
    }
private:
    PictureEditorWidget* m_w;
};

void PictureEditorWidget::load()
{
    PictureEditorWidgetAutoDisableHide autoDisableHide(this);

    QString fileName = QFileDialog::getOpenFileName(parentWidget(),
        Grid::tr_compatible(QtnGridOpenFileString), QDir::currentPath());
    if (!fileName.isEmpty())
    {
        loadImage(fileName);
    }
}

void PictureEditorWidget::save()
{
    PictureEditorWidgetAutoDisableHide autoDisableHide(this);

    QString initialPath = QDir::currentPath() + "/image"; 
    QString s;

    foreach (QByteArray format, QImageWriter::supportedImageFormats())
    {
        QString text = QString(format).toUpper();
        if (!s.isEmpty())
            s += ";;";
        s += text;
    }

    QString imageFormat;
    QString fileName = QFileDialog::getSaveFileName(this, Grid::tr_compatible(QtnGridSaveAsString),
                               initialPath,
                               s,
                               &imageFormat);
    if (fileName.isEmpty())
        return;

    QFileInfo fi(fileName);
    fileName = fi.absolutePath() + "/" + fi.baseName() + "." + imageFormat;
    saveImage(fileName, imageFormat);
}

void PictureEditorWidget::print()
{
    PictureEditorWidgetAutoDisableHide autoDisableHide(this);

    if (m_image->pixmap() == 0)
        return;
    
    QPrintDialog dialog(&m_printer, this);
    
    if (dialog.exec())
    {
        QPainter painter(&m_printer);
        QRect rect = painter.viewport();
        QSize size = m_image->pixmap()->size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(m_image->pixmap()->rect());
        painter.drawPixmap(0, 0, *m_image->pixmap());
    }
}

void PictureEditorWidget::copy()
{
    if (QApplication::clipboard() && m_image->pixmap())
        QApplication::clipboard()->setPixmap(*m_image->pixmap());
}

void PictureEditorWidget::paste()
{
    if (QApplication::clipboard() && 
        QApplication::clipboard()->mimeData() && 
        QApplication::clipboard()->mimeData()->hasImage())
    {
        setPixmapInternal(QApplication::clipboard()->pixmap(), true);
    }
}

void PictureEditorWidget::zoomIn()
{
    zoomImage(m_zoomFactor * 1.25); 
}

void PictureEditorWidget::zoomOut()
{
    zoomImage(m_zoomFactor * 0.8);
}

void PictureEditorWidget::adjustSize()
{
    if (m_image->pixmap() == 0)
        return;
    double scaleW = (double)contentsRect().width() / (double)m_image->pixmap()->size().width();
    double scaleH = (double)contentsRect().height() / (double)m_image->pixmap()->size().height();
    if (scaleH > scaleW)
        zoomImage(scaleW);
    else
        zoomImage(scaleH);
}

void PictureEditorWidget::actualSize()
{
    zoomImage(1.0);
}

/*!
\class Qtitan::PictureComboBox
\internal
*/
PictureComboBox::PictureComboBox(QWidget* parent)
: CustomComboBox(parent), m_pictureEditor(Q_NULL)
{
    m_pictureEditor = new PictureEditorWidget();
    connect(m_pictureEditor, SIGNAL(pictureChanged()), this, SLOT(pictureChanged()));
    PictureEditorPopup* ptr = new PictureEditorPopup(this);
    ptr->setCentralWidget(m_pictureEditor);
    ptr->setMinimumWidth(150);
    ptr->setMinimumHeight(100);
    ptr->setGeometry(QRect(0, 0, 290, 170));
    m_popup = ptr;
}

void PictureComboBox::setPixmap(const QPixmap& pm)
{
    m_pictureEditor->setPixmap(pm);
    setModified(false);
}

const QPixmap& PictureComboBox::pixmap() const
{
    return *m_pictureEditor->pixmap();
}

void PictureComboBox::showPopup()
{
    CustomComboBox::showPopup();
}

void PictureComboBox::hidePopup()
{
    if (isModified())
    {
        emit dataChanged();
    }
}

QPixmap PictureComboBox::getIcon()
{
    QPixmap temp(":res/cbimageicon.png");
    temp.setMask(temp.createMaskFromColor(Qt::magenta));
    return temp;
}

QWidget* PictureComboBox::editorWidget()
{
    return m_pictureEditor;
}

void PictureComboBox::undoModified()
{
    m_pictureEditor->setPixmap(m_statepm);
    m_pictureEditor->actualSize();
}

void PictureComboBox::saveDefaultState()
{
    m_statepm = *m_pictureEditor->pixmap();
}

void PictureComboBox::pictureChanged()
{
    setModified(true);
}

/*!
\class Qtitan::TableComboBox
\internal
*/
TableComboBox::TableComboBox(QWidget* parent)
: CustomComboBox(parent), m_grid(Q_NULL), m_oldRowIndex(-1)
{
    m_grid = new Grid();
    TableComboBoxPopup* ptr = new TableComboBoxPopup(this);
    ptr->setCentralWidget(m_grid);
    ptr->setMinimumWidth(150);
    ptr->setMinimumHeight(100);
    ptr->setGeometry(QRect(0, 0, 400, 320));
    m_popup = ptr;
}

void TableComboBox::showPopup()
{
    CustomComboBox::showPopup();
}

void TableComboBox::hidePopup()
{
    if (isModified())
    {
        emit dataChanged();
    }
}

void TableComboBox::focusRowChanged(int oldRowIndex, int rowIndex)
{
    Q_UNUSED(oldRowIndex);
    Q_UNUSED(rowIndex);
    setModified(true);
}

Grid* TableComboBox::grid() const
{
    return m_grid;
}

void TableComboBox::setModel(QAbstractItemModel* model)
{
    grid()->setViewType(Qtitan::Grid::TableView);
    Qtitan::GridTableView* view = grid()->view<Qtitan::GridTableView>();
    view->options().setBorderWidth(0);
    view->options().setColumnAutoWidth(true);
    connect(view, SIGNAL(focusRowChanged(int, int)), this, SLOT(focusRowChanged(int, int)));
    view->options().setAutoCreateColumns(false);
    view->setModel(model, QModelIndex());
    setModified(false);
}

QPixmap TableComboBox::getIcon()
{
    QPixmap temp(":res/cbtable.png");
    temp.setMask(temp.createMaskFromColor(Qt::magenta));
    return temp;
}

QWidget* TableComboBox::editorWidget()
{
    return m_grid;
}

void TableComboBox::undoModified()
{
    Qtitan::GridTableView* view = grid()->view<Qtitan::GridTableView>();
    if (m_oldRowIndex != -1)
        view->setFocusedRowIndex(m_oldRowIndex);
}

void TableComboBox::saveDefaultState()
{
    Qtitan::GridTableView* view = grid()->view<Qtitan::GridTableView>();
    m_oldRowIndex = view->focusedRowIndex();
}

/*!
\class Qtitan::TableComboBoxPopup
\internal
*/
TableComboBoxPopup::TableComboBoxPopup(TableComboBox* parent)
: CustomComboBoxPopup(parent)
{
}

/*!
\class GridPictureComboBoxEditorRepository
\inmodule QtitanDataGrid
*/
GridPictureComboBoxEditorRepository::GridPictureComboBoxEditorRepository()
: GridEditorRepository(),
  m_transformMode(Qt::FastTransformation),
  m_aspectRatioMode(Qt::KeepAspectRatio)
{
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QPixmap pm(":res/cbimageicon.png");
    pm.setMask(pm.createMaskFromColor(Qt::magenta));
    setIcon(pm);
    QVariant defValue = qtn_pixmap_to_qvariant(QVariant::ByteArray, QPixmap());
    m_defValueDisplay = defValue;
    m_defValueEdit = defValue;
}

GridEditor* GridPictureComboBoxEditorRepository::createEditor()
{
    return new GridPictureComboBoxEditor();
}

Qt::TransformationMode GridPictureComboBoxEditorRepository::transformMode() const
{
    return m_transformMode;
}

void GridPictureComboBoxEditorRepository::setTransformMode(Qt::TransformationMode mode)
{
    m_transformMode = mode;
    changed();
}

Qt::AspectRatioMode GridPictureComboBoxEditorRepository::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void GridPictureComboBoxEditorRepository::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    m_aspectRatioMode = mode;
    changed();
}

void GridPictureComboBoxEditorRepository::hidePictureEditor()
{
    if (view() != Q_NULL)
        view()->closeEditor();
}

/* GridPictureComboBoxEditor */
GridPictureComboBoxEditor::GridPictureComboBoxEditor()
: GridEditorBase(), m_pictureComboBox(Q_NULL)
{
}

GridPictureComboBoxEditor::~GridPictureComboBoxEditor()
{
    Q_ASSERT(!isEditMode());
    destroyEditModeContext();
}

QWidget* GridPictureComboBoxEditor::getCellWidget()
{
    return m_pictureComboBox;
}

void GridPictureComboBoxEditor::paintEvent(QPainter& painter, const GridEditorStyleOption& option)
{
    Q_UNUSED(option);
    QIcon::Mode mode = option.state & QStyle::State_Enabled ? QIcon::Normal : QIcon::Disabled;
    m_editorRepository->icon().paint(&painter, site()->geometry(), Qt::AlignHCenter | Qt::AlignVCenter, mode);
}

void GridPictureComboBoxEditor::updateContext(const QVariant& value)
{
    EditorAutoUpdater autoupdater(this);
    if (isEditMode())
        setValueToWidget(value);
    GridEditorBase::updateContext(value);
}

void GridPictureComboBoxEditor::setValueToWidget(const QVariant& value)
{
    if (m_pictureComboBox == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    QPixmap pixmap;
    qvariant_to_pixmap(value, pixmap);

    m_pictureComboBox->setAttribute(Qt::WA_UpdatesDisabled, true);
    m_pictureComboBox->setPixmap(pixmap);
    m_pictureComboBox->setAttribute(Qt::WA_UpdatesDisabled, false);
}

void GridPictureComboBoxEditor::createEditModeContext()
{
    Q_ASSERT(isEditMode());
    m_pictureComboBox = new PictureComboBox(site()->parent());
    if (site()->parent() == Q_NULL)
        m_pictureComboBox->setAttribute(Qt::WA_DontShowOnScreen);
    m_pictureComboBox->setOkButtonVisible(true);
    m_pictureComboBox->setCancelButtonVisible(true);
    m_pictureComboBox->setCrossButtonVisible(false);
    connect(m_pictureComboBox, SIGNAL(dataChanged()), this, SLOT(pictureChanged()));
    repositoryUpdated();
}

void GridPictureComboBoxEditor::destroyEditModeContext()
{
    Q_DELETE_AND_NULL(m_pictureComboBox);
}

void GridPictureComboBoxEditor::repositoryUpdated()
{
    if (m_pictureComboBox == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    if (m_editorRepository->transparentBackground() != m_pictureComboBox->testAttribute(Qt::WA_NoSystemBackground))
        m_pictureComboBox->setAttribute(Qt::WA_NoSystemBackground, m_editorRepository->transparentBackground());

    if (m_pictureComboBox->isEnabled() != m_editorRepository->isEnabled())
        m_pictureComboBox->setEnabled(m_editorRepository->isEnabled());
}

void GridPictureComboBoxEditor::pictureChanged()
{
    QVariant value = qtn_pixmap_to_qvariant(editorRepository()->defaultValue(Qt::EditRole).type(), m_pictureComboBox->pixmap());
    editingValueUpdated(value, true);
}

/*!
\class GridMemoEditorRepository
\inmodule QtitanDataGrid
*/
GridMemoEditorRepository::GridMemoEditorRepository()
: GridEditorRepository(), m_autoFormatting(QTextEdit::AutoNone),
  m_tabChangesFocus(false),
    m_isUndoRedoEnabled(true), m_lineWrapMode(QTextEdit::WidgetWidth),
    m_lineWrapColumnOrWidth(0), m_wordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere),
    m_overwriteMode(false), m_tabStopWidth(-1), m_cursorWidth(1), m_acceptRichText(true), m_isHTML(false)
{
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_defValueDisplay = QString("");
    m_defValueEdit = QString("");
}

GridEditor* GridMemoEditorRepository::createEditor()
{
    return new GridMemoEditor();
}

int GridMemoEditorRepository::heightHint(int width, const QVariant& v)
{
    QTextDocument doc;
    QTextOption opt = doc.defaultTextOption();
    setProperties(&doc);
    doc.setTextWidth(width);

    if (isHTML())
        doc.setHtml(v.toString());
    else
        doc.setPlainText(v.toString());

    return doc.size().height();


    QFontMetrics fm(view()->grid()->font());
    QRect r = fm.boundingRect(QRect(0, 0, width, 0), /*alignment() | */Qt::TextWordWrap, v.toString());
    return r.height() + fm.height() / 2;
}

void GridMemoEditorRepository::setCurrentCharFormat(const QTextCharFormat &format)
{
    m_currentCharFormat = format;
    changed();
}

QTextCharFormat GridMemoEditorRepository::currentCharFormat() const
{
    return m_currentCharFormat;
}

QTextEdit::AutoFormatting GridMemoEditorRepository::autoFormatting() const
{
    return m_autoFormatting;
}

void GridMemoEditorRepository::setAutoFormatting(QTextEdit::AutoFormatting features)
{
    m_autoFormatting = features;
    changed();
}

bool GridMemoEditorRepository::tabChangesFocus() const
{
    return m_tabChangesFocus;
}

void GridMemoEditorRepository::setTabChangesFocus(bool b)
{
    m_tabChangesFocus = b;
    changed();
}

void GridMemoEditorRepository::setDocumentTitle(const QString &title)
{
    m_documentTitle = title;
    changed();
}

const QString& GridMemoEditorRepository::documentTitle() const
{
    return m_documentTitle;
}

bool GridMemoEditorRepository::isUndoRedoEnabled() const
{
    return m_isUndoRedoEnabled;
}

void GridMemoEditorRepository::setUndoRedoEnabled(bool enable)
{
    m_isUndoRedoEnabled = enable;
    changed();
}

QTextEdit::LineWrapMode GridMemoEditorRepository::lineWrapMode() const
{
    return m_lineWrapMode;
}

void GridMemoEditorRepository::setLineWrapMode(QTextEdit::LineWrapMode mode)
{
    m_lineWrapMode = mode;
    changed();
}

int GridMemoEditorRepository::lineWrapColumnOrWidth() const
{
    return m_lineWrapColumnOrWidth;
}

void GridMemoEditorRepository::setLineWrapColumnOrWidth(int w)
{
    m_lineWrapColumnOrWidth = w;
    changed();
}

QTextOption::WrapMode GridMemoEditorRepository::wordWrapMode() const
{
    return m_wordWrapMode;
}

void GridMemoEditorRepository::setWordWrapMode(QTextOption::WrapMode policy)
{
    m_wordWrapMode = policy;
    changed();
}

bool GridMemoEditorRepository::overwriteMode() const
{
    return m_overwriteMode;
}

void GridMemoEditorRepository::setOverwriteMode(bool overwrite)
{
    m_overwriteMode = overwrite;
    changed();
}

int GridMemoEditorRepository::tabStopWidth() const
{
    return m_tabStopWidth;
}

void GridMemoEditorRepository::setTabStopWidth(int width)
{
    m_tabStopWidth = width;
    changed();
}

int GridMemoEditorRepository::cursorWidth() const
{
    return m_cursorWidth;
}

void GridMemoEditorRepository::setCursorWidth(int width)
{
    m_cursorWidth = width;
    changed();
}

bool GridMemoEditorRepository::acceptRichText() const
{
    return m_acceptRichText;
}

void GridMemoEditorRepository::setAcceptRichText(bool accept)
{
    m_acceptRichText = accept;
    changed();
}

bool GridMemoEditorRepository::isHTML() const
{
    return m_isHTML;
}

void GridMemoEditorRepository::setHTML(bool html)
{
    m_isHTML = html;
    changed();
}

void GridMemoEditorRepository::setProperties(QTextDocument* document) const
{
    QTextOption opt = document->defaultTextOption();
    bool changed = false;
    if (opt.alignment() != alignment())
    {
        opt.setAlignment(alignment());
        changed = true;
    }

    QTextOption::WrapMode wmode;
    if (lineWrapMode() == QTextEdit::NoWrap)
        wmode = QTextOption::NoWrap;
    else
        wmode = wordWrapMode();

    if (opt.wrapMode() != wmode)
    {
        opt.setWrapMode(wmode);
        changed = true;
    }

    if (changed)
        document->setDefaultTextOption(opt);

    if (document->metaInformation(QTextDocument::DocumentTitle) != documentTitle())
        document->setMetaInformation(QTextDocument::DocumentTitle, documentTitle());
}

void GridMemoEditorRepository::setProperties(QTextEdit* edit) const
{
    if (edit->currentCharFormat() != currentCharFormat())
        edit->setCurrentCharFormat(currentCharFormat());

    if (edit->autoFormatting() != autoFormatting())
        edit->setAutoFormatting(autoFormatting());

    if (edit->tabChangesFocus() != tabChangesFocus())
        edit->setTabChangesFocus(tabChangesFocus());

    if (edit->isUndoRedoEnabled() != isUndoRedoEnabled())
        edit->setUndoRedoEnabled(isUndoRedoEnabled());

    //if (edit->lineWrapMode() != lineWrapMode())
    //    edit->setLineWrapMode(lineWrapMode());

    if (edit->lineWrapColumnOrWidth() != lineWrapColumnOrWidth())
        edit->setLineWrapColumnOrWidth(lineWrapColumnOrWidth());

    if (edit->wordWrapMode() != wordWrapMode())
        edit->setWordWrapMode(wordWrapMode());

    if (edit->overwriteMode() != overwriteMode())
        edit->setOverwriteMode(overwriteMode());

    if (edit->tabStopWidth() != tabStopWidth())
        edit->setTabStopWidth(tabStopWidth());

    if (edit->cursorWidth() != cursorWidth())
        edit->setCursorWidth(cursorWidth());

    if (edit->acceptRichText() != acceptRichText())
        edit->setAcceptRichText(acceptRichText());

    setProperties(edit->document());
}

/* GridMemoEditor */
GridMemoEditor::GridMemoEditor()
: GridEditorBase(), m_inplaceWidget(Q_NULL)
{
    m_documentObject = new QTextDocument(this);
}

GridMemoEditor::~GridMemoEditor()
{
    Q_ASSERT(!isEditMode());
    destroyEditModeContext();
    Q_DELETE_AND_NULL(m_documentObject);
}

QWidget* GridMemoEditor::getCellWidget()
{
    return m_inplaceWidget;
}

void GridMemoEditor::paintEvent(QPainter& painter, const GridEditorStyleOption& option)
{
    QPalette palette = option.palette;
    palette.setCurrentColorGroup(option.state & QStyle::State_Active ? QPalette::Active : QPalette::Inactive);
#if 0
     const QColor bgcolor = 
        option.state & QStyle::State_Selected ? 
        palette.color(QPalette::Highlight) : (option.state & QStyle::State_On ? 
        palette.color(QPalette::AlternateBase) : palette.color(QPalette::Base));
#endif
    const QColor fgcolor = palette.color(option.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text);
    palette.setColor(QPalette::Text, fgcolor);
    //    ctx.palette.setColor(QPalette::Background, bgcolor);

    painter.setFont(option.font);

    if (m_documentObject->defaultFont() != option.font)
        m_documentObject->setDefaultFont(option.font);
    if (m_documentObject->textWidth() != option.rect.width())
        m_documentObject->setTextWidth(option.rect.width());

    m_documentRect = option.rect;
    qtn_QTextDocument_draw(painter, m_documentObject, palette, m_documentRect);
}

void GridMemoEditor::setValueToWidget(const QVariant& value)
{
    if (m_inplaceWidget == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);
    
    m_inplaceWidget->setAttribute(Qt::WA_UpdatesDisabled, true);
    
    if (((GridMemoEditorRepository *)m_editorRepository)->isHTML())
        m_inplaceWidget->setHtml(value.toString());
    else
        m_inplaceWidget->setPlainText(value.toString());

    if (m_editorRepository->autoSelect())
        m_inplaceWidget->selectAll();

    m_inplaceWidget->setAttribute(Qt::WA_UpdatesDisabled, false);
}

QPoint GridMemoEditor::translateMouseToTextDocument(const QPoint& pos)
{
    QPoint ret = pos - m_documentRect.topLeft();
    return ret;
}

void GridMemoEditor::mouseMoveEvent(const QPoint& pos)
{
    QString anchor = m_documentObject->documentLayout()->anchorAt(translateMouseToTextDocument(pos));
    if (anchor != "")
        site()->updateCursor(Qt::PointingHandCursor);
    else
        site()->updateCursor(QCursor());
}

void GridMemoEditor::mouseClickEvent(const QPoint& pos)
{
    QString anchor = m_documentObject->documentLayout()->anchorAt(translateMouseToTextDocument(pos));
    if (!anchor.isEmpty())
    {
        site()->linkActivated(anchor);
        return;
    }
}

void GridMemoEditor::updateContext(const QVariant& value)
{
    EditorAutoUpdater autoupdater(this);

    if (isEditMode())
    {
        setValueToWidget(value);
    }
    else
    {
        if (((GridMemoEditorRepository *)m_editorRepository)->isHTML())
            m_documentObject->setHtml(value.toString());
        else
            m_documentObject->setPlainText(value.toString());
    }

    GridEditorBase::updateContext(value);
}

void GridMemoEditor::createEditModeContext()
{
    Q_ASSERT(isEditMode());
    Q_ASSERT(m_inplaceWidget == Q_NULL);
    m_inplaceWidget = new QTextEdit(site()->parent());
    if (site()->parent() == Q_NULL)
        m_inplaceWidget->setAttribute(Qt::WA_DontShowOnScreen);
    connect(m_inplaceWidget, SIGNAL(textChanged()), this, SLOT(textChanged()));
    repositoryUpdated();
}

void GridMemoEditor::destroyEditModeContext()
{
  Q_DELETE_AND_NULL(m_inplaceWidget);
}

void GridMemoEditor::repositoryUpdated()
{
    if (m_inplaceWidget == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    GridEditorStyleOption option = site()->editorOption();

    if (m_editorRepository->transparentBackground() != m_inplaceWidget->testAttribute(Qt::WA_NoSystemBackground))
        m_inplaceWidget->setAttribute(Qt::WA_NoSystemBackground, m_editorRepository->transparentBackground());

    ((GridMemoEditorRepository *)m_editorRepository)->setProperties(m_inplaceWidget);
    ((GridMemoEditorRepository *)m_editorRepository)->setProperties(m_documentObject);

    if (m_inplaceWidget->isEnabled() != m_editorRepository->isEnabled())
        m_inplaceWidget->setEnabled(m_editorRepository->isEnabled());

    if (m_inplaceWidget->alignment() != option.decorationAlignment)
        m_inplaceWidget->setAlignment(option.decorationAlignment);
}

void GridMemoEditor::textChanged()
{
    QVariant value = ((GridMemoEditorRepository *)m_editorRepository)->isHTML() ? m_inplaceWidget->toHtml() : m_inplaceWidget->toPlainText();
    editingValueUpdated(value, false);
}

/*!
\class Qtitan::TextComboBox
\internal
*/
TextComboBox::TextComboBox(QWidget* parent)
: CustomComboBox(parent), m_isHTML(false)
{
    m_textEditor = new QTextEdit();
    connect(m_textEditor, SIGNAL(textChanged()), this, SLOT(textChanged()));
    createPopup();
}

QString TextComboBox::text() const
{
   if (m_isHTML)
       return m_textEditor->toHtml();
    else
        return m_textEditor->toPlainText();
}

void TextComboBox::setText(const QString& text)
{
   if (m_isHTML)
        m_textEditor->setHtml(text);
    else
        m_textEditor->setPlainText(text);
    setModified(false);
}

bool TextComboBox::isHTML() const
{
    return m_isHTML;
}

void TextComboBox::setHTML(bool html)
{
    m_isHTML = html;
}

QTextEdit* TextComboBox::textEdit()
{
    return m_textEditor;
}

void TextComboBox::createPopup()
{
    if (m_popup == Q_NULL)
    {
        TextComboBoxPopup* ptr = new TextComboBoxPopup(this);
        ptr->setCentralWidget(m_textEditor);
        ptr->setMinimumWidth(150);
        ptr->setMinimumHeight(100);
        ptr->setGeometry(QRect(0, 0, 290, 170));
        m_popup = ptr;
    }
}

void TextComboBox::showPopup()
{
    m_textEditor->document()->setModified(false);

    CustomComboBox::showPopup();
}

void TextComboBox::hidePopup()
{
    if (isModified())
    {
        emit dataChanged();
    }
}

QPixmap TextComboBox::getIcon()
{
    QPixmap temp(":res/cbtexticon.png");
    temp.setMask(temp.createMaskFromColor(Qt::magenta));
    return temp;
}

QWidget* TextComboBox::editorWidget()
{
    return m_textEditor;
}

void TextComboBox::undoModified()
{
   if (m_isHTML)
        m_textEditor->setHtml(m_statetext);
    else
        m_textEditor->setPlainText(m_statetext);
}

void TextComboBox::saveDefaultState()
{
   if (m_isHTML)
        m_statetext = m_textEditor->toHtml();
   else
        m_statetext = m_textEditor->toPlainText();
}

void TextComboBox::textChanged()
{
    setModified(true);
}

/*!
\class Qtitan::TextComboBoxPopup
\internal
*/
TextComboBoxPopup::TextComboBoxPopup(TextComboBox* parent)
: CustomComboBoxPopup(parent)
{
}

/*!
\class GridBlobEditorRepository
\inmodule QtitanDataGrid
*/
GridBlobEditorRepository::GridBlobEditorRepository()
: GridMemoEditorRepository()
{
    QPixmap pm(":res/cbtexticon.png");
    pm.setMask(pm.createMaskFromColor(Qt::magenta));
    setIcon(pm);
    m_defValueDisplay = QByteArray();
    m_defValueEdit = QByteArray();
}

GridEditor* GridBlobEditorRepository::createEditor()
{
    return new GridBlobEditor();
}


/* GridBlobEditor */
GridBlobEditor::GridBlobEditor()
: GridEditorBase(), m_textComboBox(Q_NULL)
{
}

GridBlobEditor::~GridBlobEditor()
{
    Q_ASSERT(!isEditMode());
    destroyEditModeContext();
}

QWidget* GridBlobEditor::getCellWidget()
{
    return m_textComboBox;
}

void GridBlobEditor::paintEvent(QPainter& painter, const GridEditorStyleOption& option)
{
    Q_UNUSED(option);
    QIcon::Mode mode = option.state & QStyle::State_Enabled ? QIcon::Normal : QIcon::Disabled;
    m_editorRepository->icon().paint(&painter, site()->geometry(), Qt::AlignHCenter | Qt::AlignVCenter, mode);
}

void GridBlobEditor::setValueToWidget(const QVariant& value)
{
    if (m_textComboBox == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    m_textComboBox->setAttribute(Qt::WA_UpdatesDisabled, true);
    m_textComboBox->setText(value.toString());
/*
    if (m_editorRepository->autoSelect())
        m_textComboBox->selectAll();
*/
    m_textComboBox->setAttribute(Qt::WA_UpdatesDisabled, false);
}

void GridBlobEditor::updateContext(const QVariant& value)
{
    EditorAutoUpdater autoupdater(this);
    if (isEditMode())
    {
        setValueToWidget(value);
    }
    GridEditorBase::updateContext(value);
}

void GridBlobEditor::createEditModeContext()
{
    Q_ASSERT(isEditMode());
    Q_ASSERT(m_textComboBox == Q_NULL);
    m_textComboBox = new TextComboBox(site()->parent());
    if (site()->parent() == Q_NULL)
        m_textComboBox->setAttribute(Qt::WA_DontShowOnScreen);
    m_textComboBox->setOkButtonVisible(true);
    m_textComboBox->setCancelButtonVisible(true);
    m_textComboBox->setCrossButtonVisible(false);
    connect(m_textComboBox, SIGNAL(dataChanged()), this, SLOT(textChanged()));
    repositoryUpdated();
}

void GridBlobEditor::destroyEditModeContext()
{
    Q_DELETE_AND_NULL(m_textComboBox);
}

void GridBlobEditor::repositoryUpdated()
{
    if (m_textComboBox == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    GridEditorStyleOption option = site()->editorOption();

    if (m_editorRepository->transparentBackground() != m_textComboBox->testAttribute(Qt::WA_NoSystemBackground))
        m_textComboBox->setAttribute(Qt::WA_NoSystemBackground, m_editorRepository->transparentBackground());

    if (m_textComboBox->isEnabled() != m_editorRepository->isEnabled())
        m_textComboBox->setEnabled(m_editorRepository->isEnabled());

    if (m_textComboBox->textEdit()->alignment() != option.decorationAlignment)
        m_textComboBox->textEdit()->setAlignment(option.decorationAlignment);

    if (m_textComboBox->isHTML() != ((GridMemoEditorRepository *)m_editorRepository)->isHTML())
        m_textComboBox->setHTML(((GridMemoEditorRepository *)m_editorRepository)->isHTML());

    ((GridBlobEditorRepository *)m_editorRepository)->setProperties(m_textComboBox->textEdit());
}

void GridBlobEditor::textChanged()
{
    editingValueUpdated(m_textComboBox->text(), true);
}

QTITAN_BEGIN_NAMESPACE

class GridDelegateAdapterEditorPublic: public GridDelegateAdapterEditor
{
    friend class FakeDelegateModel;
};

class FakeDelegateModel: public QAbstractItemModel 
{    
public:
    FakeDelegateModel(GridDelegateAdapterEditor* editor)
        : QAbstractItemModel(editor), m_editor(editor)
    {
        Q_ASSERT(m_editor != Q_NULL);
    }

    virtual QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const
    {
        return m_editor->site()->modelIndex().model()->headerData(section, orientation, role);
    }

    virtual QModelIndex parent(const QModelIndex &/*child*/) const
    {
        return QModelIndex();
    }

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
        if (parent.isValid())
            return -1;
        QModelIndex original = m_editor->site()->modelIndex();
        return original.model()->rowCount(original.parent());
    }

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
    {
        if (parent.isValid())
            return -1;
        QModelIndex original = m_editor->site()->modelIndex();
        return original.model()->columnCount(original.parent());
    }

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const
    {
        if (parent.isValid())
            return QModelIndex();

        if (row < 0 || row >= rowCount(parent))
            return QModelIndex();

        if (column < 0 || column >= columnCount(parent)) 
            return QModelIndex();

        return createIndex(row, column);
    }

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if (!index.isValid())
            return QVariant();
        
        if (index.row() < 0 || index.row() >= rowCount(index.parent()))
            return QVariant();
        
        if (index.column() < 0 || index.column() >= columnCount(index.parent()))
            return QVariant();

        if (role == QueryIndex)
        {
            return QVariant::fromValue<QModelIndex>(m_editor->site()->modelIndex());
        }
        else if (role == Qt::DisplayRole)
        {
            if (!m_editor->getContextValue().isValid())
                return m_editor->editorRepository()->defaultValue(Qt::DisplayRole);
            else
                return m_editor->getContextValue();
        }
        else if (role == Qt::EditRole)
        {
            if (!m_editor->getEditingValue().isValid())
                return m_editor->editorRepository()->defaultValue(Qt::EditRole);
            else
                return m_editor->getEditingValue();
        }
        else if (role == Qt::TextAlignmentRole)
        {
            GridEditorStyleOption option = m_editor->site()->editorOption();
            return QVariant(option.decorationAlignment);
        }
        else if (role == Qt::FontRole)
        {
            GridEditorStyleOption option = m_editor->site()->editorOption();
            return QVariant(option.font);
        }
        else if (role == Qt::BackgroundRole)
        {
            GridEditorStyleOption option = m_editor->site()->editorOption();
            return QVariant(option.palette.color(QPalette::Window));
        }
        else if (role == Qt::ForegroundRole)
        {
            GridEditorStyleOption option = m_editor->site()->editorOption();
            return QVariant(option.palette.color(QPalette::WindowText));
        }
        else
            return QVariant();
    }

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)
    {
        if (!index.isValid())
            return false;
        
        if (index.row() < 0 || index.row() >= rowCount(index.parent()))
            return false;
        
        if (index.column() < 0 || index.column() >= columnCount(index.parent()))
            return false;

        if (role == Qt::DisplayRole)
        {
            Q_ASSERT(false);
        }
        else if (role == Qt::EditRole)
        {
            ((GridDelegateAdapterEditorPublic *)m_editor)->editingValueUpdated(value, false);
        }
        else 
            return false;
        return true;
    }

    virtual Qt::ItemFlags flags(const QModelIndex &index) const
    {
        if (!index.isValid())
            return 0;
        const QAbstractItemModel* model = m_editor->site()->modelIndex().model();
        return model->flags(model->index(index.row(), index.column(), m_editor->site()->modelIndex().parent()));
    }

public:
    inline int origRow() const
    { 
        return m_editor->site()->modelIndex().row();
    }

    inline int origColumn() const
    { 
        return m_editor->site()->modelIndex().column();
    }
private:
    GridDelegateAdapterEditor* m_editor;
};

QTITAN_END_NAMESPACE

/*!
\class GridDelegateAdapterEditorRepository
\inmodule QtitanDataGrid
*/
GridDelegateAdapterEditorRepository::GridDelegateAdapterEditorRepository()
: GridEditorRepository(), m_delegate(Q_NULL)
{
    setTransparentBackground(false);
}

GridEditor* GridDelegateAdapterEditorRepository::createEditor()
{
    return new GridDelegateAdapterEditor();
}

void GridDelegateAdapterEditorRepository::setDelegate(QAbstractItemDelegate* itemDelegate)
{
    if (m_delegate != Q_NULL)
    {
        disconnect(m_delegate, SIGNAL(closeEditor(QWidget* , QAbstractItemDelegate::EndEditHint)), 
            this, SLOT(delegateCloseEditor(QWidget* , QAbstractItemDelegate::EndEditHint)));
        disconnect(m_delegate, SIGNAL(commitData(QWidget*)), this, SLOT(delegateCommitData(QWidget*)));
        disconnect(m_delegate, SIGNAL(sizeHintChanged(const QModelIndex&)),
            this, SLOT(delegateSizeHintChanged(const QModelIndex&)));
    }
    
    m_delegate = itemDelegate;

    if (m_delegate == Q_NULL)
        return;

    connect(m_delegate, SIGNAL(closeEditor(QWidget* , QAbstractItemDelegate::EndEditHint)), 
        this, SLOT(delegateCloseEditor(QWidget* , QAbstractItemDelegate::EndEditHint)));
    connect(m_delegate, SIGNAL(commitData(QWidget*)), this, SLOT(delegateCommitData(QWidget*)));
    connect(m_delegate, SIGNAL(sizeHintChanged(const QModelIndex&)),
        this, SLOT(delegateSizeHintChanged(const QModelIndex&)));

    changed();
}

QAbstractItemDelegate* GridDelegateAdapterEditorRepository::getDelegate() const
{
    return m_delegate;
}

void GridDelegateAdapterEditorRepository::delegateCloseEditor(QWidget* editor, QAbstractItemDelegate::EndEditHint hint)
{
    Q_UNUSED(editor);
    Q_UNUSED(hint);
    if (view() != Q_NULL)
        view()->closeEditor();
}

void GridDelegateAdapterEditorRepository::delegateCommitData(QWidget* editor)
{
    QHash<QWidget *, GridDelegateAdapterEditor *>::const_iterator it = m_delegateEditorHash.constFind(editor);
    if (it == m_delegateEditorHash.constEnd())
    {
        Q_ASSERT(false);
        return;
    }
    FakeDelegateModel* model = static_cast<FakeDelegateModel *>(it.value()->delegateModel());
    getDelegate()->setModelData(editor, model, model->index(model->origRow(), model->origColumn()));
}

void GridDelegateAdapterEditorRepository::delegateSizeHintChanged(const QModelIndex& index)
{
    Q_UNUSED(index);
}

/* GridDelegateAdapterEditor */
GridDelegateAdapterEditor::GridDelegateAdapterEditor()
: GridEditorBase(), m_widget(Q_NULL), m_delegateModel(Q_NULL)
{
    m_delegateModel = new FakeDelegateModel(this);
}

GridDelegateAdapterEditor::~GridDelegateAdapterEditor()
{
    destroyEditModeContext();
    Q_DELETE_AND_NULL(m_delegateModel);
}

QWidget* GridDelegateAdapterEditor::getCellWidget()
{
    return m_widget;
}

void GridDelegateAdapterEditor::paintEvent(QPainter& painter, const GridEditorStyleOption& option)
{
    if (static_cast<GridDelegateAdapterEditorRepository *>(m_editorRepository)->getDelegate() == Q_NULL)
    {
        QPalette palette = option.palette;
        palette.setCurrentColorGroup(option.state & QStyle::State_Active ? QPalette::Active : QPalette::Inactive);
        const QColor fgcolor = palette.color(option.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text);
        painter.setPen(fgcolor);
        QString s = option.fontMetrics.elidedText(Grid::tr_compatible(QtGridDelegateErrorString), Qt::ElideRight, option.rect.width());
        draw_text(painter, s, option.rect, m_editorRepository->alignment());
        return;
    }
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
    QStyleOptionViewItemV4 itemOption;
#else
    QStyleOptionViewItem itemOption;
#endif
    itemOption.widget = m_editorRepository->view()->grid();
    itemOption.state = option.state;
    itemOption.direction = option.direction;
    itemOption.rect = option.rect;
    itemOption.font = option.font;
    itemOption.icon = option.icon;
    itemOption.fontMetrics = option.fontMetrics;
    itemOption.palette = option.palette;
    itemOption.locale.setNumberOptions(QLocale::OmitGroupSeparator);
    static_cast<GridDelegateAdapterEditorRepository *>(m_editorRepository)->getDelegate()->paint(&painter,
        itemOption, static_cast<FakeDelegateModel *>(m_delegateModel)->index(
        static_cast<FakeDelegateModel *>(m_delegateModel)->origRow(),
        static_cast<FakeDelegateModel *>(m_delegateModel)->origColumn()));
}

void GridDelegateAdapterEditor::setValueToWidget(const QVariant& value)
{
    Q_UNUSED(value);

    if (m_widget == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);
    
    static_cast<GridDelegateAdapterEditorRepository *>(m_editorRepository)->getDelegate()->setEditorData(
        m_widget, m_delegateModel->index(
        static_cast<FakeDelegateModel *>(m_delegateModel)->origRow(),
        static_cast<FakeDelegateModel *>(m_delegateModel)->origColumn()));
}

void GridDelegateAdapterEditor::setEditMode(bool mode)
{
    if (m_editMode == mode)
        return;
    m_editMode = mode;
    if (m_editMode)
        createEditModeContext();
    else
        destroyEditModeContext();
}

void GridDelegateAdapterEditor::updateContext(const QVariant& value)
{
    EditorAutoUpdater autoupdater(this);

    if (isEditMode())
        setValueToWidget(value);

    GridEditorBase::updateContext(value);

}

void GridDelegateAdapterEditor::createEditModeContext()
{
    Q_ASSERT(isEditMode() && m_widget == Q_NULL);

    if (static_cast<GridDelegateAdapterEditorRepository *>(m_editorRepository)->getDelegate() == Q_NULL)
        return;

    QStyleOptionViewItem itemOption;

    m_widget = static_cast<GridDelegateAdapterEditorRepository *>(m_editorRepository)->getDelegate()->createEditor(
        site()->parent(), itemOption, m_delegateModel->index(
        static_cast<FakeDelegateModel *>(m_delegateModel)->origRow(),
        static_cast<FakeDelegateModel *>(m_delegateModel)->origColumn()));

    static_cast<GridDelegateAdapterEditorRepository *>(m_editorRepository)->m_delegateEditorHash.insert(m_widget, this);
    if (site()->parent() == Q_NULL)
        m_widget->setAttribute(Qt::WA_DontShowOnScreen);
    repositoryUpdated();
}

void GridDelegateAdapterEditor::destroyEditModeContext()
{
    static_cast<GridDelegateAdapterEditorRepository *>(m_editorRepository)->m_delegateEditorHash.remove(m_widget);
    Q_DELETE_AND_NULL(m_widget);
}

void GridDelegateAdapterEditor::repositoryUpdated()
{
    if (m_widget == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    if (m_editorRepository->transparentBackground() != m_widget->testAttribute(Qt::WA_NoSystemBackground))
        m_widget->setAttribute(Qt::WA_NoSystemBackground, m_editorRepository->transparentBackground());

    if (m_widget->isEnabled() != m_editorRepository->isEnabled())
        m_widget->setEnabled(m_editorRepository->isEnabled());
    
    //GridEditorStyleOption option = site()->editorOption();
    //if (m_textComboBox->textEdit()->alignment() != option.decorationAlignment)
    //    m_textComboBox->textEdit()->setAlignment(option.decorationAlignment);
}

/*!
\class GridTableEditorRepository
\inmodule QtitanDataGrid
*/
GridTableEditorRepository::GridTableEditorRepository()
: GridEditorRepository()
{
    QPixmap pm(":res/cbtable.png");
    pm.setMask(pm.createMaskFromColor(Qt::magenta));
    setIcon(pm);
}

GridEditor* GridTableEditorRepository::createEditor()
{
    return new GridTableEditor();
}

/* GridTableEditor */
GridTableEditor::GridTableEditor()
: GridEditorBase(), m_tableComboBox(Q_NULL)
{
}

GridTableEditor::~GridTableEditor()
{
    Q_ASSERT(!isEditMode());
    destroyEditModeContext();
}

QWidget* GridTableEditor::getCellWidget()
{
    return m_tableComboBox;
}

void GridTableEditor::paintEvent(QPainter& painter, const GridEditorStyleOption& option)
{
    Q_UNUSED(option);
    QIcon::Mode mode = option.state & QStyle::State_Enabled ? QIcon::Normal : QIcon::Disabled;
    m_editorRepository->icon().paint(&painter, site()->geometry(), Qt::AlignHCenter | Qt::AlignVCenter, mode);
}

QVariant GridTableEditor::getContextValue()
{
    if (m_tableComboBox == Q_NULL)
        return QVariant();
    Qtitan::GridTableView* view = m_tableComboBox->grid()->view<Qtitan::GridTableView>();
    if (view == Q_NULL)
        return QVariant();
    if (site()->dataBinding()->relationMode() == GridModelDataBinding::Relation &&
        site()->dataBinding()->relationModel() != Q_NULL)
    {
        QModelIndex index = view->model()->index(
            view->focusedRowIndex(),
            site()->dataBinding()->foreignKeyColumn(),
            site()->dataBinding()->relationParentIndex());
        return view->model()->data(index, site()->dataBinding()->foreignKeyRole());
    }
    else if (site()->dataBinding()->relationMode() == GridModelDataBinding::MasterDetailsRelation)
    {
        return m_masterModeValue;
    }
    return QVariant();
}

void GridTableEditor::updateContext(const QVariant& value)
{
    EditorAutoUpdater autoupdater(this);
    if (isEditMode())
        setValueToWidget(value);
    GridEditorBase::updateContext(value);
}

void GridTableEditor::createEditModeContext()
{
    Q_ASSERT(m_tableComboBox == Q_NULL);
    m_tableComboBox = new TableComboBox(site()->parent());
    if (site()->parent() == Q_NULL)
        m_tableComboBox->setAttribute(Qt::WA_DontShowOnScreen);
    if (m_editorRepository->transparentBackground())
        m_tableComboBox->setAttribute(Qt::WA_NoSystemBackground, m_editorRepository->transparentBackground());

    connect(m_tableComboBox, SIGNAL(dataChanged()), this, SLOT(focusChanged()));

    if (site()->dataBinding()->relationMode() == GridModelDataBinding::Relation &&
                site()->dataBinding()->relationModel() != Q_NULL)
    {
        m_tableComboBox->setModel(site()->dataBinding()->relationModel());
        Qtitan::GridTableView* view = m_tableComboBox->grid()->view<Qtitan::GridTableView>();
        view->options().setFocusFollowsMouse(true);
        QAbstractItemModel* model = view->model();

        m_tableComboBox->setOkButtonVisible(true);
        m_tableComboBox->setCancelButtonVisible(true);
        m_tableComboBox->setCrossButtonVisible(false);

        for (int i =0; i < model->columnCount(); ++i)
        {
            QString s = model->headerData(i, Qt::Horizontal).toString();
            if (site()->dataBinding()->foreignKeyColumnName() == s)
                continue;
            Qtitan::GridTableColumn* column = view->addColumn(i, s);
            int decorationColumn = site()->dataBinding()->getForeignColumnMapping(Qt::DecorationRole);
            if (decorationColumn == i)
                column->setEditorType(Qtitan::PictureEditorType);
        }
    } else if (site()->dataBinding()->relationMode() == GridModelDataBinding::MasterDetailsRelation &&
               site()->dataBinding()->relationModel() != Q_NULL)
    {
        m_tableComboBox->setModel(site()->dataBinding()->relationModel());
        Qtitan::GridTableView* view = m_tableComboBox->grid()->view<Qtitan::GridTableView>();
        view->options().setNewRowPlace(NewRowTop);
        QAbstractItemModel* model = view->model();

        m_tableComboBox->setOkButtonVisible(false);
        m_tableComboBox->setCancelButtonVisible(false);
        m_tableComboBox->setCrossButtonVisible(true);
        
        view->beginUpdate();
        for (int i =0; i < model->columnCount(); ++i)
        {
            QString s = model->headerData(i, Qt::Horizontal).toString();
            if (site()->dataBinding()->foreignKeyColumnName() == s)
                continue;
            Qtitan::GridTableColumn* column = view->addColumn(i, s);
            Q_UNUSED(column);
            //QString decorationColumnName = site()->dataBinding()->foreignColumn(Qt::DecorationRole);
            //if (!decorationColumnName.isEmpty() && decorationColumnName == s)
            //    column->setEditorType(Qtitan::PictureEditorType);
        }
        view->endUpdate();
    }
    repositoryUpdated();
}

void GridTableEditor::destroyEditModeContext()
{
    Q_DELETE_AND_NULL(m_tableComboBox);
}

void GridTableEditor::repositoryUpdated()
{
}

void GridTableEditor::setValueToWidget(const QVariant& value)
{
    if (m_tableComboBox == Q_NULL)
        return;

    EditorAutoUpdater autoupdater(this);

    m_tableComboBox->setAttribute(Qt::WA_UpdatesDisabled, true);

    if (site()->dataBinding()->relationMode() == GridModelDataBinding::Relation &&
                site()->dataBinding()->relationModel() != Q_NULL)
    {
        Qtitan::GridTableView* view = m_tableComboBox->grid()->view<Qtitan::GridTableView>();
        QModelIndex fIndex = site()->dataBinding()->findRelationModelIndex(value);
        if (fIndex.isValid())
        {
            GridRow* row = view->getRow(fIndex);
            if (row != Q_NULL)
            {
                view->setFocusedRowIndex(row->rowIndex());
            }
        }
        else if (view->getRowCount() > 0)
        {
            view->setFocusedRowIndex(0);
        }
        m_tableComboBox->setModified(false);
    }
    else if (site()->dataBinding()->relationMode() == GridModelDataBinding::MasterDetailsRelation)
    {
        Qtitan::GridTableView* view = m_tableComboBox->grid()->view<Qtitan::GridTableView>();
        view->modelController()->setDefaultValue(site()->dataBinding()->foreignKeyColumn(), value, Qt::EditRole);

        view->modelController()->relationFilter()->clear();
        view->modelController()->relationFilter()->setCondition(new GridFilterGroupCondition(view->modelController()->relationFilter()), false);
        view->modelController()->relationFilter()->condition()->addValue(
                site()->dataBinding()->foreignKeyColumn(),
                site()->dataBinding()->foreignKeyRole(),
                value);
        view->modelController()->relationFilter()->setActive(true);
        view->modelController()->refresh();
        m_masterModeValue = value;
    }
    m_tableComboBox->setAttribute(Qt::WA_UpdatesDisabled, false);
}

void GridTableEditor::focusChanged()
{
    QVariant value;
    editingValueUpdated(value, true);
}


