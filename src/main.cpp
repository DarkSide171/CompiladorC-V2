// Programa Principal - Analisador Léxico para Arquivos C
// Aceita arquivos .c como entrada da linha de comando

#include "lexer/include/lexer.hpp"
#include "lexer/include/error_handler.hpp"
#include "lexer/include/token.hpp"
#include "lexer_preprocessor_bridge.hpp"
#include "preprocessor/include/preprocessor.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <iomanip>
#include <map>
#include <cstring>
#include <sstream>
#include <algorithm>

using namespace Lexer;

// Enumeração para tipos de saída
enum class OutputFormat {
    VERBOSE,    // Saída detalhada com cores (padrão)
    SUMMARY,    // Saída resumida apenas com estatísticas
    JSON,       // Saída em formato JSON
    SEQUENTIAL  // Saída sequencial sem agrupamento por categoria
};

// Estrutura para filtros de tokens
struct TokenFilter {
    bool showKeywords = true;
    bool showOperators = true;
    bool showIdentifiers = true;
    bool showLiterals = true;
    bool showDelimiters = true;
    bool showPunctuation = true;
    bool showPreprocessor = true;
    bool showOthers = true;
    
    bool isFiltered() const {
        return !(showKeywords && showOperators && showIdentifiers && 
                showLiterals && showDelimiters && showPunctuation && 
                showPreprocessor && showOthers);
    }
    
    bool shouldShow(const std::string& category) const {
        if (category == "Palavras-chave") return showKeywords;
        if (category == "Operadores") return showOperators;
        if (category == "Identificadores") return showIdentifiers;
        if (category == "Literais") return showLiterals;
        if (category == "Delimitadores") return showDelimiters;
        if (category == "Pontuação") return showPunctuation;
        if (category == "Preprocessador") return showPreprocessor;
        return showOthers;
    }
};

// Códigos de cores ANSI
namespace Colors {
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string GRAY = "\033[90m";
}

// Função para obter cor baseada no tipo de token
std::string getTokenColor(TokenType type) {
    if (isKeywordToken(type)) return Colors::BLUE;
    if (isOperatorToken(type)) return Colors::MAGENTA;
    if (isLiteralToken(type)) return Colors::GREEN;
    if (type == TokenType::IDENTIFIER) return Colors::CYAN;
    if (type == TokenType::LEFT_PAREN || type == TokenType::RIGHT_PAREN ||
        type == TokenType::LEFT_BRACE || type == TokenType::RIGHT_BRACE ||
        type == TokenType::LEFT_BRACKET || type == TokenType::RIGHT_BRACKET) return Colors::YELLOW;
    return Colors::WHITE;
}

// Função para obter categoria do token
std::string getTokenCategory(TokenType type) {
    if (isKeywordToken(type)) return "Palavras-chave";
    if (isOperatorToken(type)) return "Operadores";
    if (isLiteralToken(type)) return "Literais";
    if (type == TokenType::IDENTIFIER) return "Identificadores";
    if (type == TokenType::LEFT_PAREN || type == TokenType::RIGHT_PAREN ||
        type == TokenType::LEFT_BRACE || type == TokenType::RIGHT_BRACE ||
        type == TokenType::LEFT_BRACKET || type == TokenType::RIGHT_BRACKET) return "Delimitadores";
    if (type == TokenType::SEMICOLON || type == TokenType::COMMA || type == TokenType::DOT) return "Pontuação";
    if (type == TokenType::HASH) return "Preprocessador";
    return "Outros";
}



