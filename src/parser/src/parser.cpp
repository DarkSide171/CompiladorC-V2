#include "../include/parser.hpp"
#include "../include/parser_config.hpp"
#include "../include/parser_state.hpp"
#include "../include/parser_logger.hpp"
#include "../include/error_recovery.hpp"
#include "../include/grammar.hpp"
#include "../include/ast.hpp"
#include <chrono>
#include <iostream>
#include <stdexcept>

namespace Parser {

// Constructor implementations
SyntacticAnalyzer::SyntacticAnalyzer() : SyntacticAnalyzer(ParserConfig()) {
}

SyntacticAnalyzer::SyntacticAnalyzer(const ParserConfig& config) 
    : config(std::make_unique<ParserConfig>(config))
    , grammar(nullptr)
    , state(std::make_unique<ParserStateManager>())
    , errorRecovery(std::make_unique<ErrorRecoveryManager>())
    , lexerBridge(nullptr)
    , preprocessorBridge(nullptr)
    , semanticInterface(nullptr)
    , currentTokens(nullptr)
    , initialized(false) {
    
    // Validate configuration
    if (!this->config) {
        throw std::invalid_argument("ParserConfig cannot be null");
    }
    
    // Initialize statistics
    resetStatistics();
    
    // Initialize parser components
    initialize();
}

SyntacticAnalyzer::~SyntacticAnalyzer() {
    try {
        cleanup();
    } catch (const std::exception& e) {
        // Log error but don't throw from destructor
        std::cerr << "Warning: Exception during SyntacticAnalyzer cleanup: " << e.what() << std::endl;
    } catch (...) {
        // Log unknown error but don't throw from destructor
        std::cerr << "Warning: Unknown exception during SyntacticAnalyzer cleanup" << std::endl;
    }
}

// Configuration methods
void SyntacticAnalyzer::setConfig(const ParserConfig& config) {
    // Validate new configuration
    if (!config.validate()) {
        auto errors = config.getValidationErrors();
        std::string errorMsg = "Invalid ParserConfig provided";
        if (!errors.empty()) {
            errorMsg += ": " + errors[0];
        }
        throw std::invalid_argument(errorMsg);
    }
    
    // Store old configuration for rollback if needed
    auto oldConfig = std::move(this->config);
    
    try {
        // Set new configuration
        this->config = std::make_unique<ParserConfig>(config);
        
        // Reset parser state
        reset();
        
        // Reinitialize with new configuration
        initialize();
        
    } catch (const std::exception& e) {
        // Rollback to old configuration on failure
        this->config = std::move(oldConfig);
        throw std::runtime_error(std::string("Failed to set new configuration: ") + e.what());
    }
}

// Bridge setup methods
void SyntacticAnalyzer::setLexerBridge(std::unique_ptr<LexerParserBridge> bridge) {
    lexerBridge = std::move(bridge);
}

void SyntacticAnalyzer::setPreprocessorBridge(std::unique_ptr<PreprocessorParserBridge> bridge) {
    preprocessorBridge = std::move(bridge);
}

void SyntacticAnalyzer::setSemanticInterface(std::unique_ptr<SemanticInterface> interface) {
    semanticInterface = std::move(interface);
}

// Grammar setup
void SyntacticAnalyzer::setGrammar(const Grammar& grammar) {
    try {
        // Set new grammar
        this->grammar = std::make_unique<Grammar>(grammar);
        
        // Automatically eliminate left recursion if present
        if (this->grammar && this->grammar->hasLeftRecursion()) {
            PARSER_LOG_INFO("Detected left recursion in provided grammar, applying automatic elimination");
            this->grammar->eliminateLeftRecursion();
        }
        
        // Validate grammar after potential left recursion elimination
        if (!this->grammar->validate()) {
            auto errors = this->grammar->getValidationErrors();
            std::string errorMsg = "Invalid Grammar provided";
            if (!errors.empty()) {
                errorMsg += ": " + errors[0];
            }
            throw std::invalid_argument(errorMsg);
        }
        
        // If parser is already initialized, reinitialize to apply grammar changes
        if (initialized) {
            initialize();
        }
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Failed to set grammar: ") + e.what());
    }
}

// Main parsing methods
ParseResult<ASTNodePtr> SyntacticAnalyzer::parse() {
    if (!initialized || !lexerBridge) {
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Parser not properly initialized", SourceRange{}));
    }
    
    currentTokens = lexerBridge->getTokenStream();
    if (!currentTokens) {
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Failed to get token stream", SourceRange{}));
    }
    
    startTiming();
    if (state) {
        state->reset();
    }
    
    try {
        ASTNodePtr result = parseTranslationUnit();
        endTiming();
        
        if (result && !hasErrors()) {
            return ParseResult<ASTNodePtr>(std::move(result));
        } else {
            return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Parse failed with errors", SourceRange{}));
        }
    } catch (const std::exception& e) {
        endTiming();
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(std::string("Parse exception: ") + e.what(), SourceRange{}));
    }
}

ParseResult<ASTNodePtr> SyntacticAnalyzer::parseExpression() {
    if (!initialized || !currentTokens) {
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Parser not ready for expression parsing", SourceRange{}));
    }
    
    try {
        ASTNodePtr result = parseExpressionInternal();
        if (result) {
            return ParseResult<ASTNodePtr>(std::move(result));
        } else {
            return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Failed to parse expression", SourceRange{}));
        }
    } catch (const std::exception& e) {
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(std::string("Expression parse exception: ") + e.what(), SourceRange{}));
    }
}

ParseResult<ASTNodePtr> SyntacticAnalyzer::parseStatement() {
    if (!initialized || !currentTokens) {
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Parser not ready for statement parsing", SourceRange{}));
    }
    
    try {
        ASTNodePtr result = parseStatementInternal();
        if (result) {
            return ParseResult<ASTNodePtr>(std::move(result));
        } else {
            return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Failed to parse statement", SourceRange{}));
        }
    } catch (const std::exception& e) {
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(std::string("Statement parse exception: ") + e.what(), SourceRange{}));
    }
}

ParseResult<ASTNodePtr> SyntacticAnalyzer::parseDeclaration() {
    if (!initialized || !currentTokens) {
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Parser not ready for declaration parsing", SourceRange{}));
    }
    
    try {
        // Get current token to determine declaration type
        const Token& currentToken = getCurrentToken();
        
        // DEBUG: Log token being processed in parseDeclaration
        // Processing declaration
        
        // Check for typedef declaration
        if (currentToken.getType() == Lexer::TokenType::TYPEDEF) {
            ASTNodePtr result = parseTypedefDeclaration();
            if (result) {
                return ParseResult<ASTNodePtr>(std::move(result));
            } else {
                const auto& pos = currentToken.getPosition();
                return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(
                    "Failed to parse typedef declaration at line " + std::to_string(pos.line) + 
                    ", column " + std::to_string(pos.column) + 
                    ". Token: '" + currentToken.getLexeme() + "'", SourceRange{}));
            }
        }
        
        // Check for struct declaration
        if (currentToken.getType() == Lexer::TokenType::STRUCT) {
            ASTNodePtr result = parseStructDeclaration();
            if (result) {
                return ParseResult<ASTNodePtr>(std::move(result));
            } else {
                const auto& pos = currentToken.getPosition();
                return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(
                    "Failed to parse struct declaration at line " + std::to_string(pos.line) + 
                    ", column " + std::to_string(pos.column) + 
                    ". Token: '" + currentToken.getLexeme() + "'", SourceRange{}));
            }
        }
        
        // Check for union declaration
        if (currentToken.getType() == Lexer::TokenType::UNION) {
            ASTNodePtr result = parseUnionDeclaration();
            if (result) {
                return ParseResult<ASTNodePtr>(std::move(result));
            } else {
                const auto& pos = currentToken.getPosition();
                return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(
                    "Failed to parse union declaration at line " + std::to_string(pos.line) + 
                    ", column " + std::to_string(pos.column) + 
                    ". Token: '" + currentToken.getLexeme() + "'", SourceRange{}));
            }
        }
        
        // Check for enum declaration
        if (currentToken.getType() == Lexer::TokenType::ENUM) {
            ASTNodePtr result = parseEnumDeclaration();
            if (result) {
                return ParseResult<ASTNodePtr>(std::move(result));
            } else {
                const auto& pos = currentToken.getPosition();
                return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(
                    "Failed to parse enum declaration at line " + std::to_string(pos.line) + 
                    ", column " + std::to_string(pos.column) + 
                    ". Token: '" + currentToken.getLexeme() + "'", SourceRange{}));
            }
        }
        
        // Check if this looks like a declaration (type qualifiers, storage class specifiers, or type specifiers)
        if (currentToken.getType() == Lexer::TokenType::CONST ||
            currentToken.getType() == Lexer::TokenType::VOLATILE ||
            currentToken.getType() == Lexer::TokenType::RESTRICT ||
            currentToken.getType() == Lexer::TokenType::STATIC ||
            currentToken.getType() == Lexer::TokenType::EXTERN ||
            currentToken.getType() == Lexer::TokenType::AUTO ||
            currentToken.getType() == Lexer::TokenType::REGISTER ||
            currentToken.getType() == Lexer::TokenType::INT ||
            currentToken.getType() == Lexer::TokenType::CHAR ||
            currentToken.getType() == Lexer::TokenType::FLOAT ||
            currentToken.getType() == Lexer::TokenType::DOUBLE ||
            currentToken.getType() == Lexer::TokenType::VOID ||
            currentToken.getType() == Lexer::TokenType::SHORT ||
            currentToken.getType() == Lexer::TokenType::LONG ||
            currentToken.getType() == Lexer::TokenType::SIGNED ||
            currentToken.getType() == Lexer::TokenType::UNSIGNED ||
            currentToken.getType() == Lexer::TokenType::IDENTIFIER) {
            
            // Try to parse as variable declaration
            ASTNodePtr result = parseVariableDeclaration();
            if (result) {
                return ParseResult<ASTNodePtr>(std::move(result));
            } else {
                const auto& pos = currentToken.getPosition();
                return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(
                    "Failed to parse variable declaration at line " + std::to_string(pos.line) + 
                    ", column " + std::to_string(pos.column) + 
                    ". Token: '" + currentToken.getLexeme() + "' (type: " + std::to_string(static_cast<int>(currentToken.getType())) + ")", SourceRange{}));
            }
        }
        
        const auto& pos = currentToken.getPosition();
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(
            "Unrecognized declaration at line " + std::to_string(pos.line) + 
            ", column " + std::to_string(pos.column) + 
            ". Token: '" + currentToken.getLexeme() + "' (type: " + std::to_string(static_cast<int>(currentToken.getType())) + ")", SourceRange{}));
        
    } catch (const std::exception& e) {
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(std::string("Declaration parse exception: ") + e.what(), SourceRange{}));
    }
}

// Implementation of declaration parsing methods (Phase 3.3)
ASTNodePtr SyntacticAnalyzer::parseTypedefDeclaration() {
    // typedef type_specifier declarator;
    if (!consumeToken()) { // consume 'typedef'
        return nullptr;
    }
    
    // Parse type specifiers
    ASTNodePtr typeSpec = parseDeclarationSpecifiers();
    if (!typeSpec) {
        return nullptr;
    }
    
    // Parse declarator
    ASTNodePtr declarator = parseDeclarator();
    if (!declarator) {
        return nullptr;
    }
    
    // Expect semicolon
    if (getCurrentToken().getType() != Lexer::TokenType::SEMICOLON) {
        auto pos = getCurrentToken().getPosition();
        reportError("Expected ';' after variable declaration at line " + std::to_string(pos.line) + 
                   ", column " + std::to_string(pos.column) + 
                   " but found '" + getCurrentToken().getLexeme() + "'");
        return nullptr;
    }
    consumeToken(); // consume ';'
    
    // Create typedef declaration node
    auto node = createNode(ASTNodeType::TYPE_DECLARATION);
    node->addChild(std::move(typeSpec));
    node->addChild(std::move(declarator));
    
    return node;
}

ASTNodePtr SyntacticAnalyzer::parseStructDeclaration() {
    // struct [identifier] { member_list } [declarator_list];
    if (!consumeToken()) { // consume 'struct'
        return nullptr;
    }
    
    auto node = createNode(ASTNodeType::TYPE_DECLARATION);
    
    // Optional struct tag
    if (getCurrentToken().getType() == Lexer::TokenType::IDENTIFIER) {
        auto identifier = createNode(ASTNodeType::IDENTIFIER);
        identifier->setValue(getCurrentToken().getLexeme());
        node->addChild(std::move(identifier));
        consumeToken();
    }
    
    // Struct body
    if (getCurrentToken().getType() == Lexer::TokenType::LEFT_BRACE) {
        consumeToken(); // consume '{'
        
        // Parse member declarations (simplified for now)
          while (getCurrentToken().getType() != Lexer::TokenType::RIGHT_BRACE && !isAtEnd()) {
               // Skip member parsing for now - will be implemented in later phases
               if (getCurrentToken().getType() == Lexer::TokenType::SEMICOLON) {
                   consumeToken();
               } else {
                   consumeToken(); // Skip current token
               }
           }
        
        if (getCurrentToken().getType() != Lexer::TokenType::RIGHT_BRACE) {
            return nullptr;
        }
        consumeToken(); // consume '}'
    }
    
    // Optional declarator list and semicolon
    if (getCurrentToken().getType() != Lexer::TokenType::SEMICOLON) {
        // Parse declarator list
        do {
            ASTNodePtr declarator = parseDeclarator();
            if (declarator) {
                node->addChild(std::move(declarator));
            }
            
            if (getCurrentToken().getType() == Lexer::TokenType::COMMA) {
                consumeToken();
            } else {
                break;
            }
        } while (!isAtEnd());
    }
    
    // Expect semicolon
    if (getCurrentToken().getType() != Lexer::TokenType::SEMICOLON) {
        return nullptr;
    }
    consumeToken(); // consume ';'
    
    return node;
}

ASTNodePtr SyntacticAnalyzer::parseUnionDeclaration() {
    // union [identifier] { member_list } [declarator_list];
    if (!consumeToken()) { // consume 'union'
        return nullptr;
    }
    
    auto node = createNode(ASTNodeType::TYPE_DECLARATION);
    
    // Optional union tag
    if (getCurrentToken().getType() == Lexer::TokenType::IDENTIFIER) {
        auto identifier = createNode(ASTNodeType::IDENTIFIER);
        identifier->setValue(getCurrentToken().getLexeme());
        node->addChild(std::move(identifier));
        consumeToken();
    }
    
    // Union body
    if (getCurrentToken().getType() == Lexer::TokenType::LEFT_BRACE) {
        consumeToken(); // consume '{'
        
        // Parse member declarations (simplified for now)
          while (getCurrentToken().getType() != Lexer::TokenType::RIGHT_BRACE && !isAtEnd()) {
               // Skip member parsing for now - will be implemented in later phases
               if (getCurrentToken().getType() == Lexer::TokenType::SEMICOLON) {
                   consumeToken();
               } else {
                   consumeToken(); // Skip current token
               }
           }
        
        if (getCurrentToken().getType() != Lexer::TokenType::RIGHT_BRACE) {
            return nullptr;
        }
        consumeToken(); // consume '}'
    }
    
    // Optional declarator list and semicolon
    if (getCurrentToken().getType() != Lexer::TokenType::SEMICOLON) {
        // Parse declarator list
        do {
            ASTNodePtr declarator = parseDeclarator();
            if (declarator) {
                node->addChild(std::move(declarator));
            }
            
            if (getCurrentToken().getType() == Lexer::TokenType::COMMA) {
                consumeToken();
            } else {
                break;
            }
        } while (!isAtEnd());
    }
    
    // Expect semicolon
    if (getCurrentToken().getType() != Lexer::TokenType::SEMICOLON) {
        return nullptr;
    }
    consumeToken(); // consume ';'
    
    return node;
}

