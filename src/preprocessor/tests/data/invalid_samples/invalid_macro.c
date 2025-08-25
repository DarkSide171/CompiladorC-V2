// Exemplo de macro inválida
// TODO: Usar nos testes da Fase 2.1

// Macro com nome inválido (começa com número)
// #define 123INVALID value

// Macro com parênteses não balanceados
// #define UNBALANCED(x ((x) + 1)

// Redefinição de macro sem #undef
#define DUPLICATE 1
// #define DUPLICATE 2

// Macro recursiva
// #define RECURSIVE RECURSIVE

int main() {
    return 0;
}