// Preprocessor Logger - Implementação do sistema de logging
// Implementação da classe PreprocessorLogger para logging e debug

#include "../include/preprocessor_logger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>

namespace Preprocessor {

// ============================================================================
// Funções Auxiliares para Operações de Arquivo
// ============================================================================

static bool directoryExists(const std::string& path) {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
}

static bool fileExists(const std::string& path) {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFREG));
}

static void createDirectory(const std::string& path) {
    mkdir(path.c_str(), 0755);
}

static size_t getFileSize(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) == 0) {
        return static_cast<size_t>(info.st_size);
    }
    return 0;
}

static std::vector<std::string> listDirectory(const std::string& path) {
    std::vector<std::string> files;
    DIR* dir = opendir(path.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_REG) { // Apenas arquivos regulares
                std::string filename = entry->d_name;
                if (filename.find("preprocessor_") != std::string::npos && 
                    filename.find(".log") != std::string::npos) {
                    files.push_back(path + "/" + filename);
                }
            }
        }
        closedir(dir);
    }
    return files;
}

// ============================================================================
// Construtores e Destrutor
// ============================================================================

PreprocessorLogger::PreprocessorLogger(LogLevel level)
    : currentLogLevel(level), consoleOutput(true), fileOutput(false),
      bufferingEnabled(false), maxBufferSize(1000), maxFileSize(10 * 1024 * 1024), // 10MB
      logDirectory("./logs") {
    statistics.reset();
}

PreprocessorLogger::~PreprocessorLogger() {
    flushLogBuffer();
    if (outputFile && outputFile->is_open()) {
        outputFile->close();
    }
}

// ============================================================================
// Configuração Básica
// ============================================================================

void PreprocessorLogger::setLogLevel(LogLevel level) {
    currentLogLevel = level;
}

void PreprocessorLogger::setOutputFile(const std::string& filename) {
    currentLogFile = filename;
    outputFile = std::make_unique<std::ofstream>(filename, std::ios::app);
    if (!outputFile->is_open()) {
        handleLogErrors("Não foi possível abrir arquivo de log: " + filename);
        outputFile.reset();
        fileOutput = false;
    } else {
        fileOutput = true;
    }
}

void PreprocessorLogger::setOutputStream(std::ostream* stream) {
    // Para compatibilidade com interface, mas não implementado nesta versão
    // pois usamos arquivo específico
}

void PreprocessorLogger::enableConsoleOutput(bool enable) {
    consoleOutput = enable;
}

void PreprocessorLogger::enableFileOutput(bool enable) {
    fileOutput = enable;
}

// ============================================================================
// Inicialização e Configuração Avançada
// ============================================================================

void PreprocessorLogger::initializeLogger() {
    // Criar diretório de logs se não existir
    try {
        if (!directoryExists(logDirectory)) {
            createDirectory(logDirectory);
        }
        
        // Configurar arquivo de log padrão se não foi especificado
        if (currentLogFile.empty()) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::ostringstream oss;
            oss << logDirectory << "/preprocessor_" 
                << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") 
                << ".log";
            setOutputFile(oss.str());
        }
        
        info("PreprocessorLogger inicializado com sucesso");
    } catch (const std::exception& e) {
        handleLogErrors("Erro na inicialização do logger: " + std::string(e.what()));
    }
}

void PreprocessorLogger::setLogDirectory(const std::string& directory) {
    logDirectory = directory;
}

void PreprocessorLogger::setMaxFileSize(size_t maxSize) {
    maxFileSize = maxSize;
}

void PreprocessorLogger::setMaxBufferSize(size_t maxSize) {
    maxBufferSize = maxSize;
}

void PreprocessorLogger::enableBuffering(bool enable) {
    bufferingEnabled = enable;
    if (!enable) {
        flushLogBuffer();
    }
}

// ============================================================================
// Métodos de Log por Nível
// ============================================================================

void PreprocessorLogger::debug(const std::string& message, const PreprocessorPosition& pos) {
    if (shouldLog(LogLevel::DEBUG)) {
        std::string formattedMessage = formatLogMessage(LogLevel::DEBUG, message, pos);
        updateStatistics(LogLevel::DEBUG);
        
        if (bufferingEnabled) {
            handleLogBuffering(formattedMessage);
        } else {
            if (consoleOutput) {
                writeToConsole(formattedMessage);
            }
            if (fileOutput && outputFile && outputFile->is_open()) {
                writeToFile(formattedMessage);
            }
        }
    }
}

