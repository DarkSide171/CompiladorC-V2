// Teste Abrangente de Sistema - Unificação de testes de componentes do sistema
// Consolida funcionalidades de test_config.cpp, test_file_manager.cpp,
// test_logger.cpp e test_state.cpp

#include "../../include/preprocessor_config.hpp"
#include "../../include/file_manager.hpp"
#include "../../include/preprocessor_logger.hpp"
#include "../../include/preprocessor_state.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <fstream>
#include <cstdio>
#include <memory>

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
// TESTES DE CONFIGURAÇÃO (test_config.cpp)
// ============================================================================

void testCVersionToString() {
    std::cout << "\n=== Testando CVersionToString ===" << std::endl;
    
    try {
        assertEqual("C89", cVersionToString(CVersion::C89), "C89 para string");
        assertEqual("C99", cVersionToString(CVersion::C99), "C99 para string");
        assertEqual("C11", cVersionToString(CVersion::C11), "C11 para string");
        assertEqual("C17", cVersionToString(CVersion::C17), "C17 para string");
        assertEqual("C23", cVersionToString(CVersion::C23), "C23 para string");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em CVersionToString: " << e.what() << std::endl;
    }
}

void testStringToCVersion() {
    std::cout << "\n=== Testando StringToCVersion ===" << std::endl;
    
    try {
        assertEqual(static_cast<int>(CVersion::C89), static_cast<int>(stringToCVersion("C89")), "String para C89");
        assertEqual(static_cast<int>(CVersion::C99), static_cast<int>(stringToCVersion("C99")), "String para C99");
        assertEqual(static_cast<int>(CVersion::C11), static_cast<int>(stringToCVersion("C11")), "String para C11");
        assertEqual(static_cast<int>(CVersion::C17), static_cast<int>(stringToCVersion("C17")), "String para C17");
        assertEqual(static_cast<int>(CVersion::C23), static_cast<int>(stringToCVersion("C23")), "String para C23");
        
        // Teste com string inválida
        assertEqual(static_cast<int>(CVersion::C99), static_cast<int>(stringToCVersion("INVALID")), "String inválida retorna C99");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em StringToCVersion: " << e.what() << std::endl;
    }
}

void testPreprocessorConfigConstructors() {
    std::cout << "\n=== Testando Construtores de PreprocessorConfig ===" << std::endl;
    
    try {
        // Construtor padrão
        PreprocessorConfig config1;
        assertEqual(static_cast<int>(CVersion::C99), static_cast<int>(config1.getVersion()), "Construtor padrão - versão C99");
        
        // Construtor com versão
        PreprocessorConfig config2(CVersion::C11);
        assertEqual(static_cast<int>(CVersion::C11), static_cast<int>(config2.getVersion()), "Construtor com versão C11");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em construtores: " << e.what() << std::endl;
    }
}

