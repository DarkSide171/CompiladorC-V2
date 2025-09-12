#include "../include/grammar.hpp"
#include "../include/parser_logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <map>

namespace Parser {

// Função utilitária para encontrar o diretório raiz do projeto
std::string findProjectRoot() {
    // Procura por arquivos característicos do projeto
    const std::vector<std::string> project_markers = {
        "CMakeLists.txt", "build.sh", ".git", "README.md"
    };
    
    // Primeiro tenta usar o diretório atual
    char* cwd_ptr = getcwd(nullptr, 0);
    std::string current_dir = cwd_ptr ? std::string(cwd_ptr) : "";
    if (cwd_ptr) free(cwd_ptr);
    
    std::string search_dir = current_dir;
    
    // Sobe na hierarquia de diretórios até encontrar um marcador
    while (!search_dir.empty() && search_dir != "/") {
        for (const auto& marker : project_markers) {
            std::string marker_path = search_dir + "/" + marker;
            struct stat info;
            if (stat(marker_path.c_str(), &info) == 0) {
                return search_dir;
            }
        }
        
        // Sobe um nível
        size_t last_slash = search_dir.find_last_of('/');
        if (last_slash == std::string::npos || last_slash == 0) {
            break;
        }
        search_dir = search_dir.substr(0, last_slash);
    }
    
    // Fallback: usa um caminho hardcoded conhecido do projeto
    // Isso garante que funcione mesmo quando executado de qualquer lugar
    std::string hardcoded_path = "/Users/Guilherme/CLionProjects/CompiladorC_v2";
    struct stat info;
    if (stat((hardcoded_path + "/CMakeLists.txt").c_str(), &info) == 0) {
        return hardcoded_path;
    }
    
    // Último fallback: retorna o diretório atual
    return current_dir;
}

// Função utilitária para construir caminho absoluto para arquivos de gramática
std::string getGrammarFilePath(const std::string& filename) {
    std::string project_root = findProjectRoot();
    return project_root + "/src/parser/data/grammar/" + filename;
}

// Production implementation
std::string Production::toString() const {
    std::ostringstream oss;
    oss << leftHandSide << " -> ";
    
    if (rightHandSide.empty()) {
        oss << "ε";
    } else {
        for (size_t i = 0; i < rightHandSide.size(); ++i) {
            if (i > 0) oss << " ";
            oss << rightHandSide[i];
        }
    }
    
    if (precedence != 0) {
        oss << " [prec: " << precedence << "]";
    }
    
    return oss.str();
}

// Grammar implementation
Grammar::Grammar() {}

void Grammar::addTerminal(const std::string& name, const std::string& pattern) {
    terminals.insert(name);
    if (!pattern.empty()) {
        terminalPatterns[name] = pattern;
    }
}

void Grammar::addNonTerminal(const std::string& name) {
    nonTerminals.insert(name);
}

bool Grammar::isTerminal(const std::string& name) const {
    return terminals.find(name) != terminals.end();
}

bool Grammar::isNonTerminal(const std::string& name) const {
    return nonTerminals.find(name) != nonTerminals.end();
}

bool Grammar::isSymbol(const std::string& name) const {
    return isTerminal(name) || isNonTerminal(name);
}

void Grammar::addProduction(const std::string& lhs, const std::vector<std::string>& rhs) {
    Production production(lhs, rhs);
    addProduction(production);
}

void Grammar::addProduction(const Production& production) {
    productions.push_back(production);
    
    // Ensure LHS is registered as non-terminal
    addNonTerminal(production.getLeftHandSide());
    
    // Ensure all RHS symbols are registered
    for (const auto& symbol : production.getRightHandSide()) {
        if (!isSymbol(symbol)) {
            // Default to terminal if not already defined
            addTerminal(symbol);
        }
    }
    
    invalidateAnalysisCache();
}

std::vector<Production> Grammar::getProductionsFor(const std::string& nonTerminal) const {
    std::vector<Production> result;
    for (const auto& production : productions) {
        if (production.getLeftHandSide() == nonTerminal) {
            result.push_back(production);
        }
    }
    return result;
}

// Phase 2.1 required methods
Production Grammar::getRule(size_t index) const {
    if (index >= productions.size()) {
        throw std::out_of_range("Rule index out of range: " + std::to_string(index));
    }
    return productions[index];
}

bool Grammar::hasRule(const std::string& lhs, const std::vector<std::string>& rhs) const {
    for (const auto& production : productions) {
        if (production.getLeftHandSide() == lhs && production.getRightHandSide() == rhs) {
            return true;
        }
    }
    return false;
}

void Grammar::addRule(const std::string& lhs, const std::vector<std::string>& rhs) {
    addProduction(lhs, rhs);
}

bool Grammar::removeRule(const std::string& lhs, const std::vector<std::string>& rhs) {
    auto it = std::find_if(productions.begin(), productions.end(),
        [&lhs, &rhs](const Production& prod) {
            return prod.getLeftHandSide() == lhs && prod.getRightHandSide() == rhs;
        });
    
    if (it != productions.end()) {
        productions.erase(it);
        invalidateAnalysisCache();
        return true;
    }
    return false;
}

void Grammar::clear() {
    terminals.clear();
    nonTerminals.clear();
    productions.clear();
    startSymbol.clear();
    precedenceTable.clear();
    associativityTable.clear();
    invalidateAnalysisCache();
}

void Grammar::setPrecedence(const std::string& symbol, int precedence, const std::string& associativity) {
    precedenceTable[symbol] = precedence;
    associativityTable[symbol] = associativity;
}

int Grammar::getPrecedence(const std::string& symbol) const {
    auto it = precedenceTable.find(symbol);
    return (it != precedenceTable.end()) ? it->second : 0;
}

std::string Grammar::getAssociativity(const std::string& symbol) const {
    auto it = associativityTable.find(symbol);
    return (it != associativityTable.end()) ? it->second : "none";
}

std::unordered_set<std::string> Grammar::computeFirst(const std::string& symbol) const {
    if (!firstSetsComputed) {
        computeAllFirstSets();
    }
    
    auto it = firstSets.find(symbol);
    return (it != firstSets.end()) ? it->second : std::unordered_set<std::string>();
}

std::unordered_set<std::string> Grammar::computeFirst(const std::vector<std::string>& symbols) const {
    std::unordered_set<std::string> result;
    
    for (const auto& symbol : symbols) {
        auto symbolFirst = computeFirst(symbol);
        
        // Add all non-epsilon symbols
        for (const auto& firstSymbol : symbolFirst) {
            if (firstSymbol != "ε") {
                result.insert(firstSymbol);
            }
        }
        
        // If this symbol doesn't have epsilon, stop
        if (symbolFirst.find("ε") == symbolFirst.end()) {
            break;
        }
        
        // If we've processed all symbols and all have epsilon
        if (&symbol == &symbols.back()) {
            result.insert("ε");
        }
    }
    
    return result;
}

std::unordered_set<std::string> Grammar::computeFollow(const std::string& nonTerminal) const {
    if (!followSetsComputed) {
        computeAllFollowSets();
    }
    
    auto it = followSets.find(nonTerminal);
    return (it != followSets.end()) ? it->second : std::unordered_set<std::string>();
}

bool Grammar::isLL1() const {
//    PARSER_LOG_INFO("Checking LL(1) property...");
    
    // Check LL(1) conditions for each non-terminal
    for (const auto& nt : nonTerminals) {
        auto prods = getProductionsFor(nt);
        
        if (prods.size() <= 1) {
            continue; // Single production is always LL(1)
        }
        
 //       PARSER_LOG_INFO("Checking non-terminal: " + nt + " with " + std::to_string(prods.size()) + " productions");
        
        // Condition 1: FIRST sets of productions must be disjoint
        std::unordered_set<std::string> allFirstSymbols;
        bool hasEpsilonProduction = false;
        std::vector<std::unordered_set<std::string>> productionFirstSets;
        
        for (size_t i = 0; i < prods.size(); ++i) {
            const auto& prod = prods[i];
            auto prodFirst = computeFirst(prod.getRightHandSide());
            productionFirstSets.push_back(prodFirst);
            
//            PARSER_LOG_INFO("  Production " + std::to_string(i) + ": " + prod.toString());
            std::string firstStr = "FIRST = {";
            for (const auto& f : prodFirst) {
                firstStr += f + ", ";
            }
            if (firstStr.size() > 8) firstStr = firstStr.substr(0, firstStr.size() - 2);
            firstStr += "}";
            // PARSER_LOG_INFO("    " + firstStr);
            
            // Check for epsilon production
            if (prodFirst.find("ε") != prodFirst.end()) {
                hasEpsilonProduction = true;
                prodFirst.erase("ε"); // Remove epsilon for intersection check
            }
            
            // Check for intersection in FIRST sets
            for (const auto& symbol : prodFirst) {
                if (allFirstSymbols.find(symbol) != allFirstSymbols.end()) {
                    // Find which productions have this conflicting symbol
                    // Determine grammar type based on loaded grammar file or context
                    std::string grammarType = "";
                    if (!grammarFilePath.empty()) {
                        if (grammarFilePath.find("c89") != std::string::npos) grammarType = " [C89 Grammar]";
                        else if (grammarFilePath.find("c99") != std::string::npos) grammarType = " [C99 Grammar]";
                        else if (grammarFilePath.find("c11") != std::string::npos) grammarType = " [C11 Grammar]";
                        else if (grammarFilePath.find("c17") != std::string::npos) grammarType = " [C17 Grammar]";
                        else if (grammarFilePath.find("c23") != std::string::npos) grammarType = " [C23 Grammar]";
                        else grammarType = " [Grammar: " + grammarFilePath + "]";
                    }
                    
                    std::string conflictInfo = "LL(1) conflict" + grammarType + ": Symbol '" + symbol + "' appears in FIRST sets of multiple productions for non-terminal '" + nt + "'";
                    
                    // Debug: log line numbers (removed for cleaner output)
                    
                    // Add line number information for current production
                    if (prod.getLineNumber() != -1) {
                        conflictInfo += "\n  Current production at line " + std::to_string(prod.getLineNumber()) + ": " + prod.toString();
                    } else {
                        conflictInfo += "\n  Current production (no line info): " + prod.toString();
                    }
                    
                    // Find and show the other conflicting production(s)
                    for (size_t j = 0; j < i; ++j) {
                        const auto& otherProd = prods[j];
                        auto otherFirst = productionFirstSets[j];
                        if (otherFirst.find(symbol) != otherFirst.end()) {
                            // Debug: other production line number (removed for cleaner output)
                            if (otherProd.getLineNumber() != -1) {
                                conflictInfo += "\n  Conflicting production at line " + std::to_string(otherProd.getLineNumber()) + ": " + otherProd.toString();
                            } else {
                                conflictInfo += "\n  Conflicting production (no line info): " + otherProd.toString();
                            }
                        }
                    }
                    
                    // PARSER_LOG_ERROR(conflictInfo);
                    return false; // FIRST sets not disjoint
                }
                allFirstSymbols.insert(symbol);
            }
        }
        
        // Condition 2: If epsilon is in FIRST(A), then FIRST(A) ∩ FOLLOW(A) = ∅
        if (hasEpsilonProduction) {
            auto followSet = computeFollow(nt);
            // PARSER_LOG_INFO("  Non-terminal " + nt + " has epsilon production");
            std::string followStr = "FOLLOW = {";
            for (const auto& f : followSet) {
                followStr += f + ", ";
            }
            if (followStr.size() > 10) followStr = followStr.substr(0, followStr.size() - 2);
            followStr += "}";
            // PARSER_LOG_INFO("    " + followStr);
            
            for (const auto& symbol : allFirstSymbols) {
                if (followSet.find(symbol) != followSet.end()) {
                    std::string conflictInfo = "LL(1) conflict: Symbol '" + symbol + "' appears in both FIRST and FOLLOW sets for non-terminal '" + nt + "'";
                    
                    // Find which production(s) contribute this symbol to FIRST set
                    for (size_t i = 0; i < prods.size(); ++i) {
                        const auto& prod = prods[i];
                        auto prodFirst = productionFirstSets[i];
                        if (prodFirst.find(symbol) != prodFirst.end()) {
                            if (prod.getLineNumber() != -1) {
                                conflictInfo += "\n  Production contributing to FIRST at line " + std::to_string(prod.getLineNumber()) + ": " + prod.toString();
                            } else {
                                conflictInfo += "\n  Production contributing to FIRST: " + prod.toString();
                            }
                        }
                    }
                    
                    PARSER_LOG_ERROR(conflictInfo);
                    return false; // FIRST and FOLLOW sets not disjoint
                }
            }
        }
    }
    
    // PARSER_LOG_INFO("Grammar is LL(1)");
    return true;
}

bool Grammar::isLR1() const {
    // PARSER_LOG_INFO("Checking if grammar is LR(1)");
    
    // Basic necessary conditions for LR(1)
    
    // 1. No left recursion
    if (hasLeftRecursion()) {
        PARSER_LOG_INFO("Grammar has left recursion - not LR(1)");
        return false;
    }
    
    // 2. Check for shift-reduce conflicts by examining FIRST/FOLLOW sets
    for (const auto& nonTerminal : nonTerminals) {
        auto productions = getProductionsFor(nonTerminal);
        
        // Check for reduce-reduce conflicts
        for (size_t i = 0; i < productions.size(); i++) {
            for (size_t j = i + 1; j < productions.size(); j++) {
                const auto& prod1 = productions[i];
                const auto& prod2 = productions[j];
                
                // If both productions can be reduced in the same context
                auto follow = computeFollow(nonTerminal);
                
                // For LR(1), we need to ensure no conflicts in lookahead
                // This is a simplified check - full LR(1) would need item sets
                if (prod1.getRightHandSide() != prod2.getRightHandSide()) {
                    // Different productions for same non-terminal
                    // Check if they can conflict based on FIRST sets
                    auto first1 = computeFirst(prod1.getRightHandSide());
                    auto first2 = computeFirst(prod2.getRightHandSide());
                    
                    // Check for intersection in FIRST sets
                    for (const auto& symbol1 : first1) {
                        if (symbol1 != "ε" && first2.find(symbol1) != first2.end()) {
                            // PARSER_LOG_INFO("Potential shift-reduce conflict detected for " + nonTerminal);
                            return false;
                        }
                    }
                }
            }
        }
    }
    
    // 3. Check for ambiguous productions (simplified)
    if (hasAmbiguity()) {
        // PARSER_LOG_INFO("Grammar has ambiguity - not LR(1)");
        return false;
    }
    
    // 4. Ensure grammar is well-formed
    if (!validate()) {
        // PARSER_LOG_INFO("Grammar validation failed - not LR(1)");
        return false;
    }
    
    // PARSER_LOG_INFO("Grammar appears to be LR(1) based on basic checks");
    return true;
}

bool Grammar::hasLeftRecursion() const {
    for (const auto& nt : nonTerminals) {
        if (!findLeftRecursiveProductions(nt).empty()) {
            return true;
        }
    }
    return false;
}

bool Grammar::hasAmbiguity() const {
    // PARSER_LOG_INFO("Checking grammar for ambiguity");
    
    // Check for different types of ambiguity
    
    // 1. Check for shift-reduce conflicts (common source of ambiguity)
    for (const auto& nonTerminal : nonTerminals) {
        auto productions = getProductionsFor(nonTerminal);
        
        // Check for productions that can derive the same string in multiple ways
        for (size_t i = 0; i < productions.size(); i++) {
            for (size_t j = i + 1; j < productions.size(); j++) {
                const auto& prod1 = productions[i];
                const auto& prod2 = productions[j];
                
                // Check if productions have overlapping FIRST sets
                auto first1 = computeFirst(prod1.getRightHandSide());
                auto first2 = computeFirst(prod2.getRightHandSide());
                
                // Find intersection
                for (const auto& symbol : first1) {
                    if (symbol != "ε" && first2.find(symbol) != first2.end()) {
                        // PARSER_LOG_INFO("Potential ambiguity detected in " + nonTerminal + 
                                    //    ": overlapping FIRST sets for productions");
 //                       PARSER_LOG_INFO("  Production 1: " + prod1.toString());
 //                       PARSER_LOG_INFO("  Production 2: " + prod2.toString());
 //                       PARSER_LOG_INFO("  Overlapping symbol: " + symbol);
                        
                        // This indicates potential ambiguity, but not necessarily fatal
                        // Continue checking for more severe cases
                    }
                }
            }
        }
    }
    
    // 2. Check for dangling else problem (classic ambiguity)
    for (const auto& production : productions) {
        const auto& rhs = production.getRightHandSide();
        // Look for if-then-else patterns that could be ambiguous
        for (size_t i = 0; i < rhs.size(); i++) {
            if (rhs[i] == "if" && i + 2 < rhs.size()) {
                // Check for nested if without explicit else binding
                if (std::find(rhs.begin() + i, rhs.end(), "else") != rhs.end()) {
                    // PARSER_LOG_INFO("Potential dangling else ambiguity detected");
                }
            }
        }
    }
    
    // 3. Check for operator precedence ambiguities
    for (const auto& nonTerminal : nonTerminals) {
        auto prods = getProductionsFor(nonTerminal);
        for (const auto& prod : prods) {
            const auto& rhs = prod.getRightHandSide();
            // Look for expression patterns that might be ambiguous
            if (rhs.size() >= 3) {
                for (size_t i = 1; i < rhs.size() - 1; i++) {
                    // Check for operator-like symbols
                    if (rhs[i] == "+" || rhs[i] == "-" || rhs[i] == "*" || rhs[i] == "/" ||
                        rhs[i] == "&&" || rhs[i] == "||" || rhs[i] == "==") {
                        // Check if precedence is defined
                        if (precedenceTable.find(rhs[i]) == precedenceTable.end()) {
                            // PARSER_LOG_INFO("Operator without defined precedence: " + rhs[i]);
                        }
                    }
                }
            }
        }
    }
    
    // 4. Final assessment
    bool isAmbiguous = false;
    
    // Critical ambiguities that prevent parsing
    if (!isLL1()) {
        // PARSER_LOG_INFO("Grammar is not LL(1) - attempting to resolve conflicts");
        
        // Create a non-const copy to allow modifications
        Grammar* mutableThis = const_cast<Grammar*>(this);
        
        // Try to resolve LL(1) conflicts automatically
        if (mutableThis->resolveLL1Conflicts()) {
            // PARSER_LOG_INFO("Successfully resolved some LL(1) conflicts");
            // Re-check after resolution attempts
            if (!isLL1()) {
                // PARSER_LOG_INFO("Some LL(1) conflicts remain after resolution attempts");
                isAmbiguous = true;
            } else {
                // PARSER_LOG_INFO("All LL(1) conflicts resolved - grammar is now LL(1)");
                isAmbiguous = false;
            }
        } else {
            // PARSER_LOG_INFO("Could not resolve LL(1) conflicts automatically");
            isAmbiguous = true;
        }
    }
    
    // Log final result
    if (isAmbiguous) {
        // PARSER_LOG_INFO("Grammar has significant ambiguity that may prevent parsing");
    } else {
        // PARSER_LOG_INFO("Grammar has manageable ambiguity or is unambiguous");
    }
    
    return isAmbiguous;
}
bool Grammar::resolveLL1Conflicts() {
    // PARSER_LOG_INFO("Attempting to resolve LL(1) conflicts");
    
    bool resolved = false;
    
    // Try to resolve parameter declaration conflicts
    if (resolveParameterDeclarationConflict()) {
        resolved = true;
        // PARSER_LOG_INFO("Resolved parameter declaration conflicts");
    }
    
    // Try to resolve direct declarator conflicts
    if (resolveDirectDeclaratorConflict()) {
        resolved = true;
        // PARSER_LOG_INFO("Resolved direct declarator conflicts");
    }
    
    // Try to resolve epsilon conflicts
    if (resolveEpsilonConflicts()) {
        resolved = true;
        // PARSER_LOG_INFO("Resolved epsilon conflicts");
    }
    
    // Apply left factoring to resolve FIRST conflicts
    std::unordered_set<std::string> processedNonTerminals;
    for (const auto& nonTerminal : nonTerminals) {
        if (processedNonTerminals.find(nonTerminal) != processedNonTerminals.end()) {
            continue;
        }
        
        auto factorableGroups = findFactorableProductions(nonTerminal);
        if (!factorableGroups.empty()) {
            for (const auto& group : factorableGroups) {
                if (group.size() > 1) {
                    factorProductionGroup(nonTerminal, group);
                    resolved = true;
                    // PARSER_LOG_INFO("Applied left factoring to " + nonTerminal);
                }
            }
        }
        processedNonTerminals.insert(nonTerminal);
    }
    
    // Try to resolve direct declarator conflicts again after left factoring
    // This is needed because factored non-terminals are created during left factoring
    if (resolveDirectDeclaratorConflict()) {
        resolved = true;
        // PARSER_LOG_INFO("Resolved direct declarator conflicts after left factoring");
    }
    
    if (resolved) {
        invalidateAnalysisCache();
        // PARSER_LOG_INFO("LL(1) conflict resolution completed");
    }
    
    return resolved;
}

bool Grammar::resolveParameterDeclarationConflict() {
    // PARSER_LOG_INFO("Attempting to resolve parameter_declaration conflicts");
    
    auto paramProductions = getProductionsFor("parameter_declaration");
    if (paramProductions.empty()) {
        // PARSER_LOG_INFO("No parameter_declaration productions found");
        return false;
    }
    
    // Found parameter_declaration productions (silent)
    
    bool resolved = false;
    
    // Find conflicting productions that start with declaration_specifiers
    std::vector<Production> conflictingProductions;
    for (const auto& prod : paramProductions) {
        const auto& rhs = prod.getRightHandSide();
        // Check production silently
        if (!rhs.empty() && rhs[0] == "declaration_specifiers") {
            conflictingProductions.push_back(prod);
        }
    }
    
    if (conflictingProductions.size() > 1) {
        // Remove the conflicting productions
        for (const auto& prod : conflictingProductions) {
            removeRule(prod.getLeftHandSide(), prod.getRightHandSide());
        }
        
        // Add a unified production that handles both cases
        // parameter_declaration -> declaration_specifiers parameter_declarator_opt
        addRule("parameter_declaration", {"declaration_specifiers", "parameter_declarator_opt"});
        
        // Add helper non-terminal for optional declarator
        addNonTerminal("parameter_declarator_opt");
        addRule("parameter_declarator_opt", {"declarator"});
        addRule("parameter_declarator_opt", {"abstract_declarator"});
        addRule("parameter_declarator_opt", {"epsilon"});
        
        // Resolved parameter_declaration conflict by introducing parameter_declarator_opt
        
        // Invalidate cache to ensure changes take effect
        invalidateAnalysisCache();
        
        resolved = true;
    }
    
    return resolved;
}

bool Grammar::resolveDirectDeclaratorConflict() {
    // Determine grammar type for logging
    std::string grammarType = "Unknown";
    if (this->grammarFilePath.find("c89") != std::string::npos) {
        grammarType = "C89";
    } else if (this->grammarFilePath.find("c99") != std::string::npos) {
        grammarType = "C99";
    } else if (this->grammarFilePath.find("c11") != std::string::npos) {
        grammarType = "C11";
    } else if (this->grammarFilePath.find("c17") != std::string::npos) {
        grammarType = "C17";
    } else if (this->grammarFilePath.find("c23") != std::string::npos) {
        grammarType = "C23";
    }
    
    ParserLogger::getInstance().info("=== INICIANDO resolveDirectDeclaratorConflict [" + grammarType + "] ===");
    
    // Attempting to resolve direct_declarator conflicts by removing problematic factored productions
    
    bool resolved = false;
    
    // Debug: Listar todos os non-terminais existentes
    ParserLogger::getInstance().info("Non-terminais existentes na gramática (" + std::to_string(nonTerminals.size()) + " total):");
    for (const auto& nt : nonTerminals) {
        if (nt.find("direct_declarator") != std::string::npos) {
            ParserLogger::getInstance().info("  - [DIRECT_DECLARATOR] " + nt);
        }
    }
    
    // Find and remove all factored productions that cause LL(1) conflicts
    std::vector<std::string> factored_nonterminals_to_remove;
    ParserLogger::getInstance().info("Procurando non-terminais factored conflitantes...");
    
    for (const auto& nonTerminal : nonTerminals) {
        if (nonTerminal.find("factored") != std::string::npos) {
            
            ParserLogger::getInstance().info("Analisando non-terminal factored: " + nonTerminal);
            auto productions = getProductionsFor(nonTerminal);
            ParserLogger::getInstance().info("  - Encontradas " + std::to_string(productions.size()) + " produções");
            
            // Check if this factored non-terminal has LL(1) conflicts
            // by computing FIRST sets for each production and checking FIRST/FOLLOW conflicts
            std::unordered_set<std::string> allFirstSymbols;
            bool hasConflict = false;
            
            for (const auto& prod : productions) {
                const auto& rhs = prod.getRightHandSide();
                std::string rhsStr = "";
                for (size_t i = 0; i < rhs.size(); ++i) {
                    if (i > 0) rhsStr += " ";
                    rhsStr += rhs[i];
                }
                ParserLogger::getInstance().info("    Produção: " + nonTerminal + " -> " + rhsStr);
                
                // Compute FIRST set for this production
                auto firstSet = computeFirst(rhs);
                
                // Check for conflicts with previous productions
                for (const auto& symbol : firstSet) {
                    if (allFirstSymbols.find(symbol) != allFirstSymbols.end()) {
                        hasConflict = true;
                        ParserLogger::getInstance().info("      [CONFLITO FIRST/FIRST] Símbolo '" + symbol + "' aparece em múltiplas produções");
                        break;
                    }
                }
                
                // Add symbols to the set
                allFirstSymbols.insert(firstSet.begin(), firstSet.end());
                
                if (hasConflict) break;
            }
            
            // Also check for FIRST/FOLLOW conflicts
            if (!hasConflict) {
                auto followSet = computeFollow(nonTerminal);
                for (const auto& firstSymbol : allFirstSymbols) {
                    if (followSet.find(firstSymbol) != followSet.end()) {
                        hasConflict = true;
                        ParserLogger::getInstance().info("      [CONFLITO FIRST/FOLLOW] Símbolo '" + firstSymbol + "' aparece em FIRST e FOLLOW");
                        break;
                    }
                }
            }
            
            if (hasConflict) {
                factored_nonterminals_to_remove.push_back(nonTerminal);
                ParserLogger::getInstance().info("  [CONFLITO DETECTADO - " + grammarType + "] Non-terminal conflitante: " + nonTerminal);
            } else {
                ParserLogger::getInstance().info("  [OK] Sem conflito detectado em: " + nonTerminal);
            }
        }
    }
    
    ParserLogger::getInstance().info("Total de non-terminais conflitantes encontrados: " + std::to_string(factored_nonterminals_to_remove.size()));
    
    if (factored_nonterminals_to_remove.empty()) {
        ParserLogger::getInstance().info("AVISO: Nenhum non-terminal factored conflitante encontrado!");
        
        // Debug: Procurar especificamente por direct_declarator_rest_factored_326
        std::string targetNT = "direct_declarator_rest_factored_326";
        ParserLogger::getInstance().info("Procurando especificamente por: " + targetNT);
        
        auto ntIt = std::find(nonTerminals.begin(), nonTerminals.end(), targetNT);
        if (ntIt != nonTerminals.end()) {
            ParserLogger::getInstance().info("  [ENCONTRADO] " + targetNT + " existe na gramática");
            auto targetProds = getProductionsFor(targetNT);
            ParserLogger::getInstance().info("  Produções de " + targetNT + ":");
            for (const auto& prod : targetProds) {
                const auto& rhs = prod.getRightHandSide();
                std::string rhsStr = "";
                for (size_t i = 0; i < rhs.size(); ++i) {
                    if (i > 0) rhsStr += " ";
                    rhsStr += rhs[i];
                }
                ParserLogger::getInstance().info("    " + targetNT + " -> " + rhsStr);
            }
        } else {
            ParserLogger::getInstance().info("  [NÃO ENCONTRADO] " + targetNT + " não existe na gramática");
        }
    }
    
    // Remove problematic factored non-terminals and their productions
    for (const auto& nonTerminal : factored_nonterminals_to_remove) {
        ParserLogger::getInstance().info("Removendo non-terminal conflitante: " + nonTerminal);
        auto productions = getProductionsFor(nonTerminal);
        ParserLogger::getInstance().info("  - Removendo " + std::to_string(productions.size()) + " produções");
        
        // Remove all productions for this non-terminal
        for (const auto& prod : productions) {
            bool removed = removeRule(prod.getLeftHandSide(), prod.getRightHandSide());
             ParserLogger::getInstance().info(std::string("    Remoção de produção: ") + (removed ? "SUCESSO" : "FALHOU"));
        }
        
        // Remove the non-terminal itself
         auto it = std::find(nonTerminals.begin(), nonTerminals.end(), nonTerminal);
         if (it != nonTerminals.end()) {
             nonTerminals.erase(it);
             ParserLogger::getInstance().info("  - Non-terminal removido com sucesso");
         } else {
             ParserLogger::getInstance().info("  - ERRO: Non-terminal não encontrado para remoção");
         }
        
        ParserLogger::getInstance().info("Removido non-terminal conflitante: " + nonTerminal);
        resolved = true;
    }
    
    // Now fix any productions that reference the removed factored non-terminals
    if (resolved) {
        for (const auto& removedNT : factored_nonterminals_to_remove) {
            // Find productions that reference this removed non-terminal
            std::vector<Production> productionsToFix;
            
            for (const auto& prod : productions) {
                const auto& rhs = prod.getRightHandSide();
                for (const auto& symbol : rhs) {
                    if (symbol == removedNT) {
                        productionsToFix.push_back(prod);
                        break;
                    }
                }
            }
            
            // Replace references to removed factored non-terminal with direct alternatives
            for (const auto& prod : productionsToFix) {
                const auto& lhs = prod.getLeftHandSide();
                const auto& rhs = prod.getRightHandSide();
                
                // Remove the old production
                removeRule(lhs, rhs);
                
                // Create new productions with direct alternatives
                std::vector<std::string> newRhs1 = rhs;
                std::vector<std::string> newRhs2 = rhs;
                
                // Replace the factored non-terminal with direct alternatives
                for (size_t i = 0; i < newRhs1.size(); ++i) {
                    if (newRhs1[i] == removedNT) {
                        newRhs1[i] = "parameter_type_list";
                        newRhs2[i] = "identifier_list";
                        break;
                    }
                }
                
                // Add both alternatives
                addRule(lhs, newRhs1);
                addRule(lhs, newRhs2);
                
                ParserLogger::getInstance().info("Fixed production referencing removed factored non-terminal");
            }
        }
        
        // Clean up unreachable non-terminals after removing factored ones
        removeUselessSymbols();
        invalidateAnalysisCache();
    }
    
    return resolved;
}

bool Grammar::resolveEpsilonConflicts() {
    // Attempting to resolve epsilon conflicts
    
    bool resolved = false;
    
    for (const auto& nonTerminal : nonTerminals) {
        auto productions = getProductionsFor(nonTerminal);
        
        bool hasEpsilon = false;
        for (const auto& prod : productions) {
            if (prod.getRightHandSide().empty() || 
                (prod.getRightHandSide().size() == 1 && prod.getRightHandSide()[0] == "epsilon")) {
                hasEpsilon = true;
                break;
            }
        }
        
        if (hasEpsilon && productions.size() > 1) {
            auto firstSet = computeFirst(nonTerminal);
            auto followSet = computeFollow(nonTerminal);
            
            std::unordered_set<std::string> intersection;
            for (const auto& symbol : firstSet) {
                if (followSet.find(symbol) != followSet.end()) {
                    intersection.insert(symbol);
                }
            }
            
            if (!intersection.empty()) {
                std::string conflictSymbols;
                for (const auto& sym : intersection) {
                    if (!conflictSymbols.empty()) conflictSymbols += ", ";
                    conflictSymbols += sym;
                 }
                // PARSER_LOG_INFO("Found FIRST/FOLLOW conflict in " + nonTerminal + 
                //               " with symbols: " + conflictSymbols);
                resolved = true;
            }
        }
    }
    
    return resolved;
}

void Grammar::eliminateLeftRecursion() {
    // PARSER_LOG_INFO("Starting automatic left recursion elimination");
    
    // Find all non-terminals with left recursion
    auto leftRecursiveNTs = findAllLeftRecursiveNonTerminals();
    
    for (const auto& nonTerminal : leftRecursiveNTs) {
        eliminateLeftRecursionForNonTerminal(nonTerminal);
    }
    
    // PARSER_LOG_INFO("Left recursion elimination completed");
}

void Grammar::eliminateLeftRecursionForNonTerminal(const std::string& nonTerminal) {
    // PARSER_LOG_INFO("Eliminating left recursion for non-terminal: " + nonTerminal);
    
    // Get all productions for this non-terminal
    auto allProductions = getProductionsFor(nonTerminal);
    
    // Separate left-recursive and non-left-recursive productions
    std::vector<Production> leftRecursive;
    std::vector<Production> nonLeftRecursive;
    
    for (const auto& prod : allProductions) {
        if (!prod.getRightHandSide().empty() && prod.getRightHandSide()[0] == nonTerminal) {
            leftRecursive.push_back(prod);
        } else {
            nonLeftRecursive.push_back(prod);
        }
    }
    
    if (leftRecursive.empty()) {
        return; // No left recursion to eliminate
    }
    
    // Create new non-terminal name with _rest suffix
    std::string restNonTerminal = nonTerminal + "_rest";
    
    // Add the new non-terminal
    addNonTerminal(restNonTerminal);
    
    // Remove all old productions for this non-terminal
    productions.erase(
        std::remove_if(productions.begin(), productions.end(),
            [&nonTerminal](const Production& p) {
                return p.getLeftHandSide() == nonTerminal;
            }),
        productions.end()
    );
    
    // Add new productions: A -> α A_rest for each non-left-recursive production A -> α
    for (const auto& prod : nonLeftRecursive) {
        std::vector<std::string> newRhs = prod.getRightHandSide();
        newRhs.push_back(restNonTerminal);
        addProduction(nonTerminal, newRhs);
    }
    
    // Add new productions: A_rest -> β A_rest for each left-recursive production A -> A β
    for (const auto& prod : leftRecursive) {
        std::vector<std::string> beta(prod.getRightHandSide().begin() + 1, prod.getRightHandSide().end());
        beta.push_back(restNonTerminal);
        addProduction(restNonTerminal, beta);
    }
    
    // Add epsilon production: A_rest -> ε
    addProduction(restNonTerminal, {"/* empty */"});
    
    // PARSER_LOG_INFO("Left recursion eliminated for " + nonTerminal + ", created " + restNonTerminal);
}

void Grammar::leftFactor() {
    // PARSER_LOG_INFO("Starting left factoring process");
    
    bool changed = true;
    int iterations = 0;
    const int maxIterations = 10;
    
    while (changed && iterations < maxIterations) {
        changed = false;
        iterations++;
        
        // Left factoring iteration (silent)
        
        // For each non-terminal, find productions that can be left-factored
        for (const auto& nt : nonTerminals) {
            auto factorableGroups = findFactorableProductions(nt);
            
            if (!factorableGroups.empty()) {
                // Found factorable productions (silent)
                
                for (const auto& group : factorableGroups) {
                    if (group.size() >= 2) {
                        factorProductionGroup(nt, group);
                        changed = true;
                    }
                }
            }
        }
    }
    
    if (iterations >= maxIterations) {
        PARSER_LOG_WARNING("Left factoring stopped after maximum iterations");
    }
    
    // Left factoring completed
}

void Grammar::removeEpsilonProductions() {
    // Starting epsilon production removal (silent)
    
    // Find all nullable non-terminals (can derive epsilon)
    std::unordered_set<std::string> nullable;
    bool changed = true;
    
    while (changed) {
        changed = false;
        
        for (const auto& prod : productions) {
            const auto& lhs = prod.getLeftHandSide();
            const auto& rhs = prod.getRightHandSide();
            
            // If already nullable, skip
            if (nullable.find(lhs) != nullable.end()) {
                continue;
            }
            
            // Check if this is an epsilon production
            if (prod.isEpsilonProduction()) {
                nullable.insert(lhs);
                changed = true;
                continue;
            }
            
            // Check if all symbols in RHS are nullable
            bool allNullable = true;
            for (const auto& symbol : rhs) {
                if (isNonTerminal(symbol) && nullable.find(symbol) == nullable.end()) {
                    allNullable = false;
                    break;
                }
                if (isTerminal(symbol)) {
                    allNullable = false;
                    break;
                }
            }
            
            if (allNullable) {
                nullable.insert(lhs);
                changed = true;
            }
        }
    }
    
    // PARSER_LOG_INFO("Found " + std::to_string(nullable.size()) + " nullable non-terminals");
    
    // Generate new productions without epsilon productions
    std::vector<Production> newProductions;
    
    for (const auto& prod : productions) {
        if (prod.isEpsilonProduction()) {
            // Skip epsilon productions (except for start symbol if needed)
            continue;
        }
        
        const auto& rhs = prod.getRightHandSide();
        std::vector<std::vector<std::string>> combinations;
        generateNullableCombinations(rhs, nullable, combinations);
        
        // Add all non-empty combinations
        for (const auto& combo : combinations) {
            if (!combo.empty()) {
                newProductions.emplace_back(prod.getLeftHandSide(), combo, prod.getLineNumber());
            }
        }
    }
    
    // Replace productions
    productions = newProductions;
    
    // PARSER_LOG_INFO("Epsilon production removal completed. New production count: " + std::to_string(productions.size()));
}

void Grammar::removeUselessSymbols() {
    // PARSER_LOG_INFO("Starting useless symbol removal");
    
    // Step 1: Find productive symbols (can derive terminal strings)
    auto productiveSymbols = findProductiveSymbols();
    // PARSER_LOG_INFO("Found " + std::to_string(productiveSymbols.size()) + " productive symbols");
    
    // Step 2: Remove non-productive symbols and their productions
    std::vector<Production> productiveProductions;
    for (const auto& prod : productions) {
        const auto& lhs = prod.getLeftHandSide();
        const auto& rhs = prod.getRightHandSide();
        
        // Check if LHS is productive
        if (productiveSymbols.find(lhs) == productiveSymbols.end()) {
            continue;
        }
        
        // Check if all RHS symbols are productive or terminals
        bool allProductive = true;
        for (const auto& symbol : rhs) {
            if (symbol == "ε") continue; // Epsilon is always "productive"
            
            if (isNonTerminal(symbol) && productiveSymbols.find(symbol) == productiveSymbols.end()) {
                allProductive = false;
                break;
            }
        }
        
        if (allProductive) {
            productiveProductions.push_back(prod);
        }
    }
    
    productions = productiveProductions;
    // PARSER_LOG_INFO("After removing non-productive symbols: " + std::to_string(productions.size()) + " productions");
    
    // Step 3: Find reachable symbols from start symbol
    auto reachableSymbols = findReachableSymbols();
    // PARSER_LOG_INFO("Found " + std::to_string(reachableSymbols.size()) + " reachable symbols");
    
    // Step 4: Remove unreachable symbols and their productions
    std::vector<Production> reachableProductions;
    for (const auto& prod : productions) {
        const auto& lhs = prod.getLeftHandSide();
        
        // Keep production if LHS is reachable
        if (reachableSymbols.find(lhs) != reachableSymbols.end()) {
            reachableProductions.push_back(prod);
        }
    }
    
    productions = reachableProductions;
    
    // Step 5: Update terminal and non-terminal sets
    std::unordered_set<std::string> usedTerminals;
    std::unordered_set<std::string> usedNonTerminals;
    
    for (const auto& prod : productions) {
        const auto& lhs = prod.getLeftHandSide();
        const auto& rhs = prod.getRightHandSide();
        
        if (isNonTerminal(lhs)) {
            usedNonTerminals.insert(lhs);
        }
        
        for (const auto& symbol : rhs) {
            if (symbol == "ε") continue;
            
            if (isTerminal(symbol)) {
                usedTerminals.insert(symbol);
            } else if (isNonTerminal(symbol)) {
                usedNonTerminals.insert(symbol);
            }
        }
    }
    
    terminals = usedTerminals;
    nonTerminals = usedNonTerminals;
    
    // PARSER_LOG_INFO("Useless symbol removal completed. Final counts: " + 
                //    std::to_string(productions.size()) + " productions, " +
                //    std::to_string(terminals.size()) + " terminals, " +
                //    std::to_string(nonTerminals.size()) + " non-terminals");
}

bool Grammar::validate() const {
    auto errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> Grammar::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Basic structural checks
    if (!checkStartSymbolExists()) {
        errors.push_back("No start symbol defined");
    }
    
    if (!checkSymbolConsistency()) {
        errors.push_back("Symbol consistency check failed - undefined symbols used in productions");
    }
    
    if (!checkProductionValidity()) {
        errors.push_back("Production validity check failed - invalid left-hand sides");
    }
    
    // Check for empty grammar
    if (productions.empty()) {
        errors.push_back("Grammar has no productions");
    }
    
    if (nonTerminals.empty()) {
        errors.push_back("Grammar has no non-terminals");
    }
    
    // Check for unreachable symbols
    auto reachableSymbols = findReachableSymbols();
    // PARSER_LOG_INFO("Start symbol: " + startSymbol);
    // PARSER_LOG_INFO("Total non-terminals: " + std::to_string(nonTerminals.size()));
    // PARSER_LOG_INFO("Total reachable symbols: " + std::to_string(reachableSymbols.size()));
    
    // Debug: Log all reachable symbols
    // std::string reachableList = "Reachable symbols: ";
    // for (const auto& symbol : reachableSymbols) {
    //     reachableList += symbol + ", ";
    // }
    // PARSER_LOG_INFO(reachableList);
    
    // Debug: Log all non-terminals
// */    std::string ntList = "Non-terminals: ";
//     for (const auto& nt : nonTerminals) {
//         ntList += nt + ", ";
//     }
//    PARSER_LOG_INFO(ntList);
    
    for (const auto& nt : nonTerminals) {
        if (reachableSymbols.find(nt) == reachableSymbols.end()) {
            PARSER_LOG_ERROR("Unreachable non-terminal detected: " + nt);
            errors.push_back("Unreachable non-terminal: " + nt);
        }
    }
    
    // Check for non-productive symbols
    auto productiveSymbols = findProductiveSymbols();
    for (const auto& nt : nonTerminals) {
        if (productiveSymbols.find(nt) == productiveSymbols.end()) {
            errors.push_back("Non-productive non-terminal: " + nt);
        }
    }
    
    // Check for left recursion
    if (hasLeftRecursion()) {
        auto leftRecursiveNTs = findAllLeftRecursiveNonTerminals();
        for (const auto& nt : leftRecursiveNTs) {
            errors.push_back("Left recursion detected in non-terminal: " + nt);
        }
    }
    
    // Check LL(1) property
    if (!isLL1()) {
        errors.push_back("Grammar is not LL(1) - conflicts in FIRST/FOLLOW sets");
    }
    
    return errors;
}

bool Grammar::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        PARSER_LOG_ERROR("Failed to open grammar file: " + filename);
        return false;
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    bool result = loadFromString(buffer.str());
    if (result) {
        grammarFilePath = filename;
        // PARSER_LOG_INFO("Loaded grammar from file: " + filename);
    }
    return result;
}

