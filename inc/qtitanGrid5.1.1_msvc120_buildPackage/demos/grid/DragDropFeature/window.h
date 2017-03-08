#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTreeView>
#include <QtitanGrid.h>

#include "demomainwindow.h"

class Window : public DemoMainWindow
{
    Q_OBJECT
public:
    Window();
private Q_SLOTS:
    void newPolicyActivated(int state);
    void fastScrollChanged(int state);
    void dottedLineChanged(int state);
    void selectGridLineStyles(int index);
    void zoomEnabledChanged(int state);
    void zoomIndicatorChanged(int state);
    void WindowsAirSupportChanged(int state);
    void cellButtonAutoRaiseEnabled(int state);
    void frozenRowsEnabled(int state);
    void contextMenu(ContextMenuEventArgs* args);
    void cellButtonClicked(CellButtonClickEventArgs* args);
    void printPreview();
    void showCompanyWebSite();
    void setShadeColor(const QColor& color);
    void zoomValueChanged(int value);
private:
    Qtitan::Grid* m_grid1;
    Qtitan::Grid* m_grid2;
    QTreeView* m_grid3;
};

#endif // MAINWINDOW_H
