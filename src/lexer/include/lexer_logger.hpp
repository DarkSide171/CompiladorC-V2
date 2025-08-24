#ifndef LEXER_LOGGER_HPP
#define LEXER_LOGGER_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "token.hpp"
#include "lexer_state.hpp"

namespace Lexer {

// Enumeração dos níveis de log
enum class LogLevel {
    DEBUG = 0,    // Informações detalhadas de debug
    INFO = 1,     // Informações gerais
    WARNING = 2,  // Avisos
    ERROR = 3     // Erros
};

/**
 * Classe LexerLogger
 * 
 * Sistema de logging configurável para o analisador léxico.
 * Suporta diferentes níveis de log, saída para arquivo e console,
 * e formatação específica para tokens e transições de estado.
 */
class LexerLogger {
private:
    LogLevel currentLogLevel;                    // Nível mínimo de log
    std::unique_ptr<std::ofstream> outputFile;   // Arquivo de saída (opcional)
    bool consoleOutput;                          // Flag para saída no console
    
    // Métodos privados auxiliares
    std::string formatLogMessage(LogLevel level, const std::string& message) const;
    std::string getCurrentTimestamp() const;
    void writeToFile(const std::string& message);
    void writeToConsole(const std::string& message);
    std::string logLevelToString(LogLevel level) const;
    
public:
    // Construtores e destrutor
    explicit LexerLogger(LogLevel level = LogLevel::INFO);
    ~LexerLogger() = default;
    
    // Configuração
    void setLogLevel(LogLevel level);
    void setOutputFile(const std::string& filename);
    void enableConsoleOutput(bool enable = true);
    
    // Métodos de log por nível
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    
    // Métodos de log específicos
    void logToken(const Token& token);
    void logStateTransition(const std::string& from, const std::string& to);
    void logStateTransition(LexerState from, LexerState to);
    
    // Getters
    LogLevel getLogLevel() const { return currentLogLevel; }
    bool isConsoleOutputEnabled() const { return consoleOutput; }
    bool hasOutputFile() const { return outputFile && outputFile->is_open(); }
};

// Funções utilitárias
std::string logLevelToString(LogLevel level);
LogLevel stringToLogLevel(const std::string& levelStr);

} // namespace Lexer

#endif // LEXER_LOGGER_HPP