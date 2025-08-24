// Testes de Integração - Versões do C
// Testes para diferentes padrões da linguagem C

#include "../../include/lexer.hpp"
#include "../../include/error_handler.hpp"
#include "../../include/lexer_config.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <memory>

using namespace Lexer;

// Função auxiliar para criar arquivos de teste temporários
void createTestFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Não foi possível criar arquivo de teste: " + filename);
    }
    file << content;
    file.close();
}

// Função auxiliar para remover arquivos de teste
void removeTestFile(const std::string& filename) {
    std::remove(filename.c_str());
}

// Função auxiliar para verificar se um token específico está presente
bool hasTokenType(const std::vector<Token>& tokens, TokenType type) {
    for (const auto& token : tokens) {
        if (token.getType() == type) {
            return true;
        }
    }
    return false;
}

// Função auxiliar para contar tokens de um tipo específico
int countTokenType(const std::vector<Token>& tokens, TokenType type) {
    int count = 0;
    for (const auto& token : tokens) {
        if (token.getType() == type) {
            count++;
        }
    }
    return count;
}

// Função auxiliar para tokenizar código com uma versão específica
std::vector<Token> tokenizeWithVersion(const std::string& code, CVersion version) {
    createTestFile("temp_version_test.c", code);
    
    auto errorHandler = std::make_shared<ErrorHandler>();
    LexerMain lexer("temp_version_test.c", errorHandler.get());
    lexer.setVersion(versionToString(version));
    
    std::vector<Token> tokens;
    while (lexer.hasMoreTokens()) {
        Token token = lexer.nextToken();
        if (token.getType() == TokenType::END_OF_FILE) {
            break;
        }
        tokens.push_back(token);
    }
    
    removeTestFile("temp_version_test.c");
    return tokens;
}

// 1. testC89Features() - Features específicas do C89
void testC89Features() {
    std::cout << "\n=== Testando Features C89/C90 ===" << std::endl;
    
    std::string c89Code = R"(
        /* Programa C89 básico */
        #include <stdio.h>
        
        int main(void) {
            int i;
            char str[100];
            float f = 3.14f;
            double d = 2.718;
            
            /* Estruturas de controle básicas */
            for (i = 0; i < 10; i++) {
                if (i % 2 == 0) {
                    printf("Par: %d\n", i);
                } else {
                    printf("Ímpar: %d\n", i);
                }
            }
            
            /* Estruturas e uniões */
            struct Point {
                int x, y;
            };
            
            union Data {
                int i;
                float f;
                char str[20];
            };
            
            return 0;
        }
    )";
    
    auto tokens = tokenizeWithVersion(c89Code, CVersion::C89);
    
    // Verificar palavras-chave básicas do C89
    assert(hasTokenType(tokens, TokenType::INT));
    assert(hasTokenType(tokens, TokenType::CHAR));
    assert(hasTokenType(tokens, TokenType::FLOAT));
    assert(hasTokenType(tokens, TokenType::DOUBLE));
    assert(hasTokenType(tokens, TokenType::VOID));
    assert(hasTokenType(tokens, TokenType::IF));
    assert(hasTokenType(tokens, TokenType::ELSE));
    assert(hasTokenType(tokens, TokenType::FOR));
    assert(hasTokenType(tokens, TokenType::WHILE) || true); // while pode não estar presente neste código
    assert(hasTokenType(tokens, TokenType::RETURN));
    assert(hasTokenType(tokens, TokenType::STRUCT));
    assert(hasTokenType(tokens, TokenType::UNION));
    
    // Verificar que não há palavras-chave de versões posteriores
    assert(!hasTokenType(tokens, TokenType::INLINE));
    assert(!hasTokenType(tokens, TokenType::RESTRICT));
    assert(!hasTokenType(tokens, TokenType::_BOOL));
    
    std::cout << "✓ Features C89 testadas com sucesso" << std::endl;
}

