#ifndef LEXER_STATE_HPP
#define LEXER_STATE_HPP

#include "token.hpp"
#include "error_handler.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

// Forward declaration
namespace Lexer {
    enum class LexerState;
}

// Especialização de hash para std::pair<LexerState, char>
namespace std {
    template<>
    struct hash<std::pair<Lexer::LexerState, char>> {
        std::size_t operator()(const std::pair<Lexer::LexerState, char>& p) const {
            auto h1 = std::hash<int>{}(static_cast<int>(p.first));
            auto h2 = std::hash<char>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };
}

namespace Lexer {
// Enumeração de todos os estados possíveis da máquina de estados
enum class LexerState {
    // Estado inicial
    START,
    
    // Estados para identificadores e palavras-chave
    IDENTIFIER,
    
    // Estados para números
    INTEGER,
    FLOAT_DOT,
    FLOAT_DIGITS,
    FLOAT_EXP,
    FLOAT_EXP_SIGN,
    FLOAT_EXP_DIGITS,
    HEX_PREFIX,
    HEX_DIGITS,
    OCTAL_DIGITS,
    BINARY_PREFIX,
    BINARY_DIGITS,
    
    // Estados para strings
    STRING_START,
    STRING_BODY,
    STRING_ESCAPE,
    STRING_HEX_ESCAPE,
    STRING_OCTAL_ESCAPE,
    STRING_END,
    
    // Estados para caracteres
    CHAR_START,
    CHAR_BODY,
    CHAR_ESCAPE,
    CHAR_END,
    
    // Estados para operadores
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    ASSIGN,
    EQUAL,
    NOT,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LOGICAL_AND_FIRST,
    LOGICAL_AND,
    LOGICAL_OR_FIRST,
    LOGICAL_OR,
    BITWISE_AND,
    BITWISE_OR,
    BITWISE_XOR,
    BITWISE_NOT,
    LEFT_SHIFT_FIRST,
    LEFT_SHIFT,
    RIGHT_SHIFT_FIRST,
    RIGHT_SHIFT,
    INCREMENT_FIRST,
    INCREMENT,
    DECREMENT_FIRST,
    DECREMENT,
    ARROW_FIRST,
    ARROW,
    
    // Estados para comentários
    COMMENT_START,
    LINE_COMMENT,
    BLOCK_COMMENT,
    BLOCK_COMMENT_END,
    
    // Estados para delimitadores
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    SEMICOLON,
    COMMA,
    DOT,
    COLON,
    QUESTION,
    
    // Estados especiais
    WHITESPACE,
    NEWLINE,
    END_OF_FILE,
    ERROR,
    
    // Estados de aceitação (finais)
    ACCEPT_IDENTIFIER,
    ACCEPT_INTEGER,
    ACCEPT_FLOAT,
    ACCEPT_HEX,
    ACCEPT_OCTAL,
    ACCEPT_BINARY,
    ACCEPT_STRING,
    ACCEPT_CHAR,
    ACCEPT_OPERATOR,
    ACCEPT_DELIMITER,
    ACCEPT_COMMENT,
    ACCEPT_EOF
};

// Classe para gerenciar a máquina de estados do lexer
class StateMachine {
private:
    LexerState current_state_;
    std::unordered_map<std::pair<LexerState, char>, LexerState> transition_table_;
    std::shared_ptr<ErrorHandler> error_handler_;
    
    // Métodos auxiliares privados
    void initializeTransitionTable();
    std::string stateKey(LexerState state, char input) const;
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlnum(char c) const;
    bool isHexDigit(char c) const;
    bool isOctalDigit(char c) const;
    bool isBinaryDigit(char c) const;
    bool isWhitespace(char c) const;
    
public:
    // Construtores e destrutor
    StateMachine();
    explicit StateMachine(std::shared_ptr<ErrorHandler> error_handler);
    ~StateMachine() = default;
    
    // Métodos principais de controle de estado
    LexerState getCurrentState() const;
    LexerState transition(char input);
    void reset();
    
    // Métodos de verificação de estado
    bool isAcceptingState() const;
    bool isAcceptingState(LexerState state) const;
    bool isErrorState() const;
    bool isErrorState(LexerState state) const;
    bool isValidTransition(char input) const;
    bool isValidTransition(LexerState from_state, char input) const;
    
    // Métodos relacionados a tokens
    TokenType getTokenType() const;
    TokenType getTokenType(LexerState state) const;
    
    // Métodos internos de transição
    LexerState getNextState(char input) const;
    LexerState getNextState(LexerState from_state, char input) const;
    
    // Método de otimização
    void buildTransitionTable();
    
    // Métodos utilitários
    std::string stateToString(LexerState state) const;
    void setErrorHandler(std::shared_ptr<ErrorHandler> error_handler);
    std::shared_ptr<ErrorHandler> getErrorHandler() const;
    
    // Métodos para debug e logging
    void printCurrentState() const;
    void printTransitionTable() const;
    std::vector<LexerState> getAcceptingStates() const;
    std::vector<LexerState> getErrorStates() const;
};

// Funções utilitárias globais
std::string lexerStateToString(LexerState state);
LexerState stringToLexerState(const std::string& state_str);
bool isAcceptingState(LexerState state);
bool isErrorState(LexerState state);
TokenType stateToTokenType(LexerState state);

} // namespace Lexer

#endif // LEXER_STATE_HPP