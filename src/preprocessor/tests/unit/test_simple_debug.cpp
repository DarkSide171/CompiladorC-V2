#include "../../include/directive.hpp"
#include "../../include/preprocessor_state.hpp"
#include "../../include/preprocessor_logger.hpp"
#include "../../include/macro_processor.hpp"
#include "../../include/file_manager.hpp"
#include "../../include/conditional_processor.hpp"
#include <iostream>
#include <memory>

using namespace Preprocessor;

int main() {
    std::cout << "=== Teste de Debug Simples ===" << std::endl;
    
    try {
        std::cout << "1. Criando PreprocessorState..." << std::endl;
        auto state = std::make_shared<PreprocessorState>();
        std::cout << "✅ PreprocessorState criado com sucesso" << std::endl;
        
        std::cout << "2. Criando PreprocessorLogger..." << std::endl;
        auto logger = std::make_shared<PreprocessorLogger>();
        std::cout << "✅ PreprocessorLogger criado com sucesso" << std::endl;
        
        std::cout << "3. Criando MacroProcessor..." << std::endl;
        auto macroProcessor = std::make_shared<MacroProcessor>(logger, state);
        std::cout << "✅ MacroProcessor criado com sucesso" << std::endl;
        
        std::cout << "4. Criando FileManager..." << std::endl;
        auto fileManager = std::make_shared<FileManager>(std::vector<std::string>{}, logger.get());
        std::cout << "✅ FileManager criado com sucesso" << std::endl;
        
        std::cout << "5. Criando ConditionalProcessor..." << std::endl;
        auto conditionalProcessor = std::make_shared<ConditionalProcessor>(logger.get(), macroProcessor.get());
        std::cout << "✅ ConditionalProcessor criado com sucesso" << std::endl;
        
        std::cout << "6. Criando DirectiveProcessor..." << std::endl;
        auto directiveProcessor = std::make_unique<DirectiveProcessor>(
            state.get(), logger.get(), macroProcessor.get(), 
            fileManager.get(), conditionalProcessor.get()
        );
        std::cout << "✅ DirectiveProcessor criado com sucesso" << std::endl;
        
        std::cout << "\n🎉 Todos os objetos foram criados com sucesso!" << std::endl;
        
        std::cout << "\n7. Testando processIncludeDirective..." << std::endl;
        PreprocessorPosition pos1(1, 1, "test.c");
        bool includeResult = directiveProcessor->processIncludeDirective("<stdio.h>", pos1);
        std::cout << "✅ processIncludeDirective executado (resultado: " << (includeResult ? "sucesso" : "falha") << ")" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro durante a criação: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "❌ Erro desconhecido durante a criação" << std::endl;
        return 1;
    }
    
    return 0;
}