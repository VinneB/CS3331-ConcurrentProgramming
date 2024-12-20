/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 11/29/2023 */
/* PROGRAM ASSIGNMENT 5 */
/* FILE NAME : thread.cpp */
/* PROGRAM PURPOSE : */
/* This file implements the functionality of the threads used in thread-main.cpp i.e. the threads defined in thread.h */
/* ----------------------------------------------------------- */
#include "thread.h"
#include "ThreadClass.h"

void P_Thread::ThreadFunc() {
    stdout_lock->Wait();
    ISpaces();
    if (isCannibal) {
        printf("Cannibal %d starts\n", id);
    } else {
        printf("Missionary %d starts\n", id);
    }
    stdout_lock->Signal();
    // Main execution loop
    while (true) {
        Delay(rand() % DELAY_MOD);
        if (isCannibal) {
            stdout_lock->Wait();
            ISpaces();
            printf("Cannibal %d arrives\n", id);
            stdout_lock->Signal();
            boat->CannibalArrives(id);
        } else {
            stdout_lock->Wait();
            ISpaces();
            printf("Missionary %d arrives\n", id);
            stdout_lock->Signal();
            boat->MissionaryArrives(id);
        }
    }
}

void B_Thread::ThreadFunc() {
    stdout_lock->Wait();
    printf("***** BOAT thread starts\n");
    stdout_lock->Signal();
    while (true) {
        Delay(rand() % DELAY_MOD);

        stdout_lock->Wait();
        printf("***** The boat is ready\n");
        stdout_lock->Signal();

        int ret_arr[7];
        boat->BoatReady(ret_arr);

        stdout_lock->Wait();
        printf("***** Boat load (%d): Passenger list (%c%d, %c%d, %c%d)\n",  ret_arr[6],
            (ret_arr[0]) ? 'c' : 'm', ret_arr[1], (ret_arr[2]) ? 'c' : 'm', ret_arr[3], (ret_arr[4]) ? 'c' : 'm', ret_arr[5]);
        stdout_lock->Signal();

        Delay(rand() % DELAY_MOD);
        stdout_lock->Wait();
        printf("***** Boat load (%d): Completed\n", ret_arr[6]);
        stdout_lock->Signal();

        boat->BoatDone();
    }
}

P_Thread::P_Thread(int id, int isCannibal, Semaphore *stdout_lock, B_Monitor *boat) {
    this->id = id;
    this->isCannibal = isCannibal;
    this->stdout_lock = stdout_lock;
    this->boat = boat;
}

B_Thread::B_Thread(Semaphore *stdout_lock, B_Monitor *boat) {
    this->stdout_lock = stdout_lock;
    this->boat = boat;    
}

/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : ISpaces */
/* prints a number of spaces equal to this->i */
/* ----------------------------------------------------------- */
void P_Thread::ISpaces() {
    for (int j= 0; j < this->id; j++) {
        printf(" ");
    }
}