/**
 * @file test_lexer_config.cpp
 * @brief Testes unitários simples para a classe LexerConfig
 * @author Sistema de Compilador C
 * @date 2024
 */

#include "../../include/lexer_config.hpp"
#include "../../include/error_handler.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <memory>

// Função auxiliar para testes
void assert_test(bool condition, const std::string& test_name) {
    if (condition) {
        std::cout << "[PASS] " << test_name << std::endl;
    } else {
        std::cout << "[FAIL] " << test_name << std::endl;
        assert(false);
    }
}

// Teste básico de construção
void test_basic_construction() {
    std::cout << "\n=== Teste de Construção Básica ===" << std::endl;
    
    auto errorHandler = std::make_shared<Lexer::ErrorHandler>();
    Lexer::LexerConfig config(Lexer::CVersion::C89, errorHandler.get());
    
    assert_test(config.getVersion() == Lexer::CVersion::C89, "Versão padrão é C89");
    assert_test(!config.getKeywords().empty(), "Keywords não estão vazias");
    // C89 não tem features especiais, então pode estar vazio
    std::cout << "[INFO] C89 tem " << config.getEnabledFeatures().size() << " features habilitadas" << std::endl;
}

// Teste de gerenciamento de versão
void test_version_management() {
    std::cout << "\n=== Teste de Gerenciamento de Versão ===" << std::endl;
    
    auto errorHandler = std::make_shared<Lexer::ErrorHandler>();
    Lexer::LexerConfig config(Lexer::CVersion::C89, errorHandler.get());
    
    config.setVersion(Lexer::CVersion::C99);
    assert_test(config.getVersion() == Lexer::CVersion::C99, "Definir versão C99");
    
    config.setVersion(Lexer::CVersion::C11);
    assert_test(config.getVersion() == Lexer::CVersion::C11, "Definir versão C11");
    
    config.setVersion(Lexer::CVersion::C23);
    assert_test(config.getVersion() == Lexer::CVersion::C23, "Definir versão C23");
}

// Teste de palavras-chave básicas
void test_basic_keywords() {
    std::cout << "\n=== Teste de Palavras-chave Básicas ===" << std::endl;
    
    auto errorHandler = std::make_shared<Lexer::ErrorHandler>();
    Lexer::LexerConfig config(Lexer::CVersion::C89, errorHandler.get());
    
    // Testar palavras-chave básicas do C89
    assert_test(config.isKeyword("int"), "'int' é palavra-chave");
    assert_test(config.isKeyword("char"), "'char' é palavra-chave");
    assert_test(config.isKeyword("float"), "'float' é palavra-chave");
    assert_test(config.isKeyword("double"), "'double' é palavra-chave");
    assert_test(config.isKeyword("void"), "'void' é palavra-chave");
    assert_test(config.isKeyword("if"), "'if' é palavra-chave");
    assert_test(config.isKeyword("else"), "'else' é palavra-chave");
    assert_test(config.isKeyword("while"), "'while' é palavra-chave");
    assert_test(config.isKeyword("for"), "'for' é palavra-chave");
    assert_test(config.isKeyword("return"), "'return' é palavra-chave");
    
    // Testar que identificadores normais não são palavras-chave
    assert_test(!config.isKeyword("myVariable"), "'myVariable' não é palavra-chave");
    assert_test(!config.isKeyword("function_name"), "'function_name' não é palavra-chave");
}

// Teste de palavras-chave por versão
void test_version_specific_keywords() {
    std::cout << "\n=== Teste de Palavras-chave por Versão ===" << std::endl;
    
    auto errorHandler = std::make_shared<Lexer::ErrorHandler>();
    Lexer::LexerConfig config(Lexer::CVersion::C89, errorHandler.get());
    
    // Testar C99
    config.setVersion(Lexer::CVersion::C99);
    assert_test(config.isKeyword("inline"), "C99: 'inline' é palavra-chave");
    assert_test(config.isKeyword("restrict"), "C99: 'restrict' é palavra-chave");
    assert_test(config.isKeyword("_Bool"), "C99: '_Bool' é palavra-chave");
    
    // Testar C11
    config.setVersion(Lexer::CVersion::C11);
    assert_test(config.isKeyword("_Atomic"), "C11: '_Atomic' é palavra-chave");
    assert_test(config.isKeyword("_Static_assert"), "C11: '_Static_assert' é palavra-chave");
    assert_test(config.isKeyword("_Thread_local"), "C11: '_Thread_local' é palavra-chave");
}