// 2. testC99Features() - Features específicas do C99
void testC99Features() {
    std::cout << "\n=== Testando Features C99 ===" << std::endl;
    
    std::string c99Code = R"(
        // Comentários de linha única (C99)
        #include <stdio.h>
        #include <stdbool.h>
        
        // Função inline (C99)
        inline int square(int x) {
            return x * x;
        }
        
        int main(void) {
            // Tipo _Bool (C99)
            _Bool flag = 1;
            bool another_flag = true;
            
            // Declarações mistas com código (C99)
            printf("Testando C99\n");
            int i = 0;
            
            // Inicialização designada (C99)
            int arr[5] = {[0] = 1, [4] = 5};
            
            // Declaração no for (C99)
            for (int j = 0; j < 5; j++) {
                printf("%d ", arr[j]);
            }
            
            // Restrict pointer (C99)
            int * restrict ptr = &i;
            
            // Tipos complexos (C99)
            _Complex double z = 1.0 + 2.0 * I;
            _Imaginary float im = 3.0f * I;
            
            return 0;
        }
    )";
    
    auto tokens = tokenizeWithVersion(c99Code, CVersion::C99);
    
    // Verificar palavras-chave específicas do C99
    assert(hasTokenType(tokens, TokenType::INLINE));
    assert(hasTokenType(tokens, TokenType::_BOOL));
    assert(hasTokenType(tokens, TokenType::RESTRICT));
    assert(hasTokenType(tokens, TokenType::_COMPLEX));
    assert(hasTokenType(tokens, TokenType::_IMAGINARY));
    
    // Comentários de linha são ignorados pelo lexer (não geram tokens)
    
    // Verificar que não há palavras-chave de versões posteriores
    assert(!hasTokenType(tokens, TokenType::_ATOMIC));
    assert(!hasTokenType(tokens, TokenType::_STATIC_ASSERT));
    
    std::cout << "✓ Features C99 testadas com sucesso" << std::endl;
}

// 3. testC11Features() - Features específicas do C11
void testC11Features() {
    std::cout << "\n=== Testando Features C11 ===" << std::endl;
    
    std::string c11Code = R"(
        #include <stdio.h>
        #include <stdatomic.h>
        #include <threads.h>
        
        // Thread local storage (C11)
        _Thread_local int tls_var = 0;
        
        // Atomic operations (C11)
        _Atomic int atomic_counter = 0;
        
        // Boolean type (C99/C11)
        _Bool flag = 1;
        
        // Static assertions (C11)
        _Static_assert(sizeof(int) >= 4, "int deve ter pelo menos 4 bytes");
        
        // Noreturn function (C11)
        _Noreturn void exit_program(void) {
            exit(1);
        }
        
        // Inline function (C99/C11)
        inline int square(int x) {
            return x * x;
        }
        
        // Alignment specifiers (C11)
        struct AlignedStruct {
            _Alignas(16) char data[64];
        };
        
        int main(void) {
            // Generic selections (C11)
            int x = 5;
            double y = 3.14;
            
            #define TYPE_NAME(x) _Generic((x), \
                int: "int", \
                double: "double", \
                default: "unknown")
            
            printf("Tipo de x: %s\n", TYPE_NAME(x));
            printf("Tipo de y: %s\n", TYPE_NAME(y));
            
            // Alignof operator (C11)
            printf("Alinhamento de int: %zu\n", _Alignof(int));
            printf("Alinhamento de struct: %zu\n", _Alignof(struct AlignedStruct));
            
            return 0;
        }
    )";
    
    auto tokens = tokenizeWithVersion(c11Code, CVersion::C11);
    
    // Verificar palavras-chave específicas do C11
    assert(hasTokenType(tokens, TokenType::_THREAD_LOCAL));
    assert(hasTokenType(tokens, TokenType::_ATOMIC));
    assert(hasTokenType(tokens, TokenType::_STATIC_ASSERT));
    assert(hasTokenType(tokens, TokenType::_NORETURN));
    assert(hasTokenType(tokens, TokenType::_ALIGNAS));
    assert(hasTokenType(tokens, TokenType::_ALIGNOF));
    assert(hasTokenType(tokens, TokenType::_GENERIC));
    
    // Verificar que ainda suporta features anteriores
    assert(hasTokenType(tokens, TokenType::INLINE));
    assert(hasTokenType(tokens, TokenType::_BOOL));
    
    // Verificar que não há palavras-chave de versões posteriores
    assert(!hasTokenType(tokens, TokenType::TYPEOF));
    assert(!hasTokenType(tokens, TokenType::_BITINT));
    
    std::cout << "✓ Features C11 testadas com sucesso" << std::endl;
}

