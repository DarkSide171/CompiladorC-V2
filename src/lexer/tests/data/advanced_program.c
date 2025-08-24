#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Estrutura para representar um ponto
struct Point {
    int x;
    int y;
};

// Função para calcular distância
float calculateDistance(struct Point p1, struct Point p2) {
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
}

// Função para processar array
void processArray(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] % 2 == 0) {
            printf("Número par: %d\n", arr[i]);
        } else {
            printf("Número ímpar: %d\n", arr[i]);
        }
    }
}

int main() {
    // Declaração de variáveis
    struct Point origin = {0, 0};
    struct Point target = {3, 4};
    int numbers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int size = sizeof(numbers) / sizeof(numbers[0]);
    
    // Cálculo de distância
    float distance = calculateDistance(origin, target);
    printf("Distância: %.2f\n", distance);
    
    // Processamento do array
    printf("\nProcessando números:\n");
    processArray(numbers, size);
    
    // Loop while
    int counter = 0;
    while (counter < 3) {
        printf("Counter: %d\n", counter);
        counter++;
    }
    
    // Switch case
    int choice = 2;
    switch (choice) {
        case 1:
            printf("Opção 1 selecionada\n");
            break;
        case 2:
            printf("Opção 2 selecionada\n");
            break;
        default:
            printf("Opção inválida\n");
            break;
    }
    
    return 0;
}