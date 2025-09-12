/*
 * Teste de erros do preprocessor - macros problemáticas
 * Este arquivo contém erros relacionados a definição e uso de macros
 */

#include <stdio.h>

// Macro com parênteses não balanceados
#define MACRO_DESBALANCEADA(x (x + 1)

// Macro com vírgula extra
#define MACRO_VIRGULA(a, b,) (a + b)

// Macro com nome de parâmetro duplicado
#define MACRO_DUPLICADA(x, x) (x * 2)

// Macro que se redefine
#define PI 3.14
#define PI 3.14159  // Redefinição

// Macro com expansão circular
#define A B
#define B C
#define C A

// Macro com ## no início ou fim
#define CONCAT_INICIO ## x
#define CONCAT_FIM x ##

// Macro com # sem parâmetro
#define STRING_SEM_PARAM #

// Macro variádica malformada
#define VARIADICA_ERRADA(..., x) printf(x)

// Macro com linha muito longa (pode causar problemas)
#define MACRO_LONGA(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z) \
    (a + b + c + d + e + f + g + h + i + j + k + l + m + n + o + p + q + r + s + t + u + v + w + x + y + z)

int main() {
    // Uso de macro com número incorreto de argumentos
    #define SOMA(a, b) (a + b)
    int resultado1 = SOMA(5);        // Poucos argumentos
    int resultado2 = SOMA(5, 3, 2);  // Muitos argumentos
    
    // Uso de macro circular
    int circular = A;
    
    // Macro aninhada problemática
    #define ANINHADA(x) SOMA(x, SOMA(x, x))
    int aninhado = ANINHADA(SOMA(1, 2));  // Pode causar expansão complexa
    
    // Uso de operador # incorretamente
    #define STRINGIFY(x) #x
    printf("%s\n", STRINGIFY());  // Sem argumento
    
    // Uso de ## incorretamente
    #define CONCAT(x, y) x ## y
    int CONCAT(, var) = 10;  // Token vazio antes de ##
    
    // Macro com efeitos colaterais
    #define INCREMENTA(x) (++x)
    int i = 5;
    int resultado3 = INCREMENTA(i) + INCREMENTA(i);  // Duplo incremento
    
    // Conditional compilation malformado
    #if MACRO_INDEFINIDA > 0  // Macro não definida em expressão
        printf("Macro indefinida\n");
    #endif
    
    // Nested conditionals com erro
    #ifdef DEBUG
        #if LEVEL
            printf("Debug level\n");
        #elif  // elif sem condição
            printf("Elif vazio\n");
        #endif
    #endif
    
    return 0;
}

// Macro definida após uso (pode causar problemas dependendo do preprocessor)
void funcao_tardia() {
    int x = MACRO_TARDIA;
    #define MACRO_TARDIA 42
}