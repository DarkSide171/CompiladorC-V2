#ifndef MACRO_PROCESSOR_HPP
#define MACRO_PROCESSOR_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "preprocessor_logger.hpp"
#include "preprocessor_state.hpp"

namespace Preprocessor {

// ============================================================================
// ESTRUTURAS DE DADOS
// ============================================================================

/**
 * @brief Tipos de macro suportados
 */
enum class MacroType {
    OBJECT_LIKE,    // #define NAME value
    FUNCTION_LIKE,  // #define NAME(params) body
    VARIADIC        // #define NAME(params, ...) body
};

/**
 * @brief Informações sobre uma macro definida
 */
struct MacroInfo {
    std::string name;                    // Nome da macro
    std::string value;                   // Valor/corpo da macro
    MacroType type;                      // Tipo da macro
    std::vector<std::string> parameters; // Parâmetros (para macros funcionais)
    bool isVariadic;                     // Se é uma macro variádica
    Preprocessor::PreprocessorPosition definedAt;      // Onde foi definida
    bool isPredefined;                   // Se é uma macro predefinida
    int expansionCount;                  // Contador de expansões
    
    // Construtores
    MacroInfo();
    MacroInfo(const std::string& name, const std::string& value, MacroType type = MacroType::OBJECT_LIKE);
    MacroInfo(const std::string& name, const std::string& value, 
              const std::vector<std::string>& params, bool variadic = false);
    
    // Operadores
    bool operator==(const MacroInfo& other) const;
    bool operator!=(const MacroInfo& other) const;
    
    // Métodos utilitários
    bool isFunctionLike() const;
    bool hasParameters() const;
    size_t getParameterCount() const;
    std::string toString() const;
};

/**
 * @brief Contexto de expansão de macro para detectar recursão
 */
struct MacroExpansionContext {
    std::unordered_set<std::string> expandingMacros; // Macros sendo expandidas
    int maxDepth;                                     // Profundidade máxima
    int currentDepth;                                 // Profundidade atual
    
    MacroExpansionContext(int maxDepth = 200);
    bool canExpand(const std::string& macroName) const;
    void pushMacro(const std::string& macroName);
    void popMacro(const std::string& macroName);
    void reset();
};

// ============================================================================
// CLASSE PRINCIPAL
// ============================================================================

/**
 * @brief Processador de macros do pré-processador C
 */
class MacroProcessor {
private:
    // Armazenamento de macros
    std::unordered_map<std::string, MacroInfo> macros_;
    
    // Componentes auxiliares
    std::shared_ptr<Preprocessor::PreprocessorLogger> logger_;
    std::shared_ptr<Preprocessor::PreprocessorState> state_;
    
    // Controle de expansão
    MacroExpansionContext expansionContext_;
    
    // Cache de expansões
    std::unordered_map<std::string, std::string> expansionCache_;
    bool cacheEnabled_;
    
    // Configurações de otimização
    size_t maxCacheSize_;
    bool enablePrecompilation_;
    std::unordered_map<std::string, time_t> cacheTimestamps_;
    
    // Estatísticas
    size_t totalExpansions_;
    size_t cacheHits_;
    size_t cacheMisses_;
    
    // Tratamento de erros
    void* external_error_handler_;
    
public:
    // ========================================================================
    // CONSTRUTORES E DESTRUTOR
    // ========================================================================
    
    MacroProcessor();
    MacroProcessor(std::shared_ptr<Preprocessor::PreprocessorLogger> logger,
                   std::shared_ptr<Preprocessor::PreprocessorState> state);
    ~MacroProcessor();
    
    // Não copiável, mas movível
    MacroProcessor(const MacroProcessor&) = delete;
    MacroProcessor& operator=(const MacroProcessor&) = delete;
    MacroProcessor(MacroProcessor&&) = default;
    MacroProcessor& operator=(MacroProcessor&&) = default;
    
    // ========================================================================
    // OPERAÇÕES BÁSICAS DE MACRO
    // ========================================================================
    
