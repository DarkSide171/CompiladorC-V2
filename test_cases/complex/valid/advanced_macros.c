#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 100
#define SQUARE(x) ((x) * (x))
#define DEBUG_PRINT(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#define CONCAT(a, b) a##b
#define STRINGIFY(x) #x

// Macro condicional
#ifdef DEBUG
    #define LOG(msg) printf("LOG: %s\n", msg)
#else
    #define LOG(msg)
#endif

// Estrutura complexa
typedef struct {
    int id;
    char name[MAX_SIZE];
    float *values;
    struct Node *next;
} Node;

// Função com múltiplos tipos de tokens
int processData(const char *input, Node **output, size_t count) {
    if (!input || !output || count == 0) {
        return -1;
    }
    
    // Operadores diversos
    int result = 0;
    result += SQUARE(5);
    result -= 10;
    result *= 2;
    result /= 3;
    result %= 7;
    
    // Operadores lógicos e relacionais
    if (result > 0 && result < 100) {
 //       DEBUG_PRINT("Result is valid: %d", result);
    } else if (result <= 0 || result >= 100) {
   //     DEBUG_PRINT("Result out of range: %d", result);
    }
    
    // Operadores bit a bit
    unsigned int flags = 0xFF;
    flags &= 0x0F;
    flags |= 0x10;
    flags ^= 0x05;
    flags <<= 2;
    flags >>= 1;
    
    // Ponteiros e arrays
    char buffer[256] = {0};
    char *ptr = &buffer[0];
    *ptr = 'A';
    *(ptr + 1) = 'B';
    
    // Strings e caracteres especiais
    const char *message = "Hello\nWorld\t!\"Test\"";
    char escape_chars[] = {'\\', '\n', '\t', '\r', '\0'};
    
    // Números em diferentes bases
    int decimal = 42;
    int octal = 052;        // 42 em octal
    int hex = 0x2A;         // 42 em hexadecimal
    float pi = 3.14159f;
    double e = 2.718281828;
    
    return result;
}

int main(void) {
    Node *head = NULL;
    const char *test_input = STRINGIFY(MAX_SIZE);
    
    int result = processData(test_input, &head, MAX_SIZE);
    

    
    return 0;
}