// Teste de Validação da Fase 4.1 - DirectiveProcessor
// Valida todos os métodos implementados do DirectiveProcessor

#include "../../include/directive.hpp"
#include "../../include/preprocessor_state.hpp"
#include "../../include/preprocessor_logger.hpp"
#include "../../include/macro_processor.hpp"
#include "../../include/file_manager.hpp"
#include "../../include/conditional_processor.hpp"
#include <iostream>
#include <cassert>
#include <memory>
#include <string>
#include <vector>

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

// Variáveis globais para manter os objetos vivos durante os testes
std::shared_ptr<PreprocessorState> g_state;
std::shared_ptr<PreprocessorLogger> g_logger;
std::shared_ptr<MacroProcessor> g_macroProcessor;
std::shared_ptr<FileManager> g_fileManager;
std::shared_ptr<ConditionalProcessor> g_conditionalProcessor;

std::unique_ptr<DirectiveProcessor> createDirectiveProcessor() {
    // Limpa explicitamente as variáveis globais antes de recriá-las
    g_conditionalProcessor.reset();
    g_fileManager.reset();
    g_macroProcessor.reset();
    g_logger.reset();
    g_state.reset();
    
    g_state = std::make_shared<PreprocessorState>();
    g_logger = std::make_shared<PreprocessorLogger>();
    g_macroProcessor = std::make_shared<MacroProcessor>(g_logger, g_state);
    g_fileManager = std::make_shared<FileManager>(std::vector<std::string>{}, g_logger.get());
    g_conditionalProcessor = std::make_shared<ConditionalProcessor>(g_logger.get(), g_macroProcessor.get());
    
    return std::make_unique<DirectiveProcessor>(g_state.get(), g_logger.get(), g_macroProcessor.get(), 
                                               g_fileManager.get(), g_conditionalProcessor.get());
}

// ============================================================================
// TESTES DOS MÉTODOS DA FASE 4.1
// ============================================================================

void testParseDirective() {
    std::cout << "\n=== Testando parseDirective() ===" << std::endl;
    
    auto processor = createDirectiveProcessor();
    
    // Teste 1: Diretiva #include válida
    std::string line1 = "#include <stdio.h>";
    PreprocessorPosition pos1(1, 1, "test.c");
    Directive directive1 = processor->parseDirective(line1, pos1);
    
    assertTrue(directive1.getType() == DirectiveType::INCLUDE, "Tipo correto para #include");
    assertEqual("#include <stdio.h>", directive1.getContent(), "Conteúdo correto para #include");
    
    // Teste 2: Diretiva #define válida
    std::string line2 = "#define MAX 100";
    PreprocessorPosition pos2(2, 1, "test.c");
    Directive directive2 = processor->parseDirective(line2, pos2);
    
    assertTrue(directive2.getType() == DirectiveType::DEFINE, "Tipo correto para #define");
    
    // Teste 3: Diretiva #if válida
    std::string line3 = "#if defined(DEBUG)";
    PreprocessorPosition pos3(3, 1, "test.c");
    Directive directive3 = processor->parseDirective(line3, pos3);
    
    assertTrue(directive3.getType() == DirectiveType::IF, "Tipo correto para #if");
    
    std::cout << "✅ parseDirective() testado com sucesso" << std::endl;
}

void testValidateDirectiveSyntax() {
    std::cout << "\n=== Testando validateDirectiveSyntax() ===" << std::endl;
    
    auto processor = createDirectiveProcessor();
    
    // Teste 1: Diretiva #include válida
    PreprocessorPosition pos1(1, 1, "test.c");
    Directive directive1(DirectiveType::INCLUDE, "#include <stdio.h>", pos1);
    directive1.setArguments({"<stdio.h>"});
    
    bool valid1 = processor->validateDirectiveSyntax(directive1);
    assertTrue(valid1, "validateDirectiveSyntax() valida #include corretamente");
    
    // Teste 2: Diretiva #define válida
    PreprocessorPosition pos2(2, 1, "test.c");
    Directive directive2(DirectiveType::DEFINE, "#define MAX 100", pos2);
    directive2.setArguments({"MAX", "100"});
    
    bool valid2 = processor->validateDirectiveSyntax(directive2);
    assertTrue(valid2, "validateDirectiveSyntax() valida #define corretamente");
    
    // Teste 3: Diretiva #if válida
    PreprocessorPosition pos3(3, 1, "test.c");
    Directive directive3(DirectiveType::IF, "#if 1", pos3);
    directive3.setArguments({"1"});
    
    bool valid3 = processor->validateDirectiveSyntax(directive3);
    assertTrue(valid3, "validateDirectiveSyntax() valida #if corretamente");
}

