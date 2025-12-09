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
uchar** read_mnist_images(std::string full_path, int& num_images_to_load, int& number_of_images, int& image_size);
uchar* read_mnist_labels(const std::string& full_path, int& number_of_labels);
void save_pgm(const std::string& filename, uchar* data);

// Paths
extern const std::string IMAGES_PATH;
extern const std::string LABELS_PATH;

#endif