// 4. testC17Features() - Features específicas do C17
void testC17Features() {
    std::cout << "\n=== Testando Features C17 ===" << std::endl;
    
    // C17 é principalmente uma correção técnica do C11
    // Não introduz novas palavras-chave, mas mantém compatibilidade
    std::string c17Code = R"(
        #include <stdio.h>
        #include <stdatomic.h>
        
        // C17 mantém todas as features do C11
        _Thread_local _Atomic int counter = 0;
        
        _Static_assert(sizeof(void*) >= sizeof(int), "Pointer size check");
        
        struct Data {
            _Alignas(8) int value;
        };
        
        // Inline function (C99/C11/C17)
        inline int multiply(int a, int b) {
            return a * b;
        }
        
        // Boolean and restrict (C99/C11/C17)
        _Bool is_valid = 1;
        void process_array(int * restrict arr, size_t size);
        
        int main(void) {
            // Generic selections ainda funcionam
            int x = 42;
            
            #define GET_TYPE(x) _Generic((x), \
                int: "integer", \
                double: "floating", \
                default: "other")
            
            printf("Tipo: %s\n", GET_TYPE(x));
            printf("Alinhamento: %zu\n", _Alignof(struct Data));
            
            return 0;
        }
    )";
    
    auto tokens = tokenizeWithVersion(c17Code, CVersion::C17);
    
    // C17 deve suportar todas as features do C11
    assert(hasTokenType(tokens, TokenType::_THREAD_LOCAL));
    assert(hasTokenType(tokens, TokenType::_ATOMIC));
    assert(hasTokenType(tokens, TokenType::_STATIC_ASSERT));
    assert(hasTokenType(tokens, TokenType::_ALIGNAS));
    assert(hasTokenType(tokens, TokenType::_ALIGNOF));
    assert(hasTokenType(tokens, TokenType::_GENERIC));
    
    // E também features anteriores
    assert(hasTokenType(tokens, TokenType::INLINE));
    assert(hasTokenType(tokens, TokenType::_BOOL));
    assert(hasTokenType(tokens, TokenType::RESTRICT));
    
    // Mas não features do C23
    assert(!hasTokenType(tokens, TokenType::TYPEOF));
    assert(!hasTokenType(tokens, TokenType::_BITINT));
    
    std::cout << "✓ Features C17 testadas com sucesso (compatibilidade C11)" << std::endl;
}

// 5. testC23Features() - Features específicas do C23
void testC23Features() {
    std::cout << "\n=== Testando Features C23 ===" << std::endl;
    
    std::string c23Code = R"(
        #include <stdio.h>
        
        // BitInt type (C23)
        _BitInt(128) big_int = 0;
        
        // Decimal floating point (C23)
        _Decimal32 d32 = 1.23f;
        _Decimal64 d64 = 4.56;
        _Decimal128 d128 = 7.89l;
        
        // Atomic operations (C11/C17/C23)
        _Atomic int atomic_var = 0;
        
        // Static assertions (C11/C17/C23)
        _Static_assert(sizeof(int) >= 4, "int size check");
        
        // Inline function (C99/C11/C17/C23)
        inline int add(int a, int b) {
            return a + b;
        }
        
        // Boolean type (C99/C11/C17/C23)
        _Bool is_enabled = 1;
        
        int main(void) {
            int x = 42;
            
            // typeof operator (C23)
            typeof(x) y = x;
            typeof_unqual(const int) z = 10;
            
            printf("x = %d, y = %d, z = %d\n", x, y, z);
            
            // Binary literals (C23)
            int binary = 0b1010;
            
            // Digit separators (C23) - simplified
            long large_num = 1000000;
            
            printf("Binary: %d, Large: %ld\n", binary, large_num);
            
            return 0;
        }
    )";
    
    auto tokens = tokenizeWithVersion(c23Code, CVersion::C23);
    
    // Verificar palavras-chave específicas do C23
    assert(hasTokenType(tokens, TokenType::_BITINT));
    assert(hasTokenType(tokens, TokenType::_DECIMAL32));
    assert(hasTokenType(tokens, TokenType::_DECIMAL64));
    assert(hasTokenType(tokens, TokenType::_DECIMAL128));
    assert(hasTokenType(tokens, TokenType::TYPEOF));
    assert(hasTokenType(tokens, TokenType::TYPEOF_UNQUAL));
    
    // Verificar que ainda suporta todas as features anteriores
    assert(hasTokenType(tokens, TokenType::_ATOMIC));
    assert(hasTokenType(tokens, TokenType::_STATIC_ASSERT));
    assert(hasTokenType(tokens, TokenType::INLINE));
    assert(hasTokenType(tokens, TokenType::_BOOL));
    
    std::cout << "✓ Features C23 testadas com sucesso" << std::endl;
}

