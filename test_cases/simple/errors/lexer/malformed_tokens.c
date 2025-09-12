/*
 * Teste de erros do lexer - tokens malformados
 * Este arquivo contém tokens que não seguem as regras lexicais
 */

#include <stdio.h>

int main() {
    // Números malformados
    int a = 123.45.67;  // Número com dois pontos decimais
    float b = .;        // Ponto decimal sem dígitos
    int c = 0x;         // Hexadecimal incompleto
    int d = 0b;         // Binário incompleto (se suportado)
    
    // Identificadores inválidos
    int 123var;         // Identificador começando com número
    int var-name;       // Hífen no meio do identificador
    int var.name;       // Ponto no meio do identificador
    
    // Strings malformadas
    char str1[] = "string sem fechamento
    char str2[] = 'caractere com mais de um char';
    
    // Comentários malformados
    /* Comentário sem fechamento
    
    // Operadores malformados
    int x = 5;
    int y = x ++ +;     // Operadores mal posicionados
    
    return 0;