    /**
     * @brief Define uma macro simples (object-like)
     * @param name Nome da macro
     * @param value Valor da macro
     * @param position Posição onde foi definida
     * @return true se definida com sucesso
     */
    bool defineMacro(const std::string& name, const std::string& value,
                     const Preprocessor::PreprocessorPosition& position = Preprocessor::PreprocessorPosition());
    
    /**
     * @brief Define uma macro funcional (function-like)
     * @param name Nome da macro
     * @param parameters Lista de parâmetros
     * @param body Corpo da macro
     * @param isVariadic Se é variádica
     * @param position Posição onde foi definida
     * @return true se definida com sucesso
     */
    bool defineFunctionMacro(const std::string& name,
                            const std::vector<std::string>& parameters,
                            const std::string& body,
                            bool isVariadic = false,
                            const Preprocessor::PreprocessorPosition& position = Preprocessor::PreprocessorPosition());
    
    /**
     * @brief Remove uma macro
     * @param name Nome da macro a remover
     * @return true se removida com sucesso
     */
    bool undefineMacro(const std::string& name);
    
    // ========================================================================
    // CONSULTAS DE MACRO
    // ========================================================================
    
    /**
     * @brief Verifica se uma macro está definida
     * @param name Nome da macro
     * @return true se definida
     */
    bool isDefined(const std::string& name) const;
    
    /**
     * @brief Obtém o valor de uma macro
     * @param name Nome da macro
     * @return Valor da macro ou string vazia se não definida
     */
    std::string getMacroValue(const std::string& name) const;
    
    /**
     * @brief Obtém informações completas de uma macro
     * @param name Nome da macro
     * @return Ponteiro para MacroInfo ou nullptr se não encontrada
     */
    const MacroInfo* getMacroInfo(const std::string& name) const;
    
    /**
     * @brief Obtém lista de todas as macros definidas
     * @return Vetor com nomes das macros
     */
    std::vector<std::string> getDefinedMacros() const;
    
    // ========================================================================
    // EXPANSÃO DE MACROS
    // ========================================================================
    
    /**
     * @brief Expande uma macro simples
     * @param name Nome da macro
     * @return Texto expandido
     */
    std::string expandMacro(const std::string& name);
    
    /**
     * @brief Expande uma macro funcional
     * @param name Nome da macro
     * @param arguments Argumentos da macro
     * @return Texto expandido
     */
    std::string expandFunctionMacro(const std::string& name,
                                   const std::vector<std::string>& arguments);
    
    /**
     * @brief Expande macros recursivamente em um texto
     * @param text Texto a expandir
     * @return Texto com macros expandidas
     */
    std::string expandMacroRecursively(const std::string& text);
    
    /**
     * @brief Processa uma linha completa expandindo macros
     * @param line Linha a processar
     * @return Linha processada
     */
    std::string processLine(const std::string& line);
    
    // ========================================================================
    // VALIDAÇÃO
    // ========================================================================
    
    /**
     * @brief Valida um nome de macro
     * @param name Nome a validar
     * @return true se válido
     */
    bool validateMacroName(const std::string& name) const;
    
    /**
     * @brief Valida um valor de macro
     * @param value Valor a validar
     * @return true se válido
     */
    bool validateMacroValue(const std::string& value) const;
    
    /**
     * @brief Valida parâmetros de macro funcional
     * @param parameters Parâmetros a validar
     * @return true se válidos
     */
    bool validateParameters(const std::vector<std::string>& parameters) const;
    
    /**
     * @brief Valida contagem de argumentos para macro funcional
     * @param macroName Nome da macro
     * @param argumentCount Número de argumentos fornecidos
     * @return true se válido
     */
    bool validateParameterCount(const std::string& macroName, size_t argumentCount) const;
    
    // ========================================================================
    // TRATAMENTO ESPECIAL
    // ========================================================================
    
    /**
     * @brief Trata redefinição de macro
     * @param name Nome da macro
     * @param newInfo Nova informação da macro
     * @return true se redefinição foi aceita
     */
    bool handleMacroRedefinition(const std::string& name, const MacroInfo& newInfo);
    
