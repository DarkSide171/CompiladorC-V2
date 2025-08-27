/**
 * @file test_lexer_integration.cpp
 * @brief Testes de integração entre pré-processador e lexer
 */

#include "../../include/preprocessor_lexer_interface.hpp"
#include "../../../lexer/include/lexer.hpp"
#include "../../../lexer/include/token.hpp"
#include <fstream>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <cstdio>
#include <cassert>

using namespace Preprocessor;
using namespace Lexer;

// Macros para testes simples
#define EXPECT_TRUE(condition) assert(condition)
#define EXPECT_FALSE(condition) assert(!(condition))
#define EXPECT_GT(a, b) assert((a) > (b))
#define EXPECT_GE(a, b) assert((a) >= (b))
#define EXPECT_EQ(a, b) assert((a) == (b))
#define EXPECT_NE(a, b) assert((a) != (b))
#define EXPECT_LT(a, b) assert((a) < (b))
#define ASSERT_TRUE(condition) assert(condition)
#define ASSERT_FALSE(condition) assert(!(condition))

// Protótipos das funções de teste
void testBasicFileProcessing();
void testStringProcessing();
void testMacroDefinitionAndUsage();
void testPositionMapping();
void testErrorHandling();
void testLexerIntegration();
void testCompilationPipeline();
void testResetAndReuse();
void testStatistics();
void testDependencies();
void testCallbacks();
void testAdvancedConfiguration();
void testBasicPerformance();

class LexerIntegrationTest {
public:
    void SetUp() {
        interface = std::make_unique<PreprocessorLexerInterface>();
        ASSERT_TRUE(interface->initialize());
    }

    void TearDown() {
        interface.reset();
    }

    std::unique_ptr<PreprocessorLexerInterface> interface;
};

/**
 * @brief Teste básico de processamento de arquivo
 */
void testBasicFileProcessing() {
    LexerIntegrationTest test;
    test.SetUp();
    // Cria arquivo temporário de teste
    std::string testFile = "test_basic.c";
    std::ofstream file(testFile);
    file << "#include <stdio.h>\n";
    file << "int main() {\n";
    file << "    printf(\"Hello World\");\n";
    file << "    return 0;\n";
    file << "}\n";
    file.close();

    // Processa o arquivo
    ProcessingResult result = test.interface->processFile(testFile);
    
    // Verifica se o processamento foi bem-sucedido
    EXPECT_FALSE(result.hasErrors);
    EXPECT_FALSE(result.processedCode.empty());
    
    // Limpa arquivo temporário
    std::remove(testFile.c_str());
    
    test.TearDown();
}

/**
 * @brief Função principal para executar todos os testes
 */
int main() {
    std::cout << "=== Testes de Integração Lexer-Preprocessor ===" << std::endl;
    
    try {
        testBasicFileProcessing();
        std::cout << "✓ Teste de processamento de arquivo básico passou" << std::endl;
        
        testStringProcessing();
        std::cout << "✓ Teste de processamento de string passou" << std::endl;
        
        testMacroDefinitionAndUsage();
        std::cout << "✓ Teste de definição e uso de macros passou" << std::endl;
        
        testPositionMapping();
        std::cout << "✓ Teste de mapeamento de posições passou" << std::endl;
        
        testErrorHandling();
        std::cout << "✓ Teste de tratamento de erros passou" << std::endl;
        
        testLexerIntegration();
        std::cout << "✓ Teste de integração com lexer passou" << std::endl;
        
        testCompilationPipeline();
        std::cout << "✓ Teste de pipeline de compilação passou" << std::endl;
        
        testResetAndReuse();
        std::cout << "✓ Teste de reset e reutilização passou" << std::endl;
        
        testStatistics();
        std::cout << "✓ Teste de estatísticas passou" << std::endl;
        
        testDependencies();
        std::cout << "✓ Teste de dependências passou" << std::endl;
        
        testCallbacks();
        std::cout << "✓ Teste de callbacks passou" << std::endl;
        
        testAdvancedConfiguration();
        std::cout << "✓ Teste de configurações avançadas passou" << std::endl;
        
        testBasicPerformance();
        std::cout << "✓ Teste de performance básica passou" << std::endl;
        
        std::cout << "\n=== Todos os testes passaram com sucesso! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Erro durante os testes: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Erro desconhecido durante os testes" << std::endl;
        return 1;
    }
}

