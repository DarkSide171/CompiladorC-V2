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

- **`parser.hpp`**: Interface principal do Analisador sintático
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

##### **5.2 Mapeamento de Posições (Preprocessador)**
- ⏳ **PreprocessorParserBridge** (`preprocessor_parser_bridge.hpp/cpp`)
  - Implementar mapeamento de posições entre código expandido e original
  - Implementar identificação de expansões de macro
  - Implementar utilitários para diagnósticos precisos
  - **NOTA**: Não há integração direta - apenas mapeamento via lexer

##### **5.3 Interface para Analisador Semântico**
- ⏳ **SemanticInterface** (`semantic_interface.hpp/cpp`)
  - Implementar interface para analisador semântico
  - Implementar exportação de AST
  - Implementar metadados para análise semântica

##### **5.4 Testes de Integração**
- ⏳ Testes completos lexer + parser
- ⏳ Testes de pipeline (preprocessador → lexer → parser)
- ⏳ Testes de mapeamento de posições com código pré-processado
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

