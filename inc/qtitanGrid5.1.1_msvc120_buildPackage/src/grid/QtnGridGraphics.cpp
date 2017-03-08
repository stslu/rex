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


//#define _DEBUG_PAINT
//#define _DEBUG_UPDATE
#include <QApplication>
#include <QTextEdit>
#include <QMenu>
#include <QDebug>
#include <QMessageBox>
#include <QPaintEngine>
#include <QMouseEvent>
#include <QMdiSubWindow>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif
#include <QStyleOptionFrameV3>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QAbstractTextDocumentLayout>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QStyleHints>
#define QT_INCLUDE_PRIVATE_H
#endif

#ifdef QT_INCLUDE_PRIVATE_H
#include <private/qwidget_p.h>
#endif
#ifdef Q_OS_LINUX
#include <stdint.h>
typedef long long int __int64;
#endif
#ifdef Q_OS_MACX
typedef long long int __int64;
#endif
#include "QtnGridGraphics.h"
#include "QtnGridDef.h"

QTITAN_USE_NAMESPACE

/*!
    \class GraphicControl
    \brief GraphicControl
    \inmodule QtitanDataGrid
*/

/* GraphicControl */
GraphicControl::GraphicControl(QWidget* parent)
: QWidget(parent), m_overGP(Q_NULL), m_pressedGP(Q_NULL), 
  m_activeGP(Q_NULL), m_focusedGP(Q_NULL), m_rootGP(Q_NULL),
  m_dragDropObject(Q_NULL), m_dragDropObjectOwner(false), m_dragDropState(dragNone), m_updateFocus(false)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))  
  , m_mousePressTime(0)
#endif
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_DontCreateNativeAncestors, true);
    m_effects = new Effects(this);
}

GraphicControl::~GraphicControl()
{
    Q_DELETE_AND_NULL(m_effects);
}

GridDragDropState GraphicControl::dragDropState() const
{
    return m_dragDropState;
}

GridDragDropObject* GraphicControl::getDragDropObject() const
{
    return m_dragDropObject;
}

void GraphicControl::paintEvent(QPaintEvent* e)
{
    if (m_rootGP == Q_NULL)
        return;

    bool locked = isLocked();

    QVector<QRect> paintRects = e->region().rects();
    QPainter painter(this);
#ifdef _DEBUG
#ifdef _DEBUG_PAINT
    static int s_paintCount = 0;
    s_paintCount++;
#endif
#endif
#ifdef Q_OS_WIN
    QRegion air;
#endif
    for (QVector<QRect>::const_iterator it = paintRects.constBegin(); it != paintRects.constEnd(); ++it)
    {
        const QRect& paintRect = *it;
#ifdef _DEBUG
#ifdef _DEBUG_PAINT
    QRect updaterect = paintRect;
    QString s = QString("[paint: %1]left = %2, top = %3, width = %4, height = %5\n").arg(s_paintCount/*paintRects.size()*/).arg(updaterect.left()).arg(updaterect.top()).arg(updaterect.width()).arg(updaterect.height());
    OutputDebugStringA((char *)s.toAscii().constData());
#endif
#endif
        GraphicPaintOrder paintOrder;
        m_rootGP->paint(0, 0, 0, 0, windowOpacity(), paintRect, paintOrder);
        for (GraphicPaintOrder::const_iterator it = paintOrder.begin(); it != paintOrder.end(); ++it)
        {
            const QtnPaintData& pdata = it.value();
            painter.save();
#if 0
            if (pdata.clip.width() != pdata.gp->rect().width() || 
                pdata.clip.height() != pdata.gp->rect().height())
                painter.setClipRect(pdata.clip, Qt::IntersectClip);
#endif
            painter.setClipRect(pdata.clip, Qt::ReplaceClip);
            painter.translate(pdata.gx, pdata.gy);
            if (pdata.opacity != 1.0)
                painter.setOpacity(pdata.opacity);
#ifdef Q_OS_WIN
            if (!locked && pdata.gp->airSupported())
                paintAirEffect(painter, pdata.gp, paintRect, air);
#endif
            if (!locked)
            {
                pdata.gp->paintEvent(painter);
                pdata.gp->drawBorder(painter);
            }
            pdata.gp->m_isUpdating = false;
            painter.restore();
        }
    }
    if (m_dragDropObject != Q_NULL && m_dragDropState == dragActive && !locked)
        m_dragDropObject->paintEvent(painter);
}

void GraphicControl::resizeEvent(QResizeEvent *)
{
    if (m_rootGP)
        m_rootGP->setRect(rect());
}

void GraphicControl::mouseMoveEvent(QMouseEvent* e)
{
    doMouseMove(mapFromGlobal(e->globalPos())/* Don't use e->pos(), we have a problem on MACX with it.*/, e);
}

void GraphicControl::mouseOverUpdate(const QPoint& pos, bool forcePressGP)
{
    GraphicPrimitive* target = hitTest(pos);

    if (m_overGP != target)
    {
        GraphicPrimitive* old = m_overGP;

        m_overGP = target;

        if (old)
        {
            if (forcePressGP || m_pressedGP == Q_NULL)
                old->mouseOutEvent();
        }

        if (m_overGP)
        {
            if (forcePressGP || m_pressedGP == Q_NULL)
            {
                QRect r = m_overGP->translateToSiteRect(m_overGP->rect());
                m_overGP->mouseOverEvent(QPoint(pos.x() - r.x(), pos.y() - r.y()));
            }
        }
    }
}

void GraphicControl::doMouseMove(const QPoint& pos, QMouseEvent* e)
{
    if (!m_rootGP)
        return;

    if (m_dragDropState == dragPreparing)
    {
        if ((pos - m_dragStartPos).manhattanLength() >= QApplication::startDragDistance())
        {
            m_dragDropState = dragActive;
            grabKeyboard();
            m_dragDropObject->beginEvent();
        }
    }

    if (m_dragDropState == dragActive)
    {
        Q_ASSERT(m_dragDropObject != Q_NULL);
        moveDragDropObject(pos, m_dragDropObject);
    }

    if (m_dragDropState == dragNone)
    {

        mouseOverUpdate(pos, false);

        if (m_pressedGP != Q_NULL)
        {
            QRect r = m_pressedGP->translateToSiteRect(m_pressedGP->rect());
            m_pressedGP->mouseMoveEvent(QPoint(pos.x() - r.x(), pos.y() - r.y()), e);
            setCursor(m_pressedGP->cursor());
        }
        else if (m_overGP)
        {
            QRect r = m_overGP->translateToSiteRect(m_overGP->rect());
            m_overGP->mouseMoveEvent(QPoint(pos.x() - r.x(), pos.y() - r.y()), e);
            if (m_overGP != Q_NULL)
                setCursor(m_overGP->cursor());
        }
    }
}

void GraphicControl::deleteGPNotify(GraphicPrimitive* gp)
{
    SmoothEffect* eff = m_effects->findEffect(gp);
    if (eff)
        m_effects->stop(eff);
    if (m_focusedGP == gp)
        m_focusedGP = Q_NULL;
    if (m_overGP == gp)
        m_overGP = Q_NULL;
    if (m_pressedGP == gp)
        m_pressedGP = Q_NULL;
    if (m_rootGP == gp)
        m_rootGP = Q_NULL;
    if (m_activeGP == gp)
        setActiveGP(Q_NULL);
    if (m_dragDropObject != Q_NULL && m_dragDropObject->source() == gp)
        m_dragDropObject->m_source = Q_NULL;
    if (m_dragDropObject != Q_NULL && m_dragDropObject->target() == gp)
        m_dragDropObject->m_target = Q_NULL;
}

void GraphicControl::setPressedGP(GraphicPrimitive* gp)
{
    if (m_pressedGP != Q_NULL)
        m_pressedGP->m_isLeftButtonPressed = false;

    m_pressedGP = gp;

    if (m_pressedGP != Q_NULL)
        m_pressedGP->m_isLeftButtonPressed = true;
}

void GraphicControl::mousePressEvent(QMouseEvent *event)
 {
    if (!m_rootGP)
        return;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
    bool doubleClick = event->flags().testFlag(Qt::MouseEventCreatedDoubleClick);
#else
    ulong doubleClickInterval = static_cast<ulong>(qApp->styleHints()->mouseDoubleClickInterval());
    bool samePoint = (qAbs(event->globalPos().x() - m_pressPos.x()) < 5 &&
        qAbs(event->globalPos().y() - m_pressPos.y()) < 5); /* see mouse_double_click_distance in Qt code for details */
    bool doubleClick = (event->timestamp() - m_mousePressTime < doubleClickInterval) && samePoint;
#endif    
    m_mousePressTime = event->timestamp();
    m_pressPos = event->globalPos();

    if (doubleClick)
        return;
#endif 

    if (m_pressedGP)
        emulateMouseReleaseEvent(Qt::NoButton);
    else
        m_keyReleaseTimer.start(50, this);

    GraphicPrimitive* target = hitTest(event->pos());
    Q_ASSERT(target != Q_NULL);
    if (target == Q_NULL)
        return;

    setPressedGP(target);

    QRect r = target->translateToSiteRect(target->rect());
    QPoint pos(event->pos().x() - r.x(), event->pos().y() - r.y());

    if (target != Q_NULL && event->button() == Qt::LeftButton)
    {
        m_dragDropObject = target->createDragDropObject(pos);
        if (m_dragDropObject != Q_NULL)
        {
            Q_ASSERT(m_dragDropState == dragNone);
            m_dragStartPos = event->pos();
            m_dragDropObjectOwner = false;
            if (m_dragDropObject->getImmediateStart())
            {
                m_dragDropState = dragActive;
                grabKeyboard();
                m_dragDropObject->beginEvent();
            }
            else
                m_dragDropState = dragPreparing;
        }
    }

    m_pressedGP->mousePressEvent(pos, event);
 }

void GraphicControl::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!m_rootGP)
        return;

    if (m_pressedGP)
        emulateMouseReleaseEvent(event->button());
    else
        m_keyReleaseTimer.start(50, this);

    GraphicPrimitive* target = hitTest(event->pos());
    Q_ASSERT(target != Q_NULL);
    if (target == Q_NULL)
        return;
    QRect r = target->translateToSiteRect(target->rect());
    QPoint pos(event->pos().x() - r.x(), event->pos().y() - r.y());

    m_pressedGP = target;
    m_pressedGP->m_isLeftButtonPressed = true;
    m_pressedGP->m_isLeftButtonDblClicked = true;

    m_pressedGP->mouseDoubleClickEvent(pos, event);
}

void GraphicControl::widgetFocusIn(QWidget* widget, QFocusEvent* e)
{
    GraphicPrimitive* gp = Q_NULL;
    QHash<QWidget *, EmbedGraphicPrimitive *>::const_iterator it = EmbedGraphicPrimitive::allEmbedWidgets.constFind(widget);
    if (it != EmbedGraphicPrimitive::allEmbedWidgets.constEnd())
        gp = it.value();

    if (gp == Q_NULL && widget == this)
    {
        gp = m_activeGP;
        if (gp == Q_NULL)
            gp = m_rootGP;
    }

    if (gp != Q_NULL)
        setFocusGP(gp, e->reason());
}

void GraphicControl::widgetFocusOut(QWidget* widget, QFocusEvent* e)
{
    Q_UNUSED(widget);
    hideGPFocus(e->reason());
}

void GraphicControl::focusInEvent(QFocusEvent* event)
{
    QWidget::focusInEvent(event);
    if (m_updateFocus)
        return;
    widgetFocusIn(this, event);
    update();
}

void GraphicControl::focusOutEvent(QFocusEvent* event)
{
    QWidget::focusOutEvent(event);
    widgetFocusOut(this, event);
    update();
}

void GraphicControl::hideGPFocus(Qt::FocusReason reason)
{
    if (m_updateFocus)
        return;
    GraphicPrimitive* temp = m_focusedGP;
    setFocusGP(Q_NULL, reason);
    setActiveGP(temp);
}

void GraphicControl::hideEvent(QHideEvent *)
{
    setFocusGP(m_activeGP, Qt::ActiveWindowFocusReason);
}

void GraphicControl::moveDragDropObject(const QPoint& pos, GridDragDropObject* object)
{
    GraphicPrimitive* target = hitTest(pos);
    if (target)
    {
        QRect r = target->translateToSiteRect(target->rect());
        QPoint targetPos(pos.x() - r.x(), pos.y() - r.y());
        if (!target->checkDragDropEvent(targetPos, object))
        {
            target = Q_NULL;
        }
    }
    object->setTarget(target);
    object->moveEvent(pos);
}

void GraphicControl::stopDragDrop()
{
    Q_ASSERT(m_dragDropObject != Q_NULL);
    if (m_dragDropObject != Q_NULL)
    {
        if (m_dragDropState == dragActive)
        {
            if (m_dragDropObject->source() != Q_NULL)
                m_dragDropObject->source()->endDragDropEvent();
            m_dragDropObject->endEvent();
        }
        delete m_dragDropObject;
        m_dragDropObject = Q_NULL;
    }
    releaseKeyboard();
    m_dragDropState = dragNone;
    QMouseEvent mouseEvent(QEvent::MouseMove, 
                    mapFromGlobal(QCursor::pos()),
                    QCursor::pos(), 
                    Qt::MouseButton(),
                    Qt::MouseButtons(),
                    Qt::KeyboardModifiers());

    doMouseMove(mapFromGlobal(QCursor::pos()), &mouseEvent);
}

GridNativeDragObject* GraphicControl::createNativeDragDropObject(const QPoint& pos)
{
    return new GridNativeDragObject(this, hitTest(pos));
}

Effects* GraphicControl::effects() const
{
    return m_effects;
}

bool GraphicControl::isLocked() const
{
    return false;
}

void GraphicControl::updateLayout()
{
    if (m_rootGP != Q_NULL)
        m_rootGP->updateLayout();
}

