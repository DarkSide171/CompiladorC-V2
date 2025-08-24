// Lexer Logger - Implementação do sistema de logging
// Implementação da classe LexerLogger para logging e debug

#include "../include/lexer_logger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>

namespace Lexer {

// ============================================================================
// Construtores
// ============================================================================

LexerLogger::LexerLogger(LogLevel level)
    : currentLogLevel(level), consoleOutput(true) {
}

// ============================================================================
// Configuração
// ============================================================================

void LexerLogger::setLogLevel(LogLevel level) {
    currentLogLevel = level;
}

void LexerLogger::setOutputFile(const std::string& filename) {
    outputFile = std::make_unique<std::ofstream>(filename, std::ios::app);
    if (!outputFile->is_open()) {
        std::cerr << "[LexerLogger] Erro: Não foi possível abrir arquivo de log: " << filename << std::endl;
        outputFile.reset();
    }
}

void LexerLogger::enableConsoleOutput(bool enable) {
    consoleOutput = enable;
}

// ============================================================================
// Métodos de log por nível
// ============================================================================

void LexerLogger::debug(const std::string& message) {
    if (currentLogLevel <= LogLevel::DEBUG) {
        std::string formattedMessage = formatLogMessage(LogLevel::DEBUG, message);
        if (consoleOutput) {
            writeToConsole(formattedMessage);
        }
        if (outputFile && outputFile->is_open()) {
            writeToFile(formattedMessage);
        }
    }
}

void LexerLogger::info(const std::string& message) {
    if (currentLogLevel <= LogLevel::INFO) {
        std::string formattedMessage = formatLogMessage(LogLevel::INFO, message);
        if (consoleOutput) {
            writeToConsole(formattedMessage);
        }
        if (outputFile && outputFile->is_open()) {
            writeToFile(formattedMessage);
        }
    }
}

void LexerLogger::warning(const std::string& message) {
    if (currentLogLevel <= LogLevel::WARNING) {
        std::string formattedMessage = formatLogMessage(LogLevel::WARNING, message);
        if (consoleOutput) {
            writeToConsole(formattedMessage);
        }
        if (outputFile && outputFile->is_open()) {
            writeToFile(formattedMessage);
        }
    }
}

void LexerLogger::error(const std::string& message) {
    if (currentLogLevel <= LogLevel::ERROR) {
        std::string formattedMessage = formatLogMessage(LogLevel::ERROR, message);
        if (consoleOutput) {
            writeToConsole(formattedMessage);
        }
        if (outputFile && outputFile->is_open()) {
            writeToFile(formattedMessage);
        }
    }
}

// ============================================================================
// Métodos de log específicos
// ============================================================================

void LexerLogger::logToken(const Token& token) {
    if (currentLogLevel <= LogLevel::DEBUG) {
        std::ostringstream oss;
        oss << "Token gerado: [Tipo: " << static_cast<int>(token.getType()) 
            << ", Lexeme: '" << token.getLexeme() << "'"
            << ", Posição: (" << token.getPosition().line << "," << token.getPosition().column << ")]"; 
        debug(oss.str());
    }
}

void LexerLogger::logStateTransition(const std::string& from, const std::string& to) {
    if (currentLogLevel <= LogLevel::DEBUG) {
        std::ostringstream oss;
        oss << "Transição de estado: " << from << " -> " << to;
        debug(oss.str());
    }
}

void LexerLogger::logStateTransition(LexerState from, LexerState to) {
    if (currentLogLevel <= LogLevel::DEBUG) {
        logStateTransition(lexerStateToString(from), lexerStateToString(to));
    }
}

// ============================================================================
// Métodos privados auxiliares
// ============================================================================

std::string LexerLogger::formatLogMessage(LogLevel level, const std::string& message) const {
    std::ostringstream oss;
    oss << "[" << getCurrentTimestamp() << "] "
        << "[" << logLevelToString(level) << "] "
        << message;
    return oss.str();
}

std::string LexerLogger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

void LexerLogger::writeToFile(const std::string& message) {
    if (outputFile && outputFile->is_open()) {
        *outputFile << message << std::endl;
        outputFile->flush();
    }
}

void LexerLogger::writeToConsole(const std::string& message) {
    std::cout << message << std::endl;
}

std::string LexerLogger::logLevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

// ============================================================================
// Funções utilitárias globais
// ============================================================================

std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

LogLevel stringToLogLevel(const std::string& levelStr) {
    if (levelStr == "DEBUG") return LogLevel::DEBUG;
    if (levelStr == "INFO") return LogLevel::INFO;
    if (levelStr == "WARNING") return LogLevel::WARNING;
    if (levelStr == "ERROR") return LogLevel::ERROR;
    return LogLevel::INFO; // Default
}

} // namespace Lexer