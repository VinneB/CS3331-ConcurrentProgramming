/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 10/04/2023 */
/* PROGRAM ASSIGNMENT 2 */
/* FILE NAME : shared.h (your unix file name) */
/* PROGRAM PURPOSE : */
/* A header file to define some macros and include statements */
/* ----------------------------------------------------------- */

#ifndef SHARED_H
#define SHARED_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define LOG_BUF_SZ 128
#define QSORT_PATH "./qsort"
#define QSORT_NAME "qsort"
#define MERGE_NAME "merge"
#define MERGE_PATH "./merge"

#define DEF_CMDLN_ARG_SZ 32
#define ARR_BUF_SZ 256

void log_info (char *buf, const char *str);




#endif