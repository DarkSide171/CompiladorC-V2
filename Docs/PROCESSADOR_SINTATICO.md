# Documentação do Processador Sintático (Parser) C

## Visão Geral

O **Processador Sintático** é o componente responsável pela análise sintática do código C pré-processado, construindo uma Árvore de Sintaxe Abstrata (AST) que representa a estrutura hierárquica do programa. Este módulo integra-se diretamente com o analisador léxico para consumir tokens e com o analisador semântico para fornecer a estrutura sintática validada.

### Características Principais

- **Parser Recursivo Descendente**: Implementação baseada em gramática LL(1) com lookahead
- **Construção de AST**: Geração automática de árvore de sintaxe abstrata
- **Recuperação de Erros**: Estratégias avançadas de recuperação e continuação
- **Integração Modular**: Interface limpa com lexer, preprocessor e analisador semântico
- **Suporte Completo ao C**: Compatibilidade com padrões C89, C99, C11, C17 e C23
- **Extensibilidade**: Arquitetura preparada para extensões e otimizações

---

## Índice

1. [Arquitetura e Organização](#arquitetura-e-organização)
2. [Estrutura de Arquivos e Diretórios](#estrutura-de-arquivos-e-diretórios)
3. [Integração com Outros Componentes](#integração-com-outros-componentes)
4. [Arquitetura Modular](#arquitetura-modular)
5. [Estruturas de Dados Principais](#estruturas-de-dados-principais)
6. [Gramática e Regras de Produção](#gramática-e-regras-de-produção)
7. [Tratamento de Erros e Recuperação](#tratamento-de-erros-e-recuperação)
8. [Sistema de Logging e Debug](#sistema-de-logging-e-debug)
9. [Considerações de Implementação](#considerações-de-implementação)
10. [Especificação de Métodos por Arquivo](#especificação-de-métodos-por-arquivo)
11. [Cronograma de Implementação](#cronograma-de-implementação)

---

## Arquitetura e Organização

### Visão Geral da Arquitetura

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Analisador    │───▶│   Processador    │───▶│   Analisador    │
│     Léxico      │    │    Sintático     │    │   Semântico     │
│    (Tokens)     │    │     (AST)        │    │  (Validação)    │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                                │
                                ▼
                       ┌──────────────────┐
                       │   Estruturas     │
                       │   Compartilhadas │
                       │   (AST, Errors)  │
                       └──────────────────┘
```

### Fluxo de Processamento

1. **Entrada**: Recebe stream de tokens do analisador léxico
2. **Parsing**: Aplica regras gramaticais para construir AST
3. **Validação**: Verifica correção sintática e estrutural
4. **Recuperação**: Trata erros e tenta continuar análise
5. **Saída**: Fornece AST validada para análise semântica

---

## Estrutura de Arquivos e Diretórios

### Organização Hierárquica

```
src/parser/
├── include/                    # Headers públicos e privados
│   ├── parser.hpp             # Interface principal do parser
│   ├── ast.hpp                # Definições da AST
│   ├── grammar.hpp            # Regras gramaticais
│   ├── error_recovery.hpp     # Sistema de recuperação de erros
│   ├── parser_config.hpp      # Configurações do parser
│   ├── parser_state.hpp       # Estado interno do parser
│   ├── parser_logger.hpp      # Sistema de logging
│   └── parser_types.hpp       # Tipos e estruturas compartilhadas
├── src/                       # Implementações
│   ├── parser.cpp             # Implementação principal
│   ├── ast.cpp                # Construção e manipulação da AST
│   ├── grammar.cpp            # Implementação das regras gramaticais
│   ├── error_recovery.cpp     # Estratégias de recuperação
│   ├── parser_config.cpp      # Gerenciamento de configurações
│   ├── parser_state.cpp       # Gerenciamento de estado
│   └── parser_logger.cpp      # Sistema de logging
├── data/                      # Dados de configuração
│   ├── grammar/               # Definições gramaticais
│   │   ├── c89_grammar.txt    # Gramática C89
│   │   ├── c99_grammar.txt    # Gramática C99
│   │   ├── c11_grammar.txt    # Gramática C11
│   │   ├── c17_grammar.txt    # Gramática C17
│   │   └── c23_grammar.txt    # Gramática C23
│   ├── precedence.txt         # Tabela de precedência de operadores
│   └── error_messages.txt     # Mensagens de erro padronizadas
└── tests/                     # Testes unitários e de integração
    ├── unit/                  # Testes unitários
    │   ├── test_parser.cpp    # Testes do parser principal
    │   ├── test_ast.cpp       # Testes da AST
    │   ├── test_grammar.cpp   # Testes das regras gramaticais
    │   └── test_recovery.cpp  # Testes de recuperação de erros
    ├── integration/           # Testes de integração
    │   ├── test_lexer_integration.cpp # Integração com lexer
    │   └── test_semantic_integration.cpp # Integração com semântico
    └── data/                  # Dados de teste
        ├── valid/             # Códigos C válidos
        └── invalid/           # Códigos com erros sintáticos
```

### Descrição dos Arquivos Principais

#### Headers (include/)

- **`parser.hpp`**: Interface principal do processador sintático
- **`ast.hpp`**: Definições completas da Árvore de Sintaxe Abstrata
- **`grammar.hpp`**: Regras gramaticais e sistema de produção
- **`error_recovery.hpp`**: Estratégias de recuperação de erros sintáticos
- **`parser_config.hpp`**: Sistema de configuração e versões do C
- **`parser_state.hpp`**: Gerenciamento de estado interno do parser
- **`parser_logger.hpp`**: Sistema de logging e debug
- **`parser_types.hpp`**: Tipos fundamentais e estruturas compartilhadas

#### Implementações (src/)

- **`parser.cpp`**: Lógica principal de parsing recursivo descendente
- **`ast.cpp`**: Construção, navegação e manipulação da AST
- **`grammar.cpp`**: Implementação das regras de produção
- **`error_recovery.cpp`**: Algoritmos de recuperação e sincronização
- **`parser_config.cpp`**: Carregamento e gerenciamento de configurações
- **`parser_state.cpp`**: Controle de estado e contexto de parsing
- **`parser_logger.cpp`**: Implementação do sistema de logging

#### Dados de Configuração (data/)

- **`grammar/`**: Definições gramaticais específicas por versão do C
- **`precedence.txt`**: Tabela de precedência e associatividade de operadores
- **`error_messages.txt`**: Mensagens de erro padronizadas e localizadas

---

## Flexibilidade e Evolução da Estrutura

### Extensibilidade

A arquitetura foi projetada para suportar:

- **Novas Versões do C**: Adição de gramáticas específicas
- **Otimizações**: Implementação de técnicas avançadas de parsing
- **Extensões de Linguagem**: Suporte a extensões específicas de compiladores
- **Backends Alternativos**: Diferentes representações de AST

### Modularidade

Cada componente é independente e pode ser:

- **Testado Isoladamente**: Testes unitários específicos
- **Substituído**: Implementações alternativas
- **Estendido**: Funcionalidades adicionais
- **Reutilizado**: Em outros contextos de compilação

---

## Integração com Outros Componentes

### Interface com Analisador Léxico

```cpp
// Integração direta com o sistema de tokens
class ParserLexerInterface {
public:
    // Recebe tokens do lexer
    Token getCurrentToken() const;
    Token peekNextToken() const;
    void consumeToken();
    
    // Sincronização de posições
    SourcePosition getCurrentPosition() const;
    void synchronizePosition();
};
```

### Interface com Pré-processador

```cpp
// Utiliza mapeamento de posições do preprocessor
class ParserPreprocessorInterface {
public:
    // Mapeamento de posições
    SourcePosition mapToOriginalPosition(const SourcePosition& expanded_pos);
    bool isMacroExpansion(const SourcePosition& pos);
    std::string getMacroName(const SourcePosition& pos);
};
```

### Interface com Analisador Semântico

```cpp
// Fornece AST para análise semântica
class ParserSemanticInterface {
public:
    // Transferência da AST
    std::shared_ptr<AST> getAST() const;
    void validateASTIntegrity() const;
    
    // Informações de contexto
    std::vector<Symbol> getSymbolTable() const;
    std::vector<Diagnostic> getDiagnostics() const;
};
```

### Estruturas de Dados Compartilhadas

#### Token (do Lexer)
```cpp
struct Token {
    TokenType type;
    std::string value;
    SourcePosition position;
    std::map<std::string, std::string> attributes;
};
```

#### AST Node (para Semântico)
```cpp
struct ASTNode {
    NodeType type;
    std::string value;
    SourcePosition position;
    std::vector<std::shared_ptr<ASTNode>> children;
    std::map<std::string, std::any> attributes;
};
```

#### Diagnostic (Sistema de Erros)
```cpp
struct ParserDiagnostic {
    DiagnosticLevel level;
    std::string message;
    SourcePosition position;
    std::string suggestion;
    std::vector<SourcePosition> related_positions;
};
```

---

## Arquitetura de Comunicação

### Pipeline Estendido

```
Preprocessor ──┐
               ├──▶ Parser ──▶ Semantic Analyzer
Lexer ─────────┘
```

### Sistema de Configuração Integrado

- **Configurações Compartilhadas**: Versão do C, flags de compilação
- **Configurações Específicas**: Estratégias de parsing, recuperação de erros
- **Configurações Dinâmicas**: Ajustes baseados no contexto de compilação

---

## Arquitetura Modular

### Classe Principal: Parser

```cpp
class Parser {
private:
    std::unique_ptr<Grammar> grammar;
    std::unique_ptr<ErrorRecovery> error_recovery;
    std::unique_ptr<ParserState> state;
    std::unique_ptr<ParserLogger> logger;
    std::shared_ptr<AST> ast;
    
public:
    // Interface principal
    bool parse(TokenStream& tokens);
    std::shared_ptr<AST> getAST() const;
    std::vector<Diagnostic> getDiagnostics() const;
};
```

### Módulos Especializados

#### 1. Grammar (Gramática)
- **Responsabilidade**: Definir e aplicar regras de produção
- **Funcionalidades**: Parsing recursivo descendente, verificação de precedência
- **Interface**: Métodos para cada construção sintática

#### 2. AST (Árvore de Sintaxe Abstrata)
- **Responsabilidade**: Construir e gerenciar a representação da árvore
- **Funcionalidades**: Criação de nós, navegação, serialização
- **Interface**: Factory methods, visitors, iteradores

#### 3. ErrorRecovery (Recuperação de Erros)
- **Responsabilidade**: Detectar e recuperar de erros sintáticos
- **Funcionalidades**: Sincronização, inserção/deleção de tokens
- **Interface**: Estratégias de recuperação configuráveis

#### 4. ParserState (Estado do Parser)
- **Responsabilidade**: Gerenciar contexto e estado durante parsing
- **Funcionalidades**: Stack de contextos, lookahead, backtracking
- **Interface**: Operações de estado e contexto

#### 5. ParserConfig (Configuração)
- **Responsabilidade**: Gerenciar configurações e versões do C
- **Funcionalidades**: Carregamento de gramáticas, flags de compilação
- **Interface**: Configuração dinâmica e estática

#### 6. ParserLogger (Logging)
- **Responsabilidade**: Logging detalhado do processo de parsing
- **Funcionalidades**: Debug, profiling, análise de performance
- **Interface**: Níveis de log configuráveis

---

## Estruturas de Dados Principais

### 1. Árvore de Sintaxe Abstrata (AST)

```cpp
class ASTNode {
public:
    enum class NodeType {
        // Declarações
        PROGRAM, FUNCTION_DECLARATION, VARIABLE_DECLARATION,
        STRUCT_DECLARATION, UNION_DECLARATION, ENUM_DECLARATION,
        
        // Statements
        COMPOUND_STATEMENT, EXPRESSION_STATEMENT, IF_STATEMENT,
        WHILE_STATEMENT, FOR_STATEMENT, SWITCH_STATEMENT,
        RETURN_STATEMENT, BREAK_STATEMENT, CONTINUE_STATEMENT,
        
        // Expressões
        BINARY_EXPRESSION, UNARY_EXPRESSION, ASSIGNMENT_EXPRESSION,
        CALL_EXPRESSION, MEMBER_EXPRESSION, ARRAY_ACCESS,
        CONDITIONAL_EXPRESSION, CAST_EXPRESSION,
        
        // Literais
        INTEGER_LITERAL, FLOAT_LITERAL, STRING_LITERAL,
        CHARACTER_LITERAL, IDENTIFIER,
        
        // Tipos
        TYPE_SPECIFIER, POINTER_TYPE, ARRAY_TYPE, FUNCTION_TYPE
    };
    
private:
    NodeType type;
    std::string value;
    SourcePosition position;
    std::vector<std::shared_ptr<ASTNode>> children;
    std::map<std::string, std::any> attributes;
    
public:
    // Construtores e factory methods
    static std::shared_ptr<ASTNode> create(NodeType type, const std::string& value = "");
    
    // Navegação
    void addChild(std::shared_ptr<ASTNode> child);
    std::shared_ptr<ASTNode> getChild(size_t index) const;
    size_t getChildCount() const;
    
    // Atributos
    void setAttribute(const std::string& key, const std::any& value);
    std::any getAttribute(const std::string& key) const;
    
    // Utilidades
    std::string toString() const;
    void accept(ASTVisitor& visitor);
};
```

### 2. Stack de Parsing

```cpp
class ParsingStack {
private:
    std::stack<ParsingContext> contexts;
    
public:
    struct ParsingContext {
        std::string rule_name;
        SourcePosition start_position;
        std::vector<std::shared_ptr<ASTNode>> partial_nodes;
        std::map<std::string, std::any> local_attributes;
    };
    
    void pushContext(const std::string& rule_name);
    void popContext();
    ParsingContext& getCurrentContext();
    bool isEmpty() const;
};
```

### 3. Tabela de Símbolos (Integrada)

```cpp
class ParserSymbolTable {
private:
    std::vector<std::map<std::string, SymbolInfo>> scopes;
    
public:
    struct SymbolInfo {
        std::string name;
        SymbolType type;
        SourcePosition declaration_position;
        std::shared_ptr<ASTNode> declaration_node;
        bool is_defined;
    };
    
    void enterScope();
    void exitScope();
    void declareSymbol(const SymbolInfo& symbol);
    SymbolInfo* lookupSymbol(const std::string& name);
    bool isSymbolDeclared(const std::string& name) const;
};
```

### 4. Sistema de Recuperação de Erros

```cpp
class ErrorRecoverySystem {
public:
    enum class RecoveryStrategy {
        PANIC_MODE,      // Descarta tokens até sincronização
        PHRASE_LEVEL,    // Inserção/deleção local de tokens
        ERROR_PRODUCTIONS, // Regras específicas para erros
        GLOBAL_CORRECTION  // Correção global mínima
    };
    
private:
    RecoveryStrategy current_strategy;
    std::set<TokenType> synchronization_tokens;
    
public:
    bool recoverFromError(const ParserError& error, TokenStream& tokens);
    void setSynchronizationTokens(const std::set<TokenType>& tokens);
    void setRecoveryStrategy(RecoveryStrategy strategy);
};
```

---

## Fluxo de Processamento Detalhado

### 1. Inicialização

```cpp
// Sequência de inicialização do parser
1. Carregar configurações (versão C, flags)
2. Inicializar gramática específica
3. Configurar sistema de recuperação de erros
4. Preparar estruturas de dados (AST, symbol table)
5. Configurar sistema de logging
```

### 2. Loop Principal de Parsing

```cpp
// Algoritmo principal de parsing recursivo descendente
while (!tokens.isAtEnd()) {
    try {
        // 1. Identificar construção sintática
        auto construction = identifyConstruction(tokens.peek());
        
        // 2. Aplicar regra gramatical apropriada
        auto node = applyGrammarRule(construction, tokens);
        
        // 3. Adicionar à AST
        ast->addTopLevelNode(node);
        
        // 4. Atualizar estado
        state->updateContext(node);
        
    } catch (const ParserError& error) {
        // 5. Recuperar de erro
        error_recovery->recover(error, tokens);
    }
}
```

### 3. Construção da AST

```cpp
// Processo de construção de nós da AST
1. Criar nó baseado na regra gramatical
2. Processar filhos recursivamente
3. Aplicar atributos semânticos
4. Validar estrutura local
5. Integrar ao nó pai
```

---

## Suporte a Versões do C

### Configuração por Versão

```cpp
enum class CVersion {
    C89,  // ANSI C / C90
    C99,  // ISO C99
    C11,  // ISO C11
    C17,  // ISO C17 (C18)
    C23   // ISO C23
};

class VersionSpecificGrammar {
public:
    static std::unique_ptr<Grammar> createGrammar(CVersion version);
    static std::set<std::string> getSupportedFeatures(CVersion version);
    static bool isFeatureSupported(const std::string& feature, CVersion version);
};
```

### Sistema de Features

```cpp
// Features específicas por versão
struct LanguageFeatures {
    // C99 features
    bool variable_length_arrays = false;
    bool designated_initializers = false;
    bool compound_literals = false;
    bool flexible_array_members = false;
    
    // C11 features
    bool static_assertions = false;
    bool anonymous_structs_unions = false;
    bool thread_local_storage = false;
    
    // C23 features
    bool typeof_operator = false;
    bool binary_literals = false;
    bool digit_separators = false;
};
```

---

## Tratamento de Erros e Recuperação

### Estratégias de Recuperação

#### 1. Modo Pânico (Panic Mode)
```cpp
// Descarta tokens até encontrar token de sincronização
void panicModeRecovery(TokenStream& tokens) {
    while (!tokens.isAtEnd() && 
           !synchronization_tokens.count(tokens.peek().type)) {
        tokens.consume();
    }
}
```

#### 2. Recuperação em Nível de Frase
```cpp
// Inserção ou deleção local de tokens
struct PhraseRecoveryAction {
    enum Type { INSERT, DELETE, REPLACE };
    Type action;
    TokenType expected_token;
    std::string suggestion;
};
```

#### 3. Produções de Erro
```cpp
// Regras gramaticais específicas para erros comuns
std::shared_ptr<ASTNode> parseErrorProduction(const ParserError& error) {
    // Criar nó de erro com informações de recuperação
    auto error_node = ASTNode::create(ASTNode::NodeType::ERROR);
    error_node->setAttribute("error_message", error.getMessage());
    error_node->setAttribute("recovery_suggestion", error.getSuggestion());
    return error_node;
}
```

### Sistema de Mensagens

#### Mensagens Contextualizadas
```cpp
struct ParserErrorMessage {
    std::string primary_message;     // Mensagem principal
    std::string context_info;        // Informação de contexto
    std::vector<std::string> suggestions; // Sugestões de correção
    ErrorSeverity severity;          // Gravidade do erro
};
```

#### Sugestões de Correção
```cpp
// Exemplos de sugestões automáticas
"Expected ';' after expression"
"Did you mean 'struct' instead of 'strcut'?"
"Missing closing '}' for compound statement"
"Incompatible type in assignment - did you forget a cast?"
```

#### Severidade
```cpp
enum class ErrorSeverity {
    NOTE,     // Informação adicional
    WARNING,  // Aviso - código pode funcionar
    ERROR,    // Erro - código não compila
    FATAL     // Erro fatal - interrompe compilação
};
```

---

## Sistema de Logging

### Níveis de Log

```cpp
enum class LogLevel {
    TRACE,    // Rastreamento detalhado
    DEBUG,    // Informações de debug
    INFO,     // Informações gerais
    WARNING,  // Avisos
    ERROR,    // Erros
    FATAL     // Erros fatais
};
```

### Funcionalidades de Log

```cpp
class ParserLogger {
public:
    // Logging de parsing
    void logRuleEntry(const std::string& rule_name);
    void logRuleExit(const std::string& rule_name, bool success);
    void logTokenConsumption(const Token& token);
    void logASTNodeCreation(const ASTNode& node);
    
    // Logging de erros
    void logError(const ParserError& error);
    void logRecoveryAttempt(const std::string& strategy);
    void logRecoverySuccess(const std::string& details);
    
    // Logging de performance
    void logParsingTime(const std::string& rule, double time_ms);
    void logMemoryUsage(size_t bytes);
    
    // Configuração
    void setLogLevel(LogLevel level);
    void enableFileLogging(const std::string& filename);
    void enableConsoleLogging(bool enable);
};
```

---

## Gramática e Regras de Produção

### Estrutura da Gramática

```cpp
// Gramática C simplificada (LL1)
class CGrammar {
public:
    // Programa principal
    std::shared_ptr<ASTNode> parseProgram();
    
    // Declarações
    std::shared_ptr<ASTNode> parseDeclaration();
    std::shared_ptr<ASTNode> parseFunctionDeclaration();
    std::shared_ptr<ASTNode> parseVariableDeclaration();
    std::shared_ptr<ASTNode> parseStructDeclaration();
    
    // Statements
    std::shared_ptr<ASTNode> parseStatement();
    std::shared_ptr<ASTNode> parseCompoundStatement();
    std::shared_ptr<ASTNode> parseIfStatement();
    std::shared_ptr<ASTNode> parseWhileStatement();
    std::shared_ptr<ASTNode> parseForStatement();
    
    // Expressões (com precedência)
    std::shared_ptr<ASTNode> parseExpression();
    std::shared_ptr<ASTNode> parseAssignmentExpression();
    std::shared_ptr<ASTNode> parseConditionalExpression();
    std::shared_ptr<ASTNode> parseLogicalOrExpression();
    std::shared_ptr<ASTNode> parseLogicalAndExpression();
    std::shared_ptr<ASTNode> parseEqualityExpression();
    std::shared_ptr<ASTNode> parseRelationalExpression();
    std::shared_ptr<ASTNode> parseAdditiveExpression();
    std::shared_ptr<ASTNode> parseMultiplicativeExpression();
    std::shared_ptr<ASTNode> parseUnaryExpression();
    std::shared_ptr<ASTNode> parsePostfixExpression();
    std::shared_ptr<ASTNode> parsePrimaryExpression();
};
```

### Tabela de Precedência

```cpp
// Precedência de operadores (maior número = maior precedência)
struct OperatorPrecedence {
    static const std::map<TokenType, int> precedence_table;
    static const std::map<TokenType, Associativity> associativity_table;
    
    enum class Associativity {
        LEFT_TO_RIGHT,
        RIGHT_TO_LEFT,
        NON_ASSOCIATIVE
    };
    
    static int getPrecedence(TokenType op);
    static Associativity getAssociativity(TokenType op);
};
```

---

## Casos de Uso e Exemplos

### Parsing de Código C Básico

```c
// Exemplo de entrada
int main() {
    int x = 10;
    if (x > 5) {
        printf("Hello, World!\n");
    }
    return 0;
}
```

```cpp
// AST resultante (simplificada)
Program
├── FunctionDeclaration (main)
│   ├── ReturnType (int)
│   ├── Parameters ()
│   └── CompoundStatement
│       ├── VariableDeclaration (x)
│       │   ├── Type (int)
│       │   └── Initializer (10)
│       ├── IfStatement
│       │   ├── Condition (x > 5)
│       │   └── ThenStatement
│       │       └── ExpressionStatement
│       │           └── CallExpression (printf)
│       └── ReturnStatement
│           └── Expression (0)
```

### Tratamento de Erros Comuns

```c
// Código com erro sintático
int main() {
    int x = 10  // Falta ponto e vírgula
    if (x > 5) {
        printf("Hello\n");
    }
    return 0;
}
```

```
// Saída do parser
Error: Expected ';' after variable declaration
  --> example.c:2:15
   |
 2 |     int x = 10  // Falta ponto e vírgula
   |               ^
   |
Suggestion: Add ';' after the expression

Recovery: Inserted ';' and continued parsing
```

---

## Estratégia de Testes

### Testes Funcionais

#### 1. Testes de Parsing Básico
```cpp
// Testes para construções sintáticas fundamentais
TEST(ParserTest, ParseSimpleDeclaration) {
    std::string code = "int x;";
    auto ast = parser.parse(code);
    ASSERT_TRUE(ast != nullptr);
    EXPECT_EQ(ast->getType(), ASTNode::NodeType::VARIABLE_DECLARATION);
}

TEST(ParserTest, ParseFunctionDefinition) {
    std::string code = "int func() { return 0; }";
    auto ast = parser.parse(code);
    ASSERT_TRUE(ast != nullptr);
    EXPECT_EQ(ast->getType(), ASTNode::NodeType::FUNCTION_DECLARATION);
}
```

#### 2. Testes de Expressões
```cpp
TEST(ParserTest, ParseComplexExpression) {
    std::string code = "x = a + b * c - d / e;";
    auto ast = parser.parseExpression(code);
    // Verificar precedência correta na AST
    EXPECT_EQ(getOperatorPrecedence(ast), expected_precedence);
}
```

### Testes de Robustez

#### 1. Testes de Recuperação de Erros
```cpp
TEST(ParserTest, RecoverFromMissingSemicolon) {
    std::string code = "int x = 10\nint y = 20;";
    auto result = parser.parse(code);
    EXPECT_TRUE(result.has_errors);
    EXPECT_EQ(result.error_count, 1);
    EXPECT_TRUE(result.recovered_successfully);
}
```

#### 2. Testes de Código Malformado
```cpp
TEST(ParserTest, HandleSeverelyMalformedCode) {
    std::string code = "int { } ( ) [ ] ; , . ->";
    auto result = parser.parse(code);
    EXPECT_FALSE(result.success);
    EXPECT_GT(result.error_count, 0);
}
```

### Framework de Testes

```cpp
class ParserTestFramework {
public:
    struct TestCase {
        std::string name;
        std::string input_code;
        bool should_succeed;
        std::vector<std::string> expected_errors;
        std::function<bool(const AST&)> ast_validator;
    };
    
    void runTestSuite(const std::vector<TestCase>& tests);
    void generateCoverageReport();
    void benchmarkPerformance();
};
```

---

## Considerações de Implementação

### Arquitetura Extensível

- **Padrão Strategy**: Para diferentes algoritmos de parsing
- **Padrão Visitor**: Para traversal e transformação da AST
- **Padrão Factory**: Para criação de nós específicos da AST
- **Padrão Observer**: Para notificação de eventos de parsing

### Performance

- **Parsing Incremental**: Suporte a re-parsing parcial
- **Memoização**: Cache de resultados de parsing
- **Lazy Evaluation**: Construção sob demanda de partes da AST
- **Memory Pooling**: Gerenciamento eficiente de memória

### Manutenibilidade

- **Separação de Responsabilidades**: Cada classe tem função específica
- **Interfaces Bem Definidas**: Contratos claros entre componentes
- **Documentação Inline**: Comentários detalhados no código
- **Testes Abrangentes**: Cobertura de código > 95%

---

## Especificação de Métodos por Arquivo

### Headers (.hpp)

#### **parser.hpp** - Interface Principal do Parser

```cpp
class Parser {
public:
    // Construtores e Destrutor
    Parser();
    explicit Parser(const ParserConfig& config);
    ~Parser();
    
    // Métodos Principais de Parsing
    ParseResult parse(TokenStream& tokens);
    ParseResult parseString(const std::string& code);
    ParseResult parseFile(const std::string& filename);
    
    // Configuração
    void setConfig(const ParserConfig& config);
    ParserConfig getConfig() const;
    void setCVersion(CVersion version);
    
    // Acesso aos Resultados
    std::shared_ptr<AST> getAST() const;
    std::vector<Diagnostic> getDiagnostics() const;
    ParserStatistics getStatistics() const;
    
    // Estado e Controle
    void reset();
    bool isInitialized() const;
    void enableDebugMode(bool enable);
    
    // Integração
    void setLexerInterface(std::shared_ptr<LexerInterface> lexer);
    void setErrorHandler(std::shared_ptr<ErrorHandler> handler);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### **ast.hpp** - Árvore de Sintaxe Abstrata

```cpp
class ASTNode {
public:
    // Tipos de Nós
    enum class NodeType { /* ... */ };
    
    // Construtores
    ASTNode(NodeType type, const std::string& value = "");
    static std::shared_ptr<ASTNode> create(NodeType type, const std::string& value = "");
    
    // Propriedades Básicas
    NodeType getType() const;
    std::string getValue() const;
    void setValue(const std::string& value);
    SourcePosition getPosition() const;
    void setPosition(const SourcePosition& pos);
    
    // Hierarquia
    void addChild(std::shared_ptr<ASTNode> child);
    void insertChild(size_t index, std::shared_ptr<ASTNode> child);
    void removeChild(size_t index);
    std::shared_ptr<ASTNode> getChild(size_t index) const;
    size_t getChildCount() const;
    std::shared_ptr<ASTNode> getParent() const;
    
    // Navegação
    std::shared_ptr<ASTNode> getFirstChild() const;
    std::shared_ptr<ASTNode> getLastChild() const;
    std::shared_ptr<ASTNode> getNextSibling() const;
    std::shared_ptr<ASTNode> getPreviousSibling() const;
    
    // Atributos
    void setAttribute(const std::string& key, const std::any& value);
    std::any getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    
    // Utilidades
    std::string toString() const;
    std::string toXML() const;
    std::string toJSON() const;
    void accept(ASTVisitor& visitor);
    std::shared_ptr<ASTNode> clone() const;
    
    // Validação
    bool isValid() const;
    std::vector<std::string> validate() const;
};

class AST {
public:
    // Construção
    AST();
    explicit AST(std::shared_ptr<ASTNode> root);
    
    // Acesso à Raiz
    std::shared_ptr<ASTNode> getRoot() const;
    void setRoot(std::shared_ptr<ASTNode> root);
    
    // Navegação Global
    std::vector<std::shared_ptr<ASTNode>> findNodesByType(ASTNode::NodeType type) const;
    std::vector<std::shared_ptr<ASTNode>> findNodesByValue(const std::string& value) const;
    std::shared_ptr<ASTNode> findFirstNode(std::function<bool(const ASTNode&)> predicate) const;
    
    // Transformações
    void transform(ASTTransformer& transformer);
    std::shared_ptr<AST> clone() const;
    
    // Serialização
    std::string serialize() const;
    bool deserialize(const std::string& data);
    
    // Estatísticas
    size_t getNodeCount() const;
    size_t getDepth() const;
    std::map<ASTNode::NodeType, size_t> getNodeTypeStatistics() const;
    
    // Validação
    bool isValid() const;
    std::vector<std::string> validate() const;
};
```

#### **grammar.hpp** - Sistema de Gramática

```cpp
class Grammar {
public:
    // Construção
    Grammar();
    explicit Grammar(CVersion version);
    
    // Carregamento de Regras
    bool loadFromFile(const std::string& filename);
    bool loadFromString(const std::string& grammar_text);
    void addRule(const std::string& rule_name, const ProductionRule& rule);
    
    // Parsing de Construções
    std::shared_ptr<ASTNode> parseProgram(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseDeclaration(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseStatement(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseExpression(TokenStream& tokens);
    
    // Parsing de Declarações
    std::shared_ptr<ASTNode> parseFunctionDeclaration(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseVariableDeclaration(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseStructDeclaration(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseUnionDeclaration(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseEnumDeclaration(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseTypedefDeclaration(TokenStream& tokens);
    
    // Parsing de Statements
    std::shared_ptr<ASTNode> parseCompoundStatement(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseIfStatement(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseWhileStatement(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseForStatement(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseSwitchStatement(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseReturnStatement(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseBreakStatement(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseContinueStatement(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseGotoStatement(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseLabelStatement(TokenStream& tokens);
    
    // Parsing de Expressões (por precedência)
    std::shared_ptr<ASTNode> parseAssignmentExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseConditionalExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseLogicalOrExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseLogicalAndExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseInclusiveOrExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseExclusiveOrExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseAndExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseEqualityExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseRelationalExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseShiftExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseAdditiveExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseMultiplicativeExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseCastExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseUnaryExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parsePostfixExpression(TokenStream& tokens);
    std::shared_ptr<ASTNode> parsePrimaryExpression(TokenStream& tokens);
    
    // Parsing de Tipos
    std::shared_ptr<ASTNode> parseTypeSpecifier(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseDeclarator(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseAbstractDeclarator(TokenStream& tokens);
    std::shared_ptr<ASTNode> parseParameterList(TokenStream& tokens);
    
    // Utilidades
    bool isFirstToken(const std::string& rule_name, TokenType token) const;
    std::set<TokenType> getFirstSet(const std::string& rule_name) const;
    std::set<TokenType> getFollowSet(const std::string& rule_name) const;
    
    // Configuração
    void setCVersion(CVersion version);
    CVersion getCVersion() const;
    void enableFeature(const std::string& feature);
    void disableFeature(const std::string& feature);
    bool isFeatureEnabled(const std::string& feature) const;
};
```

#### **error_recovery.hpp** - Sistema de Recuperação de Erros

```cpp
class ErrorRecovery {
public:
    // Estratégias de Recuperação
    enum class Strategy {
        PANIC_MODE,
        PHRASE_LEVEL,
        ERROR_PRODUCTIONS,
        GLOBAL_CORRECTION
    };
    
    // Construção
    ErrorRecovery();
    explicit ErrorRecovery(Strategy default_strategy);
    
    // Configuração
    void setStrategy(Strategy strategy);
    Strategy getStrategy() const;
    void setSynchronizationTokens(const std::set<TokenType>& tokens);
    void addSynchronizationToken(TokenType token);
    void removeSynchronizationToken(TokenType token);
    
    // Recuperação Principal
    RecoveryResult recover(const ParserError& error, TokenStream& tokens);
    bool canRecover(const ParserError& error, const TokenStream& tokens) const;
    
    // Estratégias Específicas
    RecoveryResult panicModeRecovery(const ParserError& error, TokenStream& tokens);
    RecoveryResult phraseLevelRecovery(const ParserError& error, TokenStream& tokens);
    RecoveryResult errorProductionRecovery(const ParserError& error, TokenStream& tokens);
    RecoveryResult globalCorrectionRecovery(const ParserError& error, TokenStream& tokens);
    
    // Análise de Erros
    std::vector<RecoveryAction> analyzeError(const ParserError& error, const TokenStream& tokens);
    std::string generateSuggestion(const ParserError& error, const TokenStream& tokens);
    
    // Estatísticas
    RecoveryStatistics getStatistics() const;
    void resetStatistics();
    
    // Configuração Avançada
    void setMaxRecoveryAttempts(size_t max_attempts);
    void setRecoveryTimeout(std::chrono::milliseconds timeout);
    void enableRecoveryLogging(bool enable);
};
```

#### **parser_config.hpp** - Configuração do Parser

```cpp
class ParserConfig {
public:
    // Construção
    ParserConfig();
    explicit ParserConfig(CVersion version);
    
    // Configuração de Versão
    void setCVersion(CVersion version);
    CVersion getCVersion() const;
    
    // Features de Linguagem
    void enableFeature(const std::string& feature);
    void disableFeature(const std::string& feature);
    bool isFeatureEnabled(const std::string& feature) const;
    std::set<std::string> getEnabledFeatures() const;
    
    // Configuração de Parsing
    void setMaxRecursionDepth(size_t depth);
    size_t getMaxRecursionDepth() const;
    void setMaxErrorCount(size_t count);
    size_t getMaxErrorCount() const;
    
    // Configuração de Recuperação
    void setErrorRecoveryStrategy(ErrorRecovery::Strategy strategy);
    ErrorRecovery::Strategy getErrorRecoveryStrategy() const;
    void setSynchronizationTokens(const std::set<TokenType>& tokens);
    std::set<TokenType> getSynchronizationTokens() const;
    
    // Configuração de Logging
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;
    void enableFileLogging(const std::string& filename);
    void disableFileLogging();
    void enableConsoleLogging(bool enable);
    bool isConsoleLoggingEnabled() const;
    
    // Configuração de Performance
    void enableMemoization(bool enable);
    bool isMemoizationEnabled() const;
    void setMemoryLimit(size_t limit_bytes);
    size_t getMemoryLimit() const;
    
    // Serialização
    std::string serialize() const;
    bool deserialize(const std::string& data);
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
    // Validação
    bool isValid() const;
    std::vector<std::string> validate() const;
};
```

#### **parser_state.hpp** - Estado do Parser

```cpp
class ParserState {
public:
    // Construção
    ParserState();
    explicit ParserState(const ParserConfig& config);
    
    // Controle de Estado
    void reset();
    void initialize(TokenStream& tokens);
    bool isInitialized() const;
    
    // Stack de Contextos
    void pushContext(const std::string& rule_name);
    void popContext();
    ParsingContext getCurrentContext() const;
    size_t getContextDepth() const;
    bool isInContext(const std::string& rule_name) const;
    
    // Controle de Tokens
    Token getCurrentToken() const;
    Token peekToken(size_t lookahead = 1) const;
    void consumeToken();
    void synchronizeToToken(TokenType token);
    bool isAtEnd() const;
    
    // Posicionamento
    SourcePosition getCurrentPosition() const;
    void updatePosition(const SourcePosition& pos);
    
    // Controle de Erros
    void reportError(const ParserError& error);
    void addDiagnostic(const Diagnostic& diagnostic);
    std::vector<Diagnostic> getDiagnostics() const;
    size_t getErrorCount() const;
    bool hasErrors() const;
    
    // Estatísticas
    void incrementNodeCount();
    void incrementRuleApplicationCount(const std::string& rule_name);
    ParserStatistics getStatistics() const;
    
    // Configuração
    void setConfig(const ParserConfig& config);
    ParserConfig getConfig() const;
    
    // Debug e Profiling
    void startRuleTiming(const std::string& rule_name);
    void endRuleTiming(const std::string& rule_name);
    std::map<std::string, double> getRuleTimings() const;
};
```

#### **parser_logger.hpp** - Sistema de Logging

```cpp
class ParserLogger {
public:
    // Construção
    ParserLogger();
    explicit ParserLogger(LogLevel level);
    
    // Configuração
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;
    void enableFileLogging(const std::string& filename);
    void disableFileLogging();
    void enableConsoleLogging(bool enable);
    void enableTimestamps(bool enable);
    
    // Logging Básico
    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);
    
    // Logging de Parsing
    void logRuleEntry(const std::string& rule_name, const SourcePosition& pos);
    void logRuleExit(const std::string& rule_name, bool success, double time_ms);
    void logTokenConsumption(const Token& token);
    void logASTNodeCreation(const ASTNode& node);
    void logErrorRecovery(const std::string& strategy, const ParserError& error);
    
    // Logging de Performance
    void logMemoryUsage(size_t bytes);
    void logParsingStatistics(const ParserStatistics& stats);
    
    // Controle de Sessão
    void startSession(const std::string& session_name);
    void endSession();
    void flush();
    
    // Utilidades
    std::string getLogFilename() const;
    size_t getLogSize() const;
    void clearLog();
};
```

#### **parser_types.hpp** - Tipos e Estruturas

```cpp
// Enumerações Principais
enum class CVersion {
    C89, C99, C11, C17, C23
};

enum class LogLevel {
    TRACE, DEBUG, INFO, WARNING, ERROR, FATAL
};

enum class DiagnosticLevel {
    NOTE, WARNING, ERROR, FATAL
};

// Estruturas de Dados
struct ParseResult {
    bool success;
    std::shared_ptr<AST> ast;
    std::vector<Diagnostic> diagnostics;
    ParserStatistics statistics;
    std::string error_summary;
};

struct ParsingContext {
    std::string rule_name;
    SourcePosition start_position;
    size_t token_index;
    std::vector<std::shared_ptr<ASTNode>> partial_nodes;
    std::map<std::string, std::any> attributes;
};

struct ParserError {
    std::string message;
    SourcePosition position;
    TokenType expected_token;
    TokenType actual_token;
    std::string context;
    std::vector<std::string> suggestions;
};

struct RecoveryResult {
    bool success;
    size_t tokens_consumed;
    std::string strategy_used;
    std::vector<std::string> actions_taken;
};

struct ParserStatistics {
    size_t nodes_created;
    size_t rules_applied;
    size_t errors_encountered;
    size_t recoveries_attempted;
    size_t recoveries_successful;
    double total_parsing_time_ms;
    size_t memory_used_bytes;
    std::map<std::string, size_t> rule_application_counts;
    std::map<std::string, double> rule_timing_ms;
};

// Interfaces
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(ASTNode& node) = 0;
    virtual bool shouldVisitChildren(const ASTNode& node) const { return true; }
};

class ASTTransformer {
public:
    virtual ~ASTTransformer() = default;
    virtual std::shared_ptr<ASTNode> transform(std::shared_ptr<ASTNode> node) = 0;
};

class TokenStream {
public:
    virtual ~TokenStream() = default;
    virtual Token peek(size_t lookahead = 0) const = 0;
    virtual Token consume() = 0;
    virtual bool isAtEnd() const = 0;
    virtual size_t getPosition() const = 0;
    virtual void setPosition(size_t pos) = 0;
};
```

### Implementações (.cpp)

#### **parser.cpp** - Implementação Principal

```cpp
// Métodos de Controle Principal
Parser::Parser();
Parser::Parser(const ParserConfig& config);
Parser::~Parser();

// Métodos de Parsing
ParseResult Parser::parse(TokenStream& tokens);
ParseResult Parser::parseString(const std::string& code);
ParseResult Parser::parseFile(const std::string& filename);

// Métodos de Configuração
void Parser::setConfig(const ParserConfig& config);
ParserConfig Parser::getConfig() const;
void Parser::setCVersion(CVersion version);

// Métodos de Acesso aos Resultados
std::shared_ptr<AST> Parser::getAST() const;
std::vector<Diagnostic> Parser::getDiagnostics() const;
ParserStatistics Parser::getStatistics() const;

// Métodos de Estado e Controle
void Parser::reset();
bool Parser::isInitialized() const;
void Parser::enableDebugMode(bool enable);

// Métodos de Integração
void Parser::setLexerInterface(std::shared_ptr<LexerInterface> lexer);
void Parser::setErrorHandler(std::shared_ptr<ErrorHandler> handler);

// Métodos Privados de Implementação
bool Parser::initializeParser(TokenStream& tokens);
void Parser::finalizeParser();
void Parser::handleParsingError(const ParserError& error);
std::shared_ptr<ASTNode> Parser::parseTopLevel(TokenStream& tokens);
```

#### **ast.cpp** - Implementação da AST

```cpp
// Métodos de ASTNode
ASTNode::ASTNode(NodeType type, const std::string& value);
std::shared_ptr<ASTNode> ASTNode::create(NodeType type, const std::string& value);

// Métodos de Propriedades
ASTNode::NodeType ASTNode::getType() const;
std::string ASTNode::getValue() const;
void ASTNode::setValue(const std::string& value);
SourcePosition ASTNode::getPosition() const;
void ASTNode::setPosition(const SourcePosition& pos);

// Métodos de Hierarquia
void ASTNode::addChild(std::shared_ptr<ASTNode> child);
void ASTNode::insertChild(size_t index, std::shared_ptr<ASTNode> child);
void ASTNode::removeChild(size_t index);
std::shared_ptr<ASTNode> ASTNode::getChild(size_t index) const;
size_t ASTNode::getChildCount() const;
std::shared_ptr<ASTNode> ASTNode::getParent() const;

// Métodos de Navegação
std::shared_ptr<ASTNode> ASTNode::getFirstChild() const;
std::shared_ptr<ASTNode> ASTNode::getLastChild() const;
std::shared_ptr<ASTNode> ASTNode::getNextSibling() const;
std::shared_ptr<ASTNode> ASTNode::getPreviousSibling() const;

// Métodos de Atributos
void ASTNode::setAttribute(const std::string& key, const std::any& value);
std::any ASTNode::getAttribute(const std::string& key) const;
bool ASTNode::hasAttribute(const std::string& key) const;
void ASTNode::removeAttribute(const std::string& key);

// Métodos de Utilidades
std::string ASTNode::toString() const;
std::string ASTNode::toXML() const;
std::string ASTNode::toJSON() const;
void ASTNode::accept(ASTVisitor& visitor);
std::shared_ptr<ASTNode> ASTNode::clone() const;

// Métodos de Validação
bool ASTNode::isValid() const;
std::vector<std::string> ASTNode::validate() const;

// Métodos de AST
AST::AST();
AST::AST(std::shared_ptr<ASTNode> root);

// Métodos de Acesso à Raiz
std::shared_ptr<ASTNode> AST::getRoot() const;
void AST::setRoot(std::shared_ptr<ASTNode> root);

// Métodos de Navegação Global
std::vector<std::shared_ptr<ASTNode>> AST::findNodesByType(ASTNode::NodeType type) const;
std::vector<std::shared_ptr<ASTNode>> AST::findNodesByValue(const std::string& value) const;
std::shared_ptr<ASTNode> AST::findFirstNode(std::function<bool(const ASTNode&)> predicate) const;

// Métodos de Transformações
void AST::transform(ASTTransformer& transformer);
std::shared_ptr<AST> AST::clone() const;

// Métodos de Serialização
std::string AST::serialize() const;
bool AST::deserialize(const std::string& data);

// Métodos de Estatísticas
size_t AST::getNodeCount() const;
size_t AST::getDepth() const;
std::map<ASTNode::NodeType, size_t> AST::getNodeTypeStatistics() const;

// Métodos de Validação
bool AST::isValid() const;
std::vector<std::string> AST::validate() const;
```

#### **grammar.cpp** - Implementação da Gramática

```cpp
// Métodos de Construção
Grammar::Grammar();
Grammar::Grammar(CVersion version);

// Métodos de Carregamento
bool Grammar::loadFromFile(const std::string& filename);
bool Grammar::loadFromString(const std::string& grammar_text);
void Grammar::addRule(const std::string& rule_name, const ProductionRule& rule);

// Métodos de Parsing Principal
std::shared_ptr<ASTNode> Grammar::parseProgram(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseDeclaration(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseStatement(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseExpression(TokenStream& tokens);

// Métodos de Parsing de Declarações
std::shared_ptr<ASTNode> Grammar::parseFunctionDeclaration(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseVariableDeclaration(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseStructDeclaration(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseUnionDeclaration(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseEnumDeclaration(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseTypedefDeclaration(TokenStream& tokens);

// Métodos de Parsing de Statements
std::shared_ptr<ASTNode> Grammar::parseCompoundStatement(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseIfStatement(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseWhileStatement(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseForStatement(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseSwitchStatement(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseReturnStatement(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseBreakStatement(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseContinueStatement(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseGotoStatement(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseLabelStatement(TokenStream& tokens);

// Métodos de Parsing de Expressões
std::shared_ptr<ASTNode> Grammar::parseAssignmentExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseConditionalExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseLogicalOrExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseLogicalAndExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseInclusiveOrExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseExclusiveOrExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseAndExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseEqualityExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseRelationalExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseShiftExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseAdditiveExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseMultiplicativeExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseCastExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseUnaryExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parsePostfixExpression(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parsePrimaryExpression(TokenStream& tokens);

// Métodos de Parsing de Tipos
std::shared_ptr<ASTNode> Grammar::parseTypeSpecifier(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseDeclarator(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseAbstractDeclarator(TokenStream& tokens);
std::shared_ptr<ASTNode> Grammar::parseParameterList(TokenStream& tokens);

// Métodos de Utilidades
bool Grammar::isFirstToken(const std::string& rule_name, TokenType token) const;
std::set<TokenType> Grammar::getFirstSet(const std::string& rule_name) const;
std::set<TokenType> Grammar::getFollowSet(const std::string& rule_name) const;

// Métodos de Configuração
void Grammar::setCVersion(CVersion version);
CVersion Grammar::getCVersion() const;
void Grammar::enableFeature(const std::string& feature);
void Grammar::disableFeature(const std::string& feature);
bool Grammar::isFeatureEnabled(const std::string& feature) const;
```

#### **error_recovery.cpp** - Implementação da Recuperação de Erros

```cpp
// Métodos de Construção
ErrorRecovery::ErrorRecovery();
ErrorRecovery::ErrorRecovery(Strategy default_strategy);

// Métodos de Configuração
void ErrorRecovery::setStrategy(Strategy strategy);
ErrorRecovery::Strategy ErrorRecovery::getStrategy() const;
void ErrorRecovery::setSynchronizationTokens(const std::set<TokenType>& tokens);
void ErrorRecovery::addSynchronizationToken(TokenType token);
void ErrorRecovery::removeSynchronizationToken(TokenType token);

// Métodos de Recuperação Principal
RecoveryResult ErrorRecovery::recover(const ParserError& error, TokenStream& tokens);
bool ErrorRecovery::canRecover(const ParserError& error, const TokenStream& tokens) const;

// Métodos de Estratégias Específicas
RecoveryResult ErrorRecovery::panicModeRecovery(const ParserError& error, TokenStream& tokens);
RecoveryResult ErrorRecovery::phraseLevelRecovery(const ParserError& error, TokenStream& tokens);
RecoveryResult ErrorRecovery::errorProductionRecovery(const ParserError& error, TokenStream& tokens);
RecoveryResult ErrorRecovery::globalCorrectionRecovery(const ParserError& error, TokenStream& tokens);

// Métodos de Análise de Erros
std::vector<RecoveryAction> ErrorRecovery::analyzeError(const ParserError& error, const TokenStream& tokens);
std::string ErrorRecovery::generateSuggestion(const ParserError& error, const TokenStream& tokens);

// Métodos de Estatísticas
RecoveryStatistics ErrorRecovery::getStatistics() const;
void ErrorRecovery::resetStatistics();

// Métodos de Configuração Avançada
void ErrorRecovery::setMaxRecoveryAttempts(size_t max_attempts);
void ErrorRecovery::setRecoveryTimeout(std::chrono::milliseconds timeout);
void ErrorRecovery::enableRecoveryLogging(bool enable);
```

#### **parser_config.cpp** - Implementação da Configuração

```cpp
// Métodos de Construção
ParserConfig::ParserConfig();
ParserConfig::ParserConfig(CVersion version);

// Métodos de Configuração de Versão
void ParserConfig::setCVersion(CVersion version);
CVersion ParserConfig::getCVersion() const;

// Métodos de Features de Linguagem
void ParserConfig::enableFeature(const std::string& feature);
void ParserConfig::disableFeature(const std::string& feature);
bool ParserConfig::isFeatureEnabled(const std::string& feature) const;
std::set<std::string> ParserConfig::getEnabledFeatures() const;

// Métodos de Configuração de Parsing
void ParserConfig::setMaxRecursionDepth(size_t depth);
size_t ParserConfig::getMaxRecursionDepth() const;
void ParserConfig::setMaxErrorCount(size_t count);
size_t ParserConfig::getMaxErrorCount() const;

// Métodos de Configuração de Recuperação
void ParserConfig::setErrorRecoveryStrategy(ErrorRecovery::Strategy strategy);
ErrorRecovery::Strategy ParserConfig::getErrorRecoveryStrategy() const;
void ParserConfig::setSynchronizationTokens(const std::set<TokenType>& tokens);
std::set<TokenType> ParserConfig::getSynchronizationTokens() const;

// Métodos de Configuração de Logging
void ParserConfig::setLogLevel(LogLevel level);
LogLevel ParserConfig::getLogLevel() const;
void ParserConfig::enableFileLogging(const std::string& filename);
void ParserConfig::disableFileLogging();
void ParserConfig::enableConsoleLogging(bool enable);
bool ParserConfig::isConsoleLoggingEnabled() const;

// Métodos de Configuração de Performance
void ParserConfig::enableMemoization(bool enable);
bool ParserConfig::isMemoizationEnabled() const;
void ParserConfig::setMemoryLimit(size_t limit_bytes);
size_t ParserConfig::getMemoryLimit() const;

// Métodos de Serialização
std::string ParserConfig::serialize() const;
bool ParserConfig::deserialize(const std::string& data);
bool ParserConfig::loadFromFile(const std::string& filename);
bool ParserConfig::saveToFile(const std::string& filename) const;

// Métodos de Validação
bool ParserConfig::isValid() const;
std::vector<std::string> ParserConfig::validate() const;
```

#### **parser_state.cpp** - Implementação do Estado

```cpp
// Métodos de Construção
ParserState::ParserState();
ParserState::ParserState(const ParserConfig& config);

// Métodos de Controle de Estado
void ParserState::reset();
void ParserState::initialize(TokenStream& tokens);
bool ParserState::isInitialized() const;

// Métodos de Stack de Contextos
void ParserState::pushContext(const std::string& rule_name);
void ParserState::popContext();
ParsingContext ParserState::getCurrentContext() const;
size_t ParserState::getContextDepth() const;
bool ParserState::isInContext(const std::string& rule_name) const;

// Métodos de Controle de Tokens
Token ParserState::getCurrentToken() const;
Token ParserState::peekToken(size_t lookahead) const;
void ParserState::consumeToken();
void ParserState::synchronizeToToken(TokenType token);
bool ParserState::isAtEnd() const;

// Métodos de Posicionamento
SourcePosition ParserState::getCurrentPosition() const;
void ParserState::updatePosition(const SourcePosition& pos);

// Métodos de Controle de Erros
void ParserState::reportError(const ParserError& error);
void ParserState::addDiagnostic(const Diagnostic& diagnostic);
std::vector<Diagnostic> ParserState::getDiagnostics() const;
size_t ParserState::getErrorCount() const;
bool ParserState::hasErrors() const;

// Métodos de Estatísticas
void ParserState::incrementNodeCount();
void ParserState::incrementRuleApplicationCount(const std::string& rule_name);
ParserStatistics ParserState::getStatistics() const;

// Métodos de Configuração
void ParserState::setConfig(const ParserConfig& config);
ParserConfig ParserState::getConfig() const;

// Métodos de Debug e Profiling
void ParserState::startRuleTiming(const std::string& rule_name);
void ParserState::endRuleTiming(const std::string& rule_name);
std::map<std::string, double> ParserState::getRuleTimings() const;
```

#### **parser_logger.cpp** - Implementação do Logging

```cpp
// Métodos de Construção
ParserLogger::ParserLogger();
ParserLogger::ParserLogger(LogLevel level);

// Métodos de Configuração
void ParserLogger::setLogLevel(LogLevel level);
LogLevel ParserLogger::getLogLevel() const;
void ParserLogger::enableFileLogging(const std::string& filename);
void ParserLogger::disableFileLogging();
void ParserLogger::enableConsoleLogging(bool enable);
void ParserLogger::enableTimestamps(bool enable);

// Métodos de Logging Básico
void ParserLogger::trace(const std::string& message);
void ParserLogger::debug(const std::string& message);
void ParserLogger::info(const std::string& message);
void ParserLogger::warning(const std::string& message);
void ParserLogger::error(const std::string& message);
void ParserLogger::fatal(const std::string& message);

// Métodos de Logging de Parsing
void ParserLogger::logRuleEntry(const std::string& rule_name, const SourcePosition& pos);
void ParserLogger::logRuleExit(const std::string& rule_name, bool success, double time_ms);
void ParserLogger::logTokenConsumption(const Token& token);
void ParserLogger::logASTNodeCreation(const ASTNode& node);
void ParserLogger::logErrorRecovery(const std::string& strategy, const ParserError& error);

// Métodos de Logging de Performance
void ParserLogger::logMemoryUsage(size_t bytes);
void ParserLogger::logParsingStatistics(const ParserStatistics& stats);

// Métodos de Controle de Sessão
void ParserLogger::startSession(const std::string& session_name);
void ParserLogger::endSession();
void ParserLogger::flush();

// Métodos de Utilidades
std::string ParserLogger::getLogFilename() const;
size_t ParserLogger::getLogSize() const;
void ParserLogger::clearLog();
```

### Arquivos de Dados (data/)

#### **grammar/c89_grammar.txt** - Gramática C89
```
# Gramática C89 (ANSI C)
# Programa principal
program -> translation_unit

# Unidade de tradução
translation_unit -> external_declaration
                 | translation_unit external_declaration

# Declarações externas
external_declaration -> function_definition
                     | declaration

# Definição de função
function_definition -> declaration_specifiers declarator declaration_list compound_statement
                    | declaration_specifiers declarator compound_statement
                    | declarator declaration_list compound_statement
                    | declarator compound_statement

# Especificadores de declaração
declaration_specifiers -> storage_class_specifier
                       | storage_class_specifier declaration_specifiers
                       | type_specifier
                       | type_specifier declaration_specifiers
                       | type_qualifier
                       | type_qualifier declaration_specifiers

# Especificadores de classe de armazenamento
storage_class_specifier -> 'auto' | 'register' | 'static' | 'extern' | 'typedef'

# Especificadores de tipo
type_specifier -> 'void' | 'char' | 'short' | 'int' | 'long' | 'float' | 'double'
               | 'signed' | 'unsigned'
               | struct_or_union_specifier
               | enum_specifier
               | typedef_name

# Qualificadores de tipo
type_qualifier -> 'const' | 'volatile'

# Declarações
declaration -> declaration_specifiers ';'
            | declaration_specifiers init_declarator_list ';'

# Lista de declaradores com inicialização
init_declarator_list -> init_declarator
                     | init_declarator_list ',' init_declarator

# Declarador com inicialização
init_declarator -> declarator
                | declarator '=' initializer

# Statements
statement -> labeled_statement
          | compound_statement
          | expression_statement
          | selection_statement
          | iteration_statement
          | jump_statement

# Statement composto
compound_statement -> '{' '}'
                   | '{' statement_list '}'
                   | '{' declaration_list '}'
                   | '{' declaration_list statement_list '}'

# Expressões
expression -> assignment_expression
           | expression ',' assignment_expression

# Expressão de atribuição
assignment_expression -> conditional_expression
                      | unary_expression assignment_operator assignment_expression

# Operadores de atribuição
assignment_operator -> '=' | '*=' | '/=' | '%=' | '+=' | '-=' | '<<=' | '>>=' | '&=' | '^=' | '|='

# Expressão condicional
conditional_expression -> logical_or_expression
                        | logical_or_expression '?' expression ':' conditional_expression

# Expressões lógicas e aritméticas (por precedência)
logical_or_expression -> logical_and_expression
                      | logical_or_expression '||' logical_and_expression

logical_and_expression -> inclusive_or_expression
                       | logical_and_expression '&&' inclusive_or_expression

inclusive_or_expression -> exclusive_or_expression
                        | inclusive_or_expression '|' exclusive_or_expression

exclusive_or_expression -> and_expression
                        | exclusive_or_expression '^' and_expression

and_expression -> equality_expression
               | and_expression '&' equality_expression

equality_expression -> relational_expression
                    | equality_expression '==' relational_expression
                    | equality_expression '!=' relational_expression

relational_expression -> shift_expression
                      | relational_expression '<' shift_expression
                      | relational_expression '>' shift_expression
                      | relational_expression '<=' shift_expression
                      | relational_expression '>=' shift_expression

shift_expression -> additive_expression
                 | shift_expression '<<' additive_expression
                 | shift_expression '>>' additive_expression

additive_expression -> multiplicative_expression
                    | additive_expression '+' multiplicative_expression
                    | additive_expression '-' multiplicative_expression

multiplicative_expression -> cast_expression
                          | multiplicative_expression '*' cast_expression
                          | multiplicative_expression '/' cast_expression
                          | multiplicative_expression '%' cast_expression

cast_expression -> unary_expression
                | '(' type_name ')' cast_expression

unary_expression -> postfix_expression
                 | '++' unary_expression
                 | '--' unary_expression
                 | unary_operator cast_expression
                 | 'sizeof' unary_expression
                 | 'sizeof' '(' type_name ')'

unary_operator -> '&' | '*' | '+' | '-' | '~' | '!'

postfix_expression -> primary_expression
                   | postfix_expression '[' expression ']'
                   | postfix_expression '(' ')'
                   | postfix_expression '(' argument_expression_list ')'
                   | postfix_expression '.' identifier
                   | postfix_expression '->' identifier
                   | postfix_expression '++'
                   | postfix_expression '--'

primary_expression -> identifier
                   | constant
                   | string_literal
                   | '(' expression ')'

# Constantes
constant -> integer_constant
         | character_constant
         | floating_constant
         | enumeration_constant
```

#### **precedence.txt** - Tabela de Precedência
```
# Tabela de Precedência de Operadores C
# Formato: operador:precedencia:associatividade
# Precedência: 1 (menor) a 15 (maior)
# Associatividade: L (esquerda), R (direita), N (não-associativo)

# Precedência 1 (menor)
,:1:L

# Precedência 2
=:2:R
*=:2:R
/=:2:R
%=:2:R
+=:2:R
-=:2:R
<<=:2:R
>>=:2:R
&=:2:R
^=:2:R
|=:2:R

# Precedência 3
?::3:R

# Precedência 4
||:4:L

# Precedência 5
&&:5:L

# Precedência 6
|:6:L

# Precedência 7
^:7:L

# Precedência 8
&:8:L

# Precedência 9
==:9:L
!=:9:L

# Precedência 10
<:10:L
>:10:L
<=:10:L
>=:10:L

# Precedência 11
<<:11:L
>>:11:L

# Precedência 12
+:12:L
-:12:L

# Precedência 13
*:13:L
/:13:L
%:13:L

# Precedência 14
++:14:R
--:14:R
+:14:R
-:14:R
!:14:R
~:14:R
*:14:R
&:14:R
sizeof:14:R

# Precedência 15 (maior)
():15:L
[]:15:L
.:15:L
->:15:L
++:15:L
--:15:L
```

#### **error_messages.txt** - Mensagens de Erro
```
# Mensagens de Erro do Parser
# Formato: codigo_erro:mensagem:sugestao

# Erros de sintaxe básica
E001:Expected ';' after statement:Add a semicolon at the end of the statement
E002:Expected '{' to begin compound statement:Add an opening brace
E003:Expected '}' to end compound statement:Add a closing brace
E004:Expected '(' after function name:Add opening parenthesis
E005:Expected ')' to close parameter list:Add closing parenthesis

# Erros de declaração
E010:Expected identifier in declaration:Provide a valid identifier name
E011:Expected type specifier:Add a type like 'int', 'char', etc.
E012:Missing declarator:Add a variable or function name
E013:Invalid storage class specifier:Use 'auto', 'register', 'static', 'extern', or 'typedef'

# Erros de expressão
E020:Expected expression:Provide a valid expression
E021:Invalid assignment target:Left side must be a modifiable lvalue
E022:Mismatched parentheses:Check parentheses balance
E023:Invalid operator usage:Check operator syntax and operands

# Erros de função
E030:Expected function body:Add compound statement after function header
E031:Invalid parameter declaration:Check parameter syntax
E032:Missing return statement:Add 'return' statement in non-void function

# Erros de estrutura
E040:Expected member declaration:Add valid member declaration
E041:Invalid struct/union syntax:Check struct/union declaration syntax
E042:Missing tag name:Provide a name for struct/union/enum

# Erros de controle de fluxo
E050:Expected condition in if statement:Add condition in parentheses
E051:Expected condition in while statement:Add condition in parentheses
E052:Invalid for loop syntax:Check for loop structure
E053:Invalid switch statement:Check switch syntax
E054:Invalid case label:Case must be in switch statement

# Erros de recuperação
E100:Unexpected token:Check syntax around this location
E101:Parse error - attempting recovery:Syntax error detected, trying to continue
E102:Too many syntax errors:Fix previous errors first
E103:Parser stack overflow:Expression or statement too complex
```

### Arquivos de Teste

#### **tests/unit/test_parser.cpp** - Testes do Parser Principal

```cpp
#include <gtest/gtest.h>
#include "parser.hpp"
#include "test_utils.hpp"

class ParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        config.setCVersion(CVersion::C99);
        parser.setConfig(config);
    }
    
    Parser parser;
    ParserConfig config;
};

// Testes básicos de parsing
TEST_F(ParserTest, ParseEmptyProgram) {
    std::string code = "";
    auto result = parser.parseString(code);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.diagnostics.size(), 0);
}

TEST_F(ParserTest, ParseSimpleFunction) {
    std::string code = "int main() { return 0; }";
    auto result = parser.parseString(code);
    EXPECT_TRUE(result.success);
    ASSERT_NE(result.ast, nullptr);
    
    auto root = result.ast->getRoot();
    EXPECT_EQ(root->getType(), ASTNode::NodeType::PROGRAM);
    EXPECT_EQ(root->getChildCount(), 1);
    
    auto func = root->getChild(0);
    EXPECT_EQ(func->getType(), ASTNode::NodeType::FUNCTION_DECLARATION);
}

TEST_F(ParserTest, ParseVariableDeclaration) {
    std::string code = "int x = 10;";
    auto result = parser.parseString(code);
    EXPECT_TRUE(result.success);
    
    auto root = result.ast->getRoot();
    auto decl = root->getChild(0);
    EXPECT_EQ(decl->getType(), ASTNode::NodeType::VARIABLE_DECLARATION);
}

TEST_F(ParserTest, ParseComplexExpression) {
    std::string code = "int result = a + b * c - d / e;";
    auto result = parser.parseString(code);
    EXPECT_TRUE(result.success);
    
    // Verificar precedência correta na AST
    auto root = result.ast->getRoot();
    auto decl = root->getChild(0);
    auto init = decl->getChild(1); // Inicializador
    
    // Deve ser: a + (b * c) - (d / e)
    EXPECT_EQ(init->getType(), ASTNode::NodeType::BINARY_EXPRESSION);
    EXPECT_EQ(init->getValue(), "-");
}

// Testes de recuperação de erros
TEST_F(ParserTest, RecoverFromMissingSemicolon) {
    std::string code = "int x = 10\nint y = 20;";
    auto result = parser.parseString(code);
    
    EXPECT_FALSE(result.success);
    EXPECT_GT(result.diagnostics.size(), 0);
    
    // Deve ter recuperado e continuado parsing
    auto root = result.ast->getRoot();
    EXPECT_EQ(root->getChildCount(), 2); // Duas declarações
}

TEST_F(ParserTest, RecoverFromMissingBrace) {
    std::string code = "int main() { int x = 10; return 0;";
    auto result = parser.parseString(code);
    
    EXPECT_FALSE(result.success);
    EXPECT_GT(result.diagnostics.size(), 0);
    
    // Verificar se a mensagem de erro é apropriada
    bool found_brace_error = false;
    for (const auto& diag : result.diagnostics) {
        if (diag.message.find("Expected '}'") != std::string::npos) {
            found_brace_error = true;
            break;
        }
    }
    EXPECT_TRUE(found_brace_error);
}

// Testes de performance
TEST_F(ParserTest, ParseLargeFile) {
    std::string large_code = generateLargeCode(1000); // 1000 linhas
    
    auto start = std::chrono::high_resolution_clock::now();
    auto result = parser.parseString(large_code);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_TRUE(result.success);
    EXPECT_LT(duration.count(), 1000); // Menos de 1 segundo
}

// Testes de versões do C
TEST_F(ParserTest, ParseC99Features) {
    config.setCVersion(CVersion::C99);
    parser.setConfig(config);
    
    std::string code = "int arr[] = {[0] = 1, [2] = 3};"; // Designated initializers
    auto result = parser.parseString(code);
    EXPECT_TRUE(result.success);
}

TEST_F(ParserTest, RejectC99FeaturesInC89) {
    config.setCVersion(CVersion::C89);
    parser.setConfig(config);
    
    std::string code = "int arr[] = {[0] = 1, [2] = 3};"; // Designated initializers
    auto result = parser.parseString(code);
    EXPECT_FALSE(result.success);
}
```

#### **tests/unit/test_ast.cpp** - Testes da AST

```cpp
#include <gtest/gtest.h>
#include "ast.hpp"
#include "ast_visitor.hpp"

class ASTTest : public ::testing::Test {
protected:
    void SetUp() override {
        ast = std::make_unique<AST>();
    }
    
    std::unique_ptr<AST> ast;
};

TEST_F(ASTTest, CreateEmptyAST) {
    EXPECT_EQ(ast->getRoot(), nullptr);
    EXPECT_EQ(ast->getNodeCount(), 0);
}

TEST_F(ASTTest, CreateProgramNode) {
    auto program = ast->createNode(ASTNode::NodeType::PROGRAM);
    ast->setRoot(program);
    
    EXPECT_NE(ast->getRoot(), nullptr);
    EXPECT_EQ(ast->getRoot()->getType(), ASTNode::NodeType::PROGRAM);
    EXPECT_EQ(ast->getNodeCount(), 1);
}

TEST_F(ASTTest, AddChildNodes) {
    auto program = ast->createNode(ASTNode::NodeType::PROGRAM);
    auto func = ast->createNode(ASTNode::NodeType::FUNCTION_DECLARATION);
    
    program->addChild(func);
    ast->setRoot(program);
    
    EXPECT_EQ(program->getChildCount(), 1);
    EXPECT_EQ(program->getChild(0), func);
    EXPECT_EQ(func->getParent(), program);
}

TEST_F(ASTTest, VisitorPattern) {
    // Criar uma AST simples
    auto program = ast->createNode(ASTNode::NodeType::PROGRAM);
    auto func = ast->createNode(ASTNode::NodeType::FUNCTION_DECLARATION);
    auto stmt = ast->createNode(ASTNode::NodeType::RETURN_STATEMENT);
    
    program->addChild(func);
    func->addChild(stmt);
    ast->setRoot(program);
    
    // Testar visitor
    class CountingVisitor : public ASTVisitor {
    public:
        int count = 0;
        void visit(ASTNode* node) override {
            count++;
            ASTVisitor::visit(node); // Visitar filhos
        }
    };
    
    CountingVisitor visitor;
    ast->accept(&visitor);
    
    EXPECT_EQ(visitor.count, 3); // program + func + stmt
}
```

#### **tests/integration/test_c_programs.cpp** - Testes de Integração

```cpp
#include <gtest/gtest.h>
#include "parser.hpp"
#include "lexer.hpp"
#include "preprocessor.hpp"
#include <fstream>
#include <sstream>

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config.setCVersion(CVersion::C99);
        parser.setConfig(config);
    }
    
    std::string loadTestFile(const std::string& filename) {
        std::ifstream file("test_data/" + filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    Parser parser;
    ParserConfig config;
};

TEST_F(IntegrationTest, ParseHelloWorld) {
    std::string code = R"(
        #include <stdio.h>
        
        int main() {
            printf("Hello, World!\n");
            return 0;
        }
    )";
    
    auto result = parser.parseString(code);
    EXPECT_TRUE(result.success);
    
    auto root = result.ast->getRoot();
    EXPECT_EQ(root->getType(), ASTNode::NodeType::PROGRAM);
    
    // Deve ter uma função main
    bool found_main = false;
    for (size_t i = 0; i < root->getChildCount(); i++) {
        auto child = root->getChild(i);
        if (child->getType() == ASTNode::NodeType::FUNCTION_DECLARATION) {
            if (child->getValue() == "main") {
                found_main = true;
                break;
            }
        }
    }
    EXPECT_TRUE(found_main);
}

TEST_F(IntegrationTest, ParseComplexProgram) {
    std::string code = loadTestFile("complex_program.c");
    
    auto start = std::chrono::high_resolution_clock::now();
    auto result = parser.parseString(code);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_TRUE(result.success);
    EXPECT_LT(duration.count(), 5000); // Menos de 5 segundos
    
    // Verificar estrutura da AST
    auto root = result.ast->getRoot();
    EXPECT_GT(root->getChildCount(), 0);
}

TEST_F(IntegrationTest, ParseWithPreprocessor) {
    std::string code = R"(
        #define MAX_SIZE 100
        #define SQUARE(x) ((x) * (x))
        
        int main() {
            int arr[MAX_SIZE];
            int result = SQUARE(5);
            return result;
        }
    )";
    
    // Simular integração com preprocessador
    Preprocessor preprocessor;
    auto preprocessed = preprocessor.process(code);
    
    auto result = parser.parseString(preprocessed.code);
    EXPECT_TRUE(result.success);
}
```

## Próximos Passos e Cronograma de Implementação

### Fases de Implementação Detalhadas

#### **Fase 1: Estruturas Fundamentais (Semanas 1-2)**

##### **1.1 Configuração do Projeto**
- ✅ Criar estrutura de diretórios (`src/parser/`, `include/`, `data/`, `tests/`)
- ✅ Configurar CMakeLists.txt para o módulo parser
- ✅ Configurar dependências (Google Test, bibliotecas do projeto)
- ✅ Criar arquivos de configuração base

##### **1.2 Estruturas de Dados Base**
- ⏳ **ASTNode** (`ast_node.hpp/cpp`)
  - Implementar construtores e destrutor
  - Implementar `getType()`, `getValue()`, `setValue()`
  - Implementar `getParent()`, `setParent()`
  - Implementar `addChild()`, `removeChild()`, `getChild()`, `getChildCount()`
  - Implementar `getSourcePosition()`, `setSourcePosition()`
  - Implementar `accept()` para padrão Visitor
  - Implementar `clone()` e `toString()`

- ⏳ **AST** (`ast.hpp/cpp`)
  - Implementar construtores e destrutor
  - Implementar `createNode()`, `deleteNode()`
  - Implementar `getRoot()`, `setRoot()`
  - Implementar `getNodeCount()`, `clear()`
  - Implementar `accept()` para visitor
  - Implementar `serialize()`, `deserialize()`
  - Implementar `validate()` e `getStatistics()`

##### **1.3 Configuração e Logging**
- ⏳ **ParserConfig** (`parser_config.hpp/cpp`)
  - Implementar construtores e métodos de versão C
  - Implementar configuração de features de linguagem
  - Implementar configuração de parsing e recuperação
  - Implementar configuração de logging e performance
  - Implementar serialização e validação

- ⏳ **ParserLogger** (`parser_logger.hpp/cpp`)
  - Implementar construtores e configuração de nível
  - Implementar logging básico (trace, debug, info, warning, error, fatal)
  - Implementar logging específico de parsing
  - Implementar controle de sessão e utilidades

##### **1.4 Testes Básicos**
- ⏳ Criar `test_ast_node.cpp` com testes de criação e manipulação
- ⏳ Criar `test_ast.cpp` com testes de árvore completa
- ⏳ Criar `test_parser_config.cpp` com testes de configuração
- ⏳ Configurar framework de testes e CI básico

#### **Fase 2: Componentes de Suporte (Semanas 3-4)**

##### **2.1 Sistema de Gramática**
- ⏳ **Grammar** (`grammar.hpp/cpp`)
  - Implementar construtores e carregamento de arquivos
  - Implementar `loadFromFile()`, `loadFromString()`
  - Implementar `getRule()`, `getRules()`, `hasRule()`
  - Implementar `addRule()`, `removeRule()`, `clear()`
  - Implementar `validate()` e `getStatistics()`
  - Implementar `serialize()` e `toString()`

- ⏳ **Arquivos de Gramática**
  - Criar `c89_grammar.txt` com regras completas
  - Criar `c99_grammar.txt` com extensões
  - Criar `c11_grammar.txt`, `c17_grammar.txt`, `c23_grammar.txt`
  - Criar `precedence.txt` com tabela de precedência

##### **2.2 Estado do Parser**
- ⏳ **ParserState** (`parser_state.hpp/cpp`)
  - Implementar construtores e controle de estado
  - Implementar stack de contextos (`pushContext()`, `popContext()`)
  - Implementar controle de tokens (`getCurrentToken()`, `peekToken()`, `consumeToken()`)
  - Implementar controle de posicionamento
  - Implementar controle de erros e estatísticas
  - Implementar debug e profiling

##### **2.3 Recuperação de Erros**
- ⏳ **ErrorRecovery** (`error_recovery.hpp/cpp`)
  - Implementar construtores e configuração
  - Implementar `recoverFromError()` principal
  - Implementar estratégias específicas:
    - `synchronizeToToken()`, `skipToSemicolon()`
    - `recoverFromMissingToken()`, `recoverFromUnexpectedToken()`
    - `recoverFromMissingBrace()`, `recoverFromExpressionError()`
  - Implementar análise de erros e estatísticas

##### **2.4 Testes de Componentes**
- ⏳ Criar `test_grammar.cpp` com testes de carregamento e validação
- ⏳ Criar `test_parser_state.cpp` com testes de estado
- ⏳ Criar `test_error_recovery.cpp` com testes de recuperação
- ⏳ Testes de integração entre componentes

#### **Fase 3: Parser Principal (Semanas 5-7)**

##### **3.1 Classe Parser Base**
- ⏳ **Parser** (`parser.hpp/cpp`) - Métodos de Construção
  - Implementar construtores e destrutor
  - Implementar `setConfig()`, `getConfig()`
  - Implementar `setGrammar()`, `getGrammar()`

##### **3.2 Métodos de Parsing Principal**
- ⏳ **Parser** - Métodos de Parsing
  - Implementar `parseFile()`, `parseString()`, `parseTokens()`
  - Implementar `reset()`, `initialize()`
  - Implementar `getCurrentToken()`, `peekToken()`, `consumeToken()`
  - Implementar `synchronize()`, `isAtEnd()`

##### **3.3 Parsing de Declarações**
- ⏳ **Parser** - Declarações
  - Implementar `parseDeclaration()`
  - Implementar `parseFunctionDefinition()`
  - Implementar `parseVariableDeclaration()`
  - Implementar `parseTypeDeclaration()`
  - Implementar `parseStructDeclaration()`, `parseUnionDeclaration()`
  - Implementar `parseEnumDeclaration()`
  - Implementar `parseTypedefDeclaration()`

##### **3.4 Parsing de Statements**
- ⏳ **Parser** - Statements
  - Implementar `parseStatement()`
  - Implementar `parseCompoundStatement()`
  - Implementar `parseExpressionStatement()`
  - Implementar `parseIfStatement()`, `parseWhileStatement()`
  - Implementar `parseForStatement()`, `parseDoWhileStatement()`
  - Implementar `parseSwitchStatement()`, `parseCaseStatement()`
  - Implementar `parseBreakStatement()`, `parseContinueStatement()`
  - Implementar `parseReturnStatement()`, `parseGotoStatement()`
  - Implementar `parseLabeledStatement()`

##### **3.5 Parsing de Expressões**
- ⏳ **Parser** - Expressões
  - Implementar `parseExpression()`
  - Implementar `parseAssignmentExpression()`
  - Implementar `parseConditionalExpression()`
  - Implementar `parseLogicalOrExpression()`, `parseLogicalAndExpression()`
  - Implementar `parseInclusiveOrExpression()`, `parseExclusiveOrExpression()`
  - Implementar `parseAndExpression()`, `parseEqualityExpression()`
  - Implementar `parseRelationalExpression()`, `parseShiftExpression()`
  - Implementar `parseAdditiveExpression()`, `parseMultiplicativeExpression()`
  - Implementar `parseCastExpression()`, `parseUnaryExpression()`
  - Implementar `parsePostfixExpression()`, `parsePrimaryExpression()`

##### **3.6 Parsing de Tipos**
- ⏳ **Parser** - Tipos
  - Implementar `parseType()`
  - Implementar `parseDeclarationSpecifiers()`
  - Implementar `parseDeclarator()`, `parseDirectDeclarator()`
  - Implementar `parseAbstractDeclarator()`
  - Implementar `parseParameterList()`, `parseParameterDeclaration()`
  - Implementar `parseInitializer()`, `parseInitializerList()`

##### **3.7 Testes de Parser**
- ⏳ Criar `test_parser_declarations.cpp`
- ⏳ Criar `test_parser_statements.cpp`
- ⏳ Criar `test_parser_expressions.cpp`
- ⏳ Criar `test_parser_types.cpp`
- ⏳ Testes de precedência e associatividade

#### **Fase 4: Funcionalidades Avançadas (Semanas 8-9)**

##### **4.1 Suporte a Versões do C**
- ⏳ **Parser** - Configuração de Versões
  - Implementar `enableC99Features()`, `enableC11Features()`
  - Implementar `enableC17Features()`, `enableC23Features()`
  - Implementar verificação de compatibilidade por versão
  - Implementar parsing condicional baseado em versão

##### **4.2 Otimizações de Performance**
- ⏳ **Parser** - Performance
  - Implementar cache de tokens
  - Implementar otimizações de memória
  - Implementar parsing incremental (se aplicável)
  - Implementar profiling integrado

##### **4.3 Visitor Pattern**
- ⏳ **ASTVisitor** (`ast_visitor.hpp/cpp`)
  - Implementar classe base abstrata
  - Implementar `visit()` para cada tipo de nó
  - Implementar `visitChildren()` genérico

- ⏳ **Visitors Específicos**
  - Implementar `ASTPrinter` para debug
  - Implementar `ASTValidator` para validação
  - Implementar `ASTStatistics` para métricas

##### **4.4 Testes Avançados**
- ⏳ Testes de compatibilidade entre versões C
- ⏳ Testes de performance com arquivos grandes
- ⏳ Testes de visitor pattern
- ⏳ Testes de casos extremos

#### **Fase 5: Integração com Outros Módulos (Semanas 10-11)**

##### **5.1 Integração com Lexer**
- ⏳ **TokenStream** (`token_stream.hpp/cpp`)
  - Implementar interface para comunicação com lexer
  - Implementar buffer de tokens com lookahead
  - Implementar mapeamento de posições

- ⏳ **LexerParserBridge** (`lexer_parser_bridge.hpp/cpp`)
  - Implementar ponte entre lexer e parser
  - Implementar conversão de tokens
  - Implementar tratamento de erros integrado

##### **5.2 Integração com Preprocessador**
- ⏳ **PreprocessorParserBridge** (`preprocessor_parser_bridge.hpp/cpp`)
  - Implementar integração com preprocessador
  - Implementar mapeamento de posições expandidas
  - Implementar tratamento de diretivas

##### **5.3 Interface para Analisador Semântico**
- ⏳ **SemanticInterface** (`semantic_interface.hpp/cpp`)
  - Implementar interface para analisador semântico
  - Implementar exportação de AST
  - Implementar metadados para análise semântica

##### **5.4 Testes de Integração**
- ⏳ Testes completos lexer + parser
- ⏳ Testes preprocessador + parser
- ⏳ Testes de pipeline completo
- ⏳ Testes com programas C reais

#### **Fase 6: Validação e Otimização Final (Semanas 12-13)**

##### **6.1 Validação Completa**
- ⏳ Testes com suite completa de programas C
- ⏳ Validação contra compiladores de referência
- ⏳ Testes de conformidade com padrões C
- ⏳ Testes de robustez com código malformado

##### **6.2 Otimizações Finais**
- ⏳ Profiling detalhado e otimizações
- ⏳ Otimização de uso de memória
- ⏳ Otimização de velocidade de parsing
- ⏳ Tuning de recuperação de erros

##### **6.3 Documentação e Entrega**
- ⏳ Documentação de API completa
- ⏳ Guias de uso e exemplos
- ⏳ Documentação de arquitetura atualizada
- ⏳ Métricas finais e relatórios

### Dependências Entre Fases

```
Fase 1 (Estruturas) → Fase 2 (Componentes) → Fase 3 (Parser)
                                                    ↓
Fase 6 (Validação) ← Fase 5 (Integração) ← Fase 4 (Avançadas)
```

### Critérios de Conclusão por Fase

#### **Fase 1 - Completa quando:**
- [ ] Todas as estruturas base implementadas e testadas
- [ ] Testes unitários passando (>90%)
- [ ] Configuração e logging funcionais
- [ ] Build system configurado

#### **Fase 2 - Completa quando:**
- [ ] Gramática carregando corretamente
- [ ] Estado do parser funcional
- [ ] Recuperação de erros básica implementada
- [ ] Testes de componentes passando

#### **Fase 3 - Completa quando:**
- [ ] Parser parseando construções C89 básicas
- [ ] AST sendo gerada corretamente
- [ ] Precedência de operadores funcionando
- [ ] Testes de parsing passando (>85%)

#### **Fase 4 - Completa quando:**
- [ ] Suporte a múltiplas versões C implementado
- [ ] Visitor pattern funcional
- [ ] Otimizações básicas implementadas
- [ ] Testes avançados passando

#### **Fase 5 - Completa quando:**
- [ ] Integração com lexer funcional
- [ ] Integração com preprocessador funcional
- [ ] Interface semântica implementada
- [ ] Testes de integração passando (>90%)

#### **Fase 6 - Completa quando:**
- [ ] Todos os testes passando (>95%)
- [ ] Performance dentro das métricas
- [ ] Documentação completa
- [ ] Validação final aprovada

### Marcos de Progresso

#### **Marco 1: Parser Básico Funcional** ✅
- [ ] Parsing de expressões aritméticas
- [ ] Parsing de declarações de variáveis
- [ ] Parsing de funções simples
- [ ] AST básica funcional
- [ ] Testes unitários passando (>80%)

#### **Marco 2: Parser Completo** 🔄
- [ ] Parsing de todas as construções C89
- [ ] Sistema de recuperação de erros
- [ ] Integração com lexer
- [ ] Testes de integração passando

#### **Marco 3: Funcionalidades Avançadas** ⏳
- [ ] Suporte a múltiplas versões do C
- [ ] Otimizações de performance
- [ ] Logging e debugging completos
- [ ] Métricas de qualidade atingidas

#### **Marco 4: Integração Completa** ⏳
- [ ] Integração com preprocessador
- [ ] Interface para analisador semântico
- [ ] Testes end-to-end
- [ ] Documentação completa

### Estratégia de Testes Incrementais

#### **Testes Unitários**
- **Cobertura Mínima**: 85%
- **Frequência**: A cada commit
- **Foco**: Componentes individuais

#### **Testes de Integração**
- **Cobertura**: Fluxos principais
- **Frequência**: Diária
- **Foco**: Interação entre componentes

#### **Testes de Performance**
- **Métricas**: Tempo de parsing, uso de memória
- **Frequência**: Semanal
- **Benchmarks**: Programas C reais

#### **Testes de Compatibilidade**
- **Versões**: C89, C99, C11, C17, C23
- **Frequência**: Por marco
- **Validação**: Programas de referência

### Métricas de Progresso

#### **Métricas de Código**
- **Cobertura de Testes**: 85%+ ✅
- **Complexidade Ciclomática**: <10 por função ✅
- **Linhas de Código**: ~15,000 linhas estimadas
- **Documentação**: 100% das APIs públicas

#### **Métricas de Funcionalidade**
- **Construções C Suportadas**: 0/45 (0%) ⏳
- **Versões C Suportadas**: 0/5 (0%) ⏳
- **Estratégias de Recuperação**: 0/8 (0%) ⏳
- **Tipos de Nó AST**: 0/25 (0%) ⏳

#### **Métricas de Performance**
- **Velocidade de Parsing**: >10,000 linhas/segundo ⏳
- **Uso de Memória**: <100MB para arquivos grandes ⏳
- **Tempo de Inicialização**: <100ms ⏳

#### **Métricas de Qualidade**
- **Detecção de Erros**: >95% de erros sintáticos ⏳
- **Recuperação de Erros**: >80% de casos ⏳
- **Precisão da AST**: 100% para código válido ⏳

### Cronograma de Revisões

#### **Revisões Semanais**
- **Segunda-feira**: Planejamento da semana
- **Quarta-feira**: Revisão de progresso
- **Sexta-feira**: Revisão de código e testes

#### **Revisões de Marco**
- **Revisão Técnica**: Arquitetura e design
- **Revisão de Qualidade**: Testes e métricas
- **Revisão de Performance**: Benchmarks e otimizações

#### **Revisão Final**
- **Validação Completa**: Todos os testes passando
- **Documentação**: Completa e atualizada
- **Performance**: Métricas atingidas
- **Integração**: Funcionando com outros módulos

---

## Conclusão

Esta documentação serve como um **guia arquitetural completo** para a implementação do processador sintático do compilador C. O design modular, as interfaces bem definidas e a estratégia de implementação em fases garantem:

### Pontos Fortes da Arquitetura

1. **Modularidade**: Componentes independentes e testáveis
2. **Extensibilidade**: Fácil adição de novas funcionalidades
3. **Manutenibilidade**: Código organizado e bem documentado
4. **Performance**: Otimizações integradas desde o design
5. **Robustez**: Sistema robusto de recuperação de erros

### Garantias de Compatibilidade

1. **Padrões C**: Suporte completo a C89/C99/C11/C17/C23
2. **Integração**: Interfaces compatíveis com lexer e preprocessador
3. **Extensibilidade**: Preparado para analisador semântico
4. **Portabilidade**: Código multiplataforma

### Próximos Passos Imediatos

1. **Implementar estruturas base** (AST, Token, Config)
2. **Criar parser básico** para expressões simples
3. **Implementar testes unitários** fundamentais
4. **Configurar pipeline de CI/CD** para testes automáticos

A implementação seguirá este documento como referência, com atualizações incrementais conforme o progresso e descobertas durante o desenvolvimento.