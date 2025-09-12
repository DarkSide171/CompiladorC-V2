#ifndef TOKEN_STREAM_HPP
#define TOKEN_STREAM_HPP

#include "parser_types.hpp"
#include <vector>
#include <memory>

namespace Parser {

// Token stream interface for parser state management
class TokenStream {
public:
    virtual ~TokenStream() = default;
    
    // Token access
    virtual const Token& current() const = 0;
    virtual const Token& peek(size_t offset = 1) const = 0;
    virtual bool advance() = 0;
    virtual bool isAtEnd() const = 0;
    
    // Position management
    virtual size_t getPosition() const = 0;
    virtual void setPosition(size_t position) = 0;
    virtual size_t size() const = 0;
    
    // Token history
    virtual const Token& previous(size_t offset = 1) const = 0;
    virtual std::vector<Token> getRange(size_t start, size_t end) const = 0;
};

} // namespace Parser

#endif // TOKEN_STREAM_HPP