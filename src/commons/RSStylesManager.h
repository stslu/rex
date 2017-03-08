#ifndef RSSTYLESMANAGER_H
#define RSSTYLESMANAGER_H

#include <QObject>

class RSStylesManager : public QObject
{
    Q_OBJECT

private:
    RSStylesManager(QObject *parent = 0);

public:
    ~RSStylesManager();
    static RSStylesManager* Instance(QObject *parent = 0);

private:
    void createObjects();
    void createConnections();

private:
    static RSStylesManager* m_instance;
};

#endif // RSSTYLESMANAGER_H
