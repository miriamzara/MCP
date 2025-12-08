#include <iostream>
#include "import_utils.h"

// Compile main script + source files together, e.g.
// g++ -std=c++11 serial.cpp import_utils.cpp -o serial.out


typedef struct{
    int n_layers;
    int* sizes; // array of layer sizes
    float** weights; // weights[i] is the (flat) array of weights for layer i
    float** biases; // biases[i] is the (flat) array of weights for layer i
} NeuralNet;



int main(){

    FILE *file = fopen("nn.bin", "rb"); // r= read, b= binary
    if (file == NULL) {
        printf("Could not open file.\n");
        return NULL;
    }
    int n_layers;
    fread(&n_layers, sizeof(int), 1, file); // address where to store, how many bits to read, how many times, from where
    int* sizes = (int*) malloc(n_layers * sizeof(int));
    fread(sizes, sizeof(int), n_layers, file);
    std::cout << "N layers: "<< n_layers << std::endl;
    for(int i=0; i< n_layers; i++){
        std::cout << sizes[i] << "\t";
    }

    // Populate the struct
    NeuralNet net;
    net.n_layers = n_layers;
    net.sizes = sizes;
    net.weights = (float **)malloc( (net.n_layers-1) * sizeof(float*));

    for(int layer_idx=0; layer_idx <= net.n_layers - 2; layer_idx++){
        size_t ncols = net.sizes[layer_idx];
        size_t nrows = net.sizes[layer_idx + 1];
        size_t tot_elements = nrows * ncols;
        net.weights[layer_idx] = (float*)malloc( tot_elements * sizeof(float));


        fread(net.weights[layer_idx], sizeof(float), tot_elements, file);
        std::cout << "Loaded weights of layer " << layer_idx << std::endl;

        // Check - print a small subset of the weight matrix
        for(int i = 0; i < std::min(nrows, size_t(10)); i++){
            std::cout << "\n";
            for(int j=0; j < std::min(ncols, size_t(10)); j++){
                std::cout <<  net.weights[layer_idx][i*ncols + j] << "\t"; 
            }
        }
        std::cout << "\n";
    }

    // Free
    for(int layer_idx=0; layer_idx <= net.n_layers - 2; layer_idx++){
        free(net.weights[layer_idx]);
    }
    free(net.weights);
    return 0;
}




    /*
    net.n_layers = N_LAYERS;
    int sizes[N_LAYERS] = {N_INPUT, N_HIDDEN, N_OUTPUT};
    string w_filenames[2] = {w1_file, w2_file};
    net.sizes = sizes;
    net.weights = (float **)malloc( (net.n_layers-1) * sizeof(float*));
    for(int layer_idx=0; layer_idx <= net.n_layers - 2; layer_idx++){
        size_t nrows, ncols;
        net.weights[layer_idx] = read_weights(w_filenames[layer_idx], &nrows, &ncols);
        std::cout << nrows << " " << ncols << "\n";
        std::cout << "Loaded weights of layer " << layer_idx << std::endl;


        for(int i = 0; i < std::min(nrows, size_t(10)); i++){
            std::cout << "\n";
            for(int j=0; j < std::min(ncols, size_t(10)); j++){
                std::cout <<  net.weights[layer_idx][i*ncols + j] << "\t"; 
            }
        }
        std::cout << "\n";
    }
    */
    /*
    // Free
    for(int layer_idx=0; layer_idx <= net.n_layers - 2; layer_idx++){
        free(net.weights[layer_idx]);
    }
    free(net.weights);
    */
    /*
    // Test: read_weights()
    string filename = "neural_networks/matrice_prova.txt";
    size_t nrows, ncols;
    std::cout << "Reading weights..." << std::endl;
    float* w = read_weights(filename, &nrows, &ncols);
    std::cout << nrows << " " << ncols << "\n";
    for(int i = 0; i < nrows; i++){
        std::cout << "\n";
        for(int j=0; j < ncols; j++){
            std::cout << w[i*ncols + j] << "\t"; 
        }
    }
    std::cout << std::endl;
    */
    /*
    // Test: read_mnist_images(), read_mnist_labels()

    int n_images, image_size;
    uchar** images = read_mnist_images(IMAGES_PATH, n_images, image_size);
    std::cout << "Loaded " << n_images << " images\n";
    std::cout << "Each image has " << image_size << " pixels\n";
    int rows = 28;
    int cols = 28;
    save_pgm("mnist_sample.pgm", images[0], rows, cols);
    std::cout << "Saved mnist_sample.pgm\n";

    int n_labels;
    uchar* labels = read_mnist_labels(LABELS_PATH, n_labels);
    std::cout << "Loaded " << n_labels << " labels\n";
    for(int i = 0; i < std::min(10, n_labels); i++)
        std::cout << "Label[" << i << "] = " << (int)labels[i] << "\n";





    // Cleanup
    for(int i = 0; i < n_images; i++)
        free(images[i]);
    free(images);
    free(labels);

    */
    //free(w);
