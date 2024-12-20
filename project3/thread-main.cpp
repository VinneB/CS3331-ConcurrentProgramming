/* ----------------------------------------------------------- */
/* NAME : Vincent Barfield User ID: vsbarfie */
/* DUE DATE : 10/16/2023 */
/* PROGRAM ASSIGNMENT 3 */
/* FILE NAME : thread-main.cpp */
/* PROGRAM PURPOSE : */
/* This is the main file for assignment 3. It reads in 'n' integers and spawns threads to compute the prefix sum of the integers concurrently. */
/* ----------------------------------------------------------- */
#include "thread.h"
#include <iostream>
using namespace std;

int *b = NULL;
int n = 0;

int main () {
    int *x, k = 0;

    cout << "Concurrent Prefix Sum Computation" << endl; 

    // Read in size, malloc, and read in ints
    cin >> n;

    cout << "Number of input data = " << n << endl;
    cout << "Input array:" << endl;

    x = (int *) malloc(n*sizeof(int));
    for (int i = 0; i < n; i++) {
        cin >> x[i];
        printf("%4d", x[i]);
    }
    cout << endl;

    PS_Thread *threads[n];

    // Calculate k = log_2(n) for iteration number
    for (int i = n; i != 1; (i=i>>1)&&k++);

    // Create b
    b = (int *) malloc(sizeof(int)*((k+1)*n));
    for (int i = 0; i < n; i++) { b[i] = x[i]; }

    for (int i = 1; i <= k; i++) {
        cout << "Run " << i << ":" << endl;
        // Create and run n threads to compute an iteration of the prefix sum algorithm
        for (int j = 0; j < n; j++) { threads[j] = new PS_Thread(i, j); threads[j]->Begin(); }
        // Wait on the n threads
        for (int j = 0; j < n; j++) { threads[j]->Join(); }
        // Print the result after this iteration
        cout << "Result after run " << i << ":" << endl;
        for (int j = 0; j < n; j++) {
            printf("%4d", b[(n*i)+j]);
        }
        cout << endl;
    }

    cout << "\nFinal result after run " << k << ":" << endl;
    for (int j = 0; j < n; j++) {
        printf("%4d", b[(n*k)+j]);
    }
    cout << endl;

    Exit();
    



}