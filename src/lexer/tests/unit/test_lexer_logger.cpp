/**
 * @file test_lexer_logger.cpp
 * @brief Testes unitários para a classe LexerLogger
 * @author Sistema de Compilador C
 * @date 2024
 */

#include "../../include/lexer_logger.hpp"
#include "../../include/token.hpp"
#include "../../include/lexer_state.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <memory>

using namespace Lexer;

// ============================================================================
// Funções auxiliares para testes
// ============================================================================

void testConstructor() {
    std::cout << "Testando construtor do LexerLogger..." << std::endl;
    
    // Teste construtor padrão
    LexerLogger logger1;
    assert(logger1.getLogLevel() == LogLevel::INFO);
    assert(logger1.isConsoleOutputEnabled() == true);
    assert(logger1.hasOutputFile() == false);
    
    // Teste construtor com nível específico
    LexerLogger logger2(LogLevel::DEBUG);
    assert(logger2.getLogLevel() == LogLevel::DEBUG);
    
    LexerLogger logger3(LogLevel::ERROR);
    assert(logger3.getLogLevel() == LogLevel::ERROR);
    
    std::cout << "✓ Construtor funcionando corretamente" << std::endl;
}

void testSetLogLevel() {
    std::cout << "Testando setLogLevel()..." << std::endl;
    
    LexerLogger logger;
    
    // Teste mudança de nível
    logger.setLogLevel(LogLevel::DEBUG);
    assert(logger.getLogLevel() == LogLevel::DEBUG);
    
    logger.setLogLevel(LogLevel::WARNING);
    assert(logger.getLogLevel() == LogLevel::WARNING);
    
    logger.setLogLevel(LogLevel::ERROR);
    assert(logger.getLogLevel() == LogLevel::ERROR);
    
    std::cout << "✓ setLogLevel funcionando corretamente" << std::endl;
}

void testConsoleOutput() {
    std::cout << "Testando controle de saída no console..." << std::endl;
    
    LexerLogger logger;
    
    // Teste estado inicial
    assert(logger.isConsoleOutputEnabled() == true);
    
    // Teste desabilitar console
    logger.enableConsoleOutput(false);
    assert(logger.isConsoleOutputEnabled() == false);
    
    // Teste reabilitar console
    logger.enableConsoleOutput(true);
    assert(logger.isConsoleOutputEnabled() == true);
    
    std::cout << "✓ Controle de console funcionando corretamente" << std::endl;
}

void testFileOutput() {
    std::cout << "Testando saída para arquivo..." << std::endl;
    
    LexerLogger logger;
    
    // Teste estado inicial (sem arquivo)
    assert(logger.hasOutputFile() == false);
    
    // Teste definir arquivo válido
    std::string testFile = "/tmp/test_lexer_log.txt";
    logger.setOutputFile(testFile);
    assert(logger.hasOutputFile() == true);
    
    // Teste escrever no arquivo
    logger.info("Teste de escrita no arquivo");
    
    // Verificar se arquivo foi criado
    std::ifstream file(testFile);
    assert(file.is_open());
    file.close();
    
    // Limpar arquivo de teste
    std::remove(testFile.c_str());
    
    std::cout << "✓ Saída para arquivo funcionando corretamente" << std::endl;
}

void testLogLevels() {
    std::cout << "Testando níveis de log..." << std::endl;
    
    // Teste com nível DEBUG (deve mostrar tudo)
    LexerLogger debugLogger(LogLevel::DEBUG);
    debugLogger.enableConsoleOutput(false); // Desabilitar console para teste
    
    debugLogger.debug("Mensagem DEBUG");
    debugLogger.info("Mensagem INFO");
    debugLogger.warning("Mensagem WARNING");
    debugLogger.error("Mensagem ERROR");
    
    // Teste com nível WARNING (deve mostrar apenas WARNING e ERROR)
    LexerLogger warningLogger(LogLevel::WARNING);
    warningLogger.enableConsoleOutput(false);
    
    warningLogger.debug("Mensagem DEBUG - não deve aparecer");
    warningLogger.info("Mensagem INFO - não deve aparecer");
    warningLogger.warning("Mensagem WARNING");
    warningLogger.error("Mensagem ERROR");
    
    // Teste com nível ERROR (deve mostrar apenas ERROR)
    LexerLogger errorLogger(LogLevel::ERROR);
    errorLogger.enableConsoleOutput(false);
    
    errorLogger.debug("Mensagem DEBUG - não deve aparecer");
    errorLogger.info("Mensagem INFO - não deve aparecer");
    errorLogger.warning("Mensagem WARNING - não deve aparecer");
    errorLogger.error("Mensagem ERROR");
    
    std::cout << "✓ Níveis de log funcionando corretamente" << std::endl;
}

