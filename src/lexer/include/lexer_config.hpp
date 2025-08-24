#ifndef LEXER_CONFIG_HPP
#define LEXER_CONFIG_HPP

#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include "token.hpp"
#include "error_handler.hpp"

namespace Lexer {

// Enumeração das versões do padrão C suportadas
enum class CVersion {
    C89,    // C89/C90 - ANSI C
    C99,    // C99 - ISO/IEC 9899:1999
    C11,    // C11 - ISO/IEC 9899:2011
    C17,    // C17 - ISO/IEC 9899:2018 (C18)
    C23     // C23 - ISO/IEC 9899:2023
};

// Enumeração de features específicas por versão
enum class Feature {
    // Features C99
    INLINE_FUNCTIONS,
    RESTRICT_KEYWORD,
    BOOL_TYPE,
    COMPLEX_NUMBERS,
    VARIABLE_LENGTH_ARRAYS,
    DESIGNATED_INITIALIZERS,
    COMPOUND_LITERALS,
    FLEXIBLE_ARRAY_MEMBERS,
    
    // Features C11
    ALIGNAS_ALIGNOF,
    ATOMIC_OPERATIONS,
    STATIC_ASSERTIONS,
    NORETURN_FUNCTIONS,
    THREAD_LOCAL_STORAGE,
    GENERIC_SELECTIONS,
    ANONYMOUS_STRUCTS_UNIONS,
    
    // Features C17 (compatível com C11)
    
    // Features C23
    TYPEOF_OPERATOR,
    BITINT_TYPE,
    DECIMAL_FLOATING_POINT,
    BINARY_LITERALS,
    DIGIT_SEPARATORS,
    ATTRIBUTES,
    LAMBDA_EXPRESSIONS
};

/**
 * Classe LexerConfig
 * 
 * Gerencia configurações do analisador léxico, incluindo:
 * - Versão do padrão C a ser utilizada
 * - Features habilitadas/desabilitadas
 * - Carregamento de palavras-chave por versão
 * - Validação de configurações
 */
class LexerConfig {
private:
    CVersion currentVersion;                           // Versão atual do padrão C
    std::unordered_set<Feature> enabledFeatures;      // Features habilitadas
    std::set<std::string> keywords;                   // Palavras-chave carregadas
    std::unordered_map<std::string, TokenType> keywordMap; // Mapeamento palavra-chave -> tipo
    ErrorHandler* errorHandler;                       // Handler de erros
    
    // Métodos privados
    void initializeFeatures(CVersion version);        // Inicializa features baseado na versão
    void loadDefaultKeywords(CVersion version);       // Carrega palavras-chave padrão
    bool validateVersion(CVersion version) const;     // Valida se versão é suportada
    void clearConfiguration();                        // Limpa configuração atual
    
    // Funções utilitárias privadas
    TokenType mapKeywordToTokenType(const std::string& keyword) const;
    
public:
    // Construtores e destrutor
    explicit LexerConfig(CVersion version = CVersion::C99, ErrorHandler* handler = nullptr);
    ~LexerConfig() = default;
    
    // Cópia e atribuição
    LexerConfig(const LexerConfig& other);
    LexerConfig& operator=(const LexerConfig& other);
    
    // Gerenciamento de versão
    void setVersion(CVersion version);
    CVersion getVersion() const;
    std::string getVersionString() const;
    
    // Gerenciamento de palavras-chave
    void loadKeywords(const std::string& filename);
    std::set<std::string> getKeywords() const;
    bool isKeyword(const std::string& word) const;
    TokenType getKeywordType(const std::string& word) const;
    void addKeyword(const std::string& word, TokenType type);
    void removeKeyword(const std::string& word);
    
    // Gerenciamento de features
    bool isFeatureEnabled(Feature feature) const;
    void enableFeature(Feature feature);
    void disableFeature(Feature feature);
    void enableAllFeatures(CVersion version);
    void disableAllFeatures();
    std::unordered_set<Feature> getEnabledFeatures() const;
    
    // Configuração de arquivos
    void parseConfigFile(const std::string& filename);
    void saveConfigFile(const std::string& filename) const;
    
    // Validação e informações
    bool isVersionSupported(CVersion version) const;
    bool isFeatureSupported(Feature feature, CVersion version) const;
    void printConfiguration() const;
    void printSupportedFeatures(CVersion version) const;
    
    // Operadores de comparação
    bool operator==(const LexerConfig& other) const;
    bool operator!=(const LexerConfig& other) const;
    
    // Operador de stream
    friend std::ostream& operator<<(std::ostream& os, const LexerConfig& config);
};

// Funções utilitárias globais
std::string versionToString(CVersion version);
CVersion stringToVersion(const std::string& versionStr);
std::string featureToString(Feature feature);
Feature stringToFeature(const std::string& featureStr);
bool isFeatureAvailableInVersion(Feature feature, CVersion version);
std::unordered_set<Feature> getDefaultFeatures(CVersion version);

} // namespace Lexer

#endif // LEXER_CONFIG_HPP