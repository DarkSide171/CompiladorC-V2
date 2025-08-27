// Teste básico de operações aritméticas
#include <stdio.h>

#define PI 3.14159
#define SQUARE(x) ((x) * (x))

int main() {
    int a = 10;
    int b = 5;
    float radius = 2.5;
    
    int sum = a + b;
    int diff = a - b;
    int product = a * b;
    int quotient = a / b;
    int remainder = a % b;
    
    float area = PI * SQUARE(radius);
    
    printf("Soma: %d\n", sum);
    printf("Diferença: %d\n", diff);
    printf("Produto: %d\n", product);
    printf("Quociente: %d\n", quotient);
    printf("Resto: %d\n", remainder);
    printf("Área do círculo: %.2f\n", area);
    
    return 0;
}