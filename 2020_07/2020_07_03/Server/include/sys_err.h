#ifndef __SYS_ERR_H_
#define __SYS_ERR_H_

/* 对常用的系统函数的返回值进行出错判断 */

/* 1.失败返回-1,并设置errno */
void perr_exit(const char* name_of_efunc);





#endif
