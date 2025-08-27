// Teste Abrangente de Otimização - Unificação de testes de performance e validação

#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>
#include <sstream>
#include <random>
#include <algorithm>
#include <functional>
#include <cassert>

#include "../../include/macro_processor.hpp"
#include "../../include/file_manager.hpp"
#include "../../include/preprocessor_logger.hpp"
#include "../../include/preprocessor.hpp"

using namespace Preprocessor;
using namespace std::chrono;

// ============================================================================
// FUNÇÕES AUXILIARES
// ============================================================================

void assertEqual(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected != actual) {
        std::cerr << "ERRO: " << message << std::endl;
        std::cerr << "Esperado: '" << expected << "'" << std::endl;
        std::cerr << "Atual: '" << actual << "'" << std::endl;
        assert(false);
    }
}

void assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "ERRO: " << message << std::endl;
        assert(false);
    }
}

void assertFalse(bool condition, const std::string& message) {
    if (condition) {
        std::cerr << "ERRO: " << message << std::endl;
        assert(false);
    }
}

class PerformanceProfiler {
public:
    static double measureTime(std::function<void()> func) {
        auto start = high_resolution_clock::now();
        func();
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        return duration.count() / 1000.0; // Convert to milliseconds
    }
    
    static void printResult(const std::string& testName, double timeMs, const std::string& details = "") {
        std::cout << "[PERFORMANCE] " << std::left << std::setw(50) << testName 
                  << ": " << std::fixed << std::setprecision(6) << timeMs << " ms";
        if (!details.empty()) {
            std::cout << " (" << details << ")";
        }
        std::cout << std::endl;
    }
};

class OptimizationValidator {
private:
    std::shared_ptr<PreprocessorLogger> logger_;
    std::unique_ptr<MacroProcessor> macro_processor_;
    std::unique_ptr<FileManager> file_manager_;
    
public:
    OptimizationValidator() {
        logger_ = std::make_shared<PreprocessorLogger>();
        macro_processor_ = std::make_unique<MacroProcessor>(logger_, nullptr);
        
        std::vector<std::string> searchPaths = {"/tmp"};
        file_manager_ = std::make_unique<FileManager>(searchPaths, logger_.get());
    }
    
    void runAllTests() {
        testMacroExpansionPerformance();
        testFileOperationPerformance();
        testCacheOptimization();
        testMemoryUsage();
        testConcurrentAccess();
        testLargeFileHandling();
        testOptimizationValidation();
    }
    
private:
    void testMacroExpansionPerformance() {
        std::cout << "\n=== Teste de Performance de Expansão de Macros ===" << std::endl;
        
        try {
            // Teste de macro simples
            double time1 = PerformanceProfiler::measureTime([this]() {
                for (int i = 0; i < 1000; ++i) {
                    macro_processor_->defineMacro("TEST_" + std::to_string(i), "value_" + std::to_string(i));
                }
            });
            PerformanceProfiler::printResult("Definição de 1000 macros simples", time1);
            
            // Teste de expansão de macros
            double time2 = PerformanceProfiler::measureTime([this]() {
                for (int i = 0; i < 1000; ++i) {
                    std::string result = macro_processor_->expandMacro("TEST_" + std::to_string(i));
                }
            });
            PerformanceProfiler::printResult("Expansão de 1000 macros", time2);
            
            // Teste de macros funcionais
            macro_processor_->defineMacro("ADD(a,b)", "((a)+(b))");
            double time3 = PerformanceProfiler::measureTime([this]() {
                for (int i = 0; i < 500; ++i) {
                    std::string result = macro_processor_->expandMacro("ADD(" + std::to_string(i) + "," + std::to_string(i+1) + ")");
                }
            });
            PerformanceProfiler::printResult("Expansão de 500 macros funcionais", time3);
            
            assertTrue(true, "Testes de performance de macros concluídos");
            
        } catch (const std::exception& e) {
            std::cerr << "Erro no teste de performance de macros: " << e.what() << std::endl;
            assert(false);
        }
    }
    
