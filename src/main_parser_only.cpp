#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <string>
#include "parser/include/parser.hpp"
#include "parser/include/token_stream.hpp"
#include "parser/include/ast_printer.hpp"
#include "lexer/include/token.hpp"

using namespace Parser;
using namespace Lexer;

// Implementação simples de TokenStream para ler de arquivos txt
class TextFileTokenStream : public TokenStream {
private:
    std::vector<Token> tokens;
    size_t currentPos;
    Token eofToken;

public:
    TextFileTokenStream() : currentPos(0), eofToken(TokenType::END_OF_FILE, "", Lexer::Position{0, 0, 0}) {}
    
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Erro: Não foi possível abrir o arquivo " << filename << std::endl;
            return false;
        }
        
        std::string line;
        int lineNum = 1;
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') { // Ignora linhas vazias e comentários
                lineNum++;
                continue;
            }
            
            // Formato: "1. INT                  │ 'int' │ L1:C1 │ Palavras-chave"
            size_t dotPos = line.find('.');
            if (dotPos == std::string::npos) {
                lineNum++;
                continue;
            }
            
            size_t firstPipe = line.find("│", dotPos);
             size_t secondPipe = line.find("│", firstPipe + 1);
             size_t thirdPipe = line.find("│", secondPipe + 1);
            
            if (firstPipe == std::string::npos || secondPipe == std::string::npos || thirdPipe == std::string::npos) {
                lineNum++;
                continue;
            }
            
            // Extrair tipo do token
            std::string tokenTypeStr = line.substr(dotPos + 1, firstPipe - dotPos - 1);
            // Remover espaços
            tokenTypeStr.erase(0, tokenTypeStr.find_first_not_of(" \t"));
            tokenTypeStr.erase(tokenTypeStr.find_last_not_of(" \t") + 1);
            
            // Extrair valor do token (entre aspas simples)
             std::string valueSection = line.substr(firstPipe + 1, secondPipe - firstPipe - 1);
             valueSection.erase(0, valueSection.find_first_not_of(" \t"));
             valueSection.erase(valueSection.find_last_not_of(" \t") + 1);
             
             std::string lexeme;
             // Procurar por aspas simples no valueSection
             size_t firstQuote = valueSection.find('\'');
             size_t lastQuote = valueSection.rfind('\'');
             
             if (firstQuote != std::string::npos && lastQuote != std::string::npos && firstQuote != lastQuote) {
                 lexeme = valueSection.substr(firstQuote + 1, lastQuote - firstQuote - 1);
             } else if (firstQuote != std::string::npos && lastQuote != std::string::npos && firstQuote == lastQuote) {
                 // Caso especial para tokens vazios como END_OF_FILE
                 lexeme = "";
             } else {
                 // Se não encontrar aspas, usar o valor como está
                 lexeme = valueSection;
             }
            
            // Extrair posição (formato L1:C1)
            std::string posSection = line.substr(secondPipe + 1, thirdPipe - secondPipe - 1);
            posSection.erase(0, posSection.find_first_not_of(" \t"));
            posSection.erase(posSection.find_last_not_of(" \t") + 1);
            
            int line_num = 1, col_num = 1;
            if (posSection.size() > 1 && posSection[0] == 'L') {
                size_t colonPos = posSection.find(':');
                if (colonPos != std::string::npos && colonPos + 1 < posSection.size() && posSection[colonPos + 1] == 'C') {
                    line_num = std::stoi(posSection.substr(1, colonPos - 1));
                    col_num = std::stoi(posSection.substr(colonPos + 2));
                }
            }
            
            TokenType type = stringToTokenType(tokenTypeStr);
            if (type != TokenType::UNKNOWN) {
                Lexer::Position pos{line_num, col_num, static_cast<int>(tokens.size())};
                tokens.emplace_back(type, lexeme, pos);
            }
            lineNum++;
        }
        
        // Adiciona token EOF no final
        Lexer::Position eofPos{lineNum, 1, static_cast<int>(tokens.size())};
        tokens.emplace_back(TokenType::END_OF_FILE, "", eofPos);
        
        return true;
    }
    
    const Token& current() const override {
        if (currentPos >= tokens.size()) {
            return eofToken;
        }
        return tokens[currentPos];
    }
    
    const Token& peek(size_t offset = 1) const override {
        size_t peekPos = currentPos + offset;
        if (peekPos >= tokens.size()) {
            return eofToken;
        }
        return tokens[peekPos];
    }
    
    bool advance() override {
        if (currentPos < tokens.size()) {
            currentPos++;
            return true;
        }
        return false;
    }
    
    bool isAtEnd() const override {
        return currentPos >= tokens.size() || tokens[currentPos].getType() == TokenType::END_OF_FILE;
    }
    
    size_t getPosition() const override {
        return currentPos;
    }
    
    void setPosition(size_t position) override {
        currentPos = std::min(position, tokens.size());
    }
    
    size_t size() const override {
        return tokens.size();
    }
    
    const Token& previous(size_t offset = 1) const override {
        if (currentPos < offset) {
            return tokens[0];
        }
        return tokens[currentPos - offset];
    }
    
    std::vector<Token> getRange(size_t start, size_t end) const override {
        if (start >= tokens.size()) return {};
        end = std::min(end, tokens.size());
        return std::vector<Token>(tokens.begin() + start, tokens.begin() + end);
    }
    
