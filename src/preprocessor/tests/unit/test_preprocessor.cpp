#include "../../include/preprocessor.hpp"
#include <iostream>
#include <cassert>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

void test_basic_initialization() {
    std::cout << "Test: Basic Initialization - ";
    try {
        Preprocessor::PreprocessorMain preprocessor;
        
        // Teste de configuração de versão
        preprocessor.setVersion(Preprocessor::CVersion::C99);
        
        // Teste de reset
        preprocessor.reset();
        
        std::cout << "PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "FAILED: " << e.what() << std::endl;
    }
}

void test_macro_operations() {
    std::cout << "Test: Macro Operations - ";
    try {
        Preprocessor::PreprocessorMain preprocessor;
        
        // Teste de definição de macro simples
        preprocessor.defineMacro("TEST_MACRO", "42");
        assert(preprocessor.isMacroDefined("TEST_MACRO"));
        
        // Teste de macro com espaços
        preprocessor.defineMacro("MACRO_WITH_SPACES", "hello world");
        assert(preprocessor.isMacroDefined("MACRO_WITH_SPACES"));
        
        // Teste de macro vazia
        preprocessor.defineMacro("EMPTY_MACRO", "");
        assert(preprocessor.isMacroDefined("EMPTY_MACRO"));
        
        // Teste de redefinição de macro
        preprocessor.defineMacro("TEST_MACRO", "100");
        assert(preprocessor.isMacroDefined("TEST_MACRO"));
        
        // Teste de remoção de macro
        preprocessor.undefineMacro("TEST_MACRO");
        assert(!preprocessor.isMacroDefined("TEST_MACRO"));
        
        // Teste de remoção de macro inexistente (não deve falhar)
        preprocessor.undefineMacro("NONEXISTENT_MACRO");
        
        std::cout << "PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "FAILED: " << e.what() << std::endl;
    }
}

void test_string_processing() {
    std::cout << "Test: String Processing - ";
    try {
        Preprocessor::PreprocessorMain preprocessor;
        
        // Teste 1: Processamento de string simples
        std::string input1 = "#define MAX 100\nint x = MAX;";
        bool result1 = preprocessor.processString(input1);
        assert(result1);
        
        std::string expanded1 = preprocessor.getExpandedCode();
        assert(!expanded1.empty());
        
        // Reset para próximo teste
        preprocessor.reset();
        
        // Teste 2: String vazia
        std::string input2 = "";
        bool result2 = preprocessor.processString(input2);
        assert(result2); // String vazia deve ser processada com sucesso
        
        // Teste 3: Múltiplas linhas com comentários
        preprocessor.reset();
        std::string input3 = "#define PI 3.14159\n/* Comentario */\n#define E 2.71828\nfloat pi = PI;";
        bool result3 = preprocessor.processString(input3);
        assert(result3);
        
        // Teste 4: Macros aninhadas
        preprocessor.reset();
        std::string input4 = "#define A 1\n#define B A\n#define C B\nint value = C;";
        bool result4 = preprocessor.processString(input4);
        assert(result4);
        
        std::cout << "PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "FAILED: " << e.what() << std::endl;
    }
}

void test_dependencies() {
    std::cout << "Test: Dependencies - ";
    try {
        Preprocessor::PreprocessorMain preprocessor;
        
        // Teste de processamento com includes
        std::string input = "#define A 1\n#define B 2";
        preprocessor.processString(input);
        
        // Verifica se as dependências podem ser obtidas
        auto deps = preprocessor.getDependencies();
        
        std::cout << "PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "FAILED: " << e.what() << std::endl;
    }
}

void test_statistics_collection() {
    std::cout << "Test: Statistics Collection - ";
    try {
        Preprocessor::PreprocessorMain preprocessor;
        
        // Processa algum conteúdo
        std::string input = "#define A 1\n#define B 2";
        preprocessor.processString(input);
        
        // Coleta estatísticas
        auto stats = preprocessor.getStatistics();
        
        std::cout << "PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "FAILED: " << e.what() << std::endl;
    }
}

