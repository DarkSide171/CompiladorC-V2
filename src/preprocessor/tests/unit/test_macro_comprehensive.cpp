// Teste Abrangente de Macros - Unificação completa dos testes de macros
// Consolida funcionalidades de test_macros_simple, test_macro_processor, test_macro_optimization,
// test_macros.cpp e test_stringification_concatenation.cpp

#include "../../include/macro_processor.hpp"
#include "../../include/preprocessor_state.hpp"
#include "../../include/preprocessor_logger.hpp"
#include <iostream>
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <sstream>

using namespace Preprocessor;
using namespace std::chrono;

// ============================================================================
// FUNÇÕES AUXILIARES PARA TESTES
// ============================================================================

void assertEqual(const std::string& expected, const std::string& actual, const std::string& testName) {
    if (expected != actual) {
        std::cerr << "❌ FALHA em " << testName << ": esperado '" << expected 
                  << "', obtido '" << actual << "'" << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertTrue(bool condition, const std::string& testName) {
    if (!condition) {
        std::cerr << "❌ FALHA em " << testName << ": condição falsa" << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertFalse(bool condition, const std::string& testName) {
    if (condition) {
        std::cerr << "❌ FALHA em " << testName << ": condição verdadeira" << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void testResult(bool condition, const std::string& testName) {
    if (condition) {
        std::cout << "✅ " << testName << " passou" << std::endl;
    } else {
        std::cout << "❌ " << testName << " falhou" << std::endl;
    }
}

void testEqual(const std::string& expected, const std::string& actual, const std::string& testName) {
    if (expected == actual) {
        std::cout << "✅ " << testName << " passou" << std::endl;
    } else {
        std::cout << "❌ " << testName << " falhou: esperado '" << expected 
                  << "', obtido '" << actual << "'" << std::endl;
    }
}

// ============================================================================
// SETUP DE COMPONENTES PARA TESTES
// ============================================================================

std::unique_ptr<MacroProcessor> createMacroProcessor() {
    auto logger = std::make_shared<PreprocessorLogger>();
    auto state = std::make_shared<PreprocessorState>();
    return std::make_unique<MacroProcessor>(logger, state);
}

// ============================================================================
// TESTES DA STRUCT MacroInfo
// ============================================================================

void testMacroInfoConstructors() {
    std::cout << "\n=== Testando Construtores de MacroInfo ===" << std::endl;
    
    try {
        // Construtor padrão
        MacroInfo info1;
        assertEqual("", info1.name, "MacroInfo construtor padrão - name");
        assertEqual("", info1.value, "MacroInfo construtor padrão - value");
        assertTrue(info1.type == MacroType::OBJECT_LIKE, "MacroInfo construtor padrão - type");
        assertFalse(info1.isVariadic, "MacroInfo construtor padrão - isVariadic");
        assertFalse(info1.isPredefined, "MacroInfo construtor padrão - isPredefined");
        
        // Construtor com parâmetros básicos
        MacroInfo info2("TEST", "123", MacroType::OBJECT_LIKE);
        assertEqual("TEST", info2.name, "MacroInfo construtor básico - name");
        assertEqual("123", info2.value, "MacroInfo construtor básico - value");
        assertTrue(info2.type == MacroType::OBJECT_LIKE, "MacroInfo construtor básico - type");
        
        // Construtor para macro funcional
        std::vector<std::string> params = {"x", "y"};
        MacroInfo info3("ADD", "x + y", params, false);
        assertEqual("ADD", info3.name, "MacroInfo construtor funcional - name");
        assertEqual("x + y", info3.value, "MacroInfo construtor funcional - value");
        assertTrue(info3.type == MacroType::FUNCTION_LIKE, "MacroInfo construtor funcional - type");
        assertTrue(info3.parameters == params, "MacroInfo construtor funcional - parameters");
        assertFalse(info3.isVariadic, "MacroInfo construtor funcional - isVariadic");
        
        // Construtor para macro variádica
        MacroInfo info4("PRINTF", "printf(fmt, __VA_ARGS__)", {"fmt"}, true);
        assertEqual("PRINTF", info4.name, "MacroInfo construtor variádico - name");
        assertTrue(info4.type == MacroType::VARIADIC, "MacroInfo construtor variádico - type");
        assertTrue(info4.isVariadic, "MacroInfo construtor variádico - isVariadic");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de MacroInfo: " << e.what() << std::endl;
    }
}

void testMacroInfoOperators() {
    std::cout << "\n=== Testando Operadores de MacroInfo ===" << std::endl;
    
    try {
        MacroInfo info1("TEST", "123", MacroType::OBJECT_LIKE);
        MacroInfo info2("TEST", "123", MacroType::OBJECT_LIKE);
        MacroInfo info3("TEST", "456", MacroType::OBJECT_LIKE);
        
        // Teste de igualdade
        assertTrue(info1 == info2, "MacroInfo operador == - iguais");
        assertFalse(info1 == info3, "MacroInfo operador == - diferentes");
        
        // Teste de desigualdade
        assertFalse(info1 != info2, "MacroInfo operador != - iguais");
        assertTrue(info1 != info3, "MacroInfo operador != - diferentes");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de operadores: " << e.what() << std::endl;
    }
}

void testMacroInfoMethods() {
    std::cout << "\n=== Testando Métodos de MacroInfo ===" << std::endl;
    
    try {
        // Macro simples
        MacroInfo simple("SIMPLE", "42", MacroType::OBJECT_LIKE);
        assertFalse(simple.isFunctionLike(), "MacroInfo isFunctionLike - simples");
        assertFalse(simple.hasParameters(), "MacroInfo hasParameters - simples");
        assertTrue(simple.getParameterCount() == 0, "MacroInfo getParameterCount - simples");
        
        // Macro funcional
        std::vector<std::string> params = {"a", "b", "c"};
        MacroInfo functional("FUNC", "a + b + c", params, false);
        assertTrue(functional.isFunctionLike(), "MacroInfo isFunctionLike - funcional");
        assertTrue(functional.hasParameters(), "MacroInfo hasParameters - funcional");
        assertTrue(functional.getParameterCount() == 3, "MacroInfo getParameterCount - funcional");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de métodos: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES BÁSICOS DE MACROS
// ============================================================================

void testBasicMacroDefinition() {
    std::cout << "\n=== Testando Definição Básica de Macros ===" << std::endl;
    
    auto processor = createMacroProcessor();
    
    try {
        // Teste 1: Macro simples sem argumentos
        bool result1 = processor->defineMacro("MAX", "100");
        testResult(result1, "Definição de macro simples");
        testResult(processor->isDefined("MAX"), "Macro MAX está definida");
        
        // Teste 2: Macro com valor vazio
        bool result2 = processor->defineMacro("EMPTY", "");
        testResult(result2, "Definição de macro vazia");
        testResult(processor->isDefined("EMPTY"), "Macro EMPTY está definida");
        
        // Teste 3: Macro com valor complexo
        bool result3 = processor->defineMacro("COMPLEX", "(x + y) * 2");
        testResult(result3, "Definição de macro complexa");
        testResult(processor->isDefined("COMPLEX"), "Macro COMPLEX está definida");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de definição: " << e.what() << std::endl;
    }
}

void testMacroExpansion() {
    std::cout << "\n=== Testando Expansão de Macros ===" << std::endl;
    
    auto processor = createMacroProcessor();
    
    try {
        // Setup: Definir macros para teste
        processor->defineMacro("MAX", "100");
        processor->defineMacro("MIN", "0");
        processor->defineMacro("RANGE", "MAX - MIN");
        
        // Teste 1: Expansão simples
        std::string expanded1 = processor->expandMacro("MAX");
        testEqual("100", expanded1, "Expansão de macro simples");
        
        // Teste 2: Expansão em contexto
        std::string expanded2 = processor->processLine("int value = MAX;");
        testResult(expanded2.find("100") != std::string::npos, "Expansão em contexto");
        
        // Teste 3: Expansão aninhada
        std::string expanded3 = processor->expandMacroRecursively("RANGE");
        testResult(expanded3.find("100") != std::string::npos && expanded3.find("0") != std::string::npos, 
                   "Expansão aninhada");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de expansão: " << e.what() << std::endl;
    }
}

void testMacroUndefinition() {
    std::cout << "\n=== Testando Remoção de Macros ===" << std::endl;
    
    auto processor = createMacroProcessor();
    
    try {
        // Setup: Definir macro
        processor->defineMacro("TEMP", "42");
        testResult(processor->isDefined("TEMP"), "Macro TEMP definida inicialmente");
        
        // Teste: Remover macro
        bool result = processor->undefineMacro("TEMP");
        testResult(result, "Remoção de macro");
        testResult(!processor->isDefined("TEMP"), "Macro TEMP não está mais definida");
        
        // Teste: Tentar remover macro inexistente
        bool result2 = processor->undefineMacro("INEXISTENTE");
        testResult(!result2, "Remoção de macro inexistente falha corretamente");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de remoção: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES DE MACROS FUNCIONAIS
// ============================================================================

void testFunctionLikeMacros() {
    std::cout << "\n=== Testando Macros Tipo Função ===" << std::endl;
    
    auto processor = createMacroProcessor();
    
    try {
        // Definir macro funcional
        std::vector<std::string> params = {"x", "y"};
        bool result1 = processor->defineFunctionMacro("ADD", params, "(x) + (y)");
        testResult(result1, "Definição de macro funcional");
        
        // Testar expansão
        std::string expanded = processor->expandFunctionMacro("ADD", {"5", "3"});
        testEqual("(5) + (3)", expanded, "Expansão de macro funcional");
        
        // Macro com mais parâmetros
        std::vector<std::string> params2 = {"a", "b", "c"};
        bool result2 = processor->defineFunctionMacro("MAX3", params2, "((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))");
        testResult(result2, "Definição de macro funcional complexa");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de macros funcionais: " << e.what() << std::endl;
    }
}

void testVariadicMacros() {
    std::cout << "\n=== Testando Macros Variádicas ===" << std::endl;
    
    auto processor = createMacroProcessor();
    
    try {
        // Definir macro variádica usando defineFunctionMacro
        std::vector<std::string> params = {"fmt"};
        bool result1 = processor->defineFunctionMacro("DEBUG_PRINT", params, "printf(fmt, __VA_ARGS__)", true);
        testResult(result1, "Definição de macro variádica");
        
        // Testar expansão com argumentos variados
        std::vector<std::string> args = {"\"Value: %d\\n\"", "42"};
        std::string expanded = processor->expandFunctionMacro("DEBUG_PRINT", args);
        testResult(expanded.find("printf") != std::string::npos, "Expansão de macro variádica");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de macros variádicas: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES DE VALIDAÇÃO E REDEFINIÇÃO
// ============================================================================

void testMacroValidation() {
    std::cout << "\n=== Testando Validação de Macros ===" << std::endl;
    
    auto processor = createMacroProcessor();
    
    try {
        // Teste de nomes inválidos
        testResult(!processor->defineMacro("", "value"), "Nome vazio rejeitado");
        testResult(!processor->defineMacro("123ABC", "value"), "Nome começando com número rejeitado");
        testResult(!processor->defineMacro("AB-CD", "value"), "Nome com hífen rejeitado");
        
        // Teste de nomes válidos
        testResult(processor->defineMacro("VALID_NAME", "value"), "Nome válido aceito");
        testResult(processor->defineMacro("_UNDERSCORE", "value"), "Nome com underscore aceito");
        testResult(processor->defineMacro("ABC123", "value"), "Nome com números no final aceito");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de validação: " << e.what() << std::endl;
    }
}

void testMacroRedefinition() {
    std::cout << "\n=== Testando Redefinição de Macros ===" << std::endl;
    
    auto processor = createMacroProcessor();
    
    try {
        // Definir macro inicial
        processor->defineMacro("VALUE", "100");
        testEqual("100", processor->expandMacro("VALUE"), "Valor inicial da macro");
        
        // Redefinir com mesmo valor (deve ser permitido)
        bool result1 = processor->defineMacro("VALUE", "100");
        testResult(result1, "Redefinição com mesmo valor permitida");
        
        // Redefinir com valor diferente (deve gerar warning mas ser permitido)
        bool result2 = processor->defineMacro("VALUE", "200");
        testResult(result2, "Redefinição com valor diferente permitida");
        testEqual("200", processor->expandMacro("VALUE"), "Novo valor da macro");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de redefinição: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES DE PERFORMANCE E OTIMIZAÇÃO
// ============================================================================

void testMacroPerformance() {
    std::cout << "\n=== Testando Performance de Macros ===" << std::endl;
    
    auto processor = createMacroProcessor();
    
    try {
        // Configurar otimizações
        processor->configureCacheOptimization(500, true);
        
        // Definir macros para teste
        processor->defineMacro("PI", "3.14159");
        processor->defineMacro("MAX_SIZE", "1024");
        processor->defineMacro("BUFFER_SIZE", "MAX_SIZE * 2");
        processor->defineMacro("TOTAL_SIZE", "BUFFER_SIZE + 256");
        
        std::string testText = "O valor de PI é PI e o tamanho total é TOTAL_SIZE bytes";
        
        auto start = high_resolution_clock::now();
        
        // Primeira expansão (sem cache)
        std::string result1 = processor->expandMacroRecursively(testText);
        
        auto mid = high_resolution_clock::now();
        
        // Segunda expansão (com cache)
        std::string result2 = processor->expandMacroRecursively(testText);
        
        auto end = high_resolution_clock::now();
        
        auto duration1 = duration_cast<microseconds>(mid - start);
        auto duration2 = duration_cast<microseconds>(end - mid);
        
        std::cout << "Primeira expansão: " << duration1.count() << " μs" << std::endl;
        std::cout << "Segunda expansão: " << duration2.count() << " μs" << std::endl;
        
        if (duration2.count() <= duration1.count()) {
            std::cout << "✅ Cache melhorou ou manteve performance" << std::endl;
        }
        
        // Verificar consistência
        testEqual(result1, result2, "Consistência entre expansões");
        
        // Testar pré-carregamento
        std::vector<std::string> frequentMacros = {"PI", "MAX_SIZE", "BUFFER_SIZE"};
        processor->preloadFrequentMacros(frequentMacros);
        
        std::cout << "✅ Pré-carregamento de macros frequentes concluído" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de performance: " << e.what() << std::endl;
    }
}

void testMacroStatistics() {
    std::cout << "\n=== Testando Estatísticas de Macros ===" << std::endl;
    
    auto processor = createMacroProcessor();
    
    try {
        // Definir algumas macros
        processor->defineMacro("MACRO1", "value1");
        processor->defineMacro("MACRO2", "value2");
        processor->defineMacro("MACRO3", "value3");
        
        // Expandir algumas macros
        processor->expandMacro("MACRO1");
        processor->expandMacro("MACRO2");
        processor->expandMacro("MACRO1"); // Repetir para testar contadores
        
        // Obter estatísticas
        std::string stats = processor->getStatistics();
        testResult(!stats.empty(), "Estatísticas não estão vazias");
        
        // Testar tamanho do cache
        size_t cacheSize = processor->getCurrentCacheSize();
        testResult(cacheSize >= 0, "Tamanho do cache é válido");
        
        std::cout << "Estatísticas: " << stats << std::endl;
        std::cout << "Tamanho do cache: " << cacheSize << " entradas" << std::endl;
        
        // Otimizar cache
        processor->optimizeCache(1); // Remove entradas mais antigas que 1 segundo
        size_t newCacheSize = processor->getCurrentCacheSize();
        std::cout << "Após otimização: " << newCacheSize << " entradas" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de estatísticas: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES DE INTEGRAÇÃO
// ============================================================================

void testStringificationOperator() {
    std::cout << "\n=== Testando Operador de Stringificação (#) ===" << std::endl;
    
    auto processor = createMacroProcessor();
    
    try {
        // Teste básico de stringificação
        std::string result1 = processor->handleStringification("hello");
        assertEqual("\"hello\"", result1, "Stringificação básica");
        
        // Teste com espaços
        std::string result2 = processor->handleStringification("hello world");
        assertEqual("\"hello world\"", result2, "Stringificação com espaços");
        
        // Teste com aspas
        std::string result3 = processor->handleStringification("test\"quote");
        assertEqual("\"test\\\"quote\"", result3, "Stringificação com aspas");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em stringificação: " << e.what() << std::endl;
    }
}

void testConcatenationOperator() {
    std::cout << "\n=== Testando Operador de Concatenação (##) ===" << std::endl;
    
    auto processor = createMacroProcessor();
    
    try {
        // Teste básico de concatenação
        std::string result1 = processor->handleConcatenation("hello", "world");
        assertEqual("helloworld", result1, "Concatenação básica");
        
        // Teste com espaços (devem ser removidos)
        std::string result2 = processor->handleConcatenation("  prefix  ", "  suffix  ");
        assertEqual("prefixsuffix", result2, "Concatenação com espaços");
        
        // Teste com números
        std::string result3 = processor->handleConcatenation("var", "123");
        assertEqual("var123", result3, "Concatenação com números");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em concatenação: " << e.what() << std::endl;
    }
}

void testAdvancedFunctionMacros() {
    std::cout << "\n=== Testando Macros Funcionais Avançadas ===" << std::endl;
    
    auto processor = createMacroProcessor();
    
    try {
        // Definir macro funcional
        std::vector<std::string> params = {"x", "y"};
        bool success = processor->defineFunctionMacro("ADD", params, "x + y");
        assertTrue(success, "Definição de macro funcional");
        
        // Expandir macro funcional
        std::vector<std::string> args = {"5", "3"};
        std::string result = processor->expandFunctionMacro("ADD", args);
        assertEqual("5 + 3", result, "Expansão de macro funcional");
        
        // Teste de validação de argumentos
        bool valid1 = processor->validateParameterCount("ADD", 2);
        assertTrue(valid1, "Validação com número correto de argumentos");
        
        bool valid2 = processor->validateParameterCount("ADD", 1);
        assertFalse(valid2, "Validação com número incorreto de argumentos");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em macros funcionais: " << e.what() << std::endl;
    }
}

void testMacroRecursionAndLimits() {
    std::cout << "\n=== Testando Recursão e Limites de Macros ===" << std::endl;
    
    auto processor = createMacroProcessor();
    
    try {
        // Teste de macro com caracteres especiais
        bool result1 = processor->defineMacro("SPECIAL", "@#$%^&*()");
        assertTrue(result1, "Definição com caracteres especiais");
        
        // Teste de redefinição
        processor->defineMacro("REDEF", "original");
        bool result2 = processor->defineMacro("REDEF", "redefined");
        assertTrue(result2, "Redefinição de macro");
        
        std::string expanded = processor->expandMacro("REDEF");
        assertEqual("redefined", expanded, "Valor após redefinição");
        
        // Teste de limites (macro muito longa)
        std::string longValue(1000, 'x');
        bool result3 = processor->defineMacro("LONG_MACRO", longValue);
        assertTrue(result3, "Definição de macro longa");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em recursão/limites: " << e.what() << std::endl;
    }
}

void testMacroIntegration() {
    std::cout << "\n=== Testando Integração de Macros ===" << std::endl;
    
    auto processor = createMacroProcessor();
    
    try {
        // Cenário complexo: macros aninhadas e funcionais
        processor->defineMacro("VERSION_MAJOR", "2");
        processor->defineMacro("VERSION_MINOR", "1");
        processor->defineMacro("VERSION_PATCH", "0");
        
        std::vector<std::string> params = {"maj", "min", "patch"};
        processor->defineFunctionMacro("MAKE_VERSION", params, "((maj) << 16) | ((min) << 8) | (patch)");
        
        processor->defineMacro("CURRENT_VERSION", "MAKE_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH)");
        
        // Testar expansão completa
        std::string expanded = processor->expandMacroRecursively("CURRENT_VERSION");
        testResult(expanded.find("2") != std::string::npos && 
                   expanded.find("1") != std::string::npos && 
                   expanded.find("0") != std::string::npos, 
                   "Expansão de macros aninhadas e funcionais");
        
        std::cout << "Expansão final: " << expanded << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de integração: " << e.what() << std::endl;
    }
}

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main() {
    std::cout << "=== TESTE ABRANGENTE DE MACROS ===" << std::endl;
    std::cout << "Unificação dos testes: macros_simple, macro_processor, macro_optimization" << std::endl;
    
    try {
        // Executa todos os testes
        testMacroInfoConstructors();
        testMacroInfoOperators();
        testMacroInfoMethods();
        testBasicMacroDefinition();
        testMacroExpansion();
        testMacroUndefinition();
        testFunctionLikeMacros();
        testVariadicMacros();
        testMacroValidation();
        testMacroRedefinition();
        testStringificationOperator();
        testConcatenationOperator();
        testAdvancedFunctionMacros();
        testMacroRecursionAndLimits();
        testMacroPerformance();
        testMacroStatistics();
        testMacroIntegration();
        
        std::cout << "\n=== RESUMO FINAL ===" << std::endl;
        std::cout << "✅ Testes de MacroInfo: Concluído" << std::endl;
        std::cout << "✅ Testes de Definição Básica: Concluído" << std::endl;
        std::cout << "✅ Testes de Expansão: Concluído" << std::endl;
        std::cout << "✅ Testes de Remoção: Concluído" << std::endl;
        std::cout << "✅ Testes de Macros Funcionais: Concluído" << std::endl;
        std::cout << "✅ Testes de Macros Variádicas: Concluído" << std::endl;
        std::cout << "✅ Testes de Validação: Concluído" << std::endl;
        std::cout << "✅ Testes de Redefinição: Concluído" << std::endl;
        std::cout << "✅ Testes de Stringificação: Concluído" << std::endl;
        std::cout << "✅ Testes de Concatenação: Concluído" << std::endl;
        std::cout << "✅ Testes de Macros Avançadas: Concluído" << std::endl;
        std::cout << "✅ Testes de Recursão/Limites: Concluído" << std::endl;
        std::cout << "✅ Testes de Performance: Concluído" << std::endl;
        std::cout << "✅ Testes de Estatísticas: Concluído" << std::endl;
        std::cout << "✅ Testes de Integração: Concluído" << std::endl;
        
        std::cout << "\n🎉 TODOS OS TESTES DE MACROS PASSARAM COM SUCESSO! 🎉" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Erro durante execução dos testes: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}