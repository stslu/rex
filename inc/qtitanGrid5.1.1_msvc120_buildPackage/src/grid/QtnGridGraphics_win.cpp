/****************************************************************************
**
** Qtitan Framework by Developer Machines
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

#include <QPaintEngine>
#include <QLibrary>
#include <QApplication>
#include <QDesktopWidget>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGui/private/qpaintengine_raster_p.h>
#endif
#include "QtnGridGraphics.h"
#include <windows.h>
QTITAN_USE_NAMESPACE

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
QT_BEGIN_NAMESPACE
    Q_GUI_EXPORT QPixmap qt_pixmapFromWinHBITMAP(HBITMAP, int hbitmapFormat = 0);
    Q_GUI_EXPORT HBITMAP qt_pixmapToWinHBITMAP(const QPixmap& p, int hbitmapFormat = 0);
    Q_GUI_EXPORT HICON qt_pixmapToWinHICON(const QPixmap& p);
    Q_GUI_EXPORT QPixmap qt_pixmapFromWinHICON(HICON icon);
QT_END_NAMESPACE

enum HBitmapFormat 
{
    HBitmapNoAlpha,
    HBitmapPremultipliedAlpha,
    HBitmapAlpha
};
#endif

void EmbedGraphicPrimitive::setInternalWinPos() const
{
    if (m_widget == Q_NULL)
        return;

    QPoint gp = mapToGlobal(QPoint(0, 0));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QWindow* w = m_widget->windowHandle();
    if (w != Q_NULL)
        w->setGeometry(QRect(gp, w->geometry().size()));
#else
    MoveWindow(m_widget->internalWinId(), gp.x(), gp.y(),
        m_widget->width(), m_widget->height(), false);
#endif
}
void Qtitan::draw_text(QPainter& painter, const QString& text, const QRect& rect, int flags, bool transparent)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
  Q_UNUSED(transparent);
  painter.drawText(rect, flags, text);
#else
    int pd = painter.device()->devType();
    if (isAirAvailable() || (painter.opacity() != 1.0 || pd == QInternal::Pixmap || pd == QInternal::Printer))
    {
        painter.drawText(rect, flags, text);
        return;
    }
    QPoint deviceOffset(int(painter.deviceMatrix().dx()),
                        int(painter.deviceMatrix().dy()));
    HDC hdc = painter.paintEngine()->getDC();
    int oldBkMode = GetBkMode(hdc);
    SelectObject(hdc, painter.font().handle());
    
    if (transparent && oldBkMode != TRANSPARENT)
        SetBkMode(hdc, TRANSPARENT);
    QRegion sysRgn = painter.paintEngine()->systemClip();
    if (painter.hasClipping())
        sysRgn &= painter.clipRegion().translated(deviceOffset);
    SelectClipRgn(hdc, sysRgn.handle()); 
    UINT format = 0;
    if (flags & Qt::AlignLeft)
        format |= DT_LEFT;
    if (flags & Qt::AlignRight)
        format |= DT_RIGHT;
    if (flags & Qt::AlignHCenter)
        format |= DT_CENTER;
    if (flags & Qt::AlignVCenter)
        format |= DT_VCENTER;
    if (flags & Qt::AlignJustify)
        Q_ASSERT(false);
    if (flags & Qt::AlignTop)
        format |= DT_TOP;
    if (flags & Qt::AlignBottom)
        format |= DT_BOTTOM;
    if (flags & Qt::TextDontClip)
        format |= DT_NOCLIP;
    if (flags & Qt::TextSingleLine)
        format |= DT_SINGLELINE;
    if (flags & Qt::TextExpandTabs)
        Q_ASSERT(false);
    if (flags & Qt::TextShowMnemonic)
        Q_ASSERT(false);
    if (flags & Qt::TextWordWrap)
        format |= DT_WORDBREAK;
    else
        format |= DT_SINGLELINE;
    if (flags & Qt::TextIncludeTrailingSpaces)
        Q_ASSERT(false);

    
    QColor color = painter.pen().color();
    SetTextColor(hdc, RGB(color.red(), color.green(), color.blue())); 
    color = painter.background().color();
    SetBkColor(hdc, RGB(color.red(), color.green(), color.blue())); 
 
    QRect area = rect.translated(deviceOffset);
    RECT r;
    SetRect(&r, area.left(), area.top(), area.right() + 1, area.bottom() + 1);
    DrawTextW(hdc, (WCHAR*)text.utf16(), -1, &r, format);

    if (transparent && oldBkMode != TRANSPARENT)
        SetBkMode(hdc, oldBkMode);
    SelectClipRgn(hdc, 0); 
    painter.paintEngine()->releaseDC(hdc);
#endif
}
void Qtitan::draw_line(QPainter& painter, const QLineF& line)
{
    painter.drawLine(line);
}

typedef struct //DTTOPTS
{
    DWORD dwSize;
    DWORD dwFlags;
    COLORREF crText;
    COLORREF crBorder;
    COLORREF crShadow;
    int eTextShadowType;
    POINT ptShadowOffset;
    int iBorderSize;
    int iFontPropId;
    int iColorPropId;
    int iStateId;
    BOOL fApplyOverlay;
    int iGlowSize;
} HLP_DTTOPTS;

#define HLP_TMT_CAPTIONFONT (801)           //TMT_CAPTIONFONT
#define HLP_DTT_COMPOSITED  (1UL << 13)     //DTT_COMPOSITED
#define HLP_DTT_GLOWSIZE    (1UL << 11)     //DTT_GLOWSIZE

#define HLP_WP_CAPTION             1 // WP_CAPTION
#define HLP_WP_MAXCAPTION          5 // WP_MAXCAPTION
#define HLP_CS_ACTIVE              1 // CS_ACTIVE
#define HLP_FS_ACTIVE              1
#define HLP_FS_INACTIVE            2
#define HLP_TMT_FILLCOLORHINT   3821 // TMT_FILLCOLORHINT
#define HLP_TMT_BORDERCOLORHINT 3822 // TMT_BORDERCOLORHINT

typedef struct          //MARGINS       
{
    int cxLeftWidth;    // width of left border that retains its size
    int cxRightWidth;   // width of right border that retains its size
    int cyTopHeight;    // height of top border that retains its size
    int cyBottomHeight; // height of bottom border that retains its size
} QTN_MARGINS;

typedef struct {
  DWORD dwFlags;
  BOOL  fEnable;
  HRGN  hRgnBlur;
  BOOL  fTransitionOnMaximized;
} QTN_DWM_BLURBEHIND;

//A value for the fEnable member has been specified.
#define QTN_DWM_BB_ENABLE 0x00000001 
//A value for the hRgnBlur member has been specified.
#define QTN_DWM_BB_BLURREGION 0x00000002
//A value for the fTransitionOnMaximized member has been specified.
#define QTN_DWM_BB_TRANSITIONONMAXIMIZED 0x00000004


typedef BOOL (WINAPI *PtrDwmDefWindowProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
typedef HRESULT (WINAPI *PtrDwmIsCompositionEnabled)(BOOL* pfEnabled);
typedef HRESULT (WINAPI *PtrDwmExtendFrameIntoClientArea)(HWND hWnd, const QTN_MARGINS* pMarInset);
typedef HRESULT (WINAPI *PtrDwmEnableBlurBehindWindow)(HWND hWnd, const QTN_DWM_BLURBEHIND *pBlurBehind);

typedef HRESULT (WINAPI *PtrDrawThemeTextEx)(HANDLE hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const HLP_DTTOPTS *pOptions);
typedef HANDLE (WINAPI *PtrOpenThemeData)(HWND hwnd, LPCWSTR pszClassList);

static PtrDwmDefWindowProc dwmDefWindowProc = 0;
static PtrDwmIsCompositionEnabled dwmIsCompositionEnabled = 0;
static PtrDwmExtendFrameIntoClientArea dwmExtendFrameIntoClientArea = 0;
static PtrDwmEnableBlurBehindWindow dwmEnableBlurBehindWindow = 0;


static PtrDrawThemeTextEx pDrawThemeTextEx = 0;
static PtrOpenThemeData pOpenThemeData = 0;
/*!
\internal
*/
bool Qtitan::isAirAvailable()
{
    static bool initAir = false;
    if (!initAir)
    {
        initAir = true;
        QLibrary dwmLib(QString::fromLatin1("dwmapi"));
        QLibrary themeLib(QString::fromLatin1("uxtheme"));
        dwmIsCompositionEnabled = (PtrDwmIsCompositionEnabled)dwmLib.resolve("DwmIsCompositionEnabled");
        if (dwmIsCompositionEnabled) 
        {
            dwmDefWindowProc = (PtrDwmDefWindowProc)dwmLib.resolve("DwmDefWindowProc");
            dwmExtendFrameIntoClientArea = (PtrDwmExtendFrameIntoClientArea)dwmLib.resolve("DwmExtendFrameIntoClientArea");
            dwmEnableBlurBehindWindow = (PtrDwmEnableBlurBehindWindow)dwmLib.resolve("DwmEnableBlurBehindWindow");

            pDrawThemeTextEx = (PtrDrawThemeTextEx)themeLib.resolve("DrawThemeTextEx");
            pOpenThemeData = (PtrOpenThemeData)themeLib.resolve("OpenThemeData");
        }
    }

    if (dwmIsCompositionEnabled == Q_NULL)
        return false;

    BOOL enabled;
    dwmIsCompositionEnabled(&enabled);
    return enabled;
}
#if 0
static void qt_add_rect(HRGN &winRegion, const QRect& r)
{
    HRGN rgn = CreateRectRgn(r.left(), r.top(), r.x() + r.width(), r.y() + r.height());
    if (rgn) {
        HRGN dest = CreateRectRgn(0,0,0,0);
        int result = CombineRgn(dest, winRegion, rgn, RGN_OR);
        if (result) {
            DeleteObject(winRegion);
            winRegion = dest;
        }
        DeleteObject(rgn);
    }
}
#endif

