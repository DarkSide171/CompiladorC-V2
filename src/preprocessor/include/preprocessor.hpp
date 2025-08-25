#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include <string>
#include <vector>
#include <memory>
#include "preprocessor_types.hpp"
#include "preprocessor_config.hpp"
#include "preprocessor_state.hpp"
#include "preprocessor_logger.hpp"
#include "directive.hpp"
#include "macro_processor.hpp"
#include "file_manager.hpp"
#include "conditional_processor.hpp"
#include "expression_evaluator.hpp"

namespace Preprocessor {

/**
 * @brief Classe principal do pré-processador C
 * 
 * Responsável pela coordenação geral do processamento, leitura e escrita de arquivos,
 * integração com componentes especializados e controle de fluxo de processamento.
 */
class PreprocessorMain {
public:
    /**
     * @brief Construtor com arquivo de configuração
     * @param config_file Caminho para arquivo de configuração
     */
    explicit PreprocessorMain(const std::string& config_file = "");
    
    /**
     * @brief Destrutor que libera recursos
     */
    ~PreprocessorMain();
    
    // Métodos principais de processamento
    
    /**
     * @brief Processa arquivo principal
     * @param filename Caminho do arquivo a ser processado
     * @return true se processamento foi bem-sucedido
     */
    bool process(const std::string& filename);
    
    /**
     * @brief Processa string de código
     * @param content Conteúdo do código a ser processado
     * @return true se processamento foi bem-sucedido
     */
    bool processString(const std::string& content);
    
    /**
     * @brief Retorna código expandido
     * @return String com código expandido
     */
    std::string getExpandedCode() const;
    
    /**
     * @brief Lista de arquivos incluídos
     * @return Vetor com caminhos dos arquivos incluídos
     */
    std::vector<std::string> getDependencies() const;
    
    /**
     * @brief Reinicializa estado do preprocessor
     */
    void reset();
    
    // Métodos de configuração
    
    /**
     * @brief Define caminhos de busca
     * @param paths Vetor com caminhos de busca para includes
     */
    void setSearchPaths(const std::vector<std::string>& paths);
    
    /**
     * @brief Define macro programaticamente
     * @param name Nome da macro
     * @param value Valor da macro
     */
    void defineMacro(const std::string& name, const std::string& value);
    
    /**
     * @brief Remove definição de macro
     * @param name Nome da macro a ser removida
     */
    void undefineMacro(const std::string& name);
    
    /**
     * @brief Verifica se macro está definida
     * @param name Nome da macro
     * @return true se macro está definida
     */
    bool isMacroDefined(const std::string& name) const;
    
    /**
     * @brief Configura versão do padrão C
     * @param version Versão do padrão C
     */
    void setVersion(CVersion version);
    
    /**
     * @brief Retorna estatísticas de processamento
     * @return Estrutura com estatísticas
     */
    PreprocessorState getStatistics() const;

private:
    // Métodos internos de controle
    
    /**
     * @brief Inicializa componentes internos
     * @return true se inicialização foi bem-sucedida
     */
    bool initializeComponents();
    
    /**
     * @brief Processa arquivo específico
     * @param filepath Caminho do arquivo
     * @return true se processamento foi bem-sucedido
     */
    bool processFile(const std::string& filepath);
    
    /**
     * @brief Processa linha individual
     * @param line Conteúdo da linha
     * @param line_number Número da linha
     * @return true se processamento foi bem-sucedido
     */
    bool processLine(const std::string& line, int line_number);
    
    /**
     * @brief Manipula diretiva específica
     * @param directive Diretiva a ser processada
     * @return true se processamento foi bem-sucedido
     */
    bool handleDirective(const Directive& directive);
    
    /**
     * @brief Escreve no buffer de saída
     * @param content Conteúdo a ser escrito
     */
    void writeOutput(const std::string& content);
    
