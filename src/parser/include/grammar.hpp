#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include "parser_types.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

namespace Parser {

// Forward declarations
class ASTNode;

// Grammar symbol types
enum class SymbolType {
    TERMINAL,
    NON_TERMINAL,
    EPSILON
};

// Grammar symbol
class GrammarSymbol {
public:
    GrammarSymbol(const std::string& name, SymbolType type)
        : symbolName(name), symbolType(type) {}
    
    virtual ~GrammarSymbol() = default;
    
    const std::string& getName() const { return symbolName; }
    SymbolType getType() const { return symbolType; }
    
    bool isTerminal() const { return symbolType == SymbolType::TERMINAL; }
    bool isNonTerminal() const { return symbolType == SymbolType::NON_TERMINAL; }
    bool isEpsilon() const { return symbolType == SymbolType::EPSILON; }
    
    virtual std::string toString() const { return symbolName; }
    
protected:
    std::string symbolName;
    SymbolType symbolType;
};

// Terminal symbol
class Terminal : public GrammarSymbol {
public:
    Terminal(const std::string& name, const std::string& pattern = "")
        : GrammarSymbol(name, SymbolType::TERMINAL), tokenPattern(pattern) {}
    
    const std::string& getPattern() const { return tokenPattern; }
    void setPattern(const std::string& pattern) { tokenPattern = pattern; }
    
    std::string toString() const override {
        return "'" + symbolName + "'";
    }
    
private:
    std::string tokenPattern;
};

// Non-terminal symbol
class NonTerminal : public GrammarSymbol {
public:
    NonTerminal(const std::string& name)
        : GrammarSymbol(name, SymbolType::NON_TERMINAL) {}
    
    std::string toString() const override {
        return "<" + symbolName + ">";
    }
};

// Production rule
class Production {
public:
    using ActionFunction = std::function<ASTNodePtr(const std::vector<ASTNodePtr>&)>;
    
    Production(const std::string& lhs, const std::vector<std::string>& rhs, int lineNumber = -1)
        : leftHandSide(lhs), rightHandSide(rhs), precedence(0), lineNumber(lineNumber) {}
    
    // Accessors
    const std::string& getLeftHandSide() const { return leftHandSide; }
    const std::vector<std::string>& getRightHandSide() const { return rightHandSide; }
    int getLineNumber() const { return lineNumber; }
    
    // Precedence and associativity
    void setPrecedence(int prec) { precedence = prec; }
    int getPrecedence() const { return precedence; }
    
    void setAssociativity(const std::string& assoc) { associativity = assoc; }
    const std::string& getAssociativity() const { return associativity; }
    
    // Semantic action
    void setAction(ActionFunction action) { semanticAction = action; }
    const ActionFunction& getAction() const { return semanticAction; }
    
    // Utility methods
    size_t getLength() const { return rightHandSide.size(); }
    bool isEmpty() const { return rightHandSide.empty(); }
    bool isEpsilonProduction() const {
        return rightHandSide.size() == 1 && rightHandSide[0] == "ε";
    }
    
    std::string toString() const;
    
    // Production comparison
    bool operator==(const Production& other) const {
        return leftHandSide == other.leftHandSide && rightHandSide == other.rightHandSide;
    }
    
private:
    std::string leftHandSide;
    std::vector<std::string> rightHandSide;
    int precedence;
    std::string associativity;
    ActionFunction semanticAction;
    int lineNumber;
};

// Grammar class
class Grammar {
public:
    Grammar();
    ~Grammar() = default;
    
    // Symbol management
    void addTerminal(const std::string& name, const std::string& pattern = "");
    void addNonTerminal(const std::string& name);
    
    bool isTerminal(const std::string& name) const;
    bool isNonTerminal(const std::string& name) const;
    bool isSymbol(const std::string& name) const;
    
    const std::unordered_set<std::string>& getTerminals() const { return terminals; }
    const std::unordered_set<std::string>& getNonTerminals() const { return nonTerminals; }
    
    // Production management
    void addProduction(const std::string& lhs, const std::vector<std::string>& rhs);
    void addProduction(const Production& production);
    
    const std::vector<Production>& getProductions() const { return productions; }
    std::vector<Production> getProductionsFor(const std::string& nonTerminal) const;
    
    // Rule management (Phase 2.1 requirements)
    Production getRule(size_t index) const;
    const std::vector<Production>& getRules() const { return productions; }
    bool hasRule(const std::string& lhs, const std::vector<std::string>& rhs) const;
    void addRule(const std::string& lhs, const std::vector<std::string>& rhs);
    bool removeRule(const std::string& lhs, const std::vector<std::string>& rhs);
    void clear();
    
    // Start symbol
    void setStartSymbol(const std::string& symbol) { startSymbol = symbol; }
    const std::string& getStartSymbol() const { return startSymbol; }
    
    // Precedence and associativity
    void setPrecedence(const std::string& symbol, int precedence, const std::string& associativity);
    int getPrecedence(const std::string& symbol) const;
    std::string getAssociativity(const std::string& symbol) const;
    
    // Grammar analysis
    std::unordered_set<std::string> computeFirst(const std::string& symbol) const;
    std::unordered_set<std::string> computeFirst(const std::vector<std::string>& symbols) const;
    std::unordered_set<std::string> computeFollow(const std::string& nonTerminal) const;
    
