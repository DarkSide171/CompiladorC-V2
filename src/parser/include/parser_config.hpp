#ifndef PARSER_CONFIG_HPP
#define PARSER_CONFIG_HPP

#include "parser_types.hpp"
#include "../../lexer/include/lexer_config.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Parser {

// Parser configuration class
class ParserConfig {
public:
    ParserConfig();
    ~ParserConfig() = default;
    
    // Standard version
    void setCStandard(CStandard standard);
    CStandard getCStandard() const { return cStandard; }
    
    // C version-specific feature enablers
    void enableC99Features();
    void enableC11Features();
    void enableC17Features();
    void enableC23Features();
    
    // Language features management (using lexer's feature system)
    std::unordered_set<Lexer::Feature> getEnabledFeatures() const { return enabledFeatures; }
    void setEnabledFeatures(const std::unordered_set<Lexer::Feature>& features) { enabledFeatures = features; }
    bool isFeatureEnabled(Lexer::Feature feature) const;
    void enableFeature(Lexer::Feature feature);
    void disableFeature(Lexer::Feature feature);
    
    // Version compatibility checking
    bool isCompatibleWithStandard(CStandard standard) const;
    std::vector<Lexer::Feature> getIncompatibleFeatures(CStandard standard) const;
    void validateFeatureCompatibility() const;
    
    // Parser flags
    void setFlags(ParserFlags flags) { parserFlags = flags; }
    ParserFlags getFlags() const { return parserFlags; }
    void addFlag(ParserFlags flag) {
        parserFlags = static_cast<ParserFlags>(static_cast<uint32_t>(parserFlags) | static_cast<uint32_t>(flag));
    }
    void removeFlag(ParserFlags flag) {
        parserFlags = static_cast<ParserFlags>(static_cast<uint32_t>(parserFlags) & ~static_cast<uint32_t>(flag));
    }
    bool hasFlag(ParserFlags flag) const {
        return (static_cast<uint32_t>(parserFlags) & static_cast<uint32_t>(flag)) != 0;
    }
    
    // Error recovery settings
    void setMaxErrors(size_t maxErrors) { maxErrorCount = maxErrors; }
    size_t getMaxErrors() const { return maxErrorCount; }
    
    void setRecoveryEnabled(bool enabled) {
        if (enabled) {
            addFlag(ParserFlags::ENABLE_RECOVERY);
        } else {
            removeFlag(ParserFlags::ENABLE_RECOVERY);
        }
    }
    bool isRecoveryEnabled() const { return hasFlag(ParserFlags::ENABLE_RECOVERY); }
    
    // Verbosity settings
    void setVerboseErrors(bool verbose) {
        if (verbose) {
            addFlag(ParserFlags::VERBOSE_ERRORS);
        } else {
            removeFlag(ParserFlags::VERBOSE_ERRORS);
        }
    }
    bool isVerboseErrors() const { return hasFlag(ParserFlags::VERBOSE_ERRORS); }
    
    // Extension support
    void setGnuExtensions(bool enabled) {
        if (enabled) {
            addFlag(ParserFlags::ALLOW_GNU_EXTENSIONS);
        } else {
            removeFlag(ParserFlags::ALLOW_GNU_EXTENSIONS);
        }
    }
    bool isGnuExtensionsEnabled() const { return hasFlag(ParserFlags::ALLOW_GNU_EXTENSIONS); }
    
    void setMsExtensions(bool enabled) {
        if (enabled) {
            addFlag(ParserFlags::ALLOW_MS_EXTENSIONS);
        } else {
            removeFlag(ParserFlags::ALLOW_MS_EXTENSIONS);
        }
    }
    bool isMsExtensionsEnabled() const { return hasFlag(ParserFlags::ALLOW_MS_EXTENSIONS); }
    
    // Strict mode
    void setStrictMode(bool strict) {
        if (strict) {
            addFlag(ParserFlags::STRICT_MODE);
        } else {
            removeFlag(ParserFlags::STRICT_MODE);
        }
    }
    bool isStrictMode() const { return hasFlag(ParserFlags::STRICT_MODE); }
    
    // Grammar file paths
    void setGrammarPath(const std::string& path) { grammarPath = path; }
    const std::string& getGrammarPath() const { return grammarPath; }
    
    void setPrecedencePath(const std::string& path) { precedencePath = path; }
    const std::string& getPrecedencePath() const { return precedencePath; }
    
    void setErrorMessagesPath(const std::string& path) { errorMessagesPath = path; }
    const std::string& getErrorMessagesPath() const { return errorMessagesPath; }
    
    // Custom defines and macros
    void addDefine(const std::string& name, const std::string& value = "") {
        defines[name] = value;
    }
    void removeDefine(const std::string& name) {
        defines.erase(name);
    }
    bool hasDefine(const std::string& name) const {
        return defines.find(name) != defines.end();
    }
    const std::string& getDefine(const std::string& name) const {
        static const std::string empty;
        auto it = defines.find(name);
        return it != defines.end() ? it->second : empty;
    }
    const std::unordered_map<std::string, std::string>& getDefines() const {
        return defines;
    }
    
    // Include paths
    void addIncludePath(const std::string& path) {
        includePaths.push_back(path);
    }
    void clearIncludePaths() {
        includePaths.clear();
    }
    const std::vector<std::string>& getIncludePaths() const {
        return includePaths;
    }
    
    // Memory settings
    void setMaxASTNodes(size_t maxNodes) { maxASTNodes = maxNodes; }
    size_t getMaxASTNodes() const { return maxASTNodes; }
    
    void setMaxParseDepth(size_t maxDepth) { maxParseDepth = maxDepth; }
    size_t getMaxParseDepth() const { return maxParseDepth; }
    
    // Performance settings
    void setTokenCacheSize(size_t cacheSize) { tokenCacheSize = cacheSize; }
    size_t getTokenCacheSize() const { return tokenCacheSize; }
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Preset configurations
    static ParserConfig createDefault();
    static ParserConfig createStrict();
    static ParserConfig createPermissive();
    static ParserConfig createForStandard(CStandard standard);
    
    // Configuration serialization
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
private:
    // Configuration data
    CStandard cStandard;
    ParserFlags parserFlags;
    std::unordered_set<Lexer::Feature> enabledFeatures;
    
    // Error handling
    size_t maxErrorCount;
    
    // File paths
    std::string grammarPath;
    std::string precedencePath;
    std::string errorMessagesPath;
    
    // Preprocessor defines and includes
    std::unordered_map<std::string, std::string> defines;
    std::vector<std::string> includePaths;
    
    // Performance settings
    size_t maxASTNodes;
    size_t maxParseDepth;
    size_t tokenCacheSize;
    
    // Helper methods
    void initializeDefaults();
    void updateLanguageFeaturesForStandard();
};

} // namespace Parser

#endif // PARSER_CONFIG_HPP