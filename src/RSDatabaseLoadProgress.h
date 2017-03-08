#ifndef RSDATABASELOADPROGRESS_H
#define RSDATABASELOADPROGRESS_H

#include <QObject>
#include <QProgressDialog>

class RSDatabaseLoadProgress : public QObject
{
    Q_OBJECT

public:
    RSDatabaseLoadProgress(QObject *parent = 0);

private:

};

#endif // RSDATABASELOADPROGRESS_H
