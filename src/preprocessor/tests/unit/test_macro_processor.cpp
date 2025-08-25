#include "../../include/macro_processor.hpp"
#include <iostream>
#include <cassert>
#include <sstream>
#include <memory>

using namespace Preprocessor;

// Funções auxiliares para testes
void assertEqual(const std::string& expected, const std::string& actual, const std::string& testName) {
    if (expected != actual) {
        std::cerr << "FALHA em " << testName << ": esperado '" << expected 
                  << "', obtido '" << actual << "'" << std::endl;
        exit(1);
    }
}

void assertTrue(bool condition, const std::string& testName) {
    if (!condition) {
        std::cerr << "FALHA em " << testName << ": condição falsa" << std::endl;
        exit(1);
    }
}

void assertFalse(bool condition, const std::string& testName) {
    if (condition) {
        std::cerr << "FALHA em " << testName << ": condição verdadeira" << std::endl;
        exit(1);
    }
}

// ============================================================================
// TESTES DA STRUCT MacroInfo
// ============================================================================

void testMacroInfoConstructors() {
    std::cout << "Testando construtores de MacroInfo..." << std::endl;
    
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
    
    std::cout << "✓ Testes de construtores de MacroInfo passaram" << std::endl;
}

void testMacroInfoOperators() {
    std::cout << "Testando operadores de MacroInfo..." << std::endl;
    
    MacroInfo info1("TEST", "123", MacroType::OBJECT_LIKE);
    MacroInfo info2("TEST", "123", MacroType::OBJECT_LIKE);
    MacroInfo info3("TEST", "456", MacroType::OBJECT_LIKE);
    
    // Teste de igualdade
    assertTrue(info1 == info2, "MacroInfo operador == - iguais");
    assertFalse(info1 == info3, "MacroInfo operador == - diferentes");
    
    // Teste de desigualdade
    assertFalse(info1 != info2, "MacroInfo operador != - iguais");
    assertTrue(info1 != info3, "MacroInfo operador != - diferentes");
    
    std::cout << "✓ Testes de operadores de MacroInfo passaram" << std::endl;
}

void testMacroInfoMethods() {
    std::cout << "Testando métodos de MacroInfo..." << std::endl;
    
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
    
    // Macro variádica
    MacroInfo variadic("VAR", "printf(__VA_ARGS__)", {"fmt"}, true);
    assertTrue(variadic.isFunctionLike(), "MacroInfo isFunctionLike - variádica");
    assertTrue(variadic.hasParameters(), "MacroInfo hasParameters - variádica");
    assertTrue(variadic.getParameterCount() == 1, "MacroInfo getParameterCount - variádica");
    
    // Teste toString
    std::string str = simple.toString();
    assertTrue(str.find("SIMPLE") != std::string::npos, "MacroInfo toString - contém nome");
    assertTrue(str.find("42") != std::string::npos, "MacroInfo toString - contém valor");
    
    std::cout << "✓ Testes de métodos de MacroInfo passaram" << std::endl;
}

// ============================================================================
// TESTES DA CLASSE MacroProcessor
// ============================================================================

void testMacroProcessorConstructors() {
    std::cout << "Testando construtores de MacroProcessor..." << std::endl;
    
    // Construtor padrão
    MacroProcessor processor1;
    assertTrue(processor1.isDefined("__FILE__"), "MacroProcessor construtor padrão - macros predefinidas");
    assertTrue(processor1.isDefined("__LINE__"), "MacroProcessor construtor padrão - __LINE__");
    assertTrue(processor1.isDefined("__DATE__"), "MacroProcessor construtor padrão - __DATE__");
    assertTrue(processor1.isDefined("__TIME__"), "MacroProcessor construtor padrão - __TIME__");
    
    // Construtor com logger e state
    auto logger = std::make_shared<Preprocessor::PreprocessorLogger>();
    auto state = std::make_shared<Preprocessor::PreprocessorState>();
    MacroProcessor processor2(logger, state);
    assertTrue(processor2.isDefined("__STDC__"), "MacroProcessor construtor com componentes - __STDC__");
    
    std::cout << "✓ Testes de construtores de MacroProcessor passaram" << std::endl;
}

