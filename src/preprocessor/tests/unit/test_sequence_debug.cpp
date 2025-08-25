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

// Variáveis globais como no teste original
std::shared_ptr<PreprocessorState> g_state;
std::shared_ptr<PreprocessorLogger> g_logger;
std::shared_ptr<MacroProcessor> g_macroProcessor;
std::shared_ptr<FileManager> g_fileManager;
std::shared_ptr<ConditionalProcessor> g_conditionalProcessor;

std::unique_ptr<DirectiveProcessor> createDirectiveProcessor() {
    g_state = std::make_shared<PreprocessorState>();
    g_logger = std::make_shared<PreprocessorLogger>();
    g_macroProcessor = std::make_shared<MacroProcessor>(g_logger, g_state);
    g_fileManager = std::make_shared<FileManager>(std::vector<std::string>{}, g_logger.get());
    g_conditionalProcessor = std::make_shared<ConditionalProcessor>(g_logger.get(), g_macroProcessor.get());
    
    return std::make_unique<DirectiveProcessor>(g_state.get(), g_logger.get(), g_macroProcessor.get(), 
                                               g_fileManager.get(), g_conditionalProcessor.get());
}

void testSpecificDirectiveProcessors() {
    std::cout << "\n=== Testando Processadores Específicos de Diretivas ===" << std::endl;
    
    std::cout << "[DEBUG] Criando DirectiveProcessor para testSpecificDirectiveProcessors..." << std::endl;
    auto processor = createDirectiveProcessor();
    std::cout << "[DEBUG] DirectiveProcessor criado" << std::endl;
    
    // Teste processIncludeDirective
    std::cout << "[DEBUG] Testando processIncludeDirective..." << std::endl;
    PreprocessorPosition pos1(1, 1, "test.c");
    try {
        bool includeResult = processor->processIncludeDirective("<stdio.h>", pos1);
        std::cout << "✅ processIncludeDirective() executado (resultado: " << (includeResult ? "sucesso" : "falha") << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠️ processIncludeDirective() falhou com exceção: " << e.what() << std::endl;
    }
    
    // Teste processDefineDirective
    std::cout << "[DEBUG] Testando processDefineDirective..." << std::endl;
    PreprocessorPosition pos2(2, 1, "test.c");
    try {
        bool defineResult = processor->processDefineDirective("MAX", "100", pos2);
        std::cout << "✅ processDefineDirective() executado (resultado: " << (defineResult ? "sucesso" : "falha") << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠️ processDefineDirective() falhou com exceção: " << e.what() << std::endl;
    }
    
    // Teste processIfDirective
    std::cout << "[DEBUG] Testando processIfDirective..." << std::endl;
    PreprocessorPosition pos3(3, 1, "test.c");
    try {
        bool ifResult = processor->processIfDirective("1", pos3);
        std::cout << "✅ processIfDirective() executado (resultado: " << (ifResult ? "sucesso" : "falha") << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠️ processIfDirective() falhou com exceção: " << e.what() << std::endl;
    }
    
    // Teste processElseDirective
    std::cout << "[DEBUG] Testando processElseDirective..." << std::endl;
    PreprocessorPosition pos4(4, 1, "test.c");
    try {
        bool elseResult = processor->processElseDirective(pos4);
        std::cout << "✅ processElseDirective() executado (resultado: " << (elseResult ? "sucesso" : "falha") << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠️ processElseDirective() falhou com exceção: " << e.what() << std::endl;
    }
    
    // Teste processEndifDirective
    std::cout << "[DEBUG] Testando processEndifDirective..." << std::endl;
    PreprocessorPosition pos5(5, 1, "test.c");
    try {
        bool endifResult = processor->processEndifDirective(pos5);
        std::cout << "✅ processEndifDirective() executado (resultado: " << (endifResult ? "sucesso" : "falha") << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠️ processEndifDirective() falhou com exceção: " << e.what() << std::endl;
    }
    
    std::cout << "[DEBUG] testSpecificDirectiveProcessors concluído" << std::endl;
}

void testUtilityMethods() {
    std::cout << "\n=== Testando Métodos Utilitários ===" << std::endl;
    
    std::cout << "[DEBUG] Criando DirectiveProcessor para testUtilityMethods..." << std::endl;
    auto processor = createDirectiveProcessor();
    std::cout << "[DEBUG] DirectiveProcessor criado" << std::endl;
    
    // Teste extractDirectiveName
    std::cout << "[DEBUG] Testando extractDirectiveName..." << std::endl;
    std::string name1 = processor->extractDirectiveName("#include <stdio.h>");
    assertEqual("include", name1, "extractDirectiveName() extrai 'include' corretamente");
    
    std::string name2 = processor->extractDirectiveName("#define MAX 100");
    assertEqual("define", name2, "extractDirectiveName() extrai 'define' corretamente");
    
    std::string name3 = processor->extractDirectiveName("#if defined(DEBUG)");
    assertEqual("if", name3, "extractDirectiveName() extrai 'if' corretamente");
    
    // Teste extractDirectiveArguments
    std::cout << "[DEBUG] Testando extractDirectiveArguments..." << std::endl;
    std::string args1 = processor->extractDirectiveArguments("#include <stdio.h>", "include");
    assertTrue(!args1.empty(), "extractDirectiveArguments() extrai argumentos de #include");
    
    std::string args2 = processor->extractDirectiveArguments("#define MAX 100", "define");
    assertTrue(!args2.empty(), "extractDirectiveArguments() extrai argumentos de #define");
    
    // Teste normalizeDirectiveLine
    std::cout << "[DEBUG] Testando normalizeDirectiveLine..." << std::endl;
    std::string normalized1 = processor->normalizeDirectiveLine("  #include <stdio.h>  // comment");
    assertTrue(normalized1.find("#include") != std::string::npos, "normalizeDirectiveLine() preserva diretiva");
    assertTrue(normalized1.find("//") == std::string::npos, "normalizeDirectiveLine() remove comentários");
    
    std::cout << "[DEBUG] testUtilityMethods concluído" << std::endl;
}

int main() {
    try {
        std::cout << "=== TESTE DE SEQUÊNCIA: testSpecificDirectiveProcessors -> testUtilityMethods ===" << std::endl;
        
        std::cout << "[DEBUG] Iniciando testSpecificDirectiveProcessors..." << std::endl;
        testSpecificDirectiveProcessors();
        std::cout << "[DEBUG] testSpecificDirectiveProcessors concluído com sucesso" << std::endl;
        
        std::cout << "[DEBUG] Iniciando testUtilityMethods..." << std::endl;
        testUtilityMethods();
        std::cout << "[DEBUG] testUtilityMethods concluído com sucesso" << std::endl;
        
        std::cout << "\n🎉 Sequência de testes concluída com sucesso!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro durante os testes: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}