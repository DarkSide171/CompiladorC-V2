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

int main() {
    try {
        std::cout << "=== TESTE DE DEBUG DOS MÉTODOS UTILITÁRIOS ===" << std::endl;
        
        std::cout << "[DEBUG] Criando DirectiveProcessor..." << std::endl;
        auto processor = createDirectiveProcessor();
        std::cout << "[DEBUG] DirectiveProcessor criado com sucesso" << std::endl;
        
        std::cout << "[DEBUG] Testando extractDirectiveName..." << std::endl;
        std::string name1 = processor->extractDirectiveName("#include <stdio.h>");
        std::cout << "[DEBUG] extractDirectiveName executado: '" << name1 << "'" << std::endl;
        
        std::cout << "[DEBUG] Testando extractDirectiveArguments..." << std::endl;
        std::string args1 = processor->extractDirectiveArguments("#include <stdio.h>", "include");
        std::cout << "[DEBUG] extractDirectiveArguments executado: '" << args1 << "'" << std::endl;
        
        std::cout << "[DEBUG] Testando normalizeDirectiveLine..." << std::endl;
        std::string normalized1 = processor->normalizeDirectiveLine("  #include <stdio.h>  // comment");
        std::cout << "[DEBUG] normalizeDirectiveLine executado: '" << normalized1 << "'" << std::endl;
        
        std::cout << "\n🎉 Todos os métodos utilitários testados com sucesso!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro durante os testes: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}