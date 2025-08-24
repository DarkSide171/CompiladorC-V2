// Exemplo complexo - Recursos avançados da linguagem C
// Arquivo de teste para validação completa do analisador léxico

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 100
#define SQUARE(x) ((x) * (x))

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef enum {
    RED,
    GREEN,
    BLUE
} Color;

union Data {
    int i;
    float f;
    char str[20];
};

static int global_counter = 0;
extern void external_function(void);

Node* createNode(int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode != NULL) {
        newNode->data = value;
        newNode->next = NULL;
    }
    return newNode;
}

void processArray(int arr[], size_t size) {
    for (size_t i = 0; i < size; i++) {
        arr[i] *= 2;
    }
}

int main(void) {
    // Teste de literais diversos
    int decimal = 42;
    int octal = 052;
    int hex = 0x2A;
    float scientific = 1.23e-4f;
    double precision = 3.141592653589793;
    char escape_chars[] = "Hello\tWorld\n";
    
    // Teste de operadores
    int a = 10, b = 20;
    int result = (a + b) * SQUARE(3) / (a - b + 1);
    
    // Teste de ponteiros e arrays
    int numbers[5] = {1, 2, 3, 4, 5};
    int* ptr = numbers;
    
    // Teste de estruturas de controle
    switch (result % 3) {
        case 0:
            printf("Divisível por 3\n");
            break;
        case 1:
            printf("Resto 1\n");
            break;
        default:
            printf("Resto 2\n");
            break;
    }
    
    // Teste de loops
    int i = 0;
    while (i < 5) {
        printf("%d ", *(ptr + i));
        i++;
    }
    
    do {
        global_counter++;
    } while (global_counter < 3);
    
    // Teste de goto (raramente usado, mas válido)
    if (result < 0) {
        goto error_handling;
    }
    
    processArray(numbers, sizeof(numbers) / sizeof(numbers[0]));
    
    // Teste de union e enum
    union Data data;
    data.i = 100;
    Color favorite = BLUE;
    
    return 0;
    
error_handling:
    fprintf(stderr, "Erro: resultado negativo\n");
    return -1;
}