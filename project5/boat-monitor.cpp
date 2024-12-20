/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 11/29/2023 */
/* PROGRAM ASSIGNMENT 5 */
/* FILE NAME : boat-monitor.cpp */
/* PROGRAM PURPOSE : */
/* This file implements the constructor and procedures for boat monitor. */
/* ----------------------------------------------------------- */
#include "boat-monitor.h"

B_Monitor::B_Monitor(Semaphore *stdout_lock, int load_c)
    : eventPassengersReady("eventPassengersReady"), eventBoard_c("eventBoard_c"), eventBoard_m("eventBoard_m"), eventDisembark("eventDisembark")
{
    MonitorBegin();
    c_arrived = 0;
    m_arrived = 0;
    isBoatReady = 0;
    isPassengersReady = 0;
    passenger_i = 0;
    loads_started = 0;
    this->load_c = load_c;
    this->stdout_lock = stdout_lock;
    MonitorEnd();
}

void B_Monitor::CannibalArrives (int cannibal_id) { // Possible combs: 3 cannibals, 2 missionaries and 1 cannibal
    /*
    MonitorBegin();
    c_arrived++;
    if (isPassengersReady == 0 && m_arrived >= 2 && c_arrived >= 2) { // 2 missionaries and 1 cannibal ; calling cannibal not included in boarding
        eventBoard_m.Signal(); eventBoard_m.Signal(); eventBoard_c.Signal();
        // Let the boat thread know that passengers are ready
        isPassengersReady = 1; eventPassengersReady.Signal();

        eventBoard_c.Wait(); // Wait to board the boat
        // Enters this block if released in order to board
        passengers[passenger_i][0] = 1; passengers[passenger_i][1] = cannibal->id; // Add calling cannibal to passengers list
        passenger_i++;
        eventDisembark.Wait(); // Calling cannibal waits for boat to finish voyage

    } else if (isPassengersReady == 0 && m_arrived >=2 && c_arrived = 1) { // 2 missionaries and 1 cannibal; calling cannibal included in boarding
        eventBoard_m.Signal(); eventBoard_m.Signal(); // Add two missionaries to the passengers list
        passengers[2][0] = 1; passengers[2][1] = cannibal->id; // Add calling cannibal to passengers list
        // Let the boat thread know that passengers are ready
        isPassengersReady = 1; eventPassengersReady.Signal();

        eventDisembark.Wait(); // Calling cannibal waits for boat to finish voyage

    } else if (isPassengersReady == 0 && c_arrived >= 4) { // 3 cannibals ; calling cannibal not included in boarding
        eventBoard_c.Signal(); eventBoard_c.Signal(); eventBoard_c.Signal();
        // Let the boat thread know that passengers are ready
        isPassengersReady = 1; eventPassengersReady.Signal();

        eventBoard_c.Wait(); // Wait to board the boat
        // Enters this block if released in order to board
        passengers[passenger_i][0] = 1; passengers[passenger_i][1] = cannibal->id; // Add calling cannibal to passengers list
        passenger_i++;
        eventDisembark.Wait(); // Calling cannibal waits for boat to finish voyage

    } else if (isPassengersReady == 0 && c_arrived == 3) { // 3 cannibals ; calling cannibal included in boarding
        eventBoard_m.Signal(); eventBoard_m.Signal(); // Add two missionaries to the passengers list
        passengers[2][0] = 1; passengers[2][1] = cannibal->id; // Add calling cannibal to passengers list
        // Let the boat thread know that passengers are ready
        isPassengersReady = 1; eventPassengersReady.Signal();

        eventDisembark.Wait(); // Calling cannibal waits for boat to finish voyage

    } else { // This is true if the ready passengers do not constitute a suitable load
        eventBoard_c.Wait();
        // Enters this block if released in order to board
        passengers[passenger_i][0] = 1; passengers[passenger_i][1] = cannibal->id; // Add calling cannibal to passengers list
        passenger_i++;
        eventDisembark.Wait(); // Calling cannibal waits for boat to finish voyage
    }

    MonitorEnd();
    */
    MonitorBegin();
    c_arrived++;
    if (isPassengersReady == 0 && ((c_arrived == 3) || ((m_arrived >= 2) && (c_arrived == 1)))) { // calling cannibal included in boarding
        // Release other passengers
        if (c_arrived == 3) { // 3 cannibals
            eventBoard_c.Signal(); eventBoard_c.Signal();
        } else { // 2 missionaries and 1 cannibal
            eventBoard_m.Signal(); eventBoard_m.Signal();
        }

        // Add calling cannibal to passengers list
        passengers[2][0] = 1; passengers[2][1] = cannibal_id;

        // Let the boat thread know that passengers are ready
        isPassengersReady = 1; eventPassengersReady.Signal();

        // Calling cannibal waits for boat to finish voyage
        eventDisembark.Wait();

    } else { // calling cannibal not included in boarding (this shouldn't occur but it's included just in case)
        if ((isPassengersReady == 0) && (c_arrived > 3)) { // 3 cannibals
            // Release other passengers
            eventBoard_c.Signal(); eventBoard_c.Signal(); eventBoard_c.Signal();
            // Let the boat thread know that passengers are ready
            isPassengersReady = 1; eventPassengersReady.Signal();
        } else if ((isPassengersReady == 0) && ((m_arrived >= 2) && (c_arrived > 1))) { // 2 missionaries and 1 cannibal
            // Release other passengers
            eventBoard_c.Signal(); eventBoard_m.Signal(); eventBoard_m.Signal();
            // Let the boat thread know that passengers are ready
            isPassengersReady = 1; eventPassengersReady.Signal();
        }

        // Wait to be released
        eventBoard_c.Wait();
        // Enters this block if released in order to board
        passengers[passenger_i][0] = 1; passengers[passenger_i][1] = cannibal_id; // Add calling cannibal to passengers list
        passenger_i++;
        c_arrived--;
        eventDisembark.Wait(); // Calling missionary waits for boat to finish voyage
    }

    MonitorEnd();
}

