// Exemplos de erros sintáticos - Para teste do tratamento de erros
// Este arquivo contém intencionalmente erros de sintaxe

#include <stdio.h>

int main(void) {
    // Erro: ponto e vírgula ausente
    int x = 10
    
    // Erro: parênteses não balanceados
    if (x > 5 {
        printf("x é maior que 5\n");
    }
    
    // Erro: chaves não balanceadas
    for (int i = 0; i < 3; i++) {
        printf("Iteração %d\n", i);
    // Falta fechar a chave
    
    // Erro: operador inválido
    int y = x ++ 5;
    
    // Erro: string não terminada
    char* msg = "Hello World;
    
    return 0;
// Falta fechar a função main