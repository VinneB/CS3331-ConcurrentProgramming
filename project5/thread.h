/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 11/29/2023 */
/* PROGRAM ASSIGNMENT 5 */
/* FILE NAME : thread.h */
/* PROGRAM PURPOSE : */
/* Header file that defines the boat and passenger thread classes. */
/* ----------------------------------------------------------- */
#ifndef THREAD_H
#define THREAD_H

#include "ThreadClass.h"
#include "boat-monitor.h"

#define DELAY_MOD 1000000

class P_Thread : public Thread {
    public:
        P_Thread (int id, int isCannibal, Semaphore *stdout_lock, B_Monitor *boat);
        void ISpaces();
    private:
        int id, isCannibal;
        Semaphore *stdout_lock;
        B_Monitor *boat;
        void ThreadFunc();
};

class B_Thread : public Thread {
    public:
        B_Thread (Semaphore *stdout_lock, B_Monitor *boat);
    private:
        Semaphore *stdout_lock;
        B_Monitor *boat;
        void ThreadFunc();
};

#endif