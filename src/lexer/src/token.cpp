#include "../include/token.hpp"
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace Lexer {

// Construtores
Token::Token() : type_(TokenType::UNKNOWN), lexeme_(""), position_(), value_() {}

Token::Token(TokenType type, const std::string& lexeme, const Position& pos)
    : type_(type), lexeme_(lexeme), position_(pos), value_() {}

Token::Token(TokenType type, const std::string& lexeme, const Position& pos, const Value& value)
    : type_(type), lexeme_(lexeme), position_(pos), value_(value) {}

// Métodos de acesso
TokenType Token::getType() const {
    return type_;
}

const std::string& Token::getLexeme() const {
    return lexeme_;
}

const Position& Token::getPosition() const {
    return position_;
}

const Value& Token::getValue() const {
    return value_;
}

// Métodos de classificação
bool Token::isKeyword() const {
    return isKeywordToken(type_);
}

bool Token::isOperator() const {
    return isOperatorToken(type_);
}

bool Token::isLiteral() const {
    return isLiteralToken(type_);
}

// Conversão para string
std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token{type=" << tokenTypeToString(type_)
        << ", lexeme=\"" << lexeme_ << "\""
        << ", position=(" << position_.line << "," << position_.column << "," << position_.offset << ")";
    
    // Adicionar valor se presente
    if (value_.type != ValueType::NONE) {
        oss << ", value=";
        switch (value_.type) {
            case ValueType::INTEGER:
                oss << value_.int_val;
                break;
            case ValueType::LONG:
                oss << value_.long_val << "L";
                break;
            case ValueType::LONG_LONG:
                oss << value_.long_long_val << "LL";
                break;
            case ValueType::FLOAT:
                oss << value_.float_val << "f";
                break;
            case ValueType::DOUBLE:
                oss << value_.double_val;
                break;
            case ValueType::CHAR:
                oss << "'" << value_.char_val << "'";
                break;
            case ValueType::STRING:
                oss << "\"" << value_.string_val << "\"";
                break;
            default:
                break;
        }
    }
    
    oss << "}";
    return oss.str();
}

// Operadores de comparação
bool Token::operator==(const Token& other) const {
    return type_ == other.type_ && lexeme_ == other.lexeme_ && position_ == other.position_;
}

bool Token::operator!=(const Token& other) const {
    return !(*this == other);
}

// Operadores de stream
std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << token.toString();
    return os;
}

std::istream& operator>>(std::istream& is, Token& token) {
    // Implementação básica para leitura de token
    std::string type_str, lexeme;
    is >> type_str >> lexeme;
    
    // Esta é uma implementação simplificada
    // Em uma implementação completa, seria necessário parsing mais sofisticado
    token.lexeme_ = lexeme;
    token.type_ = TokenType::UNKNOWN; // Default
    
    return is;
}

