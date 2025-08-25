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

void testUtilityMethods() {
    std::cout << "\n=== Testando Métodos Utilitários ===" << std::endl;
    
    std::cout << "[DEBUG] Criando DirectiveProcessor..." << std::endl;
    auto processor = createDirectiveProcessor();
    std::cout << "[DEBUG] DirectiveProcessor criado" << std::endl;
    
    // Teste extractDirectiveName - EXATAMENTE como no teste original
    std::cout << "[DEBUG] Testando extractDirectiveName #1..." << std::endl;
    std::string name1 = processor->extractDirectiveName("#include <stdio.h>");
    std::cout << "[DEBUG] name1 = '" << name1 << "'" << std::endl;
    assertEqual("include", name1, "extractDirectiveName() extrai 'include' corretamente");
    
    std::cout << "[DEBUG] Testando extractDirectiveName #2..." << std::endl;
    std::string name2 = processor->extractDirectiveName("#define MAX 100");
    std::cout << "[DEBUG] name2 = '" << name2 << "'" << std::endl;
    assertEqual("define", name2, "extractDirectiveName() extrai 'define' corretamente");
    
    std::cout << "[DEBUG] Testando extractDirectiveName #3..." << std::endl;
    std::string name3 = processor->extractDirectiveName("#if defined(DEBUG)");
    std::cout << "[DEBUG] name3 = '" << name3 << "'" << std::endl;
    assertEqual("if", name3, "extractDirectiveName() extrai 'if' corretamente");
    
    // Teste extractDirectiveArguments - EXATAMENTE como no teste original
    std::cout << "[DEBUG] Testando extractDirectiveArguments #1..." << std::endl;
    std::string args1 = processor->extractDirectiveArguments("#include <stdio.h>", "include");
    std::cout << "[DEBUG] args1 = '" << args1 << "'" << std::endl;
    assertTrue(!args1.empty(), "extractDirectiveArguments() extrai argumentos de #include");
    
    std::cout << "[DEBUG] Testando extractDirectiveArguments #2..." << std::endl;
    std::string args2 = processor->extractDirectiveArguments("#define MAX 100", "define");
    std::cout << "[DEBUG] args2 = '" << args2 << "'" << std::endl;
    assertTrue(!args2.empty(), "extractDirectiveArguments() extrai argumentos de #define");
    
    // Teste normalizeDirectiveLine - EXATAMENTE como no teste original
    std::cout << "[DEBUG] Testando normalizeDirectiveLine..." << std::endl;
    std::string normalized1 = processor->normalizeDirectiveLine("  #include <stdio.h>  // comment");
    std::cout << "[DEBUG] normalized1 = '" << normalized1 << "'" << std::endl;
    assertTrue(normalized1.find("#include") != std::string::npos, "normalizeDirectiveLine() preserva diretiva");
    assertTrue(normalized1.find("//") == std::string::npos, "normalizeDirectiveLine() remove comentários");
    
    std::cout << "[DEBUG] testUtilityMethods concluído com sucesso!" << std::endl;
}

int main() {
    try {
        std::cout << "=== TESTE EXATO DA FUNÇÃO testUtilityMethods ===" << std::endl;
        testUtilityMethods();
        std::cout << "\n🎉 Teste concluído com sucesso!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro durante os testes: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}