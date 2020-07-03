#include "../include/Server.h"


int main()
{
    Server s;
   
    s.SetConfig();

    s.Accept();
   
    //while(1)
        s.handle();

    return 0;
}