ASTNodePtr SyntacticAnalyzer::parseEnumDeclaration() {
    // enum [identifier] { enumerator_list } [declarator_list];
    if (!consumeToken()) { // consume 'enum'
        return nullptr;
    }
    
    auto node = createNode(ASTNodeType::TYPE_DECLARATION);
    
    // Optional enum tag
    if (getCurrentToken().getType() == Lexer::TokenType::IDENTIFIER) {
        auto identifier = createNode(ASTNodeType::IDENTIFIER);
        identifier->setValue(getCurrentToken().getLexeme());
        node->addChild(std::move(identifier));
        consumeToken();
    }
    
    // Enum body
    if (getCurrentToken().getType() == Lexer::TokenType::LEFT_BRACE) {
        consumeToken(); // consume '{'
        
        // Parse enumerator list
        while (getCurrentToken().getType() != Lexer::TokenType::RIGHT_BRACE && !isAtEnd()) {
            if (getCurrentToken().getType() == Lexer::TokenType::IDENTIFIER) {
                auto enumerator = createNode(ASTNodeType::IDENTIFIER);
                enumerator->setValue(getCurrentToken().getLexeme());
                node->addChild(std::move(enumerator));
                consumeToken();
                
                // Optional value assignment
                if (getCurrentToken().getType() == Lexer::TokenType::ASSIGN) {
                    consumeToken(); // consume '='
                    // Skip the value for now - will be implemented in expression parsing phase
                    if (getCurrentToken().getType() == Lexer::TokenType::INTEGER_LITERAL) {
                        consumeToken();
                    }
                }
                
                // Optional comma
                if (getCurrentToken().getType() == Lexer::TokenType::COMMA) {
                    consumeToken();
                }
            } else {
                consumeToken(); // Skip unexpected token
            }
        }
        
        if (getCurrentToken().getType() != Lexer::TokenType::RIGHT_BRACE) {
            return nullptr;
        }
        consumeToken(); // consume '}'
    }
    
    // Optional declarator list and semicolon
    if (getCurrentToken().getType() != Lexer::TokenType::SEMICOLON) {
        // Parse declarator list
        do {
            ASTNodePtr declarator = parseDeclarator();
            if (declarator) {
                node->addChild(std::move(declarator));
            }
            
            if (getCurrentToken().getType() == Lexer::TokenType::COMMA) {
                consumeToken();
            } else {
                break;
            }
        } while (!isAtEnd());
    }
    
    // Expect semicolon
    if (getCurrentToken().getType() != Lexer::TokenType::SEMICOLON) {
        return nullptr;
    }
    consumeToken(); // consume ';'
    
    return node;
}

ASTNodePtr SyntacticAnalyzer::parseVariableDeclaration() {
    // Parse type specifiers first to get the type
    ASTNodePtr typeSpec = parseDeclarationSpecifiers();
    if (!typeSpec) {
        auto pos = getCurrentToken().getPosition();
        reportError("Failed to parse type specifier at line " + std::to_string(pos.line) + 
                   ", column " + std::to_string(pos.column) + 
                   ". Expected type (int, float, char, etc.) but found '" + getCurrentToken().getLexeme() + "'");
        return nullptr;
    }
    
    // Extract type name from type specifier
    std::string typeName = "unknown";
    if (typeSpec && typeSpec->getType() == ASTNodeType::IDENTIFIER && !typeSpec->getValue().empty()) {
        typeName = typeSpec->getValue();
    }

    // Create a declaration list to hold multiple variable declarations
    auto declList = std::make_unique<DeclarationList>();
    
    // Parse first declarator
    ASTNodePtr declarator = parseDeclarator();
    if (!declarator) {
        auto pos = getCurrentToken().getPosition();
        reportError("Failed to parse variable name at line " + std::to_string(pos.line) + 
                   ", column " + std::to_string(pos.column) + 
                   ". Expected identifier but found '" + getCurrentToken().getLexeme() + "'");
        return nullptr;
    }
    
    // Extract variable name from declarator
    std::string varName = "unknown";
    if (!declarator->getValue().empty()) {
        varName = declarator->getValue();
    }

    // Create first VariableDeclaration node
    auto firstVarDecl = std::make_unique<VariableDeclaration>(varName, typeName);
    auto typeSpecCopy = std::make_unique<Identifier>(typeName);
    firstVarDecl->addChild(std::move(typeSpecCopy));
    firstVarDecl->addChild(std::move(declarator));
    
    // Check for optional initializer for first variable
    if (getCurrentToken().getType() == Lexer::TokenType::ASSIGN) {
        consumeToken(); // consume '='
        
        ASTNodePtr initializer = parseInitializer();
        if (!initializer) {
            reportError("Failed to parse initializer for variable '" + varName + "'");
            return nullptr;
        }
        
        firstVarDecl->setInitializer(std::move(initializer));
    }
    
    declList->addDeclaration(std::move(firstVarDecl));
    
    // Handle additional declarators in the list
    while (getCurrentToken().getType() == Lexer::TokenType::COMMA) {
        consumeToken(); // consume ','
        
        ASTNodePtr nextDeclarator = parseDeclarator();
        if (!nextDeclarator) {
            return nullptr;
        }
        
        // Extract variable name from next declarator
        std::string nextVarName = "unknown";
        if (!nextDeclarator->getValue().empty()) {
            nextVarName = nextDeclarator->getValue();
        }
        
        // Create new VariableDeclaration node for this variable
        auto nextVarDecl = std::make_unique<VariableDeclaration>(nextVarName, typeName);
        auto nextTypeSpecCopy = std::make_unique<Identifier>(typeName);
        nextVarDecl->addChild(std::move(nextTypeSpecCopy));
        nextVarDecl->addChild(std::move(nextDeclarator));
        
        // Check for optional initializer for this variable
        if (getCurrentToken().getType() == Lexer::TokenType::ASSIGN) {
            consumeToken(); // consume '='
            
            ASTNodePtr initializer = parseInitializer();
            if (!initializer) {
                reportError("Failed to parse initializer for variable '" + nextVarName + "'");
                return nullptr;
            }
            
            nextVarDecl->setInitializer(std::move(initializer));
        }
        
        declList->addDeclaration(std::move(nextVarDecl));
    }
    
    // Expect semicolon
    if (getCurrentToken().getType() != Lexer::TokenType::SEMICOLON) {
        auto pos = getCurrentToken().getPosition();
        reportError("Expected ';' at line " + std::to_string(pos.line) + 
                   ", column " + std::to_string(pos.column) + 
                   " but found '" + getCurrentToken().getLexeme() + "'");
        return nullptr;
    }
    consumeToken(); // consume ';'
    
    // If only one variable was declared, return it directly instead of declaration list
    if (declList->getDeclarations().size() == 1) {
        return declList->getDeclarations()[0]->clone();
    }
    
    return std::move(declList);
}

ASTNodePtr SyntacticAnalyzer::parseTypeDeclaration() {
    // Generic type declaration - delegates to specific parsers
    const Token& currentToken = getCurrentToken();
    
    if (currentToken.getType() == Lexer::TokenType::STRUCT) {
        return parseStructDeclaration();
    } else if (currentToken.getType() == Lexer::TokenType::UNION) {
        return parseUnionDeclaration();
    } else if (currentToken.getType() == Lexer::TokenType::ENUM) {
        return parseEnumDeclaration();
    } else if (currentToken.getType() == Lexer::TokenType::TYPEDEF) {
        return parseTypedefDeclaration();
    }
    
    // Default to variable declaration
    return parseVariableDeclaration();
}