void PreprocessorLogger::info(const std::string& message, const PreprocessorPosition& pos) {
    if (shouldLog(LogLevel::INFO)) {
        std::string formattedMessage = formatLogMessage(LogLevel::INFO, message, pos);
        updateStatistics(LogLevel::INFO);
        
        if (bufferingEnabled) {
            handleLogBuffering(formattedMessage);
        } else {
            if (consoleOutput) {
                writeToConsole(formattedMessage);
            }
            if (fileOutput && outputFile && outputFile->is_open()) {
                writeToFile(formattedMessage);
            }
        }
    }
}

void PreprocessorLogger::warning(const std::string& message, const PreprocessorPosition& pos) {
    if (shouldLog(LogLevel::WARNING)) {
        std::string formattedMessage = formatLogMessage(LogLevel::WARNING, message, pos);
        updateStatistics(LogLevel::WARNING);
        
        if (bufferingEnabled) {
            handleLogBuffering(formattedMessage);
        } else {
            if (consoleOutput) {
                writeToConsole(formattedMessage);
            }
            if (fileOutput && outputFile && outputFile->is_open()) {
                writeToFile(formattedMessage);
            }
        }
    }
}

void PreprocessorLogger::error(const std::string& message, const PreprocessorPosition& pos) {
    if (shouldLog(LogLevel::ERROR)) {
        std::string formattedMessage = formatLogMessage(LogLevel::ERROR, message, pos);
        updateStatistics(LogLevel::ERROR);
        
        if (bufferingEnabled) {
            handleLogBuffering(formattedMessage);
        } else {
            if (consoleOutput) {
                writeToConsole(formattedMessage);
            }
            if (fileOutput && outputFile && outputFile->is_open()) {
                writeToFile(formattedMessage);
            }
        }
    }
}

// ============================================================================
// Métodos de Log Específicos do Preprocessor
// ============================================================================

void PreprocessorLogger::logDirective(const std::string& directiveName, const std::string& content, 
                                     const PreprocessorPosition& pos) {
    if (shouldLog(LogLevel::DEBUG)) {
        std::ostringstream oss;
        oss << "Diretiva processada: #" << directiveName;
        if (!content.empty()) {
            oss << " " << content;
        }
        debug(oss.str(), pos);
    }
}

void PreprocessorLogger::logMacroExpansion(const std::string& macroName, const std::string& expansion, 
                                          const PreprocessorPosition& pos) {
    if (shouldLog(LogLevel::DEBUG)) {
        std::ostringstream oss;
        oss << "Expansão de macro: " << macroName << " -> " << expansion;
        debug(oss.str(), pos);
    }
}

void PreprocessorLogger::logIncludeFile(const std::string& filename, const PreprocessorPosition& pos) {
    if (shouldLog(LogLevel::INFO)) {
        std::ostringstream oss;
        oss << "Incluindo arquivo: " << filename;
        info(oss.str(), pos);
    }
}

void PreprocessorLogger::logConditionalBlock(const std::string& condition, bool result, 
                                            const PreprocessorPosition& pos) {
    if (shouldLog(LogLevel::DEBUG)) {
        std::ostringstream oss;
        oss << "Bloco condicional: " << condition << " = " << (result ? "true" : "false");
        debug(oss.str(), pos);
    }
}

// ============================================================================
// Validação e Filtragem
// ============================================================================

bool PreprocessorLogger::validateLogLevel(LogLevel level) const {
    return level >= LogLevel::DEBUG && level <= LogLevel::ERROR;
}

bool PreprocessorLogger::filterMessage(const std::string& message, LogLevel level) const {
    // Filtros básicos - pode ser expandido conforme necessário
    if (message.empty()) {
        return false;
    }
    
    // Filtrar mensagens muito longas
    if (message.length() > 10000) {
        return false;
    }
    
    return shouldLog(level);
}

void PreprocessorLogger::validateLogFormat(const std::string& message) const {
    // Validação básica do formato da mensagem
    if (message.find('\0') != std::string::npos) {
        throw std::invalid_argument("Mensagem de log contém caracteres nulos");
    }
}

// ============================================================================
// Enriquecimento de Mensagens
// ============================================================================

std::string PreprocessorLogger::addTimestamp(const std::string& message) const {
    return "[" + getCurrentTimestamp() + "] " + message;
}

std::string PreprocessorLogger::addSourceLocation(const std::string& message, 
                                                 const PreprocessorPosition& pos) const {
    if (pos.filename.empty()) {
        return message;
    }
    
    std::ostringstream oss;
    oss << message << " [" << pos.filename << ":" << pos.line << ":" << pos.column << "]";
    return oss.str();
}

