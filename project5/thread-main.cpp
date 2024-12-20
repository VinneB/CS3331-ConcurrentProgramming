/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 11/29/2023 */
/* PROGRAM ASSIGNMENT 5 */
/* FILE NAME : thread-main.cpp */
/* PROGRAM PURPOSE : */
/* This is the main file for assignment 5. It spawns threads, initializes the boat monitor, and program start up. */
/* ----------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "boat-monitor.h"
#include "thread.h"

// Default arg values in the case that arg equals 0
#define MISSIONARY_C_DEF 8
#define CANNIBAL_C_DEF 8
#define LOAD_C_DEF 5

void int_handler (int signum);

int main (int argc, char **argv) {
    int missionary_c, cannibal_c, load_c;

    // format check
    if (argc != 4) {
        fprintf(stderr, "format: ./prog5 <# cannibals> <# missionaries> <# boat loads>\n");
        exit(1);
    }
    // shell arg processing
    missionary_c = atoi(argv[1]);
    cannibal_c = atoi(argv[2]);
    load_c = atoi(argv[3]);

    // valid arg check
    if ((missionary_c < 0) || (cannibal_c < 0) || (load_c < 0)) {
        fprintf(stderr, "shell arguments must be greater than zero\n");
        exit(1);
    }

    // default value assignment
    if (missionary_c == 0) {
        missionary_c = MISSIONARY_C_DEF;
    }
    if (cannibal_c == 0) {
        cannibal_c = CANNIBAL_C_DEF;
    }
    if (load_c == 0) {
        load_c = LOAD_C_DEF;
    }

    P_Thread *m_threads[missionary_c];
    P_Thread *c_threads[cannibal_c];
    B_Thread *b_thread;

    Semaphore *stdout_lock = new Semaphore("Stdout Lock", 1);
    B_Monitor *boat = new B_Monitor(stdout_lock, load_c);
    
    // handle interrupts
    struct sigaction int_action;
    int_action.sa_handler = int_handler;
    sigemptyset(&int_action.sa_mask);
    int_action.sa_flags = 0;
    sigaction(SIGINT, &int_action, NULL);

    // create threads
    b_thread = new B_Thread(stdout_lock, boat);
    for (int i = 0; i < missionary_c; i++) {
        m_threads[i] = new P_Thread(i+1, 0, stdout_lock, boat);
    }
    for (int i = 0; i < cannibal_c; i++) {
        c_threads[i] = new P_Thread(i+1, 1, stdout_lock, boat);
    }

    // run threads
    b_thread->Begin();
    for (int i = 0; i < missionary_c; i++) {
        m_threads[i]->Begin();
    }
    for (int i = 0; i < cannibal_c; i++) {
        c_threads[i]->Begin();
    }

    for (int i = 0; i < cannibal_c+missionary_c; i++) {
        if (i < cannibal_c) {
            c_threads[i]->Join();
        } else {
            m_threads[i-cannibal_c]->Join();
        }
    }

}

/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : int_handler */
/* Terminates thread */
/* ----------------------------------------------------------- */
void int_handler (int signum) {
    Exit();
}