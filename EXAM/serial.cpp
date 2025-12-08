#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;
typedef unsigned char uchar;



// --------------- IMPORT MNIST IMAGES AND LABELS ---------------------------------------------

// Source - https://stackoverflow.com/a
// Posted by dariush, modified by community. See post 'Timeline' for change history
// Retrieved 2025-12-08, License - CC BY-SA 3.0


const string IMAGES_PATH = "/Users/miriamzara/MCP/EXAM/data/MNIST/raw/t10k-images-idx3-ubyte";
const string LABELS_PATH = "/Users/miriamzara/MCP/EXAM/data/MNIST/raw/t10k-labels-idx1-ubyte";

uchar** read_mnist_images(string full_path, int& number_of_images, int& image_size) {
    // number_of_images and image_size are output parameters (will be inferred and filled up)
    auto reverseInt = [](int i) {
        unsigned char c1, c2, c3, c4;
        c1 = i & 255, c2 = (i >> 8) & 255, c3 = (i >> 16) & 255, c4 = (i >> 24) & 255;
        return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
    };

    typedef unsigned char uchar;

    ifstream file(full_path, ios::binary);

    if(file.is_open()) {
        int magic_number = 0, n_rows = 0, n_cols = 0;

        file.read((char *)&magic_number, sizeof(magic_number));
        magic_number = reverseInt(magic_number);

        if(magic_number != 2051) throw runtime_error("Invalid MNIST image file!");

        file.read((char *)&number_of_images, sizeof(number_of_images)), number_of_images = reverseInt(number_of_images);
        file.read((char *)&n_rows, sizeof(n_rows)), n_rows = reverseInt(n_rows);
        file.read((char *)&n_cols, sizeof(n_cols)), n_cols = reverseInt(n_cols);

        image_size = n_rows * n_cols;

        uchar** _dataset = new uchar*[number_of_images];
        for(int i = 0; i < number_of_images; i++) {
            _dataset[i] = new uchar[image_size];
            file.read((char *)_dataset[i], image_size);
        }
        return _dataset;
    } else {
        throw runtime_error("Cannot open file `" + full_path + "`!");
    }
}


uchar* read_mnist_labels(const string& full_path, int& number_of_labels) {
    auto reverseInt = [](int i) {
        unsigned char c1 = i & 255, c2 = (i >> 8) & 255, c3 = (i >> 16) & 255, c4 = (i >> 24) & 255;
        return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
    };

    ifstream file(full_path, ios::binary);
    if(!file.is_open())
        throw runtime_error("Cannot open file `" + full_path + "`!");

    int magic_number = 0;
    file.read((char*)&magic_number, sizeof(magic_number));
    magic_number = reverseInt(magic_number);
    if(magic_number != 2049) throw runtime_error("Invalid MNIST label file!");
    file.read((char*)&number_of_labels, sizeof(number_of_labels));
    number_of_labels = reverseInt(number_of_labels);
    uchar* labels = new uchar[number_of_labels];
    file.read((char*)labels, number_of_labels);
    if(file.gcount() != number_of_labels)
        throw runtime_error("Failed to read all labels");

    return labels;
}

void save_pgm(const string& filename, uchar* data, int rows, int cols) {
    ofstream out(filename, ios::binary);
    out << "P5\n" << cols << " " << rows << "\n255\n";
    out.write((char*)data, rows * cols);
    out.close();
}


// --------------- IMPORT NEURAL NETWORK WEIGHTS ---------------------------------------------

const int N_INPUT = 784;
const int N_HIDDEN = 256;
const int N_OUTPUT = 10;

const string w1_file =  "fc1.weight.txt";
const string b1_file =  "fc1.bias.txt";
const string w2_file =  "fc2.weight.txt";
const string b2_file =  "fc2.bias.txt";

float* read_weights(const string filename, size_t* nrows, size_t* ncols){
    /*  
    nrows and ncols are inferred automatically from the file and filled up
    stores the weights in a flat 1D array, row-wise
    */
    FILE *file = fopen(filename.c_str(), "r");
    if (file == NULL) {
        printf("Could not open file.\n");
        return NULL;
    }
    unsigned long long r, c;
    if (fscanf(file, "%llu %llu", &r, &c) != 2) {
        printf("Error reading dimensions.\n");
        fclose(file);
        return NULL;
    }
    *nrows = (size_t)r; 
    *ncols = (size_t)c;
    size_t n_elements = (*nrows) * (*ncols);
    float *weights = (float *)malloc(n_elements * sizeof(float)); 
    for (int i = 0; i < n_elements; i++) {
        fscanf(file, "%f", &weights[i]); // %d: int, %f: float
    }
    fclose(file);
    return weights;  
}





int main(){

    
    // Test: read_weights()
    string filename = "neural_networks/prova.txt";
    size_t nrows, ncols;
    float* w = read_weights(filename, &nrows, &ncols);
    cout << "Reading weights..." <<endl;
    cout << nrows << " " << ncols << "\n";
    for(int i = 0; i < nrows; i++){
        cout << "\n";
        for(int j=0; j < ncols; j++){
            cout << w[i*ncols + j] << "\t"; 
        }
    }
    cout << endl;
    free(w);
    

    // Test: read_mnist_images(), read_mnist_labels()
    int n_images, image_size;
    uchar** images = read_mnist_images(IMAGES_PATH, n_images, image_size);
    cout << "Loaded " << n_images << " images\n";
    cout << "Each image has " << image_size << " pixels\n";
    int rows = 28;
    int cols = 28;
    save_pgm("mnist_sample.pgm", images[0], rows, cols);
    cout << "Saved mnist_sample.pgm\n";

    int n_labels;
    uchar* labels = read_mnist_labels(LABELS_PATH, n_labels);

    cout << "Loaded " << n_labels << " labels\n";

    for(int i = 0; i < min(100, n_labels); i++)
        cout << "Label[" << i << "] = " << (int)labels[i] << "\n";

    // Cleanup
    for(int i = 0; i < n_images; i++)
        delete[] images[i];
    delete[] images;
    delete[] labels;

    return 0;
}
