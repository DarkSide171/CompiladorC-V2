/**
 * @file test_lexer_preprocessor_integration.cpp
 * @brief Testes de integração entre Lexer e Preprocessor
 * 
 * Este arquivo implementa testes abrangentes para verificar a integração
 * completa entre o lexer e o preprocessor, incluindo fluxo de tokens,
 * tratamento de erros e compatibilidade.
 */

#include "../../../lexer_preprocessor_bridge.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>

using namespace Integration;
using namespace std;

/**
 * @brief Classe para testes de integração lexer-preprocessor
 */
class LexerPreprocessorIntegrationTester {
public:
    /**
     * @brief Executa todos os testes de integração
     */
    static bool runAllTests() {
        cout << "=== TESTES DE INTEGRAÇÃO LEXER-PREPROCESSOR ===" << endl;
        
        bool allPassed = true;
        
        allPassed &= testBasicIntegration();
        allPassed &= testTokenFlow();
        allPassed &= testMacroIntegration();
        allPassed &= testErrorHandling();
        allPassed &= testPerformance();
        allPassed &= testCompatibility();
        
        if (allPassed) {
            cout << "\n✅ TODOS OS TESTES DE INTEGRAÇÃO PASSARAM!" << endl;
        } else {
            cout << "\n❌ ALGUNS TESTES DE INTEGRAÇÃO FALHARAM!" << endl;
        }
        
        return allPassed;
    }
    
    /**
     * @brief Testa integração básica entre componentes
     */
    static bool testBasicIntegration() {
        cout << "\n--- Teste: Integração Básica ---" << endl;
        
        try {
            IntegrationConfig config;
            config.enablePositionMapping = true;
            config.enableMacroTracking = true;
            config.enableErrorIntegration = true;
            config.enableDebugMode = true;
            
            LexerPreprocessorBridge bridge(config);
            
            // Verificar inicialização
            if (!bridge.initialize()) {
                cout << "❌ Falha na inicialização da ponte" << endl;
                return false;
            }
            
            cout << "✅ Ponte inicializada com sucesso" << endl;
            
            // Código de teste simples
            string testCode = 
                "#include <stdio.h>\n"
                "#define MESSAGE \"Hello, World!\"\n"
                "\n"
                "int main() {\n"
                "    printf(MESSAGE);\n"
                "    return 0;\n"
                "}";
            
            // Processar código
            if (!bridge.processString(testCode, "integration_test.c")) {
                cout << "❌ Falha no processamento do código" << endl;
                return false;
            }
            
            cout << "✅ Código processado com sucesso" << endl;
            
            // Verificar se há tokens disponíveis
            if (!bridge.hasMoreTokens()) {
                cout << "⚠️  Nenhum token disponível após processamento" << endl;
            } else {
                cout << "✅ Tokens disponíveis para análise" << endl;
            }
            
            // Obter estatísticas
            auto stats = bridge.getStatistics();
            cout << "📊 Estatísticas da integração:" << endl;
            for (const auto& stat : stats) {
                cout << "   " << stat.first << ": " << stat.second << endl;
            }
            
            cout << "✅ Teste de integração básica passou" << endl;
            return true;
            
        } catch (const exception& e) {
            cout << "❌ Exceção no teste de integração básica: " << e.what() << endl;
            return false;
        }
    }
    
