#ifndef PREPROCESSOR_STATE_HPP
#define PREPROCESSOR_STATE_HPP

#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <memory>

namespace Preprocessor {

/**
 * @brief Enumeração dos estados de processamento do pré-processador
 * 
 * Define os diferentes estados que o pré-processador pode assumir durante
 * o processamento de arquivos, permitindo controle de fluxo e contexto.
 */
enum class ProcessingState {
    IDLE,                    ///< Estado inativo, aguardando processamento
    PROCESSING_FILE,         ///< Processando arquivo principal
    PROCESSING_INCLUDE,      ///< Processando arquivo incluído
    PROCESSING_MACRO,        ///< Expandindo macro
    PROCESSING_CONDITIONAL,  ///< Processando bloco condicional
    SKIPPING_CONDITIONAL,    ///< Pulando bloco condicional
    PROCESSING_DIRECTIVE,    ///< Processando diretiva
    ERROR_STATE,            ///< Estado de erro
    FINISHED                ///< Processamento concluído
};

/**
 * @brief Enumeração dos modos de processamento
 * 
 * Define diferentes modos de operação do pré-processador.
 */
enum class ProcessingMode {
    NORMAL,          ///< Modo normal de processamento
    MACRO_EXPANSION, ///< Modo de expansão de macros
    CONDITIONAL,     ///< Modo de processamento condicional
    INCLUDE,         ///< Modo de inclusão de arquivos
    DIRECTIVE        ///< Modo de processamento de diretivas
};

/**
 * @brief Estrutura para representar contexto de arquivo
 * 
 * Mantém informações sobre o arquivo sendo processado,
 * incluindo posição e estado de processamento.
 */
struct FileContext {
    std::string filename;        ///< Nome do arquivo
    size_t line;                ///< Linha atual no arquivo
    size_t column;              ///< Coluna atual no arquivo
    size_t absolute_position;   ///< Posição absoluta no arquivo
    ProcessingState state;      ///< Estado de processamento do arquivo
    
    /**
     * @brief Construtor padrão
     */
    FileContext() 
        : filename(""), line(1), column(1), absolute_position(0), 
          state(ProcessingState::IDLE) {}
    
    /**
     * @brief Construtor com parâmetros
     * @param file Nome do arquivo
     * @param l Linha inicial
     * @param c Coluna inicial
     */
    FileContext(const std::string& file, size_t l = 1, size_t c = 1)
        : filename(file), line(l), column(c), absolute_position(0),
          state(ProcessingState::PROCESSING_FILE) {}
    
    /**
     * @brief Operador de igualdade
     * @param other Outro contexto para comparação
     * @return true se iguais, false caso contrário
     */
    bool operator==(const FileContext& other) const {
        return filename == other.filename && line == other.line && 
               column == other.column && absolute_position == other.absolute_position;
    }
};

/**
 * @brief Estrutura para estatísticas de estado
 * 
 * Coleta informações estatísticas sobre o uso do sistema de estado.
 */
struct StateStats {
    size_t state_transitions;     ///< Número de transições de estado
    size_t max_stack_depth;      ///< Profundidade máxima da pilha
    size_t files_processed;      ///< Número de arquivos processados
    size_t conditional_blocks;   ///< Número de blocos condicionais
    size_t macro_expansions;     ///< Número de expansões de macro
    size_t include_depth;        ///< Profundidade máxima de includes
    
    /**
     * @brief Construtor padrão
     */
    StateStats() 
        : state_transitions(0), max_stack_depth(0), files_processed(0),
          conditional_blocks(0), macro_expansions(0), include_depth(0) {}
};

/**
 * @brief Classe principal para gerenciamento de estado do pré-processador
 * 
 * Gerencia o estado de processamento, contexto de arquivos, posição atual
 * e pilha de estados para controle de fluxo complexo.
 */
class PreprocessorState {
private:
    std::stack<ProcessingState> state_stack_;        ///< Pilha de estados
    std::stack<FileContext> file_context_stack_;     ///< Pilha de contextos de arquivo
    FileContext current_file_context_;               ///< Contexto de arquivo atual
    ProcessingMode processing_mode_;                 ///< Modo de processamento atual
    bool in_conditional_block_;                      ///< Flag de bloco condicional
    bool has_error_;                                ///< Flag de estado de erro
    std::unordered_set<std::string> processed_files_; ///< Arquivos já processados
    StateStats statistics_;                          ///< Estatísticas de uso
    
public:
    /**
     * @brief Construtor padrão
     * Inicializa com estado IDLE
     */
    PreprocessorState();
    
    /**
     * @brief Destrutor
     */
    ~PreprocessorState() = default;
    
    // ========================================================================
    // MÉTODOS DE PILHA DE ESTADOS
    // ========================================================================
    
    /**
     * @brief Empilha novo estado
     * @param state Estado a ser empilhado
     */
    void pushState(ProcessingState state);
    
    /**
     * @brief Desempilha estado atual
     * @return Estado que foi desempilhado
     * @throws std::runtime_error se pilha estiver vazia
     */
    ProcessingState popState();
    
    /**
     * @brief Obtém estado atual sem remover da pilha
     * @return Estado atual
     */
    ProcessingState getCurrentState() const;
    
    /**
     * @brief Verifica se pilha de estados está vazia
     * @return true se vazia, false caso contrário
     */
    bool isEmpty() const;
    
