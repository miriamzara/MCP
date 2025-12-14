#include "NeuralNet_utils.h"



void free_NeuralNet(NeuralNet* nn){

    for(int i=0; i< (nn->n_layers) - 1; i++){
        free(nn->weights[i]);
        free(nn->biases[i]);
        free(nn->o_all[i]);
    }

    free(nn->sizes);
    free(nn->weights);
    free(nn->biases);
    free(nn->o_all);
}

NeuralNet* create_NeuralNet(const std::string& relative_path, bool verbose=0){

    // 0 = False, any non-zero int is True

    // pass string by reference
    // zero copying → no extra memory allocation
	// cannot be null → guarantees safety
	// cannot be modified (because of const)

    FILE *file = fopen(relative_path.c_str(), "rb"); // r= read, b= binary
    if (file == NULL) {
        std::cerr << "Error in create_NeuralNet(): could not open file.\n";
        return nullptr; // if allocation fails, return a null pointer
    }


    // Populate the struct
    NeuralNet* net = (NeuralNet*) malloc(sizeof(NeuralNet)); 
    /* 
        allocates space for the struct - which is made of pointers,
        therefore it is not necessary to know in advance how many layers etc
        the network has.

        fread syntax: address where to store, how many bits to read, how many times, from where
    */

    // layers and sizes
    fread(&(net->n_layers), sizeof(size_t), 1, file); 
    net->sizes = (size_t*) malloc(net->n_layers * sizeof(size_t));
    fread(net->sizes, sizeof(size_t), net->n_layers, file);

    if(verbose){
        std::cout << "Loading the neural network... \n";
        std::cout << "N layers: "<< net->n_layers << "\n\n";
        std::cout << "Layer sizes: \t";
        for(int i=0; i< net->n_layers; i++){
            std::cout << net->sizes[i] << "\t";
        }
        std::cout<<"\n\n";
    }

    // weights and biases
    net->weights = (float **)malloc( (net->n_layers - 1) * sizeof(float*));
    net->biases = (float **)malloc( (net->n_layers - 1) * sizeof(float*));
    

    for(int layer_idx=0; layer_idx <= net->n_layers - 2; layer_idx++){

        size_t ncols = net->sizes[layer_idx];
        size_t nrows = net->sizes[layer_idx + 1];
        size_t tot_elements = nrows * ncols;

        //weights
        net->weights[layer_idx] = (float*)malloc( tot_elements * sizeof(float));
        fread(net->weights[layer_idx], sizeof(float), tot_elements, file);

        if(verbose){
            std::cout << "Loaded weights of layer " << layer_idx << std::endl;
            // Check - print a small subset of the weight matrix
            for(int i = 0; i < std::min(nrows, size_t(10)); i++){
                std::cout << "\n";
                for(int j=0; j < std::min(ncols, size_t(10)); j++){
                    std::cout <<  net->weights[layer_idx][i*ncols + j] << "\t"; 
                }
            }
            std::cout<<"\n";
        }
        if(verbose){ std::cout << "\n";}

        //biases
        net->biases[layer_idx] = (float*)malloc(nrows*sizeof(float));
        fread(net->biases[layer_idx], sizeof(float), nrows, file);

        if(verbose){
            std::cout << "Loaded bias of layer " << layer_idx << std::endl;
            // Check - print a small subset of the weight matrix
            for(int i = 0; i < std::min(nrows, size_t(10)); i++){
                std::cout << "\n";
                std::cout <<  net->biases[layer_idx][i] << "\t"; 
            }
            std::cout<<"\n";
        }
        if(verbose){ std::cout << "\n";}
    }
    fclose(file);

    // allocate output arrays
    net->o_all = (float **)malloc( (net->n_layers) * sizeof(float*));
    for(int layer_idx = 0; layer_idx < net->n_layers; layer_idx++){
        net->o_all[layer_idx] = (float*)malloc( net->sizes[layer_idx] * sizeof(float) );
    }

    return net;
}



void ReLu(float* a, size_t length_a){
    for(size_t i=0; i< length_a; i++){
        a[i] = (a[i] > 0.0f) ? a[i] : 0.0f;
    }
}

void layer_linear_transform(float* a, float* o_previous, float* layer_weights, float* layer_biases, size_t nrows, size_t ncols){
    /*
    layer_linear_transform() stands for Layer Linear Transformation
    a: input of layer (n) - before activation
    o: output of layer (n-1)
    nrows: number of neurons in layer n
    ncols: number of neurons in layer (n-1)
    layer_weights: flat array of nrows*ncols elements

    Computes a_n = weights @ o_(n-1)
    Overwrites the pointer a that is provided as input
    */

    if(a == nullptr || o_previous == nullptr || layer_weights == nullptr){
        std::cerr << "Error in layer_linear_transform(): detected null pointers.\n";
        return;
    }

    for(size_t i = 0; i < nrows; i++){
        float sum = 0;
        for(size_t j=0; j<ncols; j++){
            sum += o_previous[j]*layer_weights[ncols*i + j];
        }
        a[i] = sum + layer_biases[i];
    }
}


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