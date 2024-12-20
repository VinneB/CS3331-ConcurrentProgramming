/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 11/8/2023 */
/* PROGRAM ASSIGNMENT 4 */
/* FILE NAME : thread.h */
/* PROGRAM PURPOSE : */
/* This is the header file which defines the E_Thread class, extern control variables, and prototypes for thread-support functions. */
/* ----------------------------------------------------------- */

#ifndef THREAD_H
#define THREAD_H

#include "ThreadClass.h"

#define SIG_HANDLE_THREAD_CNT 64
#define DELAY_MOD 1000000

extern Semaphore *s_resources;
extern Semaphore *do_sleep;
extern Semaphore *end_main;
extern Semaphore *resource_mutex;
extern Semaphore *print_mutex;

extern int resources;
extern int sleep_signaled;
extern int pot_num;
extern int cycle;

class E_Thread : public Thread {
    public:
        int resource_tot, claimed_resource, i, cycle_n;
        E_Thread (int i, bool isSpecial, int resource_tot);
        void ISpaces();
    private:
        int isSpecial;
        void ThreadFunc();
};


void ready_to_eat(E_Thread *thread);

void finish_eating(E_Thread *thread);

void goto_sleep(E_Thread *thread);

void food_ready(E_Thread *thread);


#endif