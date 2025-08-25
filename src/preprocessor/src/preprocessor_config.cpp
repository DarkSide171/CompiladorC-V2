#include "../include/preprocessor_config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

namespace Preprocessor {

// ============================================================================
// IMPLEMENTAÇÃO DA CLASSE PreprocessorConfig
// ============================================================================

PreprocessorConfig::PreprocessorConfig() : c_version_(CVersion::C99) {
    initializeDefaultConfig();
}

PreprocessorConfig::PreprocessorConfig(CVersion version) : c_version_(version) {
    initializeDefaultConfig();
    setVersion(version);
}

void PreprocessorConfig::initializeDefaultConfig() {
    // Limpar configurações existentes
    include_paths_.clear();
    predefined_macros_.clear();
    config_values_.clear();
    
    // Configurar valores padrão
    debug_mode_ = false;
    enable_warnings_ = true;
    strict_mode_ = false;
    preserve_comments_ = false;
    
    // Configurar limites padrão
    max_macro_expansion_size_ = 1024 * 1024; // 1MB
    max_include_depth_ = 200;
    max_macro_recursion_depth_ = 1000;
    
    // Inicializar features e macros para a versão atual
    initializeVersionFeatures(c_version_);
    loadPredefinedMacrosForVersion(c_version_);
    
    // Adicionar caminhos de inclusão padrão do sistema (apenas se existirem)
    if (validateIncludePath("/usr/include")) {
        include_paths_.push_back("/usr/include");
    }
    if (validateIncludePath("/usr/local/include")) {
        include_paths_.push_back("/usr/local/include");
    }
    // Adicionar caminhos alternativos para macOS
    if (validateIncludePath("/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include")) {
        include_paths_.push_back("/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include");
    }
    
    // Configurações padrão
    config_values_["version"] = cVersionToString(c_version_);
    config_values_["debug"] = debug_mode_ ? "true" : "false";
    config_values_["warnings"] = enable_warnings_ ? "true" : "false";
    config_values_["strict"] = strict_mode_ ? "true" : "false";
}

bool PreprocessorConfig::loadConfiguration(const std::string& filepath) {
    return loadConfigFromFile(filepath);
}

bool PreprocessorConfig::loadConfigFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (!parseConfigLine(line)) {
            file.close();
            return false;
        }
    }
    
    file.close();
    applyConfigChanges();
    return validateConfiguration();
}

bool PreprocessorConfig::validateConfiguration() {
    // Validar versão do C
    if (c_version_ < CVersion::C89 || c_version_ > CVersion::C23) {
        return false;
    }
    
    // Validar caminhos de inclusão
    for (const auto& path : include_paths_) {
        if (!validateIncludePath(path)) {
            return false;
        }
    }
    
    // Validar limites
    if (max_macro_expansion_size_ == 0 || max_include_depth_ <= 0 || max_macro_recursion_depth_ <= 0) {
        return false;
    }
    
    // Validar macros predefinidas
    for (const auto& macro : predefined_macros_) {
        if (macro.first.empty()) {
            return false;
        }
    }
    
    return true;
}

