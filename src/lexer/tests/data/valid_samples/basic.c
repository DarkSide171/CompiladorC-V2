// Exemplo básico - Estruturas fundamentais da linguagem C
// Arquivo de teste para validação do analisador léxico

#include <stdio.h>

int main(void) {
    int x = 10;
    float y = 3.14f;
    char c = 'A';
    
    if (x > 5) {
        printf("x é maior que 5\n");
    } else {
        printf("x é menor ou igual a 5\n");
    }
    
    for (int i = 0; i < 3; i++) {
        printf("Iteração %d\n", i);
    }
    
    return 0;
}