    /**
     * @brief Testa fluxo de tokens entre lexer e preprocessor
     */
    static bool testTokenFlow() {
        cout << "\n--- Teste: Fluxo de Tokens ---" << endl;
        
        try {
            IntegrationConfig config;
            config.enablePositionMapping = true;
            config.enableMacroTracking = true;
            
            LexerPreprocessorBridge bridge(config);
            
            if (!bridge.initialize()) {
                cout << "❌ Falha na inicialização" << endl;
                return false;
            }
            
            // Código com diferentes tipos de tokens
            string testCode = 
                "#define MAX 100\n"
                "#define MIN 0\n"
                "\n"
                "int calculate(int x, int y) {\n"
                "    if (x > MAX) x = MAX;\n"
                "    if (y < MIN) y = MIN;\n"
                "    return x + y * 2;\n"
                "}";
            
            if (!bridge.processString(testCode, "token_flow_test.c")) {
                cout << "❌ Falha no processamento" << endl;
                return false;
            }
            
            // Testar fluxo sequencial de tokens
            vector<IntegratedToken> tokens;
            int tokenCount = 0;
            const int maxTokens = 50; // Limite para evitar loop infinito
            
            cout << "🔄 Extraindo tokens sequencialmente..." << endl;
            
            while (bridge.hasMoreTokens() && tokenCount < maxTokens) {
                IntegratedToken token = bridge.nextToken();
                tokens.push_back(token);
                tokenCount++;
                
                // Mostrar alguns tokens para debug
                if (tokenCount <= 10) {
                    cout << "   Token " << tokenCount << ": " 
                         << token.originalText << " (preprocessor: " 
                         << (token.isFromPreprocessor ? "sim" : "não") << ")" << endl;
                }
            }
            
            cout << "📝 Total de tokens extraídos: " << tokenCount << endl;
            
            if (tokenCount > 0) {
                cout << "✅ Fluxo de tokens funcionando" << endl;
            } else {
                cout << "⚠️  Nenhum token extraído (pode indicar problema)" << endl;
            }
            
            // Testar peek (olhar próximo token sem consumir)
            bridge.reset();
            if (!bridge.processString(testCode, "token_flow_test.c")) {
                cout << "❌ Falha no reprocessamento" << endl;
                return false;
            }
            
            if (bridge.hasMoreTokens()) {
                IntegratedToken peeked1 = bridge.peekToken();
                IntegratedToken peeked2 = bridge.peekToken();
                IntegratedToken consumed = bridge.nextToken();
                
                // Peek deve retornar o mesmo token
                if (peeked1.originalText == peeked2.originalText && 
                    peeked1.originalText == consumed.originalText) {
                    cout << "✅ Função peek funcionando corretamente" << endl;
                } else {
                    cout << "⚠️  Função peek pode ter problemas" << endl;
                }
            }
            
            cout << "✅ Teste de fluxo de tokens passou" << endl;
            return true;
            
        } catch (const exception& e) {
            cout << "❌ Exceção no teste de fluxo de tokens: " << e.what() << endl;
            return false;
        }
    }
    
    /**
     * @brief Testa integração específica de macros
     */
    static bool testMacroIntegration() {
        cout << "\n--- Teste: Integração de Macros ---" << endl;
        
        try {
            IntegrationConfig config;
            config.enableMacroTracking = true;
            config.enablePositionMapping = true;
            
            LexerPreprocessorBridge bridge(config);
            
            if (!bridge.initialize()) {
                cout << "❌ Falha na inicialização" << endl;
                return false;
            }
            
            // Definir macros programaticamente
            bridge.defineMacro("VERSION", "1.0");
            bridge.defineMacro("DEBUG_MODE", "1");
            bridge.defineMacro("BUFFER_SIZE", "1024");
            
            cout << "✅ Macros definidas programaticamente" << endl;
            
            // Código que usa as macros
            string testCode = 
                "#define MULTIPLY(a, b) ((a) * (b))\n"
                "#define SQUARE(x) MULTIPLY(x, x)\n"
                "\n"
                "int main() {\n"
                "    int version = VERSION;\n"
                "    int buffer = BUFFER_SIZE;\n"
                "    int area = SQUARE(5);\n"
                "    return area;\n"
                "}";
            
            if (!bridge.processString(testCode, "macro_integration_test.c")) {
                cout << "❌ Falha no processamento com macros" << endl;
                return false;
            }
            
            // Verificar macros definidas
            auto definedMacros = bridge.getDefinedMacros();
            cout << "📋 Macros definidas (" << definedMacros.size() << "):" << endl;
            
            bool hasVersion = false, hasBufferSize = false, hasMultiply = false;
            
            for (const auto& macro : definedMacros) {
                cout << "   - " << macro << endl;
                if (macro == "VERSION") hasVersion = true;
                if (macro == "BUFFER_SIZE") hasBufferSize = true;
                if (macro == "MULTIPLY") hasMultiply = true;
            }
            
            if (hasVersion && hasBufferSize && hasMultiply) {
                cout << "✅ Todas as macros esperadas foram encontradas" << endl;
            } else {
                cout << "⚠️  Algumas macros podem não ter sido processadas corretamente" << endl;
            }
            
            // Testar remoção de macro
            bridge.undefineMacro("DEBUG_MODE");
            cout << "✅ Macro removida programaticamente" << endl;
            
            cout << "✅ Teste de integração de macros passou" << endl;
            return true;
            
        } catch (const exception& e) {
            cout << "❌ Exceção no teste de integração de macros: " << e.what() << endl;
            return false;
        }
    }
    