void testBasicMacroOperations() {
    std::cout << "Testando operações básicas de macro..." << std::endl;
    
    MacroProcessor processor;
    Preprocessor::PreprocessorPosition pos("test.c", 1, 1);
    
    // Teste de definição de macro simples
    assertTrue(processor.defineMacro("PI", "3.14159", pos), "defineMacro - sucesso");
    assertTrue(processor.isDefined("PI"), "isDefined - macro definida");
    assertEqual("3.14159", processor.getMacroValue("PI"), "getMacroValue - valor correto");
    
    // Teste de macro inválida
    assertFalse(processor.defineMacro("123INVALID", "value", pos), "defineMacro - nome inválido");
    assertFalse(processor.defineMacro("", "value", pos), "defineMacro - nome vazio");
    
    // Teste de remoção de macro
    assertTrue(processor.undefineMacro("PI"), "undefineMacro - sucesso");
    assertFalse(processor.isDefined("PI"), "isDefined - macro removida");
    assertEqual("", processor.getMacroValue("PI"), "getMacroValue - macro removida");
    
    // Teste de remoção de macro inexistente
    assertFalse(processor.undefineMacro("NONEXISTENT"), "undefineMacro - macro inexistente");
    
    // Teste de tentativa de remover macro predefinida
    assertFalse(processor.undefineMacro("__FILE__"), "undefineMacro - macro predefinida");
    
    std::cout << "✓ Testes de operações básicas de macro passaram" << std::endl;
}

void testMacroExpansion() {
    std::cout << "Testando expansão de macros..." << std::endl;
    
    MacroProcessor processor;
    Preprocessor::PreprocessorPosition pos("test.c", 1, 1);
    
    // Teste de expansão simples
    processor.defineMacro("MAX_SIZE", "1024", pos);
    assertEqual("1024", processor.expandMacro("MAX_SIZE"), "expandMacro - simples");
    
    // Teste de expansão de macro inexistente
    assertEqual("UNDEFINED", processor.expandMacro("UNDEFINED"), "expandMacro - inexistente");
    
    // Teste de expansão recursiva
    processor.defineMacro("A", "B", pos);
    processor.defineMacro("B", "C", pos);
    processor.defineMacro("C", "42", pos);
    assertEqual("42", processor.expandMacro("A"), "expandMacro - recursiva");
    
    // Teste de processamento de linha
    processor.defineMacro("SIZE", "100", pos);
    processor.defineMacro("TYPE", "int", pos);
    std::string line = "TYPE array[SIZE];";
    std::string expanded = processor.processLine(line);
    assertEqual("int array[100];", expanded, "processLine - múltiplas macros");
    
    std::cout << "✓ Testes de expansão de macros passaram" << std::endl;
}

void testFunctionMacros() {
    std::cout << "Testando macros funcionais..." << std::endl;
    
    MacroProcessor processor;
    Preprocessor::PreprocessorPosition pos("test.c", 1, 1);
    
    // Teste de definição de macro funcional
    std::vector<std::string> params = {"x", "y"};
    assertTrue(processor.defineFunctionMacro("ADD", params, "(x) + (y)", false, pos), 
               "defineFunctionMacro - sucesso");
    assertTrue(processor.isDefined("ADD"), "isDefined - macro funcional");
    
    // Teste de expansão de macro funcional
    std::vector<std::string> args = {"5", "3"};
    assertEqual("(5) + (3)", processor.expandFunctionMacro("ADD", args), 
                "expandFunctionMacro - básica");
    
    // Teste com argumentos complexos
    std::vector<std::string> complexArgs = {"a + b", "c * d"};
    assertEqual("(a + b) + (c * d)", processor.expandFunctionMacro("ADD", complexArgs), 
                "expandFunctionMacro - argumentos complexos");
    
    // Teste de macro com número incorreto de argumentos
    std::vector<std::string> wrongArgs = {"1"};
    assertEqual("ADD", processor.expandFunctionMacro("ADD", wrongArgs), 
                "expandFunctionMacro - argumentos insuficientes");
    
    std::cout << "✓ Testes de macros funcionais passaram" << std::endl;
}

