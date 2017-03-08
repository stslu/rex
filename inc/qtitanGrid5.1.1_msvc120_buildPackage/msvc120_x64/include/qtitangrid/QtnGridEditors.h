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

#ifndef QTN_GRID_EDITORS_H
#define QTN_GRID_EDITORS_H

#include <QHash>
#include <QTextEdit>
#include <QTextDocument>
#include <QComboBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QDateTimeEdit>
#include <QLabel>
#include <QSizeGrip>
#include <QScrollArea>
#include <QPushButton>
#include <QToolButton>
#include <QProgressBar>

#include "QtnGrid.h"

QTITAN_BEGIN_NAMESPACE

class GridEditor;
class GridEditorRepository;
class PictureEditorWindow;
class PictureComboBox;
class PictureEditorWidget;
class TextComboBox;
class TextEditorWidget;

enum GridEditorType
{
    AutoEditorType,
    StringEditorType,
    MemoEditorType,
    ComboBoxBlobEditorType,
    NumericEditorType,
    CheckBoxEditorType,
    ComboBoxEditorType,
    TableEditorType,
    DateTimeEditorType,
    TimeEditorType,
    DateEditorType,
    PictureEditorType,
    ComboBoxPictureEditorType,
    DelegateAdapterEditorType,
    ProgressEditorType,
    CustomEditType
};

enum EditorItemDataRole
{
    QueryIndex = Qt::UserRole + 100
};

class QTITAN_EXPORT GridEditorStyleOption: public QStyleOptionViewItem
{
public:
    GridEditorStyleOption(int version = QStyleOptionViewItem::Version);
public:
    QSize iconSize;
    QString selectedText;
    Qt::CaseSensitivity selectCaseSensitivity;
};

class GridEditorSite
{
public:
    virtual void invalidate(bool fullyViewUpdate = false) = 0;
    virtual GridModelDataBinding* dataBinding() const = 0;
    virtual QModelIndex modelIndex() const = 0;
    virtual QWidget* parent() const = 0;
    virtual QRect geometry() const = 0;
    virtual GridEditorStyleOption editorOption() = 0;
    virtual void showHint(const QString& text) const = 0;
    virtual void updateCursor(const QCursor& cursor) = 0;
    virtual void linkActivated(const QString& anchor) = 0;
};


class QTITAN_EXPORT EditorEventArgs: public BaseEventArgs
{
public:
    EditorEventArgs(const GridColumnBase* column, GridRow* row, GridEditorActivationPolicyFlag policyFlag);
    const GridColumnBase* column() const;
    GridRow* row() const;
    GridModelDataBinding* dataBinding() const;
    GridEditorActivationPolicyFlag activationPolicyFlag() const;
private:
    const GridColumnBase* m_column;
    GridRow* m_row;
    GridEditorActivationPolicyFlag m_policyFlag;
};


class QTITAN_EXPORT EditorLinkEventArgs : public EditorEventArgs
{
public:
    EditorLinkEventArgs(const GridColumnBase* column, GridRow* row, const QString& anchor);
    const QString& anchor() const;
private:
    QString m_anchor;
};

class QTITAN_EXPORT EditorValidationEventArgs: public BaseEventArgs
{
public:
    friend class GridEditorBase;
    EditorValidationEventArgs(GridEditor* editor, const QVariant& editingValue, const QVariant& editValue);
    const QVariant& editingValue() const;
    void setEditingValue(const QVariant& value);
    const QVariant& editValue() const;
    bool isValueValid() const;
    void setValueValid(bool valid);
    void setErrorHint(const QString& hint);
    const QString& errorHint() const;
    GridModelDataBinding* dataBinding() const;
    QModelIndex modelIndex() const;
private:
    bool m_valueValid;
    QString m_hint;
    QVariant m_editingValue;
    QVariant m_editValue;
    bool m_editingValueModified;
    GridEditor* m_editor;
};

class QTITAN_EXPORT GridEditor: public QObject
{
public:
    virtual void initEditor(GridEditorRepository* repository, GridEditorSite* site) = 0;
    virtual GridEditorRepository* editorRepository() const = 0;
    virtual GridEditorSite* site() const = 0;
    virtual bool isEditMode() = 0;
    virtual void setEditMode(bool mode) = 0;
    virtual QVariant getContextValue() = 0;
    virtual void updateContext(const QVariant& value) = 0;
    virtual bool isContextModified() = 0;
    virtual bool isValidated() = 0;
    virtual QWidget* getCellWidget() = 0;
    virtual void repositoryUpdated() = 0;
    virtual bool validate() = 0;
    //editor's events
    virtual void paintEvent(QPainter& painter, const GridEditorStyleOption& option) = 0;
    virtual void mouseMoveEvent(const QPoint& pos) = 0;
    virtual void mousePressEvent(const QPoint& pos) = 0;
    virtual void mouseReleaseEvent(const QPoint& pos) = 0;
    virtual void mouseClickEvent(const QPoint& pos) = 0;
    virtual void mouseDblClickEvent(const QPoint& pos) = 0;
    virtual void mouseOverEvent(const QPoint& pos) = 0;
    virtual void mouseOutEvent() = 0;
};

