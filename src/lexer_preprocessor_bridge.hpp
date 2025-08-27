#ifndef LEXER_PREPROCESSOR_BRIDGE_HPP
#define LEXER_PREPROCESSOR_BRIDGE_HPP

/**
 * @file lexer_preprocessor_bridge.hpp
 * @brief Ponte de integração entre o Lexer e o Preprocessor
 * 
 * Esta classe implementa a fase 4.5 da especificação, fornecendo
 * uma interface unificada para integração completa entre os componentes.
 */

#include "lexer/include/lexer.hpp"
#include "lexer/include/token.hpp"
#include "preprocessor/include/preprocessor_lexer_interface.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <sstream>

namespace Integration {

/**
 * @struct IntegrationConfig
 * @brief Configuração para a integração lexer-preprocessor
 */
struct IntegrationConfig {
    bool enablePositionMapping;     ///< Habilita mapeamento de posições
    bool enableMacroTracking;       ///< Habilita rastreamento de macros
    bool enableErrorIntegration;    ///< Habilita integração de erros
    bool enableDebugMode;           ///< Habilita modo de debug
    std::string cStandard;          ///< Padrão C a ser usado ("c99", "c11", etc.)
    std::vector<std::string> includePaths; ///< Caminhos de busca para includes
    
    IntegrationConfig() 
        : enablePositionMapping(true)
        , enableMacroTracking(true)
        , enableErrorIntegration(true)
        , enableDebugMode(false)
        , cStandard("c99") {}
};

/**
 * @struct IntegratedToken
 * @brief Token com informações de mapeamento integradas
 */
struct IntegratedToken {
    Lexer::Token lexerToken;                    ///< Token original do lexer
    Preprocessor::SourceMapping sourceMapping;  ///< Mapeamento de posição
    bool isFromPreprocessor;                    ///< Se veio do preprocessor
    std::string originalText;                   ///< Texto original antes do preprocessamento
    
    IntegratedToken() : isFromPreprocessor(false) {}
    
    IntegratedToken(const Lexer::Token& token, const Preprocessor::SourceMapping& mapping)
        : lexerToken(token), sourceMapping(mapping), isFromPreprocessor(true) {
        originalText = token.getLexeme();
    }
};

/**
 * @class LexerPreprocessorBridge
 * @brief Classe principal da ponte de integração
 */
class LexerPreprocessorBridge {
private:
    std::unique_ptr<Preprocessor::PreprocessorLexerInterface> preprocessorInterface;
    std::unique_ptr<Lexer::LexerMain> lexer;
    std::unique_ptr<Lexer::ErrorHandler> errorHandler;
    
    IntegrationConfig config;
    Preprocessor::ProcessingResult lastProcessingResult;
    std::vector<IntegratedToken> tokenCache;
    size_t currentTokenIndex;
    bool isInitialized;
    bool hasProcessedInput;
    
    // Stream para armazenar código processado
    std::unique_ptr<std::istringstream> codeStream;
    
    // Callbacks para eventos de integração
    std::function<void(const std::string&, const Preprocessor::SourceMapping&)> onMacroExpanded;
    std::function<void(const std::string&)> onFileIncluded;
    std::function<void(const std::string&, size_t, size_t)> onIntegrationError;
    
    // Métodos auxiliares privados
    bool initializeComponents();
    void setupErrorIntegration();
    void buildIntegratedTokens();
    IntegratedToken createIntegratedToken(const Lexer::Token& token, size_t position);
    Preprocessor::SourceMapping findMappingForPosition(size_t line, size_t column) const;
    void validatePositionMappings();
    
public:
    /**
     * @brief Construtor
     * @param config Configuração da integração
     */
    explicit LexerPreprocessorBridge(const IntegrationConfig& config = IntegrationConfig());
    
    /**
     * @brief Destrutor
     */
    ~LexerPreprocessorBridge();
    
    // ========================================================================
    // MÉTODOS PRINCIPAIS DE INTEGRAÇÃO
    // ========================================================================
    
    /**
     * @brief Inicializa a ponte de integração
     * @return true se inicialização bem-sucedida
     */
    bool initialize();
    
    /**
     * @brief Processa arquivo através do pipeline completo
     * @param filename Nome do arquivo a ser processado
     * @return true se processamento bem-sucedido
     */
    bool processFile(const std::string& filename);
    
    /**
     * @brief Processa string através do pipeline completo
     * @param code Código a ser processado
     * @param filename Nome do arquivo (para referência)
     * @return true se processamento bem-sucedido
     */
    bool processString(const std::string& code, const std::string& filename = "<string>");
    
    /**
     * @brief Obtém próximo token integrado
     * @return Token com informações de mapeamento
     */
    IntegratedToken nextToken();
    
    /**
     * @brief Visualiza próximo token sem consumi-lo
     * @return Token que seria retornado por nextToken()
     */
    IntegratedToken peekToken();
    
    /**
     * @brief Verifica se há mais tokens disponíveis
     * @return true se há mais tokens
     */
    bool hasMoreTokens() const;
    