private:
    TokenType stringToTokenType(const std::string& str) {
        // Mapeamento básico de strings para TokenType
        if (str == "IDENTIFIER") return TokenType::IDENTIFIER;
        if (str == "INTEGER_LITERAL") return TokenType::INTEGER_LITERAL;
        if (str == "FLOAT_LITERAL") return TokenType::FLOAT_LITERAL;
        if (str == "STRING_LITERAL") return TokenType::STRING_LITERAL;
        if (str == "CHAR_LITERAL") return TokenType::CHAR_LITERAL;
        
        // Palavras-chave
        if (str == "INT") return TokenType::INT;
        if (str == "FLOAT") return TokenType::FLOAT;
        if (str == "DOUBLE") return TokenType::DOUBLE;
        if (str == "CHAR") return TokenType::CHAR;
        if (str == "VOID") return TokenType::VOID;
        if (str == "IF") return TokenType::IF;
        if (str == "ELSE") return TokenType::ELSE;
        if (str == "WHILE") return TokenType::WHILE;
        if (str == "FOR") return TokenType::FOR;
        if (str == "RETURN") return TokenType::RETURN;
        if (str == "STRUCT") return TokenType::STRUCT;
        if (str == "UNION") return TokenType::UNION;
        if (str == "ENUM") return TokenType::ENUM;
        
        // Operadores
        if (str == "ASSIGN") return TokenType::ASSIGN;
        if (str == "PLUS") return TokenType::PLUS;
        if (str == "MINUS") return TokenType::MINUS;
        if (str == "MULTIPLY") return TokenType::MULTIPLY;
        if (str == "DIVIDE") return TokenType::DIVIDE;
        if (str == "MODULO") return TokenType::MODULO;
        
        // Operadores de atribuição
        if (str == "PLUS_ASSIGN") return TokenType::PLUS_ASSIGN;
        if (str == "MINUS_ASSIGN") return TokenType::MINUS_ASSIGN;
        if (str == "MULT_ASSIGN") return TokenType::MULT_ASSIGN;
        if (str == "DIV_ASSIGN") return TokenType::DIV_ASSIGN;
        if (str == "MOD_ASSIGN") return TokenType::MOD_ASSIGN;
        
        // Operadores relacionais e lógicos
        if (str == "EQUAL") return TokenType::EQUAL;
        if (str == "NOT_EQUAL") return TokenType::NOT_EQUAL;
        if (str == "LESS" || str == "LESS_THAN") return TokenType::LESS_THAN;
        if (str == "LESS_EQUAL") return TokenType::LESS_EQUAL;
        if (str == "GREATER" || str == "GREATER_THAN") return TokenType::GREATER_THAN;
        if (str == "GREATER_EQUAL") return TokenType::GREATER_EQUAL;
        if (str == "LOGICAL_AND") return TokenType::LOGICAL_AND;
        if (str == "LOGICAL_OR") return TokenType::LOGICAL_OR;
        if (str == "INCREMENT") return TokenType::INCREMENT;
        if (str == "DECREMENT") return TokenType::DECREMENT;
        
        // Delimitadores
        if (str == "LEFT_PAREN") return TokenType::LEFT_PAREN;
        if (str == "RIGHT_PAREN") return TokenType::RIGHT_PAREN;
        if (str == "LEFT_BRACE") return TokenType::LEFT_BRACE;
        if (str == "RIGHT_BRACE") return TokenType::RIGHT_BRACE;
        if (str == "LEFT_BRACKET") return TokenType::LEFT_BRACKET;
        if (str == "RIGHT_BRACKET") return TokenType::RIGHT_BRACKET;
        if (str == "SEMICOLON") return TokenType::SEMICOLON;
        if (str == "COMMA") return TokenType::COMMA;
        if (str == "DOT") return TokenType::DOT;
        
        return TokenType::UNKNOWN;
    }
};

