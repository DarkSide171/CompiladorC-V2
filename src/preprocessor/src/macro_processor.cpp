#include "../include/macro_processor.hpp"
#include <algorithm>
#include <sstream>
#include <regex>
#include <cctype>
#include <ctime>
#include <iomanip>

namespace Preprocessor {

// ============================================================================
// IMPLEMENTAÇÃO DA STRUCT MacroInfo
// ============================================================================

MacroInfo::MacroInfo() 
    : name(""), value(""), type(MacroType::OBJECT_LIKE), 
      isVariadic(false), isPredefined(false), expansionCount(0) {
}

MacroInfo::MacroInfo(const std::string& name, const std::string& value, MacroType type)
    : name(name), value(value), type(type), 
      isVariadic(false), isPredefined(false), expansionCount(0) {
}

MacroInfo::MacroInfo(const std::string& name, const std::string& value, 
                     const std::vector<std::string>& params, bool variadic)
    : name(name), value(value), type(variadic ? MacroType::VARIADIC : MacroType::FUNCTION_LIKE),
      parameters(params), isVariadic(variadic), isPredefined(false), expansionCount(0) {
}

bool MacroInfo::operator==(const MacroInfo& other) const {
    return name == other.name && 
           value == other.value && 
           type == other.type &&
           parameters == other.parameters &&
           isVariadic == other.isVariadic;
}

bool MacroInfo::operator!=(const MacroInfo& other) const {
    return !(*this == other);
}

bool MacroInfo::isFunctionLike() const {
    return type == MacroType::FUNCTION_LIKE || type == MacroType::VARIADIC;
}

bool MacroInfo::hasParameters() const {
    return !parameters.empty();
}

size_t MacroInfo::getParameterCount() const {
    return parameters.size();
}

std::string MacroInfo::toString() const {
    std::ostringstream oss;
    oss << "Macro: " << name;
    
    if (isFunctionLike()) {
        oss << "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << parameters[i];
        }
        if (isVariadic) {
            if (!parameters.empty()) oss << ", ";
            oss << "...";
        }
        oss << ")";
    }
    
    oss << " = " << value;
    oss << " [Type: " << macroTypeToString(type) << "]";
    oss << " [Expansions: " << expansionCount << "]";
    
    return oss.str();
}

// ============================================================================
// IMPLEMENTAÇÃO DA STRUCT MacroExpansionContext
// ============================================================================

MacroExpansionContext::MacroExpansionContext(int maxDepth) 
    : maxDepth(maxDepth), currentDepth(0) {
}

bool MacroExpansionContext::canExpand(const std::string& macroName) const {
    return currentDepth < maxDepth && 
           expandingMacros.find(macroName) == expandingMacros.end();
}

void MacroExpansionContext::pushMacro(const std::string& macroName) {
    expandingMacros.insert(macroName);
    currentDepth++;
}

void MacroExpansionContext::popMacro(const std::string& macroName) {
    expandingMacros.erase(macroName);
    if (currentDepth > 0) {
        currentDepth--;
    }
}

void MacroExpansionContext::reset() {
    expandingMacros.clear();
    currentDepth = 0;
}

// ============================================================================
// IMPLEMENTAÇÃO DA CLASSE MacroProcessor
// ============================================================================

// Construtores e Destrutor
MacroProcessor::MacroProcessor() 
    : logger_(nullptr), state_(nullptr), expansionContext_(200),
      cacheEnabled_(true), totalExpansions_(0), cacheHits_(0), cacheMisses_(0) {
    initializeComponents();
}

MacroProcessor::MacroProcessor(std::shared_ptr<Preprocessor::PreprocessorLogger> logger,
                               std::shared_ptr<Preprocessor::PreprocessorState> state)
    : logger_(logger), state_(state), expansionContext_(200),
      cacheEnabled_(true), totalExpansions_(0), cacheHits_(0), cacheMisses_(0) {
    initializeComponents();
}

MacroProcessor::~MacroProcessor() {
    // Destrutor padrão
}

