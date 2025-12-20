#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


// --- --- --- --- --- --- ---
// Firstprivate and private
// --- --- --- --- --- --- ---

/*
-firstprivate:
    the local copies are **initialized** with the value of the global variable.
    In this case, x=5. 
- private:
    the local copies are initialized to zero.

In both cases, the global variable is not affected by the work done inside the parallel block;
*/


/*
int main(void) {

    int x = 5;

    #pragma omp parallel private(x)
    //#pragma omp parallel firstprivate(x)
    {
      int thread_id = omp_get_thread_num();
      x = x + 1;  // Each thread has its own copy of x, initialized to 5
      printf("Thread %d: x = %d\n", thread_id, x);

    }

    printf("After parallel for: x = %d\n", x);

    return 0;
}
*/


// --- --- --- --- --- --- ---
// Last private
// --- --- --- --- --- --- ---

/*
Each thread does its bunch of iterations on its local copy of x, without interference with
what the other threads are doing. 
The value of x in the last iteration of the loop (here, i = 99), is copied back to the global variable
(regardless of which thread was assigned to iteration 99).

Execution order of the iterations remain non deterministic
*/

int main(void) {

    int x = 0;

    #pragma omp parallel for lastprivate(x)
    for (int i = 0; i < 100; i++) {
        x = i;  // Each thread updates its private copy of x
        printf("Thread %d set x to %d\n", omp_get_thread_num(), x);
    }

    printf("After parallel for: x = %d\n", x);

    return 0;
}
