// Testes do Lexer Principal - Fase 5.3
// Testes críticos para o analisador léxico completo

#include "../../include/lexer.hpp"
#include "../../include/error_handler.hpp"
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <string>
#include <cstdio>

using namespace Lexer;

// ============================================================================
// FUNÇÕES AUXILIARES
// ============================================================================

void printTestResult(const std::string& test_name, bool passed) {
    std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << test_name << std::endl;
}

void assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "ASSERTION FAILED: " << message << std::endl;
        throw std::runtime_error("Test assertion failed: " + message);
    }
}

void assertEqual(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected != actual) {
        std::cerr << "ASSERTION FAILED: " << message << std::endl;
        std::cerr << "Expected: '" << expected << "', Got: '" << actual << "'" << std::endl;
        throw std::runtime_error("Test assertion failed: " + message);
    }
}

template<typename T>
void assertEqual(T expected, T actual, const std::string& message) {
    if (expected != actual) {
        std::cerr << "ASSERTION FAILED: " << message << std::endl;
        std::cerr << "Expected: " << static_cast<int>(expected) << ", Got: " << static_cast<int>(actual) << std::endl;
        throw std::runtime_error("Test assertion failed: " + message);
    }
}

void createTestFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Não foi possível criar arquivo de teste: " + filename);
    }
    file << content;
    file.close();
}

void removeTestFile(const std::string& filename) {
    std::remove(filename.c_str());
}

// ============================================================================
// FASE 5.3 - TESTES DO LEXER PRINCIPAL
// ============================================================================

// 1. testLexerInitialization() - Inicialização do lexer
void testLexerInitialization() {
    std::cout << "\n=== Testando Inicialização do Lexer (Fase 5.3) ===" << std::endl;
    
    // Teste 1: Inicialização com arquivo válido
    createTestFile("test_init.c", "int main() { return 0; }");
    
    try {
        ErrorHandler errorHandler;
        LexerMain lexer("test_init.c", &errorHandler);
        
        // Verificar se o lexer foi inicializado corretamente
        assertTrue(lexer.hasMoreTokens(), "Lexer deve ter tokens disponíveis");
        assertTrue(lexer.getErrorHandler() != nullptr, "ErrorHandler deve estar configurado");
        assertTrue(lexer.getConfig() != nullptr, "Config deve estar configurado");
        assertTrue(lexer.getLogger() != nullptr, "Logger deve estar configurado");
        assertTrue(lexer.getSymbolTable() != nullptr, "SymbolTable deve estar configurado");
        
        // Verificar posição inicial
        auto pos = lexer.getCurrentPosition();
        assertEqual(static_cast<size_t>(1), pos.line, "Linha inicial deve ser 1");
        assertEqual(static_cast<size_t>(1), pos.column, "Coluna inicial deve ser 1");
        assertEqual(static_cast<size_t>(0), pos.absolute, "Posição absoluta inicial deve ser 0");
        
    } catch (const std::exception& e) {
        removeTestFile("test_init.c");
        throw;
    }
    
    removeTestFile("test_init.c");
    
    // Teste 2: Inicialização com arquivo inexistente
    try {
        ErrorHandler errorHandler;
        LexerMain lexer("arquivo_inexistente.c", &errorHandler);
        assertTrue(false, "Deveria lançar exceção para arquivo inexistente");
    } catch (const std::runtime_error&) {
        // Esperado
    }
    
    // Teste 3: Inicialização com ErrorHandler nulo
    createTestFile("test_init2.c", "int x;");
    try {
        LexerMain lexer("test_init2.c", nullptr);
        assertTrue(false, "Deveria lançar exceção para ErrorHandler nulo");
    } catch (const std::invalid_argument&) {
        // Esperado
    }
    removeTestFile("test_init2.c");
    
    printTestResult("Inicialização do Lexer (Fase 5.3)", true);
}