    // Grammar properties
    bool isLL1() const;
    bool isLR1() const;
    bool hasLeftRecursion() const;
    bool hasAmbiguity() const;
    
    // LL(1) conflict resolution
    bool resolveLL1Conflicts();
    bool resolveParameterDeclarationConflict();
    bool resolveDirectDeclaratorConflict();
    bool resolveEpsilonConflicts();
    
    // Grammar transformations
    void eliminateLeftRecursion();
    void leftFactor();
    void removeEpsilonProductions();
    void removeUselessSymbols();
    
    // Grammar validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Grammar loading/saving
    bool loadFromFile(const std::string& filename);
    bool loadFromString(const std::string& grammarText);
    bool saveToFile(const std::string& filename) const;
    
    // Serialization (Phase 2.1 requirement)
    std::string serialize() const;
    
    // Standard C grammars
    static Grammar createC89Grammar();
    static Grammar createC99Grammar();
    static Grammar createC11Grammar();
    static Grammar createC17Grammar();
    static Grammar createC23Grammar();
    
    // Grammar statistics
    struct GrammarStats {
        size_t terminalCount = 0;
        size_t nonTerminalCount = 0;
        size_t productionCount = 0;
        size_t maxProductionLength = 0;
        bool hasEpsilonProductions = false;
        bool hasLeftRecursion = false;
    };
    
    GrammarStats getStatistics() const;
    
    // Debug and introspection
    std::string toString() const;
    void dump() const;
    
private:
    std::unordered_set<std::string> terminals;
    std::unordered_set<std::string> nonTerminals;
    std::vector<Production> productions;
    std::string startSymbol;
    std::string grammarFilePath; // Path to the loaded grammar file
    
    // Terminal patterns
    std::unordered_map<std::string, std::string> terminalPatterns;
    
    // Precedence and associativity tables
    std::unordered_map<std::string, int> precedenceTable;
    std::unordered_map<std::string, std::string> associativityTable;
    
    // Cached analysis results
    mutable std::unordered_map<std::string, std::unordered_set<std::string>> firstSets;
    mutable std::unordered_map<std::string, std::unordered_set<std::string>> followSets;
    mutable bool firstSetsComputed = false;
    mutable bool followSetsComputed = false;
    
    // Helper methods
    void computeAllFirstSets() const;
    void computeAllFollowSets() const;
    void invalidateAnalysisCache();
    
    // Grammar transformation helpers
    std::vector<Production> findLeftRecursiveProductions(const std::string& nonTerminal) const;
    std::vector<std::vector<Production>> findFactorableProductions(const std::string& nonTerminal) const;
    void eliminateLeftRecursionForNonTerminal(const std::string& nonTerminal);
    void factorProductionGroup(const std::string& nonTerminal, const std::vector<Production>& group);
    void generateNullableCombinations(const std::vector<std::string>& rhs, 
                                     const std::unordered_set<std::string>& nullable,
                                     std::vector<std::vector<std::string>>& combinations) const;
    
    // Validation helpers
    bool checkSymbolConsistency() const;
    bool checkProductionValidity() const;
    bool checkStartSymbolExists() const;
    
    // Helper methods for grammar validation
    std::unordered_set<std::string> findReachableSymbols() const;
    std::unordered_set<std::string> findProductiveSymbols() const;
    std::unordered_set<std::string> findAllLeftRecursiveNonTerminals() const;
    bool hasCycleDFS(const std::string& node, 
                     const std::unordered_map<std::string, std::unordered_set<std::string>>& graph,
                     std::unordered_set<std::string>& visited,
                     std::unordered_set<std::string>& recursionStack) const;
    
    // Helper method for parsing grammar text
    void parseAndAddProduction(const std::string& lhs, const std::string& rhsStr, int lineNumber = -1);
};

// Grammar builder utility class
class GrammarBuilder {
public:
    GrammarBuilder();
    
    // Fluent interface for building grammars
    GrammarBuilder& terminal(const std::string& name, const std::string& pattern = "");
    GrammarBuilder& nonTerminal(const std::string& name);
    GrammarBuilder& production(const std::string& lhs, const std::vector<std::string>& rhs);
    GrammarBuilder& production(const std::string& lhs, const std::string& rhs); // Single symbol RHS
    GrammarBuilder& startSymbol(const std::string& symbol);
    GrammarBuilder& precedence(const std::string& symbol, int prec, const std::string& assoc = "left");
    
    // Build the final grammar
    Grammar build();
    
private:
    Grammar grammar;
};

// Operator precedence parser helper
class OperatorPrecedenceTable {
public:
    enum class Relation {
        LESS_THAN,      // <
        GREATER_THAN,   // >
        EQUAL,          // =
        UNDEFINED       // undefined relation
    };
    
    OperatorPrecedenceTable();
    
    void setRelation(const std::string& op1, const std::string& op2, Relation relation);
    Relation getRelation(const std::string& op1, const std::string& op2) const;
    
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
    // Build table from grammar
    void buildFromGrammar(const Grammar& grammar);
    
    std::string toString() const;
    
private:
    std::unordered_map<std::string, std::unordered_map<std::string, Relation>> table;
    
    std::string relationToString(Relation relation) const;
    Relation stringToRelation(const std::string& str) const;
};

} // namespace Parser

#endif // GRAMMAR_HPP