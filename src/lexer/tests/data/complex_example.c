// Exemplo complexo - Estruturas, ponteiros e funções
// Programa mais elaborado para teste do analisador léxico

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[50];
    float salary;
} Employee;

Employee* createEmployee(int id, const char* name, float salary) {
    Employee* emp = (Employee*)malloc(sizeof(Employee));
    if (emp != NULL) {
        emp->id = id;
        strncpy(emp->name, name, sizeof(emp->name) - 1);
        emp->name[sizeof(emp->name) - 1] = '\0';
        emp->salary = salary;
    }
    return emp;
}

void printEmployee(const Employee* emp) {
    if (emp) {
        printf("ID: %d, Name: %s, Salary: %.2f\n", 
               emp->id, emp->name, emp->salary);
    }
}

int main() {
    Employee* employees[3];
    
    employees[0] = createEmployee(1, "John Doe", 50000.0f);
    employees[1] = createEmployee(2, "Jane Smith", 60000.0f);
    employees[2] = createEmployee(3, "Bob Johnson", 55000.0f);
    
    for (int i = 0; i < 3; i++) {
        printEmployee(employees[i]);
        free(employees[i]);
    }
    
    return 0;
}