void printUsage(const char* programName) {
    std::cout << "Uso: " << programName << " <arquivo_tokens.txt>" << std::endl;
    std::cout << "\nFormato do arquivo de tokens:" << std::endl;
    std::cout << "Cada linha deve conter: TIPO_TOKEN lexeme" << std::endl;
    std::cout << "Exemplo:" << std::endl;
    std::cout << "INT int" << std::endl;
    std::cout << "IDENTIFIER main" << std::endl;
    std::cout << "LEFT_PAREN (" << std::endl;
    std::cout << "RIGHT_PAREN )" << std::endl;
    std::cout << "LEFT_BRACE {" << std::endl;
    std::cout << "RETURN return" << std::endl;
    std::cout << "INTEGER_LITERAL 0" << std::endl;
    std::cout << "SEMICOLON ;" << std::endl;
    std::cout << "RIGHT_BRACE }" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string filename = argv[1];
    
    std::cout << "=== Parser Standalone - Análise de Tokens ===" << std::endl;
    std::cout << "Arquivo: " << filename << std::endl << std::endl;
    
    // Carrega tokens do arquivo
    auto tokenStream = std::make_unique<TextFileTokenStream>();
    if (!tokenStream->loadFromFile(filename)) {
        return 1;
    }
    
    std::cout << "Tokens carregados: " << tokenStream->size() << std::endl;
    
    // Lista os tokens carregados
    std::cout << "\n=== Tokens Carregados ===" << std::endl;
    for (size_t i = 0; i < tokenStream->size(); ++i) {
        tokenStream->setPosition(i);
        const Token& token = tokenStream->current();
        std::cout << i << ": " << token.toString() << std::endl;
    }
    tokenStream->setPosition(0); // Reset para o início
    
    // Configurar parser com gramática padrão
    std::cout << "Parser: Criando parser com gramática C89..." << std::endl;
    
    // Usar função de criação padrão
    auto parser = Parser::createParser(Parser::CStandard::C89);
    
    std::cout << "Parser: Parser C89 criado com sucesso" << std::endl;
    if (!parser) {
        std::cerr << "❌ Erro: Falha ao criar parser" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== Parser: Verificando configuração do parser ===" << std::endl;
    std::cout << "Parser criado: " << (parser ? "SIM" : "NÃO") << std::endl;
    
    // Verificar se o parser está configurado corretamente
    std::cout << "Parser: Verificando configuração do parser..." << std::endl;
    try {
        // Tentar verificar se o parser tem métodos de configuração
        std::cout << "Parser: Testando métodos de configuração..." << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Parser: Erro ao verificar configuração: " << e.what() << std::endl;
    }
    
    // Declarar result fora do try para poder usar no return
    bool parseSuccess = false;
    
    // Debug: verificar se o parser está configurado corretamente
    try {
        std::cout << "\n=== Parser: Testando configuração básica ===" << std::endl;
        
        // Verificar se conseguimos acessar informações básicas do parser
        std::cout << "Tentando verificar estado interno do parser..." << std::endl;
        
        std::cout << "\n=== Iniciando Análise Sintática ===" << std::endl;
        
        // Executa o parsing
        std::cout << "Parser: Chamando parseTokens..." << std::endl;
        auto result = parser->parseTokens(std::move(tokenStream));
        std::cout << "Parser: parseTokens retornou" << std::endl;
        
        if (result.isSuccess()) {
            std::cout << "✅ Parsing realizado com sucesso!" << std::endl;
            parseSuccess = true;
            
            // Imprime a AST usando o ASTPrinter
            std::cout << "\n=== Árvore Sintática Abstrata (AST) ===" << std::endl;
            ASTPrinter printer;
            std::string astOutput = printer.print(*result.getValue());
            std::cout << astOutput << std::endl;
            
            // Estatísticas do parser
            std::cout << "\n=== Estatísticas ===" << std::endl;
            std::cout << "AST gerada com sucesso" << std::endl;
            
        } else {
            std::cout << "❌ Erro durante o parsing:" << std::endl;
            parseSuccess = false;
            if (result.getError()) {
                std::cout << "Erro: " << result.getError()->getMessage() << std::endl;
            }
            
            // Lista todos os erros encontrados
            const auto& errors = parser->getErrors();
            if (!errors.empty()) {
                std::cout << "\n=== Erros Encontrados ===" << std::endl;
                for (size_t i = 0; i < errors.size(); ++i) {
                    const auto& error = errors[i];
                    std::cout << "Erro " << (i+1) << ": " << error->getMessage() << std::endl;
                }
            }
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "❌ Erro de argumento inválido: " << e.what() << std::endl;
        std::cerr << "Tipo da exceção: " << typeid(e).name() << std::endl;
        parseSuccess = false;
    } catch (const std::exception& e) {
        std::cerr << "❌ Exceção durante o parsing: " << e.what() << std::endl;
        std::cerr << "Tipo da exceção: " << typeid(e).name() << std::endl;
        parseSuccess = false;
    }
    
    return parseSuccess ? 0 : 1;
}