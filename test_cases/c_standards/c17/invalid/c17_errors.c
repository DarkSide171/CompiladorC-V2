/*
 * Teste de erros e usos incorretos específicos do C17
 * C17 é principalmente uma correção do C11, então os erros são similares
 * mas com algumas nuances específicas
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>
#include <stdnoreturn.h>
#include <stdbool.h>
#include <stdatomic.h>

// ERRO: _Static_assert sem mensagem (C17 requer mensagem)
 _Static_assert(sizeof(int) >= 4);  // Erro: falta mensagem

// ERRO: _Static_assert com expressão não constante
 int runtime_value = 10;
 _Static_assert(runtime_value > 5, "Runtime check");  // Erro: não é constante

// ERRO: _Alignas com valor inválido
 _Alignas(3) int bad_align1;  // Erro: 3 não é potência de 2
 _Alignas(-1) int bad_align2;  // Erro: valor negativo
 _Alignas(0) int bad_align3;   // Erro: zero não é válido

// ERRO: _Alignas maior que o máximo suportado
 _Alignas(65536) char huge_align[10];  // Pode causar erro dependendo do sistema

// ERRO: _Generic com tipos duplicados
#define BAD_GENERIC(x) _Generic((x), \
    int: "integer", \
    int: "duplicate int", \
    float: "float" \
)


// ERRO: _Generic sem default e tipo não coberto

#define INCOMPLETE_GENERIC(x) _Generic((x), \
    int: 1, \
    float: 2 \
)
 char c = 'x';
 int result = INCOMPLETE_GENERIC(c);  // Erro: char não coberto


// ERRO: _Noreturn em função que retorna

_Noreturn int bad_noreturn_function(void) {
    printf("This function should not return\n");
    return 42;  // Erro: função _Noreturn não deve retornar
}


// ERRO: _Noreturn em declaração de variável
// _Noreturn int bad_variable;  // Erro: _Noreturn só para funções

// ERRO: Operações atômicas em tipos não atômicos
void test_atomic_errors(void) {
    int regular_int = 10;
    
    // atomic_store(&regular_int, 20);  // Erro: regular_int não é atômico
    // int value = atomic_load(&regular_int);  // Erro: regular_int não é atômico
    
    // ERRO: Misturar operações atômicas e não-atômicas
    _Atomic int atomic_var = 0;
    // atomic_var = 10;  // Erro: deve usar atomic_store
    // int x = atomic_var;  // Erro: deve usar atomic_load
    
    // ERRO: Operações atômicas com tipos incompatíveis
    _Atomic float atomic_float = 0.0f;
    atomic_fetch_add(&atomic_float, 1.0f);  // Erro: fetch_add não suporta float
}

// ERRO: Array flexível em contexto inválido
/*
struct bad_flexible1 {
    int data[];  // Erro: array flexível deve ser o último membro
    int count;
};
*/

struct bad_flexible2 {
    int count;
    int data[];  // OK: último membro
    // int extra;  // Erro se descomentado: não pode ter membros após array flexível
};

// ERRO: Array flexível em union
/*
union bad_flexible_union {
    int count;
    int data[];  // Erro: array flexível não permitido em union
};
*/

// ERRO: Array flexível como único membro
/*
struct bad_flexible3 {
    int data[];  // Erro: struct deve ter pelo menos um membro nomeado
};
*/

// ERRO: Inicialização de array flexível
/*
struct flexible_array {
    size_t count;
    int data[];
};

struct flexible_array bad_init = {
    .count = 3,
    .data = {1, 2, 3}  // Erro: não pode inicializar array flexível
};
*/

// ERRO: sizeof em array flexível
void test_flexible_array_errors(void) {
    struct flexible_array {
        size_t count;
        int data[];
    };
    
    // size_t size = sizeof(struct flexible_array);  // OK: tamanho sem array
    // size_t bad_size = sizeof(((struct flexible_array*)0)->data);  // Erro: sizeof array flexível
}

