/*
 * Casos extremos inválidos para testar detecção de erros
 * Demonstra situações que devem ser rejeitadas pelo lexer/preprocessor
 */

#include <stdio.h>

// ERRO: Identificador começando com dígito
int 123invalid_identifier = 10;

// ERRO: Caracteres inválidos em identificadores
int invalid@identifier = 10;
int invalid-identifier = 10;
int invalid$identifier = 10;

// ERRO: Palavras-chave usadas como identificadores
// int int = 10;  // ERRO: 'int' é palavra-chave
// float if = 3.14;  // ERRO: 'if' é palavra-chave
// char while = 'a';  // ERRO: 'while' é palavra-chave

// ERRO: Números malformados
int bad_octal = 09;  // ERRO: '9' não é válido em octal
int bad_hex = 0xG;   // ERRO: 'G' não é dígito hexadecimal
float bad_float = 1.2.3;  // ERRO: Múltiplos pontos decimais
double bad_exp = 1e;  // ERRO: Expoente incompleto
float bad_exp2 = 1e+;  // ERRO: Expoente sem dígitos

// ERRO: Caracteres de escape inválidos
// char bad_escape1 = '\q';  // ERRO: Escape '\q' não existe
// char bad_escape2 = '\999';  // ERRO: Valor octal muito grande
// char bad_escape3 = '\x';   // ERRO: Escape hex incompleto
// char bad_escape4 = '\xGG'; // ERRO: Dígitos hex inválidos

// ERRO: Literais de caractere malformados
// char empty_char = '';      // ERRO: Caractere vazio
// char multi_char = 'abc';   // ERRO: Múltiplos caracteres
// char unclosed_char = 'a;   // ERRO: Caractere não fechado

// ERRO: Strings malformadas
// char *unclosed_string = "Hello World;  // ERRO: String não fechada
// char *bad_escape_string = "Hello \q World";  // ERRO: Escape inválido

