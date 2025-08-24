# Fases do Compilador C

Este documento detalha cada fase do processo de compilação, suas responsabilidades, algoritmos utilizados e estruturas de dados envolvidas.

## 1. Análise Léxica (Lexical Analysis)

### Responsabilidades
- Converter o fluxo de caracteres do código fonte em uma sequência de tokens
- Eliminar espaços em branco, comentários e caracteres irrelevantes
- Detectar e reportar erros léxicos
- Manter informações de localização (linha, coluna) para relatórios de erro

### Tokens Reconhecidos

#### Palavras-chave
```
int, float, double, char, void, if, else, while, for, do, switch, case, default,
break, continue, return, struct, union, enum, typedef, static, extern, auto,
register, const, volatile, sizeof
```

#### Operadores
```
Aritméticos: +, -, *, /, %
Relacionais: <, >, <=, >=, ==, !=
Lógicos: &&, ||, !
Bitwise: &, |, ^, ~, <<, >>
Atribuição: =, +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=
Incremento/Decremento: ++, --
Acesso: ., ->
Ternário: ? :
```

#### Delimitadores
```
( ) [ ] { } ; , :
```

#### Literais
- **Inteiros**: 123, 0x1A, 077
- **Ponto flutuante**: 3.14, 1.5e-10
- **Caracteres**: 'a', '\n', '\0'
- **Strings**: "Hello World", "\"quoted\""

#### Identificadores
- Nomes de variáveis, funções, tipos definidos pelo usuário
- Padrão: [a-zA-Z_][a-zA-Z0-9_]*

### Estruturas de Dados

```cpp
enum TokenType {
    // Literais
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    CHAR_LITERAL,
    STRING_LITERAL,
    
    // Identificadores
    IDENTIFIER,
    
    // Palavras-chave
    KW_INT, KW_FLOAT, KW_IF, KW_ELSE, /* ... */
    
    // Operadores
    OP_PLUS, OP_MINUS, OP_MULTIPLY, /* ... */
    
    // Delimitadores
    LPAREN, RPAREN, LBRACE, RBRACE, /* ... */
    
    // Especiais
    END_OF_FILE,
    INVALID_TOKEN
};

struct Token {
    TokenType type;
    std::string lexeme;  // Texto original
    int line;
    int column;
    
    // Para literais numéricos
    union {
        int intValue;
        double floatValue;
    };
};
```

### Algoritmo
1. **Autômato Finito Determinístico (DFA)** para reconhecimento de padrões
2. **Tabela de símbolos** para palavras-chave
3. **Buffer duplo** para leitura eficiente do arquivo

---

## 2. Análise Sintática (Syntax Analysis)

### Responsabilidades
- Verificar se a sequência de tokens segue a gramática da linguagem C
- Construir a Árvore Sintática Abstrata (AST)
- Detectar e reportar erros sintáticos
- Implementar recuperação de erros para análise contínua

### Gramática (Subconjunto de C)

```
program → declaration_list

declaration_list → declaration_list declaration | declaration

declaration → var_declaration | fun_declaration

var_declaration → type_specifier ID ';' | type_specifier ID '[' NUM ']' ';'

fun_declaration → type_specifier ID '(' params ')' compound_stmt

type_specifier → 'int' | 'float' | 'void'

params → param_list | 'void'
param_list → param_list ',' param | param
param → type_specifier ID | type_specifier ID '[' ']'

compound_stmt → '{' local_declarations statement_list '}'

local_declarations → local_declarations var_declaration | empty

statement_list → statement_list statement | empty

statement → expression_stmt | compound_stmt | selection_stmt | iteration_stmt | return_stmt

expression_stmt → expression ';' | ';'

selection_stmt → 'if' '(' expression ')' statement | 'if' '(' expression ')' statement 'else' statement

iteration_stmt → 'while' '(' expression ')' statement | 'for' '(' expression_stmt expression_stmt expression ')' statement

return_stmt → 'return' ';' | 'return' expression ';'

expression → var '=' expression | simple_expression

simple_expression → additive_expression relop additive_expression | additive_expression

relop → '<=' | '<' | '>' | '>=' | '==' | '!='

additive_expression → additive_expression addop term | term

addop → '+' | '-'

term → term mulop factor | factor

mulop → '*' | '/'

factor → '(' expression ')' | var | call | NUM

call → ID '(' args ')'

args → arg_list | empty
arg_list → arg_list ',' expression | expression

var → ID | ID '[' expression ']'
```

### Estrutura da AST

```cpp
enum NodeType {
    PROGRAM_NODE,
    DECLARATION_NODE,
    FUNCTION_NODE,
    VARIABLE_NODE,
    STATEMENT_NODE,
    EXPRESSION_NODE
};

struct ASTNode {
    NodeType nodeType;
    std::string name;
    std::vector<ASTNode*> children;
    
    // Informações específicas do nó
    TokenType op;  // Para expressões
    int value;     // Para literais
    std::string type;  // Para declarações
    
    // Informações de localização
    int line;
    int column;
};
```

