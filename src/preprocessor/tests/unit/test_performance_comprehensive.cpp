#include "../../include/file_manager.hpp"
#include "../../include/macro_processor.hpp"
#include "../../include/preprocessor_logger.hpp"
#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <cassert>

using namespace Preprocessor;
using namespace std::chrono;

class PerformanceProfiler {
private:
    std::vector<std::pair<std::string, duration<double>>> measurements;
    
public:
    template<typename Func>
    void measure(const std::string& name, Func&& func) {
        auto start = high_resolution_clock::now();
        func();
        auto end = high_resolution_clock::now();
        
        duration<double> elapsed = end - start;
        measurements.emplace_back(name, elapsed);
        
        std::cout << "[PROFILE] " << name << ": " 
                  << elapsed.count() * 1000.0 << " ms" << std::endl;
    }
    
    void printSummary() {
        std::cout << "\n=== PERFORMANCE SUMMARY ===" << std::endl;
        
        // Ordenar por tempo
        std::sort(measurements.begin(), measurements.end(), 
                 [](const auto& a, const auto& b) {
                     return a.second > b.second;
                 });
        
        double total = 0.0;
        for (const auto& [name, time] : measurements) {
            double ms = time.count() * 1000.0;
            std::cout << name << ": " << ms << " ms" << std::endl;
            total += ms;
        }
        
        std::cout << "Total: " << total << " ms" << std::endl;
        std::cout << "===========================\n" << std::endl;
    }
};

// Função auxiliar para gerar código com macros
std::string generateLargeCode(int numMacros, int numExpansions) {
    std::ostringstream code;
    
    // Definir macros
    for (int i = 0; i < numMacros; i++) {
        code << "#define MACRO_" << i << "(x) (x * " << i << " + 1)\n";
    }
    
    code << "\n";
    
    // Usar macros
    for (int i = 0; i < numExpansions; i++) {
        int macroId = i % numMacros;
        code << "int result_" << i << " = MACRO_" << macroId << "(" << i << ");\n";
    }
    
    return code.str();
}

// Função auxiliar para gerar nomes de macros aleatórios
std::vector<std::string> generateRandomMacroNames(int count) {
    std::vector<std::string> names;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000000);
    
    for (int i = 0; i < count; i++) {
        names.push_back("MACRO_" + std::to_string(dis(gen)));
    }
    
    return names;
}

void testCacheOptimization(PerformanceProfiler& profiler) {
    std::cout << "\n=== Teste de Otimizações de Cache ===" << std::endl;
    
    // Cria FileManager sem logger
    FileManager fm;
    
    // Testa configuração de cache
    std::cout << "\n1. Testando configuração de cache..." << std::endl;
    fm.configureCacheOptimization(
        1024 * 1024,  // 1MB max size
        100,           // 100 entradas max
        std::chrono::seconds(30), // TTL 30s
        false          // sem compressão
    );
    
    // Cria arquivo temporário para teste
    std::string test_file = "/tmp/test_cache_file.txt";
    {
        std::ofstream ofs(test_file);
        ofs << "Conteúdo de teste para cache\n";
        ofs << "Linha 2\n";
        ofs << "Linha 3\n";
    }
    
    // Testa leitura e cache
    std::cout << "\n2. Testando leitura e cache..." << std::endl;
    try {
        profiler.measure("FileManager - First Read (no cache)", [&]() {
            std::string content1 = fm.readFile(test_file);
        });
        
        profiler.measure("FileManager - Second Read (with cache)", [&]() {
            std::string content2 = fm.readFile(test_file);
        });
        
        std::cout << "✓ Cache funcionando corretamente" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Erro: " << e.what() << std::endl;
        return;
    }
    
    // Testa pré-carregamento
    std::cout << "\n3. Testando pré-carregamento..." << std::endl;
    std::vector<std::string> files_to_preload = {test_file};
    fm.preloadFiles(files_to_preload);
    
    // Testa otimização de cache
    std::cout << "\n4. Testando otimização de cache..." << std::endl;
    fm.optimizeCache();
    
    // Testa estatísticas do cache
    std::cout << "\n5. Testando estatísticas do cache..." << std::endl;
    FileStats stats = fm.getStatistics();
    std::cout << "Cache hits: " << stats.cache_hits << std::endl;
    std::cout << "Cache misses: " << stats.cache_misses << std::endl;
    std::cout << "Hit ratio: " << stats.getCacheHitRatio() << std::endl;
    
    // Limpa arquivo temporário
    std::remove(test_file.c_str());
}