/*!
\internal
*/
void Qtitan::paintAirEffect(QPainter& painter, GraphicPrimitive* gp, const QRect& rect, QRegion& air)
{
    if (QPaintDevice* dev = painter.device())
    {
        if (dev->devType() == QInternal::Widget)
        {
            QWidget* widget = static_cast<QWidget*>(dev);
            QWidget* native = widget->topLevelWidget();
            if (native == Q_NULL)
                return;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            QRasterPaintEngine* en = (QRasterPaintEngine *)painter.paintEngine();
#else
            QPaintEngine* en = painter.paintEngine();
#endif
            HDC hdc = en->getDC();
            if (hdc)
            {
                if (isAirAvailable())
                {
                    QRect mapRect = QRect(gp->mapToSite(QPoint(0, 0)), gp->rect().size());
                    GraphicPrimitive* parent = gp->parentGP();
                    while (parent != 0)
                    {
                        QRect temp = QRect(parent->mapToSite(QPoint(parent->borderLeft(), parent->borderTop())), 
                            QSize(parent->rect().width() - parent->borderLeft() - parent->borderRight(), parent->rect().height() - parent->borderTop() - parent->borderBottom()));
                        mapRect = temp.intersected(mapRect);
                        parent = parent->parentGP();
                    }

                    if (!mapRect.isEmpty())
                    {
                        QPoint p = mapRect.topLeft();
                        p = widget->mapTo(native, p);
                        QRect siteRect = QRect(p, mapRect.size());

                        air = air.united(siteRect);

                        {
                            QTN_DWM_BLURBEHIND bb;
                            memset(&bb, 0, sizeof(QTN_DWM_BLURBEHIND));
                            bb.dwFlags = QTN_DWM_BB_ENABLE | QTN_DWM_BB_BLURREGION;
                            bb.fEnable = true;
                            HRGN ar = CreateRectRgn(0, 0, 0, 0);
#if 0
                            for (int i = 0; i < air.rectCount(); ++i)
                                qt_add_rect(ar, air.rects()[i]);
#endif
                            bb.hRgnBlur = ar;
                            dwmEnableBlurBehindWindow((HWND)native->internalWinId(), &bb);
                            DeleteObject(ar);
                        }
        
                        siteRect = mapRect.intersected(rect);
                        p = siteRect.topLeft();
                        p = widget->mapTo(native, p);
                        siteRect = QRect(p, siteRect.size());

                        RECT rc;
                        rc.left    = siteRect.left();
                        rc.top     = siteRect.top();
                        rc.right   = siteRect.right() + 1;
                        rc.bottom  = siteRect.bottom() + 1;
                        COLORREF old = ::SetBkColor(hdc, RGB(0, 0, 0));
                        ::ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
                        ::SetBkColor(hdc, old);
                    }
                }
                en->releaseDC(hdc);
            }
        }
    }
}

