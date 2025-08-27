#ifndef LEXER_HPP
#define LEXER_HPP

// Analisador Léxico - Arquivo de Cabeçalho Principal
// Implementação da classe Lexer para análise léxica

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include "token.hpp"
#include "error_handler.hpp"
#include "lexer_config.hpp"
#include "lexer_logger.hpp"
#include "lexer_state.hpp"
#include "lookahead_buffer.hpp"
#include "symbol_table.hpp"

namespace Lexer {

/**
 * @class Lexer
 * @brief Analisador léxico principal para linguagem C
 * 
 * Esta classe implementa o analisador léxico (scanner) que converte
 * código fonte C em uma sequência de tokens. Suporta diferentes
 * versões do padrão C (C89, C99, C11, C17, C23).
 */
class LexerMain {
private:
    std::unique_ptr<std::ifstream> sourceFile;     ///< Arquivo fonte sendo analisado
    std::istream* inputStream;                    ///< Stream de entrada (para código de string)
    ErrorHandler* errorHandler;                   ///< Manipulador de erros (não possui)
    std::unique_ptr<LexerConfig> config;          ///< Configurações do lexer
    std::unique_ptr<LexerLogger> logger;          ///< Sistema de logging
    std::unique_ptr<LookaheadBuffer> buffer;      ///< Buffer de lookahead
    std::unique_ptr<SymbolTable> symbolTable;     ///< Tabela de símbolos
    
    LexerState currentState;                      ///< Estado atual do lexer
    size_t currentLine;                           ///< Linha atual no arquivo
    size_t currentColumn;                         ///< Coluna atual no arquivo
    size_t currentPosition;                       ///< Posição absoluta no arquivo
    bool endOfFile;                               ///< Flag indicando fim do arquivo
    
    // Cache para peekToken
    mutable bool hasCachedToken;                  ///< Flag indicando se há token em cache
    mutable Token cachedToken;                    ///< Token em cache para peekToken
    
    // Métodos auxiliares privados
    char readNextChar();                          ///< Lê próximo caractere do buffer
    void skipWhitespace();                        ///< Pula espaços em branco
    void updatePosition(char ch);                 ///< Atualiza posição atual
    Token recognizeToken();                       ///< Reconhece e cria token
    
    // Métodos de reconhecimento de tokens (Fase 4.2)
    Token recognizeIdentifier();                  ///< Reconhece identificadores e palavras-chave
    Token recognizeNumber();                      ///< Reconhece literais numéricos
    Token recognizeString();                      ///< Reconhece literais de string
    Token recognizeCharacter();                   ///< Reconhece literais de caractere
    Token recognizeOperator();                    ///< Reconhece operadores
    Token recognizeComment();                     ///< Reconhece comentários
    void skipComment();                           ///< Pula comentários sem criar tokens
    
    // Métodos de validação
    bool isValidIdentifierStart(char ch) const;   ///< Verifica início válido de identificador
    bool isValidIdentifierChar(char ch) const;    ///< Verifica caractere válido em identificador
    
    // Métodos de tratamento de erros (Fase 4.3)
    void handleError(ErrorType type, const std::string& message, const Lexer::Position& pos);
    Token recoverFromError(ErrorType type, const Lexer::Position& pos);
    bool validateToken(const Token& token) const;
    void synchronizeAfterError();
    
public:
    /**
     * @brief Construtor do LexerMain para arquivo
     * @param filename Nome do arquivo fonte a ser analisado
     * @param errorHandler Ponteiro para o manipulador de erros
     */
    LexerMain(const std::string& filename, ErrorHandler* errorHandler);
    
    /**
     * @brief Construtor do LexerMain para stream de código
     * @param inputStream Stream contendo o código fonte
     * @param errorHandler Ponteiro para o manipulador de erros
     * @param sourceName Nome opcional do arquivo fonte (para debug)
     */
    LexerMain(std::istream& inputStream, ErrorHandler* errorHandler, const std::string& sourceName = "<string>");
    
    /**
     * @brief Destrutor do LexerMain
     * Libera todos os recursos alocados
     */
    ~LexerMain();
    
    // Métodos principais da Fase 4.1
    
    /**
     * @brief Obtém o próximo token da entrada
     * @return Token encontrado ou EOF se fim do arquivo
     */
    Token nextToken();
    
    /**
     * @brief Visualiza o próximo token sem consumi-lo
     * @return Token que seria retornado por nextToken()
     */
    Token peekToken();
    
    /**
     * @brief Tokeniza todo o arquivo de uma vez
     * @return Vetor com todos os tokens encontrados
     */
    std::vector<Token> tokenizeAll();
    
    /**
     * @brief Verifica se ainda há tokens disponíveis
     * @return true se há mais tokens, false caso contrário
     */
    bool hasMoreTokens() const;
    
    /**
     * @brief Reinicializa o lexer para o início do arquivo
     */
    void reset();
    
    /**
     * @brief Obtém a posição atual no arquivo
     * @return Estrutura com linha, coluna e posição absoluta
     */
    struct Position {
        size_t line;
        size_t column;
        size_t absolute;
    };
    Position getCurrentPosition() const;
    
    /**
     * @brief Configura a versão do padrão C a ser usado
     * @param version Versão do padrão (C89, C99, C11, C17, C23)
     */
    void setVersion(const std::string& version);
    
    // Métodos de acesso aos componentes
    ErrorHandler* getErrorHandler() const { return errorHandler; }
    LexerConfig* getConfig() const { return config.get(); }
    LexerLogger* getLogger() const { return logger.get(); }
    SymbolTable* getSymbolTable() const { return symbolTable.get(); }
};

} // namespace Lexer

#endif // LEXER_HPP