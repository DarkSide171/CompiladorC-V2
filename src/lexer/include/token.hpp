#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <iostream>
#include <cstdint>

namespace Lexer {

// Enumeração de todos os tipos de tokens suportados
enum class TokenType {
    // Tokens especiais
    END_OF_FILE,
    UNKNOWN,
    
    // Identificadores e literais
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    CHAR_LITERAL,
    STRING_LITERAL,
    
    // Palavras-chave C89/C90
    AUTO, BREAK, CASE, CHAR, CONST, CONTINUE, DEFAULT, DO,
    DOUBLE, ELSE, ENUM, EXTERN, FLOAT, FOR, GOTO, IF,
    INT, LONG, REGISTER, RETURN, SHORT, SIGNED, SIZEOF, STATIC,
    STRUCT, SWITCH, TYPEDEF, UNION, UNSIGNED, VOID, VOLATILE, WHILE,
    
    // Palavras-chave C99
    INLINE, RESTRICT, _BOOL, _COMPLEX, _IMAGINARY,
    
    // Palavras-chave C11
    _ALIGNAS, _ALIGNOF, _ATOMIC, _STATIC_ASSERT, _NORETURN,
    _THREAD_LOCAL, _GENERIC,
    
    // Palavras-chave C17 (compatível com C11)
    
    // Palavras-chave C23
    TYPEOF, TYPEOF_UNQUAL, _BITINT, _DECIMAL128, _DECIMAL32, _DECIMAL64,
    
    // Operadores aritméticos
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    INCREMENT,      // ++
    DECREMENT,      // --
    
    // Operadores de atribuição
    ASSIGN,         // =
    PLUS_ASSIGN,    // +=
    MINUS_ASSIGN,   // -=
    MULT_ASSIGN,    // *=
    DIV_ASSIGN,     // /=
    MOD_ASSIGN,     // %=
    
    // Operadores relacionais
    EQUAL,          // ==
    NOT_EQUAL,      // !=
    LESS_THAN,      // <
    GREATER_THAN,   // >
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=
    
    // Operadores lógicos
    LOGICAL_AND,    // &&
    LOGICAL_OR,     // ||
    LOGICAL_NOT,    // !
    
    // Operadores bitwise
    BITWISE_AND,    // &
    BITWISE_OR,     // |
    BITWISE_XOR,    // ^
    BITWISE_NOT,    // ~
    LEFT_SHIFT,     // <<
    RIGHT_SHIFT,    // >>
    AND_ASSIGN,     // &=
    OR_ASSIGN,      // |=
    XOR_ASSIGN,     // ^=
    LEFT_SHIFT_ASSIGN,  // <<=
    RIGHT_SHIFT_ASSIGN, // >>=
    
    // Operadores especiais
    CONDITIONAL,    // ?
    COLON,          // :
    COMMA,          // ,
    DOT,            // .
    ARROW,          // ->
    
    // Delimitadores
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    SEMICOLON,      // ;
    HASH,           // #
    
    // Comentários
    LINE_COMMENT,   // //
    BLOCK_COMMENT   // /* */
};

// Enumeração para associatividade de operadores
enum class Associativity {
    LEFT,
    RIGHT,
    NONE
};

// Estrutura para posição no código fonte
struct Position {
    int line;
    int column;
    int offset;
    
    Position() : line(1), column(1), offset(0) {}
    Position(int l, int c, int o) : line(l), column(c), offset(o) {}
    
    bool operator==(const Position& other) const {
        return line == other.line && column == other.column && offset == other.offset;
    }
    
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

// Enumeração para tipo de valor
enum class ValueType {
    NONE,
    INTEGER,
    LONG,
    LONG_LONG,
    FLOAT,
    DOUBLE,
    CHAR,
    STRING
};

// União para valores de literais
struct Value {
    ValueType type;
    union {
        int int_val;
        long long_val;
        long long long_long_val;
        float float_val;
        double double_val;
        char char_val;
    };
    std::string string_val;
    
    Value() : type(ValueType::NONE), int_val(0) {}
    Value(int v) : type(ValueType::INTEGER), int_val(v) {}
    Value(long v) : type(ValueType::LONG), long_val(v) {}
    Value(long long v) : type(ValueType::LONG_LONG), long_long_val(v) {}
    Value(float v) : type(ValueType::FLOAT), float_val(v) {}
    Value(double v) : type(ValueType::DOUBLE), double_val(v) {}
    Value(char v) : type(ValueType::CHAR), char_val(v) {}
    Value(const std::string& v) : type(ValueType::STRING), string_val(v) {}
    Value(std::string&& v) : type(ValueType::STRING), string_val(std::move(v)) {}
    
    // Copy constructor
    Value(const Value& other) : type(other.type), string_val(other.string_val) {
        switch (type) {
            case ValueType::INTEGER: int_val = other.int_val; break;
            case ValueType::LONG: long_val = other.long_val; break;
            case ValueType::LONG_LONG: long_long_val = other.long_long_val; break;
            case ValueType::FLOAT: float_val = other.float_val; break;
            case ValueType::DOUBLE: double_val = other.double_val; break;
            case ValueType::CHAR: char_val = other.char_val; break;
            default: break;
        }
    }
    
    // Assignment operator
    Value& operator=(const Value& other) {
        if (this != &other) {
            type = other.type;
            string_val = other.string_val;
            switch (type) {
                case ValueType::INTEGER: int_val = other.int_val; break;
                case ValueType::LONG: long_val = other.long_val; break;
                case ValueType::LONG_LONG: long_long_val = other.long_long_val; break;
                case ValueType::FLOAT: float_val = other.float_val; break;
                case ValueType::DOUBLE: double_val = other.double_val; break;
                case ValueType::CHAR: char_val = other.char_val; break;
                default: break;
            }
        }
        return *this;
    }
};

// Classe principal Token
class Token {
private:
    TokenType type_;
    std::string lexeme_;
    Position position_;
    Value value_;
    
public:
    // Construtores
    Token();
    Token(TokenType type, const std::string& lexeme, const Position& pos);
    Token(TokenType type, const std::string& lexeme, const Position& pos, const Value& value);
    
    // Métodos de acesso
    TokenType getType() const;
    const std::string& getLexeme() const;
    const Position& getPosition() const;
    const Value& getValue() const;
    
    // Métodos de classificação
    bool isKeyword() const;
    bool isOperator() const;
    bool isLiteral() const;
    
    // Conversão para string
    std::string toString() const;
    
    // Operadores de comparação
    bool operator==(const Token& other) const;
    bool operator!=(const Token& other) const;
    
    // Operadores de stream
    friend std::ostream& operator<<(std::ostream& os, const Token& token);
    friend std::istream& operator>>(std::istream& is, Token& token);
};

// Funções utilitárias globais
std::string tokenTypeToString(TokenType type);
bool isKeywordToken(TokenType type);
bool isOperatorToken(TokenType type);
bool isLiteralToken(TokenType type);
int getOperatorPrecedence(TokenType type);
Associativity getOperatorAssociativity(TokenType type);

} // namespace Lexer

#endif // TOKEN_HPP