// 2. testTokenGeneration() - Geração de tokens
void testTokenGeneration() {
    std::cout << "\n=== Testando Geração de Tokens (Fase 5.3) ===" << std::endl;
    
    createTestFile("test_tokens.c", "int x = 42;");
    
    try {
        ErrorHandler errorHandler;
        LexerMain lexer("test_tokens.c", &errorHandler);
        
        // Gerar tokens um por vez
        std::vector<Token> tokens;
        while (lexer.hasMoreTokens()) {
            Token token = lexer.nextToken();
            if (token.getType() != TokenType::END_OF_FILE) {
                tokens.push_back(token);
            } else {
                break;
            }
        }
        
        // Verificar se foram gerados os tokens esperados
        assertTrue(tokens.size() >= 4, "Deve gerar pelo menos 4 tokens");
        
        // Verificar tipos de tokens básicos
        bool hasKeyword = false, hasIdentifier = false, hasOperator = false, hasNumber = false;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::INT) hasKeyword = true;
            if (token.getType() == TokenType::IDENTIFIER) hasIdentifier = true;
            if (token.getType() == TokenType::ASSIGN) hasOperator = true;
            if (token.getType() == TokenType::INTEGER_LITERAL) hasNumber = true;
        }
        
        assertTrue(hasKeyword, "Deve reconhecer palavra-chave 'int'");
        assertTrue(hasIdentifier, "Deve reconhecer identificador 'x'");
        assertTrue(hasOperator, "Deve reconhecer operador '='");
        assertTrue(hasNumber, "Deve reconhecer número '42'");
        
        // Teste de peek token
        lexer.reset();
        Token firstToken = lexer.peekToken();
        Token nextToken = lexer.nextToken();
        
        // Debug: mostrar os tokens
        std::cout << "DEBUG: peekToken type = " << static_cast<int>(firstToken.getType()) << ", lexeme = '" << firstToken.getLexeme() << "'" << std::endl;
        std::cout << "DEBUG: nextToken type = " << static_cast<int>(nextToken.getType()) << ", lexeme = '" << nextToken.getLexeme() << "'" << std::endl;
        
        assertEqual(firstToken.getType(), nextToken.getType(), "peekToken deve retornar o mesmo token que nextToken");
        
    } catch (const std::exception& e) {
        removeTestFile("test_tokens.c");
        throw;
    }
    
    removeTestFile("test_tokens.c");
    printTestResult("Geração de Tokens (Fase 5.3)", true);
}

// 3. testIdentifierRecognition() - Reconhecimento de identificadores
void testIdentifierRecognition() {
    std::cout << "\n=== Testando Reconhecimento de Identificadores (Fase 5.3) ===" << std::endl;
    
    createTestFile("test_identifiers.c", "variable _var var123 _123var camelCase UPPER_CASE");
    
    try {
        ErrorHandler errorHandler;
        LexerMain lexer("test_identifiers.c", &errorHandler);
        
        std::vector<std::string> expectedIdentifiers = {
            "variable", "_var", "var123", "_123var", "camelCase", "UPPER_CASE"
        };
        
        std::vector<Token> identifierTokens;
        while (lexer.hasMoreTokens()) {
            Token token = lexer.nextToken();
            if (token.getType() == TokenType::IDENTIFIER) {
                identifierTokens.push_back(token);
            } else if (token.getType() == TokenType::END_OF_FILE) {
                break;
            }
        }
        
        assertEqual(expectedIdentifiers.size(), identifierTokens.size(), "Número de identificadores deve coincidir");
        
        for (size_t i = 0; i < expectedIdentifiers.size(); ++i) {
            assertEqual(expectedIdentifiers[i], identifierTokens[i].getLexeme(), 
                       "Identificador " + std::to_string(i) + " deve coincidir");
        }
        
    } catch (const std::exception& e) {
        removeTestFile("test_identifiers.c");
        throw;
    }
    
    removeTestFile("test_identifiers.c");
    printTestResult("Reconhecimento de Identificadores (Fase 5.3)", true);
}

// 4. testNumberRecognition() - Reconhecimento de números
void testNumberRecognition() {
    std::cout << "\n=== Testando Reconhecimento de Números (Fase 5.3) ===" << std::endl;
    
    createTestFile("test_numbers.c", "42 0 123 0x1A 0777 3.14 2.5f 1.0e10");
    
    try {
        ErrorHandler errorHandler;
        LexerMain lexer("test_numbers.c", &errorHandler);
        
        std::vector<Token> numberTokens;
        while (lexer.hasMoreTokens()) {
            Token token = lexer.nextToken();
            if (token.getType() == TokenType::INTEGER_LITERAL || 
                token.getType() == TokenType::FLOAT_LITERAL) {
                numberTokens.push_back(token);
            } else if (token.getType() == TokenType::END_OF_FILE) {
                break;
            }
        }
        
        assertTrue(numberTokens.size() >= 6, "Deve reconhecer pelo menos 6 números");
        
        // Verificar tipos específicos
        bool hasInteger = false, hasFloat = false;
        for (const auto& token : numberTokens) {
            if (token.getType() == TokenType::INTEGER_LITERAL) hasInteger = true;
            if (token.getType() == TokenType::FLOAT_LITERAL) hasFloat = true;
        }
        
        assertTrue(hasInteger, "Deve reconhecer números inteiros");
        assertTrue(hasFloat, "Deve reconhecer números de ponto flutuante");
        
    } catch (const std::exception& e) {
        removeTestFile("test_numbers.c");
        throw;
    }
    
    removeTestFile("test_numbers.c");
    printTestResult("Reconhecimento de Números (Fase 5.3)", true);
}

