/*
 * Teste de erros do preprocessor - diretivas malformadas
 * Este arquivo contém erros em diretivas de preprocessamento
 */

// Include sem aspas ou brackets
#include stdio.h

// Include de arquivo inexistente
#include <arquivo_inexistente.h>
#include "outro_inexistente.h"

// Define sem nome
#define

// Define com nome mas sem valor (tecnicamente válido, mas pode causar problemas)
#define VAZIO

// Define com sintaxe incorreta
#define MACRO_ERRADA(

// Define recursivo
#define RECURSIVO RECURSIVO

// Undef de macro não definida
#undef MACRO_NAO_DEFINIDA

// If sem condição
#if
int x = 1;
#endif

// Ifdef sem nome
#ifdef
int y = 2;
#endif

// Endif sem if correspondente
#endif

// Else sem if
#else
int z = 3;

// If aninhado malformado
#if 1
    #if 2
        int a = 4;
    // Falta #endif para o segundo if
#endif

// Pragma malformado
#pragma

// Line com número inválido
#line abc "arquivo.c"

// Error sem mensagem
#error

// Warning sem mensagem (se suportado)
#warning

int main() {
    // Uso de macro não definida
    int resultado = MACRO_INDEFINIDA;
    
    // Macro com parâmetros incorretos
    #define SOMA(a, b) (a + b)
    int soma = SOMA(5);  // Falta um parâmetro
    
    // Concatenação de tokens malformada
    #define CONCAT(x, y) x ## y
    int CONCAT(var,) = 10;  // Token vazio após ##
    
    return 0;
}

// Diretiva no meio do código (pode causar problemas)
void funcao() {
    int x = 5;
    #define MEIO_FUNCAO 10
    x = MEIO_FUNCAO;
}