#ifndef PREPROCESSOR_LOGGER_HPP
#define PREPROCESSOR_LOGGER_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include "preprocessor_config.hpp"
#include "preprocessor_types.hpp"

namespace Preprocessor {

/**
 * @brief Enumeração dos níveis de log
 * 
 * Define a hierarquia de importância das mensagens de log,
 * permitindo filtragem baseada no nível mínimo configurado.
 */
enum class LogLevel {
    DEBUG = 0,    ///< Informações detalhadas de debug
    INFO = 1,     ///< Informações gerais
    WARNING = 2,  ///< Avisos
    ERROR = 3     ///< Erros
};

/**
 * @brief Estrutura para estatísticas de logging
 * 
 * Armazena contadores e métricas sobre o uso do sistema de logging.
 */
struct LogStatistics {
    size_t debugCount = 0;      ///< Número de mensagens DEBUG
    size_t infoCount = 0;       ///< Número de mensagens INFO
    size_t warningCount = 0;    ///< Número de mensagens WARNING
    size_t errorCount = 0;      ///< Número de mensagens ERROR
    size_t totalMessages = 0;   ///< Total de mensagens processadas
    size_t bufferedMessages = 0; ///< Mensagens em buffer
    size_t fileRotations = 0;   ///< Número de rotações de arquivo
    
    void reset() {
        debugCount = infoCount = warningCount = errorCount = 0;
        totalMessages = bufferedMessages = fileRotations = 0;
    }
};

/**
 * @brief Classe PreprocessorLogger
 * 
 * Sistema de logging configurável para o pré-processador.
 * Suporta diferentes níveis de log, saída para arquivo e console,
 * buffering, rotação de arquivos e formatação específica para
 * diretivas e posições no código fonte.
 */
class PreprocessorLogger {
private:
    LogLevel currentLogLevel;                           ///< Nível mínimo de log
    std::unique_ptr<std::ofstream> outputFile;          ///< Arquivo de saída (opcional)
    bool consoleOutput;                                 ///< Flag para saída no console
    bool fileOutput;                                    ///< Flag para saída em arquivo
    bool bufferingEnabled;                              ///< Flag para buffering
    size_t maxBufferSize;                               ///< Tamanho máximo do buffer
    size_t maxFileSize;                                 ///< Tamanho máximo do arquivo de log
    std::string currentLogFile;                         ///< Nome do arquivo de log atual
    std::string logDirectory;                           ///< Diretório para arquivos de log
    
    // Buffer de mensagens
    std::queue<std::string> messageBuffer;              ///< Buffer de mensagens
    
    // Estatísticas
    LogStatistics statistics;                           ///< Estatísticas de logging
    
    // Métodos privados auxiliares
    std::string formatLogMessage(LogLevel level, const std::string& message, 
                                const PreprocessorPosition& pos = PreprocessorPosition()) const;
    std::string getCurrentTimestamp() const;
    void writeToFile(const std::string& message);
    void writeToConsole(const std::string& message);
    std::string logLevelToString(LogLevel level) const;
    bool shouldLog(LogLevel level) const;
    void updateStatistics(LogLevel level);
    void checkFileRotation();
    std::string generateRotatedFileName() const;
    
public:
    // Construtores e destrutor
    explicit PreprocessorLogger(LogLevel level = LogLevel::INFO);
    ~PreprocessorLogger();
    
    // Configuração básica
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const { return currentLogLevel; }
    void setOutputFile(const std::string& filename);
    void setOutputStream(std::ostream* stream);
    void enableConsoleOutput(bool enable = true);
    void enableFileOutput(bool enable = true);
    
    // Inicialização e configuração avançada
    void initializeLogger();
    void setLogDirectory(const std::string& directory);
    void setMaxFileSize(size_t maxSize);
    void setMaxBufferSize(size_t maxSize);
    void enableBuffering(bool enable = true);
    
    // Métodos de log por nível
    void debug(const std::string& message, const PreprocessorPosition& pos = PreprocessorPosition());
    void info(const std::string& message, const PreprocessorPosition& pos = PreprocessorPosition());
    void warning(const std::string& message, const PreprocessorPosition& pos = PreprocessorPosition());
    void error(const std::string& message, const PreprocessorPosition& pos = PreprocessorPosition());
    
    // Métodos de log específicos do preprocessor
    void logDirective(const std::string& directiveName, const std::string& content, 
                     const PreprocessorPosition& pos);
    void logMacroExpansion(const std::string& macroName, const std::string& expansion, 
                          const PreprocessorPosition& pos);
    void logIncludeFile(const std::string& filename, const PreprocessorPosition& pos);
    void logConditionalBlock(const std::string& condition, bool result, 
                            const PreprocessorPosition& pos);
    
    // Validação e filtragem
    bool validateLogLevel(LogLevel level) const;
    bool filterMessage(const std::string& message, LogLevel level) const;
    void validateLogFormat(const std::string& message) const;
    
    // Enriquecimento de mensagens
    std::string addTimestamp(const std::string& message) const;
    std::string addSourceLocation(const std::string& message, const PreprocessorPosition& pos) const;
    
    // Gerenciamento de buffer
    void handleLogBuffering(const std::string& message);
    void flushLogBuffer();
    void clearBuffer();
    size_t getBufferSize() const { return messageBuffer.size(); }
    
    // Gerenciamento de arquivos
    void rotateLogFile();
    void compressOldLogs();
    std::vector<std::string> getLogFiles() const;
    void cleanupOldLogs(size_t maxFiles = 10);
    
    // Relatórios e estatísticas
    LogStatistics calculateLogStatistics() const { return statistics; }
    std::string generateLogReport() const;
    void resetStatistics();
    
    // Tratamento de erros e otimização
    void handleLogErrors(const std::string& errorMessage);
    void optimizeLogPerformance();
    
    // Getters
    bool isConsoleOutputEnabled() const { return consoleOutput; }
    bool isFileOutputEnabled() const { return fileOutput; }
    bool isBufferingEnabled() const { return bufferingEnabled; }
    bool hasOutputFile() const { return outputFile && outputFile->is_open(); }
    std::string getCurrentLogFile() const { return currentLogFile; }
    std::string getLogDirectory() const { return logDirectory; }
};

// Funções utilitárias globais
std::string logLevelToString(LogLevel level);
LogLevel stringToLogLevel(const std::string& levelStr);
std::string formatPreprocessorPosition(const PreprocessorPosition& pos);
bool isValidLogLevel(LogLevel level);

} // namespace Preprocessor

#endif // PREPROCESSOR_LOGGER_HPP