void B_Monitor::MissionaryArrives (int missionary_id) { // Possible combs: 3 missionaries, 2 missionaries and 1 cannibal
    MonitorBegin();
    m_arrived++;
    if (isPassengersReady == 0 && ((m_arrived == 3) || ((m_arrived == 2) && (c_arrived >= 1)))) { // calling missionary included in boarding
        // Release other passengers
        if (m_arrived == 3) { // 3 missionaries
            eventBoard_m.Signal(); eventBoard_m.Signal();
        } else { // 2 missionaries and 1 cannibal
            eventBoard_c.Signal(); eventBoard_m.Signal();
        }

        // Add calling missionary to passengers list
        passengers[2][0] = 0; passengers[2][1] = missionary_id;

        // Let the boat thread know that passengers are ready
        isPassengersReady = 1; eventPassengersReady.Signal();

        // Calling missionary waits for boat to finish voyage
        eventDisembark.Wait();

    } else { // calling missionary not included in boarding (this shouldn't occur but it's included just in case)
        if ((isPassengersReady == 0) && (m_arrived > 3)) { // 3 missionaries
            // Release other passengers
            eventBoard_m.Signal(); eventBoard_m.Signal(); eventBoard_m.Signal();
            // Let the boat thread know that passengers are ready
            isPassengersReady = 1; eventPassengersReady.Signal();
        } else if ((isPassengersReady == 0) && ((m_arrived > 2) && (c_arrived >= 1))) { // 2 missionaries and 1 cannibal
            // Release other passengers
            eventBoard_c.Signal(); eventBoard_m.Signal(); eventBoard_m.Signal();
            // Let the boat thread know that passengers are ready
            isPassengersReady = 1; eventPassengersReady.Signal();
        }

        // Wait to be released
        eventBoard_m.Wait();
        // Enters this block if released in order to board
        passengers[passenger_i][0] = 0; passengers[passenger_i][1] = missionary_id; // Add calling missionary to passengers list
        passenger_i++;
        m_arrived--;
        eventDisembark.Wait(); // Calling missionary waits for boat to finish voyage
    }

    MonitorEnd();
}

void B_Monitor::BoatReady (int *ret_arr) {
    MonitorBegin();
    if (!isPassengersReady) {
        eventPassengersReady.Wait();
    }
    passenger_i = 0;
    loads_started++;
    int loadType = passengers[0][0] + passengers[1][0] + passengers[2][0];
    if (loadType == 0) { // 3 missionaries
        stdout_lock->Wait();
        printf("MONITOR(%d): three missionaries (%d, %d, %d) are selected\n", loads_started, passengers[0][1], passengers[1][1], passengers[2][1]);
        stdout_lock->Signal();
    } else if (loadType == 3) { // 3 cannibals
        stdout_lock->Wait();
        printf("MONITOR(%d): three cannibals (%d, %d, %d) are selected\n", loads_started, passengers[0][1], passengers[1][1], passengers[2][1]);
        stdout_lock->Signal();
    } else if (loadType == 1) { // 2 missionaries and 1 cannibal
        int pass_ids[3], j = 1; // pass_ids[0] is cannibal
        for (int i = 0; i < 3; i++) {
            if (passengers[i][0]) {
                pass_ids[0] = passengers[i][1];
            } else {
                pass_ids[j++] = passengers[i][1];
            }
        }
        stdout_lock->Wait();
        printf("MONITOR(%d): one cannibal (%d) and two missionaries (%d, %d) are selected\n", loads_started, pass_ids[0], pass_ids[1], pass_ids[2]);
        stdout_lock->Signal();
    }

    ret_arr[0] = passengers[0][0]; ret_arr[1] = passengers[0][1]; ret_arr[2] = passengers[1][0]; ret_arr[3] = passengers[1][1];
    ret_arr[4] = passengers[2][0]; ret_arr[5] = passengers[2][1]; ret_arr[6] = loads_started;
    MonitorEnd();
}

void B_Monitor::BoatDone () {
    MonitorBegin();
    eventDisembark.Signal(); eventDisembark.Signal(); eventDisembark.Signal();
    for (int i = 0; i < 3; i++) {
        passengers[i][0] = 0; passengers[i][1] = 0;
    }
    if (loads_started == load_c) {
        stdout_lock->Wait();
        printf("MONITOR: %d crosses have been made.\nMONITOR: This river cross is closed indefinitely for rennovation.\n", load_c);
        stdout_lock->Signal();
        kill(getpid(), SIGINT);
    }
    isPassengersReady = 0;
    MonitorEnd();
}