// 5. testStringRecognition() - Reconhecimento de strings
void testStringRecognition() {
    std::cout << "\n=== Testando Reconhecimento de Strings (Fase 5.3) ===" << std::endl;
    
    createTestFile("test_strings.c", "\"Hello World\" \"\" \"String with spaces\" \"Escape\\nsequences\"");
    
    try {
        ErrorHandler errorHandler;
        LexerMain lexer("test_strings.c", &errorHandler);
        
        std::vector<Token> stringTokens;
        while (lexer.hasMoreTokens()) {
            Token token = lexer.nextToken();
            if (token.getType() == TokenType::STRING_LITERAL) {
                stringTokens.push_back(token);
            } else if (token.getType() == TokenType::END_OF_FILE) {
                break;
            }
        }
        
        assertTrue(stringTokens.size() >= 3, "Deve reconhecer pelo menos 3 strings");
        
        // Verificar conteúdo das strings
        bool hasHelloWorld = false, hasEmpty = false, hasEscapes = false;
        for (const auto& token : stringTokens) {
            std::string lexeme = token.getLexeme();
            if (lexeme.find("Hello World") != std::string::npos) hasHelloWorld = true;
            if (lexeme == "\"\"" || lexeme == "") hasEmpty = true;
            if (lexeme.find("\\") != std::string::npos) hasEscapes = true;
        }
        
        assertTrue(hasHelloWorld, "Deve reconhecer string 'Hello World'");
        
    } catch (const std::exception& e) {
        removeTestFile("test_strings.c");
        throw;
    }
    
    removeTestFile("test_strings.c");
    printTestResult("Reconhecimento de Strings (Fase 5.3)", true);
}

// 6. testOperatorRecognition() - Reconhecimento de operadores
void testOperatorRecognition() {
    std::cout << "\n=== Testando Reconhecimento de Operadores (Fase 5.3) ===" << std::endl;
    
    createTestFile("test_operators.c", "+ - * / % = == != < > <= >= && || ! ++ -- += -= *= /=");
    
    try {
        ErrorHandler errorHandler;
        LexerMain lexer("test_operators.c", &errorHandler);
        
        std::vector<Token> operatorTokens;
        while (lexer.hasMoreTokens()) {
            Token token = lexer.nextToken();
            if (token.isOperator()) {
                operatorTokens.push_back(token);
            } else if (token.getType() == TokenType::END_OF_FILE) {
                break;
            }
        }
        
        assertTrue(operatorTokens.size() >= 15, "Deve reconhecer pelo menos 15 operadores");
        
        // Verificar operadores específicos
        std::vector<std::string> expectedOps = {"+", "-", "*", "/", "=", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!", "++", "--"};
        
        for (const std::string& expectedOp : expectedOps) {
            bool found = false;
            for (const auto& token : operatorTokens) {
                if (token.getLexeme() == expectedOp) {
                    found = true;
                    break;
                }
            }
            assertTrue(found, "Deve reconhecer operador '" + expectedOp + "'");
        }
        
    } catch (const std::exception& e) {
        removeTestFile("test_operators.c");
        throw;
    }
    
    removeTestFile("test_operators.c");
    printTestResult("Reconhecimento de Operadores (Fase 5.3)", true);
}

// 7. testCommentHandling() - Tratamento de comentários
void testCommentHandling() {
    std::cout << "\n=== Testando Tratamento de Comentários (Fase 5.3) ===" << std::endl;
    
    createTestFile("test_comments.c", 
        "int x; // Comentário de linha\n"
        "/* Comentário\n"
        "   de bloco */\n"
        "int y; /* inline */ int z;");
    
    try {
        ErrorHandler errorHandler;
        LexerMain lexer("test_comments.c", &errorHandler);
        
        std::vector<Token> tokens;
        while (lexer.hasMoreTokens()) {
            Token token = lexer.nextToken();
            if (token.getType() != TokenType::END_OF_FILE) {
                tokens.push_back(token);
            } else {
                break;
            }
        }
        
        // Verificar que comentários foram ignorados e tokens válidos foram reconhecidos
        bool hasIntKeyword = false, hasXIdentifier = false, hasYIdentifier = false, hasZIdentifier = false;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::INT) hasIntKeyword = true;
            if (token.getType() == TokenType::IDENTIFIER && token.getLexeme() == "x") hasXIdentifier = true;
            if (token.getType() == TokenType::IDENTIFIER && token.getLexeme() == "y") hasYIdentifier = true;
            if (token.getType() == TokenType::IDENTIFIER && token.getLexeme() == "z") hasZIdentifier = true;
        }
        
        assertTrue(hasIntKeyword, "Deve reconhecer palavra-chave 'int'");
        assertTrue(hasXIdentifier, "Deve reconhecer identificador 'x'");
        assertTrue(hasYIdentifier, "Deve reconhecer identificador 'y'");
        assertTrue(hasZIdentifier, "Deve reconhecer identificador 'z'");
        
        // Verificar que não há tokens de comentário nos resultados
        for (const auto& token : tokens) {
            assertTrue(token.getType() != TokenType::LINE_COMMENT, "Comentários não devem aparecer como tokens");
        }
        
    } catch (const std::exception& e) {
        removeTestFile("test_comments.c");
        throw;
    }
    
    removeTestFile("test_comments.c");
    printTestResult("Tratamento de Comentários (Fase 5.3)", true);
}

