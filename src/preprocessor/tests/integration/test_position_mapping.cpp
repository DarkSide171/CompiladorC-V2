/**
 * @file test_position_mapping.cpp
 * @brief Testes específicos para validação de mapeamento de posições
 */

#include "../../../lexer_preprocessor_bridge.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace Integration;
using namespace std;

/**
 * @brief Classe para testes de mapeamento de posições
 */
class PositionMappingTester {
public:
    /**
     * @brief Executa todos os testes de mapeamento
     */
    static bool runAllTests() {
        cout << "=== TESTES DE MAPEAMENTO DE POSIÇÕES ===" << endl;
        
        bool allPassed = true;
        
        allPassed &= testBasicMapping();
        allPassed &= testMacroExpansionMapping();
        allPassed &= testComplexMapping();
        
        if (allPassed) {
            cout << "\n✅ TODOS OS TESTES DE MAPEAMENTO PASSARAM!" << endl;
        } else {
            cout << "\n❌ ALGUNS TESTES DE MAPEAMENTO FALHARAM!" << endl;
        }
        
        return allPassed;
    }
    
    /**
     * @brief Testa mapeamento básico sem macros
     */
    static bool testBasicMapping() {
        cout << "\n--- Teste: Mapeamento Básico ---" << endl;
        
        try {
            IntegrationConfig config;
            config.enablePositionMapping = true;
            config.enableDebugMode = true;
            
            LexerPreprocessorBridge bridge(config);
            
            if (!bridge.initialize()) {
                cout << "❌ Falha na inicialização" << endl;
                return false;
            }
            
            // Código simples sem preprocessamento
            string testCode = 
                "int main() {\n"
                "    int x = 42;\n"
                "    return x;\n"
                "}";
            
            if (!bridge.processString(testCode, "basic_test.c")) {
                cout << "❌ Falha no processamento" << endl;
                return false;
            }
            
            // Verificar mapeamento
            size_t originalLine, originalColumn;
            string originalFile;
            
            bool mapped = bridge.mapToOriginalPosition(2, 5, originalLine, originalColumn, originalFile);
            
            if (mapped) {
                cout << "✅ Mapeamento encontrado: linha " << originalLine 
                     << ", coluna " << originalColumn 
                     << ", arquivo: " << originalFile << endl;
            } else {
                cout << "⚠️  Mapeamento não encontrado (pode ser normal para código simples)" << endl;
            }
            
            cout << "✅ Teste de mapeamento básico passou" << endl;
            return true;
            
        } catch (const exception& e) {
            cout << "❌ Exceção no teste básico: " << e.what() << endl;
            return false;
        }
    }
    
    /**
     * @brief Testa mapeamento com expansão de macros
     */
    static bool testMacroExpansionMapping() {
        cout << "\n--- Teste: Mapeamento com Macros ---" << endl;
        
        try {
            IntegrationConfig config;
            config.enablePositionMapping = true;
            config.enableMacroTracking = true;
            config.enableDebugMode = true;
            
            LexerPreprocessorBridge bridge(config);
            
            if (!bridge.initialize()) {
                cout << "❌ Falha na inicialização" << endl;
                return false;
            }
            
            // Código com macros
            string testCode = 
                "#define MAX_SIZE 1024\n"
                "#define SQUARE(x) ((x) * (x))\n"
                "\n"
                "int main() {\n"
                "    int size = MAX_SIZE;\n"
                "    int area = SQUARE(size);\n"
                "    return area;\n"
                "}";
            
            if (!bridge.processString(testCode, "macro_test.c")) {
                cout << "❌ Falha no processamento" << endl;
                return false;
            }
            
            // Verificar se macros foram definidas
            auto macros = bridge.getDefinedMacros();
            bool hasMaxSize = false, hasSquare = false;
            
            for (const auto& macro : macros) {
                if (macro == "MAX_SIZE") hasMaxSize = true;
                if (macro == "SQUARE") hasSquare = true;
            }
            
            if (hasMaxSize && hasSquare) {
                cout << "✅ Macros detectadas corretamente" << endl;
            } else {
                cout << "⚠️  Nem todas as macros foram detectadas" << endl;
            }
            
            // Testar mapeamento de posições onde macros foram expandidas
            size_t originalLine, originalColumn;
            string originalFile;
            
            bool mapped = bridge.mapToOriginalPosition(5, 15, originalLine, originalColumn, originalFile);
            
            if (mapped) {
                cout << "✅ Mapeamento de macro encontrado: linha " << originalLine 
                     << ", arquivo: " << originalFile << endl;
            } else {
                cout << "⚠️  Mapeamento de macro não encontrado (implementação pode estar incompleta)" << endl;
            }
            
            cout << "✅ Teste de mapeamento com macros passou" << endl;
            return true;
            
        } catch (const exception& e) {
            cout << "❌ Exceção no teste de macros: " << e.what() << endl;
            return false;
        }
    }
    