class QTITAN_EXPORT GridEditorBase: public GridEditor
{
public:
    GridEditorBase();
    virtual ~GridEditorBase();
    virtual void initEditor(GridEditorRepository* repository, GridEditorSite* site);
    GridEditorRepository* editorRepository() const;
    GridEditorSite* site() const;
    virtual void paintEvent(QPainter& painter, const GridEditorStyleOption& option);
    virtual void mouseMoveEvent(const QPoint& pos);
    virtual void mousePressEvent(const QPoint& pos);
    virtual void mouseReleaseEvent(const QPoint& pos);
    virtual void mouseClickEvent(const QPoint& pos);
    virtual void mouseDblClickEvent(const QPoint& pos);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
    virtual void updateContext(const QVariant& value);
    virtual void repositoryUpdated();
    virtual void createEditModeContext() = 0;
    virtual void destroyEditModeContext() = 0;
    virtual void setValueToWidget(const QVariant& value) = 0;
public:
    virtual bool isEditMode();
    virtual void setEditMode(bool mode);
    virtual bool isContextModified();
    virtual QVariant getContextValue();
    virtual bool isValidated();
    virtual QWidget* getCellWidget();
    virtual QString contextValueToString();
    virtual bool validate();
    void beginUpdateContext();
    void endUpdateContext();
    void setEditingValue(const QVariant& value);
    const QVariant& getEditingValue() const;
    bool isContextUpdating() const;
protected:
    void setContextModified(bool modified);
    void paintSimpleText(QPainter& painter, const GridEditorStyleOption& option, const QString& text);
    void editingValueUpdated(const QVariant& value, bool updateSite);
protected:
    GridEditorRepository* m_editorRepository;
    bool m_editMode;
    bool m_needToolTip;
    QVariant m_contextValue;
    QVariant m_contextEditingValue;
private:
    GridEditorSite* m_site;
    bool m_contextValidated;
    bool m_contextModified;
    int m_updateContextCount;
};

class QTITAN_EXPORT GridEditorRepository: public QObject
{
    Q_OBJECT
public:
    friend class GridEditorBase;
    GridEditorRepository();
    virtual ~GridEditorRepository();
    virtual GridEditor* createEditor() = 0;
    void setAlignment(Qt::Alignment flag);
    Qt::Alignment alignment() const;
    const QIcon& icon() const;
    void setIcon(const QIcon& icon);
    IconApperance iconApperance() const;
    void setIconApperance(IconApperance apperance);
    bool isEnabled() const;
    void setEnabled(bool enabled);
    bool isEditable() const;
    void setEditable(bool editable);
    bool transparentBackground() const;
    void setTransparentBackground(bool transparent);
    const GridEditorActivationPolicy& editorActivationPolicy() const;
    void setEditorActivationPolicy(const GridEditorActivationPolicy& activationPolicy);
    bool autoSelect() const;
    void setAutoSelect(bool autoSelect);
    bool isHintVisible() const;
    void setHintVisible(bool visible);
    QVariant defaultValue(Qt::ItemDataRole role) const;
    void setDefaultValue(const QVariant& value, Qt::ItemDataRole role);
    void removeDefaultValue(Qt::ItemDataRole role);
    bool validateOnEnter() const;
    void setValidateOnEnter(bool flag);
    bool isValidationEnabled() const;
    void setValidationEnabled(bool b);
    QValidator* validator() const;
    void setValidator(QValidator* validator);
    virtual int heightHint(int width, const QVariant& v);
    bool immediatePost() const;
    void setImmediatePost(bool b);
    bool isAutoGeneratedValue() const;
    void setAutoGeneratedValue(bool autoGenerated);
public:
    virtual void initialize(GridViewBase* view, GridModelDataBinding* binding);
    GridViewBase* view() const;
    GridModelDataBinding* dataBinding() const;
    void changed();
protected:
    QVariant m_defValueDisplay;
    QVariant m_defValueEdit;
private:
    GridViewBase* m_view;
    GridModelDataBinding* m_dataBinding;
    QList<GridEditor *> m_editors;
    Qt::Alignment m_alignment;
    QIcon m_icon;
    IconApperance m_iconApperance;
    bool m_enabled;
    bool m_editable;
    bool m_transparentBackground;
    GridEditorActivationPolicy m_editorActivationPolicy;
    bool m_autoSelect;
    bool m_hintVisible;
    bool m_validateOnEnter;
    bool m_validationEnabled;
    QValidator* m_validator;
    bool m_immediatePost;
    bool m_autoGeneratedValue;
};