// Operações Básicas de Macro
bool MacroProcessor::defineMacro(const std::string& name, const std::string& value,
                                const Preprocessor::PreprocessorPosition& position) {
    if (!validateMacroName(name)) {
        logMacroError("Nome de macro inválido: " + name, position);
        return false;
    }
    
    if (!validateMacroValue(value)) {
        logMacroError("Valor de macro inválido para: " + name, position);
        return false;
    }
    
    // Verifica redefinição
    auto it = macros_.find(name);
    if (it != macros_.end()) {
        MacroInfo newInfo(name, value, MacroType::OBJECT_LIKE);
        newInfo.definedAt = position;
        
        if (!handleMacroRedefinition(name, newInfo)) {
            return false;
        }
    }
    
    // Define a macro
    MacroInfo info(name, value, MacroType::OBJECT_LIKE);
    info.definedAt = position;
    macros_[name] = info;
    
    // Limpa cache relacionado
    clearCache();
    
    if (logger_) {
        logger_->info("Macro definida: " + name + " = " + value, position);
    }
    
    return true;
}

bool MacroProcessor::defineFunctionMacro(const std::string& name,
                                        const std::vector<std::string>& parameters,
                                        const std::string& body,
                                        bool isVariadic,
                                        const Preprocessor::PreprocessorPosition& position) {
    if (!validateMacroName(name)) {
        logMacroError("Nome de macro inválido: " + name, position);
        return false;
    }
    
    if (!validateParameters(parameters)) {
        logMacroError("Parâmetros inválidos para macro: " + name, position);
        return false;
    }
    
    if (!validateMacroValue(body)) {
        logMacroError("Corpo de macro inválido para: " + name, position);
        return false;
    }
    
    // Verifica redefinição
    auto it = macros_.find(name);
    if (it != macros_.end()) {
        MacroInfo newInfo(name, body, parameters, isVariadic);
        newInfo.definedAt = position;
        
        if (!handleMacroRedefinition(name, newInfo)) {
            return false;
        }
    }
    
    // Define a macro funcional
    MacroInfo info(name, body, parameters, isVariadic);
    info.definedAt = position;
    macros_[name] = info;
    
    // Limpa cache relacionado
    clearCache();
    
    if (logger_) {
        std::ostringstream oss;
        oss << "Macro funcional definida: " << name << "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << parameters[i];
        }
        if (isVariadic) {
            if (!parameters.empty()) oss << ", ";
            oss << "...";
        }
        oss << ") = " << body;
        logger_->info(oss.str(), position);
    }
    
    return true;
}

bool MacroProcessor::undefineMacro(const std::string& name) {
    auto it = macros_.find(name);
    if (it == macros_.end()) {
        return false; // Macro não existe
    }
    
    if (it->second.isPredefined) {
        logMacroWarning("Tentativa de remover macro predefinida: " + name);
        return false;
    }
    
    macros_.erase(it);
    clearCache();
    
    if (logger_) {
        logger_->info("Macro removida: " + name);
    }
    
    return true;
}

// Consultas de Macro
bool MacroProcessor::isDefined(const std::string& name) const {
    return macros_.find(name) != macros_.end();
}

std::string MacroProcessor::getMacroValue(const std::string& name) const {
    auto it = macros_.find(name);
    return (it != macros_.end()) ? it->second.value : "";
}

const MacroInfo* MacroProcessor::getMacroInfo(const std::string& name) const {
    auto it = macros_.find(name);
    return (it != macros_.end()) ? &it->second : nullptr;
}

std::vector<std::string> MacroProcessor::getDefinedMacros() const {
    std::vector<std::string> result;
    result.reserve(macros_.size());
    
    for (const auto& pair : macros_) {
        result.push_back(pair.first);
    }
    
    std::sort(result.begin(), result.end());
    return result;
}

// Expansão de Macros
std::string MacroProcessor::expandMacro(const std::string& name) {
    if (!isDefined(name)) {
        return name; // Retorna o nome original se não definida
    }
    
    if (!expansionContext_.canExpand(name)) {
        logMacroError("Recursão infinita detectada na macro: " + name);
        return name;
    }
    
    // Verifica cache
    if (cacheEnabled_) {
        std::string cacheKey = generateCacheKey(name);
        auto cacheIt = expansionCache_.find(cacheKey);
        if (cacheIt != expansionCache_.end()) {
            cacheHits_++;
            return cacheIt->second;
        }
        cacheMisses_++;
    }
    
    const MacroInfo& info = macros_[name];
    
    if (info.isFunctionLike()) {
        // Macro funcional sem argumentos - retorna nome original
        return name;
    }
    
    // Expande macro simples
    expansionContext_.pushMacro(name);
    std::string result = expandMacroRecursively(info.value);
    expansionContext_.popMacro(name);
    
    // Atualiza estatísticas
    totalExpansions_++;
    macros_[name].expansionCount++;
    
    // Armazena no cache
    if (cacheEnabled_) {
        cacheMacroResult(generateCacheKey(name), result);
    }
    
    return result;
}

