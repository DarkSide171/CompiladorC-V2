# Arquitetura Técnica - Compilador C

Este documento descreve a arquitetura técnica detalhada do compilador C, incluindo design de classes, interfaces, padrões utilizados e estrutura modular.

## Visão Geral da Arquitetura

### Padrão Arquitetural: Pipeline

O compilador segue o padrão de pipeline, onde cada fase processa a saída da fase anterior:

```
Código Fonte → [Lexer] → Tokens → [Parser] → AST → [Semantic] → AST Decorada → [CodeGen] → Assembly
```

### Princípios de Design

1. **Separação de Responsabilidades**: Cada fase tem uma responsabilidade específica
2. **Baixo Acoplamento**: Fases comunicam através de interfaces bem definidas
3. **Alta Coesão**: Componentes relacionados agrupados logicamente
4. **Extensibilidade**: Fácil adição de novas funcionalidades
5. **Testabilidade**: Cada componente pode ser testado independentemente

---

## Estrutura de Diretórios

```
src/
├── common/              # Utilitários e estruturas comuns
│   ├── error_handler.h/cpp
│   ├── position.h/cpp
│   ├── utils.h/cpp
│   └── types.h
├── lexer/               # Analisador Léxico
│   ├── token.h/cpp
│   ├── lexer.h/cpp
│   └── keyword_table.h/cpp
├── parser/              # Analisador Sintático
│   ├── ast.h/cpp
│   ├── parser.h/cpp
│   └── grammar.h
├── semantic/            # Analisador Semântico
│   ├── symbol_table.h/cpp
│   ├── type_checker.h/cpp
│   ├── scope_manager.h/cpp
│   └── semantic_analyzer.h/cpp
├── codegen/             # Geração de Código
│   ├── intermediate/
│   │   ├── ir.h/cpp
│   │   └── ir_generator.h/cpp
│   ├── target/
│   │   ├── x86_64/
│   │   │   ├── x86_64_codegen.h/cpp
│   │   │   └── x86_64_registers.h
│   │   └── code_generator.h
│   └── optimizer/
│       ├── optimizer.h/cpp
│       └── optimization_passes.h/cpp
├── driver/              # Driver Principal
│   ├── compiler.h/cpp
│   └── main.cpp
└── tests/               # Testes Unitários
    ├── lexer_tests.cpp
    ├── parser_tests.cpp
    ├── semantic_tests.cpp
    └── codegen_tests.cpp
```

---

## Módulo Common

### ErrorHandler

```cpp
class ErrorHandler {
public:
    enum ErrorType {
        LEXICAL_ERROR,
        SYNTAX_ERROR,
        SEMANTIC_ERROR,
        CODEGEN_ERROR
    };
    
    struct Error {
        ErrorType type;
        std::string message;
        Position position;
        std::string suggestion;
    };
    
private:
    std::vector<Error> errors;
    std::vector<Error> warnings;
    bool hasErrors;
    
public:
    void reportError(ErrorType type, const std::string& message, 
                    const Position& pos, const std::string& suggestion = "");
    void reportWarning(const std::string& message, const Position& pos);
    
    bool hasErrorsOccurred() const { return hasErrors; }
    const std::vector<Error>& getErrors() const { return errors; }
    const std::vector<Error>& getWarnings() const { return warnings; }
    
    void printErrors() const;
    void clear();
};
```

### Position

```cpp
struct Position {
    int line;
    int column;
    std::string filename;
    
    Position(int l = 1, int c = 1, const std::string& f = "") 
        : line(l), column(c), filename(f) {}
    
    std::string toString() const;
};
```

### Types

```cpp
enum class DataType {
    VOID,
    INT,
    FLOAT,
    CHAR,
    ARRAY,
    FUNCTION,
    UNKNOWN
};

struct TypeInfo {
    DataType baseType;
    int arraySize;  // -1 se não for array
    std::vector<DataType> paramTypes;  // para funções
    DataType returnType;  // para funções
    
    bool isArray() const { return arraySize >= 0; }
    bool isFunction() const { return baseType == DataType::FUNCTION; }
    std::string toString() const;
};
```

