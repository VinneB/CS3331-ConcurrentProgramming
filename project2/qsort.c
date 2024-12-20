/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 10/04/2023 */
/* PROGRAM ASSIGNMENT 2 */
/* FILE NAME : qsort.c (your unix file name) */
/* PROGRAM PURPOSE : */
/* This program uses a multi-process quicksort algorithm to sort a array stored in a shared memory segment. */
/* ----------------------------------------------------------- */

#include "shared.h"

int median3 (int *arr, int left_i, int right_i);
void swap (int *arr, int ind_1, int ind_2);

int main (int argc, char **argv) {
    char log_buf[LOG_BUF_SZ];
    int shm_id, left_i, right_i, *shmem_arr;
    // Sort Vars
    int pivot_i, store_i;
    char *qsort_exec_buf[5], qsort_arg1[DEF_CMDLN_ARG_SZ], qsort_arg2[DEF_CMDLN_ARG_SZ], qsort_arg3[DEF_CMDLN_ARG_SZ], qsort_arg4[DEF_CMDLN_ARG_SZ];
    char arr_buf[ARR_BUF_SZ], element_buf[8];

    if (argc < 4) {
        log_info(log_buf, "\tqsort <left_index> <right_index> <shared_mem_id>\n");
    }

    // Extract args
    left_i = atoi(argv[1]);
    right_i = atoi(argv[2]);
    shm_id = atoi(argv[3]);

    if (left_i < right_i) {

        // LOG
        snprintf(log_buf, LOG_BUF_SZ, "\t### Q-PROC(%d): entering with a[%d..%d]\n", getpid(), left_i, right_i);
        log_info(log_buf, NULL);

        // Attached shared memory
        if ((shmem_arr = shmat(shm_id, NULL, 0)) == (void*)-1) {
            snprintf(log_buf, LOG_BUF_SZ, "\tqsort (%d) - Error: shmat\n", getpid());
            log_info(log_buf, NULL);
            perror("shmat");
            exit(1);
        }

        // Choose a pivot
        pivot_i = median3(shmem_arr, left_i, right_i);

        // LOG
        snprintf(log_buf, LOG_BUF_SZ, "\t### Q-PROC(%d): pivot element is arr[%d] = %d\n", getpid(), pivot_i, shmem_arr[pivot_i]);
        log_info(log_buf, NULL);


        // Swap pivot and right
        swap(shmem_arr, pivot_i, right_i);
        // Partiton
        store_i = left_i;
        for (int i = left_i; i < right_i; i++) {
            if (shmem_arr[i] <= shmem_arr[right_i]) {
                swap(shmem_arr, i, store_i++);
            }
        }
        swap(shmem_arr, store_i, right_i);
        pivot_i = store_i;

        // LOG
        sprintf(arr_buf, "\t### Q-PROC(%d): partitioned array\n\t\t", getpid());
        for (int i = left_i; i <= right_i; i++) {
            sprintf(element_buf, "%d ", shmem_arr[i]);
            strcat(arr_buf, element_buf);
        }
        strcat(arr_buf, "\n");
        log_info(arr_buf, NULL);

        // LOG
        snprintf(log_buf, LOG_BUF_SZ, "\t### Q-PROC(%d): starting qsort process to sort[%d..%d]\n", getpid(), left_i, pivot_i-1);
        log_info(log_buf, NULL);

        // Recursively run algorithm on left section
        strcpy(qsort_arg1, QSORT_NAME);
        sprintf(qsort_arg2, "%d", left_i);
        sprintf(qsort_arg3, "%d", pivot_i-1);
        sprintf(qsort_arg4, "%d", shm_id);
        qsort_exec_buf[0] = qsort_arg1; qsort_exec_buf[1] = qsort_arg2; qsort_exec_buf[2] = qsort_arg3; qsort_exec_buf[3] = qsort_arg4; qsort_exec_buf[4] = NULL;
        if (fork() == 0) {
            // EXECUTED BY CHILD
            if (execvp(QSORT_PATH, qsort_exec_buf) == -1) {
                perror("\tCHILD: execvp (qsort)");
                exit(1);
            }
        }

        // LOG
        snprintf(log_buf, LOG_BUF_SZ, "\t### Q-PROC(%d): starting qsort process to sort[%d..%d]\n", getpid(), pivot_i+1, right_i);
        log_info(log_buf, NULL);

        // Recursively run algorithm on right section
        strcpy(qsort_arg1, QSORT_NAME);
        sprintf(qsort_arg2, "%d", pivot_i + 1);
        sprintf(qsort_arg3, "%d", right_i);
        sprintf(qsort_arg4, "%d", shm_id);
        qsort_exec_buf[0] = qsort_arg1; qsort_exec_buf[1] = qsort_arg2; qsort_exec_buf[2] = qsort_arg3; qsort_exec_buf[3] = qsort_arg4; qsort_exec_buf[4] = NULL;
        if (fork() == 0) {
            // EXECUTED BY CHILD
            if (execvp(QSORT_PATH, qsort_exec_buf) == -1) {
                perror("\tCHILD: execvp (qsort)");
                exit(1);
            }
        }

        // Wait for child
        wait(NULL);
        wait(NULL);

        // LOG
        snprintf(log_buf, LOG_BUF_SZ, "\t### Q-PROC(%d): section arr[%d..%d] is sorted\n", getpid(), pivot_i+1, right_i);
        log_info(log_buf, NULL);

        // LOG
        snprintf(log_buf, LOG_BUF_SZ, "\t### Q-PROC(%d): section arr[%d..%d] is sorted\n", getpid(), left_i, pivot_i-1);
        log_info(log_buf, NULL);

        snprintf(log_buf, LOG_BUF_SZ, "\t### Q-PROC(%d): exits\n", getpid());
        log_info(log_buf, NULL);

        

    }
    exit(0);


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
/* FUNCTION xxyyzz : median3 */
/* Selects the pivot using the standard median-3 technique. Returns the middle index */
/* PARAMETER USAGE : */
/* arr - the array being sorted    ;    left_i - the leftmost index used by this process    ;    right_i - the rightmost index used by this process */
/* FUNCTION CALLED : */
/* swap */
/* ----------------------------------------------------------- */
int median3 (int *arr, int left_i, int right_i) {
    int mid_i = left_i + ((right_i - left_i)/2);
    if ((arr[mid_i] >= arr[right_i]) && (arr[left_i] <= arr[mid_i])) {
        swap(arr, mid_i, right_i);
    } else if ((arr[mid_i] <= arr[right_i]) && (arr[left_i] >= arr[mid_i])) {
        swap(arr, left_i, mid_i);
    } else if ((arr[left_i] <= arr[mid_i]) && (arr[left_i] >= arr[right_i])) {
        swap(arr, left_i, right_i);
        swap(arr, mid_i, right_i);
    } else if ((arr[left_i] >= arr[right_i]) && (arr[mid_i] <= arr[right_i])) {
        swap(arr, left_i, right_i);
        swap(arr, left_i, mid_i);
    } else if ((arr[left_i] >= arr[mid_i]) && (arr[mid_i] >= arr[right_i])) {
        swap(arr, left_i, right_i);
    }
    return mid_i;
}

/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : swap */
/* Swaps the value at ind_1 with the value at ind_2 */
/* PARAMETER USAGE : */
/* ind_1 - the index of one of the values to be swapped    ;    ind_2 - the index of the other value to be swapped */
/* ----------------------------------------------------------- */
void swap (int *arr, int ind_1, int ind_2) {
    int temp_val = arr[ind_1];
    arr[ind_1] = arr[ind_2];
    arr[ind_2] = temp_val;
}