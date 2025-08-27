/* 
 * Teste de funcionalidades específicas do C89 (ANSI C)
 * Padrão: ISO/IEC 9899:1990
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>

/* Macros típicas do C89 */
#define MAX_BUFFER 256
#define SQUARE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* Declarações de função no estilo C89 */
int calculate_sum(a, b)
int a, b;
{
    return a + b;
}

/* Função com parâmetros void explícito */
void print_header(void)
{
    printf("=== C89 Feature Test ===");
}

/* Estrutura com campos típicos */
struct Point {
    int x;
    int y;
    char label[MAX_BUFFER];
};

/* União para demonstrar uso de memória compartilhada */
union Number {
    int i;
    float f;
    double d;
};

/* Enumeração simples */
enum Status {
    STATUS_OK,
    STATUS_ERROR,
    STATUS_PENDING
};

int main(void)
{
    /* Declarações no início do bloco (C89 requirement) */
    int numbers[10];
    struct Point origin;
    union Number value;
    enum Status current_status;
    int i;
    
    /* Inicialização de estrutura */
    origin.x = 0;
    origin.y = 0;
    strcpy(origin.label, "Origin Point");
    
    /* Uso de união */
    value.i = 42;
    printf("Integer value: %d\n", value.i);
    
    value.f = 3.14f;
    printf("Float value: %.2f\n", value.f);
    
    /* Loop com declaração de variável no início */
    for (i = 0; i < 10; i++) {
        numbers[i] = SQUARE(i);
    }
    
    /* Uso de enumeração */
    current_status = STATUS_OK;
    
    /* Operadores típicos do C89 */
    if (current_status == STATUS_OK && origin.x >= 0) {
        printf("Status is OK and origin is valid\n");
    }
    
    /* Demonstração de limites do sistema */
    printf("INT_MAX: %d\n", INT_MAX);
    printf("CHAR_BIT: %d\n", CHAR_BIT);
    printf("FLT_MAX: %e\n", FLT_MAX);
    
    /* Chamada de função com estilo C89 */
    printf("Sum: %d\n", calculate_sum(10, 20));
    
    print_header();
    
    return 0;
}

/* Função com múltiplos parâmetros no estilo C89 */
int complex_calculation(a, b, c, operation)
int a, b, c;
char operation;
{
    switch (operation) {
        case '+':
            return a + b + c;
        case '*':
            return a * b * c;
        case 'm': /* max */
            return MAX(MAX(a, b), c);
        default:
            return 0;
    }
}