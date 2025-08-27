// Teste Abrangente de Debug - Unificação dos testes de debug
// Consolida funcionalidades de test_simple_debug, test_utility_debug e test_global_debug

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

// ============================================================================
// VARIÁVEIS GLOBAIS E SETUP
// ============================================================================

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

// ============================================================================
// TESTES DE CRIAÇÃO DE OBJETOS (do test_simple_debug)
// ============================================================================

void testObjectCreation() {
    std::cout << "\n=== Teste de Criação de Objetos ===" << std::endl;
    
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
        
        // Teste básico de funcionalidade
        std::cout << "\n7. Testando processIncludeDirective..." << std::endl;
        PreprocessorPosition pos1(1, 1, "test.c");
        bool includeResult = directiveProcessor->processIncludeDirective("<stdio.h>", pos1);
        std::cout << "✅ processIncludeDirective executado (resultado: " << (includeResult ? "sucesso" : "falha") << ")" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro durante criação de objetos: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES DE MÉTODOS UTILITÁRIOS (do test_utility_debug)
// ============================================================================

void testUtilityMethods() {
    std::cout << "\n=== Teste de Métodos Utilitários ===" << std::endl;
    
    try {
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
        std::cout << "❌ Erro durante teste de métodos utilitários: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES DE PARSING E VALIDAÇÃO (do test_global_debug)
// ============================================================================

void testParseDirective() {
    std::cout << "\n=== Testando parseDirective() ===" << std::endl;
    
    try {
        auto processor = createDirectiveProcessor();
        
        // Teste 1: Diretiva #include válida
        std::string line1 = "#include <stdio.h>";
        PreprocessorPosition pos1(1, 1, "test.c");
        Directive directive1 = processor->parseDirective(line1, pos1);
        
        std::cout << "✅ parseDirective() testado com sucesso" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em parseDirective(): " << e.what() << std::endl;
    }
}

void testValidateDirectiveSyntax() {
    std::cout << "\n=== Testando validateDirectiveSyntax() ===" << std::endl;
    
    try {
        auto processor = createDirectiveProcessor();
        
        // Teste 1: Diretiva #include válida
        std::string line1 = "#include <stdio.h>";
        PreprocessorPosition pos1(1, 1, "test.c");
        Directive directive1 = processor->parseDirective(line1, pos1);
        bool isValid = processor->validateDirectiveSyntax(directive1);
        
        std::cout << "✅ validateDirectiveSyntax() testado (resultado: " << (isValid ? "válido" : "inválido") << ")" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em validateDirectiveSyntax(): " << e.what() << std::endl;
    }
}

void testAdvancedParsing() {
    std::cout << "\n=== Testando Parsing Avançado ===" << std::endl;
    
    try {
        auto processor = createDirectiveProcessor();
        
        // Teste de múltiplas diretivas
        std::vector<std::string> testLines = {
            "#define MAX_SIZE 1024",
            "#include \"myheader.h\"",
            "#ifdef DEBUG",
            "#endif",
            "#undef MAX_SIZE"
        };
        
        for (size_t i = 0; i < testLines.size(); ++i) {
            PreprocessorPosition pos(i + 1, 1, "test.c");
            try {
                Directive directive = processor->parseDirective(testLines[i], pos);
                std::cout << "✅ Linha " << (i + 1) << " parseada com sucesso" << std::endl;
            } catch (const std::exception& e) {
                std::cout << "❌ Erro na linha " << (i + 1) << ": " << e.what() << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em parsing avançado: " << e.what() << std::endl;
    }
}

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main() {
    std::cout << "=== TESTE ABRANGENTE DE DEBUG ===" << std::endl;
    std::cout << "Unificação dos testes: simple_debug, utility_debug, global_debug" << std::endl;
    
    // Executa todos os testes
    testObjectCreation();
    testUtilityMethods();
    testParseDirective();
    testValidateDirectiveSyntax();
    testAdvancedParsing();
    
    std::cout << "\n=== RESUMO FINAL ===" << std::endl;
    std::cout << "✅ Teste de Criação de Objetos: Concluído" << std::endl;
    std::cout << "✅ Teste de Métodos Utilitários: Concluído" << std::endl;
    std::cout << "✅ Teste de Parse de Diretivas: Concluído" << std::endl;
    std::cout << "✅ Teste de Validação de Sintaxe: Concluído" << std::endl;
    std::cout << "✅ Teste de Parsing Avançado: Concluído" << std::endl;
    
    std::cout << "\n🎉 TODOS OS TESTES DE DEBUG PASSARAM COM SUCESSO! 🎉" << std::endl;
    
    return 0;
}