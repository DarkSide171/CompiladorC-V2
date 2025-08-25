#ifndef EXPRESSION_EVALUATOR_HPP
#define EXPRESSION_EVALUATOR_HPP

// Avaliador de Expressões Constantes
// Implementação da Fase 2.3 - Avaliador de Expressões
// Responsável por avaliar expressões constantes em diretivas condicionais

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "preprocessor_types.hpp"
#include "preprocessor_logger.hpp"

namespace Preprocessor {

// Forward declarations
class MacroProcessor;

/**
 * @brief Tipos de tokens para expressões
 */
enum class ExpressionTokenType {
    NUMBER,          // Literal numérico
    IDENTIFIER,      // Identificador/macro
    OPERATOR,        // Operador (+, -, *, /, etc.)
    LEFT_PAREN,      // Parêntese esquerdo
    RIGHT_PAREN,     // Parêntese direito
    DEFINED,         // Operador defined()
    END_OF_EXPR,     // Fim da expressão
    INVALID          // Token inválido
};

/**
 * @brief Token de expressão
 */
struct ExpressionToken {
    ExpressionTokenType type;
    std::string value;
    long long numericValue;  // Valor numérico para tokens NUMBER
    int precedence;
    bool isRightAssociative;
    
    ExpressionToken(ExpressionTokenType t = ExpressionTokenType::INVALID, 
                   const std::string& v = "", 
                   long long numVal = 0,
                   int prec = 0, 
                   bool rightAssoc = false)
        : type(t), value(v), numericValue(numVal), precedence(prec), isRightAssociative(rightAssoc) {}
};

/**
 * @brief Operadores suportados
 */
enum class OperatorType {
    // Aritméticos
    ADD,             // +
    SUBTRACT,        // -
    MULTIPLY,        // *
    DIVIDE,          // /
    MODULO,          // %
    
    // Relacionais
    LESS_THAN,       // <
    GREATER_THAN,    // >
    LESS_EQUAL,      // <=
    GREATER_EQUAL,   // >=
    EQUAL,           // ==
    NOT_EQUAL,       // !=
    
    // Lógicos
    LOGICAL_AND,     // &&
    LOGICAL_OR,      // ||
    LOGICAL_NOT,     // !
    
    // Bitwise
    BITWISE_AND,     // &
    BITWISE_OR,      // |
    BITWISE_XOR,     // ^
    BITWISE_NOT,     // ~
    LEFT_SHIFT,      // <<
    RIGHT_SHIFT,     // >>
    
    // Unários
    UNARY_PLUS,      // +
    UNARY_MINUS,     // -
    
    // Especiais
    DEFINED_OP       // defined()
};

/**
 * @brief Classe principal do avaliador de expressões
 */
class ExpressionEvaluator {
public:
    /**
     * @brief Construtor
     * @param macro_proc Ponteiro para o processador de macros
     * @param logger Ponteiro para o logger
     */
    ExpressionEvaluator(MacroProcessor* macro_proc, PreprocessorLogger* logger);
    
    /**
     * @brief Destrutor
     */
    ~ExpressionEvaluator();
    
    // Métodos principais de avaliação
    
    /**
     * @brief Avalia uma expressão e retorna o resultado como inteiro
     * @param expression String da expressão
     * @param pos Posição no código fonte
     * @return Resultado da avaliação
     */
    long long evaluateExpression(const std::string& expression, const PreprocessorPosition& pos);
    
    /**
     * @brief Avalia uma expressão como booleana
     * @param expression String da expressão
     * @param pos Posição no código fonte
     * @return true se a expressão é verdadeira, false caso contrário
     */
    bool evaluateBooleanExpression(const std::string& expression, const PreprocessorPosition& pos);
    
    /**
     * @brief Verifica se uma expressão é válida
     * @param expression String da expressão
     * @return true se válida, false caso contrário
     */
    bool isValidExpression(const std::string& expression) const;
    
    /**
     * @brief Expande macros em uma expressão
     * @param expression String da expressão
     * @param pos Posição no código fonte
     * @return Expressão com macros expandidas
     */
    std::string expandMacrosInExpression(const std::string& expression, const PreprocessorPosition& pos);
    
    // Métodos de tokenização e parsing
    
    /**
     * @brief Tokeniza uma expressão
     * @param expression String da expressão
     * @return Vetor de tokens
     */
    std::vector<ExpressionToken> tokenizeExpression(const std::string& expression);
    
    /**
     * @brief Faz o parsing dos tokens
     * @param tokens Vetor de tokens
     * @return Tokens organizados para avaliação
     */
    std::vector<ExpressionToken> parseTokens(const std::vector<ExpressionToken>& tokens);
    
private:
    // Membros privados
    MacroProcessor* macroProcessor;
    PreprocessorLogger* logger;
    
    // Mapa de precedência de operadores
    std::unordered_map<std::string, int> operatorPrecedence;
    
    // Mapa de operadores para tipos
    std::unordered_map<std::string, OperatorType> operatorMap;
    
    // Métodos privados de avaliação
    
    /**
     * @brief Avalia uma subexpressão
     * @param tokens Tokens da subexpressão
     * @param start Índice inicial
     * @param end Índice final
     * @return Resultado da avaliação
     */
    long long evaluateSubexpression(const std::vector<ExpressionToken>& tokens, size_t start, size_t end);
    
    // Métodos de manipulação de operadores
    
    /**
     * @brief Trata precedência de operadores
     * @param tokens Tokens da expressão
     * @return Tokens reorganizados por precedência
     */
    std::vector<ExpressionToken> handleOperatorPrecedence(const std::vector<ExpressionToken>& tokens);
    