bool Grammar::loadFromString(const std::string& grammarText) {
    std::istringstream stream(grammarText);
    std::string line;
    std::string currentStartSymbol;
    std::string currentLhs;
    int lineNumber = 0;
    
    // Clear existing grammar
    terminals.clear();
    nonTerminals.clear();
    productions.clear();
    
    while (std::getline(stream, line)) {
        lineNumber++;
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Skip empty lines, comments, and grammar syntax elements
        if (line.empty() || line[0] == '#' || line == ";" || line == "|") {
            continue;
        }
        
        // Handle continuation lines that start with '|' (BNF alternatives)
        if (line[0] == '|' && !currentLhs.empty()) {
            std::string rhsStr = line.substr(1); // Remove the '|'
            parseAndAddProduction(currentLhs, rhsStr);
            continue;
        }
        
        // Handle START symbol declaration
        if (line.find("START:") == 0) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                currentStartSymbol = line.substr(colonPos + 1);
                currentStartSymbol.erase(0, currentStartSymbol.find_first_not_of(" \t"));
                currentStartSymbol.erase(currentStartSymbol.find_last_not_of(" \t") + 1);
                setStartSymbol(currentStartSymbol);
                addNonTerminal(currentStartSymbol);
            }
            continue;
        }
        
        // Check if this is a standalone alternative production
        if (line.find(':') == std::string::npos && line.find(" -> ") == std::string::npos && 
            !currentLhs.empty()) {
            // Check if it's a simple identifier, contains terminals, or is a sequence of symbols
            bool isAlternative = false;
            if (line.find_first_of(" \t|;") == std::string::npos) {
                // Simple identifier
                isAlternative = true;
            } else if (line[0] == '\'' || line.find("'\'") != std::string::npos) {
                // Line contains terminals in quotes
                isAlternative = true;
            } else if (line.find_first_not_of(" \t") != std::string::npos && 
                       line.find_first_of("#") == std::string::npos) {
                // Line contains symbols separated by spaces (no comments)
                // This is likely a production rule continuation
                isAlternative = true;
            }
            
            if (isAlternative) {
                parseAndAddProduction(currentLhs, line, lineNumber);
                continue;
            }
        }
        
        // Parse production rule (format: LHS: RHS | RHS | ...)
        // Find the first colon that's not inside quotes
        size_t colonPos = std::string::npos;
        bool inQuotes = false;
        for (size_t i = 0; i < line.length(); i++) {
            if (line[i] == '\'' && (i == 0 || line[i-1] != '\\')) {
                inQuotes = !inQuotes;
            } else if (line[i] == ':' && !inQuotes) {
                colonPos = i;
                break;
            }
        }
        
        if (colonPos == std::string::npos) {
            // Try alternative format (LHS -> RHS)
            size_t arrowPos = line.find(" -> ");
            if (arrowPos == std::string::npos) {
                PARSER_LOG_WARNING("Invalid production format: " + line);
                continue;
            }
            
            std::string lhsName = line.substr(0, arrowPos);
            std::string rhsStr = line.substr(arrowPos + 4);
            
            // Trim LHS
            lhsName.erase(0, lhsName.find_first_not_of(" \t"));
            lhsName.erase(lhsName.find_last_not_of(" \t") + 1);
            
            currentLhs = lhsName; // Store for potential continuation lines
            addNonTerminal(lhsName);
            parseAndAddProduction(lhsName, rhsStr, lineNumber);
        } else {
            // Standard format (LHS: RHS | RHS | ...)
            std::string lhsName = line.substr(0, colonPos);
            std::string rhsStr = line.substr(colonPos + 1);
            
            // Trim LHS
            lhsName.erase(0, lhsName.find_first_not_of(" \t"));
            lhsName.erase(lhsName.find_last_not_of(" \t") + 1);
            
            currentLhs = lhsName; // Store for potential continuation lines
            addNonTerminal(lhsName);
            
            // Split by '|' for multiple productions
            std::istringstream rhsStream(rhsStr);
            std::string singleRhs;
            
            while (std::getline(rhsStream, singleRhs, '|')) {
                parseAndAddProduction(lhsName, singleRhs, lineNumber);
            }
        }
    }
    
    // If no start symbol was explicitly set, use the first non-terminal
    if (startSymbol.empty() && !nonTerminals.empty()) {
        setStartSymbol(*nonTerminals.begin());
    }
    
    // PARSER_LOG_INFO("Loaded grammar from string with " + std::to_string(productions.size()) + " productions");
    return true;
}