    /**
     * @brief Processa operador de stringificação (#)
     * @param argument Argumento a stringificar
     * @return String stringificada
     */
    std::string handleStringification(const std::string& argument);
    
    /**
     * @brief Processa operador de concatenação (##)
     * @param left Lado esquerdo
     * @param right Lado direito
     * @return Tokens concatenados
     */
    std::string handleConcatenation(const std::string& left, const std::string& right);
    
    /**
     * @brief Processa argumentos variádicos (__VA_ARGS__)
     * @param variadicArgs Argumentos variádicos
     * @return String processada
     */
    std::string expandVariadicArguments(const std::vector<std::string>& variadicArgs);
    
    // ========================================================================
    // PARSING E UTILITÁRIOS
    // ========================================================================
    
    /**
     * @brief Faz parsing de lista de parâmetros
     * @param parameterList String com parâmetros
     * @return Vetor de parâmetros
     */
    std::vector<std::string> parseParameterList(const std::string& parameterList);
    
    /**
     * @brief Faz parsing de argumentos de chamada de macro
     * @param argumentList String com argumentos
     * @return Vetor de argumentos
     */
    std::vector<std::string> parseArgumentList(const std::string& argumentList);
    
    // ========================================================================
    // MACROS PREDEFINIDAS
    // ========================================================================
    
    /**
     * @brief Inicializa macros predefinidas do sistema
     */
    void initializePredefinedMacros();
    
    /**
     * @brief Define macros específicas do compilador
     */
    void defineCompilerMacros();
    
    /**
     * @brief Define macros de data/hora
     */
    void defineDateTimeMacros();
    
    // ========================================================================
    // OTIMIZAÇÃO E CACHE
    // ========================================================================
    
    /**
     * @brief Habilita/desabilita cache de expansões
     * @param enabled Se deve habilitar cache
     */
    void setCacheEnabled(bool enabled);
    
    /**
     * @brief Limpa cache de expansões
     */
    void clearCache();
    
    /**
     * @brief Otimiza expansão de macro específica
     * @param macroName Nome da macro a otimizar
     * @return true se otimização foi aplicada
     */
    bool optimizeMacroExpansion(const std::string& macroName);
    
    /**
     * @brief Configura otimizações de cache para macros
     * @param maxCacheSize Tamanho máximo do cache
     * @param enablePrecompilation Habilita pré-compilação de macros frequentes
     */
    void configureCacheOptimization(size_t maxCacheSize, bool enablePrecompilation = true);
    
    /**
     * @brief Otimiza cache removendo entradas antigas
     * @param maxAge Idade máxima das entradas em segundos
     */
    void optimizeCache(int maxAge = 300);
    
    /**
     * @brief Pré-carrega macros frequentemente usadas
     * @param macroNames Lista de macros para pré-carregar
     */
    void preloadFrequentMacros(const std::vector<std::string>& macroNames);
    
    /**
     * @brief Obtém tamanho atual do cache
     * @return Número de entradas no cache
     */
    size_t getCurrentCacheSize() const;
    
    /**
     * @brief Armazena resultado no cache
     * @param key Chave do cache
     * @param result Resultado a armazenar
     */
    void cacheMacroResult(const std::string& key, const std::string& result);
    
    // ========================================================================
    // ESTATÍSTICAS E RELATÓRIOS
    // ========================================================================
    
    /**
     * @brief Obtém estatísticas de uso
     * @return String com estatísticas
     */
    std::string getStatistics() const;
    
    /**
     * @brief Gera relatório de macros definidas
     * @return String com relatório
     */
    std::string generateMacroReport() const;
    
    /**
     * @brief Reseta estatísticas
     */
    void resetStatistics();
    
    // ========================================================================
    // CONFIGURAÇÃO E CONTROLE
    // ========================================================================
    
    /**
     * @brief Define profundidade máxima de expansão
     * @param maxDepth Nova profundidade máxima
     */
    void setMaxExpansionDepth(int maxDepth);
    
    /**
     * @brief Reseta contexto de expansão
     */
    void resetExpansionContext();
    