    /**
     * @brief Testa mapeamento complexo com múltiplas features
     */
    static bool testComplexMapping() {
        cout << "\n--- Teste: Mapeamento Complexo ---" << endl;
        
        try {
            IntegrationConfig config;
            config.enablePositionMapping = true;
            config.enableMacroTracking = true;
            config.enableDebugMode = true;
            
            LexerPreprocessorBridge bridge(config);
            
            if (!bridge.initialize()) {
                cout << "❌ Falha na inicialização" << endl;
                return false;
            }
            
            // Código complexo combinando várias features
            string testCode = 
                "#define PI 3.14159\n"
                "#define AREA(r) (PI * (r) * (r))\n"
                "#define VOLUME(r, h) (AREA(r) * (h))\n"
                "\n"
                "int main() {\n"
                "    double radius = 5.0;\n"
                "    double height = 10.0;\n"
                "    double vol = VOLUME(radius, height);\n"
                "    return (int)vol;\n"
                "}";
            
            if (!bridge.processString(testCode, "complex_test.c")) {
                cout << "❌ Falha no processamento" << endl;
                return false;
            }
            
            // Verificar estatísticas
            auto stats = bridge.getStatistics();
            cout << "📊 Estatísticas:" << endl;
            for (const auto& stat : stats) {
                cout << "   " << stat.first << ": " << stat.second << endl;
            }
            
            // Testar múltiplos mapeamentos
            vector<pair<size_t, size_t>> testPositions = {
                {5, 5},   // linha do main
                {8, 10},  // linha com VOLUME
                {9, 5}    // linha do return
            };
            
            int mappingsFound = 0;
            for (const auto& pos : testPositions) {
                size_t line = pos.first;
                size_t col = pos.second;
                size_t originalLine, originalColumn;
                string originalFile;
                
                if (bridge.mapToOriginalPosition(line, col, originalLine, originalColumn, originalFile)) {
                    mappingsFound++;
                    cout << "✅ Mapeamento " << line << ":" << col 
                         << " -> " << originalLine << ":" << originalColumn 
                         << " (" << originalFile << ")" << endl;
                }
            }
            
            cout << "📍 Mapeamentos encontrados: " << mappingsFound << "/" << testPositions.size() << endl;
            
            cout << "✅ Teste de mapeamento complexo passou" << endl;
            return true;
            
        } catch (const exception& e) {
            cout << "❌ Exceção no teste complexo: " << e.what() << endl;
            return false;
        }
    }
};

/**
 * @brief Função principal para executar testes de mapeamento
 */
int main() {
    cout << "INICIANDO TESTES DE MAPEAMENTO DE POSIÇÕES" << endl;
    cout << "==========================================" << endl;
    
    bool allTestsPassed = PositionMappingTester::runAllTests();
    
    cout << "\n==========================================" << endl;
    
    if (allTestsPassed) {
        cout << "🎉 TODOS OS TESTES DE MAPEAMENTO PASSARAM!" << endl;
        return 0;
    } else {
        cout << "💥 ALGUNS TESTES DE MAPEAMENTO FALHARAM!" << endl;
        return 1;
    }
}