class QTITAN_EXPORT GridStringEditorRepository: public GridEditorRepository
{
public:
    GridStringEditorRepository();
    virtual GridEditor* createEditor();
public:
    const QString& inputMask() const;
    void setInputMask(const QString& mask);
    QLineEdit::EchoMode echoMode() const;
    void setEchoMode(QLineEdit::EchoMode mode);
    int maxLength() const;
    void setMaxLength(int length);
    bool isHLink() const;
    void setHLink(bool link);
private:
    QString m_inputMask;
    QLineEdit::EchoMode m_echoMode;
    int m_maxLength;
    bool m_hlink;
};

class QTITAN_EXPORT GridStringEditor: public GridEditorBase
{
    Q_OBJECT
public:
    GridStringEditor();
    virtual ~GridStringEditor();
    virtual QWidget* getCellWidget();
    virtual void repositoryUpdated();
    virtual void paintEvent(QPainter& painter, const GridEditorStyleOption& option);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
    virtual void updateContext(const QVariant& value);
    virtual void createEditModeContext();
    virtual void destroyEditModeContext();
    virtual void setValueToWidget(const QVariant& value);
private:
    QLineEdit* m_textEdit;
private Q_SLOTS:
    void textEdited(const QString &text);
};

class QTITAN_EXPORT GridNumericEditorRepository: public GridEditorRepository
{
public:
    GridNumericEditorRepository();
    virtual GridEditor* createEditor();
public:
    double maximum() const;
    void setMaximum(double max);
    double minimum() const;
    void setMinimum(double min);
    void getRange(double& minimum, double& maximum) const;
    void setRange(double minimum, double maximum);
    const QString& prefix() const;
    void setPrefix(const QString& prefix);
    const QString& suffix() const;
    void setSuffix(const QString& suffix);
    int decimals() const;
    void setDecimals(int prec);
    void setSingleStep(double val);
    double singleStep() const;
    bool wrapping() const;
    void setWrapping(bool wrapping);
private:
    double m_maximum;
    double m_minimum;
    QString m_prefix;
    QString m_suffix;
    int m_decimals;
    double m_singleStep;
    bool m_wrapping;
};

class QTITAN_EXPORT GridNumericEditor: public GridEditorBase
{
    Q_OBJECT
public:
    GridNumericEditor();
    virtual ~GridNumericEditor();
    virtual QWidget* getCellWidget();
    virtual void repositoryUpdated();

    virtual void updateContext(const QVariant& value);
    virtual void createEditModeContext();
    virtual void destroyEditModeContext();
    virtual QString contextValueToString();
    virtual void setValueToWidget(const QVariant& value);
private:
    QDoubleSpinBox* m_numericEdit;
private Q_SLOTS:
    void valueChanged(double d);
};

class QTITAN_EXPORT GridProgressEditorRepository: public GridEditorRepository
{
public:
    GridProgressEditorRepository();
    virtual GridEditor* createEditor();
public:
    double maximum() const;
    void setMaximum(double max);
    double minimum() const;
    void setMinimum(double min);
    void getRange(double& minimum, double& maximum) const;
    void setRange(double minimum, double maximum);
    void setSingleStep(double val);
    double singleStep() const;
    bool wrapping() const;
    void setWrapping(bool wrapping);
    void setTextVisible(bool textVisible);
    bool isTextVisible() const;
    bool isDialEdit() const;
    void setDialEdit(bool dial);
private:
    double m_maximum;
    double m_minimum;
    double m_singleStep;
    bool m_wrapping;
    bool m_textVisible;
    bool m_dialEdit;
};

class QTITAN_EXPORT ProgressEdit: public QProgressBar
{
public:
    enum EditFieldKind
    {
        emNone,
        emSpinBox,
        emDial
    };
    explicit ProgressEdit(QWidget* parent = 0);
    EditFieldKind editFieldKind() const;
    void setEditFieldKind(EditFieldKind kind);
    void stepBy(int steps);
    void setSingleStep(double val);
    double singleStep() const;
    bool wrapping() const;
    void setWrapping(bool wrapping);
    int bound(int val) const;
protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void timerEvent(QTimerEvent *event);
    virtual void changeEvent(QEvent *event);
protected:
    void spin_initStyleOption(QStyleOptionSpinBox* spinBoxOption);
    void dial_initStyleOption(QStyleOptionSlider* option);
    bool updateSpiBoxHoverControl(const QPoint& pos);
    QStyle::SubControl newSpinBoxHoverControl(const QPoint &pos);
    void spin_reset();
    void spin_updateState(bool up, bool fromKeyboard = false);
    QAbstractSpinBox::StepEnabled spin_stepEnabled() const;
    QVariant bound(const QVariant &val, const QVariant &old, int steps) const;
    int notchSize() const;
    int dial_valueFromPoint(const QPoint &p) const;
protected:
    enum Button {
        None = 0x000,
        Keyboard = 0x001,
        Mouse = 0x002,
        Wheel = 0x004,
        ButtonMask = 0x008,
        Up = 0x010,
        Down = 0x020,
        DirectionMask = 0x040
    };
    EditFieldKind m_editFieldKind;
    int m_buttonState;
    QAbstractSpinBox::ButtonSymbols m_buttonSymbols;
    bool m_accelerate;
    int m_acceleration;
    int m_effectiveSpinRepeatRate;
    int m_spinBoxClickTimerId;
    int m_spinBoxClickThresholdTimerId;
    int m_spinBoxClickTimerInterval;
    int m_spinBoxClickThresholdTimerInterval;
    QStyle::SubControl m_hoverControl;
    QRect m_hoverRect;
    double m_singleStep;
    double m_pageStep;
    bool m_wrapping;
    double m_notchTarget;
};