bool PreprocessorConfig::validateConfigValue(const std::string& key, const std::string& value) {
    if (key == "version") {
        try {
            stringToCVersion(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (key == "debug" || key == "warnings" || key == "strict" || key == "preserve_comments") {
        return value == "true" || value == "false";
    } else if (key == "max_macro_expansion_size" || key == "max_include_depth" || key == "max_macro_recursion_depth") {
        try {
            int val = std::stoi(value);
            return val > 0;
        } catch (...) {
            return false;
        }
    }
    
    return validateInternalConfigValue(key, value);
}

bool PreprocessorConfig::parseConfigLine(const std::string& line) {
    // Ignorar linhas vazias e comentários
    std::string trimmed = line;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    
    if (trimmed.empty() || trimmed[0] == '#') {
        return true;
    }
    
    // Procurar por '=' para separar chave e valor
    size_t eq_pos = trimmed.find('=');
    if (eq_pos == std::string::npos) {
        return false;
    }
    
    std::string key = trimmed.substr(0, eq_pos);
    std::string value = trimmed.substr(eq_pos + 1);
    
    // Remover espaços em branco
    key.erase(0, key.find_first_not_of(" \t"));
    key.erase(key.find_last_not_of(" \t") + 1);
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);
    
    if (!validateConfigValue(key, value)) {
        return false;
    }
    
    config_values_[key] = value;
    return true;
}

void PreprocessorConfig::applyConfigChanges() {
    for (const auto& config : config_values_) {
        const std::string& key = config.first;
        const std::string& value = config.second;
        
        if (key == "version") {
            setVersion(stringToCVersion(value));
        } else if (key == "debug") {
            debug_mode_ = (value == "true");
        } else if (key == "warnings") {
            enable_warnings_ = (value == "true");
        } else if (key == "strict") {
            strict_mode_ = (value == "true");
        } else if (key == "preserve_comments") {
            preserve_comments_ = (value == "true");
        } else if (key == "max_macro_expansion_size") {
            max_macro_expansion_size_ = std::stoull(value);
        } else if (key == "max_include_depth") {
            max_include_depth_ = std::stoi(value);
        } else if (key == "max_macro_recursion_depth") {
            max_macro_recursion_depth_ = std::stoi(value);
        }
    }
}

bool PreprocessorConfig::saveConfigToFile(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# Configuração do Pré-processador C\n";
    file << "# Gerado automaticamente\n\n";
    
    file << "version=" << cVersionToString(c_version_) << "\n";
    file << "debug=" << (debug_mode_ ? "true" : "false") << "\n";
    file << "warnings=" << (enable_warnings_ ? "true" : "false") << "\n";
    file << "strict=" << (strict_mode_ ? "true" : "false") << "\n";
    file << "preserve_comments=" << (preserve_comments_ ? "true" : "false") << "\n";
    file << "max_macro_expansion_size=" << max_macro_expansion_size_ << "\n";
    file << "max_include_depth=" << max_include_depth_ << "\n";
    file << "max_macro_recursion_depth=" << max_macro_recursion_depth_ << "\n";
    
    file << "\n# Caminhos de Inclusão\n";
    for (const auto& path : include_paths_) {
        file << "include_path=" << path << "\n";
    }
    
    file << "\n# Macros Predefinidas\n";
    for (const auto& macro : predefined_macros_) {
        file << "define=" << macro.first << "=" << macro.second << "\n";
    }
    
    file.close();
    return true;
}

void PreprocessorConfig::mergeConfigurations(const PreprocessorConfig& other) {
    // Mesclar caminhos de inclusão
    for (const auto& path : other.include_paths_) {
        if (std::find(include_paths_.begin(), include_paths_.end(), path) == include_paths_.end()) {
            include_paths_.push_back(path);
        }
    }
    
    // Mesclar macros predefinidas
    for (const auto& macro : other.predefined_macros_) {
        predefined_macros_[macro.first] = macro.second;
    }
    
    // Mesclar valores de configuração
    for (const auto& config : other.config_values_) {
        config_values_[config.first] = config.second;
    }
}

void PreprocessorConfig::resetToDefaults() {
    c_version_ = CVersion::C99;
    initializeDefaultConfig();
}

bool PreprocessorConfig::validateCStandardCompatibility(CVersion standard) {
    // Verificar se a versão atual é compatível com o padrão solicitado
    return c_version_ >= standard;
}

void PreprocessorConfig::updateFeatureFlags(CVersion standard) {
    setVersion(standard);
}

void PreprocessorConfig::resolveIncludePaths() {
    std::vector<std::string> resolved_paths;
    
    for (const auto& path : include_paths_) {
        std::string normalized = normalizeIncludePath(path);
        if (validateIncludePath(normalized)) {
            resolved_paths.push_back(normalized);
        }
    }
    
    include_paths_ = resolved_paths;
}

bool PreprocessorConfig::validateIncludePath(const std::string& path) {
    if (path.empty()) {
        return false;
    }
    
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) != 0) {
        return false;
    }
    
    return S_ISDIR(path_stat.st_mode);
}

