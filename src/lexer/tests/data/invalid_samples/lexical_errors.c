// Exemplos de erros léxicos - Para teste do analisador léxico
// Este arquivo contém intencionalmente erros de análise léxica

#include <stdio.h>

int main(void) {
    // Erro: caractere inválido no identificador
    int var@ble = 10;
    
    // Erro: número mal formado
    float bad_number = 3.14.15;
    
    // Erro: sequência de escape inválida
    char invalid_escape = '\q';
    
    // Erro: string com quebra de linha não escapada
    char* multiline = "Esta string
    continua na próxima linha";
    
    // Erro: comentário não terminado
    /* Este comentário nunca termina
    int x = 5;
    
    // Erro: caractere Unicode inválido em contexto ASCII
    int variável = 20; // caractere não-ASCII no identificador
    
    // Erro: literal de caractere vazio
    char empty = '';
    
    // Erro: literal hexadecimal inválido
    int hex_error = 0xGHI;
    
    return 0;
}