---

## Módulo Lexer

### Token

```cpp
enum class TokenType {
    // Literais
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    CHAR_LITERAL,
    STRING_LITERAL,
    
    // Identificadores
    IDENTIFIER,
    
    // Palavras-chave
    KW_INT, KW_FLOAT, KW_CHAR, KW_VOID,
    KW_IF, KW_ELSE, KW_WHILE, KW_FOR,
    KW_RETURN, KW_BREAK, KW_CONTINUE,
    
    // Operadores
    OP_PLUS, OP_MINUS, OP_MULTIPLY, OP_DIVIDE, OP_MODULO,
    OP_ASSIGN, OP_PLUS_ASSIGN, OP_MINUS_ASSIGN,
    OP_LT, OP_LE, OP_GT, OP_GE, OP_EQ, OP_NE,
    OP_AND, OP_OR, OP_NOT,
    OP_INCREMENT, OP_DECREMENT,
    
    // Delimitadores
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    SEMICOLON, COMMA, DOT,
    
    // Especiais
    END_OF_FILE,
    INVALID_TOKEN
};

class Token {
private:
    TokenType type;
    std::string lexeme;
    Position position;
    
    // Valores para literais
    union {
        int intValue;
        double floatValue;
        char charValue;
    };
    std::string stringValue;
    
public:
    Token(TokenType t, const std::string& lex, const Position& pos);
    
    // Getters
    TokenType getType() const { return type; }
    const std::string& getLexeme() const { return lexeme; }
    const Position& getPosition() const { return position; }
    
    // Setters para valores
    void setIntValue(int val) { intValue = val; }
    void setFloatValue(double val) { floatValue = val; }
    void setCharValue(char val) { charValue = val; }
    void setStringValue(const std::string& val) { stringValue = val; }
    
    // Getters para valores
    int getIntValue() const { return intValue; }
    double getFloatValue() const { return floatValue; }
    char getCharValue() const { return charValue; }
    const std::string& getStringValue() const { return stringValue; }
    
    std::string toString() const;
};
```

### Lexer

```cpp
class Lexer {
private:
    std::ifstream input;
    std::string filename;
    Position currentPos;
    char currentChar;
    ErrorHandler* errorHandler;
    
    // Buffer para lookahead
    std::string buffer;
    size_t bufferPos;
    
    // Tabela de palavras-chave
    std::unordered_map<std::string, TokenType> keywords;
    
public:
    Lexer(const std::string& filename, ErrorHandler* eh);
    ~Lexer();
    
    Token nextToken();
    Token peekToken();  // Lookahead
    
private:
    void advance();
    char peek();
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    
    Token readNumber();
    Token readString();
    Token readChar();
    Token readIdentifier();
    Token readOperator();
    
    void initializeKeywords();
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNumeric(char c);
};
```

---

## Módulo Parser

### AST Nodes

