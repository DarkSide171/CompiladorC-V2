/*
 * Teste de violações específicas do padrão C89
 * Este arquivo contém construções que NÃO são válidas em C89
 */

#include <stdio.h>

/* ERRO: Comentários de linha única não existiam em C89 */
// Este tipo de comentário não é válido em C89

int main(void)
{
    /* ERRO: Declaração de variável no meio do código */
    int a = 10;
    printf("Value: %d\n", a);
    
    int b = 20; /* ERRO: Declaração após statement */
    
    /* ERRO: Inicialização de array com designadores */
    int arr[5] = {[0] = 1, [2] = 3, [4] = 5};
    
    /* ERRO: Declaração de variável no for loop */
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    
    /* ERRO: Literal de string longa sem concatenação */
    char *long_string = "This is a very long string that exceeds the typical \
line length limits and should be broken into multiple parts in C89";
    
    /* ERRO: Uso de const em contexto não suportado */
    const int readonly = 100;
    
    /* ERRO: Função inline (não existe em C89) */
    inline int square(int x) { return x * x; }
    
    /* ERRO: Array de tamanho variável */
    int size = 10;
    int vla[size]; /* Variable Length Array não existe em C89 */
    
    /* ERRO: Inicialização de estrutura com designadores */
    struct Point {
        int x, y;
    } p = {.x = 1, .y = 2};
    
    /* ERRO: Declaração de função com parâmetros mistos */
    int mixed_params(int a, b, c) int b; { return a + b + c; }
    
    /* ERRO: Uso de restrict (C99 feature) */
    void process_arrays(int * restrict a, int * restrict b); 
    
    /* ERRO: Literal hexadecimal de ponto flutuante */
    double hex_float = 0x1.fp3; /* Não existe em C89 */
    
    /* ERRO: Macro com argumentos variáveis */
    #define DEBUG_PRINT(fmt, ...) printf(fmt, __VA_ARGS__)
    
    return 0;
}

/* ERRO: Função com parâmetros void implícito */
int invalid_function() /* Deveria ser int invalid_function(void) */
{
    return 42;
}