// Teste de gerenciamento de features
void test_feature_management() {
    std::cout << "\n=== Teste de Gerenciamento de Features ===" << std::endl;
    
    auto errorHandler = std::make_shared<Lexer::ErrorHandler>();
    Lexer::LexerConfig config(Lexer::CVersion::C89, errorHandler.get());
    
    config.setVersion(Lexer::CVersion::C11);
    
    // Testar habilitação de feature
    config.enableFeature(Lexer::Feature::ATOMIC_OPERATIONS);
    assert_test(config.isFeatureEnabled(Lexer::Feature::ATOMIC_OPERATIONS), "Feature ATOMIC_OPERATIONS habilitada");
    
    // Testar desabilitação de feature
    config.disableFeature(Lexer::Feature::ATOMIC_OPERATIONS);
    assert_test(!config.isFeatureEnabled(Lexer::Feature::ATOMIC_OPERATIONS), "Feature ATOMIC_OPERATIONS desabilitada");
    
    // Testar desabilitar todas as features
    config.enableFeature(Lexer::Feature::ATOMIC_OPERATIONS);
    config.enableFeature(Lexer::Feature::THREAD_LOCAL_STORAGE);
    config.disableAllFeatures();
    assert_test(config.getEnabledFeatures().empty(), "Todas as features desabilitadas");
}

// Teste de funções utilitárias
void test_utility_functions() {
    std::cout << "\n=== Teste de Funções Utilitárias ===" << std::endl;
    
    // Testar conversão de versão para string
    assert_test(Lexer::versionToString(Lexer::CVersion::C89) == "C89", "Conversão C89 para string");
    assert_test(Lexer::versionToString(Lexer::CVersion::C99) == "C99", "Conversão C99 para string");
    assert_test(Lexer::versionToString(Lexer::CVersion::C11) == "C11", "Conversão C11 para string");
    assert_test(Lexer::versionToString(Lexer::CVersion::C17) == "C17", "Conversão C17 para string");
    assert_test(Lexer::versionToString(Lexer::CVersion::C23) == "C23", "Conversão C23 para string");
    
    // Testar conversão de string para versão
    assert_test(Lexer::stringToVersion("C89") == Lexer::CVersion::C89, "Conversão string para C89");
    assert_test(Lexer::stringToVersion("C99") == Lexer::CVersion::C99, "Conversão string para C99");
    assert_test(Lexer::stringToVersion("C11") == Lexer::CVersion::C11, "Conversão string para C11");
    assert_test(Lexer::stringToVersion("C17") == Lexer::CVersion::C17, "Conversão string para C17");
    assert_test(Lexer::stringToVersion("C23") == Lexer::CVersion::C23, "Conversão string para C23");
}

// Teste de operadores de comparação
void test_comparison_operators() {
    std::cout << "\n=== Teste de Operadores de Comparação ===" << std::endl;
    
    auto errorHandler = std::make_shared<Lexer::ErrorHandler>();
    Lexer::LexerConfig config1(Lexer::CVersion::C89, errorHandler.get());
    Lexer::LexerConfig config2(Lexer::CVersion::C89, errorHandler.get());
    
    // Configurações iguais
    config1.setVersion(Lexer::CVersion::C99);
    config2.setVersion(Lexer::CVersion::C99);
    assert_test(config1 == config2, "Configurações iguais");
    assert_test(!(config1 != config2), "Operador != com configurações iguais");
    
    // Configurações diferentes
    config2.setVersion(Lexer::CVersion::C11);
    assert_test(config1 != config2, "Configurações diferentes");
    assert_test(!(config1 == config2), "Operador == com configurações diferentes");
}

// Função principal para executar todos os testes
int main() {
    std::cout << "=== INICIANDO TESTES DA LEXERCONFIG ===" << std::endl;
    
    try {
        test_basic_construction();
        test_version_management();
        test_basic_keywords();
        test_version_specific_keywords();
        test_feature_management();
        test_utility_functions();
        test_comparison_operators();
        
        std::cout << "\n=== TODOS OS TESTES PASSARAM! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\n=== ERRO NOS TESTES: " << e.what() << " ===" << std::endl;
        return 1;
    } catch (...) {
        std::cout << "\n=== ERRO DESCONHECIDO NOS TESTES ===" << std::endl;
        return 1;
    }
}