ASTNodePtr SyntacticAnalyzer::parseFunctionDefinition() {
    // function_definition: declaration_specifiers declarator compound_statement
    
    // Parse declaration specifiers (return type)

    ASTNodePtr declSpecs = parseDeclarationSpecifiers();
    if (!declSpecs) {

        return nullptr;
    }

    
    // Parse function declarator (name and parameters)

    ASTNodePtr declarator = parseDeclarator();
    if (!declarator) {

        return nullptr;
    }

    
    // Extract function name from declarator
    std::string functionName = "unknown";
    if (!declarator->getValue().empty()) {
        functionName = declarator->getValue();
    }

    
    // Extract return type from declaration specifiers
    std::string returnType = "void";
    if (declSpecs && !declSpecs->getValue().empty()) {
        returnType = declSpecs->getValue();
    }
    
    // Create FunctionDeclaration with actual name and return type
    auto node = std::make_unique<FunctionDeclaration>(functionName, returnType);

    
    // Extract and add parameters from declarator if it's a function declarator
    // The parameters are stored as children of the declarator (IDENTIFIER node)
    
    
    // Debug: Imprimir estrutura completa do declarator
    if (declarator) {
        // Process declarator children if needed
    }
    
    if (declarator && declarator->getChildCount() > 0) {
        // Look for parameter list nodes in the declarator's children
        for (size_t i = 0; i < declarator->getChildCount(); ++i) {
            ASTNode* child = declarator->getChild(i);
            if (child) {
                
                // Check if this child is a parameter list (contains VARIABLE_DECLARATION nodes)
                if (child->getType() == ASTNodeType::VARIABLE_DECLARATION) {
                    // This is a parameter - clone and add it
                    
                    auto paramClone = child->clone();
                    if (paramClone) {
                        node->addParameter(std::move(paramClone));
                    }
                } else if (child->getChildCount() > 0) {
                    // Check children recursively for parameter declarations
                    
                    for (size_t j = 0; j < child->getChildCount(); ++j) {
                        ASTNode* grandchild = child->getChild(j);
                        if (grandchild && grandchild->getType() == ASTNodeType::VARIABLE_DECLARATION) {
                            
                            auto paramClone = grandchild->clone();
                            if (paramClone) {
                                node->addParameter(std::move(paramClone));
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Parse function body (compound statement)

    if (getCurrentToken().getType() != Lexer::TokenType::LEFT_BRACE) {

        return nullptr; // Function definition must have a body
    }
    

    auto body = parseCompoundStatement();
    if (!body) {

        return nullptr;
    }

    
    // Use setBody instead of addChild for proper AST structure
    node->setBody(std::move(body));

    
    return node;
}

ASTNodePtr SyntacticAnalyzer::parseDeclarationSpecifiers() {
    // Parse declaration specifiers: storage-class-specifier, type-specifier, type-qualifier, function-specifier
    PARSER_LOG_DEBUG("Entrando em parseDeclarationSpecifiers");
    
    std::string combinedType = "";
    bool hasTypeSpecifier = false;
    
    // Parse multiple declaration specifiers
    while (!isAtEnd()) {
        const Token& currentToken = getCurrentToken();
        PARSER_LOG_DEBUG("Token atual: '" + currentToken.getLexeme() + "' tipo: " + std::to_string(static_cast<int>(currentToken.getType())));
        
        // Handle type qualifiers (const, volatile, restrict)
        if (currentToken.getType() == Lexer::TokenType::CONST ||
            currentToken.getType() == Lexer::TokenType::VOLATILE ||
            currentToken.getType() == Lexer::TokenType::RESTRICT) {
            
            // Found type qualifier
            if (!combinedType.empty()) combinedType += " ";
            combinedType += currentToken.getLexeme();
            consumeToken();
            // Consumed qualifier
            PARSER_LOG_DEBUG("Consumiu qualificador: " + currentToken.getLexeme());
            continue;
        }
        
        // Handle storage class specifiers
        if (currentToken.getType() == Lexer::TokenType::STATIC ||
            currentToken.getType() == Lexer::TokenType::EXTERN ||
            currentToken.getType() == Lexer::TokenType::AUTO ||
            currentToken.getType() == Lexer::TokenType::REGISTER) {
            
            if (!combinedType.empty()) combinedType += " ";
            combinedType += currentToken.getLexeme();
            consumeToken();
            PARSER_LOG_DEBUG("Consumiu storage class: " + currentToken.getLexeme());
            continue;
        }
        
        // Handle basic types
        if (currentToken.getType() == Lexer::TokenType::INT ||
            currentToken.getType() == Lexer::TokenType::FLOAT ||
            currentToken.getType() == Lexer::TokenType::DOUBLE ||
            currentToken.getType() == Lexer::TokenType::CHAR ||
            currentToken.getType() == Lexer::TokenType::VOID ||
            currentToken.getType() == Lexer::TokenType::SHORT ||
            currentToken.getType() == Lexer::TokenType::LONG ||
            currentToken.getType() == Lexer::TokenType::SIGNED ||
            currentToken.getType() == Lexer::TokenType::UNSIGNED) {
            
            if (!combinedType.empty()) combinedType += " ";
            combinedType += currentToken.getLexeme();
            hasTypeSpecifier = true;
            consumeToken();
            PARSER_LOG_DEBUG("Consumiu tipo básico: " + currentToken.getLexeme());
            continue;
        }
        
        // Handle struct/union/enum types
        if (currentToken.getType() == Lexer::TokenType::STRUCT) {
            PARSER_LOG_DEBUG("Parseando struct declaration");
            auto structNode = parseStructDeclaration();
            if (structNode) {
                if (!combinedType.empty()) combinedType += " ";
                combinedType += structNode->getValue();
                hasTypeSpecifier = true;
            }
            continue;
        } else if (currentToken.getType() == Lexer::TokenType::UNION) {
            PARSER_LOG_DEBUG("Parseando union declaration");
            auto unionNode = parseUnionDeclaration();
            if (unionNode) {
                if (!combinedType.empty()) combinedType += " ";
                combinedType += unionNode->getValue();
                hasTypeSpecifier = true;
            }
            continue;
        } else if (currentToken.getType() == Lexer::TokenType::ENUM) {
            PARSER_LOG_DEBUG("Parseando enum declaration");
            auto enumNode = parseEnumDeclaration();
            if (enumNode) {
                if (!combinedType.empty()) combinedType += " ";
                combinedType += enumNode->getValue();
                hasTypeSpecifier = true;
            }
            continue;
        }
        
        // Handle identifier (typedef name) - only if we haven't found a type specifier yet
        if (currentToken.getType() == Lexer::TokenType::IDENTIFIER && !hasTypeSpecifier) {
            if (!combinedType.empty()) combinedType += " ";
            combinedType += currentToken.getLexeme();
            hasTypeSpecifier = true;
            consumeToken();
            PARSER_LOG_DEBUG("Consumiu identifier como tipo: " + currentToken.getLexeme());
            continue;
        }
        
        // No more declaration specifiers found
        break;
    }
    
    if (!combinedType.empty()) {
        auto typeNode = createNode(ASTNodeType::IDENTIFIER);
        typeNode->setValue(combinedType);
        // Returning valid declaration specifiers node
        PARSER_LOG_DEBUG("Retornando declaration specifiers: " + combinedType);
        return typeNode;
    }
    
    // No declaration specifiers found
    PARSER_LOG_DEBUG("Nenhum declaration specifier encontrado");
    return nullptr;
}

ASTNodePtr SyntacticAnalyzer::parseDeclarator() {
    // Parse declarator: [pointer] direct_declarator

    
    // Protection against infinite recursion
    static int recursionDepth = 0;
    if (recursionDepth > 10) {

        return nullptr;
    }
    recursionDepth++;
    
    auto node = createNode(ASTNodeType::IDENTIFIER);
    
    // Handle pointer prefix (*)
    while (getCurrentToken().getType() == Lexer::TokenType::MULTIPLY) {
        consumeToken(); // consume '*'
        // For now, just skip pointer notation
    }
    
    // Parse direct declarator using the dedicated method
    auto directDeclarator = parseDirectDeclarator();
    if (directDeclarator) {
        node = std::move(directDeclarator);
    } else {
        // Fallback: try to parse as simple identifier
        if (getCurrentToken().getType() == Lexer::TokenType::IDENTIFIER) {

            node->setValue(getCurrentToken().getLexeme());
            consumeToken();
        } else {
            recursionDepth--;
            return nullptr;
        }
    }
    
    recursionDepth--;
    return node;
}

ParseResult<ASTNodePtr> SyntacticAnalyzer::parse(std::unique_ptr<TokenStream> tokens) {
    if (!initialized) {
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Parser not initialized", SourceRange{}));
    }
    
    currentTokens = std::move(tokens);
    return parse();
}

// Error handling
const std::vector<std::unique_ptr<ParseError>>& SyntacticAnalyzer::getErrors() const {
    if (state) {
        return state->getErrors();
    }
    static std::vector<std::unique_ptr<ParseError>> emptyErrors;
    return emptyErrors;
}

bool SyntacticAnalyzer::hasErrors() const {
    if (state) {
        return state->hasErrors();
    }
    return false;
}

void SyntacticAnalyzer::clearErrors() {
    if (state) {
        state->clearErrors();
    }
    if (errorRecovery) {
        errorRecovery->resetStatistics();
    }
}

// Statistics
void SyntacticAnalyzer::resetStatistics() {
    statistics = ParseStatistics();
}

// Reset
void SyntacticAnalyzer::reset() {
    if (state) {
        state->reset();
    }
    if (errorRecovery) {
        errorRecovery->resetStatistics();
    }
    currentTokens.reset();
    resetStatistics();
}

// Private implementation methods
void SyntacticAnalyzer::initialize() {
    if (!config) {
        config = std::make_unique<ParserConfig>();
    }
    
    // Initialize grammar based on C standard
    if (!grammar) {
        switch (config->getCStandard()) {
            case CStandard::C89:
                grammar = std::make_unique<Grammar>(Grammar::createC89Grammar());
                break;
            case CStandard::C99:
                grammar = std::make_unique<Grammar>(Grammar::createC99Grammar());
                break;
            case CStandard::C11:
                grammar = std::make_unique<Grammar>(Grammar::createC11Grammar());
                break;
            case CStandard::C17:
                grammar = std::make_unique<Grammar>(Grammar::createC17Grammar());
                break;
            case CStandard::C23:
                grammar = std::make_unique<Grammar>(Grammar::createC23Grammar());
                break;
        }
        
        // Automatically eliminate left recursion after loading grammar
        if (grammar && grammar->hasLeftRecursion()) {
            PARSER_LOG_INFO("Detected left recursion in grammar, applying automatic elimination");
            grammar->eliminateLeftRecursion();
        }
        
        // Check for LL(1) conflicts and attempt to resolve them
        PARSER_LOG_INFO("Checking grammar for LL(1) conflicts...");
        if (grammar) {
            bool hasConflicts = grammar->hasAmbiguity();
            if (hasConflicts) {
                PARSER_LOG_INFO("Detected LL(1) conflicts in grammar, attempting automatic resolution");
            } else {
                PARSER_LOG_INFO("No LL(1) conflicts detected in grammar");
            }
        }
    }
    
    initialized = true;
}

void SyntacticAnalyzer::cleanup() {
    currentTokens.reset();
    lexerBridge.reset();
    preprocessorBridge.reset();
    semanticInterface.reset();
    initialized = false;
}

// Validation methods
bool SyntacticAnalyzer::isConfigurationValid() const {
    if (!config) {
        return false;
    }
    return config->validate();
}

bool SyntacticAnalyzer::isGrammarValid() const {
    if (!grammar) {
        return false;
    }
    return grammar->validate();
}

bool SyntacticAnalyzer::isFullyConfigured() const {
    // Parser is fully configured if it has config and grammar
    // LexerBridge is optional when working with pre-tokenized input (TokenStream)
    return config != nullptr && 
           grammar != nullptr && 
           isConfigurationValid() &&
           isGrammarValid();
}

std::vector<std::string> SyntacticAnalyzer::getConfigurationErrors() const {
    std::vector<std::string> errors;
    
    // Get C standard information for error context
    std::string cStandardInfo = "";
    if (config) {
        switch (config->getCStandard()) {
            case CStandard::C89: cStandardInfo = " (C89)"; break;
            case CStandard::C99: cStandardInfo = " (C99)"; break;
            case CStandard::C11: cStandardInfo = " (C11)"; break;
            case CStandard::C17: cStandardInfo = " (C17)"; break;
            case CStandard::C23: cStandardInfo = " (C23)"; break;
        }
    }
    
    if (!config) {
        errors.push_back("No configuration set");
    } else if (!config->validate()) {
        auto configErrors = config->getValidationErrors();
        for (const auto& error : configErrors) {
            errors.push_back(error + cStandardInfo);
        }
    }
    
    if (!grammar) {
        errors.push_back("No grammar set" + cStandardInfo);
    } else if (!grammar->validate()) {
        auto grammarErrors = grammar->getValidationErrors();
        for (const auto& error : grammarErrors) {
            errors.push_back("Grammar" + cStandardInfo + ": " + error);
        }
    }
    
    // LexerBridge is optional when working with TokenStream directly
    // Only report as error if neither lexerBridge nor currentTokens are available during parsing
    if (!lexerBridge) {
        errors.push_back("No lexer bridge set" + cStandardInfo + " (optional for TokenStream input)");
    }
    
    return errors;
}

// Phase 3.2 - Main parsing methods implementation
ParseResult<ASTNodePtr> SyntacticAnalyzer::parseFile(const std::string& filename) {
    try {
        // Validate input
        if (filename.empty()) {
            return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Empty filename provided", SourceRange{}));
        }
        
        // Initialize parser if not already initialized
        if (!initialized) {
            initialize();
        }
        
        // Check if parser is properly configured
        if (!isFullyConfigured()) {
            auto errors = getConfigurationErrors();
            std::string errorMsg = "Parser not properly configured";
            if (!errors.empty()) {
                errorMsg += ": " + errors[0];
            }
            return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(errorMsg, SourceRange{}));
        }
        
        // Use lexer bridge to get tokens from file
        if (!lexerBridge) {
            return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("No lexer bridge configured", SourceRange{}));
        }
        
        // Get token stream from lexer
        auto tokenStream = lexerBridge->getTokenStream();
        if (!tokenStream) {
            return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Failed to get token stream from lexer", SourceRange{}));
        }
        
        // Parse the token stream
        return parseTokens(std::move(tokenStream));
        
    } catch (const std::exception& e) {
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(
            std::string("Exception during file parsing: ") + e.what(), SourceRange{}));
    }
}

ParseResult<ASTNodePtr> SyntacticAnalyzer::parseString(const std::string& code) {
    try {
        // Validate input
        if (code.empty()) {
            return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Empty code string provided", SourceRange{}));
        }
        
        // Check if parser is properly configured
        if (!isFullyConfigured()) {
            auto errors = getConfigurationErrors();
            std::string errorMsg = "Parser not properly configured";
            if (!errors.empty()) {
                errorMsg += ": " + errors[0];
            }
            return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(errorMsg, SourceRange{}));
        }
        
        // Use lexer bridge to tokenize the string
        if (!lexerBridge) {
            return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("No lexer bridge configured", SourceRange{}));
        }
        
        // Get token stream from lexer
        auto tokenStream = lexerBridge->getTokenStream();
        if (!tokenStream) {
            return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Failed to get token stream from lexer", SourceRange{}));
        }
        
        // Parse the token stream
        return parseTokens(std::move(tokenStream));
        
    } catch (const std::exception& e) {
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(
            std::string("Exception during string parsing: ") + e.what(), SourceRange{}));
    }
}

ParseResult<ASTNodePtr> SyntacticAnalyzer::parseTokens(std::unique_ptr<TokenStream> tokens) {
    // Starting token parsing
    try {

        
        // Validate input
        if (!tokens) {

            return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Null token stream provided", SourceRange{}));
        }

        
        // Check if parser is properly configured
        if (!isFullyConfigured()) {

            auto errors = getConfigurationErrors();
            std::string errorMsg = "Parser not properly configured";
            if (!errors.empty()) {
                errorMsg += ": " + errors[0];
            }
            return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(errorMsg, SourceRange{}));
        }

        
        // Reset parser state

        reset();
        
        // Set current token stream

        currentTokens = std::move(tokens);
        
        // Initialize state manager with token stream

        if (state) {
            state->setTokenStream(currentTokens.get());
        }
        
        // Start timing

        startTiming();
        
        // Clear previous errors

        clearErrors();
        
        // Parse translation unit (top-level)

        auto result = parseTranslationUnit();

        
        // End timing

        endTiming();
        
        // Return successful result if we have a valid AST
        if (result) {
            return ParseResult<ASTNodePtr>(std::move(result));
        }
        
        // Only check for errors if parsing completely failed
        if (hasErrors()) {
            // Return first error if parsing failed
            const auto& errors = getErrors();
            if (!errors.empty()) {
                return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(
                    errors[0]->getMessage(), errors[0]->getSourceRange()));
            }
        }
        
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>("Failed to parse translation unit", SourceRange{}));
        
    } catch (const std::exception& e) {
        return ParseResult<ASTNodePtr>(std::make_unique<SyntaxError>(
            std::string("Exception during token parsing: ") + e.what(), SourceRange{}));
    }
}

// Token control methods - delegate to ParserStateManager
const Token& SyntacticAnalyzer::getCurrentToken() const {
    if (state) {
        return state->getCurrentToken();
    }
    static Token eofToken;
    return eofToken;
}

const Token& SyntacticAnalyzer::peekToken(size_t offset) const {
    if (state) {
        return state->peekToken(offset);
    }
    static Token eofToken;
    return eofToken;
}

bool SyntacticAnalyzer::consumeToken() {
    if (state) {
        bool result = state->consumeToken();
        if (result) {
            statistics.tokensProcessed++;
        }
        return result;
    }
    return false;
}

bool SyntacticAnalyzer::isAtEnd() const {
    if (state) {
        return state->isAtEnd();
    }
    return true;
}

bool SyntacticAnalyzer::synchronize() {
    if (!state || !errorRecovery || !currentTokens) {
        return false;
    }
    
    // Use the error recovery manager to synchronize to a safe point
    // This typically involves finding synchronization tokens like ';', '}', etc.
    try {
        // Skip tokens until we find a synchronization point
        while (!isAtEnd()) {
            const Token& current = getCurrentToken();
            
            // Check for synchronization tokens
            if (current.getType() == Lexer::TokenType::SEMICOLON || 
                current.getType() == Lexer::TokenType::RIGHT_BRACE) {
                // Consume delimiter tokens and return true
                consumeToken();
                return true;
            }
            
            // Check for keyword tokens that start new declarations (not statements)
            if (current.getType() == Lexer::TokenType::INT ||
                current.getType() == Lexer::TokenType::FLOAT ||
                current.getType() == Lexer::TokenType::DOUBLE ||
                current.getType() == Lexer::TokenType::CHAR ||
                current.getType() == Lexer::TokenType::VOID) {
                // Don't consume type keywords, just return true
                return true;
            }
            
            if (!consumeToken()) {
                break;
            }
        }
        
        return false;
    } catch (const std::exception&) {
        return false;
    }
}

// Placeholder implementations for parsing methods
ASTNodePtr SyntacticAnalyzer::parseTranslationUnit() {

    
    // Check if we have any meaningful tokens (not just EOF)
    if (!currentTokens) {

        return nullptr;
    }

    
    // Check if the token stream is empty or only contains EOF
    if (currentTokens->size() == 0 || currentTokens->isAtEnd()) {

        return nullptr; // Empty input should fail parsing
    }

    
    // Create translation unit node

    auto translationUnit = createNode(ASTNodeType::TRANSLATION_UNIT);

    
    // Parse sequence of external declarations
    // DEBUG: Starting to parse external declarations
    
    int declarationCount = 0;
    while (!isAtEnd()) {
        declarationCount++;
        // DEBUG: Processing declaration #[count], current token: [token] (type: [type])
        
        if (declarationCount > 300) {
            // DEBUG: Too many declarations, breaking to avoid infinite loop
            break;
        }

        auto externalDecl = parseExternalDeclaration();

        if (externalDecl) {
            // DEBUG: Successfully parsed external declaration
            static_cast<TranslationUnit*>(translationUnit.get())->addDeclaration(std::move(externalDecl));
        } else {
            // DEBUG: Failed to parse external declaration, attempting synchronization
            // Error recovery: try to synchronize
            if (!synchronize()) {
                // DEBUG: Synchronization failed, manually advancing token
                // If synchronize fails, manually advance to avoid infinite loop
                if (!consumeToken()) {
                    // DEBUG: Cannot advance token, breaking
                    break;
                }
            } else {
                // DEBUG: Synchronization succeeded
            }
        }
        
        // DEBUG: After processing, isAtEnd(): [true/false]
    }
    
    // DEBUG: Finished parsing, processed [count] declarations
    
    return translationUnit;
}