    /**
     * @brief Obtém profundidade da pilha de estados
     * @return Número de estados na pilha
     */
    size_t getDepth() const;
    
    // ========================================================================
    // MÉTODOS DE CONTEXTO DE ARQUIVO
    // ========================================================================
    
    /**
     * @brief Define contexto de arquivo atual
     * @param context Novo contexto de arquivo
     */
    void setFileContext(const FileContext& context);
    
    /**
     * @brief Obtém contexto de arquivo atual
     * @return Contexto de arquivo atual
     */
    FileContext getFileContext() const;
    
    /**
     * @brief Empilha contexto de arquivo (para includes)
     * @param filename Nome do arquivo
     * @param line Linha inicial
     * @param column Coluna inicial
     */
    void pushFileContext(const std::string& filename, size_t line = 1, size_t column = 1);
    
    /**
     * @brief Desempilha contexto de arquivo
     * @throws std::runtime_error se pilha estiver vazia
     */
    void popFileContext();
    
    // ========================================================================
    // MÉTODOS DE POSIÇÃO
    // ========================================================================
    
    /**
     * @brief Define linha atual
     * @param line Nova linha
     */
    void setCurrentLine(size_t line);
    
    /**
     * @brief Define coluna atual
     * @param column Nova coluna
     */
    void setCurrentColumn(size_t column);
    
    /**
     * @brief Obtém linha atual
     * @return Linha atual
     */
    size_t getCurrentLine() const;
    
    /**
     * @brief Obtém coluna atual
     * @return Coluna atual
     */
    size_t getCurrentColumn() const;
    
    /**
     * @brief Incrementa linha atual
     */
    void incrementLine();
    
    /**
     * @brief Incrementa coluna atual
     */
    void incrementColumn();
    
    /**
     * @brief Reseta coluna para início da linha
     */
    void resetColumn();
    
    // ========================================================================
    // MÉTODOS DE MODO DE PROCESSAMENTO
    // ========================================================================
    
    /**
     * @brief Define modo de processamento
     * @param mode Novo modo de processamento
     */
    void setProcessingMode(ProcessingMode mode);
    
    /**
     * @brief Obtém modo de processamento atual
     * @return Modo de processamento atual
     */
    ProcessingMode getProcessingMode() const;
    
    // ========================================================================
    // MÉTODOS DE BLOCOS CONDICIONAIS
    // ========================================================================
    
    /**
     * @brief Habilita processamento de bloco condicional
     */
    void enableConditionalBlock();
    
    /**
     * @brief Desabilita processamento de bloco condicional
     */
    void disableConditionalBlock();
    
    /**
     * @brief Verifica se está em bloco condicional
     * @return true se em bloco condicional, false caso contrário
     */
    bool isInConditionalBlock() const;
    
    // ========================================================================
    // MÉTODOS DE ESTADO DE ERRO
    // ========================================================================
    
    /**
     * @brief Define estado de erro
     * @param hasError Flag de erro
     */
    void setErrorState(bool hasError);
    
    /**
     * @brief Verifica se há erro
     * @return true se há erro, false caso contrário
     */
    bool hasError() const;
    
    // ========================================================================
    // MÉTODOS DE ARQUIVOS PROCESSADOS
    // ========================================================================
    
    /**
     * @brief Adiciona arquivo à lista de processados
     * @param filename Nome do arquivo
     */
    void addProcessedFile(const std::string& filename);
    
    /**
     * @brief Verifica se arquivo foi processado
     * @param filename Nome do arquivo
     * @return true se foi processado, false caso contrário
     */
    bool wasFileProcessed(const std::string& filename) const;
    
    /**
     * @brief Obtém lista de arquivos processados
     * @return Vetor com nomes dos arquivos processados
     */
    std::vector<std::string> getProcessedFiles() const;
    
    // ========================================================================
    // MÉTODOS UTILITÁRIOS
    // ========================================================================
    
    /**
     * @brief Reinicializa estado para valores padrão
     */
    void reset();
    
    /**
     * @brief Obtém estatísticas de uso
     * @return Estrutura com estatísticas
     */
    StateStats getStatistics() const;
    
    /**
     * @brief Valida consistência do estado atual
     * @return true se estado é consistente, false caso contrário
     */
    bool validateState() const;
    
    /**
     * @brief Gera relatório textual do estado atual
     * @return String com relatório detalhado
     */
    std::string generateStateReport() const;
};

// ============================================================================
// FUNÇÕES UTILITÁRIAS GLOBAIS
// ============================================================================

/**
 * @brief Converte ProcessingState para string
 * @param state Estado a ser convertido
 * @return Nome do estado como string
 */
std::string processingStateToString(ProcessingState state);

/**
 * @brief Converte ProcessingMode para string
 * @param mode Modo a ser convertido
 * @return Nome do modo como string
 */
std::string processingModeToString(ProcessingMode mode);

/**
 * @brief Verifica se transição de estado é válida
 * @param from Estado origem
 * @param to Estado destino
 * @return true se transição é válida, false caso contrário
 */
bool isValidStateTransition(ProcessingState from, ProcessingState to);

/**
 * @brief Verifica se estado requer contexto de arquivo
 * @param state Estado a ser verificado
 * @return true se requer contexto, false caso contrário
 */
bool requiresFileContext(ProcessingState state);

} // namespace Preprocessor

#endif // PREPROCESSOR_STATE_HPP