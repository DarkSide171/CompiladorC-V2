// Lexer Config - Implementação das configurações
// Implementação da classe LexerConfig para gerenciamento de configurações

#include "../include/lexer_config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace Lexer {

// ============================================================================
// Construtores e Destrutor
// ============================================================================

LexerConfig::LexerConfig(CVersion version, ErrorHandler* handler)
    : currentVersion(version), errorHandler(handler) {
    if (!validateVersion(version)) {
        if (errorHandler) {
            errorHandler->reportError(ErrorType::INTERNAL_ERROR, 
                "Versão do C não suportada", Position{0, 0, 0});
        }
        currentVersion = CVersion::C99; // Fallback para C99
    }
    
    initializeFeatures(currentVersion);
    loadDefaultKeywords(currentVersion);
}

LexerConfig::LexerConfig(const LexerConfig& other)
    : currentVersion(other.currentVersion),
      enabledFeatures(other.enabledFeatures),
      keywords(other.keywords),
      keywordMap(other.keywordMap),
      errorHandler(other.errorHandler) {
}

LexerConfig& LexerConfig::operator=(const LexerConfig& other) {
    if (this != &other) {
        currentVersion = other.currentVersion;
        enabledFeatures = other.enabledFeatures;
        keywords = other.keywords;
        keywordMap = other.keywordMap;
        errorHandler = other.errorHandler;
    }
    return *this;
}

// ============================================================================
// Gerenciamento de Versão
// ============================================================================

void LexerConfig::setVersion(CVersion version) {
    if (!validateVersion(version)) {
        if (errorHandler) {
            errorHandler->reportError(ErrorType::INTERNAL_ERROR,
                "Versão do C não suportada: " + versionToString(version), Position{0, 0, 0});
        }
        return;
    }
    
    currentVersion = version;
    clearConfiguration();
    initializeFeatures(version);
    loadDefaultKeywords(version);
}

CVersion LexerConfig::getVersion() const {
    return currentVersion;
}

std::string LexerConfig::getVersionString() const {
    return versionToString(currentVersion);
}

// ============================================================================
// Gerenciamento de Palavras-chave
// ============================================================================

void LexerConfig::loadKeywords(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        if (errorHandler) {
            errorHandler->reportError(ErrorType::FILE_NOT_FOUND,
                "Não foi possível abrir arquivo de palavras-chave: " + filename, Position{0, 0, 0});
        }
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Remove comentários e espaços em branco
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Remove espaços em branco no início e fim
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (!line.empty()) {
            keywords.insert(line);
            // Mapear para tipo de token apropriado baseado na palavra-chave
            TokenType type = this->mapKeywordToTokenType(line);
            keywordMap[line] = type;
        }
    }
    
    file.close();
}

std::set<std::string> LexerConfig::getKeywords() const {
    return keywords;
}

bool LexerConfig::isKeyword(const std::string& word) const {
    return keywords.find(word) != keywords.end();
}

TokenType LexerConfig::getKeywordType(const std::string& word) const {
    auto it = keywordMap.find(word);
    return (it != keywordMap.end()) ? it->second : TokenType::IDENTIFIER;
}

void LexerConfig::addKeyword(const std::string& word, TokenType type) {
    keywords.insert(word);
    keywordMap[word] = type;
}

void LexerConfig::removeKeyword(const std::string& word) {
    keywords.erase(word);
    keywordMap.erase(word);
}

// ============================================================================
// Gerenciamento de Features
// ============================================================================

bool LexerConfig::isFeatureEnabled(Feature feature) const {
    return enabledFeatures.find(feature) != enabledFeatures.end();
}

void LexerConfig::enableFeature(Feature feature) {
    enabledFeatures.insert(feature);
}

void LexerConfig::disableFeature(Feature feature) {
    enabledFeatures.erase(feature);
}

void LexerConfig::enableAllFeatures(CVersion version) {
    enabledFeatures = getDefaultFeatures(version);
}

void LexerConfig::disableAllFeatures() {
    enabledFeatures.clear();
}

std::unordered_set<Feature> LexerConfig::getEnabledFeatures() const {
    return enabledFeatures;
}

// ============================================================================
// Validação e Informações
// ============================================================================

bool LexerConfig::isVersionSupported(CVersion version) const {
    return validateVersion(version);
}