std::string PreprocessorConfig::normalizeIncludePath(const std::string& path) {
    // Implementação simples de normalização de caminho
    std::string normalized = path;
    
    // Remover barras duplas
    size_t pos = 0;
    while ((pos = normalized.find("//", pos)) != std::string::npos) {
        normalized.replace(pos, 2, "/");
    }
    
    // Remover barra final se não for raiz
    if (normalized.length() > 1 && normalized.back() == '/') {
        normalized.pop_back();
    }
    
    return normalized;
}

void PreprocessorConfig::handleEnvironmentVariables() {
    // Processar variável C_INCLUDE_PATH
    const char* c_include_path = std::getenv("C_INCLUDE_PATH");
    if (c_include_path) {
        std::string paths(c_include_path);
        std::stringstream ss(paths);
        std::string path;
        
        while (std::getline(ss, path, ':')) {
            if (!path.empty() && validateIncludePath(path)) {
                addIncludePath(path);
            }
        }
    }
    
    // Processar variável CPATH
    const char* cpath = std::getenv("CPATH");
    if (cpath) {
        std::string paths(cpath);
        std::stringstream ss(paths);
        std::string path;
        
        while (std::getline(ss, path, ':')) {
            if (!path.empty() && validateIncludePath(path)) {
                addIncludePath(path);
            }
        }
    }
}

std::string PreprocessorConfig::generateConfigReport() const {
    std::stringstream report;
    
    report << "=== Relatório de Configuração do Pré-processador ===\n\n";
    
    report << "Versão do C: " << cVersionToString(c_version_) << "\n";
    report << "Modo Debug: " << (debug_mode_ ? "Ativo" : "Inativo") << "\n";
    report << "Warnings: " << (enable_warnings_ ? "Habilitados" : "Desabilitados") << "\n";
    report << "Modo Estrito: " << (strict_mode_ ? "Ativo" : "Inativo") << "\n";
    report << "Preservar Comentários: " << (preserve_comments_ ? "Sim" : "Não") << "\n\n";
    
    report << "Limites:\n";
    report << "  Tamanho máximo de expansão de macro: " << max_macro_expansion_size_ << " bytes\n";
    report << "  Profundidade máxima de inclusão: " << max_include_depth_ << "\n";
    report << "  Profundidade máxima de recursão de macro: " << max_macro_recursion_depth_ << "\n\n";
    
    report << "Features Suportadas:\n";
    report << "  Macros variádicas: " << (version_features_.supports_variadic_macros ? "Sim" : "Não") << "\n";
    report << "  #pragma once: " << (version_features_.supports_pragma_once ? "Sim" : "Não") << "\n";
    report << "  _Generic: " << (version_features_.supports_generic_selection ? "Sim" : "Não") << "\n";
    report << "  _Static_assert: " << (version_features_.supports_static_assert ? "Sim" : "Não") << "\n";
    report << "  _Alignof: " << (version_features_.supports_alignof ? "Sim" : "Não") << "\n";
    report << "  _Thread_local: " << (version_features_.supports_thread_local ? "Sim" : "Não") << "\n";
    report << "  Literais Unicode: " << (version_features_.supports_unicode_literals ? "Sim" : "Não") << "\n";
    report << "  typeof: " << (version_features_.supports_typeof ? "Sim" : "Não") << "\n";
    report << "  Floats decimais: " << (version_features_.supports_decimal_floats ? "Sim" : "Não") << "\n\n";
    
    report << "Caminhos de Inclusão (" << include_paths_.size() << "):\n";
    for (const auto& path : include_paths_) {
        report << "  " << path << "\n";
    }
    
    report << "\nMacros Predefinidas (" << predefined_macros_.size() << "):\n";
    for (const auto& macro : predefined_macros_) {
        report << "  " << macro.first << " = " << macro.second << "\n";
    }
    
    return report.str();
}

void PreprocessorConfig::setVersion(CVersion version) {
    c_version_ = version;
    initializeVersionFeatures(version);
    loadPredefinedMacrosForVersion(version);
    config_values_["version"] = cVersionToString(version);
}

