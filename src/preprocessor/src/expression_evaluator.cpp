#include "../include/expression_evaluator.hpp"
#include "../include/macro_processor.hpp"
#include "../include/preprocessor_logger.hpp"
#include <cctype>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <stdexcept>
#include <sstream>

namespace Preprocessor {

// Construtor
ExpressionEvaluator::ExpressionEvaluator(MacroProcessor* macro_proc, PreprocessorLogger* logger)
    : macroProcessor(macro_proc), logger(logger) {
    initializeOperatorMaps();
}

// Destrutor
ExpressionEvaluator::~ExpressionEvaluator() = default;

// Método principal de avaliação
long long ExpressionEvaluator::evaluateExpression(const std::string& expression, 
                                                 const PreprocessorPosition& pos) {
    try {
        if (expression.empty()) {
            throw std::invalid_argument("Expression cannot be empty");
        }
        
        // Expandir macros na expressão antes de tokenizar
        std::string expandedExpression = expandMacrosInExpression(expression, pos);
        
        // Tokenizar a expressão expandida
        auto tokens = tokenizeExpression(expandedExpression);
        
        if (tokens.empty()) {
            throw std::invalid_argument("No valid tokens found in expression");
        }
        
        // Validar parênteses balanceados
        if (!validateParentheses(tokens)) {
            throw std::invalid_argument("Unbalanced parentheses");
        }
        
        // Validar sintaxe de operadores
        if (!validateOperatorSyntax(tokens)) {
            handleExpressionErrors("Invalid operator syntax", pos);
            throw std::invalid_argument("Invalid operator syntax");
        }
        
        // Avaliar os tokens usando parsing
        auto parsedTokens = parseTokens(tokens);
        return evaluateSubexpression(parsedTokens, 0, parsedTokens.size());
        
    } catch (const std::exception& e) {
        handleExpressionErrors(e.what(), pos);
        throw; // Relança a exceção para que os testes possam capturá-la
    }
}

// Tokenização da expressão
std::vector<ExpressionToken> ExpressionEvaluator::tokenizeExpression(const std::string& expression) {
    std::vector<ExpressionToken> tokens;
    size_t pos = 0;
    
    while (pos < expression.length()) {
        // Pular espaços em branco
        pos = skipWhitespace(expression, pos);
        if (pos >= expression.length()) break;
        
        // Números (decimais, octais, hexadecimais)
        if (std::isdigit(expression[pos]) || 
            (expression[pos] == '0' && pos + 1 < expression.length() && 
             (expression[pos + 1] == 'x' || expression[pos + 1] == 'X'))) {
            
            std::string numberStr = parseNumberString(expression, pos);
            long long numValue = 0;
            
            // Converter string para número
            if (numberStr.length() > 2 && numberStr[0] == '0' && (numberStr[1] == 'x' || numberStr[1] == 'X')) {
                // Hexadecimal
                numValue = std::stoll(numberStr, nullptr, 16);
            } else if (numberStr.length() > 1 && numberStr[0] == '0' && std::isdigit(numberStr[1])) {
                // Octal
                numValue = std::stoll(numberStr, nullptr, 8);
            } else {
                // Decimal
                numValue = std::stoll(numberStr, nullptr, 10);
            }
            
            tokens.emplace_back(ExpressionTokenType::NUMBER, numberStr, numValue);
            continue;
        }
        
        // Identificadores (incluindo 'defined')
        if (std::isalpha(expression[pos]) || expression[pos] == '_') {
            std::string identifier = parseIdentifierString(expression, pos);
            if (identifier == "defined") {
                tokens.emplace_back(ExpressionTokenType::DEFINED, identifier, 0);
            } else {
                tokens.emplace_back(ExpressionTokenType::IDENTIFIER, identifier, 0);
            }
            continue;
        }
        
        // Operadores e parênteses
        std::string operatorStr = parseOperatorString(expression, pos);
        if (!operatorStr.empty()) {
            if (operatorStr == "(") {
                tokens.emplace_back(ExpressionTokenType::LEFT_PAREN, operatorStr, 0);
            } else if (operatorStr == ")") {
                tokens.emplace_back(ExpressionTokenType::RIGHT_PAREN, operatorStr, 0);
            } else {
                tokens.emplace_back(ExpressionTokenType::OPERATOR, operatorStr, 0);
            }
            continue;
        }
        
        // Token desconhecido - pular
        pos++;
    }
    
    return tokens;
}

// Métodos auxiliares de parsing
std::string ExpressionEvaluator::parseNumberString(const std::string& expression, size_t& pos) {
    size_t start = pos;
    
    // Hexadecimal
    if (pos + 1 < expression.length() && expression[pos] == '0' && 
        (expression[pos + 1] == 'x' || expression[pos + 1] == 'X')) {
        pos += 2; // Pular '0x'
        while (pos < expression.length() && std::isxdigit(expression[pos])) {
            pos++;
        }
        return expression.substr(start, pos - start);
    }
    
    // Octal (começa com 0)
    if (expression[pos] == '0' && pos + 1 < expression.length() && 
        std::isdigit(expression[pos + 1])) {
        while (pos < expression.length() && expression[pos] >= '0' && expression[pos] <= '7') {
            pos++;
        }
        return expression.substr(start, pos - start);
    }
    
    // Decimal
    while (pos < expression.length() && std::isdigit(expression[pos])) {
        pos++;
    }
    
    return expression.substr(start, pos - start);
}

std::string ExpressionEvaluator::parseIdentifierString(const std::string& expression, size_t& pos) {
    size_t start = pos;
    
    // Ler identificador
    while (pos < expression.length() && 
           (std::isalnum(expression[pos]) || expression[pos] == '_')) {
        pos++;
    }
    
    return expression.substr(start, pos - start);
}

std::string ExpressionEvaluator::parseOperatorString(const std::string& expression, size_t& pos) {
    // Operadores de dois caracteres
    if (pos + 1 < expression.length()) {
        std::string twoChar = expression.substr(pos, 2);
        
        if (twoChar == "&&" || twoChar == "||" || twoChar == "==" || 
            twoChar == "!=" || twoChar == "<=" || twoChar == ">=" ||
            twoChar == "<<" || twoChar == ">>") {
            pos += 2;
            return twoChar;
        }
    }
    
    // Operadores de um caractere
    char c = expression[pos];
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
        c == '&' || c == '|' || c == '^' || c == '~' || c == '!' ||
        c == '<' || c == '>' || c == '(' || c == ')') {
        pos++;
        return std::string(1, c);
    }
    
