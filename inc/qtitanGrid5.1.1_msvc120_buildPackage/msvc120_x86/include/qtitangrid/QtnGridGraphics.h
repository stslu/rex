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

#ifndef QTN_GRID_GRAPHICS_H
#define QTN_GRID_GRAPHICS_H

#include <QSet>
#include <QBasicTimer> 
#include <QTime>
#include <QWidget>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QScrollBar>
#include <QMouseEvent>
#include <QGraphicsEffect>
#include <QTextDocument>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QWindow>
#endif

#include "QtnGridDef.h"

QTITAN_BEGIN_NAMESPACE

class GraphicControl;
class GraphicPrimitive;
class GridDragDropObject;
class GridGraphicMask;
class Effects;
class GridNativeDragObject;
enum GridDragDropState
{
    dragNone,
    dragPreparing,
    dragActive
};

class QTITAN_EXPORT GraphicControl: public QWidget
{
Q_OBJECT
public:
    friend class GraphicPrimitive;
    friend class GraphicPrimitiveScrollBar;
    friend class EmbedGraphicPrimitive;
    GraphicControl(QWidget* parent);
    virtual ~GraphicControl();
    GridDragDropState dragDropState() const;
    GridDragDropObject* getDragDropObject() const;
    void updateLayout();
    void setFocusGP(GraphicPrimitive* gp, Qt::FocusReason reason = Qt::OtherFocusReason);
    GraphicPrimitive* focusedGP() const { return m_focusedGP; }
    void setPressedGP(GraphicPrimitive* gp);
    GraphicPrimitive* activeGP() const { return m_activeGP; }
    GraphicPrimitive* overGP() const { return m_overGP; }
    GraphicPrimitive* pressedGP() const { return m_pressedGP; }
    GraphicPrimitive* hitTest(const QPoint& point);
    void mouseOverUpdate(const QPoint& pos, bool forcePressGP);
    void geometryChanged();
protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void focusInEvent(QFocusEvent *);
    virtual void focusOutEvent(QFocusEvent *);
    virtual void hideEvent(QHideEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void wheelEvent(QWheelEvent *);
    virtual bool event(QEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void dragMoveEvent(QDragMoveEvent *);
    virtual void dragLeaveEvent(QDragLeaveEvent *);
    virtual void dropEvent(QDropEvent *);
    virtual void contextMenuEvent(QContextMenuEvent* event);
protected:
    virtual void moveDragDropObject(const QPoint& pos, GridDragDropObject* object);
    virtual void stopDragDrop();
    virtual GridNativeDragObject* createNativeDragDropObject(const QPoint& pos);
    Effects* effects() const;
    virtual bool isLocked() const;
protected:
    GraphicPrimitive* rootPrimitive() const;
    void setRoot(GraphicPrimitive* root);        
    void setActiveGP(GraphicPrimitive* gp);
    void widgetFocusIn(QWidget* widget, QFocusEvent* e);
    void widgetFocusOut(QWidget* widget, QFocusEvent* e);

    GraphicPrimitive* m_overGP;
    GraphicPrimitive* m_pressedGP;
    GraphicPrimitive* m_activeGP;
    GraphicPrimitive* m_focusedGP;
    GraphicPrimitive* m_rootGP;
    GridDragDropObject* m_dragDropObject;
    bool m_dragDropObjectOwner;
private:
    void hideGPFocus(Qt::FocusReason reason);
    void doMouseMove(const QPoint& pos, QMouseEvent* e);
    void deleteGPNotify(GraphicPrimitive* gp);
    bool emulateMouseReleaseEvent(Qt::MouseButton button);
    GridDragDropState m_dragDropState;
    QPoint m_dragStartPos;
    Effects* m_effects;
    bool m_updateFocus;
    QBasicTimer m_keyReleaseTimer;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    ulong m_mousePressTime;
    QPoint m_pressPos;
#endif
};


class GridDragDropObject: public QObject
{
    Q_OBJECT
public:
    friend class GraphicControl;
    GridDragDropObject(GraphicPrimitive* source);
    virtual ~GridDragDropObject();
    bool getImmediateStart();
    void setImmediateStart(bool immediateStart);
    const QCursor& cursor() const;
    void setCursor(const QCursor& cursor);
    void setDefaultCursor();
    const QCursor& acceptedCursor() const;
    void setAcceptedCursor(const QCursor& cursor);
    void setDefaultAcceptedCursor();
    virtual void beginEvent();
    virtual void moveEvent(const QPoint& pos);
    virtual void acceptEvent();
    virtual void endEvent();
    virtual void paintEvent(QPainter& painter);
    GraphicPrimitive* source() const;
    GraphicPrimitive* target() const;
    QPoint cursorPos() const;
    virtual bool isAccepted() const;
private:
    void setTarget(GraphicPrimitive* target);
    GraphicPrimitive* m_source;
    GraphicPrimitive* m_target;
    bool m_immediateStart;
    QCursor m_cursor;
    QCursor m_acceptedCursor;
};

class GridNativeDragObject: public GridDragDropObject
{
    Q_OBJECT
public:
    GridNativeDragObject(GraphicControl* _control, GraphicPrimitive* source);
    QMimeData* mimeData() const;
    void setMimeData(QMimeData* data);
    Qt::DropAction dropAction() const;
    void setDropAction(Qt::DropAction action);
    GraphicControl* control() const;
private:
    GraphicControl* m_control;
    QMimeData* m_data;
    Qt::DropAction m_dropAction;
};

struct QtnPaintData
{
    GraphicPrimitive* gp;
    double opacity;
    int gx;
    int gy;
    QRect clip;
};

typedef QMultiMap<int, QtnPaintData> GraphicPaintOrder;

class GridGraphicMask
{
public:
    GridGraphicMask(GraphicPrimitive* owner, const QRect& r);
    void clear() const;
    const QRect& rect() const;
    void setRect(const QRect& r);
    void addGP(GraphicPrimitive* gp) const;
    void removeGP(GraphicPrimitive* gp) const;
    bool isGPMasked(GraphicPrimitive* gp) const;
protected:
private:
    GraphicPrimitive* m_owner;
    QRect m_rect;
};

class QTITAN_EXPORT GraphicPrimitive : public QObject
{
public:
    struct FixedState
    {
        FixedState(bool fx, bool fy) { x = fx; y = fy; }
        bool x;
        bool y;
    };
    friend class GraphicControl;
    friend class ScrollableGraphicPrimitive;
    friend class GridGraphicMask;
    friend class GridDragDropObject;
    typedef QVector<GraphicPrimitive *> GraphicChildren;
    GraphicPrimitive(GraphicControl* control, GraphicPrimitive* parent);
    GraphicPrimitive(GraphicPrimitive* parent);
    virtual ~GraphicPrimitive();
    GraphicControl* control() const;
    int id() const;
    void setId(int id);
    void setRect(const QRect& rect);
    void setLeft(int left);
    void setTop(int top);
    void setWidth(int width);
    void setHeight(int height);
    inline const QRect& rect() const { return m_rect; }
    inline int left() const { return m_rect.left(); }
    inline int top() const { return m_rect.top(); }
    inline int width() const { return m_rect.width(); }
    inline int height() const { return m_rect.height(); }
    inline QSize size() const { return m_rect.size(); }
    void setBorder(int width);
    void setBorder(int left, int right, int top, int bottom);
    void setBorderLeft(int left);
    virtual int borderLeft() const;
    void setBorderRight(int right);
    virtual int borderRight() const;
    void setBorderTop(int top);
    virtual int borderTop() const;
    void setBorderBottom(int bottom);
    virtual int borderBottom() const;
    QColor borderColor() const;
    void setBorderColor(const QColor & color);
    QBrush borderBrush() const;
    void setBorderBrush(const QBrush & brush);
    virtual const QPen& borderPen() const;
    void setBorderPen(const QPen& pen);
    bool isMouseOver() const;
    bool isMouseOverRecursive() const;
    inline bool isLeftButtonPressed() const { return m_isLeftButtonPressed; }
    inline bool isLeftButtonDblClicked() const { return m_isLeftButtonDblClicked; }
    bool visibility() const;
    void setVisibility(bool visible);
    void setOpacity(double value);
    double opacity() const;
    virtual QRect viewRect() const;
    bool isDragging() const;
    QRect boundedRect() const;
    virtual QCursor cursor() const;
    void paint(int gx, int gy, int parentScrollX, int parentScrollY, 
        double opacity, const QRect& parentRect, GraphicPaintOrder& paintOrder);
    void print(QPainter& painter);
    GraphicPrimitive* parentGP() const;
    void setParentGP(GraphicPrimitive* parent);
    const GraphicChildren& childrenGP() const;
    QPoint mapViewportToSite(const QPoint& point) const;
    QPoint mapViewportFromSite(const QPoint& point) const;
    QPoint mapViewportToGlobal(const QPoint& point) const;
    QPoint mapViewportFromGlobal(const QPoint& point) const;
    QPoint mapToSite(const QPoint& point) const;
    QPoint mapFromSite(const QPoint& point) const;
    QPoint mapToGlobal(const QPoint& point) const;
    QPoint mapFromGlobal(const QPoint& point) const;
    GridGraphicMask& mask();
    bool transparent() const;
    void setTransparent(bool transparent);
    int zIndex() const;
    void setZIndex(int zIndex);
    virtual void update(const QRect& rect);
    virtual void update();
    virtual void repaint(const QRect& rect);
    virtual void repaint();
    virtual void updateLayout();
    virtual bool scrollable() const;
    virtual bool embeded() const;
    bool childrenDrawable();
    void setChildrenDrawable(bool drawable);
    bool isUpdating() const;
    bool airSupported() const;
    void setAirSupported(bool b);
    virtual QSize sizeHint() { return QSize(50, 10);  }
    bool visibleOnScreen() const;
    void setFocus();
    bool hasFocus() const;
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void resizeEvent(const QRect& rect);
    virtual void wheelEvent(const QPoint& pos, QWheelEvent* event);
    /* focus events */
    virtual void focusInEvent(QFocusEvent* event);
    virtual void focusOutEvent(QFocusEvent* event);
    /* mouse events */
    virtual void mouseMoveEvent(const QPoint& pos, QMouseEvent* event);
    virtual void mousePressEvent(const QPoint& pos, QMouseEvent* event);
    virtual void mouseReleaseEvent(const QPoint& pos, QMouseEvent* event);
    virtual void mouseDoubleClickEvent(const QPoint& pos, QMouseEvent* event);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
    /* keyboard events */
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    /* block events */
    virtual void windowBlockedEvent(QEvent* event);
    virtual void windowUnBlockedEvent(QEvent* event);
    /* drag and drop events and functions */
    virtual bool checkDragDropEvent(const QPoint& pos, GridDragDropObject* object);
    virtual void acceptDragDropEvent(const QPoint& pos, GridDragDropObject* object);
    virtual void endDragDropEvent();
    virtual void showContextMenuEvent(const QPoint& pos, QContextMenuEvent* event);
protected:
    void init(GraphicControl* control, GraphicPrimitive* parent);
    GraphicPrimitive* hitTest(const QPoint& point, int gx, int gy, int parentScrollX, int parentScrollY, const QRect& parentRect, int& zindex);
    bool checkClickEvent(QMouseEvent* e, bool dblClick = false);
    void setCursor(const QCursor& cursor);
    QRect translateToSiteRect(const QRect& rect) const;
    QRect translateFromSiteRect(const QRect& rect) const;
    bool isLayoutChanged() const;
    void layoutChanged();
    virtual void drawBorder(QPainter& painter);
    virtual GridDragDropObject* createDragDropObject(const QPoint& pos);
    Effects* effects() const { return control()->effects(); }
    GraphicControl* m_control;
    int m_id;
    QRect m_rect;
    QPen m_borderPen;
    int m_borderLeft;
    int m_borderRight;
    int m_borderTop;
    int m_borderBottom;
private:
    void deleteChildren();
    GraphicPrimitive* m_parentGP;
    GraphicChildren m_childrenGP;
    bool m_visible;
    double m_opacity;
    QCursor m_cursor;
    bool m_isLeftButtonPressed;
    bool m_isLeftButtonDblClicked;
    GridGraphicMask m_graphicMask;
    bool m_transparent;
    int m_zIndex;
    bool m_layoutChanged;
    FixedState m_fixedState;
    bool m_masked;
    bool m_childrenDrawable;
    bool m_isUpdating;
    bool m_airSupported;
public:
    static void drawBorderPrimitive(QPainter& painter,
                                    QPen pen, 
                                    const QRect& rect,
                                    const QRect& clip,
                                    int borderLeft, int borderRight,
                                    int borderTop, int borderBottom);
};

enum ScrollOverflow
{
    gsoNone,
    gsoVisible,
    gsoAuto
};

class QTITAN_EXPORT ScrollableGraphicPrimitive : public GraphicPrimitive
{
    Q_OBJECT
public:
    friend class GraphicControl;
    ScrollableGraphicPrimitive(GraphicPrimitive* parent);
    ScrollableGraphicPrimitive(GraphicControl* control, GraphicPrimitive* parent);
    virtual ~ScrollableGraphicPrimitive();
    QScrollBar* scrollVer();
    QScrollBar* scrollHor();
    virtual int scrollX() const;
    virtual int scrollY() const;
    void setScrollX(int x);
    void setScrollY(int y);
    inline ScrollOverflow overflowX() const { return m_overflowX; }
    inline ScrollOverflow overflowY() const { return m_overflowY; }
    void setOverflow(ScrollOverflow overflow);
    void setOverflowX(ScrollOverflow overflow);
    void setOverflowY(ScrollOverflow overflow);
    const QSize& viewSize() const;
    void setViewSize(const QSize& size);
    virtual QRect viewRect() const;
    QRect boundedRect() const;
    static void setFixedGPX(GraphicPrimitive* primitive, bool value);
    static void setFixedGPY(GraphicPrimitive* primitive, bool value);
    static bool isGPFixedX(const GraphicPrimitive* primitive);
    static bool isGPFixedY(const GraphicPrimitive* primitive);
    virtual void updateLayout();
    virtual bool scrollable() const;
    virtual bool canScrollX() const;
    virtual bool canScrollY() const;
    virtual void updateScrollBars();
    QSize minBoundedSize();
    bool isUpdateOnScrolling() const;
    void setUpdateOnScrolling(bool updateOn);
    void setVerScrollBarVisible(bool visible);
    bool isVerScrollBarVisible() const;
    void setHorScrollBarVisible(bool visible);
    bool isHorScrollBarVisible() const;
protected:
    void setScrollBarsPosition();
    void initGP();
protected:
    virtual void wheelEvent(const QPoint&, QWheelEvent *);
    virtual void scrollXEvent(int position);
    virtual void scrollYEvent(int position);
private:
    QScrollBar* m_scrollVer2;
    QScrollBar* m_scrollHor2;
    int m_scrollX;
    int m_scrollY;
    ScrollOverflow m_overflowX;
    ScrollOverflow m_overflowY;
    QSize m_viewSize;
    bool m_updateOnScrolling;
    bool m_verScrollBarVisible;
    bool m_horScrollBarVisible;
public Q_SLOTS:
    void scrollVerChanged(int value);
    void scrollHorChanged(int value);
};

class PaintBuffer
{
public:
    PaintBuffer();
    void setSize(const QSize& size);
    QPainter& begin();
    void end();
    const QImage& buffer() const;
    bool empty() const;
    void reset();
private:
    QImage m_buffer;
    QPainter m_painter;
    bool m_empty;
};
    
class BufferedGraphicPrimitive: public GraphicPrimitive
{
public:
    BufferedGraphicPrimitive(GraphicControl* control, GraphicPrimitive* parent);
    virtual void update(const QRect& rect);
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void resizeEvent(const QRect& rect);
    virtual void paintBufferEvent(QPainter& painter) = 0;
protected:
    void resetPaintBuffer();
private:
    PaintBuffer m_paintBuffer;
};
    
class EmbedGraphicPrimitive: public GraphicPrimitive
{
public:
    friend class GraphicControl;
    EmbedGraphicPrimitive(GraphicControl* control, GraphicPrimitive* parent);
    virtual ~EmbedGraphicPrimitive();
    virtual void setWidget(QWidget* widget);
    QWidget* widget() const;
    bool isSubWidget(QWidget* widget);
    virtual QSize sizeHint();
    virtual bool embeded() const;
protected:
    virtual void paintEvent(QPainter& painter);
    virtual void resizeEvent(const QRect& rect);
    void updateWidget(bool force = false);
    virtual bool eventFilter(QObject *, QEvent *);
    /* mouse events */
    virtual void mouseMoveEvent(const QPoint& point, QMouseEvent* e);
    virtual void mousePressEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseReleaseEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e);
    virtual void mouseOverEvent(const QPoint& pos);
    virtual void mouseOutEvent();
    virtual void wheelEvent(const QPoint&, QWheelEvent *);
    /* keyboard events */
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void keyReleaseEvent(QKeyEvent* e);
    /* focus events */
    virtual void focusInEvent(QFocusEvent *);
    virtual void focusOutEvent(QFocusEvent *);
    virtual void showContextMenuEvent(const QPoint& pos, QContextMenuEvent* event);
    virtual QCursor cursor() const;
private:
    void installFilterToWidget(QWidget* widget);
    void removeFilterFromWidget(QWidget* widget);
    void removeFocus();
    void doMouseOverEvent(const QPoint& pos);
    void doMouseOutEvent();
    void connectWidget();
    void disconnectWidget();
    void setInternalWinPos() const;
    QWidget* getOverWidget(const QPoint& pos, QPoint& widgetPos);
    void setFocusChild(QWidget* fw);
    QWidget* m_widget;
    QSet<QWidget*> m_filteredWidgets;
    QWidget* m_overWidget;
    QWidget* m_pressedWidget;
    QPixmap m_paintBuffer;
private:
    bool m_keyhandling;
private:
    static QHash<QWidget *, EmbedGraphicPrimitive *> allEmbedWidgets;
    static bool qapplication_callback(void** context);
};

class GraphicPrimitiveScrollBar: public QScrollBar
{
    Q_OBJECT
public:
    friend class ScrollableGraphicPrimitive;
    GraphicPrimitiveScrollBar(Qt::Orientation, ScrollableGraphicPrimitive* parent);
protected:
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mousePressEvent(QMouseEvent *);
private:
    ScrollableGraphicPrimitive* m_scrollableGraphic;
};

extern int mul_div(int a, int b, int c);
extern void draw_text(QPainter& painter, const QString& text, const QRect& rect, int flags, bool transparent = true);
extern bool intersect_line(QLineF& line, const QRect& region);
extern void draw_line(QPainter& painter, const QLineF& line);
extern void draw_focus_frame(QPainter& painter, const QColor& color, const QRect& rect);
extern void set_clip_reg(QPainter& painter, const QRect& rect);
extern QColor qcolor_mix_rgb(const QColor& first, const QColor& second);
extern QColor qcolor_mix_cmy(const QColor& first, const QColor& second);
extern QVariant::Type QTITAN_EXPORT qvariant_to_pixmap(const QVariant& v, QPixmap& pixmap);
extern void draw_pixmap_texture(QPainter& painter, const QRect& rect, const QPixmap& texture, int left, int top, int right, int bottom);
extern QPixmap apply_effect_to_pixmap(const QPixmap& pixmap, QGraphicsEffect *effect, double opacity = 1.0);
extern QPixmap qtn_create_background(const QColor& color);
extern void qtn_QTextDocument_draw(QPainter& painter, QTextDocument* document, const QPalette& palette, const QRect& rect);


class SmoothEffect
{
public:
    friend class Effects;
    SmoothEffect();
    void intialize(GraphicPrimitive* gp);
    void setInitialState(const QImage& iState);
    void setInitialStateToEmpty(int width, int height);
    int duration() const;
    void setDuration(int duration);
    bool isActive() const;
    void paintToCurrent(QPainter* painter, double& currentOpacity);
    void paintToInitial(QPainter* painter);
private:
    QImage m_istate;
    GraphicPrimitive* m_gp;
    int m_duration;
    bool m_active;
    QTime m_time;
};

class Effects: public QObject
{
public:
    Effects(QWidget* control);
    virtual ~Effects();
    void run(SmoothEffect* effect);
    void stop(SmoothEffect* effect);
    void update();
    SmoothEffect* findEffect(GraphicPrimitive* gp);
protected:
    virtual bool event(QEvent* e);
private:
    QWidget* m_control;
    QList<SmoothEffect *> m_list;
    QBasicTimer m_timer;
};

class EffectSettings
{
public:
    EffectSettings();
    virtual ~EffectSettings();
    void setStyleOption(const QStyleOptionHeader& option);
    const QStyleOptionHeader& styleOption() const;
    QSize size() const;
    void setBg(const QImage& bg);
    const QImage& bg() const;
private:
    QStyleOptionHeader m_opt;
    QImage m_bg;
};

#ifdef Q_OS_MAC
void set_widget_mac_hook(EmbedGraphicPrimitive* gp, QWidget* w);
void unset_widget_mac_hook(EmbedGraphicPrimitive* gp, QWidget* w);
#endif
#ifdef Q_OS_WIN
void paintAirEffect(QPainter& painter, GraphicPrimitive* gp, const QRect& rect, QRegion& air);
void drawDwmCaptionText(QPainter* painter, const QRect& rect, const QString& strWindowText, bool active);
#endif
bool isAirAvailable();

class CustomShadowEffect : public QGraphicsEffect
{
    Q_OBJECT
public:
    explicit CustomShadowEffect(QObject *parent = 0);

