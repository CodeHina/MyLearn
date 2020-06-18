#define _GNU_SOURCE     /* To get pthread_getattr_np() declaration */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define handle_error_en(en, msg) \
       do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static void
display_pthread_attr(pthread_attr_t *attr, char *prefix)
{
   int s, i;                //s - 返回值 / i - 分离/非分离状态 - scope - 
   size_t v;                //
   void *stkaddr;           //分配栈的空间的首地址
   struct sched_param sp;   //

   s = pthread_attr_getdetachstate(attr, &i);
   if (s != 0)
       handle_error_en(s, "pthread_attr_getdetachstate");
   printf("%sDetach state        = %s\n", prefix,                           //前缀串
           (i == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" :     //分离态
           (i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE" :     //非分离态
           "???");                                                          //未知

   s = pthread_attr_getscope(attr, &i);                                     //线程作用域
   if (s != 0)
       handle_error_en(s, "pthread_attr_getscope");
   printf("%sScope               = %s\n", prefix,
           (i == PTHREAD_SCOPE_SYSTEM)  ? "PTHREAD_SCOPE_SYSTEM" :          //PTHREAD_SCOPE_SYSTEM - 同系统内的所有线程共同竞争时间片 - 默认值
           (i == PTHREAD_SCOPE_PROCESS) ? "PTHREAD_SCOPE_PROCESS" :         //PTHREAD_SCOPE_PROCESS - 只和同进程的线程竞争时间片
           "???");

   s = pthread_attr_getinheritsched(attr, &i);
   if (s != 0)
       handle_error_en(s, "pthread_attr_getinheritsched");
   printf("%sInherit scheduler   = %s\n", prefix,                       
           (i == PTHREAD_INHERIT_SCHED)  ? "PTHREAD_INHERIT_SCHED" :        //表示新线程继承创建自己的线程的调度策略和调度参数
           (i == PTHREAD_EXPLICIT_SCHED) ? "PTHREAD_EXPLICIT_SCHED" :       //表示显示的使用自定义attr的设置的值
           "???");

   s = pthread_attr_getschedpolicy(attr, &i);
   if (s != 0)
       handle_error_en(s, "pthread_attr_getschedpolicy");
   printf("%sScheduling policy   = %s\n", prefix,                           
           (i == SCHED_OTHER) ? "SCHED_OTHER" :
           (i == SCHED_FIFO)  ? "SCHED_FIFO" :                              //First In First Out - 调度策略
           (i == SCHED_RR)    ? "SCHED_RR" :                                //Round-robin - 轮循调度算法
           "???");

   s = pthread_attr_getschedparam(attr, &sp);
   if (s != 0)
       handle_error_en(s, "pthread_attr_getschedparam");
   printf("%sScheduling priority = %d\n", prefix, sp.sched_priority);       //调度参数

   s = pthread_attr_getguardsize(attr, &v);
   if (s != 0)
       handle_error_en(s, "pthread_attr_getguardsize");
   printf("%sGuard size          = %d bytes\n", prefix, v);                 //警戒缓冲区大小

   s = pthread_attr_getstack(attr, &stkaddr, &v);
   if (s != 0)
       handle_error_en(s, "pthread_attr_getstack");
   printf("%sStack address       = %p\n", prefix, stkaddr);                 //栈基址
   printf("%sStack size          = 0x%zx bytes\n", prefix, v);              //栈大小
}

static void *
thread_start(void *arg)                                                     //线程创建后的回调函数
{
   int s;
   pthread_attr_t gattr;

   /* pthread_getattr_np() is a non-standard GNU extension that
      retrieves the attributes of the thread specified in its
      first argument */

   s = pthread_getattr_np(pthread_self(), &gattr);      //获取已创建线程的属性
   if (s != 0)
       handle_error_en(s, "pthread_getattr_np");

   printf("Thread attributes:\n");
   display_pthread_attr(&gattr, "\t");

   exit(EXIT_SUCCESS);         /* Terminate all threads */
}

int
main(int argc, char *argv[])
{
   pthread_t thr;
   pthread_attr_t attr;
   pthread_attr_t *attrp;      /* NULL or &attr */
   int s;

   attrp = NULL;

   /* If a command-line argument was supplied, use it to set the
      stack-size attribute and set a few other thread attributes,
      and set attrp pointing to thread attributes object */

   if (argc > 1) {
       int stack_size;
       void *sp;

       attrp = &attr;

       s = pthread_attr_init(&attr);                                        //初始化线程attr
       if (s != 0)
           handle_error_en(s, "pthread_attr_init");

       s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);     //设置为分离态
       if (s != 0)
           handle_error_en(s, "pthread_attr_setdetachstate");

       s = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);     //显式继承
       if (s != 0)
           handle_error_en(s, "pthread_attr_setinheritsched");

       stack_size = strtoul(argv[1], NULL, 0);                              //命令行参数指定为设定的栈大小

       s = posix_memalign(&sp, sysconf(_SC_PAGESIZE), stack_size);          //分配一块内存
       if (s != 0)
           handle_error_en(s, "posix_memalign");

       printf("posix_memalign() allocated at %p\n", sp);            

       s = pthread_attr_setstack(&attr, sp, stack_size);                    //线程的栈属性设置
       if (s != 0)
           handle_error_en(s, "pthread_attr_setstack");
   }

   s = pthread_create(&thr, attrp, &thread_start, NULL);                    //按照以上属性设置线程
   if (s != 0)
       handle_error_en(s, "pthread_create");

   if (attrp != NULL) {
       s = pthread_attr_destroy(attrp);                                     //消耗设置的attrp
       if (s != 0)
           handle_error_en(s, "pthread_attr_destroy");
   }

   pause();    /* Terminates when other thread calls exit() */
}