// Função para escapar strings JSON
std::string escapeJsonString(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

// Função para exibir informações resumidas do pré-processador
void printPreprocessorSummary(const Integration::LexerPreprocessorBridge& bridge) {
    auto includedFiles = bridge.getIncludedFiles();
    auto definedMacros = bridge.getDefinedMacros();
    auto errors = bridge.getErrorMessages();
    auto warnings = bridge.getWarningMessages();
    
    std::cout << Colors::BOLD << Colors::BLUE << "\n📋 Pré-processamento: " << Colors::RESET;
    if (bridge.hasErrors()) {
        std::cout << Colors::RED << "❌ Erro" << Colors::RESET;
    } else {
        std::cout << Colors::GREEN << "✅ Concluído" << Colors::RESET;
    }
    std::cout << " (" << includedFiles.size() << " arquivos, " 
              << definedMacros.size() << " macros)" << std::endl;
    
    if (!errors.empty()) {
        std::cout << "   ❌ Erros: " << Colors::RED << errors.size() << Colors::RESET << std::endl;
        for (size_t i = 0; i < std::min(errors.size(), size_t(2)); ++i) {
            std::string errorMsg = errors[i];
            if (errorMsg.length() > 60) {
                errorMsg = errorMsg.substr(0, 57) + "...";
            }
            std::cout << "      • " << Colors::RED << errorMsg << Colors::RESET << std::endl;
        }
        if (errors.size() > 2) {
            std::cout << "      ... e mais " << Colors::YELLOW << (errors.size() - 2) << Colors::RESET << " erro(s)" << std::endl;
        }
    }
    
    if (!warnings.empty()) {
        std::cout << "   ⚠️  Avisos: " << Colors::YELLOW << warnings.size() << Colors::RESET << std::endl;
        for (size_t i = 0; i < std::min(warnings.size(), size_t(2)); ++i) {
            std::string warningMsg = warnings[i];
            if (warningMsg.length() > 60) {
                warningMsg = warningMsg.substr(0, 57) + "...";
            }
            std::cout << "      • " << Colors::YELLOW << warningMsg << Colors::RESET << std::endl;
        }
        if (warnings.size() > 2) {
            std::cout << "      ... e mais " << Colors::YELLOW << (warnings.size() - 2) << Colors::RESET << " aviso(s)" << std::endl;
        }
    }
}

// Função para exibir informações detalhadas do pré-processador
void printPreprocessorInfo(const Integration::LexerPreprocessorBridge& bridge) {
    auto stats = bridge.getStatistics();
    auto includedFiles = bridge.getIncludedFiles();
    auto definedMacros = bridge.getDefinedMacros();
    auto errors = bridge.getErrorMessages();
    auto warnings = bridge.getWarningMessages();
    
    std::cout << Colors::BOLD << Colors::BLUE << "\n╔══════════════════════════════════════════════════════════════╗" << Colors::RESET << std::endl;
    std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << " 📋 INFORMAÇÕES DO PRÉ-PROCESSADOR" << std::string(26, ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
    std::cout << Colors::BOLD << Colors::BLUE << "╠══════════════════════════════════════════════════════════════╣" << Colors::RESET << std::endl;
    
    // Status geral
    std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << " Status: ";
    if (bridge.hasErrors()) {
        std::cout << Colors::RED << "❌ Erro durante o processamento" << Colors::RESET;
    } else {
        std::cout << Colors::GREEN << "✅ Processamento concluído com sucesso" << Colors::RESET;
    }
    std::cout << std::string(20, ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
    
    // Estatísticas
    std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << " Estatísticas:" << std::string(47, ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
    std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << "   • Arquivos incluídos: " << Colors::YELLOW << includedFiles.size() << Colors::RESET;
    std::cout << std::string(33, ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
    
    std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << "   • Macros definidas: " << Colors::YELLOW << definedMacros.size() << Colors::RESET;
    std::cout << std::string(35, ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
    
    // Arquivos incluídos
    if (!includedFiles.empty()) {
        std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << " Arquivos incluídos:" << std::string(41, ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
        for (size_t i = 0; i < std::min(includedFiles.size(), size_t(5)); ++i) {
            std::string filename = includedFiles[i];
            if (filename.length() > 55) {
                filename = "..." + filename.substr(filename.length() - 52);
            }
            std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << "   📄 " << Colors::CYAN << filename << Colors::RESET;
            std::cout << std::string(55 - filename.length(), ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
        }
        if (includedFiles.size() > 5) {
            std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << "   ... e mais " << Colors::YELLOW << (includedFiles.size() - 5) << Colors::RESET << " arquivo(s)";
            std::cout << std::string(32, ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
        }
    }
    
    // Macros definidas
    if (!definedMacros.empty()) {
        std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << " Macros definidas:" << std::string(43, ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
        for (size_t i = 0; i < std::min(definedMacros.size(), size_t(5)); ++i) {
            std::string macroName = definedMacros[i];
            if (macroName.length() > 55) {
                macroName = macroName.substr(0, 52) + "...";
            }
            std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << "   🔧 " << Colors::MAGENTA << macroName << Colors::RESET;
            std::cout << std::string(55 - macroName.length(), ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
        }
        if (definedMacros.size() > 5) {
            std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << "   ... e mais " << Colors::YELLOW << (definedMacros.size() - 5) << Colors::RESET << " macro(s)";
            std::cout << std::string(34, ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
        }
    }
    
    // Erros do pré-processador
    if (!errors.empty()) {
        std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << " Erros encontrados:" << std::string(42, ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
        for (size_t i = 0; i < std::min(errors.size(), size_t(3)); ++i) {
            std::string errorMsg = errors[i];
            if (errorMsg.length() > 55) {
                errorMsg = errorMsg.substr(0, 52) + "...";
            }
            std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << "   ❌ " << Colors::RED << errorMsg << Colors::RESET;
            std::cout << std::string(55 - errorMsg.length(), ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
        }
        if (errors.size() > 3) {
            std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << "   ... e mais " << Colors::YELLOW << (errors.size() - 3) << Colors::RESET << " erro(s)";
            std::cout << std::string(36, ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
        }
    }
    
    // Avisos do pré-processador
    if (!warnings.empty()) {
        std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << " Avisos encontrados:" << std::string(41, ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
        for (size_t i = 0; i < std::min(warnings.size(), size_t(3)); ++i) {
            std::string warningMsg = warnings[i];
            if (warningMsg.length() > 55) {
                warningMsg = warningMsg.substr(0, 52) + "...";
            }
            std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << "   ⚠️  " << Colors::YELLOW << warningMsg << Colors::RESET;
            std::cout << std::string(54 - warningMsg.length(), ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
        }
        if (warnings.size() > 3) {
            std::cout << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << "   ... e mais " << Colors::YELLOW << (warnings.size() - 3) << Colors::RESET << " aviso(s)";
            std::cout << std::string(35, ' ') << Colors::BOLD << Colors::BLUE << "║" << Colors::RESET << std::endl;
        }
    }
    
    std::cout << Colors::BOLD << Colors::BLUE << "╚══════════════════════════════════════════════════════════════╝" << Colors::RESET << std::endl;
}

// Função para exibir informações do pré-processador em formato JSON
void printPreprocessorInfoJson(const Integration::LexerPreprocessorBridge& bridge) {
    const auto& result = bridge.getLastProcessingResult();
    auto stats = bridge.getStatistics();
    auto includedFiles = bridge.getIncludedFiles();
    auto definedMacros = bridge.getDefinedMacros();
    auto errors = bridge.getErrorMessages();
    auto warnings = bridge.getWarningMessages();
    
    std::cout << ",\n  \"preprocessor\": {" << std::endl;
    std::cout << "    \"status\": \"" << (bridge.hasErrors() ? "error" : "success") << "\"," << std::endl;
    std::cout << "    \"statistics\": {" << std::endl;
    std::cout << "      \"files_included\": " << includedFiles.size() << "," << std::endl;
    std::cout << "      \"macros_defined\": " << definedMacros.size() << "," << std::endl;
    if (stats.find("position_mappings") != stats.end()) {
        std::cout << "      \"position_mappings\": " << stats["position_mappings"] << "," << std::endl;
    }
    std::cout << "      \"errors\": " << errors.size() << "," << std::endl;
    std::cout << "      \"warnings\": " << warnings.size() << std::endl;
    std::cout << "    }," << std::endl;
    
    std::cout << "    \"included_files\": [" << std::endl;
    for (size_t i = 0; i < includedFiles.size(); ++i) {
        std::cout << "      \"" << escapeJsonString(includedFiles[i]) << "\"";
        if (i < includedFiles.size() - 1) std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << "    ]," << std::endl;
    
    std::cout << "    \"defined_macros\": [" << std::endl;
    for (size_t i = 0; i < definedMacros.size(); ++i) {
        std::cout << "      \"" << escapeJsonString(definedMacros[i]) << "\"";
        if (i < definedMacros.size() - 1) std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << "    ]," << std::endl;
    
    std::cout << "    \"errors\": [" << std::endl;
    for (size_t i = 0; i < errors.size(); ++i) {
        std::cout << "      \"" << escapeJsonString(errors[i]) << "\"";
        if (i < errors.size() - 1) std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << "    ]," << std::endl;
    
    std::cout << "    \"warnings\": [" << std::endl;
    for (size_t i = 0; i < warnings.size(); ++i) {
        std::cout << "      \"" << escapeJsonString(warnings[i]) << "\"";
        if (i < warnings.size() - 1) std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << "    ]" << std::endl;
    std::cout << "  }" << std::endl;
}

// Função para saída em formato JSON
void printTokensAsJson(const std::vector<Token>& tokens, const std::string& filename, bool hasErrors, const TokenFilter& filter = TokenFilter{}) {
    std::cout << "{\n";
    std::cout << "  \"file\": \"" << escapeJsonString(filename) << "\",\n";
    std::cout << "  \"hasErrors\": " << (hasErrors ? "true" : "false") << ",\n";
    std::cout << "  \"totalTokens\": " << tokens.size() << ",\n";
    std::cout << "  \"tokens\": [\n";
    
    std::vector<Token> filteredTokens;
    for (const auto& token : tokens) {
        std::string category = getTokenCategory(token.getType());
        if (filter.shouldShow(category)) {
            filteredTokens.push_back(token);
        }
    }
    
    for (size_t i = 0; i < filteredTokens.size(); ++i) {
        const auto& token = filteredTokens[i];
        std::cout << "    {\n";
        std::cout << "      \"type\": \"" << tokenTypeToString(token.getType()) << "\",\n";
        std::cout << "      \"lexeme\": \"" << escapeJsonString(token.getLexeme()) << "\",\n";
        std::cout << "      \"line\": " << token.getPosition().line << ",\n";
        std::cout << "      \"column\": " << token.getPosition().column << ",\n";
        std::cout << "      \"category\": \"" << getTokenCategory(token.getType()) << "\"\n";
        std::cout << "    }" << (i < filteredTokens.size() - 1 ? "," : "") << "\n";
    }
    
    std::cout << "  ],\n";
    
    // Estatísticas por categoria
    std::map<std::string, int> categoryCount;
    for (const auto& token : filteredTokens) {
        categoryCount[getTokenCategory(token.getType())]++;
    }
    
    int totalFilteredTokens = filteredTokens.size();
    
    std::cout << "  \"statistics\": {\n";
    std::cout << "    \"categories\": " << categoryCount.size() << ",\n";
    std::cout << "    \"distribution\": {\n";
    
    size_t catIndex = 0;
    for (const auto& [category, count] : categoryCount) {
        double percentage = totalFilteredTokens == 0 ? 0.0 : (static_cast<double>(count) / totalFilteredTokens) * 100.0;
        std::cout << "      \"" << category << "\": {\n";
        std::cout << "        \"count\": " << count << ",\n";
        std::cout << "        \"percentage\": " << std::fixed << std::setprecision(1) << percentage << "\n";
        std::cout << "      }" << (catIndex < categoryCount.size() - 1 ? "," : "") << "\n";
        catIndex++;
    }
    
    std::cout << "    }\n";
    std::cout << "  }\n";
    std::cout << "}\n";
}

// Função para saída resumida
void printTokensSummary(const std::vector<Token>& tokens, const std::string& filename, bool hasErrors, const TokenFilter& filter = TokenFilter{}) {
    std::cout << Colors::CYAN << "📄 " << filename << Colors::RESET << "\n";
    
    // Filtrar tokens
    std::vector<Token> filteredTokens;
    for (const auto& token : tokens) {
        std::string category = getTokenCategory(token.getType());
        if (filter.shouldShow(category)) {
            filteredTokens.push_back(token);
        }
    }
    
    // Contar tokens por categoria
    std::map<std::string, int> categoryCount;
    for (const auto& token : filteredTokens) {
        categoryCount[getTokenCategory(token.getType())]++;
    }
    
    std::cout << "📊 Total: " << Colors::YELLOW << filteredTokens.size() << Colors::RESET << " tokens";
    std::cout << " | Categorias: " << Colors::YELLOW << categoryCount.size() << Colors::RESET;
    
    if (hasErrors) {
        std::cout << " | " << Colors::RED << "❌ Com erros" << Colors::RESET;
    } else {
        std::cout << " | " << Colors::GREEN << "✅ Sem erros" << Colors::RESET;
    }
    std::cout << "\n";
    
    // Mostrar distribuição por categoria em uma linha
    std::cout << "📈 ";
    size_t catIndex = 0;
    for (const auto& [category, count] : categoryCount) {
        double percentage = filteredTokens.empty() ? 0.0 : (static_cast<double>(count) / filteredTokens.size()) * 100.0;
        std::cout << category << ": " << Colors::YELLOW << count << Colors::RESET;
        std::cout << " (" << std::fixed << std::setprecision(1) << percentage << "%)";
        if (catIndex < categoryCount.size() - 1) std::cout << " | ";
        catIndex++;
    }
    std::cout << "\n\n";
}

// Função para verificar se um arquivo tem extensão .c
bool isCFile(const std::string& filename) {
    return filename.size() >= 2 && filename.substr(filename.size() - 2) == ".c";
}

// Função para verificar se um caminho é um arquivo regular
bool isRegularFile(const std::string& path) {
    struct stat statbuf;
    return (stat(path.c_str(), &statbuf) == 0) && S_ISREG(statbuf.st_mode);
}

// Função para verificar se um caminho é um diretório
bool isDirectory(const std::string& path) {
    struct stat statbuf;
    return (stat(path.c_str(), &statbuf) == 0) && S_ISDIR(statbuf.st_mode);
}

// Função para verificar se um arquivo existe
bool fileExists(const std::string& path) {
    struct stat statbuf;
    return stat(path.c_str(), &statbuf) == 0;
}

// Função para buscar arquivos .c em um diretório
std::vector<std::string> findCFiles(const std::string& directory) {
    std::vector<std::string> cFiles;
    
    DIR* dir = opendir(directory.c_str());
    if (dir == nullptr) {
        std::cerr << "Erro ao abrir diretório: " << directory << std::endl;
        return cFiles;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        
        // Pular . e ..
        if (filename == "." || filename == "..") {
            continue;
        }
        
        std::string fullPath = directory + "/" + filename;
        
        if (isRegularFile(fullPath) && isCFile(filename)) {
            cFiles.push_back(fullPath);
        }
    }
    
    closedir(dir);
    return cFiles;
}

// Função para imprimir tokens no formato verbose com filtros
void printTokensSequential(const std::vector<Token>& tokens, const std::string& filename, bool hasErrors, const TokenFilter& filter = TokenFilter{}) {
    std::cout << Colors::BOLD << "\n📊 ANÁLISE LÉXICA SEQUENCIAL" << Colors::RESET << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    int tokenCount = 0;
    std::map<std::string, int> categoryCount;
    
    for (const auto& token : tokens) {
        std::string category = getTokenCategory(token.getType());
        if (filter.shouldShow(category)) {
            tokenCount++;
            categoryCount[category]++;
            
            std::string color = getTokenColor(token.getType());
            
            std::cout << std::setw(4) << tokenCount << ". "
                     << color << std::setw(20) << std::left << tokenTypeToString(token.getType()) << Colors::RESET
                     << " │ " << Colors::WHITE << "'" << token.getLexeme() << "'" << Colors::RESET
                     << " │ " << Colors::GRAY << "L" << token.getPosition().line 
                     << ":C" << token.getPosition().column << Colors::RESET
                     << " │ " << Colors::CYAN << category << Colors::RESET << std::endl;
        }
    }
    
    if (tokenCount == 0) {
        std::cout << Colors::YELLOW << "\n⚠️  Nenhum token encontrado com os filtros aplicados." << Colors::RESET << std::endl;
        return;
    }
    
    // Estatísticas finais
    std::cout << Colors::BOLD << Colors::GREEN << "\n📈 ESTATÍSTICAS" << Colors::RESET << std::endl;
    std::cout << std::string(30, '=') << std::endl;
    std::cout << Colors::CYAN << "Total de tokens: " << Colors::BOLD << tokenCount << Colors::RESET << std::endl;
    std::cout << Colors::CYAN << "Categorias encontradas: " << Colors::BOLD << categoryCount.size() << Colors::RESET << std::endl;
    
    if (filter.isFiltered()) {
        std::cout << Colors::YELLOW << "\n🔍 Filtros aplicados - mostrando apenas categorias selecionadas" << Colors::RESET << std::endl;
    }
    
    std::cout << Colors::YELLOW << "\nDistribuição por categoria:" << Colors::RESET << std::endl;
    for (const auto& [category, count] : categoryCount) {
        double percentage = (double)count / tokenCount * 100;
        std::cout << "  • " << Colors::WHITE << std::setw(15) << std::left << category << Colors::RESET
                 << ": " << Colors::BOLD << std::setw(3) << count << Colors::RESET
                 << " (" << Colors::YELLOW << std::fixed << std::setprecision(1) << percentage << "%" << Colors::RESET << ")" << std::endl;
    }
    
    // Verificar se houve erros
    if (hasErrors) {
        std::cout << Colors::RED << "\n⚠️  Erros encontrados durante o processamento" << Colors::RESET << std::endl;
    } else {
        std::cout << Colors::GREEN << "\n✅ Análise concluída sem erros!" << Colors::RESET << std::endl;
    }
}

void printTokensVerbose(const std::vector<Token>& tokens, const std::string& filename, bool hasErrors, const TokenFilter& filter = TokenFilter{}) {
    // Organizar tokens por categoria
    std::map<std::string, std::vector<Token>> tokensByCategory;
    
    for (const auto& token : tokens) {
        std::string category = getTokenCategory(token.getType());
        if (filter.shouldShow(category)) {
            tokensByCategory[category].push_back(token);
        }
    }
    
    if (tokensByCategory.empty()) {
        std::cout << Colors::YELLOW << "\n⚠️  Nenhum token encontrado com os filtros aplicados." << Colors::RESET << std::endl;
        return;
    }
    
    // Exibir tokens organizados por categoria
    std::cout << Colors::BOLD << "\n📊 ANÁLISE LÉXICA DETALHADA" << Colors::RESET << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    int totalTokens = 0;
    for (const auto& [category, categoryTokens] : tokensByCategory) {
        std::cout << Colors::BOLD << "\n🔹 " << category << " (" << categoryTokens.size() << ")" << Colors::RESET << std::endl;
        std::cout << std::string(30, '-') << std::endl;
        
        for (size_t i = 0; i < categoryTokens.size(); ++i) {
            const Token& token = categoryTokens[i];
            std::string color = getTokenColor(token.getType());
            
            std::cout << std::setw(3) << (i + 1) << ". "
                     << color << std::setw(20) << std::left << tokenTypeToString(token.getType()) << Colors::RESET
                     << " │ " << Colors::WHITE << "'" << token.getLexeme() << "'" << Colors::RESET
                     << " │ " << Colors::GRAY << "L" << token.getPosition().line 
                     << ":C" << token.getPosition().column << Colors::RESET << std::endl;
        }
        totalTokens += categoryTokens.size();
    }
    
    // Estatísticas finais
    std::cout << Colors::BOLD << Colors::GREEN << "\n📈 ESTATÍSTICAS" << Colors::RESET << std::endl;
    std::cout << std::string(30, '=') << std::endl;
    std::cout << Colors::CYAN << "Total de tokens: " << Colors::BOLD << totalTokens << Colors::RESET << std::endl;
    std::cout << Colors::CYAN << "Categorias encontradas: " << Colors::BOLD << tokensByCategory.size() << Colors::RESET << std::endl;
    
    if (filter.isFiltered()) {
        std::cout << Colors::YELLOW << "\n🔍 Filtros aplicados - mostrando apenas categorias selecionadas" << Colors::RESET << std::endl;
    }
    
    std::cout << Colors::YELLOW << "\nDistribuição por categoria:" << Colors::RESET << std::endl;
    for (const auto& [category, categoryTokens] : tokensByCategory) {
        double percentage = (double)categoryTokens.size() / totalTokens * 100;
        std::cout << "  • " << Colors::WHITE << std::setw(15) << std::left << category << Colors::RESET
                 << ": " << Colors::BOLD << std::setw(3) << categoryTokens.size() << Colors::RESET
                 << " (" << Colors::YELLOW << std::fixed << std::setprecision(1) << percentage << "%" << Colors::RESET << ")" << std::endl;
    }
    
    // Verificar se houve erros
    if (hasErrors) {
        std::cout << Colors::RED << "\n⚠️  Erros encontrados durante o processamento" << Colors::RESET << std::endl;
    } else {
        std::cout << Colors::GREEN << "\n✅ Análise concluída sem erros!" << Colors::RESET << std::endl;
    }
}

// Função para processar um arquivo .c
bool processFile(const std::string& filename, OutputFormat format = OutputFormat::VERBOSE, const TokenFilter& filter = TokenFilter{}) {
    std::cout << "[DEBUG] processFile chamada para: " << filename << std::endl;
    std::cout << Colors::BOLD << Colors::CYAN << "\n╔══════════════════════════════════════════════════════════════╗" << Colors::RESET << std::endl;
    std::cout << Colors::BOLD << Colors::CYAN << "║" << Colors::RESET << " Processando: " << Colors::YELLOW << filename << Colors::RESET;
    std::cout << std::string(std::max(0, 40 - (int)filename.length()), ' ') << Colors::BOLD << Colors::CYAN << "║" << Colors::RESET << std::endl;
    std::cout << Colors::BOLD << Colors::CYAN << "╚══════════════════════════════════════════════════════════════╝" << Colors::RESET << std::endl;
    
    try {
        // Verificar se o arquivo existe
        if (!fileExists(filename)) {
            std::cerr << Colors::RED << "❌ Arquivo não encontrado: " << filename << Colors::RESET << std::endl;
            return true;
        }
        
        // Usar o pipeline integrado preprocessor-lexer
        Integration::IntegrationConfig config;
        config.enableErrorIntegration = true;
        Integration::LexerPreprocessorBridge bridge(config);
        
        // Inicializar o bridge explicitamente
        if (!bridge.initialize()) {
            std::cerr << Colors::RED << "❌ Erro na inicialização do pipeline" << Colors::RESET << std::endl;
            return true;
        }
        
        // Processar o arquivo através do pipeline
        bool processingSuccess = bridge.processFile(filename);
        
        // Verificar se há erros críticos do pré-processador
        bool hasPreprocessorErrors = bridge.hasErrors();
        
        if (!processingSuccess && hasPreprocessorErrors) {
            std::cerr << Colors::RED << "❌ Erro crítico no pré-processador - interrompendo processamento" << Colors::RESET << std::endl;
            
            // Ainda assim, exibir informações do pré-processador para debug
            if (format == OutputFormat::VERBOSE) {
                printPreprocessorInfo(bridge);
            } else if (format == OutputFormat::SUMMARY) {
                printPreprocessorSummary(bridge);
            }
            
            return true;
        }
        
        // Exibir informações detalhadas do pré-processador
        if (format == OutputFormat::VERBOSE) {
            printPreprocessorInfo(bridge);
        } else if (format == OutputFormat::SUMMARY) {
            printPreprocessorSummary(bridge);
        }
        
        // Obter todos os tokens processados
        auto integratedTokens = bridge.tokenizeAll();
        
        // Coletar todos os tokens preservando a ordem original
        std::vector<Token> allTokens;
        std::map<std::string, int> categoryCount;
        int totalTokens = 0;
        
        for (const auto& integratedToken : integratedTokens) {
            const auto& token = integratedToken.lexerToken;
            
            // Adicionar token preservando ordem sequencial
            allTokens.push_back(token);
            
            // Contar por categoria para estatísticas
            std::string category = getTokenCategory(token.getType());
            categoryCount[category]++;
            totalTokens++;
        }
        
        bool hasErrors = bridge.hasErrors();
        
        // Verificar se há tokens UNKNOWN (erros léxicos)
        for (const auto& token : allTokens) {
            if (token.getType() == TokenType::UNKNOWN) {
                hasErrors = true;
                break;
            }
        }
        
        // Escolher formato de saída
        switch (format) {
            case OutputFormat::JSON:
                printTokensAsJson(allTokens, filename, hasErrors, filter);
                printPreprocessorInfoJson(bridge);
                break;
                
            case OutputFormat::SUMMARY:
                printTokensSummary(allTokens, filename, hasErrors, filter);
                break;
                
            case OutputFormat::SEQUENTIAL:
                printTokensSequential(allTokens, filename, hasErrors, filter);
                printPreprocessorSummary(bridge);
                break;
                
            case OutputFormat::VERBOSE:
            default:
                printTokensVerbose(allTokens, filename, hasErrors, filter);
                printPreprocessorInfo(bridge);
                break;
        }
        
        return hasErrors;
        
    } catch (const std::exception& e) {
        std::cerr << Colors::RED << "❌ Erro durante o processamento: " << e.what() << Colors::RESET << std::endl;
        return true;
    }
}

// Função para mostrar ajuda
void showHelp() {
    std::cout << "\n=== Analisador Léxico para Arquivos C ===" << std::endl;
    std::cout << "\nUso:" << std::endl;
    std::cout << "  ./main <arquivo.c>                 - Processar um arquivo específico" << std::endl;
    std::cout << "  ./main <diretório>                 - Processar todos os arquivos .c no diretório" << std::endl;
    std::cout << "  ./main -h ou --help                - Mostrar esta ajuda" << std::endl;
    std::cout << "\nExemplos:" << std::endl;
    std::cout << "  ./main examples/hello_world.c" << std::endl;
    std::cout << "  ./main src/lexer/tests/data/" << std::endl;
    std::cout << "  ./main .                           - Processar todos os .c no diretório atual" << std::endl;
}

void printHeader() {
    std::cout << Colors::BOLD << Colors::CYAN << "\n╔══════════════════════════════════════════════════════════════╗" << Colors::RESET << std::endl;
    std::cout << Colors::BOLD << Colors::CYAN << "║" << Colors::RESET << "                                                              " << Colors::BOLD << Colors::CYAN << "║" << Colors::RESET << std::endl;
    std::cout << Colors::BOLD << Colors::CYAN << "║" << Colors::RESET << "        " << Colors::BOLD << Colors::WHITE << "🔍 ANALISADOR LÉXICO PARA ARQUIVOS C" << Colors::RESET << "         " << Colors::BOLD << Colors::CYAN << "║" << Colors::RESET << std::endl;
    std::cout << Colors::BOLD << Colors::CYAN << "║" << Colors::RESET << "                                                              " << Colors::BOLD << Colors::CYAN << "║" << Colors::RESET << std::endl;
    std::cout << Colors::BOLD << Colors::CYAN << "║" << Colors::RESET << "  " << Colors::GRAY << "Desenvolvido para análise detalhada de código C" << Colors::RESET << "          " << Colors::BOLD << Colors::CYAN << "║" << Colors::RESET << std::endl;
    std::cout << Colors::BOLD << Colors::CYAN << "║" << Colors::RESET << "  " << Colors::GRAY << "Suporte a múltiplos arquivos e diretórios" << Colors::RESET << "             " << Colors::BOLD << Colors::CYAN << "║" << Colors::RESET << std::endl;
    std::cout << Colors::BOLD << Colors::CYAN << "║" << Colors::RESET << "                                                              " << Colors::BOLD << Colors::CYAN << "║" << Colors::RESET << std::endl;
    std::cout << Colors::BOLD << Colors::CYAN << "╚══════════════════════════════════════════════════════════════╝" << Colors::RESET << std::endl;
}

void printUsage(const char* programName) {
    std::cout << Colors::YELLOW << "\n📋 USO:" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::BOLD << programName << Colors::RESET << " " << Colors::CYAN << "[opções] <arquivo.c ou diretório>" << Colors::RESET << std::endl;
    
    std::cout << Colors::YELLOW << "\n⚙️  OPÇÕES DE FORMATO:" << Colors::RESET << std::endl;
    std::cout << "  -v, --verbose   Saída detalhada com cores (padrão)" << std::endl;
    std::cout << "  -s, --summary   Saída resumida apenas com estatísticas" << std::endl;
    std::cout << "  -j, --json      Saída em formato JSON" << std::endl;
    std::cout << "  -seq, --sequential  Saída sequencial sem agrupamento por categoria" << std::endl;
    
    std::cout << Colors::YELLOW << "\n🔍 OPÇÕES DE FILTRO:" << Colors::RESET << std::endl;
    std::cout << "  --filter-keywords      Mostrar apenas palavras-chave" << std::endl;
    std::cout << "  --filter-operators     Mostrar apenas operadores" << std::endl;
    std::cout << "  --filter-identifiers   Mostrar apenas identificadores" << std::endl;
    std::cout << "  --filter-literals      Mostrar apenas literais" << std::endl;
    std::cout << "  --filter-delimiters    Mostrar apenas delimitadores" << std::endl;
    std::cout << "  --filter-punctuation   Mostrar apenas pontuação" << std::endl;
    std::cout << "  --filter-preprocessor  Mostrar apenas diretivas de preprocessador" << std::endl;
    
    std::cout << Colors::YELLOW << "\n❓ AJUDA:" << Colors::RESET << std::endl;
    std::cout << "  -h, --help      Mostra esta ajuda" << std::endl;
    
    std::cout << Colors::YELLOW << "\n💡 EXEMPLOS:" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::WHITE << "• Análise completa:" << Colors::RESET << std::endl;
    std::cout << "    " << Colors::GRAY << programName << " programa.c" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::WHITE << "• Saída resumida:" << Colors::RESET << std::endl;
    std::cout << "    " << Colors::GRAY << programName << " --summary programa.c" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::WHITE << "• Apenas palavras-chave:" << Colors::RESET << std::endl;
    std::cout << "    " << Colors::GRAY << programName << " --filter-keywords programa.c" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::WHITE << "• JSON com filtro:" << Colors::RESET << std::endl;
    std::cout << "    " << Colors::GRAY << programName << " --json --filter-operators diretorio/" << Colors::RESET << std::endl;
}

int main(int argc, char* argv[]) {
    OutputFormat format = OutputFormat::SEQUENTIAL;
    TokenFilter filter;
    std::string inputPath;
    bool hasFilterOptions = false;
    
    // Processar argumentos da linha de comando
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printHeader();
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--verbose") {
            format = OutputFormat::VERBOSE;
        } else if (arg == "-s" || arg == "--summary") {
            format = OutputFormat::SUMMARY;
        } else if (arg == "-j" || arg == "--json") {
            format = OutputFormat::JSON;
        } else if (arg == "-seq" || arg == "--sequential") {
            format = OutputFormat::SEQUENTIAL;
        } else if (arg == "--filter-keywords") {
            if (!hasFilterOptions) {
                filter = TokenFilter{};
                filter.showKeywords = filter.showOperators = filter.showIdentifiers = 
                filter.showLiterals = filter.showDelimiters = filter.showPunctuation = 
                filter.showPreprocessor = filter.showOthers = false;
                hasFilterOptions = true;
            }
            filter.showKeywords = true;
        } else if (arg == "--filter-operators") {
            if (!hasFilterOptions) {
                filter = TokenFilter{};
                filter.showKeywords = filter.showOperators = filter.showIdentifiers = 
                filter.showLiterals = filter.showDelimiters = filter.showPunctuation = 
                filter.showPreprocessor = filter.showOthers = false;
                hasFilterOptions = true;
            }
            filter.showOperators = true;
        } else if (arg == "--filter-identifiers") {
            if (!hasFilterOptions) {
                filter = TokenFilter{};
                filter.showKeywords = filter.showOperators = filter.showIdentifiers = 
                filter.showLiterals = filter.showDelimiters = filter.showPunctuation = 
                filter.showPreprocessor = filter.showOthers = false;
                hasFilterOptions = true;
            }
            filter.showIdentifiers = true;
        } else if (arg == "--filter-literals") {
            if (!hasFilterOptions) {
                filter = TokenFilter{};
                filter.showKeywords = filter.showOperators = filter.showIdentifiers = 
                filter.showLiterals = filter.showDelimiters = filter.showPunctuation = 
                filter.showPreprocessor = filter.showOthers = false;
                hasFilterOptions = true;
            }
            filter.showLiterals = true;
        } else if (arg == "--filter-delimiters") {
            if (!hasFilterOptions) {
                filter = TokenFilter{};
                filter.showKeywords = filter.showOperators = filter.showIdentifiers = 
                filter.showLiterals = filter.showDelimiters = filter.showPunctuation = 
                filter.showPreprocessor = filter.showOthers = false;
                hasFilterOptions = true;
            }
            filter.showDelimiters = true;
        } else if (arg == "--filter-punctuation") {
            if (!hasFilterOptions) {
                filter = TokenFilter{};
                filter.showKeywords = filter.showOperators = filter.showIdentifiers = 
                filter.showLiterals = filter.showDelimiters = filter.showPunctuation = 
                filter.showPreprocessor = filter.showOthers = false;
                hasFilterOptions = true;
            }
            filter.showPunctuation = true;
        } else if (arg == "--filter-preprocessor") {
            if (!hasFilterOptions) {
                filter = TokenFilter{};
                filter.showKeywords = filter.showOperators = filter.showIdentifiers = 
                filter.showLiterals = filter.showDelimiters = filter.showPunctuation = 
                filter.showPreprocessor = filter.showOthers = false;
                hasFilterOptions = true;
            }
            filter.showPreprocessor = true;
        } else if (arg[0] != '-') {
            // É o caminho do arquivo/diretório
            if (inputPath.empty()) {
                inputPath = arg;
            } else {
                std::cout << Colors::RED << "❌ Erro: Múltiplos caminhos especificados!" << Colors::RESET << std::endl;
                printUsage(argv[0]);
                return 1;
            }
        } else {
            std::cout << Colors::RED << "❌ Erro: Opção desconhecida: " << arg << Colors::RESET << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (inputPath.empty()) {
        printHeader();
        std::cout << Colors::RED << "❌ Erro: Nenhum arquivo ou diretório especificado!" << Colors::RESET << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    // Mostrar cabeçalho apenas para formato verbose
    if (format == OutputFormat::VERBOSE) {
        printHeader();
    }
    
    bool hasErrors = false;
    
    try {
        if (isRegularFile(inputPath)) {
            if (isCFile(inputPath)) {
                hasErrors = processFile(inputPath, format, filter);
            } else {
                std::cout << Colors::RED << "❌ Erro: O arquivo deve ter extensão .c" << Colors::RESET << std::endl;
                return 1;
            }
        } else if (isDirectory(inputPath)) {
            std::vector<std::string> cFiles = findCFiles(inputPath);
            
            if (cFiles.empty()) {
                std::cout << Colors::YELLOW << "⚠️  Nenhum arquivo .c encontrado no diretório: " << inputPath << Colors::RESET << std::endl;
                return 1;
            }
            
            // Para JSON, criar um array
            if (format == OutputFormat::JSON) {
                std::cout << "{\n";
                std::cout << "  \"directory\": \"" << escapeJsonString(inputPath) << "\",\n";
                std::cout << "  \"totalFiles\": " << cFiles.size() << ",\n";
                std::cout << "  \"files\": [\n";
            } else if (format == OutputFormat::VERBOSE) {
                std::cout << Colors::BOLD << Colors::BLUE << "\n🔍 BUSCA EM DIRETÓRIO" << Colors::RESET << std::endl;
                std::cout << Colors::CYAN << "Diretório: " << Colors::YELLOW << inputPath << Colors::RESET << std::endl;
                std::cout << Colors::GREEN << "\n📁 Encontrados " << Colors::BOLD << cFiles.size() << Colors::RESET << Colors::GREEN << " arquivo(s) .c:" << Colors::RESET << std::endl;
                for (size_t i = 0; i < cFiles.size(); ++i) {
                    std::cout << "  " << Colors::GRAY << (i + 1) << "." << Colors::RESET << " " << Colors::WHITE << cFiles[i] << Colors::RESET << std::endl;
                }
            }
            
            for (size_t i = 0; i < cFiles.size(); ++i) {
                if (format == OutputFormat::JSON && i > 0) {
                    std::cout << ",\n";
                }
                if (processFile(cFiles[i], format, filter)) {
                    hasErrors = true;
                }
            }
            
            if (format == OutputFormat::JSON) {
                std::cout << "\n  ]\n}\n";
            }
        } else {
            std::cout << Colors::RED << "❌ Erro: Caminho não encontrado ou inválido: " << inputPath << Colors::RESET << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cout << Colors::RED << "❌ Erro: " << e.what() << Colors::RESET << std::endl;
        return 1;
    }
    
    // Mostrar mensagem final apenas para formato verbose
    if (format == OutputFormat::VERBOSE) {
        if (hasErrors) {
            std::cout << Colors::BOLD << Colors::RED << "\n❌ PROCESSAMENTO FINALIZADO COM ERROS!" << Colors::RESET << std::endl;
        } else {
            std::cout << Colors::BOLD << Colors::GREEN << "\n🎉 PROCESSAMENTO FINALIZADO COM SUCESSO!" << Colors::RESET << std::endl;
        }
        std::cout << Colors::GRAY << "Obrigado por usar o Analisador Léxico C!" << Colors::RESET << std::endl;
    }
    
    return hasErrors ? 1 : 0;
}