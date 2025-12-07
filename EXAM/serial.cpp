#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;


const int N_INPUT = 784;
const int N_HIDDEN = 256;
const int N_OUTPUT = 10;

const string w1_file =  "fc1.weight.txt";
const string b1_file =  "fc1.bias.txt";
const string w2_file =  "fc2.weight.txt";
const string b2_file =  "fc2.bias.txt";



float* read_weights(const string filename, size_t* nrows, size_t* ncols){
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

    float *weights = (float *)malloc(n_elements * sizeof(float)); // weights is a pointer to a float
    for (int i = 0; i < n_elements; i++) {
        fscanf(file, "%f", &weights[i]); // %d: int, %f: float
    }
    fclose(file);  
    return weights;  
}




int main(){
    size_t nrows, ncols;
    float* b1 = read_weights(b1_file, &nrows, &ncols);
    cout << "Reading b1" <<endl;
    cout << nrows << " " << ncols << "\n";
    for(int i = 0; i < ncols; i++){
        cout << b1[i] << "\n"; // 
    }
    cout << endl;

    free(b1);
    return 0;
}