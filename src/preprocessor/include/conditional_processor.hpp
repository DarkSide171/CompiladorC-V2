#ifndef CONDITIONAL_PROCESSOR_HPP
#define CONDITIONAL_PROCESSOR_HPP

#include <string>
#include <stack>
#include <vector>
#include <memory>
#include "directive.hpp"
#include "preprocessor_logger.hpp"
#include "expression_evaluator.hpp"
#include "macro_processor.hpp"

namespace Preprocessor {

/**
 * @brief Tipos de condicionais suportados
 */
enum class ConditionalType {
    IF,      ///< Condicional #if
    IFDEF,   ///< Condicional #ifdef
    IFNDEF,  ///< Condicional #ifndef
    ELSE,    ///< Alternativa #else
    ELIF     ///< Alternativa condicional #elif
};

/**
 * @brief Estados de avaliação condicional
 */
enum class ConditionalState {
    CONDITIONAL_TRUE,   ///< Condição verdadeira - processar bloco
    CONDITIONAL_FALSE,  ///< Condição falsa - pular bloco
    CONDITIONAL_SKIP    ///< Pular bloco (já processou alternativa verdadeira)
};

/**
 * @brief Contexto de processamento condicional
 * 
 * Representa o estado de um bloco condicional na pilha de contextos.
 */
struct ConditionalContext {
    ConditionalType type;           ///< Tipo da condicional
    ConditionalState state;         ///< Estado atual da avaliação
    std::string condition;          ///< Condição original (para #if/#elif)
    PreprocessorPosition position;  ///< Posição no código fonte
    bool has_else;                  ///< Se já processou #else
    bool has_true_branch;           ///< Se já teve uma condição verdadeira
    int nesting_level;              ///< Nível de aninhamento
    
    /**
     * @brief Construtor padrão
     */
    ConditionalContext()
        : type(ConditionalType::IF), state(ConditionalState::CONDITIONAL_FALSE),
          condition(""), position(), has_else(false), has_true_branch(false),
          nesting_level(0) {}
    
    /**
     * @brief Construtor com parâmetros
     */
    ConditionalContext(ConditionalType t, ConditionalState s, const std::string& cond,
                      const PreprocessorPosition& pos, int level = 0)
        : type(t), state(s), condition(cond), position(pos),
          has_else(false), has_true_branch(s == ConditionalState::CONDITIONAL_TRUE),
          nesting_level(level) {}
};

/**
 * @brief Estatísticas de processamento condicional
 */
struct ConditionalStats {
    size_t total_conditionals;      ///< Total de condicionais processadas
    size_t true_branches;           ///< Branches verdadeiras executadas
    size_t false_branches;          ///< Branches falsas puladas
    size_t max_nesting_level;       ///< Nível máximo de aninhamento
    size_t evaluation_errors;       ///< Erros de avaliação
    size_t structure_errors;        ///< Erros de estrutura
    
    ConditionalStats()
        : total_conditionals(0), true_branches(0), false_branches(0),
          max_nesting_level(0), evaluation_errors(0), structure_errors(0) {}
};

/**
 * @brief Processador de compilação condicional
 * 
 * Responsável por processar diretivas condicionais (#if, #ifdef, #ifndef, #else, #elif, #endif)
 * e gerenciar a pilha de contextos condicionais para suporte a aninhamento.
 */
class ConditionalProcessor {
private:
    std::stack<ConditionalContext> context_stack_;  ///< Pilha de contextos condicionais
    PreprocessorLogger* logger_;                    ///< Logger para mensagens (não possui)
    std::unique_ptr<ExpressionEvaluator> evaluator_; ///< Avaliador de expressões
    MacroProcessor* macro_processor_;               ///< Processador de macros (não possui)
    ConditionalStats statistics_;                   ///< Estatísticas de uso
    bool optimization_enabled_;                     ///< Flag de otimização
    void* external_error_handler_;                  ///< ErrorHandler externo
    
public:
    /**
     * @brief Construtor
     * @param logger Logger para mensagens
     * @param macro_proc Processador de macros
     */
    ConditionalProcessor(PreprocessorLogger* logger, MacroProcessor* macro_proc);
    
    /**
     * @brief Destrutor
     */
    ~ConditionalProcessor() = default;
    
    // ========================================================================
    // MÉTODOS DE PILHA DE CONTEXTOS
    // ========================================================================
    
    /**
     * @brief Adiciona contexto condicional à pilha
     * @param condition Resultado da avaliação da condição
     * @param type Tipo da condicional
     * @param condition_str String da condição original
     * @param pos Posição no código fonte
     * @return true se sucesso, false se erro
     */
    bool pushConditionalContext(bool condition, ConditionalType type,
                               const std::string& condition_str,
                               const PreprocessorPosition& pos);
    
    /**
     * @brief Remove contexto condicional da pilha
     * @return true se sucesso, false se pilha vazia
     */
    bool popConditionalContext();
    
    /**
     * @brief Obtém contexto condicional atual
     * @return Contexto atual ou nullptr se pilha vazia
     */
    ConditionalContext* getCurrentContext();
    
    /**
     * @brief Obtém contexto condicional atual (const)
     * @return Contexto atual ou nullptr se pilha vazia
     */
    const ConditionalContext* getCurrentContext() const;
    
    // ========================================================================
    // MÉTODOS DE AVALIAÇÃO DE CONDIÇÕES
    // ========================================================================
    