```cpp
enum class NodeType {
    PROGRAM,
    DECLARATION,
    FUNCTION_DECL,
    VARIABLE_DECL,
    PARAMETER,
    
    COMPOUND_STMT,
    EXPRESSION_STMT,
    IF_STMT,
    WHILE_STMT,
    FOR_STMT,
    RETURN_STMT,
    BREAK_STMT,
    CONTINUE_STMT,
    
    BINARY_EXPR,
    UNARY_EXPR,
    ASSIGNMENT_EXPR,
    CALL_EXPR,
    ARRAY_ACCESS,
    IDENTIFIER,
    LITERAL
};

class ASTNode {
protected:
    NodeType nodeType;
    Position position;
    TypeInfo* typeInfo;  // Preenchido na análise semântica
    
public:
    ASTNode(NodeType type, const Position& pos) 
        : nodeType(type), position(pos), typeInfo(nullptr) {}
    virtual ~ASTNode() = default;
    
    NodeType getNodeType() const { return nodeType; }
    const Position& getPosition() const { return position; }
    
    void setTypeInfo(TypeInfo* info) { typeInfo = info; }
    TypeInfo* getTypeInfo() const { return typeInfo; }
    
    virtual std::string toString() const = 0;
    virtual void accept(class ASTVisitor* visitor) = 0;
};

// Nós específicos
class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> declarations;
    
    ProgramNode(const Position& pos) : ASTNode(NodeType::PROGRAM, pos) {}
    
    void addDeclaration(std::unique_ptr<ASTNode> decl) {
        declarations.push_back(std::move(decl));
    }
    
    std::string toString() const override;
    void accept(ASTVisitor* visitor) override;
};

class BinaryExprNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    TokenType operator_;
    std::unique_ptr<ASTNode> right;
    
    BinaryExprNode(std::unique_ptr<ASTNode> l, TokenType op, 
                   std::unique_ptr<ASTNode> r, const Position& pos)
        : ASTNode(NodeType::BINARY_EXPR, pos), left(std::move(l)), 
          operator_(op), right(std::move(r)) {}
    
    std::string toString() const override;
    void accept(ASTVisitor* visitor) override;
};

// ... outros nós específicos
```

### Parser

```cpp
class Parser {
private:
    Lexer* lexer;
    Token currentToken;
    ErrorHandler* errorHandler;
    
public:
    Parser(Lexer* lex, ErrorHandler* eh);
    
    std::unique_ptr<ProgramNode> parseProgram();
    
private:
    void advance();
    bool match(TokenType type);
    bool check(TokenType type);
    void consume(TokenType type, const std::string& message);
    
    // Métodos de parsing recursivo descendente
    std::unique_ptr<ASTNode> parseDeclaration();
    std::unique_ptr<ASTNode> parseVariableDeclaration();
    std::unique_ptr<ASTNode> parseFunctionDeclaration();
    
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseCompoundStatement();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseWhileStatement();
    std::unique_ptr<ASTNode> parseForStatement();
    std::unique_ptr<ASTNode> parseReturnStatement();
    
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseLogicalOr();
    std::unique_ptr<ASTNode> parseLogicalAnd();
    std::unique_ptr<ASTNode> parseEquality();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parseUnary();
    std::unique_ptr<ASTNode> parseCall();
    std::unique_ptr<ASTNode> parsePrimary();
    
    // Recuperação de erros
    void synchronize();
    void skipTo(TokenType type);
};
```

---

## Módulo Semantic

### Symbol Table

```cpp
enum class SymbolKind {
    VARIABLE,
    FUNCTION,
    PARAMETER
};

class Symbol {
public:
    std::string name;
    SymbolKind kind;
    TypeInfo type;
    int scope;
    Position declaration;
    bool initialized;
    
    Symbol(const std::string& n, SymbolKind k, const TypeInfo& t, 
           int s, const Position& pos)
        : name(n), kind(k), type(t), scope(s), declaration(pos), 
          initialized(false) {}
};

class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, std::unique_ptr<Symbol>>> scopes;
    int currentScope;
    
public:
    SymbolTable();
    
    void enterScope();
    void exitScope();
    
    bool insert(std::unique_ptr<Symbol> symbol);
    Symbol* lookup(const std::string& name);
    Symbol* lookupCurrentScope(const std::string& name);
    
    int getCurrentScope() const { return currentScope; }
    void printTable() const;
};
```

### Semantic Analyzer