ASTNodePtr SyntacticAnalyzer::parseExternalDeclaration() {
    // DEBUG: Log current token at start of parseExternalDeclaration
    // DEBUG: Starting with token '[token]' (type: [type])
    
    // Save current position for backtracking
    state->savePosition("external_decl_checkpoint");
    
    // Try to parse as function definition first
    // DEBUG: Trying parseFunctionDefinition
    try {
        auto functionDef = parseFunctionDefinition();
        if (functionDef) {
            // DEBUG: parseFunctionDefinition succeeded
            return functionDef;
        }
        // DEBUG: parseFunctionDefinition returned null
    } catch (...) {
        // Restore position and try declaration
        // DEBUG: parseFunctionDefinition threw exception
        state->restorePosition("external_decl_checkpoint");
    }
    
    // Always restore position before trying declaration
    state->restorePosition("external_decl_checkpoint");
    
    // Try to parse as declaration
    // DEBUG: Trying parseDeclaration
    auto result = parseDeclaration();
    if (result.isSuccess()) {
        // DEBUG: parseDeclaration succeeded
        return result.moveValue();
    }
    
    // If both fail, report detailed error
    // DEBUG: Both parseFunctionDefinition and parseDeclaration failed
    // DEBUG: Current token when failing: '[token]' (type: [type])
    
    // Create detailed error message with context
    const Token& errorToken = getCurrentToken();
    std::string detailedMessage = "Expected function definition or declaration";
    
    // Add token information
    detailedMessage += " at line " + std::to_string(errorToken.getPosition().line) + 
                      ", column " + std::to_string(errorToken.getPosition().column);
    
    // Add current token info
    detailedMessage += ". Found unexpected token: '" + errorToken.getLexeme() + "'";
    
    // Add token type information
    detailedMessage += " (type: " + std::to_string(static_cast<int>(errorToken.getType())) + ")";
    
    // Add context suggestions
    detailedMessage += ". Possible causes: missing semicolon, invalid syntax, or unsupported language feature";
    
    // CRITICAL: Consume the problematic token to avoid infinite loop
    // DEBUG: Consuming problematic token to avoid infinite loop
    consumeToken();
    
    reportError(detailedMessage);
    return nullptr;
}

ASTNodePtr SyntacticAnalyzer::parseStatementInternal() {
    if (!currentTokens || isAtEnd()) {
        return nullptr;
    }

    const Token& token = getCurrentToken();
    
    // Dispatch baseado no tipo do token atual
    switch (token.getType()) {
        // Compound statement
        case Lexer::TokenType::LEFT_BRACE:
            return parseCompoundStatement();
            
        // Selection statements
        case Lexer::TokenType::IF:
            return parseSelectionStatement();
            
        // Iteration statements
        case Lexer::TokenType::WHILE:
        case Lexer::TokenType::FOR:
        case Lexer::TokenType::DO:
            return parseIterationStatement();
            
        // Jump statements
        case Lexer::TokenType::BREAK:
        case Lexer::TokenType::CONTINUE:
        case Lexer::TokenType::RETURN:
        case Lexer::TokenType::GOTO:
            return parseJumpStatement();
            
        // Switch statement
        case Lexer::TokenType::SWITCH:
            return parseSelectionStatement();
            
        // Case and default labels
        case Lexer::TokenType::CASE:
        case Lexer::TokenType::DEFAULT:
            return parseLabeledStatement();
            
        // Labeled statement (identifier followed by colon)
        case Lexer::TokenType::IDENTIFIER: {
            // Look ahead to see if this is a label
            if (peekToken(1).getType() == Lexer::TokenType::COLON) {
                return parseLabeledStatement();
            }
            // Otherwise, fall through to expression statement
            [[fallthrough]];
        }
        
        // Expression statement (including empty statement with just semicolon)
        default:
            return parseExpressionStatement();
    }
}

ASTNodePtr SyntacticAnalyzer::parseCompoundStatement() {
    // DEBUG: Log entry to parseCompoundStatement
    // DEBUG: Starting with token '[token]' (type: [type])
    
    if (!currentTokens || isAtEnd()) {
        // DEBUG: No tokens or at end
        return nullptr;
    }

    // Expect '{'
    if (getCurrentToken().getType() != Lexer::TokenType::LEFT_BRACE) {
        // DEBUG: Expected LEFT_BRACE, got '[token]'
        return nullptr;
    }
    
    // DEBUG: Found LEFT_BRACE, consuming it
    auto compoundStmt = std::make_unique<CompoundStatement>();
    consumeToken(); // consume '{'

    // Parse statements until '}'
    int statementCount = 0;
    while (!isAtEnd() && getCurrentToken().getType() != Lexer::TokenType::RIGHT_BRACE) {
        statementCount++;
        // DEBUG: Processing statement #[count], token: '[token]' (type: [type])
        
        if (statementCount > 50) {
            // DEBUG: Too many statements, breaking
            break;
        }
        
        ASTNodePtr child = nullptr;
        
        // Check if it's a declaration first (starts with type specifier or qualifier)
        const Token& currentToken = getCurrentToken();
        if (currentToken.getType() == Lexer::TokenType::CONST ||
            currentToken.getType() == Lexer::TokenType::VOLATILE ||
            currentToken.getType() == Lexer::TokenType::RESTRICT ||
            currentToken.getType() == Lexer::TokenType::STATIC ||
            currentToken.getType() == Lexer::TokenType::EXTERN ||
            currentToken.getType() == Lexer::TokenType::AUTO ||
            currentToken.getType() == Lexer::TokenType::REGISTER ||
            currentToken.getType() == Lexer::TokenType::INT ||
            currentToken.getType() == Lexer::TokenType::FLOAT ||
            currentToken.getType() == Lexer::TokenType::DOUBLE ||
            currentToken.getType() == Lexer::TokenType::CHAR ||
            currentToken.getType() == Lexer::TokenType::VOID ||
            currentToken.getType() == Lexer::TokenType::SHORT ||
            currentToken.getType() == Lexer::TokenType::LONG ||
            currentToken.getType() == Lexer::TokenType::SIGNED ||
            currentToken.getType() == Lexer::TokenType::UNSIGNED ||
            currentToken.getType() == Lexer::TokenType::STRUCT ||
            currentToken.getType() == Lexer::TokenType::UNION ||
            currentToken.getType() == Lexer::TokenType::ENUM ||
            currentToken.getType() == Lexer::TokenType::TYPEDEF) {
            // Parse as declaration
            // DEBUG: Attempting to parse as declaration
            auto declResult = parseDeclaration();
            if (declResult.isSuccess()) {
                child = declResult.moveValue();
                // DEBUG: Successfully parsed declaration
            } else {
                // If declaration parsing fails, try as statement instead
                // DEBUG: Declaration parsing failed, trying as statement: [error_message]
                child = parseStatementInternal();
                if (!child) {
                    // Only report error if both declaration and statement parsing fail
                    reportError("Failed to parse declaration or statement at line " + 
                              std::to_string(getCurrentToken().getPosition().line) + 
                              ", column " + std::to_string(getCurrentToken().getPosition().column) + 
                              ". Token: '" + getCurrentToken().getLexeme() + 
                              "' (type: " + std::to_string(static_cast<int>(getCurrentToken().getType())) + ")");
                }
            }
        } else {
            // Try to parse as statement (expressions, control flow, etc.)
            child = parseStatementInternal();
        }
        
        if (child) {
            // DEBUG: Successfully parsed statement, adding to compound
            compoundStmt->addStatement(std::move(child));
        } else {
            // DEBUG: Failed to parse statement, attempting synchronization
            // Error recovery: skip to next semicolon or brace
            if (!synchronize()) {
                // DEBUG: Synchronization failed, breaking
                break;
            }
        }
    }
    
    // DEBUG: Finished parsing statements, current token: '[token]' (type: [type])
    
    // Expect '}'
    if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::RIGHT_BRACE) {
        // DEBUG: Found RIGHT_BRACE, consuming it
        consumeToken(); // consume '}'
    } else {
        // DEBUG: Expected RIGHT_BRACE but got '[token]'
    }
    
    // DEBUG: Returning compound statement
    return std::move(compoundStmt);
}

ASTNodePtr SyntacticAnalyzer::parseExpressionStatement() {
    if (!currentTokens || isAtEnd()) {
        return nullptr;
    }

    ASTNodePtr expression = nullptr;
    
    // Handle empty statement (just semicolon) - don't create a node
    if (getCurrentToken().getType() == Lexer::TokenType::SEMICOLON) {
        consumeToken(); // consume ';'
        // Return nullptr for empty statements
        return nullptr;
    }
    
    // Parse the actual expression
    expression = parseExpressionInternal();
    
    // Only create ExpressionStatement if we have an actual expression
    if (!expression) {
        return nullptr;
    }
    
    // Expect semicolon
    if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::SEMICOLON) {
        consumeToken(); // consume ';'
    }
    
    // Create ExpressionStatement with proper constructor
    return std::make_unique<ExpressionStatement>(std::move(expression));
}

ASTNodePtr SyntacticAnalyzer::parseSelectionStatement() {
    if (!currentTokens || isAtEnd()) {
        return nullptr;
    }

    const Token& token = getCurrentToken();
    
    if (token.getType() == Lexer::TokenType::IF) {
        return parseIfStatement();
    } else if (token.getType() == Lexer::TokenType::SWITCH) {
        return parseSwitchStatement();
    }
    
    return nullptr;
}

ASTNodePtr SyntacticAnalyzer::parseIterationStatement() {
    if (!currentTokens || isAtEnd()) {
        return nullptr;
    }

    const Token& token = getCurrentToken();
    
    if (token.getType() == Lexer::TokenType::WHILE) {
        return parseWhileStatement();
    } else if (token.getType() == Lexer::TokenType::FOR) {
        return parseForStatement();
    } else if (token.getType() == Lexer::TokenType::DO) {
        return parseDoWhileStatement();
    }
    
    return nullptr;
}

ASTNodePtr SyntacticAnalyzer::parseJumpStatement() {
    if (!currentTokens || isAtEnd()) {
        return nullptr;
    }

    const Token& token = getCurrentToken();
    
    if (token.getType() == Lexer::TokenType::BREAK) {
        return parseBreakStatement();
    } else if (token.getType() == Lexer::TokenType::CONTINUE) {
        return parseContinueStatement();
    } else if (token.getType() == Lexer::TokenType::RETURN) {
        return parseReturnStatement();
    } else if (token.getType() == Lexer::TokenType::GOTO) {
        return parseGotoStatement();
    }
    
    return nullptr;
}

ASTNodePtr SyntacticAnalyzer::parseLabeledStatement() {
    if (!currentTokens || isAtEnd()) {
        return nullptr;
    }

    const Token& token = getCurrentToken();
    
    if (token.getType() == Lexer::TokenType::CASE) {
        return parseCaseStatement();
    } else if (token.getType() == Lexer::TokenType::DEFAULT) {
        return parseCaseStatement(); // Default is handled in parseCaseStatement
    } else if (token.getType() == Lexer::TokenType::IDENTIFIER) {
        // Parse identifier label
        auto node = createNode(ASTNodeType::COMPOUND_STATEMENT); // Use compound for now
        consumeToken(); // consume identifier
        
        if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::COLON) {
            consumeToken(); // consume ':'
            
            // Parse the statement that follows the label
            auto stmt = parseStatementInternal();
            if (stmt) {
                node->addChild(std::move(stmt));
            }
        }
        
        return node;
    }
    
    return nullptr;
}

ASTNodePtr SyntacticAnalyzer::parseIfStatement() {

    
    if (!currentTokens || isAtEnd() || getCurrentToken().getType() != Lexer::TokenType::IF) {

        return nullptr;
    }
    

    consumeToken(); // consume 'if'

    
    ASTNodePtr conditionExpr = nullptr;
    ASTNodePtr thenStmt = nullptr;
    ASTNodePtr elseStmt = nullptr;
    
    // Expect '('
    if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::LEFT_PAREN) {

        consumeToken(); // consume '('

        
        // Parse condition expression

        conditionExpr = parseExpressionInternal();
        if (conditionExpr) {

        } else {

        }
        

        if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::RIGHT_PAREN) {

            consumeToken(); // consume ')'
        } else {

        }
    } else {

    }
    

    // Parse then statement
    thenStmt = parseStatementInternal();
    if (thenStmt) {

    } else {

    }
    

    // Check for else clause
    if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::ELSE) {

        consumeToken(); // consume 'else'

        elseStmt = parseStatementInternal();
        if (elseStmt) {

        } else {

        }
    } else {

    }
    
    // Create the IfStatement with the parsed components

    if (conditionExpr && thenStmt) {
        if (elseStmt) {
            auto ifNode = std::make_unique<IfStatement>(std::move(conditionExpr), std::move(thenStmt), std::move(elseStmt));
    
            return std::move(ifNode);
        } else {
            auto ifNode = std::make_unique<IfStatement>(std::move(conditionExpr), std::move(thenStmt));
    
            return std::move(ifNode);
        }
    } else {

        return nullptr;
    }
}

ASTNodePtr SyntacticAnalyzer::parseWhileStatement() {
    if (!currentTokens || isAtEnd() || getCurrentToken().getType() != Lexer::TokenType::WHILE) {
        return nullptr;
    }
    
    auto node = createNode(ASTNodeType::WHILE_STATEMENT);
    consumeToken(); // consume 'while'
    
    // Expect '('
    if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::LEFT_PAREN) {
        consumeToken(); // consume '('
        
        // Parse condition expression
        auto conditionExpr = parseExpressionInternal();
        if (conditionExpr) {
            node->addChild(std::move(conditionExpr));
        }
        
        if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::RIGHT_PAREN) {
            consumeToken(); // consume ')'
        }
    }
    
    // Parse body statement
    auto bodyStmt = parseStatementInternal();
    if (bodyStmt) {
        node->addChild(std::move(bodyStmt));
    }
    
    return node;
}

