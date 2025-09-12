#include "../include/parser_config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace Parser {

ParserConfig::ParserConfig() {
    initializeDefaults();
}

void ParserConfig::initializeDefaults() {
    // Set default values
    cStandard = CStandard::C17;
    parserFlags = ParserFlags::ENABLE_RECOVERY;
    maxErrorCount = 100;
    maxASTNodes = 1000000;
    maxParseDepth = 1000;
    tokenCacheSize = 1024;
    
    // Initialize enabled features for C17 using lexer's system
    enabledFeatures = getDefaultFeatures(Lexer::CVersion::C17);
    
    // Default file paths (relative to project root)
    grammarPath = "src/parser/data/grammar/";
    precedencePath = "src/parser/data/precedence.txt";
    errorMessagesPath = "src/parser/data/error_messages.txt";
    
    // Standard include paths
    includePaths.clear();
    includePaths.push_back("/usr/include");
    includePaths.push_back("/usr/local/include");
    
    // Standard defines
    defines.clear();
    defines["__STDC__"] = "1";
    defines["__STDC_VERSION__"] = "201710L"; // C17
}

void ParserConfig::setCStandard(CStandard standard) {
    cStandard = standard;
    updateLanguageFeaturesForStandard();
    
    // Update __STDC_VERSION__ define based on standard
    switch (standard) {
        case CStandard::C89:
            defines["__STDC_VERSION__"] = "199409L";
            break;
        case CStandard::C99:
            defines["__STDC_VERSION__"] = "199901L";
            break;
        case CStandard::C11:
            defines["__STDC_VERSION__"] = "201112L";
            break;
        case CStandard::C17:
            defines["__STDC_VERSION__"] = "201710L";
            break;
        case CStandard::C23:
            defines["__STDC_VERSION__"] = "202311L";
            break;
    }
}

void ParserConfig::updateLanguageFeaturesForStandard() {
    // Convert CStandard to Lexer::CVersion
    Lexer::CVersion lexerVersion;
    switch (cStandard) {
        case CStandard::C89: lexerVersion = Lexer::CVersion::C89; break;
        case CStandard::C99: lexerVersion = Lexer::CVersion::C99; break;
        case CStandard::C11: lexerVersion = Lexer::CVersion::C11; break;
        case CStandard::C17: lexerVersion = Lexer::CVersion::C17; break;
        case CStandard::C23: lexerVersion = Lexer::CVersion::C23; break;
    }
    enabledFeatures = getDefaultFeatures(lexerVersion);
}

void ParserConfig::enableC99Features() {
    setCStandard(CStandard::C99);
}

void ParserConfig::enableC11Features() {
    setCStandard(CStandard::C11);
}

void ParserConfig::enableC17Features() {
    setCStandard(CStandard::C17);
}

void ParserConfig::enableC23Features() {
    setCStandard(CStandard::C23);
}

bool ParserConfig::isFeatureEnabled(Lexer::Feature feature) const {
    return enabledFeatures.find(feature) != enabledFeatures.end();
}

void ParserConfig::enableFeature(Lexer::Feature feature) {
    enabledFeatures.insert(feature);
}

void ParserConfig::disableFeature(Lexer::Feature feature) {
    enabledFeatures.erase(feature);
}

bool ParserConfig::isCompatibleWithStandard(CStandard standard) const {
    // Convert CStandard to Lexer::CVersion
    Lexer::CVersion lexerVersion;
    switch (standard) {
        case CStandard::C89: lexerVersion = Lexer::CVersion::C89; break;
        case CStandard::C99: lexerVersion = Lexer::CVersion::C99; break;
        case CStandard::C11: lexerVersion = Lexer::CVersion::C11; break;
        case CStandard::C17: lexerVersion = Lexer::CVersion::C17; break;
        case CStandard::C23: lexerVersion = Lexer::CVersion::C23; break;
    }
    
    for (const auto& feature : enabledFeatures) {
        if (!isFeatureAvailableInVersion(feature, lexerVersion)) {
            return false;
        }
    }
    return true;
}