    /**
     * @brief Aplica um operador
     * @param op Tipo do operador
     * @param left Operando esquerdo
     * @param right Operando direito (opcional para operadores unários)
     * @return Resultado da operação
     */
    long long applyOperator(OperatorType op, long long left, long long right = 0);
    
    /**
     * @brief Avalia operador unário
     * @param op Tipo do operador
     * @param operand Operando
     * @return Resultado da operação
     */
    long long evaluateUnaryOperator(OperatorType op, long long operand);
    
    /**
     * @brief Avalia operador binário
     * @param op Tipo do operador
     * @param left Operando esquerdo
     * @param right Operando direito
     * @return Resultado da operação
     */
    long long evaluateBinaryOperator(OperatorType op, long long left, long long right);
    
    // Métodos de tratamento de parênteses
    
    /**
     * @brief Trata parênteses na expressão
     * @param tokens Tokens da expressão
     * @return Tokens com parênteses processados
     */
    std::vector<ExpressionToken> handleParentheses(const std::vector<ExpressionToken>& tokens);
    
    /**
     * @brief Valida parênteses balanceados
     * @param tokens Tokens da expressão
     * @return true se balanceados, false caso contrário
     */
    bool validateParentheses(const std::vector<ExpressionToken>& tokens);
    
    /**
     * @brief Valida sintaxe de operadores
     * @param tokens Tokens da expressão
     * @return true se sintaxe válida, false caso contrário
     */
    bool validateOperatorSyntax(const std::vector<ExpressionToken>& tokens);
    
    // Métodos de conversão numérica
    
    /**
     * @brief Converte string para número
     * @param str String a ser convertida
     * @return Valor numérico
     */
    long long convertToNumber(const std::string& str);
    
    /**
     * @brief Valida literal numérico
     * @param literal String do literal
     * @return true se válido, false caso contrário
     */
    bool validateNumericLiteral(const std::string& literal);
    
    // Métodos de tratamento de identificadores
    
    /**
     * @brief Trata identificador na expressão
     * @param identifier Nome do identificador
     * @param pos Posição no código fonte
     * @return Valor do identificador
     */
    long long handleIdentifier(const std::string& identifier, const PreprocessorPosition& pos);
    
    /**
     * @brief Resolve valor de identificador
     * @param identifier Nome do identificador
     * @param pos Posição no código fonte
     * @return Valor resolvido
     */
    long long resolveIdentifierValue(const std::string& identifier, const PreprocessorPosition& pos);
    
    // Métodos de otimização
    
    /**
     * @brief Otimiza expressão
     * @param tokens Tokens da expressão
     * @return Tokens otimizados
     */
    std::vector<ExpressionToken> optimizeExpression(const std::vector<ExpressionToken>& tokens);
    
    /**
     * @brief Simplifica expressão
     * @param tokens Tokens da expressão
     * @return Tokens simplificados
     */
    std::vector<ExpressionToken> simplifyExpression(const std::vector<ExpressionToken>& tokens);
    
    // Métodos de tratamento de erros
    
    /**
     * @brief Trata erros de expressão
     * @param error Mensagem de erro
     * @param pos Posição no código fonte
     */
    void handleExpressionErrors(const std::string& error, const PreprocessorPosition& pos);
    
    /**
     * @brief Reporta erro de sintaxe
     * @param message Mensagem de erro
     * @param pos Posição no código fonte
     */
    void reportSyntaxError(const std::string& message, const PreprocessorPosition& pos);
    
    // Métodos auxiliares
    
    /**
     * @brief Inicializa mapas de operadores
     */
    void initializeOperatorMaps();
    
    /**
     * @brief Verifica se é operador
     * @param str String a verificar
     * @return true se é operador, false caso contrário
     */
    bool isOperator(const std::string& str) const;
    
    /**
     * @brief Verifica se é número
     * @param str String a verificar
     * @return true se é número, false caso contrário
     */
    bool isNumber(const std::string& str) const;
    
    /**
     * @brief Verifica se é identificador válido
     * @param str String a verificar
     * @return true se é identificador válido, false caso contrário
     */
    bool isValidIdentifier(const std::string& str) const;
    
    /**
     * @brief Pula espaços em branco
     * @param expr Expressão
     * @param pos Posição atual
     * @return Nova posição após espaços
     */
    size_t skipWhitespace(const std::string& expr, size_t pos) const;
    
    // Métodos auxiliares de parsing
    
    /**
     * @brief Faz parsing de string numérica
     * @param expression Expressão
     * @param pos Posição atual (será atualizada)
     * @return String do número
     */
    std::string parseNumberString(const std::string& expression, size_t& pos);
    
    /**
     * @brief Faz parsing de string de identificador
     * @param expression Expressão
     * @param pos Posição atual (será atualizada)
     * @return String do identificador
     */
    std::string parseIdentifierString(const std::string& expression, size_t& pos);
    
    /**
      * @brief Faz parsing de string de operador
      * @param expression Expressão
      * @param pos Posição atual (será atualizada)
      * @return String do operador
      */
     std::string parseOperatorString(const std::string& expression, size_t& pos);
     
     /**
      * @brief Verifica se um tipo de token é um operador
      * @param type Tipo do token
      * @return true se for operador
      */
     bool isOperatorToken(ExpressionTokenType type) const;
     
     /**
      * @brief Aplica operação binária
      * @param left Operando esquerdo
      * @param right Operando direito
      * @param op Operador
      * @return Resultado da operação
      */
     long long applyBinaryOperation(long long left, long long right, const std::string& op) const;
};

} // namespace Preprocessor

#endif // EXPRESSION_EVALUATOR_HPP