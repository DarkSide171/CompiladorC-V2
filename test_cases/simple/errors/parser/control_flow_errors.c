/*
 * Teste de erros do parser - estruturas de controle malformadas
 * Este arquivo contém erros em if, for, while, switch, etc.
 */

#include <stdio.h>

int main() {
    int x = 10;
    int y = 20;
    
    // If sem condição
    if {
        printf("Sem condição\n");
    }
    
    // If com condição malformada
    if (x = 5) {  // Atribuição ao invés de comparação
        printf("Atribuição\n");
    }
    
    // Else sem if
    else {
        printf("Else órfão\n");
    }
    
    // For com partes ausentes
    for (;;) {  // Tecnicamente válido, mas pode causar problemas
        break;
    }
    
    // For com sintaxe incorreta
    for (int i = 0 i < 10; i++) {  // Falta ponto e vírgula
        printf("%d\n", i);
    }
    
    // While com múltiplas condições sem operador
    while (x y) {  // Falta operador entre x e y
        x--;
    }
    
    // Do-while malformado
    do {
        printf("Do-while\n");
    } while  // Falta parênteses e condição
    
    // Switch com casos malformados
    switch (x) {
        case:  // Case sem valor
            printf("Case vazio\n");
            break;
        case 1 2:  // Múltiplos valores sem vírgula
            printf("Case múltiplo\n");
            break;
        default default:  // Múltiplos defaults
            printf("Default duplo\n");
    }
    
    // Break fora de loop
    break;
    
    // Continue fora de loop
    continue;
    
    // Return com tipo incorreto
    return "string";  // main deve retornar int
}

// Função sem return quando deveria ter
int funcao_sem_return() {
    int x = 5;
    // Falta return
}