GraphicPrimitive* GraphicControl::hitTest(const QPoint& point)
{
    if (m_rootGP != Q_NULL)
    {
        int zIndex = m_rootGP->zIndex();
        return m_rootGP->hitTest(point, 0, 0, 0, 0, rect(), zIndex);
    }
    else
        return Q_NULL;
}

GraphicPrimitive* GraphicControl::rootPrimitive() const
{ 
    return m_rootGP;
}

void GraphicControl::geometryChanged()
{
    QPoint pos = mapFromGlobal(QCursor::pos());
    mouseOverUpdate(pos, true);
}

void GraphicControl::setRoot(GraphicPrimitive* root)
{
    if (m_rootGP != Q_NULL)
        delete m_rootGP;
    m_rootGP = root;
    if (m_rootGP == Q_NULL)
        return;
    m_rootGP->setRect(rect());
    if (hasFocus())
        setFocusGP(m_rootGP);
}

void GraphicControl::setFocusGP(GraphicPrimitive* gp, Qt::FocusReason reason)
{
    if (m_focusedGP == gp)
        return;

    bool hasFocus = this->hasFocus();

    if (m_focusedGP != Q_NULL)
    {
        QFocusEvent out(QEvent::FocusOut, reason);
        m_focusedGP->focusOutEvent(&out);
    }

    setActiveGP(gp);

    m_focusedGP = hasFocus ? gp : Q_NULL;

    if (m_focusedGP != Q_NULL)
    {
        QFocusEvent in(QEvent::FocusIn, reason);
        m_focusedGP->focusInEvent(&in);
    }

    m_updateFocus = true;
    if (m_focusedGP != Q_NULL)
        setFocus(reason);
    m_updateFocus = false;
}

void GraphicControl::setActiveGP(GraphicPrimitive* gp)
{
    m_activeGP = gp;
    if (m_activeGP != Q_NULL && m_activeGP->embeded() && static_cast<EmbedGraphicPrimitive *>(m_activeGP)->widget() != Q_NULL)
        setFocusProxy(static_cast<EmbedGraphicPrimitive *>(m_activeGP)->widget());
    else
        setFocusProxy(Q_NULL);
}

void GraphicControl::mouseReleaseEvent(QMouseEvent* event)
{
    m_keyReleaseTimer.stop();

    if (m_pressedGP != Q_NULL)
    {
        QRect r = m_pressedGP->translateToSiteRect(m_pressedGP->rect());
        QPoint pos(event->pos().x() - r.x(), event->pos().y() - r.y());
        m_pressedGP->mouseReleaseEvent(pos, event); //May destroy a graphic primitive.
        if (m_pressedGP != 0)
        {
            m_pressedGP->m_isLeftButtonPressed = false;
            m_pressedGP->m_isLeftButtonDblClicked = false;
        }
        m_pressedGP = Q_NULL;
    }
    if (m_dragDropObject != Q_NULL)
    {       
        Q_ASSERT(m_dragDropState != dragNone);
        if (m_dragDropObject->isAccepted())
        {
            GraphicPrimitive* target = m_dragDropObject->target();
            if (target != Q_NULL)
            {
                QRect r = target->translateToSiteRect(target->rect());
                QPoint pos(event->pos().x() - r.x(), event->pos().y() - r.y());
                target->acceptDragDropEvent(pos, m_dragDropObject);
            }
            m_dragDropObject->acceptEvent();
        }
        stopDragDrop();
    }
}

void GraphicControl::enterEvent(QEvent *)
{
    if (!m_rootGP)
        return;

    if (m_dragDropState == dragNone)
    {
        QPoint pos = mapFromGlobal(QCursor::pos());
        mouseOverUpdate(pos, false);
        /*
        Q_ASSERT(m_overGP == Q_NULL);
        m_overGP = hitTest(pos);
        if (m_overGP)
        {
            QRect r = m_overGP->translateToSiteRect(m_overGP->rect());
            m_overGP->mouseOverEvent(QPoint(pos.x() - r.x(), pos.y() - r.y()));
            if (m_overGP != Q_NULL)
                setCursor(m_overGP->cursor());
        }
        */
    }
}

void GraphicControl::leaveEvent(QEvent *)
{
    unsetCursor();

    if (!m_rootGP)
        return;

    if (m_dragDropState == dragNone)
    {
        if (m_overGP)
            m_overGP->mouseOutEvent();
        m_overGP = Q_NULL;
    }
}

void GraphicControl::wheelEvent(QWheelEvent* e)
{
    GraphicPrimitive* target = hitTest(e->pos());
    if (target == Q_NULL)
        target = m_focusedGP;
    e->ignore();
    while (target != Q_NULL && !e->isAccepted())
    {
        QRect r = target->translateToSiteRect(target->rect());
        target->wheelEvent(QPoint(e->pos().x() - r.x(), e->pos().y() - r.y()), e);
        target = target->parentGP();
    }
}

bool GraphicControl::emulateMouseReleaseEvent(Qt::MouseButton button)
{
    Qt::MouseButtons buttons = qApp->mouseButtons();
    QMouseEvent mouseEvent(QEvent::MouseButtonRelease, 
        mapFromGlobal(QCursor::pos()),
        QCursor::pos(), 
        button,
        buttons,
        qApp->keyboardModifiers());
    mouseReleaseEvent(&mouseEvent);
    return mouseEvent.isAccepted();
}

bool GraphicControl::event(QEvent* e)
{
    switch(e->type())
    {
    case QEvent::KeyPress:
        {
            QKeyEvent *ke = static_cast<QKeyEvent *>(e);
            if (ke->key() == Qt::Key_Tab || ke->key() == Qt::Key_Backtab)
            {
                keyPressEvent(ke);
                ke->accept();
                return true;
            }
        }
        break;
    case QEvent::WindowBlocked:
        {
            if (m_pressedGP != Q_NULL)
                m_pressedGP->windowBlockedEvent(e);
        }
        break;
    case QEvent::WindowUnblocked:
        {
            if (m_pressedGP != Q_NULL)
                m_pressedGP->windowUnBlockedEvent(e);
        }
        break;

    case QEvent::Timer:
        {
            if (static_cast<QTimerEvent *>(e)->timerId() == m_keyReleaseTimer.timerId())
            {
                Qt::MouseButtons buttons = qApp->mouseButtons();
                if (!(buttons & Qt::LeftButton))
                {
                    emulateMouseReleaseEvent(Qt::NoButton);
                    e->accept();
                }
                return true;
            }
        }
        break;

    default:
        break;
    }

    return QWidget::event(e);
}

void GraphicControl::keyPressEvent(QKeyEvent* e)
{
    if (m_focusedGP != Q_NULL)
    {
        GraphicPrimitive* gp = m_focusedGP;
        e->ignore();
        while (gp)
        {
            gp->keyPressEvent(e);
            if (e->isAccepted())
                break;
            gp = gp->parentGP();
        }
    }
}

void GraphicControl::keyReleaseEvent(QKeyEvent* e)
{
    if (m_focusedGP != Q_NULL)
    {
        GraphicPrimitive* gp = m_focusedGP;
        e->ignore();
        while (gp)
        {
            gp->keyReleaseEvent(e);
            if (e->isAccepted())
                break;
            gp = gp->parentGP();
        }
    }
}

void GraphicControl::dragEnterEvent(QDragEnterEvent* event)
{
    if (m_dragDropState == dragActive)
    {
        GridNativeDragObject* nobj = qobject_cast<GridNativeDragObject *>(m_dragDropObject);
        if (nobj != Q_NULL)
        {
            nobj->setMimeData((QMimeData *)event->mimeData());
            nobj->setDropAction(event->dropAction());
        }
        event->accept();
        return;
    }

    Q_ASSERT(m_dragDropObject == Q_NULL);
    m_dragDropObject = createNativeDragDropObject(event->pos());
    if (m_dragDropObject != Q_NULL)
    {
        GridNativeDragObject* nobj = qobject_cast<GridNativeDragObject *>(m_dragDropObject);
        if (nobj != Q_NULL)
        {
            nobj->setMimeData((QMimeData *)event->mimeData());
            nobj->setDropAction(event->dropAction());
        }

        m_dragDropObjectOwner = true;
        m_dragDropState = dragActive;
        GraphicPrimitive* target = hitTest(event->pos());
        m_dragDropObject->setTarget(target);
        m_dragDropObject->beginEvent();
        event->accept();
    }
    else
        event->ignore();
}

void GraphicControl::dragMoveEvent(QDragMoveEvent* event)
{
    event->ignore();

    GridNativeDragObject* nobj = qobject_cast<GridNativeDragObject *>(m_dragDropObject);
    if (nobj != Q_NULL)
    {
        nobj->setMimeData((QMimeData *)event->mimeData());
        nobj->setDropAction(event->dropAction());
    }

    if (m_dragDropObject != Q_NULL)
    {
        moveDragDropObject(event->pos(), m_dragDropObject);
        if (m_dragDropObject->isAccepted())
            event->accept();
    }
}

void GraphicControl::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->ignore();

    GridNativeDragObject* nobj = qobject_cast<GridNativeDragObject *>(m_dragDropObject);
    if (nobj != Q_NULL)
    {
        nobj->setMimeData(Q_NULL);
        nobj->setDropAction(Qt::IgnoreAction);
    }

    if (m_dragDropObject != Q_NULL)
    {
        m_dragDropObject->setTarget(Q_NULL);
        m_dragDropObject->moveEvent(QPoint(-1,-1));
        if (m_dragDropObjectOwner)
        {
            m_dragDropObjectOwner = false;
            m_dragDropObject->endEvent();
            Q_DELETE_AND_NULL(m_dragDropObject);
            m_dragDropState = dragNone;
            event->accept();
        }
    }
}

void GraphicControl::dropEvent(QDropEvent* event)
{
    event->ignore();
    if (m_dragDropObject != Q_NULL)
    {
        if (m_dragDropObject->isAccepted())
        {
            GraphicPrimitive* target = m_dragDropObject->target();
            if (target != Q_NULL)
            {
                QRect r = target->translateToSiteRect(target->rect());
                QPoint pos(event->pos().x() - r.x(), event->pos().y() - r.y());
                target->acceptDragDropEvent(pos, m_dragDropObject);
            }
            m_dragDropObject->acceptEvent();
        }

        m_dragDropObjectOwner = false;
        m_dragDropObject->endEvent();
        Q_DELETE_AND_NULL(m_dragDropObject);
        m_dragDropState = dragNone;

        event->accept();
    }
}

void GraphicControl::contextMenuEvent(QContextMenuEvent* event)
{
    GraphicPrimitive* target = hitTest(event->pos());
    if (target != Q_NULL)
    {
        QRect r = target->translateToSiteRect(target->rect());
        target->showContextMenuEvent(QPoint(event->pos().x() - r.x(), event->pos().y() - r.y()), event);
    }
}

/* GridDragDropObject */
GridDragDropObject::GridDragDropObject(GraphicPrimitive* source)
    : QObject(Q_NULL), m_source(source), m_target(Q_NULL), m_immediateStart(false),
    m_cursor(QCursor(Qt::ForbiddenCursor)), m_acceptedCursor(QCursor(Qt::ArrowCursor))
{
}

GridDragDropObject::~GridDragDropObject()
{
    m_source = Q_NULL;
    m_target = Q_NULL;
}

bool GridDragDropObject::getImmediateStart()
{
    return m_immediateStart;
}

void GridDragDropObject::setImmediateStart(bool immediateStart)
{
    m_immediateStart = immediateStart;
}

const QCursor& GridDragDropObject::cursor() const
{
    return m_cursor;
}

void GridDragDropObject::setCursor(const QCursor& cursor)
{
    m_cursor = cursor;
}

void GridDragDropObject::setDefaultCursor()
{
    m_cursor = QCursor(Qt::ForbiddenCursor);
}

const QCursor& GridDragDropObject::acceptedCursor() const
{
    return m_acceptedCursor;
}

void GridDragDropObject::setAcceptedCursor(const QCursor& cursor)
{
    m_acceptedCursor = cursor;
}

void GridDragDropObject::setDefaultAcceptedCursor()
{
    m_acceptedCursor = QCursor(Qt::ArrowCursor);
}

void GridDragDropObject::beginEvent()
{
    QApplication::setOverrideCursor(cursor());
}

void GridDragDropObject::moveEvent(const QPoint& /*pos*/)
{
    if (isAccepted())
        QApplication::changeOverrideCursor(acceptedCursor());
    else
        QApplication::changeOverrideCursor(cursor());
}

void GridDragDropObject::acceptEvent()
{
    Q_ASSERT(isAccepted());
}

void GridDragDropObject::endEvent()
{
    QApplication::restoreOverrideCursor();
}

void GridDragDropObject::paintEvent(QPainter& painter)
{
    Q_UNUSED(painter);
}

GraphicPrimitive* GridDragDropObject::source() const
{
    return m_source;
}

GraphicPrimitive* GridDragDropObject::target() const
{
    return m_target;
}

QPoint GridDragDropObject::cursorPos() const
{
    return QCursor::pos();
}

void GridDragDropObject::setTarget(GraphicPrimitive* target)
{
    m_target = target;
}

bool GridDragDropObject::isAccepted() const
{
    return target() != Q_NULL;
}

/* GridNativeDragObject */
GridNativeDragObject::GridNativeDragObject(GraphicControl* _control, GraphicPrimitive* source)
    : GridDragDropObject(source), m_control(_control), m_data(Q_NULL), m_dropAction(Qt::IgnoreAction)
{
}

QMimeData* GridNativeDragObject::mimeData() const
{
    return m_data;
}

void GridNativeDragObject::setMimeData(QMimeData* data)
{
    m_data = data;
}

Qt::DropAction GridNativeDragObject::dropAction() const
{
    return m_dropAction;
}

void GridNativeDragObject::setDropAction(Qt::DropAction action)
{
    m_dropAction = action;
}


GraphicControl* GridNativeDragObject::control() const
{
    return m_control;
}

/* GraphicPrimitive */
GraphicPrimitive::GraphicPrimitive(GraphicControl* control, 
                                   GraphicPrimitive* parent)