std::string MacroProcessor::expandFunctionMacro(const std::string& name,
                                               const std::vector<std::string>& arguments) {
    if (!isDefined(name)) {
        return name;
    }
    
    const MacroInfo& info = macros_[name];
    
    if (!info.isFunctionLike()) {
        logMacroError("Tentativa de chamar macro não-funcional como função: " + name);
        return name;
    }
    
    if (!validateParameterCount(name, arguments.size())) {
        return name;
    }
    
    if (!expansionContext_.canExpand(name)) {
        logMacroError("Recursão infinita detectada na macro: " + name);
        return name;
    }
    
    // Verifica cache
    if (cacheEnabled_) {
        std::string cacheKey = generateCacheKey(name, arguments);
        auto cacheIt = expansionCache_.find(cacheKey);
        if (cacheIt != expansionCache_.end()) {
            cacheHits_++;
            return cacheIt->second;
        }
        cacheMisses_++;
    }
    
    // Substitui parâmetros
    expansionContext_.pushMacro(name);
    std::string result = substituteParameters(info.value, info.parameters, arguments);
    result = expandMacroRecursively(result);
    expansionContext_.popMacro(name);
    
    // Atualiza estatísticas
    totalExpansions_++;
    macros_[name].expansionCount++;
    
    // Armazena no cache
    if (cacheEnabled_) {
        cacheMacroResult(generateCacheKey(name, arguments), result);
    }
    
    return result;
}

std::string MacroProcessor::expandMacroRecursively(const std::string& text) {
    if (text.empty()) {
        return text;
    }
    
    std::string result = text;
    size_t pos = 0;
    
    while (pos < result.length()) {
        auto macroInfo = findNextMacro(result, pos);
        if (macroInfo.second.empty()) {
            break; // Nenhuma macro encontrada
        }
        
        size_t macroPos = macroInfo.first;
        const std::string& macroName = macroInfo.second;
        
        // Verifica se está dentro de string literal ou comentário
        if (isInsideStringLiteral(result, macroPos) || isInsideComment(result, macroPos)) {
            pos = macroPos + macroName.length();
            continue;
        }
        
        // Expande a macro
        std::string expansion = expandMacro(macroName);
        if (expansion != macroName) {
            result.replace(macroPos, macroName.length(), expansion);
            pos = macroPos + expansion.length();
        } else {
            pos = macroPos + macroName.length();
        }
    }
    
    return result;
}

std::string MacroProcessor::processLine(const std::string& line) {
    return expandMacroRecursively(line);
}

// Validação
bool MacroProcessor::validateMacroName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // Primeiro caractere deve ser letra ou underscore
    if (!isValidMacroNameChar(name[0], true)) {
        return false;
    }
    
    // Demais caracteres devem ser letras, dígitos ou underscores
    for (size_t i = 1; i < name.length(); ++i) {
        if (!isValidMacroNameChar(name[i], false)) {
            return false;
        }
    }
    
    // Verifica se é palavra reservada
    if (isReservedName(name)) {
        return false;
    }
    
    return true;
}

bool MacroProcessor::validateMacroValue(const std::string& value) const {
    // Por enquanto, aceita qualquer valor
    // Pode ser expandido para validações mais específicas
    return true;
}

bool MacroProcessor::validateParameters(const std::vector<std::string>& parameters) const {
    std::unordered_set<std::string> seen;
    
    for (const std::string& param : parameters) {
        if (!validateMacroName(param)) {
            return false;
        }
        
        // Verifica duplicatas
        if (seen.find(param) != seen.end()) {
            return false;
        }
        seen.insert(param);
    }
    
    return true;
}

bool MacroProcessor::validateParameterCount(const std::string& macroName, size_t argumentCount) const {
    auto it = macros_.find(macroName);
    if (it == macros_.end()) {
        return false;
    }
    
    const MacroInfo& info = it->second;
    
    if (info.isVariadic) {
        // Macro variádica: deve ter pelo menos o número de parâmetros fixos
        return argumentCount >= info.parameters.size();
    } else {
        // Macro normal: deve ter exatamente o número de parâmetros
        return argumentCount == info.parameters.size();
    }
}

