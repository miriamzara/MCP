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
    std::string filename = "nn.bin";
    NeuralNet* nn = create_NeuralNet(filename, 0);
    

    // Reading input data
    size_t num_samples = 5;
    size_t sample_idx = 4; // from 0 to (num_samples - 1)

    uchar** images = read_mnist_images(IMAGES_PATH, num_samples);
    /*
    // Check: save the images to .pgm for visualization
    std::string img_filename;
    for(int i=0; i<num_samples; i++){
        img_filename = "mnist_sample" + std::to_string(i) + ".pgm";
        save_pgm(img_filename, images[i]);
    }
    std::cout << "Saved mnist_sample.pgm\n";
    */
    uchar* labels = read_mnist_labels(LABELS_PATH, num_samples);

    //std::cout << "Loaded " << num_samples << " labels\n";
    //for(int i = 0; i < std::min(size_t(10), num_samples); i++)
    //    std::cout << "Label[" << i << "] = " << (int)labels[i] << "\n";



    // Forward pass

    
    // 1. Pre- allocate arrays to store the output of each layer
    // and the predicted label

    float** o_all = (float**)malloc( (nn->n_layers) * sizeof(float*) );
    for(int layer_idx = 0; layer_idx < nn->n_layers; layer_idx++){
        o_all[layer_idx] = (float*)malloc( nn->sizes[layer_idx] * sizeof(float) );
    }

    // 2. Load input and true label
    uchar y_true = labels[sample_idx];
    size_t image_size = 784;

    if(image_size != nn->sizes[0]){
        std::cerr << "Error in forward_pass(): Input dimension != neurons in the first layer.\n";
        return 1;
    }
    for (size_t i = 0; i < nn->sizes[0]; i++) {
        o_all[0][i] = static_cast<float>(images[sample_idx][i]); 
    }

    // 3. Perform the forward pass
    std::cout << "Forward pass \n\n";

    for(int layer_idx = 1; layer_idx < nn->n_layers; layer_idx++){
        //std::cout << "Layer [" << layer_idx << "]: \n\n";
        size_t nrows = nn->sizes[layer_idx];
        size_t ncols = nn->sizes[layer_idx - 1];
        layer_linear_transform(o_all[layer_idx], o_all[layer_idx - 1], nn->weights[layer_idx - 1], nrows, ncols);

        //---check
        /*
        std::cout << "Before activation: \n";
        for(int i=0; i<nrows; i++){
            std::cout << o_all[layer_idx][i] << "\t";
        }
        std::cout<< "\n\n";
        */
        if(layer_idx < (nn->n_layers - 1)){
            ReLu(o_all[layer_idx], nrows);

            //---check
            /*
            std::cout << "After activation: \n";
            for(int i=0; i<nrows; i++){
                std::cout << o_all[layer_idx][i] << "\t";
            }
            std::cout<< "\n\n";
            */
        }
    }

    // 4. Predict
    /*
    size_t last = nn->n_layers - 1;
    float* logits = o_all[last];

    size_t y_pred = 0;
    float max_logit = logits[0];

    for(size_t i = 1; i < nn->sizes[last]; i++){
        if(logits[i] > max_logit){
            max_logit = logits[i];
            y_pred = i;
        }
    }
    */

    std::cout << "Predicted label = " << (int)y_pred << "\n";
    std::cout << "True label = " << (int)y_true << "\n";

    // Cleanup

    // data
    for(int i = 0; i < num_samples; i++)
        free(images[i]);
    free(images);
    free(labels);

    // neural net
    for(int layer_idx = 1; layer_idx < nn->n_layers; layer_idx++){
        free(o_all[layer_idx]);
    }
    free(o_all);
    free_NeuralNet(nn);

    return 0;
};



