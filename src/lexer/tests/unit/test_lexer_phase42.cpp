// Testes Unitários - Analisador Léxico Fase 4.2
// Testes específicos para reconhecimento de tokens

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

// Teste de reconhecimento de identificadores
void testIdentifierRecognition() {
    std::cout << "Testando reconhecimento de identificadores...";
    
    createTestFile("test_input.c", "variable _var var123 _123var");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve reconhecer 4 identificadores + EOF
        int identifierCount = 0;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::IDENTIFIER) {
                identifierCount++;
            }
        }
        
        if (identifierCount == 4) {
            std::cout << " PASSOU (" << identifierCount << " identificadores)" << std::endl;
        } else {
            std::cout << " FALHOU: Esperado 4, encontrado " << identifierCount << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste de reconhecimento de palavras-chave
void testKeywordRecognition() {
    std::cout << "Testando reconhecimento de palavras-chave...";
    
    createTestFile("test_input.c", "int float if else while for");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve reconhecer palavras-chave específicas
        int keywordCount = 0;
        for (const auto& token : tokens) {
            TokenType type = token.getType();
            if (type == TokenType::INT || type == TokenType::FLOAT || 
                type == TokenType::IF || type == TokenType::ELSE ||
                type == TokenType::WHILE || type == TokenType::FOR) {
                keywordCount++;
            }
        }
        
        if (keywordCount == 6) {
            std::cout << " PASSOU (" << keywordCount << " palavras-chave)" << std::endl;
        } else {
            std::cout << " FALHOU: Esperado 6, encontrado " << keywordCount << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste de reconhecimento de números
void testNumberRecognition() {
    std::cout << "Testando reconhecimento de números...";
    
    createTestFile("test_input.c", "123 456.789 0x1A 0777");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve reconhecer diferentes tipos de números
        int numberCount = 0;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::INTEGER_LITERAL || 
                token.getType() == TokenType::FLOAT_LITERAL) {
                numberCount++;
            }
        }
        
        if (numberCount >= 2) { // Pelo menos int e float
            std::cout << " PASSOU (" << numberCount << " números)" << std::endl;
        } else {
            std::cout << " FALHOU: Esperado pelo menos 2, encontrado " << numberCount << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste de reconhecimento de strings
void testStringRecognition() {
    std::cout << "Testando reconhecimento de strings...";
    
    createTestFile("test_input.c", "\"hello\" \"world\\n\" \"test\"");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve reconhecer strings
        int stringCount = 0;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::STRING_LITERAL) {
                stringCount++;
            }
        }
        
        if (stringCount == 3) {
            std::cout << " PASSOU (" << stringCount << " strings)" << std::endl;
        } else {
            std::cout << " FALHOU: Esperado 3, encontrado " << stringCount << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste de reconhecimento de caracteres
void testCharacterRecognition() {
    std::cout << "Testando reconhecimento de caracteres...";
    
    createTestFile("test_input.c", "'a' 'Z' '\\n' '\\t'");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve reconhecer caracteres literais
        int charCount = 0;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::CHAR_LITERAL) {
                charCount++;
            }
        }
        
        if (charCount == 4) {
            std::cout << " PASSOU (" << charCount << " caracteres)" << std::endl;
        } else {
            std::cout << " FALHOU: Esperado 4, encontrado " << charCount << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste de reconhecimento de operadores
void testOperatorRecognition() {
    std::cout << "Testando reconhecimento de operadores...";
    
    createTestFile("test_input.c", "+ - * / == != <= >= ++ -- += -=");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve reconhecer vários operadores
        int operatorCount = 0;
        for (const auto& token : tokens) {
            TokenType type = token.getType();
            if (type == TokenType::PLUS || type == TokenType::MINUS ||
                type == TokenType::MULTIPLY || type == TokenType::DIVIDE ||
                type == TokenType::EQUAL || type == TokenType::NOT_EQUAL ||
                type == TokenType::LESS_EQUAL || type == TokenType::GREATER_EQUAL ||
                type == TokenType::INCREMENT || type == TokenType::DECREMENT ||
                type == TokenType::PLUS_ASSIGN || type == TokenType::MINUS_ASSIGN) {
                operatorCount++;
            }
        }
        
        if (operatorCount >= 8) {
            std::cout << " PASSOU (" << operatorCount << " operadores)" << std::endl;
        } else {
            std::cout << " FALHOU: Esperado pelo menos 8, encontrado " << operatorCount << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste de reconhecimento de comentários
void testCommentRecognition() {
    std::cout << "Testando reconhecimento de comentários...";
    
    createTestFile("test_input.c", "// linha\n/* bloco */");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve reconhecer comentários
        int commentCount = 0;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::LINE_COMMENT) {
                commentCount++;
            }
        }
        
        if (commentCount >= 1) {
            std::cout << " PASSOU (" << commentCount << " comentários)" << std::endl;
        } else {
            std::cout << " FALHOU: Esperado pelo menos 1, encontrado " << commentCount << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste integrado com código C real
void testRealCCode() {
    std::cout << "Testando código C real...";
    
    createTestFile("test_input.c", 
        "#include <stdio.h>\n"
        "int main() {\n"
        "    int x = 42;\n"
        "    float y = 3.14;\n"
        "    char c = 'A';\n"
        "    // comentário\n"
        "    if (x > 0) {\n"
        "        printf(\"Hello World!\\n\");\n"
        "    }\n"
        "    return 0;\n"
        "}");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve tokenizar sem erros críticos
        bool hasTokens = !tokens.empty();
        bool hasEOF = tokens.back().getType() == TokenType::END_OF_FILE;
        
        if (hasTokens && hasEOF) {
            std::cout << " PASSOU (" << tokens.size() << " tokens total)" << std::endl;
        } else {
            std::cout << " FALHOU: Tokenização incompleta" << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

int main() {
    std::cout << "=== Testes da Fase 4.2 - Reconhecimento de Tokens ===" << std::endl;
    
    testIdentifierRecognition();
    testKeywordRecognition();
    testNumberRecognition();
    testStringRecognition();
    testCharacterRecognition();
    testOperatorRecognition();
    testCommentRecognition();
    testRealCCode();
    
    std::cout << "\n=== Testes da Fase 4.2 concluídos ===" << std::endl;
    
    return 0;
}