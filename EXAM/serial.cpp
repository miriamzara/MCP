#include <iostream>
#include "MNIST_utils.h"
#include "NeuralNet_utils.h"

// Compile main script + source files together, e.g.
// g++ -std=c++11 serial.cpp MNIST_utils.cpp  NeuralNet_utils.cpp -o serial.out



int main(){

    // Test: reading the neural network
    std::string filename = "tiny_net.bin";
    NeuralNet* nn = create_NeuralNet(filename, 1);
    free_NeuralNet(nn);

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

    return 0;
};