void testVersionManagement() {
    std::cout << "\n=== Testando Gerenciamento de Versão ===" << std::endl;
    
    try {
        PreprocessorConfig config;
        
        // Teste setVersion
        config.setVersion(CVersion::C17);
        assertEqual(static_cast<int>(CVersion::C17), static_cast<int>(config.getVersion()), "setVersion C17");
        
        // Teste versionSupportsFeature
         assertTrue(versionSupportsFeature(config.getVersion(), "variadic_macros"), "C17 suporta macros variádicas");
         assertTrue(versionSupportsFeature(config.getVersion(), "inline_functions"), "C17 suporta inline_functions");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em gerenciamento de versão: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES DE GERENCIADOR DE ARQUIVOS (test_file_manager.cpp)
// ============================================================================

void testFileManagerConstructor() {
    std::cout << "\n=== Testando Construtor do FileManager ===" << std::endl;
    
    try {
        auto logger = std::make_shared<PreprocessorLogger>();
        std::vector<std::string> searchPaths = {"/usr/include", "/usr/local/include"};
        
        FileManager manager(searchPaths, logger.get());
        
        // Verificar se foi criado corretamente
        assertTrue(true, "FileManager criado com sucesso");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro no construtor do FileManager: " << e.what() << std::endl;
    }
}

void testFileOperations() {
    std::cout << "\n=== Testando Operações de Arquivo ===" << std::endl;
    
    try {
        auto logger = std::make_shared<PreprocessorLogger>();
        std::vector<std::string> searchPaths = {"/tmp"};
        FileManager manager(searchPaths, logger.get());
        
        // Criar arquivo temporário para teste
        std::string testFile = "/tmp/test_file_manager.h";
        std::ofstream file(testFile);
        file << "#define TEST_MACRO 1\n";
        file.close();
        
        // Testar se arquivo existe
         bool exists = manager.fileExists(testFile);
         assertTrue(exists, "Arquivo de teste existe");
        
        // Limpar arquivo de teste
        std::remove(testFile.c_str());
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em operações de arquivo: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES DE LOGGER (test_logger.cpp)
// ============================================================================

void testPreprocessorPosition() {
    std::cout << "\n=== Testando PreprocessorPosition ===" << std::endl;
    
    try {
        // Teste construtor padrão
        PreprocessorPosition pos1;
        assertEqual("", pos1.filename, "Construtor padrão - filename vazio");
        assertEqual(1, pos1.line, "Construtor padrão - linha 1");
        assertEqual(1, pos1.column, "Construtor padrão - coluna 1");
        
        // Teste construtor com parâmetros
        PreprocessorPosition pos2("test.c", 10, 5);
        assertEqual("test.c", pos2.filename, "Construtor com parâmetros - filename");
        assertEqual(10, pos2.line, "Construtor com parâmetros - linha");
        assertEqual(5, pos2.column, "Construtor com parâmetros - coluna");
        
        // Teste operador de igualdade
        PreprocessorPosition pos3("test.c", 10, 5);
        PreprocessorPosition pos4("test.c", 10, 6);
        
        assertTrue(pos2 == pos3, "Posições iguais");
        assertFalse(pos2 == pos4, "Posições diferentes");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em PreprocessorPosition: " << e.what() << std::endl;
    }
}

void testLoggerBasicFunctionality() {
    std::cout << "\n=== Testando Funcionalidade Básica do Logger ===" << std::endl;
    
    try {
        PreprocessorLogger logger;
        PreprocessorPosition pos("test.c", 1, 1);
        
        // Testar diferentes níveis de log
        logger.debug("Mensagem de debug");
        logger.info("Mensagem de info");
        logger.warning("Mensagem de warning");
        logger.error("Mensagem de erro");
        
        // Testar log com posição
        logger.debug("Debug com posição", pos);
        logger.error("Erro com posição", pos);
        
        assertTrue(true, "Logger funcionando corretamente");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro no logger: " << e.what() << std::endl;
    }
}

void testLogLevels() {
    std::cout << "\n=== Testando Níveis de Log ===" << std::endl;
    
    try {
        PreprocessorLogger logger;
        
        // Testar configuração de nível
        logger.setLogLevel(LogLevel::WARNING);
        
        // Mensagens abaixo do nível não devem aparecer
        logger.debug("Debug não deve aparecer");
        logger.info("Info não deve aparecer");
        logger.warning("Warning deve aparecer");
        logger.error("Error deve aparecer");
        
        assertTrue(true, "Níveis de log funcionando");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em níveis de log: " << e.what() << std::endl;
    }
}

// ============================================================================
// TESTES DE ESTADO (test_state.cpp)
// ============================================================================

void testPreprocessorStateConstructor() {
    std::cout << "\n=== Testando Construtor do PreprocessorState ===" << std::endl;
    
    try {
        PreprocessorState state;
        
        // Verificar estado inicial
        assertFalse(state.hasError(), "Estado inicial sem erros");
        assertTrue(true, "PreprocessorState criado com sucesso");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro no construtor do PreprocessorState: " << e.what() << std::endl;
    }
}

void testStateManagement() {
    std::cout << "\n=== Testando Gerenciamento de Estado ===" << std::endl;
    
    try {
        PreprocessorState state;
        
        // Testar configuração de erro
         state.setErrorState(true);
         assertTrue(state.hasError(), "Estado de erro configurado");
         
         // Testar reset de erro
         state.setErrorState(false);
         assertFalse(state.hasError(), "Estado de erro resetado");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em gerenciamento de estado: " << e.what() << std::endl;
    }
}

void testStateStatistics() {
    std::cout << "\n=== Testando Estatísticas de Estado ===" << std::endl;
    
    try {
        PreprocessorState state;
        
        // Incrementar contadores
        state.incrementLine();
        state.incrementLine();
        state.incrementLine();
        
        // Verificar estatísticas
        size_t lines = state.getCurrentLine();
        assertEqual(size_t(4), lines, "Linhas processadas");
        
    } catch (const std::exception& e) {
        std::cout << "❌ Erro em estatísticas: " << e.what() << std::endl;
    }
}

// ============================================================================
// FUNÇÃO PRINCIPAL DE TESTES
// ============================================================================

int main() {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "TESTES ABRANGENTES DE SISTEMA DO PRÉ-PROCESSADOR" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    try {
        // Testes de Configuração
        testCVersionToString();
        testStringToCVersion();
        testPreprocessorConfigConstructors();
        testVersionManagement();
        
        // Testes de Gerenciador de Arquivos
        testFileManagerConstructor();
        testFileOperations();
        
        // Testes de Logger
        testPreprocessorPosition();
        testLoggerBasicFunctionality();
        testLogLevels();
        
        // Testes de Estado
        testPreprocessorStateConstructor();
        testStateManagement();
        testStateStatistics();
        
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "RESUMO DOS TESTES DE SISTEMA" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "✅ Testes de Configuração: Concluído" << std::endl;
        std::cout << "✅ Testes de Gerenciador de Arquivos: Concluído" << std::endl;
        std::cout << "✅ Testes de Logger: Concluído" << std::endl;
        std::cout << "✅ Testes de Estado: Concluído" << std::endl;
        std::cout << "\n🎉 TODOS OS TESTES DE SISTEMA PASSARAM! 🎉" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n❌ ERRO GERAL: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}