class QTITAN_EXPORT GridProgressEditor: public GridEditorBase
{
    Q_OBJECT
public:
    GridProgressEditor();
    virtual ~GridProgressEditor();
    virtual QWidget* getCellWidget();
    virtual void repositoryUpdated();

    virtual void updateContext(const QVariant& value);
    virtual void createEditModeContext();
    virtual void destroyEditModeContext();
    virtual QString contextValueToString();
    virtual void setValueToWidget(const QVariant& value);
    virtual void setEditMode(bool mode);
private:
    ProgressEdit* m_progressEdit;
private Q_SLOTS:
    void valueChanged(int value);
};

class QTITAN_EXPORT GridCheckBoxEditorRepository: public GridEditorRepository
{
    friend class GridCheckBoxEditor;
public:
    GridCheckBoxEditorRepository();
    virtual GridEditor* createEditor();
    void setTristate(bool state = true);
    bool isTristate() const;
    bool nativeStyle() const;
    void setNativeStyle(bool style);
private:
    bool m_tristate;
    bool m_nativeStyle;
};

class QTITAN_EXPORT CheckEdit: public QWidget
{
public:
    CheckEdit(QWidget* parent = 0);
    QCheckBox* box() const;
    void setAlignment(Qt::Alignment flag);
    Qt::Alignment alignment() const;
    bool nativeStyle() const;
    void setNativeStyle(bool style);
protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void focusInEvent(QFocusEvent *);
    virtual bool event(QEvent *);
private:
    void updateAlign();
    QCheckBox* m_cb;
    Qt::Alignment m_alignment;
};

class QTITAN_EXPORT GridCheckBoxEditor: public GridEditorBase
{
    Q_OBJECT
public:
    GridCheckBoxEditor();
    virtual ~GridCheckBoxEditor();
    virtual QWidget* getCellWidget();
    virtual void repositoryUpdated();
    virtual QString contextValueToString();
    virtual void updateContext(const QVariant& value);
    virtual void createEditModeContext();
    virtual void destroyEditModeContext();
    virtual void setValueToWidget(const QVariant& value);
    virtual void setEditMode(bool mode);
private:
    CheckEdit* m_checkEdit;
private Q_SLOTS:
    void stateChanged(int state);
};

class QTITAN_EXPORT GridComboBoxEditorRepository: public GridEditorRepository
{
    friend class GridComboBoxEditor;
public:
    GridComboBoxEditorRepository();
    virtual GridEditor* createEditor();
    bool isComboBoxEditable() const;
    void setComboBoxEditable(bool editable);
private:
    bool m_comboBoxEditable;
};

class QTITAN_EXPORT GridComboBoxEditor: public GridEditorBase
{
    Q_OBJECT
public:
    GridComboBoxEditor();
    virtual ~GridComboBoxEditor();
    virtual QWidget* getCellWidget();
    virtual void paintEvent(QPainter& painter, const GridEditorStyleOption& option);
    virtual void repositoryUpdated();

    virtual void updateContext(const QVariant& value);
    virtual void createEditModeContext();
    virtual void destroyEditModeContext();
    virtual void setValueToWidget(const QVariant& value);
private:        
    QComboBox* m_comboBox;
private Q_SLOTS:
    void currentIndexChanged(int index);
    void resolveValue(int index, QVariant& value);
};

class CheckBoxWidget : public QCheckBox
{
public:
    friend class CheckEdit;
    CheckBoxWidget(QWidget* parent = Q_NULL);
    bool isNativeStyleEnabled() const;
    void setNativeStyleEnabled(bool enabled);
    bool isMouseOverPaintEnabled() const;
    void setMouseOverPaintEnabled(bool enabled);
protected:
    virtual void paintEvent(QPaintEvent* e);
private:
    bool m_nativeStyle;
    bool m_mouseOverPaint;
};

