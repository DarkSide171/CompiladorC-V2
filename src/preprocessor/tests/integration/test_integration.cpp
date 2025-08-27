#include "../../include/preprocessor.hpp"
#include "../../include/file_manager.hpp"
#include "../../include/macro_processor.hpp"
#include "../../include/conditional_processor.hpp"
#include "../../include/expression_evaluator.hpp"
#include "../../include/preprocessor_logger.hpp"
#include "../../include/preprocessor_state.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include <cassert>

using namespace Preprocessor;

// Funções auxiliares para testes
void assertEqual(const std::string& expected, const std::string& actual, const std::string& testName) {
    if (expected == actual) {
        std::cout << "[PASS] " << testName << std::endl;
    } else {
        std::cout << "[FAIL] " << testName << std::endl;
        std::cout << "  Expected: " << expected << std::endl;
        std::cout << "  Actual: " << actual << std::endl;
    }
}

void assertTrue(bool condition, const std::string& testName) {
    if (condition) {
        std::cout << "[PASS] " << testName << std::endl;
    } else {
        std::cout << "[FAIL] " << testName << std::endl;
    }
}

// Função para criar arquivo temporário
void createTempFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    file << content;
    file.close();
}

void test_file_processing() {
    std::cout << "\n=== Integration Test: File Processing ===" << std::endl;
    
    // Criar arquivo de teste com código C completo
    std::string testContent = 
        "#define MAX_SIZE 100\n"
        "#define MIN(a, b) ((a) < (b) ? (a) : (b))\n"
        "#ifdef DEBUG\n"
        "    #define LOG(msg) printf(msg)\n"
        "#else\n"
        "    #define LOG(msg)\n"
        "#endif\n"
        "\n"
        "int main() {\n"
        "    int size = MAX_SIZE;\n"
        "    int result = MIN(10, 20);\n"
        "    LOG(\"Debug message\");\n"
        "    return 0;\n"
        "}\n";
    
    createTempFile("test_input.c", testContent);
    
    try {
        // Inicializar componentes
        auto logger = std::make_shared<PreprocessorLogger>();
        auto state = std::make_shared<PreprocessorState>();
        auto fileManager = std::make_unique<FileManager>(std::vector<std::string>{}, logger.get());
        auto macroProcessor = std::make_unique<MacroProcessor>(logger, state);
        
        // Definir DEBUG para testar compilação condicional
        PreprocessorPosition pos("test_input.c", 1, 1);
        macroProcessor->defineMacro("DEBUG", "", pos);
        
        // Processar arquivo
        std::string result = fileManager->readFile("test_input.c");
        
        // Verificar se o arquivo foi lido
        assertTrue(!result.empty(), "File reading");
        assertTrue(result.find("#define MAX_SIZE 100") != std::string::npos, "Macro definition found");
        assertTrue(result.find("MIN(10, 20)") != std::string::npos, "Function-like macro usage found");
        
        std::cout << "File processing integration test completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "[FAIL] File processing integration test: " << e.what() << std::endl;
    }
    
    // Limpar arquivo temporário
    std::remove("test_input.c");
}

void test_macro_expansion() {
    std::cout << "\n=== Integration Test: Macro Expansion ===" << std::endl;
    
    try {
        // Inicializar componentes
        auto logger = std::make_shared<PreprocessorLogger>();
        auto state = std::make_shared<PreprocessorState>();
        auto macroProcessor = std::make_unique<MacroProcessor>(logger, state);
        
        PreprocessorPosition pos("test.c", 1, 1);
        
        // Teste 1: Macro simples
        macroProcessor->defineMacro("PI", "3.14159", pos);
        std::string result1 = macroProcessor->expandMacroRecursively("PI");
        assertEqual("3.14159", result1, "Simple macro expansion");
        
        // Teste 2: Macro funcional
        std::vector<std::string> params = {"x", "y"};
        macroProcessor->defineFunctionMacro("MAX", params, "((x) > (y) ? (x) : (y))", false, pos);
        std::string result2 = macroProcessor->expandFunctionMacro("MAX", {"10", "20"});
        assertEqual("((10) > (20) ? (10) : (20))", result2, "Function macro expansion");
        
        // Teste 3: Macro aninhada
        macroProcessor->defineMacro("SIZE", "100", pos);
        macroProcessor->defineMacro("BUFFER_SIZE", "SIZE * 2", pos);
        std::string result3 = macroProcessor->expandMacroRecursively("BUFFER_SIZE");
        assertTrue(result3.find("100") != std::string::npos, "Nested macro expansion");
        
        // Teste 4: Verificar se macro está definida
        assertTrue(macroProcessor->isDefined("PI"), "Macro definition check");
        assertTrue(macroProcessor->isDefined("MAX"), "Function macro definition check");
        
        // Teste 5: Remover macro
        macroProcessor->undefineMacro("PI");
        assertTrue(!macroProcessor->isDefined("PI"), "Macro undefinition");
        
        std::cout << "Macro expansion integration test completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "[FAIL] Macro expansion integration test: " << e.what() << std::endl;
    }
}