// Funções utilitárias globais
std::string tokenTypeToString(TokenType type) {
    static const std::unordered_map<TokenType, std::string> tokenNames = {
        {TokenType::END_OF_FILE, "END_OF_FILE"},
        {TokenType::UNKNOWN, "UNKNOWN"},
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::INTEGER_LITERAL, "INTEGER_LITERAL"},
        {TokenType::FLOAT_LITERAL, "FLOAT_LITERAL"},
        {TokenType::CHAR_LITERAL, "CHAR_LITERAL"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"},
        
        // Palavras-chave C89/C90
        {TokenType::AUTO, "AUTO"}, {TokenType::BREAK, "BREAK"}, {TokenType::CASE, "CASE"},
        {TokenType::CHAR, "CHAR"}, {TokenType::CONST, "CONST"}, {TokenType::CONTINUE, "CONTINUE"},
        {TokenType::DEFAULT, "DEFAULT"}, {TokenType::DO, "DO"}, {TokenType::DOUBLE, "DOUBLE"},
        {TokenType::ELSE, "ELSE"}, {TokenType::ENUM, "ENUM"}, {TokenType::EXTERN, "EXTERN"},
        {TokenType::FLOAT, "FLOAT"}, {TokenType::FOR, "FOR"}, {TokenType::GOTO, "GOTO"},
        {TokenType::IF, "IF"}, {TokenType::INT, "INT"}, {TokenType::LONG, "LONG"},
        {TokenType::REGISTER, "REGISTER"}, {TokenType::RETURN, "RETURN"}, {TokenType::SHORT, "SHORT"},
        {TokenType::SIGNED, "SIGNED"}, {TokenType::SIZEOF, "SIZEOF"}, {TokenType::STATIC, "STATIC"},
        {TokenType::STRUCT, "STRUCT"}, {TokenType::SWITCH, "SWITCH"}, {TokenType::TYPEDEF, "TYPEDEF"},
        {TokenType::UNION, "UNION"}, {TokenType::UNSIGNED, "UNSIGNED"}, {TokenType::VOID, "VOID"},
        {TokenType::VOLATILE, "VOLATILE"}, {TokenType::WHILE, "WHILE"},
        
        // Palavras-chave C99
        {TokenType::INLINE, "INLINE"}, {TokenType::RESTRICT, "RESTRICT"}, {TokenType::_BOOL, "_BOOL"},
        {TokenType::_COMPLEX, "_COMPLEX"}, {TokenType::_IMAGINARY, "_IMAGINARY"},
        
        // Palavras-chave C11
        {TokenType::_ALIGNAS, "_ALIGNAS"}, {TokenType::_ALIGNOF, "_ALIGNOF"}, {TokenType::_ATOMIC, "_ATOMIC"},
        {TokenType::_STATIC_ASSERT, "_STATIC_ASSERT"}, {TokenType::_NORETURN, "_NORETURN"},
        {TokenType::_THREAD_LOCAL, "_THREAD_LOCAL"}, {TokenType::_GENERIC, "_GENERIC"},
        
        // Palavras-chave C23
        {TokenType::TYPEOF, "TYPEOF"}, {TokenType::TYPEOF_UNQUAL, "TYPEOF_UNQUAL"},
        {TokenType::_BITINT, "_BITINT"}, {TokenType::_DECIMAL128, "_DECIMAL128"},
        {TokenType::_DECIMAL32, "_DECIMAL32"}, {TokenType::_DECIMAL64, "_DECIMAL64"},
        
        // Operadores
        {TokenType::PLUS, "PLUS"}, {TokenType::MINUS, "MINUS"}, {TokenType::MULTIPLY, "MULTIPLY"},
        {TokenType::DIVIDE, "DIVIDE"}, {TokenType::MODULO, "MODULO"}, {TokenType::INCREMENT, "INCREMENT"},
        {TokenType::DECREMENT, "DECREMENT"}, {TokenType::ASSIGN, "ASSIGN"}, {TokenType::PLUS_ASSIGN, "PLUS_ASSIGN"},
        {TokenType::MINUS_ASSIGN, "MINUS_ASSIGN"}, {TokenType::MULT_ASSIGN, "MULT_ASSIGN"},
        {TokenType::DIV_ASSIGN, "DIV_ASSIGN"}, {TokenType::MOD_ASSIGN, "MOD_ASSIGN"},
        {TokenType::EQUAL, "EQUAL"}, {TokenType::NOT_EQUAL, "NOT_EQUAL"}, {TokenType::LESS_THAN, "LESS_THAN"},
        {TokenType::GREATER_THAN, "GREATER_THAN"}, {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"}, {TokenType::LOGICAL_AND, "LOGICAL_AND"},
        {TokenType::LOGICAL_OR, "LOGICAL_OR"}, {TokenType::LOGICAL_NOT, "LOGICAL_NOT"},
        {TokenType::BITWISE_AND, "BITWISE_AND"}, {TokenType::BITWISE_OR, "BITWISE_OR"},
        {TokenType::BITWISE_XOR, "BITWISE_XOR"}, {TokenType::BITWISE_NOT, "BITWISE_NOT"},
        {TokenType::LEFT_SHIFT, "LEFT_SHIFT"}, {TokenType::RIGHT_SHIFT, "RIGHT_SHIFT"},
        {TokenType::AND_ASSIGN, "AND_ASSIGN"}, {TokenType::OR_ASSIGN, "OR_ASSIGN"},
        {TokenType::XOR_ASSIGN, "XOR_ASSIGN"}, {TokenType::LEFT_SHIFT_ASSIGN, "LEFT_SHIFT_ASSIGN"},
        {TokenType::RIGHT_SHIFT_ASSIGN, "RIGHT_SHIFT_ASSIGN"}, {TokenType::CONDITIONAL, "CONDITIONAL"},
        {TokenType::COLON, "COLON"}, {TokenType::COMMA, "COMMA"}, {TokenType::DOT, "DOT"},
        {TokenType::ARROW, "ARROW"},
        
        // Delimitadores
        {TokenType::LEFT_PAREN, "LEFT_PAREN"}, {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::LEFT_BRACKET, "LEFT_BRACKET"}, {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"}, {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        
        // Comentários
        {TokenType::LINE_COMMENT, "LINE_COMMENT"}, {TokenType::BLOCK_COMMENT, "BLOCK_COMMENT"}
    };
    
    auto it = tokenNames.find(type);
    return (it != tokenNames.end()) ? it->second : "UNKNOWN_TOKEN_TYPE";
}

bool isKeywordToken(TokenType type) {
    static const std::unordered_set<TokenType> keywords = {
        // C89/C90
        TokenType::AUTO, TokenType::BREAK, TokenType::CASE, TokenType::CHAR, TokenType::CONST,
        TokenType::CONTINUE, TokenType::DEFAULT, TokenType::DO, TokenType::DOUBLE, TokenType::ELSE,
        TokenType::ENUM, TokenType::EXTERN, TokenType::FLOAT, TokenType::FOR, TokenType::GOTO,
        TokenType::IF, TokenType::INT, TokenType::LONG, TokenType::REGISTER, TokenType::RETURN,
        TokenType::SHORT, TokenType::SIGNED, TokenType::SIZEOF, TokenType::STATIC, TokenType::STRUCT,
        TokenType::SWITCH, TokenType::TYPEDEF, TokenType::UNION, TokenType::UNSIGNED, TokenType::VOID,
        TokenType::VOLATILE, TokenType::WHILE,
        
        // C99
        TokenType::INLINE, TokenType::RESTRICT, TokenType::_BOOL, TokenType::_COMPLEX, TokenType::_IMAGINARY,
        
        // C11
        TokenType::_ALIGNAS, TokenType::_ALIGNOF, TokenType::_ATOMIC, TokenType::_STATIC_ASSERT,
        TokenType::_NORETURN, TokenType::_THREAD_LOCAL, TokenType::_GENERIC,
        
        // C23
        TokenType::TYPEOF, TokenType::TYPEOF_UNQUAL, TokenType::_BITINT, TokenType::_DECIMAL128,
        TokenType::_DECIMAL32, TokenType::_DECIMAL64
    };
    
    return keywords.find(type) != keywords.end();
}

bool isOperatorToken(TokenType type) {
    static const std::unordered_set<TokenType> operators = {
        TokenType::PLUS, TokenType::MINUS, TokenType::MULTIPLY, TokenType::DIVIDE, TokenType::MODULO,
        TokenType::INCREMENT, TokenType::DECREMENT, TokenType::ASSIGN, TokenType::PLUS_ASSIGN,
        TokenType::MINUS_ASSIGN, TokenType::MULT_ASSIGN, TokenType::DIV_ASSIGN, TokenType::MOD_ASSIGN,
        TokenType::EQUAL, TokenType::NOT_EQUAL, TokenType::LESS_THAN, TokenType::GREATER_THAN,
        TokenType::LESS_EQUAL, TokenType::GREATER_EQUAL, TokenType::LOGICAL_AND, TokenType::LOGICAL_OR,
        TokenType::LOGICAL_NOT, TokenType::BITWISE_AND, TokenType::BITWISE_OR, TokenType::BITWISE_XOR,
        TokenType::BITWISE_NOT, TokenType::LEFT_SHIFT, TokenType::RIGHT_SHIFT, TokenType::AND_ASSIGN,
        TokenType::OR_ASSIGN, TokenType::XOR_ASSIGN, TokenType::LEFT_SHIFT_ASSIGN, TokenType::RIGHT_SHIFT_ASSIGN,
        TokenType::CONDITIONAL, TokenType::COLON, TokenType::COMMA, TokenType::DOT, TokenType::ARROW
    };
    
    return operators.find(type) != operators.end();
}

bool isLiteralToken(TokenType type) {
    static const std::unordered_set<TokenType> literals = {
        TokenType::INTEGER_LITERAL, TokenType::FLOAT_LITERAL, TokenType::CHAR_LITERAL, TokenType::STRING_LITERAL
    };
    
    return literals.find(type) != literals.end();
}

int getOperatorPrecedence(TokenType type) {
    static const std::unordered_map<TokenType, int> precedence = {
        // Precedência 1 (mais alta)
        {TokenType::LEFT_PAREN, 1}, {TokenType::RIGHT_PAREN, 1},
        {TokenType::LEFT_BRACKET, 1}, {TokenType::RIGHT_BRACKET, 1},
        {TokenType::DOT, 1}, {TokenType::ARROW, 1},
        
        // Precedência 2
        {TokenType::LOGICAL_NOT, 2}, {TokenType::BITWISE_NOT, 2},
        {TokenType::INCREMENT, 2}, {TokenType::DECREMENT, 2},
        {TokenType::SIZEOF, 2},
        
        // Precedência 3
        {TokenType::MULTIPLY, 3}, {TokenType::DIVIDE, 3}, {TokenType::MODULO, 3},
        
        // Precedência 4
        {TokenType::PLUS, 4}, {TokenType::MINUS, 4},
        
        // Precedência 5
        {TokenType::LEFT_SHIFT, 5}, {TokenType::RIGHT_SHIFT, 5},
        
        // Precedência 6
        {TokenType::LESS_THAN, 6}, {TokenType::LESS_EQUAL, 6},
        {TokenType::GREATER_THAN, 6}, {TokenType::GREATER_EQUAL, 6},
        
        // Precedência 7
        {TokenType::EQUAL, 7}, {TokenType::NOT_EQUAL, 7},
        
        // Precedência 8
        {TokenType::BITWISE_AND, 8},
        
        // Precedência 9
        {TokenType::BITWISE_XOR, 9},
        
        // Precedência 10
        {TokenType::BITWISE_OR, 10},
        
        // Precedência 11
        {TokenType::LOGICAL_AND, 11},
        
        // Precedência 12
        {TokenType::LOGICAL_OR, 12},
        
        // Precedência 13
        {TokenType::CONDITIONAL, 13}, {TokenType::COLON, 13},
        
        // Precedência 14
        {TokenType::ASSIGN, 14}, {TokenType::PLUS_ASSIGN, 14}, {TokenType::MINUS_ASSIGN, 14},
        {TokenType::MULT_ASSIGN, 14}, {TokenType::DIV_ASSIGN, 14}, {TokenType::MOD_ASSIGN, 14},
        {TokenType::AND_ASSIGN, 14}, {TokenType::OR_ASSIGN, 14}, {TokenType::XOR_ASSIGN, 14},
        {TokenType::LEFT_SHIFT_ASSIGN, 14}, {TokenType::RIGHT_SHIFT_ASSIGN, 14},
        
        // Precedência 15 (mais baixa)
        {TokenType::COMMA, 15}
    };
    
    auto it = precedence.find(type);
    return (it != precedence.end()) ? it->second : 0;
}

Associativity getOperatorAssociativity(TokenType type) {
    static const std::unordered_map<TokenType, Associativity> associativity = {
        // Associatividade à esquerda
        {TokenType::MULTIPLY, Associativity::LEFT}, {TokenType::DIVIDE, Associativity::LEFT},
        {TokenType::MODULO, Associativity::LEFT}, {TokenType::PLUS, Associativity::LEFT},
        {TokenType::MINUS, Associativity::LEFT}, {TokenType::LEFT_SHIFT, Associativity::LEFT},
        {TokenType::RIGHT_SHIFT, Associativity::LEFT}, {TokenType::LESS_THAN, Associativity::LEFT},
        {TokenType::LESS_EQUAL, Associativity::LEFT}, {TokenType::GREATER_THAN, Associativity::LEFT},
        {TokenType::GREATER_EQUAL, Associativity::LEFT}, {TokenType::EQUAL, Associativity::LEFT},
        {TokenType::NOT_EQUAL, Associativity::LEFT}, {TokenType::BITWISE_AND, Associativity::LEFT},
        {TokenType::BITWISE_XOR, Associativity::LEFT}, {TokenType::BITWISE_OR, Associativity::LEFT},
        {TokenType::LOGICAL_AND, Associativity::LEFT}, {TokenType::LOGICAL_OR, Associativity::LEFT},
        {TokenType::COMMA, Associativity::LEFT}, {TokenType::DOT, Associativity::LEFT},
        {TokenType::ARROW, Associativity::LEFT},
        
        // Associatividade à direita
        {TokenType::LOGICAL_NOT, Associativity::RIGHT}, {TokenType::BITWISE_NOT, Associativity::RIGHT},
        {TokenType::INCREMENT, Associativity::RIGHT}, {TokenType::DECREMENT, Associativity::RIGHT},
        {TokenType::SIZEOF, Associativity::RIGHT}, {TokenType::CONDITIONAL, Associativity::RIGHT},
        {TokenType::ASSIGN, Associativity::RIGHT}, {TokenType::PLUS_ASSIGN, Associativity::RIGHT},
        {TokenType::MINUS_ASSIGN, Associativity::RIGHT}, {TokenType::MULT_ASSIGN, Associativity::RIGHT},
        {TokenType::DIV_ASSIGN, Associativity::RIGHT}, {TokenType::MOD_ASSIGN, Associativity::RIGHT},
        {TokenType::AND_ASSIGN, Associativity::RIGHT}, {TokenType::OR_ASSIGN, Associativity::RIGHT},
        {TokenType::XOR_ASSIGN, Associativity::RIGHT}, {TokenType::LEFT_SHIFT_ASSIGN, Associativity::RIGHT},
        {TokenType::RIGHT_SHIFT_ASSIGN, Associativity::RIGHT}
    };
    
    auto it = associativity.find(type);
    return (it != associativity.end()) ? it->second : Associativity::NONE;
}

} // namespace Lexer