class QTITAN_EXPORT GridDateTimeEditorRepository: public GridEditorRepository
{
public:
    GridDateTimeEditorRepository();
    virtual GridEditor* createEditor();
    bool calendarPopup() const;
    void setCalendarPopup(bool flag);
    QDateTime maximumDateTime() const;
    void setMaximumDateTime(const QDateTime& dt);
    QDate maximumDate() const;
    void setMaximumDate(const QDate& max);
    QTime maximumTime() const;
    void setMaximumTime(const QTime& max);
    QDate minimumDate() const;
    void setMinimumDate(const QDate& min);
    QDateTime minimumDateTime() const;
    void setMinimumDateTime(const QDateTime& dt);
    QTime minimumTime() const;
    void setMinimumTime(const QTime& min);
    Qt::TimeSpec timeSpec() const;
    void setTimeSpec(Qt::TimeSpec spec);
    const QString& displayFormat() const;
    void setDisplayFormat(const QString& format);
    void getDateRange(QDate &min, QDate& max) const;
    void setDateRange(const QDate& min, const QDate& max);
    void getDateTimeRange(QDateTime& min, QDateTime& max) const;
    void setDateTimeRange(const QDateTime& min, const QDateTime& max);
    void getTimeRange(QTime& min, QTime& max) const;
    void setTimeRange(const QTime& min, const QTime& max);
private:
    bool m_calendarPopup;
    QDateTime m_maximumDateTime;
    QDate m_maximumDate;
    QTime m_maximumTime;
    QDate m_minimumDate;
    QDateTime m_minimumDateTime;
    QTime m_minimumTime;
    Qt::TimeSpec m_timeSpec;
    QString m_displayFormat;
};

class QTITAN_EXPORT GridDateTimeEditor: public GridEditorBase
{
    Q_OBJECT
public:
    GridDateTimeEditor();
    virtual ~GridDateTimeEditor();
    virtual QWidget* getCellWidget();
    virtual QString contextValueToString();
    virtual void repositoryUpdated();
    virtual void updateContext(const QVariant& value);
    virtual void createEditModeContext();
    virtual void destroyEditModeContext();
    virtual void setValueToWidget(const QVariant& value);
protected:
    virtual QDateTimeEdit* createDateTimeWidget();
private:
    QDateTimeEdit* m_dtEdit;
private Q_SLOTS:
    void dateTimeChanged(const QDateTime& datetime);
};

class QTITAN_EXPORT GridTimeEditorRepository: public GridDateTimeEditorRepository
{
public:
    GridTimeEditorRepository();
    virtual GridEditor* createEditor();
private:
};

class QTITAN_EXPORT GridTimeEditor: public GridDateTimeEditor
{
public:
    GridTimeEditor();
    virtual QString contextValueToString();
protected:
    virtual QDateTimeEdit* createDateTimeWidget();
private:
};

class QTITAN_EXPORT GridDateEditorRepository: public GridDateTimeEditorRepository
{
public:
    GridDateEditorRepository();
    virtual GridEditor* createEditor();
private:
};

class QTITAN_EXPORT GridDateEditor: public GridDateTimeEditor
{
public:
    GridDateEditor();
    virtual QString contextValueToString();
protected:
    virtual QDateTimeEdit* createDateTimeWidget();
private:
};

class QTITAN_EXPORT GridPictureEditorRepository: public GridEditorRepository
{
    Q_OBJECT
public:
    friend class GridPictureEditor;
    GridPictureEditorRepository();
    virtual GridEditor* createEditor();
    Qt::TransformationMode transformMode() const;
    void setTransformMode(Qt::TransformationMode mode);
    Qt::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(Qt::AspectRatioMode mode);
    bool isScaled() const;
    void setScaled(bool scaled);
    virtual int heightHint(int width, const QVariant& v);
    const QString& imageEmptyText() const;
    void setImageEmptyText(const QString& text);
    static QVariant convertToVariant(QVariant::Type type, const QPixmap& pixmap);
private:
    Qt::TransformationMode m_transformMode;
    Qt::AspectRatioMode m_aspectRatioMode;
    bool m_scaled;
    QString m_imageEmptyText;
private Q_SLOTS:
    void hidePictureEditor();
};

class QTITAN_EXPORT GridPictureEditor: public GridEditorBase
{
    Q_OBJECT
public:
    GridPictureEditor();
    virtual ~GridPictureEditor();
    virtual QWidget* getCellWidget();
    virtual void paintEvent(QPainter& painter, const GridEditorStyleOption& option);
    virtual void repositoryUpdated();

    virtual void updateContext(const QVariant& value);
    virtual void createEditModeContext();
    virtual void destroyEditModeContext();
    virtual void setValueToWidget(const QVariant& value);
private:
    PictureEditorWidget* m_inplaceWidget;
    QPixmap m_displayPixmap;
private Q_SLOTS:
    void pictureChanged();
};

class CustomComboBoxPopup;

