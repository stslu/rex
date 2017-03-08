#ifndef IRSEXPORTMANAGER
#define IRSEXPORTMANAGER

class IRSExportManager
{
public:
    virtual void setProgressBarRate(double)  = 0;
    virtual ~IRSExportManager(){}
};

#endif // IRSEXPORTMANAGER