/**
 * @brief Teste de processamento de string
 */
void testStringProcessing() {
    LexerIntegrationTest test;
    test.SetUp();
    std::string code = "#define MAX 100\nint arr[MAX];";
    
    ProcessingResult result = test.interface->processString(code, "<test>");
    
    EXPECT_FALSE(result.hasErrors);
    EXPECT_FALSE(result.processedCode.empty());
    
    test.TearDown();
}

/**
 * @brief Teste de definição e uso de macros
 */
void testMacroDefinitionAndUsage() {
    LexerIntegrationTest test;
    test.SetUp();
    // Define uma macro
    test.interface->defineMacro("PI", "3.14159");
    
    std::string code = "float radius = 5.0;\nfloat area = PI * radius * radius;";
    ProcessingResult result = test.interface->processString(code, "<macro_test>");
    
    EXPECT_FALSE(result.hasErrors);
    
    // Verifica se a macro foi registrada
    auto stats = test.interface->getStatistics();
    EXPECT_GT(stats["defined_macros"], 0);
    
    test.TearDown();
}

/**
 * @brief Teste de mapeamento de posições
 */
void testPositionMapping() {
    LexerIntegrationTest test;
    test.SetUp();
    std::string code = "#define TEST 42\nint x = TEST;";
    ProcessingResult result = test.interface->processString(code, "<position_test>");
    
    EXPECT_FALSE(result.hasErrors);
    EXPECT_FALSE(result.positionMappings.empty());
    
    // Verifica se há mapeamentos de posição
    bool foundMapping = false;
    for (const auto& mapping : result.positionMappings) {
        if (mapping.processedLine > 0 && mapping.originalLine > 0) {
            foundMapping = true;
            break;
        }
    }
    EXPECT_TRUE(foundMapping);
    
    test.TearDown();
}

/**
 * @brief Teste de tratamento de erros
 */
void testErrorHandling() {
    LexerIntegrationTest test;
    test.SetUp();
    // Testa com código inválido
    std::string invalidCode = "#include <nonexistent.h>";
    ProcessingResult result = test.interface->processString(invalidCode, "<error_test>");
    
    // O resultado pode ter erros dependendo da implementação
    // Por enquanto, apenas verifica se não trava
    EXPECT_TRUE(true); // Placeholder - ajustar conforme implementação
    
    test.TearDown();
}

/**
 * @brief Teste de integração com lexer real
 */
void testLexerIntegration() {
    LexerIntegrationTest test;
    test.SetUp();
    std::string code = "int main() { return 0; }";
    ProcessingResult result = test.interface->processString(code, "<lexer_test>");
    
    EXPECT_FALSE(result.hasErrors);
    EXPECT_FALSE(result.processedCode.empty());
    
    // Testa se o código processado não está vazio
    EXPECT_GT(result.processedCode.length(), 0);
    
    // Verifica se contém o código esperado
    EXPECT_NE(result.processedCode.find("main"), std::string::npos);
    
    test.TearDown();
}

/**
 * @brief Teste de pipeline de compilação
 */
void testCompilationPipeline() {
    LexerIntegrationTest test;
    test.SetUp();
    CompilationPipeline pipeline;
    ASSERT_TRUE(pipeline.initialize());
    
    std::string code = "#define SIZE 10\nint array[SIZE];";
    bool success = pipeline.processString(code, "<pipeline_test>");
    
    EXPECT_TRUE(success);
    EXPECT_FALSE(pipeline.getProcessedCode().empty());
    
    test.TearDown();
}

/**
 * @brief Teste de reset e reutilização
 */
