# Exemplos de Uso do Pré-processador C

## Visão Geral

Este documento fornece exemplos práticos de como usar o pré-processador C em diferentes cenários, desde uso básico até integrações avançadas.

## Índice

1. [Exemplo Básico](#exemplo-básico)
2. [Processamento de String](#processamento-de-string)
3. [Configuração Avançada](#configuração-avançada)
4. [Uso com Macros Complexas](#uso-com-macros-complexas)
5. [Integração com Sistema de Build](#integração-com-sistema-de-build)
6. [Tratamento de Erros](#tratamento-de-erros)
7. [Otimização de Performance](#otimização-de-performance)
8. [Debugging e Diagnóstico](#debugging-e-diagnóstico)

---

## Exemplo Básico

### Processamento Simples de Arquivo

```cpp
#include "preprocessor.hpp"
#include <iostream>
#include <vector>

int main() {
    // Criar instância do pré-processador
    Preprocessor::PreprocessorMain preprocessor;
    
    // Processar arquivo
    if (preprocessor.process("example.c")) {
        // Obter código expandido
        std::string expanded_code = preprocessor.getExpandedCode();
        std::cout << "=== Código Expandido ===\n";
        std::cout << expanded_code << std::endl;
        
        // Obter lista de dependências
        auto dependencies = preprocessor.getDependencies();
        std::cout << "\n=== Dependências ===\n";
        for (const auto& dep : dependencies) {
            std::cout << "- " << dep << std::endl;
        }
        
        // Obter estatísticas
        auto stats = preprocessor.getStatistics();
        std::cout << "\n=== Estatísticas ===\n";
        std::cout << "Linhas processadas: " << stats.lines_processed << std::endl;
        std::cout << "Macros expandidas: " << stats.macros_expanded << std::endl;
        std::cout << "Arquivos incluídos: " << stats.files_included << std::endl;
    } else {
        std::cerr << "Erro ao processar arquivo" << std::endl;
        return 1;
    }
    
    return 0;
}
```

### Arquivo de Exemplo (example.c)

```c
#define VERSION "1.0.0"
#define DEBUG 1

#include <stdio.h>
#include <stdlib.h>

#if DEBUG
#define LOG(msg) printf("[DEBUG] %s\n", msg)
#else
#define LOG(msg)
#endif

int main() {
    LOG("Programa iniciado");
    printf("Versão: %s\n", VERSION);
    return 0;
}
```

---

## Processamento de String

### Processamento Direto de Código

```cpp
#include "preprocessor.hpp"
#include <iostream>
#include <string>

int main() {
    Preprocessor::PreprocessorMain preprocessor;
    
    // Definir macros programaticamente
    preprocessor.defineMacro("MAX_SIZE", "1024");
    preprocessor.defineMacro("PLATFORM", "\"Linux\"");
    
    // Código C como string
    std::string source_code = R"(
#define BUFFER_SIZE (MAX_SIZE * 2)
#define GREETING "Hello from " PLATFORM

#ifdef DEBUG
    #define DBG_PRINT(x) printf("DEBUG: %s\n", x)
#else
    #define DBG_PRINT(x)
#endif

int buffer[BUFFER_SIZE];
const char* message = GREETING;

void init() {
    DBG_PRINT("Initializing buffer");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }
}
)";
    
    // Processar string
    if (preprocessor.processString(source_code)) {
        std::string result = preprocessor.getExpandedCode();
        std::cout << "=== Código Processado ===\n";
        std::cout << result << std::endl;
    } else {
        std::cerr << "Erro no processamento" << std::endl;
    }
    
    return 0;
}
```

---

## Configuração Avançada

### Configuração Completa com Caminhos e Versão

```cpp
#include "preprocessor.hpp"
#include <iostream>
#include <vector>

class AdvancedPreprocessorExample {
private:
    Preprocessor::PreprocessorMain preprocessor;
    
public:
    void setupConfiguration() {
        // Configurar caminhos de busca
        std::vector<std::string> include_paths = {
            "/usr/include",
            "/usr/local/include",
            "./include",
            "./third_party/include",
            "../common/include"
        };
        preprocessor.setSearchPaths(include_paths);
        
        // Configurar versão do padrão C
        preprocessor.setVersion(Preprocessor::CVersion::C11);
        
        // Definir macros do sistema
        setupSystemMacros();
        
        // Definir macros do projeto
        setupProjectMacros();
    }
    
    void setupSystemMacros() {
        // Macros de sistema operacional
        #ifdef __linux__
            preprocessor.defineMacro("__LINUX__", "1");
            preprocessor.defineMacro("_GNU_SOURCE", "1");
        #elif defined(__APPLE__)
            preprocessor.defineMacro("__MACOS__", "1");
            preprocessor.defineMacro("_DARWIN_C_SOURCE", "1");
        #elif defined(_WIN32)
            preprocessor.defineMacro("__WINDOWS__", "1");
            preprocessor.defineMacro("_WIN32_WINNT", "0x0601");
        #endif
        
        // Macros de arquitetura
        #ifdef __x86_64__
            preprocessor.defineMacro("__ARCH_X64__", "1");
        #elif defined(__i386__)
            preprocessor.defineMacro("__ARCH_X86__", "1");
        #elif defined(__arm__)
            preprocessor.defineMacro("__ARCH_ARM__", "1");
        #endif
    }
    
    void setupProjectMacros() {
        // Macros do projeto
        preprocessor.defineMacro("PROJECT_NAME", "\"MyProject\"");
        preprocessor.defineMacro("PROJECT_VERSION_MAJOR", "2");
        preprocessor.defineMacro("PROJECT_VERSION_MINOR", "1");
        preprocessor.defineMacro("PROJECT_VERSION_PATCH", "0");
        
        // Macros de configuração
        #ifdef DEBUG
            preprocessor.defineMacro("DEBUG_BUILD", "1");
            preprocessor.defineMacro("LOG_LEVEL", "3");
        #else
            preprocessor.defineMacro("RELEASE_BUILD", "1");
            preprocessor.defineMacro("LOG_LEVEL", "1");
        #endif
        
        // Macros de features
        preprocessor.defineMacro("ENABLE_NETWORKING", "1");
        preprocessor.defineMacro("ENABLE_GRAPHICS", "1");
        preprocessor.defineMacro("ENABLE_AUDIO", "0");
    }
    
    bool processFile(const std::string& filename) {
        std::cout << "Processando arquivo: " << filename << std::endl;
        
        if (!preprocessor.process(filename)) {
            std::cerr << "Erro ao processar " << filename << std::endl;
            return false;
        }
        
        // Exibir informações detalhadas
        displayProcessingInfo();
        
        return true;
    }
    
    void displayProcessingInfo() {
        auto stats = preprocessor.getStatistics();
        auto deps = preprocessor.getDependencies();
        
        std::cout << "\n=== Informações de Processamento ===\n";
        std::cout << "Linhas processadas: " << stats.lines_processed << std::endl;
        std::cout << "Macros definidas: " << stats.macros_defined << std::endl;
        std::cout << "Macros expandidas: " << stats.macros_expanded << std::endl;
        std::cout << "Condicionais processadas: " << stats.conditionals_processed << std::endl;
        std::cout << "Arquivos incluídos: " << stats.files_included << std::endl;
        
        std::cout << "\n=== Dependências ===\n";
        for (size_t i = 0; i < deps.size(); ++i) {
            std::cout << i + 1 << ". " << deps[i] << std::endl;
        }
    }
    
    void reset() {
        preprocessor.reset();
    }
};

int main() {
    AdvancedPreprocessorExample example;
    
    // Configurar pré-processador
    example.setupConfiguration();
    
    // Processar múltiplos arquivos
    std::vector<std::string> files = {
        "main.c",
        "utils.c",
        "network.c"
    };
    
    for (const auto& file : files) {
        if (!example.processFile(file)) {
            std::cerr << "Falha no processamento de " << file << std::endl;
            continue;
        }
        
        std::cout << "\n" << std::string(50, '-') << "\n";
        example.reset(); // Reset para próximo arquivo
    }
    
    return 0;
}
```

---

## Uso com Macros Complexas

### Definição e Expansão de Macros Funcionais

```cpp
#include "macro_processor.hpp"
#include "preprocessor.hpp"
#include <iostream>

class MacroExample {
private:
    Preprocessor::PreprocessorMain preprocessor;
    
public:
    void setupComplexMacros() {
        // Macros funcionais básicas
        preprocessor.defineMacro("MAX(a,b)", "((a) > (b) ? (a) : (b))");
        preprocessor.defineMacro("MIN(a,b)", "((a) < (b) ? (a) : (b))");
        preprocessor.defineMacro("ABS(x)", "((x) < 0 ? -(x) : (x))");
        
        // Macros de logging com níveis
        preprocessor.defineMacro("LOG_ERROR(msg)", "fprintf(stderr, \"[ERROR] %s:%d: %s\\n\", __FILE__, __LINE__, msg)");
        preprocessor.defineMacro("LOG_WARN(msg)", "fprintf(stderr, \"[WARN] %s:%d: %s\\n\", __FILE__, __LINE__, msg)");
        preprocessor.defineMacro("LOG_INFO(msg)", "printf(\"[INFO] %s\\n\", msg)");
        
        // Macros de estruturas de dados
        preprocessor.defineMacro("DECLARE_VECTOR(type)", 
            "typedef struct { \\\n"
            "    type* data; \\\n"
            "    size_t size; \\\n"
            "    size_t capacity; \\\n"
            "} vector_##type;");
        
        // Macros de iteração
        preprocessor.defineMacro("FOR_EACH(var, array, size)", 
            "for (size_t _i = 0; _i < (size) && ((var) = (array)[_i], 1); ++_i)");
        
        // Macros condicionais complexas
        preprocessor.defineMacro("ASSERT(condition)", 
            "do { \\\n"
            "    if (!(condition)) { \\\n"
            "        fprintf(stderr, \"Assertion failed: %s at %s:%d\\n\", \\\n"
            "                #condition, __FILE__, __LINE__); \\\n"
            "        abort(); \\\n"
            "    } \\\n"
            "} while(0)");
    }
    
    void demonstrateMacroExpansion() {
        std::string test_code = R"(
#include <stdio.h>
#include <stdlib.h>

DECLARE_VECTOR(int)
DECLARE_VECTOR(float)

int main() {
    int a = 10, b = 20;
    int max_val = MAX(a, b);
    int min_val = MIN(a, b);
    
    LOG_INFO("Programa iniciado");
    printf("Máximo: %d, Mínimo: %d\n", max_val, min_val);
    
    int numbers[] = {1, 2, 3, 4, 5};
    int num;
    
    printf("Números: ");
    FOR_EACH(num, numbers, 5) {
        printf("%d ", num);
    }
    printf("\n");
    
    ASSERT(max_val > min_val);
    
    return 0;
}
)";
        
        if (preprocessor.processString(test_code)) {
            std::cout << "=== Código com Macros Expandidas ===\n";
            std::cout << preprocessor.getExpandedCode() << std::endl;
        }
    }
};

int main() {
    MacroExample example;
    example.setupComplexMacros();
    example.demonstrateMacroExpansion();
    return 0;
}
```

---

## Integração com Sistema de Build

### Integração com CMake

```cpp
// build_integration.cpp
#include "preprocessor.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

class BuildSystemIntegration {
private:
    Preprocessor::PreprocessorMain preprocessor;
    std::string build_dir;
    std::string source_dir;
    
public:
    BuildSystemIntegration(const std::string& src_dir, const std::string& build_dir)
        : source_dir(src_dir), build_dir(build_dir) {
        setupBuildConfiguration();
    }
    
    void setupBuildConfiguration() {
        // Configurar caminhos baseados no sistema de build
        std::vector<std::string> include_paths = {
            source_dir + "/include",
            source_dir + "/src",
            build_dir + "/generated",
            "/usr/include",
            "/usr/local/include"
        };
        preprocessor.setSearchPaths(include_paths);
        
        // Macros de configuração do build
        loadBuildConfiguration();
    }
    
    void loadBuildConfiguration() {
        // Simular leitura de configuração do CMake
        std::ifstream config_file(build_dir + "/config.h");
        if (config_file.is_open()) {
            std::string line;
            while (std::getline(config_file, line)) {
                if (line.find("#define") == 0) {
                    // Processar definições do arquivo de configuração
                    size_t space1 = line.find(' ', 8);
                    size_t space2 = line.find(' ', space1 + 1);
                    
                    if (space1 != std::string::npos) {
                        std::string macro_name = line.substr(8, space1 - 8);
                        std::string macro_value = (space2 != std::string::npos) 
                            ? line.substr(space1 + 1, space2 - space1 - 1)
                            : line.substr(space1 + 1);
                        
                        preprocessor.defineMacro(macro_name, macro_value);
                    }
                }
            }
        }
        
        // Macros padrão do sistema de build
        preprocessor.defineMacro("CMAKE_BUILD", "1");
        preprocessor.defineMacro("BUILD_TIMESTAMP", std::to_string(std::time(nullptr)));
    }
    
    bool processSourceTree() {
        std::vector<std::string> processed_files;
        std::vector<std::string> failed_files;
        
        // Processar todos os arquivos .c e .h
        for (const auto& entry : std::filesystem::recursive_directory_iterator(source_dir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".c" || ext == ".h" || ext == ".cpp" || ext == ".hpp") {
                    std::string filepath = entry.path().string();
                    
                    std::cout << "Processando: " << filepath << std::endl;
                    
                    if (preprocessor.process(filepath)) {
                        processed_files.push_back(filepath);
                        
                        // Salvar resultado processado
                        saveProcessedFile(filepath);
                        
                        // Gerar arquivo de dependências
                        generateDependencyFile(filepath);
                    } else {
                        failed_files.push_back(filepath);
                        std::cerr << "Falha ao processar: " << filepath << std::endl;
                    }
                    
                    preprocessor.reset();
                }
            }
        }
        
        // Relatório final
        generateBuildReport(processed_files, failed_files);
        
        return failed_files.empty();
    }
    
    void saveProcessedFile(const std::string& original_path) {
        std::string relative_path = std::filesystem::relative(original_path, source_dir).string();
        std::string output_path = build_dir + "/preprocessed/" + relative_path + ".i";
        
        // Criar diretórios se necessário
        std::filesystem::create_directories(std::filesystem::path(output_path).parent_path());
        
        // Salvar código expandido
        std::ofstream output_file(output_path);
        if (output_file.is_open()) {
            output_file << preprocessor.getExpandedCode();
            output_file.close();
        }
    }
    
    void generateDependencyFile(const std::string& original_path) {
        std::string relative_path = std::filesystem::relative(original_path, source_dir).string();
        std::string dep_path = build_dir + "/deps/" + relative_path + ".d";
        
        // Criar diretórios se necessário
        std::filesystem::create_directories(std::filesystem::path(dep_path).parent_path());
        
        // Gerar arquivo de dependências no formato Makefile
        std::ofstream dep_file(dep_path);
        if (dep_file.is_open()) {
            std::string obj_file = relative_path;
            size_t dot_pos = obj_file.find_last_of('.');
            if (dot_pos != std::string::npos) {
                obj_file = obj_file.substr(0, dot_pos) + ".o";
            }
            
            dep_file << obj_file << ": " << original_path;
            
            auto dependencies = preprocessor.getDependencies();
            for (const auto& dep : dependencies) {
                dep_file << " \\\n  " << dep;
            }
            dep_file << std::endl;
            
            dep_file.close();
        }
    }
    
    void generateBuildReport(const std::vector<std::string>& processed,
                           const std::vector<std::string>& failed) {
        std::string report_path = build_dir + "/preprocessing_report.txt";
        std::ofstream report(report_path);
        
        if (report.is_open()) {
            report << "=== Relatório de Pré-processamento ===\n\n";
            report << "Data: " << std::ctime(&(std::time_t){std::time(nullptr)});
            report << "Diretório fonte: " << source_dir << "\n";
            report << "Diretório build: " << build_dir << "\n\n";
            
            report << "Arquivos processados com sucesso: " << processed.size() << "\n";
            for (const auto& file : processed) {
                report << "  ✓ " << file << "\n";
            }
            
            if (!failed.empty()) {
                report << "\nArquivos com falha: " << failed.size() << "\n";
                for (const auto& file : failed) {
                    report << "  ✗ " << file << "\n";
                }
            }
            
            report.close();
            std::cout << "Relatório salvo em: " << report_path << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <source_dir> <build_dir>" << std::endl;
        return 1;
    }
    
    std::string source_dir = argv[1];
    std::string build_dir = argv[2];
    
    BuildSystemIntegration integration(source_dir, build_dir);
    
    if (integration.processSourceTree()) {
        std::cout << "Pré-processamento concluído com sucesso!" << std::endl;
        return 0;
    } else {
        std::cerr << "Pré-processamento falhou!" << std::endl;
        return 1;
    }
}
```

---

## Tratamento de Erros

### Exemplo Robusto com Tratamento de Erros

```cpp
#include "preprocessor.hpp"
#include <iostream>
#include <exception>

class ErrorHandlingExample {
private:
    Preprocessor::PreprocessorMain preprocessor;
    
public:
    enum class ProcessingResult {
        SUCCESS,
        FILE_NOT_FOUND,
        SYNTAX_ERROR,
        CIRCULAR_INCLUSION,
        MACRO_ERROR,
        UNKNOWN_ERROR
    };
    
    ProcessingResult processFileWithErrorHandling(const std::string& filename) {
        try {
            std::cout << "Iniciando processamento de: " << filename << std::endl;
            
            // Verificar se arquivo existe antes de processar
            if (!std::filesystem::exists(filename)) {
                std::cerr << "Erro: Arquivo não encontrado: " << filename << std::endl;
                return ProcessingResult::FILE_NOT_FOUND;
            }
            
            // Tentar processar o arquivo
            if (!preprocessor.process(filename)) {
                // Analisar tipo de erro baseado no estado do pré-processador
                return analyzeProcessingError();
            }
            
            // Validar resultado
            if (!validateProcessingResult()) {
                std::cerr << "Erro: Resultado de processamento inválido" << std::endl;
                return ProcessingResult::UNKNOWN_ERROR;
            }
            
            std::cout << "Processamento concluído com sucesso" << std::endl;
            return ProcessingResult::SUCCESS;
            
        } catch (const std::runtime_error& e) {
            std::cerr << "Erro de runtime: " << e.what() << std::endl;
            return ProcessingResult::UNKNOWN_ERROR;
        } catch (const std::exception& e) {
            std::cerr << "Exceção: " << e.what() << std::endl;
            return ProcessingResult::UNKNOWN_ERROR;
        } catch (...) {
            std::cerr << "Erro desconhecido durante processamento" << std::endl;
            return ProcessingResult::UNKNOWN_ERROR;
        }
    }
    
    ProcessingResult analyzeProcessingError() {
        auto stats = preprocessor.getStatistics();
        
        // Verificar diferentes tipos de erro
        if (stats.circular_inclusions > 0) {
            std::cerr << "Erro: Inclusão circular detectada" << std::endl;
            return ProcessingResult::CIRCULAR_INCLUSION;
        }
        
        if (stats.macro_errors > 0) {
            std::cerr << "Erro: Problemas com macros" << std::endl;
            return ProcessingResult::MACRO_ERROR;
        }
        
        if (stats.syntax_errors > 0) {
            std::cerr << "Erro: Problemas de sintaxe" << std::endl;
            return ProcessingResult::SYNTAX_ERROR;
        }
        
        return ProcessingResult::UNKNOWN_ERROR;
    }
    
    bool validateProcessingResult() {
        std::string expanded = preprocessor.getExpandedCode();
        
        // Verificações básicas de sanidade
        if (expanded.empty()) {
            std::cerr << "Aviso: Código expandido está vazio" << std::endl;
            return false;
        }
        
        // Verificar balanceamento de chaves (exemplo básico)
        int brace_count = 0;
        for (char c : expanded) {
            if (c == '{') brace_count++;
            else if (c == '}') brace_count--;
        }
        
        if (brace_count != 0) {
            std::cerr << "Aviso: Chaves desbalanceadas no código expandido" << std::endl;
            // Não é necessariamente um erro fatal
        }
        
        return true;
    }
    
    void handleProcessingResult(ProcessingResult result, const std::string& filename) {
        switch (result) {
            case ProcessingResult::SUCCESS:
                std::cout << "✓ Sucesso: " << filename << std::endl;
                displaySuccessInfo();
                break;
                
            case ProcessingResult::FILE_NOT_FOUND:
                std::cerr << "✗ Arquivo não encontrado: " << filename << std::endl;
                suggestFileResolution(filename);
                break;
                
            case ProcessingResult::SYNTAX_ERROR:
                std::cerr << "✗ Erro de sintaxe em: " << filename << std::endl;
                displaySyntaxErrorHelp();
                break;
                
            case ProcessingResult::CIRCULAR_INCLUSION:
                std::cerr << "✗ Inclusão circular em: " << filename << std::endl;
                displayCircularInclusionHelp();
                break;
                
            case ProcessingResult::MACRO_ERROR:
                std::cerr << "✗ Erro de macro em: " << filename << std::endl;
                displayMacroErrorHelp();
                break;
                
            case ProcessingResult::UNKNOWN_ERROR:
                std::cerr << "✗ Erro desconhecido em: " << filename << std::endl;
                displayGeneralErrorHelp();
                break;
        }
    }
    
    void displaySuccessInfo() {
        auto stats = preprocessor.getStatistics();
        auto deps = preprocessor.getDependencies();
        
        std::cout << "  Linhas processadas: " << stats.lines_processed << std::endl;
        std::cout << "  Macros expandidas: " << stats.macros_expanded << std::endl;
        std::cout << "  Arquivos incluídos: " << deps.size() << std::endl;
    }
    
    void suggestFileResolution(const std::string& filename) {
        std::cout << "Sugestões:\n";
        std::cout << "  - Verifique se o caminho está correto\n";
        std::cout << "  - Verifique se o arquivo existe\n";
        std::cout << "  - Adicione o diretório aos caminhos de busca\n";
    }
    
    void displaySyntaxErrorHelp() {
        std::cout << "Sugestões para erros de sintaxe:\n";
        std::cout << "  - Verifique balanceamento de parênteses e chaves\n";
        std::cout << "  - Verifique sintaxe das diretivas (#define, #include, etc.)\n";
        std::cout << "  - Verifique se todas as strings estão fechadas\n";
    }
    
    void displayCircularInclusionHelp() {
        std::cout << "Sugestões para inclusão circular:\n";
        std::cout << "  - Use include guards (#ifndef, #define, #endif)\n";
        std::cout << "  - Use #pragma once se suportado\n";
        std::cout << "  - Reorganize a estrutura de headers\n";
        
        auto deps = preprocessor.getDependencies();
        if (!deps.empty()) {
            std::cout << "  Cadeia de inclusões:\n";
            for (size_t i = 0; i < deps.size(); ++i) {
                std::cout << "    " << i + 1 << ". " << deps[i] << std::endl;
            }
        }
    }
    
    void displayMacroErrorHelp() {
        std::cout << "Sugestões para erros de macro:\n";
        std::cout << "  - Verifique sintaxe das definições de macro\n";
        std::cout << "  - Verifique número de parâmetros em macros funcionais\n";
        std::cout << "  - Evite redefinições conflitantes\n";
        std::cout << "  - Use parênteses para proteger expansões\n";
    }
    
    void displayGeneralErrorHelp() {
        std::cout << "Sugestões gerais:\n";
        std::cout << "  - Verifique logs detalhados se disponíveis\n";
        std::cout << "  - Tente processar arquivos menores primeiro\n";
        std::cout << "  - Verifique configuração do pré-processador\n";
        std::cout << "  - Reporte o bug se o problema persistir\n";
    }
};

int main() {
    ErrorHandlingExample example;
    
    std::vector<std::string> test_files = {
        "valid_file.c",
        "nonexistent_file.c",
        "syntax_error_file.c",
        "circular_include.c"
    };
    
    for (const auto& file : test_files) {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        auto result = example.processFileWithErrorHandling(file);
        example.handleProcessingResult(result, file);
    }
    
    return 0;
}
```

---

## Otimização de Performance

### Configuração para Máxima Performance

```cpp
#include "preprocessor.hpp"
#include "macro_processor.hpp"
#include "file_manager.hpp"
#include <chrono>
#include <iostream>

class PerformanceOptimizedPreprocessor {
private:
    Preprocessor::PreprocessorMain preprocessor;
    std::chrono::high_resolution_clock::time_point start_time;
    
public:
    void setupOptimizations() {
        std::cout << "Configurando otimizações de performance..." << std::endl;
        
        // Configurar caminhos de busca otimizados
        setupOptimizedSearchPaths();
        
        // Configurar cache de arquivos
        setupFileCache();
        
        // Configurar cache de macros
        setupMacroCache();
        
        // Pré-carregar recursos frequentes
        preloadFrequentResources();
    }
    
    void setupOptimizedSearchPaths() {
        // Ordenar caminhos por frequência de uso (mais usados primeiro)
        std::vector<std::string> optimized_paths = {
            "./include",           // Projeto local (mais frequente)
            "./src",               // Código fonte local
            "/usr/local/include",  // Bibliotecas instaladas localmente
            "/usr/include",        // Sistema (menos frequente)
        };
        
        preprocessor.setSearchPaths(optimized_paths);
    }
    
    void setupFileCache() {
        // Configurar cache de arquivos para máxima performance
        // Cache de 100MB, 5000 entradas, TTL de 10 minutos, com compressão
        
        std::cout << "Configurando cache de arquivos (100MB, 5000 entradas)" << std::endl;
        
        // Nota: Esta configuração seria feita através da API do FileManager
        // quando integrado com o PreprocessorMain
    }
    
    void setupMacroCache() {
        // Configurar cache de expansões de macro
        std::cout << "Configurando cache de macros" << std::endl;
        
        // Nota: Esta configuração seria feita através da API do MacroProcessor
        // quando integrado com o PreprocessorMain
    }
    
    void preloadFrequentResources() {
        std::cout << "Pré-carregando recursos frequentes..." << std::endl;
        
        // Headers do sistema mais comuns
        std::vector<std::string> common_headers = {
            "stdio.h",
            "stdlib.h",
            "string.h",
            "math.h",
            "stdint.h",
            "stdbool.h"
        };
        
        // Macros predefinidas comuns
        std::vector<std::string> common_macros = {
            "NULL",
            "EOF",
            "TRUE",
            "FALSE",
            "__FILE__",
            "__LINE__"
        };
        
        std::cout << "Pré-carregamento concluído" << std::endl;
    }
    
    void startTiming() {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    void endTiming(const std::string& operation) {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        std::cout << "Tempo de " << operation << ": " << duration.count() << " μs" << std::endl;
    }
    
    void benchmarkProcessing(const std::vector<std::string>& files) {
        std::cout << "\n=== Benchmark de Performance ===\n";
        
        for (const auto& file : files) {
            std::cout << "\nProcessando: " << file << std::endl;
            
            // Medir tempo de processamento
            startTiming();
            bool success = preprocessor.process(file);
            endTiming("processamento");
            
            if (success) {
                // Medir tempo de obtenção de resultados
                startTiming();
                auto expanded = preprocessor.getExpandedCode();
                endTiming("obtenção do código expandido");
                
                startTiming();
                auto deps = preprocessor.getDependencies();
                endTiming("obtenção de dependências");
                
                startTiming();
                auto stats = preprocessor.getStatistics();
                endTiming("obtenção de estatísticas");
                
                // Exibir métricas de performance
                displayPerformanceMetrics(file, expanded.size(), deps.size(), stats);
            } else {
                std::cerr << "Falha no processamento de " << file << std::endl;
            }
            
            preprocessor.reset();
        }
    }
    
    void displayPerformanceMetrics(const std::string& filename,
                                 size_t code_size,
                                 size_t deps_count,
                                 const auto& stats) {
        std::cout << "\n--- Métricas de Performance ---\n";
        std::cout << "Arquivo: " << filename << std::endl;
        std::cout << "Tamanho do código expandido: " << code_size << " bytes" << std::endl;
        std::cout << "Número de dependências: " << deps_count << std::endl;
        std::cout << "Linhas processadas: " << stats.lines_processed << std::endl;
        std::cout << "Macros expandidas: " << stats.macros_expanded << std::endl;
        
        if (stats.lines_processed > 0) {
            std::cout << "Bytes por linha: " << (code_size / stats.lines_processed) << std::endl;
        }
        
        if (stats.macros_expanded > 0) {
            std::cout << "Linhas por macro: " << (stats.lines_processed / stats.macros_expanded) << std::endl;
        }
    }
    
    void runPerformanceTest() {
        std::cout << "=== Teste de Performance do Pré-processador ===\n";
        
        // Configurar otimizações
        setupOptimizations();
        
        // Arquivos de teste (do menor ao maior)
        std::vector<std::string> test_files = {
            "small_test.c",      // ~100 linhas
            "medium_test.c",     // ~1000 linhas
            "large_test.c",      // ~10000 linhas
            "huge_test.c"        // ~100000 linhas
        };
        
        // Executar benchmark
        benchmarkProcessing(test_files);
        
        // Teste de stress com múltiplas iterações
        stressTest("medium_test.c", 100);
    }
    
    void stressTest(const std::string& filename, int iterations) {
        std::cout << "\n=== Teste de Stress ===\n";
        std::cout << "Arquivo: " << filename << std::endl;
        std::cout << "Iterações: " << iterations << std::endl;
        
        std::vector<long long> times;
        times.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
            startTiming();
            bool success = preprocessor.process(filename);
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            
            if (success) {
                times.push_back(duration.count());
            }
            
            preprocessor.reset();
            
            if ((i + 1) % 10 == 0) {
                std::cout << "Progresso: " << (i + 1) << "/" << iterations << std::endl;
            }
        }
        
        // Calcular estatísticas
        if (!times.empty()) {
            long long total = 0;
            long long min_time = times[0];
            long long max_time = times[0];
            
            for (long long time : times) {
                total += time;
                min_time = std::min(min_time, time);
                max_time = std::max(max_time, time);
            }
            
            long long avg_time = total / times.size();
            
            std::cout << "\n--- Resultados do Teste de Stress ---\n";
            std::cout << "Iterações bem-sucedidas: " << times.size() << "/" << iterations << std::endl;
            std::cout << "Tempo médio: " << avg_time << " μs" << std::endl;
            std::cout << "Tempo mínimo: " << min_time << " μs" << std::endl;
            std::cout << "Tempo máximo: " << max_time << " μs" << std::endl;
            std::cout << "Desvio (max-min): " << (max_time - min_time) << " μs" << std::endl;
        }
    }
};

int main() {
    PerformanceOptimizedPreprocessor perf_test;
    perf_test.runPerformanceTest();
    return 0;
}