: QObject(Q_NULL),
  m_borderPen(Qt::black, 0, Qt::SolidLine),
  m_graphicMask(this, QRect(0, 0, 0, 0)),
  m_fixedState(false, false),
  m_masked(false), m_childrenDrawable(true), m_airSupported(false)
{
    init(control, parent);
}

GraphicPrimitive::GraphicPrimitive(GraphicPrimitive* parent)
: QObject(Q_NULL), m_borderPen(Qt::black, 0, Qt::SolidLine),
  m_graphicMask(this, QRect(0, 0, 0, 0)),
  m_fixedState(false, false), m_masked(false), m_childrenDrawable(true)
{ 
    init(parent->control(), parent);
}

void GraphicPrimitive::deleteChildren()
{
    while (m_childrenGP.size() != 0)
    {
        GraphicPrimitive* gp = *m_childrenGP.begin();
        delete gp;
    }
    Q_ASSERT(m_childrenGP.size() == 0);
}

void GraphicPrimitive::setRect(const QRect& rect)
{ 
    if (m_rect == rect)
        return;
    m_rect = rect;
    resizeEvent(m_rect);
}

void GraphicPrimitive::setLeft(int left)
{
    if (m_rect.left() == left)
        return;
    m_rect.moveLeft(left);
    resizeEvent(m_rect);
}

void GraphicPrimitive::setTop(int top)
{ 
    if (m_rect.top() == top)
        return;
    m_rect.moveTop(top);
    resizeEvent(m_rect);
}

void GraphicPrimitive::setWidth(int width)
{ 
    if (m_rect.width() == width)
        return;
    m_rect.setWidth(width);
    resizeEvent(m_rect);
}

void GraphicPrimitive::setHeight(int height)
{ 
    if (m_rect.height() == height)
        return;
    m_rect.setHeight(height);
    resizeEvent(m_rect);
}

GraphicPrimitive::~GraphicPrimitive()
{    
    setParentGP(Q_NULL);
    deleteChildren();
    Q_ASSERT(m_control != Q_NULL);
    m_control->deleteGPNotify(this);
}

GraphicControl* GraphicPrimitive::control() const
{
    return m_control;
}

int GraphicPrimitive::id() const
{
    return m_id;
}

void GraphicPrimitive::setId(int id)
{
    m_id = id;
}

void GraphicPrimitive::init(GraphicControl* control, GraphicPrimitive* parent)
{
    m_parentGP = Q_NULL;
    setParentGP(parent);
    m_control = control;
    m_id = 0;
    m_rect = QRect(0, 0, 100, 50);
    m_visible = true;
    m_opacity = 1.0;
    m_isUpdating = false;
    m_airSupported = false;
    m_isLeftButtonPressed = false;
    m_isLeftButtonDblClicked = false;
    m_transparent = false;
    m_zIndex = 0;
    m_borderLeft = 0;
    m_borderRight = 0;
    m_borderTop = 0;
    m_borderBottom = 0;
    m_graphicMask.clear();
}

void GraphicPrimitive::setBorder(int width)
{
    setBorder(width, width, width, width);
}

void GraphicPrimitive::setBorder(int left, int right, int top, int bottom)
{
    setBorderLeft(left);
    setBorderRight(right);
    setBorderTop(top);
    setBorderBottom(bottom);
}

void GraphicPrimitive::setBorderLeft(int left)
{
    m_borderLeft = left;
}

int GraphicPrimitive::borderLeft() const
{
    return m_borderLeft;
}

void GraphicPrimitive::setBorderRight(int right)
{
    m_borderRight = right;
}

int GraphicPrimitive::borderRight() const
{
    return m_borderRight;
}

void GraphicPrimitive::setBorderTop(int top)
{
    m_borderTop = top;
}

int GraphicPrimitive::borderTop() const
{
    return m_borderTop;
}

void GraphicPrimitive::setBorderBottom(int bottom)
{
    m_borderBottom = bottom;
}

int GraphicPrimitive::borderBottom() const
{
    return m_borderBottom;
}

QColor GraphicPrimitive::borderColor() const
{
    return m_borderPen.color();
}

void GraphicPrimitive::setBorderColor(const QColor& color)
{
    m_borderPen.setColor(color);
}

const QPen& GraphicPrimitive::borderPen() const
{
    return m_borderPen;
}

void GraphicPrimitive::setBorderPen(const QPen& pen)
{
    m_borderPen = pen;
}

bool GraphicPrimitive::isMouseOver() const
{
    if (m_control == Q_NULL)
        return false;

    return m_control->m_overGP == this;
}

bool GraphicPrimitive::isMouseOverRecursive() const
{
    if (m_control == Q_NULL)
        return false;
    GraphicPrimitive* gp = m_control->m_overGP;
    while (gp != Q_NULL)
    {
        if (gp == this)
            return true;
        gp = gp->parentGP();
    }
    return false;
}

QBrush GraphicPrimitive::borderBrush() const
{
    return m_borderPen.brush();
}

void GraphicPrimitive::setBorderBrush(const QBrush& brush)
{
    m_borderPen.setBrush(brush);
}

QRect GraphicPrimitive::viewRect() const
{
    return rect();
}

void GraphicPrimitive::setVisibility(bool visible)
{
    m_visible = visible;
    m_isUpdating = false;
}

bool GraphicPrimitive::visibility() const
{
    return m_visible;
}

void GraphicPrimitive::setOpacity(double value)
{
    m_opacity = value;
}

double GraphicPrimitive::opacity() const
{
    return m_opacity;
}

void GraphicPrimitive::setParentGP(GraphicPrimitive* parent)
{
    GraphicPrimitive* oldParent = parentGP();
    if (oldParent == parent)
        return;

    if (oldParent != Q_NULL)
    {
        Q_ASSERT(oldParent->m_childrenGP.indexOf(this) != -1);
        oldParent->m_childrenGP.remove(oldParent->m_childrenGP.indexOf(this));
    }
    
    if (parent != Q_NULL)
    {
        Q_ASSERT(parent->m_childrenGP.indexOf(this) == -1);
        parent->m_childrenGP.push_back(this);
    }
    
    m_parentGP = parent;
}

const GraphicPrimitive::GraphicChildren& GraphicPrimitive::childrenGP() const
{
    return m_childrenGP;
}

bool GraphicPrimitive::isDragging() const
{
    return m_control->m_dragDropObject != Q_NULL && m_control->m_dragDropObject->source() == this && m_control->m_dragDropState == dragActive;
}

GraphicPrimitive* GraphicPrimitive::parentGP() const
{
    return m_parentGP;
}

//GridDragDropObject* GraphicPrimitive::getDragDropObject()
//{
//    return m_dragDropObject;
//}

QCursor GraphicPrimitive::cursor() const
{
    return m_cursor;
}

void GraphicPrimitive::setCursor(const QCursor& cursor)
{
    m_cursor = cursor;
}

void GraphicPrimitive::paint(int gx, int gy, int parentScrollX, int parentScrollY,
                                 double opacity, const QRect& parentRect, GraphicPaintOrder& paintOrder)
{
    if (!visibility())
        return;

    QtnPaintData pdata;
    pdata.clip = parentRect.intersected(rect().translated(gx - parentScrollX, gy - parentScrollY));

    if (pdata.clip.isEmpty())
        return;

    pdata.gp = this;
    pdata.opacity = opacity * this->opacity();

    ScrollableGraphicPrimitive* scrollElement = Q_NULL;
    if (scrollable())
        scrollElement = static_cast<ScrollableGraphicPrimitive *>(this);
    int scrollX = 0;
    int scrollY = 0;
    int rightOffset = 0;
    int bottomOffset = 0;
    if (scrollElement)
    {
        scrollX = scrollElement->scrollX();
        scrollY = scrollElement->scrollY();
        rightOffset = scrollElement->isVerScrollBarVisible() ? scrollElement->scrollVer()->width() : 0;
        bottomOffset = scrollElement->isHorScrollBarVisible() ? scrollElement->scrollHor()->height() : 0;
    }

    pdata.gx = gx + rect().left() - parentScrollX;
    pdata.gy = gy + rect().top() - parentScrollY;
    paintOrder.insert(zIndex(), pdata);

    if (!childrenDrawable())
        return;

    const GraphicChildren& list = childrenGP();
    for (GraphicChildren::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        GraphicPrimitive* element = *it;
        Q_ASSERT(element != Q_NULL);
        if (!element->visibility())
            continue;
        QRect intersect;
        if (mask().isGPMasked(element))
            intersect = pdata.clip.intersected(
                mask().rect().translated(pdata.gx + m_borderLeft, pdata.gy + m_borderTop));
        else
            intersect = pdata.clip;

        intersect = rect().translated(gx - parentScrollX, gy - parentScrollY).adjusted(
            m_borderLeft, m_borderTop, -(m_borderRight + rightOffset), -(m_borderBottom + bottomOffset)).intersected(intersect);
        if (!intersect.isEmpty())            
            element->paint(pdata.gx + m_borderLeft, pdata.gy + m_borderTop, 
                        (scrollElement && scrollElement->isGPFixedX(element) ? 0 : scrollX),
                        (scrollElement && scrollElement->isGPFixedY(element) ? 0 : scrollY),
                        pdata.opacity, intersect, paintOrder);
    }
}

void GraphicPrimitive::print(QPainter& painter)
{
    GraphicPaintOrder paintOrder;

    QRect parentRect;
    if (parentGP() == Q_NULL)
        parentRect = rect();
    else if (parentGP()->scrollable())
        parentRect = static_cast<ScrollableGraphicPrimitive *>(parentGP())->viewRect();
    else
        parentRect = QRect(QPoint(0, 0), parentGP()->rect().size());

    paint(0, 0, 0, 0, control()->windowOpacity(), parentRect, paintOrder);

    for (GraphicPaintOrder::const_iterator it = paintOrder.begin(); it != paintOrder.end(); ++it)
    {
        const QtnPaintData& pdata = it.value();
        painter.save();
#if 0
        if (pdata.clip.width() != pdata.gp->rect().width() || 
            pdata.clip.height() != pdata.gp->rect().height())
#endif
        painter.setClipRect(pdata.clip.translated(-left(), -top()), Qt::IntersectClip);
        painter.translate(pdata.gx - left(), pdata.gy - top());
        if (pdata.opacity != 1.0)
            painter.setOpacity(pdata.opacity);
        pdata.gp->paintEvent(painter);
        pdata.gp->drawBorder(painter);
        painter.restore();
    }
}

bool GraphicPrimitive::scrollable() const
{
    return false;
}

bool GraphicPrimitive::embeded() const
{
    return false;
}

bool GraphicPrimitive::childrenDrawable()
{
    return m_childrenDrawable;
}

void GraphicPrimitive::setChildrenDrawable(bool drawable)
{
    m_childrenDrawable = drawable;
}

void GraphicPrimitive::wheelEvent(const QPoint& , QWheelEvent *)
{
}

void GraphicPrimitive::paintEvent(QPainter& painter)
{
    Q_UNUSED(painter);
}

void GraphicPrimitive::resizeEvent(const QRect& rect)
{
    Q_UNUSED(rect);
}

void GraphicPrimitive::focusInEvent(QFocusEvent *)
{
}

void GraphicPrimitive::focusOutEvent(QFocusEvent *)
{
}

void GraphicPrimitive::mouseMoveEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
}

void GraphicPrimitive::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
}

void GraphicPrimitive::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
}

void GraphicPrimitive::mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e)
{
    Q_UNUSED(point);
    Q_UNUSED(e);
}

void GraphicPrimitive::mouseOverEvent(const QPoint& pos)
{
    Q_UNUSED(pos);
}

void GraphicPrimitive::mouseOutEvent()
{
}

void GraphicPrimitive::keyPressEvent(QKeyEvent* e)
{
    Q_UNUSED(e);
}

void GraphicPrimitive::keyReleaseEvent(QKeyEvent* e)
{
    Q_UNUSED(e);
}

void GraphicPrimitive::windowBlockedEvent(QEvent* e)
{
    Q_UNUSED(e);
}

void GraphicPrimitive::windowUnBlockedEvent(QEvent* e)
{
    Q_UNUSED(e);
}

bool GraphicPrimitive::checkDragDropEvent(const QPoint& pos, GridDragDropObject* object)
{
    Q_UNUSED(pos);
    Q_UNUSED(object);
    return false;
}

void GraphicPrimitive::acceptDragDropEvent(const QPoint& pos, GridDragDropObject* object)
{   
    Q_UNUSED(object);
    Q_UNUSED(pos);
    Q_ASSERT(object != Q_NULL);
}

void GraphicPrimitive::endDragDropEvent()
{
}

void GraphicPrimitive::showContextMenuEvent(const QPoint& pos, QContextMenuEvent* event)
{
    if (parentGP() != Q_NULL)
        parentGP()->showContextMenuEvent(pos, event);
}

