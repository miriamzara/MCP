/*
Basic loop behaviour: with the directive

#pragma omp parallel for
                    _____
                    
the iterations of the for loop are divided among threads
*/

#include <omp.h>
#include <iostream>
using namespace std;

int main(){
    int max_threads = omp_get_max_threads();
    cout << "Max number of threads:" << max_threads << endl;
    #pragma omp parallel for
    {
        for(int i=0; i< 10; i++){
        int thread_idx = omp_get_thread_num();
            #pragma omp critical
            {
                // critical: one thread allowed to print at once - avoids garbage messy output
                cout << "Hello from thread: " << thread_idx
                    << "/" << max_threads << "  iter= "<<i<< endl;
            }
        }
    }

    return 0;
}