// ERRO: _Alignof com expressão inválida
void test_alignof_errors(void) {
    // size_t align1 = _Alignof(void);  // Erro: void não tem alinhamento
    // size_t align2 = _Alignof(int[]);  // Erro: array incompleto
    
    struct incomplete_struct;
    // size_t align3 = _Alignof(struct incomplete_struct);  // Erro: tipo incompleto
    
    // ERRO: _Alignof com função
    void func(void);
    // size_t align4 = _Alignof(func);  // Erro: função não tem alinhamento
}

// ERRO: Uso incorreto de compound literals
void test_compound_literal_errors(void) {
    // ERRO: Compound literal com tipo incompleto
    // int *ptr = (int[]){}; // Erro: tamanho não especificado sem inicializadores
    
    // ERRO: Compound literal com VLA em escopo de arquivo
    // static int n = 5;
    // static int *bad_vla = (int[n]){1, 2, 3, 4, 5};  // Erro: VLA em escopo estático
    
    // ERRO: Modificar compound literal const
    const int *const_array = (const int[]){1, 2, 3, 4, 5};
    // const_array[0] = 10;  // Erro: modificar const
}

// ERRO: Designated initializers incorretos
void test_designated_initializer_errors(void) {
    // ERRO: Designador fora dos limites
    // int arr[5] = {[10] = 1};  // Erro: índice 10 fora dos limites
    
    // ERRO: Designador negativo
    // int arr2[5] = {[-1] = 1};  // Erro: índice negativo
    
    // ERRO: Designador duplicado
    // int arr3[5] = {[2] = 1, [2] = 2};  // Erro: índice 2 duplicado
    
    struct point {
        int x, y;
    };
    
    // ERRO: Designador para membro inexistente
    // struct point p = {.z = 10};  // Erro: membro 'z' não existe
    
    // ERRO: Misturar designadores e não-designadores incorretamente
    // struct point p2 = {10, .y = 20};  // Erro: mistura inválida
}

// ERRO: _Generic com associações inválidas
void test_generic_errors(void) {
    // ERRO: _Generic com tipo void
    /*
    #define BAD_GENERIC_VOID(x) _Generic((x), \
        void: "void type", \
        int: "int type" \
    )
    */
    
    // ERRO: _Generic com array
    /*
    #define BAD_GENERIC_ARRAY(x) _Generic((x), \
        int[]: "array type", \
        int: "int type" \
    )
    */
    
    // ERRO: _Generic com função
    /*
    #define BAD_GENERIC_FUNC(x) _Generic((x), \
        void(void): "function type", \
        int: "int type" \
    )
    */
}

// ERRO: Thread local storage incorreto (C17)
// _Thread_local int global_tls;  // Pode não estar disponível em todos os sistemas

void test_thread_errors(void) {
    // ERRO: _Thread_local em parâmetro de função
    // void bad_func(_Thread_local int param);  // Erro: não permitido em parâmetros
    
    // ERRO: _Thread_local com extern em diferentes unidades
    // extern _Thread_local int external_tls;
    // _Thread_local int external_tls = 10;  // Erro: definições conflitantes
}

// ERRO: Uso incorreto de _Atomic
void test_atomic_type_errors(void) {
    // ERRO: _Atomic com array
    // _Atomic int atomic_array[10];  // Erro: array de atômicos não permitido
    
    // ERRO: _Atomic com função
    // _Atomic void (*atomic_func_ptr)(void);  // Erro: ponteiro atômico para função
    
    // ERRO: _Atomic com tipo incompleto
    struct incomplete;
    // _Atomic struct incomplete atomic_incomplete;  // Erro: tipo incompleto
    
    // ERRO: _Atomic com qualificadores
    // _Atomic const int bad_atomic1;  // Erro: const com _Atomic
    // _Atomic volatile int bad_atomic2;  // Erro: volatile com _Atomic
}