    return "";
}

// Inicializar mapas de operadores
void ExpressionEvaluator::initializeOperatorMaps() {
    // Precedência dos operadores (maior número = maior precedência)
    operatorPrecedence["||"] = 1;
    operatorPrecedence["&&"] = 2;
    operatorPrecedence["|"] = 3;
    operatorPrecedence["^"] = 4;
    operatorPrecedence["&"] = 5;
    operatorPrecedence["=="] = 6;
    operatorPrecedence["!="] = 6;
    operatorPrecedence["<"] = 7;
    operatorPrecedence[">"] = 7;
    operatorPrecedence["<="] = 7;
    operatorPrecedence[">="] = 7;
    operatorPrecedence["<<"] = 8;
    operatorPrecedence[">>"] = 8;
    operatorPrecedence["+"] = 9;
    operatorPrecedence["-"] = 9;
    operatorPrecedence["*"] = 10;
    operatorPrecedence["/"] = 10;
    operatorPrecedence["%"] = 10;
    operatorPrecedence["!"] = 11;
    operatorPrecedence["~"] = 11;
    
    // Mapeamento de operadores
    operatorMap["+"] = OperatorType::ADD;
    operatorMap["-"] = OperatorType::SUBTRACT;
    operatorMap["*"] = OperatorType::MULTIPLY;
    operatorMap["/"] = OperatorType::DIVIDE;
    operatorMap["%"] = OperatorType::MODULO;
    operatorMap["<"] = OperatorType::LESS_THAN;
    operatorMap[">"] = OperatorType::GREATER_THAN;
    operatorMap["<="] = OperatorType::LESS_EQUAL;
    operatorMap[">="] = OperatorType::GREATER_EQUAL;
    operatorMap["=="] = OperatorType::EQUAL;
    operatorMap["!="] = OperatorType::NOT_EQUAL;
    operatorMap["&&"] = OperatorType::LOGICAL_AND;
    operatorMap["||"] = OperatorType::LOGICAL_OR;
    operatorMap["!"] = OperatorType::LOGICAL_NOT;
    operatorMap["&"] = OperatorType::BITWISE_AND;
    operatorMap["|"] = OperatorType::BITWISE_OR;
    operatorMap["^"] = OperatorType::BITWISE_XOR;
    operatorMap["~"] = OperatorType::BITWISE_NOT;
    operatorMap["<<"] = OperatorType::LEFT_SHIFT;
    operatorMap[">>"] = OperatorType::RIGHT_SHIFT;
}

