#include <stdio.h>
#include <stdlib.h>

// CUDA kernel to convert to grayscale
__global__ void rgb_to_grayscale(int* device_rgb, int* device_grayscale, int n_pixels){    
    int pixel_idx = threadIdx.x + blockDim.x * blockIdx.x;
    if(pixel_idx < n_pixels){
        float rgb_factors[3] = {0.21, 0.71, 0.07};
        float result = 0.;
        for(int i = 0; i < 3; i++){
            result += device_rgb[3 * pixel_idx + i] * rgb_factors[i];
        }
        device_grayscale[pixel_idx] = int(ceil(result));
    }
}

// Function to read the PPM image into a 1D array
// (can be modified to read it into a 2D array or any other data types)
// 
int *read_pgm(const char *filename, int *width, int *height, int *max_val) {
    
    // Open the input file in read mode "r"
    FILE *file = fopen(filename, "r");
    
    // Check if file can be opened
    if (file == NULL) {
        printf("Could not open file.\n");
        return NULL;
    }

    // Read the PPM header, composed by 3 lines, e.g.:
    //
    // P3                           [magic number]
    // 1024 768                     [pixel_width pixel_height]
    // 65535                        [max color levels]
    //
    // More info here -- https://www.wikiwand.com/en/articles/Netpbm

    // Read the first line, and verify if it states `P3` 
    char format[3];
    fscanf(file, "%s", format);
    if (format[0] != 'P' || format[1] != '3') {
        printf("Not a valid PGM (ASCII P3) file.\n");
        fclose(file);
        return NULL;
    }

    // Read the width, height, and maximum grayscale value
    fscanf(file, "%d %d", width, height);
    fscanf(file, "%d", max_val);

    // Compute the total amount of pixels
    int total_pixels = (*width) * (*height);

    // Allocate host memory for the image data
    // 3 x the image size to allocate R G B values
    int *image = (int *)malloc(3 * total_pixels * sizeof(int));
    
    // Read pixel values into the array
    // R0 G0 B0 R1 G1 B1 ...
    for (int i = 0; i < total_pixels * 3; i++) {
        fscanf(file, "%d", &image[i]);
    }

    // Close the input file
    fclose(file);  

    // Return the pixel array
    return image;  
}

// Function to write the PGM image from a 1D array
void write_pgm(const char *filename, int *image, int width, int height, int max_val) {

    // Open the output file in write mode "w"
    FILE *file = fopen(filename, "w");

    // Check if file can be opened
    if (file == NULL) {
        printf("Could not open file for writing.\n");
        return;
    }

    // Write the PGM header
    fprintf(file, "P2\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "%d\n", max_val);

    // Write the pixel values
    for (int i = 0; i < width * height; i++) {
        fprintf(file, "%d ", image[i]);
        // Include a newline every "width" number of pixels
        if ((i + 1) % width == 0) {
            fprintf(file, "\n");
        }
    }

    // Close the output file
    fclose(file);  
}

int main() {
    int width, height, max_val;
    // Read the PGM image
    int *host_rgb = read_pgm("ny.ppm", &width, &height, &max_val);
    if (host_rgb == NULL) {
        return 1;  // Error reading the file
    }
    cudaEvent_t start_alloc, start_copy, start_kernel, stop_kernel, stop_copy;
    cudaEventCreate(&start_alloc);
    cudaEventCreate(&start_copy);
    cudaEventCreate(&start_kernel);
    cudaEventCreate(&stop_kernel);
    cudaEventCreate(&stop_copy);

    // Allocate memory for the grayscale image on the host
    int num_pixels = width * height;
    int*host_grayscale = (int*)malloc(num_pixels * sizeof(int));

    // Allocate memory for the RGB image and Grayscale image on the GPU
    int* device_rgb;
    int* device_grayscale;
    cudaEventRecord(start_alloc);
    cudaMalloc((void**)&device_rgb, 3*num_pixels * sizeof(int));
    cudaMalloc((void**)&device_grayscale, num_pixels * sizeof(int));

    // Copy the RGB image data from the host to the device (GPU)
    cudaEventRecord(start_copy);
    cudaMemcpy(device_rgb, host_rgb, 3*num_pixels * sizeof(int), cudaMemcpyHostToDevice);

    // Define the block and grid dimensions
    dim3 blockSize(1024);
    dim3 gridSize(ceil(float(num_pixels)/blockSize.x));

    // Launch the CUDA kernel to convert RGB to Grayscale
    cudaEventRecord(start_kernel);
    rgb_to_grayscale<<<gridSize, blockSize>>>(device_rgb, device_grayscale, num_pixels);
    cudaEventRecord(stop_kernel);
    cudaEventSynchronize(stop_kernel);

    // Copy the Grayscale image data back to the host
    cudaMemcpy(host_grayscale, device_grayscale, num_pixels * sizeof(int), cudaMemcpyDeviceToHost);
    cudaEventRecord(stop_copy);
    // Write the grayscale image to a new PGM file
    write_pgm("ny_grayscale.pgm", host_grayscale, width, height, max_val);


    float elapsed_kernel, elapsed_copy, elapsed_alloc_to_copy;

    cudaEventElapsedTime(&elapsed_kernel,start_kernel, stop_kernel); // passing elapsed by reference
    cudaEventElapsedTime(&elapsed_copy,start_copy, stop_copy);
    cudaEventElapsedTime(&elapsed_alloc_to_copy,start_alloc, stop_copy);

    printf("Elapsed time (kernel):                 %.1f us\n", elapsed_kernel*1000);
    printf("Elapsed time (kernel+copy):            %.1f us\n", elapsed_copy*1000);
    printf("Elapsed time (kernel+copy+allocation): %.1f us\n", elapsed_alloc_to_copy*1000);

    // Free the memory on the host and the GPU
    cudaFree(device_grayscale);
    cudaFree(device_rgb);
    free(host_grayscale);
    free(host_rgb);
    return 0;
}
