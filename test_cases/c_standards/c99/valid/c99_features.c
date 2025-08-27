/*
 * Teste de funcionalidades específicas do C99
 * Padrão: ISO/IEC 9899:1999
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>  // C99: Tipo bool
#include <stdint.h>   // C99: Tipos de tamanho fixo
#include <inttypes.h> // C99: Macros para printf/scanf
#include <complex.h>  // C99: Números complexos
#include <tgmath.h>   // C99: Type-generic math

// C99: Comentários de linha única
// Este é um comentário válido em C99

// C99: Macros com argumentos variáveis
#define DEBUG_PRINT(fmt, ...) printf("[DEBUG] " fmt "\n", __VA_ARGS__)
#define LOG_INFO(msg) printf("[INFO] %s\n", msg)

// C99: Função inline
inline int square(int x) {
    return x * x;
}

// C99: Função com restrict
void copy_arrays(int * restrict dest, const int * restrict src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

int main(void)
{
    // C99: Declaração de variáveis em qualquer lugar
    printf("=== C99 Features Test ===\n");
    
    // C99: Tipo bool
    bool is_valid = true;
    bool is_complete = false;
    
    // C99: Tipos de tamanho fixo
    int8_t  small_int = 127;
    int16_t medium_int = 32767;
    int32_t large_int = 2147483647;
    uint64_t huge_int = 18446744073709551615ULL;
    
    // C99: Declaração no for loop
    for (int i = 0; i < 5; i++) {
        printf("Loop iteration: %d\n", i);
    }
    
    // C99: Array de tamanho variável (VLA)
    int size = 10;
    int vla[size];
    
    // Inicializar VLA
    for (int j = 0; j < size; j++) {
        vla[j] = j * j;
    }
    
    // C99: Inicialização de array com designadores
    int sparse_array[10] = {[0] = 1, [2] = 4, [4] = 16, [9] = 81};
    
    // C99: Inicialização de estrutura com designadores
    struct Point {
        int x, y;
        char *label;
    } origin = {.x = 0, .y = 0, .label = "Origin"};
    
    // C99: Literal composto
    struct Point *temp_point = &(struct Point){.x = 5, .y = 10, .label = "Temp"};
    
    // C99: Números complexos
    double complex z1 = 1.0 + 2.0 * I;
    double complex z2 = 3.0 + 4.0 * I;
    double complex result = z1 + z2;
    
    printf("Complex result: %.1f + %.1fi\n", creal(result), cimag(result));
    
    // C99: Literais hexadecimais de ponto flutuante
    double hex_float = 0x1.8p3; // 1.5 * 2^3 = 12.0
    printf("Hex float: %.1f\n", hex_float);
    
    // C99: Uso de macros com argumentos variáveis
    DEBUG_PRINT("Testing with values: %d, %d", 42, 84);
    LOG_INFO("C99 features working correctly");
    
    // C99: Uso de restrict
    int source[5] = {1, 2, 3, 4, 5};
    int destination[5];
    copy_arrays(destination, source, 5);
    
    // C99: Printf com tipos de tamanho fixo
    printf("Fixed-size types:\n");
    printf("int8_t: %" PRId8 "\n", small_int);
    printf("int16_t: %" PRId16 "\n", medium_int);
    printf("int32_t: %" PRId32 "\n", large_int);
    printf("uint64_t: %" PRIu64 "\n", huge_int);
    
    // C99: Uso de bool
    if (is_valid && !is_complete) {
        printf("Status: Valid but incomplete\n");
    }
    
    // C99: Declaração de variável após statement
    printf("About to declare a new variable...\n");
    int late_declaration = 999;
    printf("Late declaration value: %d\n", late_declaration);
    
    // C99: Array flexível (flexible array member)
    struct FlexibleArray {
        size_t count;
        int data[]; // C99: Flexible array member
    };
    
    // Alocar estrutura com array flexível
    struct FlexibleArray *flex = malloc(sizeof(struct FlexibleArray) + 5 * sizeof(int));
    if (flex) {
        flex->count = 5;
        for (size_t k = 0; k < flex->count; k++) {
            flex->data[k] = (int)(k * 10);
        }
        
        printf("Flexible array contents: ");
        for (size_t k = 0; k < flex->count; k++) {
            printf("%d ", flex->data[k]);
        }
        printf("\n");
        
        free(flex);
    }
    
    return 0;
}

// C99: Função com VLA como parâmetro
void process_matrix(int rows, int cols, int matrix[rows][cols]) {
    printf("Processing %dx%d matrix\n", rows, cols);
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = i * cols + j;
        }
    }
}