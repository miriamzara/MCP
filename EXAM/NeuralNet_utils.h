#ifndef NEURALNET_UTILS_H
#define NEURALNET_UTILS_H



#include <string>
#include <fstream>
#include <stdexcept>
#include <cstdio>
#include <iostream>
#include <cstdlib>

typedef struct{
    size_t n_layers;
    size_t* sizes; // array of layer sizes
    float** weights; // weights[i] is the (flat) array of weights for layer i
    float** biases; // biases[i] is the (flat) array of weights for layer i
} NeuralNet;

// Methods declarations

NeuralNet* create_NeuralNet(const std::string& relative_path, bool verbose);

void free_NeuralNet(NeuralNet* nn);

void ReLu(float* a, size_t length_a);

void layer_linear_transform(float* a, float* o_previous, float* layer_weights, size_t nrows, size_t ncols);

#endif