GraphicPrimitive* GraphicPrimitive::hitTest(const QPoint& point, int gx, int gy, int parentScrollX, int parentScrollY, const QRect& parentRect, int& zindex)
{
    GraphicPrimitive* retval = Q_NULL;
    QRect clip = parentRect.intersected(rect().translated(gx - parentScrollX, gy - parentScrollY));

    if (visibility() && !transparent() && clip.contains(point) && zIndex() >= zindex)
    {
        retval = this;
        zindex = zIndex();
    }

    if (retval == Q_NULL)
        return retval;

    ScrollableGraphicPrimitive* scrollElement = Q_NULL;
    if (scrollable())
        scrollElement = static_cast<ScrollableGraphicPrimitive *>(this);

    int scrollX = 0;
    int scrollY  = 0;
    int rightOffset = 0;
    int bottomOffset = 0;
    if (scrollElement)
    {
        scrollX = scrollElement->scrollX();
        scrollY = scrollElement->scrollY();
        rightOffset = scrollElement->isVerScrollBarVisible() ? scrollElement->scrollVer()->width() : 0;
        bottomOffset = scrollElement->isHorScrollBarVisible() ? scrollElement->scrollHor()->height() : 0;
    }

    int cur_gx = gx + rect().left() - parentScrollX;
    int cur_gy = gy + rect().top() - parentScrollY;
    const GraphicChildren& list = childrenGP();
    for (GraphicChildren::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        GraphicPrimitive* element = *it;
        Q_ASSERT(element != Q_NULL);
        if (!element->visibility())
            continue;
        QRect intersect;
        if (mask().isGPMasked(element))
            intersect = clip.intersected(
            mask().rect().translated(cur_gx + borderLeft(), cur_gy + borderTop()));
        else
            intersect = clip;

        intersect = rect().translated(gx - parentScrollX, gy - parentScrollY).adjusted(
            borderLeft(), borderTop(), -(rightOffset + borderRight()), -(bottomOffset + borderBottom())).intersected(intersect);
        if (!intersect.isEmpty())  
        {
            GraphicPrimitive* temp = element->hitTest(point, cur_gx + borderLeft(), cur_gy + borderTop(), 
                        (scrollElement && scrollElement->isGPFixedX(element) ? 0 : scrollX),
                        (scrollElement && scrollElement->isGPFixedY(element) ? 0 : scrollY),
                        intersect, zindex);
            if (temp)
                retval = temp;

        }
    }
    return retval;
}

bool GraphicPrimitive::checkClickEvent(QMouseEvent* e, bool dblClick)
{
    if (!dblClick)
        return isMouseOverRecursive() && control()->dragDropState() != dragActive && e->button() == Qt::LeftButton && !isLeftButtonDblClicked();
    else
        return isMouseOverRecursive() && control()->dragDropState() != dragActive && e->button() == Qt::LeftButton;
}

static QRect translate_to_site_rect(const GraphicControl* control,
                                    const GraphicPrimitive* element, 
                                    const QRect& rect, bool to_site, bool viewport)
{    
    QRect retval = rect;
    if (viewport)
    {
        const ScrollableGraphicPrimitive* scrollElement = Q_NULL;
        if (element->scrollable())
            scrollElement = static_cast<const ScrollableGraphicPrimitive*>(element);
        if (scrollElement)
        {
            if (to_site)
                retval.translate(-scrollElement->scrollX(), -scrollElement->scrollY());
            else
                retval.translate(scrollElement->scrollX(), scrollElement->scrollY());
        }
    }

    while (element)
    {
        GraphicPrimitive* parent = element->parentGP();
        
        int parentX = parent != Q_NULL ?  parent->rect().x() + parent->borderLeft() : control->rect().x();
        int parentY = parent != Q_NULL ?  parent->rect().y() + parent->borderTop() : control->rect().y();

        if (to_site)
            retval.translate(parentX, parentY);
        else
            retval.translate(-parentX, -parentY);
        if (parent)
        {
            const ScrollableGraphicPrimitive* scrollElement = Q_NULL;
            if (parent->scrollable())
                scrollElement = static_cast<const ScrollableGraphicPrimitive*>(parent);
            int scrollX = scrollElement && !scrollElement->isGPFixedX(element) ? scrollElement->scrollX() : 0;
            int scrollY = scrollElement && !scrollElement->isGPFixedY(element) ? scrollElement->scrollY() : 0;
            if (scrollX || scrollY)
            {
                if (to_site)
                    retval.translate(-scrollX, -scrollY);
                else
                    retval.translate(scrollX, scrollY);
            }
        }
        element = parent;
    }
    return retval;
}

QRect GraphicPrimitive::translateToSiteRect(const QRect& rect) const
{
    return translate_to_site_rect(m_control, this, rect, true, true);
}

QRect GraphicPrimitive::translateFromSiteRect(const QRect& rect) const
{
    return translate_to_site_rect(m_control, this, rect, false, true);
}

QPoint GraphicPrimitive::mapViewportToSite(const QPoint& point) const
{
    QRect temp = rect();
    temp.adjust(point.x(), point.y(), 0, 0);
    temp = translate_to_site_rect(m_control, this, temp, true, true);
    return temp.topLeft();
}

QPoint GraphicPrimitive::mapViewportFromSite(const QPoint& point) const
{
    QRect temp = QRect(point.x(), point.y(), 1, 1);
    temp = translate_to_site_rect(m_control, this, temp, false, true);
    return temp.topLeft();
}

QPoint GraphicPrimitive::mapViewportToGlobal(const QPoint& point) const
{
    return control()->mapToGlobal(mapViewportToSite(point));
}

QPoint GraphicPrimitive::mapViewportFromGlobal(const QPoint& point) const
{
    return mapViewportFromSite(control()->mapFromGlobal(point));
}

QPoint GraphicPrimitive::mapToSite(const QPoint& point) const
{
    QRect temp = rect();
    temp.adjust(point.x() + borderLeft(), point.y() + borderTop(), 0, 0);
    temp = translate_to_site_rect(m_control, this, temp, true, false);
    return temp.topLeft();
}

QPoint GraphicPrimitive::mapFromSite(const QPoint& point) const
{
    QRect temp = QRect(point.x() - rect().x() - borderLeft(), point.y() - rect().y() - borderTop(), 0, 0);
    temp = translate_to_site_rect(m_control, this, temp, false, false);
    return temp.topLeft();
}

QPoint GraphicPrimitive::mapToGlobal(const QPoint& point) const
{
    return control()->mapToGlobal(mapToSite(point));
}

QPoint GraphicPrimitive::mapFromGlobal(const QPoint& point) const
{
    return mapFromSite(control()->mapFromGlobal(point));
}

GridGraphicMask& GraphicPrimitive::mask()
{
    return m_graphicMask;
}

bool GraphicPrimitive::transparent() const
{
    return m_transparent;
}

void GraphicPrimitive::setTransparent(bool transparent)
{
    m_transparent = transparent;
}

int GraphicPrimitive::zIndex() const
{
    if (m_zIndex <= 0 && m_parentGP != Q_NULL)
        return m_parentGP->zIndex() + 1;
    else
        return m_zIndex;
}

void GraphicPrimitive::setZIndex(int zIndex)
{
    m_zIndex = zIndex;
}

void GraphicPrimitive::update(const QRect& rect)
{
    if (control()->isLocked())
        return;

    control()->update(QRect(mapToSite(rect.topLeft()), rect.size()).adjusted(-borderLeft(), -borderTop(), 0, 0));
#ifndef _DEBUG
#ifdef _DEBUG_UPDATE
    static int s_updateCount = 0;
    QRect updaterect = QRect(mapToSite(rect.topLeft()), rect.size());
    QString s = QString("[update: %1]left = %2, top = %3, width = %4, height = %5\n").arg(s_updateCount).arg(updaterect.left()).arg(updaterect.top()).arg(updaterect.width()).arg(updaterect.height());
    qDebug() << s;
    s_updateCount++;
#endif
#endif
}

void GraphicPrimitive::update()
{
    if (isUpdating())
        return;
    m_isUpdating = true;
    update(QRect(QPoint(0, 0), rect().size()));
}

void GraphicPrimitive::repaint(const QRect& rect)
{
    control()->repaint(QRect(mapToSite(rect.topLeft()), rect.size()));
}

void GraphicPrimitive::repaint()
{
    repaint(boundedRect());
}

void GraphicPrimitive::updateLayout()
{
    const GraphicChildren& list = childrenGP();
    for (GraphicChildren::const_iterator it = list.begin(); it != list.end(); ++it)
    {
        GraphicPrimitive* element = *it;
        Q_ASSERT(element != Q_NULL);
        element->updateLayout();
    }
}

bool GraphicPrimitive::isUpdating() const
{
    return m_isUpdating;
}

bool GraphicPrimitive::airSupported() const
{
#ifdef Q_OS_WIN
    return m_airSupported && isAirAvailable();
#else
    return false;
#endif
}

void GraphicPrimitive::setAirSupported(bool b)
{
    m_airSupported = b;
}

bool GraphicPrimitive::visibleOnScreen() const
{
    if (!visibility())
        return false;
    GraphicPrimitive* p = parentGP();
    while (p != Q_NULL)
    {
        if (!p->visibility())
            return false;
        p = p->parentGP();
    }
    return true;
}

void GraphicPrimitive::setFocus()
{
    control()->setFocusGP(this);
}

bool GraphicPrimitive::hasFocus() const
{
    return control()->focusedGP() == this;
}

bool GraphicPrimitive::isLayoutChanged() const
{
    return m_layoutChanged;
}

void GraphicPrimitive::layoutChanged()
{
    m_layoutChanged = true;
}

void GraphicPrimitive::drawBorderPrimitive(QPainter& painter, QPen pen, const QRect& rect, const QRect& clip,
                                               int borderLeft, int borderRight, int borderTop, int borderBottom)
{
    if (borderLeft == 0 && borderRight == 0 && borderTop == 0 && borderBottom == 0)
        return;

    QPen old = painter.pen();
    int penWidth = 0;

    if (borderLeft > 0)
    {
        pen.setWidth(borderLeft);
        painter.setPen(pen);
        penWidth = borderLeft;
        QLineF l(rect.left() + penWidth / 2,
                 rect.top() + penWidth / 2, 
                 rect.left() + penWidth / 2,
                 rect.top() + rect.height() - (penWidth + 1) / 2);
        if (intersect_line(l, clip))
            draw_line(painter, l);
    }

    if (borderRight > 0)
    {
        if (borderRight != penWidth)
        {
            pen.setWidth(borderRight);
            painter.setPen(pen);
            penWidth = borderRight;
        }
        QLineF l(rect.left() + rect.width() - (penWidth + 1) / 2,
                 rect.top() + penWidth / 2, 
                 rect.left() + rect.width() - (penWidth + 1) / 2,
                 rect.top() + rect.height() - (penWidth + 1) / 2);

        if (intersect_line(l, clip))
            draw_line(painter, l);
    }

    if (borderTop > 0)
    {
        if (borderTop != penWidth)
        {
          pen.setWidth(borderTop);
          painter.setPen(pen);
          penWidth = borderTop; 
        }
        QLineF l(rect.left() + penWidth / 2,
                 rect.top() + penWidth / 2,
                 rect.left() + rect.width() - (penWidth + 1) / 2,
                 rect.top() + penWidth / 2);
        if (intersect_line(l, clip))
            draw_line(painter, l);
    }

    if (borderBottom > 0)
    {
        if (borderBottom != penWidth)
        {
            pen.setWidth(borderBottom);
            painter.setPen(pen);
            penWidth = borderBottom;
        }
        QLineF l(rect.left() + penWidth / 2,
                 rect.top() + rect.height() - (penWidth + 1) / 2,
                 rect.left() + rect.width() - (penWidth + 1) / 2,
                 rect.top() + rect.height() - (penWidth + 1) / 2);
        if (intersect_line(l, clip))
            draw_line(painter, l);
    } 
    painter.setPen(old);
}

void GraphicPrimitive::drawBorder(QPainter& painter)
{
    QRect r(QPoint(0, 0), rect().size());
    drawBorderPrimitive(painter, m_borderPen, r, r, m_borderLeft, m_borderRight, m_borderTop, m_borderBottom);
}

QRect GraphicPrimitive::boundedRect() const
{
    return QRect(borderLeft(),
                 borderTop(), 
                 width() - (borderLeft() +  borderRight()),
                 height() - (borderTop() + borderBottom()));
}

GridDragDropObject* GraphicPrimitive::createDragDropObject(const QPoint&)
{
    return Q_NULL;
}

/* ScrollableGraphicPrimitive */
ScrollableGraphicPrimitive::ScrollableGraphicPrimitive(GraphicControl* control,
                                                       GraphicPrimitive* parent)
: GraphicPrimitive(control, parent), 
  m_scrollX(0), m_scrollY(0), 
  m_overflowX(gsoNone), m_overflowY(gsoNone), m_viewSize(m_rect.size()), m_updateOnScrolling(false)
{
    initGP();
}

ScrollableGraphicPrimitive::ScrollableGraphicPrimitive(GraphicPrimitive* parent)
    : GraphicPrimitive(parent->control(), parent),
      m_scrollX(0), m_scrollY(0),
      m_overflowX(gsoNone), m_overflowY(gsoNone), m_viewSize(m_rect.size()), m_updateOnScrolling(false)
{
    initGP();
}

void ScrollableGraphicPrimitive::initGP()
{
    m_verScrollBarVisible = false;
    m_horScrollBarVisible = false;
    m_scrollVer2 = Q_NULL;
    m_scrollHor2 = Q_NULL;
}

ScrollableGraphicPrimitive::~ScrollableGraphicPrimitive()
{
    Q_DELETE_AND_NULL(m_scrollVer2);
    Q_DELETE_AND_NULL(m_scrollHor2);
}

QScrollBar* ScrollableGraphicPrimitive::scrollVer()
{ 
    if (m_scrollVer2 == Q_NULL)
    {
        m_scrollVer2 = new GraphicPrimitiveScrollBar(Qt::Vertical, this);
        m_scrollVer2->setVisible(false);
        QObject::connect(m_scrollVer2, SIGNAL(valueChanged(int)), this, SLOT(scrollVerChanged(int)));
    }

    return m_scrollVer2; 
}

QScrollBar* ScrollableGraphicPrimitive::scrollHor()
{
    if (m_scrollHor2 == Q_NULL)
    {
        m_scrollHor2 = new GraphicPrimitiveScrollBar(Qt::Horizontal, this);
        m_scrollHor2->setVisible(false);
        QObject::connect(m_scrollHor2, SIGNAL(valueChanged(int)), this, SLOT(scrollHorChanged(int)));
    }
    return m_scrollHor2;
}