// Tratamento Especial
bool MacroProcessor::handleMacroRedefinition(const std::string& name, const MacroInfo& newInfo) {
    auto it = macros_.find(name);
    if (it == macros_.end()) {
        return true; // Não é redefinição
    }
    
    const MacroInfo& oldInfo = it->second;
    
    if (oldInfo.isPredefined) {
        logMacroError("Tentativa de redefinir macro predefinida: " + name);
        return false;
    }
    
    // Verifica se a redefinição é idêntica
    if (oldInfo == newInfo) {
        // Redefinição idêntica é permitida
        return true;
    }
    
    // Redefinição diferente gera aviso
    logMacroWarning("Redefinição de macro: " + name);
    return true; // Permite redefinição com aviso
}

std::string MacroProcessor::handleStringification(const std::string& argument) {
    std::string result = "\"";
    
    for (char c : argument) {
        if (c == '\"' || c == '\\') {
            result += '\\';
        }
        result += c;
    }
    
    result += "\"";
    return result;
}

std::string MacroProcessor::handleConcatenation(const std::string& left, const std::string& right) {
    std::string leftTrimmed = trimWhitespace(left);
    std::string rightTrimmed = trimWhitespace(right);
    return leftTrimmed + rightTrimmed;
}

std::string MacroProcessor::expandVariadicArguments(const std::vector<std::string>& variadicArgs) {
    if (variadicArgs.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < variadicArgs.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << variadicArgs[i];
    }
    
    return oss.str();
}

// Parsing e Utilitários
std::vector<std::string> MacroProcessor::parseParameterList(const std::string& parameterList) {
    std::vector<std::string> parameters;
    
    if (parameterList.empty()) {
        return parameters;
    }
    
    std::istringstream iss(parameterList);
    std::string param;
    
    while (std::getline(iss, param, ',')) {
        param = trimWhitespace(param);
        if (!param.empty()) {
            parameters.push_back(param);
        }
    }
    
    return parameters;
}

std::vector<std::string> MacroProcessor::parseArgumentList(const std::string& argumentList) {
    std::vector<std::string> arguments;
    
    if (argumentList.empty()) {
        return arguments;
    }
    
    // Parser simples que considera parênteses balanceados
    std::string current;
    int parenLevel = 0;
    bool inString = false;
    bool escaped = false;
    
    for (char c : argumentList) {
        if (escaped) {
            current += c;
            escaped = false;
            continue;
        }
        
        if (c == '\\') {
            escaped = true;
            current += c;
            continue;
        }
        
        if (c == '\"') {
            inString = !inString;
            current += c;
            continue;
        }
        
        if (inString) {
            current += c;
            continue;
        }
        
        if (c == '(') {
            parenLevel++;
            current += c;
        } else if (c == ')') {
            parenLevel--;
            current += c;
        } else if (c == ',' && parenLevel == 0) {
            arguments.push_back(trimWhitespace(current));
            current.clear();
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        arguments.push_back(trimWhitespace(current));
    }
    
    return arguments;
}

// Macros Predefinidas
void MacroProcessor::initializePredefinedMacros() {
    defineCompilerMacros();
    defineDateTimeMacros();
}

void MacroProcessor::defineCompilerMacros() {
    // Macros do compilador
    MacroInfo info;
    info.isPredefined = true;
    
    // __FILE__
    info.name = "__FILE__";
    info.value = "\"<unknown>\"";
    info.type = MacroType::OBJECT_LIKE;
    macros_["__FILE__"] = info;
    
    // __LINE__
    info.name = "__LINE__";
    info.value = "1";
    macros_["__LINE__"] = info;
    
    // __STDC__
    info.name = "__STDC__";
    info.value = "1";
    macros_["__STDC__"] = info;
    
    // __STDC_VERSION__
    info.name = "__STDC_VERSION__";
    info.value = "199901L";
    macros_["__STDC_VERSION__"] = info;
}

void MacroProcessor::defineDateTimeMacros() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    MacroInfo info;
    info.isPredefined = true;
    info.type = MacroType::OBJECT_LIKE;
    
    // __DATE__
    std::ostringstream dateStream;
    dateStream << std::put_time(&tm, "\"%b %d %Y\"");
    info.name = "__DATE__";
    info.value = dateStream.str();
    macros_["__DATE__"] = info;
    
    // __TIME__
    std::ostringstream timeStream;
    timeStream << std::put_time(&tm, "\"%H:%M:%S\"");
    info.name = "__TIME__";
    info.value = timeStream.str();
    macros_["__TIME__"] = info;
}