class QTITAN_EXPORT CustomComboBox: public QWidget
{
    Q_OBJECT
public:
    friend class CustomComboBoxPopup;
    CustomComboBox(QWidget* parent);
    virtual ~CustomComboBox();
    bool isModified() const;
    void setModified(bool modified);
    virtual void showPopup();
    virtual void hidePopup() = 0;
    bool okButtonVisible();
    void setOkButtonVisible(bool visible);
    bool cancelButtonVisible() const;
    void setCancelButtonVisible(bool visible) const;
    bool crossButtonVisible() const;
    void setCrossButtonVisible(bool visible);
protected:
    virtual QWidget* editorWidget() = 0;
    virtual void undoModified() = 0;
    virtual void saveDefaultState();
    QStyle::SubControl newHoverControl(const QPoint &pos);
    bool updateHoverControl(const QPoint &pos);
    void updateArrow(QStyle::StateFlag state);
    bool event(QEvent *event);
    void initStyleOption(QStyleOptionComboBox *option) const;
    virtual void paintEvent(QPaintEvent* e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual QPixmap getIcon() = 0;
    void updatePopupButtons();
Q_SIGNALS:
    void dataChanged();
private:
    QStyle::StateFlag arrowState;
    QStyle::SubControl hoverControl;
    QRect hoverRect;
protected:
    CustomComboBoxPopup* m_popup;
    QIcon m_icon;
    bool m_modified;
};

class QTITAN_EXPORT GridToolButton : public QAbstractButton
{
public:
    GridToolButton(QWidget* parent = 0);
    virtual QSize sizeHint() const;
    bool autoRise() const;
    void setAutoRaise(bool autoRaise);
protected:
    void paintTexture(QPainter& painter, const QColor& accentColor, const QRect& rect);
    QPalette getButtonPalette() const;
    virtual void paintEvent(QPaintEvent *e);
private:
    bool m_autoRise;
};

class QTITAN_EXPORT EditorPopup: public QWidget
{
    Q_OBJECT
public:
    EditorPopup(QWidget* parent, bool okButton = true, bool cancelButton = true, bool crossButton = true);
    void setCentralWidget(QWidget* w);
    void setAutoHiding(bool flag);
    QAbstractButton* okButton() const;
    QAbstractButton* cancelButton() const;
    QAbstractButton* crossButton() const;
protected:
    virtual void paintEvent(QPaintEvent *);
    QWidget* centralWidget() const;
    virtual void setVisible(bool visible);
private:
    QAbstractButton* m_okButton;
    QAbstractButton* m_cancelButton;
    QAbstractButton* m_closeButton;
    bool m_autoHiding;
    QWidget* m_centralWidget;
    QSizeGrip* m_grip;
protected Q_SLOTS:
    virtual void okClick();
    virtual void cancelClick();
    virtual void closeClick();
};

class QTITAN_EXPORT CustomComboBoxPopup: public EditorPopup
{
public:
    CustomComboBoxPopup(CustomComboBox* parent, bool okButton = true, bool cancelButton = true, bool crossButton = true);
protected:
    virtual void hideEvent(QHideEvent *);
    virtual void cancelClick();
private:
    CustomComboBox* m_comboBox;
};


class QTITAN_EXPORT PictureEditorPopup: public CustomComboBoxPopup
{
public:
    PictureEditorPopup(PictureComboBox* parent);
protected:
};

class QTITAN_EXPORT PictureComboBox: public CustomComboBox
{
    Q_OBJECT
public:
    PictureComboBox(QWidget* parent = Q_NULL);
    void setPixmap(const QPixmap& pm);
    const QPixmap& pixmap() const;
    virtual void showPopup();
    virtual void hidePopup();
protected:
    virtual QPixmap getIcon();
    virtual QWidget* editorWidget();
    virtual void undoModified();
    virtual void saveDefaultState();
private:
    PictureEditorWidget* m_pictureEditor;
    QPixmap m_statepm;
protected Q_SLOTS:
    void pictureChanged();
};

class QTITAN_EXPORT PictureEditorWidget: public QScrollArea
{
    Q_OBJECT
public:
    PictureEditorWidget(QWidget* parent = Q_NULL);
    void setPixmap(const QPixmap& pixmap);
    const QPixmap* pixmap() const;
    bool isModified() const;
    void setModified(bool m = true);
protected:
    virtual void create();
    virtual void contextMenuEvent(QContextMenuEvent* event);
    void setPixmapInternal(const QPixmap& pixmap, bool modified);
private:
    void createActions();
    void zoomImage(double factor);
    void loadImage(const QString& fileName);
    void saveImage(const QString& fileName, const QString& format);
    QLabel* m_image;
    QAction* m_loadAction;
    QAction* m_saveAction;
    QAction* m_copyAction;
    QAction* m_pasteAction;
    QAction* m_zoomInAction;
    QAction* m_zoomOutAction;
    QAction* m_adjustSizeAction;
    QAction* m_actualSizeAction;
    QAction* m_printAction;
    QMenu* m_contextMenu;
    double m_zoomFactor;
    bool m_modified;
    QPrinter m_printer;
public Q_SLOTS:
    void load();
    void save();
    void copy();
    void paste();
    void zoomIn();
    void zoomOut();
    void adjustSize();
    void actualSize();
    void print();
Q_SIGNALS:
    void pictureChanged();
};

class QTITAN_EXPORT TableComboBox: public CustomComboBox
{
    Q_OBJECT
public:
    TableComboBox(QWidget* parent = Q_NULL);
    virtual void showPopup();
    virtual void hidePopup();
    Grid* grid() const;
    void setModel(QAbstractItemModel* model);
protected:
    virtual QPixmap getIcon();
    virtual QWidget* editorWidget();
    virtual void undoModified();
    virtual void saveDefaultState();
private:
    Grid* m_grid;
    int m_oldRowIndex;
private Q_SLOTS:
    void focusRowChanged(int oldRowIndex, int rowIndex);
};

class QTITAN_EXPORT TableComboBoxPopup: public CustomComboBoxPopup
{
public:
    TableComboBoxPopup(TableComboBox* parent);
protected:
};

class QTITAN_EXPORT GridPictureComboBoxEditorRepository: public GridEditorRepository
{
    Q_OBJECT
public:
    friend class GridPictureComboBoxEditor;
    GridPictureComboBoxEditorRepository();
    virtual GridEditor* createEditor();
    Qt::TransformationMode transformMode() const;
    void setTransformMode(Qt::TransformationMode mode);
    Qt::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(Qt::AspectRatioMode mode);
private:
    Qt::TransformationMode m_transformMode;
    Qt::AspectRatioMode m_aspectRatioMode;
private Q_SLOTS:
    void hidePictureEditor();
};

class QTITAN_EXPORT GridPictureComboBoxEditor: public GridEditorBase
{
    Q_OBJECT
public:
    GridPictureComboBoxEditor();
    virtual ~GridPictureComboBoxEditor();
    virtual QWidget* getCellWidget();
    virtual void paintEvent(QPainter& painter, const GridEditorStyleOption& option);
    virtual void repositoryUpdated();