// ERRO: Comentários malformados
/* Este comentário não está fechado
   e continua indefinidamente...

// ERRO: Diretivas de preprocessador inválidas
#invalid_directive
// #define  // ERRO: Nome da macro faltando
// #define 123MACRO value  // ERRO: Nome de macro inválido
// #define MACRO(  // ERRO: Lista de parâmetros não fechada

// ERRO: Uso de ## em posições inválidas
// #define BAD_CONCAT1 ##token  // ERRO: ## no início
// #define BAD_CONCAT2 token##  // ERRO: ## no final
// #define BAD_CONCAT3 ## ##    // ERRO: ## consecutivos

// ERRO: Uso de # fora de macro ou em posição inválida
// #define BAD_STRINGIFY1 #     // ERRO: # sem argumento
// #define BAD_STRINGIFY2(x) x# // ERRO: # após argumento

// ERRO: Redefinição de macro sem #undef
#define REDEFINED_MACRO 1
// #define REDEFINED_MACRO 2  // ERRO: Redefinição sem #undef

// ERRO: Macro recursiva
// #define RECURSIVE_MACRO RECURSIVE_MACRO  // ERRO: Recursão infinita

// ERRO: Condicionais não balanceadas
#ifdef SOME_CONDITION
    int conditional_var = 1;
// ERRO: #endif faltando (comentado para não quebrar o arquivo)

// ERRO: #elif sem #if
// #elif defined(OTHER_CONDITION)  // ERRO: #elif sem #if anterior

// ERRO: #else duplicado
#ifdef ANOTHER_CONDITION
    int value1 = 1;
#else
    int value2 = 2;
// #else  // ERRO: #else duplicado
//     int value3 = 3;
#endif

// ERRO: Operadores malformados
// int bad_op1 = 5 ++ 3;    // ERRO: ++ não é operador binário
// int bad_op2 = 5 +++ 3;   // ERRO: +++ não existe
// int bad_op3 = 5 <=> 3;   // ERRO: <=> não existe em C
// int bad_op4 = 5 ** 3;    // ERRO: ** não é operador em C

// ERRO: Declarações malformadas
// int;  // ERRO: Declaração sem identificador
// int x y;  // ERRO: Múltiplos identificadores sem vírgula
// int [5] array;  // ERRO: Tipo antes do identificador

// ERRO: Inicializadores malformados
// int array[3] = {1, 2, 3, 4, 5};  // ERRO: Muitos inicializadores
// int incomplete[] = {};  // ERRO: Array vazio sem tamanho
// struct { int x; } s = {1, 2};  // ERRO: Muitos inicializadores

// ERRO: Uso de tipos inexistentes
// unknown_type variable;  // ERRO: Tipo não declarado
// struct undefined_struct s;  // ERRO: Struct não definida

// ERRO: Ponteiros malformados
// int **ptr = &&&variable;  // ERRO: Múltiplos & consecutivos
// int *ptr = *;  // ERRO: * sem operando

// ERRO: Arrays malformados
// int array[];  // ERRO: Array sem tamanho (fora de parâmetro)
// int array[-5];  // ERRO: Tamanho negativo
// int array[3.14];  // ERRO: Tamanho não inteiro

// ERRO: Funções malformadas
// int function(int x, int x);  // ERRO: Parâmetros duplicados
// int function(int, int);  // ERRO: Parâmetros sem nome (em definição)
// int function() { return; }  // ERRO: Return sem valor em função não-void

// ERRO: Estruturas malformadas
// struct { int x; int x; } duplicate_members;  // ERRO: Membros duplicados
// struct incomplete;  // ERRO: Declaração incompleta sem definição

// ERRO: Enums malformados
// enum { A, B, A };  // ERRO: Valores duplicados
// enum { FIRST = 1.5 };  // ERRO: Valor não inteiro

// ERRO: Casts malformados
// int x = (int;  // ERRO: Cast incompleto
// int y = int)5;  // ERRO: Cast malformado
// int z = ()5;   // ERRO: Cast vazio

// ERRO: Expressões malformadas
// int result = 5 + ;  // ERRO: Operando faltando
// int result2 = + * 5;  // ERRO: Operadores consecutivos
// int result3 = (5 + 3;  // ERRO: Parênteses não fechados
// int result4 = 5 + 3);  // ERRO: Parênteses não abertos

// ERRO: Labels malformados
123label: printf("Invalid label");  // ERRO: Label começando com número
// goto 123label;  // ERRO: Goto para label inválido

// ERRO: Pragmas inválidas
#pragma invalid_pragma_that_does_not_exist
// #pragma  // ERRO: Pragma vazia

int main(void)
{
    printf("=== Extreme Error Cases Test ===\n");
    printf("This file contains many commented errors for testing\n");
    
    // Alguns erros que podem ser testados em runtime
    
    // ERRO: Divisão por zero (runtime)
    // int zero = 0;
    // int result = 10 / zero;  // ERRO: Divisão por zero
    
    // ERRO: Acesso a ponteiro nulo (runtime)
    // int *null_ptr = NULL;
    // int value = *null_ptr;  // ERRO: Dereferência de ponteiro nulo
    
    // ERRO: Overflow de array (runtime)
    // int array[5];
    // array[10] = 42;  // ERRO: Acesso fora dos limites
    
    // ERRO: Uso de variável não inicializada
    // int uninitialized;
    // printf("Uninitialized: %d\n", uninitialized);  // ERRO: Valor indefinido
    
    return 0;
}

// ERRO: Função sem tipo de retorno (C99+)
// function_without_return_type() {  // ERRO: Tipo implícito não permitido
//     return 42;
// }

// ERRO: Função main com tipo incorreto
// void main() {  // ERRO: main deve retornar int
//     printf("Wrong main signature\n");
// }

// ERRO: Variáveis globais com mesmo nome
int global_var = 10;
// int global_var = 20;  // ERRO: Redefinição de variável global

// ERRO: Funções com mesmo nome e assinatura
int test_function(int x) {
    return x * 2;
}
// int test_function(int x) {  // ERRO: Redefinição de função
//     return x * 3;
// }

// ERRO: Goto para label inexistente
void test_goto(void) {
    // goto nonexistent_label;  // ERRO: Label não existe
    printf("Goto test\n");
}

// ERRO: Switch sem break pode causar fall-through (warning)
void test_switch(int value) {
    switch (value) {
        case 1:
            printf("Case 1\n");
            // AVISO: Fall-through sem break
        case 2:
            printf("Case 2\n");
            break;
        default:
            printf("Default\n");
    }
}

// Fechar o comentário que foi deixado aberto intencionalmente
*/

// ERRO: Arquivo terminando com backslash (linha de continuação)
// Esta linha deveria continuar na próxima, mas o arquivo termina \