// Otimização e Cache
void MacroProcessor::setCacheEnabled(bool enabled) {
    cacheEnabled_ = enabled;
    if (!enabled) {
        clearCache();
    }
}

void MacroProcessor::clearCache() {
    expansionCache_.clear();
}

bool MacroProcessor::optimizeMacroExpansion(const std::string& macroName) {
    // Implementação básica de otimização
    // Pode ser expandida no futuro
    return false;
}

void MacroProcessor::cacheMacroResult(const std::string& key, const std::string& result) {
    if (cacheEnabled_) {
        expansionCache_[key] = result;
    }
}

// Estatísticas e Relatórios
std::string MacroProcessor::getStatistics() const {
    std::ostringstream oss;
    oss << "=== Estatísticas do Processador de Macros ===\n";
    oss << "Macros definidas: " << macros_.size() << "\n";
    oss << "Expansões totais: " << totalExpansions_ << "\n";
    oss << "Cache hits: " << cacheHits_ << "\n";
    oss << "Cache misses: " << cacheMisses_ << "\n";
    
    if (cacheHits_ + cacheMisses_ > 0) {
        double hitRate = static_cast<double>(cacheHits_) / (cacheHits_ + cacheMisses_) * 100.0;
        oss << "Taxa de acerto do cache: " << std::fixed << std::setprecision(2) << hitRate << "%\n";
    }
    
    return oss.str();
}

std::string MacroProcessor::generateMacroReport() const {
    std::ostringstream oss;
    oss << "=== Relatório de Macros Definidas ===\n";
    
    std::vector<std::string> macroNames = getDefinedMacros();
    
    for (const std::string& name : macroNames) {
        const MacroInfo* info = getMacroInfo(name);
        if (info) {
            oss << info->toString() << "\n";
        }
    }
    
    return oss.str();
}

void MacroProcessor::resetStatistics() {
    totalExpansions_ = 0;
    cacheHits_ = 0;
    cacheMisses_ = 0;
    
    // Reseta contadores de expansão das macros
    for (auto& pair : macros_) {
        pair.second.expansionCount = 0;
    }
}

// Configuração e Controle
void MacroProcessor::setMaxExpansionDepth(int maxDepth) {
    expansionContext_.maxDepth = maxDepth;
}

void MacroProcessor::resetExpansionContext() {
    expansionContext_.reset();
}

void MacroProcessor::clearUserMacros() {
    auto it = macros_.begin();
    while (it != macros_.end()) {
        if (!it->second.isPredefined) {
            it = macros_.erase(it);
        } else {
            ++it;
        }
    }
    clearCache();
}

void MacroProcessor::clearAllMacros() {
    macros_.clear();
    clearCache();
}

// Métodos Auxiliares Privados
void MacroProcessor::initializeComponents() {
    // Inicializa macros predefinidas
    initializePredefinedMacros();
}

bool MacroProcessor::isReservedName(const std::string& name) const {
    // Lista de palavras reservadas do C
    static const std::unordered_set<std::string> reserved = {
        "auto", "break", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if",
        "int", "long", "register", "return", "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
    };
    
    return reserved.find(name) != reserved.end();
}

std::string MacroProcessor::substituteParameters(const std::string& body,
                                                const std::vector<std::string>& parameters,
                                                const std::vector<std::string>& arguments) {
    std::string result = body;
    
    // Primeiro passo: processar operadores # (stringificação) e ## (concatenação)
    result = processStringificationAndConcatenation(result, parameters, arguments);
    
    // Segundo passo: substituir parâmetros normais
    for (size_t i = 0; i < parameters.size() && i < arguments.size(); ++i) {
        const std::string& param = parameters[i];
        const std::string& arg = arguments[i];
        
        // Usa regex simples para substituir palavras completas
        std::regex paramRegex("\\b" + param + "\\b");
        result = std::regex_replace(result, paramRegex, arg);
    }
    
    // Terceiro passo: tratar argumentos variádicos
    if (arguments.size() > parameters.size()) {
        std::vector<std::string> variadicArgs(arguments.begin() + parameters.size(), arguments.end());
        std::string variadicExpansion = expandVariadicArguments(variadicArgs);
        
        // Substitui __VA_ARGS__
        std::regex variadicRegex("\\b__VA_ARGS__\\b");
        result = std::regex_replace(result, variadicRegex, variadicExpansion);
    }
    
    return result;
}

