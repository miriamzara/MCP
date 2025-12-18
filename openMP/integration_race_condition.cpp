// to compile: gomp name.cpp -o name.out
// to run: OMP_NUM_THREADS=8 ./name.out

/*
Setting the number of threads
Precedence (highest → lowest)
	1.	#pragma omp parallel num_threads(8)
	2.	inside the main(), with: omp_set_num_threads()
	3.	when executing, with: MP_NUM_THREADS=8 ./name.out
*/

#include<omp.h>
#include<stdio.h>
#include<stdlib.h>

int N_THREADS = 8; // because I have 8 cores on Apple M2 chip
static long num_steps = 1000;
static float step = 1.0/(float) num_steps;
int main (){
    omp_set_num_threads(N_THREADS);
    float pi = 0.;
    double time;
    double tdata;
    int actual_num_threads;

    tdata = omp_get_wtime();
    #pragma omp parallel
    {
        int thread_idx = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        if(thread_idx == 0){
            // only one thread should write to the global variable address,
            // to avoid race conditions (not in this case, but it is a good practice)
            actual_num_threads = num_threads;
        }
        int block_width = num_steps / num_threads; //integer division
        int idx_start = thread_idx * block_width;
        int idx_end = (thread_idx + 1) * block_width;
        if (thread_idx == num_threads - 1) idx_end = num_steps; // last thread does the extra work
        float x;
        float sum = 0;
        for(int i = idx_start; i < idx_end; i++){
            x = (i + 0.5) * step;
            sum += 4.0/(1.0+x*x); 
        }
        pi += sum;

    }
    pi *= step;
    tdata = omp_get_wtime() - tdata;
    time += tdata;

    printf("=== Parallel 2. time ===\n");
    printf("pi = %f in %f secs\n",pi, time);
    printf("threads launched = %d \n", actual_num_threads);
    return 0;
}