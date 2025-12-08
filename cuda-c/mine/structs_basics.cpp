#include<iostream>
using namespace std;


typedef struct{
    float x;
    float y;
} Point;


void translation(Point* ptr, float dx = 0.1, float dy = 0.2){
    // Operates translation on a point
    // pass by pointer.
    // to de-reference the pointer, you need to use the operator
    // ptr->x instead of (*ptr).x
    ptr->x += dx;
    ptr->y += dy;
}

int main(int argc, char** argv){

    // Command-line argument passing
    // argc = number of arguments passed to ./ , including program name
    if(argc < 2){
        std::cout<< "Usage: " << argv[0] << " <integer_value> \n";
        return 1;
    }

    int a = atoi(argv[1]); // argument-to-integer
    std::cout<< "Value of keyboard argument= "<< a << std::endl;

    int* array = (int*) malloc(a * sizeof(int));
    free(array);

    // Static allocation
    std::cout << "Static allocation" << std::endl;
    Point p;
    p.x = 12.3;
    p.y = 11;
    std::cout<< p.x << "\t" << p.y << std::endl;
    translation(&p);
    std::cout<< p.x << "\t" << p.y << std::endl;

    // Dynamic allocation (malloc)
    // works as usual for structs - which is great
    std::cout << "Dynamic allocation" << std::endl;
    Point* ppp = (Point*) malloc(sizeof(Point));
    ppp->x = 1;
    ppp->y = 3;
    std::cout<< ppp->x << "\t" << ppp->y << std::endl;
    free(ppp);

    return(0);
}