#ifndef WINDOW_H
#define WINDOW_H

#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QtitanGrid.h>
#include <QSqlTableModel>
#include "demomainwindow.h"

class Window : public DemoMainWindow
{
    Q_OBJECT
public:
    Window();
    void createRelationGrid();
    void createMasterDetailsGrid();
private Q_SLOTS:
    void editorModeChanged(GridEditor* editor);
    void contextMenu(ContextMenuEventArgs* args);
    void printPreview();
    void showCompanyWebSite();
    void setShadeColor(const QColor& color);
private:
    QTabWidget* m_tabs;
    Qtitan::Grid* m_relationGrid;
    Qtitan::Grid* m_masterDetailsGrid;
};

class CountriesDemoModel: public QSqlTableModel
{
public:
    CountriesDemoModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif
