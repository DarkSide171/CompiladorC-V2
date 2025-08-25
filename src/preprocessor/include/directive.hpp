#ifndef DIRECTIVE_HPP
#define DIRECTIVE_HPP

#include <string>
#include <vector>
#include "preprocessor_types.hpp"

namespace Preprocessor {

/**
 * @brief Enumeração dos tipos de diretivas do pré-processador
 * 
 * Define todos os tipos de diretivas suportadas pelo pré-processador,
 * incluindo diretivas condicionais, de inclusão, definição de macros, etc.
 */
enum class DirectiveType {
    // Diretivas básicas
    INCLUDE,    // #include
    DEFINE,     // #define
    UNDEF,      // #undef
    
    // Diretivas condicionais
    IF,         // #if
    IFDEF,      // #ifdef
    IFNDEF,     // #ifndef
    ELSE,       // #else
    ELIF,       // #elif
    ENDIF,      // #endif
    
    // Diretivas de controle
    ERROR,      // #error
    WARNING,    // #warning (extensão comum)
    PRAGMA,     // #pragma
    LINE,       // #line
    
    // Tipo desconhecido
    UNKNOWN
};

/**
 * @brief Classe para representação de uma diretiva de pré-processamento
 * 
 * Encapsula todas as informações necessárias sobre uma diretiva,
 * incluindo tipo, conteúdo, posição e argumentos.
 */
class Directive {
private:
    DirectiveType type_;                    ///< Tipo da diretiva
    std::string content_;                   ///< Conteúdo completo da diretiva
    PreprocessorPosition position_;         ///< Posição no código fonte
    std::vector<std::string> arguments_;    ///< Argumentos da diretiva
    bool valid_;                           ///< Flag de validade
    
public:
    /**
     * @brief Construtor básico
     * @param type Tipo da diretiva
     * @param content Conteúdo da diretiva
     * @param pos Posição no código fonte
     */
    Directive(DirectiveType type, const std::string& content, const PreprocessorPosition& pos);
    
    /**
     * @brief Destrutor
     */
    ~Directive() = default;
    
    // Métodos de acesso
    
    /**
     * @brief Retorna o tipo da diretiva
     * @return Tipo da diretiva
     */
    DirectiveType getType() const { return type_; }
    
    /**
     * @brief Retorna o conteúdo da diretiva
     * @return Conteúdo completo da diretiva
     */
    std::string getContent() const { return content_; }
    
    /**
     * @brief Retorna a posição da diretiva
     * @return Posição no código fonte
     */
    PreprocessorPosition getPosition() const { return position_; }
    
    /**
     * @brief Retorna os argumentos da diretiva
     * @return Vetor com argumentos da diretiva
     */
    std::vector<std::string> getArguments() const { return arguments_; }
    
    /**
     * @brief Verifica se a diretiva é válida
     * @return true se válida, false caso contrário
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Converte a diretiva para representação string
     * @return String representando a diretiva
     */
    std::string toString() const;
    
    // Métodos de validação
    
    /**
     * @brief Valida a sintaxe básica da diretiva
     * @return true se sintaxe válida, false caso contrário
     */
    bool validateSyntax() const;
    
    /**
     * @brief Valida os argumentos da diretiva
     * @return true se argumentos válidos, false caso contrário
     */
    bool validateArguments() const;
    
    /**
     * @brief Define os argumentos da diretiva
     * @param args Vetor com argumentos
     */
    void setArguments(const std::vector<std::string>& args);
    
    /**
     * @brief Marca a diretiva como válida ou inválida
     * @param valid Flag de validade
     */
    void setValid(bool valid) { valid_ = valid; }
};

// Funções utilitárias

/**
 * @brief Converte DirectiveType para string
 * @param type Tipo da diretiva
 * @return Nome da diretiva como string
 */
std::string directiveTypeToString(DirectiveType type);

/**
 * @brief Converte string para DirectiveType
 * @param str Nome da diretiva
 * @return Tipo da diretiva correspondente
 */
DirectiveType stringToDirectiveType(const std::string& str);

/**
 * @brief Verifica se uma diretiva é condicional
 * @param type Tipo da diretiva
 * @return true se for diretiva condicional, false caso contrário
 */
bool isConditionalDirective(DirectiveType type);

/**
 * @brief Verifica se uma diretiva requer argumentos
 * @param type Tipo da diretiva
 * @return true se requer argumentos, false caso contrário
 */
bool requiresArguments(DirectiveType type);

} // namespace Preprocessor

#endif // DIRECTIVE_HPP