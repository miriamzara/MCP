#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// AXPY operation: y = a*x + y
// where a is a scalar, x and y are vectors

void axpy(int n, double a, const double *x, double *y) {
    for (int i = 0; i < n; i++) {
        y[i] = a * x[i] + y[i];
    }
}

// Function to verify the result
int verify_result(int n, double a, const double *x, const double *y_original, const double *y_result) {
    double tolerance = 1e-9;
    
    for (int i = 0; i < n; i++) {
        double expected = a * x[i] + y_original[i];
        double error = fabs(y_result[i] - expected);
        
        if (error > tolerance) {
            printf("Verification failed at index %d: expected %.10f, got %.10f (error: %.2e)\n", 
                   i, expected, y_result[i], error);
            return 0;
        }
    }
    return 1;
}

// Function to get elapsed time in seconds
double get_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(int argc, char *argv[]) {
    // Check command line arguments
    if (argc != 3) {
        printf("Usage: %s <vector_size> <scalar_a>\n", argv[0]);
        printf("Example: %s 1000000 2.5\n", argv[0]);
        return 1;
    }
    
    // Parse command line arguments
    int n = atoi(argv[1]);
    double a = atof(argv[2]);
    
    // Validate input
    if (n <= 0) {
        printf("Error: vector size must be positive\n");
        return 1;
    }
    
    printf("=== AXPY Operation ===\n");
    printf("Vector size: %d\n", n);
    printf("Scalar a: %.6f\n\n", a);
    
    // Allocate memory for vectors
    double *x = (double *)malloc(n * sizeof(double));
    double *y = (double *)malloc(n * sizeof(double));
    double *y_original = (double *)malloc(n * sizeof(double));
    
    if (x == NULL || y == NULL || y_original == NULL) {
        printf("Error: memory allocation failed\n");
        free(x);
        free(y);
        free(y_original);
        return 1;
    }
    
    // Initialize vectors with sample data
    srand(42); // Fixed seed for reproducibility
    for (int i = 0; i < n; i++) {
        x[i] = (double)rand() / RAND_MAX;
        y[i] = (double)rand() / RAND_MAX;
        y_original[i] = y[i]; // Save original y for verification
    }
    
    // Perform AXPY with timing
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    axpy(n, a, x, y);
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = get_elapsed_time(start, end);
    
    // Verify the result
    printf("Verifying result...\n");
    int success = verify_result(n, a, x, y_original, y);
    
    if (success) {
        printf("✓ Verification PASSED\n\n");
    } else {
        printf("✗ Verification FAILED\n\n");
    }
    
    // Print timing results
    printf("=== Performance ===\n");
    printf("Execution time: %.6f seconds\n", elapsed);
    printf("Operations: %d\n", 2 * n); // n multiplications + n additions
    printf("FLOPS: %.2e ops/sec\n", (2.0 * n) / elapsed);
    
    // Print sample results (first and last few elements)
    printf("\n=== Sample Results ===\n");
    int sample_size = (n < 5) ? n : 5;
    printf("First %d elements:\n", sample_size);
    for (int i = 0; i < sample_size; i++) {
        printf("y[%d] = %.6f * %.6f + %.6f = %.6f\n", 
               i, a, x[i], y_original[i], y[i]);
    }
    
    if (n > 10) {
        printf("\nLast %d elements:\n", sample_size);
        for (int i = n - sample_size; i < n; i++) {
            printf("y[%d] = %.6f * %.6f + %.6f = %.6f\n", 
                   i, a, x[i], y_original[i], y[i]);
        }
    }
    
    // Free allocated memory
    free(x);
    free(y);
    free(y_original);
    
    return 0;
}