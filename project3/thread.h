/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 10/16/2023 */
/* PROGRAM ASSIGNMENT 3 */
/* FILE NAME : thread.h */
/* PROGRAM PURPOSE : */
/* This is a header file. It defines the PS_Thread class and declares external global variables b and n. */
/* ----------------------------------------------------------- */
#ifndef P3_THREAD
#define P3_THREAD

#include "ThreadClass.h"
class PS_Thread : public Thread {
    public:
        PS_Thread (int i, int j);
    private :
        int i, j;
        void ThreadFunc();
};

// 2D array containing prefix sum
extern int n;
extern int *b;

#endif