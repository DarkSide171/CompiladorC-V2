#ifndef PREPROCESSOR_LEXER_INTERFACE_HPP
#define PREPROCESSOR_LEXER_INTERFACE_HPP

/**
 * @file preprocessor_lexer_interface.hpp
 * @brief Interface de integração entre o pré-processador e o analisador léxico
 * 
 * Esta interface permite que o pré-processador forneça código processado
 * diretamente ao lexer, mantendo informações de mapeamento de posições
 * e metadados necessários para análise e debug.
 */

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include "preprocessor.hpp"

// Forward declarations para evitar dependências circulares
namespace Lexer {
    class Token;
    struct Position;
    enum class TokenType;
}

namespace Preprocessor {

/**
 * @struct SourceMapping
 * @brief Mapeia posições no código processado para posições originais
 */
struct SourceMapping {
    size_t processedLine;      ///< Linha no código processado
    size_t processedColumn;    ///< Coluna no código processado
    size_t originalLine;       ///< Linha no arquivo original
    size_t originalColumn;     ///< Coluna no arquivo original
    std::string originalFile;  ///< Nome do arquivo original
    bool fromMacroExpansion;   ///< Se veio de expansão de macro
    std::string macroName;     ///< Nome da macro (se aplicável)
    
    SourceMapping() : processedLine(0), processedColumn(0), originalLine(0), 
                     originalColumn(0), fromMacroExpansion(false) {}
    
    SourceMapping(size_t pLine, size_t pCol, size_t oLine, size_t oCol, 
                 const std::string& file, bool fromMacro = false, 
                 const std::string& macro = "")
        : processedLine(pLine), processedColumn(pCol), originalLine(oLine),
          originalColumn(oCol), originalFile(file), fromMacroExpansion(fromMacro),
          macroName(macro) {}
};

/**
 * @struct ProcessedToken
 * @brief Token processado com informações de mapeamento
 */
struct ProcessedToken {
    std::string text;          ///< Texto do token
    SourceMapping mapping;     ///< Mapeamento de posição
    bool isFromDirective;      ///< Se veio de uma diretiva de pré-processamento
    std::string directiveType; ///< Tipo da diretiva (se aplicável)
    
    ProcessedToken() : isFromDirective(false) {}
    
    ProcessedToken(const std::string& tokenText, const SourceMapping& map,
                  bool fromDirective = false, const std::string& directive = "")
        : text(tokenText), mapping(map), isFromDirective(fromDirective),
          directiveType(directive) {}
};

/**
 * @struct ProcessingResult
 * @brief Resultado completo do pré-processamento
 */
struct ProcessingResult {
    std::string processedCode;                    ///< Código completamente processado
    std::vector<SourceMapping> positionMappings; ///< Mapeamentos de posição
    std::vector<std::string> includedFiles;      ///< Arquivos incluídos
    std::vector<std::string> definedMacros;      ///< Macros definidas
    std::unordered_map<std::string, std::string> macroDefinitions; ///< Definições de macros
    bool hasErrors;                               ///< Se houve erros durante processamento
    std::vector<std::string> errorMessages;      ///< Mensagens de erro
    std::vector<std::string> warningMessages;    ///< Mensagens de aviso
    
    ProcessingResult() : hasErrors(false) {}
    
    void addError(const std::string& message) {
        hasErrors = true;
        errorMessages.push_back(message);
    }
    
    void addWarning(const std::string& message) {
        warningMessages.push_back(message);
    }
    
    void clear() {
        processedCode.clear();
        positionMappings.clear();
        includedFiles.clear();
        definedMacros.clear();
        macroDefinitions.clear();
        hasErrors = false;
        errorMessages.clear();
        warningMessages.clear();
    }
};

/**
 * @class PositionMapper
 * @brief Classe para mapear posições entre código processado e original
 */
class PositionMapper {
private:
    std::vector<SourceMapping> mappings;
    std::unordered_map<size_t, size_t> lineToMappingIndex;
    
public:
    /**
     * @brief Adiciona um mapeamento de posição
     * @param mapping Mapeamento a ser adicionado
     */
    void addMapping(const SourceMapping& mapping);
    
    /**
     * @brief Encontra o mapeamento para uma posição processada
     * @param processedLine Linha no código processado
     * @param processedColumn Coluna no código processado
     * @return Mapeamento correspondente ou nullptr se não encontrado
     */
    const SourceMapping* findMapping(size_t processedLine, size_t processedColumn) const;
    
    /**
     * @brief Converte posição processada para posição original
     * @param processedLine Linha no código processado
     * @param processedColumn Coluna no código processado
     * @param originalLine Referência para linha original (saída)
     * @param originalColumn Referência para coluna original (saída)
     * @param originalFile Referência para arquivo original (saída)
     * @return true se mapeamento encontrado, false caso contrário
     */
    bool mapToOriginal(size_t processedLine, size_t processedColumn,
                      size_t& originalLine, size_t& originalColumn,
                      std::string& originalFile) const;
    
