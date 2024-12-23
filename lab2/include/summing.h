#ifndef SUMMING_H
#define SUMMING_H

bool sum_arrays(int K, int N, int** arrays, int* result, int max_threads, int& max_active_threads);
int** create_arrays(int K, int N, int value = 1);
void free_arrays(int** arrays, int K);
int* create_result(int N);
void free_result(int* result);

#endif
