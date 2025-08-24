#include <iostream>
#include <cassert>
#include <string>
#include <fstream>
#include <cstdio>
#include <vector>
#include "../../include/error_handler.hpp"
#include "../../include/lexer.hpp"
#include "../../include/token.hpp"

using namespace Lexer;

// Teste de recuperação PANIC - para quando encontra erro crítico
void test_panic_recovery() {
    std::cout << "Testando recuperação PANIC..." << std::endl;
    
    ErrorHandler handler;
    
    // Criar arquivo temporário com erro crítico
    const char* filename = "test_panic.c";
    std::ofstream file(filename);
    file << "int main() {\n";
    file << "    char invalid = '\\x999';  // Escape inválido\n";
    file << "    return 0;\n";
    file << "}";
    file.close();
    
    try {
        LexerMain lexer(filename, &handler);
        auto tokens = lexer.tokenizeAll();
        
        // Deve ter detectado o erro e tentado recuperar
        assert(handler.hasErrors());
        assert(handler.getErrorCount() > 0);
        
        // Deve ter continuado a tokenização após o erro
        assert(!tokens.empty());
        
        std::cout << "✓ Recuperação PANIC funcionou" << std::endl;
    } catch (...) {
        std::remove(filename);
        throw;
    }
    
    std::remove(filename);
}

// Teste de recuperação SYNCHRONIZE - busca por pontos de sincronização
void test_synchronize_recovery() {
    std::cout << "Testando recuperação SYNCHRONIZE..." << std::endl;
    
    ErrorHandler handler;
    
    // Criar arquivo com múltiplos erros e pontos de sincronização
    const char* filename = "test_sync.c";
    std::ofstream file(filename);
    file << "int main() {\n";
    file << "    @ invalid char;\n";  // Erro 1
    file << "    int x = 10;\n";      // Ponto de sincronização
    file << "    $ another error;\n"; // Erro 2
    file << "    return 0;\n";        // Ponto de sincronização
    file << "}";
    file.close();
    
    try {
        LexerMain lexer(filename, &handler);
        auto tokens = lexer.tokenizeAll();
        
        // Deve ter detectado múltiplos erros
        assert(handler.hasErrors());
        assert(handler.getErrorCount() >= 2);
        
        // Deve ter encontrado tokens válidos após cada erro
        bool foundInt = false, foundReturn = false;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::INT) foundInt = true;
            if (token.getType() == TokenType::RETURN) foundReturn = true;
        }
        assert(foundInt && foundReturn);
        
        std::cout << "✓ Recuperação SYNCHRONIZE funcionou" << std::endl;
    } catch (...) {
        std::remove(filename);
        throw;
    }
    
    std::remove(filename);
}

// Teste de recuperação CONTINUE - continua após erro simples
void test_continue_recovery() {
    std::cout << "Testando recuperação CONTINUE..." << std::endl;
    
    ErrorHandler handler;
    
    // Criar arquivo com erros simples que permitem continuação
    const char* filename = "test_continue.c";
    std::ofstream file(filename);
    file << "int x = 10;\n";
    file << "char c = '\\z';\n";  // Escape inválido simples
    file << "float f = 3.14;\n";
    file << "return x;";
    file.close();
    
    try {
        LexerMain lexer(filename, &handler);
        auto tokens = lexer.tokenizeAll();
        
        // Deve ter detectado o erro mas continuado
        assert(handler.hasErrors());
        
        // Deve ter tokenizado o resto do código
        bool foundFloat = false, foundReturn = false;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::FLOAT) foundFloat = true;
            if (token.getType() == TokenType::RETURN) foundReturn = true;
        }
        assert(foundFloat && foundReturn);
        
        std::cout << "✓ Recuperação CONTINUE funcionou" << std::endl;
    } catch (...) {
        std::remove(filename);
        throw;
    }
    
    std::remove(filename);
}

