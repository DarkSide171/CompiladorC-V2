#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Enumerações
enum Status {
    STATUS_OK = 0,
    STATUS_ERROR = -1,
    STATUS_PENDING = 1,
    STATUS_TIMEOUT = 2
};

// União
union Data {
    int integer;
    float floating;
    char character;
    void *pointer;
};

// Função recursiva
long long factorial(int n) {
    if (n <= 1) {
        return 1LL;
    }
    return n * factorial(n - 1);
}

// Função com ponteiro para função
int apply_operation(int a, int b, int (*operation)(int, int)) {
    return operation ? operation(a, b) : 0;
}

// Funções auxiliares
static int add(int x, int y) { return x + y; }
static int multiply(int x, int y) { return x * y; }

// Função principal com estruturas de controle complexas
int main(int argc, char *argv[]) {
    // Declarações com inicializadores
    int numbers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const size_t count = sizeof(numbers) / sizeof(numbers[0]);
    bool found = false;
    
    // Loop for tradicional
    for (size_t i = 0; i < count; ++i) {
        printf("Number[%zu] = %d\n", i, numbers[i]);
        
        // Switch aninhado
        switch (numbers[i] % 3) {
            case 0:
                printf("  Divisible by 3\n");
                break;
            case 1:
                printf("  Remainder 1 when divided by 3\n");
                break;
            case 2:
                printf("  Remainder 2 when divided by 3\n");
                break;
            default:
                printf("  Unexpected case\n");
                break;
        }
    }
    
    // Loop while com condições complexas
    int target = 7;
    size_t index = 0;
    while (index < count && !found) {
        if (numbers[index] == target) {
            found = true;
            printf("Found %d at index %zu\n", target, index);
        } else {
            ++index;
        }
    }
    
    // Loop do-while
    int attempts = 0;
    do {
        printf("Attempt %d\n", ++attempts);
        
        // Operador ternário aninhado
        enum Status status = (attempts < 3) ? STATUS_PENDING : 
                           (attempts < 5) ? STATUS_TIMEOUT : STATUS_ERROR;
        
        if (status == STATUS_ERROR) {
            break;
        }
        
    } while (attempts < 10);
    
    // Teste de ponteiros para função
    int result1 = apply_operation(10, 5, add);
    int result2 = apply_operation(10, 5, multiply);
    int result3 = apply_operation(10, 5, NULL);
    
    printf("10 + 5 = %d\n", result1);
    printf("10 * 5 = %d\n", result2);
    printf("NULL operation = %d\n", result3);
    
    // Teste de união
    union Data data;
    data.integer = 42;
    printf("Union as int: %d\n", data.integer);
    
    data.floating = 3.14f;
    printf("Union as float: %.2f\n", data.floating);
    
    // Cálculo de fatorial
    for (int i = 0; i <= 10; ++i) {
        printf("%d! = %lld\n", i, factorial(i));
    }
    
    // Goto (raramente usado, mas válido)
    int error_code = 0;
    if (argc > 10) {
        error_code = 1;
        goto cleanup;
    }
    
    printf("Normal execution path\n");
    
cleanup:
    if (error_code != 0) {
        printf("Error occurred, cleaning up...\n");
    }
    
    return error_code;
}