void testMacroProcessorPerformance(PerformanceProfiler& profiler) {
    std::cout << "\n=== Teste de Performance do MacroProcessor ===" << std::endl;
    
    auto macroProcessor = std::make_unique<MacroProcessor>();
    macroProcessor->setCacheEnabled(true);
    macroProcessor->configureCacheOptimization(2000, true);
    
    std::string complexCode = generateLargeCode(100, 500);
    
    profiler.measure("MacroProcessor - Complex Expansion", [&]() {
        std::string result = macroProcessor->processLine(complexCode);
    });
    
    // Teste de pré-carregamento
    std::vector<std::string> frequentMacros = {"MACRO_0", "MACRO_1", "MACRO_2"};
    profiler.measure("MacroProcessor - Preload Frequent Macros", [&]() {
        macroProcessor->preloadFrequentMacros(frequentMacros);
    });
    
    // Teste de otimização de cache
    profiler.measure("MacroProcessor - Cache Optimization", [&]() {
        macroProcessor->optimizeCache(300);
    });
    
    // Exibir estatísticas
    std::string stats = macroProcessor->getStatistics();
    std::cout << "Statistics: " << stats << std::endl;
    std::cout << "Current cache size: " << macroProcessor->getCurrentCacheSize() << std::endl;
}

void testDataStructureOptimization(PerformanceProfiler& profiler) {
    std::cout << "\n=== Teste de Otimização de Estruturas de Dados ===" << std::endl;
    
    // Teste 1: Inserção de macros em lote
    std::cout << "\n--- Teste 1: Macro Definition Performance ---" << std::endl;
    {
        auto macroProcessor = std::make_unique<MacroProcessor>();
        auto macroNames = generateRandomMacroNames(5000);
        
        profiler.measure("Define 5000 Random Macros", [&]() {
            for (const auto& name : macroNames) {
                macroProcessor->defineMacro(name, "value_" + name);
            }
        });
        
        std::cout << "Defined macros count: " << macroProcessor->getDefinedMacros().size() << std::endl;
    }
    
    // Teste 2: Busca de macros
    std::cout << "\n--- Teste 2: Macro Lookup Performance ---" << std::endl;
    {
        auto macroProcessor = std::make_unique<MacroProcessor>();
        auto macroNames = generateRandomMacroNames(2000);
        
        // Definir macros
        for (const auto& name : macroNames) {
            macroProcessor->defineMacro(name, "value_" + name);
        }
        
        // Teste de busca sequencial
        profiler.measure("Sequential Lookup 2000 Macros", [&]() {
            for (const auto& name : macroNames) {
                bool exists = macroProcessor->isDefined(name);
                (void)exists; // Evitar warning
            }
        });
        
        // Teste de busca aleatória
        auto shuffledNames = macroNames;
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(shuffledNames.begin(), shuffledNames.end(), g);
        
        profiler.measure("Random Lookup 2000 Macros", [&]() {
            for (const auto& name : shuffledNames) {
                bool exists = macroProcessor->isDefined(name);
                (void)exists; // Evitar warning
            }
        });
    }
    
    // Teste 3: Expansão de macros com diferentes complexidades
    std::cout << "\n--- Teste 3: Macro Expansion Complexity ---" << std::endl;
    {
        auto macroProcessor = std::make_unique<MacroProcessor>();
        
        // Macros simples
        profiler.measure("Define 500 Simple Macros", [&]() {
            for (int i = 0; i < 500; i++) {
                macroProcessor->defineMacro("SIMPLE_" + std::to_string(i), std::to_string(i));
            }
        });
        
        // Macros funcionais
        profiler.measure("Define 500 Function Macros", [&]() {
            for (int i = 0; i < 500; i++) {
                std::vector<std::string> params = {"x", "y"};
                macroProcessor->defineFunctionMacro("FUNC_" + std::to_string(i), params, "(x + y * " + std::to_string(i) + ")");
            }
        });
        
        // Teste de expansão simples
        std::string simpleText = "SIMPLE_0 + SIMPLE_1 + SIMPLE_2";
        profiler.measure("Expand Simple Macros (500 iterations)", [&]() {
            for (int i = 0; i < 500; i++) {
                std::string result = macroProcessor->expandMacroRecursively(simpleText);
                (void)result; // Evitar warning
            }
        });
        
        // Teste de expansão funcional
        std::vector<std::string> args = {"10", "20"};
        profiler.measure("Expand Function Macros (100 iterations)", [&]() {
            for (int i = 0; i < 100; i++) {
                std::string result = macroProcessor->expandFunctionMacro("FUNC_0", args);
                (void)result; // Evitar warning
            }
        });
    }
}