void PreprocessorConfig::addIncludePath(const std::string& path) {
    std::string normalized = normalizeIncludePath(path);
    if (std::find(include_paths_.begin(), include_paths_.end(), normalized) == include_paths_.end()) {
        include_paths_.push_back(normalized);
    }
}

void PreprocessorConfig::removeIncludePath(const std::string& path) {
    std::string normalized = normalizeIncludePath(path);
    include_paths_.erase(
        std::remove(include_paths_.begin(), include_paths_.end(), normalized),
        include_paths_.end()
    );
}

void PreprocessorConfig::clearIncludePaths() {
    include_paths_.clear();
}

void PreprocessorConfig::addPredefinedMacro(const std::string& name, const std::string& value) {
    predefined_macros_[name] = value;
}

void PreprocessorConfig::removePredefinedMacro(const std::string& name) {
    predefined_macros_.erase(name);
}

void PreprocessorConfig::initializeVersionFeatures(CVersion version) {
    // Resetar todas as features
    version_features_ = VersionFeatures{};
    
    switch (version) {
        case CVersion::C89:
            // C89 não suporta features modernas
            break;
            
        case CVersion::C99:
            version_features_.supports_variadic_macros = true;
            break;
            
        case CVersion::C11:
            version_features_.supports_variadic_macros = true;
            version_features_.supports_pragma_once = true;
            version_features_.supports_generic_selection = true;
            version_features_.supports_static_assert = true;
            version_features_.supports_alignof = true;
            version_features_.supports_thread_local = true;
            version_features_.supports_unicode_literals = true;
            break;
            
        case CVersion::C17:
            // C17 mantém as mesmas features do C11
            version_features_.supports_variadic_macros = true;
            version_features_.supports_pragma_once = true;
            version_features_.supports_generic_selection = true;
            version_features_.supports_static_assert = true;
            version_features_.supports_alignof = true;
            version_features_.supports_thread_local = true;
            version_features_.supports_unicode_literals = true;
            break;
            
        case CVersion::C23:
            // C23 inclui todas as features anteriores mais novas
            version_features_.supports_variadic_macros = true;
            version_features_.supports_pragma_once = true;
            version_features_.supports_generic_selection = true;
            version_features_.supports_static_assert = true;
            version_features_.supports_alignof = true;
            version_features_.supports_thread_local = true;
            version_features_.supports_unicode_literals = true;
            version_features_.supports_typeof = true;
            version_features_.supports_decimal_floats = true;
            break;
    }
}

void PreprocessorConfig::loadPredefinedMacrosForVersion(CVersion version) {
    // Limpar macros existentes
    predefined_macros_.clear();
    
    // Macros comuns a todas as versões
    predefined_macros_["__STDC__"] = "1";
    
    switch (version) {
        case CVersion::C89:
            predefined_macros_["__STDC_VERSION__"] = "199409L";
            break;
            
        case CVersion::C99:
            predefined_macros_["__STDC_VERSION__"] = "199901L";
            predefined_macros_["__STDC_HOSTED__"] = "1";
            predefined_macros_["__STDC_IEC_559__"] = "1";
            break;
            
        case CVersion::C11:
            predefined_macros_["__STDC_VERSION__"] = "201112L";
            predefined_macros_["__STDC_HOSTED__"] = "1";
            predefined_macros_["__STDC_IEC_559__"] = "1";
            predefined_macros_["__STDC_NO_ATOMICS__"] = "1";
            predefined_macros_["__STDC_NO_COMPLEX__"] = "1";
            predefined_macros_["__STDC_NO_THREADS__"] = "1";
            predefined_macros_["__STDC_NO_VLA__"] = "1";
            break;
            
        case CVersion::C17:
            predefined_macros_["__STDC_VERSION__"] = "201710L";
            predefined_macros_["__STDC_HOSTED__"] = "1";
            predefined_macros_["__STDC_IEC_559__"] = "1";
            predefined_macros_["__STDC_NO_ATOMICS__"] = "1";
            predefined_macros_["__STDC_NO_COMPLEX__"] = "1";
            predefined_macros_["__STDC_NO_THREADS__"] = "1";
            predefined_macros_["__STDC_NO_VLA__"] = "1";
            break;
            
        case CVersion::C23:
            predefined_macros_["__STDC_VERSION__"] = "202311L";
            predefined_macros_["__STDC_HOSTED__"] = "1";
            predefined_macros_["__STDC_IEC_559__"] = "1";
            predefined_macros_["__STDC_NO_ATOMICS__"] = "1";
            predefined_macros_["__STDC_NO_COMPLEX__"] = "1";
            predefined_macros_["__STDC_NO_THREADS__"] = "1";
            predefined_macros_["__STDC_NO_VLA__"] = "1";
            break;
    }
    
    // Macros específicas do compilador/plataforma
    predefined_macros_["__FILE__"] = "\"<current_file>\"";
    predefined_macros_["__LINE__"] = "1";
    predefined_macros_["__DATE__"] = "\"Jan  1 2024\"";
    predefined_macros_["__TIME__"] = "\"00:00:00\"";
}

