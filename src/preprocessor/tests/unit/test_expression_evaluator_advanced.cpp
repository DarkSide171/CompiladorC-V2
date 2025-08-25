// Testes avançados para ExpressionEvaluator
// Casos complexos e edge cases para validação robusta

#include "../../include/expression_evaluator.hpp"
#include "../../include/preprocessor_logger.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <stdexcept>
#include <functional>
#include <climits>

using namespace Preprocessor;

// ============================================================================
// DECLARAÇÕES DE FUNÇÕES DE TESTE AVANÇADAS
// ============================================================================

void testComplexExpressions();
void testEdgeCases();
void testLargeNumbers();
void testNestedComplexity();
void testOperatorCombinations();
void testBoundaryConditions();
void testMalformedExpressions();
void testPerformanceStress();

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
// TESTES DE EXPRESSÕES COMPLEXAS
// ============================================================================

void testComplexExpressions() {
    std::cout << "\n=== Testando Expressões Complexas ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Expressões com múltiplas precedências
    assertEqual(44, evaluator.evaluateExpression("2 + 3 * 4 + 5 * 6", pos), "Múltiplas multiplicações e adições");
    assertEqual(-10, evaluator.evaluateExpression("2 - 3 * 4", pos), "Subtração com multiplicação");
    assertEqual(26, evaluator.evaluateExpression("2 * 3 + 4 * 5", pos), "Duas multiplicações com adição");
    
    // Expressões com divisão e módulo
    assertEqual(3, evaluator.evaluateExpression("15 / 3 - 12 / 6", pos), "Múltiplas divisões");
    assertEqual(6, evaluator.evaluateExpression("17 % 6 + 13 % 4", pos), "Múltiplos módulos");
    assertEqual(2, evaluator.evaluateExpression("(15 + 3) / (4 + 5)", pos), "Divisão de somas");
    
    // Expressões com operadores unários complexos
    assertEqual(-30, evaluator.evaluateExpression("-2 * 3 * 5", pos), "Unário com múltiplas multiplicações");
    assertEqual(30, evaluator.evaluateExpression("-(-2 * 3 * 5)", pos), "Duplo unário");
    assertEqual(-7, evaluator.evaluateExpression("-2 - 5", pos), "Unário seguido de subtração");
    
    // Expressões com parênteses complexos
    assertEqual(100, evaluator.evaluateExpression("((2 + 3) * (4 * 4)) + ((3 - 1) * (5 + 5))", pos), "Parênteses aninhados complexos");
    assertEqual(1, evaluator.evaluateExpression("(((2 + 1) * 3) - 8) / 1", pos), "Três níveis de parênteses");
}

// ============================================================================
// TESTES DE CASOS EXTREMOS
// ============================================================================

void testEdgeCases() {
    std::cout << "\n=== Testando Casos Extremos ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Números com zeros à esquerda
    assertEqual(8, evaluator.evaluateExpression("010", pos), "Octal com zero à esquerda");
    assertEqual(0, evaluator.evaluateExpression("000", pos), "Múltiplos zeros");
    
    // Hexadecimais com casos mistos
    assertEqual(2748, evaluator.evaluateExpression("0xAbC", pos), "Hexadecimal com maiúsculas e minúsculas");
    assertEqual(291, evaluator.evaluateExpression("0x123", pos), "Hexadecimal com números");
    
    // Expressões com espaços variados
    assertEqual(15, evaluator.evaluateExpression("  3   *   5  ", pos), "Múltiplos espaços");
    assertEqual(8, evaluator.evaluateExpression("(2+3)*2-2", pos), "Sem espaços");
    assertEqual(10, evaluator.evaluateExpression(" ( 2 + 3 ) * 2 ", pos), "Espaços em parênteses");
    
    // Operadores unários em contextos diversos
    assertEqual(-1, evaluator.evaluateExpression("-(2-1)", pos), "Unário em parênteses");
    assertEqual(1, evaluator.evaluateExpression("-(-1)", pos), "Duplo unário simples");
    assertEqual(-25, evaluator.evaluateExpression("-5 * 5", pos), "Unário com multiplicação");
}

// ============================================================================
// TESTES COM NÚMEROS GRANDES
// ============================================================================

