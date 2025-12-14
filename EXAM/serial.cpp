#include <iostream>
#include "MNIST_utils.h"
#include "NeuralNet_utils.h"
#include <fstream>
#include <chrono>

// Compile main script + source files together, e.g.
// g++ -std=c++11 serial.cpp MNIST_utils.cpp  NeuralNet_utils.cpp -o serial.out

//std::string folder_name = "serial_code_validation/"; // only use for validation of the Forward Pass code against Pytorch reference
const std::string OUTPUT_FOLDER = "serial_benchmark/";
const std::string INPUT_FOLDER = "pytorch_nets/";

// --- Benchmark ---
size_t N_SAMPLES = 20;
const int N_RUNS = 100;
std::chrono::time_point<std::chrono::high_resolution_clock> start;
std::chrono::time_point<std::chrono::high_resolution_clock> end;
std::chrono::duration<float> elapsed; // duration is an ojbect that contains the number of ticks

int main(){

    // --- Input ---
    std::string filename = INPUT_FOLDER + "mlp_H1024_L1.bin";
    // --- Output ---
    std::ofstream fout(OUTPUT_FOLDER + "JETSON_benchmark_H1024_L1.csv", std::ios::app); // app=append, out=overwrite
    for(int run_idx = 0; run_idx < N_RUNS; run_idx ++){
        fout << "run " << run_idx;
        if (run_idx < N_RUNS - 1){
            fout << ",";
        }
    }
    fout << "\n";

    // --- Load network and input samples ---
    NeuralNet* nn = create_NeuralNet(filename, 0);
    float** images = read_mnist_images(IMAGES_PATH, N_SAMPLES);
    uchar* labels = read_mnist_labels(LABELS_PATH, N_SAMPLES);

    for(int sample_idx = 0; sample_idx < int(N_SAMPLES); sample_idx ++){

        // 1. Load input and true label
        int y_true = (int)labels[sample_idx];
        size_t image_size = 784;

        // 2. Perform the forward pass
        for(int run_idx = 0; run_idx < N_RUNS; run_idx ++){
            start = std::chrono::high_resolution_clock::now();
            ForwardPass(nn, images[sample_idx]);
            end = std::chrono::high_resolution_clock::now();    
            elapsed = end - start; 
            float elapsed_time_ms =  elapsed.count() * 1000;
            //std::cout << "Elapsed time (ms): "<< elapsed_time_ms << "\n" ; 
            fout << elapsed_time_ms;
            if (run_idx < N_RUNS - 1){
                fout << ",";
            }
        }
        fout << "\n";

        // 3. Predict
        size_t y_pred = Predict(nn);
        std::cout << "Predicted label = " << (int)y_pred << "\n";
        std::cout << "True label = " << y_true << "\n";
    }


    // Cleanup
    fout.close();

    for(int i = 0; i < N_SAMPLES; i++)
        free(images[i]);
    free(images);
    free(labels);
    free_NeuralNet(nn);

    return 0;
};



