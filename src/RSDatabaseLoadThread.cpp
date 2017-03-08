#include "RSDatabaseLoadThread.h"
#include "IRSDatabaseAccess.h"

RSDatabaseLoadThread::RSDatabaseLoadThread(QObject *parent) : QThread(parent)
{

}

RSDatabaseLoadThread::~RSDatabaseLoadThread()
{

}

void RSDatabaseLoadThread::run()
{
    IRSDatabaseAccess::Instance()->createDatasetTable();
    IRSDatabaseAccess::Instance()->setDatasetTable();
}