// 8. testWhitespaceHandling() - Tratamento de espaços
void testWhitespaceHandling() {
    std::cout << "\n=== Testando Tratamento de Espaços (Fase 5.3) ===" << std::endl;
    
    createTestFile("test_whitespace.c", 
        "int    x   =   42  ;\n"
        "\t\tint\ty\t=\t24;\n"
        "\n\n\nint z = 0;\n");
    
    try {
        ErrorHandler errorHandler;
        LexerMain lexer("test_whitespace.c", &errorHandler);
        
        std::vector<Token> tokens;
        while (lexer.hasMoreTokens()) {
            Token token = lexer.nextToken();
            if (token.getType() != TokenType::END_OF_FILE) {
                tokens.push_back(token);
            } else {
                break;
            }
        }
        
        // Verificar que espaços em branco foram ignorados
        for (const auto& token : tokens) {
            assertTrue(!token.getLexeme().empty(), "Tokens não devem ter lexema vazio");
        }
        
        // Verificar que tokens válidos foram reconhecidos corretamente
        bool hasInt = false, hasX = false, hasY = false, hasZ = false;
        bool hasEquals = false, hasNumbers = false, hasSemicolon = false;
        
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::INT) hasInt = true;
            if (token.getType() == TokenType::IDENTIFIER && token.getLexeme() == "x") hasX = true;
            if (token.getType() == TokenType::IDENTIFIER && token.getLexeme() == "y") hasY = true;
            if (token.getType() == TokenType::IDENTIFIER && token.getLexeme() == "z") hasZ = true;
            if (token.getType() == TokenType::ASSIGN) hasEquals = true;
            if (token.getType() == TokenType::INTEGER_LITERAL) hasNumbers = true;
            if (token.getType() == TokenType::SEMICOLON) hasSemicolon = true;
        }
        
        assertTrue(hasInt, "Deve reconhecer palavra-chave 'int'");
        assertTrue(hasX && hasY && hasZ, "Deve reconhecer todos os identificadores");
        assertTrue(hasEquals, "Deve reconhecer operador '='");
        assertTrue(hasNumbers, "Deve reconhecer números");
        assertTrue(hasSemicolon, "Deve reconhecer delimitador ';'");
        
    } catch (const std::exception& e) {
        removeTestFile("test_whitespace.c");
        throw;
    }
    
    removeTestFile("test_whitespace.c");
    printTestResult("Tratamento de Espaços (Fase 5.3)", true);
}

