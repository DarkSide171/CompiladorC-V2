/*
 * Teste de erros de preprocessamento
 * Demonstra vários tipos de erros que podem ocorrer durante o preprocessamento
 */

#include <stdio.h>

// ERRO: Macro recursiva infinita (comentada para evitar loop)
// #define RECURSIVE RECURSIVE

// ERRO: Redefinição de macro sem #undef
#define MAX_SIZE 100
#define MAX_SIZE 200  // ERRO: Redefinição sem #undef

// ERRO: Macro com parênteses não balanceados
// #define BAD_MACRO(x printf("Value: %d\n", x  // Falta fechar parênteses

// ERRO: Uso de ## em posição inválida
#define BAD_CONCAT ## invalid ##  // ## no início/fim

// ERRO: Stringify de token inválido
// #define BAD_STRINGIFY(x) #  // # sem argumento

// ERRO: Macro com argumentos variáveis malformada
// #define BAD_VARIADIC(fmt, ...) printf(fmt, __VA_ARGS__)  // Problema se não há argumentos variáveis

// ERRO: Condicional não fechada
#ifdef DEBUG_MODE
    #define DEBUG_PRINT(x) printf(x)
// #endif  // ERRO: Comentado para demonstrar #ifdef não fechado

// ERRO: #elif sem #if
// #elif defined(RELEASE_MODE)  // ERRO: #elif sem #if anterior
//     #define DEBUG_PRINT(x)

// ERRO: #else duplicado
#ifdef SOME_CONDITION
    #define VALUE 1
#else
    #define VALUE 2
// #else  // ERRO: #else duplicado
//     #define VALUE 3
#endif

// ERRO: Include de arquivo inexistente
#include "nonexistent_file.h"  // ERRO: Arquivo não existe

// ERRO: Include circular (simulado com comentários)
// Arquivo A inclui B, B inclui A
// #include "circular_a.h"

// ERRO: Macro com número incorreto de argumentos
#define ADD_THREE(a, b, c) ((a) + (b) + (c))

// ERRO: Uso de macro indefinida
// int result = UNDEFINED_MACRO(5);  // ERRO: Macro não definida

// ERRO: Concatenação que resulta em token inválido
#define MAKE_INVALID(x) x##@##invalid
int MAKE_INVALID(var) = 10;  // Resultaria em var@invalid (token inválido)

// ERRO: Macro que expande para código sintaticamente incorreto
#define BAD_SYNTAX if (1
// BAD_SYNTAX printf("test");  // Expande para: if (1 printf("test");

// ERRO: Uso incorreto de __VA_ARGS__ sem ...
// #define NO_VARIADIC(x) printf(__VA_ARGS__)  // ERRO: __VA_ARGS__ sem ...

// ERRO: Linha muito longa após expansão de macro
#define VERY_LONG_STRING "This is a very long string that when repeated multiple times in a macro expansion could potentially exceed the maximum line length supported by some preprocessors and cause issues during compilation process"
// char *long_line = VERY_LONG_STRING VERY_LONG_STRING VERY_LONG_STRING VERY_LONG_STRING;

// ERRO: Macro que tenta redefinir palavra-chave
// #define int float  // ERRO: Tentativa de redefinir palavra-chave

// ERRO: Uso de # fora de macro
char *str = #invalid;  // ERRO: # fora de contexto de macro

// ERRO: Uso de ## fora de macro
// int var##name = 10;  // ERRO: ## fora de contexto de macro

// ERRO: Macro com escape inválido em string
#define BAD_STRING "This string has invalid escape \q sequence"

// ERRO: Condicional com expressão inválida
// #if UNDEFINED_SYMBOL > 10  // ERRO: Símbolo não definido em expressão
//     #define CONDITIONAL_VALUE 1
// #endif

// ERRO: Pragma inválida
#pragma invalid_pragma_directive

// ERRO: Diretiva de preprocessador inválida
#invalid_directive

// ERRO: Macro que causa overflow em expressão aritmética
#define LARGE_NUMBER 999999999999999999999
// #if LARGE_NUMBER * LARGE_NUMBER > 0  // Pode causar overflow
//     #define OVERFLOW_RESULT 1
// #endif

// ERRO: Aninhamento excessivo de includes (simulado)
// Muitos níveis de #include podem causar stack overflow

// ERRO: Caractere inválido em nome de macro
#define invalid-name 10  // ERRO: hífen não é válido em identificador

// ERRO: Macro vazia com problema de sintaxe
#define EMPTY_MACRO
// int x = EMPTY_MACRO 5;  // Pode causar erro de sintaxe

// ERRO: Uso de reserved identifier
// #define __reserved_name 10  // ERRO: Identificadores começando com __ são reservados

int main(void)
{
    printf("=== Preprocessor Error Test ===\n");
    
    // Teste de macro com argumentos corretos
    int sum = ADD_THREE(1, 2, 3);
    printf("Sum: %d\n", sum);
    
    // ERRO: Chamada de macro com número incorreto de argumentos
    // int bad_sum = ADD_THREE(1, 2);  // ERRO: Faltam argumentos
    // int bad_sum2 = ADD_THREE(1, 2, 3, 4);  // ERRO: Muitos argumentos
    
    // ERRO: Uso de macro após #undef
    #undef MAX_SIZE
    // int size = MAX_SIZE;  // ERRO: Macro foi removida com #undef
    
    // Teste de string com escape inválido (comentado)
    // printf(BAD_STRING);  // ERRO: Escape inválido
    
    return 0;
}

// ERRO: Função definida dentro de macro de forma incorreta
#define DEFINE_FUNCTION(name) \
    void name(void) { \
        printf("Function: %s\n", #name); \
    // }  // ERRO: Chave não fechada na macro

// ERRO: Tentativa de usar macro como tipo
#define MY_TYPE int
// MY_TYPE* ptr;  // Isso é válido
// typedef MY_TYPE new_type;  // Isso também é válido
// struct MY_TYPE { int x; };  // ERRO: Não pode usar macro como nome de struct

// ERRO: Macro que gera código com variável não declarada
#define USE_UNDEFINED_VAR printf("%d\n", undefined_variable)

// Função que demonstra alguns erros
void demonstrate_errors(void)
{
    // ERRO: Uso de macro que gera código inválido
    // USE_UNDEFINED_VAR;  // ERRO: undefined_variable não existe
    
    // ERRO: Expansão de macro que causa conflito de tipos
    #define CONFLICTING_TYPE double
    int value = 10;
    // CONFLICTING_TYPE value = 3.14;  // ERRO: Redeclaração com tipo diferente
    
    printf("Error demonstration function\n");
}

// Fechar o #ifdef que foi deixado aberto intencionalmente
#endif