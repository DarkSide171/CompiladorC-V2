// Exemplo de compilação condicional
// TODO: Usar nos testes da Fase 3.1

#include <stdio.h>

#define DEBUG 1
#define VERSION 2

#if DEBUG
    #define LOG(msg) printf("DEBUG: %s\n", msg)
#else
    #define LOG(msg)
#endif

#ifdef VERSION
    #if VERSION >= 2
        #define FEATURE_ENABLED 1
    #else
        #define FEATURE_ENABLED 0
    #endif
#endif

#ifndef MAX_BUFFER
    #define MAX_BUFFER 256
#endif

int main() {
    LOG("Starting program");
    
    #if FEATURE_ENABLED
        printf("New feature is enabled\n");
    #endif
    
    return 0;
}