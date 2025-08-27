#ifndef PREPROCESSOR_CONFIG_HPP
#define PREPROCESSOR_CONFIG_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Preprocessor {

/**
 * @brief Enumeração das versões do padrão C suportadas
 * 
 * Define as diferentes versões do padrão C que o pré-processador pode processar,
 * cada uma com suas características específicas de diretivas e macros predefinidas.
 */
enum class CVersion {
    C89,    ///< C89/C90 - Padrão ANSI C original
    C99,    ///< C99 - Padrão ISO C de 1999
    C11,    ///< C11 - Padrão ISO C de 2011
    C17,    ///< C17/C18 - Correções técnicas do C11
    C23     ///< C23 - Padrão ISO C de 2023 (mais recente)
};

/**
 * @brief Estrutura para armazenar configurações de features por versão do C
 */
struct VersionFeatures {
    bool supports_variadic_macros = false;      ///< Suporte a macros variádicas
    bool supports_pragma_once = false;          ///< Suporte a #pragma once
    bool supports_generic_selection = false;    ///< Suporte a _Generic
    bool supports_static_assert = false;        ///< Suporte a _Static_assert
    bool supports_alignof = false;              ///< Suporte a _Alignof
    bool supports_thread_local = false;         ///< Suporte a _Thread_local
    bool supports_unicode_literals = false;     ///< Suporte a literais Unicode
    bool supports_inline_functions = false;     ///< Suporte a funções inline
    bool supports_typeof = false;               ///< Suporte a typeof (C23)
    bool supports_decimal_floats = false;       ///< Suporte a floats decimais (C23)
};

/**
 * @brief Classe principal de configuração do pré-processador
 * 
 * Gerencia todas as configurações do pré-processador, incluindo versão do C,
 * caminhos de inclusão, macros predefinidas e flags de features.
 */
class PreprocessorConfig {
public:
    /**
     * @brief Construtor padrão
     * Inicializa com configurações padrão (C99)
     */
    PreprocessorConfig();
    
    /**
     * @brief Construtor com versão específica
     * @param version Versão do C a ser utilizada
     */
    explicit PreprocessorConfig(CVersion version);
    
    /**
     * @brief Destrutor
     */
    ~PreprocessorConfig() = default;
    
    // ========================================================================
    // MÉTODOS DE INICIALIZAÇÃO E CONFIGURAÇÃO
    // ========================================================================
    
    /**
     * @brief Inicializa configurações padrão
     * Define valores padrão para todas as configurações
     */
    void initializeDefaultConfig();
    
    /**
     * @brief Carrega configuração de arquivo
     * @param filepath Caminho para o arquivo de configuração
     * @return true se carregado com sucesso, false caso contrário
     */
    bool loadConfiguration(const std::string& filepath);
    
    /**
     * @brief Carrega configuração de arquivo específico
     * @param filepath Caminho para o arquivo de configuração
     * @return true se carregado com sucesso, false caso contrário
     */
    bool loadConfigFromFile(const std::string& filepath);
    
    /**
     * @brief Valida a configuração atual
     * @return true se válida, false caso contrário
     */
    bool validateConfiguration();
    
    /**
     * @brief Valida um valor de configuração específico
     * @param key Chave da configuração
     * @param value Valor a ser validado
     * @return true se válido, false caso contrário
     */
    bool validateConfigValue(const std::string& key, const std::string& value);
    
    // ========================================================================
    // MÉTODOS DE PARSING E PROCESSAMENTO
    // ========================================================================
    
    /**
     * @brief Analisa uma linha de configuração
     * @param line Linha a ser analisada
     * @return true se processada com sucesso, false caso contrário
     */
    bool parseConfigLine(const std::string& line);
    
    /**
     * @brief Aplica mudanças de configuração
     * Aplica todas as mudanças pendentes na configuração
     */
    void applyConfigChanges();
    
    /**
     * @brief Salva configuração em arquivo
     * @param filepath Caminho para salvar a configuração
     * @return true se salvo com sucesso, false caso contrário
     */
    bool saveConfigToFile(const std::string& filepath);
    
