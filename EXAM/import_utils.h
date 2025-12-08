#ifndef IMPORT_UTILS_H
#define IMPORT_UTILS_H

#include <string>
#include <fstream>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
typedef unsigned char uchar;
using std::string;

// MNIST image/label functions
uchar** read_mnist_images(string full_path, int& number_of_images, int& image_size);
uchar* read_mnist_labels(const string& full_path, int& number_of_labels);
void save_pgm(const string& filename, uchar* data, int rows, int cols);

// Neural network weights
float* read_weights(const string filename, size_t* nrows, size_t* ncols);

// Paths and NN sizes
extern const string IMAGES_PATH;
extern const string LABELS_PATH;

extern const int N_INPUT;
extern const int N_HIDDEN;
extern const int N_OUTPUT;

extern const string w1_file;
extern const string b1_file;
extern const string w2_file;
extern const string b2_file;

#endif