std::string MacroProcessor::processStringificationAndConcatenation(const std::string& body,
                                                                  const std::vector<std::string>& parameters,
                                                                  const std::vector<std::string>& arguments) {
    std::string result = body;
    
    // Processar operador de stringificação (#)
    for (size_t i = 0; i < parameters.size() && i < arguments.size(); ++i) {
        const std::string& param = parameters[i];
        const std::string& arg = arguments[i];
        
        // Escapar caracteres especiais do parâmetro para regex
        std::string escapedParam = std::regex_replace(param, std::regex("[.^$|()\\[\\]{}*+?]"), "\\$&");
        
        // Procurar por #param
        try {
            std::regex stringifyRegex("#\\s*" + escapedParam + "\\b");
            std::string stringifiedArg = handleStringification(arg);
            result = std::regex_replace(result, stringifyRegex, stringifiedArg);
        } catch (const std::regex_error&) {
            // Se a regex falhar, usar busca simples
            std::string pattern = "# " + param;
            size_t pos = result.find(pattern);
            if (pos != std::string::npos) {
                result.replace(pos, pattern.length(), handleStringification(arg));
            }
        }
    }
    
    // Processar stringificação de __VA_ARGS__
    if (arguments.size() > parameters.size()) {
        std::vector<std::string> variadicArgs(arguments.begin() + parameters.size(), arguments.end());
        std::string variadicExpansion = expandVariadicArguments(variadicArgs);
        
        try {
            std::regex stringifyVariadicRegex("#\\s*__VA_ARGS__\\b");
            std::string stringifiedVariadic = handleStringification(variadicExpansion);
            result = std::regex_replace(result, stringifyVariadicRegex, stringifiedVariadic);
        } catch (const std::regex_error&) {
            // Fallback para busca simples
            std::string pattern = "#__VA_ARGS__";
            size_t pos = result.find(pattern);
            if (pos != std::string::npos) {
                result.replace(pos, pattern.length(), handleStringification(variadicExpansion));
            }
        }
    }
    
    // Processar operador de concatenação (##)
    for (size_t i = 0; i < parameters.size() && i < arguments.size(); ++i) {
        const std::string& param = parameters[i];
        const std::string& arg = arguments[i];
        
        // Escapar caracteres especiais do parâmetro para regex
        std::string escapedParam = std::regex_replace(param, std::regex("[.^$|()\\[\\]{}*+?]"), "\\$&");
        
        try {
            // Procurar por token##param ou param##token
            std::regex concatLeftRegex("(\\w+)\\s*##\\s*" + escapedParam + "\\b");
            std::regex concatRightRegex("\\b" + escapedParam + "\\s*##\\s*(\\w+)");
            
            // Substituir usando busca manual para evitar problemas com lambdas
            std::smatch match;
            if (std::regex_search(result, match, concatLeftRegex)) {
                std::string replacement = handleConcatenation(match[1].str(), arg);
                result = std::regex_replace(result, concatLeftRegex, replacement);
            }
            
            if (std::regex_search(result, match, concatRightRegex)) {
                std::string replacement = handleConcatenation(arg, match[1].str());
                result = std::regex_replace(result, concatRightRegex, replacement);
            }
        } catch (const std::regex_error&) {
            // Fallback para busca simples
            std::string leftPattern = param + "##";
            std::string rightPattern = "##" + param;
            
            size_t pos = result.find(leftPattern);
            if (pos != std::string::npos) {
                result.replace(pos, leftPattern.length(), arg);
            }
            
            pos = result.find(rightPattern);
            if (pos != std::string::npos) {
                result.replace(pos, rightPattern.length(), arg);
            }
        }
    }
    
    // Processar concatenação com __VA_ARGS__
    if (arguments.size() > parameters.size()) {
        std::vector<std::string> variadicArgs(arguments.begin() + parameters.size(), arguments.end());
        std::string variadicExpansion = expandVariadicArguments(variadicArgs);
        
        try {
            std::regex concatVariadicLeftRegex("(\\w+)\\s*##\\s*__VA_ARGS__\\b");
            std::regex concatVariadicRightRegex("\\b__VA_ARGS__\\s*##\\s*(\\w+)");
            
            std::smatch match;
            if (std::regex_search(result, match, concatVariadicLeftRegex)) {
                std::string replacement = handleConcatenation(match[1].str(), variadicExpansion);
                result = std::regex_replace(result, concatVariadicLeftRegex, replacement);
            }
            
            if (std::regex_search(result, match, concatVariadicRightRegex)) {
                std::string replacement = handleConcatenation(variadicExpansion, match[1].str());
                result = std::regex_replace(result, concatVariadicRightRegex, replacement);
            }
        } catch (const std::regex_error&) {
            // Fallback para busca simples
            std::string leftPattern = "__VA_ARGS__##";
            std::string rightPattern = "##__VA_ARGS__";
            
            size_t pos = result.find(leftPattern);
            if (pos != std::string::npos) {
                result.replace(pos, leftPattern.length(), variadicExpansion);
            }
            
            pos = result.find(rightPattern);
            if (pos != std::string::npos) {
                result.replace(pos, rightPattern.length(), variadicExpansion);
            }
        }
    }
    
    return result;
}

