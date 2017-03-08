#ifndef WINDOW_H
#define WINDOW_H

#include <QtitanGrid.h>
#include "demomainwindow.h"
#include <QMdiArea>
#include <QRadioButton>


/* Window */
class Window : public DemoMainWindow
{
    Q_OBJECT
public:
    Window();
    virtual ~Window();

protected:
    QWidget* createSettingsWidget();
    void setShadeColor(const QColor& color);
private Q_SLOTS:
    void addGrid();
    void removeGrid();
    void setMDIViewMode(int index);
    void subWindowActivated(QMdiSubWindow*);
private:
    QMdiArea* m_mdiArea;
};


class CustomDataModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    CustomDataModel(QObject *parent = 0);
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

#endif// WINDOW_H

