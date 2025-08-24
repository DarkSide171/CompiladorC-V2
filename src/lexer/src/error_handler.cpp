#include "../include/error_handler.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

namespace Lexer {

// ============================================================================
// CompilerError - Implementação
// ============================================================================

CompilerError::CompilerError() 
    : type(ErrorType::INTERNAL_ERROR), message(""), position(), filename(""), 
      context(""), suggestion(""), is_warning(false) {}

CompilerError::CompilerError(ErrorType type, const std::string& message, const Position& pos)
    : type(type), message(message), position(pos), filename(""), 
      context(""), suggestion(""), is_warning(isWarningType(type)) {}

CompilerError::CompilerError(ErrorType type, const std::string& message, const Position& pos, 
                            const std::string& filename)
    : type(type), message(message), position(pos), filename(filename), 
      context(""), suggestion(""), is_warning(isWarningType(type)) {}

CompilerError::CompilerError(ErrorType type, const std::string& message, const Position& pos,
                            const std::string& filename, const std::string& context)
    : type(type), message(message), position(pos), filename(filename), 
      context(context), suggestion(""), is_warning(isWarningType(type)) {}

bool CompilerError::operator==(const CompilerError& other) const {
    return type == other.type && message == other.message && 
           position == other.position && filename == other.filename;
}

bool CompilerError::operator!=(const CompilerError& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const CompilerError& error) {
    os << formatErrorMessage(error);
    return os;
}

// ============================================================================
// ErrorHandler - Implementação
// ============================================================================

ErrorHandler::ErrorHandler(int max_errors) 
    : max_errors_(max_errors), recovery_mode_(RecoveryMode::CONTINUE), 
      current_filename_(""), log_file_(nullptr), verbose_mode_(false) {
    errors_.reserve(max_errors_);
    warnings_.reserve(max_errors_);
}

ErrorHandler::~ErrorHandler() {
    if (log_file_ && log_file_->is_open()) {
        log_file_->close();
    }
}

// Métodos principais de relatório
void ErrorHandler::reportError(ErrorType type, const std::string& message, const Position& pos) {
    reportError(type, message, pos, current_filename_);
}

void ErrorHandler::reportError(ErrorType type, const std::string& message, const Position& pos,
                              const std::string& filename) {
    if (errors_.size() >= static_cast<size_t>(max_errors_)) {
        return; // Limite de erros atingido
    }
    
    CompilerError error(type, message, pos, filename);
    
    // Extrair contexto se possível
    if (!filename.empty()) {
        error.context = getErrorContext(filename, pos);
    }
    
    // Gerar sugestão
    error.suggestion = generateSuggestion(type, error.context);
    
    errors_.push_back(error);
    
    // Log do erro
    if (log_file_) {
        logError(error);
    }
    
    // Exibir erro em modo verbose
    if (verbose_mode_) {
        std::cerr << formatError(error) << std::endl;
    }
}

void ErrorHandler::reportWarning(ErrorType type, const std::string& message, const Position& pos) {
    reportWarning(type, message, pos, current_filename_);
}

void ErrorHandler::reportWarning(ErrorType type, const std::string& message, const Position& pos,
                                const std::string& filename) {
    CompilerError warning(type, message, pos, filename);
    warning.is_warning = true;
    
    // Extrair contexto se possível
    if (!filename.empty()) {
        warning.context = getErrorContext(filename, pos);
    }
    
    // Gerar sugestão
    warning.suggestion = generateSuggestion(type, warning.context);
    
    warnings_.push_back(warning);
    
    // Log do warning
    if (log_file_) {
        logWarning(warning);
    }
    
    // Exibir warning em modo verbose
    if (verbose_mode_) {
        std::cerr << formatWarning(warning) << std::endl;
    }
}

// Métodos de consulta
bool ErrorHandler::hasErrors() const {
    return !errors_.empty();
}

bool ErrorHandler::hasWarnings() const {
    return !warnings_.empty();
}

int ErrorHandler::getErrorCount() const {
    return static_cast<int>(errors_.size());
}

int ErrorHandler::getWarningCount() const {
    return static_cast<int>(warnings_.size());
}

int ErrorHandler::getTotalIssueCount() const {
    return getErrorCount() + getWarningCount();
}

// Acesso aos erros e warnings
const std::vector<CompilerError>& ErrorHandler::getErrors() const {
    return errors_;
}

const std::vector<CompilerError>& ErrorHandler::getWarnings() const {
    return warnings_;
}

std::vector<CompilerError> ErrorHandler::getAllIssues() const {
    std::vector<CompilerError> all_issues;
    all_issues.reserve(errors_.size() + warnings_.size());
    
    all_issues.insert(all_issues.end(), errors_.begin(), errors_.end());
    all_issues.insert(all_issues.end(), warnings_.begin(), warnings_.end());
    
    // Ordenar por posição
    std::sort(all_issues.begin(), all_issues.end(), 
              [](const CompilerError& a, const CompilerError& b) {
                  if (a.filename != b.filename) return a.filename < b.filename;
                  if (a.position.line != b.position.line) return a.position.line < b.position.line;
                  return a.position.column < b.position.column;
              });
    
    return all_issues;
}

// Métodos de formatação e exibição
void ErrorHandler::printErrorSummary() const {
    printErrorSummary(std::cout);
}

void ErrorHandler::printErrorSummary(std::ostream& os) const {
    os << "\n=== Resumo de Erros e Warnings ===\n";
    os << "Erros: " << getErrorCount() << "\n";
    os << "Warnings: " << getWarningCount() << "\n";
    os << "Total: " << getTotalIssueCount() << "\n";
    
    if (hasErrors()) {
        os << "\n❌ Compilação falhou devido a erros.\n";
    } else if (hasWarnings()) {
        os << "\n⚠️  Compilação concluída com warnings.\n";
    } else {
        os << "\n✅ Nenhum erro ou warning encontrado.\n";
    }
}

void ErrorHandler::printDetailedReport() const {
    printDetailedReport(std::cout);
}

void ErrorHandler::printDetailedReport(std::ostream& os) const {
    auto all_issues = getAllIssues();
    
    os << "\n=== Relatório Detalhado ===\n";
    
    for (const auto& issue : all_issues) {
        if (issue.is_warning) {
            os << formatWarning(issue) << "\n";
        } else {
            os << formatError(issue) << "\n";
        }
        os << "\n";
    }
    
    printErrorSummary(os);
}

// Configuração
void ErrorHandler::setRecoveryMode(RecoveryMode mode) {
    recovery_mode_ = mode;
}

RecoveryMode ErrorHandler::getRecoveryMode() const {
    return recovery_mode_;
}

void ErrorHandler::setMaxErrors(int max_errors) {
    max_errors_ = max_errors;
}

int ErrorHandler::getMaxErrors() const {
    return max_errors_;
}

void ErrorHandler::setCurrentFilename(const std::string& filename) {
    current_filename_ = filename;
}

const std::string& ErrorHandler::getCurrentFilename() const {
    return current_filename_;
}

void ErrorHandler::setVerboseMode(bool verbose) {
    verbose_mode_ = verbose;
}

bool ErrorHandler::isVerboseMode() const {
    return verbose_mode_;
}

// Métodos de formatação
std::string ErrorHandler::formatError(const CompilerError& error) const {
    std::ostringstream oss;
    
    oss << "❌ ERRO: ";
    
    if (!error.filename.empty()) {
        oss << error.filename << ":";
    }
    
    oss << error.position.line << ":" << error.position.column << ": ";
    oss << "[" << errorTypeToString(error.type) << "] ";
    oss << error.message;
    
    if (!error.context.empty()) {
        oss << "\n  │ " << error.context;
        oss << "\n  │ " << std::string(error.position.column - 1, ' ') << "^";
    }
    
    if (!error.suggestion.empty()) {
        oss << "\n  💡 Sugestão: " << error.suggestion;
    }
    
    return oss.str();
}

std::string ErrorHandler::formatWarning(const CompilerError& warning) const {
    std::ostringstream oss;
    
    oss << "⚠️  WARNING: ";
    
    if (!warning.filename.empty()) {
        oss << warning.filename << ":";
    }
    
    oss << warning.position.line << ":" << warning.position.column << ": ";
    oss << "[" << errorTypeToString(warning.type) << "] ";
    oss << warning.message;
    
    if (!warning.context.empty()) {
        oss << "\n  │ " << warning.context;
        oss << "\n  │ " << std::string(warning.position.column - 1, ' ') << "^";
    }
    
    if (!warning.suggestion.empty()) {
        oss << "\n  💡 Sugestão: " << warning.suggestion;
    }
    
    return oss.str();
}

std::string ErrorHandler::getErrorContext(const std::string& filename, const Position& pos) const {
    return extractContextLine(filename, pos.line);
}

std::string ErrorHandler::generateSuggestion(ErrorType type, const std::string& context) const {
    static const std::unordered_map<ErrorType, std::string> suggestions = {
        {ErrorType::UNTERMINATED_STRING, "Adicione aspas duplas de fechamento"},
        {ErrorType::UNTERMINATED_CHAR, "Adicione aspas simples de fechamento"},
        {ErrorType::UNTERMINATED_COMMENT, "Adicione */ para fechar o comentário"},
        {ErrorType::INVALID_ESCAPE_SEQUENCE, "Use uma sequência de escape válida (\\n, \\t, \\\\, etc.)"},
        {ErrorType::INVALID_CHARACTER, "Remova ou substitua o caractere inválido"},
        {ErrorType::INVALID_NUMBER_FORMAT, "Verifique o formato do número (ex: 123, 123.45, 0x1A)"},
        {ErrorType::IDENTIFIER_TOO_LONG, "Reduza o tamanho do identificador"},
        {ErrorType::UNEXPECTED_TOKEN, "Verifique a sintaxe esperada nesta posição"},
        {ErrorType::MISSING_TOKEN, "Adicione o token esperado"},
        {ErrorType::UNDEFINED_IDENTIFIER, "Declare a variável ou função antes de usar"},
        {ErrorType::REDEFINED_IDENTIFIER, "Use um nome diferente ou remova a declaração duplicada"},
        {ErrorType::TYPE_MISMATCH, "Verifique os tipos das variáveis e expressões"},
        {ErrorType::UNUSED_VARIABLE, "Remova a variável ou use-a no código"},
        {ErrorType::DEPRECATED_FEATURE, "Use a alternativa moderna recomendada"},
        {ErrorType::IMPLICIT_CONVERSION, "Adicione cast explícito se necessário"}
    };
    
    auto it = suggestions.find(type);
    return (it != suggestions.end()) ? it->second : "Consulte a documentação para mais informações";
}

// Sistema de log
void ErrorHandler::enableLogging(const std::string& log_filename) {
    log_file_ = std::make_shared<std::ofstream>(log_filename, std::ios::app);
    if (log_file_->is_open()) {
        *log_file_ << "\n=== Nova sessão de compilação ===\n";
        *log_file_ << "Timestamp: " << std::time(nullptr) << "\n\n";
    }
}

void ErrorHandler::disableLogging() {
    if (log_file_ && log_file_->is_open()) {
        log_file_->close();
    }
    log_file_.reset();
}

void ErrorHandler::logError(const CompilerError& error) {
    if (log_file_ && log_file_->is_open()) {
        *log_file_ << "[ERROR] " << formatErrorMessage(error) << "\n";
        log_file_->flush();
    }
}

void ErrorHandler::logWarning(const CompilerError& warning) {
    if (log_file_ && log_file_->is_open()) {
        *log_file_ << "[WARNING] " << formatErrorMessage(warning) << "\n";
        log_file_->flush();
    }
}

// Métodos utilitários
void ErrorHandler::clear() {
    errors_.clear();
    warnings_.clear();
}

bool ErrorHandler::shouldStop() const {
    if (recovery_mode_ == RecoveryMode::PANIC && hasErrors()) {
        return true;
    }
    return errors_.size() >= static_cast<size_t>(max_errors_);
}

void ErrorHandler::reset() {
    clear();
    recovery_mode_ = RecoveryMode::CONTINUE;
    current_filename_.clear();
    verbose_mode_ = false;
    disableLogging();
}

// Métodos auxiliares privados
std::string ErrorHandler::getErrorTypeString(ErrorType type) const {
    return errorTypeToString(type);
}

std::string ErrorHandler::extractContextLine(const std::string& filename, int line) const {
    if (filename.empty() || line <= 0) {
        return "";
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::string context_line;
    int current_line = 1;
    
    while (std::getline(file, context_line) && current_line < line) {
        current_line++;
    }
    
    if (current_line == line) {
        // Remover tabs e substituir por espaços para melhor formatação
        std::replace(context_line.begin(), context_line.end(), '\t', ' ');
        return context_line;
    }
    
    return "";
}

// Métodos estáticos utilitários
std::string ErrorHandler::errorTypeToString(ErrorType type) {
    return ::Lexer::errorTypeToString(type);
}

std::string ErrorHandler::recoveryModeToString(RecoveryMode mode) {
    return ::Lexer::recoveryModeToString(mode);
}

bool ErrorHandler::isWarningType(ErrorType type) {
    return ::Lexer::isWarningType(type);
}

bool ErrorHandler::isFatalError(ErrorType type) {
    return ::Lexer::isFatalError(type);
}

// ============================================================================
// Funções utilitárias globais
// ============================================================================

std::string errorTypeToString(ErrorType type) {
    static const std::unordered_map<ErrorType, std::string> error_names = {
        // Erros léxicos
        {ErrorType::INVALID_CHARACTER, "INVALID_CHARACTER"},
        {ErrorType::UNTERMINATED_STRING, "UNTERMINATED_STRING"},
        {ErrorType::UNTERMINATED_CHAR, "UNTERMINATED_CHAR"},
        {ErrorType::UNTERMINATED_COMMENT, "UNTERMINATED_COMMENT"},
        {ErrorType::INVALID_ESCAPE_SEQUENCE, "INVALID_ESCAPE_SEQUENCE"},
        {ErrorType::INVALID_NUMBER_FORMAT, "INVALID_NUMBER_FORMAT"},
        {ErrorType::IDENTIFIER_TOO_LONG, "IDENTIFIER_TOO_LONG"},
        
        // Erros de sintaxe
        {ErrorType::UNEXPECTED_TOKEN, "UNEXPECTED_TOKEN"},
        {ErrorType::MISSING_TOKEN, "MISSING_TOKEN"},
        {ErrorType::INVALID_EXPRESSION, "INVALID_EXPRESSION"},
        {ErrorType::INVALID_DECLARATION, "INVALID_DECLARATION"},
        
        // Erros semânticos
        {ErrorType::UNDEFINED_IDENTIFIER, "UNDEFINED_IDENTIFIER"},
        {ErrorType::REDEFINED_IDENTIFIER, "REDEFINED_IDENTIFIER"},
        {ErrorType::TYPE_MISMATCH, "TYPE_MISMATCH"},
        {ErrorType::INVALID_OPERATION, "INVALID_OPERATION"},
        
        // Erros de sistema
        {ErrorType::FILE_NOT_FOUND, "FILE_NOT_FOUND"},
        {ErrorType::MEMORY_ERROR, "MEMORY_ERROR"},
        {ErrorType::INTERNAL_ERROR, "INTERNAL_ERROR"},
        
        // Warnings
        {ErrorType::UNUSED_VARIABLE, "UNUSED_VARIABLE"},
        {ErrorType::DEPRECATED_FEATURE, "DEPRECATED_FEATURE"},
        {ErrorType::IMPLICIT_CONVERSION, "IMPLICIT_CONVERSION"},
        {ErrorType::UNREACHABLE_CODE, "UNREACHABLE_CODE"}
    };
    
    auto it = error_names.find(type);
    return (it != error_names.end()) ? it->second : "UNKNOWN_ERROR_TYPE";
}

std::string recoveryModeToString(RecoveryMode mode) {
    static const std::unordered_map<RecoveryMode, std::string> mode_names = {
        {RecoveryMode::PANIC, "PANIC"},
        {RecoveryMode::SYNCHRONIZE, "SYNCHRONIZE"},
        {RecoveryMode::CONTINUE, "CONTINUE"},
        {RecoveryMode::INTERACTIVE, "INTERACTIVE"}
    };
    
    auto it = mode_names.find(mode);
    return (it != mode_names.end()) ? it->second : "UNKNOWN_RECOVERY_MODE";
}

bool isWarningType(ErrorType type) {
    static const std::unordered_set<ErrorType> warning_types = {
        ErrorType::UNUSED_VARIABLE,
        ErrorType::DEPRECATED_FEATURE,
        ErrorType::IMPLICIT_CONVERSION,
        ErrorType::UNREACHABLE_CODE
    };
    
    return warning_types.find(type) != warning_types.end();
}

bool isFatalError(ErrorType type) {
    static const std::unordered_set<ErrorType> fatal_errors = {
        ErrorType::MEMORY_ERROR,
        ErrorType::INTERNAL_ERROR,
        ErrorType::FILE_NOT_FOUND
    };
    
    return fatal_errors.find(type) != fatal_errors.end();
}

std::string formatPosition(const Position& pos) {
    std::ostringstream oss;
    oss << pos.line << ":" << pos.column;
    return oss.str();
}

std::string formatErrorMessage(const CompilerError& error) {
    std::ostringstream oss;
    
    if (!error.filename.empty()) {
        oss << error.filename << ":";
    }
    
    oss << formatPosition(error.position) << ": ";
    oss << "[" << errorTypeToString(error.type) << "] ";
    oss << error.message;
    
    return oss.str();
}

} // namespace Lexer