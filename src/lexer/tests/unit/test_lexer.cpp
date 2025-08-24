// Testes Unitários - Analisador Léxico
// Testes para a classe Lexer e seus métodos principais

#include "../../include/lexer.hpp"
#include "../../include/error_handler.hpp"
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>

using namespace Lexer;

// Função auxiliar para criar arquivo de teste
void createTestFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    file << content;
    file.close();
}

// Teste do construtor e destrutor
void testConstructorDestructor() {
    std::cout << "Testando construtor e destrutor...";
    
    // Criar arquivo de teste
    createTestFile("test_input.c", "int main() { return 0; }");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        // Se chegou até aqui, o construtor funcionou
        std::cout << " PASSOU" << std::endl;
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    // Limpar arquivo de teste
    std::remove("test_input.c");
}

// Teste do método nextToken
void testNextToken() {
    std::cout << "Testando nextToken()...";
    
    // Criar arquivo de teste simples
    createTestFile("test_input.c", "a");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        Token token = lexer.nextToken();
        
        // Deve retornar um token (mesmo que UNKNOWN)
        if (token.getType() != TokenType::END_OF_FILE) {
            std::cout << " PASSOU" << std::endl;
        } else {
            std::cout << " FALHOU: Token inesperado" << std::endl;
        }
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste do método hasMoreTokens
void testHasMoreTokens() {
    std::cout << "Testando hasMoreTokens()...";
    
    createTestFile("test_input.c", "abc");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        // Inicialmente deve ter tokens
        if (lexer.hasMoreTokens()) {
            std::cout << " PASSOU" << std::endl;
        } else {
            std::cout << " FALHOU: Deveria ter tokens" << std::endl;
        }
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste do método getCurrentPosition
void testGetCurrentPosition() {
    std::cout << "Testando getCurrentPosition()...";
    
    createTestFile("test_input.c", "test");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        auto pos = lexer.getCurrentPosition();
        
        // Posição inicial deve ser linha 1, coluna 1
        if (pos.line == 1 && pos.column == 1) {
            std::cout << " PASSOU" << std::endl;
        } else {
            std::cout << " FALHOU: Posição incorreta (" << pos.line << ", " << pos.column << ")" << std::endl;
        }
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste do método setVersion
void testSetVersion() {
    std::cout << "Testando setVersion()...";
    
    createTestFile("test_input.c", "int x;");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        // Testar diferentes versões
        lexer.setVersion("C99");
        lexer.setVersion("C11");
        lexer.setVersion("C17");
        
        std::cout << " PASSOU" << std::endl;
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste do método reset
void testReset() {
    std::cout << "Testando reset()...";
    
    createTestFile("test_input.c", "abc");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        // Consumir alguns tokens
        lexer.nextToken();
        
        // Resetar
        lexer.reset();
        
        // Verificar se voltou ao início
        auto pos = lexer.getCurrentPosition();
        if (pos.line == 1 && pos.column == 1) {
            std::cout << " PASSOU" << std::endl;
        } else {
            std::cout << " FALHOU: Reset não funcionou" << std::endl;
        }
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste do método tokenizeAll
void testTokenizeAll() {
    std::cout << "Testando tokenizeAll()...";
    
    createTestFile("test_input.c", "ab");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve ter pelo menos o token EOF
        if (!tokens.empty() && tokens.back().getType() == TokenType::END_OF_FILE) {
            std::cout << " PASSOU (" << tokens.size() << " tokens)" << std::endl;
        } else {
            std::cout << " FALHOU: Tokens incorretos" << std::endl;
        }
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

int main() {
    std::cout << "=== Testes da Fase 4.1 - LexerMain ===" << std::endl;
    
    testConstructorDestructor();
    testNextToken();
    testHasMoreTokens();
    testGetCurrentPosition();
    testSetVersion();
    testReset();
    testTokenizeAll();
    
    std::cout << "\n=== Testes concluídos ===" << std::endl;
    
    return 0;
}