void Grammar::parseAndAddProduction(const std::string& lhs, const std::string& rhsStr, int lineNumber) {
    // Parse production silently
    std::string trimmedRhs = rhsStr;
    trimmedRhs.erase(0, trimmedRhs.find_first_not_of(" \t"));
    trimmedRhs.erase(trimmedRhs.find_last_not_of(" \t") + 1);
    
    // Handle semicolon at end
    if (!trimmedRhs.empty() && trimmedRhs.back() == ';') {
        trimmedRhs.pop_back();
        trimmedRhs.erase(trimmedRhs.find_last_not_of(" \t") + 1);
    }
    
    // Parse RHS symbols
    std::istringstream iss(trimmedRhs);
    std::string symbolName;
    std::vector<std::string> rhsSymbols;
    
    while (iss >> symbolName) {
        // Process symbol silently
        // Handle quoted terminals
        if (symbolName.front() == '\'' && symbolName.back() == '\'') {
            // Terminal symbol
            std::string terminalName = symbolName.substr(1, symbolName.length() - 2);
            // Trim any whitespace from terminal name
            terminalName.erase(0, terminalName.find_first_not_of(" \t"));
            terminalName.erase(terminalName.find_last_not_of(" \t") + 1);
            addTerminal(terminalName);
            rhsSymbols.push_back(terminalName);
        } else if (symbolName == "ε" || symbolName == "epsilon") {
            // Empty production - don't add anything to RHS
            break;
        } else {
            // Check if it's a terminal or non-terminal
            // Terminals: lowercase identifiers, operators, keywords
            // Non-terminals: uppercase identifiers
            if (symbolName.length() == 1 && !std::isalpha(symbolName[0])) {
                // Single character operators like +, -, *, /, etc.
                addTerminal(symbolName);
            } else if (std::islower(symbolName[0]) || 
                       symbolName.find_first_of("+-*/=<>!&|^%~(){}[];,.") != std::string::npos) {
                // Lowercase identifiers or contains operators - treat as terminal
                addTerminal(symbolName);
            } else if (std::isupper(symbolName[0])) {
                // Uppercase identifiers - treat as non-terminal
                addNonTerminal(symbolName);
            } else {
                // Default to non-terminal for ambiguous cases
                addNonTerminal(symbolName);
            }
            rhsSymbols.push_back(symbolName);
        }
    }
    
    // Add production
    Production production(lhs, rhsSymbols, lineNumber);
    addProduction(production);
}

