/*
 * Teste de múltiplas declarações de variáveis
 * Demonstra diferentes tipos de declarações e inicializações
 */

#include <stdio.h>

int main() {
    // Declarações múltiplas de inteiros
    int a, b, c;
    int x = 10, y = 20, z = 30;
    
    // Declarações de diferentes tipos
    float pi = 3.14159f;
    double e = 2.71828;
    char letra = 'A';
    
    // Array simples
    int numeros[5] = {1, 2, 3, 4, 5};
    
    // Atribuições
    a = 5;
    b = a + 10;
    c = b * 2;
    
    // Loop for simples
    for (int i = 0; i < 5; i++) {
        printf("numeros[%d] = %d\n", i, numeros[i]);
    }
    
    // Loop while
    int contador = 0;
    while (contador < 3) {
        printf("Contador: %d\n", contador);
        contador++;
    }
    
    // Operações aritméticas
    int resultado = (x + y) * z / 2;
    printf("Resultado: %d\n", resultado);
    
    return 0;
}