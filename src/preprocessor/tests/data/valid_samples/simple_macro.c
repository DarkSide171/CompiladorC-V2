// Exemplo simples de macro
// TODO: Usar nos testes da Fase 2.1

#define MAX_SIZE 100
#define PI 3.14159
#define SQUARE(x) ((x) * (x))

int main() {
    int array[MAX_SIZE];
    double area = PI * SQUARE(5);
    return 0;
}