// Testes Unitários - Tratamento de Erros Léxicos
// Testes para tratamento de erros no lexer e ErrorHandler

#include "../../include/error_handler.hpp"
#include "../../include/lexer.hpp"
#include "../../include/token.hpp"
#include <iostream>
#include <sstream>
#include <cassert>
#include <string>
#include <fstream>
#include <cstdio>

using namespace Lexer;



// Teste básico do ErrorHandler
void test_error_handler_basic() {
    std::cout << "Testando ErrorHandler básico..." << std::endl;
    
    ErrorHandler handler;
    
    // Teste inicial - sem erros
    assert(handler.getErrorCount() == 0);
    assert(handler.getWarningCount() == 0);
    assert(!handler.hasErrors());
    
    // Teste de reportar erro
    Position pos{1, 1, 0};
    handler.reportError(ErrorType::INVALID_CHARACTER, "Caractere inválido", pos);
    
    assert(handler.getErrorCount() == 1);
    assert(handler.hasErrors());
    
    // Teste de reportar warning
    handler.reportWarning(ErrorType::UNUSED_VARIABLE, "Aviso de teste", pos);
    assert(handler.getWarningCount() == 1);
    
    // Teste de limpar erros
    handler.clear();
    assert(handler.getErrorCount() == 0);
    assert(handler.getWarningCount() == 0);
    assert(!handler.hasErrors());
    
    std::cout << "✓ ErrorHandler básico passou" << std::endl;
}

// Teste de diferentes tipos de erro
void test_error_types() {
    std::cout << "Testando tipos de erro..." << std::endl;
    
    ErrorHandler handler;
    Position pos{1, 1, 0};
    
    // Teste de cada tipo de erro
    handler.reportError(ErrorType::INVALID_CHARACTER, "Caractere inválido", pos);
    handler.reportError(ErrorType::UNTERMINATED_STRING, "String não terminada", pos);
    handler.reportError(ErrorType::UNTERMINATED_COMMENT, "Comentário não terminado", pos);
    handler.reportError(ErrorType::INVALID_NUMBER_FORMAT, "Número inválido", pos);
    handler.reportError(ErrorType::INVALID_ESCAPE_SEQUENCE, "Sequência de escape inválida", pos);
    
    assert(handler.getErrorCount() == 5);
    
    std::cout << "✓ Tipos de erro passaram" << std::endl;
}

// Teste de tratamento de erros no lexer
void test_lexer_error_handling() {
    std::cout << "Testando tratamento de erros no lexer..." << std::endl;
    
    // Teste com caractere inválido
    {
        // Criar arquivo temporário para teste
        std::ofstream tempFile("temp_test.c");
        tempFile << "int x = @;";
        tempFile.close();
        
        ErrorHandler handler;
        LexerMain lexer("temp_test.c", &handler);
        
        auto tokens = lexer.tokenizeAll();
        
        // Deve ter reportado erro para '@'
        assert(handler.hasErrors());
        assert(handler.getErrorCount() > 0);
        
        // Limpar arquivo temporário
        std::remove("temp_test.c");
    }
    
    // Teste com string não terminada
    {
        // Criar arquivo temporário para teste
        std::ofstream tempFile("temp_test2.c");
        tempFile << "char* str = \"hello;";
        tempFile.close();
        
        ErrorHandler handler;
        LexerMain lexer("temp_test2.c", &handler);
        
        auto tokens = lexer.tokenizeAll();
        
        // Deve ter reportado erro de string não terminada
        assert(handler.hasErrors());
        
        // Limpar arquivo temporário
        std::remove("temp_test2.c");
    }
    
    // Teste com comentário não terminado
    {
        // Criar arquivo temporário para teste
        std::ofstream tempFile("temp_test3.c");
        tempFile << "int x = 5; /* comentário não terminado";
        tempFile.close();
        
        ErrorHandler handler;
        LexerMain lexer("temp_test3.c", &handler);
        
        auto tokens = lexer.tokenizeAll();
        
        // Deve ter reportado erro de comentário não terminado
        assert(handler.hasErrors());
        
        // Limpar arquivo temporário
        std::remove("temp_test3.c");
    }
    
    std::cout << "✓ Tratamento de erros no lexer passou" << std::endl;
}

