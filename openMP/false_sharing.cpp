/*
There is no mechanism to load less than a cache line.


(mcp) jetson@jetson:~$ getconf LEVEL1_DCACHE_LINESIZE
64
(mcp) jetson@jetson:~$ 
*/


#include<omp.h>
#include<stdio.h>
#define N_THREADS 2

int n_iter = 1e6;

int main(){
    omp_set_num_threads(N_THREADS);
    double array[N_THREADS]; // 1 double = 8B, 8 doubles fit in one cache line

    double tdata = omp_get_wtime();
    #pragma omp parallel
    {
        int thread_idx = omp_get_thread_num();
        for(int i = 0; i < n_iter; i++) array[thread_idx] *= 3;

    }
    tdata = omp_get_wtime() - tdata;
    printf("=== Time (false sharing) ===\n");
    printf("in %f secs\n", tdata);

    // even though thread 1 only increments element 1,
    // it still takes the whole cache line, and the other threads cannot
    // write on it.


    double padded_array[N_THREADS][8]; // 1 double = 8B, 8 doubles fit in one cache line

    tdata = omp_get_wtime();
    #pragma omp parallel
    {
        int thread_idx = omp_get_thread_num();
        for(int i = 0; i < n_iter; i++) padded_array[thread_idx][0] *= 3;

    }
    tdata = omp_get_wtime() - tdata;
    printf("=== Time (padded) ===\n");
    printf("in %f secs\n", tdata);



    return 0;
}