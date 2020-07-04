#include "../include/Client.h"


int main()
{

    Client c;


    c.SetConfig();

    c.ConnectServer(); 

    while(1)
        c.Handle();

    return 0;
}
