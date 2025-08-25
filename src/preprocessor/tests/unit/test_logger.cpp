// Testes Unitários - Sistema de Logging
// Testes para preprocessor_logger.hpp - Fase 1.6
// Testa LogLevel, PreprocessorPosition, LogStatistics, PreprocessorLogger e funções utilitárias

#include "../../include/preprocessor_logger.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

using namespace Preprocessor;

// ============================================================================
// FUNÇÕES AUXILIARES PARA TESTES
// ============================================================================

void assertEqual(const std::string& expected, const std::string& actual, const std::string& testName) {
    if (expected != actual) {
        std::cerr << "❌ FALHA em " << testName << ": esperado '" << expected 
                  << "', obtido '" << actual << "'" << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertEqual(size_t expected, size_t actual, const std::string& testName) {
    if (expected != actual) {
        std::cerr << "❌ FALHA em " << testName << ": esperado " << expected 
                  << ", obtido " << actual << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertTrue(bool condition, const std::string& testName) {
    if (!condition) {
        std::cerr << "❌ FALHA em " << testName << ": condição falsa" << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertFalse(bool condition, const std::string& testName) {
    if (condition) {
        std::cerr << "❌ FALHA em " << testName << ": condição verdadeira" << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertGreaterThan(size_t value, size_t threshold, const std::string& testName) {
    if (value <= threshold) {
        std::cerr << "❌ FALHA em " << testName << ": " << value 
                  << " não é maior que " << threshold << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

// ============================================================================
// TESTES DA ESTRUTURA PreprocessorPosition
// ============================================================================

void testPreprocessorPosition() {
    std::cout << "\n=== Testando PreprocessorPosition ===" << std::endl;
    
    // Teste construtor padrão
    PreprocessorPosition pos1;
    assertEqual("", pos1.filename, "Construtor padrão - filename vazio");
    assertEqual(size_t(1), pos1.line, "Construtor padrão - linha 1");
    assertEqual(size_t(1), pos1.column, "Construtor padrão - coluna 1");
    
    // Teste construtor com parâmetros
    PreprocessorPosition pos2("test.c", 10, 5);
    assertEqual("test.c", pos2.filename, "Construtor com parâmetros - filename");
    assertEqual(size_t(10), pos2.line, "Construtor com parâmetros - linha");
    assertEqual(size_t(5), pos2.column, "Construtor com parâmetros - coluna");
    
    // Teste operador de igualdade
    PreprocessorPosition pos3("test.c", 10, 5);
    PreprocessorPosition pos4("test.c", 10, 6);
    
    assertTrue(pos2 == pos3, "Posições iguais");
    assertFalse(pos2 == pos4, "Posições diferentes");
    
    // Teste modificação de valores
    pos1.filename = "modified.h";
    pos1.line = 42;
    pos1.column = 15;
    
    assertEqual("modified.h", pos1.filename, "Modificação filename");
    assertEqual(size_t(42), pos1.line, "Modificação linha");
    assertEqual(size_t(15), pos1.column, "Modificação coluna");
}

// ============================================================================
// TESTES DO ENUM LogLevel
// ============================================================================

void testLogLevelFunctions() {
    std::cout << "\n=== Testando Funções de LogLevel ===" << std::endl;
    
    // Teste logLevelToString
    assertEqual("DEBUG", logLevelToString(LogLevel::DEBUG), "DEBUG -> string");
    assertEqual("INFO", logLevelToString(LogLevel::INFO), "INFO -> string");
    assertEqual("WARNING", logLevelToString(LogLevel::WARNING), "WARNING -> string");
    assertEqual("ERROR", logLevelToString(LogLevel::ERROR), "ERROR -> string");
    
    // Teste stringToLogLevel
    assertTrue(stringToLogLevel("DEBUG") == LogLevel::DEBUG, "string -> DEBUG");
    assertTrue(stringToLogLevel("INFO") == LogLevel::INFO, "string -> INFO");
    assertTrue(stringToLogLevel("WARNING") == LogLevel::WARNING, "string -> WARNING");
    assertTrue(stringToLogLevel("ERROR") == LogLevel::ERROR, "string -> ERROR");
    
    // Teste case insensitive não é suportado - função aceita apenas maiúsculas
    assertTrue(stringToLogLevel("INVALID") == LogLevel::INFO, "string inválida -> INFO (padrão)");
    assertTrue(stringToLogLevel("debug") == LogLevel::INFO, "debug (lowercase) -> INFO (padrão)");
    
    // Teste isValidLogLevel
    assertTrue(isValidLogLevel(LogLevel::DEBUG), "DEBUG é válido");
    assertTrue(isValidLogLevel(LogLevel::INFO), "INFO é válido");
    assertTrue(isValidLogLevel(LogLevel::WARNING), "WARNING é válido");
    assertTrue(isValidLogLevel(LogLevel::ERROR), "ERROR é válido");
}

// ============================================================================
// TESTES DA ESTRUTURA LogStatistics
// ============================================================================

void testLogStatistics() {
    std::cout << "\n=== Testando LogStatistics ===" << std::endl;
    
    LogStatistics stats;
    
    // Teste valores iniciais
    assertEqual(size_t(0), stats.debugCount, "debugCount inicial");
    assertEqual(size_t(0), stats.infoCount, "infoCount inicial");
    assertEqual(size_t(0), stats.warningCount, "warningCount inicial");
    assertEqual(size_t(0), stats.errorCount, "errorCount inicial");
    assertEqual(size_t(0), stats.totalMessages, "totalMessages inicial");
    assertEqual(size_t(0), stats.bufferedMessages, "bufferedMessages inicial");
    assertEqual(size_t(0), stats.fileRotations, "fileRotations inicial");
    
    // Teste modificação de valores
    stats.debugCount = 5;
    stats.infoCount = 10;
    stats.warningCount = 3;
    stats.errorCount = 1;
    stats.totalMessages = 19;
    stats.bufferedMessages = 2;
    stats.fileRotations = 1;
    
    assertEqual(size_t(5), stats.debugCount, "debugCount modificado");
    assertEqual(size_t(10), stats.infoCount, "infoCount modificado");
    assertEqual(size_t(3), stats.warningCount, "warningCount modificado");
    assertEqual(size_t(1), stats.errorCount, "errorCount modificado");
    assertEqual(size_t(19), stats.totalMessages, "totalMessages modificado");
    assertEqual(size_t(2), stats.bufferedMessages, "bufferedMessages modificado");
    assertEqual(size_t(1), stats.fileRotations, "fileRotations modificado");
    
    // Teste reset
    stats.reset();
    assertEqual(size_t(0), stats.debugCount, "debugCount após reset");
    assertEqual(size_t(0), stats.infoCount, "infoCount após reset");
    assertEqual(size_t(0), stats.warningCount, "warningCount após reset");
    assertEqual(size_t(0), stats.errorCount, "errorCount após reset");
    assertEqual(size_t(0), stats.totalMessages, "totalMessages após reset");
    assertEqual(size_t(0), stats.bufferedMessages, "bufferedMessages após reset");
    assertEqual(size_t(0), stats.fileRotations, "fileRotations após reset");
}

// ============================================================================
// TESTES DA CLASSE PreprocessorLogger - CONSTRUTORES
// ============================================================================

void testPreprocessorLoggerConstructors() {
    std::cout << "\n=== Testando Construtores de PreprocessorLogger ===" << std::endl;
    
    // Teste construtor padrão
    PreprocessorLogger logger1;
    assertTrue(logger1.getLogLevel() == LogLevel::INFO, "Construtor padrão usa INFO");
    assertTrue(logger1.isConsoleOutputEnabled(), "Console output habilitado por padrão");
    assertFalse(logger1.isFileOutputEnabled(), "File output desabilitado por padrão");
    assertFalse(logger1.hasOutputFile(), "Sem arquivo de output por padrão");
    
    // Teste construtor com nível específico
    PreprocessorLogger logger2(LogLevel::DEBUG);
    assertTrue(logger2.getLogLevel() == LogLevel::DEBUG, "Construtor com DEBUG");
    
    PreprocessorLogger logger3(LogLevel::ERROR);
    assertTrue(logger3.getLogLevel() == LogLevel::ERROR, "Construtor com ERROR");
    
    PreprocessorLogger logger4(LogLevel::WARNING);
    assertTrue(logger4.getLogLevel() == LogLevel::WARNING, "Construtor com WARNING");
}

// ============================================================================
// TESTES DE CONFIGURAÇÃO BÁSICA
// ============================================================================

void testBasicConfiguration() {
    std::cout << "\n=== Testando Configuração Básica ===" << std::endl;
    
    PreprocessorLogger logger;
    
    // Teste setLogLevel
    logger.setLogLevel(LogLevel::DEBUG);
    assertTrue(logger.getLogLevel() == LogLevel::DEBUG, "setLogLevel(DEBUG)");
    
    logger.setLogLevel(LogLevel::ERROR);
    assertTrue(logger.getLogLevel() == LogLevel::ERROR, "setLogLevel(ERROR)");
    
    // Teste enableConsoleOutput
    logger.enableConsoleOutput(false);
    assertFalse(logger.isConsoleOutputEnabled(), "Console output desabilitado");
    
    logger.enableConsoleOutput(true);
    assertTrue(logger.isConsoleOutputEnabled(), "Console output habilitado");
    
    // Teste enableFileOutput
    logger.enableFileOutput(true);
    assertTrue(logger.isFileOutputEnabled(), "File output habilitado");
    
    logger.enableFileOutput(false);
    assertFalse(logger.isFileOutputEnabled(), "File output desabilitado");
    
    // Teste initializeLogger
    logger.initializeLogger();
    std::cout << "✅ Logger inicializado com sucesso" << std::endl;
}

// ============================================================================
// TESTES DE CONFIGURAÇÃO AVANÇADA
// ============================================================================

void testAdvancedConfiguration() {
    std::cout << "\n=== Testando Configuração Avançada ===" << std::endl;
    
    PreprocessorLogger logger;
    
    // Teste setLogDirectory
    logger.setLogDirectory("./test_logs");
    assertEqual("./test_logs", logger.getLogDirectory(), "setLogDirectory");
    
    // Teste setMaxFileSize
    logger.setMaxFileSize(1024 * 1024); // 1MB
    std::cout << "✅ setMaxFileSize configurado" << std::endl;
    
    // Teste setMaxBufferSize
    logger.setMaxBufferSize(100);
    std::cout << "✅ setMaxBufferSize configurado" << std::endl;
    
    // Teste enableBuffering
    logger.enableBuffering(true);
    assertTrue(logger.isBufferingEnabled(), "Buffering habilitado");
    
    logger.enableBuffering(false);
    assertFalse(logger.isBufferingEnabled(), "Buffering desabilitado");
    
    // Teste setOutputFile
    const std::string testFile = "test_log.txt";
    logger.setOutputFile(testFile);
    assertTrue(logger.hasOutputFile(), "Arquivo de output configurado");
    assertEqual(testFile, logger.getCurrentLogFile(), "Nome do arquivo correto");
    
    // Limpa arquivo de teste
    std::remove(testFile.c_str());
}

// ============================================================================
// TESTES DE MÉTODOS DE LOG POR NÍVEL
// ============================================================================

void testLogMethods() {
    std::cout << "\n=== Testando Métodos de Log por Nível ===" << std::endl;
    
    PreprocessorLogger logger(LogLevel::DEBUG);
    logger.enableConsoleOutput(false); // Desabilita console para não poluir output
    
    PreprocessorPosition pos("test.c", 10, 5);
    
    // Teste métodos básicos de log
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warning("Warning message");
    logger.error("Error message");
    
    std::cout << "✅ Métodos básicos de log executados" << std::endl;
    
    // Teste métodos com posição
    logger.debug("Debug with position", pos);
    logger.info("Info with position", pos);
    logger.warning("Warning with position", pos);
    logger.error("Error with position", pos);
    
    std::cout << "✅ Métodos de log com posição executados" << std::endl;
    
    // Verifica estatísticas
    LogStatistics stats = logger.calculateLogStatistics();
    assertGreaterThan(stats.totalMessages, size_t(0), "Mensagens foram registradas");
    assertGreaterThan(stats.debugCount, size_t(0), "Mensagens DEBUG registradas");
    assertGreaterThan(stats.infoCount, size_t(0), "Mensagens INFO registradas");
    assertGreaterThan(stats.warningCount, size_t(0), "Mensagens WARNING registradas");
    assertGreaterThan(stats.errorCount, size_t(0), "Mensagens ERROR registradas");
}

// ============================================================================
// TESTES DE MÉTODOS ESPECÍFICOS DO PREPROCESSOR
// ============================================================================

void testPreprocessorSpecificMethods() {
    std::cout << "\n=== Testando Métodos Específicos do Preprocessor ===" << std::endl;
    
    PreprocessorLogger logger(LogLevel::DEBUG);
    logger.enableConsoleOutput(false);
    
    PreprocessorPosition pos("main.c", 15, 1);
    
    // Teste logDirective
    logger.logDirective("#include", "<stdio.h>", pos);
    logger.logDirective("#define", "MAX_SIZE 100", pos);
    logger.logDirective("#if", "defined(DEBUG)", pos);
    
    std::cout << "✅ logDirective executado" << std::endl;
    
    // Teste logMacroExpansion
    logger.logMacroExpansion("MAX_SIZE", "100", pos);
    logger.logMacroExpansion("DEBUG_PRINT", "printf(\"Debug: %s\\n\", msg)", pos);
    
    std::cout << "✅ logMacroExpansion executado" << std::endl;
    
    // Teste logIncludeFile
    logger.logIncludeFile("stdio.h", pos);
    logger.logIncludeFile("stdlib.h", pos);
    logger.logIncludeFile("custom_header.h", pos);
    
    std::cout << "✅ logIncludeFile executado" << std::endl;
    
    // Teste logConditionalBlock
    logger.logConditionalBlock("defined(DEBUG)", true, pos);
    logger.logConditionalBlock("VERSION >= 2", false, pos);
    logger.logConditionalBlock("__cplusplus", false, pos);
    
    std::cout << "✅ logConditionalBlock executado" << std::endl;
    
    // Verifica que mensagens foram registradas
    LogStatistics stats = logger.calculateLogStatistics();
    assertGreaterThan(stats.totalMessages, size_t(10), "Múltiplas mensagens registradas");
}

// ============================================================================
// TESTES DE VALIDAÇÃO E FILTRAGEM
// ============================================================================

void testValidationAndFiltering() {
    std::cout << "\n=== Testando Validação e Filtragem ===" << std::endl;
    
    PreprocessorLogger logger;
    
    // Teste validateLogLevel
    assertTrue(logger.validateLogLevel(LogLevel::DEBUG), "DEBUG é válido");
    assertTrue(logger.validateLogLevel(LogLevel::INFO), "INFO é válido");
    assertTrue(logger.validateLogLevel(LogLevel::WARNING), "WARNING é válido");
    assertTrue(logger.validateLogLevel(LogLevel::ERROR), "ERROR é válido");
    
    // Teste filterMessage
    assertTrue(logger.filterMessage("Valid message", LogLevel::INFO), "Mensagem válida passa no filtro");
    assertTrue(logger.filterMessage("Error occurred", LogLevel::ERROR), "Mensagem de erro passa no filtro");
    
    // Teste validateLogFormat
    logger.validateLogFormat("Valid log message");
    logger.validateLogFormat("Another valid message with numbers 123");
    
    std::cout << "✅ Validação de formato executada" << std::endl;
}

// ============================================================================
// TESTES DE ENRIQUECIMENTO DE MENSAGENS
// ============================================================================

void testMessageEnrichment() {
    std::cout << "\n=== Testando Enriquecimento de Mensagens ===" << std::endl;
    
    PreprocessorLogger logger;
    PreprocessorPosition pos("test.c", 42, 10);
    
    // Teste addTimestamp
    std::string message = "Test message";
    std::string timestamped = logger.addTimestamp(message);
    
    assertFalse(timestamped.empty(), "Timestamp adicionado");
    assertTrue(timestamped.length() > message.length(), "Mensagem com timestamp é maior");
    
    std::cout << "✅ Timestamp: " << timestamped.substr(0, 50) << "..." << std::endl;
    
    // Teste addSourceLocation
    std::string located = logger.addSourceLocation(message, pos);
    
    assertFalse(located.empty(), "Source location adicionada");
    assertTrue(located.find("test.c") != std::string::npos, "Filename incluído");
    assertTrue(located.find("42") != std::string::npos, "Linha incluída");
    assertTrue(located.find("10") != std::string::npos, "Coluna incluída");
    
    std::cout << "✅ Source location: " << located << std::endl;
}

// ============================================================================
// TESTES DE GERENCIAMENTO DE BUFFER
// ============================================================================

void testBufferManagement() {
    std::cout << "\n=== Testando Gerenciamento de Buffer ===" << std::endl;
    
    PreprocessorLogger logger;
    logger.enableBuffering(true);
    logger.setMaxBufferSize(5);
    
    // Teste buffer inicial vazio
    assertEqual(size_t(0), logger.getBufferSize(), "Buffer inicial vazio");
    
    // Teste handleLogBuffering
    logger.handleLogBuffering("Message 1");
    logger.handleLogBuffering("Message 2");
    logger.handleLogBuffering("Message 3");
    
    assertGreaterThan(logger.getBufferSize(), size_t(0), "Mensagens no buffer");
    
    std::cout << "✅ Buffer size: " << logger.getBufferSize() << std::endl;
    
    // Teste flushLogBuffer
    logger.flushLogBuffer();
    std::cout << "✅ Buffer flushed" << std::endl;
    
    // Teste clearBuffer
    logger.handleLogBuffering("New message");
    assertGreaterThan(logger.getBufferSize(), size_t(0), "Nova mensagem no buffer");
    
    logger.clearBuffer();
    assertEqual(size_t(0), logger.getBufferSize(), "Buffer limpo");
}

// ============================================================================
// TESTES DE GERENCIAMENTO DE ARQUIVOS
// ============================================================================

void testFileManagement() {
    std::cout << "\n=== Testando Gerenciamento de Arquivos ===" << std::endl;
    
    PreprocessorLogger logger;
    const std::string testDir = "./test_logs";
    const std::string testFile = testDir + "/test.log";
    
    // Cria diretório de teste se não existir
    mkdir(testDir.c_str(), 0755);
    
    logger.setLogDirectory(testDir);
    logger.setOutputFile(testFile);
    logger.enableFileOutput(true);
    
    // Teste escrita em arquivo
    logger.info("Test message for file");
    logger.warning("Another test message");
    
    assertTrue(logger.hasOutputFile(), "Arquivo de output ativo");
    
    // Teste getLogFiles
    std::vector<std::string> logFiles = logger.getLogFiles();
    std::cout << "✅ Arquivos de log encontrados: " << logFiles.size() << std::endl;
    
    // Teste rotateLogFile
    logger.rotateLogFile();
    std::cout << "✅ Rotação de arquivo executada" << std::endl;
    
    // Teste cleanupOldLogs
    logger.cleanupOldLogs(5);
    std::cout << "✅ Limpeza de logs antigos executada" << std::endl;
    
    // Teste compressOldLogs
    logger.compressOldLogs();
    std::cout << "✅ Compressão de logs antigos executada" << std::endl;
    
    // Limpa arquivos de teste
    std::remove(testFile.c_str());
    rmdir(testDir.c_str());
}

// ============================================================================
// TESTES DE RELATÓRIOS E ESTATÍSTICAS
// ============================================================================

void testReportsAndStatistics() {
    std::cout << "\n=== Testando Relatórios e Estatísticas ===" << std::endl;
    
    PreprocessorLogger logger(LogLevel::DEBUG);
    logger.enableConsoleOutput(false);
    
    // Gera algumas mensagens para estatísticas
    logger.debug("Debug 1");
    logger.debug("Debug 2");
    logger.info("Info 1");
    logger.info("Info 2");
    logger.info("Info 3");
    logger.warning("Warning 1");
    logger.error("Error 1");
    
    // Teste calculateLogStatistics
    LogStatistics stats = logger.calculateLogStatistics();
    
    assertEqual(size_t(2), stats.debugCount, "2 mensagens DEBUG");
    assertEqual(size_t(3), stats.infoCount, "3 mensagens INFO");
    assertEqual(size_t(1), stats.warningCount, "1 mensagem WARNING");
    assertEqual(size_t(1), stats.errorCount, "1 mensagem ERROR");
    assertEqual(size_t(7), stats.totalMessages, "7 mensagens totais");
    
    // Teste generateLogReport
    std::string report = logger.generateLogReport();
    
    assertFalse(report.empty(), "Relatório gerado");
    assertTrue(report.find("DEBUG") != std::string::npos, "Relatório contém DEBUG");
    assertTrue(report.find("INFO") != std::string::npos, "Relatório contém INFO");
    assertTrue(report.find("WARNING") != std::string::npos, "Relatório contém WARNING");
    assertTrue(report.find("ERROR") != std::string::npos, "Relatório contém ERROR");
    
    std::cout << "✅ Relatório gerado: " << report.length() << " caracteres" << std::endl;
    
    // Teste resetStatistics
    logger.resetStatistics();
    LogStatistics resetStats = logger.calculateLogStatistics();
    
    assertEqual(size_t(0), resetStats.debugCount, "debugCount resetado");
    assertEqual(size_t(0), resetStats.infoCount, "infoCount resetado");
    assertEqual(size_t(0), resetStats.warningCount, "warningCount resetado");
    assertEqual(size_t(0), resetStats.errorCount, "errorCount resetado");
    assertEqual(size_t(0), resetStats.totalMessages, "totalMessages resetado");
}

// ============================================================================
// TESTES DE TRATAMENTO DE ERROS E OTIMIZAÇÃO
// ============================================================================

void testErrorHandlingAndOptimization() {
    std::cout << "\n=== Testando Tratamento de Erros e Otimização ===" << std::endl;
    
    PreprocessorLogger logger;
    
    // Teste handleLogErrors
    logger.handleLogErrors("Test error message");
    logger.handleLogErrors("Another error occurred");
    
    std::cout << "✅ handleLogErrors executado" << std::endl;
    
    // Teste optimizeLogPerformance
    logger.optimizeLogPerformance();
    
    std::cout << "✅ optimizeLogPerformance executado" << std::endl;
}

// ============================================================================
// TESTES DE FUNÇÕES UTILITÁRIAS GLOBAIS
// ============================================================================

void testUtilityFunctions() {
    std::cout << "\n=== Testando Funções Utilitárias Globais ===" << std::endl;
    
    PreprocessorPosition pos("utility_test.c", 25, 8);
    
    // Teste formatPreprocessorPosition
    std::string formatted = formatPreprocessorPosition(pos);
    
    assertFalse(formatted.empty(), "Posição formatada");
    assertTrue(formatted.find("utility_test.c") != std::string::npos, "Filename na formatação");
    assertTrue(formatted.find("25") != std::string::npos, "Linha na formatação");
    assertTrue(formatted.find("8") != std::string::npos, "Coluna na formatação");
    
    std::cout << "✅ Posição formatada: " << formatted << std::endl;
    
    // Teste conversões bidirecionais
    for (int i = 0; i <= 3; ++i) {
        LogLevel level = static_cast<LogLevel>(i);
        std::string levelStr = logLevelToString(level);
        LogLevel convertedBack = stringToLogLevel(levelStr);
        
        assertTrue(level == convertedBack, "Conversão bidirecional " + levelStr);
    }
}

// ============================================================================
// TESTES DE CENÁRIOS COMPLEXOS
// ============================================================================

void testComplexScenarios() {
    std::cout << "\n=== Testando Cenários Complexos ===" << std::endl;
    
    // Cenário 1: Logger com múltiplas configurações
    PreprocessorLogger logger1(LogLevel::DEBUG);
    logger1.enableConsoleOutput(false);
    logger1.enableFileOutput(true);
    logger1.enableBuffering(true);
    logger1.setMaxBufferSize(10);
    logger1.setOutputFile("complex_test.log");
    
    PreprocessorPosition pos1("complex.c", 100, 1);
    PreprocessorPosition pos2("complex.h", 50, 15);
    
    // Gera logs complexos
    logger1.logDirective("#include", "\"complex.h\"", pos1);
    logger1.logMacroExpansion("COMPLEX_MACRO", "do { /* complex code */ } while(0)", pos1);
    logger1.logConditionalBlock("defined(FEATURE_X) && VERSION > 1", true, pos2);
    logger1.warning("Complex warning message", pos1);
    logger1.error("Complex error in nested include", pos2);
    
    // Flush e verifica
    logger1.flushLogBuffer();
    LogStatistics complexStats = logger1.calculateLogStatistics();
    assertGreaterThan(complexStats.totalMessages, size_t(0), "Mensagens complexas registradas");
    
    // Cenário 2: Múltiplos loggers
    PreprocessorLogger logger2(LogLevel::WARNING);
    PreprocessorLogger logger3(LogLevel::ERROR);
    
    logger2.info("This should not be logged"); // Abaixo do nível WARNING
    logger2.warning("This should be logged");
    logger3.warning("This should not be logged"); // Abaixo do nível ERROR
    logger3.error("This should be logged");
    
    std::cout << "✅ Cenários complexos executados" << std::endl;
    
    // Limpa arquivo de teste
    std::remove("complex_test.log");
}

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main() {
    std::cout << "=== EXECUTANDO TESTES DO SISTEMA DE LOGGING (FASE 1.6) ===" << std::endl;
    
    try {
        // Testes das estruturas básicas
        testPreprocessorPosition();
        testLogLevelFunctions();
        testLogStatistics();
        
        // Testes da classe PreprocessorLogger
        testPreprocessorLoggerConstructors();
        testBasicConfiguration();
        testAdvancedConfiguration();
        
        // Testes de funcionalidades de log
        testLogMethods();
        testPreprocessorSpecificMethods();
        
        // Testes de validação e processamento
        testValidationAndFiltering();
        testMessageEnrichment();
        
        // Testes de gerenciamento
        testBufferManagement();
        testFileManagement();
        
        // Testes de relatórios
        testReportsAndStatistics();
        
        // Testes de tratamento de erros
        testErrorHandlingAndOptimization();
        
        // Testes de funções utilitárias
        testUtilityFunctions();
        
        // Testes de cenários complexos
        testComplexScenarios();
        
        std::cout << "\n🎉 Todos os testes do sistema de logging passaram com sucesso!" << std::endl;
        std::cout << "📊 Total de testes executados: 14 grupos de teste" << std::endl;
        std::cout << "✅ Fase 1.6 - test_logger.cpp: CONCLUÍDO" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Erro durante os testes: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Erro desconhecido durante os testes" << std::endl;
        return 1;
    }
}