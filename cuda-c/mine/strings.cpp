#include<string.h>
#include<iostream>

int main(){



    // C strings are just arrays of characters
    char a[] = "Hello, I am a C string"; // direct
    char* ptr = "I am also a C string"; // by pointer

    std::cout << a << "\n";
    std::cout << ptr << "\n";

    // Opening a file and reading text from it
    char* filename = "file_di_prova.txt";
    int chars_to_read = 7;
    char file_content[chars_to_read + 1]; // leave one space at the end for the null character terminator, '/0'
    FILE *file = fopen(filename, "r");  // fopen expects a pointer 
    fread(&file_content, sizeof(char), chars_to_read, file);
    std::cout << file_content << "\n";
    fclose(file);


    // C++ strings
    // needs the directive #include <string>

    // supports concatenation, with the operator +
    std::string name = "Miriam";
    std::string surname = "Zara";
    std::string whole_name = name + "\t" + surname;
    std::cout << whole_name << "\n";

    // can be converted back to a plain-C string
    // but needs to be made a const (-> cannot be modified anymore)
    const char* whole_name_c = whole_name.c_str();
    std::cout << whole_name_c << "\n";
}