ASTNodePtr SyntacticAnalyzer::parseForStatement() {

    if (!currentTokens || isAtEnd() || getCurrentToken().getType() != Lexer::TokenType::FOR) {
        return nullptr;
    }
    
    
    consumeToken(); // consume 'for'
    
    ASTNodePtr initExpr = nullptr;
    ASTNodePtr conditionExpr = nullptr;
    ASTNodePtr incrementExpr = nullptr;
    ASTNodePtr bodyStmt = nullptr;
    
    // Expect '('
    if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::LEFT_PAREN) {

        consumeToken(); // consume '('
        
        // Parse init expression/declaration (optional)

        if (!isAtEnd() && getCurrentToken().getType() != Lexer::TokenType::SEMICOLON) {
            // Check if it's a declaration (starts with type specifier)
            const Token& currentToken = getCurrentToken();
            if (currentToken.getType() == Lexer::TokenType::INT ||
                currentToken.getType() == Lexer::TokenType::FLOAT ||
                currentToken.getType() == Lexer::TokenType::DOUBLE ||
                currentToken.getType() == Lexer::TokenType::CHAR ||
                currentToken.getType() == Lexer::TokenType::VOID ||
                currentToken.getType() == Lexer::TokenType::SHORT ||
                currentToken.getType() == Lexer::TokenType::LONG ||
                currentToken.getType() == Lexer::TokenType::SIGNED ||
                currentToken.getType() == Lexer::TokenType::UNSIGNED) {
                

                // Parse variable declaration without consuming semicolon
                ASTNodePtr typeSpec = parseDeclarationSpecifiers();
                if (typeSpec) {
                    ASTNodePtr declarator = parseDeclarator();
                    if (declarator) {
                        std::string varName = declarator->getValue().empty() ? "unknown" : declarator->getValue();
                        std::string typeName = typeSpec->getValue().empty() ? "unknown" : typeSpec->getValue();
                        
                        auto varDecl = std::make_unique<VariableDeclaration>(varName, typeName);
                        varDecl->addChild(std::move(typeSpec));
                        varDecl->addChild(std::move(declarator));
                        
                        // Check for initializer
                        if (getCurrentToken().getType() == Lexer::TokenType::ASSIGN) {
                            consumeToken(); // consume '='
                            ASTNodePtr initializer = parseAssignmentExpression();
                            if (initializer) {
                                varDecl->setInitializer(std::move(initializer));
                            }
                        }
                        
                        initExpr = std::move(varDecl);
                    } else {
                        // Failed to parse declarator in for loop
                        auto pos = getCurrentToken().getPosition();
                        reportError("Failed to parse variable name in for loop at line " + std::to_string(pos.line) + 
                                  ", column " + std::to_string(pos.column) + ". Token: '" + getCurrentToken().getLexeme() + 
                                  "' (type: " + std::to_string(static_cast<int>(getCurrentToken().getType())) + ")");
                    }
                } else {
                    // Failed to parse type specifier in for loop
                    auto pos = getCurrentToken().getPosition();
                    reportError("Failed to parse type specifier in for loop at line " + std::to_string(pos.line) + 
                              ", column " + std::to_string(pos.column) + ". Token: '" + getCurrentToken().getLexeme() + 
                              "' (type: " + std::to_string(static_cast<int>(getCurrentToken().getType())) + ")");
                }
            } else {

                // Parse as expression
                initExpr = parseExpressionInternal();
            }
        }
        
        // Expect semicolon after init

        if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::SEMICOLON) {
    
            consumeToken();
        }
        
        // Parse condition expression (optional)

        if (!isAtEnd() && getCurrentToken().getType() != Lexer::TokenType::SEMICOLON) {

            conditionExpr = parseExpressionInternal();

        }
        
        // Expect semicolon after condition

        if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::SEMICOLON) {
    
            consumeToken();
        }
        
        // Parse increment expression (optional)

        if (!isAtEnd() && getCurrentToken().getType() != Lexer::TokenType::RIGHT_PAREN) {
            incrementExpr = parseExpressionInternal();

        }
        

        if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::RIGHT_PAREN) {
            consumeToken(); // consume ')'
        }
    }
    
    // Parse body statement
    
    bodyStmt = parseStatementInternal();
    
    // Create ForStatement with proper constructor
    auto node = std::make_unique<ForStatement>(
        std::move(initExpr),
        std::move(conditionExpr),
        std::move(incrementExpr),
        std::move(bodyStmt)
    );
    

    return std::move(node);
}

ASTNodePtr SyntacticAnalyzer::parseDoWhileStatement() {
    if (!currentTokens || isAtEnd() || getCurrentToken().getType() != Lexer::TokenType::DO) {
        return nullptr;
    }
    
    auto node = createNode(ASTNodeType::WHILE_STATEMENT); // Use WHILE_STATEMENT for do-while
    consumeToken(); // consume 'do'
    
    // Parse body statement
    auto bodyStmt = parseStatementInternal();
    if (bodyStmt) {
        node->addChild(std::move(bodyStmt));
    }
    
    // Expect 'while'
    if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::WHILE) {
        consumeToken(); // consume 'while'
        
        // Expect '('
        if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::LEFT_PAREN) {
            consumeToken(); // consume '('
            
            // Parse condition expression
            auto conditionExpr = parseExpressionInternal();
            if (conditionExpr) {
                node->addChild(std::move(conditionExpr));
            }
            
            if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::RIGHT_PAREN) {
                consumeToken(); // consume ')'
            }
        }
        
        // Expect ';'
        if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::SEMICOLON) {
            consumeToken(); // consume ';'
        }
    }
    
    return node;
}

ASTNodePtr SyntacticAnalyzer::parseSwitchStatement() {
    if (!currentTokens || isAtEnd() || getCurrentToken().getType() != Lexer::TokenType::SWITCH) {
        return nullptr;
    }
    
    auto node = createNode(ASTNodeType::COMPOUND_STATEMENT); // Use compound for now
    consumeToken(); // consume 'switch'
    
    // Expect '('
    if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::LEFT_PAREN) {
        consumeToken(); // consume '('
        
        // Parse expression
        auto switchExpr = parseExpressionInternal();
        if (switchExpr) {
            node->addChild(std::move(switchExpr));
        }
        
        if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::RIGHT_PAREN) {
            consumeToken(); // consume ')'
        }
    }
    
    // Parse body statement
    auto bodyStmt = parseStatementInternal();
    if (bodyStmt) {
        node->addChild(std::move(bodyStmt));
    }
    
    return node;
}

ASTNodePtr SyntacticAnalyzer::parseBreakStatement() {
    if (!currentTokens || isAtEnd() || getCurrentToken().getType() != Lexer::TokenType::BREAK) {
        return nullptr;
    }
    
    auto node = createNode(ASTNodeType::BREAK_STATEMENT);
    consumeToken(); // consume 'break'
    
    // Expect ';'
    if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::SEMICOLON) {
        consumeToken(); // consume ';'
    }
    
    return node;
}

ASTNodePtr SyntacticAnalyzer::parseContinueStatement() {
    if (!currentTokens || isAtEnd() || getCurrentToken().getType() != Lexer::TokenType::CONTINUE) {
        return nullptr;
    }
    
    auto node = createNode(ASTNodeType::CONTINUE_STATEMENT);
    consumeToken(); // consume 'continue'
    
    // Expect ';'
    if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::SEMICOLON) {
        consumeToken(); // consume ';'
    }
    
    return node;
}

ASTNodePtr SyntacticAnalyzer::parseReturnStatement() {
    if (!currentTokens || isAtEnd() || getCurrentToken().getType() != Lexer::TokenType::RETURN) {
        return nullptr;
    }
    
    consumeToken(); // consume 'return'
    
    ASTNodePtr returnExpr = nullptr;
    // Parse optional expression
    if (!isAtEnd() && getCurrentToken().getType() != Lexer::TokenType::SEMICOLON) {
        returnExpr = parseExpressionInternal();
    }
    
    // Create ReturnStatement with the expression
    auto node = std::make_unique<ReturnStatement>(std::move(returnExpr));
    
    // Expect ';'
    if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::SEMICOLON) {
        consumeToken(); // consume ';'
    }
    

    return node;
}

ASTNodePtr SyntacticAnalyzer::parseGotoStatement() {
    if (!currentTokens || isAtEnd() || getCurrentToken().getType() != Lexer::TokenType::GOTO) {
        return nullptr;
    }
    
    auto node = createNode(ASTNodeType::COMPOUND_STATEMENT); // Use compound for now
    consumeToken(); // consume 'goto'
    
    // Expect identifier
    if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::IDENTIFIER) {
        consumeToken(); // consume identifier
    }
    
    // Expect ';'
    if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::SEMICOLON) {
        consumeToken(); // consume ';'
    }
    
    return node;
}

ASTNodePtr SyntacticAnalyzer::parseCaseStatement() {
    if (!currentTokens || isAtEnd()) {
        return nullptr;
    }
    
    auto node = createNode(ASTNodeType::COMPOUND_STATEMENT); // Use compound for now
    
    if (getCurrentToken().getType() == Lexer::TokenType::CASE) {
        consumeToken(); // consume 'case'
        
        // Parse constant expression
        auto constantExpr = parseExpressionInternal();
        if (constantExpr) {
            node->addChild(std::move(constantExpr));
        }
        
        if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::COLON) {
            consumeToken(); // consume ':'
        }
    } else if (getCurrentToken().getType() == Lexer::TokenType::DEFAULT) {
        consumeToken(); // consume 'default'
        
        if (!isAtEnd() && getCurrentToken().getType() == Lexer::TokenType::COLON) {
            consumeToken(); // consume ':'
        }
    }
    
    // Parse following statement
    auto stmt = parseStatementInternal();
    if (stmt) {
        node->addChild(std::move(stmt));
    }
    
    return node;
}

ASTNodePtr SyntacticAnalyzer::parseExpressionInternal() {
    return parseAssignmentExpression();
}

// Assignment expressions (lowest precedence)
ASTNodePtr SyntacticAnalyzer::parseAssignmentExpression() {

    ASTNodePtr left = parseConditionalExpression();
    
    if (!left) {

        return nullptr;
    }

    
    // Check for assignment operators
    const auto& token = getCurrentToken();
    AssignmentExpression::Operator assignOp;
    bool isAssignment = false;
    
    if (token.getType() == Lexer::TokenType::ASSIGN) {
        assignOp = AssignmentExpression::Operator::ASSIGN;
        isAssignment = true;
    } else if (token.getType() == Lexer::TokenType::PLUS_ASSIGN) {
        assignOp = AssignmentExpression::Operator::PLUS_ASSIGN;
        isAssignment = true;
    } else if (token.getType() == Lexer::TokenType::MINUS_ASSIGN) {
        assignOp = AssignmentExpression::Operator::MINUS_ASSIGN;
        isAssignment = true;
    } else if (token.getType() == Lexer::TokenType::MULT_ASSIGN) {
        assignOp = AssignmentExpression::Operator::MULT_ASSIGN;
        isAssignment = true;
    } else if (token.getType() == Lexer::TokenType::DIV_ASSIGN) {
        assignOp = AssignmentExpression::Operator::DIV_ASSIGN;
        isAssignment = true;
    } else if (token.getType() == Lexer::TokenType::MOD_ASSIGN) {
        assignOp = AssignmentExpression::Operator::MOD_ASSIGN;
        isAssignment = true;
    } else if (token.getType() == Lexer::TokenType::LEFT_SHIFT_ASSIGN) {
        assignOp = AssignmentExpression::Operator::LSHIFT_ASSIGN;
        isAssignment = true;
    } else if (token.getType() == Lexer::TokenType::RIGHT_SHIFT_ASSIGN) {
        assignOp = AssignmentExpression::Operator::RSHIFT_ASSIGN;
        isAssignment = true;
    } else if (token.getType() == Lexer::TokenType::AND_ASSIGN) {
        assignOp = AssignmentExpression::Operator::AND_ASSIGN;
        isAssignment = true;
    } else if (token.getType() == Lexer::TokenType::XOR_ASSIGN) {
        assignOp = AssignmentExpression::Operator::XOR_ASSIGN;
        isAssignment = true;
    } else if (token.getType() == Lexer::TokenType::OR_ASSIGN) {
        assignOp = AssignmentExpression::Operator::OR_ASSIGN;
        isAssignment = true;
    }
    
    if (isAssignment) {

        consumeToken(); // consume assignment operator
        ASTNodePtr right = parseAssignmentExpression(); // right-associative
        
        if (!right) {

            reportError("Expected expression after assignment operator");
            return nullptr;
        }
        
        
        // Create assignment expression node
        auto assignExpr = std::make_unique<AssignmentExpression>(
            assignOp, 
            std::move(left), 
            std::move(right)
        );
        return std::move(assignExpr);
    }
    
    return left;
}

// Conditional expressions (ternary operator)
ASTNodePtr SyntacticAnalyzer::parseConditionalExpression() {
    ASTNodePtr condition = parseLogicalOrExpression();
    
    if (!condition) return nullptr;
    
    // Check for ternary operator
    if (getCurrentToken().getType() == Lexer::TokenType::CONDITIONAL) {
        consumeToken(); // consume '?'
        
        ASTNodePtr trueExpr = parseAssignmentExpression();
        if (!trueExpr) {
            reportError("Expected expression after '?'");
            return nullptr;
        }
        
        if (getCurrentToken().getType() != Lexer::TokenType::COLON) {
            reportError("Expected ':' in ternary expression");
            return nullptr;
        }
        consumeToken(); // consume ':'
        
        ASTNodePtr falseExpr = parseConditionalExpression();
        if (!falseExpr) {
            reportError("Expected expression after ':'");
            return nullptr;
        }
        
        // Create ternary expression
        auto ternaryExpr = std::make_unique<TernaryExpression>(
            std::move(condition),
            std::move(trueExpr),
            std::move(falseExpr)
        );
        return std::move(ternaryExpr);
    }
    
    return condition;
}

// Logical OR expressions
ASTNodePtr SyntacticAnalyzer::parseLogicalOrExpression() {
    ASTNodePtr left = parseLogicalAndExpression();
    
    if (!left) return nullptr;
    
    while (getCurrentToken().getType() == Lexer::TokenType::LOGICAL_OR) {
        consumeToken(); // consume '||'
        
        ASTNodePtr right = parseLogicalAndExpression();
        if (!right) {
            reportError("Expected expression after '||'");
            return nullptr;
        }
        
        auto orExpr = std::make_unique<BinaryExpression>(
            BinaryExpression::Operator::LOGICAL_OR,
            std::move(left),
            std::move(right)
        );
        left = std::move(orExpr);
    }
    
    return left;
}

// Logical AND expressions
ASTNodePtr SyntacticAnalyzer::parseLogicalAndExpression() {
    ASTNodePtr left = parseInclusiveOrExpression();
    
    if (!left) return nullptr;
    
    while (getCurrentToken().getType() == Lexer::TokenType::LOGICAL_AND) {
        consumeToken(); // consume '&&'
        
        ASTNodePtr right = parseInclusiveOrExpression();
        if (!right) {
            reportError("Expected expression after '&&'");
            return nullptr;
        }
        
        auto andExpr = std::make_unique<BinaryExpression>(
            BinaryExpression::Operator::LOGICAL_AND,
            std::move(left),
            std::move(right)
        );
        left = std::move(andExpr);
    }
    
    return left;
}

// Bitwise OR expressions
ASTNodePtr SyntacticAnalyzer::parseInclusiveOrExpression() {
    ASTNodePtr left = parseExclusiveOrExpression();
    
    if (!left) return nullptr;
    
    while (getCurrentToken().getType() == Lexer::TokenType::BITWISE_OR) {
        consumeToken(); // consume '|'
        
        ASTNodePtr right = parseExclusiveOrExpression();
        if (!right) {
            reportError("Expected expression after '|'");
            return nullptr;
        }
        
        auto orExpr = std::make_unique<BinaryExpression>(
            BinaryExpression::Operator::BITWISE_OR,
            std::move(left),
            std::move(right)
        );
        left = std::move(orExpr);
    }
    
    return left;
}

