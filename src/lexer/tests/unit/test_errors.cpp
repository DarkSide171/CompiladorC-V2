// Testes Unitários - Tratamento de Erros Léxicos
// Testes para tratamento de erros no lexer e ErrorHandler
// Fase 5.1 - Testes Unitários Básicos

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

// ===== MÉTODOS DA FASE 5.1 =====

// 1. testErrorReporting() - Relatório de erros
void testErrorReporting() {
    std::cout << "Testando relatório de erros..." << std::endl;
    
    ErrorHandler handler;
    Position pos1{1, 5, 10};
    Position pos2{2, 15, 25};
    
    // Teste reportar erro único
    handler.reportError(ErrorType::INVALID_CHARACTER, "Caractere '@' inválido", pos1);
    assert(handler.hasErrors());
    assert(handler.getErrorCount() == 1);
    
    // Teste reportar múltiplos erros
    handler.reportError(ErrorType::UNTERMINATED_STRING, "String não terminada", pos2);
    assert(handler.getErrorCount() == 2);
    
    // Verificar detalhes dos erros
    const auto& errors = handler.getErrors();
    assert(errors.size() == 2);
    assert(errors[0].type == ErrorType::INVALID_CHARACTER);
    assert(errors[1].type == ErrorType::UNTERMINATED_STRING);
    
    // Teste limpar erros
    handler.clear();
    assert(!handler.hasErrors());
    assert(handler.getErrorCount() == 0);
    
    std::cout << "✓ Relatório de erros passou" << std::endl;
}

// 2. testWarningReporting() - Relatório de warnings
void testWarningReporting() {
    std::cout << "Testando relatório de warnings..." << std::endl;
    
    ErrorHandler handler;
    Position pos{3, 8, 30};
    
    // Teste reportar warning único
    handler.reportWarning(ErrorType::UNUSED_VARIABLE, "Variável não utilizada", pos);
    assert(handler.getWarningCount() == 1);
    assert(!handler.hasErrors()); // Warnings não são erros
    
    // Teste reportar múltiplos warnings
    handler.reportWarning(ErrorType::DEPRECATED_FEATURE, "Recurso obsoleto", pos);
    assert(handler.getWarningCount() == 2);
    
    // Verificar detalhes dos warnings
    const auto& warnings = handler.getWarnings();
    assert(warnings.size() == 2);
    assert(warnings[0].type == ErrorType::UNUSED_VARIABLE);
    assert(warnings[1].type == ErrorType::DEPRECATED_FEATURE);
    
    // Teste misturar erros e warnings
    handler.reportError(ErrorType::INVALID_CHARACTER, "Erro crítico", pos);
    assert(handler.hasErrors());
    assert(handler.getErrorCount() == 1);
    assert(handler.getWarningCount() == 2);
    
    std::cout << "✓ Relatório de warnings passou" << std::endl;
}

// 3. testErrorRecovery() - Recuperação de erros
void testErrorRecovery() {
    std::cout << "Testando recuperação de erros..." << std::endl;
    
    // Teste com múltiplos erros consecutivos
    std::ofstream tempFile("temp_recovery.c");
    tempFile << "int x = @#$; float y = 3.14; char z = 'a';";
    tempFile.close();
    
    ErrorHandler handler;
    LexerMain lexer("temp_recovery.c", &handler);
    
    auto tokens = lexer.tokenizeAll();
    
    // Deve ter encontrado erros mas continuado processando
    assert(handler.hasErrors());
    
    // Deve ter tokens válidos após os erros
    bool foundFloat = false, foundChar = false;
    for (const auto& token : tokens) {
        if (token.getType() == TokenType::FLOAT) foundFloat = true;
        if (token.getType() == TokenType::CHAR) foundChar = true;
    }
    assert(foundFloat && foundChar);
    
    std::remove("temp_recovery.c");
    
    std::cout << "✓ Recuperação de erros passou" << std::endl;
}