// Métodos auxiliares
size_t ExpressionEvaluator::skipWhitespace(const std::string& expr, size_t pos) const {
    while (pos < expr.length() && std::isspace(expr[pos])) {
        pos++;
    }
    return pos;
}

// Implementações dos métodos principais
std::vector<ExpressionToken> ExpressionEvaluator::parseTokens(const std::vector<ExpressionToken>& tokens) {
    // Verificar se há operadores defined() que precisam de tratamento especial
    bool hasDefinedOperator = false;
    for (const auto& token : tokens) {
        if (token.type == ExpressionTokenType::DEFINED) {
            hasDefinedOperator = true;
            break;
        }
    }
    
    // Se há operador defined(), pré-processar para avaliar defined() primeiro
    if (hasDefinedOperator) {
        return preprocessDefinedOperators(tokens);
    }
    
    // Usar o algoritmo Shunting Yard para converter para notação pós-fixa
    return handleOperatorPrecedence(tokens);
}

std::vector<ExpressionToken> ExpressionEvaluator::preprocessDefinedOperators(const std::vector<ExpressionToken>& tokens) {
    std::vector<ExpressionToken> result;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].type == ExpressionTokenType::DEFINED) {
            // Processar defined(identifier) ou defined identifier
            size_t nextIndex = i + 1;
            bool hasParens = false;
            
            // Verificar se há parênteses
            if (nextIndex < tokens.size() && tokens[nextIndex].type == ExpressionTokenType::LEFT_PAREN) {
                hasParens = true;
                nextIndex++; // Pular o parêntese esquerdo
            }
            
            // Obter o identificador
            if (nextIndex < tokens.size() && tokens[nextIndex].type == ExpressionTokenType::IDENTIFIER) {
                std::string macroName = tokens[nextIndex].value;
                bool isDefined = macroProcessor->isDefined(macroName);
                
                // Criar token numérico com o resultado
                ExpressionToken resultToken;
                resultToken.type = ExpressionTokenType::NUMBER;
                resultToken.value = isDefined ? "1" : "0";
                resultToken.numericValue = isDefined ? 1 : 0;
                result.push_back(resultToken);
                
                // Avançar o índice
                i = nextIndex;
                if (hasParens) {
                    // Pular o parêntese direito se existir
                    if (i + 1 < tokens.size() && tokens[i + 1].type == ExpressionTokenType::RIGHT_PAREN) {
                        i++;
                    }
                }
            } else {
                // Erro: defined sem identificador válido
                result.push_back(tokens[i]);
            }
        } else {
            result.push_back(tokens[i]);
        }
    }
    
    // Aplicar Shunting Yard ao resultado processado
    return handleOperatorPrecedence(result);
}