bool Grammar::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        PARSER_LOG_ERROR("Failed to create grammar file: " + filename);
        return false;
    }
    
    file << "# Grammar file generated by Parser\n";
    file << "# Start symbol: " << startSymbol << "\n\n";
    
    for (const auto& production : productions) {
        file << production.toString() << "\n";
    }
    
    file.close();
    // PARSER_LOG_INFO("Saved grammar to file: " + filename);
    return true;
}

Grammar Grammar::createC89Grammar() {
    Grammar grammar;
    std::string grammarPath = getGrammarFilePath("simple_c.grammar");
    
    if (!grammar.loadFromFile(grammarPath)) {
        ParserLogger::getInstance().error("Failed to load C89 grammar from file: " + grammarPath);
        // Fallback to minimal grammar
        GrammarBuilder builder;
        return builder
            .startSymbol("translation_unit")
            .nonTerminal("translation_unit")
            .nonTerminal("external_declaration")
            .terminal("IDENTIFIER")
            .terminal("CONSTANT")
            .production("translation_unit", std::vector<std::string>{"external_declaration"})
            .production("translation_unit", std::vector<std::string>{"translation_unit", "external_declaration"})
            .build();
    }
    
    ParserLogger::getInstance().info("Successfully loaded C89 grammar from file: " + grammarPath);
    return grammar;
}

