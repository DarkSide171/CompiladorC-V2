/*
 * Teste de funcionalidades específicas do C17 (C18)
 * C17 é principalmente uma correção técnica do C11, com poucas novas features
 * Foca em melhorias de compatibilidade e correções de bugs
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>
#include <stdnoreturn.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>

// C17 mantém todas as funcionalidades do C11
// _Static_assert para verificações em tempo de compilação
_Static_assert(sizeof(int) >= 4, "int deve ter pelo menos 4 bytes");
_Static_assert(sizeof(void*) == sizeof(size_t), "Ponteiro e size_t devem ter o mesmo tamanho");

// Teste de alinhamento com _Alignas (melhorado no C17)
_Alignas(16) char aligned_buffer[64];
_Alignas(double) int aligned_int;
_Alignas(32) struct {
    int x, y, z;
} aligned_struct;

// _Generic para seleção de tipo genérica (estável no C17)
#define TYPE_NAME(x) _Generic((x), \
    int: "int", \
    float: "float", \
    double: "double", \
    long: "long", \
    unsigned: "unsigned", \
    char*: "string", \
    const char*: "const string", \
    default: "unknown")

#define PRINT_VALUE(x) _Generic((x), \
    int: printf("Integer: %d\n", x), \
    float: printf("Float: %.2f\n", x), \
    double: printf("Double: %.6f\n", x), \
    long: printf("Long: %ld\n", x), \
    unsigned: printf("Unsigned: %u\n", x), \
    char*: printf("String: %s\n", x), \
    const char*: printf("Const String: %s\n", x), \
    default: printf("Unknown type\n"))

// Função genérica melhorada
#define MAX(a, b) _Generic((a) + (b), \
    int: max_int, \
    float: max_float, \
    double: max_double, \
    long: max_long \
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

static inline long max_long(long a, long b) {
    return (a > b) ? a : b;
}

// Função noreturn (comportamento melhorado no C17)
_Noreturn void fatal_error(const char *message) {
    fprintf(stderr, "FATAL ERROR: %s\n", message);
    exit(EXIT_FAILURE);
}

// Variáveis atômicas (comportamento estabilizado no C17)
_Atomic int atomic_counter = 0;
atomic_int shared_data = ATOMIC_VAR_INIT(100);

// Estrutura com array flexível (comportamento clarificado no C17)
struct flexible_array {
    size_t count;
    _Alignas(double) int data[];
};

// Teste de Unicode melhorado (C17 clarifica comportamento)
void test_unicode_c17(void) {
    printf("=== C17 Unicode Test ===\n");
    
    // Literais Unicode básicos (UTF-8) - comportamento clarificado
    const char *utf8_string = "Hello, World! 🌍 C17";
    printf("UTF-8 string: %s\n", utf8_string);
    printf("UTF-8 string length: %zu bytes\n", strlen(utf8_string));
    
    // Caracteres wide (wchar_t) - comportamento melhorado
    const wchar_t *wide_string = L"Wide string C17: 世界";
    printf("Wide string created successfully\n");
    
    // Teste de caracteres especiais
    const char *special_chars = "Tabs:\t Newlines:\n Quotes:\" Backslash:\\";
    printf("Special characters: %s\n", special_chars);
}

// Teste de _Alignof (comportamento estabilizado)
void test_alignment_c17(void) {
    printf("=== C17 Alignment Information ===\n");
    printf("Alignment of char: %zu\n", _Alignof(char));
    printf("Alignment of int: %zu\n", _Alignof(int));
    printf("Alignment of long: %zu\n", _Alignof(long));
    printf("Alignment of double: %zu\n", _Alignof(double));
    printf("Alignment of void*: %zu\n", _Alignof(void*));
    printf("Alignment of aligned_buffer: %zu\n", _Alignof(aligned_buffer));
    printf("Alignment of aligned_struct: %zu\n", _Alignof(aligned_struct));
    
    // Verificar se o alinhamento foi respeitado
    printf("Address of aligned_buffer: %p (mod 16 = %zu)\n", 
           (void*)aligned_buffer, (uintptr_t)aligned_buffer % 16);
    
    // Teste de alinhamento máximo
    printf("Max align: %zu\n", _Alignof(max_align_t));
}

// Teste de operações atômicas (comportamento refinado no C17)
void test_atomics_c17(void) {
    printf("=== C17 Atomic Operations Test ===\n");
    
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
    
    // Teste de diferentes ordens de memória
    atomic_store_explicit(&shared_data, 500, memory_order_release);
    int loaded = atomic_load_explicit(&shared_data, memory_order_acquire);
    printf("Explicit memory order load: %d\n", loaded);
    
    // Teste do contador atômico
    for (int i = 0; i < 10; i++) {
        atomic_fetch_add(&atomic_counter, 1);
    }
    printf("Atomic counter after 10 increments: %d\n", atomic_load(&atomic_counter));
}

// Teste de _Generic melhorado (C17)
void test_generic_c17(void) {
    printf("=== C17 Generic Selection Test ===\n");
    
    int i = 42;
    float f = 3.14f;
    double d = 2.71828;
    long l = 1000000L;
    unsigned u = 4294967295U;
    char *s = "Hello";
    const char *cs = "Const Hello";
    
    printf("Type of i: %s\n", TYPE_NAME(i));
    printf("Type of f: %s\n", TYPE_NAME(f));
    printf("Type of d: %s\n", TYPE_NAME(d));
    printf("Type of l: %s\n", TYPE_NAME(l));
    printf("Type of u: %s\n", TYPE_NAME(u));
    printf("Type of s: %s\n", TYPE_NAME(s));
    printf("Type of cs: %s\n", TYPE_NAME(cs));
    
    PRINT_VALUE(i);
    PRINT_VALUE(f);
    PRINT_VALUE(d);
    PRINT_VALUE(l);
    PRINT_VALUE(u);
    PRINT_VALUE(s);
    PRINT_VALUE(cs);
    
    // Teste de MAX genérico
    printf("MAX(10, 20) = %d\n", MAX(10, 20));
    printf("MAX(3.14f, 2.71f) = %.2f\n", MAX(3.14f, 2.71f));
    printf("MAX(1.414, 1.732) = %.3f\n", MAX(1.414, 1.732));
    printf("MAX(1000L, 2000L) = %ld\n", MAX(1000L, 2000L));
}

// Teste de array flexível (comportamento clarificado no C17)
void test_flexible_array_c17(void) {
    printf("=== C17 Flexible Array Test ===\n");
    
    size_t count = 7;
    struct flexible_array *arr = malloc(sizeof(struct flexible_array) + count * sizeof(int));
    
    if (arr) {
        arr->count = count;
        for (size_t i = 0; i < count; i++) {
            arr->data[i] = (int)(i * i + i);
        }
        
        printf("Flexible array contents: ");
        for (size_t i = 0; i < arr->count; i++) {
            printf("%d ", arr->data[i]);
        }
        printf("\n");
        
        printf("Array alignment: %zu\n", _Alignof(arr->data[0]));
        printf("Struct size without array: %zu\n", sizeof(struct flexible_array));
        printf("Total allocated size: %zu\n", sizeof(struct flexible_array) + count * sizeof(int));
        
        free(arr);
    }
}

// Teste de _Static_assert em diferentes contextos (C17)
void test_static_assert_c17(void) {
    printf("=== C17 Static Assert Test ===\n");
    
    // _Static_assert pode ser usado em qualquer lugar onde declarações são permitidas
    _Static_assert(sizeof(long) >= sizeof(int), "long deve ser pelo menos do tamanho de int");
    _Static_assert(sizeof(double) >= sizeof(float), "double deve ser maior que float");
    _Static_assert(_Alignof(double) >= _Alignof(float), "double deve ter alinhamento >= float");
    
    int array[10];
    _Static_assert(sizeof(array) == 10 * sizeof(int), "Array size mismatch");
    
    printf("All C17 static assertions passed!\n");
}

// Teste de anonymous structs e unions (comportamento estabilizado no C17)
void test_anonymous_structs_c17(void) {
    printf("=== C17 Anonymous Structs/Unions Test ===\n");
    
    struct {
        int x, y;
        union {
            struct {
                int r, g, b, a;  // RGBA
            };
            int rgba[4];
            uint32_t color_value;
        };
        _Alignas(16) char padding[16];  // Padding alinhado
    } point_color;
    
    point_color.x = 100;
    point_color.y = 200;
    point_color.r = 255;
    point_color.g = 128;
    point_color.b = 64;
    point_color.a = 255;
    
    printf("Point: (%d, %d)\n", point_color.x, point_color.y);
    printf("Color RGBA: (%d, %d, %d, %d)\n", 
           point_color.r, point_color.g, point_color.b, point_color.a);
    printf("Color array: [%d, %d, %d, %d]\n", 
           point_color.rgba[0], point_color.rgba[1], 
           point_color.rgba[2], point_color.rgba[3]);
    printf("Color as uint32: 0x%08X\n", point_color.color_value);
    printf("Struct alignment: %zu\n", _Alignof(point_color));
}

// Função principal
int main(void)
{
    printf("=== C17 (C18) Features Test ===\n");
    printf("C17 é uma correção técnica do C11 com melhorias de estabilidade\n\n");
    
    // Verificar se estamos usando C17
    #if __STDC_VERSION__ >= 201710L
        printf("C17 standard detected (__STDC_VERSION__ = %ld)\n\n", __STDC_VERSION__);
    #elif __STDC_VERSION__ >= 201112L
        printf("C11 standard detected (__STDC_VERSION__ = %ld)\n");
        printf("C17 features should work with C11 compiler\n\n");
    #else
        printf("Warning: C17/C11 not detected\n\n");
    #endif
    
    test_alignment_c17();
    printf("\n");
    
    test_generic_c17();
    printf("\n");
    
    test_unicode_c17();
    printf("\n");
    
    test_atomics_c17();
    printf("\n");
    
    test_flexible_array_c17();
    printf("\n");
    
    test_static_assert_c17();
    printf("\n");
    
    test_anonymous_structs_c17();
    printf("\n");
    
    printf("Todos os testes C17 concluídos com sucesso!\n");
    
    return 0;
}

// Demonstração de melhorias de compatibilidade do C17
#define COMPATIBILITY_TEST(x) _Generic((x), \
    _Bool: "bool", \
    char: "char", \
    signed char: "signed char", \
    unsigned char: "unsigned char", \
    short: "short", \
    unsigned short: "unsigned short", \
    int: "int", \
    unsigned int: "unsigned int", \
    long: "long", \
    unsigned long: "unsigned long", \
    long long: "long long", \
    unsigned long long: "unsigned long long", \
    float: "float", \
    double: "double", \
    long double: "long double", \
    default: "other type")

// Função que demonstra a compatibilidade melhorada
void demonstrate_c17_compatibility(void) {
    printf("=== C17 Compatibility Test ===\n");
    
    // Teste com todos os tipos básicos
    _Bool b = true;
    char c = 'A';
    signed char sc = -1;
    unsigned char uc = 255;
    short s = -1000;
    unsigned short us = 65535;
    int i = -100000;
    unsigned int ui = 4000000000U;
    long l = -1000000L;
    unsigned long ul = 4000000000UL;
    long long ll = -1000000000000LL;
    unsigned long long ull = 18000000000000000000ULL;
    float f = 3.14f;
    double d = 2.71828;
    long double ld = 1.41421356L;
    
    printf("Type of b: %s\n", COMPATIBILITY_TEST(b));
    printf("Type of c: %s\n", COMPATIBILITY_TEST(c));
    printf("Type of sc: %s\n", COMPATIBILITY_TEST(sc));
    printf("Type of uc: %s\n", COMPATIBILITY_TEST(uc));
    printf("Type of s: %s\n", COMPATIBILITY_TEST(s));
    printf("Type of us: %s\n", COMPATIBILITY_TEST(us));
    printf("Type of i: %s\n", COMPATIBILITY_TEST(i));
    printf("Type of ui: %s\n", COMPATIBILITY_TEST(ui));
    printf("Type of l: %s\n", COMPATIBILITY_TEST(l));
    printf("Type of ul: %s\n", COMPATIBILITY_TEST(ul));
    printf("Type of ll: %s\n", COMPATIBILITY_TEST(ll));
    printf("Type of ull: %s\n", COMPATIBILITY_TEST(ull));
    printf("Type of f: %s\n", COMPATIBILITY_TEST(f));
    printf("Type of d: %s\n", COMPATIBILITY_TEST(d));
    printf("Type of ld: %s\n", COMPATIBILITY_TEST(ld));
}