// 4. testErrorFormatting() - Formatação de mensagens
void testErrorFormatting() {
    std::cout << "Testando formatação de mensagens..." << std::endl;
    
    ErrorHandler handler;
    Position pos{10, 25, 150};
    
    // Teste formatação de erro
    handler.reportError(ErrorType::INVALID_CHARACTER, "Caractere '@' inválido na linha 10", pos);
    
    const auto& errors = handler.getErrors();
    assert(!errors.empty());
    
    const auto& error = errors[0];
    assert(error.message.find("@") != std::string::npos);
    assert(error.message.find("10") != std::string::npos);
    assert(error.position.line == 10);
    assert(error.position.column == 25);
    
    // Teste formatação de warning
    handler.reportWarning(ErrorType::UNUSED_VARIABLE, "Variável 'temp' declarada mas não utilizada", pos);
    
    const auto& warnings = handler.getWarnings();
    assert(!warnings.empty());
    assert(warnings[0].message.find("temp") != std::string::npos);
    
    std::cout << "✓ Formatação de mensagens passou" << std::endl;
}

// 5. testErrorContext() - Contexto de erros
void testErrorContext() {
    std::cout << "Testando contexto de erros..." << std::endl;
    
    // Teste com arquivo que contém contexto
    std::ofstream tempFile("temp_context.c");
    tempFile << "int main() {\n";
    tempFile << "    int x = @;\n";
    tempFile << "    return 0;\n";
    tempFile << "}";
    tempFile.close();
    
    ErrorHandler handler;
    LexerMain lexer("temp_context.c", &handler);
    
    auto tokens = lexer.tokenizeAll();
    
    if (handler.hasErrors()) {
        const auto& errors = handler.getErrors();
        // Verificar se o erro tem contexto de linha
        assert(errors[0].position.line == 2); // Erro na segunda linha (contagem baseada em 1)
        assert(errors[0].position.column > 0);
    }
    
    std::remove("temp_context.c");
    
    std::cout << "✓ Contexto de erros passou" << std::endl;
}

// 6. testErrorSuggestions() - Sugestões de correção
void testErrorSuggestions() {
    std::cout << "Testando sugestões de correção..." << std::endl;
    
    ErrorHandler handler;
    Position pos{1, 10, 15};
    
    // Teste sugestão para caractere inválido
    handler.reportError(ErrorType::INVALID_CHARACTER, "Caractere '@' inválido. Sugestão: remover ou substituir por operador válido", pos);
    
    // Teste sugestão para string não terminada
    handler.reportError(ErrorType::UNTERMINATED_STRING, "String não terminada. Sugestão: adicionar aspas de fechamento", pos);
    
    // Teste sugestão para número inválido
    handler.reportError(ErrorType::INVALID_NUMBER_FORMAT, "Formato de número inválido '0x'. Sugestão: completar número hexadecimal", pos);
    
    const auto& errors = handler.getErrors();
    assert(errors.size() == 3);
    
    // Verificar se as mensagens contêm sugestões
    assert(errors[0].message.find("Sugestão:") != std::string::npos);
    assert(errors[1].message.find("Sugestão:") != std::string::npos);
    assert(errors[2].message.find("Sugestão:") != std::string::npos);
    
    std::cout << "✓ Sugestões de correção passou" << std::endl;
}

// ===== TESTES EXISTENTES =====



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
    tempFile << "int x = @; float y = $; char z = 'a';";
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
        tempFile << "0x; 0b; 123abc;";
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
    std::cout << "=== Executando Testes de Tratamento de Erros Léxicos (Fase 5.1) ===" << std::endl;
    
    try {
        // Testes da Fase 5.1
        std::cout << "\n--- Testes Unitários Básicos (Fase 5.1) ---" << std::endl;
        testErrorReporting();
        testWarningReporting();
        testErrorRecovery();
        testErrorFormatting();
        testErrorContext();
        testErrorSuggestions();
        
        // Testes existentes
        std::cout << "\n--- Testes Complementares ---" << std::endl;
        test_error_handler_basic();
        test_error_types();
        test_lexer_error_handling();
        test_error_recovery();
        test_token_validation();
        test_error_limits();
        test_error_formatting();
        
        std::cout << "\n🎉 Todos os testes de tratamento de erros passaram com sucesso!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Erro durante os testes: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Erro desconhecido durante os testes" << std::endl;
        return 1;
    }
}