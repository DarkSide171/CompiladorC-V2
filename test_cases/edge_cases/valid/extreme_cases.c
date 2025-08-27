/*
 * Casos extremos válidos para testar os limites do lexer e preprocessor
 * Testa situações limítrofes que devem ser aceitas pelo compilador
 */

#include <stdio.h>
#include <limits.h>
#include <float.h>

// Teste de identificadores muito longos (mas dentro do limite)
#define VERY_LONG_IDENTIFIER_NAME_THAT_TESTS_THE_MAXIMUM_LENGTH_SUPPORTED_BY_THE_LEXER_ABCDEFGHIJKLMNOPQRSTUVWXYZ 42

// Teste de macros com muitos parâmetros
#define MACRO_WITH_MANY_PARAMS(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) \
    ((a)+(b)+(c)+(d)+(e)+(f)+(g)+(h)+(i)+(j)+(k)+(l)+(m)+(n)+(o)+(p))

// Teste de aninhamento profundo de macros
#define LEVEL1(x) LEVEL2(x)
#define LEVEL2(x) LEVEL3(x)
#define LEVEL3(x) LEVEL4(x)
#define LEVEL4(x) LEVEL5(x)
#define LEVEL5(x) LEVEL6(x)
#define LEVEL6(x) LEVEL7(x)
#define LEVEL7(x) LEVEL8(x)
#define LEVEL8(x) (x * 2)

// Teste de concatenação complexa
#define CONCAT(a, b) a##b
#define MAKE_VAR(prefix, num) CONCAT(prefix, num)
#define DECLARE_VARS(prefix) \
    int MAKE_VAR(prefix, 1) = 1; \
    int MAKE_VAR(prefix, 2) = 2; \
    int MAKE_VAR(prefix, 3) = 3;

// Teste de stringify com casos especiais
#define STRINGIFY(x) #x
#define STRINGIFY_EXPANDED(x) STRINGIFY(x)

// Macro que gera código com muitas linhas
#define GENERATE_SWITCH(var) \
    switch(var) { \
        case 0: printf("Zero\n"); break; \
        case 1: printf("One\n"); break; \
        case 2: printf("Two\n"); break; \
        case 3: printf("Three\n"); break; \
        case 4: printf("Four\n"); break; \
        case 5: printf("Five\n"); break; \
        case 6: printf("Six\n"); break; \
        case 7: printf("Seven\n"); break; \
        case 8: printf("Eight\n"); break; \
        case 9: printf("Nine\n"); break; \
        default: printf("Other\n"); break; \
    }

// Teste de condicionais aninhadas profundamente
#ifdef DEBUG
    #ifdef VERBOSE
        #ifdef EXTRA_VERBOSE
            #ifdef SUPER_VERBOSE
                #ifdef ULTRA_VERBOSE
                    #define LOG_LEVEL 5
                #else
                    #define LOG_LEVEL 4
                #endif
            #else
                #define LOG_LEVEL 3
            #endif
        #else
            #define LOG_LEVEL 2
        #endif
    #else
        #define LOG_LEVEL 1
    #endif
#else
    #define LOG_LEVEL 0
#endif

// Teste de números nos limites
#define MAX_INT_LITERAL 2147483647
#define MIN_INT_LITERAL (-2147483648)
#define LARGE_LONG_LITERAL 9223372036854775807L
#define LARGE_UNSIGNED_LITERAL 4294967295U

// Teste de números de ponto flutuante extremos
#define VERY_SMALL_FLOAT 1.175494351e-38F
#define VERY_LARGE_FLOAT 3.402823466e+38F
#define VERY_SMALL_DOUBLE 2.2250738585072014e-308
#define VERY_LARGE_DOUBLE 1.7976931348623157e+308

// Teste de strings muito longas (mas válidas)
#define LONG_STRING "This is a very long string that tests the lexer's ability to handle " \
                    "strings that span multiple lines using the backslash continuation " \
                    "mechanism. This should be properly concatenated into a single " \
                    "string literal by the preprocessor and then processed by the lexer " \
                    "as one continuous string without any issues or buffer overflows."

// Teste de caracteres especiais em strings
#define SPECIAL_CHARS "\a\b\f\n\r\t\v\\'\"\\\0"
#define OCTAL_CHARS "\001\002\003\177\377"
#define HEX_CHARS "\x01\x02\x03\x7F\xFF"

// Teste de comentários em situações extremas
/* Este é um comentário que deve ser tratado corretamente */

// Comentário de linha única no final do arquivo sem quebra de linha

// Teste de operadores em sequência
#define COMPLEX_EXPRESSION ((((1 + 2) * 3) - 4) / 5) % 6
#define BITWISE_OPERATIONS (~(0xFF & 0xAA) | (0x55 ^ 0x33)) << 2 >> 1

// Teste de declarações complexas
typedef struct {
    int (*function_pointer)(int, float, double, char*, void*);
    union {
        struct {
            unsigned int bit0 : 1;
            unsigned int bit1 : 1;
            unsigned int bit2 : 1;
            unsigned int bit3 : 1;
            unsigned int bit4 : 1;
            unsigned int bit5 : 1;
            unsigned int bit6 : 1;
            unsigned int bit7 : 1;
        } bits;
        unsigned char byte;
    } flags;
    volatile const int * const * restrict array_of_pointers[10][20];
} ComplexStruct;

