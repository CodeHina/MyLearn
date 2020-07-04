#include "../include/Server.h"


int main()
{
    Server s;
   
    s.SetConfig();

    s.AcceptClient();
   
    while(1)
        s.Handle();

    return 0;
}
