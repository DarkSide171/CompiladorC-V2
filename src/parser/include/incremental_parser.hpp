#ifndef INCREMENTAL_PARSER_HPP
#define INCREMENTAL_PARSER_HPP

#include "ast.hpp"
#include "token_stream.hpp"
#include "parser_types.hpp"
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>

namespace Parser {

// Represents a change in the source code
struct TextChange {
    size_t startOffset;
    size_t endOffset;
    std::string newText;
    
    TextChange(size_t start, size_t end, const std::string& text)
        : startOffset(start), endOffset(end), newText(text) {}
};

// Represents a cached AST node with its validity range
struct CachedASTNode {
    std::shared_ptr<ASTNode> node;
    size_t startOffset;
    size_t endOffset;
    std::chrono::steady_clock::time_point timestamp;
    bool isValid;
    
    CachedASTNode(std::shared_ptr<ASTNode> n, size_t start, size_t end)
        : node(n), startOffset(start), endOffset(end), 
          timestamp(std::chrono::steady_clock::now()), isValid(true) {}
};

// Statistics for incremental parsing performance
struct IncrementalStats {
    size_t totalParses;
    size_t incrementalParses;
    size_t fullParses;
    size_t nodesReused;
    size_t nodesReparsed;
    std::chrono::milliseconds totalTime;
    std::chrono::milliseconds incrementalTime;
    
    IncrementalStats() : totalParses(0), incrementalParses(0), fullParses(0),
                        nodesReused(0), nodesReparsed(0), totalTime(0), incrementalTime(0) {}
    
    double getReuseRatio() const {
        return totalParses > 0 ? static_cast<double>(nodesReused) / (nodesReused + nodesReparsed) : 0.0;
    }
    
    double getIncrementalRatio() const {
        return totalParses > 0 ? static_cast<double>(incrementalParses) / totalParses : 0.0;
    }
};

// Incremental parser that reuses AST nodes when possible
class IncrementalParser {
private:
    // Cache of previously parsed AST nodes
    std::vector<CachedASTNode> astCache;
    
    // Mapping from source positions to cached nodes
    std::unordered_map<size_t, size_t> positionToCache;
    
    // Statistics
    mutable IncrementalStats stats;
    
    // Configuration
    size_t maxCacheSize;
    std::chrono::milliseconds cacheTimeout;
    
    // Helper methods
    bool isNodeAffectedByChange(const CachedASTNode& cached, const TextChange& change) const;
    void invalidateAffectedNodes(const std::vector<TextChange>& changes);
    void updateCachePositions(const std::vector<TextChange>& changes);
    std::vector<size_t> findReusableNodes(size_t startOffset, size_t endOffset) const;
    void addToCache(std::shared_ptr<ASTNode> node, size_t startOffset, size_t endOffset);
    void cleanupCache();
    void cacheSubtrees(std::shared_ptr<ASTNode> node);
    
public:
    explicit IncrementalParser(size_t maxCache = 1000, 
                              std::chrono::milliseconds timeout = std::chrono::minutes(10))
        : maxCacheSize(maxCache), cacheTimeout(timeout) {}
    
    // Main parsing methods
    std::shared_ptr<ASTNode> parse(TokenStream& tokens, 
                                  const std::vector<TextChange>& changes = {});
    
    std::shared_ptr<ASTNode> parseIncremental(TokenStream& tokens, 
                                             const std::vector<TextChange>& changes);
    
    std::shared_ptr<ASTNode> parseFull(TokenStream& tokens);
    
    // Cache management
    void clearCache();
    void optimizeCache();
    size_t getCacheSize() const { return astCache.size(); }
    
    // Statistics
    const IncrementalStats& getStatistics() const { return stats; }
    void resetStatistics() { stats = IncrementalStats(); }
    
    // Configuration
    void setMaxCacheSize(size_t size) { maxCacheSize = size; }
    void setCacheTimeout(std::chrono::milliseconds timeout) { cacheTimeout = timeout; }
    
    // Utility methods
    bool canReuseNode(const CachedASTNode& cached, size_t startOffset, size_t endOffset) const;
    std::vector<std::shared_ptr<ASTNode>> extractReusableNodes(size_t startOffset, size_t endOffset);
};

// Factory function for creating incremental parser
std::unique_ptr<IncrementalParser> createIncrementalParser(size_t maxCacheSize = 1000);

} // namespace Parser

#endif // INCREMENTAL_PARSER_HPP