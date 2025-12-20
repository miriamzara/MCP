#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#ifndef N
#define N 5
#endif
#ifndef FS
#define FS 38
#endif

/*
Computes the numbers of fibonacci sequence from n = FS to n = FS + N

Takes a while because every time you ask to compute fib(n), the whole sequence from n=0 gets
recomputed.

Task: parallelize this program using openMP
*/

struct node {
   int data;
   int fibdata;
   struct node* next;
};

int fib(int n) {
   // compute F(n) = F(n-1) + F(n-2)
   int x, y;
   if (n < 2) {
      return (n);
   } else {
      x = fib(n - 1);
      y = fib(n - 2);
	  return (x + y);
   }
}

void processwork(struct node* p) 
{
   // computes the fibonacci element on the node p
   int n;
   n = p->data;
   p->fibdata = fib(n);
}

struct node* init_list(struct node* p) {
    int i;
    struct node* head = NULL;
    struct node* temp = NULL;
    
    head = (struct node*)malloc(sizeof(struct node));
    p = head;
    p->data = FS; // index of the sequence, n
    p->fibdata = 0; // value of the sequence, F(n)
    for (i=0; i< N; i++) {
       temp  =  (struct node*)malloc(sizeof(struct node));
       p->next = temp;
       p = temp;
       p->data = FS + i + 1;
       p->fibdata = i+1;
    }
    p->next = NULL;
    return head;
}

// --- --- --- --- ----


int main(int argc, char *argv[]) {
     double start, end;
     struct node *p=NULL;
     struct node *temp=NULL;
     struct node *head=NULL;
     
	 printf("Process linked list\n");
     printf("  Each linked list node will be processed by function 'processwork()'\n");
     printf("  Each ll node will compute %d fibonacci numbers beginning with %d\n",N,FS);      
 
     p = init_list(p);
     head = p;

     // STEP 1. Count elements in the list:
     // the list was defined with the attribute "next"
     // so you can increment a counter until that attribute does not have value

     int n_elements = 0;
     while(p != NULL){
      n_elements += 1;
      p = p-> next;
     }
     printf("Number of elements in the list p = %d \n", n_elements);

     // STEP 2. I need to access elements of the list by index, otherwise how can i schedule work to threads?
     // I dont have an index, but I can construct one

     struct node** parray = (struct node**) malloc(n_elements * sizeof(node*));
     p = head;
     for(int i = 0; i < n_elements; i++){
         // fill it up
         parray[i] = p;
         p = p->next;
     }

     // STEP 3. Process in parallel
     p = head;
     start = omp_get_wtime();
     #pragma omp parallel for 
     for(int i=0; i <n_elements; i++){
         processwork(parray[i]);
     }
     end = omp_get_wtime();

     /* Original loop
     p = head;
     start = omp_get_wtime();
     {
        while (p != NULL) {
		   processwork(p);
		   p = p->next;
        }
     }
     end = omp_get_wtime();
     */


     p = head;
	 while (p != NULL) {
        printf("%d : %d\n",p->data, p->fibdata);
        temp = p->next;
        free (p);
        p = temp;
     }  
	 free (p);

     printf("Compute Time: %f seconds\n", end - start);

     return 0;
}