// ERRO: Alinhamento inconsistente
void test_alignment_errors(void) {
    // ERRO: _Alignas menor que o alinhamento natural
    // _Alignas(1) double bad_double;  // Erro: double precisa de alinhamento >= 8
    
    // ERRO: _Alignas em bitfield
    struct {
        // _Alignas(8) int bf : 4;  // Erro: _Alignas não permitido em bitfield
        int normal_bf : 4;
    } bitfield_struct;
    
    // ERRO: _Alignas em parâmetro de função
    // void bad_func_align(_Alignas(16) int param);  // Erro: não permitido em parâmetros
}

// ERRO: Overflow em expressões constantes
void test_constant_expression_errors(void) {
    // ERRO: Overflow em _Static_assert
    // _Static_assert(INT_MAX + 1 > INT_MAX, "Overflow");  // Erro: overflow
    
    // ERRO: Divisão por zero em tempo de compilação
    // _Static_assert(10 / 0 == 0, "Division by zero");  // Erro: divisão por zero
    
    // ERRO: Shift inválido
    _Static_assert((1 << 32) > 0, "Invalid shift");  // Erro: shift >= width
}

// ERRO: Uso incorreto de restrict (C17 clarifica comportamento)
void test_restrict_errors(void) {
    // ERRO: restrict em tipo não-ponteiro
    // restrict int bad_restrict1;  // Erro: restrict só para ponteiros
    
    // ERRO: restrict em ponteiro para função
    // void (* restrict bad_func_ptr)(void);  // Erro: restrict não para ponteiros de função
    
    // ERRO: Violação de restrict (comportamento indefinido)
    /*
    void bad_restrict_usage(int * restrict p1, int * restrict p2) {
        // Se p1 e p2 apontam para o mesmo objeto, é erro
        *p1 = 10;
        *p2 = 20;  // Comportamento indefinido se p1 == p2
    }
    */
}

// ERRO: VLA em contextos inválidos (C17)
void test_vla_errors(void) {
    int n = 10;
    
    // ERRO: VLA com linkage
    // extern int bad_vla[n];  // Erro: VLA não pode ter linkage externo
    
    // ERRO: VLA estático
    // static int bad_static_vla[n];  // Erro: VLA não pode ser estático
    
    // ERRO: VLA como membro de struct
    /*
    struct bad_vla_struct {
        int count;
        int data[n];  // Erro: VLA não permitido em struct
    };
    */
    
    // ERRO: sizeof VLA em expressão não-avaliada
    // _Static_assert(sizeof(int[n]) > 0, "VLA size");  // Erro: n não é constante
}

int main(void)
{
    printf("=== C17 Error Examples ===\n");
    printf("Este arquivo contém exemplos de erros específicos do C17\n");
    printf("A maioria dos erros está comentada para permitir compilação\n");
    
    // Alguns testes que podem ser executados
    test_atomic_errors();
    test_flexible_array_errors();
    test_alignof_errors();
    test_compound_literal_errors();
    test_designated_initializer_errors();
    test_generic_errors();
    test_thread_errors();
    test_atomic_type_errors();
    test_alignment_errors();
    test_constant_expression_errors();
    test_restrict_errors();
    test_vla_errors();
    
    printf("\nTodos os testes de erro C17 concluídos\n");
    printf("Verifique os comentários para ver os erros específicos\n");
    
    return 0;
}

// ERRO: Redefinição de _Static_assert
// #define _Static_assert(expr, msg) /* nothing */  // Erro: redefinir palavra-chave

// ERRO: Uso de identificadores reservados
// int __reserved_identifier = 10;  // Erro: identificador reservado
// int _Reserved_start = 20;        // Erro: identificador reservado

// ERRO: Pragma inválida
// #pragma unknown_pragma_directive  // Erro: pragma desconhecida

// ERRO: Atributos em locais incorretos (preparação para C23)
// [[deprecated]] int bad_attr_var;  // Erro: atributo em local inválido (C17 não tem atributos padrão)