/*
 * Teste de ponteiros e manipulação de memória
 * Demonstra uso básico de ponteiros, referências e dereferenciamento
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    // Variáveis básicas
    int numero = 42;
    float valor = 3.14f;
    char caractere = 'X';
    
    // Ponteiros
    int *ptr_numero = &numero;
    float *ptr_valor = &valor;
    char *ptr_caractere = &caractere;
    
    // Impressão de valores e endereços
    printf("Valor de numero: %d\n", numero);
    printf("Endereço de numero: %p\n", (void*)&numero);
    printf("Valor através do ponteiro: %d\n", *ptr_numero);
    printf("Endereço armazenado no ponteiro: %p\n", (void*)ptr_numero);
    
    // Modificação através do ponteiro
    *ptr_numero = 100;
    printf("\nApós modificação via ponteiro:\n");
    printf("Valor de numero: %d\n", numero);
    
    // Array e ponteiros
    int array[5] = {10, 20, 30, 40, 50};
    int *ptr_array = array;
    
    printf("\nElementos do array via ponteiro:\n");
    for (int i = 0; i < 5; i++) {
        printf("array[%d] = %d (via ponteiro: %d)\n", i, array[i], *(ptr_array + i));
    }
    
    // Aritmética de ponteiros
    printf("\nAritmética de ponteiros:\n");
    int *ptr = array;
    for (int i = 0; i < 5; i++) {
        printf("Posição %d: valor = %d, endereço = %p\n", i, *ptr, (void*)ptr);
        ptr++;
    }
    
    // Ponteiro para ponteiro
    int **ptr_ptr = &ptr_numero;
    printf("\nPonteiro para ponteiro:\n");
    printf("Valor através de **ptr_ptr: %d\n", **ptr_ptr);
    
    // Alocação dinâmica simples
    int *dinamico = (int*)malloc(sizeof(int));
    if (dinamico != NULL) {
        *dinamico = 999;
        printf("\nValor alocado dinamicamente: %d\n", *dinamico);
        free(dinamico);
    }
    
    // Array dinâmico
    int tamanho = 3;
    int *array_dinamico = (int*)malloc(tamanho * sizeof(int));
    if (array_dinamico != NULL) {
        for (int i = 0; i < tamanho; i++) {
            array_dinamico[i] = (i + 1) * 10;
        }
        
        printf("\nArray dinâmico:\n");
        for (int i = 0; i < tamanho; i++) {
            printf("elemento[%d] = %d\n", i, array_dinamico[i]);
        }
        
        free(array_dinamico);
    }
    
    return 0;
}