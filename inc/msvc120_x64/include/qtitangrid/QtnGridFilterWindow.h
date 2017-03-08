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

#ifndef QTN_GRID_FILTER_GRAPHICS_H
#define QTN_GRID_FILTER_GRAPHICS_H

#include <QDialog>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QFormLayout>

#include "QtnGridFilter.h"
#include "QtnGridGraphics.h"
#include "QtnGridTableViewBase.h"
#include "QtnGridTableView.h"

QTITAN_BEGIN_NAMESPACE

class GridFilterWindow: public GraphicControl
{
public:
    GridFilterWindow(QWidget* parent, GridViewBase* view);
    virtual ~GridFilterWindow();
private:
    GridViewBase* m_viewBase;
};

class GridFilterArea: public ScrollableGraphicPrimitive
{
public:
    GridFilterArea(GridFilterWindow* window, GridFilter* filter);
    virtual ~GridFilterArea();
protected:
private:
    GridFilter* m_filter;
};

class GridFilterColumnPopup: public EditorPopup
{
    Q_OBJECT
public:
    GridFilterColumnPopup(QWidget* parent, GridDataTableColumn* column);
    void setCustomFilter();
    void setBlanksFilter();
    void setNoBlanksFilter();
    int heightHint();
    void updateMask();
protected:
    virtual void okClick();
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *); 
private Q_SLOTS:
    void selectionChanged(GridSelection* selection, GridSelection* oldSelection);
private:
    GridViewBase* m_view;
    GridDataTableColumn* m_column;
    QPixmap m_bg;
};

class FilterPopupWidget: public QWidget
{
    Q_OBJECT
public:
    FilterPopupWidget(QWidget* parent, GridDataTableColumn* column);
    GridViewBase* getGridView();
protected:
    QAbstractItemModel* createModel();
private:
    GridDataTableColumn* m_column;
    QWidget* m_header;
    Qtitan::Grid* m_valuesGrid;
private Q_SLOTS:
    void selectBlanksClicked(const QString& link);
    void selectNoBlanksClicked(const QString& link);
    void selectClicked(const QString& link);
    void unselectClicked(const QString& link);
};


class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    FilterDialog(GridViewBase* view, GridFilterGroupCondition* condition, QWidget *parent);
    virtual ~FilterDialog();
    void setEnabledSecondField(bool enabled);
    void fillColumns(QComboBox* box);
    GridFilterGroupCondition* condition() const;
    GridViewBase* view() { return m_view; };
private Q_SLOTS:
    void dialogOk();
    void dialogCancel();
    void dialogClicked(QAbstractButton * button);
    void clickedOR(bool checked);
    void clickedAND(bool checked);
    void clickedANDNOT(bool checked);
    void column1IndexChanged(int index);
    void column2IndexChanged(int index);
private:
    GridViewBase* m_view;
    GridFilterGroupCondition* m_condition;
    QTabWidget* m_tab;
    QWidget* m_filterWidget;
    QComboBox* m_columns1;
    QComboBox* m_operator1;
    QWidget* m_findWhatContainer1;
    QComboBox* m_columns2;
    QComboBox* m_operator2;
    QWidget* m_findWhatContainer2;
    QHBoxLayout* m_findwhatlayout1;
    QHBoxLayout* m_findwhatlayout2;

    QCheckBox* m_addToFilter;
    QRadioButton* m_or;
    QRadioButton* m_and;
    QRadioButton* m_andnot;
    QDialogButtonBox* m_buttons;
    QFormLayout* m_formLayout;
    GridEditorSite* m_columnSite1;
    GridEditorSite* m_columnSite2;
};

class FilterTextWidget: public QWidget
{
    Q_OBJECT
public:
    FilterTextWidget(QWidget* parent);
    void setText(const QString& text);
    virtual QSize sizeHint() const;
    bool isGlassHighlight() const;
    void setGlassHighlight(bool flag);
Q_SIGNALS:
    void clicked();
protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    bool event(QEvent* e);
private:
    QString m_text;
    bool n_needHint;
    bool m_hover;
    QBasicTimer m_tooltipTimer;
    bool m_glassHighlight;
};

class GridFilterHistoryCloseButtonPrimitive : public GridCloseButtonPrimitive
{
public:
    GridFilterHistoryCloseButtonPrimitive(GraphicPrimitive* parent);
protected:
    virtual void clicked();
private:
};

class GridHistoryItemButton : public GridShadeButtonPrimitive
{
    Q_OBJECT
public:
    GridHistoryItemButton(GraphicPrimitive* parent);
    virtual QSize sizeHint();
protected:
    virtual void paintTexture(QPainter& painter, const QColor& accentColor, const QRect& rect);
protected:
    virtual QPalette getButtonPalette() const;
private:
};

class FilterHistoryPopupPrimitive : public GridWinShadowedPrimitive
{
    Q_OBJECT
public:
    FilterHistoryPopupPrimitive(GraphicPrimitive* parent, GridFilterHistory* history);
    virtual ~FilterHistoryPopupPrimitive();
    virtual void updateLayout();
    virtual QSize sizeHint();
protected:
    virtual void paintBackground(QPainter& painter);
private:
    QList<GridHistoryItemButton *> m_items;
    GridFilterHistory* m_history;
    GridCloseButtonPrimitive* m_closeButtonGP;
    QSize m_sizeHint;
private Q_SLOTS:
    void itemClicked();
    void fillItems();
};

QTITAN_END_NAMESPACE

#endif //QTN_GRID_FILTER_GRAPHICS_H


