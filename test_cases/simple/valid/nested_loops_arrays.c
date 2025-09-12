/*
 * Teste de loops aninhados e manipulação de arrays
 * Demonstra for, while, do-while aninhados e operações com matrizes
 */

#include <stdio.h>

int main() {
    // Matriz 3x3
    int matriz[3][3];
    int contador = 2;
    
    // Preenchimento da matriz com loops aninhados
    printf("Preenchendo matriz 3x3:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matriz[i][j] = contador;
            contador++;
        }
    }
    
    // Impressão da matriz
    printf("\nMatriz preenchida:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
    
    // Array unidimensional
    int numeros[10];
    
    // Preenchimento com while
    int indice = 0;
    while (indice < 10) {
        numeros[indice] = indice * 2;
        indice++;
    }
    
    // Impressão com do-while
    printf("\nArray de números pares:\n");
    int k = 0;
    do {
        printf("%d ", numeros[k]);
        k++;
    } while (k < 10);
    printf("\n");
    
    // Busca do maior elemento
    int maior = numeros[0];
    for (int i = 1; i < 10; i++) {
        if (numeros[i] > maior) {
            maior = numeros[i];
        }
    }
    printf("\nMaior elemento: %d\n, maior);
    
    // Soma dos elementos da diagonal principal
    int soma_diagonal = 0;
    for (int i = 0; i < 3; i++) {
        soma_diagonal += matriz[i][i];
    }
    printf("Soma da diagonal principal: %d\n", soma_diagonal);
    
    // Loop com break e continue
    printf("\nNúmeros ímpares menores que 15:\n");
    for (int i = 1; i < 20; i++) {
        if (i >= 15) {
            break;
        }
        if (i % 2 == 0) {
            continue;
        }
        printf("%d ", i);
    }
    printf("\n");
    
    return 0;
}