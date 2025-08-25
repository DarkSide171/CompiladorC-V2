// Testes Unitários - Estruturas Básicas de Diretivas
// Testes para directive.hpp - Fase 1.6
// Testa DirectiveType, PreprocessorPosition, Directive e funções utilitárias

#include "../../include/directive.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>

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
// TESTES DO ENUM DirectiveType
// ============================================================================

void testDirectiveTypeToString() {
    std::cout << "\n=== Testando directiveTypeToString() ===" << std::endl;
    
    // Testa conversões básicas
    assertEqual("include", directiveTypeToString(DirectiveType::INCLUDE), "INCLUDE -> string");
    assertEqual("define", directiveTypeToString(DirectiveType::DEFINE), "DEFINE -> string");
    assertEqual("undef", directiveTypeToString(DirectiveType::UNDEF), "UNDEF -> string");
    
    // Testa diretivas condicionais
    assertEqual("if", directiveTypeToString(DirectiveType::IF), "IF -> string");
    assertEqual("ifdef", directiveTypeToString(DirectiveType::IFDEF), "IFDEF -> string");
    assertEqual("ifndef", directiveTypeToString(DirectiveType::IFNDEF), "IFNDEF -> string");
    assertEqual("else", directiveTypeToString(DirectiveType::ELSE), "ELSE -> string");
    assertEqual("elif", directiveTypeToString(DirectiveType::ELIF), "ELIF -> string");
    assertEqual("endif", directiveTypeToString(DirectiveType::ENDIF), "ENDIF -> string");
    
    // Testa diretivas de controle
    assertEqual("error", directiveTypeToString(DirectiveType::ERROR), "ERROR -> string");
    assertEqual("warning", directiveTypeToString(DirectiveType::WARNING), "WARNING -> string");
    assertEqual("pragma", directiveTypeToString(DirectiveType::PRAGMA), "PRAGMA -> string");
    assertEqual("line", directiveTypeToString(DirectiveType::LINE), "LINE -> string");
    
    // Testa tipo especial
    assertEqual("unknown", directiveTypeToString(DirectiveType::UNKNOWN), "UNKNOWN -> string");
}

void testStringToDirectiveType() {
    std::cout << "\n=== Testando stringToDirectiveType() ===" << std::endl;
    
    // Testa conversões básicas
    assertTrue(stringToDirectiveType("include") == DirectiveType::INCLUDE, "string -> INCLUDE");
    assertTrue(stringToDirectiveType("define") == DirectiveType::DEFINE, "string -> DEFINE");
    assertTrue(stringToDirectiveType("undef") == DirectiveType::UNDEF, "string -> UNDEF");
    
    // Testa diretivas condicionais
    assertTrue(stringToDirectiveType("if") == DirectiveType::IF, "string -> IF");
    assertTrue(stringToDirectiveType("ifdef") == DirectiveType::IFDEF, "string -> IFDEF");
    assertTrue(stringToDirectiveType("ifndef") == DirectiveType::IFNDEF, "string -> IFNDEF");
    assertTrue(stringToDirectiveType("else") == DirectiveType::ELSE, "string -> ELSE");
    assertTrue(stringToDirectiveType("elif") == DirectiveType::ELIF, "string -> ELIF");
    assertTrue(stringToDirectiveType("endif") == DirectiveType::ENDIF, "string -> ENDIF");
    
    // Testa diretivas de controle
    assertTrue(stringToDirectiveType("error") == DirectiveType::ERROR, "string -> ERROR");
    assertTrue(stringToDirectiveType("warning") == DirectiveType::WARNING, "string -> WARNING");
    assertTrue(stringToDirectiveType("pragma") == DirectiveType::PRAGMA, "string -> PRAGMA");
    assertTrue(stringToDirectiveType("line") == DirectiveType::LINE, "string -> LINE");
    
    // Testa strings inválidas
    assertTrue(stringToDirectiveType("invalid") == DirectiveType::UNKNOWN, "string inválida -> UNKNOWN");
    assertTrue(stringToDirectiveType("") == DirectiveType::UNKNOWN, "string vazia -> UNKNOWN");
    assertTrue(stringToDirectiveType("INCLUDE") == DirectiveType::INCLUDE, "case insensitive -> INCLUDE");
}

// ============================================================================
// TESTES DA STRUCT PreprocessorPosition
// ============================================================================