void ScrollableGraphicPrimitive::updateScrollBars()
{
    int scrollXShowing = -1;
    if (m_overflowX == gsoVisible)
        scrollXShowing = 1;
    if (m_overflowX == gsoNone)
        scrollXShowing = 0;

    int scrollYShowing = -1;
    if (m_overflowY == gsoVisible)
        scrollYShowing = 1;
    if (m_overflowY == gsoNone)
        scrollYShowing = 0;

    QSize minSize = minBoundedSize();

    if (scrollXShowing == -1 && viewSize().width() <= minSize.width())
        scrollXShowing = 0;
    else if (scrollXShowing == -1 && viewSize().width() > minSize.width() + scrollVer()->sizeHint().width())
        scrollXShowing = 1;

    if (scrollYShowing == -1 && viewSize().height() <= minSize.height())
        scrollYShowing = 0;
    else if (scrollYShowing == -1 && viewSize().height() > minSize.height() + scrollHor()->sizeHint().height())
        scrollYShowing = 1;

    if (scrollXShowing == -1 && scrollYShowing == -1)
    {
        scrollXShowing = 0;
        scrollYShowing = 0;
    }
    else if (scrollXShowing == -1)
        scrollXShowing = scrollYShowing;
    else if (scrollYShowing == -1)
        scrollYShowing = scrollXShowing;

    setHorScrollBarVisible(scrollXShowing == 1);
    setVerScrollBarVisible(scrollYShowing == 1);

    setScrollBarsPosition();
    QRect bounded = boundedRect();
    int rangeX = viewSize().width() - bounded.width();
    if (rangeX < 0)
        rangeX = 0;

    int rangeY = viewSize().height() - bounded.height();
    if (rangeY < 0)
        rangeY = 0;

    scrollHor()->setRange(0, rangeX);
    scrollHor()->setPageStep(bounded.width());
    scrollVer()->setRange(0, rangeY);
    scrollVer()->setPageStep(bounded.height());
}

void ScrollableGraphicPrimitive::setScrollBarsPosition()
{
    bool horscbar = isHorScrollBarVisible() && visibleOnScreen();
    bool verscbar = isVerScrollBarVisible() && visibleOnScreen();
    scrollHor()->setVisible(horscbar);
    scrollVer()->setVisible(verscbar);

    int horHeight = isHorScrollBarVisible() ? scrollHor()->sizeHint().height() : 0;
    int verWidth = isVerScrollBarVisible() ? scrollVer()->sizeHint().width() : 0;

    if (horscbar)
    {
        QRect bounded = GraphicPrimitive::boundedRect();
        QRect scrollHorRect(
            0,
            bounded.height() - horHeight,
            bounded.width() - verWidth, 
            horHeight);
        QRect scrollHorRectT(mapToSite(scrollHorRect.topLeft()), scrollHorRect.size());
        scrollHor()->setGeometry(scrollHorRectT);
    }

    if (verscbar)
    {
        QRect bounded = GraphicPrimitive::boundedRect();
        QRect scrollVerRect(
            bounded.width() - verWidth,
            0, 
            verWidth, 
            bounded.height() - horHeight);
        QRect scrollVerRectT(mapToSite(scrollVerRect.topLeft()), scrollVerRect.size());
        scrollVer()->setGeometry(scrollVerRectT);
    }
}

int ScrollableGraphicPrimitive::scrollX() const
{ 
    return m_scrollX;
}

int ScrollableGraphicPrimitive::scrollY() const
{ 
    return m_scrollY;
}

void ScrollableGraphicPrimitive::setScrollX(int x)
{
    m_scrollX = x;
}

void ScrollableGraphicPrimitive::setScrollY(int y)
{
    m_scrollY = y;
}

void ScrollableGraphicPrimitive::setOverflow(ScrollOverflow overflow)
{
    if (m_overflowX == overflow && m_overflowY == overflow)
        return;
    m_overflowX = overflow;
    m_overflowY = overflow;
    updateScrollBars();
}

void ScrollableGraphicPrimitive::setOverflowX(ScrollOverflow overflow)
{
    if (m_overflowX == overflow)
        return;
    m_overflowX = overflow;
    updateScrollBars();
}

void ScrollableGraphicPrimitive::setOverflowY(ScrollOverflow overflow)
{
    if (m_overflowY == overflow)
        return;
    m_overflowY = overflow;
    updateScrollBars();
}

void ScrollableGraphicPrimitive::setFixedGPX(GraphicPrimitive* primitive, bool value)
{
    primitive->m_fixedState.x = value;
}

void ScrollableGraphicPrimitive::setFixedGPY(GraphicPrimitive* primitive, bool value)
{
    primitive->m_fixedState.y = value;
}

bool ScrollableGraphicPrimitive::isGPFixedX(const GraphicPrimitive* primitive)
{
    return primitive->m_fixedState.x;
}

bool ScrollableGraphicPrimitive::isGPFixedY(const GraphicPrimitive* primitive)
{
    return primitive->m_fixedState.y;
}

void ScrollableGraphicPrimitive::updateLayout()
{
    updateScrollBars();
    GraphicPrimitive::updateLayout();
}

bool ScrollableGraphicPrimitive::scrollable() const
{
    return true;
}

void ScrollableGraphicPrimitive::wheelEvent(const QPoint& pos, QWheelEvent* e)
{
    Q_UNUSED(pos);
    if (m_scrollVer2 != Q_NULL)
        static_cast<GraphicPrimitiveScrollBar *>(m_scrollVer2)->wheelEvent(e);
}

void ScrollableGraphicPrimitive::scrollXEvent(int position)
{
    if (isUpdateOnScrolling())
        update();
    Q_UNUSED(position);
}

void ScrollableGraphicPrimitive::scrollYEvent(int position)
{
    if (isUpdateOnScrolling())
        update();
    Q_UNUSED(position);
}

bool ScrollableGraphicPrimitive::canScrollX() const
{
    return true;
}

bool ScrollableGraphicPrimitive::canScrollY() const
{
    return true;
}

QSize ScrollableGraphicPrimitive::minBoundedSize()
{
    return QSize(m_rect.width() - borderLeft() - borderRight() - scrollVer()->sizeHint().width(),
        m_rect.height() - borderTop() - borderBottom() - scrollHor()->sizeHint().height());
}

const QSize& ScrollableGraphicPrimitive::viewSize() const
{
    return m_viewSize;
}

void ScrollableGraphicPrimitive::setViewSize(const QSize& size)
{
    if (m_viewSize == size)
        return;
    m_viewSize = size;
}

QRect ScrollableGraphicPrimitive::viewRect() const
{
    QRect retval = rect();
    if (viewSize().width() > retval.width())
        retval.setWidth(viewSize().width());
    if (viewSize().height() > retval.height())
        retval.setHeight(viewSize().height());
    return retval;
}

QRect ScrollableGraphicPrimitive::boundedRect() const
{
    QRect retval = GraphicPrimitive::boundedRect();
    int horHeight = isHorScrollBarVisible() ? m_scrollHor2->sizeHint().height() : 0;
    int verWidth = isVerScrollBarVisible() ? m_scrollVer2->sizeHint().width() : 0;
    if (horHeight > 0)
        retval.setHeight(retval.height() > horHeight ? retval.height() - horHeight : 0);
    if (verWidth > 0)
        retval.setWidth(retval.width() > verWidth ? retval.width() - verWidth : 0);
    return retval;
}

void ScrollableGraphicPrimitive::scrollVerChanged(int value)
{
    setScrollY(value);
    scrollYEvent(value);
}

void ScrollableGraphicPrimitive::scrollHorChanged(int value)
{
    setScrollX(value);
    scrollXEvent(value);
}

bool ScrollableGraphicPrimitive::isUpdateOnScrolling() const
{
    return m_updateOnScrolling;
}

void ScrollableGraphicPrimitive::setUpdateOnScrolling(bool updateOn)
{
    m_updateOnScrolling = updateOn;
}

void ScrollableGraphicPrimitive::setVerScrollBarVisible(bool visible)
{
    m_verScrollBarVisible = visible;
    if (visible)
        scrollVer();
}

bool ScrollableGraphicPrimitive::isVerScrollBarVisible() const
{
    return m_verScrollBarVisible;
}

void ScrollableGraphicPrimitive::setHorScrollBarVisible(bool visible)
{
    m_horScrollBarVisible = visible;
    if (visible)
        scrollHor();
}

bool ScrollableGraphicPrimitive::isHorScrollBarVisible() const
{
    return m_horScrollBarVisible;
}

int Qtitan::mul_div(int a, int b, int c)
{
    int s;
    int d;
    s = 1;
    if (a < 0) { a = -a; s = -1; }
    if (b < 0) { b = -b; s = -s; }
    if (c < 0) { c = -c; s = -s; }

    d = (int)( c > 0 ? ( (__int64)a * b + ( c >> 1 ) ) / c
                         : 0x7FFFFFFFL );

    return ( s > 0 ) ? d : -d;
}

/* GridGraphicMask */
void GridGraphicMask::clear() const
{
    const GraphicPrimitive::GraphicChildren& list = m_owner->childrenGP();
    for (GraphicPrimitive::GraphicChildren::const_iterator it = list.begin(); it != list.end(); ++it)
        (*it)->m_masked = false;
}

const QRect& GridGraphicMask::rect() const
{
    return m_rect;
}

void GridGraphicMask::setRect(const QRect& r)
{
    m_rect = r;
}

void GridGraphicMask::addGP(GraphicPrimitive* gp) const
{
    gp->m_masked = true;
}

void GridGraphicMask::removeGP(GraphicPrimitive* gp) const
{
    gp->m_masked = false;
}

bool GridGraphicMask::isGPMasked(GraphicPrimitive* gp) const
{
    return gp->m_masked;
}

GridGraphicMask::GridGraphicMask(GraphicPrimitive* owner, const QRect& r)
: m_owner(owner), m_rect(r)
{
}

/* PaintBuffer */
PaintBuffer::PaintBuffer()
{
    reset();
}

bool PaintBuffer::empty() const
{
    return m_empty;
}

void PaintBuffer::reset()
{
    m_buffer.fill(Qt::transparent);
    m_empty = true;
}

void PaintBuffer::setSize(const QSize& size)
{
    if (m_buffer.size() == size)
        return;
    m_buffer = QImage(size, QImage::Format_RGB32);
    reset();
}

QPainter& PaintBuffer::begin()
{
    m_buffer.fill(0xFFFFFF);
    //m_buffer.fill(0x0);
    m_painter.begin(&m_buffer);
    return m_painter;
}

void PaintBuffer::end()
{
    m_painter.end();
    m_empty = false;
}

const QImage& PaintBuffer::buffer() const
{
    return m_buffer;
}

/*!
    \class BufferedGraphicPrimitive
    \inmodule QtitanDataGrid
    \internal
*/

BufferedGraphicPrimitive::BufferedGraphicPrimitive(GraphicControl* control, GraphicPrimitive* parent)
        : GraphicPrimitive(control, parent)
{
}

void BufferedGraphicPrimitive::update(const QRect& rect)
{
    m_paintBuffer.reset();
    GraphicPrimitive::update(rect);
}

void BufferedGraphicPrimitive::paintEvent(QPainter& painter)
{
    if (m_paintBuffer.empty())
    {
        paintBufferEvent(m_paintBuffer.begin());
        m_paintBuffer.end();
    }
    painter.drawImage(boundedRect(), m_paintBuffer.buffer());
}

void BufferedGraphicPrimitive::resizeEvent(const QRect& rect)
{
    m_paintBuffer.setSize(boundedRect().size()/*rect.size()*/);
    GraphicPrimitive::resizeEvent(rect);
}

void BufferedGraphicPrimitive::resetPaintBuffer()
{
    m_paintBuffer.reset();
}

struct CallBackContext
{
    QObject *receiver;
    QEvent *event;
    bool result;
};

static QWidget* get_top_level_widget(QWidget* widget)
{
    QWidget* parent = widget;
    while (parent->parentWidget() != Q_NULL)
    {
        if (parent->isWindow())
            break;
        parent = parent->parentWidget();
    }
    return parent;
}

bool EmbedGraphicPrimitive::qapplication_callback(void** context)
{
    CallBackContext* cb = (CallBackContext *)(context);
    QWidget* widget = static_cast<QWidget *>(cb->receiver);
    switch (cb->event->type())
    {
    case QEvent::WinIdChange:
        {
#ifdef Q_OS_MAC
            QWidget* parent = get_top_level_widget(widget);
            if (parent->parentWidget() != Q_NULL)
                break;

            QHash<QWidget *, EmbedGraphicPrimitive *>::const_iterator it = allEmbedWidgets.constFind(parent);
            if (it == allEmbedWidgets.constEnd())
                break;

            unset_widget_mac_hook(it.value(), widget);
            set_widget_mac_hook(it.value(), widget);
#endif
        }
        break;
    case QEvent::Create:
        {
            QWidget* parent = get_top_level_widget(widget);
            if (parent->parentWidget() != Q_NULL)
                break;

            QHash<QWidget *, EmbedGraphicPrimitive *>::const_iterator it = allEmbedWidgets.constFind(parent);
            if (it == allEmbedWidgets.constEnd())
                break;
            it.value()->installFilterToWidget(widget);
        }
        break;
    case QEvent::Destroy:
        {
            QHash<QWidget *, EmbedGraphicPrimitive *>::const_iterator it = allEmbedWidgets.constFind(widget);
            if (it == allEmbedWidgets.constEnd())
                break;
            it.value()->removeFilterFromWidget(widget);
        }
        break;
    case QEvent::KeyPress:
        {
            QHash<QWidget *, EmbedGraphicPrimitive *>::const_iterator it = allEmbedWidgets.constFind(widget);
            if (it == allEmbedWidgets.constEnd())
                break;
            if (it.value()->m_keyhandling)
                break;

            Q_ASSERT(it.value()->control()->focusedGP() == it.value());
            it.value()->control()->keyPressEvent(static_cast<QKeyEvent *>(cb->event));
            return true;
        }
        break;
    case QEvent::KeyRelease:
        {
            QHash<QWidget *, EmbedGraphicPrimitive *>::const_iterator it = allEmbedWidgets.constFind(widget);
            if (it == allEmbedWidgets.constEnd())
                break;
            if (it.value()->m_keyhandling)
                break;
#if _DEBUG
            GraphicPrimitive* p =  it.value();
            GraphicPrimitive* f =  it.value()->control()->focusedGP();
            Q_ASSERT(f == p);
#endif
            it.value()->control()->keyReleaseEvent(static_cast<QKeyEvent *>(cb->event));
            return true;
        }
        break;
    case QEvent::FocusIn:
        break;
    case QEvent::FocusOut:
        break;
    default:
        break;
    }
    return false;
}

