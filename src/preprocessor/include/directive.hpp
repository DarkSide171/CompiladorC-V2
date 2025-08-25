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

// Forward declarations
class PreprocessorState;
class PreprocessorLogger;
class MacroProcessor;
class FileManager;
class ConditionalProcessor;

/**
 * @brief Classe para processamento avançado de diretivas
 * 
 * Responsável pelo parsing, validação e processamento de todas as diretivas
 * do pré-processador, incluindo tratamento de erros e validação de contexto.
 */
class DirectiveProcessor {
private:
    PreprocessorState* state_;              ///< Estado do pré-processador
    PreprocessorLogger* logger_;            ///< Sistema de logging
    MacroProcessor* macro_processor_;       ///< Processador de macros
    FileManager* file_manager_;             ///< Gerenciador de arquivos
    ConditionalProcessor* conditional_processor_; ///< Processador condicional
    
public:
    /**
     * @brief Construtor
     * @param state Estado do pré-processador
     * @param logger Sistema de logging
     * @param macro_processor Processador de macros
     * @param file_manager Gerenciador de arquivos
     * @param conditional_processor Processador condicional
     */
    DirectiveProcessor(PreprocessorState* state,
                      PreprocessorLogger* logger,
                      MacroProcessor* macro_processor,
                      FileManager* file_manager,
                      ConditionalProcessor* conditional_processor);
    
    /**
     * @brief Destrutor
     */
    ~DirectiveProcessor() = default;
    
    // Métodos de parsing e validação
    
    /**
     * @brief Faz o parsing de uma linha de diretiva
     * @param line Linha contendo a diretiva
     * @param pos Posição no código fonte
     * @return Objeto Directive parseado
     */
    Directive parseDirective(const std::string& line, const PreprocessorPosition& pos);
    
    /**
     * @brief Valida a sintaxe de uma diretiva
     * @param directive Diretiva a ser validada
     * @return true se sintaxe válida, false caso contrário
     */
    bool validateDirectiveSyntax(const Directive& directive);
    
    // Métodos de processamento de diretivas básicas
    
    /**
     * @brief Processa diretiva #include
     * @param filename Nome do arquivo a incluir
     * @param pos Posição no código fonte
     * @return true se processado com sucesso, false caso contrário
     */
    bool processIncludeDirective(const std::string& filename, const PreprocessorPosition& pos);
    
    /**
     * @brief Processa diretiva #define
     * @param macro_name Nome da macro
     * @param macro_value Valor da macro
     * @param pos Posição no código fonte
     * @return true se processado com sucesso, false caso contrário
     */
    bool processDefineDirective(const std::string& macro_name, const std::string& macro_value, const PreprocessorPosition& pos);
    
    // Métodos de processamento de diretivas condicionais
    
    /**
     * @brief Processa diretiva #if
     * @param condition Condição a ser avaliada
     * @param pos Posição no código fonte
     * @return true se processado com sucesso, false caso contrário
     */
    bool processIfDirective(const std::string& condition, const PreprocessorPosition& pos);
    
    /**
     * @brief Processa diretiva #ifdef
     * @param macro_name Nome da macro a verificar
     * @param pos Posição no código fonte
     * @return true se processado com sucesso, false caso contrário
     */
    bool processIfdefDirective(const std::string& macro_name, const PreprocessorPosition& pos);
    
    /**
     * @brief Processa diretiva #ifndef
     * @param macro_name Nome da macro a verificar
     * @param pos Posição no código fonte
     * @return true se processado com sucesso, false caso contrário
     */
    bool processIfndefDirective(const std::string& macro_name, const PreprocessorPosition& pos);
    
    // Métodos de processamento de estruturas condicionais
    
    /**
     * @brief Processa diretiva #else
     * @param pos Posição no código fonte
     * @return true se processado com sucesso, false caso contrário
     */
    bool processElseDirective(const PreprocessorPosition& pos);
    
    /**
     * @brief Processa diretiva #elif
     * @param condition Condição a ser avaliada
     * @param pos Posição no código fonte
     * @return true se processado com sucesso, false caso contrário
     */
    bool processElifDirective(const std::string& condition, const PreprocessorPosition& pos);
    