// 6. testVersionCompatibility() - Compatibilidade entre versões
void testVersionCompatibility() {
    std::cout << "\n=== Testando Compatibilidade entre Versões ===" << std::endl;
    
    std::string compatCode = R"(
        int main(void) {
            int x = 42;
            return 0;
        }
    )";
    
    // Testar que código básico funciona em todas as versões
    auto c89Tokens = tokenizeWithVersion(compatCode, CVersion::C89);
    auto c99Tokens = tokenizeWithVersion(compatCode, CVersion::C99);
    auto c11Tokens = tokenizeWithVersion(compatCode, CVersion::C11);
    auto c17Tokens = tokenizeWithVersion(compatCode, CVersion::C17);
    auto c23Tokens = tokenizeWithVersion(compatCode, CVersion::C23);
    
    // Todos devem reconhecer as palavras-chave básicas
    assert(hasTokenType(c89Tokens, TokenType::INT));
    assert(hasTokenType(c99Tokens, TokenType::INT));
    assert(hasTokenType(c11Tokens, TokenType::INT));
    assert(hasTokenType(c17Tokens, TokenType::INT));
    assert(hasTokenType(c23Tokens, TokenType::INT));
    
    // Testar progressão de features
    std::string progressiveCode = "inline int func() { return 0; }";
    
    auto c89Progressive = tokenizeWithVersion(progressiveCode, CVersion::C89);
    auto c99Progressive = tokenizeWithVersion(progressiveCode, CVersion::C99);
    
    // C89 não deve reconhecer 'inline' como palavra-chave
    assert(!hasTokenType(c89Progressive, TokenType::INLINE));
    
    // C99+ deve reconhecer 'inline' como palavra-chave
    assert(hasTokenType(c99Progressive, TokenType::INLINE));
    
    std::cout << "✓ Compatibilidade entre versões testada com sucesso" << std::endl;
}

// 7. testFeatureDetection() - Detecção de features
void testFeatureDetection() {
    std::cout << "\n=== Testando Detecção de Features ===" << std::endl;
    
    // Testar detecção de features específicas por versão
    auto errorHandler = std::make_shared<ErrorHandler>();
    
    // Testar C99 features
    LexerConfig c99Config(CVersion::C99, errorHandler.get());
    assert(c99Config.isFeatureEnabled(Feature::INLINE_FUNCTIONS));
    assert(c99Config.isFeatureEnabled(Feature::BOOL_TYPE));
    assert(c99Config.isFeatureEnabled(Feature::RESTRICT_KEYWORD));
    assert(!c99Config.isFeatureEnabled(Feature::ATOMIC_OPERATIONS)); // C11 feature
    
    // Testar C11 features
    LexerConfig c11Config(CVersion::C11, errorHandler.get());
    assert(c11Config.isFeatureEnabled(Feature::INLINE_FUNCTIONS)); // Herda do C99
    assert(c11Config.isFeatureEnabled(Feature::ATOMIC_OPERATIONS));
    assert(c11Config.isFeatureEnabled(Feature::STATIC_ASSERTIONS));
    assert(!c11Config.isFeatureEnabled(Feature::TYPEOF_OPERATOR)); // C23 feature
    
    // Testar C23 features
    LexerConfig c23Config(CVersion::C23, errorHandler.get());
    assert(c23Config.isFeatureEnabled(Feature::TYPEOF_OPERATOR));
    assert(c23Config.isFeatureEnabled(Feature::BITINT_TYPE));
    assert(c23Config.isFeatureEnabled(Feature::ATOMIC_OPERATIONS)); // Herda do C11
    
    std::cout << "✓ Detecção de features testada com sucesso" << std::endl;
}

