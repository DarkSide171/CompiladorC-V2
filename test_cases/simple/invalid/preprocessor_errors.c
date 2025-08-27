// Arquivo de teste com erros de pré-processador

#include "arquivo_inexistente.h"  // Erro: arquivo não encontrado

#define MACRO_RECURSIVA MACRO_RECURSIVA  // Erro: macro recursiva

#if UNDEFINED_MACRO == 1  // Aviso: macro não definida
    int x = 1;
#endif

#error "Este é um erro forçado pelo pré-processador"

int main() {
    return 0;
}