    /**
     * @brief Processa diretiva #endif
     * @param pos Posição no código fonte
     * @return true se processado com sucesso, false caso contrário
     */
    bool processEndifDirective(const PreprocessorPosition& pos);
    
    // Métodos de processamento de outras diretivas
    
    /**
     * @brief Processa diretiva #undef
     * @param macro_name Nome da macro a remover
     * @param pos Posição no código fonte
     * @return true se processado com sucesso, false caso contrário
     */
    bool processUndefDirective(const std::string& macro_name, const PreprocessorPosition& pos);
    
    /**
     * @brief Processa diretiva #error
     * @param message Mensagem de erro
     * @param pos Posição no código fonte
     * @return true se processado com sucesso, false caso contrário
     */
    bool processErrorDirective(const std::string& message, const PreprocessorPosition& pos);
    
    /**
     * @brief Processa diretiva #warning
     * @param message Mensagem de aviso
     * @param pos Posição no código fonte
     * @return true se processado com sucesso, false caso contrário
     */
    bool processWarningDirective(const std::string& message, const PreprocessorPosition& pos);
    
    // Métodos de processamento de diretivas especiais
    
    /**
     * @brief Processa diretiva #pragma
     * @param pragma_text Texto do pragma
     * @param pos Posição no código fonte
     * @return true se processado com sucesso, false caso contrário
     */
    bool processPragmaDirective(const std::string& pragma_text, const PreprocessorPosition& pos);
    
    /**
     * @brief Processa diretiva #line
     * @param line_info Informações da linha
     * @param pos Posição no código fonte
     * @return true se processado com sucesso, false caso contrário
     */
    bool processLineDirective(const std::string& line_info, const PreprocessorPosition& pos);
    
    // Métodos de tratamento de erros
    
    /**
     * @brief Manipula erros de diretiva
     * @param error_msg Mensagem de erro
     * @param pos Posição no código fonte
     */
    void handleDirectiveErrors(const std::string& error_msg, const PreprocessorPosition& pos);
    
    /**
     * @brief Reporta erro de diretiva
     * @param directive Diretiva com erro
     * @param error_msg Mensagem de erro
     */
    void reportDirectiveError(const Directive& directive, const std::string& error_msg);
    
    // Métodos de validação de contexto
    
    /**
     * @brief Valida contexto da diretiva
     * @param type Tipo da diretiva
     * @return true se contexto válido, false caso contrário
     */
    bool validateDirectiveContext(DirectiveType type);
    
    /**
     * @brief Verifica aninhamento de diretivas
     * @param type Tipo da diretiva
     * @return true se aninhamento válido, false caso contrário
     */
    bool checkDirectiveNesting(DirectiveType type);
    
    // Métodos de otimização
    
    /**
     * @brief Otimiza processamento de diretivas
     */
    void optimizeDirectiveProcessing();
    
    // Métodos auxiliares
    
    /**
     * @brief Extrai nome da diretiva de uma linha
     * @param line Linha contendo a diretiva
     * @return Nome da diretiva
     */
    std::string extractDirectiveName(const std::string& line);
    
    /**
     * @brief Extrai argumentos da diretiva
     * @param line Linha contendo a diretiva
     * @param directive_name Nome da diretiva
     * @return Argumentos da diretiva
     */
    std::string extractDirectiveArguments(const std::string& line, const std::string& directive_name);
    
    /**
     * @brief Normaliza linha de diretiva
     * @param line Linha a ser normalizada
     * @return Linha normalizada
     */
    std::string normalizeDirectiveLine(const std::string& line);
    
    /**
     * @brief Atualiza estatísticas de diretiva
     * @param type Tipo da diretiva
     */
    void updateDirectiveStatistics(DirectiveType type);
    
    /**
     * @brief Registra processamento de diretiva
     * @param directive Diretiva processada
     * @param pos Posição no código fonte
     */
    void logDirectiveProcessing(const Directive& directive, const PreprocessorPosition& pos);
};

} // namespace Preprocessor

#endif // DIRECTIVE_HPP