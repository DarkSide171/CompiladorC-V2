#ifndef PARSER_STATE_HPP
#define PARSER_STATE_HPP

#include "parser_types.hpp"
#include "token_stream.hpp"
#include <stack>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <chrono>

// Forward declare to avoid circular dependency
namespace Parser { class ParseError; }

namespace Parser {

// Forward declarations
class Grammar;

// Symbol table entry
struct Symbol {
    std::string name;
    ASTNodeType type;
    size_t scopeLevel;
    Position declaration;
    
    Symbol(const std::string& n, ASTNodeType t, size_t level, const Position& pos)
        : name(n), type(t), scopeLevel(level), declaration(pos) {}
};

// Scope management
class Scope {
public:
    Scope(size_t level, const std::string& name = "")
        : scopeLevel(level), scopeName(name) {}
    
    void addSymbol(const std::string& name, ASTNodeType type, const Position& pos) {
        symbols[name] = std::make_unique<Symbol>(name, type, scopeLevel, pos);
    }
    
    bool hasSymbol(const std::string& name) const {
        return symbols.find(name) != symbols.end();
    }
    
    const Symbol* getSymbol(const std::string& name) const {
        auto it = symbols.find(name);
        return it != symbols.end() ? it->second.get() : nullptr;
    }
    
    size_t getLevel() const { return scopeLevel; }
    const std::string& getName() const { return scopeName; }
    
    const std::unordered_map<std::string, std::unique_ptr<Symbol>>& getSymbols() const {
        return symbols;
    }
    
private:
    size_t scopeLevel;
    std::string scopeName;
    std::unordered_map<std::string, std::unique_ptr<Symbol>> symbols;
};

// Parser context stack entry
struct ParseContext {
    ParserState state;
    size_t tokenPosition;
    std::string contextName;
    
    ParseContext(ParserState s, size_t pos, const std::string& name)
        : state(s), tokenPosition(pos), contextName(name) {}
};

// Error recovery information
struct RecoveryPoint {
    size_t tokenPosition;
    ParserState state;
    size_t scopeLevel;
    std::string description;
    
    RecoveryPoint(size_t pos, ParserState s, size_t level, const std::string& desc)
        : tokenPosition(pos), state(s), scopeLevel(level), description(desc) {}
};

// Main parser state class
class ParserStateManager {
public:
    ParserStateManager();
    ~ParserStateManager() = default;
    
    // State management
    void setState(ParserState state) { currentState = state; }
    ParserState getState() const { return currentState; }
    
    void addState(ParserState state) {
        currentState = static_cast<ParserState>(
            static_cast<uint32_t>(currentState) | static_cast<uint32_t>(state)
        );
    }
    
    void removeState(ParserState state) {
        currentState = static_cast<ParserState>(
            static_cast<uint32_t>(currentState) & ~static_cast<uint32_t>(state)
        );
    }
    
    bool hasState(ParserState state) const {
        return (static_cast<uint32_t>(currentState) & static_cast<uint32_t>(state)) != 0;
    }
    
    // Context stack management
    void pushContext(const std::string& contextName, size_t tokenPos);
    void popContext();
    const ParseContext* getCurrentContext() const;
    size_t getContextDepth() const { return contextStack.size(); }
    
    // Scope management
    void enterScope(const std::string& scopeName = "");
    void exitScope();
    size_t getCurrentScopeLevel() const { return currentScopeLevel; }
    
    // Symbol table operations
    void declareSymbol(const std::string& name, ASTNodeType type, const Position& pos);
    const Symbol* lookupSymbol(const std::string& name) const;
    bool isSymbolDeclared(const std::string& name) const;
    bool isSymbolInCurrentScope(const std::string& name) const;
    
    // Error tracking
    void addError(std::unique_ptr<ParseError> error);
    const std::vector<std::unique_ptr<ParseError>>& getErrors() const { return errors; }
    size_t getErrorCount() const { return errors.size(); }
    bool hasErrors() const { return !errors.empty(); }
    void clearErrors();
    
    // Recovery points
    void setRecoveryPoint(size_t tokenPos, const std::string& description);
    const RecoveryPoint* getLastRecoveryPoint() const;
    void clearRecoveryPoints() { recoveryPoints.clear(); }
    
    // Token control methods
    void setTokenStream(TokenStream* stream) { tokenStream = stream; }
    const Token& getCurrentToken() const;
    const Token& peekToken(size_t offset = 1) const;
    bool consumeToken();
    bool isAtEnd() const;
    
    // Token position tracking
    void setCurrentTokenPosition(size_t position) { currentTokenPosition = position; }
    size_t getCurrentTokenPosition() const { return currentTokenPosition; }
    void advanceTokenPosition() { currentTokenPosition++; statistics.totalTokensProcessed++; }
    
    // Advanced positioning control
    void savePosition(const std::string& label = "");
    bool restorePosition(const std::string& label = "");
    void clearSavedPositions();
    std::vector<std::string> getSavedPositionLabels() const;
    
    // Position history and navigation
    void pushPositionHistory();
    bool popPositionHistory();
    void clearPositionHistory();
    size_t getPositionHistorySize() const;
    