std::vector<Lexer::Feature> ParserConfig::getIncompatibleFeatures(CStandard standard) const {
    // Convert CStandard to Lexer::CVersion
    Lexer::CVersion lexerVersion;
    switch (standard) {
        case CStandard::C89: lexerVersion = Lexer::CVersion::C89; break;
        case CStandard::C99: lexerVersion = Lexer::CVersion::C99; break;
        case CStandard::C11: lexerVersion = Lexer::CVersion::C11; break;
        case CStandard::C17: lexerVersion = Lexer::CVersion::C17; break;
        case CStandard::C23: lexerVersion = Lexer::CVersion::C23; break;
    }
    
    std::vector<Lexer::Feature> incompatible;
    for (const auto& feature : enabledFeatures) {
        if (!isFeatureAvailableInVersion(feature, lexerVersion)) {
            incompatible.push_back(feature);
        }
    }
    return incompatible;
}

void ParserConfig::validateFeatureCompatibility() const {
    auto incompatible = getIncompatibleFeatures(cStandard);
    if (!incompatible.empty()) {
        std::string message = "Incompatible features for C standard: ";
        for (size_t i = 0; i < incompatible.size(); ++i) {
            if (i > 0) message += ", ";
            message += featureToString(incompatible[i]);
        }
        throw std::runtime_error(message);
    }
}

bool ParserConfig::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> ParserConfig::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Check basic constraints
    if (maxErrorCount == 0) {
        errors.push_back("Maximum error count cannot be zero");
    }
    
    if (maxASTNodes == 0) {
        errors.push_back("Maximum AST nodes cannot be zero");
    }
    
    if (maxParseDepth == 0) {
        errors.push_back("Maximum parse depth cannot be zero");
    }
    
    if (tokenCacheSize == 0) {
        errors.push_back("Token cache size cannot be zero");
    }
    
    // Check file paths exist (basic validation)
    if (grammarPath.empty()) {
        errors.push_back("Grammar path cannot be empty");
    }
    
    if (precedencePath.empty()) {
        errors.push_back("Precedence path cannot be empty");
    }
    
    if (errorMessagesPath.empty()) {
        errors.push_back("Error messages path cannot be empty");
    }
    
    // Validate reasonable limits
    if (maxErrorCount > 10000) {
        errors.push_back("Maximum error count is too large (> 10000)");
    }
    
    if (maxASTNodes > 100000000) {
        errors.push_back("Maximum AST nodes is too large (> 100M)");
    }
    
    if (maxParseDepth > 10000) {
        errors.push_back("Maximum parse depth is too large (> 10000)");
    }
    
    return errors;
}

ParserConfig ParserConfig::createDefault() {
    ParserConfig config;
    return config;
}

ParserConfig ParserConfig::createStrict() {
    ParserConfig config;
    config.setStrictMode(true);
    config.setGnuExtensions(false);
    config.setMsExtensions(false);
    config.setRecoveryEnabled(false);
    config.setMaxErrors(1); // Stop at first error
    return config;
}

ParserConfig ParserConfig::createPermissive() {
    ParserConfig config;
    config.setStrictMode(false);
    config.setGnuExtensions(true);
    config.setMsExtensions(true);
    config.setRecoveryEnabled(true);
    config.setVerboseErrors(false);
    config.setMaxErrors(1000);
    return config;
}