bool PreprocessorConfig::validateInternalConfigValue(const std::string& key, const std::string& value) {
    if (key.compare(0, 12, "include_path") == 0) {
        return validateIncludePath(value);
    } else if (key.compare(0, 6, "define") == 0) {
        // Validar formato de macro (nome=valor)
        return !value.empty();
    }
    
    return true;
}

// ============================================================================
// FUNÇÕES UTILITÁRIAS
// ============================================================================

std::string cVersionToString(CVersion version) {
    switch (version) {
        case CVersion::C89: return "C89";
        case CVersion::C99: return "C99";
        case CVersion::C11: return "C11";
        case CVersion::C17: return "C17";
        case CVersion::C23: return "C23";
        default: return "Unknown";
    }
}

CVersion stringToCVersion(const std::string& version_str) {
    if (version_str == "C89" || version_str == "c89" || version_str == "C90" || version_str == "c90") return CVersion::C89;
    if (version_str == "C99" || version_str == "c99") return CVersion::C99;
    if (version_str == "C11" || version_str == "c11") return CVersion::C11;
    if (version_str == "C17" || version_str == "c17" || version_str == "C18" || version_str == "c18") return CVersion::C17;
    if (version_str == "C23" || version_str == "c23") return CVersion::C23;
    
    throw std::invalid_argument("Versão do C inválida: " + version_str);
}

bool versionSupportsFeature(CVersion version, const std::string& feature) {
    PreprocessorConfig config(version);
    const VersionFeatures& features = config.getVersionFeatures();
    
    if (feature == "variadic_macros") return features.supports_variadic_macros;
    if (feature == "pragma_once") return features.supports_pragma_once;
    if (feature == "generic_selection") return features.supports_generic_selection;
    if (feature == "static_assert") return features.supports_static_assert;
    if (feature == "alignof") return features.supports_alignof;
    if (feature == "thread_local") return features.supports_thread_local;
    if (feature == "unicode_literals") return features.supports_unicode_literals;
    if (feature == "typeof") return features.supports_typeof;
    if (feature == "decimal_floats") return features.supports_decimal_floats;
    
    return false;
}

std::vector<std::string> getSupportedFeatures(CVersion version) {
    std::vector<std::string> features;
    PreprocessorConfig config(version);
    const VersionFeatures& version_features = config.getVersionFeatures();
    
    if (version_features.supports_variadic_macros) features.push_back("variadic_macros");
    if (version_features.supports_pragma_once) features.push_back("pragma_once");
    if (version_features.supports_generic_selection) features.push_back("generic_selection");
    if (version_features.supports_static_assert) features.push_back("static_assert");
    if (version_features.supports_alignof) features.push_back("alignof");
    if (version_features.supports_thread_local) features.push_back("thread_local");
    if (version_features.supports_unicode_literals) features.push_back("unicode_literals");
    if (version_features.supports_typeof) features.push_back("typeof");
    if (version_features.supports_decimal_floats) features.push_back("decimal_floats");
    
    return features;
}

} // namespace Preprocessor