void testLargeNumbers() {
    std::cout << "\n=== Testando Números Grandes ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Números grandes dentro dos limites
    assertEqual(2000000000LL, evaluator.evaluateExpression("1000000000 + 1000000000", pos), "Soma de números grandes");
    assertEqual(1000000000000000000LL, evaluator.evaluateExpression("1000000000 * 1000000000", pos), "Multiplicação de números grandes");
    
    // Hexadecimais grandes
    assertEqual(4294967295LL, evaluator.evaluateExpression("0xFFFFFFFF", pos), "Hexadecimal máximo 32-bit");
    assertEqual(268435456LL, evaluator.evaluateExpression("0x10000000", pos), "Hexadecimal grande");
    
    // Octais grandes
    assertEqual(134217728LL, evaluator.evaluateExpression("01000000000", pos), "Octal grande");
    assertEqual(511LL, evaluator.evaluateExpression("0777", pos), "Octal máximo 9-bit");
}

// ============================================================================
// TESTES DE COMPLEXIDADE ANINHADA
// ============================================================================

void testNestedComplexity() {
    std::cout << "\n=== Testando Complexidade Aninhada ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Parênteses profundamente aninhados
    assertEqual(44, evaluator.evaluateExpression("((((2 + 3) * 4) + 2) * 2)", pos), "Quatro níveis de aninhamento");
    assertEqual(120, evaluator.evaluateExpression("(((2 * 3) + 4) * ((5 + 1) * 2))", pos), "Aninhamento com múltiplas operações");
    
    // Expressões com múltiplos grupos
    assertEqual(50, evaluator.evaluateExpression("(2 + 3) * (4 + 6)", pos), "Dois grupos independentes");
    assertEqual(60, evaluator.evaluateExpression("(2 + 3) * (4 + 6) + (3 * 2) + 4", pos), "Múltiplos grupos com operações");
    
    // Combinações complexas de operadores
    assertEqual(23, evaluator.evaluateExpression("2 + 3 * 4 + 5 * 2 - 1", pos), "Precedência complexa");
    assertEqual(37, evaluator.evaluateExpression("2 * 3 + 4 * 5 + 5 * 2 + 1", pos), "Múltiplas multiplicações e adições");
}

// ============================================================================
// TESTES DE COMBINAÇÕES DE OPERADORES
// ============================================================================

void testOperatorCombinations() {
    std::cout << "\n=== Testando Combinações de Operadores ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Todas as combinações básicas
    assertEqual(17, evaluator.evaluateExpression("2 + 3 * 5", pos), "Adição e multiplicação");
    assertEqual(13, evaluator.evaluateExpression("2 * 3 + 7", pos), "Multiplicação e adição");
    assertEqual(-4, evaluator.evaluateExpression("2 - 3 * 2", pos), "Subtração e multiplicação");
    assertEqual(4, evaluator.evaluateExpression("2 * 3 - 2", pos), "Multiplicação e subtração");
    
    // Divisão e módulo com outras operações
    assertEqual(7, evaluator.evaluateExpression("15 / 3 + 2", pos), "Divisão e adição");
    assertEqual(3, evaluator.evaluateExpression("15 / 3 - 2", pos), "Divisão e subtração");
    assertEqual(4, evaluator.evaluateExpression("17 % 7 + 1", pos), "Módulo e adição");
    assertEqual(2, evaluator.evaluateExpression("17 % 7 - 1", pos), "Módulo e subtração");
    
    // Operadores unários com binários
    assertEqual(-8, evaluator.evaluateExpression("-2 * 4", pos), "Unário e multiplicação");
    assertEqual(-6, evaluator.evaluateExpression("-2 - 4", pos), "Unário e subtração");
    assertEqual(2, evaluator.evaluateExpression("-2 + 4", pos), "Unário e adição");
    assertEqual(2, evaluator.evaluateExpression("-8 / -4", pos), "Dois unários com divisão");
}

// ============================================================================
// TESTES DE CONDIÇÕES LIMITE
// ============================================================================

