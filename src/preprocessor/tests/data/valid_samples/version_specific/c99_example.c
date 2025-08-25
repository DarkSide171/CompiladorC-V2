// Exemplo específico do C99
// TODO: Usar nos testes da Fase 1.3

#include <stdio.h>

// Macro variádica (C99)
#define DEBUG_PRINT(fmt, ...) printf("DEBUG: " fmt "\n", ##__VA_ARGS__)

// Pragma (C99)
#pragma once

int main() {
    // Comentário de linha única (C99)
    DEBUG_PRINT("Testing C99 features");
    DEBUG_PRINT("Value: %d", 42);
    
    return 0;
}