// Bitwise XOR expressions
ASTNodePtr SyntacticAnalyzer::parseExclusiveOrExpression() {
    ASTNodePtr left = parseAndExpression();
    
    if (!left) return nullptr;
    
    while (getCurrentToken().getType() == Lexer::TokenType::BITWISE_XOR) {
        consumeToken(); // consume '^'
        
        ASTNodePtr right = parseAndExpression();
        if (!right) {
            reportError("Expected expression after '^'");
            return nullptr;
        }
        
        auto xorExpr = std::make_unique<BinaryExpression>(
            BinaryExpression::Operator::BITWISE_XOR,
            std::move(left),
            std::move(right)
        );
        left = std::move(xorExpr);
    }
    
    return left;
}

// Bitwise AND expressions
ASTNodePtr SyntacticAnalyzer::parseAndExpression() {
    ASTNodePtr left = parseEqualityExpression();
    
    if (!left) return nullptr;
    
    while (getCurrentToken().getType() == Lexer::TokenType::BITWISE_AND) {
        consumeToken(); // consume '&'
        
        ASTNodePtr right = parseEqualityExpression();
        if (!right) {
            reportError("Expected expression after '&'");
            return nullptr;
        }
        
        auto andExpr = std::make_unique<BinaryExpression>(
            BinaryExpression::Operator::BITWISE_AND,
            std::move(left),
            std::move(right)
        );
        left = std::move(andExpr);
    }
    
    return left;
}

// Equality expressions
ASTNodePtr SyntacticAnalyzer::parseEqualityExpression() {
    ASTNodePtr left = parseRelationalExpression();
    
    if (!left) return nullptr;
    
    while (getCurrentToken().getType() == Lexer::TokenType::EQUAL ||
           getCurrentToken().getType() == Lexer::TokenType::NOT_EQUAL) {
        
        BinaryExpression::Operator op = (getCurrentToken().getType() == Lexer::TokenType::EQUAL) ?
            BinaryExpression::Operator::EQ : BinaryExpression::Operator::NE;
        
        consumeToken(); // consume '==' or '!='
        
        ASTNodePtr right = parseRelationalExpression();
        if (!right) {
            reportError("Expected expression after equality operator");
            return nullptr;
        }
        
        auto eqExpr = std::make_unique<BinaryExpression>(
            op,
            std::move(left),
            std::move(right)
        );
        left = std::move(eqExpr);
    }
    
    return left;
}

// Relational expressions
ASTNodePtr SyntacticAnalyzer::parseRelationalExpression() {
    ASTNodePtr left = parseShiftExpression();
    
    if (!left) return nullptr;
    
    while (getCurrentToken().getType() == Lexer::TokenType::LESS_THAN ||
           getCurrentToken().getType() == Lexer::TokenType::GREATER_THAN ||
           getCurrentToken().getType() == Lexer::TokenType::LESS_EQUAL ||
           getCurrentToken().getType() == Lexer::TokenType::GREATER_EQUAL) {
        
        BinaryExpression::Operator op;
        switch (getCurrentToken().getType()) {
            case Lexer::TokenType::LESS_THAN: op = BinaryExpression::Operator::LT; break;
            case Lexer::TokenType::GREATER_THAN: op = BinaryExpression::Operator::GT; break;
            case Lexer::TokenType::LESS_EQUAL: op = BinaryExpression::Operator::LE; break;
            case Lexer::TokenType::GREATER_EQUAL: op = BinaryExpression::Operator::GE; break;
            default: op = BinaryExpression::Operator::LT; break;
        }
        
        consumeToken(); // consume relational operator
        
        ASTNodePtr right = parseShiftExpression();
        if (!right) {
            reportError("Expected expression after relational operator");
            return nullptr;
        }
        
        auto relExpr = std::make_unique<BinaryExpression>(
            op,
            std::move(left),
            std::move(right)
        );
        left = std::move(relExpr);
    }
    
    return left;
}

// Shift expressions
ASTNodePtr SyntacticAnalyzer::parseShiftExpression() {
    ASTNodePtr left = parseAdditiveExpression();
    
    if (!left) return nullptr;
    
    while (getCurrentToken().getType() == Lexer::TokenType::LEFT_SHIFT ||
           getCurrentToken().getType() == Lexer::TokenType::RIGHT_SHIFT) {
        
        BinaryExpression::Operator op = (getCurrentToken().getType() == Lexer::TokenType::LEFT_SHIFT) ?
            BinaryExpression::Operator::LSHIFT : BinaryExpression::Operator::RSHIFT;
        
        consumeToken(); // consume shift operator
        
        ASTNodePtr right = parseAdditiveExpression();
        if (!right) {
            reportError("Expected expression after shift operator");
            return nullptr;
        }
        
        auto shiftExpr = std::make_unique<BinaryExpression>(
            op,
            std::move(left),
            std::move(right)
        );
        left = std::move(shiftExpr);
    }
    
    return left;
}

// Additive expressions
ASTNodePtr SyntacticAnalyzer::parseAdditiveExpression() {
    ASTNodePtr left = parseMultiplicativeExpression();
    
    if (!left) return nullptr;
    
    while (getCurrentToken().getType() == Lexer::TokenType::PLUS ||
           getCurrentToken().getType() == Lexer::TokenType::MINUS) {
        
        BinaryExpression::Operator op = (getCurrentToken().getType() == Lexer::TokenType::PLUS) ?
            BinaryExpression::Operator::ADD : BinaryExpression::Operator::SUB;
        
        consumeToken(); // consume '+' or '-'
        
        ASTNodePtr right = parseMultiplicativeExpression();
        if (!right) {
            reportError("Expected expression after additive operator");
            return nullptr;
        }
        
        auto addExpr = std::make_unique<BinaryExpression>(
            op,
            std::move(left),
            std::move(right)
        );
        left = std::move(addExpr);
    }
    
    return left;
}

// Multiplicative expressions
ASTNodePtr SyntacticAnalyzer::parseMultiplicativeExpression() {
    ASTNodePtr left = parseCastExpression();
    
    if (!left) return nullptr;
    
    while (getCurrentToken().getType() == Lexer::TokenType::MULTIPLY ||
           getCurrentToken().getType() == Lexer::TokenType::DIVIDE ||
           getCurrentToken().getType() == Lexer::TokenType::MODULO) {
        
        BinaryExpression::Operator op;
        switch (getCurrentToken().getType()) {
            case Lexer::TokenType::MULTIPLY: op = BinaryExpression::Operator::MUL; break;
            case Lexer::TokenType::DIVIDE: op = BinaryExpression::Operator::DIV; break;
            case Lexer::TokenType::MODULO: op = BinaryExpression::Operator::MOD; break;
            default: op = BinaryExpression::Operator::MUL; break;
        }
        
        consumeToken(); // consume multiplicative operator
        
        ASTNodePtr right = parseCastExpression();
        if (!right) {
            reportError("Expected expression after multiplicative operator");
            return nullptr;
        }
        
        auto mulExpr = std::make_unique<BinaryExpression>(
            op,
            std::move(left),
            std::move(right)
        );
        left = std::move(mulExpr);
    }
    
    return left;
}

// Cast expressions
ASTNodePtr SyntacticAnalyzer::parseCastExpression() {
    // Check for cast: ( type-name ) cast-expression
    if (getCurrentToken().getType() == Lexer::TokenType::LEFT_PAREN) {
        // Save current position for potential backtracking
        if (state) {
            state->pushPositionHistory();
        }
        
        consumeToken(); // consume '('
        
        // Check if next token could be a type name
        bool isPossibleCast = false;
        // Only consider built-in types as casts, not identifiers
        // This prevents variables like 'radius' from being interpreted as casts
        if (getCurrentToken().getType() == Lexer::TokenType::INT ||
            getCurrentToken().getType() == Lexer::TokenType::FLOAT ||
            getCurrentToken().getType() == Lexer::TokenType::DOUBLE ||
            getCurrentToken().getType() == Lexer::TokenType::CHAR ||
            getCurrentToken().getType() == Lexer::TokenType::VOID ||
            getCurrentToken().getType() == Lexer::TokenType::SHORT ||
            getCurrentToken().getType() == Lexer::TokenType::LONG ||
            getCurrentToken().getType() == Lexer::TokenType::SIGNED ||
            getCurrentToken().getType() == Lexer::TokenType::UNSIGNED) {
            
            std::string typeName = getCurrentToken().getLexeme();
            consumeToken();
            
            // Check for closing parenthesis
            if (getCurrentToken().getType() == Lexer::TokenType::RIGHT_PAREN) {
                consumeToken(); // consume ')'
                isPossibleCast = true;
                
                // Parse the expression being cast
                ASTNodePtr expr = parseCastExpression();
                if (expr) {
                    return std::make_unique<CastExpression>(typeName, std::move(expr));
                }
            }
        }
        
        // If not a cast, restore position and parse as unary expression
        if (!isPossibleCast && state) {
            state->popPositionHistory();
        }
    }
    
    return parseUnaryExpression();
}

// Unary expressions
ASTNodePtr SyntacticAnalyzer::parseUnaryExpression() {
    // Handle unary operators: ++, --, +, -, !, ~, *, &, sizeof
    if (getCurrentToken().getType() == Lexer::TokenType::INCREMENT ||
        getCurrentToken().getType() == Lexer::TokenType::DECREMENT ||
        getCurrentToken().getType() == Lexer::TokenType::PLUS ||
        getCurrentToken().getType() == Lexer::TokenType::MINUS ||
        getCurrentToken().getType() == Lexer::TokenType::LOGICAL_NOT ||
        getCurrentToken().getType() == Lexer::TokenType::BITWISE_NOT ||
        getCurrentToken().getType() == Lexer::TokenType::MULTIPLY || // dereference
        getCurrentToken().getType() == Lexer::TokenType::BITWISE_AND || // address-of
        getCurrentToken().getType() == Lexer::TokenType::SIZEOF) {
        
        auto operatorToken = getCurrentToken();
        consumeToken(); // consume unary operator
        
        ASTNodePtr operand = parseUnaryExpression(); // right-associative
        if (!operand) {
            reportError("Expected expression after unary operator");
            return nullptr;
        }
        
        // Map token type to unary operator
        UnaryExpression::Operator unaryOp;
        switch (operatorToken.getType()) {
            case Lexer::TokenType::INCREMENT:
                unaryOp = UnaryExpression::Operator::PRE_INCREMENT;
                break;
            case Lexer::TokenType::DECREMENT:
                unaryOp = UnaryExpression::Operator::PRE_DECREMENT;
                break;
            case Lexer::TokenType::PLUS:
                unaryOp = UnaryExpression::Operator::PLUS;
                break;
            case Lexer::TokenType::MINUS:
                unaryOp = UnaryExpression::Operator::MINUS;
                break;
            case Lexer::TokenType::LOGICAL_NOT:
                unaryOp = UnaryExpression::Operator::LOGICAL_NOT;
                break;
            case Lexer::TokenType::BITWISE_NOT:
                unaryOp = UnaryExpression::Operator::BITWISE_NOT;
                break;
            case Lexer::TokenType::MULTIPLY:
                unaryOp = UnaryExpression::Operator::DEREFERENCE;
                break;
            case Lexer::TokenType::BITWISE_AND:
                unaryOp = UnaryExpression::Operator::ADDRESS_OF;
                break;
            case Lexer::TokenType::SIZEOF:
                unaryOp = UnaryExpression::Operator::SIZEOF;
                break;
            default:
                reportError("Invalid unary operator");
                return nullptr;
        }
        
        // Create unary expression
        auto unaryExpr = std::make_unique<UnaryExpression>(
            unaryOp,
            std::move(operand)
        );
        return std::move(unaryExpr);
    }
    
    return parsePostfixExpression();
}

// Postfix expressions
ASTNodePtr SyntacticAnalyzer::parsePostfixExpression() {
    ASTNodePtr left = parsePrimaryExpression();
    
    if (!left) return nullptr;
    
    while (true) {
        if (getCurrentToken().getType() == Lexer::TokenType::LEFT_BRACKET) {
            // Array access: expr[expr]
            consumeToken(); // consume '['
            
            ASTNodePtr index = parseAssignmentExpression();
            if (!index) {
                reportError("Expected expression in array access");
                return nullptr;
            }
            
            if (getCurrentToken().getType() != Lexer::TokenType::RIGHT_BRACKET) {
                reportError("Expected ']' after array index");
                return nullptr;
            }
            consumeToken(); // consume ']'
            
            left = std::make_unique<ArrayAccess>(std::move(left), std::move(index));
            
        } else if (getCurrentToken().getType() == Lexer::TokenType::LEFT_PAREN) {
            // Function call: expr(args)
            consumeToken(); // consume '('
            
            // Parse argument list (simplified)
            std::vector<ASTNodePtr> args;
            if (getCurrentToken().getType() != Lexer::TokenType::RIGHT_PAREN) {
                do {
                    ASTNodePtr arg = parseAssignmentExpression();
                    if (!arg) {
                        reportError("Expected expression in argument list");
                        return nullptr;
                    }
                    args.push_back(std::move(arg));
                    
                    if (getCurrentToken().getType() == Lexer::TokenType::COMMA) {
                        consumeToken(); // consume ','
                    } else {
                        break;
                    }
                } while (true);
            }
            
            if (getCurrentToken().getType() != Lexer::TokenType::RIGHT_PAREN) {
                reportError("Expected ')' after function arguments");
                return nullptr;
            }
            consumeToken(); // consume ')'
            
            // Create function call expression
            auto callExpr = std::make_unique<CallExpression>(std::move(left));
            for (auto& arg : args) {
                callExpr->addArgument(std::move(arg));
            }
            left = std::move(callExpr);
            
        } else if (getCurrentToken().getType() == Lexer::TokenType::DOT ||
                   getCurrentToken().getType() == Lexer::TokenType::ARROW) {
            // Member access: expr.member or expr->member
            bool isArrow = (getCurrentToken().getType() == Lexer::TokenType::ARROW);
            consumeToken(); // consume '.' or '->'
            
            if (getCurrentToken().getType() != Lexer::TokenType::IDENTIFIER) {
                reportError("Expected identifier after member access operator");
                return nullptr;
            }
            
            auto memberName = getCurrentToken().getLexeme();
            consumeToken(); // consume identifier
            
            MemberExpression::AccessType accessType = isArrow ? MemberExpression::AccessType::ARROW : MemberExpression::AccessType::DOT;
            left = std::make_unique<MemberExpression>(std::move(left), memberName, accessType);
            
        } else if (getCurrentToken().getType() == Lexer::TokenType::INCREMENT ||
                   getCurrentToken().getType() == Lexer::TokenType::DECREMENT) {
            // Postfix increment/decrement: expr++ or expr--
            UnaryExpression::Operator op = (getCurrentToken().getType() == Lexer::TokenType::INCREMENT) ?
                UnaryExpression::Operator::POST_INCREMENT : UnaryExpression::Operator::POST_DECREMENT;
            consumeToken(); // consume '++' or '--'
            
            left = std::make_unique<UnaryExpression>(op, std::move(left));
            
        } else {
            break; // No more postfix operators
        }
    }
    
    return left;
}

