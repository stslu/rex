#ifndef WINDOW_H
#define WINDOW_H

#include <QCheckBox>
#include <QtitanGrid.h>
#include "demomainwindow.h"

class Window : public DemoMainWindow
{
    Q_OBJECT
public:
    Window();
    QWidget* createSettingsWidget();
private Q_SLOTS:
    void newRowPositionActivated(int index);
    void newRowEffectActivated(int index);
    void fastScrollChanged(int state);
    void dottedLineChanged(int state);
    void selectGridLineStyles(int index);
    void selectViewOrientation(int index);
    void zoomEnabledChanged(int state);
    void zoomIndicatorChanged(int state);
    void WindowsAirSupportChanged(int state);
    void cellButtonAutoRaiseEnabled(int state);
    void transparentBackgroundEnabled(int state);
    void previewExpandButtonEnabled(int state);
    void contextMenu(ContextMenuEventArgs* args);
    void editorLinkActivated(EditorLinkEventArgs* args);
    void cellButtonClicked(CellButtonClickEventArgs* args);
    void printPreview();
    void showCompanyWebSite();
    void setShadeColor(const QColor& color);
    void addPreviewRow();
    void zoomValueChanged(int value);
private:
    Qtitan::Grid* m_grid;
};
#endif