bool LexerConfig::isFeatureSupported(Feature feature, CVersion version) const {
    return isFeatureAvailableInVersion(feature, version);
}

void LexerConfig::printConfiguration() const {
    std::cout << "=== Configuração do Analisador Léxico ===\n";
    std::cout << "Versão: " << versionToString(currentVersion) << "\n";
    std::cout << "Palavras-chave carregadas: " << keywords.size() << "\n";
    std::cout << "Features habilitadas: " << enabledFeatures.size() << "\n";
    
    if (!enabledFeatures.empty()) {
        std::cout << "\nFeatures ativas:\n";
        for (const auto& feature : enabledFeatures) {
            std::cout << "  - " << featureToString(feature) << "\n";
        }
    }
    
    std::cout << "\nPalavras-chave:\n";
    for (const auto& keyword : keywords) {
        std::cout << "  - " << keyword << "\n";
    }
}

void LexerConfig::printSupportedFeatures(CVersion version) const {
    std::cout << "=== Features Suportadas em " << versionToString(version) << " ===\n";
    auto defaultFeatures = getDefaultFeatures(version);
    
    for (const auto& feature : defaultFeatures) {
        std::cout << "  - " << featureToString(feature) << "\n";
    }
}

// ============================================================================
// Operadores
// ============================================================================

bool LexerConfig::operator==(const LexerConfig& other) const {
    return currentVersion == other.currentVersion &&
           enabledFeatures == other.enabledFeatures &&
           keywords == other.keywords;
}

bool LexerConfig::operator!=(const LexerConfig& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const LexerConfig& config) {
    os << "LexerConfig{version=" << versionToString(config.currentVersion)
       << ", features=" << config.enabledFeatures.size()
       << ", keywords=" << config.keywords.size() << "}";
    return os;
}

// ============================================================================
// Métodos Privados
// ============================================================================

void LexerConfig::initializeFeatures(CVersion version) {
    enabledFeatures = getDefaultFeatures(version);
}

void LexerConfig::loadDefaultKeywords(CVersion version) {
    keywords.clear();
    keywordMap.clear();
    
    // Palavras-chave básicas do C89/C90
    std::vector<std::pair<std::string, TokenType>> c89Keywords = {
        {"auto", TokenType::AUTO}, {"break", TokenType::BREAK},
        {"case", TokenType::CASE}, {"char", TokenType::CHAR},
        {"const", TokenType::CONST}, {"continue", TokenType::CONTINUE},
        {"default", TokenType::DEFAULT}, {"do", TokenType::DO},
        {"double", TokenType::DOUBLE}, {"else", TokenType::ELSE},
        {"enum", TokenType::ENUM}, {"extern", TokenType::EXTERN},
        {"float", TokenType::FLOAT}, {"for", TokenType::FOR},
        {"goto", TokenType::GOTO}, {"if", TokenType::IF},
        {"int", TokenType::INT}, {"long", TokenType::LONG},
        {"register", TokenType::REGISTER}, {"return", TokenType::RETURN},
        {"short", TokenType::SHORT}, {"signed", TokenType::SIGNED},
        {"sizeof", TokenType::SIZEOF}, {"static", TokenType::STATIC},
        {"struct", TokenType::STRUCT}, {"switch", TokenType::SWITCH},
        {"typedef", TokenType::TYPEDEF}, {"union", TokenType::UNION},
        {"unsigned", TokenType::UNSIGNED}, {"void", TokenType::VOID},
        {"volatile", TokenType::VOLATILE}, {"while", TokenType::WHILE}
    };
    
    // Adiciona palavras-chave básicas
    for (const auto& kw : c89Keywords) {
        keywords.insert(kw.first);
        keywordMap[kw.first] = kw.second;
    }
    
    // Adiciona palavras-chave específicas da versão
    if (version >= CVersion::C99) {
        std::vector<std::pair<std::string, TokenType>> c99Keywords = {
            {"inline", TokenType::INLINE}, {"restrict", TokenType::RESTRICT},
            {"_Bool", TokenType::_BOOL}, {"_Complex", TokenType::_COMPLEX},
            {"_Imaginary", TokenType::_IMAGINARY}
        };
        
        for (const auto& kw : c99Keywords) {
            keywords.insert(kw.first);
            keywordMap[kw.first] = kw.second;
        }
    }
    
    if (version >= CVersion::C11) {
        std::vector<std::pair<std::string, TokenType>> c11Keywords = {
            {"_Alignas", TokenType::_ALIGNAS}, {"_Alignof", TokenType::_ALIGNOF},
            {"_Atomic", TokenType::_ATOMIC}, {"_Static_assert", TokenType::_STATIC_ASSERT},
            {"_Noreturn", TokenType::_NORETURN}, {"_Thread_local", TokenType::_THREAD_LOCAL},
            {"_Generic", TokenType::_GENERIC}
        };
        
        for (const auto& kw : c11Keywords) {
            keywords.insert(kw.first);
            keywordMap[kw.first] = kw.second;
        }
    }
    
    if (version >= CVersion::C23) {
        std::vector<std::pair<std::string, TokenType>> c23Keywords = {
            {"typeof", TokenType::TYPEOF}, {"typeof_unqual", TokenType::TYPEOF_UNQUAL},
            {"_BitInt", TokenType::_BITINT}, {"_Decimal128", TokenType::_DECIMAL128},
            {"_Decimal32", TokenType::_DECIMAL32}, {"_Decimal64", TokenType::_DECIMAL64}
        };
        
        for (const auto& kw : c23Keywords) {
            keywords.insert(kw.first);
            keywordMap[kw.first] = kw.second;
        }
    }
}

