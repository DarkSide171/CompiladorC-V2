#include "../include/directive.hpp"
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <cctype>

namespace Preprocessor {

// ============================================================================
// Implementação da classe Directive
// ============================================================================

Directive::Directive(DirectiveType type, const std::string& content, const PreprocessorPosition& pos)
    : type_(type), content_(content), position_(pos), valid_(false) {
    // Validar a diretiva durante a construção
    valid_ = validateSyntax() && validateArguments();
}

std::string Directive::toString() const {
    std::ostringstream oss;
    oss << "Directive{";
    oss << "type=" << directiveTypeToString(type_);
    oss << ", content=\"" << content_ << "\"";
    oss << ", position=(" << position_.original_line << "," << position_.original_column << ")";
    oss << ", file=\"" << position_.filename << "\"";
    oss << ", valid=" << (valid_ ? "true" : "false");
    oss << ", args=[";
    for (size_t i = 0; i < arguments_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << arguments_[i] << "\"";
    }
    oss << "]}";
    return oss.str();
}

bool Directive::validateSyntax() const {
    // Validação básica de sintaxe
    if (content_.empty()) {
        return false;
    }
    
    // Deve começar com #
    if (content_[0] != '#') {
        return false;
    }
    
    // Extrair nome da diretiva
    size_t start = 1;
    while (start < content_.length() && std::isspace(content_[start])) {
        start++;
    }
    
    if (start >= content_.length()) {
        return false; // Apenas # sem nome
    }
    
    size_t end = start;
    while (end < content_.length() && 
           (std::isalnum(content_[end]) || content_[end] == '_')) {
        end++;
    }
    
    if (end == start) {
        return false; // Nome vazio
    }
    
    std::string directive_name = content_.substr(start, end - start);
    
    // Verificar se é um tipo conhecido
    DirectiveType detected_type = stringToDirectiveType(directive_name);
    if (detected_type == DirectiveType::UNKNOWN && type_ != DirectiveType::UNKNOWN) {
        return false;
    }
    
    return true;
}

bool Directive::validateArguments() const {
    // Validação específica por tipo de diretiva
    switch (type_) {
        case DirectiveType::INCLUDE:
            // #include deve ter exatamente um argumento
            return arguments_.size() == 1 && !arguments_[0].empty();
            
        case DirectiveType::DEFINE:
            // #define deve ter pelo menos um argumento (nome da macro)
            return arguments_.size() >= 1 && !arguments_[0].empty();
            
        case DirectiveType::UNDEF:
            // #undef deve ter exatamente um argumento
            return arguments_.size() == 1 && !arguments_[0].empty();
            
        case DirectiveType::IF:
        case DirectiveType::ELIF:
            // #if e #elif devem ter pelo menos um argumento (expressão)
            return arguments_.size() >= 1;
            
        case DirectiveType::IFDEF:
        case DirectiveType::IFNDEF:
            // #ifdef e #ifndef devem ter exatamente um argumento
            return arguments_.size() == 1 && !arguments_[0].empty();
            
        case DirectiveType::ELSE:
        case DirectiveType::ENDIF:
            // #else e #endif não devem ter argumentos
            return arguments_.empty();
            
        case DirectiveType::ERROR:
        case DirectiveType::WARNING:
            // #error e #warning podem ter argumentos opcionais
            return true;
            
        case DirectiveType::PRAGMA:
            // #pragma deve ter pelo menos um argumento
            return arguments_.size() >= 1;
            
        case DirectiveType::LINE:
            // #line deve ter pelo menos um argumento (número da linha)
            return arguments_.size() >= 1 && !arguments_[0].empty();
            
        case DirectiveType::UNKNOWN:
            // Diretivas desconhecidas são consideradas inválidas
            return false;
            
        default:
            return false;
    }
}

void Directive::setArguments(const std::vector<std::string>& args) {
    arguments_ = args;
    // Revalidar após definir argumentos
    valid_ = validateSyntax() && validateArguments();
}

// ============================================================================
// Funções utilitárias
// ============================================================================

std::string directiveTypeToString(DirectiveType type) {
    static const std::unordered_map<DirectiveType, std::string> type_names = {
        {DirectiveType::INCLUDE, "include"},
        {DirectiveType::DEFINE, "define"},
        {DirectiveType::UNDEF, "undef"},
        {DirectiveType::IF, "if"},
        {DirectiveType::IFDEF, "ifdef"},
        {DirectiveType::IFNDEF, "ifndef"},
        {DirectiveType::ELSE, "else"},
        {DirectiveType::ELIF, "elif"},
        {DirectiveType::ENDIF, "endif"},
        {DirectiveType::ERROR, "error"},
        {DirectiveType::WARNING, "warning"},
        {DirectiveType::PRAGMA, "pragma"},
        {DirectiveType::LINE, "line"},
        {DirectiveType::UNKNOWN, "unknown"}
    };
    
    auto it = type_names.find(type);
    return (it != type_names.end()) ? it->second : "unknown";
}

DirectiveType stringToDirectiveType(const std::string& str) {
    static const std::unordered_map<std::string, DirectiveType> string_to_type = {
        {"include", DirectiveType::INCLUDE},
        {"define", DirectiveType::DEFINE},
        {"undef", DirectiveType::UNDEF},
        {"if", DirectiveType::IF},
        {"ifdef", DirectiveType::IFDEF},
        {"ifndef", DirectiveType::IFNDEF},
        {"else", DirectiveType::ELSE},
        {"elif", DirectiveType::ELIF},
        {"endif", DirectiveType::ENDIF},
        {"error", DirectiveType::ERROR},
        {"warning", DirectiveType::WARNING},
        {"pragma", DirectiveType::PRAGMA},
        {"line", DirectiveType::LINE}
    };
    
    // Converter para minúsculas para comparação case-insensitive
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    
    auto it = string_to_type.find(lower_str);
    return (it != string_to_type.end()) ? it->second : DirectiveType::UNKNOWN;
}

bool isConditionalDirective(DirectiveType type) {
    switch (type) {
        case DirectiveType::IF:
        case DirectiveType::IFDEF:
        case DirectiveType::IFNDEF:
        case DirectiveType::ELSE:
        case DirectiveType::ELIF:
        case DirectiveType::ENDIF:
            return true;
        default:
            return false;
    }
}

bool requiresArguments(DirectiveType type) {
    switch (type) {
        case DirectiveType::INCLUDE:
        case DirectiveType::DEFINE:
        case DirectiveType::UNDEF:
        case DirectiveType::IF:
        case DirectiveType::IFDEF:
        case DirectiveType::IFNDEF:
        case DirectiveType::ELIF:
        case DirectiveType::PRAGMA:
        case DirectiveType::LINE:
            return true;
            
        case DirectiveType::ELSE:
        case DirectiveType::ENDIF:
            return false;
            
        case DirectiveType::ERROR:
        case DirectiveType::WARNING:
            return false; // Argumentos são opcionais
            
        case DirectiveType::UNKNOWN:
        default:
            return false;
    }
}

} // namespace Preprocessor