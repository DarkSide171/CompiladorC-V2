#ifndef ERROR_RECOVERY_HPP
#define ERROR_RECOVERY_HPP

#include "parser_types.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

namespace Parser {

// Forward declarations
class ParserStateManager;

// Parse error class
class ParseError {
public:
    ParseError(ErrorSeverity severity, const std::string& message, 
               const SourceRange& range = SourceRange())
        : errorSeverity(severity), errorMessage(message), sourceRange(range) {}
    
    virtual ~ParseError() = default;
    
    // Accessors
    ErrorSeverity getSeverity() const { return errorSeverity; }
    const std::string& getMessage() const { return errorMessage; }
    const SourceRange& getSourceRange() const { return sourceRange; }
    
    void setSeverity(ErrorSeverity severity) { errorSeverity = severity; }
    void setMessage(const std::string& message) { errorMessage = message; }
    void setSourceRange(const SourceRange& range) { sourceRange = range; }
    
    // Error formatting
    virtual std::string format() const;
    virtual std::string getErrorCode() const { return "PARSE_ERROR"; }
    
    // Additional context
    void addNote(const std::string& note) { notes.push_back(note); }
    const std::vector<std::string>& getNotes() const { return notes; }
    
    void addSuggestion(const std::string& suggestion) { suggestions.push_back(suggestion); }
    const std::vector<std::string>& getSuggestions() const { return suggestions; }
    
protected:
    ErrorSeverity errorSeverity;
    std::string errorMessage;
    SourceRange sourceRange;
    std::vector<std::string> notes;
    std::vector<std::string> suggestions;
};

// Specific error types
class SyntaxError : public ParseError {
public:
    SyntaxError(const std::string& message, const SourceRange& range = SourceRange())
        : ParseError(ErrorSeverity::ERROR, message, range) {}
    
    std::string getErrorCode() const override { return "SYNTAX_ERROR"; }
};

class UnexpectedTokenError : public SyntaxError {
public:
    UnexpectedTokenError(const std::string& expected, const std::string& actual, 
                        const SourceRange& range = SourceRange())
        : SyntaxError("Expected '" + expected + "' but found '" + actual + "'", range),
          expectedToken(expected), actualToken(actual) {}
    
    const std::string& getExpected() const { return expectedToken; }
    const std::string& getActual() const { return actualToken; }
    
    std::string getErrorCode() const override { return "UNEXPECTED_TOKEN"; }
    
private:
    std::string expectedToken;
    std::string actualToken;
};

class MissingTokenError : public SyntaxError {
public:
    MissingTokenError(const std::string& missing, const SourceRange& range = SourceRange())
        : SyntaxError("Missing '" + missing + "'", range), missingToken(missing) {}
    
    const std::string& getMissingToken() const { return missingToken; }
    
    std::string getErrorCode() const override { return "MISSING_TOKEN"; }
    
private:
    std::string missingToken;
};

class SemanticError : public ParseError {
public:
    SemanticError(const std::string& message, const SourceRange& range = SourceRange())
        : ParseError(ErrorSeverity::ERROR, message, range) {}
    
    std::string getErrorCode() const override { return "SEMANTIC_ERROR"; }
};

// Recovery strategy interface
class RecoveryStrategy {
public:
    virtual ~RecoveryStrategy() = default;
    
    // Attempt to recover from an error
    virtual bool recover(ParserStateManager& state, const std::vector<Token>& tokens, 
                        size_t& currentPos) = 0;
    
    // Get strategy name for debugging
    virtual std::string getName() const = 0;
    
    // Check if this strategy can handle the current error
    virtual bool canHandle(const ParseError& error, const std::vector<Token>& tokens, 
                          size_t currentPos) const = 0;
};

// Panic mode recovery - skip tokens until synchronization point
class PanicModeRecovery : public RecoveryStrategy {
public:
    PanicModeRecovery();
    
    bool recover(ParserStateManager& state, const std::vector<Token>& tokens, 
                size_t& currentPos) override;
    
    std::string getName() const override { return "PanicMode"; }
    
    bool canHandle(const ParseError& error, const std::vector<Token>& tokens, 
                  size_t currentPos) const override;
    
    // Configuration
    void addSyncToken(const std::string& token) { syncTokens.insert(token); }
    void removeSyncToken(const std::string& token) { syncTokens.erase(token); }
    void setSyncTokens(const std::unordered_set<std::string>& tokens) { syncTokens = tokens; }
    
private:
    std::unordered_set<std::string> syncTokens;
    void initializeDefaultSyncTokens();
};

// Phrase level recovery - insert/delete tokens locally
class PhraseLevelRecovery : public RecoveryStrategy {
public:
    PhraseLevelRecovery();
    
    bool recover(ParserStateManager& state, const std::vector<Token>& tokens, 
                size_t& currentPos) override;
    
    std::string getName() const override { return "PhraseLevel"; }
    
    bool canHandle(const ParseError& error, const std::vector<Token>& tokens, 
                  size_t currentPos) const override;
    
private:
    std::unordered_set<std::string> insertableTokens;
    std::unordered_set<std::string> deletableTokens;
    
    void initializeDefaultTokens();
    bool tryInsertToken(const std::string& token, ParserStateManager& state, 
                       const std::vector<Token>& tokens, size_t& currentPos);
    bool tryDeleteToken(ParserStateManager& state, const std::vector<Token>& tokens, 
                       size_t& currentPos);
};

// Error production recovery - use error productions in grammar
class ErrorProductionRecovery : public RecoveryStrategy {
public:
    ErrorProductionRecovery();
    
    bool recover(ParserStateManager& state, const std::vector<Token>& tokens, 
                size_t& currentPos) override;
    