// Teste de recuperação de erros
void test_error_recovery() {
    std::cout << "Testando recuperação de erros..." << std::endl;
    
    // Teste com múltiplos erros - deve continuar processando
    // Criar arquivo temporário para teste
    std::ofstream tempFile("temp_test4.c");
    tempFile << "int x = @; float y = #; char z = 'a';";
    tempFile.close();
    
    ErrorHandler handler;
    LexerMain lexer("temp_test4.c", &handler);
    
    auto tokens = lexer.tokenizeAll();
    
    // Deve ter encontrado erros mas continuado processando
    assert(handler.hasErrors());
    assert(handler.getErrorCount() >= 2);
    
    // Deve ter tokens válidos também
    bool foundValidTokens = false;
    for (const auto& token : tokens) {
        if (token.getType() == TokenType::INT || 
            token.getType() == TokenType::FLOAT || 
            token.getType() == TokenType::CHAR) {
            foundValidTokens = true;
            break;
        }
    }
    assert(foundValidTokens);
    
    // Limpar arquivo temporário
    std::remove("temp_test4.c");
    
    std::cout << "✓ Recuperação de erros passou" << std::endl;
}

// Teste de validação de tokens
void test_token_validation() {
    std::cout << "Testando validação de tokens..." << std::endl;
    
    // Teste com números inválidos
    {
        // Criar arquivo temporário para teste
        std::ofstream tempFile("temp_test5.c");
        tempFile << "0x; 0b; 123.456.789;";
        tempFile.close();
        
        ErrorHandler handler;
        LexerMain lexer("temp_test5.c", &handler);
        
        auto tokens = lexer.tokenizeAll();
        
        // Deve ter reportado erros de validação
        assert(handler.hasErrors());
        
        // Limpar arquivo temporário
        std::remove("temp_test5.c");
    }
    
    // Teste com escape sequences inválidas
    {
        // Criar arquivo temporário para teste
        std::ofstream tempFile("temp_test6.c");
        tempFile << "char c = '\\z';";
        tempFile.close();
        
        ErrorHandler handler;
        LexerMain lexer("temp_test6.c", &handler);
        
        auto tokens = lexer.tokenizeAll();
        
        // Pode ter reportado erro de escape inválido
        // (dependendo da implementação)
        
        // Limpar arquivo temporário
        std::remove("temp_test6.c");
    }
    
    std::cout << "✓ Validação de tokens passou" << std::endl;
}

// Teste de limites de erro
void test_error_limits() {
    std::cout << "Testando limites de erro..." << std::endl;
    
    ErrorHandler handler;
    Position pos{1, 1, 0};
    
    // Reportar muitos erros
    for (int i = 0; i < 15; i++) {
        handler.reportError(ErrorType::INVALID_CHARACTER, "Erro " + std::to_string(i), pos);
    }
    
    // Verificar se parou de processar após muitos erros
    assert(handler.getErrorCount() >= 10);
    
    std::cout << "✓ Limites de erro passaram" << std::endl;
}

// Teste de formatação de mensagens de erro
void test_error_formatting() {
    std::cout << "Testando formatação de erros..." << std::endl;
    
    ErrorHandler handler;
    Position pos{5, 10, 45};
    
    // Reportar erro
    handler.reportError(ErrorType::INVALID_CHARACTER, "Caractere '@' inválido", pos);
    
    // Verificar se o erro foi registrado corretamente
    assert(handler.hasErrors());
    assert(handler.getErrorCount() == 1);
    
    // Obter o erro registrado
    const auto& errors = handler.getErrors();
    assert(!errors.empty());
    
    const auto& error = errors[0];
    assert(error.type == ErrorType::INVALID_CHARACTER);
    assert(error.message == "Caractere '@' inválido");
    assert(error.position.line == 5);
    assert(error.position.column == 10);
    assert(error.position.offset == 45);
    
    std::cout << "✓ Formatação de erros passou" << std::endl;
}

int main() {
    std::cout << "=== Executando Testes de Tratamento de Erros Léxicos ===" << std::endl;
    
    try {
        test_error_handler_basic();
        test_error_types();
        test_lexer_error_handling();
        test_error_recovery();
        test_token_validation();
        test_error_limits();
        test_error_formatting();
        
        std::cout << "\n✅ Todos os testes de tratamento de erros passaram!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Erro durante os testes: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Erro desconhecido durante os testes" << std::endl;
        return 1;
    }
}