/*!
\internal
*/

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
LOGFONT qfont_to_LOGFONT_win(const QFont& f)
{
    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));

    lf.lfHeight = -qRound((double)f.pixelSize());
    lf.lfWidth                = 0;
    lf.lfEscapement        = 0;
    lf.lfOrientation        = 0;
    if (f.weight() == 50)
        lf.lfWeight = FW_DONTCARE;
    else
        lf.lfWeight = (f.weight()*900)/99;
    lf.lfItalic         = f.style() != QFont::StyleNormal;
    lf.lfCharSet        = DEFAULT_CHARSET;

    int strat = OUT_DEFAULT_PRECIS;
    if (f.styleStrategy() & QFont::PreferBitmap) {
        strat = OUT_RASTER_PRECIS;
#ifndef Q_OS_WINCE
    } else if (f.styleStrategy() & QFont::PreferDevice) {
        strat = OUT_DEVICE_PRECIS;
    } else if (f.styleStrategy() & QFont::PreferOutline) {
        strat = OUT_OUTLINE_PRECIS;
    } else if (f.styleStrategy() & QFont::ForceOutline) {
        strat = OUT_TT_ONLY_PRECIS;
#endif
    }

    lf.lfOutPrecision   = strat;

    int qual = DEFAULT_QUALITY;

    if (f.styleStrategy() & QFont::PreferMatch)
        qual = DRAFT_QUALITY;
