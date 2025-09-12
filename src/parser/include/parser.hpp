#ifndef PARSER_HPP
#define PARSER_HPP

#include "parser_types.hpp"
#include "ast.hpp"
#include "grammar.hpp"
#include "parser_config.hpp"
#include "parser_state.hpp"
#include "parser_logger.hpp"
#include "error_recovery.hpp"
#include "token_stream.hpp"
#include <vector>
#include <memory>
#include <string>
#include <functional>

namespace Parser {

// Forward declarations
class LexerParserBridge;
class PreprocessorParserBridge;
class SemanticInterface;



// Parser interface bridges
class LexerParserBridge {
public:
    virtual ~LexerParserBridge() = default;
    
    // Token stream access
    virtual std::unique_ptr<TokenStream> getTokenStream() = 0;
    
    // Lexer state queries
    virtual bool hasMoreTokens() const = 0;
    virtual size_t getCurrentLine() const = 0;
    virtual size_t getCurrentColumn() const = 0;
    
    // Error reporting to lexer
    virtual void reportLexerError(const std::string& message, const Position& pos) = 0;
};

class PreprocessorParserBridge {
public:
    virtual ~PreprocessorParserBridge() = default;
    
    // IMPORTANTE: O parser NÃO recebe dados diretamente do preprocessor!
    // Esta bridge é usada apenas para mapeamento de posições
    
    // Position mapping (expanded -> original)
    virtual Position mapToOriginalPosition(const Position& expandedPos) const = 0;
    virtual SourceRange mapToOriginalRange(const SourceRange& expandedRange) const = 0;
    
    // Macro expansion information
    virtual bool isPositionInMacro(const Position& pos) const = 0;
    virtual std::string getMacroNameAtPosition(const Position& pos) const = 0;
    
    // Include file information
    virtual std::string getOriginalFilename(const Position& pos) const = 0;
    virtual bool isPositionInInclude(const Position& pos) const = 0;
};

class SemanticInterface {
public:
    virtual ~SemanticInterface() = default;
    
    // AST processing
    virtual void processAST(const ASTNodePtr& ast) = 0;
    
    // Symbol table queries
    virtual bool isSymbolDeclared(const std::string& name) const = 0;
    virtual ASTNodeType getSymbolType(const std::string& name) const = 0;
    
    // Type checking support
    virtual bool areTypesCompatible(const ASTNodePtr& type1, const ASTNodePtr& type2) const = 0;
    
    // Error reporting to semantic analyzer
    virtual void reportSemanticError(const std::string& message, const SourceRange& range) = 0;
};

// Main parser class
class SyntacticAnalyzer {
public:
    SyntacticAnalyzer();
    explicit SyntacticAnalyzer(const ParserConfig& config);
    ~SyntacticAnalyzer();
    
    // Configuration
    void setConfig(const ParserConfig& config);
    const ParserConfig& getConfig() const { return *config; }
    
    // Bridge interfaces
    void setLexerBridge(std::unique_ptr<LexerParserBridge> bridge);
    void setPreprocessorBridge(std::unique_ptr<PreprocessorParserBridge> bridge);
    void setSemanticInterface(std::unique_ptr<SemanticInterface> interface);
    
    // Grammar management
    void setGrammar(const Grammar& grammar);
    const Grammar& getGrammar() const { return *grammar; }
    
    // Main parsing interface
    ParseResult<ASTNodePtr> parse();
    ParseResult<ASTNodePtr> parseExpression();
    ParseResult<ASTNodePtr> parseStatement();
    ParseResult<ASTNodePtr> parseDeclaration();
    
    // Phase 3.2 - Main parsing methods
    ParseResult<ASTNodePtr> parseFile(const std::string& filename);
    ParseResult<ASTNodePtr> parseString(const std::string& code);
    ParseResult<ASTNodePtr> parseTokens(std::unique_ptr<TokenStream> tokens);
    
    // Token control methods
    const Token& getCurrentToken() const;
    const Token& peekToken(size_t offset = 1) const;
    bool consumeToken();
    bool isAtEnd() const;
    
    // Parsing from token stream (legacy)
    ParseResult<ASTNodePtr> parse(std::unique_ptr<TokenStream> tokens);
    
    // Error handling
    const std::vector<std::unique_ptr<ParseError>>& getErrors() const;
    bool hasErrors() const;
    void clearErrors();
    
    // Parser state
    const ParserStateManager& getState() const { return *state; }
    ParserStateManager& getState() { return *state; }
    
    // Statistics and diagnostics
    struct ParseStatistics {
        size_t tokensProcessed = 0;
        size_t nodesCreated = 0;
        size_t errorsEncountered = 0;
        size_t recoveryAttempts = 0;
        size_t successfulRecoveries = 0;
        std::chrono::milliseconds parseTime{0};
        size_t maxParseDepth = 0;
    };
    
    const ParseStatistics& getStatistics() const { return statistics; }
    void resetStatistics();
    
    // Parser control
    void reset();
    bool isInitialized() const { return initialized; }
    
    // Validation methods
    bool isConfigurationValid() const;
    bool isGrammarValid() const;
    bool isFullyConfigured() const;
    std::vector<std::string> getConfigurationErrors() const;
    
private:
    // Core components
    std::unique_ptr<ParserConfig> config;
    std::unique_ptr<Grammar> grammar;
    std::unique_ptr<ParserStateManager> state;
    std::unique_ptr<ErrorRecoveryManager> errorRecovery;
    