// ============================================================================
// Gerenciamento de Buffer
// ============================================================================

void PreprocessorLogger::handleLogBuffering(const std::string& message) {
    messageBuffer.push(message);
    statistics.bufferedMessages++;
    
    // Flush automático se buffer estiver cheio
    if (messageBuffer.size() >= maxBufferSize) {
        flushLogBuffer();
    }
}

void PreprocessorLogger::flushLogBuffer() {
    while (!messageBuffer.empty()) {
        const std::string& message = messageBuffer.front();
        
        if (consoleOutput) {
            writeToConsole(message);
        }
        if (fileOutput && outputFile && outputFile->is_open()) {
            writeToFile(message);
        }
        
        messageBuffer.pop();
        if (statistics.bufferedMessages > 0) {
            statistics.bufferedMessages--;
        }
    }
}

void PreprocessorLogger::clearBuffer() {
    while (!messageBuffer.empty()) {
        messageBuffer.pop();
    }
    statistics.bufferedMessages = 0;
}

// ============================================================================
// Gerenciamento de Arquivos
// ============================================================================

void PreprocessorLogger::rotateLogFile() {
    if (!outputFile || !outputFile->is_open()) {
        return;
    }
    
    // Fechar arquivo atual
    outputFile->close();
    
    // Gerar nome do arquivo rotacionado
    std::string rotatedFileName = generateRotatedFileName();
    
    try {
        // Renomear arquivo atual
        if (std::rename(currentLogFile.c_str(), rotatedFileName.c_str()) == 0) {
            // Reabrir arquivo com nome original
            outputFile = std::make_unique<std::ofstream>(currentLogFile, std::ios::app);
            
            if (outputFile->is_open()) {
                statistics.fileRotations++;
                info("Arquivo de log rotacionado: " + rotatedFileName);
            } else {
                handleLogErrors("Erro ao reabrir arquivo de log após rotação");
            }
        } else {
            handleLogErrors("Erro ao renomear arquivo de log");
        }
    } catch (const std::exception& e) {
        handleLogErrors("Erro na rotação do arquivo de log: " + std::string(e.what()));
    }
}

void PreprocessorLogger::compressOldLogs() {
    // Implementação básica - pode ser expandida com compressão real
    try {
        auto logFiles = getLogFiles();
        for (const auto& file : logFiles) {
            if (file != currentLogFile && file.find(".compressed") == std::string::npos) {
                std::string compressedName = file + ".compressed";
                std::rename(file.c_str(), compressedName.c_str());
            }
        }
    } catch (const std::exception& e) {
        handleLogErrors("Erro na compressão de logs antigos: " + std::string(e.what()));
    }
}

std::vector<std::string> PreprocessorLogger::getLogFiles() const {
    std::vector<std::string> logFiles;
    
    try {
        if (directoryExists(logDirectory)) {
            logFiles = listDirectory(logDirectory);
        }
    } catch (const std::exception& e) {
        // Silenciosamente ignora erros de listagem
    }
    
    return logFiles;
}

void PreprocessorLogger::cleanupOldLogs(size_t maxFiles) {
    try {
        auto logFiles = getLogFiles();
        if (logFiles.size() <= maxFiles) {
            return;
        }
        
        // Ordenar por nome (aproximação da data de modificação)
        std::sort(logFiles.begin(), logFiles.end());
        
        // Remover arquivos mais antigos
        size_t filesToRemove = logFiles.size() - maxFiles;
        for (size_t i = 0; i < filesToRemove; ++i) {
            if (logFiles[i] != currentLogFile) {
                std::remove(logFiles[i].c_str());
            }
        }
    } catch (const std::exception& e) {
        handleLogErrors("Erro na limpeza de logs antigos: " + std::string(e.what()));
    }
}

// ============================================================================
// Relatórios e Estatísticas
// ============================================================================

