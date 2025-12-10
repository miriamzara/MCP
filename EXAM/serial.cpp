#include <iostream>
#include "MNIST_utils.h"
#include "NeuralNet_utils.h"

// Compile main script + source files together, e.g.
// g++ -std=c++11 serial.cpp MNIST_utils.cpp  NeuralNet_utils.cpp -o serial.out


size_t argmax(float* logits, size_t length){
    size_t y_pred = 0;
    float max_logit = logits[0];
    for(size_t i = 1; i < length; i++){
        if(logits[i] > max_logit){
            max_logit = logits[i];
            y_pred = i;
        }
    }
    return y_pred;
}








int main(){

    // Reading the neural network
    std::string filename = "neural_networks/nn.bin";
    NeuralNet* nn = create_NeuralNet(filename, 1); // verbose 0:False !0 True
    

    // Reading input data
    size_t num_samples = 1;
    size_t sample_idx = 0; // from 0 to (num_samples - 1)

    float** images = read_mnist_images(IMAGES_PATH, num_samples);
    
    // Check: save the images to .pgm for visualization
    std::string img_filename;
    for(int i=0; i<num_samples; i++){
        img_filename = "mnist_sample" + std::to_string(i) + ".pgm";
        save_pgm(img_filename, images[i]);
    }
    std::cout << "Saved mnist_sample.pgm\n";
    
    uchar* labels = read_mnist_labels(LABELS_PATH, num_samples);

    //std::cout << "Loaded " << num_samples << " labels\n";
    //for(int i = 0; i < std::min(size_t(10), num_samples); i++)
    //    std::cout << "Label[" << i << "] = " << (int)labels[i] << "\n";



    // Forward pass

    
    // 1. Load input and true label
    uchar y_true = labels[sample_idx];
    size_t image_size = 784;

    if(image_size != nn->sizes[0]){
        std::cerr << "Error in forward_pass(): Input dimension != neurons in the first layer.\n";
        return 1;
    }
    std::cout<< "Input: \n\n";
    for (size_t i = 0; i < nn->sizes[0]; i++) {
        nn->o_all[0][i] = images[sample_idx][i]; 
        std::cout<<nn->o_all[0][i]<<"\t";
    }
    std::cout<<"\n\n";

    // 2. Perform the forward pass
    std::cout << "Forward pass \n\n";

    for(int layer_idx = 1; layer_idx < nn->n_layers; layer_idx++){
        std::cout << "Layer [" << layer_idx << "]: \n\n";
        size_t nrows = nn->sizes[layer_idx];
        size_t ncols = nn->sizes[layer_idx - 1];
        layer_linear_transform(nn->o_all[layer_idx], nn->o_all[layer_idx - 1], nn->weights[layer_idx - 1], nn->biases[layer_idx - 1], nrows, ncols);

        //---check
        
        std::cout << "Before activation: \n";
        for(int i=0; i<nrows; i++){
            std::cout << nn->o_all[layer_idx][i] << "\t";
        }
        std::cout<< "\n\n";
        
        if(layer_idx < (nn->n_layers - 1)){
            ReLu(nn->o_all[layer_idx], nrows);

            //---check
            
            std::cout << "After activation: \n";
            for(int i=0; i<nrows; i++){
                std::cout << nn->o_all[layer_idx][i] << "\t";
            }
            std::cout<< "\n\n";
            
        }
    }

    // 3. Predict
    
    size_t last = nn->n_layers - 1;
    size_t y_pred = argmax(nn->o_all[last], nn->sizes[last]);

    std::cout << "Predicted label = " << (int)y_pred << "\n";
    std::cout << "True label = " << (int)y_true << "\n";

    // Cleanup

    // data
    for(int i = 0; i < num_samples; i++)
        free(images[i]);
    free(images);
    free(labels);

    // neural net
    free_NeuralNet(nn);

    return 0;
};



