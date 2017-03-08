#ifndef WINDOW_H
#define WINDOW_H

#include <QCheckBox>
#include <QDate>
#include <QtSql>
#include <QtitanGrid.h>
#include "demomainwindow.h"

class Window : public DemoMainWindow
{
    Q_OBJECT
public:
    Window();
    QWidget* createSettingsWidget();
public Q_SLOTS:
    void autoWidthStateChanged(int state);
    void fastScrollChanged(int state);
    void dottedLineChanged(int state);
    void selectGridLineStyles(int index);
    void zoomEnabledChanged(int state);
    void zoomIndicatorChanged(int state);
    void WindowsAirSupportChanged(int state);
    void cellButtonAutoRaiseEnabled(int state);
    void transparentBackgroundEnabled(int state);
    void contextMenu(ContextMenuEventArgs* args);
    void cellButtonClicked(CellButtonClickEventArgs* args);
    void printPreview();
    void showCompanyWebSite();
    void setShadeColor(const QColor& color);
    void zoomValueChanged(int value);
private:
    Qtitan::Grid* m_grid;
};

class StandartDelegateDemoModel: public QAbstractItemModel 
{    
    Q_OBJECT
public:   
    StandartDelegateDemoModel(QObject *parent = 0);
    virtual ~StandartDelegateDemoModel();   
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
        QString title;
        QString genre;
        QString artist;
        int rating;
        DataItem() : rating(0) { }
        DataItem(const QString& t, const QString& g, const QString& a, int r)
        {
            title = t;
            genre = g;
            artist = a;
            rating = r;

        }
    };
    QVector<DataItem> m_values;
};

#endif
