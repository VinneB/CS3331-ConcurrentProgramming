/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 11/8/2023 */
/* PROGRAM ASSIGNMENT 4 */
/* FILE NAME : thread-support.cpp */
/* PROGRAM PURPOSE : */
/* This file implements thread functionality for the child and parent thread. */
/* ----------------------------------------------------------- */

#ifndef THREAD_H
#include "thread.h"
#endif

/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : ISpaces */
/* prints a number of spaces equal to this->i */
/* PARAMETER USAGE : */
/* thread - the thread calling this function */
/* FUNCTION CALLED: ISpaces */
/* ----------------------------------------------------------- */
void ready_to_eat(E_Thread *thread) {
    print_mutex->Wait();
    thread->ISpaces();
    printf("Baby eagle %d is ready to eat.\n", thread->i);
    print_mutex->Signal();
    resource_mutex->Wait();
    if (resources <= 0 && sleep_signaled == 0) {
        if (cycle != 0) {
            print_mutex->Wait();
            thread->ISpaces();
            printf("Baby eagle %d sees all the feeding pots are empty and wakes up the mother.\n", thread->i);
            print_mutex->Signal();
            sleep_signaled = thread->i;
        }
        do_sleep->Signal();
    }
    resource_mutex->Signal();
    s_resources->Wait();
    resource_mutex->Wait();
    thread->claimed_resource = pot_num++;
    resource_mutex->Signal();
}

/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : ISpaces */
/* prints a number of spaces equal to this->i */
/* PARAMETER USAGE : */
/* thread - the thread calling this function */
/* ----------------------------------------------------------- */
void finish_eating(E_Thread *thread) {
    print_mutex->Wait();
    thread->ISpaces();
    printf("Baby eagle %d finishes eating.\n", thread->i);
    print_mutex->Signal();
    resource_mutex->Wait();
    resources--;
    thread->claimed_resource = 0;
    resource_mutex->Signal();
}

/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : ISpaces */
/* prints a number of spaces equal to this->i */
/* PARAMETER USAGE : */
/* thread - the thread calling this function */
/* ----------------------------------------------------------- */
void goto_sleep(E_Thread *thread) {
    print_mutex->Wait();
    printf("Mother eagle takes a nap\n");
    print_mutex->Signal();
    do_sleep->Wait();
    print_mutex->Wait();
    printf("Mother eagle is awoke by baby eagle %d and starts preparing food.\n", sleep_signaled);
    print_mutex->Signal();
}

/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : ISpaces */
/* prints a number of spaces equal to this->i */
/* PARAMETER USAGE : */
/* thread - the thread calling this function */
/* FUNCTION CALLED: ThreadFunc, ready_to_eat, finish_eating */
/* ----------------------------------------------------------- */
void food_ready(E_Thread *thread) {
    thread->cycle_n++;
    print_mutex->Wait();
    printf("Mother eagle says \"Feeding (%d)\"\n", thread->cycle_n);
    print_mutex->Signal();
    resource_mutex->Wait();
    for (int j = 0; j < thread->resource_tot; j++) {
        s_resources->Signal();
        resources++;
    }
    pot_num = 1;
    sleep_signaled = 0;
    cycle--;
    resource_mutex->Signal();
    if (cycle == 0) {
        do_sleep->Wait();
        print_mutex->Wait();
        printf("Mother eagle retires after serving %d feedings. Game is over!!!\n", thread->cycle_n);
        print_mutex->Signal();
        end_main->Signal();
        thread->Exit();
    }
}