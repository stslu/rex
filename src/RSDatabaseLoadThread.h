#ifndef RSDATABASELOADTHREAD_H
#define RSDATABASELOADTHREAD_H

#include <QThread>

class RSDatabaseLoadThread : public QThread
{
    Q_OBJECT

public:
    RSDatabaseLoadThread(QObject* parent = 0);
    ~RSDatabaseLoadThread();

protected:
    void run();
};

#endif // RSDATABASELOADTHREAD_H