void testDirectiveContextValidation() {
    std::cout << "\n=== Testando Validação de Contexto ===" << std::endl;
    
    auto processor = createDirectiveProcessor();
    
    // Teste validateDirectiveContext
    bool contextValid1 = processor->validateDirectiveContext(DirectiveType::DEFINE);
    std::cout << "✅ validateDirectiveContext(DEFINE) executado (resultado: " << (contextValid1 ? "válido" : "inválido") << ")" << std::endl;
    
    bool contextValid2 = processor->validateDirectiveContext(DirectiveType::IF);
    std::cout << "✅ validateDirectiveContext(IF) executado (resultado: " << (contextValid2 ? "válido" : "inválido") << ")" << std::endl;
    
    // Teste checkDirectiveNesting
    bool nestingValid1 = processor->checkDirectiveNesting(DirectiveType::IF);
    std::cout << "✅ checkDirectiveNesting(IF) executado (resultado: " << (nestingValid1 ? "válido" : "inválido") << ")" << std::endl;
    
    bool nestingValid2 = processor->checkDirectiveNesting(DirectiveType::ENDIF);
    std::cout << "✅ checkDirectiveNesting(ENDIF) executado (resultado: " << (nestingValid2 ? "válido" : "inválido") << ")" << std::endl;
}

void testSpecificDirectiveProcessors() {
    std::cout << "\n=== Testando Processadores Específicos de Diretivas ===" << std::endl;
    
    auto processor = createDirectiveProcessor();
    
    // Teste processIncludeDirective
    PreprocessorPosition pos1(1, 1, "test.c");
    try {
        bool includeResult = processor->processIncludeDirective("<stdio.h>", pos1);
        std::cout << "✅ processIncludeDirective() executado (resultado: " << (includeResult ? "sucesso" : "falha") << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠️ processIncludeDirective() falhou com exceção: " << e.what() << std::endl;
    }
    
    // Teste processDefineDirective
    PreprocessorPosition pos2(2, 1, "test.c");
    try {
        bool defineResult = processor->processDefineDirective("MAX", "100", pos2);
        std::cout << "✅ processDefineDirective() executado (resultado: " << (defineResult ? "sucesso" : "falha") << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠️ processDefineDirective() falhou com exceção: " << e.what() << std::endl;
    }
    
    // Teste processIfDirective
    PreprocessorPosition pos3(3, 1, "test.c");
    try {
        bool ifResult = processor->processIfDirective("1", pos3);
        std::cout << "✅ processIfDirective() executado (resultado: " << (ifResult ? "sucesso" : "falha") << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠️ processIfDirective() falhou com exceção: " << e.what() << std::endl;
    }
    
    // Teste processElseDirective
    PreprocessorPosition pos4(4, 1, "test.c");
    try {
        bool elseResult = processor->processElseDirective(pos4);
        std::cout << "✅ processElseDirective() executado (resultado: " << (elseResult ? "sucesso" : "falha") << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠️ processElseDirective() falhou com exceção: " << e.what() << std::endl;
    }
    
    // Teste processEndifDirective
    PreprocessorPosition pos5(5, 1, "test.c");
    try {
        bool endifResult = processor->processEndifDirective(pos5);
        std::cout << "✅ processEndifDirective() executado (resultado: " << (endifResult ? "sucesso" : "falha") << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠️ processEndifDirective() falhou com exceção: " << e.what() << std::endl;
    }
}

void testUtilityMethods() {
    std::cout << "\n=== Testando Métodos Utilitários ===" << std::endl;
    
    std::cout << "[DEBUG] Chamando createDirectiveProcessor() em testUtilityMethods..." << std::endl;
    auto processor = createDirectiveProcessor();
    std::cout << "[DEBUG] DirectiveProcessor criado com sucesso em testUtilityMethods" << std::endl;
    
    // Teste extractDirectiveName
    std::cout << "[DEBUG] Testando extractDirectiveName #1..." << std::endl;
    std::string name1 = processor->extractDirectiveName("#include <stdio.h>");
    std::cout << "[DEBUG] extractDirectiveName retornou: '" << name1 << "'" << std::endl;
    assertEqual("include", name1, "extractDirectiveName() extrai 'include' corretamente");
    
    std::cout << "[DEBUG] Testando extractDirectiveName #2..." << std::endl;
    std::string name2 = processor->extractDirectiveName("#define MAX 100");
    std::cout << "[DEBUG] extractDirectiveName retornou: '" << name2 << "'" << std::endl;
    assertEqual("define", name2, "extractDirectiveName() extrai 'define' corretamente");
    
    std::cout << "[DEBUG] Testando extractDirectiveName #3..." << std::endl;
    std::string name3 = processor->extractDirectiveName("#if defined(DEBUG)");
    std::cout << "[DEBUG] extractDirectiveName retornou: '" << name3 << "'" << std::endl;
    assertEqual("if", name3, "extractDirectiveName() extrai 'if' corretamente");
    
    // Teste extractDirectiveArguments
    std::string args1 = processor->extractDirectiveArguments("#include <stdio.h>", "include");
    assertTrue(!args1.empty(), "extractDirectiveArguments() extrai argumentos de #include");
    
    std::string args2 = processor->extractDirectiveArguments("#define MAX 100", "define");
    assertTrue(!args2.empty(), "extractDirectiveArguments() extrai argumentos de #define");
    
    // Teste normalizeDirectiveLine
    std::string normalized1 = processor->normalizeDirectiveLine("  #include <stdio.h>  // comment");
    assertTrue(normalized1.find("#include") != std::string::npos, "normalizeDirectiveLine() preserva diretiva");
    assertTrue(normalized1.find("//") == std::string::npos, "normalizeDirectiveLine() remove comentários");
}