    /**
     * @brief Limpa todas as macros (exceto predefinidas)
     */
    void clearUserMacros();
    
    /**
     * @brief Limpa todas as macros (incluindo predefinidas)
     */
    void clearAllMacros();
    
    /**
     * @brief Define o manipulador de erros externo
     * @param errorHandler Ponteiro para o manipulador de erros
     */
    void setErrorHandler(void* errorHandler);
    
private:
    // ========================================================================
    // MÉTODOS AUXILIARES PRIVADOS
    // ========================================================================
    
    /**
     * @brief Inicializa componentes internos
     */
    void initializeComponents();
    
    /**
     * @brief Verifica se nome é palavra reservada
     * @param name Nome a verificar
     * @return true se é reservado
     */
    bool isReservedName(const std::string& name) const;
    
    /**
     * @brief Substitui parâmetros por argumentos no corpo da macro
     * @param body Corpo da macro
     * @param parameters Lista de parâmetros
     * @param arguments Lista de argumentos
     * @return String com parâmetros substituídos
     */
    std::string substituteParameters(const std::string& body,
                                   const std::vector<std::string>& parameters,
                                   const std::vector<std::string>& arguments);
    
    /**
     * @brief Processa operadores # (stringificação) e ## (concatenação)
     * @param body Corpo da macro
     * @param parameters Lista de parâmetros
     * @param arguments Lista de argumentos
     * @return String com operadores processados
     */
    std::string processStringificationAndConcatenation(const std::string& body,
                                                      const std::vector<std::string>& parameters,
                                                      const std::vector<std::string>& arguments);
    
    /**
     * @brief Encontra próxima ocorrência de macro no texto
     * @param text Texto a analisar
     * @param startPos Posição inicial
     * @return Par com posição e nome da macro (ou string vazia se não encontrada)
     */
    std::pair<size_t, std::string> findNextMacro(const std::string& text, size_t startPos = 0);
    
    /**
     * @brief Verifica se posição está dentro de string literal
     * @param text Texto
     * @param pos Posição
     * @return true se dentro de string
     */
    bool isInsideStringLiteral(const std::string& text, size_t pos) const;
    
    /**
     * @brief Verifica se posição está dentro de comentário
     * @param text Texto
     * @param pos Posição
     * @return true se dentro de comentário
     */
    bool isInsideComment(const std::string& text, size_t pos) const;
    
    /**
     * @brief Gera chave de cache para expansão
     * @param macroName Nome da macro
     * @param arguments Argumentos (se houver)
     * @return Chave do cache
     */
    std::string generateCacheKey(const std::string& macroName,
                               const std::vector<std::string>& arguments = {}) const;
    
    /**
     * @brief Registra erro de macro
     * @param message Mensagem de erro
     * @param position Posição do erro
     */
    void logMacroError(const std::string& message,
                      const Preprocessor::PreprocessorPosition& position = Preprocessor::PreprocessorPosition());
    
    /**
     * @brief Registra aviso de macro
     * @param message Mensagem de aviso
     * @param position Posição do aviso
     */
    void logMacroWarning(const std::string& message,
                        const Preprocessor::PreprocessorPosition& position = Preprocessor::PreprocessorPosition());
};

// ============================================================================
// FUNÇÕES UTILITÁRIAS GLOBAIS
// ============================================================================

/**
 * @brief Converte tipo de macro para string
 * @param type Tipo da macro
 * @return String representando o tipo
 */
std::string macroTypeToString(MacroType type);

/**
 * @brief Verifica se caractere é válido para nome de macro
 * @param c Caractere a verificar
 * @param isFirst Se é o primeiro caractere
 * @return true se válido
 */
bool isValidMacroNameChar(char c, bool isFirst = false);

/**
 * @brief Escapa string para uso em macro
 * @param str String a escapar
 * @return String escapada
 */
std::string escapeMacroString(const std::string& str);

/**
 * @brief Remove espaços em branco de string
 * @param str String a processar
 * @return String sem espaços extras
 */
std::string trimWhitespace(const std::string& str);

} // namespace Preprocessor

#endif // MACRO_PROCESSOR_HPP