    /**
     * @brief Tokeniza todo o input de uma vez
     * @return Vetor com todos os tokens integrados
     */
    std::vector<IntegratedToken> tokenizeAll();
    
    // ========================================================================
    // MÉTODOS DE CONFIGURAÇÃO E CONTROLE
    // ========================================================================
    
    /**
     * @brief Define macro programaticamente
     * @param name Nome da macro
     * @param value Valor da macro
     */
    void defineMacro(const std::string& name, const std::string& value = "");
    
    /**
     * @brief Remove definição de macro
     * @param name Nome da macro
     */
    void undefineMacro(const std::string& name);
    
    /**
     * @brief Adiciona caminho de busca para includes
     * @param path Caminho a ser adicionado
     */
    void addIncludePath(const std::string& path);
    
    /**
     * @brief Define padrão C a ser usado
     * @param standard Padrão C ("c89", "c99", "c11", "c17", "c23")
     */
    void setStandard(const std::string& standard);
    
    /**
     * @brief Reinicializa a ponte
     */
    void reset();
    
    // ========================================================================
    // MÉTODOS DE MAPEAMENTO E VALIDAÇÃO
    // ========================================================================
    
    /**
     * @brief Mapeia posição no código processado para posição original
     * @param processedLine Linha no código processado
     * @param processedColumn Coluna no código processado
     * @param originalLine Linha original (saída)
     * @param originalColumn Coluna original (saída)
     * @param originalFile Arquivo original (saída)
     * @return true se mapeamento encontrado
     */
    bool mapToOriginalPosition(size_t processedLine, size_t processedColumn,
                              size_t& originalLine, size_t& originalColumn,
                              std::string& originalFile) const;
    
    /**
     * @brief Valida integridade dos mapeamentos de posição
     * @return true se mapeamentos são válidos
     */
    bool validatePositionMappings() const;
    
    /**
     * @brief Executa testes de integração end-to-end
     * @return true se todos os testes passaram
     */
    bool runIntegrationTests();
    
    // ========================================================================
    // MÉTODOS DE INFORMAÇÃO E ESTATÍSTICAS
    // ========================================================================
    
    /**
     * @brief Obtém resultado do último processamento
     * @return Resultado do processamento
     */
    const Preprocessor::ProcessingResult& getLastProcessingResult() const;
    
    /**
     * @brief Obtém lista de arquivos incluídos
     * @return Vetor com caminhos dos arquivos
     */
    std::vector<std::string> getIncludedFiles() const;
    
    /**
     * @brief Obtém lista de macros definidas
     * @return Vetor com nomes das macros
     */
    std::vector<std::string> getDefinedMacros() const;
    
    /**
     * @brief Obtém estatísticas de processamento
     * @return Mapa com estatísticas
     */
    std::unordered_map<std::string, size_t> getStatistics() const;
    
    /**
     * @brief Verifica se houve erros durante processamento
     * @return true se houve erros
     */
    bool hasErrors() const;
    
    /**
     * @brief Obtém mensagens de erro
     * @return Vetor com mensagens de erro
     */
    std::vector<std::string> getErrorMessages() const;
    
    /**
     * @brief Obtém mensagens de aviso
     * @return Vetor com mensagens de aviso
     */
    std::vector<std::string> getWarningMessages() const;
    
    // ========================================================================
    // MÉTODOS DE CALLBACK E EVENTOS
    // ========================================================================
    
    /**
     * @brief Define callback para expansão de macro
     * @param callback Função a ser chamada quando macro é expandida
     */
    void setOnMacroExpanded(std::function<void(const std::string&, const Preprocessor::SourceMapping&)> callback);
    
    /**
     * @brief Define callback para inclusão de arquivo
     * @param callback Função a ser chamada quando arquivo é incluído
     */
    void setOnFileIncluded(std::function<void(const std::string&)> callback);
    
    /**
     * @brief Define callback para erros de integração
     * @param callback Função a ser chamada quando erro ocorre
     */
    void setOnIntegrationError(std::function<void(const std::string&, size_t, size_t)> callback);
};

/**
 * @class IntegrationTester
 * @brief Classe para testes de integração
 */
class IntegrationTester {
public:
    /**
     * @brief Executa suite completa de testes de integração
     * @return true se todos os testes passaram
     */
    static bool runAllTests();
    
    /**
     * @brief Testa mapeamento de posições
     * @return true se teste passou
     */
    static bool testPositionMapping();
    
    /**
     * @brief Testa processamento de macros
     * @return true se teste passou
     */
    static bool testMacroProcessing();
    
    /**
     * @brief Testa inclusão de arquivos
     * @return true se teste passou
     */
    static bool testFileInclusion();
    
    /**
     * @brief Testa pipeline end-to-end
     * @return true se teste passou
     */
    static bool testEndToEndPipeline();
    
    /**
     * @brief Testa compatibilidade entre componentes
     * @return true se teste passou
     */
    static bool testCompatibility();
};

} // namespace Integration

#endif // LEXER_PREPROCESSOR_BRIDGE_HPP