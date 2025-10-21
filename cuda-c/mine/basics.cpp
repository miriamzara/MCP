// #include <stdio.h> // the io library of c
#include <iostream> // the io library of c++
#include <stdlib.h> // string manipulation - needed for malloc(), free()
using namespace std;

int main(){
    //printf("guardiamo come funzionano i puntatori\n"); //plain c syntax
    cout << "guardiamo come funzionano i puntatori" << endl;
    int a = 2;
    int* p = &a;
    printf("a = %d\n", a);
    printf("&a = %p\n",&a);
    printf("p = %p\n", p); // the value of p is the address of a
    //printf("*p = %p\n", *p); // * extracts the value at that address (a)
    // Pointers to arrays
    int arr[4] = {10,20, 30, 40};
    //int* p = arr; // p was already declared. if you declare again it raises error
    p = arr; // but you can still re-use it
    for (int i = 0; i < 4; i++){
        cout << "p + "<< i << " = "  << p+i << endl;  //address of arr[i]
        cout << "&arr["<< i << "] = "  << &arr[i] << endl;  //address of arr[i]
        cout << "*(p + "<< i << ")=" << *(p+i) << endl; // element arr[i]
    }
    cout << "sizeof(p)= "<< sizeof(p) <<endl; // 8 bytes,
    //  i.e. the size of a memory address in a 64-bit architecture
    //
    // Strange behaviours of arrays
    /*
    arr automatically "decays" to a POINTER to the first element of the array
    when it is used in expressions:
    */
    cout << "\n\n Array che si comportano come puntatori: " << endl;
    cout<< "arr = " << arr << endl;
    cout << "&arr[0] = " << &arr[0] << endl << endl; 
    cout << "ora guardiamo come si usa malloc" << endl;
    // malloc() returns a pointer to the allocated address
    //
    // dynamic allocation of an int
    //
    //
    int* pp = (int *) malloc(sizeof(int));
    cout << "pp =" << pp << endl; // the actual address is 64 bits, 
    // here only 4x9 are shown probabily because the leading bits are zeros
    cout << "*pp =" << *pp << endl;  // = 0
    //cout << sizeof(int) << endl; // 4
    //cout << sizeof(pp) << endl;
    // filling the address execution time
    *pp = 2;
    cout << "After assignation:" << endl << "*pp =" << *pp << endl; 
    // free the address
    free(pp);
    // still, the address may still temporarily contain your variable...
    // until some other part of the code reclaims it
    cout << "After free(): " << endl;
    cout << "pp =" << pp << endl;
    cout << "*pp =" << *pp << endl; // = 0
    //
    //
    //
    // dynamic allocation of an array of ints
    cout << endl << endl << "Dynamic allocation of an array:" <<endl;
    int arr_size = 5;
    int* pp_a = (int *) malloc(arr_size * sizeof(int));
    cout << "pp_a =" << pp_a << endl; // this is the address of the first element
    for(int i = 0; i< arr_size; i++){
        // these are two equivalent ways to access the value of the array element:
        cout << "pp_a[i]= " << pp_a[i]  <<endl; // ! does NOT return an address, but the value contained ...
        cout << "*(pp_a + i) =" << *(pp_a + i) << endl; // ... exactly like this
        cout << "&pp_a[i]" << &pp_a[i] << endl;
        //*pp_a = 5 * i;
        //pp_a += 1; // increment address
    }
    cout << endl << "Now lets fill the array: " << endl;
    cout << "value\t address" << endl;
    for(int i = 0; i < arr_size; i++){
        pp_a[i] = i + 5;
    }
    for(int i = 0; i < arr_size; i++){
       cout << pp_a[i] << "\t" << &pp_a[i] << endl;
    }
    //
    //
    cout << "Copying blocks of memory: "<< endl;
    int * pp_b = (int*) malloc(arr_size * sizeof(int)); // you need to allocate memory for pp_b
    // otherwise you get a segmentation fault (trying to access memory locations that do not exist)
    cout << "pp_b ="<< pp_b << endl;
    memcpy(pp_b, pp_a, arr_size * sizeof(int));
    cout << "value\t address" << endl;
    for(int i = 0; i < arr_size; i++){
       cout << pp_b[i] << "\t" << &pp_b[i] << endl;
    }
    cout << endl << "Double Pointers: pointers to pointers" << endl;
    int some_value = 11;
    int*  r = &some_value; // pointer to a
    int** s = &r; // pointer to r

    // usage: dynamic allocation of 2d array
    int rows = 3, cols = 4;
    int** array = (int**) malloc(rows * sizeof(int*));  // allocate array of pointers
    for(int i = 0; i < rows; i++){
        array[i] = (int*) malloc(cols * sizeof(int));   // allocate each row
    }
    cout << "row \t col" << endl;
    for(int i= 0; i < rows; i++){
        for(int j =0; j < cols; j++){
            array[i][j] = i*j;
            cout << i << "\t" << j << "\t" << array[i][j] << endl;
        }
    }
    return(0);
}