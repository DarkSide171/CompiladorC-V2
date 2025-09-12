#include "../include/parser_state.hpp"
#include "../include/error_recovery.hpp"
#include "../include/parser_logger.hpp"
#include <algorithm>
#include <sstream>
#include <chrono>
#include <iostream>

namespace Parser {

// ParserStateManager implementation
ParserStateManager::ParserStateManager() 
    : currentState(ParserState::NORMAL), currentTokenPosition(0), parseDepth(0),
      currentScopeLevel(0), nextScopeId(0) {
    // Initialize with global scope
    enterScope("global");
    statistics.startTime = std::chrono::high_resolution_clock::now();
}

// Context stack management
void ParserStateManager::pushContext(const std::string& contextName, size_t tokenPos) {
    contextStack.push(ParseContext(currentState, tokenPos, contextName));
}

void ParserStateManager::popContext() {
    if (!contextStack.empty()) {
        contextStack.pop();
    }
}

const ParseContext* ParserStateManager::getCurrentContext() const {
    return contextStack.empty() ? nullptr : &contextStack.top();
}

// Scope management
void ParserStateManager::enterScope(const std::string& scopeName) {
    auto scope = std::make_unique<Scope>(currentScopeLevel++, scopeName);
    scopes.push_back(std::move(scope));
    statistics.scopesCreated++;
    updateStatistics();
}

void ParserStateManager::exitScope() {
    if (currentScopeLevel > 0) {
        currentScopeLevel--;
        // Remove scopes at this level
        scopes.erase(
            std::remove_if(scopes.begin(), scopes.end(),
                [this](const std::unique_ptr<Scope>& scope) {
                    return scope->getLevel() > currentScopeLevel;
                }),
            scopes.end()
        );
        updateStatistics();
    }
}

// Symbol table operations
void ParserStateManager::declareSymbol(const std::string& name, ASTNodeType type, const Position& pos) {
    if (!scopes.empty()) {
        scopes.back()->addSymbol(name, type, pos);
        statistics.symbolsDeclarated++;
        updateStatistics();
    }
}

const Symbol* ParserStateManager::lookupSymbol(const std::string& name) const {
    // Search from most recent scope backwards
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (const Symbol* symbol = (*it)->getSymbol(name)) {
            return symbol;
        }
    }
    return nullptr;
}

bool ParserStateManager::isSymbolDeclared(const std::string& name) const {
    return lookupSymbol(name) != nullptr;
}

bool ParserStateManager::isSymbolInCurrentScope(const std::string& name) const {
    if (!scopes.empty()) {
        return scopes.back()->hasSymbol(name);
    }
    return false;
}

// Error tracking
void ParserStateManager::addError(std::unique_ptr<ParseError> error) {
    errors.push_back(std::move(error));
    statistics.totalErrors++;
    updateStatistics();
}

void ParserStateManager::clearErrors() {
    errors.clear();
    statistics.totalErrors = 0;
    updateStatistics();
}

void ParserStateManager::addWarning(const std::string& message, const Position& pos) {
    // For now, we'll store warnings as a simple counter
    // In a full implementation, we might want a separate warnings vector
    statistics.totalWarnings++;
    updateStatistics();
    
    // Log the warning for debugging purposes
    std::cout << "Warning at " << pos.line << ":" << pos.column << " - " << message << std::endl;
}

// Recovery points
void ParserStateManager::setRecoveryPoint(size_t tokenPos, const std::string& description) {
    recoveryPoints.emplace_back(tokenPos, currentState, currentScopeLevel, description);
}

const RecoveryPoint* ParserStateManager::getLastRecoveryPoint() const {
    return recoveryPoints.empty() ? nullptr : &recoveryPoints.back();
}

// Statistics and diagnostics
void ParserStateManager::startParsing() {
    statistics.startTime = std::chrono::high_resolution_clock::now();
    resetStatistics();
}

void ParserStateManager::endParsing() {
    statistics.endTime = std::chrono::high_resolution_clock::now();
    updateStatistics();
}

void ParserStateManager::startTimer(const std::string& operation) {
    timers[operation] = std::chrono::high_resolution_clock::now();
}

void ParserStateManager::endTimer(const std::string& operation) {
    auto it = timers.find(operation);
    if (it != timers.end()) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - it->second);
        timingResults[operation] = duration.count() / 1000.0; // Convert to milliseconds
        timers.erase(it);
    }
}

std::unordered_map<std::string, double> ParserStateManager::getTimingProfile() const {
    return timingResults;
}

void ParserStateManager::incrementTokensProcessed() {
    statistics.totalTokensProcessed++;
}