    std::string getName() const override { return "ErrorProduction"; }
    
    bool canHandle(const ParseError& error, const std::vector<Token>& tokens, 
                  size_t currentPos) const override;
    
private:
    // Error productions would be defined here
    // This is a placeholder for future implementation
};

// Main error recovery manager
class ErrorRecoveryManager {
public:
    ErrorRecoveryManager();
    ~ErrorRecoveryManager() = default;
    
    // Strategy management
    void addStrategy(std::unique_ptr<RecoveryStrategy> strategy);
    void removeStrategy(const std::string& strategyName);
    void clearStrategies();
    
    // Recovery execution
    bool attemptRecovery(ParseError& error, ParserStateManager& state, 
                        const std::vector<Token>& tokens, size_t& currentPos);
    
    // Main recovery method
    bool recoverFromError(ParseError& error, ParserStateManager& state, 
                         const std::vector<Token>& tokens, size_t& currentPos);
    
    // Specific recovery strategies
    bool synchronizeToToken(Lexer::TokenType targetType, ParserStateManager& state, 
                           const std::vector<Token>& tokens, size_t& currentPos);
    bool skipToSemicolon(ParserStateManager& state, const std::vector<Token>& tokens, 
                        size_t& currentPos);
    bool recoverFromMissingToken(const std::string& expectedToken, ParserStateManager& state, 
                                const std::vector<Token>& tokens, size_t& currentPos);
    bool recoverFromUnexpectedToken(const std::string& unexpectedToken, 
                                   const std::string& expectedToken, ParserStateManager& state, 
                                   const std::vector<Token>& tokens, size_t& currentPos);
    bool recoverFromMissingBrace(char braceType, ParserStateManager& state, 
                                const std::vector<Token>& tokens, size_t& currentPos);
    bool recoverFromExpressionError(ParserStateManager& state, const std::vector<Token>& tokens, 
                                   size_t& currentPos);
    
    // Helper methods for recovery strategies
    bool isSkippableToken(const std::string& token) const;
    bool canInsertToken(const std::string& token, const std::vector<Token>& tokens, size_t currentPos) const;
    bool canReplaceToken(const std::string& actual, const std::string& expected) const;
    bool isIdentifierOrLiteral(const std::string& token) const;
    bool isOperator(const std::string& token) const;
    
    // Configuration
    void setMaxRecoveryAttempts(size_t maxAttempts) { maxRecoveryAttempts = maxAttempts; }
    size_t getMaxRecoveryAttempts() const { return maxRecoveryAttempts; }
    
    void setRecoveryEnabled(bool enabled) { recoveryEnabled = enabled; }
    bool isRecoveryEnabled() const { return recoveryEnabled; }
    
    // Statistics
    struct RecoveryStats {
        size_t totalAttempts = 0;
        size_t successfulRecoveries = 0;
        size_t failedRecoveries = 0;
        std::unordered_map<std::string, size_t> strategyUsage;
    };
    
    const RecoveryStats& getStatistics() const { return stats; }
    void resetStatistics();
    
    // Error analysis
    std::vector<std::string> analyzeError(const ParseError& error, 
                                         const std::vector<Token>& tokens, 
                                         size_t currentPos) const;
    
    // Error suggestions
    std::vector<std::string> generateSuggestions(const ParseError& error, 
                                                 const std::vector<Token>& tokens, 
                                                 size_t currentPos) const;
    
private:
    std::vector<std::unique_ptr<RecoveryStrategy>> strategies;
    size_t maxRecoveryAttempts;
    bool recoveryEnabled;
    RecoveryStats stats;
    
    void updateStatistics(const std::string& strategyName, bool success);
};

// Error reporter class
class ErrorReporter {
public:
    ErrorReporter();
    ~ErrorReporter() = default;
    
    // Error reporting
    void reportError(const ParseError& error);
    void reportErrors(const std::vector<std::unique_ptr<ParseError>>& errors);
    
    // Configuration
    void setMaxErrors(size_t maxErrors) { maxErrorsToReport = maxErrors; }
    size_t getMaxErrors() const { return maxErrorsToReport; }
    
    void setVerbose(bool verbose) { verboseMode = verbose; }
    bool isVerbose() const { return verboseMode; }
    
    void setColorOutput(bool useColors) { colorOutput = useColors; }
    bool isColorOutput() const { return colorOutput; }
    
    // Error formatting
    std::string formatError(const ParseError& error) const;
    std::string formatErrorWithContext(const ParseError& error, 
                                      const std::string& sourceCode) const;
    
    // Statistics
    size_t getReportedErrorCount() const { return reportedErrors; }
    void resetErrorCount() { reportedErrors = 0; }
    
private:
    size_t maxErrorsToReport;
    bool verboseMode;
    bool colorOutput;
    size_t reportedErrors;
    
    std::string getColorCode(ErrorSeverity severity) const;
    std::string getSeverityString(ErrorSeverity severity) const;
    std::string extractSourceLine(const std::string& sourceCode, size_t line) const;
};

// Utility functions
std::unique_ptr<ParseError> createSyntaxError(const std::string& message, 
                                             const SourceRange& range = SourceRange());

std::unique_ptr<ParseError> createUnexpectedTokenError(const std::string& expected, 
                                                      const std::string& actual, 
                                                      const SourceRange& range = SourceRange());

std::unique_ptr<ParseError> createMissingTokenError(const std::string& missing, 
                                                   const SourceRange& range = SourceRange());

std::unique_ptr<ParseError> createSemanticError(const std::string& message, 
                                               const SourceRange& range = SourceRange());

} // namespace Parser

#endif // ERROR_RECOVERY_HPP