#ifndef Q_OS_WINCE
    else if (f.styleStrategy() & QFont::PreferQuality)
        qual = PROOF_QUALITY;
#endif

    if (f.styleStrategy() & QFont::PreferAntialias) {
        if (QSysInfo::WindowsVersion >= QSysInfo::WV_XP) {
            qual = CLEARTYPE_QUALITY;
        } else {
            qual = ANTIALIASED_QUALITY;
        }
    } else if (f.styleStrategy() & QFont::NoAntialias) {
        qual = NONANTIALIASED_QUALITY;
    }

    lf.lfQuality        = qual;

    lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;

    int hint = FF_DONTCARE;
    switch (f.styleHint()) {
        case QFont::Helvetica:
            hint = FF_SWISS;
            break;
        case QFont::Times:
            hint = FF_ROMAN;
            break;
        case QFont::Courier:
            hint = FF_MODERN;
            break;
        case QFont::OldEnglish:
            hint = FF_DECORATIVE;
            break;
        case QFont::System:
            hint = FF_MODERN;
            break;
        default:
            break;
    }

    lf.lfPitchAndFamily = DEFAULT_PITCH | hint;

    QString fam = f.family();

    if (fam.isEmpty())
        fam = QStringLiteral("MS Sans Serif");

    if ((fam == QStringLiteral("MS Sans Serif"))
        && (f.style() == QFont::StyleItalic || (-lf.lfHeight > 18 && -lf.lfHeight != 24))) {
        fam = QStringLiteral("Arial"); // MS Sans Serif has bearing problems in italic, and does not scale
    }
    if (fam == QStringLiteral("Courier") && !(f.styleStrategy() & QFont::PreferBitmap))
        fam = QStringLiteral("Courier New");

    memcpy(lf.lfFaceName, fam.utf16(), sizeof(wchar_t) * qMin(fam.length() + 1, 32));  // 32 = Windows hard-coded

    return lf;
}
#endif