    /**
     * @brief Mescla configurações com outra instância
     * @param other Configuração a ser mesclada
     */
    void mergeConfigurations(const PreprocessorConfig& other);
    
    /**
     * @brief Restaura configurações padrão
     * Reseta todas as configurações para os valores padrão
     */
    void resetToDefaults();
    
    // ========================================================================
    // MÉTODOS DE COMPATIBILIDADE E FEATURES
    // ========================================================================
    
    /**
     * @brief Valida compatibilidade com padrão C
     * @param standard Padrão C a ser validado
     * @return true se compatível, false caso contrário
     */
    bool validateCStandardCompatibility(CVersion standard);
    
    /**
     * @brief Atualiza flags de features baseado na versão
     * @param standard Versão do C para atualizar features
     */
    void updateFeatureFlags(CVersion standard);
    
    // ========================================================================
    // MÉTODOS DE GERENCIAMENTO DE CAMINHOS
    // ========================================================================
    
    /**
     * @brief Resolve caminhos de inclusão
     * Processa e valida todos os caminhos de inclusão configurados
     */
    void resolveIncludePaths();
    
    /**
     * @brief Valida um caminho de inclusão
     * @param path Caminho a ser validado
     * @return true se válido, false caso contrário
     */
    bool validateIncludePath(const std::string& path);
    
    /**
     * @brief Normaliza um caminho de inclusão
     * @param path Caminho a ser normalizado
     * @return Caminho normalizado
     */
    std::string normalizeIncludePath(const std::string& path);
    
    /**
     * @brief Manipula variáveis de ambiente
     * Processa variáveis de ambiente relevantes para configuração
     */
    void handleEnvironmentVariables();
    
    /**
     * @brief Gera relatório de configuração
     * @return String com relatório detalhado da configuração
     */
    std::string generateConfigReport() const;
    
    // ========================================================================
    // GETTERS E SETTERS
    // ========================================================================
    
    /**
     * @brief Obtém a versão atual do C
     * @return Versão do C configurada
     */
    CVersion getVersion() const { return c_version_; }
    
    /**
     * @brief Define a versão do C
     * @param version Nova versão do C
     */
    void setVersion(CVersion version);
    
    /**
     * @brief Obtém os caminhos de inclusão
     * @return Vetor com caminhos de inclusão
     */
    const std::vector<std::string>& getIncludePaths() const { return include_paths_; }
    
    /**
     * @brief Adiciona caminho de inclusão
     * @param path Caminho a ser adicionado
     */
    void addIncludePath(const std::string& path);
    
    /**
     * @brief Remove caminho de inclusão
     * @param path Caminho a ser removido
     */
    void removeIncludePath(const std::string& path);
    
    /**
     * @brief Limpa todos os caminhos de inclusão
     */
    void clearIncludePaths();
    
    /**
     * @brief Obtém as macros predefinidas
     * @return Mapa com macros predefinidas
     */
    const std::unordered_map<std::string, std::string>& getPredefinedMacros() const { return predefined_macros_; }
    
    /**
     * @brief Adiciona macro predefinida
     * @param name Nome da macro
     * @param value Valor da macro
     */
    void addPredefinedMacro(const std::string& name, const std::string& value);
    
    /**
     * @brief Remove macro predefinida
     * @param name Nome da macro a ser removida
     */
    void removePredefinedMacro(const std::string& name);
    
    /**
     * @brief Obtém as features da versão atual
     * @return Estrutura com features suportadas
     */
    const VersionFeatures& getVersionFeatures() const { return version_features_; }
    
    /**
     * @brief Verifica se modo debug está ativo
     * @return true se debug ativo, false caso contrário
     */
    bool isDebugMode() const { return debug_mode_; }
    
    /**
     * @brief Define modo debug
     * @param debug Novo estado do modo debug
     */
    void setDebugMode(bool debug) { debug_mode_ = debug; }
    