long long ExpressionEvaluator::evaluateSubexpression(const std::vector<ExpressionToken>& tokens, size_t start, size_t end) {
    if (start >= end || start >= tokens.size()) {
        return 0;
    }
    
    // Avaliar expressão em notação pós-fixa usando uma pilha
    std::vector<long long> stack;
    
    for (size_t i = start; i < end; ++i) {
        const auto& token = tokens[i];
        
        if (token.type == ExpressionTokenType::NUMBER) {
            stack.push_back(token.numericValue);
        }
        else if (token.type == ExpressionTokenType::DEFINED) {
            // Operador defined() - pode ser defined(identifier) ou defined identifier
            std::string macroName;
            bool foundMacro = false;
            
            // Verificar se próximo token é parêntese esquerdo
            if (i + 1 < end && tokens[i + 1].type == ExpressionTokenType::LEFT_PAREN) {
                // Formato: defined(identifier)
                if (i + 2 < end && tokens[i + 2].type == ExpressionTokenType::IDENTIFIER &&
                    i + 3 < end && tokens[i + 3].type == ExpressionTokenType::RIGHT_PAREN) {
                    macroName = tokens[i + 2].value;
                    foundMacro = true;
                    i += 3; // Pular '(', 'identifier', ')'
                }
            } else if (i + 1 < end && tokens[i + 1].type == ExpressionTokenType::IDENTIFIER) {
                // Formato: defined identifier
                macroName = tokens[i + 1].value;
                foundMacro = true;
                ++i; // Pular o identificador
            }
            
            if (foundMacro) {
                bool isDefined = false;
                if (macroProcessor != nullptr) {
                    isDefined = macroProcessor->isDefined(macroName);
                }
                stack.push_back(isDefined ? 1 : 0);
            } else {
                // Erro: defined() sem identificador válido
                stack.push_back(0);
            }
        }
        else if (token.type == ExpressionTokenType::IDENTIFIER) {
            // Resolver valor do identificador
            long long value = resolveIdentifierValue(token.value, PreprocessorPosition());
            stack.push_back(value);
        }
        else if (token.type == ExpressionTokenType::OPERATOR) {
            // Verificar se é operador unário (marcado com 'u')
            if (token.value.length() > 1 && token.value[0] == 'u') {
                // Operador unário
                if (stack.size() >= 1) {
                    long long operand = stack.back();
                    stack.pop_back();
                    std::string op = token.value.substr(1); // Remover o 'u'
                    if (op == "-") {
                        stack.push_back(-operand);
                    } else if (op == "+") {
                        stack.push_back(operand); // + unário não muda o valor
                    } else if (op == "!") {
                        stack.push_back(operand == 0 ? 1 : 0); // NOT lógico
                    }
                }
            }
            // Verificar se é operador NOT (!)
            else if (token.value == "!") {
                // Operador NOT unário
                if (stack.size() >= 1) {
                    long long operand = stack.back();
                    stack.pop_back();
                    stack.push_back(!operand ? 1 : 0);
                }
            }
            else if (stack.size() >= 2) {
                // Operador binário
                long long right = stack.back(); stack.pop_back();
                long long left = stack.back(); stack.pop_back();
                stack.push_back(applyBinaryOperation(left, right, token.value));
            }
        }
    }
    
    return stack.empty() ? 0 : stack.back();
}

// Método auxiliar para verificar se é um token de operador
bool ExpressionEvaluator::isOperatorToken(ExpressionTokenType type) const {
    return type == ExpressionTokenType::OPERATOR;
}

// Método auxiliar para aplicar operações binárias
long long ExpressionEvaluator::applyBinaryOperation(long long left, long long right, const std::string& op) const {
    if (op == "+") return left + right;
    if (op == "-") return left - right;
    if (op == "*") return left * right;
    if (op == "/") {
        if (right == 0) {
            throw std::runtime_error("Division by zero");
        }
        return left / right;
    }
    if (op == "%") {
        if (right == 0) {
            throw std::runtime_error("Modulo by zero");
        }
        return left % right;
    }
    if (op == "<") return left < right ? 1 : 0;
    if (op == ">") return left > right ? 1 : 0;
    if (op == "<=") return left <= right ? 1 : 0;
    if (op == ">=") return left >= right ? 1 : 0;
    if (op == "==") return left == right ? 1 : 0;
    if (op == "!=") return left != right ? 1 : 0;
    if (op == "&&") return (left && right) ? 1 : 0;
    if (op == "||") return (left || right) ? 1 : 0;
    if (op == "&") return left & right;
    if (op == "|") return left | right;
    if (op == "^") return left ^ right;
    if (op == "<<") return left << right;
    if (op == ">>") return left >> right;
    
    return 0;
}

