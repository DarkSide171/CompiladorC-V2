#include <stdio.h>
#include <nonexistent.h>
#define TEST_MACRO 42

int main() {
    printf("Test: %d\n", TEST_MACRO);
    return 0;
}