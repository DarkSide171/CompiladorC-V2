// Testes de integração entre ExpressionEvaluator e MacroProcessor
// Validação da funcionalidade do operador defined() e expansão de macros

#include "../../include/expression_evaluator.hpp"
#include "../../include/macro_processor.hpp"
#include "../../include/preprocessor_logger.hpp"
#include <iostream>
#include <cassert>
#include <memory>
#include <string>
#include <vector>

using namespace Preprocessor;

// ============================================================================
// DECLARAÇÕES DE FUNÇÕES DE TESTE
// ============================================================================

void testDefinedOperatorBasic();
void testDefinedOperatorInExpressions();
void testMacroExpansionInExpressions();
void testComplexMacroIntegration();
void testEdgeCases();

// ============================================================================
// FUNÇÕES AUXILIARES DE TESTE
// ============================================================================

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
    if (condition) {
        std::cout << "✅ " << testName << " - PASSOU" << std::endl;
    } else {
        std::cout << "❌ " << testName << " - FALHOU" << std::endl;
        assert(false);
    }
}

// Variáveis globais para os testes
std::unique_ptr<PreprocessorLogger> logger;
std::unique_ptr<MacroProcessor> macroProcessor;
std::unique_ptr<ExpressionEvaluator> evaluator;
PreprocessorPosition pos("test", 1, 1);

void setupTest() {
    logger = std::make_unique<PreprocessorLogger>();
    macroProcessor = std::make_unique<MacroProcessor>();
    evaluator = std::make_unique<ExpressionEvaluator>(macroProcessor.get(), logger.get());
}

void teardownTest() {
    evaluator.reset();
    macroProcessor.reset();
    logger.reset();
}

// ============================================================================
// IMPLEMENTAÇÃO DOS TESTES
// ============================================================================

void testDefinedOperatorBasic() {
    std::cout << "\n=== Testando Operador defined() Básico ===" << std::endl;
    setupTest();
    
    // Macro não definida deve retornar 0
    long long result = evaluator->evaluateExpression("defined(UNDEFINED_MACRO)", pos);
    assertEqual(0, result, "Macro não definida");
    
    // Definir uma macro
    macroProcessor->defineMacro("TEST_MACRO", "42", pos);
    
    // Macro definida deve retornar 1
    result = evaluator->evaluateExpression("defined(TEST_MACRO)", pos);
    assertEqual(1, result, "Macro definida");
    
    teardownTest();
}

void testDefinedOperatorInExpressions() {
    std::cout << "\n=== Testando defined() em Expressões ===" << std::endl;
    setupTest();
    
    // Definir algumas macros
    macroProcessor->defineMacro("MACRO_A", "10", pos);
    macroProcessor->defineMacro("MACRO_B", "20", pos);
    
    // Teste: defined(MACRO_A) + defined(MACRO_B) + defined(UNDEFINED)
    long long result = evaluator->evaluateExpression("defined(MACRO_A) + defined(MACRO_B) + defined(UNDEFINED)", pos);
    assertEqual(2, result, "Soma de defined()"); // 1 + 1 + 0 = 2
    
    macroProcessor->defineMacro("FEATURE_A", "1", pos);
    
    // Teste: defined(FEATURE_A) && defined(FEATURE_B)
    result = evaluator->evaluateExpression("defined(FEATURE_A) && defined(FEATURE_B)", pos);
    assertEqual(0, result, "defined() com AND"); // 1 && 0 = 0
    
    // Teste: defined(FEATURE_A) || defined(FEATURE_B)
    result = evaluator->evaluateExpression("defined(FEATURE_A) || defined(FEATURE_B)", pos);
    assertEqual(1, result, "defined() com OR"); // 1 || 0 = 1
    
    teardownTest();
}