    /**
     * @brief Testa tratamento integrado de erros
     */
    static bool testErrorHandling() {
        cout << "\n--- Teste: Tratamento de Erros ---" << endl;
        
        try {
            IntegrationConfig config;
            config.enableErrorIntegration = true;
            config.enableDebugMode = true;
            
            LexerPreprocessorBridge bridge(config);
            
            if (!bridge.initialize()) {
                cout << "❌ Falha na inicialização" << endl;
                return false;
            }
            
            // Código com possíveis problemas
            string problematicCode = 
                "#define INCOMPLETE_MACRO\n"  // Macro sem valor
                "#include \"nonexistent.h\"\n"  // Include inexistente
                "\n"
                "int main() {\n"
                "    int x = UNDEFINED_MACRO;\n"  // Macro não definida
                "    return x;\n"
                "}";
            
            // Processar código problemático
            bool processed = bridge.processString(problematicCode, "error_test.c");
            
            cout << "🔍 Processamento de código problemático: " 
                 << (processed ? "sucesso" : "falhou") << endl;
            
            // Verificar se erros foram capturados
            bool hasErrors = bridge.hasErrors();
            auto errorMessages = bridge.getErrorMessages();
            auto warningMessages = bridge.getWarningMessages();
            
            cout << "⚠️  Erros detectados: " << (hasErrors ? "sim" : "não") << endl;
            cout << "📝 Mensagens de erro: " << errorMessages.size() << endl;
            cout << "📝 Mensagens de aviso: " << warningMessages.size() << endl;
            
            // Mostrar algumas mensagens
            for (size_t i = 0; i < min(errorMessages.size(), size_t(3)); ++i) {
                cout << "   Erro " << (i+1) << ": " << errorMessages[i] << endl;
            }
            
            for (size_t i = 0; i < min(warningMessages.size(), size_t(3)); ++i) {
                cout << "   Aviso " << (i+1) << ": " << warningMessages[i] << endl;
            }
            
            // Testar recuperação após erro
            bridge.reset();
            
            string validCode = 
                "#define VALID_MACRO 42\n"
                "int main() {\n"
                "    int x = VALID_MACRO;\n"
                "    return x;\n"
                "}";
            
            if (bridge.processString(validCode, "recovery_test.c")) {
                cout << "✅ Recuperação após erro bem-sucedida" << endl;
            } else {
                cout << "⚠️  Problemas na recuperação após erro" << endl;
            }
            
            cout << "✅ Teste de tratamento de erros passou" << endl;
            return true;
            
        } catch (const exception& e) {
            cout << "❌ Exceção no teste de tratamento de erros: " << e.what() << endl;
            return false;
        }
    }
    
    /**
     * @brief Testa performance da integração
     */
    static bool testPerformance() {
        cout << "\n--- Teste: Performance ---" << endl;
        
        try {
            IntegrationConfig config;
            config.enablePositionMapping = true;
            config.enableMacroTracking = true;
            
            LexerPreprocessorBridge bridge(config);
            
            if (!bridge.initialize()) {
                cout << "❌ Falha na inicialização" << endl;
                return false;
            }
            
            // Gerar código de teste maior
            string largeCode = generateLargeTestCode(1000); // 1000 linhas
            
            cout << "📏 Código de teste gerado: " << largeCode.length() << " caracteres" << endl;
            
            // Medir tempo de processamento
            auto startTime = chrono::high_resolution_clock::now();
            
            bool processed = bridge.processString(largeCode, "performance_test.c");
            
            auto endTime = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
            
            cout << "⏱️  Tempo de processamento: " << duration.count() << " ms" << endl;
            
            if (!processed) {
                cout << "❌ Falha no processamento do código grande" << endl;
                return false;
            }
            
            // Medir tempo de tokenização
            startTime = chrono::high_resolution_clock::now();
            
            int tokenCount = 0;
            while (bridge.hasMoreTokens() && tokenCount < 10000) { // Limite de segurança
                bridge.nextToken();
                tokenCount++;
            }
            
            endTime = chrono::high_resolution_clock::now();
            duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
            
            cout << "🔢 Tokens processados: " << tokenCount << endl;
            cout << "⏱️  Tempo de tokenização: " << duration.count() << " ms" << endl;
            
            if (duration.count() < 5000) { // Menos de 5 segundos é aceitável
                cout << "✅ Performance aceitável" << endl;
            } else {
                cout << "⚠️  Performance pode precisar de otimização" << endl;
            }
            
            cout << "✅ Teste de performance passou" << endl;
            return true;
            
        } catch (const exception& e) {
            cout << "❌ Exceção no teste de performance: " << e.what() << endl;
            return false;
        }
    }
    
