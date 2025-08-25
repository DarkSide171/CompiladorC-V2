#include "../../include/expression_evaluator.hpp"
#include "../../include/preprocessor_logger.hpp"
#include <iostream>
#include <cassert>

using namespace Preprocessor;

int main() {
    std::cout << "=== Testando Novos Métodos do ExpressionEvaluator ===\n";
    
    // Criar logger e avaliador
    auto logger = std::make_shared<PreprocessorLogger>();
    ExpressionEvaluator evaluator(nullptr, logger.get());
    PreprocessorPosition pos("test", 1, 1);
    
    // Teste 1: evaluateBooleanExpression
    std::cout << "\n=== Testando evaluateBooleanExpression ===\n";
    
    bool result1 = evaluator.evaluateBooleanExpression("1", pos);
    assert(result1 == true);
    std::cout << "✅ evaluateBooleanExpression(\"1\") = true - PASSOU\n";
    
    bool result2 = evaluator.evaluateBooleanExpression("0", pos);
    assert(result2 == false);
    std::cout << "✅ evaluateBooleanExpression(\"0\") = false - PASSOU\n";
    
    bool result3 = evaluator.evaluateBooleanExpression("5 + 3", pos);
    assert(result3 == true);
    std::cout << "✅ evaluateBooleanExpression(\"5 + 3\") = true - PASSOU\n";
    
    bool result4 = evaluator.evaluateBooleanExpression("10 - 10", pos);
    assert(result4 == false);
    std::cout << "✅ evaluateBooleanExpression(\"10 - 10\") = false - PASSOU\n";
    
    // Teste 2: isValidExpression
    std::cout << "\n=== Testando isValidExpression ===\n";
    
    bool valid1 = evaluator.isValidExpression("1 + 2");
    assert(valid1 == true);
    std::cout << "✅ isValidExpression(\"1 + 2\") = true - PASSOU\n";
    
    bool valid2 = evaluator.isValidExpression("(3 * 4)");
    assert(valid2 == true);
    std::cout << "✅ isValidExpression(\"(3 * 4)\") = true - PASSOU\n";
    
    bool valid3 = evaluator.isValidExpression("1 + +");
    assert(valid3 == false);
    std::cout << "✅ isValidExpression(\"1 + +\") = false - PASSOU\n";
    
    bool valid4 = evaluator.isValidExpression("((1 + 2)");
    assert(valid4 == false);
    std::cout << "✅ isValidExpression(\"((1 + 2)\") = false - PASSOU\n";
    
    bool valid5 = evaluator.isValidExpression("");
    assert(valid5 == false);
    std::cout << "✅ isValidExpression(\"\") = false - PASSOU\n";
    
    // Teste 3: expandMacrosInExpression (sem MacroProcessor)
    std::cout << "\n=== Testando expandMacrosInExpression ===\n";
    
    std::string expanded = evaluator.expandMacrosInExpression("1 + 2", pos);
    assert(expanded == "1 + 2");
    std::cout << "✅ expandMacrosInExpression(\"1 + 2\") = \"1 + 2\" - PASSOU\n";
    
    // Teste 4: Verificar se métodos privados foram implementados (teste indireto)
    std::cout << "\n=== Testando Funcionalidades Internas ===\n";
    
    // Testar números hexadecimais (usa convertToNumber internamente)
    long long hexResult = evaluator.evaluateExpression("0xFF", pos);
    assert(hexResult == 255);
    std::cout << "✅ Avaliação de hexadecimal 0xFF = 255 - PASSOU\n";
    
    // Testar números octais (usa convertToNumber internamente)
    long long octResult = evaluator.evaluateExpression("010", pos);
    assert(octResult == 8);
    std::cout << "✅ Avaliação de octal 010 = 8 - PASSOU\n";
    
    // Testar identificadores não definidos (usa resolveIdentifierValue internamente)
    long long idResult = evaluator.evaluateExpression("UNDEFINED_MACRO", pos);
    assert(idResult == 0);
    std::cout << "✅ Identificador não definido = 0 - PASSOU\n";
    
    std::cout << "\n🎉 Todos os testes dos novos métodos passaram com sucesso!\n";
    
    return 0;
}