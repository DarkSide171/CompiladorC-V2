#ifndef INTEGRATED_ERROR_HANDLER_HPP
#define INTEGRATED_ERROR_HANDLER_HPP

/**
 * @file integrated_error_handler.hpp
 * @brief Sistema integrado de tratamento de erros para Lexer e Preprocessor
 * 
 * Esta classe unifica o tratamento de erros entre os componentes do lexer
 * e preprocessor, fornecendo uma interface consistente e coordenada.
 */

#include "lexer/include/error_handler.hpp"
#include "preprocessor/include/preprocessor.hpp"
#include <memory>
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>

namespace Integration {

/**
 * @enum IntegratedErrorType
 * @brief Tipos de erro integrados que podem ocorrer durante o processamento
 */
enum class IntegratedErrorType {
    LEXER_ERROR,           ///< Erro originado no lexer
    PREPROCESSOR_ERROR,    ///< Erro originado no preprocessor
    INTEGRATION_ERROR,     ///< Erro na integração entre componentes
    BRIDGE_ERROR          ///< Erro específico do bridge
};

/**
 * @struct IntegratedError
 * @brief Estrutura que representa um erro integrado
 */
struct IntegratedError {
    IntegratedErrorType type;           ///< Tipo do erro integrado
    std::string message;                ///< Mensagem do erro
    std::string filename;               ///< Arquivo onde ocorreu o erro
    size_t line;                        ///< Linha do erro
    size_t column;                      ///< Coluna do erro
    std::string component;              ///< Componente que gerou o erro
    std::string context;                ///< Contexto adicional
    bool is_warning;                    ///< Se é um warning ou erro
    
    // Dados específicos do lexer (se aplicável)
    std::unique_ptr<Lexer::CompilerError> lexer_error;
    
    IntegratedError(IntegratedErrorType type, const std::string& message,
                   const std::string& filename, size_t line, size_t column,
                   const std::string& component, bool is_warning = false)
        : type(type), message(message), filename(filename), line(line),
          column(column), component(component), context(""), is_warning(is_warning) {}
};

/**
 * @class IntegratedErrorHandler
 * @brief Classe que gerencia erros de forma integrada entre lexer e preprocessor
 */
class IntegratedErrorHandler {
private:
    std::unique_ptr<Lexer::ErrorHandler> lexer_error_handler_;  ///< Handler do lexer
    std::vector<IntegratedError> integrated_errors_;            ///< Lista de erros integrados
    std::vector<IntegratedError> integrated_warnings_;          ///< Lista de warnings integrados
    
    // Callbacks para notificação de erros
    std::function<void(const IntegratedError&)> on_error_callback_;
    std::function<void(const IntegratedError&)> on_warning_callback_;
    
    // Configurações
    bool verbose_mode_;
    int max_errors_;
    std::string current_context_;
    
    // Estatísticas
    std::unordered_map<std::string, size_t> error_counts_by_component_;
    
public:
    /**
     * @brief Construtor
     * @param max_errors Número máximo de erros antes de parar
     */
    explicit IntegratedErrorHandler(int max_errors = 100);
    
    /**
     * @brief Destrutor
     */
    ~IntegratedErrorHandler();
    
    // ========================================================================
    // Métodos de Configuração
    // ========================================================================
    
    /**
     * @brief Configura o handler do lexer
     * @param lexer_handler Ponteiro para o ErrorHandler do lexer
     */
    void setLexerErrorHandler(std::unique_ptr<Lexer::ErrorHandler> lexer_handler);
    
    /**
     * @brief Obtém o handler do lexer
     * @return Ponteiro para o ErrorHandler do lexer
     */
    Lexer::ErrorHandler* getLexerErrorHandler() const;
    
    /**
     * @brief Define callback para notificação de erros
     * @param callback Função a ser chamada quando um erro ocorrer
     */
    void setOnErrorCallback(std::function<void(const IntegratedError&)> callback);
    
    /**
     * @brief Define callback para notificação de warnings
     * @param callback Função a ser chamada quando um warning ocorrer
     */
    void setOnWarningCallback(std::function<void(const IntegratedError&)> callback);
    