    void draw(QPainter* painter);
    QRectF boundingRectFor(const QRectF& rect) const;

    inline void setDistance(qreal distance) { _distance = distance; updateBoundingRect(); }
    inline qreal distance() const { return _distance; }

    inline void setBlurRadius(qreal blurRadius) { _blurRadius = blurRadius; updateBoundingRect(); }
    inline qreal blurRadius() const { return _blurRadius; }

    inline void setColor(const QColor& color) { _color = color; }
    inline QColor color() const { return _color; }

private:
    qreal  _distance;
    qreal  _blurRadius;
    QColor _color;
};


QTITAN_END_NAMESPACE

#ifndef QT_NO_EFFECTS
QT_BEGIN_NAMESPACE
struct QEffects
{
    enum Direction {
        LeftScroll  = 0x0001,
        RightScroll = 0x0002,
        UpScroll    = 0x0004,
        DownScroll  = 0x0008
    };

    typedef uint DirFlags;
};

extern void Q_GUI_EXPORT qScrollEffect(QWidget*, QEffects::DirFlags dir = QEffects::DownScroll, int time = -1);
extern void Q_GUI_EXPORT qFadeEffect(QWidget*, int time = -1);
QT_END_NAMESPACE
#endif

#endif //QTN_GRID_GRAPHICS_H