Grammar Grammar::createC99Grammar() {
    Grammar grammar;
    std::string grammarPath = getGrammarFilePath("c99.grammar");
    
    if (!grammar.loadFromFile(grammarPath)) {
        ParserLogger::getInstance().error("Failed to load C99 grammar from file: " + grammarPath);
        // Fallback to C89 grammar
        return createC89Grammar();
    }
    
    ParserLogger::getInstance().info("Successfully loaded C99 grammar from file: " + grammarPath);
    return grammar;
}

Grammar Grammar::createC11Grammar() {
    Grammar grammar;
    std::string grammarPath = getGrammarFilePath("c11.grammar");
    
    if (!grammar.loadFromFile(grammarPath)) {
        ParserLogger::getInstance().error("Failed to load C11 grammar from file: " + grammarPath);
        // Fallback to C99 grammar
        return createC99Grammar();
    }
    
    ParserLogger::getInstance().info("Successfully loaded C11 grammar from file: " + grammarPath);
    return grammar;
}

Grammar Grammar::createC17Grammar() {
    Grammar grammar;
    std::string grammarPath = getGrammarFilePath("c17.grammar");
    
    if (!grammar.loadFromFile(grammarPath)) {
        ParserLogger::getInstance().error("Failed to load C17 grammar from file: " + grammarPath);
        // Fallback to C11 grammar
        return createC11Grammar();
    }
    
    ParserLogger::getInstance().info("Successfully loaded C17 grammar from file: " + grammarPath);
    return grammar;
}

