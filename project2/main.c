/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 10/04/2023 */
/* PROGRAM ASSIGNMENT 2 */
/* FILE NAME : main.c (your unix file name) */
/* PROGRAM PURPOSE : */
/* This is the main file for assignment 2. It spawns the intial child processes to sort an array and merge two arrays. It also creates and deletes shared memory segments. */
/* ----------------------------------------------------------- */

#include "shared.h"

void cleanup(int *shm_id, int *arr[3]);

int main () {
    // 0: a ; 1: x ; 2: y
    int arr_size[3] = {0, 0, 0}, shm_id[3] = {-1, -1, -1}, merged_id;
    int *arr[3] = {NULL, NULL, NULL}, child_pid[2], *merge_arr;
    char log_write_buf[LOG_BUF_SZ], *exec_buf[7], exec_arg1[DEF_CMDLN_ARG_SZ], exec_arg2[DEF_CMDLN_ARG_SZ], exec_arg3[DEF_CMDLN_ARG_SZ], exec_arg4[DEF_CMDLN_ARG_SZ], exec_arg5[DEF_CMDLN_ARG_SZ], exec_arg6[DEF_CMDLN_ARG_SZ];


    // Create shared memory space for each array and read array into shared space
    for (int i = 0; i < 3; i++) {
        // Read in size of array
        scanf("%d", &(arr_size[i]));
        // Create and attach shared memory
        if((shm_id[i] = shmget(IPC_PRIVATE, arr_size[i]*sizeof(int), IPC_CREAT | 0666)) == -1) {
            perror("shmget");
            cleanup(shm_id, arr);
            exit(1);
        }
        // LOG
        snprintf(log_write_buf, LOG_BUF_SZ, "*** MAIN: shared memory key = %d\n", shm_id[i]);
        write(1, log_write_buf, strlen(log_write_buf));
        snprintf(log_write_buf, LOG_BUF_SZ, "*** MAIN: shared memory created\n");
        write(1, log_write_buf, strlen(log_write_buf));
        if ((arr[i] = shmat(shm_id[i], NULL, 0)) == ((void*)-1)) {
            perror("shmat");
            cleanup(shm_id, arr);
            exit(1);
        }
        //LOG
        snprintf(log_write_buf, LOG_BUF_SZ, "*** MAIN: shared memory attached at %p and is ready to use\n", arr[i]);
        write(1, log_write_buf, strlen(log_write_buf));
        // Read array into shared memory
        for (int j = 0; j < arr_size[i]; j++) {
            scanf("%d", &(arr[i][j]));
        }
    }

    // Allocate a shared mem section for the merged arrays
     if((merged_id = shmget(IPC_PRIVATE, (arr_size[1]+arr_size[2])*sizeof(int), IPC_CREAT | 0666)) == -1) {
            perror("shmget (merge)");
            exit(1);
    }

    // LOG
    snprintf(log_write_buf, LOG_BUF_SZ, "*** MAIN: merge array shared memory key = %d\n", merged_id);
    write(1, log_write_buf, strlen(log_write_buf));
    snprintf(log_write_buf, LOG_BUF_SZ, "*** MAIN: merge array shared memory created\n");
    write(1, log_write_buf, strlen(log_write_buf));

    // Attach array merged
    if ((merge_arr = shmat(merged_id, NULL, 0)) == ((void*)-1)) {
        perror("shmat (merge)");
        exit(1);
    }

    // LOG
    snprintf(log_write_buf, LOG_BUF_SZ, "*** MAIN: merge array shared memory attached at %p and is ready to use\n", merge_arr);
    write(1, log_write_buf, strlen(log_write_buf));

    // LOG
    snprintf(log_write_buf, LOG_BUF_SZ, "\nInput array for qsort has %d elements:\n\t", arr_size[0]);
    write(1, log_write_buf, strlen(log_write_buf));
    for (int i = 0; i < arr_size[0]; i++) {
        printf("%d ", arr[0][i]);
    }
    printf("\n\nInput array x[] for merge has %d elements:\n\t", arr_size[1]);
    for (int i = 0; i < arr_size[1]; i++) {
        printf("%d ", arr[1][i]);
    }
    printf("\n\nInput array y[] for merge has %d elements:\n\t", arr_size[2]);
    for (int i = 0; i < arr_size[2]; i++) {
        printf("%d ", arr[2][i]);
    }
    printf("\n\n");



    // Start Qsort
    log_info(log_write_buf, "*** MAIN: about to spawn the process for qsort\n");

    // Copy commandline args into exec_buf for Qsort
    // Format: qsort <start_index> <end_index> <shared_memory_id>
    strcpy(exec_arg1, QSORT_NAME);
    strcpy(exec_arg2, "0");
    sprintf(exec_arg3, "%d", arr_size[0]-1);
    sprintf(exec_arg4, "%d", shm_id[0]);
    exec_buf[0] = exec_arg1; exec_buf[1] = exec_arg2; exec_buf[2] = exec_arg3; exec_buf[3] = exec_arg4; exec_buf[4] = NULL;
    if ((child_pid[0] = fork()) == 0) {
        // EXECUTED BY CHILD
        if (execvp(QSORT_PATH, exec_buf) == -1) {
            perror("CHILD: execvp (qsort)");
            exit(1);
        }
    }


    // Start Merge
    log_info(log_write_buf, "*** MAIN: about to spawn the process for merge\n");

    // Copy commandline args into exec_buf for merge
    // Format: merge <x_shared_mem_id> <y_shared_mem_id> <shared_memory_id>
    strcpy(exec_arg1, MERGE_NAME);
    sprintf(exec_arg2, "%d", arr_size[1]);
    sprintf(exec_arg3, "%d", shm_id[1]);
    sprintf(exec_arg4, "%d", arr_size[2]);
    sprintf(exec_arg5, "%d", shm_id[2]);
    sprintf(exec_arg6, "%d", merged_id);
    exec_buf[0] = exec_arg1; exec_buf[1] = exec_arg2; exec_buf[2] = exec_arg3; exec_buf[3] = exec_arg4; exec_buf[4] = exec_arg5; exec_buf[5] = exec_arg6; exec_buf[6] = NULL;
    if ((child_pid[1] = fork()) == 0) {
        // EXECUTED BY CHILD
        if (execvp(MERGE_PATH, exec_buf) == -1) {
            perror("CHILD: execvp (merge)");
            exit(1);
        }
    }


    wait(NULL);
    wait(NULL);

    // LOG
    log_info(log_write_buf, "\n*** MAIN: sorted array by qsort:\n\t");
    for (int i = 0; i < arr_size[0]; i++) {
        printf("%d ", arr[0][i]);
    }
    printf("\n");

    //LOG
    log_info(log_write_buf, "\n*** MAIN: merged array:\n\t");
    for (int i = 0; i < (arr_size[1]+arr_size[2]); i++) {
        printf("%d ", merge_arr[i]);
    }
    printf("\n\n");



    // Deattach and delete shared memory for merge array
    shmdt(merge_arr);
    printf("*** MAIN: shared memory successfully detached\n");
    merge_arr = NULL;
    shmctl(merged_id, IPC_RMID, NULL);
    printf("*** MAIN: shared memory successfully removed\n");

    cleanup(shm_id, arr);

    exit(1);



    
}

/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : log_info */
/* logs information to stdout. this is done to ensure stdout output isn't malformed */
/* PARAMETER USAGE : */
/* buf - a char array to put log in   ;    str - only used sometimes. NULL if not used. Will be copied to buf. */
/* ----------------------------------------------------------- */
void log_info (char *buf, const char *str) {
    if (str != NULL) {
        strcpy (buf, str);
    }
    write(1, buf, strlen(buf));
}

/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : cleanup */
/* deattaches and deletes shared memory sections */
/* PARAMETER USAGE : */
/* shm_id - a list of the shared memory ids   ;    arr - the attached shared memory sections */
/* ----------------------------------------------------------- */
void cleanup(int *shm_id, int *arr[3]) {
    for (int i = 0; i < 3; i++) {
        if (arr[i] != NULL) {
            shmdt(arr[i]);
            arr[i] = NULL;
        }
        printf("*** MAIN: shared memory successfully detached\n");
        if (shm_id[i] != -1) {
            shmctl(shm_id[i], IPC_RMID, NULL);
            shm_id[i] = -1;
        }
        printf("*** MAIN: shared memory successfully removed\n");
    }
}