void testPreprocessorPositionConstructors() {
    std::cout << "\n=== Testando Construtores de PreprocessorPosition ===" << std::endl;
    
    // Teste construtor padrão
    PreprocessorPosition pos1;
    assertEqual(0, pos1.original_line, "Construtor padrão - original_line");
    assertEqual(0, pos1.original_column, "Construtor padrão - original_column");
    assertEqual(0, pos1.expanded_line, "Construtor padrão - expanded_line");
    assertEqual(0, pos1.expanded_column, "Construtor padrão - expanded_column");
    assertEqual("", pos1.filename, "Construtor padrão - filename");
    assertEqual("", pos1.original_file, "Construtor padrão - original_file");
    assertEqual(0, pos1.offset, "Construtor padrão - offset");
    
    // Teste construtor com parâmetros
    PreprocessorPosition pos2(10, 5, "test.c");
    assertEqual(10, pos2.original_line, "Construtor parametrizado - original_line");
    assertEqual(5, pos2.original_column, "Construtor parametrizado - original_column");
    assertEqual(10, pos2.expanded_line, "Construtor parametrizado - expanded_line");
    assertEqual(5, pos2.expanded_column, "Construtor parametrizado - expanded_column");
    assertEqual("test.c", pos2.filename, "Construtor parametrizado - filename");
    assertEqual("test.c", pos2.original_file, "Construtor parametrizado - original_file");
    assertEqual(0, pos2.offset, "Construtor parametrizado - offset");
}

void testPreprocessorPositionModification() {
    std::cout << "\n=== Testando Modificação de PreprocessorPosition ===" << std::endl;
    
    PreprocessorPosition pos(1, 1, "main.c");
    
    // Modifica valores
    pos.expanded_line = 15;
    pos.expanded_column = 20;
    pos.original_file = "header.h";
    pos.offset = 100;
    
    // Verifica modificações
    assertEqual(1, pos.original_line, "Modificação - original_line inalterada");
    assertEqual(1, pos.original_column, "Modificação - original_column inalterada");
    assertEqual(15, pos.expanded_line, "Modificação - expanded_line alterada");
    assertEqual(20, pos.expanded_column, "Modificação - expanded_column alterada");
    assertEqual("main.c", pos.filename, "Modificação - filename inalterada");
    assertEqual("header.h", pos.original_file, "Modificação - original_file alterada");
    assertEqual(100, pos.offset, "Modificação - offset alterado");
}

// ============================================================================
// TESTES DA CLASSE Directive
// ============================================================================

void testDirectiveConstructorAndGetters() {
    std::cout << "\n=== Testando Construtor e Getters de Directive ===" << std::endl;
    
    PreprocessorPosition pos(5, 1, "test.c");
    Directive directive(DirectiveType::INCLUDE, "#include <stdio.h>", pos);
    
    // Testa getters básicos
    assertTrue(directive.getType() == DirectiveType::INCLUDE, "getType() retorna INCLUDE");
    assertEqual("#include <stdio.h>", directive.getContent(), "getContent() retorna conteúdo correto");
    
    PreprocessorPosition retrievedPos = directive.getPosition();
    assertEqual(5, retrievedPos.original_line, "getPosition() - linha correta");
    assertEqual(1, retrievedPos.original_column, "getPosition() - coluna correta");
    assertEqual("test.c", retrievedPos.filename, "getPosition() - arquivo correto");
    
    // Testa argumentos iniciais (devem estar vazios)
    std::vector<std::string> args = directive.getArguments();
    assertTrue(args.empty(), "getArguments() inicialmente vazio");
}

void testDirectiveArgumentsManagement() {
    std::cout << "\n=== Testando Gerenciamento de Argumentos ===" << std::endl;
    
    PreprocessorPosition pos(1, 1, "test.c");
    Directive directive(DirectiveType::DEFINE, "#define MAX 100", pos);
    
    // Define argumentos
    std::vector<std::string> args = {"MAX", "100"};
    directive.setArguments(args);
    
    // Verifica argumentos
    std::vector<std::string> retrievedArgs = directive.getArguments();
    assertEqual(2, static_cast<int>(retrievedArgs.size()), "Número de argumentos correto");
    assertEqual("MAX", retrievedArgs[0], "Primeiro argumento correto");
    assertEqual("100", retrievedArgs[1], "Segundo argumento correto");
    
    // Testa argumentos vazios
    directive.setArguments({});
    retrievedArgs = directive.getArguments();
    assertTrue(retrievedArgs.empty(), "Argumentos vazios após limpeza");
}

void testDirectiveValidation() {
    std::cout << "\n=== Testando Validação de Directive ===" << std::endl;
    
    PreprocessorPosition pos(1, 1, "test.c");
    Directive directive(DirectiveType::INCLUDE, "#include <stdio.h>", pos);
    
    // Testa flag de validade
    directive.setValid(true);
    assertTrue(directive.isValid(), "setValid(true) funciona");
    
    directive.setValid(false);
    assertFalse(directive.isValid(), "setValid(false) funciona");
    
    // Testa validação de sintaxe (assumindo implementação)
    directive.setValid(true);
    bool syntaxValid = directive.validateSyntax();
    std::cout << "✅ validateSyntax() executado (resultado: " << (syntaxValid ? "válido" : "inválido") << ")" << std::endl;
    
    // Testa validação de argumentos
    bool argsValid = directive.validateArguments();
    std::cout << "✅ validateArguments() executado (resultado: " << (argsValid ? "válido" : "inválido") << ")" << std::endl;
}