// Função com muitos parâmetros
int function_with_many_parameters(
    int param1, float param2, double param3, char param4,
    short param5, long param6, unsigned param7, signed param8,
    const int param9, volatile float param10, double* restrict param11,
    const volatile unsigned long long param12,
    struct { int x, y; } param13,
    union { int i; float f; } param14,
    int param15,
    int (*param16)(int, int),
    void (*param17)(void),
    int param18[],
    char param19[100],
    const char* restrict param20
) {
    return param1 + (int)param2 + (int)param3 + param4 + param5 + 
           (int)param6 + (int)param7 + param8 + param9 + (int)param10 + 
           (param11 ? (int)*param11 : 0) + (int)param12 + param13.x + 
           param14.i + param15 + (param16 ? param16(1, 2) : 0) + 
           (param18 ? param18[0] : 0) + param19[0] + 
           (param20 ? (int)param20[0] : 0);
}

// Teste de arrays multidimensionais
int multidimensional_array[5][10][15][20];
char string_array[100][200];
float matrix[3][3] = {
    {1.0f, 2.0f, 3.0f},
    {4.0f, 5.0f, 6.0f},
    {7.0f, 8.0f, 9.0f}
};

// Teste de inicializadores complexos
struct {
    int a;
    struct {
        float b;
        union {
            int c;
            char d[4];
        } inner_union;
    } inner_struct;
    int array[5];
} complex_initializer = {
    .a = 42,
    .inner_struct = {
        .b = 3.14f,
        .inner_union.c = 100
    },
    .array = {1, 2, 3, 4, 5}
};

// Função principal para testar casos extremos
int main(void)
{
    printf("=== Extreme Cases Test ===\n");
    
    // Teste de identificador longo
    int result = VERY_LONG_IDENTIFIER_NAME_THAT_TESTS_THE_MAXIMUM_LENGTH_SUPPORTED_BY_THE_LEXER_ABCDEFGHIJKLMNOPQRSTUVWXYZ;
    printf("Long identifier value: %d\n", result);
    
    // Teste de macro com muitos parâmetros
    int sum = MACRO_WITH_MANY_PARAMS(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
    printf("Sum of 16 parameters: %d\n", sum);
    
    // Teste de aninhamento de macros
    int nested_result = LEVEL1(5);
    printf("Nested macro result: %d\n", nested_result);
    
    // Teste de variáveis geradas por macro
    DECLARE_VARS(test)
    printf("Generated variables: %d, %d, %d\n", test1, test2, test3);
    
    // Teste de stringify
    printf("Stringified: %s\n", STRINGIFY(Hello World));
    printf("Expanded stringify: %s\n", STRINGIFY_EXPANDED(MAX_INT_LITERAL));
    
    // Teste de switch gerado
    int test_value = 5;
    printf("Generated switch for value %d:\n", test_value);
    GENERATE_SWITCH(test_value)
    
    // Teste de números extremos
    printf("Max int: %d\n", MAX_INT_LITERAL);
    printf("Min int: %d\n", MIN_INT_LITERAL);
    printf("Large long: %ld\n", LARGE_LONG_LITERAL);
    printf("Large unsigned: %u\n", LARGE_UNSIGNED_LITERAL);
    
    // Teste de floats extremos
    printf("Very small float: %e\n", VERY_SMALL_FLOAT);
    printf("Very large float: %e\n", VERY_LARGE_FLOAT);
    printf("Very small double: %e\n", VERY_SMALL_DOUBLE);
    printf("Very large double: %e\n", VERY_LARGE_DOUBLE);
    
    // Teste de string longa
    printf("Long string length: %zu\n", sizeof(LONG_STRING) - 1);
    
    // Teste de caracteres especiais
    printf("Special chars: %s\n", SPECIAL_CHARS);
    
    // Teste de expressões complexas
    int complex_expr = COMPLEX_EXPRESSION;
    int bitwise_expr = BITWISE_OPERATIONS;
    printf("Complex expression: %d\n", complex_expr);
    printf("Bitwise operations: %d\n", bitwise_expr);
    
    // Teste de função com muitos parâmetros
    struct { int x, y; } temp_struct = {12, 13};
    union { int i; float f; } temp_union = {.i = 14};
    int temp_array[] = {1, 2, 3};
    char temp_char_array[100] = "test";
    
    int func_result = function_with_many_parameters(
        1, 2.0f, 3.0, 'A', 5, 6L, 7U, 8,
        9, 10.0f, NULL, 11ULL,
        temp_struct, temp_union,
        15, NULL, NULL, temp_array, temp_char_array, "param20"
    );
    printf("Function with many parameters result: %d\n", func_result);
    
    // Teste de array multidimensional
    multidimensional_array[0][0][0][0] = 42;
    printf("Multidimensional array element: %d\n", multidimensional_array[0][0][0][0]);
    
    // Teste de inicializador complexo
    printf("Complex initializer: a=%d, b=%.2f, c=%d\n", 
           complex_initializer.a, 
           complex_initializer.inner_struct.b,
           complex_initializer.inner_struct.inner_union.c);
    
    printf("Log level: %d\n", LOG_LEVEL);
    
    printf("All extreme cases tested successfully!\n");
    
    return 0;
}

// Teste de função com declaração muito longa
static inline const volatile unsigned long long int * restrict const * const restrict 
function_with_very_long_declaration_that_tests_parser_limits(
    const volatile unsigned long long int * restrict const * const restrict param
) {
    return param;
}

// Teste de macro que se expande para código muito longo
#define VERY_LONG_MACRO_EXPANSION \
    do { \
        printf("This is a very long macro that expands to multiple statements "); \
        printf("and tests the preprocessor's ability to handle long expansions "); \
        printf("without running into buffer overflow issues or other problems. "); \
        printf("The macro should expand correctly and the resulting code should "); \
        printf("compile and run without any issues whatsoever in the system.\n"); \
    } while(0)

// Função que usa a macro longa
void test_long_macro(void) {
    VERY_LONG_MACRO_EXPANSION;
}