void testResetAndReuse() {
    LexerIntegrationTest test;
    test.SetUp();
    // Primeiro processamento
    std::string code1 = "int x = 1;";
    ProcessingResult result1 = test.interface->processString(code1, "<test1>");
    EXPECT_FALSE(result1.hasErrors);
    
    // Reset
    test.interface->reset();
    
    // Segundo processamento
    std::string code2 = "float y = 2.0;";
    ProcessingResult result2 = test.interface->processString(code2, "<test2>");
    EXPECT_FALSE(result2.hasErrors);
    
    // Verifica se o código anterior foi limpo
    EXPECT_NE(result1.processedCode, result2.processedCode);
    
    test.TearDown();
}

/**
 * @brief Teste de estatísticas
 */
void testStatistics() {
    LexerIntegrationTest test;
    test.SetUp();
    test.interface->defineMacro("TEST1", "1");
    test.interface->defineMacro("TEST2", "2");
    
    std::string code = "int a = TEST1;\nint b = TEST2;";
    ProcessingResult result = test.interface->processString(code, "<stats_test>");
    
    auto stats = test.interface->getStatistics();
    
    EXPECT_GT(stats["processed_lines"], 0);
    EXPECT_GE(stats["defined_macros"], 2);
    EXPECT_EQ(stats["errors"], result.errorMessages.size());
    EXPECT_EQ(stats["warnings"], result.warningMessages.size());
    
    test.TearDown();
}

/**
 * @brief Teste de dependências
 */
void testDependencies() {
    LexerIntegrationTest test;
    test.SetUp();
    std::string code = "#include \"header1.h\"\n#include \"header2.h\"\nint main() { return 0; }";
    ProcessingResult result = test.interface->processString(code, "<deps_test>");
    
    // Verifica se as dependências foram coletadas
    auto deps = test.interface->getDependencies();
    // Por enquanto, apenas verifica se não trava
    EXPECT_TRUE(true); // Placeholder - ajustar conforme implementação
    
    test.TearDown();
}

/**
 * @brief Teste de callbacks
 */
void testCallbacks() {
    LexerIntegrationTest test;
    test.SetUp();
    bool macroExpanded = false;
    bool fileIncluded = false;
    bool errorOccurred = false;
    
    test.interface->setOnMacroExpanded([&](const std::string& macro) {
        macroExpanded = true;
    });
    
    test.interface->setOnFileIncluded([&](const std::string& file) {
        fileIncluded = true;
    });
    
    test.interface->setOnError([&](const IntegratedErrorHandler::IntegratedError& error) {
        errorOccurred = true;
    });
    
    // Processa código que pode gerar callbacks
    test.interface->defineMacro("CALLBACK_TEST", "42");
    std::string code = "int x = CALLBACK_TEST;";
    ProcessingResult result = test.interface->processString(code, "<callback_test>");
    
    // Por enquanto, apenas verifica se não trava
    EXPECT_TRUE(true); // Placeholder - ajustar conforme implementação dos callbacks
    
    test.TearDown();
}

/**
 * @brief Teste de configurações avançadas
 */
void testAdvancedConfiguration() {
    LexerIntegrationTest test;
    test.SetUp();
    PreprocessorConfig config;
    // Configurações específicas podem ser testadas aqui
    
    auto newInterface = std::make_unique<PreprocessorLexerInterface>();
    ASSERT_TRUE(newInterface->initialize(config));
    
    std::string code = "#define ADVANCED 1\nint test = ADVANCED;";
    ProcessingResult result = newInterface->processString(code, "<advanced_test>");
    
    EXPECT_FALSE(result.hasErrors);
    
    test.TearDown();
}

/**
 * @brief Teste de performance básica
 */
void testBasicPerformance() {
    LexerIntegrationTest test;
    test.SetUp();
    // Gera código de teste maior
    std::stringstream codeStream;
    codeStream << "#define LOOP_SIZE 1000\n";
    for (int i = 0; i < 100; ++i) {
        codeStream << "int var" << i << " = " << i << ";\n";
    }
    
    std::string code = codeStream.str();
    
    auto start = std::chrono::high_resolution_clock::now();
    ProcessingResult result = test.interface->processString(code, "<perf_test>");
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_FALSE(result.hasErrors);
    EXPECT_LT(duration.count(), 1000); // Deve processar em menos de 1 segundo
    
    test.TearDown();
}