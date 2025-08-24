#include <stdio.h>

int main() {
    int x = 42;
    float pi = 3.14159;
    char letter = 'A';
    char* message = "Hello, World!";
    
    printf("x = %d\n", x);
    printf("pi = %.2f\n", pi);
    printf("letter = %c\n", letter);
    printf("%s\n", message);
    
    return 0;
}