// Aplicação de operadores
long long ExpressionEvaluator::applyOperator(OperatorType op, long long left, long long right) {
    switch (op) {
        case OperatorType::ADD:
            return left + right;
        case OperatorType::SUBTRACT:
            return left - right;
        case OperatorType::MULTIPLY:
            return left * right;
        case OperatorType::DIVIDE:
            if (right == 0) {
                throw std::runtime_error("Division by zero");
            }
            return left / right;
        case OperatorType::MODULO:
            if (right == 0) {
                throw std::runtime_error("Modulo by zero");
            }
            return left % right;
        case OperatorType::LESS_THAN:
            return left < right ? 1 : 0;
        case OperatorType::GREATER_THAN:
            return left > right ? 1 : 0;
        case OperatorType::LESS_EQUAL:
            return left <= right ? 1 : 0;
        case OperatorType::GREATER_EQUAL:
            return left >= right ? 1 : 0;
        case OperatorType::EQUAL:
            return left == right ? 1 : 0;
        case OperatorType::NOT_EQUAL:
            return left != right ? 1 : 0;
        case OperatorType::LOGICAL_AND:
            return (left && right) ? 1 : 0;
        case OperatorType::LOGICAL_OR:
            return (left || right) ? 1 : 0;
        case OperatorType::BITWISE_AND:
            return left & right;
        case OperatorType::BITWISE_OR:
            return left | right;
        case OperatorType::BITWISE_XOR:
            return left ^ right;
        case OperatorType::LEFT_SHIFT:
            return left << right;
        case OperatorType::RIGHT_SHIFT:
            return left >> right;
        case OperatorType::LOGICAL_NOT:
            return !left ? 1 : 0;
        case OperatorType::BITWISE_NOT:
            return ~left;
        case OperatorType::UNARY_PLUS:
            return +left;
        case OperatorType::UNARY_MINUS:
            return -left;
        default:
            throw std::runtime_error("Unknown operator type");
    }
}

// Avaliação de operadores unários
long long ExpressionEvaluator::evaluateUnaryOperator(OperatorType op, long long operand) {
    switch (op) {
        case OperatorType::LOGICAL_NOT:
            return !operand ? 1 : 0;
        case OperatorType::BITWISE_NOT:
            return ~operand;
        case OperatorType::UNARY_PLUS:
            return +operand;
        case OperatorType::UNARY_MINUS:
            return -operand;
        default:
            throw std::runtime_error("Invalid unary operator");
    }
}

// Avaliação de operadores binários
long long ExpressionEvaluator::evaluateBinaryOperator(OperatorType op, long long left, long long right) {
    return applyOperator(op, left, right);
}

// Manipulação de precedência de operadores
std::vector<ExpressionToken> ExpressionEvaluator::handleOperatorPrecedence(const std::vector<ExpressionToken>& tokens) {
    if (tokens.empty()) {
        return tokens;
    }
    
    std::vector<ExpressionToken> output;
    std::vector<ExpressionToken> operatorStack;
    
    // Algoritmo Shunting Yard modificado para lidar com operadores unários
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        
        if (token.type == ExpressionTokenType::NUMBER || token.type == ExpressionTokenType::IDENTIFIER) {
            output.push_back(token);
        }
        else if (token.type == ExpressionTokenType::DEFINED) {
            // O operador defined deve ser tratado como um operador unário especial
            // que tem alta precedência e deve ser processado imediatamente
            operatorStack.push_back(token);
        }
        else if (token.type == ExpressionTokenType::OPERATOR) {
            // Verificar se é operador unário
            bool isUnary = false;
            if ((token.value == "-" || token.value == "+" || token.value == "!") && 
                (i == 0 || tokens[i-1].type == ExpressionTokenType::LEFT_PAREN || 
                 tokens[i-1].type == ExpressionTokenType::OPERATOR)) {
                isUnary = true;
            }
            
            if (isUnary) {
                // Criar um token especial para operador unário
                ExpressionToken unaryToken = token;
                unaryToken.value = "u" + token.value; // Marcar como unário
                operatorStack.push_back(unaryToken);
            } else {
                // Operador binário - usar precedência normal
                int currentPrecedence = 0;
                auto it = operatorPrecedence.find(token.value);
                if (it != operatorPrecedence.end()) {
                    currentPrecedence = it->second;
                }
                
                // Pop operadores com precedência maior ou igual
                while (!operatorStack.empty() && 
                       operatorStack.back().type == ExpressionTokenType::OPERATOR &&
                       operatorStack.back().type != ExpressionTokenType::LEFT_PAREN) {
                    
                    int stackPrecedence = 0;
                    std::string stackOp = operatorStack.back().value;
                    // Operadores unários têm precedência alta
                    if (stackOp.length() > 1 && stackOp[0] == 'u') {
                        stackPrecedence = 100; // Alta precedência para unários
                    } else {
                        auto stackIt = operatorPrecedence.find(stackOp);
                        if (stackIt != operatorPrecedence.end()) {
                            stackPrecedence = stackIt->second;
                        }
                    }
                    
                    if (stackPrecedence >= currentPrecedence) {
                        output.push_back(operatorStack.back());
                        operatorStack.pop_back();
                    } else {
                        break;
                    }
                }
                
                operatorStack.push_back(token);
            }
        }
        else if (token.type == ExpressionTokenType::LEFT_PAREN) {
            operatorStack.push_back(token);
        }
        else if (token.type == ExpressionTokenType::RIGHT_PAREN) {
            // Pop até encontrar o parêntese esquerdo
            while (!operatorStack.empty() && 
                   operatorStack.back().type != ExpressionTokenType::LEFT_PAREN) {
                output.push_back(operatorStack.back());
                operatorStack.pop_back();
            }
            
            // Remover o parêntese esquerdo
            if (!operatorStack.empty()) {
                operatorStack.pop_back();
            }
        }
    }
    
    // Pop todos os operadores restantes
    while (!operatorStack.empty()) {
        output.push_back(operatorStack.back());
        operatorStack.pop_back();
    }
    
    return output;
}

