/*
 * Arquivo de teste de performance com erros intencionais
 * Contém uma grande quantidade de código C com erros para testar
 * a capacidade do lexer de detectar erros em arquivos grandes
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ERRO: Macro com sintaxe inválida
#define BAD_MACRO(x) ((x) * @invalid)

// ERRO: Redefinição de macro
#define MAX_SIZE 100
#define MAX_SIZE 200

// ERRO: Identificadores inválidos
int 123invalid = 10;
int var@ble = 20;
int test$var = 30;

// ERRO: Números malformados
float bad_float = 3.14.159;
int bad_octal = 09;
int bad_hex = 0xGHI;

// ERRO: Caracteres de escape inválidos
char bad_escape1 = '\q';
char bad_escape2 = '\999';

// ERRO: Strings malformadas
char *unclosed_string = "This string is never closed"

// ERRO: Comentário não fechado
 Este comentário nunca é fechado
   e continua indefinidamente...

struct BadStruct {
    int x;
    float y @ z;  // ERRO: Caractere inválido
};

// ERRO: Enum com valores inválidos
enum BadEnum {
    VALUE1 = 1.5,  // ERRO: Valor não inteiro
    VALUE2 = "string",  // ERRO: String em enum
    VALUE3 @ 10  // ERRO: Operador inválido
};

// ERRO: Função com sintaxe inválida
int bad_function(int a, b c) {  // ERRO: Parâmetros malformados
    return a + b + c;
}

// ERRO: Array com tamanho inválido
int bad_array[3.14];  // ERRO: Tamanho não inteiro

// ERRO: Operadores inexistentes
int test_operators(int a, int b) {
    int result1 = a <> b;  // ERRO: Operador <> não existe
    int result2 = a ** b;  // ERRO: Operador ** não existe
    int result3 = a >< b;  // ERRO: Operador >< não existe
    return result1 + result2 + result3;
}

// ERRO: Literais de caractere inválidos
char bad_chars[] = {
    ''',      // ERRO: Caractere vazio
    'abc',   // ERRO: Múltiplos caracteres
    'x       // ERRO: Não fechado
};

// ERRO: Declarações malformadas
int @ global_var = 42;  // ERRO: @ no nome
float # another_var = 3.14;  // ERRO: # no nome

// ERRO: Preprocessor inválido
#invalid_directive
#define  // ERRO: Nome da macro faltando
#define 123MACRO value  // ERRO: Nome de macro inválido

// ERRO: Estrutura com sintaxe inválida
struct {
    int x
    float y;  // ERRO: Falta ponto e vírgula na linha anterior
    char z @;  // ERRO: @ inválido
} bad_struct_instance;

// ERRO: União com problemas
union BadUnion {
    int a;
    float b
    char c[10];  // ERRO: Falta ponto e vírgula na linha anterior
};

// ERRO: Função com return type inválido
@ int invalid_return_type(void) {  // ERRO: @ antes do tipo
    return 42;
}

// ERRO: Loop com sintaxe inválida
void bad_loops(void) {
    // ERRO: For loop malformado
    for (int i = 0; i < 10 i++) {  // ERRO: Falta ponto e vírgula
        printf("%d\n", i);
    }
    
    // ERRO: While com condição inválida
    while (x @ 5) {  // ERRO: @ não é operador válido
        x++;
    }
    
    // ERRO: Do-while malformado
    do {
        printf("test\n");
    } while (true)  // ERRO: Falta ponto e vírgula
}

// ERRO: Switch com problemas
void bad_switch(int x) {
    switch (x) {
        case 1.5:  // ERRO: Case com float
            printf("One and half\n");
            break;
        case "string":  // ERRO: Case com string
            printf("String case\n");
            break;
        default @:  // ERRO: @ após default
            printf("Default\n");
    }
}

// ERRO: Ponteiros malformados
void bad_pointers(void) {
    int x = 10;
    int *@ ptr = &x;  // ERRO: @ após *
    int **@ double_ptr = &ptr;  // ERRO: @ após **
    
    // ERRO: Desreferenciamento inválido
    int value = *@ptr;  // ERRO: @ após *
}

// ERRO: Casting inválido
void bad_casts(void) {
    int x = 10;
    float y = (float@)x;  // ERRO: @ no cast
    char z = (char*)x;  // ERRO: Cast de int para char*
}

// ERRO: Inicializadores malformados
int bad_init_array[] = {
    1, 2, 3,
    4, 5, 6
    7, 8, 9  // ERRO: Falta vírgula
};

struct Point {
    int x, y;
};

struct Point bad_init_struct = {
    .x = 10
    .y = 20  // ERRO: Falta vírgula
};

// ERRO: Expressões malformadas
int bad_expressions(void) {
    int a = 10, b = 20, c = 30;
    
    // ERRO: Parênteses não balanceados
    int result1 = (a + b * c;  // ERRO: Falta )
    int result2 = a + (b * c));  // ERRO: ) extra
    
    // ERRO: Operadores malformados
    int result3 = a +* b;  // ERRO: +* não é válido
    int result4 = a /% b;  // ERRO: /% não é válido
    
    return result1 + result2 + result3 + result4;
}

// ERRO: Labels inválidos
void bad_labels(void) {
    123label:  // ERRO: Label não pode começar com número
        printf("Bad label\n");
    
    @label:  // ERRO: @ no label
        printf("Another bad label\n");
    
    goto 123label;  // ERRO: Goto para label inválido
}

// ERRO: Typedef malformado
typedef struct @ BadTypedef {  // ERRO: @ no nome da struct
    int x, y;
} BadTypedef;

// ERRO: Função main malformada
int @ main(void) {  // ERRO: @ antes do tipo de retorno
    printf("Hello, World!\n");
    
    // ERRO: Chamada de função inválida
    printf@("Invalid function call\n");  // ERRO: @ no nome da função
    
    // ERRO: Return inválido
    return @0;  // ERRO: @ antes do valor
}

// ERRO: Mais erros diversos
void more_errors(void) {
    // ERRO: Declaração de variável inválida
    int @ x = 10;  // ERRO: @ no nome da variável
    float y @ = 3.14;  // ERRO: @ após o nome
    
    // ERRO: Operações aritméticas inválidas
    int result = x @+ y;  // ERRO: @ antes do +
    result = x +@ y;  // ERRO: @ após o +
    
    // ERRO: Comparações inválidas
    if (x @< y) {  // ERRO: @ antes do <
        printf("x is less than y\n");
    }
    
    if (x <@ y) {  // ERRO: @ após o <
        printf("x is less than y\n");
    }
    
    // ERRO: Incremento/decremento inválido
    x@++;  // ERRO: @ antes do ++
    ++@x;  // ERRO: @ após o ++
    y@--;  // ERRO: @ antes do --
    --@y;  // ERRO: @ após o --
}

// ERRO: Função sem tipo de retorno
bad_no_return_type(void) {  // ERRO: Sem tipo de retorno
    printf("Function without return type\n");
}

// ERRO: Parâmetros de função inválidos
int bad_parameters(int @a, float b@, char @c@) {  // ERRO: @ nos parâmetros
    return a + (int)b + c;
}

// ERRO: Array multidimensional inválido
int bad_multi_array[10@][20@];  // ERRO: @ nas dimensões

// ERRO: Ponteiro para função inválido
int (*@bad_func_ptr)(int, float);  // ERRO: @ no ponteiro de função

// ERRO: Estrutura com bit fields inválidos
struct BadBitFields {
    int a : @5;  // ERRO: @ no bit field
    int b : 3@;  // ERRO: @ após o tamanho
    int c : 0@;  // ERRO: @ após zero
};

// ERRO: Mais macros inválidas
#define BAD_STRINGIFY(x) @#x  // ERRO: @ antes do #
#define BAD_CONCAT(a, b) a@##@b  // ERRO: @ ao redor do ##

// ERRO: Condicionais de preprocessor inválidas
#if @defined(SOME_MACRO)  // ERRO: @ antes do defined
    #define CONDITIONAL_MACRO 1
#elif @1  // ERRO: @ na condição
    #define CONDITIONAL_MACRO 2
#endif@  // ERRO: @ após endif

// ERRO: Include inválido
#include @<stdio.h>  // ERRO: @ antes do <
#include <math.h@>  // ERRO: @ antes do >

// Final do arquivo com erro de sintaxe
// ERRO: Chave não fechada
void final_function(void) {
    printf("This function is never closed properly\n");
    // Falta a chave de fechamento }