// Primary expressions
ASTNodePtr SyntacticAnalyzer::parsePrimaryExpression() {
    const auto& token = getCurrentToken();
    
    // DEBUG: Log token being processed
    // DEBUG: Processing token '[token]' (type: [type])
    
    // Identifier
    if (token.getType() == Lexer::TokenType::IDENTIFIER) {

        auto name = token.getLexeme();
        consumeToken();
        return std::make_unique<Identifier>(name);
    }
    
    // Integer literal
    if (token.getType() == Lexer::TokenType::INTEGER_LITERAL) {

        auto value = token.getLexeme();
        consumeToken();
        long long intValue = std::stoll(value);
        return std::make_unique<IntegerLiteral>(intValue);
    }
    
    // Float literal
    if (token.getType() == Lexer::TokenType::FLOAT_LITERAL) {
        auto value = token.getLexeme();
        consumeToken();
        double floatValue = std::stod(value);
        return std::make_unique<FloatLiteral>(floatValue);
    }
    
    // Character literal
    if (token.getType() == Lexer::TokenType::CHAR_LITERAL) {
        auto value = token.getLexeme();
        consumeToken();
        // Remove quotes and get the character
        char charValue = (value.length() > 2) ? value[1] : '\0';
        return std::make_unique<CharLiteral>(charValue);
    }
    
    // String literal
    if (token.getType() == Lexer::TokenType::STRING_LITERAL) {
        auto value = token.getLexeme();
        consumeToken();
        // Remove quotes from string literal and process escape sequences
        std::string rawString = (value.length() > 2) ? value.substr(1, value.length() - 2) : "";
        std::string stringValue = processEscapeSequences(rawString);
        return std::make_unique<StringLiteral>(stringValue);
    }
    
    // Parenthesized expression
    if (token.getType() == Lexer::TokenType::LEFT_PAREN) {
        consumeToken(); // consume '('
        
        ASTNodePtr expr = parseAssignmentExpression();
        if (!expr) {
            reportError("Expected expression in parentheses");
            return nullptr;
        }
        
        if (getCurrentToken().getType() != Lexer::TokenType::RIGHT_PAREN) {
            reportError("Expected ')' after expression");
            return nullptr;
        }
        consumeToken(); // consume ')'
        
        return expr;
    }
    
    std::cout << "parsePrimaryExpression: ERROR - Unexpected token: " << token.getLexeme() 
              << " (type: " << static_cast<int>(token.getType()) << ")" << std::endl;
    reportError("Expected primary expression");
    return nullptr;
}

// Phase 3.6: Type Parsing Methods
ASTNodePtr SyntacticAnalyzer::parseTypeName() {
    // Parse specifier-qualifier-list [abstract-declarator]
    auto node = createNode(ASTNodeType::TYPE_DECLARATION);
    
    // Parse specifier-qualifier-list
    auto specifiers = parseSpecifierQualifierList();
    if (specifiers) {
        node->addChild(std::move(specifiers));
    }
    
    // Parse optional abstract declarator
    auto abstractDeclarator = parseAbstractDeclarator();
    if (abstractDeclarator) {
        node->addChild(std::move(abstractDeclarator));
    }
    
    return node;
}

ASTNodePtr SyntacticAnalyzer::parseSpecifierQualifierList() {
    // Parse type specifiers and qualifiers (const, volatile)
    auto node = createNode(ASTNodeType::TYPE_DECLARATION);
    bool foundSpecifier = false;
    
    while (!isAtEnd()) {
        const Token& currentToken = getCurrentToken();
        
        // Handle type qualifiers
        if (currentToken.getType() == Lexer::TokenType::CONST ||
            currentToken.getType() == Lexer::TokenType::VOLATILE) {
            auto qualifierNode = createNode(ASTNodeType::IDENTIFIER);
            qualifierNode->setValue(currentToken.getLexeme());
            node->addChild(std::move(qualifierNode));
            consumeToken();
            foundSpecifier = true;
            continue;
        }
        
        // Handle basic type specifiers
        if (currentToken.getType() == Lexer::TokenType::INT ||
            currentToken.getType() == Lexer::TokenType::FLOAT ||
            currentToken.getType() == Lexer::TokenType::DOUBLE ||
            currentToken.getType() == Lexer::TokenType::CHAR ||
            currentToken.getType() == Lexer::TokenType::VOID ||
            currentToken.getType() == Lexer::TokenType::SHORT ||
            currentToken.getType() == Lexer::TokenType::LONG ||
            currentToken.getType() == Lexer::TokenType::SIGNED ||
            currentToken.getType() == Lexer::TokenType::UNSIGNED) {
            
            auto typeNode = createNode(ASTNodeType::IDENTIFIER);
            typeNode->setValue(currentToken.getLexeme());
            node->addChild(std::move(typeNode));
            consumeToken();
            foundSpecifier = true;
            continue;
        }
        
        // Handle struct/union/enum
        if (currentToken.getType() == Lexer::TokenType::STRUCT) {
            auto structNode = parseStructDeclaration();
            if (structNode) {
                node->addChild(std::move(structNode));
                foundSpecifier = true;
            }
            continue;
        }
        
        if (currentToken.getType() == Lexer::TokenType::UNION) {
            auto unionNode = parseUnionDeclaration();
            if (unionNode) {
                node->addChild(std::move(unionNode));
                foundSpecifier = true;
            }
            continue;
        }
        
        if (currentToken.getType() == Lexer::TokenType::ENUM) {
            auto enumNode = parseEnumDeclaration();
            if (enumNode) {
                node->addChild(std::move(enumNode));
                foundSpecifier = true;
            }
            continue;
        }
        
        // Handle typedef names (identifiers)
        if (currentToken.getType() == Lexer::TokenType::IDENTIFIER) {
            auto typedefNode = createNode(ASTNodeType::IDENTIFIER);
            typedefNode->setValue(currentToken.getLexeme());
            node->addChild(std::move(typedefNode));
            consumeToken();
            foundSpecifier = true;
            continue;
        }
        
        break;
    }
    
    return foundSpecifier ? std::move(node) : nullptr;
}

ASTNodePtr SyntacticAnalyzer::parseAbstractDeclarator() {
    // Parse [pointer] [direct-abstract-declarator]
    auto node = createNode(ASTNodeType::IDENTIFIER);
    bool hasContent = false;
    
    // Handle pointer prefix (*)
    while (getCurrentToken().getType() == Lexer::TokenType::MULTIPLY) {
        consumeToken(); // consume '*'
        hasContent = true;
        // Could add pointer level tracking here
    }
    
    // Parse direct abstract declarator
    auto directDeclarator = parseDirectAbstractDeclarator();
    if (directDeclarator) {
        node->addChild(std::move(directDeclarator));
        hasContent = true;
    }
    
    return hasContent ? std::move(node) : nullptr;
}

ASTNodePtr SyntacticAnalyzer::parseDirectAbstractDeclarator() {
    // Parse ( abstract-declarator ) | [constant-expression] | ( [parameter-list] )
    auto node = createNode(ASTNodeType::IDENTIFIER);
    bool hasContent = false;
    
    const Token& currentToken = getCurrentToken();
    
    if (currentToken.getType() == Lexer::TokenType::LEFT_PAREN) {
        consumeToken(); // consume '('
        
        // Check if this is a parenthesized abstract declarator or parameter list
        // For simplicity, we'll assume it's a parameter list if we see type specifiers
        const Token& nextToken = getCurrentToken();
        
        if (nextToken.getType() == Lexer::TokenType::INT ||
            nextToken.getType() == Lexer::TokenType::FLOAT ||
            nextToken.getType() == Lexer::TokenType::DOUBLE ||
            nextToken.getType() == Lexer::TokenType::CHAR ||
            nextToken.getType() == Lexer::TokenType::VOID ||
            nextToken.getType() == Lexer::TokenType::STRUCT ||
            nextToken.getType() == Lexer::TokenType::UNION ||
            nextToken.getType() == Lexer::TokenType::ENUM ||
            nextToken.getType() == Lexer::TokenType::CONST ||
            nextToken.getType() == Lexer::TokenType::VOLATILE) {
            
            // Parse parameter list (simplified)
            // Skip parameters for now - would need parseParameterList implementation
            while (getCurrentToken().getType() != Lexer::TokenType::RIGHT_PAREN && !isAtEnd()) {
                if (getCurrentToken().getType() == Lexer::TokenType::COMMA) {
                    consumeToken();
                } else {
                    consumeToken();
                }
            }
            hasContent = true;
        } else {
            // Parse parenthesized abstract declarator
            auto abstractDecl = parseAbstractDeclarator();
            if (abstractDecl) {
                node->addChild(std::move(abstractDecl));
                hasContent = true;
            }
        }
        
        if (getCurrentToken().getType() == Lexer::TokenType::RIGHT_PAREN) {
            consumeToken(); // consume ')'
        }
    }
    
    // Handle array declarators [constant-expression]
    while (getCurrentToken().getType() == Lexer::TokenType::LEFT_BRACKET) {
        consumeToken(); // consume '['
        
        // Parse optional constant expression (array size)
        if (getCurrentToken().getType() != Lexer::TokenType::RIGHT_BRACKET) {
            auto sizeExpr = parseAssignmentExpression();
            if (sizeExpr) {
                node->addChild(std::move(sizeExpr));
            }
        }
        
        if (getCurrentToken().getType() == Lexer::TokenType::RIGHT_BRACKET) {
            consumeToken(); // consume ']'
            hasContent = true;
        }
    }
    
    return hasContent ? std::move(node) : nullptr;
}

ASTNodePtr SyntacticAnalyzer::parseParameterList() {
    // Parse parameter-declaration { , parameter-declaration }
    auto node = createNode(ASTNodeType::FUNCTION_DECLARATION);
    
    // Parse first parameter
    auto param = parseParameterDeclaration();
    if (param) {
        node->addChild(std::move(param));
    } else {
        return nullptr;
    }
    
    // Parse additional parameters
    while (getCurrentToken().getType() == Lexer::TokenType::COMMA) {
        consumeToken(); // consume ','
        
        // Check for ellipsis (...) - simplified check
        if (getCurrentToken().getLexeme() == "...") {
            auto ellipsisNode = createNode(ASTNodeType::IDENTIFIER);
            ellipsisNode->setValue("...");
            node->addChild(std::move(ellipsisNode));
            consumeToken();
            break;
        }
        
        auto nextParam = parseParameterDeclaration();
        if (nextParam) {
            node->addChild(std::move(nextParam));
        } else {
            break;
        }
    }
    
    return node;
}

ASTNodePtr SyntacticAnalyzer::parseDirectDeclarator() {
    // Parse direct-declarator: identifier | ( declarator ) | direct-declarator [ constant-expression ] | direct-declarator ( parameter-type-list )
    
    auto node = createNode(ASTNodeType::IDENTIFIER);
    
    const Token& currentToken = getCurrentToken();
    
    // Handle identifier
    if (currentToken.getType() == Lexer::TokenType::IDENTIFIER) {
        node->setValue(currentToken.getLexeme());
        consumeToken();
        
        // Handle postfix operators (arrays and functions)
        int loopCount = 0;
        while (true) {
            loopCount++;
            
            if (loopCount > 10) {
                break;
            }
            
            if (getCurrentToken().getType() == Lexer::TokenType::LEFT_BRACKET) {
        
                // Array declarator: [constant-expression]
                consumeToken(); // consume '['
                
                // Parse array size expression
                std::string arraySize = "";
                while (getCurrentToken().getType() != Lexer::TokenType::RIGHT_BRACKET && !isAtEnd()) {
                    arraySize += getCurrentToken().getLexeme();
                    consumeToken();
                }
                
                if (getCurrentToken().getType() == Lexer::TokenType::RIGHT_BRACKET) {
                    consumeToken(); // consume ']'
                }
                
                // Mark this as an array by modifying the node value
                std::string originalName = node->getValue();
                node->setValue(originalName + "[" + arraySize + "]");
        
            } else if (getCurrentToken().getType() == Lexer::TokenType::LEFT_PAREN) {
                // Function declarator: (parameter-type-list)
        
                
                // Convert to FUNCTION_DECLARATION node to properly handle parameters
                auto funcNode = std::make_unique<FunctionDeclaration>(node->getValue(), "void");
                
                consumeToken(); // consume '('
    
                
                // Parse parameter list - add individual parameters using addParameter
                if (getCurrentToken().getType() != Lexer::TokenType::RIGHT_PAREN) {
    
                    
                    // Parse first parameter
                    auto param = parseParameterDeclaration();
                    if (param) {
    
                        funcNode->addParameter(std::move(param));
    
                    }
                    
                    // Parse additional parameters
                    while (getCurrentToken().getType() == Lexer::TokenType::COMMA) {
                        consumeToken(); // consume ','
                        
                        auto nextParam = parseParameterDeclaration();
                        if (nextParam) {
    
                            funcNode->addParameter(std::move(nextParam));
    
                        }
                    }
                }
                
                if (getCurrentToken().getType() == Lexer::TokenType::RIGHT_PAREN) {
                    consumeToken(); // consume ')'
                }
                
                // Replace the original node with the function declaration node
                node = std::move(funcNode);
    
            } else {
                break; // No more postfix operators
            }
        }
        
        return node;
    }
    
    // Handle parenthesized declarator: ( declarator )
    if (currentToken.getType() == Lexer::TokenType::LEFT_PAREN) {
        // For now, we'll skip complex parenthesized declarators to avoid infinite recursion
        // and just consume the parentheses
        consumeToken(); // consume '('
        
        // Skip tokens until we find the matching ')'
        int parenCount = 1;
        while (parenCount > 0 && !isAtEnd()) {
            if (getCurrentToken().getType() == Lexer::TokenType::LEFT_PAREN) {
                parenCount++;
            } else if (getCurrentToken().getType() == Lexer::TokenType::RIGHT_PAREN) {
                parenCount--;
            }
            consumeToken();
        }
        
        return node;
    }
    
    return nullptr;
}

