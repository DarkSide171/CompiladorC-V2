// Exemplo específico do C11
// TODO: Usar nos testes da Fase 1.3

#include <stdio.h>

// _Static_assert (C11)
_Static_assert(sizeof(int) >= 4, "int must be at least 4 bytes");

// _Generic (C11)
#define TYPE_NAME(x) _Generic((x), \
    int: "int", \
    float: "float", \
    double: "double", \
    default: "unknown")

int main() {
    int value = 42;
    printf("Type of value: %s\n", TYPE_NAME(value));
    
    return 0;
}