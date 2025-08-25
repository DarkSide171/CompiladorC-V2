// Testes unitários para ExpressionEvaluator
// Validação completa das funcionalidades do avaliador de expressões

#include "../../include/expression_evaluator.hpp"
#include "../../include/preprocessor_logger.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <stdexcept>
#include <functional>

using namespace Preprocessor;

// ============================================================================
// DECLARAÇÕES DE FUNÇÕES DE TESTE
// ============================================================================

void testBasicArithmetic();
void testOperatorPrecedence();
void testParentheses();
void testNumericLiterals();
void testHexadecimalNumbers();
void testOctalNumbers();
void testErrorHandling();

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

void assertFalse(bool condition, const std::string& testName) {
    if (!condition) {
        std::cout << "✅ " << testName << " - PASSOU" << std::endl;
    } else {
        std::cout << "❌ " << testName << " - FALHOU" << std::endl;
        assert(false);
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
// TESTES DE ARITMÉTICA BÁSICA
// ============================================================================

void testBasicArithmetic() {
    std::cout << "\n=== Testando Aritmética Básica ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Testes de adição
    assertEqual(5, evaluator.evaluateExpression("2 + 3", pos), "Adição simples");
    assertEqual(10, evaluator.evaluateExpression("7 + 3", pos), "Adição com números maiores");
    assertEqual(0, evaluator.evaluateExpression("5 + (-5)", pos), "Adição com número negativo");
    
    // Testes de subtração
    assertEqual(2, evaluator.evaluateExpression("5 - 3", pos), "Subtração simples");
    assertEqual(-2, evaluator.evaluateExpression("3 - 5", pos), "Subtração com resultado negativo");
    assertEqual(10, evaluator.evaluateExpression("5 - (-5)", pos), "Subtração de número negativo");
    
    // Testes de multiplicação
    assertEqual(15, evaluator.evaluateExpression("3 * 5", pos), "Multiplicação simples");
    assertEqual(0, evaluator.evaluateExpression("0 * 100", pos), "Multiplicação por zero");
    assertEqual(-15, evaluator.evaluateExpression("3 * (-5)", pos), "Multiplicação com negativo");
    
    // Testes de divisão
    assertEqual(3, evaluator.evaluateExpression("15 / 5", pos), "Divisão simples");
    assertEqual(2, evaluator.evaluateExpression("7 / 3", pos), "Divisão com resto (truncada)");
    assertEqual(-2, evaluator.evaluateExpression("7 / (-3)", pos), "Divisão com negativo");
    
    // Testes de módulo
    assertEqual(1, evaluator.evaluateExpression("7 % 3", pos), "Módulo simples");
    assertEqual(0, evaluator.evaluateExpression("6 % 3", pos), "Módulo com resto zero");
    assertEqual(2, evaluator.evaluateExpression("8 % 3", pos), "Módulo com resto");
}

// ============================================================================
// TESTES DE PRECEDÊNCIA DE OPERADORES
// ============================================================================

void testOperatorPrecedence() {
    std::cout << "\n=== Testando Precedência de Operadores ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Multiplicação antes de adição
    assertEqual(14, evaluator.evaluateExpression("2 + 3 * 4", pos), "Multiplicação tem precedência sobre adição");
    assertEqual(20, evaluator.evaluateExpression("(2 + 3) * 4", pos), "Parênteses alteram precedência");
    
    // Divisão antes de subtração
    assertEqual(5, evaluator.evaluateExpression("11 - 12 / 2", pos), "Divisão tem precedência sobre subtração");
    assertEqual(0, evaluator.evaluateExpression("(11 - 12) / 2", pos), "Parênteses alteram precedência na divisão");
    
    // Múltiplas operações
    assertEqual(23, evaluator.evaluateExpression("2 + 3 * 4 + 5 * 2 - 1", pos), "Múltiplas operações com precedência");
    assertEqual(1, evaluator.evaluateExpression("10 - 3 * 2 - 3", pos), "Subtração e multiplicação");
    
    // Operadores unários
    assertEqual(-14, evaluator.evaluateExpression("-2 - 3 * 4", pos), "Operador unário com precedência");
    assertEqual(10, evaluator.evaluateExpression("-2 * -5", pos), "Dois operadores unários");
}

// ============================================================================
// TESTES DE PARÊNTESES
// ============================================================================

void testParentheses() {
    std::cout << "\n=== Testando Parênteses ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Parênteses simples
    assertEqual(20, evaluator.evaluateExpression("(2 + 3) * 4", pos), "Parênteses simples");
    assertEqual(14, evaluator.evaluateExpression("2 * (3 + 4)", pos), "Parênteses à direita");
    
    // Parênteses aninhados
    assertEqual(50, evaluator.evaluateExpression("((2 + 3) * (4 + 6))", pos), "Parênteses aninhados");
    assertEqual(46, evaluator.evaluateExpression("2 * (3 + (4 * 5))", pos), "Parênteses aninhados complexos");
    
    // Múltiplos grupos de parênteses
    assertEqual(35, evaluator.evaluateExpression("(2 + 3) * (4 + 3)", pos), "Múltiplos grupos");
    assertEqual(1, evaluator.evaluateExpression("(10 - 5) - (2 + 2)", pos), "Subtração com parênteses");
}

// ============================================================================
// TESTES DE LITERAIS NUMÉRICOS
// ============================================================================

void testNumericLiterals() {
    std::cout << "\n=== Testando Literais Numéricos ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Números decimais
    assertEqual(42, evaluator.evaluateExpression("42", pos), "Número decimal simples");
    assertEqual(0, evaluator.evaluateExpression("0", pos), "Zero");
    assertEqual(1234567890, evaluator.evaluateExpression("1234567890", pos), "Número grande");
    
    // Números com operações
    assertEqual(84, evaluator.evaluateExpression("42 * 2", pos), "Operação com literal");
    assertEqual(44, evaluator.evaluateExpression("42 + 2", pos), "Adição com literal");
}

void testHexadecimalNumbers() {
    std::cout << "\n=== Testando Números Hexadecimais ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Números hexadecimais básicos
    assertEqual(15, evaluator.evaluateExpression("0xF", pos), "Hexadecimal F");
    assertEqual(255, evaluator.evaluateExpression("0xFF", pos), "Hexadecimal FF");
    assertEqual(16, evaluator.evaluateExpression("0x10", pos), "Hexadecimal 10");
    assertEqual(0, evaluator.evaluateExpression("0x0", pos), "Hexadecimal 0");
    
    // Hexadecimais com letras minúsculas
    assertEqual(15, evaluator.evaluateExpression("0xf", pos), "Hexadecimal f minúsculo");
    assertEqual(255, evaluator.evaluateExpression("0xff", pos), "Hexadecimal ff minúsculo");
    assertEqual(171, evaluator.evaluateExpression("0xab", pos), "Hexadecimal ab");
    
    // Operações com hexadecimais
    assertEqual(31, evaluator.evaluateExpression("0xF + 0x10", pos), "Soma de hexadecimais");
    assertEqual(240, evaluator.evaluateExpression("0xF * 0x10", pos), "Multiplicação de hexadecimais");
}

void testOctalNumbers() {
    std::cout << "\n=== Testando Números Octais ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Números octais básicos
    assertEqual(8, evaluator.evaluateExpression("010", pos), "Octal 10");
    assertEqual(64, evaluator.evaluateExpression("0100", pos), "Octal 100");
    assertEqual(7, evaluator.evaluateExpression("07", pos), "Octal 7");
    assertEqual(0, evaluator.evaluateExpression("00", pos), "Octal 0");
    
    // Operações com octais
    assertEqual(15, evaluator.evaluateExpression("07 + 010", pos), "Soma de octais");
    assertEqual(56, evaluator.evaluateExpression("07 * 010", pos), "Multiplicação de octais");
}

// ============================================================================
// TESTES DE TRATAMENTO DE ERROS
// ============================================================================

void testErrorHandling() {
    std::cout << "\n=== Testando Tratamento de Erros ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Divisão por zero
    assertThrows([&]() { evaluator.evaluateExpression("5 / 0", pos); }, "Divisão por zero");
    assertThrows([&]() { evaluator.evaluateExpression("10 % 0", pos); }, "Módulo por zero");
    
    // Parênteses desbalanceados
    assertThrows([&]() { evaluator.evaluateExpression("(5 + 3", pos); }, "Parênteses não fechados");
    assertThrows([&]() { evaluator.evaluateExpression("5 + 3)", pos); }, "Parênteses extras");
    assertThrows([&]() { evaluator.evaluateExpression("((5 + 3)", pos); }, "Parênteses aninhados desbalanceados");
    
    // Expressões vazias ou inválidas
    assertThrows([&]() { evaluator.evaluateExpression("", pos); }, "Expressão vazia");
    assertThrows([&]() { evaluator.evaluateExpression("   ", pos); }, "Expressão só com espaços");
    
    // Operadores inválidos
    assertThrows([&]() { evaluator.evaluateExpression("5 + + 3", pos); }, "Operadores consecutivos");
    assertThrows([&]() { evaluator.evaluateExpression("5 +", pos); }, "Operador sem operando");
    assertThrows([&]() { evaluator.evaluateExpression("+ 5 +", pos); }, "Operador no final");
}

// ============================================================================
// FUNÇÃO PRINCIPAL DE TESTES
// ============================================================================

int main() {
    std::cout << "Executando Testes do ExpressionEvaluator..." << std::endl;
    
    try {
        testBasicArithmetic();
        testOperatorPrecedence();
        testParentheses();
        testNumericLiterals();
        testHexadecimalNumbers();
        testOctalNumbers();
        testErrorHandling();
        
        std::cout << "\n🎉 Todos os testes do ExpressionEvaluator passaram com sucesso!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n💥 Erro durante os testes: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}