    /**
     * @brief Obtém informações sobre arquivo original
     * @param processedLine Linha no código processado
     * @return Nome do arquivo original
     */
    std::string getOriginalFile(size_t processedLine) const;
    
    /**
     * @brief Verifica se uma posição veio de expansão de macro
     * @param processedLine Linha no código processado
     * @return true se veio de macro, false caso contrário
     */
    bool isFromMacroExpansion(size_t processedLine) const;
    
    /**
     * @brief Obtém nome da macro que gerou uma posição
     * @param processedLine Linha no código processado
     * @return Nome da macro ou string vazia
     */
    std::string getMacroName(size_t processedLine) const;
    
    /**
     * @brief Limpa todos os mapeamentos
     */
    void clear();
    
    /**
     * @brief Obtém todos os mapeamentos
     * @return Vetor com todos os mapeamentos
     */
    const std::vector<SourceMapping>& getAllMappings() const { return mappings; }
};

/**
 * @class IntegratedErrorHandler
 * @brief Manipulador de erros integrado para pré-processador e lexer
 */
class IntegratedErrorHandler {
public:
    enum class ErrorSource {
        PREPROCESSOR,
        LEXER,
        INTEGRATION
    };
    
    struct IntegratedError {
        ErrorSource source;
        std::string message;
        size_t originalLine;
        size_t originalColumn;
        size_t processedLine;
        size_t processedColumn;
        std::string originalFile;
        std::string context;
        
        IntegratedError(ErrorSource src, const std::string& msg,
                       size_t origLine, size_t origCol, size_t procLine, size_t procCol,
                       const std::string& file, const std::string& ctx = "")
            : source(src), message(msg), originalLine(origLine), originalColumn(origCol),
              processedLine(procLine), processedColumn(procCol), originalFile(file), context(ctx) {}
    };
    
private:
    std::vector<IntegratedError> errors;
    std::vector<IntegratedError> warnings;
    PositionMapper* positionMapper;
    
public:
    /**
     * @brief Construtor
     * @param mapper Ponteiro para o mapeador de posições
     */
    explicit IntegratedErrorHandler(PositionMapper* mapper = nullptr)
        : positionMapper(mapper) {}
    
    /**
     * @brief Reporta um erro
     * @param source Fonte do erro
     * @param message Mensagem de erro
     * @param line Linha no código processado
     * @param column Coluna no código processado
     * @param context Contexto adicional
     */
    void reportError(ErrorSource source, const std::string& message,
                    size_t line, size_t column, const std::string& context = "");
    
    /**
     * @brief Reporta um aviso
     * @param source Fonte do aviso
     * @param message Mensagem de aviso
     * @param line Linha no código processado
     * @param column Coluna no código processado
     * @param context Contexto adicional
     */
    void reportWarning(ErrorSource source, const std::string& message,
                      size_t line, size_t column, const std::string& context = "");
    
    /**
     * @brief Verifica se há erros
     * @return true se há erros, false caso contrário
     */
    bool hasErrors() const { return !errors.empty(); }
    
    /**
     * @brief Verifica se há avisos
     * @return true se há avisos, false caso contrário
     */
    bool hasWarnings() const { return !warnings.empty(); }
    
    /**
     * @brief Obtém todos os erros
     * @return Vetor com todos os erros
     */
    const std::vector<IntegratedError>& getErrors() const { return errors; }
    
    /**
     * @brief Obtém todos os avisos
     * @return Vetor com todos os avisos
     */
    const std::vector<IntegratedError>& getWarnings() const { return warnings; }
    
    /**
     * @brief Limpa todos os erros e avisos
     */
    void clear();
    
    /**
     * @brief Define o mapeador de posições
     * @param mapper Ponteiro para o mapeador
     */
    void setPositionMapper(PositionMapper* mapper) { positionMapper = mapper; }
};

/**
 * @class PreprocessorLexerInterface
 * @brief Interface principal para integração pré-processador-lexer
 */
class PreprocessorLexerInterface {
private:
    std::unique_ptr<PreprocessorMain> preprocessor;
    std::unique_ptr<PositionMapper> positionMapper;
    std::unique_ptr<IntegratedErrorHandler> errorHandler;
    ProcessingResult lastResult;
    bool isInitialized;
    
    // Callbacks para eventos
    std::function<void(const std::string&)> onMacroExpanded;
    std::function<void(const std::string&)> onFileIncluded;
    std::function<void(const IntegratedErrorHandler::IntegratedError&)> onError;
    
