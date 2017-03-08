#include <Signaler.h>


Signaler* Signaler::s_instance = 0;

 Signaler* Signaler::instance(QObject* parent)
{
    if(!s_instance)
        s_instance = new Signaler(parent);

    return s_instance;
}


 void  Signaler::killInstance()
{
    if(s_instance)
    {
        delete s_instance;
        s_instance = 0;
    }
}