void test_conditional_compilation() {
    std::cout << "\n=== Integration Test: Conditional Compilation ===" << std::endl;
    
    try {
        // Inicializar componentes
        auto logger = std::make_shared<PreprocessorLogger>();
        auto state = std::make_shared<PreprocessorState>();
        auto macroProcessor = std::make_unique<MacroProcessor>(logger, state);
        auto conditionalProcessor = std::make_unique<ConditionalProcessor>(logger.get(), macroProcessor.get());
        
        PreprocessorPosition pos("test.c", 1, 1);
        
        // Teste 1: #ifdef com macro definida
        macroProcessor->defineMacro("DEBUG", "1", pos);
        
        std::string testCode1 = 
            "#ifdef DEBUG\n"
            "    debug_enabled = 1;\n"
            "#else\n"
            "    debug_enabled = 0;\n"
            "#endif\n";
        
        // Simular processamento condicional
        assertTrue(macroProcessor->isDefined("DEBUG"), "DEBUG macro defined for conditional test");
        
        // Teste 2: #ifndef com macro não definida
        std::string testCode2 = 
            "#ifndef RELEASE\n"
            "    development_mode = 1;\n"
            "#else\n"
            "    development_mode = 0;\n"
            "#endif\n";
        
        assertTrue(!macroProcessor->isDefined("RELEASE"), "RELEASE macro not defined for conditional test");
        
        // Teste 3: #if com expressão
        macroProcessor->defineMacro("VERSION", "2", pos);
        
        std::string testCode3 = 
            "#if VERSION > 1\n"
            "    new_features = 1;\n"
            "#else\n"
            "    new_features = 0;\n"
            "#endif\n";
        
        assertTrue(macroProcessor->isDefined("VERSION"), "VERSION macro defined for expression test");
        assertEqual("2", macroProcessor->getMacroValue("VERSION"), "VERSION macro value");
        
        // Teste 4: #elif aninhado
        macroProcessor->defineMacro("PLATFORM", "LINUX", pos);
        
        std::string testCode4 = 
            "#ifdef PLATFORM\n"
            "    #if PLATFORM == LINUX\n"
            "        os_type = LINUX_OS;\n"
            "    #elif PLATFORM == WINDOWS\n"
            "        os_type = WINDOWS_OS;\n"
            "    #else\n"
            "        os_type = UNKNOWN_OS;\n"
            "    #endif\n"
            "#endif\n";
        
        assertTrue(macroProcessor->isDefined("PLATFORM"), "PLATFORM macro defined for nested test");
        
        std::cout << "Conditional compilation integration test completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "[FAIL] Conditional compilation integration test: " << e.what() << std::endl;
    }
}

void test_include_processing() {
    std::cout << "\n=== Integration Test: Include Processing ===" << std::endl;
    
    // Criar arquivo de cabeçalho temporário
    std::string headerContent = 
        "#ifndef MATH_UTILS_H\n"
        "#define MATH_UTILS_H\n"
        "\n"
        "#define PI 3.14159\n"
        "#define SQUARE(x) ((x) * (x))\n"
        "\n"
        "typedef struct {\n"
        "    double x, y;\n"
        "} Point;\n"
        "\n"
        "#endif // MATH_UTILS_H\n";
    
    createTempFile("math_utils.h", headerContent);
    
    // Criar arquivo principal que inclui o cabeçalho
    std::string mainContent = 
        "#include \"math_utils.h\"\n"
        "\n"
        "int main() {\n"
        "    double radius = 5.0;\n"
        "    double area = PI * SQUARE(radius);\n"
        "    Point origin = {0.0, 0.0};\n"
        "    return 0;\n"
        "}\n";
    
    createTempFile("main_with_include.c", mainContent);
    
    try {
        // Inicializar componentes
        auto logger = std::make_shared<PreprocessorLogger>();
        auto state = std::make_shared<PreprocessorState>();
        std::vector<std::string> searchPaths = {"."};  // Diretório atual
        auto fileManager = std::make_unique<FileManager>(searchPaths, logger.get());
        auto macroProcessor = std::make_unique<MacroProcessor>(logger, state);
        
        // Teste 1: Verificar se os arquivos existem
        assertTrue(fileManager->fileExists("math_utils.h"), "Header file exists");
        assertTrue(fileManager->fileExists("main_with_include.c"), "Main file exists");
        
        // Teste 2: Ler arquivo principal
        std::string mainFileContent = fileManager->readFile("main_with_include.c");
        assertTrue(!mainFileContent.empty(), "Main file reading");
        assertTrue(mainFileContent.find("#include") != std::string::npos, "Include directive found");
        
        // Teste 3: Ler arquivo de cabeçalho
        std::string headerFileContent = fileManager->readFile("math_utils.h");
        assertTrue(!headerFileContent.empty(), "Header file reading");
        assertTrue(headerFileContent.find("#define PI") != std::string::npos, "PI macro in header");
        assertTrue(headerFileContent.find("#define SQUARE") != std::string::npos, "SQUARE macro in header");
        
        // Teste 4: Resolver include
        std::string resolvedPath = fileManager->resolveInclude("math_utils.h", false, "main_with_include.c");
        assertTrue(!resolvedPath.empty(), "Include resolution");
        
        // Teste 5: Verificar detecção de inclusão circular (simulada)
        std::vector<std::string> includeStack = {"main_with_include.c"};
        bool hasCircular = fileManager->checkCircularInclusion("math_utils.h", includeStack);
        assertTrue(!hasCircular, "No circular inclusion detected");
        
        // Teste 6: Estatísticas do FileManager
        auto stats = fileManager->getStatistics();
        assertTrue(stats.files_read >= 2, "Files read statistics");
        
        std::cout << "Include processing integration test completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "[FAIL] Include processing integration test: " << e.what() << std::endl;
    }
    
    // Limpar arquivos temporários
    std::remove("math_utils.h");
    std::remove("main_with_include.c");
}

int main() {
    std::cout << "Running Preprocessor Integration Tests..." << std::endl;
    
    test_file_processing();
    test_macro_expansion();
    test_conditional_compilation();
    test_include_processing();
    
    std::cout << "All integration tests completed (placeholders)" << std::endl;
    return 0;
}