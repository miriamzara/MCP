#include "MNIST_utils.h"

// Paths
const std::string IMAGES_PATH = "/Users/miriamzara/MCP/EXAM/data/MNIST/raw/t10k-images-idx3-ubyte";
const std::string LABELS_PATH = "/Users/miriamzara/MCP/EXAM/data/MNIST/raw/t10k-labels-idx1-ubyte";



// ---------------- MNIST functions -----------------

int reverseInt(int i){
    unsigned char c1, c2, c3, c4;
    c1 = i & 255, c2 = (i >> 8) & 255, c3 = (i >> 16) & 255, c4 = (i >> 24) & 255;
    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}


uchar** read_mnist_images(std::string full_path, size_t& num_samples) {
    /*
    full_path: path of the binary file (-ubyte) containing the images data
    num_samples: number of images to load
    */
    std::ifstream file(full_path, std::ios::binary);
    if(file.is_open()) {

        // Check magic number
        int magic_number = 0;
        file.read((char *)&magic_number, sizeof(magic_number));
        magic_number = reverseInt(magic_number);
        if(magic_number != 2051) throw std::runtime_error("Invalid MNIST image file!");

        // Retrieve number_of_images and compute image size
        int n_rows = 0, n_cols = 0;
        int number_of_images = 0;
        int image_size = 0;
        file.read((char *)&number_of_images, sizeof(number_of_images)), number_of_images = reverseInt(number_of_images);
        file.read((char *)&n_rows, sizeof(n_rows)), n_rows = reverseInt(n_rows);
        file.read((char *)&n_cols, sizeof(n_cols)), n_cols = reverseInt(n_cols);
        image_size = n_rows * n_cols;

        // Populate the dataset
        if(num_samples > size_t(number_of_images)){
            throw std::runtime_error("Error in read_mnist_images(): num_samples > number_of_images.");
        }

        uchar** _dataset = (uchar**)malloc(num_samples * sizeof(uchar*));
        for(int i = 0; i < num_samples; i++) {
            _dataset[i] = (uchar*) malloc(image_size* sizeof(uchar));
            file.read((char *)_dataset[i], image_size);
        }
        return _dataset;
    } else {
        throw std::runtime_error("Cannot open file `" + full_path + "`!");
    }
}

uchar* read_mnist_labels(const std::string& full_path, size_t& num_samples) {
    std::ifstream file(full_path, std::ios::binary);
    if(!file.is_open())
        throw std::runtime_error("Cannot open file `" + full_path + "`!");
    int magic_number = 0;
    file.read((char*)&magic_number, sizeof(magic_number));
    magic_number = reverseInt(magic_number);
    if(magic_number != 2049) throw std::runtime_error("Invalid MNIST label file!");

    int number_of_labels = 0;
    file.read((char*)&number_of_labels, sizeof(number_of_labels));
    number_of_labels = reverseInt(number_of_labels);

    if(num_samples > size_t(number_of_labels)){
        throw std::runtime_error("Error in read_mnist_labels(): num_samples > number_of_labels.");
    }

    uchar* labels = (uchar*) malloc(num_samples * sizeof(uchar));
    file.read((char*)labels, num_samples);

    if(file.gcount() != num_samples)
        throw std::runtime_error("Failed to read all labels");
    return labels;
}

void save_pgm(const std::string& filename, uchar* data) {
    int rows = 28;
    int cols = 28;
    std::ofstream out(filename, std::ios::binary);
    out << "P5\n" << cols << " " << rows << "\n255\n";
    out.write((char*)data, rows * cols);
    out.close();
}