    /**
     * @brief Avalia condição de diretiva condicional
     * @param condition String da condição a avaliar
     * @param pos Posição no código fonte
     * @return Resultado da avaliação
     */
    bool evaluateCondition(const std::string& condition, const PreprocessorPosition& pos);
    
    // ========================================================================
    // MÉTODOS DE VALIDAÇÃO DE ESTRUTURA
    // ========================================================================
    
    /**
     * @brief Valida estrutura de condicionais aninhadas
     * @return true se estrutura válida, false caso contrário
     */
    bool validateConditionalStructure() const;
    
    /**
     * @brief Manipula condicionais aninhadas
     * @param directive Diretiva sendo processada
     * @return true se sucesso, false se erro
     */
    bool handleNestedConditionals(const Directive& directive);
    
    // ========================================================================
    // MÉTODOS DE PROCESSAMENTO DE DIRETIVAS
    // ========================================================================
    
    /**
     * @brief Processa diretiva #if
     * @param expression Expressão a avaliar
     * @param pos Posição no código fonte
     * @return true se sucesso, false se erro
     */
    bool processIfDirective(const std::string& expression, const PreprocessorPosition& pos);
    
    /**
     * @brief Processa diretiva #ifdef
     * @param macro_name Nome da macro a verificar
     * @param pos Posição no código fonte
     * @return true se sucesso, false se erro
     */
    bool processIfdefDirective(const std::string& macro_name, const PreprocessorPosition& pos);
    
    /**
     * @brief Processa diretiva #ifndef
     * @param macro_name Nome da macro a verificar
     * @param pos Posição no código fonte
     * @return true se sucesso, false se erro
     */
    bool processIfndefDirective(const std::string& macro_name, const PreprocessorPosition& pos);
    
    /**
     * @brief Processa diretiva #else
     * @param pos Posição no código fonte
     * @return true se sucesso, false se erro
     */
    bool processElseDirective(const PreprocessorPosition& pos);
    
    /**
     * @brief Processa diretiva #elif
     * @param expression Expressão a avaliar
     * @param pos Posição no código fonte
     * @return true se sucesso, false se erro
     */
    bool processElifDirective(const std::string& expression, const PreprocessorPosition& pos);
    
    /**
     * @brief Processa diretiva #endif
     * @param pos Posição no código fonte
     * @return true se sucesso, false se erro
     */
    bool processEndifDirective(const PreprocessorPosition& pos);
    
    // ========================================================================
    // MÉTODOS DE CONTROLE DE PROCESSAMENTO
    // ========================================================================
    
    /**
     * @brief Verifica se condição atual é verdadeira
     * @return true se deve processar código, false caso contrário
     */
    bool isConditionTrue() const;
    
    /**
     * @brief Verifica se deve processar bloco atual
     * @return true se deve processar, false se deve pular
     */
    bool shouldProcessBlock() const;
    
    // ========================================================================
    // MÉTODOS DE TRATAMENTO DE ERROS
    // ========================================================================
    
    /**
     * @brief Manipula erros de processamento condicional
     * @param error_msg Mensagem de erro
     * @param pos Posição do erro
     * @return false (sempre retorna false para indicar erro)
     */
    bool handleConditionalErrors(const std::string& error_msg, const PreprocessorPosition& pos);
    
    // ========================================================================
    // MÉTODOS DE OTIMIZAÇÃO
    // ========================================================================
    
    /**
     * @brief Otimiza avaliação de condicionais
     * @param enable Flag para habilitar/desabilitar otimização
     */
    void optimizeConditionalEvaluation(bool enable = true);
    
    // ========================================================================
    // MÉTODOS UTILITÁRIOS
    // ========================================================================
    
    /**
     * @brief Obtém nível atual de aninhamento
     * @return Nível de aninhamento (0 = sem condicionais)
     */
    int getCurrentNestingLevel() const;
    
    /**
     * @brief Verifica se há condicionais não fechadas
     * @return true se há condicionais abertas, false caso contrário
     */
    bool hasOpenConditionals() const;
    
    /**
     * @brief Reinicializa estado do processador
     */
    void reset();
    
    /**
     * @brief Obtém estatísticas de uso
     * @return Estrutura com estatísticas
     */
    ConditionalStats getStatistics() const;
    
    /**
     * @brief Gera relatório de estado atual
     * @return String com relatório detalhado
     */
    std::string generateStatusReport() const;
    
    /**
     * @brief Define o manipulador de erros externo
     * @param errorHandler Ponteiro para o manipulador de erros
     */
    void setErrorHandler(void* errorHandler);
};

// ============================================================================
// FUNÇÕES UTILITÁRIAS
// ============================================================================

/**
 * @brief Converte tipo condicional para string
 * @param type Tipo a converter
 * @return String representando o tipo
 */
std::string conditionalTypeToString(ConditionalType type);

/**
 * @brief Converte estado condicional para string
 * @param state Estado a converter
 * @return String representando o estado
 */
std::string conditionalStateToString(ConditionalState state);

/**
 * @brief Verifica se tipo de diretiva é condicional
 * @param type Tipo da diretiva
 * @return true se é condicional, false caso contrário
 */
bool isConditionalDirectiveType(DirectiveType type);

} // namespace Preprocessor

#endif // CONDITIONAL_PROCESSOR_HPP