// Validação de parênteses
bool ExpressionEvaluator::validateParentheses(const std::vector<ExpressionToken>& tokens) {
    int balance = 0;
    
    for (const auto& token : tokens) {
        if (token.type == ExpressionTokenType::LEFT_PAREN) {
            balance++;
        } else if (token.type == ExpressionTokenType::RIGHT_PAREN) {
            balance--;
            // Se balance fica negativo, temos parênteses extras à direita
            if (balance < 0) {
                return false;
            }
        }
    }
    
    // Se balance não é zero, temos parênteses não fechados
    return balance == 0;
}

// Validação de sintaxe de operadores
bool ExpressionEvaluator::validateOperatorSyntax(const std::vector<ExpressionToken>& tokens) {
    if (tokens.empty()) {
        return false;
    }
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        
        if (token.type == ExpressionTokenType::OPERATOR) {
            // Operadores no final da expressão são inválidos
            if (i == tokens.size() - 1) {
                return false;
            }
            
            // Verificar operadores consecutivos
            if (i + 1 < tokens.size() && tokens[i + 1].type == ExpressionTokenType::OPERATOR) {
                // Permitir apenas +, - e ! como operadores unários consecutivos
                if (tokens[i+1].value != "+" && tokens[i+1].value != "-" && tokens[i+1].value != "!") {
                    return false; // Outros operadores consecutivos sempre inválidos
                }
                
                // Verificar se há operando após operador unário
                if (i + 2 >= tokens.size() || 
                    (tokens[i + 2].type != ExpressionTokenType::NUMBER && 
                     tokens[i + 2].type != ExpressionTokenType::IDENTIFIER &&
                     tokens[i + 2].type != ExpressionTokenType::LEFT_PAREN)) {
                    return false;
                }
                
                // Casos específicos inválidos: "número + + número" ou "número - - número"
                // Exemplo: "5 + + 3" é inválido, mas "-2 * -5" é válido
                if (i > 0 && 
                    (tokens[i-1].type == ExpressionTokenType::NUMBER || tokens[i-1].type == ExpressionTokenType::IDENTIFIER) &&
                    (token.value == "+" || token.value == "-") &&
                    (tokens[i+1].value == token.value)) { // Mesmo operador consecutivo
                    return false; // Casos como "5 + + 3" ou "5 - - 3"
                }
            }
        }
        
        // Operador no início só é válido se for unário
        if (i == 0 && token.type == ExpressionTokenType::OPERATOR) {
            if (token.value != "+" && token.value != "-" && token.value != "!") {
                return false;
            }
        }
    }
    
    return true;
}

// Tratamento de erros
void ExpressionEvaluator::handleExpressionErrors(const std::string& errorMessage,
                                                const PreprocessorPosition& position) {
    if (logger) {
        logger->error("Expression evaluation error: " + errorMessage, position);
    }
}

// Avaliação de expressões booleanas
bool ExpressionEvaluator::evaluateBooleanExpression(const std::string& expression, 
                                                   const PreprocessorPosition& pos) {
    try {
        long long result = evaluateExpression(expression, pos);
        return result != 0;  // Em C, qualquer valor não-zero é verdadeiro
    } catch (const std::exception& e) {
        handleExpressionErrors("Boolean expression evaluation failed: " + std::string(e.what()), pos);
        return false;
    }
}