QHash<QWidget *, EmbedGraphicPrimitive *> EmbedGraphicPrimitive::allEmbedWidgets;
static int cbCount = 0;
/* EmbedGraphicPrimitive */
EmbedGraphicPrimitive::EmbedGraphicPrimitive(GraphicControl* control, GraphicPrimitive* parent)
: GraphicPrimitive(control, parent), m_widget(Q_NULL),
  m_overWidget(Q_NULL), m_pressedWidget(Q_NULL), m_keyhandling(false)
{
    if (cbCount == 0)
        QInternal::registerCallback(QInternal::EventNotifyCallback, qapplication_callback);
    cbCount++;
}

EmbedGraphicPrimitive::~EmbedGraphicPrimitive()
{
    cbCount--;
    if (cbCount == 0)
        QInternal::unregisterCallback(QInternal::EventNotifyCallback, qapplication_callback);
    disconnectWidget();
    Q_DELETE_AND_NULL(m_widget);
}

void EmbedGraphicPrimitive::installFilterToWidget(QWidget* widget)
{
    if (!m_filteredWidgets.contains(widget))
    {
        widget->installEventFilter(this);
        m_filteredWidgets.insert(widget);
        allEmbedWidgets.insert(widget, this);

        QList<QAction*> acclist = widget->actions();
        for (QList<QAction*>::const_iterator ait = acclist.constBegin(); ait != acclist.constEnd(); ++ait)
            control()->addAction(*ait);
    }
    const QObjectList& list = widget->children();
    for (QObjectList::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        QWidget* child = qobject_cast<QWidget *>(*it);
        if (child == Q_NULL || child->windowType() != Qt::Widget)
            continue;
        installFilterToWidget(child);
    }
}

void EmbedGraphicPrimitive::removeFocus()
{
    QWidget* widget = control()->focusProxy();
    control()->setFocusProxy(Q_NULL);
    setFocusChild(Q_NULL);
    if (widget != Q_NULL && widget->hasFocus())
        control()->setFocus();
}

void EmbedGraphicPrimitive::removeFilterFromWidget(QWidget* widget)
{
    if (m_widget == widget)
        m_widget = Q_NULL;
    if (m_overWidget == widget)
        m_overWidget = Q_NULL;
    if (m_pressedWidget == widget)
        m_pressedWidget = Q_NULL;

    if (control()->focusProxy() == widget)
    {
        removeFocus();
    }

    QSet<QWidget*>::iterator it = m_filteredWidgets.find(widget);
    if (it != m_filteredWidgets.end())
    {
        QList<QAction*> acclist = widget->actions();
        for (QList<QAction*>::const_iterator ait = acclist.constBegin(); ait != acclist.constEnd(); ++ait)
            control()->removeAction(*ait);

        widget->removeEventFilter(this);
        m_filteredWidgets.erase(it);
    }
}

void EmbedGraphicPrimitive::setWidget(QWidget* widget)
{
    if (m_widget == widget)
        return;
    if (m_widget != Q_NULL)
        disconnectWidget();
    m_widget = widget;
    connectWidget();
    updateWidget(true);
}

QWidget* EmbedGraphicPrimitive::widget() const
{
    return m_widget;
}

bool EmbedGraphicPrimitive::isSubWidget(QWidget* widget)
{
    if (m_widget == Q_NULL)
        return false;

    while (widget)
    {
        if (widget == m_widget)
            return true;
        widget = widget->parentWidget();
    }
    return false;
}

QSize EmbedGraphicPrimitive::sizeHint()
{
    if (m_widget == Q_NULL)
        return GraphicPrimitive::sizeHint();
    return m_widget->sizeHint();
}

bool EmbedGraphicPrimitive::embeded() const
{
    return true;
}

void EmbedGraphicPrimitive::paintEvent(QPainter& painter)
{
    if (m_widget == Q_NULL || m_widget->parentWidget() != Q_NULL)
        return;

    QSize bsize = boundedRect().size();
    if (m_paintBuffer.size() != bsize)
        m_paintBuffer = QPixmap(bsize);
    m_paintBuffer.fill(Qt::transparent);
#ifndef Q_OS_MAC
    //if (painter.device()->devType() != QInternal::Widget || !painter.hasClipping() || painter.opacity() == 1.0)
    //{
    //    m_widget->render(&painter, QPoint(borderLeft(), borderTop()));
    //    return;
    //}
#endif

    //font.setStyleStrategy(QFont::PreferAntialias);
    m_widget->render(&m_paintBuffer);

    painter.drawPixmap(QPoint(borderLeft(), borderTop()), m_paintBuffer);
}

void EmbedGraphicPrimitive::resizeEvent(const QRect& rect)
{
    GraphicPrimitive::resizeEvent(rect);
    updateWidget();
}

void EmbedGraphicPrimitive::updateWidget(bool force)
{
    if (m_widget == Q_NULL)
        return;
    QSize size = boundedRect().size();
    if (force || size != m_widget->size())
    {
        if (size.width() > 0 && size.height() > 0)
            m_widget->setGeometry(QRect(QPoint(0, 0), size));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        m_widget->setAttribute(Qt::WA_Mapped);
        m_widget->setVisible(true);
#else
#endif
    }
}

#define PTR_ADDR long long

static void set_focus_child(QWidget* w, QWidget* child)
{
#ifdef QT_INCLUDE_PRIVATE_H
    qt_widget_private(w)->focus_child = child;
#if 0
    QString version = qVersion();
    PTR_ADDR focus_child_offset = (PTR_ADDR)&(qt_widget_private(w)->focus_child) - (PTR_ADDR)qt_widget_private(w);
    QMessageBox::information(w, version, QString("%1").arg(focus_child_offset));
#endif
#else
    QString version = qVersion();
    PTR_ADDR focus_child_offset = 0;
#ifdef Q_OS_WIN
    if (version == "4.5.0" || version == "4.5.1" || version == "4.5.2" || version == "4.5.3")
    {
        if (QSysInfo::WordSize == 32)
        {
            focus_child_offset = 0x00000094;
        }
        if (QSysInfo::WordSize == 64)
        {
            focus_child_offset = 0x000000f8;
        }
    }
    if (version == "4.6.0" || version == "4.6.1" || version == "4.6.2" || version == "4.6.3")
    {
        if (QSysInfo::WordSize == 32)
        {
            focus_child_offset = 0x0000005c;
        }
        if (QSysInfo::WordSize == 64)
        {
            focus_child_offset = 0x000000a8;
        }
    }
    if (version == "4.7.0" || version == "4.7.1" || version == "4.7.2" || version == "4.7.3" || version == "4.7.4")
    {
        if (QSysInfo::WordSize == 32)
        {
            focus_child_offset = 0x0000005c;
        }
        if (QSysInfo::WordSize == 64)
        {
            focus_child_offset = 0x000000a8;
        }
    }

    if (version == "4.8.0" || version == "4.8.1" || version == "4.8.2" || version == "4.8.3" || version == "4.8.4" || version == "4.8.5" || version == "4.8.6")
    {
        if (QSysInfo::WordSize == 32)
        {
            focus_child_offset = 0x00000058;
        }
        if (QSysInfo::WordSize == 64)
        {
            focus_child_offset = 0x000000a0;
        }
    }
#endif
#ifdef Q_OS_LINUX
    if (version == "4.5.0" || version == "4.5.1" || version == "4.5.2" || version == "4.5.3")
    {
        if (QSysInfo::WordSize == 32)
        {
            focus_child_offset = 0x00000094;
        }
        if (QSysInfo::WordSize == 64)
        {
            focus_child_offset = 0x000000f8;
        }
    }
    if (version == "4.6.0" || version == "4.6.1" || version == "4.6.2" || version == "4.6.3")
    {
        if (QSysInfo::WordSize == 32)
        {
            focus_child_offset = 0x0000005c;
        }
        if (QSysInfo::WordSize == 64)
        {
            focus_child_offset = 0x000000a8;
        }
    }
    if (version == "4.7.0" || version == "4.7.1" || version == "4.7.2" || version == "4.7.3" || version == "4.7.4")
    {
        if (QSysInfo::WordSize == 32)
        {
            focus_child_offset = 0x0000005c;
        }
        if (QSysInfo::WordSize == 64)
        {
            focus_child_offset = 0x000000a8;
        }
    }
    
    if (version == "4.8.0" || version == "4.8.1" || version == "4.8.2" || version == "4.8.3" || version == "4.8.4" || version == "4.8.5" || version == "4.8.6")
    {
        if (QSysInfo::WordSize == 32)
        {
            focus_child_offset = 0x00000058;
        }
        if (QSysInfo::WordSize == 64)
        {
            focus_child_offset = 0x000000a0;
        }
    }
#endif 
#ifdef Q_OS_MACX
    if (version == "4.5.0" || version == "4.5.1" || version == "4.5.2" || version == "4.5.3")
    {
        if (QSysInfo::WordSize == 32)
        {
            focus_child_offset = 0x00000094;
        }
        if (QSysInfo::WordSize == 64)
        {
            focus_child_offset = 0x000000f8;
        }
    }
    if (version == "4.6.0" || version == "4.6.1" || version == "4.6.2" || version == "4.6.3")
    {
        if (QSysInfo::WordSize == 32)
        {
            focus_child_offset = 0x0000005c;
        }
        if (QSysInfo::WordSize == 64)
        {
            focus_child_offset = 0x000000a8;
        }
    }
    if (version == "4.7.0" || version == "4.7.1" || version == "4.7.1" || version == "4.7.2" || version == "4.7.3" || version == "4.7.4")
    {
        if (QSysInfo::WordSize == 32)
        {
            focus_child_offset = 0x0000005c;
        }
        if (QSysInfo::WordSize == 64)
        {
            focus_child_offset = 0x000000a8;
        }
    }
    
    if (version == "4.8.0" || version == "4.8.1" || version == "4.8.2" || version == "4.8.3" || version == "4.8.4" || version == "4.8.5" || version == "4.8.6")
    {
        if (QSysInfo::WordSize == 32)
        {
            focus_child_offset = 0x00000058;
        }
        if (QSysInfo::WordSize == 64)
        {
            focus_child_offset = 0x000000a0;
        }
    }
#endif

    if (focus_child_offset == 0)
    {
        Q_ASSERT(false); //Unsuported Qt version, use private header directly. Use QT_INCLUDE_PRIVATE_H define.
        return;
    }
    QWidget** focusChildPtr = static_cast<QWidget **>((void *)((PTR_ADDR)qt_widget_private(w) + focus_child_offset));
    *focusChildPtr = child;
#endif
}

void EmbedGraphicPrimitive::setFocusChild(QWidget* fw)
{
    QWidget* w = control();
    if (w->isHidden())
    {
        while (w && w->isHidden())
        {
            set_focus_child(w, fw);
            w = w->isWindow() ? Q_NULL : w->parentWidget();
        }
    }
    else
    {
        while (w)
        {
            set_focus_child(w, fw);
            w = w->isWindow() ? Q_NULL : w->parentWidget();
        }
    }
}

bool EmbedGraphicPrimitive::eventFilter(QObject* obj, QEvent* e)
{
    if (m_widget != Q_NULL)
    {
        switch (e->type())
        {
        case QEvent::DragEnter:
        case QEvent::DragLeave:
        case QEvent::DragMove:
        case QEvent::Drop:
        case QEvent::DragResponse:
            break;
        case QEvent::FocusIn:
            {
                QWidget* newFocusdWidget = static_cast<QWidget *>(obj);
                setFocusChild(newFocusdWidget);
                control()->widgetFocusIn(newFocusdWidget, (QFocusEvent *)e);
                control()->update();
            }
            break;
        case QEvent::FocusOut:
            {
                QWidget* newFocusdWidget = static_cast<QWidget *>(obj);
                control()->widgetFocusOut(newFocusdWidget, (QFocusEvent *)e);
                control()->update();
            }
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        case QEvent::UpdateLater:
        case QEvent::UpdateRequest:
            {
                QPoint p = (static_cast<QWidget *>(obj)->mapTo(m_widget, QPoint(0, 0)));
                QRect r(p.x(), p.y(),
                    static_cast<QWidget *>(obj)->rect().width(),
                    static_cast<QWidget *>(obj)->rect().height());
                r.translate(borderLeft(), borderTop());
                update(r);
            }
            break;
#else
        case QEvent::Paint:
            {
                if (!control()->testAttribute(Qt::WA_WState_InPaintEvent))
                {
                    if (obj != m_widget)
                    {
                        QPoint p = (static_cast<QWidget *>(obj)->mapTo(m_widget, QPoint(0, 0)));
                        QRect r(p.x(), p.y(),
                            static_cast<QWidget *>(obj)->rect().width(),
                            static_cast<QWidget *>(obj)->rect().height());
                        r.translate(borderLeft(), borderTop());
                        update(r);
                    }
                    else
                    {
                        update(((QPaintEvent *)e)->rect().translated(borderLeft(), borderTop()));
                    }
                    return true;
                }
            }
            break;
#endif
        case QEvent::Resize:
            {
#if 0
                updateWidgetGeometry();
#endif
            }
            break;
        default:
            break;
        }
    }

    return QObject::eventFilter(obj, e);
}

