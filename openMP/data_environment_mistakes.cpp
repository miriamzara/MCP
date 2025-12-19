#include<omp.h>
#include<stdio.h>

/*
Computes the Mandelbrot set

c \in Set iff z(n+1) = z(n)**2 + c remains bounded,
i.e. |z(n)| <= K where K is an arbitrarily chosen constant, K>=2.

[-2, +0.5] x [0, 1.125] is the region of the complex plane scanned
region area = 2.5 x 1.125

The mandelbrot set is symmetric w.r.t. the x axis
Its area is twice the area occupied by the points in the rectangle [-2, +0.5] x [0, 1.125] 
Should be ≈ 1.5066

1. Spot and fix the errors
2. Once you have a working version, optimize it
*/

# define NPOINTS 1000 // points in the complex plane to check
# define MXITR 1000 // max number of iterations to check for convergence
void testpoint(double, double); 
int numoutside = 0;



int main(){
    int i, j;
    int num = 0;
    double C_real, C_imag;
    double area, error, eps = 1.0e-5;
    #pragma omp parallel for private(eps) 
    for (i=0; i < NPOINTS; i++) {
        for (j=0; j < NPOINTS; j++) {
            // i scans the x axis, j scans the y axis
            // every thread should scan ALL the y axis of its real axis chunck
            // is this the case??
            C_real = -2.0 + 2.5*(double)(i)/(double)(NPOINTS) + eps; 
            C_imag = 1.125 * (double)(j)/(double)(NPOINTS) + eps; 
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
            numoutside++;                   // can this access numoutside? i think yes
            break; // exit the loop 
        }
    }
}