    /**
     * @brief Verifica se warnings estão habilitados
     * @return true se warnings habilitados, false caso contrário
     */
    bool areWarningsEnabled() const { return enable_warnings_; }
    
    /**
     * @brief Define se warnings estão habilitados
     * @param enabled Novo estado dos warnings
     */
    void setWarningsEnabled(bool enabled) { enable_warnings_ = enabled; }
    
    /**
     * @brief Obtém tamanho máximo de expansão de macro
     * @return Tamanho máximo em bytes
     */
    size_t getMaxMacroExpansionSize() const { return max_macro_expansion_size_; }
    
    /**
     * @brief Define tamanho máximo de expansão de macro
     * @param size Novo tamanho máximo
     */
    void setMaxMacroExpansionSize(size_t size) { max_macro_expansion_size_ = size; }
    
    /**
     * @brief Obtém profundidade máxima de inclusão
     * @return Profundidade máxima
     */
    int getMaxIncludeDepth() const { return max_include_depth_; }
    
    /**
     * @brief Define profundidade máxima de inclusão
     * @param depth Nova profundidade máxima
     */
    void setMaxIncludeDepth(int depth) { max_include_depth_ = depth; }

private:
    // ========================================================================
    // MEMBROS PRIVADOS
    // ========================================================================
    
    CVersion c_version_;                                    ///< Versão do C configurada
    VersionFeatures version_features_;                      ///< Features da versão atual
    std::vector<std::string> include_paths_;               ///< Caminhos de inclusão
    std::unordered_map<std::string, std::string> predefined_macros_;  ///< Macros predefinidas
    std::unordered_map<std::string, std::string> config_values_;      ///< Valores de configuração
    
    // Configurações de comportamento
    bool debug_mode_ = false;                              ///< Modo debug ativo
    bool enable_warnings_ = true;                          ///< Warnings habilitados
    bool strict_mode_ = false;                             ///< Modo estrito
    bool preserve_comments_ = false;                       ///< Preservar comentários
    
    // Limites de processamento
    size_t max_macro_expansion_size_ = 1024 * 1024;       ///< Tamanho máximo de expansão (1MB)
    int max_include_depth_ = 200;                         ///< Profundidade máxima de inclusão
    int max_macro_recursion_depth_ = 1000;                ///< Profundidade máxima de recursão de macro
    
    // ========================================================================
    // MÉTODOS AUXILIARES PRIVADOS
    // ========================================================================
    
    /**
     * @brief Inicializa features baseado na versão
     * @param version Versão do C
     */
    void initializeVersionFeatures(CVersion version);
    
    /**
     * @brief Carrega macros predefinidas para a versão
     * @param version Versão do C
     */
    void loadPredefinedMacrosForVersion(CVersion version);
    
    /**
     * @brief Valida valor de configuração interna
     * @param key Chave da configuração
     * @param value Valor a ser validado
     * @return true se válido, false caso contrário
     */
    bool validateInternalConfigValue(const std::string& key, const std::string& value);
};

// ============================================================================
// FUNÇÕES UTILITÁRIAS
// ============================================================================

/**
 * @brief Converte versão do C para string
 * @param version Versão do C
 * @return String representando a versão
 */
std::string cVersionToString(CVersion version);

/**
 * @brief Converte string para versão do C
 * @param version_str String da versão
 * @return Versão do C correspondente
 */
CVersion stringToCVersion(const std::string& version_str);

/**
 * @brief Verifica se uma versão suporta uma feature específica
 * @param version Versão do C
 * @param feature Nome da feature
 * @return true se suportada, false caso contrário
 */
bool versionSupportsFeature(CVersion version, const std::string& feature);

/**
 * @brief Obtém lista de features suportadas por uma versão
 * @param version Versão do C
 * @return Vetor com nomes das features suportadas
 */
std::vector<std::string> getSupportedFeatures(CVersion version);

} // namespace Preprocessor

#endif // PREPROCESSOR_CONFIG_HPP