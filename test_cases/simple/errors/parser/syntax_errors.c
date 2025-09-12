/*
 * Teste de erros do parser - erros de sintaxe
 * Este arquivo contém erros sintáticos que o parser deve detectar
 */

#include <stdio.h>

int main() {
    // Parênteses não balanceados
    int x = (5 + 3;
    int y = 10 + (2 * 3));
    
    // Chaves não balanceadas
    if (x > 0) {
        printf("x é positivo\n");
    // Falta fechar a chave
    
    // Ponto e vírgula ausente
    int a = 10
    int b = 20;
    
    // Declaração de função incompleta
    int funcao_incompleta(
    
    // Estrutura de controle malformada
    for (int i = 0; i < 10) {  // Falta incremento
        printf("%d\n", i);
    }
    
    // While sem condição
    while () {
        break;
    }
    
    // Switch sem expressão
    switch {
        case 1:
            printf("Um\n");
            break;
    }
    
    // Array com índice malformado
    int arr[5];
    arr[] = 10;  // Índice vazio
    
    // Operador sem operando
    int resultado = 5 +;
    
    // Return sem ponto e vírgula
    return 0
}