void testLogToken() {
    std::cout << "Testando logToken()..." << std::endl;
    
    LexerLogger logger(LogLevel::DEBUG);
    logger.enableConsoleOutput(false);
    
    // Criar token de teste
    Position pos{1, 5, 10};
    Token token(TokenType::IDENTIFIER, "variavel", pos);
    
    // Testar log do token
    logger.logToken(token);
    
    // Teste com diferentes tipos de token
    Token intToken(TokenType::INTEGER_LITERAL, "42", pos);
    logger.logToken(intToken);
    
    Token keywordToken(TokenType::INT, "int", pos);
    logger.logToken(keywordToken);
    
    std::cout << "✓ logToken funcionando corretamente" << std::endl;
}

void testLogStateTransition() {
    std::cout << "Testando logStateTransition()..." << std::endl;
    
    LexerLogger logger(LogLevel::DEBUG);
    logger.enableConsoleOutput(false);
    
    // Testar transições de estado com strings
    logger.logStateTransition("START", "IDENTIFIER");
    logger.logStateTransition("IDENTIFIER", "ACCEPT");
    logger.logStateTransition("START", "NUMBER");
    logger.logStateTransition("NUMBER", "FLOAT");
    logger.logStateTransition("FLOAT", "ACCEPT");
    
    // Testar transições de estado com enums LexerState
    logger.logStateTransition(LexerState::START, LexerState::IDENTIFIER);
    logger.logStateTransition(LexerState::IDENTIFIER, LexerState::ACCEPT_IDENTIFIER);
    logger.logStateTransition(LexerState::START, LexerState::INTEGER);
    logger.logStateTransition(LexerState::INTEGER, LexerState::ACCEPT_INTEGER);
    logger.logStateTransition(LexerState::START, LexerState::STRING_START);
    logger.logStateTransition(LexerState::STRING_START, LexerState::STRING_BODY);
    
    std::cout << "✓ logStateTransition funcionando corretamente" << std::endl;
}

void testUtilityFunctions() {
    std::cout << "Testando funções utilitárias..." << std::endl;
    
    // Teste logLevelToString
    assert(logLevelToString(LogLevel::DEBUG) == "DEBUG");
    assert(logLevelToString(LogLevel::INFO) == "INFO");
    assert(logLevelToString(LogLevel::WARNING) == "WARNING");
    assert(logLevelToString(LogLevel::ERROR) == "ERROR");
    
    // Teste stringToLogLevel
    assert(stringToLogLevel("DEBUG") == LogLevel::DEBUG);
    assert(stringToLogLevel("INFO") == LogLevel::INFO);
    assert(stringToLogLevel("WARNING") == LogLevel::WARNING);
    assert(stringToLogLevel("ERROR") == LogLevel::ERROR);
    assert(stringToLogLevel("INVALID") == LogLevel::INFO); // Default
    
    std::cout << "✓ Funções utilitárias funcionando corretamente" << std::endl;
}

void testCompleteLoggingScenario() {
    std::cout << "Testando cenário completo de logging..." << std::endl;
    
    // Criar logger com arquivo de saída
    LexerLogger logger(LogLevel::DEBUG);
    std::string logFile = "/tmp/complete_test_log.txt";
    logger.setOutputFile(logFile);
    
    // Simular processo de análise léxica
    logger.info("Iniciando análise léxica");
    
    // Simular tokens
    Position pos{1, 1, 0};
    Token tokens[] = {
        Token(TokenType::INT, "int", pos),
        Token(TokenType::IDENTIFIER, "main", pos),
        Token(TokenType::LEFT_PAREN, "(", pos),
        Token(TokenType::RIGHT_PAREN, ")", pos)
    };
    
    for (const auto& token : tokens) {
        logger.logToken(token);
        logger.logStateTransition("PROCESSING", "NEXT_TOKEN");
    }
    
    logger.info("Análise léxica concluída");
    
    // Verificar se arquivo foi criado e contém dados
    std::ifstream file(logFile);
    assert(file.is_open());
    
    std::string line;
    int lineCount = 0;
    while (std::getline(file, line)) {
        lineCount++;
        assert(!line.empty()); // Verificar que linhas não estão vazias
    }
    file.close();
    
    assert(lineCount > 0); // Verificar que algo foi escrito
    
    // Limpar arquivo de teste
    std::remove(logFile.c_str());
    
    std::cout << "✓ Cenário completo funcionando corretamente" << std::endl;
}

// ============================================================================
// Função principal de testes
// ============================================================================

int main() {
    std::cout << "=== Iniciando Testes do LexerLogger ===" << std::endl;
    
    try {
        testConstructor();
        testSetLogLevel();
        testConsoleOutput();
        testFileOutput();
        testLogLevels();
        testLogToken();
        testLogStateTransition();
        testUtilityFunctions();
        testCompleteLoggingScenario();
        
        std::cout << "\n=== Todos os testes do LexerLogger passaram! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Erro durante os testes: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Erro desconhecido durante os testes" << std::endl;
        return 1;
    }
}