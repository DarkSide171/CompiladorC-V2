#include "include/parser_state.hpp"
#include "include/parser_logger.hpp"
#include "include/parser_types.hpp"
#include "include/token_stream.hpp"
#include <algorithm>
#include <sstream>
#include <chrono>

namespace Parser {

// ParserStateManager implementation
ParserStateManager::ParserStateManager()
    : currentState(ParserState::NORMAL)
    , currentTokenPosition(0)
    , parseDepth(0)
    , tokenStream(nullptr)
    , currentScopeLevel(0)
    , nextScopeId(1)
    , defaultSavedPositionCounter(0) {
    
    // Create global scope
    scopes.push_back(std::make_unique<Scope>(0, "global"));
    resetStatistics();
}

void ParserStateManager::pushContext(const std::string& contextName, size_t tokenPos) {
    contextStack.push(ParseContext(currentState, tokenPos, contextName));
    parseDepth++;
    updateStatistics();
    
    PARSER_LOG_DEBUG("Pushed context: " + contextName + " at token " + std::to_string(tokenPos));
}

void ParserStateManager::popContext() {
    if (!contextStack.empty()) {
        auto context = contextStack.top();
        contextStack.pop();
        parseDepth--;
        
        PARSER_LOG_DEBUG("Popped context: " + context.contextName);
    } else {
        PARSER_LOG_WARNING("Attempted to pop context from empty stack");
    }
}

const ParseContext* ParserStateManager::getCurrentContext() const {
    return contextStack.empty() ? nullptr : &contextStack.top();
}

void ParserStateManager::enterScope(const std::string& scopeName) {
    currentScopeLevel++;
    scopes.push_back(std::make_unique<Scope>(currentScopeLevel, scopeName.empty() ? 
        "scope_" + std::to_string(nextScopeId++) : scopeName));
    
    statistics.scopesCreated++;
    PARSER_LOG_DEBUG("Entered scope: " + scopes.back()->getName() + 
                    " (level " + std::to_string(currentScopeLevel) + ")");
}

void ParserStateManager::exitScope() {
    if (currentScopeLevel > 0) {
        auto scopeName = scopes.back()->getName();
        scopes.pop_back();
        currentScopeLevel--;
        
        PARSER_LOG_DEBUG("Exited scope: " + scopeName + 
                        " (level " + std::to_string(currentScopeLevel + 1) + ")");
    } else {
        PARSER_LOG_WARNING("Attempted to exit global scope");
    }
}

void ParserStateManager::declareSymbol(const std::string& name, ASTNodeType type, const Position& pos) {
    if (!scopes.empty()) {
        scopes.back()->addSymbol(name, type, pos);
        statistics.symbolsDeclarated++;
        
        PARSER_LOG_DEBUG("Declared symbol: " + name + " in scope " + scopes.back()->getName());
    } else {
        PARSER_LOG_ERROR("No scope available for symbol declaration: " + name);
    }
}

const Symbol* ParserStateManager::lookupSymbol(const std::string& name) const {
    // Search from current scope backwards to global scope
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

void ParserStateManager::addError(std::unique_ptr<ParseError> error) {
    if (error) {
        errors.push_back(std::move(error));
        statistics.totalErrors++;
        
        PARSER_LOG_ERROR("Parse error added at position " + 
                        std::to_string(currentTokenPosition));
    }
}

// getErrors() and clearErrors() are already defined inline in the header

void ParserStateManager::setRecoveryPoint(size_t tokenPos, const std::string& description) {
    recoveryPoints.emplace_back(tokenPos, currentState, currentScopeLevel, description);
    
    PARSER_LOG_DEBUG("Set recovery point: " + description + " at token " + std::to_string(tokenPos));
}

const RecoveryPoint* ParserStateManager::getLastRecoveryPoint() const {
    return recoveryPoints.empty() ? nullptr : &recoveryPoints.back();
}

// clearRecoveryPoints() is already defined inline in the header

void ParserStateManager::startParsing() {
    statistics.startTime = std::chrono::high_resolution_clock::now();
    addState(ParserState::NORMAL);
    
    PARSER_LOG_INFO("Started parsing session");
}

void ParserStateManager::endParsing() {
    statistics.endTime = std::chrono::high_resolution_clock::now();
    currentState = ParserState::NORMAL;
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        statistics.endTime - statistics.startTime).count();
    
    PARSER_LOG_INFO("Ended parsing session (duration: " + std::to_string(duration) + "ms)");
}

// getStatistics() is already defined inline in the header

void ParserStateManager::resetStatistics() {
    statistics = ParseStatistics{};
    statistics.startTime = std::chrono::high_resolution_clock::now();
    statistics.endTime = statistics.startTime;
}

bool ParserStateManager::isValidState() const {
    // Check basic state consistency
    if (currentScopeLevel != scopes.size() - 1) {
        return false;
    }
    
    if (parseDepth != contextStack.size()) {
        return false;
    }
    
    // Check scope hierarchy
    for (size_t i = 0; i < scopes.size(); ++i) {
        if (scopes[i]->getLevel() != i) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ParserStateManager::validateState() const {
    std::vector<std::string> issues;
    
    if (currentScopeLevel != scopes.size() - 1) {
        issues.push_back("Scope level mismatch: current=" + std::to_string(currentScopeLevel) + 
                        ", scopes=" + std::to_string(scopes.size() - 1));
    }
    
    if (parseDepth != contextStack.size()) {
        issues.push_back("Parse depth mismatch: depth=" + std::to_string(parseDepth) + 
                        ", stack=" + std::to_string(contextStack.size()));
    }
    
    for (size_t i = 0; i < scopes.size(); ++i) {
        if (scopes[i]->getLevel() != i) {
            issues.push_back("Scope " + std::to_string(i) + " has wrong level: " + 
                           std::to_string(scopes[i]->getLevel()));
        }
    }
    
    return issues;
}

std::string ParserStateManager::getStateDescription() const {
    std::ostringstream oss;
    
    oss << "Parser State:\n";
    oss << "  Current State: " << stateToString(currentState) << "\n";
    oss << "  Token Position: " << currentTokenPosition << "\n";
    oss << "  Parse Depth: " << parseDepth << "\n";
    oss << "  Scope Level: " << currentScopeLevel << "\n";
    oss << "  Errors: " << errors.size() << "\n";
    oss << "  Recovery Points: " << recoveryPoints.size() << "\n";
    
    return oss.str();
}

void ParserStateManager::dumpState() const {
    PARSER_LOG_INFO(getStateDescription());
}

void ParserStateManager::dumpSymbolTable() const {
    std::ostringstream oss;
    oss << "Symbol Table:\n";
    
    for (const auto& scope : scopes) {
        oss << "  Scope " << scope->getLevel() << " (" << scope->getName() << "):\n";
        
        for (const auto& [name, symbol] : scope->getSymbols()) {
            oss << "    " << name << " (type: " << static_cast<int>(symbol->type) << ")\n";
        }
    }
    
    PARSER_LOG_INFO(oss.str());
}

void ParserStateManager::dumpContextStack() const {
    std::ostringstream oss;
    oss << "Context Stack (depth: " << parseDepth << "):\n";
    
    auto stack_copy = contextStack;
    std::vector<ParseContext> contexts;
    
    while (!stack_copy.empty()) {
        contexts.push_back(stack_copy.top());
        stack_copy.pop();
    }
    
    for (auto it = contexts.rbegin(); it != contexts.rend(); ++it) {
        oss << "  " << it->contextName << " (state: " << stateToString(it->state) << 
               ", token: " << it->tokenPosition << ")\n";
    }
    
    PARSER_LOG_INFO(oss.str());
}

ParserStateManager::StateSnapshot ParserStateManager::takeSnapshot() const {
    StateSnapshot snapshot;
    snapshot.state = currentState;
    snapshot.tokenPosition = currentTokenPosition;
    snapshot.scopeLevel = currentScopeLevel;
    snapshot.parseDepth = parseDepth;
    
    // Capture context names
    auto stack_copy = contextStack;
    while (!stack_copy.empty()) {
        snapshot.contextNames.push_back(stack_copy.top().contextName);
        stack_copy.pop();
    }
    std::reverse(snapshot.contextNames.begin(), snapshot.contextNames.end());
    
    // Capture scope names
    for (const auto& scope : scopes) {
        snapshot.scopeNames.push_back(scope->getName());
    }
    
    return snapshot;
}

void ParserStateManager::restoreSnapshot(const StateSnapshot& snapshot) {
    // Note: This is a simplified restoration - in a full implementation,
    // we would need to restore the complete state including symbol tables
    currentState = snapshot.state;
    currentTokenPosition = snapshot.tokenPosition;
    parseDepth = snapshot.parseDepth;
    
    PARSER_LOG_DEBUG("Restored parser state from snapshot");
}

void ParserStateManager::updateStatistics() {
    statistics.totalTokensProcessed = currentTokenPosition;
    statistics.maxParseDepth = std::max(statistics.maxParseDepth, parseDepth);
}

std::string ParserStateManager::stateToString(ParserState state) const {
    std::vector<std::string> states;
    
    if (static_cast<uint32_t>(state) & static_cast<uint32_t>(ParserState::NORMAL)) {
        states.push_back("NORMAL");
    }
    if (static_cast<uint32_t>(state) & static_cast<uint32_t>(ParserState::IN_FUNCTION)) {
        states.push_back("IN_FUNCTION");
    }
    if (static_cast<uint32_t>(state) & static_cast<uint32_t>(ParserState::IN_LOOP)) {
        states.push_back("IN_LOOP");
    }
    if (static_cast<uint32_t>(state) & static_cast<uint32_t>(ParserState::IN_SWITCH)) {
        states.push_back("IN_SWITCH");
    }
    if (static_cast<uint32_t>(state) & static_cast<uint32_t>(ParserState::RECOVERING)) {
        states.push_back("RECOVERING");
    }
    
    if (states.empty()) {
        return "NONE";
    }
    
    std::string result = states[0];
    for (size_t i = 1; i < states.size(); ++i) {
        result += "|" + states[i];
    }
    return result;
}

// Token control methods implementation
const Token& ParserStateManager::getCurrentToken() const {
    if (!tokenStream) {
        static Token eofToken;
        return eofToken;
    }
    return tokenStream->current();
}

const Token& ParserStateManager::peekToken(size_t offset) const {
    if (!tokenStream) {
        static Token eofToken;
        return eofToken;
    }
    return tokenStream->peek(offset);
}

bool ParserStateManager::consumeToken() {
    if (!tokenStream) {
        return false;
    }
    
    bool result = tokenStream->advance();
    if (result) {
        advanceTokenPosition();
        updateStatistics();
    }
    return result;
}

bool ParserStateManager::isAtEnd() const {
    if (!tokenStream) {
        return true;
    }
    return tokenStream->isAtEnd();
}

// Enhanced error and warning tracking
void ParserStateManager::addWarning(const std::string& message, const Position& pos) {
    statistics.totalWarnings++;
    PARSER_LOG_WARNING("Warning at " + std::to_string(pos.line) + ":" + 
                      std::to_string(pos.column) + " - " + message);
}

// Performance profiling methods
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

std::vector<std::string> ParserStateManager::getSavedPositionLabels() const {
    std::vector<std::string> labels;
    for (const auto& pair : savedPositions) {
        labels.push_back(pair.first);
    }
    return labels;
}

// Position history and navigation
void ParserStateManager::pushPositionHistory() {
    positionHistory.push(currentTokenPosition);
    PARSER_LOG_DEBUG("Position pushed to history: " + std::to_string(currentTokenPosition));
}

bool ParserStateManager::popPositionHistory() {
    if (!positionHistory.empty()) {
        currentTokenPosition = positionHistory.top();
        positionHistory.pop();
        if (tokenStream) {
            tokenStream->setPosition(currentTokenPosition);
        }
        PARSER_LOG_DEBUG("Position popped from history: " + std::to_string(currentTokenPosition));
        return true;
    }
    return false;
}

void ParserStateManager::clearPositionHistory() {
    while (!positionHistory.empty()) {
        positionHistory.pop();
    }
    PARSER_LOG_DEBUG("Position history cleared");
}

size_t ParserStateManager::getPositionHistorySize() const {
    return positionHistory.size();
}

// Token range operations
bool ParserStateManager::seekToPosition(size_t position) {
    if (tokenStream && position < tokenStream->size()) {
        currentTokenPosition = position;
        tokenStream->setPosition(position);
        PARSER_LOG_DEBUG("Seeked to position: " + std::to_string(position));
        return true;
    }
    return false;
}

bool ParserStateManager::seekRelative(int offset) {
    if (tokenStream) {
        size_t newPos = static_cast<size_t>(static_cast<int>(currentTokenPosition) + offset);
        if (newPos < tokenStream->size()) {
            currentTokenPosition = newPos;
            tokenStream->setPosition(newPos);
            PARSER_LOG_DEBUG("Seeked relative by " + std::to_string(offset) + " to position: " + std::to_string(newPos));
            return true;
        }
    }
    return false;
}

SourceRange ParserStateManager::getTokenRange(size_t start, size_t end) const {
    if (tokenStream && start < tokenStream->size() && end <= tokenStream->size() && start <= end) {
        auto tokens = tokenStream->getRange(start, end);
        if (!tokens.empty()) {
            // Convert Lexer::Position to Parser::Position
            auto frontPos = tokens.front().getPosition();
            auto backPos = tokens.back().getPosition();
            Position startPos(frontPos.line, frontPos.column, frontPos.offset);
            Position endPos(backPos.line, backPos.column, backPos.offset);
            return SourceRange(startPos, endPos);
        }
    }
    return SourceRange();
}

std::vector<Token> ParserStateManager::getTokensInRange(size_t start, size_t end) const {
    if (tokenStream && start < tokenStream->size() && end <= tokenStream->size() && start <= end) {
        return tokenStream->getRange(start, end);
    }
    return std::vector<Token>();
}

// Scoped classes are already defined inline in the header

} // namespace Parser