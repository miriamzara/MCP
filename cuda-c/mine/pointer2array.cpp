#include <iostream> // the io library of c++
#include <stdlib.h> // string manipulation - needed for malloc(), free()
using namespace std;

int main(){
    int arr[5] = {1,2,3,4,5};
    // when used in expressions, an array decays
    // automatically to a pointer to its first element ---> is an address
    cout << "arr =" << arr << endl;
    cout << "arr[0]= " << arr[0] << endl;
    cout << "&arr[0] =" << &arr[0] << endl;
    cout << endl;
    /* We can then define a pointer to the first element of the array in 
    two equivalent ways:
    */
    int* p; 
    int* q;
    p = arr; // this works
    q = &arr[0]; // also this works
    cout << "p= "<< p << endl;
    cout << "q= "<< q << endl;
    /*
    p and q are two pointers to an int. When increased, they move by sizeof(int)
    into the memory, and access the second element of the array, and so on.
    There is no need to use the dereference operator
         *(p+1)  is equivalent to    p[1]     
    */
    for (int i=0; i< 5; i++){
        cout << "p["<< i <<"]= " << p[i] <<"\t\t";
        cout << "*(p+"<<i<<")=" << *(p+i) << "\t";
        cout << "arr["<< i <<"]= " << arr[i] << endl;
    };
    cout << endl;
    cout << "Pointers to int and pointers to ARRAYS of ints" <<endl;
    /*
    Both exist and they are different.
    */
    int (* p2a)[5]; //declaration of a pointer to an array of 5 int
    p2a = &arr; //assignation
    /*
    p + 1 advances by sizeof(int) - getting the address of the next array element
    p2a + 1 advances by sizeof(int[3]) = 3 * sizeof(int) - getting the first address after the array
    */
    cout << endl;
    cout << "p+1="<< p+1 << endl;
    cout << "p2a + 1="<< p2a + 1 << endl;
    cout << endl;
    for(int i =0; i < 5; i++){
        cout << "&a["<< i <<"]= "<< &arr[i] << endl;
    }
    /*
    To access array elements with a pointer-to-array,
    you need to keep the first index to 0 - otherwise it increases past the array
    and choose the element by providing a second index.
    */
    cout << endl;
    for(int i = 0; i < 5; i++){
        cout << "p2a[0]["<<i<<"]= "<< p2a[0][i] << "\t\t";
        cout << "(*p2a)["<<i<<"]= "<<(*p2a)[i] << endl;
    }
    /*
    This generalized strainghtforwardly to matrices:
    */
    cout << endl;
    cout << "Using pointers to array for matrix notation" << endl;
    float elements[3][4] = {{1,2,3,4},{5,6,7,8},{9,10,11,12}};
    /*
    `elements` is a recursive array. 
    When used in expressions, it decays to a pointer to its first element... 
    ... which is itself an array.
    Therefore, a pointer to the first row is declared like:
    */
    float (* M)[4] = elements;
    /*
    Instead, if you want a pointer to the whole matrix, you need to declare:
    float (*prova)[3][4] = &elements;
    */
    for(int i=0; i <3; i++){
        for(int j=0; j < 4; j++){
            cout << "elements[i][j]= " << elements[i][j]<< "\t";
            cout << "M[i,j]=" << M[i][j] << endl;
        }
        cout << endl;
    }
    cout << endl << endl;
    cout << "INTERMEDIATE LEVEL: Casting pointers." << endl;
    float seq[12] = {1,2,3,4,5,6,7,8,9,10,11,12};
    float* r = seq; 
    float (*n)[4] = (float (*)[4])r; // reinterpret p as pointer to array of 3 floats
    /*
    Equivalently:
    where the casting is done in-line on `element` that has decayed to pointer
    float (*n)[4] = (float (*)[4])elements;
    */
    for(int i=0; i <3; i++){
        for(int j=0; j < 4; j++){
            cout << "seq[i*4 + j= " << seq[i*4 + j]<< "\t";
            cout << "n[i,j]=" << n[i][j] << endl;
        }
        cout << endl;
    }
    /*
    float (*prova)[4] =  ?address of first row?
    */
    return 0;
}