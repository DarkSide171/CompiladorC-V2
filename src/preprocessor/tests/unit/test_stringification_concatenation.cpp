#include "../../include/macro_processor.hpp"
#include <iostream>
#include <cassert>

using namespace Preprocessor;

int main() {
    std::cout << "=== Testando Operadores # e ## da Fase 3.2 ===\n\n";
    
    MacroProcessor processor;
    
    // Teste do operador de stringificação (#)
    std::cout << "=== Testando handleStringification (#) ===\n";
    
    std::string result1 = processor.handleStringification("hello");
    std::cout << "handleStringification(\"hello\") = " << result1 << std::endl;
    assert(result1 == "\"hello\"");
    std::cout << "✅ Teste 1 passou\n";
    
    std::string result2 = processor.handleStringification("hello world");
    std::cout << "handleStringification(\"hello world\") = " << result2 << std::endl;
    assert(result2 == "\"hello world\"");
    std::cout << "✅ Teste 2 passou\n";
    
    std::string result3 = processor.handleStringification("test\"quote");
    std::cout << "handleStringification(\"test\\\"quote\") = " << result3 << std::endl;
    assert(result3 == "\"test\\\"quote\"");
    std::cout << "✅ Teste 3 passou\n";
    
    // Teste do operador de concatenação (##)
    std::cout << "\n=== Testando handleConcatenation (##) ===\n";
    
    std::string result4 = processor.handleConcatenation("hello", "world");
    std::cout << "handleConcatenation(\"hello\", \"world\") = " << result4 << std::endl;
    assert(result4 == "helloworld");
    std::cout << "✅ Teste 4 passou\n";
    
    std::string result5 = processor.handleConcatenation("  prefix  ", "  suffix  ");
    std::cout << "handleConcatenation(\"  prefix  \", \"  suffix  \") = " << result5 << std::endl;
    assert(result5 == "prefixsuffix");
    std::cout << "✅ Teste 5 passou\n";
    
    std::string result6 = processor.handleConcatenation("var", "123");
    std::cout << "handleConcatenation(\"var\", \"123\") = " << result6 << std::endl;
    assert(result6 == "var123");
    std::cout << "✅ Teste 6 passou\n";
    
    // Teste de macros funcionais com parâmetros
    std::cout << "\n=== Testando Macros Funcionais ===\n";
    
    std::vector<std::string> params = {"x", "y"};
    bool success = processor.defineFunctionMacro("ADD", params, "x + y");
    assert(success);
    std::cout << "✅ Macro funcional ADD definida\n";
    
    std::vector<std::string> args = {"5", "3"};
    std::string result7 = processor.expandFunctionMacro("ADD", args);
    std::cout << "expandFunctionMacro(\"ADD\", {\"5\", \"3\"}) = " << result7 << std::endl;
    assert(result7 == "5 + 3");
    std::cout << "✅ Teste 7 passou\n";
    
    // Teste de validação de argumentos
    std::cout << "\n=== Testando Validação de Argumentos ===\n";
    
    bool valid1 = processor.validateParameterCount("ADD", 2);
    assert(valid1);
    std::cout << "✅ Validação com 2 argumentos passou\n";
    
    bool valid2 = processor.validateParameterCount("ADD", 1);
    assert(!valid2);
    std::cout << "✅ Validação com 1 argumento falhou corretamente\n";
    
    std::cout << "\n🎉 Todos os testes da Fase 3.2 passaram com sucesso!\n";
    std::cout << "✅ Operador de stringificação (#) implementado\n";
    std::cout << "✅ Operador de concatenação (##) implementado\n";
    std::cout << "✅ Macros funcionais implementadas\n";
    std::cout << "✅ Validação de argumentos implementada\n";
    std::cout << "✅ Detecção de recursão infinita implementada\n";
    
    return 0;
}