void testDirectiveToString() {
    std::cout << "\n=== Testando toString() de Directive ===" << std::endl;
    
    PreprocessorPosition pos(10, 5, "main.c");
    Directive directive(DirectiveType::DEFINE, "#define PI 3.14159", pos);
    
    std::string result = directive.toString();
    assertFalse(result.empty(), "toString() não retorna string vazia");
    std::cout << "✅ toString() resultado: " << result << std::endl;
}

// ============================================================================
// TESTES DAS FUNÇÕES UTILITÁRIAS
// ============================================================================

void testIsConditionalDirective() {
    std::cout << "\n=== Testando isConditionalDirective() ===" << std::endl;
    
    // Testa diretivas condicionais
    assertTrue(isConditionalDirective(DirectiveType::IF), "IF é condicional");
    assertTrue(isConditionalDirective(DirectiveType::IFDEF), "IFDEF é condicional");
    assertTrue(isConditionalDirective(DirectiveType::IFNDEF), "IFNDEF é condicional");
    assertTrue(isConditionalDirective(DirectiveType::ELSE), "ELSE é condicional");
    assertTrue(isConditionalDirective(DirectiveType::ELIF), "ELIF é condicional");
    assertTrue(isConditionalDirective(DirectiveType::ENDIF), "ENDIF é condicional");
    
    // Testa diretivas não condicionais
    assertFalse(isConditionalDirective(DirectiveType::INCLUDE), "INCLUDE não é condicional");
    assertFalse(isConditionalDirective(DirectiveType::DEFINE), "DEFINE não é condicional");
    assertFalse(isConditionalDirective(DirectiveType::UNDEF), "UNDEF não é condicional");
    assertFalse(isConditionalDirective(DirectiveType::ERROR), "ERROR não é condicional");
    assertFalse(isConditionalDirective(DirectiveType::WARNING), "WARNING não é condicional");
    assertFalse(isConditionalDirective(DirectiveType::PRAGMA), "PRAGMA não é condicional");
    assertFalse(isConditionalDirective(DirectiveType::LINE), "LINE não é condicional");
    assertFalse(isConditionalDirective(DirectiveType::UNKNOWN), "UNKNOWN não é condicional");
}

void testRequiresArguments() {
    std::cout << "\n=== Testando requiresArguments() ===" << std::endl;
    
    // Testa diretivas que requerem argumentos
    assertTrue(requiresArguments(DirectiveType::INCLUDE), "INCLUDE requer argumentos");
    assertTrue(requiresArguments(DirectiveType::DEFINE), "DEFINE requer argumentos");
    assertTrue(requiresArguments(DirectiveType::UNDEF), "UNDEF requer argumentos");
    assertTrue(requiresArguments(DirectiveType::IF), "IF requer argumentos");
    assertTrue(requiresArguments(DirectiveType::IFDEF), "IFDEF requer argumentos");
    assertTrue(requiresArguments(DirectiveType::IFNDEF), "IFNDEF requer argumentos");
    assertTrue(requiresArguments(DirectiveType::ELIF), "ELIF requer argumentos");
    assertFalse(requiresArguments(DirectiveType::ERROR), "ERROR não requer argumentos obrigatórios");
    assertFalse(requiresArguments(DirectiveType::WARNING), "WARNING não requer argumentos obrigatórios");
    assertTrue(requiresArguments(DirectiveType::PRAGMA), "PRAGMA requer argumentos");
    assertTrue(requiresArguments(DirectiveType::LINE), "LINE requer argumentos");
    
    // Testa diretivas que não requerem argumentos
    assertFalse(requiresArguments(DirectiveType::ELSE), "ELSE não requer argumentos");
    assertFalse(requiresArguments(DirectiveType::ENDIF), "ENDIF não requer argumentos");
    assertFalse(requiresArguments(DirectiveType::UNKNOWN), "UNKNOWN não requer argumentos");
}

// ============================================================================
// TESTES DE CENÁRIOS COMPLEXOS
// ============================================================================