void ParserStateManager::resetStatistics() {
    statistics = ParseStatistics{};
    statistics.startTime = std::chrono::high_resolution_clock::now();
    statistics.endTime = statistics.startTime;
    
    // Clear timing data
    timers.clear();
    timingResults.clear();
}

void ParserStateManager::reset() {
    // Reset core state
    currentState = ParserState::NORMAL;
    currentTokenPosition = 0;
    parseDepth = 0;
    
    // Clear context stack
    while (!contextStack.empty()) {
        contextStack.pop();
    }
    
    // Reset scope management
    scopes.clear();
    currentScopeLevel = 0;
    nextScopeId = 0;
    
    // Re-initialize with global scope
    enterScope("global");
    
    // Clear errors and recovery points
    errors.clear();
    recoveryPoints.clear();
    
    // Reset statistics
    resetStatistics();
}

// Validation
bool ParserStateManager::isValidState() const {
    // Check if current state is consistent
    if (parseDepth > 1000) { // Arbitrary deep recursion limit
        return false;
    }
    
    if (currentScopeLevel != scopes.size()) {
        return false;
    }
    
    return true;
}

std::vector<std::string> ParserStateManager::validateState() const {
    std::vector<std::string> issues;
    
    if (parseDepth > 1000) {
        issues.push_back("Parse depth too deep: " + std::to_string(parseDepth));
    }
    
    if (currentScopeLevel != scopes.size()) {
        issues.push_back("Scope level mismatch: level=" + std::to_string(currentScopeLevel) + 
                        ", scopes=" + std::to_string(scopes.size()));
    }
    
    if (contextStack.size() > 100) { // Arbitrary limit
        issues.push_back("Context stack too deep: " + std::to_string(contextStack.size()));
    }
    
    return issues;
}

// Debug methods
std::string ParserStateManager::getStateDescription() const {
    std::ostringstream oss;
    oss << "ParserStateManager {\n";
    oss << "  State: " << stateToString(currentState) << "\n";
    oss << "  Token Position: " << currentTokenPosition << "\n";
    oss << "  Parse Depth: " << parseDepth << "\n";
    oss << "  Scope Level: " << currentScopeLevel << "\n";
    oss << "  Context Stack Depth: " << contextStack.size() << "\n";
    oss << "  Errors: " << errors.size() << "\n";
    oss << "  Recovery Points: " << recoveryPoints.size() << "\n";
    oss << "}";
    return oss.str();
}

void ParserStateManager::dumpState() const {
    std::cout << getStateDescription() << std::endl;
}

void ParserStateManager::dumpSymbolTable() const {
    std::cout << "Symbol Table:\n";
    for (size_t i = 0; i < scopes.size(); ++i) {
        const auto& scope = scopes[i];
        std::cout << "  Scope " << i << " (level " << scope->getLevel() 
                  << ", name: " << scope->getName() << "):\n";
        
        for (const auto& [name, symbol] : scope->getSymbols()) {
            std::cout << "    " << name << " : " << static_cast<int>(symbol->type) << "\n";
        }
    }
}

void ParserStateManager::dumpContextStack() const {
    std::cout << "Context Stack:\n";
    std::stack<ParseContext> temp = contextStack;
    int level = 0;
    
    while (!temp.empty()) {
        const auto& context = temp.top();
        std::cout << "  [" << level++ << "] " << context.contextName 
                  << " (pos: " << context.tokenPosition 
                  << ", state: " << stateToString(context.state) << ")\n";
        temp.pop();
    }
}

// Snapshot functionality
ParserStateManager::StateSnapshot ParserStateManager::takeSnapshot() const {
    StateSnapshot snapshot;
    snapshot.state = currentState;
    snapshot.tokenPosition = currentTokenPosition;
    snapshot.scopeLevel = currentScopeLevel;
    snapshot.parseDepth = parseDepth;
    
    // Capture context names
    std::stack<ParseContext> temp = contextStack;
    while (!temp.empty()) {
        snapshot.contextNames.push_back(temp.top().contextName);
        temp.pop();
    }
    
    // Capture scope names
    for (const auto& scope : scopes) {
        snapshot.scopeNames.push_back(scope->getName());
    }
    
    return snapshot;
}

void ParserStateManager::restoreSnapshot(const StateSnapshot& snapshot) {
    currentState = snapshot.state;
    currentTokenPosition = snapshot.tokenPosition;
    parseDepth = snapshot.parseDepth;
    
    // Restore scope level (simplified - doesn't restore actual scopes)
    currentScopeLevel = snapshot.scopeLevel;
    
    // Clear context stack (simplified restoration)
    while (!contextStack.empty()) {
        contextStack.pop();
    }
}

