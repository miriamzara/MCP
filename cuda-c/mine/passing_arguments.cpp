# include <iostream>
using namespace std;


void increment_by_value(int x){
    // logic: int x = a ----> x is a new, independent object
    // whatever you do on x does not affect a
    x++;
}

void increment_by_pointer(int* x){
    // logic: int*x = &a ----> x becomes a pointer to a
    // to act on a, you need to dereference x
    (*x)++;
}

void increment_by_reference(int& x){
    /* logic: int&x = a ----> x is an alias (another name) to a
    what you do on x affects a
    C++ ONLY
    */
    x++;
}


int main(){
    int a = 5;
    cout << "a = " << a << endl;
    increment_by_value(a);
    cout << "a = " << a <<endl;
    increment_by_pointer(&a);
    cout << "a = " << a << endl;
    increment_by_reference(a);
    cout << "a = " << a << endl;
    return 0;
}