```cpp
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visit(ProgramNode* node) = 0;
    virtual void visit(BinaryExprNode* node) = 0;
    // ... outros métodos visit
};

class SemanticAnalyzer : public ASTVisitor {
private:
    SymbolTable symbolTable;
    ErrorHandler* errorHandler;
    TypeInfo* currentFunctionReturnType;
    
public:
    SemanticAnalyzer(ErrorHandler* eh);
    
    void analyze(ProgramNode* program);
    
    // Implementação do padrão Visitor
    void visit(ProgramNode* node) override;
    void visit(BinaryExprNode* node) override;
    // ... outros métodos visit
    
private:
    void checkTypeCompatibility(TypeInfo* left, TypeInfo* right, 
                               const Position& pos);
    TypeInfo* getExpressionType(ASTNode* expr);
    bool canConvert(TypeInfo* from, TypeInfo* to);
    
    void declareVariable(const std::string& name, TypeInfo* type, 
                        const Position& pos);
    void declareFunction(const std::string& name, TypeInfo* returnType,
                        const std::vector<TypeInfo*>& paramTypes,
                        const Position& pos);
};
```

---

## Módulo CodeGen

### Intermediate Representation

```cpp
enum class IROpCode {
    // Operações aritméticas
    ADD, SUB, MUL, DIV, MOD,
    
    // Operações lógicas
    AND, OR, NOT,
    
    // Operações de comparação
    LT, LE, GT, GE, EQ, NE,
    
    // Controle de fluxo
    GOTO, IF_GOTO, IF_FALSE_GOTO,
    LABEL,
    
    // Funções
    CALL, RETURN, PARAM,
    
    // Memória
    LOAD, STORE, ALLOC,
    
    // Atribuição
    ASSIGN
};

class IRInstruction {
public:
    IROpCode opcode;
    std::string result;
    std::string arg1;
    std::string arg2;
    
    IRInstruction(IROpCode op, const std::string& res = "",
                  const std::string& a1 = "", const std::string& a2 = "")
        : opcode(op), result(res), arg1(a1), arg2(a2) {}
    
    std::string toString() const;
};

class IRGenerator : public ASTVisitor {
private:
    std::vector<IRInstruction> instructions;
    int tempCounter;
    int labelCounter;
    
public:
    IRGenerator();
    
    const std::vector<IRInstruction>& generate(ProgramNode* program);
    
    // Implementação do padrão Visitor
    void visit(ProgramNode* node) override;
    void visit(BinaryExprNode* node) override;
    // ... outros métodos visit
    
private:
    std::string newTemp();
    std::string newLabel();
    void emit(IROpCode op, const std::string& result = "",
              const std::string& arg1 = "", const std::string& arg2 = "");
    
    std::string currentResult;  // Para comunicação entre visits
};
```

### Code Generator

```cpp
class CodeGenerator {
protected:
    const std::vector<IRInstruction>& ir;
    std::ostringstream output;
    
public:
    CodeGenerator(const std::vector<IRInstruction>& instructions)
        : ir(instructions) {}
    
    virtual ~CodeGenerator() = default;
    
    virtual std::string generate() = 0;
    
protected:
    virtual void generateInstruction(const IRInstruction& instr) = 0;
};

class X86_64CodeGenerator : public CodeGenerator {
private:
    std::unordered_map<std::string, std::string> registerMap;
    std::vector<std::string> availableRegisters;
    int stackOffset;
    
public:
    X86_64CodeGenerator(const std::vector<IRInstruction>& instructions);
    
    std::string generate() override;
    
private:
    void generateInstruction(const IRInstruction& instr) override;
    
    void generatePrologue();
    void generateEpilogue();
    
    std::string allocateRegister(const std::string& temp);
    void freeRegister(const std::string& reg);
    
    void generateArithmetic(const IRInstruction& instr);
    void generateComparison(const IRInstruction& instr);
    void generateJump(const IRInstruction& instr);
    void generateCall(const IRInstruction& instr);
};
```

---

## Módulo Driver

### Compiler