    /**
     * @brief Define o contexto atual de processamento
     * @param context Contexto atual (ex: nome do arquivo sendo processado)
     */
    void setCurrentContext(const std::string& context);
    
    /**
     * @brief Habilita/desabilita modo verbose
     * @param verbose Se deve exibir erros imediatamente
     */
    void setVerboseMode(bool verbose);
    
    // ========================================================================
    // Métodos de Relatório de Erros
    // ========================================================================
    
    /**
     * @brief Reporta um erro do preprocessor
     * @param message Mensagem do erro
     * @param filename Nome do arquivo
     * @param line Linha do erro
     * @param column Coluna do erro
     */
    void reportPreprocessorError(const std::string& message, const std::string& filename,
                                size_t line, size_t column);
    
    /**
     * @brief Reporta um warning do preprocessor
     * @param message Mensagem do warning
     * @param filename Nome do arquivo
     * @param line Linha do warning
     * @param column Coluna do warning
     */
    void reportPreprocessorWarning(const std::string& message, const std::string& filename,
                                  size_t line, size_t column);
    
    /**
     * @brief Reporta um erro de integração
     * @param message Mensagem do erro
     * @param component Componente que gerou o erro
     */
    void reportIntegrationError(const std::string& message, const std::string& component);
    
    /**
     * @brief Integra um erro do lexer ao sistema
     * @param lexer_error Erro do lexer
     */
    void integrateFromLexer(const Lexer::CompilerError& lexer_error);
    
    // ========================================================================
    // Métodos de Consulta
    // ========================================================================
    
    /**
     * @brief Verifica se há erros
     * @return true se há erros
     */
    bool hasErrors() const;
    
    /**
     * @brief Verifica se há warnings
     * @return true se há warnings
     */
    bool hasWarnings() const;
    
    /**
     * @brief Obtém o número total de erros
     * @return Número de erros
     */
    size_t getErrorCount() const;
    
    /**
     * @brief Obtém o número total de warnings
     * @return Número de warnings
     */
    size_t getWarningCount() const;
    
    /**
     * @brief Obtém todos os erros integrados
     * @return Vetor com todos os erros
     */
    const std::vector<IntegratedError>& getErrors() const;
    
    /**
     * @brief Obtém todos os warnings integrados
     * @return Vetor com todos os warnings
     */
    const std::vector<IntegratedError>& getWarnings() const;
    
    /**
     * @brief Obtém estatísticas de erros por componente
     * @return Mapa com contagem de erros por componente
     */
    const std::unordered_map<std::string, size_t>& getErrorStatistics() const;
    
    // ========================================================================
    // Métodos de Formatação e Exibição
    // ========================================================================
    
    /**
     * @brief Formata um erro integrado para exibição
     * @param error Erro a ser formatado
     * @return String formatada
     */
    std::string formatError(const IntegratedError& error) const;
    
    /**
     * @brief Exibe resumo de todos os erros
     */
    void printErrorSummary() const;
    
    /**
     * @brief Exibe relatório detalhado de erros
     */
    void printDetailedReport() const;
    
    // ========================================================================
    // Métodos de Controle
    // ========================================================================
    
    /**
     * @brief Limpa todos os erros e warnings
     */
    void clear();
    
    /**
     * @brief Verifica se deve parar o processamento devido aos erros
     * @return true se deve parar
     */
    bool shouldStop() const;
    
    /**
     * @brief Cria um callback compatível com external_error_handler do preprocessor
     * @return Ponteiro para função de callback
     */
    void* createPreprocessorCallback();
    
private:
    /**
     * @brief Adiciona um erro à lista
     * @param error Erro a ser adicionado
     */
    void addError(const IntegratedError& error);
    
    /**
     * @brief Adiciona um warning à lista
     * @param warning Warning a ser adicionado
     */
    void addWarning(const IntegratedError& warning);
    
    /**
     * @brief Atualiza estatísticas de erro
     * @param component Componente que gerou o erro
     */
    void updateStatistics(const std::string& component);
};

} // namespace Integration

#endif // INTEGRATED_ERROR_HANDLER_HPP