#include<omp.h>
#include<stdio.h>

/*
3 fixes to avoid race conditions:

FIX 1: C_real and C_imag should be private
FIX 2: the inner loop index, j, must be explicitly made private
FIX 3: Protect numoutside++ from race conditions with a #pragma omp critical

Now it is working perfectly!!!
*/

# define NPOINTS 1000 // points in the complex plane to check
# define MXITR 1000 // max number of iterations to check for convergence
void testpoint(double, double); 
int numoutside = 0;



int main(){
    int i, j;
    int num = 0;
    
    double area, error, eps = 1.0e-5;
    #pragma omp parallel for private(j)
    // FIX 2. j must be made private
    for (i=0; i < NPOINTS; i++) {
        for (j=0; j < NPOINTS; j++) {
            double C_real, C_imag; // <- FIX 1. these should be PRIVATE
            C_real = -2.0 + 2.5*(double)(i)/(double)(NPOINTS) + eps; 
            C_imag = 1.125 * (double)(j)/(double)(NPOINTS) + eps; 
            // <- FIX 3. hidden here is a modification request to a global variable
            // protect with CRITICAL, from inside the function
            // you do not need to protect the whole function, it causes unnecessary waiting time
            testpoint(C_real, C_imag);
        }
    }
    area= (2.5 * 1.125) * 2.0 * (double)(NPOINTS*NPOINTS- numoutside)/(double)(NPOINTS*NPOINTS);
    error = area/(double)NPOINTS;
    printf("Area = %f, error = %f", area, error);
    // run different times, you see different results that are not compatible within the error
    // -> clear evidence of a RACE CONDITION happening
    return 0;
}



void testpoint(double C_real, double C_imag){ 
    double zr, zi;
    int iter; 
    double temp;
    zr = C_real; 
    zi = C_imag;
    for (iter = 0; iter < MXITR; iter++){
        temp = (zr*zr)-(zi*zi) + C_real;    // real part of z(n+1)
        zi = zr*zi*2 + C_imag;              // imag part of z(n+1)
        zr = temp;
        if ((zr*zr+zi*zi) > 4.0) {
            #pragma omp critical
            {
                numoutside++;                   
            }
            break; // exit the loop
        }
    }
}