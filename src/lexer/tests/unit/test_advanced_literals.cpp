// Testes para literais avançados da linguagem C
// Verifica reconhecimento de tipos long long, sufixos numéricos e literais wide

#include "../../include/lexer.hpp"
#include "../../include/error_handler.hpp"
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <cstdio>

using namespace Lexer;

// Função auxiliar para criar arquivo de teste
void createTestFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    file << content;
    file.close();
}

// Teste de literais inteiros com sufixos
void testIntegerSuffixes() {
    std::cout << "Testando literais inteiros com sufixos...";
    
    createTestFile("test_input.c", "123L 456LL 789U 101UL 202ULL 0x1AFL 0777LL");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve reconhecer todos os literais inteiros com sufixos
        int integerCount = 0;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::INTEGER_LITERAL) {
                integerCount++;
                std::string lexeme = token.getLexeme();
                // Verificar se contém sufixos válidos
                if (lexeme.find('L') != std::string::npos || 
                    lexeme.find('U') != std::string::npos) {
                    // Sufixo encontrado - OK
                }
            }
        }
        
        if (integerCount >= 7) {
            std::cout << " PASSOU (" << integerCount << " literais inteiros)" << std::endl;
        } else {
            std::cout << " FALHOU: Esperado 7, encontrado " << integerCount << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste de literais float com sufixos
void testFloatSuffixes() {
    std::cout << "Testando literais float com sufixos...";
    
    createTestFile("test_input.c", "3.14f 2.71F 1.23L 4.56e10f 7.89E-5L");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve reconhecer todos os literais float com sufixos
        int floatCount = 0;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::FLOAT_LITERAL) {
                floatCount++;
            }
        }
        
        if (floatCount >= 5) {
            std::cout << " PASSOU (" << floatCount << " literais float)" << std::endl;
        } else {
            std::cout << " FALHOU: Esperado 5, encontrado " << floatCount << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste de tipos long long (C99+)
void testLongLongTypes() {
    std::cout << "Testando tipos long long...";
    
    createTestFile("test_input.c", "long long x; unsigned long long y; long long int z;");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve reconhecer palavras-chave long, unsigned
        int longCount = 0;
        int unsignedCount = 0;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::LONG && token.getLexeme() == "long") {
                longCount++;
            }
            if (token.getType() == TokenType::UNSIGNED && token.getLexeme() == "unsigned") {
                unsignedCount++;
            }
        }
        
        if (longCount >= 4 && unsignedCount >= 1) {
            std::cout << " PASSOU (" << longCount << " long, " << unsignedCount << " unsigned)" << std::endl;
        } else {
            std::cout << " FALHOU: Esperado 4+ long e 1+ unsigned, encontrado " << longCount << " long, " << unsignedCount << " unsigned" << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste de literais wide characters (C99+)
void testWideCharacters() {
    std::cout << "Testando literais wide characters...";
    
    createTestFile("test_input.c", "L'A' u'B' U'C' u8'D'");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve reconhecer caracteres wide
        int charCount = 0;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::CHAR_LITERAL) {
                charCount++;
            }
        }
        
        if (charCount >= 4) {
            std::cout << " PASSOU (" << charCount << " caracteres wide)" << std::endl;
        } else {
            std::cout << " FALHOU: Esperado 4, encontrado " << charCount << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste de literais wide strings (C99+)
void testWideStrings() {
    std::cout << "Testando literais wide strings...";
    
    createTestFile("test_input.c", "L\"hello\" u\"world\" U\"test\" u8\"utf8\"");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve reconhecer strings wide
        int stringCount = 0;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::STRING_LITERAL) {
                stringCount++;
            }
        }
        
        if (stringCount >= 4) {
            std::cout << " PASSOU (" << stringCount << " strings wide)" << std::endl;
        } else {
            std::cout << " FALHOU: Esperado 4, encontrado " << stringCount << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste de literais binários (C23)
void testBinaryLiterals() {
    std::cout << "Testando literais binários (C23)...";
    
    createTestFile("test_input.c", "0b1010 0B1111 0b101010LL 0B11UL");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve reconhecer literais binários
        int binaryCount = 0;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::INTEGER_LITERAL) {
                std::string lexeme = token.getLexeme();
                if (lexeme.find("0b") == 0 || lexeme.find("0B") == 0) {
                    binaryCount++;
                }
            }
        }
        
        if (binaryCount >= 4) {
            std::cout << " PASSOU (" << binaryCount << " literais binários)" << std::endl;
        } else {
            std::cout << " FALHOU: Esperado 4, encontrado " << binaryCount << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

// Teste de notação científica complexa
void testScientificNotation() {
    std::cout << "Testando notação científica complexa...";
    
    createTestFile("test_input.c", "1.23e10 4.56E-5 7.89e+12f 2.71E20L 3.14e0");
    
    try {
        ErrorHandler* errorHandler = new ErrorHandler();
        LexerMain lexer("test_input.c", errorHandler);
        
        std::vector<Token> tokens = lexer.tokenizeAll();
        
        // Deve reconhecer notação científica
        int scientificCount = 0;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::FLOAT_LITERAL) {
                std::string lexeme = token.getLexeme();
                if (lexeme.find('e') != std::string::npos || lexeme.find('E') != std::string::npos) {
                    scientificCount++;
                }
            }
        }
        
        if (scientificCount >= 5) {
            std::cout << " PASSOU (" << scientificCount << " notações científicas)" << std::endl;
        } else {
            std::cout << " FALHOU: Esperado 5, encontrado " << scientificCount << std::endl;
        }
        
        delete errorHandler;
    } catch (const std::exception& e) {
        std::cout << " FALHOU: " << e.what() << std::endl;
    }
    
    std::remove("test_input.c");
}

int main() {
    std::cout << "=== Testes de Literais Avançados ===" << std::endl;
    
    testIntegerSuffixes();
    testFloatSuffixes();
    testLongLongTypes();
    testWideCharacters();
    testWideStrings();
    testBinaryLiterals();
    testScientificNotation();
    
    std::cout << "\n=== Testes Concluídos ===" << std::endl;
    return 0;
}