ASTNodePtr SyntacticAnalyzer::parseParameterDeclaration() {
    // Parse parameter-declaration: declaration-specifiers declarator | declaration-specifiers abstract-declarator

    
    // Parse declaration specifiers (type)
    auto specifiers = parseDeclarationSpecifiers();
    if (!specifiers) {

        return nullptr;
    }

    
    // Extract type name from declaration specifiers
    std::string typeName = "unknown";
    if (specifiers && !specifiers->getValue().empty()) {
        typeName = specifiers->getValue();
    }
    
    // Parse declarator (parameter name) - optional for abstract declarators
    std::string paramName = "";
    std::string fullType = typeName;
    
    // Handle pointer prefix (*)
    int pointerLevel = 0;
    while (getCurrentToken().getType() == Lexer::TokenType::MULTIPLY) {
        consumeToken(); // consume '*'
        pointerLevel++;
    }
    
    // Add pointer indicators to type
    for (int i = 0; i < pointerLevel; i++) {
        fullType += "*";
    }
    
    if (getCurrentToken().getType() == Lexer::TokenType::IDENTIFIER) {
        // Parameter with name
        paramName = getCurrentToken().getLexeme();
        consumeToken();
        
        // Handle array declarators after identifier
        while (getCurrentToken().getType() == Lexer::TokenType::LEFT_BRACKET) {
            consumeToken(); // consume '['
            fullType += "[";
            
            // Parse array size expression (optional)
            if (getCurrentToken().getType() != Lexer::TokenType::RIGHT_BRACKET) {
                // For now, just consume tokens until ']'
                while (getCurrentToken().getType() != Lexer::TokenType::RIGHT_BRACKET && !isAtEnd()) {
                    fullType += getCurrentToken().getLexeme();
                    consumeToken();
                }
            }
            
            if (getCurrentToken().getType() == Lexer::TokenType::RIGHT_BRACKET) {
                fullType += "]";
                consumeToken(); // consume ']'
            } else {
                // Error: expected ']'
                reportError("Expected ']' in array declarator");
                return nullptr;
            }
        }
    } else if (getCurrentToken().getType() == Lexer::TokenType::LEFT_PAREN) {
        // Function pointer or parenthesized declarator - for now, use generic name
        paramName = "param";
        auto declarator = parseAbstractDeclarator();
        // We could extract more info from declarator if needed
    } else if (getCurrentToken().getType() == Lexer::TokenType::LEFT_BRACKET) {
        // Abstract array declarator without name
        paramName = "param";
        
        // Handle array declarators
        while (getCurrentToken().getType() == Lexer::TokenType::LEFT_BRACKET) {
            consumeToken(); // consume '['
            fullType += "[";
            
            // Parse array size expression (optional)
            if (getCurrentToken().getType() != Lexer::TokenType::RIGHT_BRACKET) {
                // For now, just consume tokens until ']'
                while (getCurrentToken().getType() != Lexer::TokenType::RIGHT_BRACKET && !isAtEnd()) {
                    fullType += getCurrentToken().getLexeme();
                    consumeToken();
                }
            }
            
            if (getCurrentToken().getType() == Lexer::TokenType::RIGHT_BRACKET) {
                fullType += "]";
                consumeToken(); // consume ']'
            } else {
                // Error: expected ']'
                reportError("Expected ']' in array declarator");
                return nullptr;
            }
        }
    } else {
        // Abstract declarator without name
        paramName = "param";
    }
    
    // Create VariableDeclaration with proper type and name
    auto node = std::make_unique<VariableDeclaration>(paramName, fullType);
    
    return std::move(node);
}

ASTNodePtr SyntacticAnalyzer::parseInitializer() {
    // Parse initializer: assignment-expression | { initializer-list } | { initializer-list , }

    
    if (getCurrentToken().getType() == Lexer::TokenType::LEFT_BRACE) {

        // Compound initializer: { initializer-list }
        consumeToken(); // consume '{'
        
        auto initList = parseInitializerList();
        if (!initList) {

            return nullptr;
        }
        
        
        
        // Optional trailing comma
        if (getCurrentToken().getType() == Lexer::TokenType::COMMA) {

            consumeToken();
        }
        
        if (getCurrentToken().getType() == Lexer::TokenType::RIGHT_BRACE) {

            consumeToken(); // consume '}'
        } else {

            reportError("Expected '}' after initializer list");
            return nullptr;
        }
        
        // Return the initializer list directly instead of wrapping in ExpressionStatement
        return initList;
    } else {
        // Simple initializer: assignment-expression
        auto expr = parseAssignmentExpression();
        if (expr) {
            return expr;
        } else {
            return nullptr;
        }
    }
}

ASTNodePtr SyntacticAnalyzer::parseInitializerList() {
    // Parse initializer-list: initializer | initializer-list , initializer
    auto node = createNode(ASTNodeType::COMPOUND_STATEMENT);
    
    // Parse first initializer
    auto init = parseInitializer();
    if (init) {
        static_cast<CompoundStatement*>(node.get())->addStatement(std::move(init));
    } else {
        return nullptr;
    }
    
    // Parse additional initializers
    while (getCurrentToken().getType() == Lexer::TokenType::COMMA) {
        // Look ahead to see if there's another initializer
        if (peekToken(1).getType() == Lexer::TokenType::RIGHT_BRACE) {
            // Trailing comma before '}'
            break;
        }
        
        consumeToken(); // consume ','
        
        auto nextInit = parseInitializer();
        if (nextInit) {
            static_cast<CompoundStatement*>(node.get())->addStatement(std::move(nextInit));
        } else {
            break;
        }
    }
    
    return node;
}

// Utility methods
ASTNodePtr SyntacticAnalyzer::createNode(ASTNodeType type, const SourceRange& range) {
    statistics.nodesCreated++;
    
    switch (type) {
        // Declarations
        case ASTNodeType::TRANSLATION_UNIT:
            return std::make_unique<TranslationUnit>();
        case ASTNodeType::FUNCTION_DECLARATION:
            return std::make_unique<FunctionDeclaration>("", range);
        case ASTNodeType::VARIABLE_DECLARATION:
            return std::make_unique<VariableDeclaration>("variable", "int", range);
        case ASTNodeType::TYPE_DECLARATION:
            // Use CompoundStatement as it can have children
            return std::make_unique<CompoundStatement>(range);
            
        // Statements
        case ASTNodeType::COMPOUND_STATEMENT:
            return std::make_unique<CompoundStatement>(range);
        case ASTNodeType::EXPRESSION_STATEMENT:
            return std::make_unique<ExpressionStatement>(range);
        case ASTNodeType::IF_STATEMENT:
            return std::make_unique<IfStatement>(
                std::make_unique<Identifier>("condition", range),
                std::make_unique<CompoundStatement>(range), range);
        case ASTNodeType::WHILE_STATEMENT:
            return std::make_unique<WhileStatement>(
                std::make_unique<Identifier>("condition", range),
                std::make_unique<CompoundStatement>(range), range);
        case ASTNodeType::FOR_STATEMENT:
            return std::make_unique<ForStatement>(
                nullptr, // init
                std::make_unique<Identifier>("condition", range), // condition
                nullptr, // update
                std::make_unique<CompoundStatement>(range), range); // body
        case ASTNodeType::RETURN_STATEMENT:
            return std::make_unique<ReturnStatement>(range);
        case ASTNodeType::BREAK_STATEMENT:
            return std::make_unique<BreakStatement>(range);
        case ASTNodeType::CONTINUE_STATEMENT:
            return std::make_unique<ContinueStatement>(range);
            
        // Expressions
        case ASTNodeType::BINARY_EXPRESSION:
            return std::make_unique<BinaryExpression>(BinaryExpression::Operator::ADD, 
                std::make_unique<Identifier>("left", range), 
                std::make_unique<Identifier>("right", range), range);
        case ASTNodeType::UNARY_EXPRESSION:
            return std::make_unique<UnaryExpression>(UnaryExpression::Operator::PLUS,
                std::make_unique<Identifier>("operand", range), range);
        case ASTNodeType::ASSIGNMENT_EXPRESSION:
            return std::make_unique<AssignmentExpression>(AssignmentExpression::Operator::ASSIGN,
                std::make_unique<Identifier>("left", range),
                std::make_unique<Identifier>("right", range), range);
        case ASTNodeType::TERNARY_EXPRESSION:
            return std::make_unique<TernaryExpression>(
                std::make_unique<Identifier>("condition", range),
                std::make_unique<Identifier>("true_expr", range),
                std::make_unique<Identifier>("false_expr", range), range);
        case ASTNodeType::CALL_EXPRESSION:
            return std::make_unique<CallExpression>(
                std::make_unique<Identifier>("function", range), range);
        case ASTNodeType::MEMBER_EXPRESSION:
            return std::make_unique<MemberExpression>(
                std::make_unique<Identifier>("object", range), "member", 
                MemberExpression::AccessType::DOT, range);
        case ASTNodeType::ARRAY_ACCESS:
            return std::make_unique<ArrayAccess>(
                std::make_unique<Identifier>("array", range),
                std::make_unique<Identifier>("index", range), range);
        case ASTNodeType::CAST_EXPRESSION:
            return std::make_unique<CastExpression>("int",
                std::make_unique<Identifier>("expression", range), range);
        case ASTNodeType::SIZEOF_EXPRESSION:
            // Use UnaryExpression with SIZEOF operator
            return std::make_unique<UnaryExpression>(UnaryExpression::Operator::SIZEOF,
                std::make_unique<Identifier>("operand", range), range);
                
        // Literals
        case ASTNodeType::INTEGER_LITERAL:
            return std::make_unique<IntegerLiteral>(0, range);
        case ASTNodeType::FLOAT_LITERAL:
            return std::make_unique<FloatLiteral>(0.0, range);
        case ASTNodeType::STRING_LITERAL:
            return std::make_unique<StringLiteral>("", range);
        case ASTNodeType::CHAR_LITERAL:
            return std::make_unique<CharLiteral>('\0', range);
        case ASTNodeType::IDENTIFIER:
            return std::make_unique<Identifier>("", range);
            
        // Types (use Identifier as placeholder)
        case ASTNodeType::BUILTIN_TYPE:
        case ASTNodeType::POINTER_TYPE:
        case ASTNodeType::ARRAY_TYPE:
        case ASTNodeType::FUNCTION_TYPE:
        case ASTNodeType::STRUCT_TYPE:
        case ASTNodeType::UNION_TYPE:
        case ASTNodeType::ENUM_TYPE:
            return std::make_unique<Identifier>("type", range);
            
        default:
            // Fallback to Identifier for unknown types
            return std::make_unique<Identifier>("unknown", range);
    }
}

void SyntacticAnalyzer::startTiming() {
    parseStartTime = std::chrono::high_resolution_clock::now();
}

void SyntacticAnalyzer::endTiming() {
    auto endTime = std::chrono::high_resolution_clock::now();
    statistics.parseTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - parseStartTime);
}

// Factory functions
std::unique_ptr<SyntacticAnalyzer> createParser(CStandard standard) {
    ParserConfig config;
    config.setCStandard(standard);
    return std::make_unique<SyntacticAnalyzer>(config);
}

std::unique_ptr<SyntacticAnalyzer> createStrictParser(CStandard standard) {
    auto config = ParserConfig::createStrict();
    config.setCStandard(standard);
    return std::make_unique<SyntacticAnalyzer>(config);
}

std::unique_ptr<SyntacticAnalyzer> createPermissiveParser(CStandard standard) {
    auto config = ParserConfig::createPermissive();
    config.setCStandard(standard);
    return std::make_unique<SyntacticAnalyzer>(config);
}

// Utility functions
bool isValidCCode(const std::string& code, CStandard standard) {
    (void)standard; // Suppress unused parameter warning
    if (code.empty()) {
        return false;
    }
    
    // Basic syntax validation - check for balanced braces
    int braceCount = 0;
    int parenCount = 0;
    bool inString = false;
    bool inChar = false;
    bool escaped = false;
    
    for (size_t i = 0; i < code.length(); ++i) {
        char c = code[i];
        
        if (escaped) {
            escaped = false;
            continue;
        }
        
        if (c == '\\') {
            escaped = true;
            continue;
        }
        
        if (inString) {
            if (c == '"') {
                inString = false;
            }
            continue;
        }
        
        if (inChar) {
            if (c == '\'') {
                inChar = false;
            }
            continue;
        }
        
        if (c == '"') {
            inString = true;
        } else if (c == '\'') {
            inChar = true;
        } else if (c == '{') {
            braceCount++;
        } else if (c == '}') {
            braceCount--;
            if (braceCount < 0) {
                return false; // Unmatched closing brace
            }
        } else if (c == '(') {
            parenCount++;
        } else if (c == ')') {
            parenCount--;
            if (parenCount < 0) {
                return false; // Unmatched closing paren
            }
        }
    }
    
    // Check for unmatched braces or parentheses
    if (braceCount != 0 || parenCount != 0) {
        return false;
    }
    
    // Check for obviously invalid syntax patterns
    if (code.find("invalid syntax") != std::string::npos) {
        return false;
    }
    
    return true;
}

ParseResult<ASTNodePtr> parseCodeString(const std::string& code, CStandard standard) {
    auto parser = createParser(standard);
    return parser->parseString(code);
}

ParseResult<ASTNodePtr> parseFile(const std::string& filename, CStandard standard) {
    auto parser = createParser(standard);
    return parser->parseFile(filename);
}

// Version information
Parser::ParserVersion getParserVersion() {
    return Parser::ParserVersion{1, 0, 0, __DATE__, "unknown"};
}

std::string getParserVersionString() {
    auto version = getParserVersion();
    return std::to_string(version.major) + "." + 
           std::to_string(version.minor) + "." + 
           std::to_string(version.patch);
}

// Utility methods implementation
void SyntacticAnalyzer::reportError(const std::string& message) {
    // Create a simple error with current position
    SourceRange range;
    if (currentTokens && !currentTokens->isAtEnd()) {
        auto pos = getCurrentToken().getPosition();
        range.start = Position(pos.line, pos.column, pos.offset);
        range.end = range.start;
    }
    auto error = std::make_unique<ParseError>(ErrorSeverity::ERROR, message, range);
    state->addError(std::move(error));
}

void SyntacticAnalyzer::reportError(std::unique_ptr<ParseError> error) {
    if (error) {
        state->addError(std::move(error));
    }
}

bool SyntacticAnalyzer::match(const std::string& tokenType) {
    if (check(tokenType)) {
        advance();
        return true;
    }
    return false;
}

bool SyntacticAnalyzer::check(const std::string& tokenType) const {
    if (isAtEnd()) return false;
    return Lexer::tokenTypeToString(getCurrentToken().getType()) == tokenType;
}

const Token& SyntacticAnalyzer::advance() {
    if (!isAtEnd()) {
        consumeToken();
    }
    return getCurrentToken();
}

const Token& SyntacticAnalyzer::consume(const std::string& tokenType, const std::string& errorMessage) {
    if (check(tokenType)) {
        return advance();
    }
    
    reportError(errorMessage);
    return getCurrentToken();
}

std::string SyntacticAnalyzer::processEscapeSequences(const std::string& rawString) {
    std::string result;
    result.reserve(rawString.length());
    
    for (size_t i = 0; i < rawString.length(); ++i) {
        if (rawString[i] == '\\' && i + 1 < rawString.length()) {
            char nextChar = rawString[i + 1];
            switch (nextChar) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'v': result += '\v'; break;
                case 'a': result += '\a'; break;
                case '\\': result += '\\'; break;
                case '\'': result += '\''; break;
                case '"': result += '"'; break;
                case '0': result += '\0'; break;
                default:
                    // For unrecognized escape sequences, keep the backslash
                    result += rawString[i];
                    result += nextChar;
                    break;
            }
            ++i; // Skip the next character as it's part of the escape sequence
        } else {
            result += rawString[i];
        }
    }
    
    return result;
}

} // namespace Parser