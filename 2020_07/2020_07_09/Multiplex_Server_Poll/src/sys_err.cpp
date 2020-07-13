#include "../include/sys_err.h"
#include <iostream>


void perr_exit(const char* name_of_efunc)
{
    perror(name_of_efunc);
    exit(-1);
}
