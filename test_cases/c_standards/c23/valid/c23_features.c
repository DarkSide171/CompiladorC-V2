/*
 * Teste de funcionalidades específicas do C23 (C2x)
 * C23 introduz muitas novas funcionalidades importantes
 * Inclui typeof, _BitInt, _Decimal128, binary literals, etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>
#include <stdnoreturn.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>

// C23: typeof operator (similar ao GCC extension)
#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 202311L
    #define HAS_C23_TYPEOF 1
#else
    #define HAS_C23_TYPEOF 0
#endif
#else
    #define HAS_C23_TYPEOF 0
#endif

// C23: Binary literals (0b prefix)
#if HAS_C23_TYPEOF
const int binary_value = 0b11010110;  // 214 in decimal
const int binary_mask = 0b11110000;   // 240 in decimal
#else
const int binary_value = 214;  // Fallback for non-C23
const int binary_mask = 240;
#endif

// C23: Digit separators in numeric literals (fallback for non-C23)
const long large_number = 1000000000L;
const double pi_precise = 3.141592653589793;
const int hex_with_sep = 0xFFFFFFFF;

// C23: _BitInt types (arbitrary precision integers)
#if HAS_C23_TYPEOF
// _BitInt(N) where N is the number of bits
_BitInt(128) big_int = 0;
_BitInt(7) small_int = 100;  // 7-bit signed integer (-64 to 63)
unsigned _BitInt(256) huge_uint = 0;
#else
// Fallback para compiladores sem C23
long long big_int = 0;
int small_int = 100;
unsigned long long huge_uint = 0;
#endif

// C23: typeof operator examples
#if HAS_C23_TYPEOF
#define SWAP(a, b) do { \
    typeof(a) temp = (a); \
    (a) = (b); \
    (b) = temp; \
} while(0)

#define MAX_TYPEOF(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    (_a > _b) ? _a : _b; \
})
#else
// Fallback macros sem typeof
#define SWAP(a, b) do { \
    auto temp = (a); \
    (a) = (b); \
    (b) = temp; \
} while(0)

#define MAX_TYPEOF(a, b) ((a) > (b) ? (a) : (b))
#endif

// C23: _Decimal128, _Decimal64, _Decimal32 (decimal floating point)
#ifdef __STDC_IEC_60559_DFP__
_Decimal32 decimal32_val = 1.23DF;
_Decimal64 decimal64_val = 1.234567890123456DD;
_Decimal128 decimal128_val = 1.234567890123456789012345678901234DL;
#else
// Fallback para sistemas sem suporte decimal
float decimal32_val = 1.23f;
double decimal64_val = 1.234567890123456;
long double decimal128_val = 1.234567890123456789012345678901234L;
#endif

// C23: Enhanced enums with fixed underlying type (fallback syntax)
enum Status {
    STATUS_OK = 0,
    STATUS_ERROR = -1,
    STATUS_PENDING = 1,
    STATUS_TIMEOUT = 2
};

enum Flags {
    FLAG_NONE = 0,
    FLAG_READ = 0x01,
    FLAG_WRITE = 0x02,
    FLAG_EXECUTE = 0x04,
    FLAG_ALL = 0x07  // FLAG_READ | FLAG_WRITE | FLAG_EXECUTE
};

// C23: Attributes (standardized)
[[deprecated("Use new_function instead")]] 
void old_function(void) {
    printf("This function is deprecated\n");
}

[[nodiscard("Return value should be checked")]]
int important_function(void) {
    return 42;
}

[[maybe_unused]] static int unused_variable = 100;

[[noreturn]] void terminate_program(void) {
    exit(EXIT_FAILURE);
}

// C23: auto type deduction (limited)
void test_auto_c23(void) {
    printf("=== C23 Auto Type Test ===\n");
    
    // auto pode ser usado em algumas situações (fallback para não-C23)
    int x = 42;        // int
    double y = 3.14;      // double
    int z = 'A';       // int (char promoted)
    int w = true;      // int (_Bool promoted)
    
    printf("auto x = %d (should be int)\n", x);
    printf("auto y = %.2f (should be double)\n", y);
    printf("auto z = %d (should be int from char)\n", z);
    printf("auto w = %d (should be int from bool)\n", w);
}

// C23: typeof operator test
void test_typeof_c23(void) {
    printf("=== C23 typeof Test ===\n");
    
#if HAS_C23_TYPEOF
    int a = 10, b = 20;
    printf("Before swap: a=%d, b=%d\n", a, b);
    SWAP(a, b);
    printf("After swap: a=%d, b=%d\n", a, b);
    
    float f1 = 3.14f, f2 = 2.71f;
    printf("MAX_TYPEOF(%.2f, %.2f) = %.2f\n", f1, f2, MAX_TYPEOF(f1, f2));
    
    // typeof com expressões complexas
    typeof(a + b) sum = a + b;
    typeof(&a) ptr = &a;
    typeof(a > b ? a : b) max_val = a > b ? a : b;
    
    printf("sum (typeof(a + b)): %d\n", sum);
    printf("ptr (typeof(&a)): %p\n", (void*)ptr);
    printf("max_val (typeof(a > b ? a : b)): %d\n", max_val);
#else
    printf("typeof not available in this compiler\n");
#endif
}

// C23: Binary literals test
void test_binary_literals_c23(void) {
    printf("=== C23 Binary Literals Test ===\n");
    
    printf("Binary 0b11010110 = %d (decimal)\n", binary_value);
    printf("Binary 0b11110000 = %d (decimal)\n", binary_mask);
    
    // Operações com binários
    int result = binary_value & binary_mask;
    printf("0b11010110 & 0b11110000 = 0b%s (%d)\n", 
           "11010000", result);  // Manual binary representation
    
    // Teste com números grandes (sem separators para compatibilidade)
    printf("Large number: %ld\n", large_number);
    printf("Pi precise: %.15f\n", pi_precise);
    printf("Hex value: 0x%X\n", hex_with_sep);
}

// C23: _BitInt test
void test_bitint_c23(void) {
    printf("=== C23 _BitInt Test ===\n");
    
#if HAS_C23_TYPEOF
    printf("_BitInt(128) size: %zu bytes\n", sizeof(big_int));
    printf("_BitInt(7) size: %zu bytes\n", sizeof(small_int));
    printf("unsigned _BitInt(256) size: %zu bytes\n", sizeof(huge_uint));
    
    // Operações com _BitInt (usando valores compatíveis)
    big_int = 123456789012345LL;
    small_int = 63;  // Max value for 7-bit signed
    huge_uint = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF_BitInt(256);
    
    printf("big_int assigned successfully\n");
    printf("small_int = %d\n", (int)small_int);
    printf("huge_uint assigned successfully\n");
    
    // Teste de overflow em _BitInt pequeno
    small_int = 64;  // Isso deve causar overflow em 7-bit signed
    printf("small_int after overflow: %d\n", (int)small_int);
#else
    printf("_BitInt not available in this compiler\n");
    printf("Using fallback types:\n");
    printf("big_int (long long) size: %zu bytes\n", sizeof(big_int));
    printf("small_int (int) size: %zu bytes\n", sizeof(small_int));
    printf("huge_uint (unsigned long long) size: %zu bytes\n", sizeof(huge_uint));
#endif
}

// C23: Decimal floating point test
void test_decimal_c23(void) {
    printf("=== C23 Decimal Floating Point Test ===\n");
    
#ifdef __STDC_IEC_60559_DFP__
    printf("_Decimal32 size: %zu bytes\n", sizeof(decimal32_val));
    printf("_Decimal64 size: %zu bytes\n", sizeof(decimal64_val));
    printf("_Decimal128 size: %zu bytes\n", sizeof(decimal128_val));
    
    printf("_Decimal32 value: %.6DF\n", decimal32_val);
    printf("_Decimal64 value: %.15DD\n", decimal64_val);
    printf("_Decimal128 value: %.30DL\n", decimal128_val);
    
    // Operações decimais
    _Decimal64 result = decimal64_val * 2.0DD;
    printf("decimal64_val * 2 = %.15DD\n", result);
#else
    printf("Decimal floating point not available\n");
    printf("Using binary floating point fallback:\n");
    printf("decimal32_val: %.6f\n", decimal32_val);
    printf("decimal64_val: %.15f\n", decimal64_val);
    printf("decimal128_val: %.30Lf\n", decimal128_val);
#endif
}

// C23: Enhanced enums test
void test_enhanced_enums_c23(void) {
    printf("=== C23 Enhanced Enums Test ===\n");
    
    enum Status status = STATUS_OK;
    enum Flags flags = FLAG_READ | FLAG_WRITE;
    
    printf("Status enum size: %zu bytes\n", sizeof(status));
    printf("Flags enum size: %zu bytes\n", sizeof(flags));
    
    printf("STATUS_OK = %d\n", STATUS_OK);
    printf("STATUS_ERROR = %d\n", STATUS_ERROR);
    printf("STATUS_PENDING = %d\n", STATUS_PENDING);
    
    printf("FLAG_READ = 0x%02X\n", FLAG_READ);
    printf("FLAG_WRITE = 0x%02X\n", FLAG_WRITE);
    printf("FLAG_EXECUTE = 0x%02X\n", FLAG_EXECUTE);
    printf("FLAG_ALL = 0x%02X\n", FLAG_ALL);
    
    // Teste de operações com flags
    if (flags & FLAG_READ) {
        printf("Read permission granted\n");
    }
    if (flags & FLAG_WRITE) {
        printf("Write permission granted\n");
    }
    if (!(flags & FLAG_EXECUTE)) {
        printf("Execute permission denied\n");
    }
}

// C23: Attributes test
void test_attributes_c23(void) {
    printf("=== C23 Attributes Test ===\n");
    
    // Teste de função deprecated
    old_function();  // Deve gerar warning
    
    // Teste de função nodiscard
    int result = important_function();
    printf("Important function returned: %d\n", result);
    
    // important_function();  // Deve gerar warning se descomentado
    
    // Variável maybe_unused não deve gerar warning
    printf("Maybe unused variable: %d\n", unused_variable);
    
    printf("Attributes test completed\n");
}

// C23: Improved _Generic
void test_improved_generic_c23(void) {
    printf("=== C23 Improved _Generic Test ===\n");
    
    // _Generic com typeof (se disponível)
#if HAS_C23_TYPEOF
    #define PRINT_TYPE_AND_VALUE(x) do { \
        printf("Value: "); \
        _Generic((x), \
            int: printf("%d", x), \
            float: printf("%.2f", x), \
            double: printf("%.6f", x), \
            char: printf("'%c'", x), \
            char*: printf("\"%s\"", x), \
            default: printf("unknown") \
        ); \
        printf(" (type: %s)\n", #x); \
    } while(0)
#else
    #define PRINT_TYPE_AND_VALUE(x) do { \
        printf("Value: "); \
        _Generic((x), \
            int: printf("%d", x), \
            float: printf("%.2f", x), \
            double: printf("%.6f", x), \
            char: printf("'%c'", x), \
            char*: printf("\"%s\"", x), \
            default: printf("unknown") \
        ); \
        printf(" (generic)\n"); \
    } while(0)
#endif
    
    int i = 42;
    float f = 3.14f;
    double d = 2.71828;
    char c = 'X';
    char *s = "Hello C23";
    
    PRINT_TYPE_AND_VALUE(i);
    PRINT_TYPE_AND_VALUE(f);
    PRINT_TYPE_AND_VALUE(d);
    PRINT_TYPE_AND_VALUE(c);
    PRINT_TYPE_AND_VALUE(s);
}

// C23: Improved compound literals
void test_compound_literals_c23(void) {
    printf("=== C23 Compound Literals Test ===\n");
    
    // Compound literals com storage duration
    int *arr = (int[]){1, 2, 3, 4, 5};
    printf("Compound literal array: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    // Compound literal para struct
    struct Point {
        int x, y;
        char label[10];
    };
    
    struct Point *p = &(struct Point){.x = 10, .y = 20, .label = "Origin"};
    printf("Point: (%d, %d) label: %s\n", p->x, p->y, p->label);
    
    // Compound literal com designated initializers
    int *matrix = (int[3][3]){
        [0] = {1, 2, 3},
        [1] = {4, 5, 6},
        [2] = {7, 8, 9}
    };
    
    printf("Matrix:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%d ", matrix[i * 3 + j]);
        }
        printf("\n");
    }
}

// Função principal
int main(void)
{
    printf("=== C23 (C2x) Features Test ===\n");
    printf("C23 introduz muitas funcionalidades novas e melhorias\n\n");
    
    // Verificar versão do C
    #ifdef __STDC_VERSION__
        printf("C Standard Version: %ld\n", __STDC_VERSION__);
        #if __STDC_VERSION__ >= 202311L
            printf("C23 standard detected!\n\n");
        #elif __STDC_VERSION__ >= 201710L
            printf("C17 standard detected (C23 features may not work)\n\n");
        #elif __STDC_VERSION__ >= 201112L
            printf("C11 standard detected (C23 features may not work)\n\n");
        #else
            printf("Older C standard detected\n\n");
        #endif
    #else
        printf("C standard version unknown\n\n");
    #endif
    
    test_auto_c23();
    printf("\n");
    
    test_typeof_c23();
    printf("\n");
    
    test_binary_literals_c23();
    printf("\n");
    
    test_bitint_c23();
    printf("\n");
    
    test_decimal_c23();
    printf("\n");
    
    test_enhanced_enums_c23();
    printf("\n");
    
    test_attributes_c23();
    printf("\n");
    
    test_improved_generic_c23();
    printf("\n");
    
    test_compound_literals_c23();
    printf("\n");
    
    printf("Todos os testes C23 concluídos!\n");
    printf("Nota: Algumas funcionalidades podem não estar disponíveis\n");
    printf("dependendo do suporte do compilador.\n");
    
    return 0;
}

// C23: Demonstração de funcionalidades avançadas
void demonstrate_advanced_c23(void) {
    printf("=== C23 Advanced Features Demo ===\n");
    
    // Combinação de várias funcionalidades C23
#if HAS_C23_TYPEOF
    typeof(1'000'000) big_num = 1'000'000;
    typeof(0b1111'0000) binary_num = 0b1111'0000;
    
    printf("big_num: %d\n", big_num);
    printf("binary_num: %d\n", binary_num);
    
    // Macro complexa usando typeof e _Generic
    #define COMPLEX_OPERATION(x, y) _Generic((x) + (y), \
        int: ({ \
            typeof(x) _x = (x); \
            typeof(y) _y = (y); \
            printf("Integer operation: %d + %d = %d\n", _x, _y, _x + _y); \
            _x + _y; \
        }), \
        float: ({ \
            typeof(x) _x = (x); \
            typeof(y) _y = (y); \
            printf("Float operation: %.2f + %.2f = %.2f\n", _x, _y, _x + _y); \
            _x + _y; \
        }), \
        default: (x) + (y) \
    )
    
    int result1 = COMPLEX_OPERATION(10, 20);
    float result2 = COMPLEX_OPERATION(3.14f, 2.86f);
    
    printf("Results: %d, %.2f\n", result1, result2);
#else
    printf("Advanced C23 features not available\n");
#endif
}