std::pair<size_t, std::string> MacroProcessor::findNextMacro(const std::string& text, size_t startPos) {
    size_t pos = startPos;
    
    while (pos < text.length()) {
        // Procura por caractere que pode iniciar um identificador
        if (isValidMacroNameChar(text[pos], true)) {
            size_t nameStart = pos;
            
            // Coleta o nome completo
            while (pos < text.length() && isValidMacroNameChar(text[pos], false)) {
                pos++;
            }
            
            std::string name = text.substr(nameStart, pos - nameStart);
            
            // Verifica se é uma macro definida
            if (isDefined(name)) {
                return std::make_pair(nameStart, name);
            }
        } else {
            pos++;
        }
    }
    
    return std::make_pair(std::string::npos, "");
}

bool MacroProcessor::isInsideStringLiteral(const std::string& text, size_t pos) const {
    bool inString = false;
    bool escaped = false;
    
    for (size_t i = 0; i < pos && i < text.length(); ++i) {
        if (escaped) {
            escaped = false;
            continue;
        }
        
        if (text[i] == '\\') {
            escaped = true;
        } else if (text[i] == '\"') {
            inString = !inString;
        }
    }
    
    return inString;
}

bool MacroProcessor::isInsideComment(const std::string& text, size_t pos) const {
    // Implementação simples - pode ser melhorada
    size_t commentStart = text.find("//");
    if (commentStart != std::string::npos && pos > commentStart) {
        return true;
    }
    
    // Verifica comentários de bloco /* */
    size_t blockStart = text.find("/*");
    if (blockStart != std::string::npos && pos > blockStart) {
        size_t blockEnd = text.find("*/", blockStart);
        if (blockEnd == std::string::npos || pos < blockEnd) {
            return true;
        }
    }
    
    return false;
}

std::string MacroProcessor::generateCacheKey(const std::string& macroName,
                                           const std::vector<std::string>& arguments) const {
    std::ostringstream oss;
    oss << macroName;
    
    if (!arguments.empty()) {
        oss << "(";
        for (size_t i = 0; i < arguments.size(); ++i) {
            if (i > 0) oss << ",";
            oss << arguments[i];
        }
        oss << ")";
    }
    
    return oss.str();
}

void MacroProcessor::logMacroError(const std::string& message,
                                  const Preprocessor::PreprocessorPosition& position) {
    if (logger_) {
        logger_->error(message, position);
    }
}

void MacroProcessor::logMacroWarning(const std::string& message,
                                    const Preprocessor::PreprocessorPosition& position) {
    if (logger_) {
        logger_->warning(message, position);
    }
}

// ============================================================================
// FUNÇÕES UTILITÁRIAS GLOBAIS
// ============================================================================

std::string macroTypeToString(MacroType type) {
    switch (type) {
        case MacroType::OBJECT_LIKE:
            return "Object-like";
        case MacroType::FUNCTION_LIKE:
            return "Function-like";
        case MacroType::VARIADIC:
            return "Variadic";
        default:
            return "Unknown";
    }
}

bool isValidMacroNameChar(char c, bool isFirst) {
    if (isFirst) {
        return std::isalpha(c) || c == '_';
    } else {
        return std::isalnum(c) || c == '_';
    }
}

std::string escapeMacroString(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2); // Reserva espaço extra
    
    for (char c : str) {
        if (c == '\"' || c == '\\') {
            result += '\\';
        }
        result += c;
    }
    
    return result;
}

std::string trimWhitespace(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

} // namespace Preprocessor