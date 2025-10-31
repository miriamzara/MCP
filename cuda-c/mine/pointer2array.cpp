#include <iostream> // the io library of c++
#include <stdlib.h> // string manipulation - needed for malloc(), free()
using namespace std;

int main(){
    int arr[5] = {1,2,3,4,5};
    cout << "arr[0]= " << arr[0] << endl;
    // when used in expressions, an array can decay 
    // to a pointer to its first element.
    // for instance:
    int* p; 
    int* q;
    p = arr; // this works
    q = &arr[0]; // and is equivalent to this
    cout << "&arr[0]= "<< &arr[0] << endl;
    cout << "p= "<< p << endl;
    cout << "q= "<< q << endl;

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
    cout << p+1 << endl;
    cout << p2a + 1 << endl;
    for(int i =0; i < 5; i++){
        cout << "&a[ "<< i <<" ]= "<< &arr[i] << endl;
    }
    return 0;
}