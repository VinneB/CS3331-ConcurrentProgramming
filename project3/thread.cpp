/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 10/16/2023 */
/* PROGRAM ASSIGNMENT 3 */
/* FILE NAME : thread.cpp */
/* PROGRAM PURPOSE : */
/* This file implements the functionality of the threads used in thread-main.cpp. Each thread computes one iteration of the prefix sum algorithm on a single 
    integer read in. */
/* ----------------------------------------------------------- */
#include "thread.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
using namespace std;

/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : PS_Thread */
/* constructor for the PS_Thread class. Implementation is self-explanatory */
/* PARAMETER USAGE : */
/* i - prefix sum algorithm interation number   ;   j - thread number */
PS_Thread::PS_Thread(int i, int j) {
    this->i = i;
    this->j = j;
    char async_printbuf[256];

    sprintf(async_printbuf, "\tThread %d Created\n", j);
    write(1, async_printbuf, strlen(async_printbuf)+1);

}


/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : ThreadFunc */
/* defines the functionality run when PS_Thread instance is run. In this instance, it computes a single interation, for a 
    single number, of the prefix-sum algorithm */
void PS_Thread::ThreadFunc() {
    Thread::ThreadFunc();
    char async_printbuf[256];
    if ((j - (1<<(i-1))) < 0) {
        b[(i*n)+j] = b[((i-1)*n)+j];
        sprintf(async_printbuf, "\tThread %d copies x[%d]\n", j, j);
        write(1, async_printbuf, strlen(async_printbuf)+1);
    } else {
        b[(i*n)+j] = b[(n*(i-1))+(j - (1<<(i-1)))] + b[(n*(i-1))+j];
        sprintf(async_printbuf, "\tThread %d computes x[%d] + x[%d]\n", j, j, (j - (1<<(i-1))));
        write(1, async_printbuf, strlen(async_printbuf)+1);
    }
    sprintf(async_printbuf, "\tThread %d Exits\n", j);
    write(1, async_printbuf, strlen(async_printbuf)+1);
    Exit();
}