### Algoritmo
- **Parser Recursivo Descendente** com **Lookahead de 1 token**
- **Recuperação de erros** por sincronização em pontos específicos

---

## 3. Análise Semântica (Semantic Analysis)

### Responsabilidades
- Verificar a correção semântica do programa
- Construir e gerenciar tabela de símbolos
- Verificação de tipos
- Análise de escopo
- Decorar a AST com informações semânticas

### Verificações Realizadas

#### Declarações e Escopo
- Variáveis devem ser declaradas antes do uso
- Não pode haver redeclaração no mesmo escopo
- Funções devem ser declaradas antes da chamada
- Verificação de escopo aninhado

#### Verificação de Tipos
- Compatibilidade de tipos em atribuições
- Verificação de tipos em expressões
- Conversões implícitas permitidas
- Verificação de tipos em chamadas de função

#### Funções
- Verificação de assinatura de função
- Número e tipo de parâmetros
- Tipo de retorno
- Todas as funções não-void devem retornar valor

### Tabela de Símbolos

```cpp
enum SymbolType {
    VARIABLE_SYMBOL,
    FUNCTION_SYMBOL,
    PARAMETER_SYMBOL
};

struct Symbol {
    std::string name;
    SymbolType symbolType;
    std::string dataType;  // int, float, void, etc.
    int scope;
    bool isArray;
    int arraySize;
    
    // Para funções
    std::vector<std::string> paramTypes;
    std::string returnType;
};

class SymbolTable {
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    int currentScope;
    
public:
    void enterScope();
    void exitScope();
    bool insert(const Symbol& symbol);
    Symbol* lookup(const std::string& name);
    Symbol* lookupCurrentScope(const std::string& name);
};
```

### Algoritmo
1. **Travessia da AST** em profundidade
2. **Pilha de escopos** para gerenciar contextos
3. **Verificação de tipos** com regras de coerção
4. **Decoração da AST** com informações de tipo

---

## 4. Geração de Código Intermediário

### Responsabilidades
- Gerar representação intermediária independente de máquina
- Facilitar otimizações
- Simplificar a geração de código final

### Representação: Código de Três Endereços

```
Formato: resultado = operando1 operador operando2

Exemplos:
t1 = a + b
t2 = t1 * c
x = t2

if a < b goto L1
goto L2
L1: ...
L2: ...

call func, n    // chamada com n parâmetros
return x
```

### Estruturas de Dados

```cpp
enum OpCode {
    ADD, SUB, MUL, DIV, MOD,
    LT, LE, GT, GE, EQ, NE,
    ASSIGN, GOTO, IF_GOTO,
    CALL, RETURN, PARAM,
    LABEL
};

struct Quadruple {
    OpCode op;
    std::string arg1;
    std::string arg2;
    std::string result;
};

class IntermediateCode {
    std::vector<Quadruple> code;
    int tempCounter;
    int labelCounter;
    
public:
    std::string newTemp();
    std::string newLabel();
    void emit(OpCode op, const std::string& arg1, const std::string& arg2, const std::string& result);
};
```

---

## 5. Geração de Código Final

### Responsabilidades
- Traduzir código intermediário para assembly/código de máquina
- Alocação de registradores
- Otimizações específicas da arquitetura alvo

### Arquitetura Alvo
- **x86-64** (inicial)
- **MIPS** (futuro)
- **ARM** (futuro)

### Alocação de Registradores
- **Algoritmo de coloração de grafos**
- **Linear scan** para casos simples
- **Spilling** para registradores insuficientes

### Otimizações
- **Eliminação de código morto**
- **Propagação de constantes**
- **Eliminação de subexpressões comuns**
- **Otimização de loops**

---

## Fluxo de Dados Entre Fases

```
Código Fonte (.c)
       ↓
   [Lexer]
       ↓
   Tokens
       ↓
   [Parser]
       ↓
     AST
       ↓
 [Semantic Analyzer]
       ↓
  AST Decorada + Tabela de Símbolos
       ↓
 [Code Generator]
       ↓
 Código Intermediário
       ↓
 [Final Code Generator]
       ↓
 Assembly/Código de Máquina
```

## Tratamento de Erros

Cada fase deve:
1. **Detectar erros** específicos de sua responsabilidade
2. **Reportar erros** com localização precisa
3. **Tentar recuperação** quando possível
4. **Continuar análise** para encontrar mais erros

### Tipos de Erro
- **Léxicos**: Caracteres inválidos, strings não terminadas
- **Sintáticos**: Estrutura inválida, tokens inesperados
- **Semânticos**: Tipos incompatíveis, variáveis não declaradas
- **Lógicos**: Código inalcançável, loops infinitos (warnings)