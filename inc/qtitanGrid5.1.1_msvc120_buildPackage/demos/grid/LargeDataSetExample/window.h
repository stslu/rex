#ifndef WINDOW_H
#define WINDOW_H

#include <QCheckBox>
#include <QDate>
#include <QtitanGrid.h>
#include "demomainwindow.h"

class Window : public DemoMainWindow
{
    Q_OBJECT
public:
    Window();
    QWidget* createSettingsWidget();
private Q_SLOTS:
    void autoWidthStateChanged(int state);
    void fastScrollChanged(int state);
    void dottedLineChanged(int state);
    void selectGridLineStyles(int index);
    void selectViewOrientation(int index);
    void zoomEnabledChanged(int state);
    void zoomIndicatorChanged(int state);
    void WindowsAirSupportChanged(int state);
    void cellButtonAutoRaiseEnabled(int state);
    void frozenRowsEnabled(int state);
    void transparentBackgroundEnabled(int state);
    void contextMenu(ContextMenuEventArgs* args);
    void cellButtonClicked(CellButtonClickEventArgs* args);
    void printPreview();
    void showCompanyWebSite();
    void editorModifying(GridEditor* editor);
    void editorValidating(EditorValidationEventArgs* args);
    void setShadeColor(const QColor& color);
    void zoomValueChanged(int value);
private:
    Qtitan::Grid* m_grid;
};

class LargeDataSetModel: public QAbstractItemModel 
{    
    Q_OBJECT
public:   
    LargeDataSetModel(QObject *parent = 0);
    virtual ~LargeDataSetModel();   
    virtual QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const;
    virtual QModelIndex parent(const QModelIndex &child) const;  
    virtual bool hasChildren(const QModelIndex &parent) const;   
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;  
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;   
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
private:
    struct DataItem
    {
        bool v0;
        int v1;
        QString v2;
        QDate v3;
        QTime v4;
        DataItem() : v0(false), v1(0) {}
    };
    QVector<DataItem> m_values;
};

#endif
