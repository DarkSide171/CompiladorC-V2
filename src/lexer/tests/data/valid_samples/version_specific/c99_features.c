// Exemplo C99 - Recursos específicos do C99
// Arquivo de teste para validação de recursos do C99

#include <stdio.h>
#include <stdbool.h>  // C99: tipo bool
#include <stdint.h>   // C99: tipos de tamanho fixo

// C99: Função inline
inline int square(int x) {
    return x * x;
}

// C99: Declarações mistas com código
int main(void) {
    // C99: Inicialização de arrays designada
    int arr[10] = {[0] = 1, [9] = 2};
    
    // C99: Declaração de variável no meio do código
    printf("Testando recursos C99\n");
    
    for (int i = 0; i < 10; i++) {  // C99: declaração no for
        if (i == 5) {
            // C99: Declaração no meio do bloco
            bool found = true;
            if (found) {
                printf("Encontrado na posição %d\n", i);
            }
        }
    }
    
    // C99: Comentários de linha única
    int result = square(5); // Este é um comentário C99
    
    // C99: Literais hexadecimais de ponto flutuante
    double hex_float = 0x1.fp3;  // 15.5 em decimal
    
    // C99: Tipos de tamanho fixo
    uint32_t fixed_size = 0xDEADBEEF;
    
    // C99: Inicialização de struct designada
    struct Point {
        int x, y;
    } p = {.x = 10, .y = 20};
    
    // C99: Arrays de tamanho variável (VLA)
    int n = 5;
    int vla[n];
    
    for (int j = 0; j < n; j++) {
        vla[j] = j * j;
    }
    
    return 0;
}