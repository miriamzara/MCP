#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

/*
Common mistake: remember that cudaMemcpy expects the number of BITES,
not the number of elements!
*/


// CUDA kernel to boost brightness by a percentage
__global__ void boost_brightness(int* im, int* im_size, float* brightness_factor) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < (*im_size)){
        im[idx] = min(255, (int)(im[idx] * (*brightness_factor)));
    }
}

// Function to read the PGM image into a 1D array
// (can be modified to read it into a 2D array)
int *read_pgm(const char *filename, int *width, int *height, int *max_val) {
    
    // Open the input file in read mode "r"
    FILE *file = fopen(filename, "r");
    
    // Check if file can be opened
    if (file == NULL) {
        printf("Could not open file.\n");
        return NULL;
    }

    // Read the PGM header, composed by 3 lines, e.g.:
    //
    // P2                           [magic number]
    // 1024 768                     [pixel_width pixel_height]
    // 255                          [max grayscale levels]
    //
    // More info here -- https://www.wikiwand.com/en/articles/Netpbm

    // Read the first line, and verify if it states `P2` 
    char format[3];
    fscanf(file, "%s", format);
    if (format[0] != 'P' || format[1] != '2') {
        printf("Not a valid PGM (ASCII P2) file.\n");
        fclose(file);
        return NULL;
    }

    // Read the width, height, and maximum grayscale value
    fscanf(file, "%d %d", width, height);
    fscanf(file, "%d", max_val);

    // Compute the total amount of pixels
    int total_pixels = (*width) * (*height);

    // Allocate host memory for the image data
    int *image = (int *)malloc(total_pixels * sizeof(int));
    
    // Read pixel values into the array
    for (int i = 0; i < total_pixels; i++) {
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
    /*
    Amazing: the function read_pgm() is written in such a way that you 
    do not need to specify the values of width, height, max_val.
    You just declare these variables, pass them by pointer and,
    after the function call, they will be filled up with the actual
    values of your image!
    */

    // Boost brightness by 20%
    float brightness_factor = 1.2; 

    // Read the PGM image
    int *host_image = read_pgm("cat.pgm", &width, &height, &max_val);
    if (host_image == NULL) {
        return 1;  // Error reading the file
    }
    cout << "Image loaded into array on the host. Check:" << endl;
    for(int i = 0; i < 10; i++){
        cout << host_image[i] << "\t";
    }
    cout << endl;
    // Allocate memory for the image on the GPU
    int im_size = width * height;
    int* device_image;
    int* device_im_size;
    float* device_brightness_factor;
    cudaMalloc((void **) &device_image, im_size * sizeof(int));
    cudaMalloc((void **) &device_brightness_factor, sizeof(float));
    cudaMalloc((void **) &device_im_size, sizeof(int));
    // Copy the image data to the GPU
    cudaMemcpy(device_image, host_image, im_size * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(device_brightness_factor, &brightness_factor, sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(device_im_size, &im_size, sizeof(int), cudaMemcpyHostToDevice);
    // Define the block and grid dimensions
    /*
    cuda allows a max number of threads per block. our image size exceeds that.
     so we create more than one block.
    */
    int n_blocks = int(ceil(im_size / 1024));
    dim3 gridSize(n_blocks,1,1);
    dim3 blockSize(1024,1,1);
    // Launch the CUDA kernel to boost the brightness
    boost_brightness<<<gridSize, blockSize>>>(device_image, device_im_size, device_brightness_factor); 
    // you pass pointers as arguments, and dereference them inside the kernel
    // Copy the modified image data back to the host
    cudaMemcpy(host_image, device_image, im_size * sizeof(int), cudaMemcpyDeviceToHost);
    cout << "Image copied back to host. Check:" << endl;
    for(int i = 0; i < 10; i++){
        cout << host_image[i] << "\t";
    }
    cout << endl;
    // Write the brightened image to a new PGM file
    /*
    all inputs to write_pgm() are simply passed by value
    */
    write_pgm("cat_brightened.pgm", host_image, width, height, max_val);
    
    // Free the memory on the GPU (and host)
    cudaFree(device_image);
    cudaFree(device_brightness_factor);
    cudaFree(device_im_size);
    return 0;
}
