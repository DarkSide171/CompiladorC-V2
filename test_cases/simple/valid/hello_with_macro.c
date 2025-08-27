#include <stdio.h>

#define MESSAGE "Hello from preprocessor!"
#define MAX_VALUE 100

int main() {
    int value = MAX_VALUE;
    printf(MESSAGE);
    printf("\nValue: %d\n", value);
    return 0;
}