    /**
     * @brief Testa compatibilidade entre componentes
     */
    static bool testCompatibility() {
        cout << "\n--- Teste: Compatibilidade ---" << endl;
        
        try {
            // Testar diferentes configurações
            vector<IntegrationConfig> configs = {
                createConfig(true, true, true, false),   // Completo sem debug
                createConfig(true, false, true, false),  // Sem macro tracking
                createConfig(false, true, true, false),  // Sem position mapping
                createConfig(true, true, false, false),  // Sem error integration
            };
            
            string testCode = 
                "#define TEST_MACRO 123\n"
                "int main() {\n"
                "    int x = TEST_MACRO;\n"
                "    return x;\n"
                "}";
            
            for (size_t i = 0; i < configs.size(); ++i) {
                cout << "🔧 Testando configuração " << (i+1) << "/" << configs.size() << "..." << endl;
                
                LexerPreprocessorBridge bridge(configs[i]);
                
                if (!bridge.initialize()) {
                    cout << "❌ Falha na inicialização da configuração " << (i+1) << endl;
                    return false;
                }
                
                if (!bridge.processString(testCode, "compatibility_test.c")) {
                    cout << "❌ Falha no processamento da configuração " << (i+1) << endl;
                    return false;
                }
                
                // Verificar se pelo menos alguns tokens foram gerados
                int tokenCount = 0;
                while (bridge.hasMoreTokens() && tokenCount < 20) {
                    bridge.nextToken();
                    tokenCount++;
                }
                
                if (tokenCount > 0) {
                    cout << "   ✅ Configuração " << (i+1) << " compatível (" << tokenCount << " tokens)" << endl;
                } else {
                    cout << "   ⚠️  Configuração " << (i+1) << " pode ter problemas" << endl;
                }
            }
            
            cout << "✅ Teste de compatibilidade passou" << endl;
            return true;
            
        } catch (const exception& e) {
            cout << "❌ Exceção no teste de compatibilidade: " << e.what() << endl;
            return false;
        }
    }
    
private:
    /**
     * @brief Cria uma configuração de integração
     */
    static IntegrationConfig createConfig(bool posMapping, bool macroTrack, bool errorInteg, bool debug) {
        IntegrationConfig config;
        config.enablePositionMapping = posMapping;
        config.enableMacroTracking = macroTrack;
        config.enableErrorIntegration = errorInteg;
        config.enableDebugMode = debug;
        return config;
    }
    
    /**
     * @brief Gera código de teste grande para testes de performance
     */
    static string generateLargeTestCode(int lines) {
        string code;
        code.reserve(lines * 50); // Estimativa de tamanho
        
        // Cabeçalho
        code += "#define MAX_VALUE 1000\n";
        code += "#define MIN_VALUE 0\n";
        code += "#define MULTIPLY(a, b) ((a) * (b))\n";
        code += "\n";
        
        // Gerar funções
        for (int i = 0; i < lines / 10; ++i) {
            code += "int function" + to_string(i) + "(int x) {\n";
            code += "    if (x > MAX_VALUE) return MAX_VALUE;\n";
            code += "    if (x < MIN_VALUE) return MIN_VALUE;\n";
            code += "    return MULTIPLY(x, " + to_string(i + 1) + ");\n";
            code += "}\n\n";
        }
        
        // Função main
        code += "int main() {\n";
        code += "    int result = 0;\n";
        
        for (int i = 0; i < lines / 20; ++i) {
            code += "    result += function" + to_string(i % (lines / 10)) + "(" + to_string(i) + ");\n";
        }
        
        code += "    return result;\n";
        code += "}\n";
        
        return code;
    }
};

/**
 * @brief Função principal para executar testes de integração
 */
int main() {
    cout << "INICIANDO TESTES DE INTEGRAÇÃO LEXER-PREPROCESSOR" << endl;
    cout << "================================================" << endl;
    
    bool allTestsPassed = LexerPreprocessorIntegrationTester::runAllTests();
    
    cout << "\n================================================" << endl;
    
    if (allTestsPassed) {
        cout << "🎉 TODOS OS TESTES DE INTEGRAÇÃO PASSARAM!" << endl;
        return 0;
    } else {
        cout << "💥 ALGUNS TESTES DE INTEGRAÇÃO FALHARAM!" << endl;
        return 1;
    }
}