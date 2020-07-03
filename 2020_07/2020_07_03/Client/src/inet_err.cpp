#include <iostream>

void SysErr(const char* nfunc, int ret)
{
    if(ret == -1)
    {
        perror(nfunc);
        exit(-1);
    }
}

