/*
A simple program that prints the number of all threads 
and the currently active threads

To compile:        
(MacOS) g++omp exercise_1.cpp
(Jetson) g++ -fopenmp ex_00.cpp -o ex_00.out

After having done this setup (only the first time):

1)  install openmp
    brew libomp


2) define the alias g++omp

    nano ~/.zshrc
        alias g++omp='g++ -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -L/opt/homebrew/opt/libomp/lib -lomp'
    source ~/.zshrc


*/

#include <omp.h>
#include <iostream>
using namespace std;

int main(){
    int max_threads = omp_get_max_threads();
    cout << "Max number of threads:" << max_threads << endl;
    #pragma omp parallel
    {
        int thread_idx = omp_get_thread_num();

        #pragma omp critical
        {
            // critical: one thread allowed to print at once - avoids garbage messy output
            cout << "Hello from thread: " << thread_idx
                << "/" << max_threads << endl;
        }
    }

    return 0;
}