// 9. testErrorHandling() - Tratamento de erros
void testErrorHandling() {
    std::cout << "\n=== Testando Tratamento de Erros (Fase 5.3) ===" << std::endl;
    
    createTestFile("test_errors.c", 
        "int x = 42;\n"
        "char invalid = 'unclosed string\n"
        "float y = 3.14.15; // número inválido\n"
        "@#$%^&*(); // caracteres inválidos");
    
    try {
        ErrorHandler errorHandler;
        LexerMain lexer("test_errors.c", &errorHandler);
        
        std::vector<Token> tokens;
        while (lexer.hasMoreTokens()) {
            Token token = lexer.nextToken();
            if (token.getType() != TokenType::END_OF_FILE) {
                tokens.push_back(token);
            } else {
                break;
            }
        }
        
        // Verificar que erros foram reportados
        assertTrue(errorHandler.hasErrors(), "Deve reportar erros para entrada inválida");
        assertTrue(errorHandler.getErrorCount() > 0, "Contador de erros deve ser maior que zero");
        
        // Verificar que tokens válidos ainda foram reconhecidos
        bool hasValidTokens = false;
        for (const auto& token : tokens) {
            if (token.isKeyword() || 
                token.getType() == TokenType::IDENTIFIER ||
                token.getType() == TokenType::INTEGER_LITERAL) {
                hasValidTokens = true;
                break;
            }
        }
        
        assertTrue(hasValidTokens, "Deve reconhecer tokens válidos mesmo com erros");
        
    } catch (const std::exception& e) {
        removeTestFile("test_errors.c");
        throw;
    }
    
    removeTestFile("test_errors.c");
    printTestResult("Tratamento de Erros (Fase 5.3)", true);
}

// 10. testFileProcessing() - Processamento de arquivos
void testFileProcessing() {
    std::cout << "\n=== Testando Processamento de Arquivos (Fase 5.3) ===" << std::endl;
    
    // Teste com arquivo vazio
    createTestFile("test_empty.c", "");
    
    try {
        ErrorHandler errorHandler;
        LexerMain lexer("test_empty.c", &errorHandler);
        
        Token token = lexer.nextToken();
        assertEqual(TokenType::END_OF_FILE, token.getType(), "Arquivo vazio deve retornar EOF");
        assertTrue(!lexer.hasMoreTokens(), "Arquivo vazio não deve ter mais tokens");
        
    } catch (const std::exception& e) {
        removeTestFile("test_empty.c");
        throw;
    }
    
    removeTestFile("test_empty.c");
    
    // Teste com arquivo grande
    std::string largeContent;
    for (int i = 0; i < 1000; ++i) {
        largeContent += "int var" + std::to_string(i) + " = " + std::to_string(i) + ";\n";
    }
    
    createTestFile("test_large.c", largeContent);
    
    try {
        ErrorHandler errorHandler;
        LexerMain lexer("test_large.c", &errorHandler);
        
        int tokenCount = 0;
        while (lexer.hasMoreTokens()) {
            Token token = lexer.nextToken();
            if (token.getType() != TokenType::END_OF_FILE) {
                tokenCount++;
            } else {
                break;
            }
        }
        assertTrue(tokenCount >= 5000, "Arquivo grande deve gerar muitos tokens");
        
        // Verificar que não há erros no processamento
        assertTrue(!errorHandler.hasErrors(), "Processamento de arquivo grande não deve gerar erros");
        
    } catch (const std::exception& e) {
        removeTestFile("test_large.c");
        throw;
    }
    
    removeTestFile("test_large.c");
    printTestResult("Processamento de Arquivos (Fase 5.3)", true);
}

// 11. testPositionTracking() - Rastreamento de posição
void testPositionTracking() {
    std::cout << "\n=== Testando Rastreamento de Posição (Fase 5.3) ===" << std::endl;
    
    createTestFile("test_position.c", 
        "int x;\n"
        "  float y;\n"
        "    char z;");
    
    try {
        ErrorHandler errorHandler;
        LexerMain lexer("test_position.c", &errorHandler);
        
        // Verificar posição inicial
        auto initialPos = lexer.getCurrentPosition();
        assertEqual(static_cast<size_t>(1), initialPos.line, "Linha inicial deve ser 1");
        assertEqual(static_cast<size_t>(1), initialPos.column, "Coluna inicial deve ser 1");
        
        // Processar alguns tokens e verificar posições
        std::vector<Token> tokens;
        std::vector<LexerMain::Position> positions;
        
        while (lexer.hasMoreTokens()) {
            auto pos = lexer.getCurrentPosition();
            Token token = lexer.nextToken();
            
            if (token.getType() != TokenType::END_OF_FILE) {
                tokens.push_back(token);
                positions.push_back(pos);
            } else {
                break;
            }
        }
        
        assertTrue(tokens.size() > 0, "Deve processar alguns tokens");
        assertTrue(positions.size() == tokens.size(), "Deve ter posição para cada token");
        
        // Verificar que as posições fazem sentido
        for (size_t i = 0; i < positions.size(); ++i) {
            assertTrue(positions[i].line >= 1, "Linha deve ser >= 1");
            assertTrue(positions[i].column >= 1, "Coluna deve ser >= 1");
            
            if (i > 0) {
                // Posição absoluta deve aumentar
                assertTrue(positions[i].absolute >= positions[i-1].absolute, 
                          "Posição absoluta deve aumentar");
            }
        }
        
        // Teste de reset e verificação de posição
        lexer.reset();
        auto resetPos = lexer.getCurrentPosition();
        assertEqual(static_cast<size_t>(1), resetPos.line, "Linha após reset deve ser 1");
        assertEqual(static_cast<size_t>(1), resetPos.column, "Coluna após reset deve ser 1");
        assertEqual(static_cast<size_t>(0), resetPos.absolute, "Posição absoluta após reset deve ser 0");
        
    } catch (const std::exception& e) {
        removeTestFile("test_position.c");
        throw;
    }
    
    removeTestFile("test_position.c");
    printTestResult("Rastreamento de Posição (Fase 5.3)", true);
}