bool LexerConfig::validateVersion(CVersion version) const {
    return version >= CVersion::C89 && version <= CVersion::C23;
}

void LexerConfig::clearConfiguration() {
    enabledFeatures.clear();
    keywords.clear();
    keywordMap.clear();
}

TokenType LexerConfig::mapKeywordToTokenType(const std::string& keyword) const {
    // Mapeamento básico - pode ser expandido
    static const std::unordered_map<std::string, TokenType> basicMapping = {
        {"auto", TokenType::AUTO}, {"break", TokenType::BREAK},
        {"case", TokenType::CASE}, {"char", TokenType::CHAR},
        {"const", TokenType::CONST}, {"continue", TokenType::CONTINUE},
        {"default", TokenType::DEFAULT}, {"do", TokenType::DO},
        {"double", TokenType::DOUBLE}, {"else", TokenType::ELSE},
        {"enum", TokenType::ENUM}, {"extern", TokenType::EXTERN},
        {"float", TokenType::FLOAT}, {"for", TokenType::FOR},
        {"goto", TokenType::GOTO}, {"if", TokenType::IF},
        {"int", TokenType::INT}, {"long", TokenType::LONG},
        {"register", TokenType::REGISTER}, {"return", TokenType::RETURN},
        {"short", TokenType::SHORT}, {"signed", TokenType::SIGNED},
        {"sizeof", TokenType::SIZEOF}, {"static", TokenType::STATIC},
        {"struct", TokenType::STRUCT}, {"switch", TokenType::SWITCH},
        {"typedef", TokenType::TYPEDEF}, {"union", TokenType::UNION},
        {"unsigned", TokenType::UNSIGNED}, {"void", TokenType::VOID},
        {"volatile", TokenType::VOLATILE}, {"while", TokenType::WHILE},
        // C99
        {"inline", TokenType::INLINE}, {"restrict", TokenType::RESTRICT},
        {"_Bool", TokenType::_BOOL}, {"_Complex", TokenType::_COMPLEX},
        {"_Imaginary", TokenType::_IMAGINARY},
        // C11
        {"_Alignas", TokenType::_ALIGNAS}, {"_Alignof", TokenType::_ALIGNOF},
        {"_Atomic", TokenType::_ATOMIC}, {"_Static_assert", TokenType::_STATIC_ASSERT},
        {"_Noreturn", TokenType::_NORETURN}, {"_Thread_local", TokenType::_THREAD_LOCAL},
        {"_Generic", TokenType::_GENERIC},
        // C23
        {"typeof", TokenType::TYPEOF}, {"typeof_unqual", TokenType::TYPEOF_UNQUAL},
        {"_BitInt", TokenType::_BITINT}, {"_Decimal128", TokenType::_DECIMAL128},
        {"_Decimal32", TokenType::_DECIMAL32}, {"_Decimal64", TokenType::_DECIMAL64}
    };
    
    auto it = basicMapping.find(keyword);
    return (it != basicMapping.end()) ? it->second : TokenType::IDENTIFIER;
}

// ============================================================================
// Funções Utilitárias Globais
// ============================================================================

