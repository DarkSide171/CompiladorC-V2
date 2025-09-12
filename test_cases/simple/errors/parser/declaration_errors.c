/*
 * Teste de erros do parser - declarações malformadas
 * Este arquivo contém erros em declarações de variáveis e funções
 */

#include <stdio.h>

// Declaração de função com tipo de retorno ausente
funcao_sem_tipo() {
    return 5;
}

// Declaração com tipo inexistente
inexistente_type variavel;

int main() {
    // Declaração sem tipo
    x = 10;
    
    // Múltiplas declarações malformadas
    int a, , c;
    
    // Inicialização incorreta de array
    int arr[3] = {1, 2, 3, 4, 5};  // Mais elementos que o tamanho
    
    // Struct sem nome
    struct {
        int campo1;
        int campo2;
    } variavel_struct;  // OK até aqui
    
    struct {  // Struct sem nome e sem variável
        int x;
    };  // Declaração inútil
    
    // Enum malformado
    enum {
        VALOR1,
        VALOR2,
    };  // Vírgula extra no final
    
    // Ponteiro malformado
    int* *ptr;  // Espaço incorreto
    
    // Array multidimensional malformado
    int matriz[][];  // Dimensões vazias
    
    // Função com parâmetros malformados
    void funcao(int, float y);  // Primeiro parâmetro sem nome
    
    // Typedef malformado
    typedef struct Pessoa;
    
    // Declaração de variável após código
    printf("Hello\n");
    int tardia = 5;  // Declaração após statement (C89/C90)
    
    return 0;
}

// Função com corpo vazio mas sem chaves
void funcao_vazia()