    // Interface bridges
    std::unique_ptr<LexerParserBridge> lexerBridge;
    std::unique_ptr<PreprocessorParserBridge> preprocessorBridge;
    std::unique_ptr<SemanticInterface> semanticInterface;
    
    // Current parsing context
    std::unique_ptr<TokenStream> currentTokens;
    ParseStatistics statistics;
    bool initialized;
    
    // Core parsing methods
    ASTNodePtr parseTranslationUnit();
    ASTNodePtr parseExternalDeclaration();
    ASTNodePtr parseFunctionDefinition();
    ASTNodePtr parseDeclarationSpecifiers();
    ASTNodePtr parseDeclarator();
    ASTNodePtr parseDirectDeclarator();
    ASTNodePtr parseParameterList();
    ASTNodePtr parseParameterDeclaration();
    ASTNodePtr parseInitializer();
    ASTNodePtr parseInitializerList();
    ASTNodePtr parseCompoundStatement();
    
    // Declaration parsing methods (Phase 3.3)
    ASTNodePtr parseVariableDeclaration();
    ASTNodePtr parseTypeDeclaration();
    ASTNodePtr parseStructDeclaration();
    ASTNodePtr parseUnionDeclaration();
    ASTNodePtr parseEnumDeclaration();
    ASTNodePtr parseTypedefDeclaration();
    
    // Statement parsing methods
    ASTNodePtr parseStatementInternal();
    ASTNodePtr parseExpressionStatement();
    ASTNodePtr parseSelectionStatement();
    ASTNodePtr parseIterationStatement();
    ASTNodePtr parseJumpStatement();
    ASTNodePtr parseLabeledStatement();
    
    // Specific statement parsing methods
    ASTNodePtr parseIfStatement();
    ASTNodePtr parseWhileStatement();
    ASTNodePtr parseForStatement();
    ASTNodePtr parseDoWhileStatement();
    ASTNodePtr parseSwitchStatement();
    ASTNodePtr parseBreakStatement();
    ASTNodePtr parseContinueStatement();
    ASTNodePtr parseReturnStatement();
    ASTNodePtr parseGotoStatement();
    ASTNodePtr parseCaseStatement();
    
    // Expression parsing
    ASTNodePtr parseExpressionInternal();
    ASTNodePtr parseAssignmentExpression();
    ASTNodePtr parseConditionalExpression();
    ASTNodePtr parseLogicalOrExpression();
    ASTNodePtr parseLogicalAndExpression();
    ASTNodePtr parseInclusiveOrExpression();
    ASTNodePtr parseExclusiveOrExpression();
    ASTNodePtr parseAndExpression();
    ASTNodePtr parseEqualityExpression();
    ASTNodePtr parseRelationalExpression();
    ASTNodePtr parseShiftExpression();
    ASTNodePtr parseAdditiveExpression();
    ASTNodePtr parseMultiplicativeExpression();
    ASTNodePtr parseCastExpression();
    ASTNodePtr parseUnaryExpression();
    ASTNodePtr parsePostfixExpression();
    ASTNodePtr parsePrimaryExpression();
    
    // Type parsing
    ASTNodePtr parseTypeName();
    ASTNodePtr parseSpecifierQualifierList();
    ASTNodePtr parseAbstractDeclarator();
    ASTNodePtr parseDirectAbstractDeclarator();
    
    // Utility methods
    bool match(const std::string& tokenType);
    bool check(const std::string& tokenType) const;
    const Token& advance();
    const Token& consume(const std::string& tokenType, const std::string& errorMessage);
    std::string processEscapeSequences(const std::string& rawString);
    
    // Error handling helpers
    void reportError(const std::string& message);
    void reportError(std::unique_ptr<ParseError> error);
    bool synchronize();
    
    // AST construction helpers
    ASTNodePtr createNode(ASTNodeType type, const SourceRange& range = SourceRange());
    SourceRange getCurrentRange() const;
    SourceRange getRangeFromTo(const Position& start, const Position& end) const;
    
    // Position mapping helpers
    Position mapPosition(const Position& pos) const;
    SourceRange mapRange(const SourceRange& range) const;
    
    // Initialization and cleanup
    void initialize();
    void cleanup();
    
    // Statistics helpers
    void updateStatistics();
    void startTiming();
    void endTiming();
    
    std::chrono::high_resolution_clock::time_point parseStartTime;
};

// Parser factory functions
std::unique_ptr<SyntacticAnalyzer> createParser(CStandard standard = CStandard::C17);
std::unique_ptr<SyntacticAnalyzer> createStrictParser(CStandard standard = CStandard::C17);
std::unique_ptr<SyntacticAnalyzer> createPermissiveParser(CStandard standard = CStandard::C17);

// Utility functions
bool isValidCCode(const std::string& code, CStandard standard = CStandard::C17);
ParseResult<ASTNodePtr> parseCodeString(const std::string& code, CStandard standard = CStandard::C17);
ParseResult<ASTNodePtr> parseFile(const std::string& filename, CStandard standard = CStandard::C17);

// Parser version information
struct ParserVersion {
    int major;
    int minor;
    int patch;
    std::string buildDate;
    std::string gitHash;
};

ParserVersion getParserVersion();
std::string getParserVersionString();

} // namespace Parser

#endif // PARSER_HPP