std::string versionToString(Lexer::CVersion version) {
    switch (version) {
        case Lexer::CVersion::C89: return "C89";
        case Lexer::CVersion::C99: return "C99";
        case Lexer::CVersion::C11: return "C11";
        case Lexer::CVersion::C17: return "C17";
        case Lexer::CVersion::C23: return "C23";
        default: return "Unknown";
    }
}

Lexer::CVersion stringToVersion(const std::string& versionStr) {
    if (versionStr == "C89" || versionStr == "C90") return Lexer::CVersion::C89;
    if (versionStr == "C99") return Lexer::CVersion::C99;
    if (versionStr == "C11") return Lexer::CVersion::C11;
    if (versionStr == "C17" || versionStr == "C18") return Lexer::CVersion::C17;
    if (versionStr == "C23") return Lexer::CVersion::C23;
    
    throw std::invalid_argument("Versão desconhecida: " + versionStr);
}

std::string featureToString(Lexer::Feature feature) {
    switch (feature) {
        // C99 Features
        case Lexer::Feature::INLINE_FUNCTIONS: return "inline_functions";
        case Lexer::Feature::RESTRICT_KEYWORD: return "restrict_keyword";
        case Lexer::Feature::BOOL_TYPE: return "bool_type";
        case Lexer::Feature::COMPLEX_NUMBERS: return "complex_numbers";
        case Lexer::Feature::VARIABLE_LENGTH_ARRAYS: return "variable_length_arrays";
        case Lexer::Feature::DESIGNATED_INITIALIZERS: return "designated_initializers";
        case Lexer::Feature::COMPOUND_LITERALS: return "compound_literals";
        case Lexer::Feature::FLEXIBLE_ARRAY_MEMBERS: return "flexible_array_members";
        
        // C11 Features
        case Lexer::Feature::ALIGNAS_ALIGNOF: return "alignas_alignof";
        case Lexer::Feature::ATOMIC_OPERATIONS: return "atomic_operations";
        case Lexer::Feature::STATIC_ASSERTIONS: return "static_assertions";
        case Lexer::Feature::NORETURN_FUNCTIONS: return "noreturn_functions";
        case Lexer::Feature::THREAD_LOCAL_STORAGE: return "thread_local_storage";
        case Lexer::Feature::GENERIC_SELECTIONS: return "generic_selections";
        case Lexer::Feature::ANONYMOUS_STRUCTS_UNIONS: return "anonymous_structs_unions";
        
        // C23 Features
        case Lexer::Feature::TYPEOF_OPERATOR: return "typeof_operator";
        case Lexer::Feature::BITINT_TYPE: return "bitint_type";
        case Lexer::Feature::DECIMAL_FLOATING_POINT: return "decimal_floating_point";
        case Lexer::Feature::BINARY_LITERALS: return "binary_literals";
        case Lexer::Feature::DIGIT_SEPARATORS: return "digit_separators";
        case Lexer::Feature::ATTRIBUTES: return "attributes";
        case Lexer::Feature::LAMBDA_EXPRESSIONS: return "lambda_expressions";
        
        default: return "unknown_feature";
    }
}

