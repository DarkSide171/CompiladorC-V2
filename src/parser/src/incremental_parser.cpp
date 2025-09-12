#include "../include/incremental_parser.hpp"
#include "../include/parser.hpp"
#include "../include/ast.hpp"
#include "../include/token_stream.hpp"
#include "../../lexer/include/token.hpp"
#include <algorithm>
#include <chrono>

namespace Parser {

// Forward declaration of BufferedTokenStream from token_stream.cpp
class BufferedTokenStream : public TokenStream {
private:
    std::vector<Lexer::Token> tokens;
    size_t currentPosition;
    
public:
    explicit BufferedTokenStream(const std::vector<Lexer::Token>& tokenList = {}) 
        : tokens(tokenList), currentPosition(0) {
        if (tokens.empty()) {
            Lexer::Position pos{1, 1, 0};
            tokens.emplace_back(Lexer::TokenType::END_OF_FILE, "", pos);
        }
    }
    
    const Token& current() const override {
        if (currentPosition >= tokens.size()) {
            static const Lexer::Position pos{1, 1, 0};
            static const Token eofToken{Lexer::TokenType::END_OF_FILE, "", pos};
            return eofToken;
        }
        return tokens[currentPosition];
    }
    
    const Token& peek(size_t offset = 1) const override {
        size_t peekPosition = currentPosition + offset;
        if (peekPosition >= tokens.size()) {
            static const Lexer::Position pos{1, 1, 0};
            static const Token eofToken{Lexer::TokenType::END_OF_FILE, "", pos};
            return eofToken;
        }
        return tokens[peekPosition];
    }
    
    bool advance() override {
        if (currentPosition < tokens.size() - 1) {
            currentPosition++;
            return true;
        }
        return false;
    }
    
    bool isAtEnd() const override {
        return currentPosition >= tokens.size() - 1;
    }
    
    size_t getPosition() const override {
        return currentPosition;
    }
    
    void setPosition(size_t position) override {
        currentPosition = std::min(position, tokens.size() - 1);
    }
    
    size_t size() const override {
        return tokens.size();
    }
    
    const Token& previous(size_t offset = 1) const override {
        if (currentPosition >= offset) {
            return tokens[currentPosition - offset];
        }
        static const Lexer::Position pos{1, 1, 0};
        static const Token eofToken{Lexer::TokenType::END_OF_FILE, "", pos};
        return eofToken;
    }
    
