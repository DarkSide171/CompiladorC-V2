#include <stdio.h>

// Este arquivo contém caracteres inválidos para testar o lexer

int main() {
    // Caracteres inválidos em diferentes contextos
    int x = 10;
    int y @ 20;  // Caractere @ inválido
    
    // Mais caracteres inválidos
    float value = 3.14 # 2.0;  // Caractere # fora de contexto
    
    // Caractere inválido em identificador
    int test$var = 5;  // $ não é válido em identificadores
    
    // Operador inexistente
    if (x <> y) {  // <> não é um operador válido em C
        printf("Test\n");
    }
    
    return 0;
}