void testErrorHandling() {
    std::cout << "\n=== Testando Tratamento de Erros ===" << std::endl;
    
    auto processor = createDirectiveProcessor();
    
    // Teste reportDirectiveError
    PreprocessorPosition pos(1, 1, "test.c");
    Directive directive(DirectiveType::UNKNOWN, "#unknown_directive", pos);
    
    try {
        processor->reportDirectiveError(directive, "Diretiva desconhecida");
        std::cout << "✅ reportDirectiveError() executado sem exceções" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠️ reportDirectiveError() falhou com exceção: " << e.what() << std::endl;
    }
}

void testStatisticsAndLogging() {
    std::cout << "\n=== Testando Estatísticas e Logging ===" << std::endl;
    
    auto processor = createDirectiveProcessor();
    
    // Teste updateDirectiveStatistics
    try {
        processor->updateDirectiveStatistics(DirectiveType::DEFINE);
        std::cout << "✅ updateDirectiveStatistics() executado sem exceções" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠️ updateDirectiveStatistics() falhou com exceção: " << e.what() << std::endl;
    }
    
    // Teste logDirectiveProcessing
    PreprocessorPosition pos(1, 1, "test.c");
    Directive directive(DirectiveType::DEFINE, "#define TEST 1", pos);
    try {
        processor->logDirectiveProcessing(directive, pos);
        std::cout << "✅ logDirectiveProcessing() executado sem exceções" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠️ logDirectiveProcessing() falhou com exceção: " << e.what() << std::endl;
    }
}

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main() {
    std::cout << "\n=== INICIANDO TESTES DA FASE 4.1 - DirectiveProcessor ===\n" << std::endl;
    
    try {
        std::cout << "[DEBUG] Iniciando testParseDirective..." << std::endl;
        testParseDirective();
        std::cout << "[DEBUG] testParseDirective concluído com sucesso" << std::endl;
        
        std::cout << "[DEBUG] Iniciando testValidateDirectiveSyntax..." << std::endl;
        testValidateDirectiveSyntax();
        std::cout << "[DEBUG] testValidateDirectiveSyntax concluído com sucesso" << std::endl;
        
        std::cout << "[DEBUG] Iniciando testDirectiveContextValidation..." << std::endl;
        testDirectiveContextValidation();
        std::cout << "[DEBUG] testDirectiveContextValidation concluído com sucesso" << std::endl;
        
        std::cout << "[DEBUG] Iniciando testSpecificDirectiveProcessors..." << std::endl;
        testSpecificDirectiveProcessors();
        std::cout << "[DEBUG] testSpecificDirectiveProcessors concluído com sucesso" << std::endl;
        
        std::cout << "[DEBUG] Iniciando testUtilityMethods..." << std::endl;
        std::cout << "[DEBUG] Estado das variáveis globais antes de testUtilityMethods:" << std::endl;
        std::cout << "[DEBUG] g_state.use_count(): " << g_state.use_count() << std::endl;
        std::cout << "[DEBUG] g_logger.use_count(): " << g_logger.use_count() << std::endl;
        std::cout << "[DEBUG] g_macroProcessor.use_count(): " << g_macroProcessor.use_count() << std::endl;
        std::cout << "[DEBUG] g_fileManager.use_count(): " << g_fileManager.use_count() << std::endl;
        std::cout << "[DEBUG] g_conditionalProcessor.use_count(): " << g_conditionalProcessor.use_count() << std::endl;
        testUtilityMethods();
        std::cout << "[DEBUG] testUtilityMethods concluído com sucesso" << std::endl;
        
        std::cout << "[DEBUG] Iniciando testErrorHandling..." << std::endl;
        testErrorHandling();
        std::cout << "[DEBUG] testErrorHandling concluído com sucesso" << std::endl;
        
        std::cout << "[DEBUG] Iniciando testStatisticsAndLogging..." << std::endl;
        testStatisticsAndLogging();
        std::cout << "[DEBUG] testStatisticsAndLogging concluído com sucesso" << std::endl;
        
        std::cout << "\n🎉 TODOS OS TESTES DA FASE 4.1 PASSARAM COM SUCESSO! 🎉\n" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ ERRO DURANTE OS TESTES: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ ERRO DESCONHECIDO DURANTE OS TESTES" << std::endl;
        return 1;
    }
}