Lexer::Feature stringToFeature(const std::string& featureStr) {
    // C99 Features
    if (featureStr == "inline_functions") return Lexer::Feature::INLINE_FUNCTIONS;
    if (featureStr == "restrict_keyword") return Lexer::Feature::RESTRICT_KEYWORD;
    if (featureStr == "bool_type") return Lexer::Feature::BOOL_TYPE;
    if (featureStr == "complex_numbers") return Lexer::Feature::COMPLEX_NUMBERS;
    if (featureStr == "variable_length_arrays") return Lexer::Feature::VARIABLE_LENGTH_ARRAYS;
    if (featureStr == "designated_initializers") return Lexer::Feature::DESIGNATED_INITIALIZERS;
    if (featureStr == "compound_literals") return Lexer::Feature::COMPOUND_LITERALS;
    if (featureStr == "flexible_array_members") return Lexer::Feature::FLEXIBLE_ARRAY_MEMBERS;
    
    // C11 Features
    if (featureStr == "alignas_alignof") return Lexer::Feature::ALIGNAS_ALIGNOF;
    if (featureStr == "atomic_operations") return Lexer::Feature::ATOMIC_OPERATIONS;
    if (featureStr == "static_assertions") return Lexer::Feature::STATIC_ASSERTIONS;
    if (featureStr == "noreturn_functions") return Lexer::Feature::NORETURN_FUNCTIONS;
    if (featureStr == "thread_local_storage") return Lexer::Feature::THREAD_LOCAL_STORAGE;
    if (featureStr == "generic_selections") return Lexer::Feature::GENERIC_SELECTIONS;
    if (featureStr == "anonymous_structs_unions") return Lexer::Feature::ANONYMOUS_STRUCTS_UNIONS;
    
    // C23 Features
    if (featureStr == "typeof_operator") return Lexer::Feature::TYPEOF_OPERATOR;
    if (featureStr == "bitint_type") return Lexer::Feature::BITINT_TYPE;
    if (featureStr == "decimal_floating_point") return Lexer::Feature::DECIMAL_FLOATING_POINT;
    if (featureStr == "binary_literals") return Lexer::Feature::BINARY_LITERALS;
    if (featureStr == "digit_separators") return Lexer::Feature::DIGIT_SEPARATORS;
    if (featureStr == "attributes") return Lexer::Feature::ATTRIBUTES;
    if (featureStr == "lambda_expressions") return Lexer::Feature::LAMBDA_EXPRESSIONS;
    
    throw std::invalid_argument("Feature desconhecida: " + featureStr);
}

bool isFeatureAvailableInVersion(Lexer::Feature feature, Lexer::CVersion version) {
    switch (feature) {
        // C99 Features
        case Lexer::Feature::INLINE_FUNCTIONS:
        case Lexer::Feature::RESTRICT_KEYWORD:
        case Lexer::Feature::BOOL_TYPE:
        case Lexer::Feature::COMPLEX_NUMBERS:
        case Lexer::Feature::VARIABLE_LENGTH_ARRAYS:
        case Lexer::Feature::DESIGNATED_INITIALIZERS:
        case Lexer::Feature::COMPOUND_LITERALS:
        case Lexer::Feature::FLEXIBLE_ARRAY_MEMBERS:
            return version >= Lexer::CVersion::C99;
            
        // C11 Features
        case Lexer::Feature::ALIGNAS_ALIGNOF:
        case Lexer::Feature::ATOMIC_OPERATIONS:
        case Lexer::Feature::STATIC_ASSERTIONS:
        case Lexer::Feature::NORETURN_FUNCTIONS:
        case Lexer::Feature::THREAD_LOCAL_STORAGE:
        case Lexer::Feature::GENERIC_SELECTIONS:
        case Lexer::Feature::ANONYMOUS_STRUCTS_UNIONS:
            return version >= Lexer::CVersion::C11;
            
        // C23 Features
        case Lexer::Feature::TYPEOF_OPERATOR:
        case Lexer::Feature::BITINT_TYPE:
        case Lexer::Feature::DECIMAL_FLOATING_POINT:
        case Lexer::Feature::BINARY_LITERALS:
        case Lexer::Feature::DIGIT_SEPARATORS:
        case Lexer::Feature::ATTRIBUTES:
        case Lexer::Feature::LAMBDA_EXPRESSIONS:
            return version >= Lexer::CVersion::C23;
            
        default:
            return false;
    }
}

// ============================================================================
// Configuração de Arquivos
// ============================================================================

void LexerConfig::parseConfigFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        if (errorHandler) {
            errorHandler->reportError(ErrorType::FILE_NOT_FOUND,
                "Não foi possível abrir arquivo de configuração: " + filename, Position{0, 0, 0});
        }
        return;
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Remove comentários e espaços em branco
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Remove espaços em branco no início e fim
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) continue;
        
        // Parse da linha de configuração (formato: chave=valor)
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            if (errorHandler) {
                errorHandler->reportError(ErrorType::INVALID_EXPRESSION,
                    "Formato inválido na linha " + std::to_string(lineNumber) + ": " + line,
                    Position{lineNumber, 1, 0});
            }
            continue;
        }
        
        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);
        
        // Remove espaços em branco da chave e valor
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Processa as configurações
        try {
            if (key == "version") {
                CVersion version = stringToVersion(value);
                setVersion(version);
            } else if (key == "enable_feature") {
                Feature feature = stringToFeature(value);
                enableFeature(feature);
            } else if (key == "disable_feature") {
                Feature feature = stringToFeature(value);
                disableFeature(feature);
            } else if (key == "keyword_file") {
                loadKeywords(value);
            } else {
                if (errorHandler) {
                    errorHandler->reportError(ErrorType::INVALID_EXPRESSION,
                        "Chave de configuração desconhecida na linha " + std::to_string(lineNumber) + ": " + key,
                        Position{lineNumber, 1, 0});
                }
            }
        } catch (const std::exception& e) {
            if (errorHandler) {
                errorHandler->reportError(ErrorType::INVALID_EXPRESSION,
                    "Erro ao processar configuração na linha " + std::to_string(lineNumber) + ": " + e.what(),
                    Position{lineNumber, 1, 0});
            }
        }
    }
    
    file.close();
}

