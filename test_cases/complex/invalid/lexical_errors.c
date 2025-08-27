#include <stdio.h>
#include <stdlib.h>

// Este arquivo contém vários erros léxicos intencionais

#define MAX_SIZE 100
#define INVALID_MACRO(x) ((x) * @invalid_char)

int main() {
    // String mal formada (sem aspas de fechamento)
    char *str1 = "This string is not closed properly
    
    // Caractere inválido no meio do código
    int value = 42 @ 10;
    
    // Comentário mal formado
    /* Este comentário não tem fechamento
    
    int x = 10;
    
    // Número mal formado
    float pi = 3.14.159;
    
    // Caractere de escape inválido
    char invalid_escape = '\q';
    
    // Identificador com caractere inválido
    int var$able = 5;
    
    // Operador inexistente
    if (x <> 5) {
        printf("Invalid operator\n");
    }
    
    return 0;
}