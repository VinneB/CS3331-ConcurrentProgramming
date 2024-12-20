/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 11/8/2023 */
/* PROGRAM ASSIGNMENT 4 */
/* FILE NAME : thread-main.cpp */
/* PROGRAM PURPOSE : */
/* This is the main file for assignment 4. It spawns threads, initializes control variables, and handles graceful termination. */
/* ----------------------------------------------------------- */
#ifndef THREAD_H
#include "thread.h"
#endif
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

Semaphore *s_resources;
Semaphore *do_sleep;
Semaphore *end_main;
Semaphore *resource_mutex;
Semaphore *print_mutex;

int resources = 0;
int sleep_signaled = 0;
int pot_num = 0;
int cycle = 0;

void int_handler(int signum);

int main(int argc, char **argv) {
    if (argc != 4) {
        write(2, "format: ./prog4 <# pots> <# babies> <# feedings>\n", 49);
        exit(1);
    }

    srand(time(NULL));

    // Handle Interrupts
    struct sigaction int_action;
    int_action.sa_handler = int_handler;
    sigemptyset(&int_action.sa_mask);
    int_action.sa_flags = 0;
    sigaction(SIGINT, &int_action, NULL);

    // Execution Paramter Initialization (resources = m, thread_count = n, t)
    int tot_resources, thread_count, t;
    tot_resources = atoi(argv[1]); thread_count = atoi(argv[2]); t = atoi(argv[3]);
    E_Thread *threads[thread_count+1];
    cycle = t;
    

    // Sync Tools (Initalize external global semaphores)
    s_resources = new Semaphore("s_resources", 0);
    do_sleep = new Semaphore("do_sleep", 0);
    end_main = new Semaphore("end_main", 0);
    resource_mutex= new Semaphore("resource_mutex", 1);
    print_mutex = new Semaphore("print_mutex", 1);

    printf("MAIN: There are %d baby eagles, %d feeding pots, and %d feedings.\nMAIN: Game starts!!!!!\n", thread_count, tot_resources, t);

    // Create mother and children threads
    threads[0] = new E_Thread(0, true, tot_resources);
    for (int i = 1; i <= thread_count; i++) {
        threads[i] = new E_Thread(i, false, tot_resources);
    }

    // Run mother and children threads
    threads[0]->Begin();
    for (int i = 1; i <= thread_count; i++) {
        threads[i]->Begin();
    }

    // Graceful termination (uses SIGINT to get all threads to exit)
    end_main->Wait();

    kill(getpid(), SIGINT);
    Exit();

}

/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : int_handler */
/* Terminates thread */
/* ----------------------------------------------------------- */
void int_handler (int signum) {
    Exit();
}