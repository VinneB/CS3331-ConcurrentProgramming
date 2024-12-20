/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 11/29/2023 */
/* PROGRAM ASSIGNMENT 5 */
/* FILE NAME : boat-monitor.h */
/* PROGRAM PURPOSE : */
/* Header file the defines the Boat Monitor class. */
/* ----------------------------------------------------------- */
#ifndef BOAT_MONITOR_H
#define BOAT_MONITOR_H

#include "ThreadClass.h"

class B_Monitor : public Monitor {
    public:
        //B_Monitor(char *Name) : Monitor(Name, HOARE);
        B_Monitor(Semaphore *stdout_lock, int load_c);
        void CannibalArrives(int missionary_id);
        void MissionaryArrives(int missionary_id);
        void BoatReady(int *ret_arr);
        void BoatDone();
    private:
        int c_arrived, m_arrived, isBoatReady, isPassengersReady, passengers[3][2], passenger_i, loads_started, load_c;
        Semaphore *stdout_lock;
        // passengers[i][0] - 1: cannibal ; 0: missionary
        // passengers[i][1] - cannibal/missionary id
        Condition eventPassengersReady, eventBoard_c, eventBoard_m, eventDisembark;

};

#endif