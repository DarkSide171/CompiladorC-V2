// Teste Abrangente de Diretivas - Unificação dos testes de diretivas
// Consolida test_directive.cpp e test_conditionals.cpp
// Testa estruturas básicas de diretivas e processamento condicional

#include "../../include/directive.hpp"
#include "../../include/conditional_processor.hpp"
#include "../../include/preprocessor_state.hpp"
#include "../../include/preprocessor_logger.hpp"
#include "../../include/macro_processor.hpp"
#include "../../include/expression_evaluator.hpp"
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

void assertEqual(int expected, int actual, const std::string& testName) {
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

// ============================================================================
// SETUP DE COMPONENTES PARA TESTES
// ============================================================================

std::unique_ptr<ConditionalProcessor> createConditionalProcessor() {
    auto logger = std::make_shared<PreprocessorLogger>();
    auto state = std::make_shared<PreprocessorState>();
    auto macroProcessor = std::make_shared<MacroProcessor>(logger, state);
    
    return std::make_unique<ConditionalProcessor>(logger.get(), macroProcessor.get());
}

// ============================================================================
// TESTES DE ESTRUTURAS BÁSICAS DE DIRETIVAS (test_directive.cpp)
// ============================================================================

void testDirectiveTypeToString() {
    std::cout << "\n=== Testando DirectiveType para String ===" << std::endl;
    
    assertEqual("define", directiveTypeToString(DirectiveType::DEFINE), "DirectiveType::DEFINE");
    assertEqual("undef", directiveTypeToString(DirectiveType::UNDEF), "DirectiveType::UNDEF");
    assertEqual("include", directiveTypeToString(DirectiveType::INCLUDE), "DirectiveType::INCLUDE");
    assertEqual("if", directiveTypeToString(DirectiveType::IF), "DirectiveType::IF");
    assertEqual("ifdef", directiveTypeToString(DirectiveType::IFDEF), "DirectiveType::IFDEF");
    assertEqual("ifndef", directiveTypeToString(DirectiveType::IFNDEF), "DirectiveType::IFNDEF");
    assertEqual("else", directiveTypeToString(DirectiveType::ELSE), "DirectiveType::ELSE");
    assertEqual("elif", directiveTypeToString(DirectiveType::ELIF), "DirectiveType::ELIF");
    assertEqual("endif", directiveTypeToString(DirectiveType::ENDIF), "DirectiveType::ENDIF");
    assertEqual("error", directiveTypeToString(DirectiveType::ERROR), "DirectiveType::ERROR");
    assertEqual("warning", directiveTypeToString(DirectiveType::WARNING), "DirectiveType::WARNING");
    assertEqual("pragma", directiveTypeToString(DirectiveType::PRAGMA), "DirectiveType::PRAGMA");
    assertEqual("line", directiveTypeToString(DirectiveType::LINE), "DirectiveType::LINE");
    assertEqual("unknown", directiveTypeToString(DirectiveType::UNKNOWN), "DirectiveType::UNKNOWN");
}

void testStringToDirectiveType() {
    std::cout << "\n=== Testando String para DirectiveType ===" << std::endl;
    
    assertTrue(stringToDirectiveType("define") == DirectiveType::DEFINE, "string 'define'");
    assertTrue(stringToDirectiveType("DEFINE") == DirectiveType::DEFINE, "string 'DEFINE'");
    assertTrue(stringToDirectiveType("undef") == DirectiveType::UNDEF, "string 'undef'");
    assertTrue(stringToDirectiveType("include") == DirectiveType::INCLUDE, "string 'include'");
    assertTrue(stringToDirectiveType("if") == DirectiveType::IF, "string 'if'");
    assertTrue(stringToDirectiveType("ifdef") == DirectiveType::IFDEF, "string 'ifdef'");
    assertTrue(stringToDirectiveType("ifndef") == DirectiveType::IFNDEF, "string 'ifndef'");
    assertTrue(stringToDirectiveType("else") == DirectiveType::ELSE, "string 'else'");
    assertTrue(stringToDirectiveType("elif") == DirectiveType::ELIF, "string 'elif'");
    assertTrue(stringToDirectiveType("endif") == DirectiveType::ENDIF, "string 'endif'");
    assertTrue(stringToDirectiveType("error") == DirectiveType::ERROR, "string 'error'");
    assertTrue(stringToDirectiveType("warning") == DirectiveType::WARNING, "string 'warning'");
    assertTrue(stringToDirectiveType("pragma") == DirectiveType::PRAGMA, "string 'pragma'");
    assertTrue(stringToDirectiveType("line") == DirectiveType::LINE, "string 'line'");
    assertTrue(stringToDirectiveType("invalid") == DirectiveType::UNKNOWN, "string 'invalid'");
}

void testPreprocessorPositionConstructors() {
    std::cout << "\n=== Testando Construtores de PreprocessorPosition ===" << std::endl;
    
    // Construtor padrão
    PreprocessorPosition pos1;
    assertEqual("", pos1.filename, "construtor padrão - filename");
    assertEqual(1, pos1.line, "construtor padrão - line");
    assertEqual(1, pos1.column, "construtor padrão - column");
    
    // Construtor com parâmetros
    PreprocessorPosition pos2("test.c", 10, 5);
    assertEqual("test.c", pos2.filename, "construtor com parâmetros - filename");
    assertEqual(10, pos2.line, "construtor com parâmetros - line");
    assertEqual(5, pos2.column, "construtor com parâmetros - column");
    
    // Construtor de cópia
    PreprocessorPosition pos3(pos2);
    assertEqual("test.c", pos3.filename, "construtor de cópia - filename");
    assertEqual(10, pos3.line, "construtor de cópia - line");
    assertEqual(5, pos3.column, "construtor de cópia - column");
}

void testDirectiveConstructorAndGetters() {
    std::cout << "\n=== Testando Construtor e Getters de Directive ===" << std::endl;
    
    PreprocessorPosition pos("test.c", 5, 1);
    std::string content = "#define MACRO value";
    
    Directive directive(DirectiveType::DEFINE, content, pos);
    
    assertTrue(directive.getType() == DirectiveType::DEFINE, "getType()");
    assertEqual(content, directive.getContent(), "getContent()");
    assertEqual("test.c", directive.getPosition().filename, "posição filename");
    assertEqual(5, directive.getPosition().line, "posição line");
}

void testDirectiveArgumentsManagement() {
    std::cout << "\n=== Testando Gerenciamento de Argumentos ===" << std::endl;
    
    PreprocessorPosition pos;
    std::string content1 = "#include <stdio.h>";
    Directive directive1(DirectiveType::INCLUDE, content1, pos);
    
    assertEqual(content1, directive1.getContent(), "conteúdo da diretiva include");
    assertTrue(directive1.isValid(), "diretiva deve ser válida");
    
    std::string content2 = "#define MACRO value";
    Directive directive2(DirectiveType::DEFINE, content2, pos);
    
    // Teste de setArguments se disponível
    std::vector<std::string> args = {"arg1", "arg2", "arg3"};
    directive2.setArguments(args);
    assertEqual(3, static_cast<int>(directive2.getArguments().size()), "após definir múltiplos argumentos");
}

void testIsConditionalDirective() {
    std::cout << "\n=== Testando isConditionalDirective ===" << std::endl;
    
    assertTrue(isConditionalDirective(DirectiveType::IF), "IF é condicional");
    assertTrue(isConditionalDirective(DirectiveType::IFDEF), "IFDEF é condicional");
    assertTrue(isConditionalDirective(DirectiveType::IFNDEF), "IFNDEF é condicional");
    assertTrue(isConditionalDirective(DirectiveType::ELSE), "ELSE é condicional");
    assertTrue(isConditionalDirective(DirectiveType::ELIF), "ELIF é condicional");
    assertTrue(isConditionalDirective(DirectiveType::ENDIF), "ENDIF é condicional");
    
    assertFalse(isConditionalDirective(DirectiveType::DEFINE), "DEFINE não é condicional");
    assertFalse(isConditionalDirective(DirectiveType::INCLUDE), "INCLUDE não é condicional");
    assertFalse(isConditionalDirective(DirectiveType::PRAGMA), "PRAGMA não é condicional");
}

void testRequiresArguments() {
    std::cout << "\n=== Testando requiresArguments ===" << std::endl;
    
    assertTrue(requiresArguments(DirectiveType::DEFINE), "DEFINE requer argumentos");
    assertTrue(requiresArguments(DirectiveType::UNDEF), "UNDEF requer argumentos");
    assertTrue(requiresArguments(DirectiveType::INCLUDE), "INCLUDE requer argumentos");
    assertTrue(requiresArguments(DirectiveType::IF), "IF requer argumentos");
    assertTrue(requiresArguments(DirectiveType::IFDEF), "IFDEF requer argumentos");
    assertTrue(requiresArguments(DirectiveType::IFNDEF), "IFNDEF requer argumentos");
    assertTrue(requiresArguments(DirectiveType::ELIF), "ELIF requer argumentos");
    
    assertFalse(requiresArguments(DirectiveType::ELSE), "ELSE não requer argumentos");
    assertFalse(requiresArguments(DirectiveType::ENDIF), "ENDIF não requer argumentos");
}

// ============================================================================
// TESTES DE PROCESSAMENTO CONDICIONAL (test_conditionals.cpp)
// ============================================================================

void testBasicIfDirective() {
    std::cout << "\n=== Testando Diretiva IF Básica ===" << std::endl;
    
    auto processor = createConditionalProcessor();
    PreprocessorPosition pos("test.c", 1, 1);
    
    // Teste com condição verdadeira
    bool result1 = processor->processIfDirective("1", pos);
    assertTrue(result1, "IF com condição verdadeira (1)");
    
    // Reset para próximo teste
    processor->reset();
    
    // Teste com condição falsa
    bool result2 = processor->processIfDirective("0", pos);
    assertTrue(result2, "processIfDirective deve retornar true para sucesso");
    assertFalse(processor->shouldProcessBlock(), "bloco não deve ser processado com condição falsa");
}

void testIfdefDirective() {
    std::cout << "\n=== Testando Diretiva IFDEF ===" << std::endl;
    
    auto processor = createConditionalProcessor();
    PreprocessorPosition pos("test.c", 1, 1);
    
    // Teste básico de processamento ifdef
    bool result1 = processor->processIfdefDirective("TEST_MACRO", pos);
    assertTrue(result1, "processIfdefDirective deve processar com sucesso");
    
    processor->reset();
    
    bool result2 = processor->processIfdefDirective("UNDEFINED_MACRO", pos);
    assertTrue(result2, "processIfdefDirective deve processar com sucesso");
}

void testIfndefDirective() {
    std::cout << "\n=== Testando Diretiva IFNDEF ===" << std::endl;
    
    auto processor = createConditionalProcessor();
    PreprocessorPosition pos("test.c", 1, 1);
    
    // Teste básico de processamento ifndef
    bool result1 = processor->processIfndefDirective("DEFINED_MACRO", pos);
    assertTrue(result1, "processIfndefDirective deve processar com sucesso");
    
    processor->reset();
    
    bool result2 = processor->processIfndefDirective("UNDEFINED_MACRO", pos);
    assertTrue(result2, "processIfndefDirective deve processar com sucesso");
}

void testNestedConditionals() {
    std::cout << "\n=== Testando Condicionais Aninhadas ===" << std::endl;
    
    auto processor = createConditionalProcessor();
    PreprocessorPosition pos("test.c", 1, 1);
    
    // Simular estrutura aninhada
    processor->processIfDirective("1", pos);  // IF verdadeiro
    processor->processIfDirective("1", pos);  // IF aninhado verdadeiro
    
    assertTrue(processor->getCurrentNestingLevel() == 2, "nível de aninhamento correto");
    
    processor->processEndifDirective(pos);  // Fechar IF aninhado
    assertTrue(processor->getCurrentNestingLevel() == 1, "nível após primeiro endif");
    
    processor->processEndifDirective(pos);  // Fechar IF principal
    assertTrue(processor->getCurrentNestingLevel() == 0, "nível após segundo endif");
}

void testExpressionEvaluation() {
    std::cout << "\n=== Testando Avaliação de Expressões ===" << std::endl;
    
    auto processor = createConditionalProcessor();
    PreprocessorPosition pos("test.c", 1, 1);
    
    // Teste com expressões simples
    bool result1 = processor->processIfDirective("5 > 3", pos);
    assertTrue(result1, "processIfDirective deve processar com sucesso");
    
    processor->reset();
    
    bool result2 = processor->processIfDirective("1 + 1 == 2", pos);
    assertTrue(result2, "processIfDirective deve processar com sucesso");
}

void testComplexExpressions() {
    std::cout << "\n=== Testando Expressões Complexas ===" << std::endl;
    
    auto processor = createConditionalProcessor();
    PreprocessorPosition pos("test.c", 1, 1);
    
    // Teste com expressões lógicas
    bool result1 = processor->processIfDirective("(2 >= 2) && 1", pos);
    assertTrue(result1, "processIfDirective deve processar com sucesso");
    
    processor->reset();
    
    bool result2 = processor->processIfDirective("2 == 2 || 2 == 3", pos);
    assertTrue(result2, "processIfDirective deve processar com sucesso");
}

void testConditionalErrors() {
    std::cout << "\n=== Testando Tratamento de Erros ===" << std::endl;
    
    auto processor = createConditionalProcessor();
    PreprocessorPosition pos("test.c", 1, 1);
    
    // Teste de endif sem if correspondente
    bool result = processor->processEndifDirective(pos);
    // O método pode retornar false ou lançar exceção dependendo da implementação
    assertFalse(result, "processEndifDirective deve falhar sem if correspondente");
}

void testConditionalPerformance() {
    std::cout << "\n=== Testando Performance de Condicionais ===" << std::endl;
    
    auto processor = createConditionalProcessor();
    PreprocessorPosition pos("test.c", 1, 1);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Processar muitas condicionais
    for (int i = 0; i < 100; ++i) {
        processor->processIfDirective("1", pos);
        processor->processEndifDirective(pos);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Processamento de 100 condicionais: " << duration.count() << " microsegundos" << std::endl;
    assertTrue(duration.count() < 50000, "performance aceitável (< 50ms)");
}

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main() {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "TESTE ABRANGENTE DE DIRETIVAS - UNIFICADO" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    try {
        // Testes de estruturas básicas de diretivas
        std::cout << "\n[SEÇÃO 1] TESTES DE ESTRUTURAS BÁSICAS DE DIRETIVAS" << std::endl;
        testDirectiveTypeToString();
        testStringToDirectiveType();
        testPreprocessorPositionConstructors();
        testDirectiveConstructorAndGetters();
        testDirectiveArgumentsManagement();
        testIsConditionalDirective();
        testRequiresArguments();
        
        // Testes de processamento condicional
        std::cout << "\n[SEÇÃO 2] TESTES DE PROCESSAMENTO CONDICIONAL" << std::endl;
        testBasicIfDirective();
        testIfdefDirective();
        testIfndefDirective();
        testNestedConditionals();
        testExpressionEvaluation();
        testComplexExpressions();
        testConditionalErrors();
        testConditionalPerformance();
        
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "✅ TODOS OS TESTES DE DIRETIVAS PASSARAM COM SUCESSO!" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ ERRO DURANTE OS TESTES: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ ERRO DESCONHECIDO DURANTE OS TESTES" << std::endl;
        return 1;
    }
    
    return 0;
}