void EmbedGraphicPrimitive::mouseMoveEvent(const QPoint& point, QMouseEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }

    QPoint wPos = point;
    QWidget* w = Q_NULL;
    if (m_pressedWidget == Q_NULL)
    {
        w = getOverWidget(point, wPos);
        if (m_overWidget != Q_NULL && m_overWidget != w)
        {
            doMouseOutEvent();
            doMouseOverEvent(point);
        }
    }
    else
    {
        w = m_pressedWidget;
        QWidget* parent = w;
        while (parent != Q_NULL && parent->parentWidget() != Q_NULL)
        {
            wPos -= parent->geometry().topLeft();
            parent = parent->parentWidget();
            if (parent == w)
                break;
        }
    }

    QMouseEvent mouseEvent(QEvent::MouseMove, 
                    wPos,
                    ((QMouseEvent *)e)->globalPos(), 
                    Qt::MouseButton(((QMouseEvent *)e)->button()),
                    Qt::MouseButtons(((QMouseEvent *)e)->buttons() & Qt::MouseButtonMask),
                    Qt::KeyboardModifiers(((QMouseEvent *)e)->modifiers() & Qt::KeyboardModifierMask));
    QApplication::sendEvent(w, &mouseEvent);
    e->setAccepted(mouseEvent.isAccepted());
}

void EmbedGraphicPrimitive::mousePressEvent(const QPoint& point, QMouseEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }

    QPoint wPos;
    m_pressedWidget = getOverWidget(point, wPos);
    setInternalWinPos();
    QMouseEvent mouseEvent(QEvent::MouseButtonPress, 
                    wPos,
                    ((QMouseEvent *)e)->globalPos(), 
                    Qt::MouseButton(((QMouseEvent *)e)->button()),
                    Qt::MouseButtons(((QMouseEvent *)e)->buttons() & Qt::MouseButtonMask),
                    Qt::KeyboardModifiers(((QMouseEvent *)e)->modifiers() & Qt::KeyboardModifierMask));
    QApplication::sendEvent(m_pressedWidget, &mouseEvent);
    e->setAccepted(mouseEvent.isAccepted());
}

void EmbedGraphicPrimitive::mouseReleaseEvent(const QPoint& point, QMouseEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }

    QPoint wPos;
    if (m_pressedWidget == Q_NULL)
        return;

    m_pressedWidget = getOverWidget(point, wPos);
    QMouseEvent mouseEvent(QEvent::MouseButtonRelease, 
                    wPos,
                    ((QMouseEvent *)e)->globalPos(), 
                    Qt::MouseButton(((QMouseEvent *)e)->button()),
                    Qt::MouseButtons(((QMouseEvent *)e)->buttons()),// & Qt::MouseButtonMask),
                    Qt::KeyboardModifiers(((QMouseEvent *)e)->modifiers()));// & Qt::KeyboardModifierMask));
    QApplication::sendEvent(m_pressedWidget, &mouseEvent);
    e->setAccepted(mouseEvent.isAccepted());
    m_pressedWidget = Q_NULL;
}

void EmbedGraphicPrimitive::mouseDoubleClickEvent(const QPoint& point, QMouseEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }

    QPoint wPos;
    m_pressedWidget = getOverWidget(point, wPos);
    setInternalWinPos();
    QMouseEvent mouseEvent(QEvent::MouseButtonDblClick, 
                    wPos,
                    ((QMouseEvent *)e)->globalPos(), 
                    Qt::MouseButton(e->button()),
                    Qt::MouseButtons(e->buttons() & Qt::MouseButtonMask),
                    Qt::KeyboardModifiers(((QMouseEvent *)e)->modifiers() & Qt::KeyboardModifierMask));
    QApplication::sendEvent(m_pressedWidget, &mouseEvent);
    e->setAccepted(mouseEvent.isAccepted());
}

class QWidgetPublic: public QWidget
{
    friend class EmbedGraphicPrimitive;
};

void EmbedGraphicPrimitive::doMouseOverEvent(const QPoint& pos)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
        return;

    QPoint wPos;
    QWidget* w = getOverWidget(pos, wPos);

    QEvent enterEvent(QEvent::Enter);
    QApplication::sendEvent(w, &enterEvent);
    if (w->testAttribute(Qt::WA_Hover))
    {
        QHoverEvent hoverEvent(QEvent::HoverEnter, wPos, QPoint(-1, -1));
        QApplication::sendEvent(w, &hoverEvent);
    }
    m_overWidget = w;
}

void EmbedGraphicPrimitive::doMouseOutEvent()
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
        return;

    //Q_ASSERT(m_overWidget != Q_NULL);
    if (m_overWidget == Q_NULL)
        m_overWidget = m_widget;

    QEvent leaveEvent(QEvent::Leave);
    QApplication::sendEvent(m_overWidget, &leaveEvent);
    if (m_overWidget->testAttribute(Qt::WA_Hover))
    {
        QHoverEvent hoverEvent(QEvent::HoverLeave, QPoint(-1, -1), mapFromGlobal(QCursor::pos()));
        QApplication::sendEvent(m_overWidget, &hoverEvent);
    }
    m_overWidget = Q_NULL;
}

void EmbedGraphicPrimitive::mouseOverEvent(const QPoint& pos)
{
    doMouseOverEvent(pos);
}

void EmbedGraphicPrimitive::mouseOutEvent()
{
    doMouseOutEvent();
}

void EmbedGraphicPrimitive::wheelEvent(const QPoint& pos, QWheelEvent* e)
{
    e->ignore();
    if (m_widget == Q_NULL || !m_widget->isEnabled())
        return;
    QPoint wPos;
    QWidget* w = getOverWidget(pos, wPos);

    QWheelEvent we(wPos,
        e->globalPos(),
        e->delta(),
        e->buttons(),
        e->modifiers(),
        e->orientation());
    QApplication::sendEvent(w, &we);
    if (we.isAccepted())
        e->accept();
}

static QWidget* get_proxy_widget(QWidget* widget)
{
    QWidget *f = widget;
    while (f != Q_NULL && f->focusProxy() != Q_NULL)
        f = f->focusProxy();
    return f;
}

#define PREVENT_CLOSE_1 1

void EmbedGraphicPrimitive::keyPressEvent(QKeyEvent* e)
{
    if (m_keyhandling)
        return;

    e->ignore();
    if (m_widget == Q_NULL || !m_widget->isEnabled())
        return;

    setInternalWinPos();

    QWidget* focus = get_proxy_widget(m_widget);
    Q_ASSERT(focus != Q_NULL);
    m_keyhandling  = true;
#if PREVENT_CLOSE_1
    QWidgetData* data = qt_qwidget_data(m_widget);
    Qt::WindowFlags flags = data->window_flags;
    Q_UNUSED(flags);
    data->window_flags &= ~Qt::Popup; //Prevents close() if Qt::Key_Escape has been clicked in void QWidget::keyPressEvent(QKeyEvent *event)
    data->window_flags |= Qt::Window;
#endif
    QApplication::sendEvent(focus, e);
#if PREVENT_CLOSE_1
    data->window_flags = flags;
#endif
    m_keyhandling  = false;
}

void EmbedGraphicPrimitive::keyReleaseEvent(QKeyEvent* e)
{
    if (m_keyhandling)
        return;

    e->ignore();
    if (m_widget == Q_NULL || !m_widget->isEnabled())
        return;

    QWidget* focus = get_proxy_widget(m_widget);
    Q_ASSERT(focus != Q_NULL);

    m_keyhandling = true;

#if PREVENT_CLOSE_1
    QWidgetData* data = qt_qwidget_data(m_widget);
    Qt::WindowFlags flags = data->window_flags;
    Q_UNUSED(flags);
    data->window_flags &= ~Qt::Popup; //Prevents close() if Qt::Key_Escape has been clicked in void QWidget::keyPressEvent(QKeyEvent *event)
    data->window_flags |= Qt::Window;
#endif
    QApplication::sendEvent(focus, e);
#if PREVENT_CLOSE_1
    data->window_flags = flags;
#endif

    m_keyhandling  = false;
}

void EmbedGraphicPrimitive::focusInEvent(QFocusEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }
}

void EmbedGraphicPrimitive::focusOutEvent(QFocusEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }
}

void EmbedGraphicPrimitive::showContextMenuEvent(const QPoint& pos, QContextMenuEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        GraphicPrimitive::showContextMenuEvent(pos, e);
        return;
    }

    QPoint wPos;
    QWidget* w = getOverWidget(pos, wPos);

    QContextMenuEvent contextEvent(QContextMenuEvent::Mouse,
                    wPos, ((QContextMenuEvent *)e)->globalPos(),
                    ((QContextMenuEvent *)e)->modifiers());
    QApplication::sendEvent(w, &contextEvent);
    e->setAccepted(contextEvent.isAccepted());
    if (!e->isAccepted())
        GraphicPrimitive::showContextMenuEvent(pos, e);
}

QCursor EmbedGraphicPrimitive::cursor() const
{
    if (m_overWidget != Q_NULL)
        return m_overWidget->cursor();
    else if (m_widget != Q_NULL)
        return m_widget->cursor();
    else
        return GraphicPrimitive::cursor();
}