void testVariadicMacros() {
    std::cout << "Testando macros variádicas..." << std::endl;
    
    MacroProcessor processor;
    Preprocessor::PreprocessorPosition pos("test.c", 1, 1);
    
    // Teste de definição de macro variádica
    std::vector<std::string> params = {"fmt"};
    assertTrue(processor.defineFunctionMacro("PRINTF", params, "printf(fmt, __VA_ARGS__)", true, pos), 
               "defineFunctionMacro - variádica");
    
    // Teste de expansão com argumentos variádicos
    std::vector<std::string> args = {"\"Hello %s %d\"", "\"World\"", "42"};
    std::string result = processor.expandFunctionMacro("PRINTF", args);
    assertTrue(result.find("printf") != std::string::npos, "expandFunctionMacro - variádica contém printf");
    assertTrue(result.find("Hello %s %d") != std::string::npos, "expandFunctionMacro - variádica contém formato");
    
    std::cout << "✓ Testes de macros variádicas passaram" << std::endl;
}

void testMacroValidation() {
    std::cout << "Testando validação de macros..." << std::endl;
    
    MacroProcessor processor;
    Preprocessor::PreprocessorPosition pos("test.c", 1, 1);
    
    // Teste de nomes válidos
    assertTrue(processor.defineMacro("VALID_NAME", "value", pos), "validação - nome válido");
    assertTrue(processor.defineMacro("_UNDERSCORE", "value", pos), "validação - underscore inicial");
    assertTrue(processor.defineMacro("name123", "value", pos), "validação - com números");
    
    // Teste de nomes inválidos
    assertFalse(processor.defineMacro("123invalid", "value", pos), "validação - inicia com número");
    assertFalse(processor.defineMacro("invalid-name", "value", pos), "validação - hífen");
    assertFalse(processor.defineMacro("invalid name", "value", pos), "validação - espaço");
    assertFalse(processor.defineMacro("int", "value", pos), "validação - palavra reservada");
    
    std::cout << "✓ Testes de validação de macros passaram" << std::endl;
}

void testMacroRedefinition() {
    std::cout << "Testando redefinição de macros..." << std::endl;
    
    MacroProcessor processor;
    Preprocessor::PreprocessorPosition pos("test.c", 1, 1);
    
    // Primeira definição
    assertTrue(processor.defineMacro("TEST", "123", pos), "redefinição - primeira definição");
    assertEqual("123", processor.getMacroValue("TEST"), "redefinição - valor inicial");
    
    // Redefinição idêntica (deve ser permitida)
    assertTrue(processor.defineMacro("TEST", "123", pos), "redefinição - idêntica");
    assertEqual("123", processor.getMacroValue("TEST"), "redefinição - valor mantido");
    
    // Redefinição diferente (deve gerar aviso mas ser permitida)
    assertTrue(processor.defineMacro("TEST", "456", pos), "redefinição - diferente");
    assertEqual("456", processor.getMacroValue("TEST"), "redefinição - novo valor");
    
    // Tentativa de redefinir macro predefinida (deve falhar)
    assertFalse(processor.defineMacro("__FILE__", "test", pos), "redefinição - macro predefinida");
    
    std::cout << "✓ Testes de redefinição de macros passaram" << std::endl;
}

void testMacroStatistics() {
    std::cout << "Testando estatísticas de macros..." << std::endl;
    
    MacroProcessor processor;
    Preprocessor::PreprocessorPosition pos("test.c", 1, 1);
    
    // Define algumas macros e faz expansões
    processor.defineMacro("A", "1", pos);
    processor.defineMacro("B", "2", pos);
    processor.expandMacro("A");
    processor.expandMacro("B");
    processor.expandMacro("A"); // Segunda expansão de A
    
    // Verifica estatísticas
    std::string stats = processor.getStatistics();
    assertTrue(stats.find("Macros definidas:") != std::string::npos, "estatísticas - contém macros definidas");
    assertTrue(stats.find("Expansões totais:") != std::string::npos, "estatísticas - contém expansões");
    
    // Verifica relatório de macros
    std::string report = processor.generateMacroReport();
    assertTrue(report.find("A") != std::string::npos, "relatório - contém macro A");
    assertTrue(report.find("B") != std::string::npos, "relatório - contém macro B");
    
    // Reset de estatísticas
    processor.resetStatistics();
    std::string newStats = processor.getStatistics();
    assertTrue(newStats.find("Expansões totais: 0") != std::string::npos, "reset - expansões zeradas");
    
    std::cout << "✓ Testes de estatísticas de macros passaram" << std::endl;
}

