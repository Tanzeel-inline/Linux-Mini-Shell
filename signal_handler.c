#include "signal_handler.h"


void signalhandler(int num)
{
        //Childpgid = 0 means that there's no child
        if ( childpgid == 0 )
        {
                return;
        }
        //Kill child with signal passed
        kill(child,num);
}