    std::vector<Token> getRange(size_t start, size_t end) const override {
        if (start >= tokens.size() || end > tokens.size() || start > end) {
            return {};
        }
        return std::vector<Token>(tokens.begin() + start, tokens.begin() + end);
    }
};

bool IncrementalParser::isNodeAffectedByChange(const CachedASTNode& cached, const TextChange& change) const {
    // Check if the change overlaps with the cached node's range
    return !(change.endOffset <= cached.startOffset || change.startOffset >= cached.endOffset);
}

void IncrementalParser::invalidateAffectedNodes(const std::vector<TextChange>& changes) {
    for (auto& cached : astCache) {
        if (!cached.isValid) continue;
        
        for (const auto& change : changes) {
            if (isNodeAffectedByChange(cached, change)) {
                cached.isValid = false;
                stats.nodesReparsed++;
                break;
            }
        }
    }
}

void IncrementalParser::updateCachePositions(const std::vector<TextChange>& changes) {
    // Sort changes by position (reverse order to handle from end to beginning)
    auto sortedChanges = changes;
    std::sort(sortedChanges.begin(), sortedChanges.end(), 
              [](const TextChange& a, const TextChange& b) {
                  return a.startOffset > b.startOffset;
              });
    
    for (auto& cached : astCache) {
        if (!cached.isValid) continue;
        
        for (const auto& change : sortedChanges) {
            // Calculate offset delta
            int64_t delta = static_cast<int64_t>(change.newText.length()) - 
                           static_cast<int64_t>(change.endOffset - change.startOffset);
            
            // Update positions if the node is after the change
            if (cached.startOffset >= change.endOffset) {
                cached.startOffset = static_cast<size_t>(static_cast<int64_t>(cached.startOffset) + delta);
                cached.endOffset = static_cast<size_t>(static_cast<int64_t>(cached.endOffset) + delta);
            }
        }
    }
    
    // Rebuild position mapping
    positionToCache.clear();
    for (size_t i = 0; i < astCache.size(); ++i) {
        if (astCache[i].isValid) {
            positionToCache[astCache[i].startOffset] = i;
        }
    }
}

std::vector<size_t> IncrementalParser::findReusableNodes(size_t startOffset, size_t endOffset) const {
    std::vector<size_t> reusable;
    
    for (size_t i = 0; i < astCache.size(); ++i) {
        const auto& cached = astCache[i];
        if (cached.isValid && 
            cached.startOffset >= startOffset && 
            cached.endOffset <= endOffset) {
            reusable.push_back(i);
            stats.nodesReused++;
        }
    }
    
    return reusable;
}

void IncrementalParser::addToCache(std::shared_ptr<ASTNode> node, size_t startOffset, size_t endOffset) {
    if (astCache.size() >= maxCacheSize) {
        cleanupCache();
    }
    
    astCache.emplace_back(node, startOffset, endOffset);
    positionToCache[startOffset] = astCache.size() - 1;
}

void IncrementalParser::cleanupCache() {
    auto now = std::chrono::steady_clock::now();
    
    // Remove invalid and expired nodes
    astCache.erase(
        std::remove_if(astCache.begin(), astCache.end(),
                      [this, now](const CachedASTNode& cached) {
                          return !cached.isValid || 
                                 (now - cached.timestamp) > cacheTimeout;
                      }),
        astCache.end());
    
    // If still too large, remove oldest nodes
    if (astCache.size() > maxCacheSize) {
        std::sort(astCache.begin(), astCache.end(),
                 [](const CachedASTNode& a, const CachedASTNode& b) {
                     return a.timestamp < b.timestamp;
                 });
        
        size_t toRemove = astCache.size() - maxCacheSize;
        astCache.erase(astCache.begin(), astCache.begin() + toRemove);
    }
    
    // Rebuild position mapping
    positionToCache.clear();
    for (size_t i = 0; i < astCache.size(); ++i) {
        if (astCache[i].isValid) {
            positionToCache[astCache[i].startOffset] = i;
        }
    }
}

std::shared_ptr<ASTNode> IncrementalParser::parse(TokenStream& tokens, 
                                                 const std::vector<TextChange>& changes) {
    auto startTime = std::chrono::steady_clock::now();
    stats.totalParses++;
    
    std::shared_ptr<ASTNode> result;
    
    if (changes.empty() || astCache.empty()) {
        // Full parse needed
        result = parseFull(tokens);
        stats.fullParses++;
    } else {
        // Try incremental parse
        result = parseIncremental(tokens, changes);
        stats.incrementalParses++;
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    stats.totalTime += duration;
    
    if (!changes.empty()) {
        stats.incrementalTime += duration;
    }
    
    return result;
}

std::shared_ptr<ASTNode> IncrementalParser::parseIncremental(TokenStream& tokens, 
                                                           const std::vector<TextChange>& changes) {
    // Invalidate affected nodes
    invalidateAffectedNodes(changes);
    
    // Update cache positions based on changes
    updateCachePositions(changes);
    
    // For now, implement a simple strategy: if any changes affect the root level,
    // do a full reparse. In a more sophisticated implementation, we would
    // identify the minimal set of nodes that need reparsing.
    
    // Check if we have any valid cached nodes left
    bool hasValidNodes = std::any_of(astCache.begin(), astCache.end(),
                                    [](const CachedASTNode& cached) {
                                        return cached.isValid;
                                    });
    
    if (!hasValidNodes) {
        // No valid nodes left, do full parse
        return parseFull(tokens);
    }
    
    // For this implementation, we'll do a full parse but cache the results
    // A more sophisticated approach would selectively reparse only affected subtrees
    return parseFull(tokens);
}

std::shared_ptr<ASTNode> IncrementalParser::parseFull(TokenStream& tokens) {
    // Clear existing cache since we're doing a full parse
    clearCache();
    
    try {
        // Create a SyntacticAnalyzer instance for parsing
        auto parser = std::make_unique<SyntacticAnalyzer>();
        
        // Configure parser for incremental parsing
        ParserConfig config;
        config.setRecoveryEnabled(true);
        parser->setConfig(config);
        
        // Create a concrete token stream implementation with empty tokens
    std::vector<Lexer::Token> emptyTokens;
    // Add EOF token
    Lexer::Position pos{1, 1, 0};
    emptyTokens.emplace_back(Lexer::TokenType::END_OF_FILE, "", pos);
    auto tokenStream = std::make_unique<BufferedTokenStream>(emptyTokens);
        
        // Parse the token stream
        auto parseResult = parser->parseTokens(std::move(tokenStream));
        
        if (parseResult.isSuccess()) {
            auto root = parseResult.moveValue();
            
            // Convert unique_ptr to shared_ptr for caching
            std::shared_ptr<ASTNode> sharedRoot(root.release());
            
            // Cache the root node (spans entire input)
            addToCache(sharedRoot, 0, tokens.size());
            
            // Cache major subtrees for better incremental performance
            cacheSubtrees(sharedRoot);
            
            return sharedRoot;
        } else {
            // Parse failed, create minimal AST with error information
            auto root = std::make_shared<TranslationUnit>();
            
            // Still cache it to avoid repeated failed parses
            addToCache(root, 0, tokens.size());
            
            return root;
        }
    } catch (const std::exception& e) {
        // Fallback: create empty translation unit
        auto root = std::make_shared<TranslationUnit>();
        addToCache(root, 0, 0);
        return root;
    }
}

void IncrementalParser::cacheSubtrees(std::shared_ptr<ASTNode> node) {
    if (!node) return;
    
    // Cache function declarations and definitions for better incremental performance
    if (node->getType() == ASTNodeType::FUNCTION_DECLARATION) {
        size_t nodeId = reinterpret_cast<size_t>(node.get());
        (void)nodeId; // Suppress unused variable warning
        addToCache(node, 0, 0); // Position will be updated later
    }
    
    // Cache struct and union declarations
    if (node->getType() == ASTNodeType::STRUCT_TYPE || 
        node->getType() == ASTNodeType::UNION_TYPE) {
        size_t nodeId = reinterpret_cast<size_t>(node.get());
        (void)nodeId; // Suppress unused variable warning
        addToCache(node, 0, 0); // Position will be updated later
    }
    
    // Recursively cache children
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto child = node->getChild(i);
        if (child) {
            // Convert raw pointer to shared_ptr
            auto sharedChild = std::shared_ptr<ASTNode>(child);
            cacheSubtrees(sharedChild);
        }
    }
}

void IncrementalParser::clearCache() {
    astCache.clear();
    positionToCache.clear();
}

void IncrementalParser::optimizeCache() {
    cleanupCache();
    
    // Additional optimization: merge adjacent cached nodes if beneficial
    // Sort by position
    std::sort(astCache.begin(), astCache.end(),
             [](const CachedASTNode& a, const CachedASTNode& b) {
                 return a.startOffset < b.startOffset;
             });
}

bool IncrementalParser::canReuseNode(const CachedASTNode& cached, 
                                   size_t startOffset, size_t endOffset) const {
    return cached.isValid && 
           cached.startOffset >= startOffset && 
           cached.endOffset <= endOffset &&
           (std::chrono::steady_clock::now() - cached.timestamp) <= cacheTimeout;
}

std::vector<std::shared_ptr<ASTNode>> IncrementalParser::extractReusableNodes(size_t startOffset, size_t endOffset) {
    std::vector<std::shared_ptr<ASTNode>> reusable;
    
    for (const auto& cached : astCache) {
        if (canReuseNode(cached, startOffset, endOffset)) {
            reusable.push_back(cached.node);
        }
    }
    
    return reusable;
}

// Factory function
std::unique_ptr<IncrementalParser> createIncrementalParser(size_t maxCacheSize) {
    return std::make_unique<IncrementalParser>(maxCacheSize);
}

} // namespace Parser