HFONT get_font_handle_win(const QFont& f)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    LOGFONT lf = qfont_to_LOGFONT_win(f);
    return CreateFontIndirect(&lf);
#else
    return f.handle();
#endif
}

void release_font_handle_win(HFONT hfont)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    DeleteObject(hfont);
#else
    Q_UNUSED(hfont);
#endif
}

void Qtitan::drawDwmCaptionText(QPainter* painter, const QRect& rect, const QString& strWindowText, bool active)
{
    if (!rect.isValid())
        return;

    HANDLE hTheme = pOpenThemeData((HWND)QApplication::desktop()->winId(), L"WINDOW");
    if (!hTheme) 
       return;

    // Set up a memory DC and bitmap that we'll draw into
    HDC dcMem;
    HBITMAP bmp;
    BITMAPINFO dib;
    memset(&dib, 0, sizeof(BITMAPINFO));
    dcMem = CreateCompatibleDC(/*hdc*/0);

    dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    dib.bmiHeader.biWidth = rect.width();
    dib.bmiHeader.biHeight = -rect.height();
    dib.bmiHeader.biPlanes = 1;
    dib.bmiHeader.biBitCount = 32;
    dib.bmiHeader.biCompression = BI_RGB;

    bmp = CreateDIBSection(/*hdc*/0, &dib, DIB_RGB_COLORS, NULL, NULL, 0);

    // Set up the DC
    HFONT hCaptionFont = get_font_handle_win(painter->font());
    HBITMAP hOldBmp = (HBITMAP)SelectObject(dcMem, (HGDIOBJ) bmp);
    HFONT hOldFont = (HFONT)SelectObject(dcMem, (HGDIOBJ) hCaptionFont);

    // Draw the text!
    HLP_DTTOPTS dto;
    memset(&dto, 0, sizeof(HLP_DTTOPTS));

    const UINT uFormat = DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX;
    RECT rctext = {0, 0, rect.width(), rect.height()};

    dto.dwFlags = HLP_DTT_COMPOSITED;// | HLP_DTT_GLOWSIZE;
    dto.iGlowSize = 5;//glowSize();

    pDrawThemeTextEx(hTheme, dcMem, 0/*getStyle()*/ & WS_MAXIMIZE ? HLP_WP_MAXCAPTION : HLP_WP_CAPTION, active ? HLP_FS_ACTIVE : HLP_FS_INACTIVE, 
       (LPCWSTR)strWindowText.utf16(), -1, uFormat, &rctext, &dto);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QPixmap pxx = ::qt_pixmapFromWinHBITMAP(bmp, HBitmapPremultipliedAlpha);
#else
    QPixmap pxx = QPixmap::fromWinHBITMAP(bmp, QPixmap::PremultipliedAlpha);
#endif
    painter->drawPixmap(rect.left(), rect.top(), pxx);

//  ::BitBlt(hdc, rect.left(), rect.top(), rect.width(), rect.height(), dcMem, 0, 0, SRCCOPY);
    SelectObject(dcMem, (HGDIOBJ) hOldBmp);
    SelectObject(dcMem, (HGDIOBJ) hOldFont);
    DeleteObject(bmp);
    DeleteDC(dcMem);
    release_font_handle_win(hCaptionFont);
}


