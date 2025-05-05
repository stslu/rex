#ifndef RSPICTOMANAGER_H
#define RSPICTOMANAGER_H

#include <QObject>
#include <QIcon>
#include <QtAwesome>

class RSPictoManager : public QObject
{
    Q_OBJECT

private:
    RSPictoManager(QObject *parent = 0);

public:
    ~RSPictoManager();
    static RSPictoManager* Instance(QObject *parent = 0);

private:
    void createObjects();
    void createConnections();

public:
    void setColor(const QVariant& color);
    void setActiveColor(const QVariant& color);
    QIcon getIcon(fa::icon faEnum, const QVariant &color = "black");

private:
    static RSPictoManager* m_instance;

    QtAwesome* m_awesomePicto;
};


#endif // RSPICTOMANAGER_H