void LexerConfig::saveConfigFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        if (errorHandler) {
            errorHandler->reportError(ErrorType::FILE_NOT_FOUND,
                "Não foi possível criar arquivo de configuração: " + filename, Position{0, 0, 0});
        }
        return;
    }
    
    // Cabeçalho do arquivo
    file << "# Arquivo de Configuração do Analisador Léxico\n";
    file << "# Gerado automaticamente\n\n";
    
    // Versão do C
    file << "# Versão do padrão C\n";
    file << "version=" << versionToString(currentVersion) << "\n\n";
    
    // Features habilitadas
    if (!enabledFeatures.empty()) {
        file << "# Features habilitadas\n";
        for (const auto& feature : enabledFeatures) {
            file << "enable_feature=" << featureToString(feature) << "\n";
        }
        file << "\n";
    }
    
    // Palavras-chave personalizadas (apenas as que não são padrão)
    std::set<std::string> defaultKeywords;
    LexerConfig tempConfig(currentVersion, nullptr);
    auto tempKeywords = tempConfig.getKeywords();
    
    file << "# Palavras-chave personalizadas (além das padrão)\n";
    bool hasCustomKeywords = false;
    for (const auto& keyword : keywords) {
        if (tempKeywords.find(keyword) == tempKeywords.end()) {
            file << "# Palavra-chave personalizada: " << keyword << "\n";
            hasCustomKeywords = true;
        }
    }
    
    if (!hasCustomKeywords) {
        file << "# Nenhuma palavra-chave personalizada definida\n";
    }
    
    file << "\n# Fim da configuração\n";
    file.close();
}

std::unordered_set<Lexer::Feature> getDefaultFeatures(Lexer::CVersion version) {
    std::unordered_set<Lexer::Feature> features;
    
    if (version >= Lexer::CVersion::C99) {
        features.insert(Lexer::Feature::INLINE_FUNCTIONS);
        features.insert(Lexer::Feature::RESTRICT_KEYWORD);
        features.insert(Lexer::Feature::BOOL_TYPE);
        features.insert(Lexer::Feature::COMPLEX_NUMBERS);
        features.insert(Lexer::Feature::VARIABLE_LENGTH_ARRAYS);
        features.insert(Lexer::Feature::DESIGNATED_INITIALIZERS);
        features.insert(Lexer::Feature::COMPOUND_LITERALS);
        features.insert(Lexer::Feature::FLEXIBLE_ARRAY_MEMBERS);
    }
    
    if (version >= Lexer::CVersion::C11) {
        features.insert(Lexer::Feature::ALIGNAS_ALIGNOF);
        features.insert(Lexer::Feature::ATOMIC_OPERATIONS);
        features.insert(Lexer::Feature::STATIC_ASSERTIONS);
        features.insert(Lexer::Feature::NORETURN_FUNCTIONS);
        features.insert(Lexer::Feature::THREAD_LOCAL_STORAGE);
        features.insert(Lexer::Feature::GENERIC_SELECTIONS);
        features.insert(Lexer::Feature::ANONYMOUS_STRUCTS_UNIONS);
    }
    
    if (version >= Lexer::CVersion::C23) {
        features.insert(Lexer::Feature::TYPEOF_OPERATOR);
        features.insert(Lexer::Feature::BITINT_TYPE);
        features.insert(Lexer::Feature::DECIMAL_FLOATING_POINT);
        features.insert(Lexer::Feature::BINARY_LITERALS);
        features.insert(Lexer::Feature::DIGIT_SEPARATORS);
        features.insert(Lexer::Feature::ATTRIBUTES);
        features.insert(Lexer::Feature::LAMBDA_EXPRESSIONS);
    }
    
    return features;
}

} // namespace Lexer