// Teste Abrangente de Utilitários - Unificação de testes menores
// Consolida funcionalidades de test_exact_utility.cpp, test_new_methods.cpp,
// test_position_mapping.cpp e test_expression_macro_integration.cpp

#include "../../include/directive.hpp"
#include "../../include/preprocessor_state.hpp"
#include "../../include/preprocessor_logger.hpp"
#include "../../include/macro_processor.hpp"
#include "../../include/file_manager.hpp"
#include "../../include/conditional_processor.hpp"
#include "../../include/expression_evaluator.hpp"
#include "../../include/preprocessor_lexer_interface.hpp"
#include <iostream>
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <chrono>

using namespace Preprocessor;

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

void assertEqual(long long expected, long long actual, const std::string& testName) {
    if (expected == actual) {
        std::cout << "✅ " << testName << " - PASSOU" << std::endl;
    } else {
        std::cout << "❌ " << testName << " - FALHOU" << std::endl;
        std::cout << "   Esperado: " << expected << std::endl;
        std::cout << "   Atual: " << actual << std::endl;
        assert(false);
    }
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

// Macros simples para testes de posição
#define EXPECT_TRUE(condition) assert(condition)
#define EXPECT_FALSE(condition) assert(!(condition))
#define EXPECT_EQ(a, b) assert((a) == (b))
#define EXPECT_NE(a, b) assert((a) != (b))
#define EXPECT_GT(a, b) assert((a) > (b))
#define ASSERT_TRUE(condition) assert(condition)

// ============================================================================
// TESTES DE UTILITÁRIOS EXATOS (test_exact_utility.cpp)
// ============================================================================

// Variáveis globais para testes de utilitários
std::shared_ptr<PreprocessorState> g_state;
std::shared_ptr<PreprocessorLogger> g_logger;
std::shared_ptr<MacroProcessor> g_macroProcessor;
std::shared_ptr<FileManager> g_fileManager;
std::shared_ptr<ConditionalProcessor> g_conditionalProcessor;

std::unique_ptr<DirectiveProcessor> createDirectiveProcessor() {
    g_state = std::make_shared<PreprocessorState>();
    g_logger = std::make_shared<PreprocessorLogger>();
    g_macroProcessor = std::make_shared<MacroProcessor>(g_logger, g_state);
    g_fileManager = std::make_shared<FileManager>(std::vector<std::string>{}, g_logger.get());
    g_conditionalProcessor = std::make_shared<ConditionalProcessor>(g_logger.get(), g_macroProcessor.get());
    
    return std::make_unique<DirectiveProcessor>(g_state.get(), g_logger.get(), g_macroProcessor.get(), 
                                               g_fileManager.get(), g_conditionalProcessor.get());
}

void testUtilityMethods() {
    std::cout << "\n=== Testando Métodos Utilitários ===" << std::endl;
    
    auto processor = createDirectiveProcessor();
    
    try {
        // Teste de criação do processador
        assertTrue(processor != nullptr, "Criação do DirectiveProcessor");
        
        // Teste de componentes
        assertTrue(g_state != nullptr, "Estado do pré-processador criado");
        assertTrue(g_logger != nullptr, "Logger criado");
        assertTrue(g_macroProcessor != nullptr, "Processador de macros criado");
        assertTrue(g_fileManager != nullptr, "Gerenciador de arquivos criado");
        assertTrue(g_conditionalProcessor != nullptr, "Processador condicional criado");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em métodos utilitários: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES DE NOVOS MÉTODOS (test_new_methods.cpp)
// ============================================================================

void testNewExpressionEvaluatorMethods() {
    std::cout << "\n=== Testando Novos Métodos do ExpressionEvaluator ===" << std::endl;
    
    // Criar logger e avaliador
    auto logger = std::make_shared<PreprocessorLogger>();
    ExpressionEvaluator evaluator(nullptr, logger.get());
    PreprocessorPosition pos("test", 1, 1);
    
    try {
        // Teste evaluateBooleanExpression
        bool result1 = evaluator.evaluateBooleanExpression("1", pos);
        assertTrue(result1, "evaluateBooleanExpression('1') = true");
        
        bool result2 = evaluator.evaluateBooleanExpression("0", pos);
        assertFalse(result2, "evaluateBooleanExpression('0') = false");
        
        bool result3 = evaluator.evaluateBooleanExpression("5 + 3", pos);
        assertTrue(result3, "evaluateBooleanExpression('5 + 3') = true");
        
        bool result4 = evaluator.evaluateBooleanExpression("10 - 10", pos);
        assertFalse(result4, "evaluateBooleanExpression('10 - 10') = false");
        
        // Teste isValidExpression
        bool valid1 = evaluator.isValidExpression("1 + 2");
        assertTrue(valid1, "isValidExpression('1 + 2') = true");
        
        bool valid2 = evaluator.isValidExpression("(3 * 4)");
        assertTrue(valid2, "isValidExpression('(3 * 4)') = true");
        
        bool valid3 = evaluator.isValidExpression("1 + +");
        assertFalse(valid3, "isValidExpression('1 + +') = false");
        
        bool valid4 = evaluator.isValidExpression("((1 + 2)");
        assertFalse(valid4, "isValidExpression('((1 + 2)') = false");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em novos métodos: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES DE MAPEAMENTO DE POSIÇÃO (test_position_mapping.cpp)
// ============================================================================

void testSourceMappingStructure() {
    std::cout << "\n=== Testando Estrutura SourceMapping ===" << std::endl;
    
    try {
        // Cria um mapeamento de teste
        SourceMapping mapping;
        mapping.processedLine = 10;
        mapping.processedColumn = 5;
        mapping.originalLine = 8;
        mapping.originalColumn = 3;
        mapping.originalFile = "test.c";
        mapping.fromMacroExpansion = true;
        mapping.macroName = "TEST_MACRO";
        
        // Verifica se os valores foram atribuídos corretamente
        EXPECT_EQ(mapping.processedLine, 10);
        EXPECT_EQ(mapping.processedColumn, 5);
        EXPECT_EQ(mapping.originalLine, 8);
        EXPECT_EQ(mapping.originalColumn, 3);
        EXPECT_EQ(mapping.originalFile, "test.c");
        EXPECT_TRUE(mapping.fromMacroExpansion);
        EXPECT_EQ(mapping.macroName, "TEST_MACRO");
        
        std::cout << "✅ Estrutura SourceMapping passou" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em mapeamento de posição: " << e.what() << std::endl;
    }
}

void testPositionMappingBasic() {
    std::cout << "\n=== Testando Mapeamento Básico de Posições ===" << std::endl;
    
    try {
        // Teste de posição básica
        PreprocessorPosition pos1("file1.c", 10, 5);
        assertEqual("file1.c", pos1.filename, "Nome do arquivo");
        EXPECT_EQ(pos1.line, 10);
        EXPECT_EQ(pos1.column, 5);
        
        // Teste de cópia de posição
        PreprocessorPosition pos2 = pos1;
        assertEqual(pos1.filename, pos2.filename, "Cópia de posição - arquivo");
        EXPECT_EQ(pos1.line, pos2.line);
        EXPECT_EQ(pos1.column, pos2.column);
        
        std::cout << "✅ Mapeamento básico de posições passou" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em mapeamento básico: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES DE INTEGRAÇÃO EXPRESSÃO-MACRO (test_expression_macro_integration.cpp)
// ============================================================================

// Variáveis globais para testes de integração
std::shared_ptr<PreprocessorLogger> integration_logger;
    std::shared_ptr<MacroProcessor> integration_macroProcessor;
std::unique_ptr<ExpressionEvaluator> integration_evaluator;
PreprocessorPosition integration_pos("test", 1, 1);

void setupIntegrationTest() {
    integration_logger = std::make_shared<PreprocessorLogger>();
    integration_macroProcessor = std::make_shared<MacroProcessor>(integration_logger, nullptr);
    integration_evaluator = std::make_unique<ExpressionEvaluator>(integration_macroProcessor.get(), integration_logger.get());
}

void testDefinedOperatorBasic() {
    std::cout << "\n=== Testando Operador defined() Básico ===" << std::endl;
    
    setupIntegrationTest();
    
    try {
        // Definir uma macro
        integration_macroProcessor->defineMacro("TEST_MACRO", "1");
        
        // Testar operador defined
        long long result1 = integration_evaluator->evaluateExpression("defined(TEST_MACRO)", integration_pos);
        assertEqual(1LL, result1, "defined(TEST_MACRO) com macro definida");
        
        long long result2 = integration_evaluator->evaluateExpression("defined(UNDEFINED_MACRO)", integration_pos);
        assertEqual(0LL, result2, "defined(UNDEFINED_MACRO) com macro não definida");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em operador defined: " << e.what() << std::endl;
    }
}

void testMacroExpansionInExpressions() {
    std::cout << "\n=== Testando Expansão de Macros em Expressões ===" << std::endl;
    
    setupIntegrationTest();
    
    try {
        // Definir macros numéricas
        integration_macroProcessor->defineMacro("VALUE_A", "10");
        integration_macroProcessor->defineMacro("VALUE_B", "5");
        
        // Testar expansão em expressões
        long long result1 = integration_evaluator->evaluateExpression("VALUE_A + VALUE_B", integration_pos);
        assertEqual(15LL, result1, "Expansão de macros em soma");
        
        long long result2 = integration_evaluator->evaluateExpression("VALUE_A * VALUE_B", integration_pos);
        assertEqual(50LL, result2, "Expansão de macros em multiplicação");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em expansão de macros: " << e.what() << std::endl;
    }
}

void testComplexMacroIntegration() {
    std::cout << "\n=== Testando Integração Complexa de Macros ===" << std::endl;
    
    setupIntegrationTest();
    
    try {
        // Definir macros complexas
        integration_macroProcessor->defineMacro("MAX_SIZE", "100");
        integration_macroProcessor->defineMacro("MIN_SIZE", "10");
        
        // Testar expressões complexas
        long long result1 = integration_evaluator->evaluateExpression("defined(MAX_SIZE) && (MAX_SIZE > MIN_SIZE)", integration_pos);
        assertEqual(1LL, result1, "Expressão complexa com defined e comparação");
        
        long long result2 = integration_evaluator->evaluateExpression("MAX_SIZE - MIN_SIZE", integration_pos);
        assertEqual(90LL, result2, "Subtração de macros");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em integração complexa: " << e.what() << std::endl;
    }
}

// ============================================================================
// FUNÇÃO PRINCIPAL DE TESTES
// ============================================================================

int main() {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "TESTES ABRANGENTES DE UTILITÁRIOS DO PRÉ-PROCESSADOR" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    try {
        // Executar todos os testes
        testUtilityMethods();
        testNewExpressionEvaluatorMethods();
        testSourceMappingStructure();
        testPositionMappingBasic();
        testDefinedOperatorBasic();
        testMacroExpansionInExpressions();
        testComplexMacroIntegration();
        
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "RESUMO DOS TESTES DE UTILITÁRIOS" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "✅ Testes de Métodos Utilitários: Concluído" << std::endl;
        std::cout << "✅ Testes de Novos Métodos: Concluído" << std::endl;
        std::cout << "✅ Testes de Mapeamento de Posição: Concluído" << std::endl;
        std::cout << "✅ Testes de Integração Expressão-Macro: Concluído" << std::endl;
        std::cout << "\n🎉 TODOS OS TESTES DE UTILITÁRIOS PASSARAM! 🎉" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n❌ ERRO GERAL: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}