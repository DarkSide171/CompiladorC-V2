#include "../include/token_stream.hpp"
#include "../../lexer/include/token.hpp"
#include <stdexcept>
#include <algorithm>

namespace Parser {

// Concrete implementation of TokenStream with caching and lookahead buffer
class BufferedTokenStream : public TokenStream {
private:
    std::vector<Lexer::Token> tokens;
    size_t currentPosition;
    
    // Cache statistics
    mutable size_t cacheHits;
    mutable size_t cacheMisses;
    
public:
    explicit BufferedTokenStream(const std::vector<Lexer::Token>& tokenList, size_t bufferSize = 64)
        : tokens(tokenList), currentPosition(0), cacheHits(0), cacheMisses(0) {
        (void)bufferSize; // Suppress unused parameter warning
        if (tokens.empty()) {
            // Add EOF token if empty
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
        cacheHits++;
        return tokens[currentPosition];
    }
    
    const Token& peek(size_t offset = 1) const override {
        size_t peekPosition = currentPosition + offset;
        if (peekPosition >= tokens.size()) {
            static const Lexer::Position pos{1, 1, 0};
            static const Token eofToken{Lexer::TokenType::END_OF_FILE, "", pos};
            cacheMisses++;
            return eofToken;
        }
        cacheHits++;
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
        return currentPosition >= tokens.size() - 1 || 
               tokens[currentPosition].getType() == Lexer::TokenType::END_OF_FILE;
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
        if (currentPosition < offset) {
            static const Lexer::Position pos{1, 1, 0};
            static const Token invalidToken{Lexer::TokenType::END_OF_FILE, "", pos};
            cacheMisses++;
            return invalidToken;
        }
        cacheHits++;
        return tokens[currentPosition - offset];
    }
    
    std::vector<Token> getRange(size_t start, size_t end) const override {
        if (start >= tokens.size() || end > tokens.size() || start > end) {
            return {};
        }
        
        std::vector<Token> result;
        result.reserve(end - start);
        
        for (size_t i = start; i < end; ++i) {
            result.push_back(tokens[i]);
        }
        
        cacheHits += (end - start);
        return result;
    }
    
    // Additional utility methods for performance monitoring
    double getCacheHitRatio() const {
        size_t total = cacheHits + cacheMisses;
        return total > 0 ? static_cast<double>(cacheHits) / total : 0.0;
    }
    
    void resetStatistics() {
        cacheHits = 0;
        cacheMisses = 0;
    }
};

// Factory function to create buffered token stream
std::unique_ptr<TokenStream> createBufferedTokenStream(const std::vector<Lexer::Token>& tokens, size_t bufferSize) {
    return std::make_unique<BufferedTokenStream>(tokens, bufferSize);
}

} // namespace Parser