void EmbedGraphicPrimitive::connectWidget()
{
    if (m_widget == Q_NULL)
        return;
    Q_ASSERT(m_widget->testAttribute(Qt::WA_DontShowOnScreen));
    m_widget->setAttribute(Qt::WA_QuitOnClose, false);
    QWidgetData* data = qt_qwidget_data(m_widget);
    if (m_widget->isHidden())
    {
        data->window_flags |= Qt::ToolTip;
        m_widget->show();
        data->window_flags &= ~Qt::ToolTip;
    }
    data->window_flags |= Qt::Popup;
#ifdef Q_OS_WIN
    if (SetWindowLongPtrW((HWND)m_widget->internalWinId(), GWL_STYLE, WS_POPUP) == 0)
    {
#ifdef _DEBUG
        qErrnoWarning("EmbedGraphicPrimitive::connectWidget: Failed to set window style");
#endif
    }
#endif

#ifdef Q_OS_MAC
    set_widget_mac_hook(this, m_widget);
#endif

    m_filteredWidgets.reserve(16);
    installFilterToWidget(m_widget);
    m_pressedWidget = Q_NULL;

    if (control()->focusedGP() == this)
    {
        control()->setFocusProxy(m_widget);
        control()->setFocus();
    }

    if (control()->overGP() == this)
    {
        QPoint globalPos = QCursor::pos();
        QPoint pos = mapFromGlobal(globalPos);
        mouseOverEvent(pos);
        QMouseEvent e(QEvent::MouseMove, pos, globalPos, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
        mouseMoveEvent(pos, &e);
    }
}

void EmbedGraphicPrimitive::disconnectWidget()
{
    if (m_widget == Q_NULL)
        return;

#ifdef Q_OS_MAC
    unset_widget_mac_hook(this, m_widget);
#endif

    while (m_filteredWidgets.size() > 0)
    {
        QWidget* widget = (*m_filteredWidgets.constBegin());
        allEmbedWidgets.remove(widget);
        removeFilterFromWidget(widget);
    }

    Q_ASSERT(m_widget == Q_NULL);
    Q_ASSERT(m_overWidget == Q_NULL);
    Q_ASSERT(m_pressedWidget == Q_NULL);
    Q_ASSERT(m_filteredWidgets.size() == 0);
//    m_overWidget = Q_NULL;
//    m_pressedWidget = Q_NULL;
}

QWidget* EmbedGraphicPrimitive::getOverWidget(const QPoint& pos, QPoint& widgetPos)
{
    if (m_widget == Q_NULL)
         return Q_NULL;

    QPoint boundedPos = pos - QPoint(borderLeft(), borderTop());
    QWidget* retval = Q_NULL;

    retval = m_widget->childAt(boundedPos);
    if (retval == Q_NULL)
    {
        widgetPos = boundedPos;
        return m_widget;
    }
    QWidget* parent = retval;
    widgetPos = boundedPos;
    while (parent->parentWidget() != Q_NULL)
    {
        widgetPos -= parent->geometry().topLeft();
        parent = parent->parentWidget();
        if (parent == retval)
            break;
    }
    return retval;
}

/* GraphicPrimitiveScrollBar */
GraphicPrimitiveScrollBar::GraphicPrimitiveScrollBar(
    Qt::Orientation orientation, ScrollableGraphicPrimitive* parent)
    : QScrollBar(orientation, parent->control()), m_scrollableGraphic(parent)
{
    setMouseTracking(true);
}

void GraphicPrimitiveScrollBar::mousePressEvent(QMouseEvent* e)
{
    if (orientation() == Qt::Horizontal && !m_scrollableGraphic->canScrollX())
        return;
    if (orientation() == Qt::Vertical && !m_scrollableGraphic->canScrollY())
        return;
    QScrollBar::mousePressEvent(e);
}

class ScrollBarMouseEvent: public QMouseEvent
{
    ScrollBarMouseEvent(ScrollBarMouseEvent& e) : QMouseEvent(e) {}
    friend class Qtitan::GraphicPrimitiveScrollBar;
};

void GraphicPrimitiveScrollBar::mouseMoveEvent(QMouseEvent* e)
{
    QScrollBar::mouseMoveEvent(e);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QPointF p = static_cast<ScrollBarMouseEvent *>(e)->l;
    static_cast<ScrollBarMouseEvent *>(e)->l = m_scrollableGraphic->mapFromGlobal(e->globalPos());
    m_scrollableGraphic->control()->mouseMoveEvent(e);
    static_cast<ScrollBarMouseEvent *>(e)->l = p;
#else
    QPoint p = static_cast<ScrollBarMouseEvent *>(e)->p;
    static_cast<ScrollBarMouseEvent *>(e)->p = m_scrollableGraphic->mapFromGlobal(e->globalPos());
    m_scrollableGraphic->control()->mouseMoveEvent(e);
    static_cast<ScrollBarMouseEvent *>(e)->p = p;
#endif
}

bool Qtitan::intersect_line(QLineF& line, const QRect& region)
{
    bool containsP1 = region.contains(QPoint((int)line.x1(), (int)line.y1()));
    bool containsP2 = region.contains(QPoint((int)line.x2(), (int)line.y2()));
    if (containsP1 && containsP2)
        return true;
    if (!containsP1 && !containsP2)
    {
        line = QLineF();
        return false;
    }

    QLineF left(region.left(), region.top(), region.left(), region.top() + region.height());
    QLineF top(region.left(), region.top(), region.left() + region.width(), region.top());
    QLineF right(region.left() + region.width(), region.top(), region.left() + region.width(), region.top() + region.height());
    QLineF bottom(region.left(), region.top() + region.height(), region.left() + region.width(), region.top() + region.height());

    QPointF p;
    if (left.intersect(line, &p) == QLineF::BoundedIntersection)
        line.setP1(p);
    if (top.intersect(line, &p) == QLineF::BoundedIntersection)
        line.setP1(p);
    if (right.intersect(line, &p) == QLineF::BoundedIntersection)
        line.setP2(p);
    if (bottom.intersect(line, &p) == QLineF::BoundedIntersection)
        line.setP2(p);

    return containsP1 || containsP2;
}

void Qtitan::draw_focus_frame(QPainter& painter, const QColor& color, const QRect& rect)
{
    QPen pen;
    pen.setWidth(1);
    pen.setBrush(QBrush(color, Qt::Dense4Pattern));
    painter.setPen(pen);
    painter.drawRect(rect.adjusted(0, 0, -1, -1));
}

inline uint qHash(const QStyleOptionButton& option)
{
    uint ret = qHash(option.version) + qHash(option.type) + qHash(option.state) + qHash(option.direction);
    ret += (qHash(option.rect.x()) + qHash(option.rect.y()) + qHash(option.rect.width()) + qHash(option.rect.height())) / 4;
    return ret;
}

inline bool operator==(const QStyleOptionButton &o1, const QStyleOptionButton &o2)
 {
    return o1.version == o2.version &&
        o1.type == o2.type &&
        o1.state == o2.state &&
        o1.direction == o2.direction &&
        o1.rect == o2.rect &&
        o1.fontMetrics == o2.fontMetrics &&
        o1.palette == o2.palette &&
        o1.features == o2.features &&
        o1.text == o2.text &&
        o1.icon.isNull() == o2.icon.isNull() &&
        o1.iconSize == o2.iconSize;
}


/* SmoothEffect */
SmoothEffect::SmoothEffect()
: m_gp(0), m_duration(0), m_active(false)
{
}

void SmoothEffect::intialize(GraphicPrimitive* gp)
{
    m_gp = gp;
}

void SmoothEffect::setInitialState(const QImage& iState)
{
    m_istate = iState;
}

void SmoothEffect::setInitialStateToEmpty(int width, int height)
{
    QPixmap empty(width, height);
    empty.fill(Qt::transparent);
    m_istate = empty.toImage();
}

int SmoothEffect::duration() const
{
    return m_duration;
}

void SmoothEffect::setDuration(int duration)
{
    m_time = QTime::currentTime();
    m_duration = duration;
}

bool SmoothEffect::isActive() const
{
    return m_active;
}

void SmoothEffect::paintToInitial(QPainter* painter)
{
    double alpha = 1.0;
    if (duration() > 0)
    {
        QTime now = QTime::currentTime();

        if (m_time > now)
            m_time = now;

        int diff = m_time.msecsTo(now);
        alpha = diff / (double)duration();
        if (diff > duration())
        {
            m_active = false;
            alpha = 1.0;
        }
    }
    else
        m_active = false;

    double saveopacity = painter->opacity();
    painter->setOpacity(saveopacity * (1 - alpha));

    painter->drawImage(m_istate.rect(), m_istate);
    painter->setOpacity(saveopacity);
}

void SmoothEffect::paintToCurrent(QPainter* painter, double& currentOpacity)
{
    currentOpacity = 1.0;
    if (duration() > 0)
    {
        QTime now = QTime::currentTime();

        if (m_time > now)
            m_time = now;

        int diff = m_time.msecsTo(now);
        if (diff > duration())
        {
            m_active = false;
            currentOpacity = 1.0;
        }
        else
            currentOpacity = (double)diff / (double)duration();
    }
    else
        m_active = false;

    painter->drawImage(m_istate.rect(), m_istate);
}

/* Effects */
Effects::Effects(QWidget* control)
: m_control(control)
{
}

Effects::~Effects()
{
    while (m_list.size() > 0)
        stop(m_list[0]);
    m_control = Q_NULL;
}

void Effects::run(SmoothEffect* effect)
{
    effect->m_active = true;
    m_list.push_back(effect);
    if (!m_timer.isActive())
        m_timer.start(50, this); 
}

void Effects::stop(SmoothEffect* effect)
{
    m_list.removeAll(effect);
    delete effect;
    if (m_list.size() == 0 && m_timer.isActive())
        m_timer.stop(); 
}

void Effects::update()
{
    QRegion region;
    GraphicPrimitive* parent;
    bool next;

    for (int i = m_list.size() - 1; i >= 0; --i)
    {
        SmoothEffect* effect = m_list[i];
        Q_ASSERT(effect->m_gp != 0);
        if (effect->m_gp != 0)
        {
            parent = effect->m_gp;
            next = false;
            while (parent != 0)
            {
                if (!parent->visibility())
                {
                    stop(effect);
                    next = true;
                    break;
                }
                parent = parent->parentGP();
            }
            if (next)
              continue;

            region += QRect(effect->m_gp->mapToSite(QPoint(0, 0)), effect->m_gp->rect().size());
        }
        if (!effect->isActive())
            stop(effect);
    }
    m_control->update(region);
}

SmoothEffect* Effects::findEffect(GraphicPrimitive* gp)
{
    for (QList<SmoothEffect *>::const_iterator it = m_list.constBegin(); it != m_list.constEnd(); ++it)
    {
        if ((*it)->m_gp == gp)
            return *it;
    }
    return 0;
}

bool Effects::event(QEvent* e)
{
    switch (e->type())
    {
    case QEvent::Timer:
        {
            if (static_cast<QTimerEvent *>(e)->timerId() == m_timer.timerId())
            {
                update();
                e->accept();
                return true;
            }
        }
        break;
    default:
        break;
    }
    return QObject::event(e); 
}

/*!
\class EffectSettings
\internal
*/
EffectSettings::EffectSettings()
{
}

EffectSettings::~EffectSettings()
{
}

void EffectSettings::setStyleOption(const QStyleOptionHeader& option)
{
    m_opt = option;
}

const QStyleOptionHeader& EffectSettings::styleOption() const
{
    return m_opt;
}

QSize EffectSettings::size() const
{
    return m_bg.size();
}

void EffectSettings::setBg(const QImage& bg)
{
    m_bg = bg;
}

const QImage& EffectSettings::bg() const
{
    return m_bg;
}

CustomShadowEffect::CustomShadowEffect(QObject *parent) :
QGraphicsEffect(parent),
_distance(4.0f),
_blurRadius(10.0f),
_color(0, 0, 0, 80)
{
}

QT_BEGIN_NAMESPACE
extern Q_GUI_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

void CustomShadowEffect::draw(QPainter* painter)
{
    // if nothing to show outside the item, just draw source
    if ((blurRadius() + distance()) <= 0) {
        drawSource(painter);
        return;
    }

    PixmapPadMode mode = QGraphicsEffect::PadToEffectiveBoundingRect;
    QPoint offset;
    const QPixmap px = sourcePixmap(Qt::DeviceCoordinates, &offset, mode);

    // return if no source
    if (px.isNull())
        return;

    // save world transform
    QTransform restoreTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());

    // Calculate size for the background image
    QSize szi(px.size().width() + 2 * distance(), px.size().height() + 2 * distance());

    QImage tmp(szi, QImage::Format_ARGB32_Premultiplied);
    QPixmap scaled = px.scaled(szi);
    tmp.fill(0);
    QPainter tmpPainter(&tmp);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_Source);
    tmpPainter.drawPixmap(QPointF(-distance(), -distance()), scaled);
    tmpPainter.end();

    // blur the alpha channel
    QImage blurred(tmp.size(), QImage::Format_ARGB32_Premultiplied);
    blurred.fill(0);
    QPainter blurPainter(&blurred);
    qt_blurImage(&blurPainter, tmp, blurRadius(), false, true);
    blurPainter.end();

    tmp = blurred;

    // blacken the image...
    tmpPainter.begin(&tmp);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tmpPainter.fillRect(tmp.rect(), color());
    tmpPainter.end();

    // draw the blurred shadow...
    painter->drawImage(offset, tmp);

    // draw the actual pixmap...
    painter->drawPixmap(offset, px, QRectF());

    // restore world transform
    painter->setWorldTransform(restoreTransform);
}

QRectF CustomShadowEffect::boundingRectFor(const QRectF& rect) const
{
    qreal delta = blurRadius() + distance();
    return rect.united(rect.adjusted(-delta, -delta, delta, delta));
}

QColor Qtitan::qcolor_mix_rgb(const QColor& first, const QColor& second)
{
    return QColor((first.red() + second.red()) / 2, (first.green() + second.green()) / 2,
                  (first.blue() + second.blue()) / 2, (first.alpha() + second.alpha()) / 2); 
} 

QColor Qtitan::qcolor_mix_cmy(const QColor& first, const QColor& second)
{
    QColor retval;
    qreal c1, m1, y1, k1, a1;
    qreal c2, m2, y2, k2, a2;
    ((QColor &)first).getCmykF(&c1, &m1, &y1, &k1, &a1);
    ((QColor &)second).getCmykF(&c2, &m2, &y2, &k2, &a2);
    retval.setCmykF((c1 + c2) / 2, (m1 + m2) / 2, (y1 + y2) / 2, (k1 + k2) / 2, (a1 + a2) / 2);
    return retval;
}

QVariant::Type Qtitan::qvariant_to_pixmap(const QVariant& v, QPixmap& pixmap)
{
    pixmap = QPixmap();
    switch (v.type())
    {
    case QVariant::ByteArray:
        {
            if (pixmap.loadFromData(v.value<QByteArray>()))
                return v.type();
            pixmap = QPixmap();
            return v.type();
        }
        break;
    case QVariant::Pixmap:
    case QVariant::Bitmap:
    case QVariant::Icon:
    case QVariant::Image:
        {
            pixmap = v.value<QPixmap>();
            return v.type();
        }
        break;
    default:
        pixmap = QPixmap();
    }
    return QVariant::Invalid;
}

void Qtitan::draw_pixmap_texture(QPainter& painter, const QRect& rect, const QPixmap& texture, int left, int top, int right, int bottom)
{
    painter.drawPixmap(QPoint(rect.x(), rect.y()), texture, QRect(0, 0, left, top));
    painter.drawPixmap(QPoint(rect.x(), rect.y() + rect.height() - bottom), texture, QRect(0, texture.height() - bottom, left, bottom));
    painter.drawPixmap(QPoint(rect.x() + rect.width() - right, rect.y()), texture, QRect(texture.width() - right, 0, right, top));
    painter.drawPixmap(QPoint(rect.x() + rect.width() - right, rect.y() + rect.height() - bottom), texture, QRect(texture.width() - right, texture.height() - bottom, right, bottom));

    QPixmap temp = texture.copy(left, 0, texture.width() - right - left, top);
    temp = temp.scaled(rect.width() - left - right, top, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawPixmap(QPoint(rect.x() + left, rect.y()), temp);

    temp = texture.copy(0, top, left, texture.height() - top - bottom);
    temp = temp.scaled(left, rect.height() - top - bottom, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawPixmap(QPoint(rect.x(), rect.y() + top), temp);

    temp = texture.copy(texture.width() - right, top, right, texture.height() - top - bottom);
    temp = temp.scaled(right, rect.height() - top - bottom, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawPixmap(QPoint(rect.x() + rect.width() - right, rect.y() + top), temp);

    temp = texture.copy(right, texture.height() - bottom, texture.width() - left - right, bottom);
    temp = temp.scaled(rect.width() - left - right, bottom, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawPixmap(QPoint(rect.x() + left, rect.y() + rect.height() - bottom), temp);

    temp = texture.copy(right, top, texture.width() - left - right, texture.height() - top - bottom);
    temp = temp.scaled(rect.width() - left - right, rect.height() - top - bottom, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawPixmap(QPoint(rect.x() + left, rect.y() + top), temp);
}

QPixmap Qtitan::apply_effect_to_pixmap(const QPixmap& pixmap, QGraphicsEffect *effect, double opacity)
{
    Q_ASSERT(!pixmap.isNull() && effect != Q_NULL);
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(pixmap);
    item.setGraphicsEffect(effect);
    item.setOpacity(opacity);
    scene.addItem(&item);
    QPixmap res(pixmap.size());
    res.fill(Qt::transparent);
    {
        QPainter ptr(&res);
        scene.render(&ptr, QRectF(), QRectF(0, 0, pixmap.width(), pixmap.height()));
    }
    return res;
}

QPixmap Qtitan::qtn_create_background(const QColor& color)
{
    QPixmap bg(":res/grid_fieldchooser_background.png");
    QPixmap temp(bg.width(), bg.height());
    temp.fill(Qt::white);
    {
        QLinearGradient linearGrad(QPointF(0, 0), QPointF(1000, 55));
        linearGrad.setColorAt(0, color.darker());
        linearGrad.setColorAt(1, color.lighter());

        QPainter pixmapPainter(&temp);
        pixmapPainter.fillRect(QRect(0, 0, temp.width(), temp.height()), linearGrad);
        pixmapPainter.drawPixmap(0, 0, bg);
    }
    return temp;
}

extern void Qtitan::qtn_QTextDocument_draw(QPainter& painter, QTextDocument* document, const QPalette& palette, const QRect& rect)
{
    painter.save();
    QAbstractTextDocumentLayout::PaintContext ctx;
    painter.translate(rect.left(), rect.top());
    ctx.clip = QRect(0, 0, rect.width(), rect.height());
    painter.setClipRect(ctx.clip, Qt::IntersectClip);
    ctx.palette = palette;
    document->documentLayout()->draw(&painter, ctx);
    painter.restore();
}