// Private helper methods
void ParserStateManager::updateStatistics() {
    statistics.maxParseDepth = std::max(statistics.maxParseDepth, parseDepth);
    statistics.totalErrors = errors.size();
    statistics.scopesCreated = scopes.size();
    
    // Count total symbols declared across all scopes
    statistics.symbolsDeclarated = 0;
    for (const auto& scope : scopes) {
        statistics.symbolsDeclarated += scope->getSymbols().size();
    }
}

std::string ParserStateManager::stateToString(ParserState state) const {
    std::string result = "";
    if (static_cast<uint32_t>(state) & static_cast<uint32_t>(ParserState::NORMAL)) {
        result += "NORMAL ";
    }
    if (static_cast<uint32_t>(state) & static_cast<uint32_t>(ParserState::IN_FUNCTION)) {
        result += "IN_FUNCTION ";
    }
    if (static_cast<uint32_t>(state) & static_cast<uint32_t>(ParserState::IN_LOOP)) {
        result += "IN_LOOP ";
    }
    if (static_cast<uint32_t>(state) & static_cast<uint32_t>(ParserState::IN_SWITCH)) {
        result += "IN_SWITCH ";
    }
    if (static_cast<uint32_t>(state) & static_cast<uint32_t>(ParserState::RECOVERING)) {
        result += "RECOVERING ";
    }
    return result.empty() ? "NONE" : result;
}

// Debug and profiling methods
std::string ParserStateManager::getDebugInfo() const {
    if (!debugModeEnabled) {
        return "Debug mode disabled";
    }
    
    std::ostringstream oss;
    oss << "=== Parser Debug Information ===\n";
    oss << "Debug Level: " << static_cast<int>(currentDebugLevel) << "\n";
    oss << getStateDescription() << "\n";
    
    if (currentDebugLevel >= DebugLevel::DETAILED) {
        oss << "\nTiming Profile:\n";
        for (const auto& [operation, time] : timingResults) {
            oss << "  " << operation << ": " << time << "ms\n";
        }
        
        oss << "\nMemory Profile:\n";
        for (const auto& [operation, memory] : memoryProfile) {
            oss << "  " << operation << ": " << memory << " bytes\n";
        }
    }
    
    return oss.str();
}

std::string ParserStateManager::getDetailedStateInfo() const {
    std::ostringstream oss;
    oss << "=== Detailed Parser State ===\n";
    oss << getStateDescription() << "\n";
    
    oss << "\nContext Stack Details:\n";
    std::stack<ParseContext> temp = contextStack;
    int level = 0;
    while (!temp.empty()) {
        const auto& context = temp.top();
        oss << "  [" << level++ << "] " << context.contextName 
            << " (pos: " << context.tokenPosition 
            << ", state: " << stateToString(context.state) << ")\n";
        temp.pop();
    }
    
    oss << "\nScope Details:\n";
    for (size_t i = 0; i < scopes.size(); ++i) {
        const auto& scope = scopes[i];
        oss << "  Scope " << i << " (level " << scope->getLevel() 
            << ", name: " << scope->getName() << "):\n";
        
        for (const auto& [name, symbol] : scope->getSymbols()) {
            oss << "    " << name << " : " << static_cast<int>(symbol->type) 
                << " at (" << symbol->declaration.line << "," << symbol->declaration.column << ")\n";
        }
    }
    
    return oss.str();
}

std::string ParserStateManager::getPerformanceReport() const {
    std::ostringstream oss;
    oss << "=== Performance Report ===\n";
    
    const auto& stats = getStatistics();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        stats.endTime - stats.startTime).count();
    
    oss << "Total Parse Time: " << duration << "ms\n";
    oss << "Tokens Processed: " << stats.totalTokensProcessed << "\n";
    oss << "Max Parse Depth: " << stats.maxParseDepth << "\n";
    oss << "Scopes Created: " << stats.scopesCreated << "\n";
    oss << "Symbols Declared: " << stats.symbolsDeclarated << "\n";
    oss << "Errors: " << stats.totalErrors << "\n";
    oss << "Warnings: " << stats.totalWarnings << "\n";
    
    if (stats.totalTokensProcessed > 0 && duration > 0) {
        oss << "Tokens/ms: " << (stats.totalTokensProcessed / static_cast<double>(duration)) << "\n";
    }
    
    oss << "\nOperation Timings:\n";
    for (const auto& [operation, time] : timingResults) {
        oss << "  " << operation << ": " << time << "ms\n";
    }
    
    oss << "\nMemory Usage:\n";
    for (const auto& [operation, memory] : memoryProfile) {
        oss << "  " << operation << ": " << memory << " bytes\n";
    }
    
    return oss.str();
}