    virtual void updateContext(const QVariant& value);
    virtual void createEditModeContext();
    virtual void destroyEditModeContext();
    virtual void setValueToWidget(const QVariant& value);
private:
    PictureComboBox* m_pictureComboBox;
private Q_SLOTS:
    void pictureChanged();
};


class QTITAN_EXPORT GridMemoEditorRepository: public GridEditorRepository
{
    Q_OBJECT
public:
    friend class GridMemoEditor;
    GridMemoEditorRepository();
    virtual GridEditor* createEditor();
    virtual int heightHint(int width, const QVariant& v);
    void setCurrentCharFormat(const QTextCharFormat &format);
    QTextCharFormat currentCharFormat() const;
    QTextEdit::AutoFormatting autoFormatting() const;
    void setAutoFormatting(QTextEdit::AutoFormatting features);
    bool tabChangesFocus() const;
    void setTabChangesFocus(bool b);
    void setDocumentTitle(const QString &title);
    const QString& documentTitle() const;
    bool isUndoRedoEnabled() const;
    void setUndoRedoEnabled(bool enable);
    QTextEdit::LineWrapMode lineWrapMode() const;
    void setLineWrapMode(QTextEdit::LineWrapMode mode);
    int lineWrapColumnOrWidth() const;
    void setLineWrapColumnOrWidth(int w);
    QTextOption::WrapMode wordWrapMode() const;
    void setWordWrapMode(QTextOption::WrapMode wrapMode);
    bool overwriteMode() const;
    void setOverwriteMode(bool overwrite);
    int tabStopWidth() const;
    void setTabStopWidth(int width);
    int cursorWidth() const;
    void setCursorWidth(int width);
    bool acceptRichText() const;
    void setAcceptRichText(bool accept);
    bool isHTML() const;
    void setHTML(bool html);
protected:
    void setProperties(QTextEdit* edit) const;
    void setProperties(QTextDocument* document) const;
private:
    QTextCharFormat m_currentCharFormat;
    QTextEdit::AutoFormatting m_autoFormatting;
    bool m_tabChangesFocus;
    QString m_documentTitle;
    bool m_isUndoRedoEnabled;
    QTextEdit::LineWrapMode m_lineWrapMode;
    int m_lineWrapColumnOrWidth;
    QTextOption::WrapMode m_wordWrapMode;
    bool m_overwriteMode;
    int m_tabStopWidth;
    int m_cursorWidth;
    bool m_acceptRichText;
    bool m_isHTML;
};

class QTITAN_EXPORT GridMemoEditor: public GridEditorBase
{
    Q_OBJECT
public:
    GridMemoEditor();
    virtual ~GridMemoEditor();
    virtual QWidget* getCellWidget();
    virtual void paintEvent(QPainter& painter, const GridEditorStyleOption& option);
    virtual void repositoryUpdated();

