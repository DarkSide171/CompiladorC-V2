/*
 * Teste de funcionalidades específicas do C11
 * Inclui _Generic, _Static_assert, _Alignas, _Alignof, atomics básicos
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>
#include <stdnoreturn.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>

// _Static_assert para verificações em tempo de compilação
_Static_assert(sizeof(int) >= 4, "int deve ter pelo menos 4 bytes");
_Static_assert(sizeof(void*) == sizeof(size_t), "Ponteiro e size_t devem ter o mesmo tamanho");

// Teste de alinhamento com _Alignas
_Alignas(16) char aligned_buffer[64];
_Alignas(double) int aligned_int;
_Alignas(32) struct {
    int x, y, z;
} aligned_struct;

// _Generic para seleção de tipo genérica
#define TYPE_NAME(x) _Generic((x), \
    int: "int", \
    float: "float", \
    double: "double", \
    char*: "string", \
    default: "unknown")

#define PRINT_VALUE(x) _Generic((x), \
    int: printf("Integer: %d\n", x), \
    float: printf("Float: %.2f\n", x), \
    double: printf("Double: %.6f\n", x), \
    char*: printf("String: %s\n", x), \
    default: printf("Unknown type\n"))

// Função genérica usando _Generic
#define MAX(a, b) _Generic((a) + (b), \
    int: max_int, \
    float: max_float, \
    double: max_double \
    )(a, b)

static inline int max_int(int a, int b) {
    return (a > b) ? a : b;
}

static inline float max_float(float a, float b) {
    return (a > b) ? a : b;
}

static inline double max_double(double a, double b) {
    return (a > b) ? a : b;
}

// Função noreturn
_Noreturn void fatal_error(const char *message) {
    fprintf(stderr, "FATAL ERROR: %s\n", message);
    exit(EXIT_FAILURE);
}

// Variáveis atômicas
_Atomic int atomic_counter = 0;
atomic_int shared_data = ATOMIC_VAR_INIT(100);

// Estrutura com array flexível (C99, mas melhorado no C11)
struct flexible_array {
    size_t count;
    _Alignas(double) int data[];
};

// Função que demonstra Unicode básico
void test_unicode(void) {
    printf("=== Unicode Test ===\n");
    
    // Literais Unicode básicos (UTF-8)
    const char *utf8_string = "Hello, World! 🌍";
    printf("UTF-8 string: %s\n", utf8_string);
    printf("UTF-8 string length: %zu bytes\n", strlen(utf8_string));
    
    // Caracteres wide (wchar_t)
    const wchar_t *wide_string = L"Wide string: 世界";
    printf("Wide string created successfully\n");
}

// Teste de _Alignof
void test_alignment(void) {
    printf("=== Alignment Information ===\n");
    printf("Alignment of char: %zu\n", _Alignof(char));
    printf("Alignment of int: %zu\n", _Alignof(int));
    printf("Alignment of double: %zu\n", _Alignof(double));
    printf("Alignment of void*: %zu\n", _Alignof(void*));
    printf("Alignment of aligned_buffer: %zu\n", _Alignof(aligned_buffer));
    printf("Alignment of aligned_struct: %zu\n", _Alignof(aligned_struct));
    
    // Verificar se o alinhamento foi respeitado
    printf("Address of aligned_buffer: %p (mod 16 = %zu)\n", 
           (void*)aligned_buffer, (uintptr_t)aligned_buffer % 16);
}

// Teste de operações atômicas
void test_atomics(void) {
    printf("=== Atomic Operations Test ===\n");
    
    // Operações básicas
    atomic_store(&shared_data, 200);
    int old_value = atomic_load(&shared_data);
    printf("Atomic load: %d\n", old_value);
    
    // Compare and swap
    int expected = 200;
    bool success = atomic_compare_exchange_strong(&shared_data, &expected, 300);
    printf("Compare and swap success: %s\n", success ? "true" : "false");
    printf("New value: %d\n", atomic_load(&shared_data));
    
    // Fetch and add
    int prev = atomic_fetch_add(&shared_data, 50);
    printf("Previous value: %d, New value: %d\n", prev, atomic_load(&shared_data));
    
    // Teste do contador atômico
    for (int i = 0; i < 10; i++) {
        atomic_fetch_add(&atomic_counter, 1);
    }
    printf("Atomic counter after 10 increments: %d\n", atomic_load(&atomic_counter));
}

// Teste de _Generic
void test_generic(void) {
    printf("=== Generic Selection Test ===\n");
    
    int i = 42;
    float f = 3.14f;
    double d = 2.71828;
    char *s = "Hello";
    
    printf("Type of i: %s\n", TYPE_NAME(i));
    printf("Type of f: %s\n", TYPE_NAME(f));
    printf("Type of d: %s\n", TYPE_NAME(d));
    printf("Type of s: %s\n", TYPE_NAME(s));
    
    PRINT_VALUE(i);
    PRINT_VALUE(f);
    PRINT_VALUE(d);
    PRINT_VALUE(s);
    
    // Teste de MAX genérico
    printf("MAX(10, 20) = %d\n", MAX(10, 20));
    printf("MAX(3.14f, 2.71f) = %.2f\n", MAX(3.14f, 2.71f));
    printf("MAX(1.414, 1.732) = %.3f\n", MAX(1.414, 1.732));
}

// Teste de array flexível
void test_flexible_array(void) {
    printf("=== Flexible Array Test ===\n");
    
    size_t count = 5;
    struct flexible_array *arr = malloc(sizeof(struct flexible_array) + count * sizeof(int));
    
    if (arr) {
        arr->count = count;
        for (size_t i = 0; i < count; i++) {
            arr->data[i] = (int)(i * i);
        }
        
        printf("Flexible array contents: ");
        for (size_t i = 0; i < arr->count; i++) {
            printf("%d ", arr->data[i]);
        }
        printf("\n");
        
        printf("Array alignment: %zu\n", _Alignof(arr->data[0]));
        
        free(arr);
    }
}

// Teste de _Static_assert em função
void test_static_assert(void) {
    printf("=== Static Assert Test ===\n");
    
    // _Static_assert pode ser usado em qualquer lugar onde declarações são permitidas
    _Static_assert(sizeof(long) >= sizeof(int), "long deve ser pelo menos do tamanho de int");
    
    int array[10];
    _Static_assert(sizeof(array) == 10 * sizeof(int), "Array size mismatch");
    
    printf("All static assertions passed!\n");
}

// Teste de anonymous structs e unions (C11)
void test_anonymous_structs(void) {
    printf("=== Anonymous Structs/Unions Test ===\n");
    
    struct {
        int x, y;
        union {
            struct {
                int r, g, b;
            };
            int rgb[3];
        };
    } point_color;
    
    point_color.x = 10;
    point_color.y = 20;
    point_color.r = 255;
    point_color.g = 128;
    point_color.b = 64;
    
    printf("Point: (%d, %d)\n", point_color.x, point_color.y);
    printf("Color RGB: (%d, %d, %d)\n", point_color.r, point_color.g, point_color.b);
    printf("Color array: [%d, %d, %d]\n", 
           point_color.rgb[0], point_color.rgb[1], point_color.rgb[2]);
}

// Função principal
int main(void)
{
    printf("=== C11 Features Test ===\n");
    printf("Compiled with C11 support\n\n");
    
    // Verificar se estamos realmente usando C11
    #if __STDC_VERSION__ >= 201112L
        printf("C11 standard detected (__STDC_VERSION__ = %ld)\n\n", __STDC_VERSION__);
    #else
        printf("Warning: C11 not detected\n\n");
    #endif
    
    test_alignment();
    printf("\n");
    
    test_generic();
    printf("\n");
    
    test_unicode();
    printf("\n");
    
    test_atomics();
    printf("\n");
    
    test_flexible_array();
    printf("\n");
    
    test_static_assert();
    printf("\n");
    
    test_anonymous_structs();
    printf("\n");
    
    printf("Todos os testes C11 concluídos!\n");
    
    return 0;
}

// Demonstração adicional de _Generic com diferentes tipos
#define ABS(x) _Generic((x), \
    int: abs_int, \
    float: abs_float, \
    double: abs_double \
    )(x)

static inline int abs_int(int x) {
    return x < 0 ? -x : x;
}

static inline float abs_float(float x) {
    return x < 0.0f ? -x : x;
}

static inline double abs_double(double x) {
    return x < 0.0 ? -x : x;
}

// Função que demonstra o uso de ABS genérico
void demonstrate_generic_abs(void) {
    printf("=== Generic ABS Test ===\n");
    
    int i = -42;
    float f = -3.14f;
    double d = -2.71828;
    
    printf("ABS(%d) = %d\n", i, ABS(i));
    printf("ABS(%.2f) = %.2f\n", f, ABS(f));
    printf("ABS(%.5f) = %.5f\n", d, ABS(d));
}