void testBoundaryConditions() {
    std::cout << "\n=== Testando Condições Limite ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Divisões que resultam em zero
    assertEqual(0, evaluator.evaluateExpression("1 / 2", pos), "Divisão com truncamento");
    assertEqual(0, evaluator.evaluateExpression("3 / 4", pos), "Divisão menor que 1");
    
    // Módulos com números pequenos
    assertEqual(1, evaluator.evaluateExpression("1 % 2", pos), "Módulo menor que divisor");
    assertEqual(0, evaluator.evaluateExpression("2 % 1", pos), "Módulo por 1");
    
    // Operações que resultam em zero
    assertEqual(0, evaluator.evaluateExpression("5 - 5", pos), "Subtração resultando em zero");
    assertEqual(0, evaluator.evaluateExpression("0 * 1000", pos), "Multiplicação por zero");
    assertEqual(0, evaluator.evaluateExpression("0 + 0", pos), "Adição de zeros");
    
    // Números negativos em operações
    assertEqual(-1, evaluator.evaluateExpression("-1", pos), "Número negativo simples");
    assertEqual(1, evaluator.evaluateExpression("-1 * -1", pos), "Produto de negativos");
    assertEqual(-2, evaluator.evaluateExpression("-1 + -1", pos), "Soma de negativos");
}

// ============================================================================
// TESTES DE EXPRESSÕES MAL FORMADAS
// ============================================================================

void testMalformedExpressions() {
    std::cout << "\n=== Testando Expressões Mal Formadas ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Operadores consecutivos variados
    assertThrows([&]() { evaluator.evaluateExpression("5 + * 3", pos); }, "Operadores diferentes consecutivos");
    assertThrows([&]() { evaluator.evaluateExpression("5 * / 3", pos); }, "Multiplicação e divisão consecutivas");
    assertThrows([&]() { evaluator.evaluateExpression("5 - - 3", pos); }, "Dois menos consecutivos");
    
    // Parênteses mal posicionados
    assertThrows([&]() { evaluator.evaluateExpression(")5 + 3(", pos); }, "Parênteses invertidos");
    assertThrows([&]() { evaluator.evaluateExpression("5 + (3", pos); }, "Parêntese não fechado");
    assertThrows([&]() { evaluator.evaluateExpression("5 + 3)", pos); }, "Parêntese extra");
    
    // Operadores em posições inválidas
    assertThrows([&]() { evaluator.evaluateExpression("* 5", pos); }, "Operador no início");
    assertThrows([&]() { evaluator.evaluateExpression("5 /", pos); }, "Operador no final");
    assertThrows([&]() { evaluator.evaluateExpression("5 + + + 3", pos); }, "Múltiplos operadores consecutivos");
    
    // Expressões vazias ou só com operadores
    assertThrows([&]() { evaluator.evaluateExpression("+", pos); }, "Só operador");
    assertEqual(0, evaluator.evaluateExpression("()", pos), "Parênteses vazios");
    assertEqual(0, evaluator.evaluateExpression("( )", pos), "Parênteses com espaço");
}

// ============================================================================
// TESTES DE STRESS DE PERFORMANCE
// ============================================================================

void testPerformanceStress() {
    std::cout << "\n=== Testando Stress de Performance ===" << std::endl;
    
    PreprocessorLogger logger;
    ExpressionEvaluator evaluator(nullptr, &logger);
    PreprocessorPosition pos("test", 1, 1);
    
    // Expressão longa com muitas operações
    std::string longExpr = "1";
    for (int i = 2; i <= 50; ++i) {
        longExpr += " + " + std::to_string(i);
    }
    assertEqual(1275, evaluator.evaluateExpression(longExpr, pos), "Expressão longa com 50 termos");
    
    // Expressão com muitos parênteses aninhados
    std::string nestedExpr = "1";
    for (int i = 0; i < 10; ++i) {
        nestedExpr = "(" + nestedExpr + " + 1)";
    }
    assertEqual(11, evaluator.evaluateExpression(nestedExpr, pos), "10 níveis de parênteses aninhados");
    
    // Expressão com alternância de operações
    std::string alternatingExpr = "100";
    for (int i = 0; i < 20; ++i) {
        if (i % 2 == 0) {
            alternatingExpr += " - 1";
        } else {
            alternatingExpr += " + 1";
        }
    }
    assertEqual(100, evaluator.evaluateExpression(alternatingExpr, pos), "Alternância de operações");
}

// ============================================================================
// FUNÇÃO PRINCIPAL DE TESTES AVANÇADOS
// ============================================================================

int main() {
    std::cout << "Executando Testes Avançados do ExpressionEvaluator..." << std::endl;
    
    try {
        testComplexExpressions();
        testEdgeCases();
        testLargeNumbers();
        testNestedComplexity();
        testOperatorCombinations();
        testBoundaryConditions();
        testMalformedExpressions();
        testPerformanceStress();
        
        std::cout << "\n🎉 Todos os testes avançados do ExpressionEvaluator passaram com sucesso!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n💥 Erro durante os testes avançados: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}