void testUtilityFunctions() {
    std::cout << "Testando funções utilitárias..." << std::endl;
    
    // Teste macroTypeToString
    assertEqual("Object-like", macroTypeToString(MacroType::OBJECT_LIKE), "macroTypeToString - OBJECT_LIKE");
    assertEqual("Function-like", macroTypeToString(MacroType::FUNCTION_LIKE), "macroTypeToString - FUNCTION_LIKE");
    assertEqual("Variadic", macroTypeToString(MacroType::VARIADIC), "macroTypeToString - VARIADIC");
    
    // Teste isValidMacroNameChar
    assertTrue(isValidMacroNameChar('a', true), "isValidMacroNameChar - letra inicial");
    assertTrue(isValidMacroNameChar('_', true), "isValidMacroNameChar - underscore inicial");
    assertFalse(isValidMacroNameChar('1', true), "isValidMacroNameChar - número inicial");
    assertTrue(isValidMacroNameChar('1', false), "isValidMacroNameChar - número não-inicial");
    assertFalse(isValidMacroNameChar('-', false), "isValidMacroNameChar - hífen");
    
    // Teste trimWhitespace
    assertEqual("test", trimWhitespace("  test  "), "trimWhitespace - espaços");
    assertEqual("test", trimWhitespace("\t\ntest\r\n"), "trimWhitespace - whitespace");
    assertEqual("", trimWhitespace("   "), "trimWhitespace - só espaços");
    assertEqual("test", trimWhitespace("test"), "trimWhitespace - sem espaços");
    
    // Teste escapeMacroString
    assertEqual("\\\"hello\\\"", escapeMacroString("\"hello\""), "escapeMacroString - aspas");
    assertEqual("hello\\\\world", escapeMacroString("hello\\world"), "escapeMacroString - backslash");
    
    std::cout << "✓ Testes de funções utilitárias passaram" << std::endl;
}

// ============================================================================
// FUNÇÃO PRINCIPAL DE TESTE
// ============================================================================

int main() {
    std::cout << "=== INICIANDO TESTES DO PROCESSADOR DE MACROS ===" << std::endl;
    
    try {
        // Testes da struct MacroInfo
        testMacroInfoConstructors();
        testMacroInfoOperators();
        testMacroInfoMethods();
        
        // Testes da classe MacroProcessor
        testMacroProcessorConstructors();
        testBasicMacroOperations();
        testMacroExpansion();
        testFunctionMacros();
        testVariadicMacros();
        testMacroValidation();
        testMacroRedefinition();
        testMacroStatistics();
        
        // Testes de funções utilitárias
        testUtilityFunctions();
        
        std::cout << "\n=== TODOS OS TESTES PASSARAM COM SUCESSO! ===" << std::endl;
        std::cout << "✓ Struct MacroInfo implementada corretamente" << std::endl;
        std::cout << "✓ Classe MacroProcessor implementada corretamente" << std::endl;
        std::cout << "✓ Operações básicas de macro funcionando" << std::endl;
        std::cout << "✓ Expansão de macros funcionando" << std::endl;
        std::cout << "✓ Macros funcionais implementadas" << std::endl;
        std::cout << "✓ Macros variádicas implementadas" << std::endl;
        std::cout << "✓ Validação de macros funcionando" << std::endl;
        std::cout << "✓ Tratamento de redefinição implementado" << std::endl;
        std::cout << "✓ Sistema de estatísticas funcionando" << std::endl;
        std::cout << "✓ Funções utilitárias implementadas" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERRO durante os testes: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERRO desconhecido durante os testes" << std::endl;
        return 1;
    }
}