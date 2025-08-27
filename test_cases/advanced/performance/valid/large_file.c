/*
 * Arquivo de teste de performance para lexer e preprocessor
 * Contém uma grande quantidade de código C válido para testar desempenho
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>

// Macros complexas para teste de preprocessor
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, min, max) (MIN(MAX(x, min), max))
#define STRINGIFY(x) #x
#define CONCAT(a, b) a##b
#define REPEAT_10(x) x x x x x x x x x x
#define REPEAT_100(x) REPEAT_10(REPEAT_10(x))
#define REPEAT_1000(x) REPEAT_100(REPEAT_10(x))

// Estruturas complexas
struct Point2D {
    double x, y;
};

struct Point3D {
    double x, y, z;
};

struct Matrix4x4 {
    double m[4][4];
};

struct ComplexStruct {
    int id;
    char name[256];
    struct Point3D position;
    struct Point3D velocity;
    struct Matrix4x4 transform;
    bool active;
    uint64_t timestamp;
    float data[100];
};

// Enums extensos
enum ErrorCodes {
    ERROR_NONE = 0,
    ERROR_INVALID_INPUT = 1,
    ERROR_OUT_OF_MEMORY = 2,
    ERROR_FILE_NOT_FOUND = 3,
    ERROR_PERMISSION_DENIED = 4,
    ERROR_NETWORK_TIMEOUT = 5,
    ERROR_INVALID_FORMAT = 6,
    ERROR_BUFFER_OVERFLOW = 7,
    ERROR_DIVISION_BY_ZERO = 8,
    ERROR_NULL_POINTER = 9,
    ERROR_INDEX_OUT_OF_BOUNDS = 10,
    ERROR_INVALID_STATE = 11,
    ERROR_RESOURCE_BUSY = 12,
    ERROR_OPERATION_FAILED = 13,
    ERROR_TIMEOUT = 14,
    ERROR_CANCELLED = 15,
    ERROR_NOT_IMPLEMENTED = 16,
    ERROR_DEPRECATED = 17,
    ERROR_VERSION_MISMATCH = 18,
    ERROR_CHECKSUM_FAILED = 19,
    ERROR_UNKNOWN = 999
};

// Funções com muitos parâmetros
int complex_function_1(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {
    return a + b + c + d + e + f + g + h + i + j;
}

double complex_function_2(double x1, double y1, double z1, double x2, double y2, double z2,
                         double x3, double y3, double z3, double x4, double y4, double z4) {
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2)) +
           sqrt((x2-x3)*(x2-x3) + (y2-y3)*(y2-y3) + (z2-z3)*(z2-z3)) +
           sqrt((x3-x4)*(x3-x4) + (y3-y4)*(y3-y4) + (z3-z4)*(z3-z4));
}

// Arrays grandes
static int large_array_1[1000] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
    81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100
};

static double large_array_2[500] = {
    3.14159, 2.71828, 1.41421, 1.73205, 2.23607, 1.61803, 2.44949, 2.64575, 1.32472, 3.31662,
    2.44949, 1.73205, 2.23607, 3.14159, 2.71828, 1.41421, 1.61803, 2.64575, 1.32472, 3.31662,
    1.41421, 3.14159, 2.71828, 1.73205, 2.23607, 1.61803, 2.44949, 2.64575, 1.32472, 3.31662,
    2.71828, 1.41421, 3.14159, 1.73205, 2.23607, 1.61803, 2.44949, 2.64575, 1.32472, 3.31662,
    1.73205, 2.23607, 1.61803, 3.14159, 2.71828, 1.41421, 2.44949, 2.64575, 1.32472, 3.31662
};

// Strings longas
static const char *long_strings[] = {
    "This is a very long string that contains a lot of text to test the lexer's ability to handle large string literals efficiently and correctly without any performance degradation or memory issues that might occur during the tokenization process of very long string constants in C source code files.",
    "Another extremely long string with different content to ensure that the lexer can handle multiple long strings in the same source file without any problems or performance issues that might arise from processing large amounts of string data during the lexical analysis phase of compilation.",
    "Yet another long string with even more content to thoroughly test the lexer's string handling capabilities and ensure that it can process large amounts of string data efficiently and correctly without any memory leaks or performance bottlenecks that might occur during tokenization.",
    "A fourth long string to continue testing the lexer's ability to handle multiple long string literals in a single source file and ensure that the performance remains consistent even when processing large amounts of string data during the lexical analysis phase.",
    "The fifth and final long string in this array to complete the testing of the lexer's string handling capabilities and ensure that it can efficiently process large amounts of string data without any issues or performance degradation during the tokenization process."
};

// Declarações de funções
int partition(int arr[], int low, int high);

// Funções com loops complexos
void matrix_multiply(double a[4][4], double b[4][4], double result[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i][j] = 0.0;
            for (int k = 0; k < 4; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void bubble_sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void quick_sort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    
    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    
    return (i + 1);
}

// Função principal com muito código
int main(void) {
    printf("Starting performance test...\n");
    
    // Teste de arrays grandes
    int sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += large_array_1[i];
    }
    printf("Sum of large_array_1: %d\n", sum);
    
    double double_sum = 0.0;
    for (int i = 0; i < 500; i++) {
        double_sum += large_array_2[i];
    }
    printf("Sum of large_array_2: %f\n", double_sum);
    
    // Teste de estruturas complexas
    struct ComplexStruct objects[100];
    for (int i = 0; i < 100; i++) {
        objects[i].id = i;
        sprintf(objects[i].name, "Object_%d", i);
        objects[i].position.x = i * 1.0;
        objects[i].position.y = i * 2.0;
        objects[i].position.z = i * 3.0;
        objects[i].velocity.x = i * 0.1;
        objects[i].velocity.y = i * 0.2;
        objects[i].velocity.z = i * 0.3;
        objects[i].active = (i % 2 == 0);
        objects[i].timestamp = time(NULL) + i;
        
        for (int j = 0; j < 100; j++) {
            objects[i].data[j] = sin(i * j * 0.01);
        }
        
        // Inicializar matriz identidade
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                objects[i].transform.m[row][col] = (row == col) ? 1.0 : 0.0;
            }
        }
    }
    
    // Teste de funções complexas
    int result1 = complex_function_1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    printf("Complex function 1 result: %d\n", result1);
    
    double result2 = complex_function_2(1.0, 2.0, 3.0, 4.0, 5.0, 6.0,
                                       7.0, 8.0, 9.0, 10.0, 11.0, 12.0);
    printf("Complex function 2 result: %f\n", result2);
    
    // Teste de multiplicação de matrizes
    double matrix_a[4][4] = {
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        {9.0, 10.0, 11.0, 12.0},
        {13.0, 14.0, 15.0, 16.0}
    };
    
    double matrix_b[4][4] = {
        {16.0, 15.0, 14.0, 13.0},
        {12.0, 11.0, 10.0, 9.0},
        {8.0, 7.0, 6.0, 5.0},
        {4.0, 3.0, 2.0, 1.0}
    };
    
    double matrix_result[4][4];
    matrix_multiply(matrix_a, matrix_b, matrix_result);
    
    printf("Matrix multiplication result:\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%8.2f ", matrix_result[i][j]);
        }
        printf("\n");
    }
    
    // Teste de ordenação
    int test_array[1000];
    for (int i = 0; i < 1000; i++) {
        test_array[i] = 1000 - i;
    }
    
    bubble_sort(test_array, 1000);
    printf("Array sorted successfully\n");
    
    // Teste de strings longas
    for (int i = 0; i < 5; i++) {
        printf("String %d length: %zu\n", i, strlen(long_strings[i]));
    }
    
    // Teste de macros complexas
    int a = 10, b = 20, c = 15;
    printf("MAX(%d, %d) = %d\n", a, b, MAX(a, b));
    printf("MIN(%d, %d) = %d\n", a, b, MIN(a, b));
    printf("CLAMP(%d, %d, %d) = %d\n", c, a, b, CLAMP(c, a, b));
    
    // Teste de cálculos matemáticos intensivos
    double math_result = 0.0;
    for (int i = 1; i <= 1000; i++) {
        math_result += sin(i * 0.01) * cos(i * 0.02) * tan(i * 0.005);
        math_result += sqrt(i) * log(i) * exp(i * 0.001);
        math_result += pow(i, 0.5) * fabs(sin(i));
    }
    printf("Math result: %f\n", math_result);
    
    // Teste de operações bit a bit
    uint64_t bit_result = 0;
    for (int i = 0; i < 64; i++) {
        bit_result |= (1ULL << i);
        bit_result ^= (0xAAAAAAAAAAAAAAAAULL >> i);
        bit_result &= (0x5555555555555555ULL << (i % 32));
    }
    printf("Bit operations result: %llu\n", bit_result);
    
    printf("Performance test completed successfully!\n");
    return 0;
}

// Funções adicionais para completar o arquivo
void additional_function_1(void) {
    // Função adicional 1
    for (int i = 0; i < 100; i++) {
        printf("Additional function 1: %d\n", i);
    }
}

void additional_function_2(void) {
    // Função adicional 2
    double values[50];
    for (int i = 0; i < 50; i++) {
        values[i] = sin(i * 0.1) + cos(i * 0.2);
    }
}

void additional_function_3(void) {
    // Função adicional 3
    struct Point3D points[25];
    for (int i = 0; i < 25; i++) {
        points[i].x = i * 1.5;
        points[i].y = i * 2.5;
        points[i].z = i * 3.5;
    }
}

// Mais definições de macros para teste
#define LARGE_MACRO_1(x) \
    do { \
        printf("Processing: %d\n", x); \
        for (int i = 0; i < x; i++) { \
            printf("  Item %d\n", i); \
        } \
    } while(0)

#define LARGE_MACRO_2(a, b, c) \
    ((a) * (b) + (c) * (a) - (b) * (c) + \
     sqrt((a) * (a) + (b) * (b)) + \
     pow((c), 2.0) + log(fabs(a) + 1.0))

// Estruturas aninhadas complexas
struct NestedStruct {
    struct {
        int inner_a;
        double inner_b;
        struct {
            char deep_char;
            float deep_float;
        } deep_struct;
    } middle_struct;
    
    union {
        int union_int;
        float union_float;
        char union_array[16];
    } test_union;
};

// Função final para completar o arquivo
void final_test_function(void) {
    struct NestedStruct nested;
    nested.middle_struct.inner_a = 42;
    nested.middle_struct.inner_b = 3.14159;
    nested.middle_struct.deep_struct.deep_char = 'A';
    nested.middle_struct.deep_struct.deep_float = 2.71828f;
    nested.test_union.union_int = 12345;
    
    printf("Nested structure test completed\n");
}