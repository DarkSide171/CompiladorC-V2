// Teste de estruturas de controle
#include <stdio.h>

#define MAX_COUNT 10

int main() {
    int i, count = 0;
    
    // Teste de loop for
    for (i = 0; i < MAX_COUNT; i++) {
        if (i % 2 == 0) {
            count++;
            printf("Par: %d\n", i);
        } else {
            printf("Ímpar: %d\n", i);
        }
    }
    
    // Teste de while
    int j = 0;
    while (j < 3) {
        printf("While: %d\n", j);
        j++;
    }
    
    // Teste de switch
    switch (count) {
        case 0:
            printf("Nenhum número par\n");
            break;
        case 1:
            printf("Um número par\n");
            break;
        default:
            printf("Múltiplos números pares: %d\n", count);
            break;
    }
    
    return 0;
}