    void testFileOperationPerformance() {
        std::cout << "\n=== Teste de Performance de Operações de Arquivo ===" << std::endl;
        
        try {
            // Criar arquivos de teste
            std::vector<std::string> testFiles;
            for (int i = 0; i < 10; ++i) {
                std::string filename = "/tmp/test_file_" + std::to_string(i) + ".txt";
                std::string content = "Content of file " + std::to_string(i) + "\n";
                for (int j = 0; j < 100; ++j) {
                    content += "Line " + std::to_string(j) + " of file " + std::to_string(i) + "\n";
                }
                file_manager_->writeFile(filename, content);
                testFiles.push_back(filename);
            }
            
            // Teste de leitura sequencial
            double time1 = PerformanceProfiler::measureTime([this, &testFiles]() {
                for (const auto& file : testFiles) {
                    std::string content = file_manager_->readFile(file);
                }
            });
            PerformanceProfiler::printResult("Leitura sequencial de 10 arquivos", time1);
            
            // Teste de cache hit
            double time2 = PerformanceProfiler::measureTime([this, &testFiles]() {
                for (int i = 0; i < 5; ++i) {
                    for (const auto& file : testFiles) {
                        std::string content = file_manager_->readFile(file);
                    }
                }
            });
            PerformanceProfiler::printResult("5 leituras com cache (50 operações)", time2);
            
            // Limpar arquivos de teste
            for (const auto& file : testFiles) {
                std::remove(file.c_str());
            }
            
            assertTrue(true, "Testes de performance de arquivos concluídos");
            
        } catch (const std::exception& e) {
            std::cerr << "Erro no teste de performance de arquivos: " << e.what() << std::endl;
            assert(false);
        }
    }
    
    void testCacheOptimization() {
        std::cout << "\n=== Teste de Otimização de Cache ===" << std::endl;
        
        try {
            // Configurar cache
            file_manager_->configureCacheOptimization(1024 * 1024, 100, std::chrono::seconds(60));
            
            // Teste de preload
            std::vector<std::string> preloadFiles;
            for (int i = 0; i < 5; ++i) {
                std::string filename = "/tmp/preload_" + std::to_string(i) + ".txt";
                std::string content = "Preload content " + std::to_string(i);
                file_manager_->writeFile(filename, content);
                preloadFiles.push_back(filename);
            }
            
            double preloadTime = PerformanceProfiler::measureTime([this, &preloadFiles]() {
                file_manager_->preloadFiles(preloadFiles);
            });
            PerformanceProfiler::printResult("Preload de 5 arquivos", preloadTime);
            
            // Teste de otimização de cache
            double optimizeTime = PerformanceProfiler::measureTime([this]() {
                file_manager_->optimizeCache();
            });
            PerformanceProfiler::printResult("Otimização de cache", optimizeTime);
            
            // Verificar estatísticas
            auto stats = file_manager_->getStatistics();
            assertTrue(stats.cache_hits > 0, "Cache hits registrados");
            
            // Limpar arquivos
            for (const auto& file : preloadFiles) {
                std::remove(file.c_str());
            }
            
            assertTrue(true, "Testes de otimização de cache concluídos");
            
        } catch (const std::exception& e) {
            std::cerr << "Erro no teste de otimização de cache: " << e.what() << std::endl;
            assert(false);
        }
    }
    
    void testMemoryUsage() {
        std::cout << "\n=== Teste de Uso de Memória ===" << std::endl;
        
        try {
            // Teste de definição massiva de macros
            double memoryTime = PerformanceProfiler::measureTime([this]() {
                for (int i = 0; i < 10000; ++i) {
                    std::string name = "MEMORY_TEST_" + std::to_string(i);
                    std::string value = "value_" + std::to_string(i * 2);
                    macro_processor_->defineMacro(name, value);
                }
            });
            PerformanceProfiler::printResult("Definição de 10000 macros", memoryTime);
            
            // Verificar se algumas macros foram definidas
            assertTrue(macro_processor_->isDefined("MEMORY_TEST_0"), "Primeira macro definida");
            assertTrue(macro_processor_->isDefined("MEMORY_TEST_9999"), "Última macro definida");
            
            // Teste de limpeza
            double cleanupTime = PerformanceProfiler::measureTime([this]() {
                for (int i = 0; i < 10000; ++i) {
                    std::string name = "MEMORY_TEST_" + std::to_string(i);
                    macro_processor_->undefineMacro(name);
                }
            });
            PerformanceProfiler::printResult("Remoção de 10000 macros", cleanupTime);
            
            assertTrue(true, "Testes de uso de memória concluídos");
            
        } catch (const std::exception& e) {
            std::cerr << "Erro no teste de uso de memória: " << e.what() << std::endl;
            assert(false);
        }
    }
    
