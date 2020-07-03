#include "../include/Client.h"


int main()
{

    Client c;
   

    c.SetConfig();


    c.Connect(); 

    while(1)
        c.handle();

    return 0;
}