    /**
     * @brief Atualiza mapeamento de posições
     * @param original Posição original
     * @param expanded Posição expandida
     */
    void updatePositionMapping(const PreprocessorPosition& original, const PreprocessorPosition& expanded);
    
    /**
     * @brief Manipula erros de processamento
     * @param error_msg Mensagem de erro
     * @param pos Posição do erro
     */
    void handleErrors(const std::string& error_msg, const PreprocessorPosition& pos);
    
    /**
     * @brief Reporta erro com posição
     * @param error_msg Mensagem de erro
     * @param pos Posição do erro
     */
    void reportError(const std::string& error_msg, const PreprocessorPosition& pos);
    
    /**
     * @brief Tenta recuperação de erro
     * @param error_msg Mensagem de erro
     * @param pos Posição do erro
     */
    void attemptErrorRecovery(const std::string& error_msg, const PreprocessorPosition& pos);
    
    /**
     * @brief Limpeza e reset de recursos
     */
    void cleanup();
    
    /**
     * @brief Valida entrada antes do processamento
     * @param input Entrada a ser validada
     * @return true se entrada é válida
     */
    bool validateInput(const std::string& input);
    
    /**
     * @brief Valida saída após processamento
     * @param output Saída a ser validada
     * @return true se saída é válida
     */
    bool validateOutput(const std::string& output);
    
    /**
     * @brief Otimiza processamento
     */
    void optimizeProcessing();
    
    /**
     * @brief Habilita otimizações específicas
     */
    void enableOptimizations();

private:
    // Método auxiliar para parsear diretivas
    Directive parseDirective(const std::string& line, const PreprocessorPosition& pos);
    
    /**
     * @brief Sincroniza todos os processadores
     * @return true se sincronização foi bem-sucedida
     */
    bool synchronizeProcessors();
    
    /**
     * @brief Realiza verificação de integridade final
     * @return true se verificação passou
     */
    bool performIntegrityCheck();
    
    /**
     * @brief Gera relatório de processamento
     * @return string com estatísticas de processamento
     */
    std::string generateProcessingReport();
    
    /**
     * @brief Coordena comunicação entre módulos
     * @return true se coordenação foi bem-sucedida
     */
    bool coordinateModules();
    
    /**
     * @brief Propaga estado entre componentes
     * @param state_info Informações de estado a propagar
     */
    void propagateState(const std::string& state_info);
    
    /**
     * @brief Valida consistência entre módulos
     * @return true se todos os módulos estão consistentes
     */
    bool validateModuleConsistency();
    
    /**
     * @brief Notifica mudanças de estado para componentes
     * @param component Nome do componente
     * @param event Evento ocorrido
     */
    void notifyStateChange(const std::string& component, const std::string& event);
    
    // Métodos de sincronização entre processadores
    bool synchronizeAllProcessors();
    void synchronizeMacroWithConditional();
    void synchronizeFileWithState();
    void synchronizeExpressionWithMacro();
    bool checkProcessorConsistency();
    void resolveProcessorConflicts();
    
    // Componentes do preprocessor
    std::unique_ptr<PreprocessorConfig> config_;
    std::unique_ptr<PreprocessorState> state_;
    std::unique_ptr<PreprocessorLogger> logger_;
    std::unique_ptr<MacroProcessor> macro_processor_;
    std::unique_ptr<FileManager> file_manager_;
    std::unique_ptr<ConditionalProcessor> conditional_processor_;
    std::unique_ptr<ExpressionEvaluator> expression_evaluator_;
    
    // Buffers e estado interno
    std::string expanded_code_;
    std::vector<std::string> dependencies_;
    std::vector<std::pair<PreprocessorPosition, PreprocessorPosition>> position_mappings_;
    
    // Estado de processamento
    bool initialized_;
    bool processing_active_;
    std::string current_file_;
    int current_line_;
    
    // Estatísticas
    PreprocessorState stats_;
};

} // namespace Preprocessor

#endif // PREPROCESSOR_HPP