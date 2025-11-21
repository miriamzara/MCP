/*
NESTED LOOPS

*/

#include <omp.h>
#include <iostream>
using namespace std;

int main(){
    int max_threads = omp_get_max_threads();
    cout << "Max number of threads:" << max_threads << endl;
    #pragma omp parallel for
    //
    // COMPLETE AT HOME



    //
    return 0;
}