```cpp
class Compiler {
private:
    ErrorHandler errorHandler;
    std::string inputFile;
    std::string outputFile;
    bool optimizationEnabled;
    bool debugMode;
    
public:
    Compiler();
    
    bool compile(const std::string& input, const std::string& output = "");
    
    void setOptimization(bool enabled) { optimizationEnabled = enabled; }
    void setDebugMode(bool enabled) { debugMode = enabled; }
    
private:
    bool lexicalAnalysis(std::vector<Token>& tokens);
    bool syntaxAnalysis(const std::vector<Token>& tokens, 
                       std::unique_ptr<ProgramNode>& ast);
    bool semanticAnalysis(ProgramNode* ast);
    bool codeGeneration(ProgramNode* ast, std::string& assembly);
    
    void printTokens(const std::vector<Token>& tokens);
    void printAST(ProgramNode* ast);
    void writeOutput(const std::string& assembly);
};
```

---

## Padrões de Design Utilizados

### 1. Visitor Pattern
- **Uso**: Travessia da AST para análise semântica e geração de código
- **Benefício**: Separação entre estrutura da árvore e operações

### 2. Strategy Pattern
- **Uso**: Diferentes geradores de código para diferentes arquiteturas
- **Benefício**: Fácil adição de novas arquiteturas alvo

### 3. Factory Pattern
- **Uso**: Criação de nós da AST
- **Benefício**: Centralização da criação de objetos

### 4. Observer Pattern
- **Uso**: Sistema de relatório de erros
- **Benefício**: Desacoplamento entre detecção e relatório de erros

### 5. Template Method Pattern
- **Uso**: Estrutura geral do compilador
- **Benefício**: Definição do algoritmo geral com passos específicos

---

## Interfaces e Contratos

### Interface ILexer
```cpp
class ILexer {
public:
    virtual ~ILexer() = default;
    virtual Token nextToken() = 0;
    virtual Token peekToken() = 0;
};
```

### Interface IParser
```cpp
class IParser {
public:
    virtual ~IParser() = default;
    virtual std::unique_ptr<ProgramNode> parse() = 0;
};
```

### Interface ISemanticAnalyzer
```cpp
class ISemanticAnalyzer {
public:
    virtual ~ISemanticAnalyzer() = default;
    virtual bool analyze(ProgramNode* ast) = 0;
};
```

### Interface ICodeGenerator
```cpp
class ICodeGenerator {
public:
    virtual ~ICodeGenerator() = default;
    virtual std::string generateCode(ProgramNode* ast) = 0;
};
```

---

## Tratamento de Erros

### Estratégia de Recuperação

1. **Lexer**: Continua após caractere inválido
2. **Parser**: Sincronização em pontos específicos (`;`, `}`, etc.)
3. **Semantic**: Continua verificação após erro de tipo
4. **CodeGen**: Para na primeira falha crítica

### Níveis de Severidade

- **Error**: Para a compilação
- **Warning**: Continua a compilação
- **Info**: Informações adicionais
- **Debug**: Apenas em modo debug

---

## Otimizações

### Passes de Otimização

1. **Constant Folding**: Avaliação de expressões constantes
2. **Dead Code Elimination**: Remoção de código não utilizado
3. **Common Subexpression Elimination**: Eliminação de subexpressões comuns
4. **Register Allocation**: Alocação eficiente de registradores

### Pipeline de Otimização

```cpp
class OptimizationPipeline {
private:
    std::vector<std::unique_ptr<OptimizationPass>> passes;
    
public:
    void addPass(std::unique_ptr<OptimizationPass> pass);
    void optimize(std::vector<IRInstruction>& ir);
};
```

---

## Extensibilidade

### Adição de Novas Funcionalidades

1. **Novos Tipos**: Estender enum DataType e TypeInfo
2. **Novos Operadores**: Adicionar em TokenType e implementar parsing
3. **Novas Estruturas**: Criar novos nós AST e implementar visitor
4. **Novas Arquiteturas**: Implementar nova classe CodeGenerator

### Pontos de Extensão

- **Lexer**: Novos tipos de token
- **Parser**: Novas construções sintáticas
- **Semantic**: Novas regras semânticas
- **CodeGen**: Novas arquiteturas alvo

Esta arquitetura fornece uma base sólida e extensível para o desenvolvimento do compilador C, seguindo boas práticas de engenharia de software e permitindo fácil manutenção e evolução do sistema.