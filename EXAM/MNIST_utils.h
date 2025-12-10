#ifndef MNIST_UTILS_H
#define MNIST_UTILS_H

#include <string>
#include <fstream>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
typedef unsigned char uchar;


// MNIST image/label functions
int reverseInt(int);
float** read_mnist_images(std::string full_path, size_t& num_samples);
uchar* read_mnist_labels(const std::string& full_path, size_t& num_samples);
void save_pgm(const std::string& filename, float* image);


// Paths
extern const std::string IMAGES_PATH;
extern const std::string LABELS_PATH;

#endif