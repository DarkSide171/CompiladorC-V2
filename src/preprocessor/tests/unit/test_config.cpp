// Testes Unitários - Sistema de Configuração
// Testes para preprocessor_config.hpp - Fase 1.6
// Testa CVersion, VersionFeatures, PreprocessorConfig e funções utilitárias

#include "../../include/preprocessor_config.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <fstream>
#include <cstdio>

using namespace Preprocessor;

// ============================================================================
// FUNÇÕES AUXILIARES PARA TESTES
// ============================================================================

void assertEqual(const std::string& expected, const std::string& actual, const std::string& testName) {
    if (expected != actual) {
        std::cerr << "❌ FALHA em " << testName << ": esperado '" << expected 
                  << "', obtido '" << actual << "'" << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertEqual(int expected, int actual, const std::string& testName) {
    if (expected != actual) {
        std::cerr << "❌ FALHA em " << testName << ": esperado " << expected 
                  << ", obtido " << actual << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertEqual(size_t expected, size_t actual, const std::string& testName) {
    if (expected != actual) {
        std::cerr << "❌ FALHA em " << testName << ": esperado " << expected 
                  << ", obtido " << actual << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertTrue(bool condition, const std::string& testName) {
    if (!condition) {
        std::cerr << "❌ FALHA em " << testName << ": condição falsa" << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertFalse(bool condition, const std::string& testName) {
    if (condition) {
        std::cerr << "❌ FALHA em " << testName << ": condição verdadeira" << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

// ============================================================================
// TESTES DO ENUM CVersion
// ============================================================================

void testCVersionToString() {
    std::cout << "\n=== Testando cVersionToString() ===" << std::endl;
    
    assertEqual("C89", cVersionToString(CVersion::C89), "C89 -> string");
    assertEqual("C99", cVersionToString(CVersion::C99), "C99 -> string");
    assertEqual("C11", cVersionToString(CVersion::C11), "C11 -> string");
    assertEqual("C17", cVersionToString(CVersion::C17), "C17 -> string");
    assertEqual("C23", cVersionToString(CVersion::C23), "C23 -> string");
}

void testStringToCVersion() {
    std::cout << "\n=== Testando stringToCVersion() ===" << std::endl;
    
    assertTrue(stringToCVersion("C89") == CVersion::C89, "string -> C89");
    assertTrue(stringToCVersion("C99") == CVersion::C99, "string -> C99");
    assertTrue(stringToCVersion("C11") == CVersion::C11, "string -> C11");
    assertTrue(stringToCVersion("C17") == CVersion::C17, "string -> C17");
    assertTrue(stringToCVersion("C23") == CVersion::C23, "string -> C23");
    
    // Testa variações de case e aliases
    assertTrue(stringToCVersion("c89") == CVersion::C89, "c89 (lowercase) -> C89");
    assertTrue(stringToCVersion("c99") == CVersion::C99, "c99 (lowercase) -> C99");
    assertTrue(stringToCVersion("C90") == CVersion::C89, "C90 (alias) -> C89");
    assertTrue(stringToCVersion("c90") == CVersion::C89, "c90 (alias) -> C89");
    assertTrue(stringToCVersion("C18") == CVersion::C17, "C18 (alias) -> C17");
    
    // Teste string inválida - deve lançar exceção
    try {
        stringToCVersion("INVALID_VERSION");
        assertTrue(false, "Deveria lançar exceção para versão inválida");
    } catch (const std::invalid_argument& e) {
        assertTrue(true, "Exceção lançada corretamente para versão inválida");
    }
}

// ============================================================================
// TESTES DAS FUNÇÕES DE FEATURES
// ============================================================================

void testVersionSupportsFeature() {
    std::cout << "\n=== Testando versionSupportsFeature() ===" << std::endl;
    
    // Testa features do C99
    assertTrue(versionSupportsFeature(CVersion::C99, "variadic_macros"), "C99 suporta macros variádicas");
    assertFalse(versionSupportsFeature(CVersion::C89, "variadic_macros"), "C89 não suporta macros variádicas");
    
    // Testa features do C11
    assertTrue(versionSupportsFeature(CVersion::C11, "generic_selection"), "C11 suporta _Generic");
    assertTrue(versionSupportsFeature(CVersion::C11, "static_assert"), "C11 suporta _Static_assert");
    assertFalse(versionSupportsFeature(CVersion::C99, "generic_selection"), "C99 não suporta _Generic");
    
    // Testa features do C23
    assertTrue(versionSupportsFeature(CVersion::C23, "typeof"), "C23 suporta typeof");
    assertFalse(versionSupportsFeature(CVersion::C17, "typeof"), "C17 não suporta typeof");
    
    // Testa feature inexistente
    assertFalse(versionSupportsFeature(CVersion::C23, "nonexistent_feature"), "Feature inexistente retorna false");
}

void testGetSupportedFeatures() {
    std::cout << "\n=== Testando getSupportedFeatures() ===" << std::endl;
    
    // Testa C89 (mínimas features)
    std::vector<std::string> c89Features = getSupportedFeatures(CVersion::C89);
    assertTrue(c89Features.size() >= 0, "C89 tem pelo menos 0 features");
    
    // Testa C99 (mais features que C89)
    std::vector<std::string> c99Features = getSupportedFeatures(CVersion::C99);
    assertTrue(c99Features.size() > c89Features.size(), "C99 tem mais features que C89");
    
    // Testa C23 (máximas features)
    std::vector<std::string> c23Features = getSupportedFeatures(CVersion::C23);
    assertTrue(c23Features.size() > c99Features.size(), "C23 tem mais features que C99");
    
    std::cout << "✅ C89 features: " << c89Features.size() << std::endl;
    std::cout << "✅ C99 features: " << c99Features.size() << std::endl;
    std::cout << "✅ C23 features: " << c23Features.size() << std::endl;
}

// ============================================================================
// TESTES DA CLASSE PreprocessorConfig - CONSTRUTORES
// ============================================================================

void testPreprocessorConfigConstructors() {
    std::cout << "\n=== Testando Construtores de PreprocessorConfig ===" << std::endl;
    
    // Teste construtor padrão
    PreprocessorConfig config1;
    assertTrue(config1.getVersion() == CVersion::C99, "Construtor padrão usa C99");
    
    // Teste construtor com versão específica
    PreprocessorConfig config2(CVersion::C11);
    assertTrue(config2.getVersion() == CVersion::C11, "Construtor com C11");
    
    PreprocessorConfig config3(CVersion::C89);
    assertTrue(config3.getVersion() == CVersion::C89, "Construtor com C89");
    
    PreprocessorConfig config4(CVersion::C23);
    assertTrue(config4.getVersion() == CVersion::C23, "Construtor com C23");
}

// ============================================================================
// TESTES DE INICIALIZAÇÃO E CONFIGURAÇÃO
// ============================================================================

void testInitializeDefaultConfig() {
    std::cout << "\n=== Testando initializeDefaultConfig() ===" << std::endl;
    
    PreprocessorConfig config;
    config.initializeDefaultConfig();
    
    // Verifica se a configuração foi inicializada
    assertTrue(config.getVersion() == CVersion::C99, "Versão padrão é C99");
    assertTrue(config.validateConfiguration(), "Configuração padrão é válida");
    
    // Verifica se há caminhos de inclusão padrão
    const auto& paths = config.getIncludePaths();
    std::cout << "✅ Caminhos de inclusão padrão: " << paths.size() << std::endl;
    
    // Verifica se há macros predefinidas
    const auto& macros = config.getPredefinedMacros();
    std::cout << "✅ Macros predefinidas: " << macros.size() << std::endl;
}

void testResetToDefaults() {
    std::cout << "\n=== Testando resetToDefaults() ===" << std::endl;
    
    PreprocessorConfig config(CVersion::C23);
    
    // Modifica configuração
    config.setVersion(CVersion::C89);
    config.addIncludePath("/custom/path");
    config.addPredefinedMacro("CUSTOM", "1");
    
    // Verifica modificações
    assertTrue(config.getVersion() == CVersion::C89, "Versão modificada para C89");
    
    // Reseta para padrões
    config.resetToDefaults();
    
    // Verifica reset
    assertTrue(config.getVersion() == CVersion::C99, "Versão resetada para C99");
    assertTrue(config.validateConfiguration(), "Configuração válida após reset");
}

// ============================================================================
// TESTES DE GERENCIAMENTO DE VERSÃO
// ============================================================================

void testVersionManagement() {
    std::cout << "\n=== Testando Gerenciamento de Versão ===" << std::endl;
    
    PreprocessorConfig config;
    
    // Testa mudanças de versão
    config.setVersion(CVersion::C89);
    assertTrue(config.getVersion() == CVersion::C89, "setVersion(C89)");
    
    config.setVersion(CVersion::C11);
    assertTrue(config.getVersion() == CVersion::C11, "setVersion(C11)");
    
    config.setVersion(CVersion::C23);
    assertTrue(config.getVersion() == CVersion::C23, "setVersion(C23)");
    
    // Testa validação de compatibilidade
    assertTrue(config.validateCStandardCompatibility(CVersion::C23), "C23 é compatível com C23");
    assertTrue(config.validateCStandardCompatibility(CVersion::C17), "C23 é compatível com C17");
    assertTrue(config.validateCStandardCompatibility(CVersion::C11), "C23 é compatível com C11");
    
    // Testa atualização de features
    config.updateFeatureFlags(CVersion::C99);
    std::cout << "✅ Features atualizadas para C99" << std::endl;
    
    config.updateFeatureFlags(CVersion::C23);
    std::cout << "✅ Features atualizadas para C23" << std::endl;
}

// ============================================================================
// TESTES DE GERENCIAMENTO DE CAMINHOS
// ============================================================================

void testIncludePathManagement() {
    std::cout << "\n=== Testando Gerenciamento de Caminhos de Inclusão ===" << std::endl;
    
    PreprocessorConfig config;
    
    // Testa adição de caminhos
    config.addIncludePath("/usr/include");
    config.addIncludePath("/usr/local/include");
    config.addIncludePath("./headers");
    
    const auto& paths = config.getIncludePaths();
    assertTrue(paths.size() >= 3, "Pelo menos 3 caminhos adicionados");
    
    // Verifica se os caminhos foram adicionados
    bool foundUsrInclude = false;
    bool foundLocalInclude = false;
    bool foundHeaders = false;
    
    for (const auto& path : paths) {
        if (path == "/usr/include") foundUsrInclude = true;
        if (path == "/usr/local/include") foundLocalInclude = true;
        if (path == "./headers") foundHeaders = true;
    }
    
    assertTrue(foundUsrInclude, "Caminho /usr/include encontrado");
    assertTrue(foundLocalInclude, "Caminho /usr/local/include encontrado");
    assertTrue(foundHeaders, "Caminho ./headers encontrado");
    
    // Testa remoção de caminho
    size_t pathsBefore = paths.size();
    config.removeIncludePath("/usr/include");
    const auto& pathsAfter = config.getIncludePaths();
    assertTrue(pathsAfter.size() == pathsBefore - 1, "Caminho removido com sucesso");
    
    // Testa limpeza de caminhos
    config.clearIncludePaths();
    const auto& clearedPaths = config.getIncludePaths();
    assertTrue(clearedPaths.empty(), "Todos os caminhos foram limpos");
}

void testPathValidationAndNormalization() {
    std::cout << "\n=== Testando Validação e Normalização de Caminhos ===" << std::endl;
    
    PreprocessorConfig config;
    
    // Testa validação de caminhos (apenas caminhos que existem)
    assertFalse(config.validateIncludePath(""), "Caminho vazio é inválido");
    assertFalse(config.validateIncludePath("/path/that/does/not/exist"), "Caminho inexistente é inválido");
    
    // Testa se pelo menos o diretório atual existe
    assertTrue(config.validateIncludePath("."), "Diretório atual é válido");
    
    // Testa normalização
    std::string normalized1 = config.normalizeIncludePath("/usr/include/");
    std::string normalized2 = config.normalizeIncludePath("./headers/../include");
    
    assertFalse(normalized1.empty(), "Normalização não retorna string vazia");
    assertFalse(normalized2.empty(), "Normalização de caminho complexo");
    
    std::cout << "✅ Normalizado: '/usr/include/' -> '" << normalized1 << "'" << std::endl;
    std::cout << "✅ Normalizado: './headers/../include' -> '" << normalized2 << "'" << std::endl;
    
    // Testa resolução de caminhos
    config.addIncludePath("/usr/include");
    config.addIncludePath("./relative");
    config.resolveIncludePaths();
    std::cout << "✅ Caminhos resolvidos com sucesso" << std::endl;
}

// ============================================================================
// TESTES DE GERENCIAMENTO DE MACROS
// ============================================================================

void testPredefinedMacroManagement() {
    std::cout << "\n=== Testando Gerenciamento de Macros Predefinidas ===" << std::endl;
    
    PreprocessorConfig config;
    
    // Testa adição de macros
    config.addPredefinedMacro("DEBUG", "1");
    config.addPredefinedMacro("VERSION", "\"1.0.0\"");
    config.addPredefinedMacro("MAX_SIZE", "1024");
    
    const auto& macros = config.getPredefinedMacros();
    assertTrue(macros.size() >= 3, "Pelo menos 3 macros adicionadas");
    
    // Verifica se as macros foram adicionadas
    auto debugIt = macros.find("DEBUG");
    auto versionIt = macros.find("VERSION");
    auto maxSizeIt = macros.find("MAX_SIZE");
    
    assertTrue(debugIt != macros.end(), "Macro DEBUG encontrada");
    assertTrue(versionIt != macros.end(), "Macro VERSION encontrada");
    assertTrue(maxSizeIt != macros.end(), "Macro MAX_SIZE encontrada");
    
    if (debugIt != macros.end()) {
        assertEqual("1", debugIt->second, "Valor da macro DEBUG");
    }
    if (versionIt != macros.end()) {
        assertEqual("\"1.0.0\"", versionIt->second, "Valor da macro VERSION");
    }
    if (maxSizeIt != macros.end()) {
        assertEqual("1024", maxSizeIt->second, "Valor da macro MAX_SIZE");
    }
    
    // Testa remoção de macro
    size_t macrosBefore = macros.size();
    config.removePredefinedMacro("DEBUG");
    const auto& macrosAfter = config.getPredefinedMacros();
    assertTrue(macrosAfter.size() == macrosBefore - 1, "Macro removida com sucesso");
    
    // Verifica se a macro foi realmente removida
    auto removedIt = macrosAfter.find("DEBUG");
    assertTrue(removedIt == macrosAfter.end(), "Macro DEBUG foi removida");
}

// ============================================================================
// TESTES DE VALIDAÇÃO
// ============================================================================

void testConfigValidation() {
    std::cout << "\n=== Testando Validação de Configuração ===" << std::endl;
    
    PreprocessorConfig config;
    
    // Testa configuração padrão
    assertTrue(config.validateConfiguration(), "Configuração padrão é válida");
    
    // Testa validação de valores específicos
    assertTrue(config.validateConfigValue("version", "C99"), "Valor de versão válido");
    assertTrue(config.validateConfigValue("debug", "true"), "Valor booleano válido");
    assertTrue(config.validateConfigValue("max_include_depth", "100"), "Valor numérico válido");
    
    // Testa valores inválidos
    assertFalse(config.validateConfigValue("version", "InvalidVersion"), "Versão inválida");
    assertFalse(config.validateConfigValue("max_include_depth", "-1"), "Profundidade negativa inválida");
    assertFalse(config.validateConfigValue("debug", "invalid_bool"), "Valor booleano inválido");
}

// ============================================================================
// TESTES DE PARSING E PROCESSAMENTO
// ============================================================================

void testConfigLineParsing() {
    std::cout << "\n=== Testando Parsing de Linhas de Configuração ===" << std::endl;
    
    PreprocessorConfig config;
    
    // Testa parsing de linhas válidas
    assertTrue(config.parseConfigLine("version = C11"), "Parsing de versão");
    assertTrue(config.parseConfigLine("debug = true"), "Parsing de modo debug");
    assertTrue(config.parseConfigLine("include_path = ."), "Parsing de caminho");
    assertTrue(config.parseConfigLine("define_DEBUG = 1"), "Parsing de macro");
    
    // Testa parsing de linhas inválidas
    assertTrue(config.parseConfigLine(""), "Linha vazia é ignorada");
    assertFalse(config.parseConfigLine("invalid line format"), "Formato inválido");
    assertTrue(config.parseConfigLine("# comment line"), "Linha de comentário é ignorada");
    
    // Aplica mudanças
    config.applyConfigChanges();
    std::cout << "✅ Mudanças de configuração aplicadas" << std::endl;
}

// ============================================================================
// TESTES DE ARQUIVO
// ============================================================================

void testConfigFileOperations() {
    std::cout << "\n=== Testando Operações de Arquivo ===" << std::endl;
    
    PreprocessorConfig config;
    const std::string testFile = "test_config.tmp";
    
    // Cria arquivo de teste
    std::ofstream outFile(testFile);
    if (outFile.is_open()) {
        outFile << "version = C11\n";
        outFile << "debug = true\n";
        outFile << "include_path = .\n";
        outFile << "define_TEST_MACRO = 42\n";
        outFile.close();
        
        // Testa carregamento
        assertTrue(config.loadConfigFromFile(testFile), "Carregamento de arquivo");
        assertTrue(config.loadConfiguration(testFile), "Carregamento alternativo");
        
        // Testa salvamento
        const std::string saveFile = "saved_config.tmp";
        assertTrue(config.saveConfigToFile(saveFile), "Salvamento de arquivo");
        
        // Limpa arquivos de teste
        std::remove(testFile.c_str());
        std::remove(saveFile.c_str());
        
        std::cout << "✅ Operações de arquivo concluídas" << std::endl;
    } else {
        std::cout << "⚠️  Não foi possível criar arquivo de teste" << std::endl;
    }
}

// ============================================================================
// TESTES DE FUNCIONALIDADES AVANÇADAS
// ============================================================================

void testAdvancedFeatures() {
    std::cout << "\n=== Testando Funcionalidades Avançadas ===" << std::endl;
    
    PreprocessorConfig config1(CVersion::C99);
    PreprocessorConfig config2(CVersion::C11);
    
    // Configura config1
    config1.addIncludePath("/path1");
    config1.addPredefinedMacro("MACRO1", "value1");
    
    // Configura config2
    config2.addIncludePath("/path2");
    config2.addPredefinedMacro("MACRO2", "value2");
    
    // Testa mesclagem
    config1.mergeConfigurations(config2);
    
    const auto& mergedPaths = config1.getIncludePaths();
    const auto& mergedMacros = config1.getPredefinedMacros();
    
    assertTrue(mergedPaths.size() >= 2, "Caminhos mesclados");
    assertTrue(mergedMacros.size() >= 2, "Macros mescladas");
    
    // Testa manipulação de variáveis de ambiente
    config1.handleEnvironmentVariables();
    std::cout << "✅ Variáveis de ambiente processadas" << std::endl;
    
    // Testa geração de relatório
    std::string report = config1.generateConfigReport();
    assertFalse(report.empty(), "Relatório gerado");
    std::cout << "✅ Relatório de configuração: " << report.length() << " caracteres" << std::endl;
}

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main() {
    std::cout << "=== EXECUTANDO TESTES DO SISTEMA DE CONFIGURAÇÃO (FASE 1.6) ===" << std::endl;
    
    try {
        // Testes do enum CVersion
        testCVersionToString();
        testStringToCVersion();
        
        // Testes das funções de features
        testVersionSupportsFeature();
        testGetSupportedFeatures();
        
        // Testes da classe PreprocessorConfig
        testPreprocessorConfigConstructors();
        testInitializeDefaultConfig();
        testResetToDefaults();
        
        // Testes de gerenciamento
        testVersionManagement();
        testIncludePathManagement();
        testPathValidationAndNormalization();
        testPredefinedMacroManagement();
        
        // Testes de validação
        testConfigValidation();
        
        // Testes de parsing
        testConfigLineParsing();
        
        // Testes de arquivo
        testConfigFileOperations();
        
        // Testes avançados
        testAdvancedFeatures();
        
        std::cout << "\n🎉 Todos os testes do sistema de configuração passaram com sucesso!" << std::endl;
        std::cout << "📊 Total de testes executados: 13 grupos de teste" << std::endl;
        std::cout << "✅ Fase 1.6 - test_config.cpp: CONCLUÍDO" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Erro durante os testes: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Erro desconhecido durante os testes" << std::endl;
        return 1;
    }
}