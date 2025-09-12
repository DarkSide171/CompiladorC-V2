/*
 * Teste de erros do lexer - sequências de escape inválidas
 * Este arquivo contém erros relacionados a strings e caracteres
 */

#include <stdio.h>

int main() {
    // Sequências de escape inválidas
    char c1 = '\z';     // \z não é uma sequência válida
    char c2 = '\400';   // Valor octal muito grande
    char c3 = '\xGG';   // Dígitos hexadecimais inválidos
    
    // Strings com problemas
    char str1[] = "Linha 1\nLinha 2\k";  // \k inválido
    char str2[] = "Unicode: \u12GH";     // Dígitos Unicode inválidos
    
    // Caracteres vazios ou múltiplos
    char empty = '';    // Caractere vazio
    char multi = 'abc'; // Múltiplos caracteres
    
    // Números com prefixos incorretos
    int hex = 0X;       // Hexadecimal sem dígitos
    int oct = 09;       // 9 não é válido em octal
    
    // Operadores com espaços
    int x = 5;
    int y = x + + 3;    // Espaço entre operadores
    int z = x - - 2;    // Espaço entre operadores
    
    printf("Teste de erros lexicais\n");
    
    return 0;
}