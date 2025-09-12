/*
 * Teste de erros do lexer - caracteres inválidos
 * Este arquivo contém caracteres que não são reconhecidos pelo lexer
 */

#include <stdio.h>

int main() {
    // Caractere inválido no meio do código
    int x = 10;
    int y = 20@;  // @ é um caractere inválido em C
    
    // String com caractere de escape inválido
    char str[] = "Hello \q World";  // \q não é um escape válido
    
    // Número com caracteres inválidos
    int numero = 123abc;  // Mistura de dígitos e letras
    
    // Operador inexistente
    int resultado = x @@ y;  // @@ não é um operador válido
    
    // Caractere Unicode inválido
    int variável = 5;  // Caracteres acentuados podem causar erro
    
    // Comentário com caractere inválido
    /* Este é um comentário com @ caractere inválido */
    
    // String não terminada (erro léxico)
    printf("Esta string não tem fim
    
    return 0;
}