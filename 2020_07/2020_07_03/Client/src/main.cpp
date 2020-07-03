#include "../include/Client.h"


int main()
{

   Client c;
   

   c.SetConfig();


   c.Connect(); 

   c.handle();

    return 0;
}