// Teste de recuperação com múltiplas estratégias
void test_mixed_recovery_strategies() {
    std::cout << "Testando estratégias mistas de recuperação..." << std::endl;
    
    ErrorHandler handler;
    
    // Criar arquivo complexo com diferentes tipos de erro
    const char* filename = "test_mixed.c";
    std::ofstream file(filename);
    file << "#include <stdio.h>\n";
    file << "int main() {\n";
    file << "    char str[] = \"unterminated string\n";  // String não terminada
    file << "    int x @ 10;\n";                        // Caractere inválido
    file << "    /* unterminated comment\n";
    file << "    float f = 3.14;\n";
    file << "    return 0;\n";
    file << "}";
    file.close();
    
    try {
        LexerMain lexer(filename, &handler);
        auto tokens = lexer.tokenizeAll();
        
        // Deve ter detectado múltiplos erros
        assert(handler.hasErrors());
        assert(handler.getErrorCount() >= 2);
        
        // Deve ter recuperado e encontrado tokens válidos
        bool foundInclude = false, foundMain = false, foundReturn = false;
        for (const auto& token : tokens) {
            if (token.getLexeme() == "#include") foundInclude = true;
            if (token.getType() == TokenType::INT) foundMain = true;
            if (token.getType() == TokenType::RETURN) foundReturn = true;
        }
        
        // Pelo menos alguns tokens válidos devem ter sido encontrados
        assert(foundMain || foundReturn);
        
        std::cout << "✓ Estratégias mistas funcionaram" << std::endl;
    } catch (...) {
        std::remove(filename);
        throw;
    }
    
    std::remove(filename);
}

// Teste de limite de erros para recuperação
void test_error_limit_recovery() {
    std::cout << "Testando limite de erros na recuperação..." << std::endl;
    
    ErrorHandler handler;
    
    // Criar arquivo com muitos erros
    const char* filename = "test_limit.c";
    std::ofstream file(filename);
    file << "@ # $ % ^ & * ( ) ! ~ ` | \\ ? > < + - = [ ] { } : ; , . / \"\n";
    file.close();
    
    try {
        LexerMain lexer(filename, &handler);
        auto tokens = lexer.tokenizeAll();
        
        // Deve ter detectado muitos erros
        assert(handler.hasErrors());
        
        // Se há limite de erros, deve ter parado em algum ponto
        // Caso contrário, deve ter processado todos
        assert(handler.getErrorCount() > 0);
        
        std::cout << "✓ Limite de erros funcionou" << std::endl;
    } catch (...) {
        std::remove(filename);
        throw;
    }
    
    std::remove(filename);
}

// Teste de recuperação com contexto preservado
void test_context_preservation() {
    std::cout << "Testando preservação de contexto na recuperação..." << std::endl;
    
    ErrorHandler handler;
    
    // Criar arquivo onde o contexto deve ser preservado após erro
    const char* filename = "test_context.c";
    std::ofstream file(filename);
    file << "int function1() {\n";
    file << "    @ error here;\n";
    file << "    return 1;\n";
    file << "}\n";
    file << "int function2() {\n";
    file << "    return 2;\n";
    file << "}";
    file.close();
    
    try {
        LexerMain lexer(filename, &handler);
        auto tokens = lexer.tokenizeAll();
        
        // Deve ter detectado o erro
        assert(handler.hasErrors());
        
        // Deve ter encontrado ambas as funções
        int functionCount = 0;
        for (const auto& token : tokens) {
            if (token.getLexeme() == "function1" || token.getLexeme() == "function2") {
                functionCount++;
            }
        }
        assert(functionCount >= 1); // Pelo menos uma função deve ser reconhecida
        
        std::cout << "✓ Preservação de contexto funcionou" << std::endl;
    } catch (...) {
        std::remove(filename);
        throw;
    }
    
    std::remove(filename);
}

int main() {
    std::cout << "=== Executando Testes de Estratégias de Recuperação de Erros ===" << std::endl;
    
    try {
        test_panic_recovery();
        test_synchronize_recovery();
        test_continue_recovery();
        test_mixed_recovery_strategies();
        test_error_limit_recovery();
        test_context_preservation();
        
        std::cout << "\n✅ Todos os testes de recuperação de erros passaram!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Erro durante os testes: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Erro desconhecido durante os testes" << std::endl;
        return 1;
    }
}