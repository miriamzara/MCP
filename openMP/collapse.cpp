#include<omp.h>
#include<stdio.h>

/*
Lets take again the Manderbrot set exercise.
We said that the index of the inner loop of a parallel for
must be set to PRIVATE explicitly.

There is an alternative strategy for a nested loop: collapse().

Also, here I define an alternative version of test_point() which avoids atomic operations. It should scale better.
*/

# define NPOINTS 1000 // points in the complex plane to check
# define MXITR 10000 // max number of iterations to check for convergence
void testpoint(double, double); 
bool testpoint_reduction(double, double); 
int numoutside = 0;


int main(){
    int i, j;
    double C_real, C_imag;
    double area, error, eps = 1.0e-5;
    double tdata;
    bool escaped;
    // --- --- --- --- --- --- --- --- --- --- --- 
    //      Standard nested loop with private j
    // --- --- --- --- --- --- --- --- --- --- ---

    tdata = omp_get_wtime();
    #pragma omp parallel for private(j, C_real, C_imag, escaped) reduction(+:numoutside)
    for (i=0; i < NPOINTS; i++) {
        for (j=0; j < NPOINTS; j++) {
            C_real = -2.0 + 2.5*(double)(i)/(double)(NPOINTS) + eps; 
            C_imag = 1.125 * (double)(j)/(double)(NPOINTS) + eps; 
            //testpoint(C_real, C_imag);
            escaped = testpoint_reduction(C_real, C_imag);
            if(escaped) numoutside++;
        }
    }
    tdata = omp_get_wtime() - tdata;
    area= (2.5 * 1.125) * 2.0 * (double)(NPOINTS*NPOINTS- numoutside)/(double)(NPOINTS*NPOINTS);
    error = area/(double)NPOINTS;
    printf("Standard nested loop \n");
    printf("Area = %f, error = %f, time = %f \n", area, error, tdata);



    // --- --- --- --- --- --- --- --- ---
    //      Collapsed loop
    // --- --- --- --- --- --- --- --- ---

    numoutside = 0;
    tdata = omp_get_wtime();
    #pragma omp parallel for collapse(2) private(C_real, C_imag, escaped) reduction(+:numoutside)
    for (i=0; i < NPOINTS; i++) {
        for (j=0; j < NPOINTS; j++) {
            C_real = -2.0 + 2.5*(double)(i)/(double)(NPOINTS) + eps; 
            C_imag = 1.125 * (double)(j)/(double)(NPOINTS) + eps; 
            //testpoint(C_real, C_imag);
            escaped = testpoint_reduction(C_real, C_imag);
            if(escaped) numoutside++;
        }
    }
    tdata = omp_get_wtime() - tdata;
    area= (2.5 * 1.125) * 2.0 * (double)(NPOINTS*NPOINTS- numoutside)/(double)(NPOINTS*NPOINTS);
    error = area/(double)NPOINTS;
    printf("Collapsed loop\n");
    printf("Area = %f, error = %f, time = %f \n", area, error, tdata);



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
            #pragma omp atomic 
            numoutside++;                  
            break;
        }
    }
}


bool testpoint_reduction(double C_real, double C_imag){ 
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
            return true;
        }
    }
    return false;
}