    // Token range operations
    bool seekToPosition(size_t position);
    bool seekRelative(int offset);
    SourceRange getTokenRange(size_t start, size_t end) const;
    std::vector<Token> getTokensInRange(size_t start, size_t end) const;
    
    // Parse depth tracking
    void incrementDepth() { ++parseDepth; updateStatistics(); }
    void decrementDepth() { if (parseDepth > 0) { --parseDepth; updateStatistics(); } }
    size_t getParseDepth() const { return parseDepth; }
    void resetDepth() { parseDepth = 0; }
    
    // Token processing tracking
    void incrementTokensProcessed();
    
    // Statistics and diagnostics
    struct ParseStatistics {
        size_t totalTokensProcessed = 0;
        size_t maxParseDepth = 0;
        size_t totalErrors = 0;
        size_t totalWarnings = 0;
        size_t scopesCreated = 0;
        size_t symbolsDeclarated = 0;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
    };
    
    void startParsing();
    void endParsing();
    const ParseStatistics& getStatistics() const { return statistics; }
    void resetStatistics();
    
    // Reset all state
    void reset();
    
    // Enhanced error and warning tracking
    void addWarning(const std::string& message, const Position& pos);
    size_t getWarningCount() const { return statistics.totalWarnings; }
    
    // Performance profiling
    void startTimer(const std::string& operation);
    void endTimer(const std::string& operation);
    std::unordered_map<std::string, double> getTimingProfile() const;
    
    // Debug and profiling enhancements
    enum class DebugLevel {
        NONE = 0,
        BASIC = 1,
        DETAILED = 2,
        VERBOSE = 3
    };
    
    void enableDebugMode(bool enable = true) { debugModeEnabled = enable; }
    bool isDebugModeEnabled() const { return debugModeEnabled; }
    void setDebugLevel(DebugLevel level) { currentDebugLevel = level; }
    DebugLevel getDebugLevel() const { return currentDebugLevel; }
    
    // Debug information gathering
    std::string getDebugInfo() const;
    std::string getDetailedStateInfo() const;
    std::string getPerformanceReport() const;
    void logDebugInfo(const std::string& message) const;
    
    // Memory usage tracking
    size_t getMemoryUsage() const;
    void trackMemoryUsage(const std::string& operation);
    std::unordered_map<std::string, size_t> getMemoryProfile() const;
    
    // State validation
    bool isValidState() const;
    std::vector<std::string> validateState() const;
    
    // Debug and introspection
    std::string getStateDescription() const;
    void dumpState() const;
    void dumpSymbolTable() const;
    void dumpContextStack() const;
    
    // State serialization (for debugging/testing)
    struct StateSnapshot {
        ParserState state;
        size_t tokenPosition;
        size_t scopeLevel;
        size_t parseDepth;
        std::vector<std::string> contextNames;
        std::vector<std::string> scopeNames;
    };
    
    StateSnapshot takeSnapshot() const;
    void restoreSnapshot(const StateSnapshot& snapshot);
    
private:
    // Core state
    ParserState currentState;
    size_t currentTokenPosition;
    size_t parseDepth;
    
    // Token stream reference
    TokenStream* tokenStream;
    
    // Context management
    std::stack<ParseContext> contextStack;
    
    // Scope and symbol management
    std::vector<std::unique_ptr<Scope>> scopes;
    size_t currentScopeLevel;
    size_t nextScopeId;
    
    // Error handling
    std::vector<std::unique_ptr<ParseError>> errors;
    std::vector<RecoveryPoint> recoveryPoints;
    
    // Statistics
    ParseStatistics statistics;
    
    // Timing and profiling
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> timers;
    std::unordered_map<std::string, double> timingResults;
    
    // Advanced positioning support
    std::unordered_map<std::string, size_t> savedPositions;
    std::stack<size_t> positionHistory;
    size_t defaultSavedPositionCounter;
    
    // Debug and profiling data
    bool debugModeEnabled = false;
    DebugLevel currentDebugLevel = DebugLevel::NONE;
    std::unordered_map<std::string, size_t> memoryProfile;
    
    // Helper methods
    void updateStatistics();
    std::string stateToString(ParserState state) const;
};

// RAII helper for automatic scope management
class ScopedParserState {
public:
    ScopedParserState(ParserStateManager& manager, ParserState state)
        : stateManager(manager), previousState(manager.getState()) {
        stateManager.addState(state);
    }
    
    ~ScopedParserState() {
        stateManager.setState(previousState);
    }
    
private:
    ParserStateManager& stateManager;
    ParserState previousState;
};

// RAII helper for automatic scope entry/exit
class ScopedScope {
public:
    ScopedScope(ParserStateManager& manager, const std::string& scopeName = "")
        : stateManager(manager) {
        stateManager.enterScope(scopeName);
    }
    
    ~ScopedScope() {
        stateManager.exitScope();
    }
    
private:
    ParserStateManager& stateManager;
};

// RAII helper for automatic context management
class ScopedContext {
public:
    ScopedContext(ParserStateManager& manager, const std::string& contextName, size_t tokenPos)
        : stateManager(manager) {
        stateManager.pushContext(contextName, tokenPos);
    }
    
    ~ScopedContext() {
        stateManager.popContext();
    }
    
private:
    ParserStateManager& stateManager;
};

} // namespace Parser

#endif // PARSER_STATE_HPP