ParserConfig ParserConfig::createForStandard(CStandard standard) {
    ParserConfig config;
    config.setCStandard(standard);
    
    // Update standard-specific defines
    config.defines.clear();
    config.defines["__STDC__"] = "1";
    
    switch (standard) {
        case CStandard::C89:
            config.defines["__STDC_VERSION__"] = "199409L";
            config.setGnuExtensions(false);
            config.setMsExtensions(false);
            break;
            
        case CStandard::C99:
            config.defines["__STDC_VERSION__"] = "199901L";
            config.defines["__STDC_IEC_559__"] = "1";
            config.defines["__STDC_IEC_559_COMPLEX__"] = "1";
            config.defines["__STDC_ISO_10646__"] = "200009L";
            break;
            
        case CStandard::C11:
            config.defines["__STDC_VERSION__"] = "201112L";
            config.defines["__STDC_NO_ATOMICS__"] = "1";
            config.defines["__STDC_NO_COMPLEX__"] = "1";
            config.defines["__STDC_NO_THREADS__"] = "1";
            config.defines["__STDC_NO_VLA__"] = "1";
            break;
            
        case CStandard::C17:
            config.defines["__STDC_VERSION__"] = "201710L";
            break;
            
        case CStandard::C23:
            config.defines["__STDC_VERSION__"] = "202311L";
            break;
    }
    
    // Set grammar path based on standard
    std::string grammarFile;
    switch (standard) {
        case CStandard::C89: grammarFile = "simple_c.grammar"; break;
        case CStandard::C99: grammarFile = "c99.grammar"; break;
        case CStandard::C11: grammarFile = "c11.grammar"; break;
        case CStandard::C17: grammarFile = "c17.grammar"; break;
        case CStandard::C23: grammarFile = "c23.grammar"; break;
    }
    
    config.setGrammarPath(config.getGrammarPath() + grammarFile);
    
    return config;
}

bool ParserConfig::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse key=value pairs
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Process configuration options
        if (key == "standard") {
            if (value == "C89") setCStandard(CStandard::C89);
            else if (value == "C99") setCStandard(CStandard::C99);
            else if (value == "C11") setCStandard(CStandard::C11);
            else if (value == "C17") setCStandard(CStandard::C17);
            else if (value == "C23") setCStandard(CStandard::C23);
        }
        else if (key == "strict_mode") {
            setStrictMode(value == "true" || value == "1");
        }
        else if (key == "gnu_extensions") {
            setGnuExtensions(value == "true" || value == "1");
        }
        else if (key == "ms_extensions") {
            setMsExtensions(value == "true" || value == "1");
        }
        else if (key == "recovery_enabled") {
            setRecoveryEnabled(value == "true" || value == "1");
        }
        else if (key == "verbose_errors") {
            setVerboseErrors(value == "true" || value == "1");
        }
        else if (key == "max_errors") {
            setMaxErrors(std::stoul(value));
        }
        else if (key == "grammar_path") {
            setGrammarPath(value);
        }
        else if (key == "precedence_path") {
            setPrecedencePath(value);
        }
        else if (key == "error_messages_path") {
            setErrorMessagesPath(value);
        }
        else if (key.substr(0, 7) == "define_") {
            std::string defineName = key.substr(7);
            addDefine(defineName, value);
        }
        else if (key == "include_path") {
            addIncludePath(value);
        }
    }
    
    return true;
}

bool ParserConfig::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# Parser Configuration File\n";
    file << "# Generated automatically\n\n";
    
    // Standard
    file << "standard=";
    switch (cStandard) {
        case CStandard::C89: file << "C89"; break;
        case CStandard::C99: file << "C99"; break;
        case CStandard::C11: file << "C11"; break;
        case CStandard::C17: file << "C17"; break;
        case CStandard::C23: file << "C23"; break;
    }
    file << "\n";
    
    // Flags
    file << "strict_mode=" << (isStrictMode() ? "true" : "false") << "\n";
    file << "gnu_extensions=" << (isGnuExtensionsEnabled() ? "true" : "false") << "\n";
    file << "ms_extensions=" << (isMsExtensionsEnabled() ? "true" : "false") << "\n";
    file << "recovery_enabled=" << (isRecoveryEnabled() ? "true" : "false") << "\n";
    file << "verbose_errors=" << (isVerboseErrors() ? "true" : "false") << "\n";
    
    // Limits
    file << "max_errors=" << maxErrorCount << "\n";
    
    // Paths
    file << "grammar_path=" << grammarPath << "\n";
    file << "precedence_path=" << precedencePath << "\n";
    file << "error_messages_path=" << errorMessagesPath << "\n";
    
    // Defines
    file << "\n# Preprocessor defines\n";
    for (const auto& define : defines) {
        file << "define_" << define.first << "=" << define.second << "\n";
    }
    
    // Include paths
    file << "\n# Include paths\n";
    for (const auto& path : includePaths) {
        file << "include_path=" << path << "\n";
    }
    
    return true;
}

} // namespace Parser