    virtual void updateContext(const QVariant& value);
    virtual void createEditModeContext();
    virtual void destroyEditModeContext();
    virtual void setValueToWidget(const QVariant& value);
    virtual void mouseMoveEvent(const QPoint& pos);
    virtual void mouseClickEvent(const QPoint& pos);
private:
    QPoint translateMouseToTextDocument(const QPoint& pos);
    QTextEdit* m_inplaceWidget;
    QTextDocument* m_documentObject;
    QRect m_documentRect;
private Q_SLOTS:
    void textChanged();
};

class QTITAN_EXPORT TextComboBox: public CustomComboBox
{
    Q_OBJECT
public:
    friend class TextComboBoxPopup;
    TextComboBox(QWidget* parent = Q_NULL);
    QString text() const;
    void setText(const QString& text);
    bool isHTML() const;
    void setHTML(bool html);
    QTextEdit* textEdit();
    virtual void showPopup();
    virtual void hidePopup();
protected:
    virtual QPixmap getIcon();
    virtual QWidget* editorWidget();
    virtual void undoModified();
    virtual void saveDefaultState();
private:
    void createPopup();
    QTextEdit* m_textEditor;
    bool m_isHTML;
    QString m_statetext;
private Q_SLOTS:
    void textChanged();
};

class QTITAN_EXPORT TextComboBoxPopup: public CustomComboBoxPopup
{
    Q_OBJECT
public:
    friend class TextComboBox;
    TextComboBoxPopup(TextComboBox* parent);
private:
};

class QTITAN_EXPORT GridBlobEditorRepository: public GridMemoEditorRepository
{
    Q_OBJECT
public:
    friend class GridBlobEditor;
    GridBlobEditorRepository();
    virtual GridEditor* createEditor();
private:
};

class QTITAN_EXPORT GridBlobEditor: public GridEditorBase
{
    Q_OBJECT
public:
    GridBlobEditor();
    virtual ~GridBlobEditor();
    virtual QWidget* getCellWidget();
    virtual void paintEvent(QPainter& painter, const GridEditorStyleOption& option);
    virtual void repositoryUpdated();

    virtual void updateContext(const QVariant& value);
    virtual void createEditModeContext();
    virtual void destroyEditModeContext();
    virtual void setValueToWidget(const QVariant& value);
private:
    TextComboBox* m_textComboBox;
private Q_SLOTS:
    void textChanged();
};

class GridDelegateAdapterEditor;
class QTITAN_EXPORT GridDelegateAdapterEditorRepository: public GridEditorRepository
{
    Q_OBJECT
public:
    friend class GridDelegateAdapterEditor;
    GridDelegateAdapterEditorRepository();
    virtual GridEditor* createEditor();
public:
    void setDelegate(QAbstractItemDelegate* itemDelegate);
    QAbstractItemDelegate* getDelegate() const;
private:
    QAbstractItemDelegate* m_delegate;
    QHash<QWidget *, GridDelegateAdapterEditor *> m_delegateEditorHash;
private Q_SLOTS:
    void delegateCloseEditor(QWidget* editor, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint);
    void delegateCommitData(QWidget* editor);
    void delegateSizeHintChanged(const QModelIndex& index);
};

class QTITAN_EXPORT GridDelegateAdapterEditor: public GridEditorBase
{
    Q_OBJECT
public:
    friend class GridDelegateAdapterEditorRepository;
    GridDelegateAdapterEditor();
    virtual ~GridDelegateAdapterEditor();
    virtual QWidget* getCellWidget();
    virtual void paintEvent(QPainter& painter, const GridEditorStyleOption& option);
    virtual void repositoryUpdated();

    virtual void updateContext(const QVariant& value);
    virtual void createEditModeContext();
    virtual void destroyEditModeContext();
    virtual void setValueToWidget(const QVariant& value);
    virtual void setEditMode(bool mode);
public:
    QAbstractItemModel* delegateModel() const { return m_delegateModel; };
private:
    QWidget* m_widget;
    QVariant m_oldEditValue;
    QAbstractItemModel* m_delegateModel;
};

class QTITAN_EXPORT GridTableEditorRepository: public GridEditorRepository
{
    friend class GridTableEditor;
public:
    GridTableEditorRepository();
    virtual GridEditor* createEditor();
private:
};

class QTITAN_EXPORT GridTableEditor: public GridEditorBase
{
    Q_OBJECT
public:
    GridTableEditor();
    virtual ~GridTableEditor();
    virtual QWidget* getCellWidget();
    virtual void paintEvent(QPainter& painter, const GridEditorStyleOption& option);
    virtual QVariant getContextValue();
    virtual void repositoryUpdated();

    virtual void updateContext(const QVariant& value);
    virtual void createEditModeContext();
    virtual void destroyEditModeContext();
    virtual void setValueToWidget(const QVariant& value);
private:
    TableComboBox* m_tableComboBox;
    QVariant m_masterModeValue;
private Q_SLOTS:
    void focusChanged();
};

class EditorAutoUpdater
{
public:
    EditorAutoUpdater(GridEditorBase* base)
        : m_base(base)
    {
        m_base->beginUpdateContext();
    }

    ~EditorAutoUpdater()
    {
        m_base->endUpdateContext();
    }
private:
    GridEditorBase* m_base;
};

QTITAN_END_NAMESPACE

#endif //QTN_GRID_EDITORS_H