Grammar Grammar::createC23Grammar() {
    Grammar grammar;
    std::string grammarPath = getGrammarFilePath("c23.grammar");
    
    if (!grammar.loadFromFile(grammarPath)) {
        ParserLogger::getInstance().error("Failed to load C23 grammar from file: " + grammarPath);
        // Fallback to C17 grammar
        return createC17Grammar();
    }
    
    ParserLogger::getInstance().info("Successfully loaded C23 grammar from file: " + grammarPath);
    return grammar;
}

Grammar::GrammarStats Grammar::getStatistics() const {
    GrammarStats stats;
    stats.terminalCount = terminals.size();
    stats.nonTerminalCount = nonTerminals.size();
    stats.productionCount = productions.size();
    
    for (const auto& production : productions) {
        stats.maxProductionLength = std::max(stats.maxProductionLength, production.getLength());
        if (production.isEmpty()) {
            stats.hasEpsilonProductions = true;
        }
    }
    
    stats.hasLeftRecursion = hasLeftRecursion();
    
    return stats;
}

std::string Grammar::serialize() const {
    std::ostringstream oss;
    
    // Serialize start symbol
    if (!startSymbol.empty()) {
        oss << "START: " << startSymbol << "\n";
    }
    
    // Serialize terminals
    oss << "\n# Terminals\n";
    for (const auto& terminal : terminals) {
        oss << "TERMINAL: " << terminal << "\n";
    }
    
    // Serialize non-terminals
    oss << "\n# Non-terminals\n";
    for (const auto& nonTerminal : nonTerminals) {
        oss << "NON_TERMINAL: " << nonTerminal << "\n";
    }
    
    // Serialize productions
    oss << "\n# Productions\n";
    for (const auto& production : productions) {
        oss << production.getLeftHandSide() << " -> ";
        const auto& rhs = production.getRightHandSide();
        if (rhs.empty()) {
            oss << "ε";
        } else {
            for (size_t i = 0; i < rhs.size(); ++i) {
                if (i > 0) oss << " ";
                oss << rhs[i];
            }
        }
        if (production.getPrecedence() != 0) {
            oss << " [prec: " << production.getPrecedence();
            if (!production.getAssociativity().empty()) {
                oss << ", assoc: " << production.getAssociativity();
            }
            oss << "]";
        }
        oss << "\n";
    }
    
    // Serialize precedence table
    if (!precedenceTable.empty()) {
        oss << "\n# Precedence\n";
        for (const auto& [symbol, prec] : precedenceTable) {
            oss << "PRECEDENCE: " << symbol << " " << prec;
            auto assocIt = associativityTable.find(symbol);
            if (assocIt != associativityTable.end()) {
                oss << " " << assocIt->second;
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

std::string Grammar::toString() const {
    std::ostringstream oss;
    oss << "Grammar:\n";
    oss << "Start Symbol: " << startSymbol << "\n";
    oss << "Terminals: " << terminals.size() << "\n";
    oss << "Non-terminals: " << nonTerminals.size() << "\n";
    oss << "Productions: " << productions.size() << "\n\n";
    
    for (const auto& production : productions) {
        oss << production.toString() << "\n";
    }
    
    return oss.str();
}

void Grammar::dump() const {
    std::cout << toString();
}

void Grammar::computeAllFirstSets() const {
    firstSets.clear();
    
    // Initialize FIRST sets for terminals
    for (const auto& terminal : terminals) {
        firstSets[terminal].insert(terminal);
    }
    
    // Compute FIRST sets for non-terminals
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (const auto& production : productions) {
            const auto& lhs = production.getLeftHandSide();
            const auto& rhs = production.getRightHandSide();
            
            auto& lhsFirst = firstSets[lhs];
            size_t oldSize = lhsFirst.size();
            
            if (rhs.empty()) {
                // Epsilon production
                lhsFirst.insert("ε");
            } else {
                // Process RHS symbols
                bool allHaveEpsilon = true;
                for (const auto& symbol : rhs) {
                    auto& symbolFirst = firstSets[symbol];
                    
                    // Add non-epsilon symbols
                    for (const auto& firstSymbol : symbolFirst) {
                        if (firstSymbol != "ε") {
                            lhsFirst.insert(firstSymbol);
                        }
                    }
                    
                    // Check if symbol has epsilon
                    if (symbolFirst.find("ε") == symbolFirst.end()) {
                        allHaveEpsilon = false;
                        break;
                    }
                }
                
                // If all symbols have epsilon, add epsilon to LHS
                if (allHaveEpsilon) {
                    lhsFirst.insert("ε");
                }
            }
            
            if (lhsFirst.size() > oldSize) {
                changed = true;
            }
        }
    }
    
    firstSetsComputed = true;
}

void Grammar::computeAllFollowSets() const {
    followSets.clear();
    
    // Add $ to FOLLOW of start symbol
    if (!startSymbol.empty()) {
        followSets[startSymbol].insert("$");
    }
    
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (const auto& production : productions) {
            const auto& rhs = production.getRightHandSide();
            
            for (size_t i = 0; i < rhs.size(); ++i) {
                const auto& symbol = rhs[i];
                
                if (isNonTerminal(symbol)) {
                    auto& symbolFollow = followSets[symbol];
                    size_t oldSize = symbolFollow.size();
                    
                    // Look at symbols after this one
                    std::vector<std::string> beta(rhs.begin() + i + 1, rhs.end());
                    auto betaFirst = computeFirst(beta);
                    
                    // Add FIRST(beta) - {ε} to FOLLOW(symbol)
                    for (const auto& firstSymbol : betaFirst) {
                        if (firstSymbol != "ε") {
                            symbolFollow.insert(firstSymbol);
                        }
                    }
                    
                    // If ε ∈ FIRST(beta), add FOLLOW(LHS) to FOLLOW(symbol)
                    if (beta.empty() || betaFirst.find("ε") != betaFirst.end()) {
                        const auto& lhsFollow = followSets[production.getLeftHandSide()];
                        symbolFollow.insert(lhsFollow.begin(), lhsFollow.end());
                    }
                    
                    if (symbolFollow.size() > oldSize) {
                        changed = true;
                    }
                }
            }
        }
    }
    
    followSetsComputed = true;
}

void Grammar::invalidateAnalysisCache() {
    firstSetsComputed = false;
    followSetsComputed = false;
    firstSets.clear();
    followSets.clear();
}

std::vector<Production> Grammar::findLeftRecursiveProductions(const std::string& nonTerminal) const {
    std::vector<Production> result;
    
    for (const auto& production : productions) {
        if (production.getLeftHandSide() == nonTerminal && 
            !production.getRightHandSide().empty() &&
            production.getRightHandSide()[0] == nonTerminal) {
            result.push_back(production);
            // Log the line number where left recursion was found
            if (production.getLineNumber() != -1) {
                PARSER_LOG_ERROR("Left recursion detected in non-terminal '" + nonTerminal + "' at line " + std::to_string(production.getLineNumber()));
            } else {
                PARSER_LOG_ERROR("Left recursion detected in non-terminal '" + nonTerminal + "'");
            }
        }
    }
    
    return result;
}

std::vector<std::vector<Production>> Grammar::findFactorableProductions(const std::string& nonTerminal) const {
    std::vector<std::vector<Production>> factorableGroups;
    auto productions = getProductionsFor(nonTerminal);
    
    if (productions.size() < 2) {
        return factorableGroups;
    }
    
    // Group productions by common prefix
    std::map<std::string, std::vector<Production>> prefixGroups;
    
    for (const auto& prod : productions) {
        if (!prod.getRightHandSide().empty()) {
            std::string firstSymbol = prod.getRightHandSide()[0];
            prefixGroups[firstSymbol].push_back(prod);
        }
    }
    
    // Find groups with multiple productions (factorable)
    for (const auto& pair : prefixGroups) {
        if (pair.second.size() >= 2) {
            factorableGroups.push_back(pair.second);
        }
    }
    
    return factorableGroups;
}

void Grammar::factorProductionGroup(const std::string& nonTerminal, const std::vector<Production>& group) {
    if (group.size() < 2) return;
    
    // Find the common prefix
    std::string commonPrefix = group[0].getRightHandSide().empty() ? "" : group[0].getRightHandSide()[0];
    
    // Create new non-terminal for the factored part
    std::string newNonTerminal = nonTerminal + "_factored_" + std::to_string(productions.size());
    addNonTerminal(newNonTerminal);
    
    // Remove old productions
    for (const auto& prod : group) {
        removeRule(prod.getLeftHandSide(), prod.getRightHandSide());
    }
    
    // Add new production: A -> prefix A'
    std::vector<std::string> newRhs = {commonPrefix, newNonTerminal};
    addRule(nonTerminal, newRhs);
    
    // Add productions for the new non-terminal
    for (const auto& prod : group) {
        const auto& rhs = prod.getRightHandSide();
        std::vector<std::string> suffix(rhs.begin() + 1, rhs.end());
        if (suffix.empty()) {
            suffix.push_back("ε");
        }
        addRule(newNonTerminal, suffix);
    }
    
    // PARSER_LOG_INFO("Factored productions for " + nonTerminal + " with prefix: " + commonPrefix);
}

void Grammar::generateNullableCombinations(const std::vector<std::string>& rhs, 
                                          const std::unordered_set<std::string>& nullable,
                                          std::vector<std::vector<std::string>>& combinations) const {
    // Generate all possible combinations by including/excluding nullable symbols
    std::function<void(size_t, std::vector<std::string>&)> generate = 
        [&](size_t index, std::vector<std::string>& current) {
            if (index == rhs.size()) {
                combinations.push_back(current);
                return;
            }
            
            const std::string& symbol = rhs[index];
            
            // Always include the symbol
            current.push_back(symbol);
            generate(index + 1, current);
            current.pop_back();
            
            // If symbol is nullable, also try excluding it
            if (isNonTerminal(symbol) && nullable.find(symbol) != nullable.end()) {
                generate(index + 1, current);
            }
        };
    
    std::vector<std::string> current;
    generate(0, current);
}

bool Grammar::checkSymbolConsistency() const {
    // Check that all symbols used in productions are defined
    for (const auto& production : productions) {
        for (const auto& symbol : production.getRightHandSide()) {
            if (!isSymbol(symbol) && symbol != "ε") {
                return false;
            }
        }
    }
    return true;
}

bool Grammar::checkProductionValidity() const {
    // Check that all productions have valid LHS (non-terminal)
    for (const auto& production : productions) {
        if (!isNonTerminal(production.getLeftHandSide())) {
            return false;
        }
    }
    return true;
}

bool Grammar::checkStartSymbolExists() const {
    return !startSymbol.empty() && isNonTerminal(startSymbol);
}

std::unordered_set<std::string> Grammar::findReachableSymbols() const {
    std::unordered_set<std::string> reachable;
    std::queue<std::string> toProcess;
    
    // PARSER_LOG_INFO("Starting findReachableSymbols with start symbol: " + startSymbol);
    
    if (!startSymbol.empty()) {
        reachable.insert(startSymbol);
        toProcess.push(startSymbol);
        // PARSER_LOG_INFO("Added start symbol to reachable: " + startSymbol);
    } else {
        PARSER_LOG_ERROR("Start symbol is empty!");
    }
    
    while (!toProcess.empty()) {
        std::string current = toProcess.front();
        toProcess.pop();
        
        // PARSER_LOG_INFO("Processing symbol: " + current);
        auto productions = getProductionsFor(current);
        // PARSER_LOG_INFO("Found " + std::to_string(productions.size()) + " productions for " + current);
        
        for (const auto& prod : productions) {
            std::string rhsStr = "";
            for (const auto& s : prod.getRightHandSide()) {
                rhsStr += s + " ";
            }
 //           PARSER_LOG_INFO("  Production: " + current + " -> " + rhsStr);
            
            for (const auto& symbol : prod.getRightHandSide()) {
                if (reachable.find(symbol) == reachable.end()) {
                    reachable.insert(symbol);
//                    PARSER_LOG_INFO("    Added to reachable: " + symbol);
                    if (isNonTerminal(symbol)) {
                        toProcess.push(symbol);
                        // PARSER_LOG_INFO("    Added to processing queue (non-terminal): " + symbol);
                    } else {
                        // PARSER_LOG_INFO("    Terminal symbol: " + symbol);
                    }
                }
            }
        }
    }
    
    // PARSER_LOG_INFO("Finished findReachableSymbols. Total reachable: " + std::to_string(reachable.size()));
    return reachable;
}

std::unordered_set<std::string> Grammar::findProductiveSymbols() const {
    std::unordered_set<std::string> productive;
    
    // All terminals are productive
    for (const auto& terminal : terminals) {
        productive.insert(terminal);
    }
    
    // Find productive non-terminals iteratively
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (const auto& production : productions) {
            const auto& lhs = production.getLeftHandSide();
            const auto& rhs = production.getRightHandSide();
            
            // Skip if LHS is already productive
            if (productive.find(lhs) != productive.end()) {
                continue;
            }
            
            // Check if all RHS symbols are productive
            bool allProductive = true;
            for (const auto& symbol : rhs) {
                if (symbol != "ε" && productive.find(symbol) == productive.end()) {
                    allProductive = false;
                    break;
                }
            }
            
            if (allProductive || rhs.empty()) {
                productive.insert(lhs);
                changed = true;
            }
        }
    }
    
    return productive;
}

std::unordered_set<std::string> Grammar::findAllLeftRecursiveNonTerminals() const {
    std::unordered_set<std::string> leftRecursive;
    
    // 1. Direct left recursion detection
    for (const auto& nt : nonTerminals) {
        if (!findLeftRecursiveProductions(nt).empty()) {
            leftRecursive.insert(nt);
        }
    }
    
    // 2. Indirect left recursion detection using DFS
    // Build dependency graph: A -> B if A has production starting with B
    std::unordered_map<std::string, std::unordered_set<std::string>> dependencyGraph;
    
    for (const auto& production : productions) {
        const auto& lhs = production.getLeftHandSide();
        const auto& rhs = production.getRightHandSide();
        
        if (!rhs.empty() && isNonTerminal(rhs[0]) && rhs[0] != lhs) {
            // Only add if it's not direct recursion (already handled above)
            dependencyGraph[lhs].insert(rhs[0]);
        }
    }
    
    // Use DFS to find cycles in the dependency graph
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    
    for (const auto& nt : nonTerminals) {
        if (visited.find(nt) == visited.end()) {
            if (hasCycleDFS(nt, dependencyGraph, visited, recursionStack)) {
                // If there's a cycle involving this non-terminal, it has indirect left recursion
                leftRecursive.insert(nt);
            }
        }
    }
    
    return leftRecursive;
}

bool Grammar::hasCycleDFS(const std::string& node, 
                         const std::unordered_map<std::string, std::unordered_set<std::string>>& graph,
                         std::unordered_set<std::string>& visited,
                         std::unordered_set<std::string>& recursionStack) const {
    visited.insert(node);
    recursionStack.insert(node);
    
    auto it = graph.find(node);
    if (it != graph.end()) {
        for (const auto& neighbor : it->second) {
            if (recursionStack.find(neighbor) != recursionStack.end()) {
                return true; // Cycle found
            }
            
            if (visited.find(neighbor) == visited.end()) {
                if (hasCycleDFS(neighbor, graph, visited, recursionStack)) {
                    return true;
                }
            }
        }
    }
    
    recursionStack.erase(node);
    return false;
}

// GrammarBuilder implementation
GrammarBuilder::GrammarBuilder() {}

GrammarBuilder& GrammarBuilder::terminal(const std::string& name, const std::string& pattern) {
    grammar.addTerminal(name, pattern);
    return *this;
}

GrammarBuilder& GrammarBuilder::nonTerminal(const std::string& name) {
    grammar.addNonTerminal(name);
    return *this;
}

GrammarBuilder& GrammarBuilder::production(const std::string& lhs, const std::vector<std::string>& rhs) {
    grammar.addProduction(lhs, rhs);
    return *this;
}

GrammarBuilder& GrammarBuilder::production(const std::string& lhs, const std::string& rhs) {
    grammar.addProduction(lhs, {rhs});
    return *this;
}

GrammarBuilder& GrammarBuilder::startSymbol(const std::string& symbol) {
    grammar.addNonTerminal(symbol);
    grammar.setStartSymbol(symbol);
    return *this;
}

GrammarBuilder& GrammarBuilder::precedence(const std::string& symbol, int prec, const std::string& assoc) {
    grammar.setPrecedence(symbol, prec, assoc);
    return *this;
}

Grammar GrammarBuilder::build() {
    return grammar;
}

// OperatorPrecedenceTable implementation
OperatorPrecedenceTable::OperatorPrecedenceTable() {}

void OperatorPrecedenceTable::setRelation(const std::string& op1, const std::string& op2, Relation relation) {
    table[op1][op2] = relation;
}

OperatorPrecedenceTable::Relation OperatorPrecedenceTable::getRelation(const std::string& op1, const std::string& op2) const {
    auto it1 = table.find(op1);
    if (it1 != table.end()) {
        auto it2 = it1->second.find(op2);
        if (it2 != it1->second.end()) {
            return it2->second;
        }
    }
    return Relation::UNDEFINED;
}

bool OperatorPrecedenceTable::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        PARSER_LOG_ERROR("Failed to open precedence file: " + filename);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse format: op1 relation op2
        std::istringstream iss(line);
        std::string op1, relationStr, op2;
        
        if (iss >> op1 >> relationStr >> op2) {
            Relation relation = stringToRelation(relationStr);
            setRelation(op1, op2, relation);
        }
    }
    
    file.close();
    // PARSER_LOG_INFO("Loaded operator precedence from file: " + filename);
    return true;
}

