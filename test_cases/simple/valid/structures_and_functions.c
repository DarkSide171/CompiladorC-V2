/*
 * Teste de estruturas e funções
 * Demonstra definição de structs, funções e manipulação de dados
 */

#include <stdio.h>
#include <string.h>

// Definição de estrutura
struct Pessoa {
    char nome[50];
    int idade;
    float altura;
};

// Função para calcular área de retângulo
int calcular_area(int largura, int altura) {
    return largura * altura;
}

// Função para imprimir dados da pessoa
void imprimir_pessoa(struct Pessoa p) {
    printf("Nome: %s\n", p.nome);
    printf("Idade: %d anos\n", p.idade);
    printf("Altura: %.2f m\n", p.altura);
}

int main() {
    // Declaração e inicialização de estrutura
    struct Pessoa pessoa1;
    strcpy(pessoa1.nome, "João");
    pessoa1.idade = 25;
    pessoa1.altura = 1.75f;
    
    // Outra forma de inicialização
    struct Pessoa pessoa2 = {"Maria", 30, 1.68f};
    
    // Array de estruturas
    struct Pessoa pessoas[3];
    pessoas[0] = pessoa1;
    pessoas[1] = pessoa2;
    strcpy(pessoas[2].nome, "Pedro");
    pessoas[2].idade = 28;
    pessoas[2].altura = 1.80f;
    
    // Loop para imprimir todas as pessoas
    for (int i = 0; i < 3; i++) {
        printf("\n--- Pessoa %d ---\n", i + 1);
        imprimir_pessoa(pessoas[i]);
    }
    
    // Teste de função
    int area = calcular_area(10, 5);
    printf("\nÁrea do retângulo: %d\n", area);
    
    // Condicionais
    if (pessoa1.idade > pessoa2.idade) {
        printf("%s é mais velho que %s\n", pessoa1.nome, pessoa2.nome);
    } else {
        printf("%s é mais velho que %s\n", pessoa2.nome, pessoa1.nome);
    }
    
    return 0;
}