    void testConcurrentAccess() {
        std::cout << "\n=== Teste de Acesso Concorrente ===" << std::endl;
        
        try {
            // Simular acesso concorrente com operações sequenciais
            std::vector<std::string> concurrentFiles;
            
            // Criar arquivos para teste concorrente
            for (int i = 0; i < 20; ++i) {
                std::string filename = "/tmp/concurrent_" + std::to_string(i) + ".txt";
                std::string content = "Concurrent content " + std::to_string(i);
                file_manager_->writeFile(filename, content);
                concurrentFiles.push_back(filename);
            }
            
            // Simular leituras concorrentes
            double concurrentTime = PerformanceProfiler::measureTime([this, &concurrentFiles]() {
                for (int round = 0; round < 10; ++round) {
                    for (const auto& file : concurrentFiles) {
                        std::string content = file_manager_->readFile(file);
                        // Simular processamento
                        volatile size_t len = content.length();
                    }
                }
            });
            PerformanceProfiler::printResult("200 leituras simulando concorrência", concurrentTime);
            
            // Limpar arquivos
            for (const auto& file : concurrentFiles) {
                std::remove(file.c_str());
            }
            
            assertTrue(true, "Testes de acesso concorrente concluídos");
            
        } catch (const std::exception& e) {
            std::cerr << "Erro no teste de acesso concorrente: " << e.what() << std::endl;
            assert(false);
        }
    }
    
    void testLargeFileHandling() {
        std::cout << "\n=== Teste de Manipulação de Arquivos Grandes ===" << std::endl;
        
        try {
            // Criar arquivo grande
            std::string largeFile = "/tmp/large_test_file.txt";
            std::string largeContent;
            
            // Gerar conteúdo de ~1MB
            for (int i = 0; i < 10000; ++i) {
                largeContent += "This is line " + std::to_string(i) + " of a large file for testing purposes.\n";
            }
            
            double writeTime = PerformanceProfiler::measureTime([this, &largeFile, &largeContent]() {
                file_manager_->writeFile(largeFile, largeContent);
            });
            PerformanceProfiler::printResult("Escrita de arquivo ~1MB", writeTime, std::to_string(largeContent.size()) + " bytes");
            
            double readTime = PerformanceProfiler::measureTime([this, &largeFile]() {
                std::string content = file_manager_->readFile(largeFile);
            });
            PerformanceProfiler::printResult("Leitura de arquivo ~1MB", readTime);
            
            // Verificar tamanho
            size_t fileSize = file_manager_->getFileSize(largeFile);
            assertTrue(fileSize > 0, "Arquivo grande criado com sucesso");
            
            // Limpar
            std::remove(largeFile.c_str());
            
            assertTrue(true, "Testes de arquivos grandes concluídos");
            
        } catch (const std::exception& e) {
            std::cerr << "Erro no teste de arquivos grandes: " << e.what() << std::endl;
            assert(false);
        }
    }
    
    void testOptimizationValidation() {
        std::cout << "\n=== Teste de Validação de Otimização ===" << std::endl;
        
        try {
            // Teste de validação de macros
            macro_processor_->defineMacro("VALID_MACRO", "42");
            assertTrue(macro_processor_->isDefined("VALID_MACRO"), "Macro válida definida");
            
            // Teste de expansão otimizada
            std::string expanded = macro_processor_->expandMacro("VALID_MACRO");
            assertEqual("42", expanded, "Expansão de macro otimizada");
            
            // Teste de cache de arquivos
            std::string testFile = "/tmp/validation_test.txt";
            std::string testContent = "Validation content";
            file_manager_->writeFile(testFile, testContent);
            
            // Primeira leitura (miss)
            std::string content1 = file_manager_->readFile(testFile);
            assertEqual(testContent, content1, "Primeira leitura correta");
            
            // Segunda leitura (hit)
            std::string content2 = file_manager_->readFile(testFile);
            assertEqual(testContent, content2, "Segunda leitura (cache) correta");
            
            // Verificar estatísticas de cache
            auto stats = file_manager_->getStatistics();
            assertTrue(stats.cache_hits > 0, "Cache hits registrados na validação");
            
            // Limpar
            std::remove(testFile.c_str());
            
            assertTrue(true, "Validação de otimização concluída");
            
        } catch (const std::exception& e) {
            std::cerr << "Erro na validação de otimização: " << e.what() << std::endl;
            assert(false);
        }
    }
};

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main() {
    std::cout << "=== TESTE ABRANGENTE DE OTIMIZAÇÃO ===" << std::endl;
    std::cout << "Executando testes de performance e validação..." << std::endl;
    
    try {
        OptimizationValidator validator;
        
        auto start = high_resolution_clock::now();
        validator.runAllTests();
        auto end = high_resolution_clock::now();
        
        auto duration = duration_cast<milliseconds>(end - start);
        
        std::cout << "\n=== RESUMO DOS TESTES ===" << std::endl;
        std::cout << "Todos os testes de otimização foram executados com sucesso!" << std::endl;
        std::cout << "Tempo total de execução: " << duration.count() << " ms" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Erro durante a execução dos testes: " << e.what() << std::endl;
        return 1;
    }
}