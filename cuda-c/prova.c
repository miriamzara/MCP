#include <stdio.h>

int main(){
    printf("guardiamo come funzionano i puntatori\n");
    int a = 2;
    int* p = &a;
    printf("a = %d\n", a);
    printf("&a = %p\n",&a);
    printf("p = %p\n", p);
    return(0);
}