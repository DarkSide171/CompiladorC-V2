#ifndef PARSER_TYPES_HPP
#define PARSER_TYPES_HPP

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <variant>
#include <cstdint>
#include "../../lexer/include/token.hpp"

namespace Parser {

// Forward declarations
class ASTNode;
class ParseError;

// Token type alias - use Lexer::Token as the base token type
using Token = Lexer::Token;

// Type aliases
using ASTNodePtr = std::unique_ptr<ASTNode>;
using TokenPtr = std::shared_ptr<Token>;
using ParseErrorPtr = std::unique_ptr<ParseError>;

// Parser result types
template<typename T>
struct ParseResult {
    bool success;
    T value;
    ParseErrorPtr error;
    
    ParseResult(T&& val) : success(true), value(std::move(val)), error(nullptr) {}
    ParseResult(ParseErrorPtr err) : success(false), error(std::move(err)) {}
    
    bool isSuccess() const { return success; }
    bool isError() const { return !success; }
    const T& getValue() const { return value; }
    T&& moveValue() { return std::move(value); }
    const ParseErrorPtr& getError() const { return error; }
};

// Position information
struct Position {
    size_t line;
    size_t column;
    size_t offset;
    std::string filename;
    
    Position(size_t l = 0, size_t c = 0, size_t o = 0, const std::string& f = "")
        : line(l), column(c), offset(o), filename(f) {}
};

// Source range
struct SourceRange {
    Position start;
    Position end;
    
    SourceRange() = default;
    SourceRange(const Position& s, const Position& e) : start(s), end(e) {}
};

// Parser configuration flags
enum class ParserFlags : uint32_t {
    NONE = 0,
    STRICT_MODE = 1 << 0,
    ALLOW_GNU_EXTENSIONS = 1 << 1,
    ALLOW_MS_EXTENSIONS = 1 << 2,
    ENABLE_RECOVERY = 1 << 3,
    VERBOSE_ERRORS = 1 << 4
};

// C standard versions
enum class CStandard {
    C89,
    C99,
    C11,
    C17,
    C23
};

// Use the existing feature system from lexer module
// Features are managed through Lexer::Feature enum and related functions

// AST node types
enum class ASTNodeType {
    // Declarations
    TRANSLATION_UNIT,
    FUNCTION_DECLARATION,
    VARIABLE_DECLARATION,
    DECLARATION_LIST,
    TYPE_DECLARATION,
    
    // Statements
    COMPOUND_STATEMENT,
    EXPRESSION_STATEMENT,
    IF_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    RETURN_STATEMENT,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
    
    // Expressions
    BINARY_EXPRESSION,
    UNARY_EXPRESSION,
    ASSIGNMENT_EXPRESSION,
    TERNARY_EXPRESSION,
    CALL_EXPRESSION,
    MEMBER_EXPRESSION,
    ARRAY_ACCESS,
    CAST_EXPRESSION,
    SIZEOF_EXPRESSION,
    
    // Literals
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    CHAR_LITERAL,
    
    // Identifiers
    IDENTIFIER,
    
    // Types
    BUILTIN_TYPE,
    POINTER_TYPE,
    ARRAY_TYPE,
    FUNCTION_TYPE,
    STRUCT_TYPE,
    UNION_TYPE,
    ENUM_TYPE
};

// Error severity levels
enum class ErrorSeverity {
    NOTE,
    WARNING,
    ERROR,
    FATAL
};

// Parser state flags
enum class ParserState : uint32_t {
    NORMAL = 0,
    IN_FUNCTION = 1 << 0,
    IN_LOOP = 1 << 1,
    IN_SWITCH = 1 << 2,
    RECOVERING = 1 << 3
};

} // namespace Parser

#endif // PARSER_TYPES_HPP