void test_directive_processing() {
    std::cout << "Test: Directive Processing - ";
    try {
        Preprocessor::PreprocessorMain preprocessor;
        
        // Teste 1: Diretivas condicionais básicas
        std::string input1 = "#define MAX 100\n#ifdef MAX\nint value = MAX;\n#endif";
        bool result1 = preprocessor.processString(input1);
        assert(result1);
        
        std::string expanded1 = preprocessor.getExpandedCode();
        assert(!expanded1.empty());
        
        // Teste 2: #ifndef
        preprocessor.reset();
        std::string input2 = "#ifndef UNDEFINED_MACRO\nint x = 1;\n#endif";
        bool result2 = preprocessor.processString(input2);
        assert(result2);
        
        // Teste 3: #else
        preprocessor.reset();
        std::string input3 = "#ifdef UNDEFINED\nint a = 1;\n#else\nint a = 2;\n#endif";
        bool result3 = preprocessor.processString(input3);
        assert(result3);
        
        // Teste 4: #undef
        preprocessor.reset();
        std::string input4 = "#define TEMP 42\n#undef TEMP\n#ifndef TEMP\nint success = 1;\n#endif";
        bool result4 = preprocessor.processString(input4);
        assert(result4);
        
        // Teste 5: Múltiplas diretivas em sequência
        preprocessor.reset();
        std::string input5 = "#define A 1\n#define B 2\n#ifdef A\n#ifdef B\nint both_defined = 1;\n#endif\n#endif";
        bool result5 = preprocessor.processString(input5);
        assert(result5);
        
        std::cout << "PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "FAILED: " << e.what() << std::endl;
    }
}

void test_advanced_macros() {
    std::cout << "Test: Advanced Macros - ";
    try {
        Preprocessor::PreprocessorMain preprocessor;
        
        // Teste 1: Macros com expressões matemáticas
        preprocessor.defineMacro("SQUARE_5", "((5) * (5))");
        assert(preprocessor.isMacroDefined("SQUARE_5"));
        
        // Teste 2: Macros com operadores condicionais
        preprocessor.defineMacro("MAX_AB", "((10) > (5) ? (10) : (5))");
        assert(preprocessor.isMacroDefined("MAX_AB"));
        
        // Teste 3: Macros com strings literais
        preprocessor.defineMacro("MESSAGE", "\"Hello World\"");
        assert(preprocessor.isMacroDefined("MESSAGE"));
        
        // Teste 4: Macros com identificadores compostos
        preprocessor.defineMacro("VAR_NAME", "my_variable");
        assert(preprocessor.isMacroDefined("VAR_NAME"));
        
        // Teste 5: Processamento com macros avançadas
        std::string input = "int result = SQUARE_5; char* msg = MESSAGE;";
        bool result = preprocessor.processString(input);
        assert(result);
        
        std::cout << "PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "FAILED: " << e.what() << std::endl;
    }
}

void test_nested_conditionals() {
    std::cout << "Test: Nested Conditionals - ";
    try {
        Preprocessor::PreprocessorMain preprocessor;
        
        // Teste 1: Condicionais aninhadas simples
        std::string input1 = "#define A 1\n#define B 2\n#ifdef A\n#ifdef B\nint nested = 1;\n#endif\n#endif";
        bool result1 = preprocessor.processString(input1);
        assert(result1);
        
        // Teste 2: Condicionais com #else aninhado
        preprocessor.reset();
        std::string input2 = "#ifdef UNDEFINED\n#ifdef ALSO_UNDEFINED\nint a = 1;\n#else\nint a = 2;\n#endif\n#else\nint a = 3;\n#endif";
        bool result2 = preprocessor.processString(input2);
        assert(result2);
        
        // Teste 3: Múltiplos níveis de aninhamento
        preprocessor.reset();
        std::string input3 = "#define LEVEL1 1\n#ifdef LEVEL1\n#define LEVEL2 2\n#ifdef LEVEL2\n#define LEVEL3 3\n#ifdef LEVEL3\nint deep = 1;\n#endif\n#endif\n#endif";
        bool result3 = preprocessor.processString(input3);
        assert(result3);
        
        // Teste 4: #elif aninhado
        preprocessor.reset();
        std::string input4 = "#ifdef UNDEFINED\nint a = 1;\n#elif defined(ALSO_UNDEFINED)\nint a = 2;\n#else\nint a = 3;\n#endif";
        bool result4 = preprocessor.processString(input4);
        assert(result4);
        
        std::cout << "PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "FAILED: " << e.what() << std::endl;
    }
}