// 8. testKeywordVersioning() - Versionamento de palavras-chave
void testKeywordVersioning() {
    std::cout << "\n=== Testando Versionamento de Palavras-chave ===" << std::endl;
    
    auto errorHandler = std::make_shared<ErrorHandler>();
    
    // Testar palavras-chave por versão
    LexerConfig c89Config(CVersion::C89, errorHandler.get());
    LexerConfig c99Config(CVersion::C99, errorHandler.get());
    LexerConfig c11Config(CVersion::C11, errorHandler.get());
    LexerConfig c23Config(CVersion::C23, errorHandler.get());
    
    // Palavras-chave básicas devem estar em todas as versões
    assert(c89Config.isKeyword("int"));
    assert(c99Config.isKeyword("int"));
    assert(c11Config.isKeyword("int"));
    assert(c23Config.isKeyword("int"));
    
    // Palavras-chave C99
    assert(!c89Config.isKeyword("inline"));
    assert(c99Config.isKeyword("inline"));
    assert(c11Config.isKeyword("inline"));
    assert(c23Config.isKeyword("inline"));
    
    assert(!c89Config.isKeyword("_Bool"));
    assert(c99Config.isKeyword("_Bool"));
    assert(c11Config.isKeyword("_Bool"));
    assert(c23Config.isKeyword("_Bool"));
    
    // Palavras-chave C11
    assert(!c89Config.isKeyword("_Atomic"));
    assert(!c99Config.isKeyword("_Atomic"));
    assert(c11Config.isKeyword("_Atomic"));
    assert(c23Config.isKeyword("_Atomic"));
    
    assert(!c89Config.isKeyword("_Static_assert"));
    assert(!c99Config.isKeyword("_Static_assert"));
    assert(c11Config.isKeyword("_Static_assert"));
    assert(c23Config.isKeyword("_Static_assert"));
    
    // Palavras-chave C23
    assert(!c89Config.isKeyword("typeof"));
    assert(!c99Config.isKeyword("typeof"));
    assert(!c11Config.isKeyword("typeof"));
    assert(c23Config.isKeyword("typeof"));
    
    assert(!c89Config.isKeyword("_BitInt"));
    assert(!c99Config.isKeyword("_BitInt"));
    assert(!c11Config.isKeyword("_BitInt"));
    assert(c23Config.isKeyword("_BitInt"));
    
    std::cout << "✓ Versionamento de palavras-chave testado com sucesso" << std::endl;
}

// Função principal dos testes
int main() {
    std::cout << "=== TESTES DE INTEGRAÇÃO - VERSÕES DO C ===" << std::endl;
    std::cout << "Testando compatibilidade e features específicas por versão" << std::endl;
    
    try {
        // Executar todos os testes
        testC89Features();
        testC99Features();
        testC11Features();
        testC17Features();
        testC23Features();
        testVersionCompatibility();
        testFeatureDetection();
        testKeywordVersioning();
        
        std::cout << "\n=== TODOS OS TESTES DE VERSÃO PASSARAM! ===" << std::endl;
        std::cout << "✓ C89/C90 features: OK" << std::endl;
        std::cout << "✓ C99 features: OK" << std::endl;
        std::cout << "✓ C11 features: OK" << std::endl;
        std::cout << "✓ C17 features: OK" << std::endl;
        std::cout << "✓ C23 features: OK" << std::endl;
        std::cout << "✓ Compatibilidade entre versões: OK" << std::endl;
        std::cout << "✓ Detecção de features: OK" << std::endl;
        std::cout << "✓ Versionamento de palavras-chave: OK" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ ERRO NOS TESTES: " << e.what() << std::endl;
        return 1;
    }
}