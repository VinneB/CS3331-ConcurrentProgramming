/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 10/04/2023 */
/* PROGRAM ASSIGNMENT 2 */
/* FILE NAME : merge.c (your unix file name) */
/* PROGRAM PURPOSE : */
/* This program merges two arrays provided to it using a modified binary search algorithm. This is done concurrently with multiple processes. */
/* ----------------------------------------------------------- */

#include "shared.h"

int sort (int comparison_arr_sz, int *indexed_arr, int *comparison_arr, int index);
int binary_search (int val, int arr_len, int *arr);


int main (int argc, char **argv) {
    int x_size, y_size, x_shmem_id, y_shmem_id, merged_id, sort_index;
    int *x, *y, *merged;
    char log_buf[LOG_BUF_SZ];

    if (argc < 6) {
        printf ("merge <x_size> <x_shmem_id> <y_size> <y_shmem_id> <merge_shmem_id>\n");
    }

    x_size = atoi(argv[1]);
    x_shmem_id = atoi(argv[2]);
    y_size = atoi(argv[3]);
    y_shmem_id = atoi(argv[4]);
    merged_id = atoi(argv[5]);

    // Attach sorted array x
    if ((x = shmat(x_shmem_id, NULL, 0)) == ((void*)-1)) {
        perror("shmat (x)");
        exit(1);
    }

    // Attach sorted array y
    if ((y = shmat(y_shmem_id, NULL, 0)) == ((void*)-1)) {
        perror("shmat (y)");
        exit(1);
    }

    // Attach sorted array y
    if ((merged = shmat(merged_id, NULL, 0)) == ((void*)-1)) {
        perror("shmat (merge)");
        exit(1);
    }

    // Create a child process for each element in x and y
    for (int i = 0; i < x_size+y_size; i++) {
        if (fork() == 0) {
            // Executed by sorter child processes
            if (i < x_size) {
                sprintf(log_buf, "\t$$$ M-PROC(%d): handling x[%d] = %d\n", getpid(), i, x[i]);
                log_info(log_buf, NULL);
                sort_index = sort(y_size, x, y, i);
                sprintf(log_buf, "\t$$$ M-PROC(%d): about to write x[%d] = %d into position %d of the output array\n", getpid(), i, x[i], sort_index);
                log_info(log_buf, NULL);
                merged[sort_index] = x[i];
                exit(0);
            } else {
                sprintf(log_buf, "\t$$$ M-PROC(%d): handling y[%d] = %d\n", getpid(), i-x_size, y[i]);
                log_info(log_buf, NULL);
                sort_index = sort(x_size, y, x, i-x_size);
                sprintf(log_buf, "\t$$$ M-PROC(%d): about to write y[%d] = %d into position %d of the output array\n", getpid(), i-x_size, y[i], sort_index);
                log_info(log_buf, NULL);
                merged[sort_index] = y[i-x_size];
                exit(0);
            }
        }
    }


    for (int i = 0; i < x_size+y_size; i++) {
        wait(NULL);
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
/* FUNCTION xxyyzz : sort */
/* Name is somewhat of a misnomer. It provides the "modified" part of the modified binary search. Uses the conditions provided in assignment specs to choose behavior for a given index */
/* Returns the index for the item at index to be placed in the merged array*/
/* PARAMETER USAGE : */
/* comparison_arr_sz - the array which index does not reference. However, it is used to decide behavior    ;    indexed_arr - array that index is in. Used to get index's value. */
/* index - the index of the item which is being merged. */
/* FUNCTION CALLED : */
/* binary_search */
/* ----------------------------------------------------------- */
int sort (int comparison_arr_sz, int *indexed_arr, int *comparison_arr, int index) {
    int i;
    if (indexed_arr[index] < comparison_arr[0]) {
        return index;
    } else if (indexed_arr[index] > comparison_arr[comparison_arr_sz-1]) {
        return (comparison_arr_sz+index);
    } else {
        i = binary_search(indexed_arr[index], comparison_arr_sz, comparison_arr);
        return (index+i+1);
    }
}


/* ----------------------------------------------------------- */
/* FUNCTION xxyyzz : binary_search */
/* performs basic binary search over arr */
/* returns the position val would be placed based on it's relative size to the rest of the array*/
/* PARAMETER USAGE : */
/* val - the value for whos relative position is being evaluated    ; arr_len - length of arr    ;    arr - the array whos values are being compared against val */
/* FUNCTION CALLED : */
/* binary_search */
/* ----------------------------------------------------------- */
int binary_search (int val, int arr_len, int *arr) {
    int start = 0;
    int end = arr_len -1;
    int mid, res;
    while (start <= end) {
        mid = (start + end) / 2;
        if (arr[mid] < val) {
            start = mid+1;
            res = mid;
        } else {
            end = mid - 1;
        }
    }
    return res;
}