void test_error_handling() {
    std::cout << "Test: Error Handling - ";
    try {
        Preprocessor::PreprocessorMain preprocessor;
        
        // Teste 1: Diretiva malformada (deve ser tratada graciosamente)
        std::string input1 = "#define\nint x = 1;";
        bool result1 = preprocessor.processString(input1);
        // Não deve falhar completamente, mas pode retornar false
        
        // Teste 2: #endif sem #if correspondente
        preprocessor.reset();
        std::string input2 = "int x = 1;\n#endif";
        bool result2 = preprocessor.processString(input2);
        // Deve ser tratado graciosamente
        
        // Teste 3: #if sem #endif
        preprocessor.reset();
        std::string input3 = "#ifdef TEST\nint x = 1;";
        bool result3 = preprocessor.processString(input3);
        // Deve ser tratado graciosamente
        
        // Teste 4: Macro circular (deve ser detectada)
        preprocessor.reset();
        preprocessor.defineMacro("A", "B");
        preprocessor.defineMacro("B", "A");
        std::string input4 = "int x = A;";
        bool result4 = preprocessor.processString(input4);
        // Deve ser tratado sem loop infinito
        
        std::cout << "PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "FAILED: " << e.what() << std::endl;
    }
}

void test_edge_cases() {
    std::cout << "Test: Edge Cases - ";
    try {
        Preprocessor::PreprocessorMain preprocessor;
        
        // Teste 1: Macro com nome muito longo
        std::string long_name(1000, 'A');
        preprocessor.defineMacro(long_name, "1");
        assert(preprocessor.isMacroDefined(long_name));
        
        // Teste 2: Valor de macro muito longo
        std::string long_value(5000, 'X');
        preprocessor.defineMacro("LONG_MACRO", long_value);
        assert(preprocessor.isMacroDefined("LONG_MACRO"));
        
        // Teste 3: Muitas macros definidas
        for (int i = 0; i < 1000; ++i) {
            std::string name = "MACRO_" + std::to_string(i);
            std::string value = std::to_string(i);
            preprocessor.defineMacro(name, value);
        }
        
        // Teste 4: Caracteres especiais em macros (caracteres seguros)
        preprocessor.defineMacro("SPECIAL", "!@#$%^&*()_+-={}[]<>?./");
        assert(preprocessor.isMacroDefined("SPECIAL"));
        
        // Teste 5: Linhas muito longas
        std::string long_line = "#define LONG_LINE " + std::string(10000, 'L');
        bool result = preprocessor.processString(long_line);
        // Deve ser processado sem problemas
        
        std::cout << "PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "FAILED: " << e.what() << std::endl;
    }
}

void test_performance() {
    std::cout << "Test: Performance - ";
    try {
        Preprocessor::PreprocessorMain preprocessor;
        
        // Teste 1: Processamento de arquivo grande
        std::string large_input;
        for (int i = 0; i < 10000; ++i) {
            large_input += "#define MACRO_" + std::to_string(i) + " " + std::to_string(i) + "\n";
            large_input += "int var_" + std::to_string(i) + " = MACRO_" + std::to_string(i) + ";\n";
        }
        
        bool result1 = preprocessor.processString(large_input);
        assert(result1);
        
        // Teste 2: Múltiplos resets e reprocessamentos
        for (int i = 0; i < 100; ++i) {
            preprocessor.reset();
            std::string input = "#define TEST " + std::to_string(i) + "\nint x = TEST;";
            bool result = preprocessor.processString(input);
            assert(result);
        }
        
        // Teste 3: Expansão profunda de macros
        preprocessor.reset();
        std::string deep_input = "#define LEVEL0 0\n";
        for (int i = 1; i < 50; ++i) {
            deep_input += "#define LEVEL" + std::to_string(i) + " LEVEL" + std::to_string(i-1) + "\n";
        }
        deep_input += "int deep_value = LEVEL49;";
        
        bool result3 = preprocessor.processString(deep_input);
        assert(result3);
        
        std::cout << "PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "FAILED: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "=== EXECUTANDO TESTES DO PREPROCESSOR PRINCIPAL ===" << std::endl;
    
    test_basic_initialization();
    test_macro_operations();
    test_string_processing();
    test_dependencies();
    test_statistics_collection();
    test_directive_processing();
    test_advanced_macros();
    test_nested_conditionals();
    test_error_handling();
    test_edge_cases();
    test_performance();
    
    std::cout << "\n=== TODOS OS TESTES CONCLUÍDOS ===" << std::endl;
    return 0;
}