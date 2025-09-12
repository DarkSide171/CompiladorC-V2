#include "../include/error_recovery.hpp"
#include "../include/parser_logger.hpp"
#include "../include/parser_state.hpp"
#include <sstream>
#include <algorithm>
#include <set>
#include <map>

namespace Parser {

// ParseError implementation
std::string ParseError::format() const {
    std::ostringstream oss;
    oss << "[" << (errorSeverity == ErrorSeverity::WARNING ? "WARNING" : 
                    errorSeverity == ErrorSeverity::ERROR ? "ERROR" : 
                    errorSeverity == ErrorSeverity::FATAL ? "FATAL" : "UNKNOWN") << "] "
        << "Line " << sourceRange.start.line << ", Column " << sourceRange.start.column
        << ": " << errorMessage;
    
    if (!notes.empty()) {
        oss << "\n  Notes:";
        for (const auto& note : notes) {
            oss << "\n    " << note;
        }
    }
    
    if (!suggestions.empty()) {
        oss << "\n  Suggestions:";
        for (const auto& suggestion : suggestions) {
            oss << "\n    " << suggestion;
        }
    }
    
    return oss.str();
}



// PanicModeRecovery implementation
PanicModeRecovery::PanicModeRecovery() {
    initializeDefaultSyncTokens();
}

bool PanicModeRecovery::recover(ParserStateManager& state, const std::vector<Token>& tokens, size_t& currentPos) {
    if (currentPos >= tokens.size()) {
        return false;
    }

    size_t startPosition = currentPos;
    (void)startPosition; // Suppress unused variable warning
    size_t tokensSkipped = 0;
    
    // Log recovery attempt
    state.logDebugInfo("Starting panic mode recovery at position " + std::to_string(currentPos));
    const size_t MAX_TOKENS_TO_SKIP = 50;
    
    // Skip tokens until we find a synchronization token
    while (currentPos < tokens.size()) {
        const auto& currentToken = tokens[currentPos];
        
        // Check if current token is a synchronization token
        if (syncTokens.find(currentToken.getLexeme()) != syncTokens.end()) {
            if (tokensSkipped > 0) {
                PARSER_LOG_INFO("Panic mode recovery: skipped " + std::to_string(tokensSkipped) + 
                               " tokens, synchronized at '" + currentToken.getLexeme() + "'");
            }
            return true;
        }
        
        currentPos++;
        tokensSkipped++;
        
        // Prevent infinite loops
        if (tokensSkipped > MAX_TOKENS_TO_SKIP) {
            PARSER_LOG_WARNING("Panic mode recovery: exceeded maximum tokens to skip");
            return false;
        }
    }
    
    // Reached end of input without finding synchronization token
    PARSER_LOG_WARNING("Panic mode recovery: reached end of input without synchronization");
    return false;
}

bool ErrorRecoveryManager::synchronizeToToken(Lexer::TokenType targetType, ParserStateManager& state, 
                                             const std::vector<Token>& tokens, size_t& currentPos) {
    (void)state; // Suppress unused parameter warning
    PARSER_LOG_DEBUG("Synchronizing to token type: " + std::to_string(static_cast<int>(targetType)));
    
    const size_t MAX_SYNC_DISTANCE = 20;
    size_t startPos = currentPos;
    
    while (currentPos < tokens.size() && (currentPos - startPos) < MAX_SYNC_DISTANCE) {
        if (tokens[currentPos].getType() == targetType) {
            PARSER_LOG_INFO("Synchronized to target token at position " + std::to_string(currentPos));
            return true;
        }
        currentPos++;
    }
    
    // Reset position if synchronization failed
    currentPos = startPos;
    PARSER_LOG_DEBUG("Failed to synchronize to target token");
    return false;
}

bool ErrorRecoveryManager::skipToSemicolon(ParserStateManager& state, 
                                          const std::vector<Token>& tokens, size_t& currentPos) {
    (void)state; // Suppress unused parameter warning
    PARSER_LOG_DEBUG("Skipping to next semicolon");
    
    const size_t MAX_SKIP_DISTANCE = 15;
    size_t startPos = currentPos;
    
    while (currentPos < tokens.size() && (currentPos - startPos) < MAX_SKIP_DISTANCE) {
        if (tokens[currentPos].getLexeme() == ";") {
            PARSER_LOG_INFO("Found semicolon at position " + std::to_string(currentPos));
            currentPos++; // Move past the semicolon
            return true;
        }
        currentPos++;
    }
    
    // Reset position if no semicolon found
    currentPos = startPos;
    PARSER_LOG_DEBUG("No semicolon found within skip distance");
    return false;
}

bool ErrorRecoveryManager::recoverFromMissingBrace(char braceType, ParserStateManager& state, 
                                                  const std::vector<Token>& tokens, size_t& currentPos) {
    (void)state; // Suppress unused parameter warning
    PARSER_LOG_DEBUG("Attempting recovery from missing brace: " + std::string(1, braceType));
    
    // Strategy 1: Insert opening brace if we're at a statement that needs one
    if (currentPos > 0 && currentPos < tokens.size()) {
        const std::string& prevToken = tokens[currentPos - 1].getLexeme();
        const std::string& currentToken = tokens[currentPos].getLexeme();
        
        // Check if we need an opening brace
        if (braceType == '{' && (prevToken == ")" || prevToken == "else") && currentToken != "{") {
            PARSER_LOG_INFO("Inserting missing opening brace");
            // In a real implementation, we would insert the brace into the token stream
            return true;
        }
    }
    
    // Strategy 2: Look for unmatched braces and try to balance them
    int braceBalance = 0;
    size_t searchPos = currentPos;
    const size_t MAX_SEARCH_DISTANCE = 30;
    
    while (searchPos < tokens.size() && (searchPos - currentPos) < MAX_SEARCH_DISTANCE) {
        const std::string& lexeme = tokens[searchPos].getLexeme();
        
        if (lexeme == std::string(1, braceType == '{' ? '{' : '(')) {
            braceBalance++;
        } else if (lexeme == std::string(1, braceType == '{' ? '}' : ')')) {
            braceBalance--;
            
            // If we have unmatched closing brace, we might need an opening brace
            if (braceBalance < 0) {
                PARSER_LOG_INFO("Found unmatched closing brace, inserting opening brace");
                return true;
            }
        }
        searchPos++;
    }
    
    // Strategy 3: If we have unmatched opening braces, insert closing brace
    if (braceBalance > 0) {
        PARSER_LOG_INFO("Inserting missing closing brace");
        return true;
    }
    
    return false;
}

bool ErrorRecoveryManager::recoverFromExpressionError(ParserStateManager& state, 
                                                     const std::vector<Token>& tokens, size_t& currentPos) {
    (void)state; // Suppress unused parameter warning
    PARSER_LOG_DEBUG("Attempting recovery from expression error");
    
    if (currentPos >= tokens.size()) {
        return false;
    }
    
    const std::string& currentToken = tokens[currentPos].getLexeme();
    
    // Strategy 1: Skip malformed expression until we find a statement terminator
    const std::set<std::string> terminators = {";", "{", "}", "return", "if", "while", "for"};
    
    size_t searchPos = currentPos;
    const size_t MAX_EXPRESSION_SKIP = 10;
    
    while (searchPos < tokens.size() && (searchPos - currentPos) < MAX_EXPRESSION_SKIP) {
        const std::string& lexeme = tokens[searchPos].getLexeme();
        
        if (terminators.find(lexeme) != terminators.end()) {
            PARSER_LOG_INFO("Skipping malformed expression, found terminator: " + lexeme);
            currentPos = searchPos;
            return true;
        }
        searchPos++;
    }
    
    // Strategy 2: Try to fix common expression errors
    if (currentPos + 1 < tokens.size()) {
        const std::string& nextToken = tokens[currentPos + 1].getLexeme();
        
        // Missing operator between operands
        if (isIdentifierOrLiteral(currentToken) && isIdentifierOrLiteral(nextToken)) {
            PARSER_LOG_INFO("Inserting missing operator between operands");
            currentPos++; // Skip one operand
            return true;
        }
        
        // Missing operand after operator
        if (isOperator(currentToken) && (nextToken == ";" || nextToken == "}")) {
            PARSER_LOG_INFO("Removing dangling operator");
            currentPos++; // Skip the operator
            return true;
        }
    }
    
    // Strategy 3: Skip the problematic token
    PARSER_LOG_INFO("Skipping problematic token in expression: " + currentToken);
    currentPos++;
    return true;
}

bool ErrorRecoveryManager::isIdentifierOrLiteral(const std::string& token) const {
    if (token.empty()) return false;
    
    // Check if it's a number literal
    if (std::isdigit(token[0]) || (token[0] == '.' && token.length() > 1)) {
        return true;
    }
    
    // Check if it's a string literal
    if (token[0] == '"' || token[0] == '\'') {
        return true;
    }
    
    // Check if it's an identifier (starts with letter or underscore)
    if (std::isalpha(token[0]) || token[0] == '_') {
        return true;
    }
    
    return false;
}

bool ErrorRecoveryManager::isOperator(const std::string& token) const {
    static const std::set<std::string> operators = {
        "+", "-", "*", "/", "%", "=", "==", "!=", "<", ">", "<=", ">=",
        "&&", "||", "!", "&", "|", "^", "<<", ">>", "++", "--",
        "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="
    };
    
    return operators.find(token) != operators.end();
}



bool PanicModeRecovery::canHandle(const ParseError& error, const std::vector<Token>& tokens, size_t currentPos) const {
    (void)error;
    (void)tokens;
    (void)currentPos;
    // Panic mode can handle any error, but it's usually a last resort
    return true;
}

void PanicModeRecovery::initializeDefaultSyncTokens() {
    syncTokens = {
        ";", "}", "{", "if", "while", "for", "return", "break", "continue",
        "int", "char", "float", "double", "void", "struct", "union", "enum",
        "typedef", "static", "extern", "const", "volatile"
    };
}

// PhraseLevelRecovery implementation
PhraseLevelRecovery::PhraseLevelRecovery() {
    initializeDefaultTokens();
}

bool PhraseLevelRecovery::recover(ParserStateManager& state, const std::vector<Token>& tokens, size_t& currentPos) {
    if (currentPos >= tokens.size()) {
        return false;
    }

    // Try inserting common missing tokens
    for (const auto& token : insertableTokens) {
        if (tryInsertToken(token, state, tokens, currentPos)) {
            return true;
        }
    }
    
    // Try deleting the current token if it's deletable
    if (tryDeleteToken(state, tokens, currentPos)) {
        return true;
    }
    
    return false;
}



bool PhraseLevelRecovery::canHandle(const ParseError& error, const std::vector<Token>& tokens, size_t currentPos) const {
    (void)tokens;
    (void)currentPos;
    // Phrase-level recovery works best with syntax errors
    return error.getErrorCode() == "SYNTAX_ERROR" || 
           error.getErrorCode() == "UNEXPECTED_TOKEN" ||
           error.getErrorCode() == "MISSING_TOKEN";
}

void PhraseLevelRecovery::initializeDefaultTokens() {
    insertableTokens = {
        ";", "{", "}", "(", ")", ",", "="
    };
    
    deletableTokens = {
        ";", ",", "{", "}"
    };
}

bool PhraseLevelRecovery::tryInsertToken(const std::string& token, ParserStateManager& state, 
                                        const std::vector<Token>& tokens, size_t& currentPos) {
    (void)state;
    // Simple heuristic for token insertion
    if (currentPos < tokens.size()) {
        const auto& currentToken = tokens[currentPos];
        
        // Check if inserting this token would make sense
        if (token == ";" && (currentToken.getLexeme() == "}" || currentToken.getLexeme() == "return")) {
            PARSER_LOG_INFO("Phrase-level recovery: inserting missing ';' before '" + 
                           currentToken.getLexeme() + "'");
            return true;
        }
        
        if (token == "{" && (currentToken.getLexeme() == "if" || currentToken.getLexeme() == "while")) {
            PARSER_LOG_INFO("Phrase-level recovery: might need '{' after control statement");
            return true;
        }
    }
    
    return false;
}

bool PhraseLevelRecovery::tryDeleteToken(ParserStateManager& state, const std::vector<Token>& tokens, size_t& currentPos) {
    (void)state;
    if (currentPos >= tokens.size()) {
        return false;
    }
    
    const auto& currentToken = tokens[currentPos];
    
    // Check if current token can be safely deleted
    if (deletableTokens.find(currentToken.getLexeme()) != deletableTokens.end()) {
        // Check for duplicate tokens
        if (currentPos + 1 < tokens.size()) {
            const auto& nextToken = tokens[currentPos + 1];
            if (currentToken.getLexeme() == nextToken.getLexeme() && currentToken.getLexeme() == ";") {
                PARSER_LOG_INFO("Phrase-level recovery: deleting duplicate semicolon");
                currentPos++; // Skip the duplicate token
                return true;
            }
        }
    }
    
    return false;
}

// ErrorProductionRecovery implementation
ErrorProductionRecovery::ErrorProductionRecovery() {
    // Initialize error productions if needed
}

bool ErrorProductionRecovery::recover(ParserStateManager& state, const std::vector<Token>& tokens, size_t& currentPos) {
    (void)state;
    (void)tokens;
    (void)currentPos;
    if (currentPos >= tokens.size()) {
        return false;
    }
    
    const auto& currentToken = tokens[currentPos];
    PARSER_LOG_DEBUG("Attempting error production recovery at token: " + currentToken.getLexeme());
    
    // Try to recover from common error patterns
    
    // Pattern 1: Missing semicolon - insert virtual semicolon
    if (currentToken.getType() == Lexer::TokenType::RIGHT_BRACE || 
        Lexer::isKeywordToken(currentToken.getType())) {
        PARSER_LOG_INFO("Error production recovery: inserting virtual semicolon");
        // Don't advance position, just signal successful recovery
        return true;
    }
    
    // Pattern 2: Extra comma in declarations - skip it
    if (currentToken.getType() == Lexer::TokenType::COMMA) {
        // Check if next token suggests we should skip this comma
        if (currentPos + 1 < tokens.size()) {
            const auto& nextToken = tokens[currentPos + 1];
            if (nextToken.getType() == Lexer::TokenType::RIGHT_BRACE ||
                nextToken.getType() == Lexer::TokenType::SEMICOLON) {
                PARSER_LOG_INFO("Error production recovery: skipping extra comma");
                currentPos++; // Skip the comma
                return true;
            }
        }
    }
    
    // Pattern 3: Missing closing brace - insert virtual brace
    if (currentToken.getType() == Lexer::TokenType::END_OF_FILE) {
        PARSER_LOG_INFO("Error production recovery: inserting virtual closing brace at EOF");
        return true;
    }
    
    PARSER_LOG_DEBUG("Error production recovery: no applicable pattern found");
    return false;
}

bool ErrorProductionRecovery::canHandle(const ParseError& error, const std::vector<Token>& tokens, size_t currentPos) const {
    if (currentPos >= tokens.size()) {
        return false;
    }
    
    // Check if this is an error type we can handle with error productions
    const std::string& errorCode = error.getErrorCode();
    
    // Handle missing token errors
    if (errorCode == "MISSING_TOKEN") {
        return true;
    }
    
    // Handle unexpected token errors in specific contexts
    if (errorCode == "UNEXPECTED_TOKEN") {
        const auto& currentToken = tokens[currentPos];
        
        // Can handle extra commas, missing semicolons, etc.
        if (currentToken.getType() == Lexer::TokenType::COMMA ||
            currentToken.getType() == Lexer::TokenType::RIGHT_BRACE ||
            currentToken.getType() == Lexer::TokenType::END_OF_FILE) {
            return true;
        }
    }
    
    // Handle syntax errors in expression contexts
    if (errorCode == "SYNTAX_ERROR") {
        const std::string& message = error.getMessage();
        if (message.find("semicolon") != std::string::npos ||
            message.find("brace") != std::string::npos ||
            message.find("comma") != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

// ErrorRecoveryManager implementation
ErrorRecoveryManager::ErrorRecoveryManager() 
    : maxRecoveryAttempts(3), recoveryEnabled(true) {
    // Initialize with default recovery strategies
    addStrategy(std::make_unique<PanicModeRecovery>());
    addStrategy(std::make_unique<PhraseLevelRecovery>());
    addStrategy(std::make_unique<ErrorProductionRecovery>());
}

void ErrorRecoveryManager::addStrategy(std::unique_ptr<RecoveryStrategy> strategy) {
    strategies.push_back(std::move(strategy));
}

void ErrorRecoveryManager::removeStrategy(const std::string& strategyName) {
    strategies.erase(
        std::remove_if(strategies.begin(), strategies.end(),
            [&strategyName](const std::unique_ptr<RecoveryStrategy>& strategy) {
                return strategy->getName() == strategyName;
            }),
        strategies.end()
    );
}

void ErrorRecoveryManager::clearStrategies() {
    strategies.clear();
}

bool ErrorRecoveryManager::attemptRecovery(ParseError& error, ParserStateManager& state, 
                                          const std::vector<Token>& tokens, size_t& currentPos) {
    if (!recoveryEnabled || strategies.empty()) {
        PARSER_LOG_WARNING("Recovery disabled or no strategies available");
        return false;
    }
    
    stats.totalAttempts++;
    
    // Try each strategy that can handle this error
    for (auto& strategy : strategies) {
        if (strategy->canHandle(error, tokens, currentPos)) {
            PARSER_LOG_DEBUG("Attempting recovery with strategy: " + strategy->getName());
            
            size_t originalPos = currentPos;
            
            if (strategy->recover(state, tokens, currentPos)) {
                updateStatistics(strategy->getName(), true);
                stats.successfulRecoveries++;
                
                PARSER_LOG_INFO("Recovery successful with strategy: " + strategy->getName());
                return true;
            }
            
            // Restore position if strategy failed
            currentPos = originalPos;
        }
    }
    
    stats.failedRecoveries++;
    PARSER_LOG_WARNING("All applicable recovery strategies failed");
    return false;
}

bool ErrorRecoveryManager::recoverFromError(ParseError& error, ParserStateManager& state, 
                                           const std::vector<Token>& tokens, size_t& currentPos) {
    if (!recoveryEnabled) {
        return false;
    }
    
    PARSER_LOG_DEBUG("Starting error recovery for: " + error.getMessage());
    
    // Try specific recovery methods based on error type
    if (auto* unexpectedError = dynamic_cast<UnexpectedTokenError*>(&error)) {
        if (recoverFromUnexpectedToken(unexpectedError->getActual(), 
                                      unexpectedError->getExpected(), 
                                      state, tokens, currentPos)) {
            return true;
        }
    }
    
    if (auto* missingError = dynamic_cast<MissingTokenError*>(&error)) {
        if (recoverFromMissingToken(missingError->getMissingToken(), 
                                   state, tokens, currentPos)) {
            return true;
        }
    }
    
    // Fall back to general recovery strategies
    return attemptRecovery(error, state, tokens, currentPos);
}

bool ErrorRecoveryManager::recoverFromUnexpectedToken(const std::string& actual, 
                                                     const std::string& expected, 
                                                     ParserStateManager& state, 
                                                     const std::vector<Token>& tokens, 
                                                     size_t& currentPos) {
    (void)state; // Suppress unused parameter warning
    PARSER_LOG_DEBUG("Attempting recovery from unexpected token: '" + actual + "', expected: '" + expected + "'");
    
    // Strategy 1: Skip the unexpected token if it's commonly misplaced
    if (isSkippableToken(actual)) {
        PARSER_LOG_INFO("Skipping unexpected token: '" + actual + "'");
        currentPos++;
        return true;
    }
    
    // Strategy 2: Insert the expected token before the current position
    if (canInsertToken(expected, tokens, currentPos)) {
        PARSER_LOG_INFO("Inserting missing token: '" + expected + "' before '" + actual + "'");
        // In a real implementation, we would modify the token stream
        return true;
    }
    
    // Strategy 3: Replace the unexpected token with the expected one
    if (canReplaceToken(actual, expected)) {
        PARSER_LOG_INFO("Replacing '" + actual + "' with '" + expected + "'");
        currentPos++;
        return true;
    }
    
    return false;
}

bool ErrorRecoveryManager::recoverFromMissingToken(const std::string& missing, ParserStateManager& state, 
                                                  const std::vector<Token>& tokens, size_t& currentPos) {
    (void)state; // Suppress unused parameter warning
    PARSER_LOG_DEBUG("Attempting recovery from missing token: '" + missing + "'");
    
    // Strategy 1: Insert the missing token at current position
    if (canInsertToken(missing, tokens, currentPos)) {
        PARSER_LOG_INFO("Inserting missing token: '" + missing + "'");
        // In a real implementation, we would modify the token stream
        return true;
    }
    
    // Strategy 2: Look ahead for the missing token
    size_t lookAheadPos = currentPos + 1;
    const size_t MAX_LOOKAHEAD = 5;
    
    while (lookAheadPos < tokens.size() && (lookAheadPos - currentPos) < MAX_LOOKAHEAD) {
        if (tokens[lookAheadPos].getLexeme() == missing) {
            PARSER_LOG_INFO("Found missing token '" + missing + "' at position " + std::to_string(lookAheadPos));
            // Skip tokens until we reach the missing token
            currentPos = lookAheadPos;
            return true;
        }
        lookAheadPos++;
    }
    
    return false;
}

bool ErrorRecoveryManager::isSkippableToken(const std::string& token) const {
    static const std::set<std::string> skippableTokens = {
        ";", ",", "{", "}", "(", ")"
    };
    
    return skippableTokens.find(token) != skippableTokens.end();
}

bool ErrorRecoveryManager::canInsertToken(const std::string& token, 
                                         const std::vector<Token>& tokens, 
                                         size_t currentPos) const {
    // Simple heuristics for token insertion
    if (token == ";" && currentPos < tokens.size()) {
        const std::string& currentLexeme = tokens[currentPos].getLexeme();
        return currentLexeme == "}" || currentLexeme == "return" || currentLexeme == "break";
    }
    
    if (token == "{" && currentPos > 0) {
        const std::string& prevLexeme = tokens[currentPos - 1].getLexeme();
        return prevLexeme == ")" || prevLexeme == "else";
    }
    
    if (token == "}" && currentPos < tokens.size()) {
        // Look for unmatched opening braces
        return true; // Simplified - would need proper brace counting
    }
    
    return false;
}

bool ErrorRecoveryManager::canReplaceToken(const std::string& actual, 
                                          const std::string& expected) const {
    // Simple heuristics for token replacement
    static const std::map<std::string, std::set<std::string>> replaceableTokens = {
        {"=", {"==", "!="}},
        {"==", {"=", "!="}},
        {";", {","}},
        {",", {";"}} 
    };
    
    auto it = replaceableTokens.find(actual);
    if (it != replaceableTokens.end()) {
        return it->second.find(expected) != it->second.end();
    }
    
    return false;
}



void ErrorRecoveryManager::resetStatistics() {
    stats = RecoveryStats{};
}

std::vector<std::string> ErrorRecoveryManager::analyzeError(const ParseError& error, 
                                                           const std::vector<Token>& tokens, 
                                                           size_t currentPos) const {
    std::vector<std::string> analysis;
    
    analysis.push_back("Error Type: " + error.getErrorCode());
    std::string severityStr = (error.getSeverity() == ErrorSeverity::WARNING ? "WARNING" : 
                               error.getSeverity() == ErrorSeverity::ERROR ? "ERROR" : 
                               error.getSeverity() == ErrorSeverity::FATAL ? "FATAL" : "UNKNOWN");
    analysis.push_back("Severity: " + severityStr);
    
    if (currentPos < tokens.size()) {
        analysis.push_back("Current Token: '" + tokens[currentPos].getLexeme() + "'");
    }
    
    return analysis;
}

std::vector<std::string> ErrorRecoveryManager::generateSuggestions(const ParseError& error, 
                                                                  const std::vector<Token>& tokens, 
                                                                  size_t currentPos) const {
    (void)tokens;
    (void)currentPos;
    std::vector<std::string> suggestions;
    
    // Generate context-specific suggestions based on error type
    if (error.getErrorCode() == "MISSING_TOKEN") {
        suggestions.push_back("Check for missing semicolons, braces, or parentheses");
    } else if (error.getErrorCode() == "UNEXPECTED_TOKEN") {
        suggestions.push_back("Check for typos or incorrect token usage");
    } else if (error.getErrorCode() == "SYNTAX_ERROR") {
        suggestions.push_back("Review the syntax rules for this construct");
    }
    
    return suggestions;
}

void ErrorRecoveryManager::updateStatistics(const std::string& strategyName, bool success) {
    (void)success;
    stats.strategyUsage[strategyName]++;
}

// ErrorReporter implementation
ErrorReporter::ErrorReporter() 
    : maxErrorsToReport(100), verboseMode(false), colorOutput(true), reportedErrors(0) {}

void ErrorReporter::reportError(const ParseError& error) {
    if (reportedErrors >= maxErrorsToReport) {
        PARSER_LOG_WARNING("Maximum error count reached, suppressing further errors");
        return;
    }
    
    std::string formattedError = formatError(error);
    PARSER_LOG_ERROR(formattedError);
    reportedErrors++;
}

void ErrorReporter::reportErrors(const std::vector<std::unique_ptr<ParseError>>& errors) {
    for (const auto& error : errors) {
        reportError(*error);
    }
}

std::string ErrorReporter::formatError(const ParseError& error) const {
    std::ostringstream oss;
    
    if (colorOutput) {
        oss << getColorCode(error.getSeverity());
    }
    
    oss << "<input>:" << error.getSourceRange().start.line 
        << ":" << error.getSourceRange().start.column
        << ": " << (error.getSeverity() == ErrorSeverity::WARNING ? "WARNING" : 
                      error.getSeverity() == ErrorSeverity::ERROR ? "ERROR" : 
                      error.getSeverity() == ErrorSeverity::FATAL ? "FATAL" : "UNKNOWN") << ": "
        << error.getMessage();
    
    if (colorOutput) {
        oss << "\033[0m"; // Reset color
    }
    
    return oss.str();
}

std::string ErrorReporter::formatErrorWithContext(const ParseError& error, 
                                                 const std::string& sourceCode) const {
    std::string basicFormat = formatError(error);
    std::string sourceLine = extractSourceLine(sourceCode, error.getSourceRange().start.line);
    
    if (!sourceLine.empty()) {
        basicFormat += "\n" + sourceLine;
        
        // Add caret indicator
        std::string indicator(error.getSourceRange().start.column - 1, ' ');
        indicator += "^";
        basicFormat += "\n" + indicator;
    }
    
    return basicFormat;
}

std::string ErrorReporter::getColorCode(ErrorSeverity severity) const {
    switch (severity) {
        case ErrorSeverity::WARNING: return "\033[33m"; // Yellow
        case ErrorSeverity::ERROR: return "\033[31m";   // Red
        case ErrorSeverity::FATAL: return "\033[35m";   // Magenta
        default: return "\033[0m";                      // Reset
    }
}

std::string ErrorReporter::getSeverityString(ErrorSeverity severity) const {
    switch (severity) {
        case ErrorSeverity::WARNING: return "warning";
        case ErrorSeverity::ERROR: return "error";
        case ErrorSeverity::FATAL: return "fatal error";
        default: return "unknown";
    }
}

std::string ErrorReporter::extractSourceLine(const std::string& sourceCode, size_t line) const {
    std::istringstream iss(sourceCode);
    std::string currentLine;
    size_t currentLineNumber = 1;
    
    while (std::getline(iss, currentLine) && currentLineNumber <= line) {
        if (currentLineNumber == line) {
            return currentLine;
        }
        currentLineNumber++;
    }
    
    return "";
}

// Utility functions
std::unique_ptr<ParseError> createSyntaxError(const std::string& message, const SourceRange& range) {
    return std::make_unique<SyntaxError>(message, range);
}

std::unique_ptr<ParseError> createUnexpectedTokenError(const std::string& expected, 
                                                      const std::string& actual, 
                                                      const SourceRange& range) {
    return std::make_unique<UnexpectedTokenError>(expected, actual, range);
}

std::unique_ptr<ParseError> createMissingTokenError(const std::string& missing, 
                                                   const SourceRange& range) {
    return std::make_unique<MissingTokenError>(missing, range);
}

std::unique_ptr<ParseError> createSemanticError(const std::string& message, 
                                               const SourceRange& range) {
    return std::make_unique<SemanticError>(message, range);
}

} // namespace Parser