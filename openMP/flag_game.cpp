#include <stdio.h>
#include <omp.h>
#define COUNT 1000000


int main(){
    int answer = 0;
    int flag = 0;
    int err = 0;


    for(int iter=0; iter < COUNT; iter++){
        flag = 0;
        answer = 0; // reset values
        #pragma omp parallel shared(flag, answer) num_threads(2)
        {
            int thread_idx = omp_get_thread_num();
            if(thread_idx == 0){
                answer = 42;
                flag = 1;
            }
            else if(thread_idx == 1){
                while(flag == 0) {  }
                if(answer != 42) err++;
            }
        }
    }
    return 0;
}


/*
What goes on here?

1) ANSWER MAY BE DIFFERENT FROM 42 IN THE POINT OF VIEW OF THREAD 1, EVEN IF THREAD 0 HAS WRITTEN IT ALREADY.
    This can happen for two independent reasons:

    a) ORDERING OF OPERATIONS IS NOT GUARANTEED
    I write

    answer = 42;
    flag = 1;

    but it is not guaranteed that the write operations are done in this order. It may happen that 
    they are done in reverse:

    flag <- 1;
    answer <- 42;


    b)  VISIBILITY OF UPDATES IS NOT GUARANTEED

    Even if the writes are not reordered, answer may still reside in a register 
    or cache local to thread 0 when flag becomes visible to thread 1.



    For both a) and b), it may happen that thread 1 sees flag=1 but still answer=0, and err is incremented.
*/