bool OperatorPrecedenceTable::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        PARSER_LOG_ERROR("Failed to create precedence file: " + filename);
        return false;
    }
    
    file << "# Operator precedence table\n";
    file << "# Format: op1 relation op2\n\n";
    
    for (const auto& [op1, relations] : table) {
        for (const auto& [op2, relation] : relations) {
            file << op1 << " " << relationToString(relation) << " " << op2 << "\n";
        }
    }
    
    file.close();
    // PARSER_LOG_INFO("Saved operator precedence to file: " + filename);
    return true;
}

void OperatorPrecedenceTable::buildFromGrammar(const Grammar& grammar) {
    // PARSER_LOG_INFO("Building operator precedence table from grammar");
    
    // Clear existing table
    table.clear();
    
    // Get all terminals from the grammar
    std::unordered_set<std::string> terminals;
    for (const auto& terminal : grammar.getTerminals()) {
        if (terminal != "ε") { // Skip epsilon
            terminals.insert(terminal);
        }
    }
    
    // Add end-of-input marker
    terminals.insert("$");
    
    // Build precedence relations based on grammar productions
    for (const auto& production : grammar.getProductions()) {
        const auto& rhs = production.getRightHandSide();
        
        // Find consecutive terminals and operators in the production
        for (size_t i = 0; i < rhs.size(); i++) {
            const std::string& current = rhs[i];
            
            if (grammar.isTerminal(current) && current != "ε") {
                // Look for next terminal
                for (size_t j = i + 1; j < rhs.size(); j++) {
                    const std::string& next = rhs[j];
                    
                    if (grammar.isTerminal(next) && next != "ε") {
                        // Direct succession: current = next
                        setRelation(current, next, Relation::EQUAL);
                        break;
                    } else if (grammar.isNonTerminal(next)) {
                        // current < FIRST(next)
                        auto firstSet = grammar.computeFirst(next);
                        for (const auto& firstSymbol : firstSet) {
                            if (firstSymbol != "ε" && grammar.isTerminal(firstSymbol)) {
                                setRelation(current, firstSymbol, Relation::LESS_THAN);
                            }
                        }
                        
                        // If next is nullable, continue looking
                        if (firstSet.find("ε") == firstSet.end()) {
                            break;
                        }
                    }
                }
                
                // Look for previous terminal
                for (int k = static_cast<int>(i) - 1; k >= 0; k--) {
                    const std::string& prev = rhs[k];
                    
                    if (grammar.isTerminal(prev) && prev != "ε") {
                        // Already handled in forward pass
                        break;
                    } else if (grammar.isNonTerminal(prev)) {
                        // FOLLOW(prev) > current
                        auto followSet = grammar.computeFollow(prev);
                        for (const auto& followSymbol : followSet) {
                            if (followSymbol != "ε" && grammar.isTerminal(followSymbol)) {
                                setRelation(followSymbol, current, Relation::GREATER_THAN);
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    
    // Set relations with end-of-input marker
    const std::string& startSymbol = grammar.getStartSymbol();
    if (!startSymbol.empty()) {
        auto followStart = grammar.computeFollow(startSymbol);
        for (const auto& symbol : followStart) {
            if (symbol != "ε" && grammar.isTerminal(symbol)) {
                setRelation(symbol, "$", Relation::GREATER_THAN);
            }
        }
        
        auto firstStart = grammar.computeFirst(startSymbol);
        for (const auto& symbol : firstStart) {
            if (symbol != "ε" && grammar.isTerminal(symbol)) {
                setRelation("$", symbol, Relation::LESS_THAN);
            }
        }
    }
    
    // PARSER_LOG_INFO("Operator precedence table built with " + std::to_string(table.size()) + " relations");
}

std::string OperatorPrecedenceTable::toString() const {
    std::ostringstream oss;
    oss << "Operator Precedence Table:\n";
    
    for (const auto& [op1, relations] : table) {
        for (const auto& [op2, relation] : relations) {
            oss << op1 << " " << relationToString(relation) << " " << op2 << "\n";
        }
    }
    
    return oss.str();
}

std::string OperatorPrecedenceTable::relationToString(Relation relation) const {
    switch (relation) {
        case Relation::LESS_THAN: return "<";
        case Relation::GREATER_THAN: return ">";
        case Relation::EQUAL: return "=";
        default: return "?";
    }
}

OperatorPrecedenceTable::Relation OperatorPrecedenceTable::stringToRelation(const std::string& str) const {
    if (str == "<") return Relation::LESS_THAN;
    if (str == ">") return Relation::GREATER_THAN;
    if (str == "=") return Relation::EQUAL;
    return Relation::UNDEFINED;
}

} // namespace Parser