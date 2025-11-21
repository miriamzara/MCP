/*
#pragma omp parallel section   

creates a parallel region where different blocks of code
(called sections) are executed by different threads.
It is used when you want TASK PARALLELISM instead of 
looping over data.



*/

#include <omp.h>
#include <iostream>
using namespace std;

int main(){
    int max_threads = omp_get_max_threads();
    cout << "Max number of threads:" << max_threads << endl;
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            # pragma omp critical
            {
            int thread_idx = omp_get_thread_num();
            cout << "AAAAAA from thread " << thread_idx << endl;
            }
        }
        
        #pragma omp section
        {
            #pragma omp critical
            {
            int thread_idx = omp_get_thread_num();
            cout << "BBBBBB from thread " << thread_idx << endl;
            }
        }
    }

    return 0;
}