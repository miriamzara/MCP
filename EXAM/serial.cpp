#include <iostream>
#include "MNIST_utils.h"
#include "NeuralNet_utils.h"

// Compile main script + source files together, e.g.
// g++ -std=c++11 serial.cpp MNIST_utils.cpp  NeuralNet_utils.cpp -o serial.out



int main(){

    // Test: reading the neural network
    std::string filename = "nn.bin";
    NeuralNet* nn = create_NeuralNet(filename, 1);
    free_NeuralNet(nn);

    // Test: read_mnist_images(), read_mnist_labels()
    
    int n_images, image_size;
    int n_load = 2;
    uchar** images = read_mnist_images(IMAGES_PATH, n_load, n_images, image_size);
    std::cout << "Available " << n_images << " images\n";
    std::cout << "Each image has " << image_size << " pixels\n";
    std::string img_filename;
    for(int i=0; i<n_load; i++){
        img_filename = "mnist_sample" + std::to_string(i) + ".pgm";
        save_pgm(img_filename, images[i]);
    }
    std::cout << "Saved mnist_sample.pgm\n";

    int n_labels;
    uchar* labels = read_mnist_labels(LABELS_PATH, n_labels);
    std::cout << "Loaded " << n_labels << " labels\n";
    for(int i = 0; i < std::min(10, n_labels); i++)
        std::cout << "Label[" << i << "] = " << (int)labels[i] << "\n";



    // Cleanup
    for(int i = 0; i < n_load; i++)
        free(images[i]);
    free(images);
    free(labels);
    
    return 0;
};



