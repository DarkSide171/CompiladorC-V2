#include "../../include/directive.hpp"
#include "../../include/preprocessor_state.hpp"
#include "../../include/preprocessor_logger.hpp"
#include "../../include/macro_processor.hpp"
#include "../../include/file_manager.hpp"
#include "../../include/conditional_processor.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace Preprocessor;

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

void testParseDirective() {
    std::cout << "\n=== Testando parseDirective() ===" << std::endl;
    
    auto processor = createDirectiveProcessor();
    
    // Teste 1: Diretiva #include válida
    std::string line1 = "#include <stdio.h>";
    PreprocessorPosition pos1(1, 1, "test.c");
    Directive directive1 = processor->parseDirective(line1, pos1);
    
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
    std::cout << "✅ validateDirectiveSyntax() testado (resultado: " << (valid1 ? "válido" : "inválido") << ")" << std::endl;
}

int main() {
    try {
        std::cout << "=== TESTE DE DEBUG COM VARIÁVEIS GLOBAIS ===" << std::endl;
        
        testParseDirective();
        testValidateDirectiveSyntax();
        
        std::cout << "\n🎉 Todos os testes executados com sucesso!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro durante os testes: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}