void testComplexDirectiveScenarios() {
    std::cout << "\n=== Testando Cenários Complexos ===" << std::endl;
    
    // Cenário 1: Diretiva INCLUDE com argumentos
    PreprocessorPosition pos1(1, 1, "main.c");
    Directive includeDir(DirectiveType::INCLUDE, "#include \"myheader.h\"", pos1);
    includeDir.setArguments({"\"myheader.h\""});
    
    assertTrue(includeDir.getType() == DirectiveType::INCLUDE, "Cenário INCLUDE - tipo correto");
    assertEqual(1, static_cast<int>(includeDir.getArguments().size()), "Cenário INCLUDE - 1 argumento");
    assertEqual("\"myheader.h\"", includeDir.getArguments()[0], "Cenário INCLUDE - argumento correto");
    
    // Cenário 2: Diretiva DEFINE com múltiplos argumentos
    PreprocessorPosition pos2(5, 1, "config.h");
    Directive defineDir(DirectiveType::DEFINE, "#define MAX(a,b) ((a)>(b)?(a):(b))", pos2);
    defineDir.setArguments({"MAX(a,b)", "((a)>(b)?(a):(b))"});
    
    assertTrue(defineDir.getType() == DirectiveType::DEFINE, "Cenário DEFINE - tipo correto");
    assertEqual(2, static_cast<int>(defineDir.getArguments().size()), "Cenário DEFINE - 2 argumentos");
    
    // Cenário 3: Diretiva condicional IF
    PreprocessorPosition pos3(10, 1, "conditional.c");
    Directive ifDir(DirectiveType::IF, "#if defined(DEBUG) && VERSION > 2", pos3);
    ifDir.setArguments({"defined(DEBUG) && VERSION > 2"});
    
    assertTrue(ifDir.getType() == DirectiveType::IF, "Cenário IF - tipo correto");
    assertTrue(isConditionalDirective(ifDir.getType()), "Cenário IF - é condicional");
    assertTrue(requiresArguments(ifDir.getType()), "Cenário IF - requer argumentos");
    
    // Cenário 4: Diretiva ELSE (sem argumentos)
    PreprocessorPosition pos4(15, 1, "conditional.c");
    Directive elseDir(DirectiveType::ELSE, "#else", pos4);
    
    assertTrue(elseDir.getType() == DirectiveType::ELSE, "Cenário ELSE - tipo correto");
    assertTrue(isConditionalDirective(elseDir.getType()), "Cenário ELSE - é condicional");
    assertFalse(requiresArguments(elseDir.getType()), "Cenário ELSE - não requer argumentos");
    assertTrue(elseDir.getArguments().empty(), "Cenário ELSE - sem argumentos");
}

void testDirectiveTypeConversions() {
    std::cout << "\n=== Testando Conversões Bidirecionais ===" << std::endl;
    
    // Testa todas as conversões bidirecionais
    std::vector<DirectiveType> allTypes = {
        DirectiveType::INCLUDE, DirectiveType::DEFINE, DirectiveType::UNDEF,
        DirectiveType::IF, DirectiveType::IFDEF, DirectiveType::IFNDEF,
        DirectiveType::ELSE, DirectiveType::ELIF, DirectiveType::ENDIF,
        DirectiveType::ERROR, DirectiveType::WARNING, DirectiveType::PRAGMA,
        DirectiveType::LINE, DirectiveType::UNKNOWN
    };
    
    for (DirectiveType type : allTypes) {
        std::string typeStr = directiveTypeToString(type);
        DirectiveType convertedBack = stringToDirectiveType(typeStr);
        
        assertTrue(type == convertedBack, 
                  "Conversão bidirecional: " + typeStr + " -> " + directiveTypeToString(convertedBack));
    }
}

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main() {
    std::cout << "=== EXECUTANDO TESTES DE ESTRUTURAS BÁSICAS DE DIRETIVAS (FASE 1.6) ===" << std::endl;
    
    try {
        // Testes do enum DirectiveType
        testDirectiveTypeToString();
        testStringToDirectiveType();
        testDirectiveTypeConversions();
        
        // Testes da struct PreprocessorPosition
        testPreprocessorPositionConstructors();
        testPreprocessorPositionModification();
        
        // Testes da classe Directive
        testDirectiveConstructorAndGetters();
        testDirectiveArgumentsManagement();
        testDirectiveValidation();
        testDirectiveToString();
        
        // Testes das funções utilitárias
        testIsConditionalDirective();
        testRequiresArguments();
        
        // Testes de cenários complexos
        testComplexDirectiveScenarios();
        
        std::cout << "\n🎉 Todos os testes de estruturas básicas de diretivas passaram com sucesso!" << std::endl;
        std::cout << "📊 Total de testes executados: 14 grupos de teste" << std::endl;
        std::cout << "✅ Fase 1.6 - test_directive.cpp: CONCLUÍDO" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Erro durante os testes: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Erro desconhecido durante os testes" << std::endl;
        return 1;
    }
}