void ParserStateManager::logDebugInfo(const std::string& /* message */) const {
    if (debugModeEnabled && currentDebugLevel >= DebugLevel::BASIC) {
        // DEBUG: Log message output disabled
        // std::cout << "[DEBUG] " << message << std::endl;
    }
}

size_t ParserStateManager::getMemoryUsage() const {
    size_t totalMemory = 0;
    
    // Estimate memory usage of various components
    totalMemory += sizeof(ParserStateManager);
    totalMemory += errors.size() * sizeof(std::unique_ptr<ParseError>);
    totalMemory += recoveryPoints.size() * sizeof(RecoveryPoint);
    totalMemory += scopes.size() * sizeof(std::unique_ptr<Scope>);
    
    // Estimate symbol table memory
    for (const auto& scope : scopes) {
        totalMemory += scope->getSymbols().size() * sizeof(Symbol);
    }
    
    // Estimate context stack memory
    totalMemory += contextStack.size() * sizeof(ParseContext);
    
    return totalMemory;
}

void ParserStateManager::trackMemoryUsage(const std::string& operation) {
    if (debugModeEnabled) {
        memoryProfile[operation] = getMemoryUsage();
}
}
std::unordered_map<std::string, size_t> ParserStateManager::getMemoryProfile() const {
    return memoryProfile;
}

// Position history and navigation
void ParserStateManager::pushPositionHistory() {
    positionHistory.push(currentTokenPosition);
}

bool ParserStateManager::popPositionHistory() {
    if (!positionHistory.empty()) {
        currentTokenPosition = positionHistory.top();
        positionHistory.pop();
        if (tokenStream) {
            tokenStream->setPosition(currentTokenPosition);
        }
        return true;
    }
    return false;
}

void ParserStateManager::clearPositionHistory() {
    while (!positionHistory.empty()) {
        positionHistory.pop();
    }
}

size_t ParserStateManager::getPositionHistorySize() const {
    return positionHistory.size();
}

// Token control methods
const Token& ParserStateManager::getCurrentToken() const {
    if (tokenStream) {
        return tokenStream->current();
    }
    // Retorna token EOF quando não há mais tokens
    static Lexer::Position eofPos{0, 0, 0};
    static Token eofToken(Lexer::TokenType::END_OF_FILE, "", eofPos);
    return eofToken;
}

const Token& ParserStateManager::peekToken(size_t offset) const {
    if (tokenStream) {
        return tokenStream->peek(offset);
    }
    // Retorna token EOF quando não há mais tokens
    static Lexer::Position eofPos{0, 0, 0};
    static Token eofToken(Lexer::TokenType::END_OF_FILE, "", eofPos);
    return eofToken;
}

bool ParserStateManager::consumeToken() {
    if (tokenStream && !tokenStream->isAtEnd()) {
        tokenStream->advance();
        currentTokenPosition++;
        incrementTokensProcessed();
        return true;
    }
    return false;
}

bool ParserStateManager::isAtEnd() const {
    if (tokenStream) {
        return tokenStream->isAtEnd();
    }
    return true;
}

// (Constructor and destructor are inline in header)

// ScopedScope implementation  
// (Constructor and destructor are inline in header)

// ScopedContext implementation
// (Constructor and destructor are inline in header)

// Advanced positioning control implementation
void ParserStateManager::savePosition(const std::string& label) {
    std::string posLabel = label;
    if (posLabel.empty()) {
        posLabel = "auto_" + std::to_string(defaultSavedPositionCounter++);
    }
    savedPositions[posLabel] = currentTokenPosition;
    PARSER_LOG_DEBUG("Position saved with label: " + posLabel + " at position: " + std::to_string(currentTokenPosition));
}

bool ParserStateManager::restorePosition(const std::string& label) {
    std::string posLabel = label;
    if (posLabel.empty() && !savedPositions.empty()) {
        // Find the most recent auto-saved position
        size_t maxCounter = 0;
        for (const auto& pair : savedPositions) {
            if (pair.first.substr(0, 5) == "auto_") {
                size_t counter = std::stoul(pair.first.substr(5));
                if (counter > maxCounter) {
                    maxCounter = counter;
                    posLabel = pair.first;
                }
            }
        }
    }
    
    auto it = savedPositions.find(posLabel);
    if (it != savedPositions.end()) {
        currentTokenPosition = it->second;
        if (tokenStream) {
            tokenStream->setPosition(currentTokenPosition);
        }
        PARSER_LOG_DEBUG("Position restored from label: " + posLabel + " to position: " + std::to_string(currentTokenPosition));
        return true;
    }
    return false;
}

void ParserStateManager::clearSavedPositions() {
    savedPositions.clear();
    defaultSavedPositionCounter = 0;
    PARSER_LOG_DEBUG("All saved positions cleared");
}

} // namespace Parser