void testMacroExpansionInExpressions() {
    std::cout << "\n=== Testando Expansão de Macros ===" << std::endl;
    setupTest();
    
    // Definir macro com valor numérico
    macroProcessor->defineMacro("VALUE", "42", pos);
    
    // A macro deve ser expandida na expressão
    long long result = evaluator->evaluateExpression("VALUE + 8", pos);
    assertEqual(50, result, "Expansão simples de macro"); // 42 + 8 = 50
    
    // Múltiplas macros
    macroProcessor->defineMacro("A", "10", pos);
    macroProcessor->defineMacro("B", "5", pos);
    macroProcessor->defineMacro("C", "2", pos);
    
    result = evaluator->evaluateExpression("A * B + C", pos);
    assertEqual(52, result, "Múltiplas macros"); // 10 * 5 + 2 = 52
    
    // Macro com valor hexadecimal
    macroProcessor->defineMacro("HEX_VALUE", "0xFF", pos);
    result = evaluator->evaluateExpression("HEX_VALUE + 1", pos);
    assertEqual(256, result, "Macro hexadecimal"); // 255 + 1 = 256
    
    // Macro com valor octal
    macroProcessor->defineMacro("OCTAL_VALUE", "0100", pos);
    result = evaluator->evaluateExpression("OCTAL_VALUE * 2", pos);
    assertEqual(128, result, "Macro octal"); // 64 * 2 = 128
    
    teardownTest();
}

void testComplexMacroIntegration() {
    std::cout << "\n=== Testando Integração Complexa ===" << std::endl;
    setupTest();
    
    macroProcessor->defineMacro("MAX_SIZE", "100", pos);
    macroProcessor->defineMacro("MIN_SIZE", "10", pos);
    
    // Expressão complexa combinando macros e defined()
    long long result = evaluator->evaluateExpression("defined(MAX_SIZE) * MAX_SIZE", pos);
    assertEqual(100, result, "Macro com defined()"); // 1 * 100 = 100
    
    // Operadores defined aninhados com parênteses
    macroProcessor->defineMacro("FEATURE_X", "1", pos);
    macroProcessor->defineMacro("FEATURE_Y", "1", pos);
    
    result = evaluator->evaluateExpression("(defined(FEATURE_X) && defined(FEATURE_Y)) || defined(FEATURE_Z)", pos);
    assertEqual(1, result, "defined() aninhados"); // (1 && 1) || 0 = 1
    
    teardownTest();
}

void testEdgeCases() {
    std::cout << "\n=== Testando Casos Especiais ===" << std::endl;
    setupTest();
    
    // Macro não definida deve ser tratada como 0
    long long result = evaluator->evaluateExpression("UNDEFINED_MACRO + 5", pos);
    assertEqual(5, result, "Macro não definida como 0"); // 0 + 5 = 5
    
    // Macro com valor não numérico deve ser tratada como 0
    macroProcessor->defineMacro("TEXT_MACRO", "hello", pos);
    result = evaluator->evaluateExpression("TEXT_MACRO + 10", pos);
    assertEqual(10, result, "Macro não numérica"); // 0 + 10 = 10
    
    // Macro com valor vazio
    macroProcessor->defineMacro("EMPTY_MACRO", "", pos);
    result = evaluator->evaluateExpression("defined(EMPTY_MACRO) + EMPTY_MACRO", pos);
    assertEqual(1, result, "Macro vazia"); // 1 + 0 = 1
    
    // Redefinição de macro
    macroProcessor->defineMacro("COUNTER", "1", pos);
    result = evaluator->evaluateExpression("COUNTER * 10", pos);
    assertEqual(10, result, "Macro inicial");
    
    macroProcessor->defineMacro("COUNTER", "5", pos);
    result = evaluator->evaluateExpression("COUNTER * 10", pos);
    assertEqual(50, result, "Macro redefinida");
    
    teardownTest();
}

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main() {
    std::cout << "🧪 Iniciando Testes de Integração ExpressionEvaluator + MacroProcessor" << std::endl;
    std::cout << "======================================================================" << std::endl;
    
    try {
        testDefinedOperatorBasic();
        testDefinedOperatorInExpressions();
        testMacroExpansionInExpressions();
        testComplexMacroIntegration();
        testEdgeCases();
        
        std::cout << "\n🎉 Todos os testes de integração passaram com sucesso!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\n💥 Erro durante os testes: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "\n💥 Erro desconhecido durante os testes" << std::endl;
        return 1;
    }
}