    // Métodos privados auxiliares
    void buildPositionMappings(const std::string& filename, const std::string& processedCode);
    void collectMacroInformation();
    
public:
    /**
     * @brief Construtor
     */
    PreprocessorLexerInterface();
    
    /**
     * @brief Destrutor
     */
    ~PreprocessorLexerInterface();
    
    /**
     * @brief Inicializa a interface
     * @param config Configuração do pré-processador
     * @return true se inicialização bem-sucedida, false caso contrário
     */
    bool initialize(const PreprocessorConfig& config = PreprocessorConfig());
    
    /**
     * @brief Processa um arquivo e prepara para análise léxica
     * @param filename Nome do arquivo a ser processado
     * @return Resultado do processamento
     */
    ProcessingResult processFile(const std::string& filename);
    
    /**
     * @brief Processa uma string e prepara para análise léxica
     * @param code Código a ser processado
     * @param filename Nome do arquivo (para referência)
     * @return Resultado do processamento
     */
    ProcessingResult processString(const std::string& code, const std::string& filename = "<string>");
    
    /**
     * @brief Obtém o código processado pronto para o lexer
     * @return String com código processado
     */
    const std::string& getProcessedCode() const { return lastResult.processedCode; }
    
    /**
     * @brief Obtém o mapeador de posições
     * @return Referência para o mapeador de posições
     */
    const PositionMapper& getPositionMapper() const { return *positionMapper; }
    
    /**
     * @brief Obtém o manipulador de erros integrado
     * @return Referência para o manipulador de erros
     */
    const IntegratedErrorHandler& getErrorHandler() const { return *errorHandler; }
    
    /**
     * @brief Obtém o último resultado de processamento
     * @return Referência para o último resultado
     */
    const ProcessingResult& getLastResult() const { return lastResult; }
    
    /**
     * @brief Define macro
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
     * @brief Define versão do padrão C
     * @param version Versão do padrão ("c89", "c99", "c11", "c17", "c23")
     */
    void setStandard(const std::string& version);
    
    /**
     * @brief Reinicializa a interface
     */
    void reset();
    
    /**
     * @brief Verifica se a interface está inicializada
     * @return true se inicializada, false caso contrário
     */
    bool isReady() const { return isInitialized; }
    
    // Callbacks para eventos
    void setOnMacroExpanded(std::function<void(const std::string&)> callback) {
        onMacroExpanded = callback;
    }
    
    void setOnFileIncluded(std::function<void(const std::string&)> callback) {
        onFileIncluded = callback;
    }
    
    void setOnError(std::function<void(const IntegratedErrorHandler::IntegratedError&)> callback) {
        onError = callback;
    }
    
    /**
     * @brief Obtém estatísticas do processamento
     * @return Mapa com estatísticas
     */
    std::unordered_map<std::string, size_t> getStatistics() const;
    
    /**
     * @brief Obtém dependências do arquivo processado
     * @return Vetor com nomes dos arquivos dependentes
     */
    std::vector<std::string> getDependencies() const;
};

/**
 * @class CompilationPipeline
 * @brief Pipeline completo de compilação (pré-processador + lexer)
 */
class CompilationPipeline {
private:
    std::unique_ptr<PreprocessorLexerInterface> interface;
    std::string currentFile;
    bool pipelineReady;
    
public:
    /**
     * @brief Construtor
     */
    CompilationPipeline();
    
    /**
     * @brief Destrutor
     */
    ~CompilationPipeline();
    
    /**
     * @brief Inicializa o pipeline
     * @param config Configuração do pré-processador
     * @return true se inicialização bem-sucedida, false caso contrário
     */
    bool initialize(const PreprocessorConfig& config = PreprocessorConfig());
    
    /**
     * @brief Processa arquivo através do pipeline completo
     * @param filename Nome do arquivo
     * @return true se processamento bem-sucedido, false caso contrário
     */
    bool processFile(const std::string& filename);
    
    /**
     * @brief Processa string através do pipeline completo
     * @param code Código a ser processado
     * @param filename Nome do arquivo (para referência)
     * @return true se processamento bem-sucedido, false caso contrário
     */
    bool processString(const std::string& code, const std::string& filename = "<string>");
    
    /**
     * @brief Obtém o código processado pronto para tokenização
     * @return String com código processado
     */
    const std::string& getProcessedCode() const;
    
    /**
     * @brief Obtém a interface de integração
     * @return Referência para a interface
     */
    const PreprocessorLexerInterface& getInterface() const { return *interface; }
    
    /**
     * @brief Verifica se o pipeline está pronto
     * @return true se pronto, false caso contrário
     */
    bool isReady() const { return pipelineReady; }
    
    /**
     * @brief Reinicializa o pipeline
     */
    void reset();
};

} // namespace Preprocessor

#endif // PREPROCESSOR_LEXER_INTERFACE_HPP