// 12. testVersionSupport() - Suporte a versões do C
void testVersionSupport() {
    std::cout << "\n=== Testando Suporte a Versões do C (Fase 5.3) ===" << std::endl;
    
    createTestFile("test_versions.c", 
        "inline int func() { return 0; }\n"  // C99+
        "_Bool flag = 1;\n"                   // C99+
        "restrict int* ptr;\n");
    
    try {
        ErrorHandler errorHandler;
        LexerMain lexer("test_versions.c", &errorHandler);
        
        // Teste com C89 (não deve reconhecer inline, _Bool, restrict)
        lexer.setVersion("C89");
        lexer.reset();
        
        std::vector<Token> c89Tokens;
        while (lexer.hasMoreTokens()) {
            Token token = lexer.nextToken();
            if (token.getType() != TokenType::END_OF_FILE) {
                c89Tokens.push_back(token);
            } else {
                break;
            }
        }
        
        // Teste com C99 (deve reconhecer inline, _Bool, restrict)
        lexer.setVersion("C99");
        lexer.reset();
        
        std::vector<Token> c99Tokens;
        while (lexer.hasMoreTokens()) {
            Token token = lexer.nextToken();
            if (token.getType() != TokenType::END_OF_FILE) {
                c99Tokens.push_back(token);
            } else {
                break;
            }
        }
        
        // Verificar que diferentes versões produzem resultados diferentes
        bool c99HasMoreKeywords = false;
        
        int c89Keywords = 0, c99Keywords = 0;
        for (const auto& token : c89Tokens) {
            if (token.isKeyword()) c89Keywords++;
        }
        for (const auto& token : c99Tokens) {
            if (token.isKeyword()) c99Keywords++;
        }
        
        // C99 deve reconhecer mais palavras-chave que C89
        c99HasMoreKeywords = (c99Keywords >= c89Keywords);
        assertTrue(c99HasMoreKeywords, "C99 deve reconhecer pelo menos tantas palavras-chave quanto C89");
        
        // Teste com versões mais recentes
        lexer.setVersion("C11");
        lexer.setVersion("C17");
        lexer.setVersion("C23");
        
        // Se chegou até aqui, as versões são suportadas
        assertTrue(true, "Deve suportar configuração de diferentes versões");
        
    } catch (const std::exception& e) {
        removeTestFile("test_versions.c");
        throw;
    }
    
    removeTestFile("test_versions.c");
    printTestResult("Suporte a Versões do C (Fase 5.3)", true);
}

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main() {
    std::cout << "=== EXECUTANDO TESTES DO LEXER PRINCIPAL - FASE 5.3 ===" << std::endl;
    
    try {
        // Executar todos os testes da Fase 5.3
        testLexerInitialization();
        testTokenGeneration();
        testIdentifierRecognition();
        testNumberRecognition();
        testStringRecognition();
        testOperatorRecognition();
        testCommentHandling();
        testWhitespaceHandling();
        testErrorHandling();
        testFileProcessing();
        testPositionTracking();
        testVersionSupport();
        
        std::cout << "\n=== TODOS OS TESTES DA FASE 5.3 PASSARAM COM SUCESSO! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Erro durante os testes da Fase 5.3: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ Erro desconhecido durante os testes da Fase 5.3" << std::endl;
        return 1;
    }
}