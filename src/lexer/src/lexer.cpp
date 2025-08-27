// Analisador Léxico - Implementação Principal
// Implementação da classe LexerMain para análise léxica

#include "../include/lexer.hpp"
#include <stdexcept>
#include <iostream>

namespace Lexer {

// Implementação do construtor para arquivo
LexerMain::LexerMain(const std::string& filename, ErrorHandler* errorHandler)
    : currentState(LexerState::START)
    , currentLine(1)
    , currentColumn(1)
    , currentPosition(0)
    , endOfFile(false)
    , hasCachedToken(false)
    , cachedToken(TokenType::UNKNOWN, "", {0, 0, 0})
    , inputStream(nullptr)
{
    // Validar parâmetros
    if (filename.empty()) {
        throw std::invalid_argument("Nome do arquivo não pode estar vazio");
    }
    
    if (errorHandler == nullptr) {
        throw std::invalid_argument("ErrorHandler não pode ser nulo");
    }
    
    // Inicializar arquivo fonte
    sourceFile = std::make_unique<std::ifstream>(filename);
    if (!sourceFile->is_open()) {
        throw std::runtime_error("Não foi possível abrir o arquivo: " + filename);
    }
    
    // Inicializar componentes
    this->errorHandler = errorHandler; // Não tomar posse do ponteiro
    config = std::make_unique<LexerConfig>();
    logger = std::make_unique<LexerLogger>();
    buffer = std::make_unique<LookaheadBuffer>(*sourceFile);
    symbolTable = std::make_unique<SymbolTable>(config.get());
    
    // Log de inicialização
    logger->logStateTransition("INIT", "START");
}

// Implementação do construtor para stream
LexerMain::LexerMain(std::istream& inputStream, ErrorHandler* errorHandler, const std::string& sourceName)
    : currentState(LexerState::START)
    , currentLine(1)
    , currentColumn(1)
    , currentPosition(0)
    , endOfFile(false)
    , hasCachedToken(false)
    , cachedToken(TokenType::UNKNOWN, "", {0, 0, 0})
    , inputStream(&inputStream)
{
    // Validar parâmetros
    if (errorHandler == nullptr) {
        throw std::invalid_argument("ErrorHandler não pode ser nulo");
    }
    
    // Inicializar componentes
    this->errorHandler = errorHandler; // Não tomar posse do ponteiro
    config = std::make_unique<LexerConfig>();
    logger = std::make_unique<LexerLogger>();
    buffer = std::make_unique<LookaheadBuffer>(inputStream);
    symbolTable = std::make_unique<SymbolTable>(config.get());
    
    // Log de inicialização
    logger->logStateTransition("INIT", "START");
}

// Implementação do destrutor
LexerMain::~LexerMain() {
    if (logger) {
        logger->logStateTransition(lexerStateToString(currentState), "DESTROYED");
    }
    
    // Fechar arquivo se ainda estiver aberto
    if (sourceFile && sourceFile->is_open()) {
        sourceFile->close();
    }
    
    // Os unique_ptr se encarregam da limpeza automática
}

// Implementação dos métodos auxiliares privados
char LexerMain::readNextChar() {
    if (endOfFile) {
        return '\0';
    }
    
    char ch = buffer->consume();
    if (ch == '\0') {
        endOfFile = true;
        return '\0';
    }
    
    updatePosition(ch);
    return ch;
}

void LexerMain::skipWhitespace() {
    char ch;
    while ((ch = buffer->peek()) != '\0' && std::isspace(ch)) {
        readNextChar(); // Consome o caractere de espaço
    }
}

void LexerMain::updatePosition(char ch) {
    currentPosition++;
    
    if (ch == '\n') {
        currentLine++;
        currentColumn = 1;
    } else {
        currentColumn++;
    }
}

Token LexerMain::recognizeToken() {
    // Pula espaços em branco
    skipWhitespace();
    
    char ch = buffer->peek();
    if (ch == '\0') {
        Lexer::Position pos;
        pos.line = static_cast<int>(currentLine);
        pos.column = static_cast<int>(currentColumn);
        pos.offset = static_cast<int>(currentPosition);
        return Token(TokenType::END_OF_FILE, "", pos);
    }
    
    // Reconhecimento de identificadores e palavras-chave
    if (isValidIdentifierStart(ch)) {
        return recognizeIdentifier();
    }
    
    // Reconhecimento de números
    if (std::isdigit(ch)) {
        return recognizeNumber();
    }
    
    // Reconhecimento de strings
    if (ch == '"') {
        return recognizeString();
    }
    
    // Reconhecimento de caracteres
    if (ch == '\'') {
        return recognizeCharacter();
    }
    
    // Reconhecimento de comentários - pular comentários ao invés de retorná-los como tokens
    if (ch == '/' && (buffer->peek(1) == '/' || buffer->peek(1) == '*')) {
        skipComment();
        return recognizeToken(); // Recursão para encontrar o próximo token válido
    }
    
    // Reconhecimento de operadores
    if (std::ispunct(ch)) {
        return recognizeOperator();
    }
    
    // Caractere inválido - usar novo sistema de tratamento de erros
    Lexer::Position errorPos;
    errorPos.line = static_cast<int>(currentLine);
    errorPos.column = static_cast<int>(currentColumn);
    errorPos.offset = static_cast<int>(currentPosition);
    
    handleError(ErrorType::INVALID_CHARACTER, 
                "Invalid character: " + std::string(1, ch), 
                errorPos);
    
    return recoverFromError(ErrorType::INVALID_CHARACTER, errorPos);
}

// Implementação dos métodos principais
Token LexerMain::nextToken() {
    // Se há token em cache, retorná-lo
    if (hasCachedToken) {
        hasCachedToken = false;
        return cachedToken;
    }
    
    if (endOfFile) {
        Lexer::Position pos;
        pos.line = static_cast<int>(currentLine);
        pos.column = static_cast<int>(currentColumn);
        pos.offset = static_cast<int>(currentPosition);
        return Token(TokenType::END_OF_FILE, "", pos);
    }
    
    Token token = recognizeToken();
    
    // Se o token é EOF, marcar endOfFile como true
    if (token.getType() == TokenType::END_OF_FILE) {
        endOfFile = true;
    }
    
    // Validar token gerado
    if (!validateToken(token)) {
        Lexer::Position pos = {
            static_cast<int>(currentLine),
            static_cast<int>(currentColumn),
            static_cast<int>(currentPosition)
        };
        
        std::string errorMsg = "Invalid token generated: '" + token.getLexeme() + 
                              "' of type " + tokenTypeToString(token.getType());
        
        handleError(ErrorType::INVALID_CHARACTER, errorMsg, pos);
        
        // Tentar recuperar do erro
        return recoverFromError(ErrorType::INVALID_CHARACTER, pos);
    }
    
    // Log do token encontrado
    if (logger) {
        logger->logToken(token);
    }
    
    // Reportar estatísticas ao ErrorHandler se disponível
    if (errorHandler) {
        // Incrementar contador de tokens processados
        // (implementação futura para estatísticas)
    }
    
    return token;
}

Token LexerMain::peekToken() {
    // Se já há token em cache, retorná-lo
    if (hasCachedToken) {
        return cachedToken;
    }
    
    // Obter próximo token e armazená-lo em cache
    cachedToken = nextToken();
    hasCachedToken = true;
    
    return cachedToken;
}

std::vector<Token> LexerMain::tokenizeAll() {
    std::vector<Token> tokens;
    Token token;
    int tokenCount = 0;
    
    if (logger) {
        logger->info("Starting complete tokenization of file");
    }
    
    do {
        try {
            token = nextToken();
            tokens.push_back(token);
            tokenCount++;
            
            // Verificar se deve parar devido a muitos erros
            if (errorHandler && errorHandler->shouldStop()) {
                if (logger) {
                    logger->error("Tokenization stopped due to too many errors");
                }
                break;
            }
            
        } catch (const std::exception& e) {
            // Tratar exceções durante tokenização
            Lexer::Position pos = {
                static_cast<int>(currentLine),
                static_cast<int>(currentColumn),
                static_cast<int>(currentPosition)
            };
            
            std::string errorMsg = "Exception during tokenization: " + std::string(e.what());
            handleError(ErrorType::INTERNAL_ERROR, errorMsg, pos);
            
            // Tentar recuperar
            token = recoverFromError(ErrorType::INTERNAL_ERROR, pos);
            if (token.getType() != TokenType::UNKNOWN) {
                tokens.push_back(token);
            }
        }
        
    } while (token.getType() != TokenType::END_OF_FILE && !endOfFile);
    
    if (logger) {
        logger->info("Tokenization completed. Total tokens: " + std::to_string(tokenCount));
    }
    
    // Reportar estatísticas finais ao ErrorHandler
    if (errorHandler) {
        if (errorHandler->hasErrors()) {
            if (logger) {
                logger->warning("Tokenization completed with " + 
                               std::to_string(errorHandler->getErrorCount()) + " errors");
            }
        }
    }
    
    return tokens;
}

bool LexerMain::hasMoreTokens() const {
    return !endOfFile;
}

void LexerMain::reset() {
    // Reinicializar posições
    currentLine = 1;
    currentColumn = 1;
    currentPosition = 0;
    currentState = LexerState::START;
    endOfFile = false;
    
    // Limpar cache de token
    hasCachedToken = false;
    
    // Reinicializar arquivo
    if (sourceFile && sourceFile->is_open()) {
        sourceFile->clear();
        sourceFile->seekg(0, std::ios::beg);
    }
    
    // Reinicializar buffer
    if (buffer) {
        buffer->clear();
    }
    
    // Log da reinicialização
    if (logger) {
        logger->logStateTransition("RESET", "START");
    }
}

LexerMain::Position LexerMain::getCurrentPosition() const {
    Position pos;
    pos.line = currentLine;
    pos.column = currentColumn;
    pos.absolute = currentPosition;
    return pos;
}

void LexerMain::setVersion(const std::string& version) {
    if (config) {
        // Mapear string para enum CVersion
        if (version == "C89" || version == "C90") {
            config->setVersion(CVersion::C89);
        } else if (version == "C99") {
            config->setVersion(CVersion::C99);
        } else if (version == "C11") {
            config->setVersion(CVersion::C11);
        } else if (version == "C17") {
            config->setVersion(CVersion::C17);
        } else if (version == "C23") {
            config->setVersion(CVersion::C23);
        } else {
            // Versão padrão
            config->setVersion(CVersion::C99);
        }
        
        // Log da mudança de versão
        if (logger) {
            logger->logStateTransition("VERSION_CHANGE", version);
        }
    }
}

// Implementação dos métodos de validação (Fase 4.2)
bool LexerMain::isValidIdentifierStart(char ch) const {
    return std::isalpha(ch) || ch == '_';
}

bool LexerMain::isValidIdentifierChar(char ch) const {
    return std::isalnum(ch) || ch == '_';
}

// Implementação dos métodos de reconhecimento de tokens (Fase 4.2)
Token LexerMain::recognizeIdentifier() {
    std::string lexeme;
    Lexer::Position startPos;
    startPos.line = static_cast<int>(currentLine);
    startPos.column = static_cast<int>(currentColumn);
    startPos.offset = static_cast<int>(currentPosition);
    
    // Ler primeiro caractere (já validado como início de identificador)
    char ch = readNextChar();
    lexeme += ch;
    
    // Ler caracteres subsequentes
    while ((ch = buffer->peek()) != '\0' && isValidIdentifierChar(ch)) {
        lexeme += readNextChar();
    }
    
    // Verificar se é palavra-chave
    if (symbolTable && symbolTable->isKeyword(lexeme)) {
        TokenType keywordType = symbolTable->getKeywordType(lexeme);
        return Token(keywordType, lexeme, startPos);
    }
    
    // É um identificador
    symbolTable->insert(lexeme, TokenType::IDENTIFIER, startPos);
    return Token(TokenType::IDENTIFIER, lexeme, startPos);
}

Token LexerMain::recognizeNumber() {
    std::string lexeme;
    Lexer::Position startPos;
    startPos.line = static_cast<int>(currentLine);
    startPos.column = static_cast<int>(currentColumn);
    startPos.offset = static_cast<int>(currentPosition);
    
    char ch = buffer->peek();
    
    // Verificar se é hexadecimal (0x ou 0X) ou binário (0b ou 0B)
    if (ch == '0') {
        lexeme += readNextChar();
        ch = buffer->peek();
        if (ch == 'x' || ch == 'X') {
            lexeme += readNextChar();
            // Verificar se há pelo menos um dígito hexadecimal
            bool hasHexDigits = false;
            // Ler dígitos hexadecimais
            while ((ch = buffer->peek()) != '\0' && std::isxdigit(ch)) {
                lexeme += readNextChar();
                hasHexDigits = true;
            }
            // Se não há dígitos hexadecimais, é um erro
            if (!hasHexDigits) {
                Lexer::Position errorPos;
                errorPos.line = static_cast<int>(currentLine);
                errorPos.column = static_cast<int>(currentColumn);
                errorPos.offset = static_cast<int>(currentPosition);
                handleError(ErrorType::INVALID_NUMBER_FORMAT, 
                           "Invalid hexadecimal number: missing digits after '0x'", 
                           errorPos);
                return recoverFromError(ErrorType::INVALID_NUMBER_FORMAT, errorPos);
            }
            // Processar sufixos para hex
            ch = buffer->peek();
            if (ch == 'u' || ch == 'U') {
                lexeme += readNextChar();
                ch = buffer->peek();
                if (ch == 'l' || ch == 'L') {
                    lexeme += readNextChar();
                    ch = buffer->peek();
                    if (ch == 'l' || ch == 'L') {
                        lexeme += readNextChar(); // ULL
                    }
                }
            } else if (ch == 'l' || ch == 'L') {
                lexeme += readNextChar();
                ch = buffer->peek();
                if (ch == 'l' || ch == 'L') {
                    lexeme += readNextChar(); // LL
                } else if (ch == 'u' || ch == 'U') {
                    lexeme += readNextChar(); // LU
                }
            }
            return Token(TokenType::INTEGER_LITERAL, lexeme, startPos);
        }
        // Verificar se é binário (0b ou 0B)
        else if (ch == 'b' || ch == 'B') {
            lexeme += readNextChar();
            // Verificar se há pelo menos um dígito binário
            bool hasBinaryDigits = false;
            // Ler dígitos binários
            while ((ch = buffer->peek()) != '\0' && (ch == '0' || ch == '1')) {
                lexeme += readNextChar();
                hasBinaryDigits = true;
            }
            // Se não há dígitos binários, é um erro
            if (!hasBinaryDigits) {
                Lexer::Position errorPos;
                errorPos.line = static_cast<int>(currentLine);
                errorPos.column = static_cast<int>(currentColumn);
                errorPos.offset = static_cast<int>(currentPosition);
                handleError(ErrorType::INVALID_NUMBER_FORMAT, 
                           "Invalid binary number: missing digits after '0b'", 
                           errorPos);
                return recoverFromError(ErrorType::INVALID_NUMBER_FORMAT, errorPos);
            }
            // Processar sufixos para binário
            ch = buffer->peek();
            if (ch == 'u' || ch == 'U') {
                lexeme += readNextChar();
                ch = buffer->peek();
                if (ch == 'l' || ch == 'L') {
                    lexeme += readNextChar();
                    ch = buffer->peek();
                    if (ch == 'l' || ch == 'L') {
                        lexeme += readNextChar(); // ULL
                    }
                }
            } else if (ch == 'l' || ch == 'L') {
                lexeme += readNextChar();
                ch = buffer->peek();
                if (ch == 'l' || ch == 'L') {
                    lexeme += readNextChar(); // LL
                } else if (ch == 'u' || ch == 'U') {
                    lexeme += readNextChar(); // LU
                }
            }
            return Token(TokenType::INTEGER_LITERAL, lexeme, startPos);
        }
        // Verificar se é octal
        else if (std::isdigit(ch) && ch != '8' && ch != '9') {
            // Ler dígitos octais
            while ((ch = buffer->peek()) != '\0' && ch >= '0' && ch <= '7') {
                lexeme += readNextChar();
            }
            // Processar sufixos para octal
            ch = buffer->peek();
            if (ch == 'u' || ch == 'U') {
                lexeme += readNextChar();
                ch = buffer->peek();
                if (ch == 'l' || ch == 'L') {
                    lexeme += readNextChar();
                    ch = buffer->peek();
                    if (ch == 'l' || ch == 'L') {
                        lexeme += readNextChar(); // ULL
                    }
                }
            } else if (ch == 'l' || ch == 'L') {
                lexeme += readNextChar();
                ch = buffer->peek();
                if (ch == 'l' || ch == 'L') {
                    lexeme += readNextChar(); // LL
                } else if (ch == 'u' || ch == 'U') {
                    lexeme += readNextChar(); // LU
                }
            }
            return Token(TokenType::INTEGER_LITERAL, lexeme, startPos);
        }
    }
    
    // Número decimal
    bool hasDecimalPoint = false;
    while ((ch = buffer->peek()) != '\0' && (std::isdigit(ch) || ch == '.')) {
        if (ch == '.') {
            if (hasDecimalPoint) break; // Segundo ponto decimal
            hasDecimalPoint = true;
        }
        lexeme += readNextChar();
    }
    
    // Verificar notação científica (e/E)
    ch = buffer->peek();
    if (ch == 'e' || ch == 'E') {
        lexeme += readNextChar();
        ch = buffer->peek();
        // Verificar sinal opcional
        if (ch == '+' || ch == '-') {
            lexeme += readNextChar();
            ch = buffer->peek();
        }
        // Ler dígitos do expoente
        if (std::isdigit(ch)) {
            while ((ch = buffer->peek()) != '\0' && std::isdigit(ch)) {
                lexeme += readNextChar();
            }
            hasDecimalPoint = true; // Notação científica é sempre float
        }
    }
    
    // Verificar sufixos
    ch = buffer->peek();
    if (hasDecimalPoint) {
        // Sufixos para float: f, F, l, L
        if (ch == 'f' || ch == 'F' || ch == 'l' || ch == 'L') {
            lexeme += readNextChar();
        }
    } else {
        // Sufixos para inteiros: u, U, l, L, ll, LL, ul, UL, ull, ULL
        if (ch == 'u' || ch == 'U') {
            lexeme += readNextChar();
            ch = buffer->peek();
            if (ch == 'l' || ch == 'L') {
                lexeme += readNextChar();
                ch = buffer->peek();
                if (ch == 'l' || ch == 'L') {
                    lexeme += readNextChar(); // ULL
                }
            }
        } else if (ch == 'l' || ch == 'L') {
            lexeme += readNextChar();
            ch = buffer->peek();
            if (ch == 'l' || ch == 'L') {
                lexeme += readNextChar(); // LL
            } else if (ch == 'u' || ch == 'U') {
                lexeme += readNextChar(); // LU
            }
        }
    }
    
    TokenType type = hasDecimalPoint ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL;
    return Token(type, lexeme, startPos);
}

Token LexerMain::recognizeString() {
    std::string lexeme;
    Lexer::Position startPos;
    startPos.line = static_cast<int>(currentLine);
    startPos.column = static_cast<int>(currentColumn);
    startPos.offset = static_cast<int>(currentPosition);
    
    // Consumir aspas de abertura
    char quote = readNextChar();
    lexeme += quote;
    
    char ch;
    while ((ch = buffer->peek()) != '\0' && ch != quote && ch != '\n') {
        if (ch == '\\') {
            // Escape sequence
            lexeme += readNextChar(); // Backslash
            ch = buffer->peek();
            if (ch != '\0') {
                lexeme += readNextChar(); // Caractere escapado
            }
        } else {
            lexeme += readNextChar();
        }
    }
    
    // Verificar se encontrou aspas de fechamento
    if (buffer->peek() == quote) {
        lexeme += readNextChar(); // Aspas de fechamento
        return Token(TokenType::STRING_LITERAL, lexeme, startPos);
    } else {
        // String não terminada
        if (errorHandler) {
            errorHandler->reportError(ErrorType::UNTERMINATED_STRING, 
                                    "String literal não terminada", startPos);
        }
        return Token(TokenType::UNKNOWN, lexeme, startPos);
    }
}

Token LexerMain::recognizeCharacter() {
    std::string lexeme;
    Lexer::Position startPos;
    startPos.line = static_cast<int>(currentLine);
    startPos.column = static_cast<int>(currentColumn);
    startPos.offset = static_cast<int>(currentPosition);
    
    // Consumir aspas simples de abertura
    lexeme += readNextChar();
    
    char ch = buffer->peek();
    if (ch == '\0' || ch == '\n') {
        // Caractere não terminado
        if (errorHandler) {
            errorHandler->reportError(ErrorType::UNTERMINATED_CHAR, 
                                    "Literal de caractere não terminado", startPos);
        }
        return Token(TokenType::UNKNOWN, lexeme, startPos);
    }
    
    if (ch == '\\') {
        // Escape sequence
        lexeme += readNextChar(); // Backslash
        ch = buffer->peek();
        if (ch != '\0') {
            lexeme += readNextChar(); // Caractere escapado
        }
    } else {
        lexeme += readNextChar();
    }
    
    // Verificar aspas simples de fechamento
    if (buffer->peek() == '\'') {
        lexeme += readNextChar();
        return Token(TokenType::CHAR_LITERAL, lexeme, startPos);
    } else {
        // Caractere não terminado
        if (errorHandler) {
            errorHandler->reportError(ErrorType::UNTERMINATED_CHAR, 
                                    "Literal de caractere não terminado", startPos);
        }
        return Token(TokenType::UNKNOWN, lexeme, startPos);
    }
}

Token LexerMain::recognizeOperator() {
    std::string lexeme;
    Lexer::Position startPos;
    startPos.line = static_cast<int>(currentLine);
    startPos.column = static_cast<int>(currentColumn);
    startPos.offset = static_cast<int>(currentPosition);
    
    char ch = readNextChar();
    lexeme += ch;
    
    // Verificar operadores compostos
    char nextCh = buffer->peek();
    
    switch (ch) {
        case '+':
            if (nextCh == '+' || nextCh == '=') {
                lexeme += readNextChar();
            }
            break;
        case '-':
            if (nextCh == '-' || nextCh == '=' || nextCh == '>') {
                lexeme += readNextChar();
            }
            break;
        case '*':
        case '/':
        case '%':
        case '^':
        case '!':
        case '=':
            if (nextCh == '=') {
                lexeme += readNextChar();
            }
            break;
        case '<':
            if (nextCh == '=' || nextCh == '<') {
                lexeme += readNextChar();
                if (lexeme == "<<" && buffer->peek() == '=') {
                    lexeme += readNextChar();
                }
            }
            break;
        case '>':
            if (nextCh == '=' || nextCh == '>') {
                lexeme += readNextChar();
                if (lexeme == ">>" && buffer->peek() == '=') {
                    lexeme += readNextChar();
                }
            }
            break;
        case '&':
            if (nextCh == '&' || nextCh == '=') {
                lexeme += readNextChar();
            }
            break;
        case '|':
            if (nextCh == '|' || nextCh == '=') {
                lexeme += readNextChar();
            }
            break;
        case '~':
            // Operador bitwise NOT não tem versões compostas
            break;
        case '\\':
            // Backslash para continuação de linha
            break;
    }
    
    // Mapear lexeme para TokenType
    TokenType type = TokenType::UNKNOWN;
    if (lexeme == "+") type = TokenType::PLUS;
    else if (lexeme == "-") type = TokenType::MINUS;
    else if (lexeme == "*") type = TokenType::MULTIPLY;
    else if (lexeme == "/") type = TokenType::DIVIDE;
    else if (lexeme == "%") type = TokenType::MODULO;
    else if (lexeme == "=") type = TokenType::ASSIGN;
    else if (lexeme == "==") type = TokenType::EQUAL;
    else if (lexeme == "!=") type = TokenType::NOT_EQUAL;
    else if (lexeme == "<") type = TokenType::LESS_THAN;
    else if (lexeme == ">") type = TokenType::GREATER_THAN;
    else if (lexeme == "<=") type = TokenType::LESS_EQUAL;
    else if (lexeme == ">=") type = TokenType::GREATER_EQUAL;
    else if (lexeme == "&&") type = TokenType::LOGICAL_AND;
    else if (lexeme == "||") type = TokenType::LOGICAL_OR;
    else if (lexeme == "!") type = TokenType::LOGICAL_NOT;
    else if (lexeme == "&") type = TokenType::BITWISE_AND;
    else if (lexeme == "|") type = TokenType::BITWISE_OR;
    else if (lexeme == "^") type = TokenType::BITWISE_XOR;
    else if (lexeme == "~") type = TokenType::BITWISE_NOT;
    else if (lexeme == "<<") type = TokenType::LEFT_SHIFT;
    else if (lexeme == ">>") type = TokenType::RIGHT_SHIFT;
    else if (lexeme == "++") type = TokenType::INCREMENT;
    else if (lexeme == "--") type = TokenType::DECREMENT;
    else if (lexeme == "+=") type = TokenType::PLUS_ASSIGN;
    else if (lexeme == "-=") type = TokenType::MINUS_ASSIGN;
    else if (lexeme == "*=") type = TokenType::MULT_ASSIGN;
    else if (lexeme == "/=") type = TokenType::DIV_ASSIGN;
    else if (lexeme == "%=") type = TokenType::MOD_ASSIGN;
    else if (lexeme == "&=") type = TokenType::AND_ASSIGN;
    else if (lexeme == "|=") type = TokenType::OR_ASSIGN;
    else if (lexeme == "^=") type = TokenType::XOR_ASSIGN;
    else if (lexeme == "<<=") type = TokenType::LEFT_SHIFT_ASSIGN;
    else if (lexeme == ">>=") type = TokenType::RIGHT_SHIFT_ASSIGN;
    else if (lexeme == "->") type = TokenType::ARROW;
    // Delimitadores
    else if (lexeme == ";") type = TokenType::SEMICOLON;
    else if (lexeme == "(") type = TokenType::LEFT_PAREN;
    else if (lexeme == ")") type = TokenType::RIGHT_PAREN;
    else if (lexeme == "{") type = TokenType::LEFT_BRACE;
    else if (lexeme == "}") type = TokenType::RIGHT_BRACE;
    else if (lexeme == "[") type = TokenType::LEFT_BRACKET;
    else if (lexeme == "]") type = TokenType::RIGHT_BRACKET;
    else if (lexeme == ",") type = TokenType::COMMA;
    else if (lexeme == ".") type = TokenType::DOT;
    else if (lexeme == ":") type = TokenType::COLON;
    else if (lexeme == "?") type = TokenType::CONDITIONAL;
    else if (lexeme == "#") type = TokenType::HASH;
    else if (lexeme == "\\") {
        // Backslash - verificar se é continuação de linha
        if (buffer->peek() == '\n') {
            // É continuação de linha - pular o backslash e newline
            readNextChar(); // consome o \n
            return nextToken(); // retorna o próximo token
        }
        type = TokenType::UNKNOWN; // backslash isolado
    }
    
    return Token(type, lexeme, startPos);
}

void LexerMain::skipComment() {
    Lexer::Position startPos;
    startPos.line = static_cast<int>(currentLine);
    startPos.column = static_cast<int>(currentColumn);
    startPos.offset = static_cast<int>(currentPosition);
    
    char ch = readNextChar(); // '/'
    char nextCh = buffer->peek();
    
    if (nextCh == '/') {
        // Comentário de linha - pular até o final da linha
        readNextChar(); // consumir o segundo '/'
        
        while ((ch = buffer->peek()) != '\0' && ch != '\n') {
            readNextChar();
        }
    }
    else if (nextCh == '*') {
        // Comentário de bloco - pular até encontrar */
        readNextChar(); // consumir o '*'
        
        bool foundEnd = false;
        while ((ch = buffer->peek()) != '\0') {
            ch = readNextChar();
            if (ch == '*' && buffer->peek() == '/') {
                readNextChar(); // consumir o '/'
                foundEnd = true;
                break;
            }
        }
        
        if (!foundEnd && errorHandler) {
            errorHandler->reportError(ErrorType::UNTERMINATED_COMMENT, 
                                    "Comentário de bloco não terminado", startPos);
        }
    }
}

Token LexerMain::recognizeComment() {
    std::string lexeme;
    Lexer::Position startPos;
    startPos.line = static_cast<int>(currentLine);
    startPos.column = static_cast<int>(currentColumn);
    startPos.offset = static_cast<int>(currentPosition);
    
    char ch = readNextChar(); // '/'
    lexeme += ch;
    
    char nextCh = buffer->peek();
    
    if (nextCh == '/') {
        // Comentário de linha
        lexeme += readNextChar();
        
        while ((ch = buffer->peek()) != '\0' && ch != '\n') {
            lexeme += readNextChar();
        }
        
        return Token(TokenType::LINE_COMMENT, lexeme, startPos);
    }
    else if (nextCh == '*') {
        // Comentário de bloco
        lexeme += readNextChar();
        
        bool foundEnd = false;
        while ((ch = buffer->peek()) != '\0') {
            lexeme += readNextChar();
            if (ch == '*' && buffer->peek() == '/') {
                lexeme += readNextChar();
                foundEnd = true;
                break;
            }
        }
        
        if (!foundEnd && errorHandler) {
            errorHandler->reportError(ErrorType::UNTERMINATED_COMMENT, 
                                    "Comentário de bloco não terminado", startPos);
        }
        
        return Token(TokenType::BLOCK_COMMENT, lexeme, startPos);
    }
    else {
        // É apenas o operador de divisão
        return recognizeOperator();
    }
}

// ============================================================================
// Implementação dos métodos de tratamento de erros (Fase 4.3)
// ============================================================================

void LexerMain::handleError(ErrorType type, const std::string& message, const Lexer::Position& pos) {
    // Reporta o erro usando o ErrorHandler
    if (errorHandler) {
        errorHandler->reportError(type, message, pos);
        
        // Log do erro se logger estiver disponível
        if (logger) {
            logger->error("Lexical error at line " + std::to_string(pos.line) + 
                         ", column " + std::to_string(pos.column) + ": " + message);
        }
    }
    
    // Transição para estado de erro
    if (logger) {
        logger->logStateTransition(currentState, LexerState::ERROR);
    }
    currentState = LexerState::ERROR;
}

Token LexerMain::recoverFromError(ErrorType type, const Lexer::Position& pos) {
    // Obtém modo de recuperação do ErrorHandler
    RecoveryMode recoveryMode = RecoveryMode::CONTINUE;
    if (errorHandler) {
        recoveryMode = errorHandler->getRecoveryMode();
    }
    
    // Aplica estratégia baseada no modo de recuperação
    switch (recoveryMode) {
        case RecoveryMode::PANIC:
            // Modo PANIC: para imediatamente
            endOfFile = true;
            break;
            
        case RecoveryMode::SYNCHRONIZE:
            // Modo SYNCHRONIZE: busca próximo token de sincronização
            synchronizeAfterError();
            break;
            
        case RecoveryMode::CONTINUE:
            // Modo CONTINUE: aplica recuperação específica por tipo de erro
            switch (type) {
                case ErrorType::UNTERMINATED_STRING:
                case ErrorType::UNTERMINATED_CHAR:
                case ErrorType::UNTERMINATED_COMMENT:
                    // Para strings/chars/comentários não terminados, pula até o fim da linha
                    while (!endOfFile && readNextChar() != '\n') {
                        // Continua lendo até encontrar nova linha ou EOF
                    }
                    break;
                    
                case ErrorType::INVALID_CHARACTER:
                    // Para caracteres inválidos, simplesmente pula o caractere
                    if (!endOfFile) {
                        readNextChar();
                    }
                    break;
                    
                case ErrorType::INVALID_ESCAPE_SEQUENCE:
                case ErrorType::INVALID_NUMBER_FORMAT:
                    // Para sequências inválidas, pula até encontrar espaço ou delimitador
                    while (!endOfFile) {
                        char ch = buffer->peek(0);
                        if (std::isspace(ch) || std::ispunct(ch)) {
                            break;
                        }
                        readNextChar();
                    }
                    break;
                    
                default:
                    // Recuperação padrão: pula caractere atual
                    if (!endOfFile) {
                        readNextChar();
                    }
                    break;
            }
            break;
            
        case RecoveryMode::INTERACTIVE:
            // Modo INTERACTIVE: permite interação do usuário (implementação futura)
            if (logger) {
                 logger->warning("Interactive recovery mode not fully implemented, using CONTINUE mode");
             }
            // Fallback para modo CONTINUE
            if (!endOfFile) {
                readNextChar();
            }
            break;
    }
    
    // Retorna token de erro
    Lexer::Position errorPos;
    errorPos.line = static_cast<int>(pos.line);
    errorPos.column = static_cast<int>(pos.column);
    errorPos.offset = static_cast<int>(pos.offset);
    
    return Token(TokenType::UNKNOWN, "<error>", errorPos);
}

bool LexerMain::validateToken(const Token& token) const {
    // Validações básicas do token
    TokenType type = token.getType();
    const std::string& lexeme = token.getLexeme();
    
    // Verifica se o tipo é válido
    if (type == TokenType::UNKNOWN) {
        return false;
    }
    
    // Validações específicas por tipo
    switch (type) {
        case TokenType::IDENTIFIER:
            // Identificador deve começar com letra ou underscore
            if (lexeme.empty() || (!std::isalpha(lexeme[0]) && lexeme[0] != '_')) {
                return false;
            }
            // Todos os caracteres devem ser alfanuméricos ou underscore
            for (char ch : lexeme) {
                if (!std::isalnum(ch) && ch != '_') {
                    return false;
                }
            }
            break;
            
        case TokenType::INTEGER_LITERAL:
            // Literais inteiros não podem estar vazios
            if (lexeme.empty()) {
                return false;
            }
            // Verifica se todos os caracteres são dígitos (exceto prefixos como 0x)
            if (lexeme.length() >= 2 && lexeme[0] == '0' && (lexeme[1] == 'x' || lexeme[1] == 'X')) {
                // Hexadecimal - verificar dígitos hex e sufixos
                size_t i = 2;
                while (i < lexeme.length() && std::isxdigit(lexeme[i])) {
                    ++i;
                }
                // Verificar sufixos opcionais
                if (i < lexeme.length()) {
                    if (lexeme[i] == 'u' || lexeme[i] == 'U') {
                        ++i;
                        if (i < lexeme.length() && (lexeme[i] == 'l' || lexeme[i] == 'L')) {
                            ++i;
                            if (i < lexeme.length() && (lexeme[i] == 'l' || lexeme[i] == 'L')) {
                                ++i; // ULL
                            }
                        }
                    } else if (lexeme[i] == 'l' || lexeme[i] == 'L') {
                        ++i;
                        if (i < lexeme.length() && (lexeme[i] == 'l' || lexeme[i] == 'L')) {
                            ++i; // LL
                        } else if (i < lexeme.length() && (lexeme[i] == 'u' || lexeme[i] == 'U')) {
                            ++i; // LU
                        }
                    }
                }
                if (i != lexeme.length()) {
                    return false;
                }
            } else if (lexeme.length() >= 2 && lexeme[0] == '0' && lexeme[1] == 'b') {
                // Binário - verificar dígitos binários e sufixos
                size_t i = 2;
                while (i < lexeme.length() && (lexeme[i] == '0' || lexeme[i] == '1')) {
                    ++i;
                }
                // Verificar sufixos opcionais
                if (i < lexeme.length()) {
                    if (lexeme[i] == 'u' || lexeme[i] == 'U') {
                        ++i;
                        if (i < lexeme.length() && (lexeme[i] == 'l' || lexeme[i] == 'L')) {
                            ++i;
                            if (i < lexeme.length() && (lexeme[i] == 'l' || lexeme[i] == 'L')) {
                                ++i; // ULL
                            }
                        }
                    } else if (lexeme[i] == 'l' || lexeme[i] == 'L') {
                        ++i;
                        if (i < lexeme.length() && (lexeme[i] == 'l' || lexeme[i] == 'L')) {
                            ++i; // LL
                        } else if (i < lexeme.length() && (lexeme[i] == 'u' || lexeme[i] == 'U')) {
                            ++i; // LU
                        }
                    }
                }
                if (i != lexeme.length()) {
                    return false;
                }
            } else {
                // Decimal - verificar dígitos e sufixos válidos
                size_t i = 0;
                // Verificar dígitos
                while (i < lexeme.length() && std::isdigit(lexeme[i])) {
                    ++i;
                }
                // Verificar sufixos opcionais: u/U, l/L, ll/LL, ul/UL, ull/ULL
                if (i < lexeme.length()) {
                    if (lexeme[i] == 'u' || lexeme[i] == 'U') {
                        ++i;
                        if (i < lexeme.length() && (lexeme[i] == 'l' || lexeme[i] == 'L')) {
                            ++i;
                            if (i < lexeme.length() && (lexeme[i] == 'l' || lexeme[i] == 'L')) {
                                ++i; // ULL
                            }
                        }
                    } else if (lexeme[i] == 'l' || lexeme[i] == 'L') {
                        ++i;
                        if (i < lexeme.length() && (lexeme[i] == 'l' || lexeme[i] == 'L')) {
                            ++i; // LL
                        } else if (i < lexeme.length() && (lexeme[i] == 'u' || lexeme[i] == 'U')) {
                            ++i; // LU
                        }
                    }
                }
                // Verificar se todos os caracteres foram processados
                if (i != lexeme.length()) {
                    return false;
                }
            }
            break;
            
        case TokenType::FLOAT_LITERAL:
            // Literais float não podem estar vazios
            if (lexeme.empty()) {
                return false;
            }
            // Deve conter pelo menos um ponto decimal
            if (lexeme.find('.') == std::string::npos && 
                lexeme.find('e') == std::string::npos && 
                lexeme.find('E') == std::string::npos) {
                return false;
            }
            break;
            
        case TokenType::STRING_LITERAL:
            // Strings devem começar e terminar com aspas
            if (lexeme.length() < 2 || lexeme[0] != '"' || lexeme.back() != '"') {
                return false;
            }
            // Verifica escape sequences válidas
            for (size_t i = 1; i < lexeme.length() - 1; ++i) {
                if (lexeme[i] == '\\') {
                    if (i + 1 >= lexeme.length() - 1) {
                        return false; // Escape incompleto
                    }
                    char escaped = lexeme[i + 1];
                    if (escaped != 'n' && escaped != 't' && escaped != 'r' && 
                        escaped != '\\' && escaped != '"' && escaped != '\'' && 
                        escaped != '0' && escaped != 'a' && escaped != 'b' && 
                        escaped != 'f' && escaped != 'v') {
                        return false; // Escape sequence inválida
                    }
                    ++i; // Pula o caractere escapado
                }
            }
            break;
            
        case TokenType::CHAR_LITERAL:
            // Caracteres devem começar e terminar com aspas simples
            if (lexeme.length() < 3 || lexeme[0] != '\'' || lexeme.back() != '\'') {
                return false;
            }
            // Verifica conteúdo do caractere
            if (lexeme.length() == 3) {
                // Caractere simples
                char ch = lexeme[1];
                if (ch == '\\' || ch == '\'' || ch < 32 || ch > 126) {
                    return false; // Caractere inválido sem escape
                }
            } else if (lexeme.length() == 4 && lexeme[1] == '\\') {
                // Caractere com escape
                char escaped = lexeme[2];
                if (escaped != 'n' && escaped != 't' && escaped != 'r' && 
                    escaped != '\\' && escaped != '\'' && escaped != '"' && 
                    escaped != '0' && escaped != 'a' && escaped != 'b' && 
                    escaped != 'f' && escaped != 'v') {
                    return false; // Escape sequence inválida
                }
            } else {
                return false; // Tamanho inválido para char literal
            }
            break;
            
        case TokenType::IF:
        case TokenType::ELSE:
        case TokenType::WHILE:
        case TokenType::FOR:
        case TokenType::DO:
        case TokenType::BREAK:
        case TokenType::CONTINUE:
        case TokenType::RETURN:
        case TokenType::INT:
        case TokenType::FLOAT:
        case TokenType::CHAR:
        case TokenType::VOID:
        case TokenType::CONST:
        case TokenType::STATIC:
        case TokenType::EXTERN:
        case TokenType::STRUCT:
        case TokenType::UNION:
        case TokenType::ENUM:
        case TokenType::TYPEDEF:
        case TokenType::SIZEOF:
        case TokenType::SWITCH:
        case TokenType::CASE:
        case TokenType::DEFAULT:
        case TokenType::GOTO:
        case TokenType::VOLATILE:
        case TokenType::REGISTER:
        case TokenType::AUTO:
        case TokenType::SIGNED:
        case TokenType::UNSIGNED:
        case TokenType::SHORT:
        case TokenType::LONG:
        case TokenType::DOUBLE:
        // Palavras-chave C99
        case TokenType::INLINE:
        case TokenType::RESTRICT:
        case TokenType::_BOOL:
        case TokenType::_COMPLEX:
        case TokenType::_IMAGINARY:
        // Palavras-chave C11
        case TokenType::_ALIGNAS:
        case TokenType::_ALIGNOF:
        case TokenType::_ATOMIC:
        case TokenType::_STATIC_ASSERT:
        case TokenType::_NORETURN:
        case TokenType::_THREAD_LOCAL:
        case TokenType::_GENERIC:
        // Palavras-chave C23
        case TokenType::TYPEOF:
        case TokenType::TYPEOF_UNQUAL:
        case TokenType::_BITINT:
        case TokenType::_DECIMAL128:
        case TokenType::_DECIMAL32:
        case TokenType::_DECIMAL64:
            // Palavras-chave devem ter lexema não vazio
            if (lexeme.empty()) {
                return false;
            }
            break;
            
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
        case TokenType::MODULO:
        case TokenType::ASSIGN:
        case TokenType::EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::LESS_THAN:
        case TokenType::GREATER_THAN:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER_EQUAL:
        case TokenType::LOGICAL_AND:
        case TokenType::LOGICAL_OR:
        case TokenType::LOGICAL_NOT:
        case TokenType::BITWISE_AND:
        case TokenType::BITWISE_OR:
        case TokenType::BITWISE_XOR:
        case TokenType::BITWISE_NOT:
        case TokenType::LEFT_SHIFT:
        case TokenType::RIGHT_SHIFT:
        case TokenType::INCREMENT:
        case TokenType::DECREMENT:
        case TokenType::PLUS_ASSIGN:
        case TokenType::MINUS_ASSIGN:
        case TokenType::MULT_ASSIGN:
        case TokenType::DIV_ASSIGN:
        case TokenType::MOD_ASSIGN:
        case TokenType::AND_ASSIGN:
        case TokenType::OR_ASSIGN:
        case TokenType::XOR_ASSIGN:
        case TokenType::LEFT_SHIFT_ASSIGN:
        case TokenType::RIGHT_SHIFT_ASSIGN:
        case TokenType::ARROW:
        case TokenType::DOT:
        case TokenType::SEMICOLON:
        case TokenType::COMMA:
        case TokenType::LEFT_PAREN:
        case TokenType::RIGHT_PAREN:
        case TokenType::LEFT_BRACE:
        case TokenType::RIGHT_BRACE:
        case TokenType::LEFT_BRACKET:
        case TokenType::RIGHT_BRACKET:
        case TokenType::HASH:
        case TokenType::CONDITIONAL:
        case TokenType::COLON:
        case TokenType::LINE_COMMENT:
        case TokenType::BLOCK_COMMENT:
            // Operadores, delimitadores e comentários devem ter lexema não vazio
            if (lexeme.empty()) {
                return false;
            }
            break;
            
        case TokenType::END_OF_FILE:
            // EOF pode ter lexema vazio
            break;
            
        default:
            // Tipos desconhecidos são inválidos
            return false;
    }
    
    return true;
}

void LexerMain::synchronizeAfterError() {
    // Estratégia de sincronização: pula espaços em branco e procura por delimitadores
    skipWhitespace();
    
    // Procura por tokens de sincronização (ponto e vírgula, chaves, etc.)
    while (!endOfFile) {
        char ch = buffer->peek(0);
        
        // Caracteres de sincronização comuns
        if (ch == ';' || ch == '{' || ch == '}' || ch == '\n') {
            break;
        }
        
        // Se encontrou início de um novo token válido, para
        if (std::isalpha(ch) || ch == '_' || std::isdigit(ch) || ch == '"' || ch == '\'') {
            break;
        }
        
        readNextChar();
    }
    
    // Reseta estado para START após sincronização
    currentState = LexerState::START;
    
    if (logger) {
        logger->debug("Synchronized after error at line " + std::to_string(currentLine) + 
                     ", column " + std::to_string(currentColumn));
    }
}

} // namespace Lexer