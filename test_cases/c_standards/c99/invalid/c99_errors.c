/*
 * Teste de erros específicos relacionados às funcionalidades C99
 * Este arquivo contém usos incorretos das features do C99
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// ERRO: Macro com argumentos variáveis mal formada
#define BAD_MACRO(fmt, ...) printf(fmt, __VA_ARGS__ extra_token)

// ERRO: Função inline com definição externa conflitante
inline int conflicting_function(int x);
int conflicting_function(int x) { return x; } // Conflito com inline

int main(void)
{
    // ERRO: VLA com tamanho negativo
    int negative_size = -5;
    int bad_vla[negative_size]; // Tamanho inválido
    
    // ERRO: VLA com tamanho zero
    int zero_size = 0;
    int zero_vla[zero_size]; // Tamanho inválido
    
    // ERRO: Inicialização de VLA (não permitida)
    int size = 5;
    int initialized_vla[size] = {1, 2, 3, 4, 5}; // ERRO: VLA não pode ser inicializada
    
    // ERRO: Designador inválido em inicialização
    int bad_array[5] = {[10] = 42}; // Índice fora dos limites
    
    // ERRO: Designador duplicado
    struct Point {
        int x, y;
    } duplicate_init = {.x = 1, .x = 2, .y = 3}; // x inicializado duas vezes
    
    // ERRO: Uso incorreto de restrict
    int a = 10, b = 20;
    int * restrict p1 = &a;
    int * restrict p2 = &a; // ERRO: Dois ponteiros restrict para o mesmo objeto
    
    // ERRO: Modificação de literal composto const
    const struct Point *readonly_point = &(const struct Point){.x = 1, .y = 2};
    readonly_point->x = 5; // ERRO: Modificação de const
    
    // ERRO: Array flexível em contexto inválido
    
    struct BadFlexible {
        int data[]; // ERRO: Array flexível deve ser o último membro
        int count;
    };

    
    // ERRO: Array flexível em array
    struct FlexInArray {
        int count;
        int data[];
    } flex_array[10]; // ERRO: Estrutura com array flexível em array
    
    // ERRO: Declaração de VLA em escopo de arquivo (não permitida)
     extern int file_scope_vla[size]; // Seria erro se descomentado
    
    // ERRO: Uso incorreto de complex
     double complex bad_complex = 1.0 + "invalid" * I; // Tipo inválido
    
    // ERRO: Literal hexadecimal de ponto flutuante malformado
     double bad_hex1 = 0x; // Incompleto
     double bad_hex2 = 0x1.gp3; // Dígito inválido 'g'
     double bad_hex3 = 0x1.fp; // Expoente faltando
    
    // ERRO: Bool com valor inválido direto
    bool bad_bool = 2; // Tecnicamente válido, mas não recomendado
    
    // ERRO: Uso de __func__ em escopo global
     printf("Function: %s\n", __func__); // Seria erro se no escopo global
    
    // ERRO: VLA multidimensional com dimensão inválida
    int rows = 3, cols = -2;
    int bad_matrix[rows][cols]; // Dimensão negativa
    
    // ERRO: Inicialização de array com designador e tamanho incompatível
     char bad_string[5] = {[0] = 'H', [1] = 'e', [2] = 'l', [3] = 'l', [4] = 'o', [5] = '\0'}; // Índice 5 fora dos limites
    
    // ERRO: Uso incorreto de inline em declaração local
     inline int local_inline(int x) { return x * 2; } // inline em função local
    
    return 0;
}

// ERRO: Função com VLA como parâmetro mas sem especificar dimensões
 void bad_vla_param(int matrix[][]) { // Falta especificar pelo menos uma dimensão
     // Código da função
 }

// ERRO: Função inline sem definição no mesmo arquivo
inline int undefined_inline(int x); // Declaração inline sem definição