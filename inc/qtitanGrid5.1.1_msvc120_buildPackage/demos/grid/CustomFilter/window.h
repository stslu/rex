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
private Q_SLOTS:
    void contextMenu(ContextMenuEventArgs* args);
    void showCompanyWebSite();
    void applyFilter();
    void setShadeColor(const QColor& color);
private:
    Qtitan::Grid* m_grid;
};

class CustomFilterCondition: public GridFilterCondition {
public:
    CustomFilterCondition(GridFilter* filter);
    virtual bool isTrue(int modelRow);
    virtual GridFilterCondition* clone() const;
    virtual QString createPresentation() const;
    virtual int conditionCount() const;
public:
    void addRow(int modelRowIndex);
    void removeRow(int modelRowIndex);
private:
    QSet<int> m_modelRows;
};

class CustomFilterModel: public QAbstractItemModel 
{    
    Q_OBJECT
public:   
    CustomFilterModel(QObject *parent = 0);
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
        int v0;
        QString v1;
        QDate v2;
        QTime v3;
        DataItem() : v0(0) {}
    };
    QVector<DataItem> m_values;
};

#endif
