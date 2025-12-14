#include <iostream>
#include "MNIST_utils.h"
#include "NeuralNet_utils.h"
#include <fstream>

// Compile main script + source files together, e.g.
// g++ -std=c++11 serial.cpp MNIST_utils.cpp  NeuralNet_utils.cpp -o serial.out



std::string folder_name = ""; // only use for validation of the Forward Pass code against Pytorch reference

int main(){

    // --- Input ---

    std::string filename = folder_name + "mlp_H256_L1.bin";
    // --- Output ---
    std::ofstream fout(folder_name + "cpp_FP.txt", std::ios::out);
    // app=append, out=overwrite

    NeuralNet* nn = create_NeuralNet(filename, 0); // verbose 0:False !0 True

    // Reading input data
    size_t num_samples = 1;
    size_t sample_idx = 0; // from 0 to (num_samples - 1)

    float** images = read_mnist_images(IMAGES_PATH, num_samples);
    
    // Check: save the images to .pgm for visualization
    std::string img_filename;
    for(int i=0; i<num_samples; i++){
        img_filename = folder_name + "cpp_input" +  ".pgm";
        save_pgm(img_filename, images[i]);
    }
    std::cout << "Saved" + img_filename + "\n";
    
    uchar* labels = read_mnist_labels(LABELS_PATH, num_samples);

    //std::cout << labels << "\n";
    //std::cout << "Loaded " << num_samples << " labels\n";
    //for(int i = 0; i < std::min(size_t(10), num_samples); i++)
    //    std::cout << "Label[" << i << "] = " << (int)labels[i] << "\n";



    // Forward pass

    
    // 1. Load input and true label
    int y_true = (int)labels[sample_idx];
    size_t image_size = 784;

    if(image_size != nn->sizes[0]){
        std::cerr << "Error in forward_pass(): Input dimension != neurons in the first layer.\n";
        return 1;
    }
    fout<< "Input: \n\n";
    for (size_t i = 0; i < nn->sizes[0]; i++) {
        nn->o_all[0][i] = images[sample_idx][i]; 
        fout<<nn->o_all[0][i]<<"\t";
    }
    fout<<"\n\n";

    // 2. Perform the forward pass

    for(int layer_idx = 1; layer_idx < nn->n_layers; layer_idx++){
        size_t nrows = nn->sizes[layer_idx];
        size_t ncols = nn->sizes[layer_idx - 1];
        layer_linear_transform(nn->o_all[layer_idx], nn->o_all[layer_idx - 1], nn->weights[layer_idx - 1], nn->biases[layer_idx - 1], nrows, ncols);
        if(layer_idx < (nn->n_layers - 1)){
            ReLu(nn->o_all[layer_idx], nrows);
        }
        //---check
        fout << "Layer [" << layer_idx << "]: \n";
        for(int i=0; i<nrows; i++){
            fout << nn->o_all[layer_idx][i] << "\t";
        }
        fout<< "\n\n";
    }

    // 3. Predict
    
    size_t last = nn->n_layers - 1;
    size_t y_pred = argmax(nn->o_all[last], nn->sizes[last]);

    std::cout << "Predicted label = " << y_pred << "\n";
    std::cout << "True label = " << y_true << "\n";

    // Cleanup
    fout.close();
    // data
    for(int i = 0; i < num_samples; i++)
        free(images[i]);
    free(images);
    free(labels);

    // neural net
    free_NeuralNet(nn);

    return 0;
};



