/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 11/29/2023 */
/* PROGRAM ASSIGNMENT 5 */
/* FILE NAME : thread.cpp */
/* PROGRAM PURPOSE : */
/* This file implements the functionality of the threads used in thread-main.cpp i.e. the threads defined in thread.h */
/* ----------------------------------------------------------- */
#ifndef THREAD_H
#include "thread.h"
#endif
using namespace std;

/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : E_Thread */
/* constructor for the E_Thread class. Implementation is self-explanatory */
/* PARAMETER USAGE : */
/* i - thread id   ;   isSpecial - designates whether thread is the 'mother'   ;   resource_tot - the number of resources the threads are competing for */
/* ----------------------------------------------------------- */
E_Thread::E_Thread(int i, bool isSpecial, int resource_tot) {
    this->i = i;
    this->isSpecial = isSpecial;
    this->resource_tot = resource_tot;

}


/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : ThreadFunc */
/* defines the functionality run when E_Thread instance is run. Functionality depends on whether or not 'isSpecial' is true.*/
/* FUNCTION CALLED: ISpaces, goto_sleep, food_ready, ready_to_eat, finish_eating */
/* ----------------------------------------------------------- */
void E_Thread::ThreadFunc() {
    print_mutex->Wait();
    if (isSpecial) {
        printf("Mother eagle started.\n");
    } else {
        ISpaces();
        printf("Baby eagle %d started.\n", this->i);
    }
    print_mutex->Signal();

    if (isSpecial) {
        this->cycle_n = 0;
        while (true) {
            // Wait until signaled
            goto_sleep(this);
            Delay(rand() % DELAY_MOD);
            // Refill resources
            food_ready(this);
            Delay(rand() % DELAY_MOD);
        }
    } else {
        while(true) {
            Delay(rand() % DELAY_MOD);
            // Child thread attempts to gain access to a resource
            ready_to_eat(this);
            print_mutex->Wait();
            this->ISpaces();
            printf("Baby eagle %d is eating using feeding pot %d\n", this->i, this->claimed_resource);
            print_mutex->Signal();
            Delay(rand() % DELAY_MOD);
            // Returns empty resource
            finish_eating(this);
        }
    }
}

/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : ISpaces */
/* prints a number of spaces equal to this->i */
/* ----------------------------------------------------------- */
void E_Thread::ISpaces() {
    for (int j= 0; j < this->i; j++) {
        printf(" ");
    }
}