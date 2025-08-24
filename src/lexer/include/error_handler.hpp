#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include "token.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>

namespace Lexer {

// Enumeração dos tipos de erro
enum class ErrorType {
    // Erros léxicos
    INVALID_CHARACTER,
    UNTERMINATED_STRING,
    UNTERMINATED_CHAR,
    UNTERMINATED_COMMENT,
    INVALID_ESCAPE_SEQUENCE,
    INVALID_NUMBER_FORMAT,
    IDENTIFIER_TOO_LONG,
    
    // Erros de sintaxe
    UNEXPECTED_TOKEN,
    MISSING_TOKEN,
    INVALID_EXPRESSION,
    INVALID_DECLARATION,
    
    // Erros semânticos
    UNDEFINED_IDENTIFIER,
    REDEFINED_IDENTIFIER,
    TYPE_MISMATCH,
    INVALID_OPERATION,
    
    // Erros de sistema
    FILE_NOT_FOUND,
    MEMORY_ERROR,
    INTERNAL_ERROR,
    
    // Warnings
    UNUSED_VARIABLE,
    DEPRECATED_FEATURE,
    IMPLICIT_CONVERSION,
    UNREACHABLE_CODE
};

// Modos de recuperação de erro
enum class RecoveryMode {
    PANIC,          // Para na primeira ocorrência de erro
    SYNCHRONIZE,    // Tenta sincronizar com próximo token válido
    CONTINUE,       // Continua processamento ignorando erro
    INTERACTIVE     // Modo interativo para correção
};

// Estrutura que representa um erro do compilador
struct CompilerError {
    ErrorType type;
    std::string message;
    Position position;
    std::string filename;
    std::string context;        // Linha de código onde ocorreu o erro
    std::string suggestion;     // Sugestão de correção
    bool is_warning;
    
    // Construtores
    CompilerError();
    CompilerError(ErrorType type, const std::string& message, const Position& pos);
    CompilerError(ErrorType type, const std::string& message, const Position& pos, 
                  const std::string& filename);
    CompilerError(ErrorType type, const std::string& message, const Position& pos,
                  const std::string& filename, const std::string& context);
    
    // Operadores de comparação
    bool operator==(const CompilerError& other) const;
    bool operator!=(const CompilerError& other) const;
    
    // Operador de stream
    friend std::ostream& operator<<(std::ostream& os, const CompilerError& error);
};

// Classe principal para tratamento de erros
class ErrorHandler {
private:
    std::vector<CompilerError> errors_;
    std::vector<CompilerError> warnings_;
    int max_errors_;
    RecoveryMode recovery_mode_;
    std::string current_filename_;
    std::shared_ptr<std::ofstream> log_file_;
    bool verbose_mode_;
    
    // Métodos auxiliares privados
    std::string getErrorTypeString(ErrorType type) const;
    std::string extractContextLine(const std::string& filename, int line) const;
    
public:
    // Construtor
    explicit ErrorHandler(int max_errors = 100);
    
    // Destrutor
    ~ErrorHandler();
    
    // Métodos principais de relatório
    void reportError(ErrorType type, const std::string& message, const Position& pos);
    void reportError(ErrorType type, const std::string& message, const Position& pos,
                    const std::string& filename);
    void reportWarning(ErrorType type, const std::string& message, const Position& pos);
    void reportWarning(ErrorType type, const std::string& message, const Position& pos,
                      const std::string& filename);
    
    // Métodos de consulta
    bool hasErrors() const;
    bool hasWarnings() const;
    int getErrorCount() const;
    int getWarningCount() const;
    int getTotalIssueCount() const;
    
    // Acesso aos erros e warnings
    const std::vector<CompilerError>& getErrors() const;
    const std::vector<CompilerError>& getWarnings() const;
    std::vector<CompilerError> getAllIssues() const;
    
    // Métodos de formatação e exibição
    void printErrorSummary() const;
    void printErrorSummary(std::ostream& os) const;
    void printDetailedReport() const;
    void printDetailedReport(std::ostream& os) const;
    
    // Configuração
    void setRecoveryMode(RecoveryMode mode);
    RecoveryMode getRecoveryMode() const;
    void setMaxErrors(int max_errors);
    int getMaxErrors() const;
    void setCurrentFilename(const std::string& filename);
    const std::string& getCurrentFilename() const;
    void setVerboseMode(bool verbose);
    bool isVerboseMode() const;
    
    // Métodos de formatação
    std::string formatError(const CompilerError& error) const;
    std::string formatWarning(const CompilerError& warning) const;
    std::string getErrorContext(const std::string& filename, const Position& pos) const;
    std::string generateSuggestion(ErrorType type, const std::string& context) const;
    
    // Sistema de log
    void enableLogging(const std::string& log_filename);
    void disableLogging();
    void logError(const CompilerError& error);
    void logWarning(const CompilerError& warning);
    
    // Métodos utilitários
    void clear();
    bool shouldStop() const;
    void reset();
    
    // Métodos estáticos utilitários
    static std::string errorTypeToString(ErrorType type);
    static std::string recoveryModeToString(RecoveryMode mode);
    static bool isWarningType(ErrorType type);
    static bool isFatalError(ErrorType type);
};

// Funções utilitárias globais
std::string errorTypeToString(ErrorType type);
std::string recoveryModeToString(RecoveryMode mode);
bool isWarningType(ErrorType type);
bool isFatalError(ErrorType type);
std::string formatPosition(const Position& pos);
std::string formatErrorMessage(const CompilerError& error);

} // namespace Lexer

#endif // ERROR_HANDLER_HPP