std::string PreprocessorLogger::generateLogReport() const {
    std::ostringstream oss;
    oss << "=== Relatório do PreprocessorLogger ===\n";
    oss << "Nível atual: " << logLevelToString(currentLogLevel) << "\n";
    oss << "Saída no console: " << (consoleOutput ? "Habilitada" : "Desabilitada") << "\n";
    oss << "Saída em arquivo: " << (fileOutput ? "Habilitada" : "Desabilitada") << "\n";
    oss << "Buffering: " << (bufferingEnabled ? "Habilitado" : "Desabilitado") << "\n";
    oss << "\n=== Estatísticas ===\n";
    oss << "Total de mensagens: " << statistics.totalMessages << "\n";
    oss << "DEBUG: " << statistics.debugCount << "\n";
    oss << "INFO: " << statistics.infoCount << "\n";
    oss << "WARNING: " << statistics.warningCount << "\n";
    oss << "ERROR: " << statistics.errorCount << "\n";
    oss << "Mensagens em buffer: " << statistics.bufferedMessages << "\n";
    oss << "Rotações de arquivo: " << statistics.fileRotations << "\n";
    
    if (!currentLogFile.empty()) {
        oss << "\n=== Arquivo de Log ===\n";
        oss << "Arquivo atual: " << currentLogFile << "\n";
        oss << "Diretório: " << logDirectory << "\n";
        
        try {
            if (fileExists(currentLogFile)) {
                auto size = getFileSize(currentLogFile);
                oss << "Tamanho do arquivo: " << size << " bytes\n";
            }
        } catch (...) {
            oss << "Erro ao obter informações do arquivo\n";
        }
    }
    
    return oss.str();
}

void PreprocessorLogger::resetStatistics() {
    statistics.reset();
}

// ============================================================================
// Tratamento de Erros e Otimização
// ============================================================================

void PreprocessorLogger::handleLogErrors(const std::string& errorMessage) {
    // Log de erro interno - sempre vai para stderr
    std::cerr << "[PreprocessorLogger ERROR] " << errorMessage << std::endl;
}

void PreprocessorLogger::optimizeLogPerformance() {
    // Otimizações básicas
    if (bufferingEnabled && messageBuffer.size() < maxBufferSize / 2) {
        // Buffer está subutilizado, pode reduzir frequência de flush
    }
    
    // Verificar se precisa rotacionar arquivo
    checkFileRotation();
    
    // Limpeza automática de logs antigos
    cleanupOldLogs();
}

// ============================================================================
// Métodos Privados Auxiliares
// ============================================================================

std::string PreprocessorLogger::formatLogMessage(LogLevel level, const std::string& message, 
                                                const PreprocessorPosition& pos) const {
    std::ostringstream oss;
    oss << "[" << getCurrentTimestamp() << "] ";
    oss << "[" << logLevelToString(level) << "] ";
    
    if (!pos.filename.empty()) {
        oss << "[" << pos.filename << ":" << pos.line << ":" << pos.column << "] ";
    }
    
    oss << message;
    return oss.str();
}

std::string PreprocessorLogger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

void PreprocessorLogger::writeToFile(const std::string& message) {
    if (outputFile && outputFile->is_open()) {
        *outputFile << message << std::endl;
        outputFile->flush();
        
        // Verificar se precisa rotacionar
        checkFileRotation();
    }
}

void PreprocessorLogger::writeToConsole(const std::string& message) {
    // Enable console output for debugging
    std::cout << message << std::endl;
}

std::string PreprocessorLogger::logLevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

bool PreprocessorLogger::shouldLog(LogLevel level) const {
    return level >= currentLogLevel;
}

void PreprocessorLogger::updateStatistics(LogLevel level) {
    statistics.totalMessages++;
    
    switch (level) {
        case LogLevel::DEBUG:
            statistics.debugCount++;
            break;
        case LogLevel::INFO:
            statistics.infoCount++;
            break;
        case LogLevel::WARNING:
            statistics.warningCount++;
            break;
        case LogLevel::ERROR:
            statistics.errorCount++;
            break;
    }
}

void PreprocessorLogger::checkFileRotation() {
    if (!outputFile || !outputFile->is_open() || currentLogFile.empty()) {
        return;
    }
    
    try {
        if (fileExists(currentLogFile)) {
            auto size = getFileSize(currentLogFile);
            if (size >= maxFileSize) {
                rotateLogFile();
            }
        }
    } catch (const std::exception& e) {
        handleLogErrors("Erro ao verificar tamanho do arquivo: " + std::string(e.what()));
    }
}

std::string PreprocessorLogger::generateRotatedFileName() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << currentLogFile << "." 
        << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    return oss.str();
}

// ============================================================================
// Funções Utilitárias Globais
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

std::string formatPreprocessorPosition(const PreprocessorPosition& pos) {
    if (pos.filename.empty()) {
        return "<unknown>";
    }
    
    std::ostringstream oss;
    oss << pos.filename << ":" << pos.line << ":" << pos.column;
    return oss.str();
}

bool isValidLogLevel(LogLevel level) {
    return level >= LogLevel::DEBUG && level <= LogLevel::ERROR;
}

} // namespace Preprocessor