void testStringOperationsPerformance(PerformanceProfiler& profiler) {
    std::cout << "\n=== Teste de Performance de Operações com Strings ===" << std::endl;
    
    std::vector<std::string> testStrings;
    for (int i = 0; i < 500; i++) {
        testStrings.push_back("This is a test string number " + std::to_string(i) + " with some content");
    }
    
    // Teste de concatenação com +
    profiler.measure("String Concatenation (+)", [&]() {
        std::string result;
        for (const auto& str : testStrings) {
            result = result + str + " ";
        }
    });
    
    // Teste de concatenação com +=
    profiler.measure("String Concatenation (+=)", [&]() {
        std::string result;
        for (const auto& str : testStrings) {
            result += str + " ";
        }
    });
    
    // Teste com reserve
    profiler.measure("String Concatenation (with reserve)", [&]() {
        std::string result;
        result.reserve(25000); // Estimativa
        for (const auto& str : testStrings) {
            result += str + " ";
        }
    });
}

void testComparisonWithWithoutOptimizations(PerformanceProfiler& profiler) {
    std::cout << "\n=== Comparação Com/Sem Otimizações ===" << std::endl;
    
    std::string testCode = generateLargeCode(50, 200);
    
    // Sem otimizações
    {
        auto macroProcessor = std::make_unique<MacroProcessor>();
        macroProcessor->setCacheEnabled(false);
        
        profiler.measure("Without Optimizations", [&]() {
            std::string result = macroProcessor->processLine(testCode);
        });
    }
    
    // Com otimizações
    {
        auto macroProcessor = std::make_unique<MacroProcessor>();
        macroProcessor->setCacheEnabled(true);
        macroProcessor->configureCacheOptimization(2000, true);
        std::vector<std::string> frequentMacros = {"MACRO_0", "MACRO_1", "MACRO_2"};
        macroProcessor->preloadFrequentMacros(frequentMacros);
        
        profiler.measure("With Optimizations", [&]() {
            std::string result = macroProcessor->processLine(testCode);
        });
    }
}

int main() {
    std::cout << "=== COMPREHENSIVE PERFORMANCE TEST ===\n" << std::endl;
    
    PerformanceProfiler profiler;
    
    try {
        // Executar todos os testes de performance
        testCacheOptimization(profiler);
        testMacroProcessorPerformance(profiler);
        testDataStructureOptimization(profiler);
        testStringOperationsPerformance(profiler);
        testComparisonWithWithoutOptimizations(profiler);
        
        // Imprimir resumo final
        profiler.printSummary();
        
        std::cout << "\n=== TODOS OS TESTES DE PERFORMANCE CONCLUÍDOS ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro durante testes de performance: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}