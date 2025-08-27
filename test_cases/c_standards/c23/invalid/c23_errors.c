/*
 * Teste de erros e usos incorretos específicos do C23
 * C23 introduz muitas funcionalidades novas que podem ser usadas incorretamente
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>
#include <stdnoreturn.h>
#include <stdbool.h>
#include <stdatomic.h>

// ERRO: typeof com expressões inválidas
void test_typeof_errors(void) {
    // ERRO: typeof com void
    typeof(void) bad_void;  // Erro: void não é um tipo válido para variável
    
    // ERRO: typeof com função incompleta
    typeof(undefined_function()) bad_func;  // Erro: função não declarada
    
    // ERRO: typeof com array incompleto
     extern int incomplete_array[];
     typeof(incomplete_array) bad_array;  // Erro: tipo incompleto
    
    // ERRO: typeof aninhado incorretamente
    int x = 10;
     typeof(typeof(x)) nested_type = x;  // Pode ser válido, mas confuso
    
    // ERRO: typeof com expressão que tem efeitos colaterais
    int counter = 0;
     typeof(++counter) bad_side_effect = counter;  // Perigoso: ++counter é avaliado?
}

// ERRO: _BitInt com tamanhos inválidos
void test_bitint_errors(void) {
    // ERRO: _BitInt com tamanho muito pequeno
    _BitInt(0) zero_bit;     // Erro: tamanho deve ser >= 1
    _BitInt(1) one_bit;      // Pode ser válido, mas limitado
    
    // ERRO: _BitInt com tamanho muito grande
    _BitInt(1000000) huge_bit;  // Erro: pode exceder limites do compilador
    
    // ERRO: _BitInt com tamanho não constante
    int n = 64;
     _BitInt(n) variable_size;  // Erro: tamanho deve ser constante
    
    // ERRO: _BitInt com expressão complexa
     _BitInt(sizeof(int) * 8) complex_size;  // Pode não ser suportado
    
    // ERRO: Operações inválidas com _BitInt
    _BitInt(7) small = 100;  // Erro: 100 > 63 (max para 7-bit signed)
     unsigned _BitInt(3) tiny = 10;  // Erro: 10 > 7 (max para 3-bit unsigned)
}

// ERRO: Binary literals malformados
void test_binary_literal_errors(void) {
    // ERRO: Binary literal sem dígitos
     int bad1 = 0b;  // Erro: sem dígitos após 0b
    
    // ERRO: Binary literal com dígitos inválidos
     int bad2 = 0b12345;  // Erro: 2,3,4,5 não são dígitos binários
     int bad3 = 0b101a01;  // Erro: 'a' não é dígito binário
    
    // ERRO: Binary literal com separadores incorretos
     int bad4 = 0b'1010;     // Erro: separador no início
     int bad5 = 0b1010';     // Erro: separador no final
     int bad6 = 0b10''10;    // Erro: separadores consecutivos
     int bad7 = 0b10'a'10;   // Erro: separador com dígito inválido
}

// ERRO: Digit separators incorretos
void test_digit_separator_errors(void) {
    // ERRO: Separadores em posições inválidas
     int bad1 = '123;        // Erro: separador no início
     int bad2 = 123';        // Erro: separador no final
     int bad3 = 1''23;       // Erro: separadores consecutivos
    
    // ERRO: Separadores em números de ponto flutuante
     double bad4 = 3.'14;    // Erro: separador após ponto decimal
     double bad5 = 3.1'4'5;  // Pode ser válido, mas confuso
    
    // ERRO: Separadores em expoentes
     double bad6 = 1.23e'10; // Erro: separador no expoente
     double bad7 = 1.23e1'0; // Erro: separador no expoente
}

// ERRO: Enhanced enums com erros
void test_enhanced_enum_errors(void) {
    // ERRO: Enum com tipo base inválido
     enum BadEnum : float { BAD_VALUE = 1.0 };  // Erro: float não é tipo integral
     enum BadEnum2 : void { BAD_VALUE2 };       // Erro: void não é válido
    
    // ERRO: Enum com tipo base muito pequeno
     enum TooSmall : char { LARGE_VALUE = 1000 };  // Erro: 1000 não cabe em char
    
    // ERRO: Enum com valores duplicados (pode ser válido, mas suspeito)
    enum DuplicateValues {
        VALUE1 = 10,
        VALUE2 = 10  // Suspeito: valores duplicados
    };
    
    // ERRO: Enum com overflow
     enum Overflow : unsigned char {
        MAX_UCHAR = 255,
            OVERFLOW_VALUE  // Erro: 256 não cabe em unsigned char
    // };
}

// ERRO: Attributes em locais incorretos
void test_attribute_errors(void) {
    // ERRO: Atributos em expressões
     int x = [[nodiscard]] 42;  // Erro: atributo em expressão
    
    // ERRO: Atributos duplicados
     [[deprecated]] [[deprecated]] void double_deprecated(void);  // Erro: duplicado
    
    // ERRO: Atributos com argumentos incorretos
     [[deprecated()]] void bad_deprecated1(void);  // Erro: parênteses vazios
     [[nodiscard("msg1", "msg2")]] int bad_nodiscard(void);  // Erro: muitos argumentos
    
    // ERRO: Atributos em tipos
    [[maybe_unused]] int bad_type;  // Pode ser válido, mas context-dependent
    
    // ERRO: Atributos desconhecidos (devem ser ignorados, mas podem gerar warnings)
     [[unknown_attribute]] void unknown_attr(void);
}

// ERRO: auto type deduction incorreta
void test_auto_errors(void) {
    // ERRO: auto sem inicializador
     auto uninitialized;  // Erro: auto requer inicializador
    
    // ERRO: auto com array
     auto array[] = {1, 2, 3};  // Erro: auto não pode deduzir array
    
    // ERRO: auto com função
     auto func = printf;  // Erro: auto não pode deduzir tipo de função
    
    // ERRO: auto em parâmetros de função
     void bad_func(auto param);  // Erro: auto não permitido em parâmetros
    
    // ERRO: auto em declarações de struct
    
    struct BadStruct {
        auto member;  // Erro: auto não permitido em membros de struct
    };
    
}

// ERRO: _Decimal types incorretos
void test_decimal_errors(void) {
    // ERRO: Operações inválidas com decimais
     _Decimal32 d1 = 1.23DF;
     _Decimal64 d2 = 4.56DD;
     _Decimal32 result = d1 + d2;  // Erro: mistura de tipos decimais
    
    // ERRO: Conversões implícitas perigosas
     _Decimal32 d3 = 3.14159265359;  // Erro: perda de precisão
    
    // ERRO: Literais decimais malformados
     _Decimal32 bad1 = 1.23D;   // Erro: sufixo D ambíguo
     _Decimal32 bad2 = 1.23DDD; // Erro: sufixo inválido
     _Decimal32 bad3 = 1.23F;   // Erro: sufixo float para decimal
}

// ERRO: Compound literals melhorados com erros
void test_compound_literal_errors(void) {
    // ERRO: Compound literal com tipo incompleto
    struct IncompleteStruct;
     struct IncompleteStruct *ptr = &(struct IncompleteStruct){};  // Erro: tipo incompleto
    
    // ERRO: Compound literal com VLA
    int n = 5;
    int (*vla_ptr)[n] = &(int[n]){1, 2, 3, 4, 5};  // Erro: VLA em compound literal
    
    // ERRO: Compound literal com _BitInt inválido
     _BitInt(7) *ptr = &(_BitInt(7)){200};  // Erro: 200 > max para 7-bit
    
    // ERRO: Compound literal com designated initializers incorretos
     int *arr = (int[]){[0] = 1, [100] = 2};  // Erro: array muito grande implícito
}

// ERRO: _Generic melhorado com erros
void test_improved_generic_errors(void) {
    // ERRO: _Generic com typeof em associação inválida
    
    int x = 10;
    #define BAD_GENERIC(y) _Generic((y), \
        typeof(x): "dynamic type", \
        int: "int type" \
    )
    
    
    // ERRO: _Generic com _BitInt inconsistente
    
    #define BITINT_GENERIC(x) _Generic((x), \
        _BitInt(32): "32-bit", \
        _BitInt(64): "64-bit", \
        int: "regular int" \
    )
    
    
    // ERRO: _Generic com tipos decimais misturados
    
    #define DECIMAL_GENERIC(x) _Generic((x), \
        _Decimal32: "decimal32", \
        _Decimal64: "decimal64", \
        float: "binary float" \
    )
    
}

// ERRO: Preprocessor com C23 features
void test_preprocessor_c23_errors(void) {
    // ERRO: #embed com arquivo inexistente
     #embed "nonexistent_file.bin"  // Erro: arquivo não encontrado
    
    // ERRO: #embed com parâmetros inválidos
     #embed "file.bin" limit(-1)    // Erro: limite negativo
     #embed "file.bin" offset(-5)   // Erro: offset negativo
    
    // ERRO: __VA_OPT__ fora de macro variádica
    
    #define BAD_VA_OPT(x) x __VA_OPT__(,) y  // Erro: __VA_OPT__ sem __VA_ARGS__
    
    
    // ERRO: __VA_OPT__ aninhado incorretamente
    
    #define NESTED_VA_OPT(...) __VA_OPT__(__VA_OPT__(,))  // Erro: aninhamento inválido
    
}

// ERRO: Thread local storage C23
void test_thread_local_c23_errors(void) {
    // ERRO: _Thread_local com _BitInt muito grande
     _Thread_local _BitInt(1024) huge_tls;  // Erro: pode ser muito grande para TLS
    
    // ERRO: _Thread_local com inicializador não constante
    int runtime_value = 42;
     _Thread_local int bad_tls = runtime_value;  // Erro: inicializador não constante
    
    // ERRO: _Thread_local com array flexível
    
    struct FlexStruct {
        int count;
        int data[];
    };
    _Thread_local struct FlexStruct bad_flex_tls;  // Erro: array flexível em TLS

}

// ERRO: Alignment com C23 features
void test_alignment_c23_errors(void) {
    // ERRO: _Alignas com _BitInt
     _Alignas(_BitInt(128)) char bad_bitint_align[10];  // Erro: _BitInt não é valor de alinhamento
    
    // ERRO: _Alignas com typeof de expressão complexa
    int arr[10];
     _Alignas(typeof(arr[0] + arr[1])) int bad_typeof_align;  // Erro: expressão complexa
    
    // ERRO: Over-alignment extremo
     _Alignas(1048576) char extreme_align[1];  // Erro: alinhamento excessivo
}

// ERRO: Constexpr-like errors (preparação para futuras versões)
void test_constexpr_like_errors(void) {
    // ERRO: Tentativa de usar constexpr (não existe em C23)
     constexpr int bad_constexpr = 42;  // Erro: constexpr não existe em C
    
    // ERRO: _Static_assert com expressões C23 inválidas
     _Static_assert(typeof(int) == int, "Type comparison");  // Erro: typeof em _Static_assert
    
    // ERRO: Constant evaluation com _BitInt overflow
     _Static_assert((_BitInt(4))15 + 1 == 0, "BitInt overflow");  // Erro: overflow em tempo de compilação
}

// ERRO: Mixing C23 features incorrectly
void test_mixed_c23_errors(void) {
    // ERRO: typeof com _BitInt e auto
     auto x = (_BitInt(33))42;
     typeof(x) y = x;  // Pode ser válido, mas complexo
    
    // ERRO: Binary literals com _BitInt incompatível
     _BitInt(4) small = 0b11111;  // Erro: 0b11111 (31) > max para 4-bit signed (7)
    
    // ERRO: Attributes com typeof
     [[nodiscard]] typeof(int) bad_attr_typeof(void);  // Erro: sintaxe inválida
    
    // ERRO: Enhanced enum com _BitInt
    
    enum BitIntEnum : _BitInt(7) {
        BITINT_VALUE = 100  // Erro: 100 > max para _BitInt(7)
    };
    
}

int main(void)
{
    printf("=== C23 Error Examples ===\n");
    printf("Este arquivo contém exemplos de erros específicos do C23\n");
    printf("A maioria dos erros está comentada para permitir compilação\n");
    
    // Alguns testes que podem ser executados
    test_typeof_errors();
    test_bitint_errors();
    test_binary_literal_errors();
    test_digit_separator_errors();
    test_enhanced_enum_errors();
    test_attribute_errors();
    test_auto_errors();
    test_decimal_errors();
    test_compound_literal_errors();
    test_improved_generic_errors();
    test_preprocessor_c23_errors();
    test_thread_local_c23_errors();
    test_alignment_c23_errors();
    test_constexpr_like_errors();
    test_mixed_c23_errors();
    
    printf("\nTodos os testes de erro C23 concluídos\n");
    printf("Verifique os comentários para ver os erros específicos\n");
    printf("Muitas funcionalidades C23 ainda não estão amplamente suportadas\n");
    
    return 0;
}

// ERRO: Redefinição de palavras-chave C23
 #define typeof int  // Erro: redefinir palavra-chave
 #define _BitInt long  // Erro: redefinir palavra-chave

// ERRO: Uso de identificadores reservados C23
 int _BitInt_reserved = 10;  // Erro: identificador reservado
 int __typeof_reserved = 20;  // Erro: identificador reservado

// ERRO: Pragma C23 inválidas
 #pragma STDC FENV_ACCESS OFF  // Pode ser válido, mas context-dependent
 #pragma STDC FP_CONTRACT DEFAULT  // Pode ser válido, mas context-dependent

// ERRO: Atributos C23 em contextos pré-C23
 #if __STDC_VERSION__ < 202311L
 [[deprecated]] void c23_only_attr(void);  // Erro: atributos não suportados
 #endif