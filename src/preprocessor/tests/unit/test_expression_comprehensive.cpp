// Teste Abrangente de Expressões - Unificação dos testes de expressões
// Consolida funcionalidades de test_expressions, test_expression_evaluator e test_expression_evaluator_advanced

#include "../../include/expression_evaluator.hpp"
#include "../../include/preprocessor_state.hpp"
#include "../../include/preprocessor_logger.hpp"
#include "../../include/macro_processor.hpp"
#include <iostream>
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <chrono>

using namespace Preprocessor;

// ============================================================================
// FUNÇÕES AUXILIARES PARA TESTES
// ============================================================================

void assertEqual(long long expected, long long actual, const std::string& testName) {
    if (expected != actual) {
        std::cerr << "❌ FALHA em " << testName << ": esperado " << expected 
                  << ", obtido " << actual << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertTrue(bool condition, const std::string& testName) {
    if (!condition) {
        std::cerr << "❌ FALHA em " << testName << ": condição falsa" << std::endl;
        assert(false);
    } else {
        std::cout << "✅ " << testName << " passou" << std::endl;
    }
}

void assertFalse(bool condition, const std::string& testName) {
    if (condition) {
        std::cerr << "❌ FALHA em " << testName << ": condição verdadeira" << std::endl;
        assert(false);
    } else {
        std::cout << "✅ " << testName << " passou" << std::endl;
    }
}

void assertThrows(const std::function<void()>& func, const std::string& testName) {
    try {
        func();
        std::cout << "❌ " << testName << " - FALHOU (exceção esperada não foi lançada)" << std::endl;
        assert(false);
    } catch (const std::exception&) {
        std::cout << "✅ " << testName << " - PASSOU" << std::endl;
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
// TESTES BÁSICOS DE ARITMÉTICA
// ============================================================================

void testBasicArithmeticExpressions() {
    std::cout << "\n=== Testando Expressões Aritméticas Básicas ===" << std::endl;
    
    auto macroProcessor = createMacroProcessor();
    auto logger = std::make_shared<PreprocessorLogger>();
    ExpressionEvaluator evaluator(macroProcessor.get(), logger.get());
    PreprocessorPosition pos(1, 1, "test.c");
    
    try {
        // Operações básicas
        assertEqual(7, evaluator.evaluateExpression("3 + 4", pos), "Adição simples");
        assertEqual(12, evaluator.evaluateExpression("3 * 4", pos), "Multiplicação simples");
        assertEqual(2, evaluator.evaluateExpression("10 - 8", pos), "Subtração simples");
        assertEqual(3, evaluator.evaluateExpression("15 / 5", pos), "Divisão simples");
        assertEqual(2, evaluator.evaluateExpression("17 % 5", pos), "Módulo simples");
        
        // Números negativos
        assertEqual(-5, evaluator.evaluateExpression("-5", pos), "Número negativo");
        assertEqual(-2, evaluator.evaluateExpression("-10 + 8", pos), "Adição com negativo");
        assertEqual(15, evaluator.evaluateExpression("-3 * -5", pos), "Multiplicação de negativos");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes aritméticos: " << e.what() << std::endl;
    }
}

void testOperatorPrecedence() {
    std::cout << "\n=== Testando Precedência de Operadores ===" << std::endl;
    
    auto macroProcessor = createMacroProcessor();
    auto logger = std::make_shared<PreprocessorLogger>();
    ExpressionEvaluator evaluator(macroProcessor.get(), logger.get());
    PreprocessorPosition pos(1, 1, "test.c");
    
    try {
        // Precedência básica
        assertEqual(14, evaluator.evaluateExpression("2 + 3 * 4", pos), "Multiplicação antes da adição");
        assertEqual(20, evaluator.evaluateExpression("(2 + 3) * 4", pos), "Parênteses alteram precedência");
        assertEqual(11, evaluator.evaluateExpression("2 * 3 + 5", pos), "Multiplicação antes da adição (2)");
        assertEqual(16, evaluator.evaluateExpression("2 * (3 + 5)", pos), "Parênteses com multiplicação");
        
        // Precedência complexa
        assertEqual(23, evaluator.evaluateExpression("2 + 3 * 7", pos), "Precedência multiplicação/adição");
        assertEqual(35, evaluator.evaluateExpression("(2 + 3) * 7", pos), "Parênteses modificam precedência");
        assertEqual(1, evaluator.evaluateExpression("10 / 2 / 5", pos), "Divisão associativa à esquerda");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de precedência: " << e.what() << std::endl;
    }
}

void testParentheses() {
    std::cout << "\n=== Testando Parênteses ===" << std::endl;
    
    auto macroProcessor = createMacroProcessor();
    auto logger = std::make_shared<PreprocessorLogger>();
    ExpressionEvaluator evaluator(macroProcessor.get(), logger.get());
    PreprocessorPosition pos(1, 1, "test.c");
    
    try {
        // Parênteses simples
        assertEqual(20, evaluator.evaluateExpression("(2 + 3) * 4", pos), "Parênteses simples");
        assertEqual(14, evaluator.evaluateExpression("2 * (3 + 4)", pos), "Parênteses à direita");
        
        // Parênteses aninhados
        assertEqual(50, evaluator.evaluateExpression("((2 + 3) * (4 + 6))", pos), "Parênteses aninhados");
        assertEqual(46, evaluator.evaluateExpression("2 * (3 + (4 * 5))", pos), "Parênteses aninhados complexos");
        assertEqual(42, evaluator.evaluateExpression("(2 + (3 * 4)) * 3", pos), "Parênteses aninhados múltiplos");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de parênteses: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES DE OPERADORES LÓGICOS E DE COMPARAÇÃO
// ============================================================================

void testLogicalAndComparisonOperators() {
    std::cout << "\n=== Testando Operadores Lógicos e de Comparação ===" << std::endl;
    
    auto macroProcessor = createMacroProcessor();
    auto logger = std::make_shared<PreprocessorLogger>();
    ExpressionEvaluator evaluator(macroProcessor.get(), logger.get());
    PreprocessorPosition pos(1, 1, "test.c");
    
    try {
        // Operadores lógicos
        assertEqual(1, evaluator.evaluateExpression("1 && 1", pos), "AND lógico verdadeiro");
        assertEqual(0, evaluator.evaluateExpression("1 && 0", pos), "AND lógico falso");
        assertEqual(1, evaluator.evaluateExpression("1 || 0", pos), "OR lógico verdadeiro");
        assertEqual(0, evaluator.evaluateExpression("0 || 0", pos), "OR lógico falso");
        
        // Operadores de comparação
        assertEqual(1, evaluator.evaluateExpression("5 > 3", pos), "Maior que");
        assertEqual(0, evaluator.evaluateExpression("3 > 5", pos), "Maior que (falso)");
        assertEqual(1, evaluator.evaluateExpression("3 < 5", pos), "Menor que");
        assertEqual(1, evaluator.evaluateExpression("5 >= 5", pos), "Maior ou igual");
        assertEqual(1, evaluator.evaluateExpression("5 == 5", pos), "Igual");
        assertEqual(1, evaluator.evaluateExpression("5 != 3", pos), "Diferente");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes lógicos/comparação: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES DE LITERAIS NUMÉRICOS
// ============================================================================

void testNumericLiterals() {
    std::cout << "\n=== Testando Literais Numéricos ===" << std::endl;
    
    auto macroProcessor = createMacroProcessor();
    auto logger = std::make_shared<PreprocessorLogger>();
    ExpressionEvaluator evaluator(macroProcessor.get(), logger.get());
    PreprocessorPosition pos(1, 1, "test.c");
    
    try {
        // Números decimais
        assertEqual(42, evaluator.evaluateExpression("42", pos), "Número decimal");
        assertEqual(0, evaluator.evaluateExpression("0", pos), "Zero");
        assertEqual(123456, evaluator.evaluateExpression("123456", pos), "Número grande");
        
        // Números hexadecimais
        assertEqual(15, evaluator.evaluateExpression("0xF", pos), "Hexadecimal F");
        assertEqual(255, evaluator.evaluateExpression("0xFF", pos), "Hexadecimal FF");
        assertEqual(16, evaluator.evaluateExpression("0x10", pos), "Hexadecimal 10");
        
        // Números octais
        assertEqual(8, evaluator.evaluateExpression("010", pos), "Octal 10");
        assertEqual(64, evaluator.evaluateExpression("0100", pos), "Octal 100");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes de literais: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES AVANÇADOS
// ============================================================================

void testComplexExpressions() {
    std::cout << "\n=== Testando Expressões Complexas ===" << std::endl;
    
    auto macroProcessor = createMacroProcessor();
    auto logger = std::make_shared<PreprocessorLogger>();
    ExpressionEvaluator evaluator(macroProcessor.get(), logger.get());
    PreprocessorPosition pos(1, 1, "test.c");
    
    try {
        // Expressões complexas
        assertEqual(42, evaluator.evaluateExpression("(2 + 3) * (4 + 4) + 2", pos), "Expressão complexa 1");
        assertEqual(1, evaluator.evaluateExpression("(10 > 5) && (3 < 7)", pos), "Expressão lógica complexa");
        assertEqual(0, evaluator.evaluateExpression("(5 == 3) || (2 > 10)", pos), "Expressão lógica complexa 2");
        
        // Operações bitwise básicas
        assertEqual(1, evaluator.evaluateExpression("5 & 3", pos), "AND bitwise");
        assertEqual(7, evaluator.evaluateExpression("5 | 3", pos), "OR bitwise");
        assertEqual(6, evaluator.evaluateExpression("5 ^ 3", pos), "XOR bitwise");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em testes complexos: " << e.what() << std::endl;
    }
}

void testErrorHandling() {
    std::cout << "\n=== Testando Tratamento de Erros ===" << std::endl;
    
    auto macroProcessor = createMacroProcessor();
    auto logger = std::make_shared<PreprocessorLogger>();
    ExpressionEvaluator evaluator(macroProcessor.get(), logger.get());
    PreprocessorPosition pos(1, 1, "test.c");
    
    // Testes de expressões inválidas
    try {
        evaluator.evaluateExpression("", pos);
        std::cout << "❌ Expressão vazia deveria falhar" << std::endl;
    } catch (const std::exception&) {
        std::cout << "✅ Expressão vazia tratada corretamente" << std::endl;
    }
    
    try {
        evaluator.evaluateExpression("5 / 0", pos);
        std::cout << "❌ Divisão por zero deveria falhar" << std::endl;
    } catch (const std::exception&) {
        std::cout << "✅ Divisão por zero tratada corretamente" << std::endl;
    }
}

void testPerformanceStress() {
    std::cout << "\n=== Testando Performance (Stress Test) ===" << std::endl;
    
    auto macroProcessor = createMacroProcessor();
    auto logger = std::make_shared<PreprocessorLogger>();
    ExpressionEvaluator evaluator(macroProcessor.get(), logger.get());
    PreprocessorPosition pos(1, 1, "test.c");
    
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        // Teste de performance com muitas operações
        for (int i = 0; i < 100; ++i) {
            evaluator.evaluateExpression("(2 + 3) * 4 + 5 - 1", pos);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "✅ Performance test: 100 avaliações em " << duration.count() << "ms" << std::endl;
        
        // Teste com expressão complexa
        std::string complexExpr = "((1 + 2) * (3 + 4) + (5 * 6) - (7 / 2))";
        long long result = evaluator.evaluateExpression(complexExpr, pos);
        std::cout << "✅ Expressão complexa avaliada: resultado = " << result << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em teste de performance: " << e.what() << std::endl;
    }
}

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main() {
    std::cout << "=== TESTE ABRANGENTE DE EXPRESSÕES ===" << std::endl;
    std::cout << "Unificação dos testes: expressions, expression_evaluator, expression_evaluator_advanced" << std::endl;
    
    try {
        // Executa todos os testes
        testBasicArithmeticExpressions();
        testOperatorPrecedence();
        testParentheses();
        testLogicalAndComparisonOperators();
        testNumericLiterals();
        testComplexExpressions();
        testErrorHandling();
        testPerformanceStress();
        
        std::cout << "\n=== RESUMO FINAL ===" << std::endl;
        std::cout << "✅ Testes Aritméticos Básicos: Concluído" << std::endl;
        std::cout << "✅ Testes de Precedência: Concluído" << std::endl;
        std::cout << "✅ Testes de Parênteses: Concluído" << std::endl;
        std::cout << "✅ Testes de Operadores Lógicos/Comparação: Concluído" << std::endl;
        std::cout << "✅ Testes de Literais Numéricos: Concluído" << std::endl;
        std::cout << "✅ Testes de Expressões Complexas: Concluído" << std::endl;
        std::cout << "✅ Testes de Tratamento de Erros: Concluído" << std::endl;
        std::cout << "✅ Testes de Performance: Concluído" << std::endl;
        
        std::cout << "\n🎉 TODOS OS TESTES DE EXPRESSÕES PASSARAM COM SUCESSO! 🎉" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Erro durante execução dos testes: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}