// Validação de expressão
bool ExpressionEvaluator::isValidExpression(const std::string& expression) const {
    if (expression.empty()) {
        return false;
    }
    
    try {
        // Criar uma cópia temporária para tokenização
        ExpressionEvaluator* temp = const_cast<ExpressionEvaluator*>(this);
        auto tokens = temp->tokenizeExpression(expression);
        
        if (tokens.empty()) {
            return false;
        }
        
        // Validar parênteses balanceados
        if (!temp->validateParentheses(tokens)) {
            return false;
        }
        
        // Validar sintaxe de operadores
        if (!temp->validateOperatorSyntax(tokens)) {
            return false;
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Expansão de macros em expressões
std::string ExpressionEvaluator::expandMacrosInExpression(const std::string& expression, 
                                                         const PreprocessorPosition& pos) {
    if (!macroProcessor) {
        return expression;
    }
    
    // Verificar se a expressão contém operador defined()
    // Se contém, não expandir macros para preservar a semântica do defined()
    if (expression.find("defined") != std::string::npos) {
        return expression;
    }
    
    std::string result = expression;
    
    // Tokenizar para encontrar identificadores
    auto tokens = tokenizeExpression(expression);
    
    // Substituir macros na string original
    for (const auto& token : tokens) {
        if (token.type == ExpressionTokenType::IDENTIFIER) {
            // Verificar se é uma macro definida e expandir se necessário
            if (macroProcessor && macroProcessor->isDefined(token.value)) {
                std::string expanded = macroProcessor->expandMacro(token.value);
                // Substituir todas as ocorrências do identificador pela expansão da macro
                size_t pos = 0;
                while ((pos = result.find(token.value, pos)) != std::string::npos) {
                    // Verificar se é uma palavra completa (não parte de outro identificador)
                    bool isWordBoundary = true;
                    if (pos > 0 && (std::isalnum(result[pos-1]) || result[pos-1] == '_')) {
                        isWordBoundary = false;
                    }
                    if (pos + token.value.length() < result.length() && 
                        (std::isalnum(result[pos + token.value.length()]) || result[pos + token.value.length()] == '_')) {
                        isWordBoundary = false;
                    }
                    
                    if (isWordBoundary) {
                        result.replace(pos, token.value.length(), expanded);
                        pos += expanded.length();
                    } else {
                        pos += token.value.length();
                    }
                }
            }
        }
    }
    
    return result;
}

// Processamento de parênteses
std::vector<ExpressionToken> ExpressionEvaluator::handleParentheses(const std::vector<ExpressionToken>& tokens) {
    std::vector<ExpressionToken> result;
    std::vector<size_t> parenStack;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        
        if (token.type == ExpressionTokenType::LEFT_PAREN) {
            parenStack.push_back(i);
            result.push_back(token);
        } else if (token.type == ExpressionTokenType::RIGHT_PAREN) {
            if (!parenStack.empty()) {
                parenStack.pop_back();
            }
            result.push_back(token);
        } else {
            result.push_back(token);
        }
    }
    
    return result;
}

// Conversão de string para número
long long ExpressionEvaluator::convertToNumber(const std::string& str) {
    if (str.empty()) {
        throw std::invalid_argument("Empty string cannot be converted to number");
    }
    
    try {
        // Detectar base numérica
        if (str.length() > 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
            // Hexadecimal
            return std::stoll(str, nullptr, 16);
        } else if (str.length() > 1 && str[0] == '0' && std::isdigit(str[1])) {
            // Octal
            return std::stoll(str, nullptr, 8);
        } else {
            // Decimal
            return std::stoll(str, nullptr, 10);
        }
    } catch (const std::exception& e) {
        throw std::invalid_argument("Invalid numeric literal: " + str);
    }
}

// Validação de literal numérico
bool ExpressionEvaluator::validateNumericLiteral(const std::string& literal) {
    if (literal.empty()) {
        return false;
    }
    
    try {
        convertToNumber(literal);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Processamento de identificadores
long long ExpressionEvaluator::handleIdentifier(const std::string& identifier, 
                                               const PreprocessorPosition& pos) {
    return resolveIdentifierValue(identifier, pos);
}

// Resolução de valores de identificadores
long long ExpressionEvaluator::resolveIdentifierValue(const std::string& identifier, 
                                                     const PreprocessorPosition& pos) {
    if (!macroProcessor) {
        // Se não há processador de macros, identificadores não definidos são 0
        return 0;
    }
    
    // Verificar se o identificador é uma macro definida
    if (macroProcessor->isDefined(identifier)) {
        std::string macroValue = macroProcessor->expandMacro(identifier);
        // Tentar converter o valor da macro para número
        try {
            return convertToNumber(macroValue);
        } catch (const std::exception&) {
            // Se não conseguir converter, retorna 0
            return 0;
        }
    }
    
    // Identificador não definido é 0 em expressões de preprocessador
    return 0;
}

// Otimização de expressões
std::vector<ExpressionToken> ExpressionEvaluator::optimizeExpression(const std::vector<ExpressionToken>& tokens) {
    // Implementação básica - pode ser expandida para otimizações mais complexas
    std::vector<ExpressionToken> optimized;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        
        // Otimização simples: remover operações com 0
        if (token.type == ExpressionTokenType::OPERATOR && token.value == "+") {
            // Se próximo token é 0, pular ambos
            if (i + 1 < tokens.size() && tokens[i + 1].type == ExpressionTokenType::NUMBER && 
                tokens[i + 1].numericValue == 0) {
                ++i; // Pular o 0
                continue;
            }
        }
        
        optimized.push_back(token);
    }
    
    return optimized;
}

// Simplificação de expressões
std::vector<ExpressionToken> ExpressionEvaluator::simplifyExpression(const std::vector<ExpressionToken>& tokens) {
    // Implementação básica - pode ser expandida para simplificações mais complexas
    std::vector<ExpressionToken> simplified;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        
        // Simplificação: combinar números consecutivos com operadores
        if (token.type == ExpressionTokenType::NUMBER && i + 2 < tokens.size()) {
            const auto& op = tokens[i + 1];
            const auto& nextNum = tokens[i + 2];
            
            if (op.type == ExpressionTokenType::OPERATOR && nextNum.type == ExpressionTokenType::NUMBER) {
                // Avaliar operação simples
                try {
                    long long result = applyBinaryOperation(token.numericValue, nextNum.numericValue, op.value);
                    simplified.emplace_back(ExpressionTokenType::NUMBER, std::to_string(result), result);
                    i += 2; // Pular os próximos dois tokens
                    continue;
                } catch (const std::exception&) {
                    // Se não conseguir avaliar, manter tokens originais
                }
            }
        }
        
        simplified.push_back(token);
    }
    
    return simplified;
}

// Relatório de erro de sintaxe
void ExpressionEvaluator::reportSyntaxError(const std::string& message, const PreprocessorPosition& pos) {
    if (logger) {
        logger->error("Syntax error in expression: " + message, pos);
    }
    throw std::runtime_error("Expression syntax error: " + message);
}

// Verificar se string é um operador
bool ExpressionEvaluator::isOperator(const std::string& str) const {
    return operatorMap.find(str) != operatorMap.end();
}

// Verificar se string é um número
bool ExpressionEvaluator::isNumber(const std::string& str) const {
    if (str.empty()) {
        return false;
    }
    
    size_t start = 0;
    // Permitir sinal negativo ou positivo no início
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
        if (str.length() == 1) {
            return false; // Apenas um sinal não é um número
        }
    }
    
    // Verificar hexadecimal (0x ou 0X)
    if (str.length() > start + 2 && str[start] == '0' && (str[start + 1] == 'x' || str[start + 1] == 'X')) {
        for (size_t i = start + 2; i < str.length(); ++i) {
            if (!std::isxdigit(str[i])) {
                return false;
            }
        }
        return str.length() > start + 2; // Deve ter pelo menos um dígito hex
    }
    
    // Verificar octal (começa com 0)
    if (str.length() > start + 1 && str[start] == '0') {
        for (size_t i = start + 1; i < str.length(); ++i) {
            if (str[i] < '0' || str[i] > '7') {
                return false;
            }
        }
        return true;
    }
    
    // Verificar decimal
    for (size_t i = start; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

// Verificar se string é um identificador válido
bool ExpressionEvaluator::isValidIdentifier(const std::string& str) const {
    if (str.empty()) {
        return false;
    }
    
    // Primeiro caractere deve ser letra ou underscore
    if (!std::isalpha(str[0]) && str[0] != '_') {
        return false;
    }
    
    // Demais caracteres devem ser letras, dígitos ou underscores
    for (size_t i = 1; i < str.length(); ++i) {
        if (!std::isalnum(str[i]) && str[i] != '_') {
            return false;
        }
    }
    
    return true;
}

} // namespace Preprocessor