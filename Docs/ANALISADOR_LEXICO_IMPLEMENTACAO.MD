### Fases de Implementação

#### **FASE 1: Componentes Base (Semana 1)**

**1.1 Token (Prioridade: CRÍTICA)**
```cpp
// Ordem de implementação:
1. enum TokenType - Definir todos os tipos de token
2. struct Position - Sistema de posicionamento no código
3. struct Value - União para valores de literais
4. class Token - Construtores (básico e com valor)
5. getType(), getLexeme(), getPosition(), getValue() - Métodos de acesso
6. isKeyword(), isOperator(), isLiteral() - Métodos de classificação
7. toString() - Conversão para string
8. tokenTypeToString() - Função utilitária global
9. isKeywordToken(), isOperatorToken(), isLiteralToken() - Funções de verificação
10. getOperatorPrecedence(), getOperatorAssociativity() - Precedência de operadores
11. Operadores de stream (<<, >>)
```
*Dependências: Nenhuma*
*Tempo estimado: 1.5 dias*
*Marco: Token funcional com todos os tipos e métodos utilitários*

**1.2 ErrorHandler (Prioridade: CRÍTICA)**
```cpp
// Ordem de implementação:
1. enum ErrorType, enum RecoveryMode - Tipos de erro e recuperação
2. struct CompilerError - Estrutura de informações do erro
3. class ErrorHandler - Construtor com limite máximo
4. reportError() - Método principal de relatório de erros
5. reportWarning() - Método de relatório de warnings
6. hasErrors(), getErrorCount(), getWarningCount() - Métodos de consulta
7. getErrors() - Retorna lista completa de erros
8. printErrorSummary() - Imprime resumo formatado
9. setRecoveryMode() - Configuração de estratégia de recuperação
10. formatError() - Formatação de erro para exibição
11. getErrorContext() - Extração de contexto do erro
12. generateSuggestion() - Geração de sugestões de correção
13. logError() - Registro no sistema de log
```
*Dependências: Token (Position)*
*Tempo estimado: 1.5 dias*
*Marco: Sistema completo de erros e warnings operacional*

**1.3 LexerConfig (Prioridade: ALTA)**
```cpp
// Ordem de implementação:
1. enum CVersion, enum Feature - Versões e features suportadas
2. class LexerConfig - Construtor com versão padrão
3. setVersion(), getVersion() - Gerenciamento de versão
4. loadKeywords() - Carregamento de palavras-chave de arquivo
5. getKeywords() - Retorna conjunto de palavras-chave
6. isFeatureEnabled() - Verificação de features habilitadas
7. enableFeature(), disableFeature() - Gerenciamento de features
8. initializeFeatures() - Inicialização baseada na versão
9. parseConfigFile() - Leitura de configurações de arquivo
10. validateVersion() - Validação de versão suportada
11. Integração com arquivos de dados
```
*Dependências: Token, ErrorHandler*
*Tempo estimado: 1.5 dias*
*Marco: Configuração completa por versão e features*

#### **FASE 2: Componentes de Suporte (Semana 1-2)**

**2.1 SymbolTable (Prioridade: ALTA)**
```cpp
// Ordem de implementação:
1. struct SymbolInfo - Informações do símbolo
2. class SymbolTable - Estrutura de dados interna (hash table)
3. hashFunction() - Função hash para símbolos
4. insert() - Inserção de símbolos na tabela
5. lookup() - Busca de símbolos por nome
6. findSlot() - Encontra slot para símbolo
7. isKeyword() - Verificação integrada com LexerConfig
8. getKeywordType() - Retorna tipo da palavra-chave
9. addOccurrence() - Adiciona nova ocorrência de símbolo
10. getAllSymbols() - Retorna todos os símbolos registrados
11. printStatistics() - Imprime estatísticas de uso
12. rehash() - Reorganização quando fator de carga é alto
13. loadKeywordsFromFile() - Carrega palavras-chave de arquivo
```
*Dependências: Token, LexerConfig*
*Tempo estimado: 1.5 dias*
*Marco: Tabela de símbolos completa e eficiente*

**2.2 LookaheadBuffer (Prioridade: MÉDIA)**
```cpp
// Ordem de implementação:
1. class LookaheadBuffer - Buffer circular com stream de entrada
2. peek() - Visualização de caractere sem consumo
3. consume() - Consome e retorna próximo caractere
4. putback() - Retorna caractere para o buffer
5. hasMore() - Verifica se há mais caracteres disponíveis
6. fillBuffer() - Preenche buffer com novos caracteres
7. getBufferSize() - Retorna tamanho atual do buffer
8. shiftBuffer() - Move conteúdo do buffer para a esquerda
9. needsRefill() - Verifica se buffer precisa ser recarregado
10. expandBuffer() - Expande tamanho do buffer dinamicamente
```
*Dependências: Nenhuma (apenas I/O)*
*Tempo estimado: 1 dia*
*Marco: Buffer de lookahead completo e otimizado*

**2.3 LexerLogger (Prioridade: BAIXA)**
```cpp
// Ordem de implementação:
1. enum LogLevel - Níveis de log (DEBUG, INFO, WARNING, ERROR)
2. class LexerLogger - Sistema de logging com nível configurável
3. setLogLevel() - Define nível mínimo de log
4. debug(), info(), warning(), error() - Métodos por nível
5. logToken() - Log específico para tokens gerados
6. logStateTransition() - Log de transições de estado
7. setOutputFile() - Define arquivo de saída dos logs
8. formatLogMessage() - Formatação de mensagem de log
9. getCurrentTimestamp() - Gera timestamp para logs
10. writeToFile(), writeToConsole() - Métodos de saída
```
*Dependências: Token, LexerState (para logs específicos)*
*Tempo estimado: 0.5 dia*
*Marco: Sistema completo de debug operacional*

#### **FASE 3: Máquina de Estados (Semana 2)**

**3.1 LexerState (Prioridade: CRÍTICA)**
```cpp
// Ordem de implementação:
1. enum LexerState - Todos os estados possíveis da máquina
2. class StateMachine - Lógica de transições de estado
3. getCurrentState() - Retorna estado atual da máquina
4. transition() - Executa transição baseada no caractere
5. isAcceptingState() - Verifica se estado atual é de aceitação
6. isErrorState() - Verifica se estado atual é de erro
7. reset() - Reinicializa máquina para estado START
8. getTokenType() - Retorna tipo de token do estado atual
9. getNextState() - Calcula próximo estado (método interno)
10. isValidTransition() - Validação de transições
11. buildTransitionTable() - Constrói tabela de transições otimizada
```
*Dependências: Token, ErrorHandler*
*Tempo estimado: 2 dias*
*Marco: Máquina de estados completa e otimizada*

#### **FASE 4: Lexer Principal (Semana 2-3)**

**4.1 Lexer - Métodos Base (Prioridade: CRÍTICA)**
```cpp
// Ordem de implementação:
1. class Lexer - Construtor com arquivo e ErrorHandler
2. ~Lexer() - Destrutor que libera recursos
3. nextToken() - Método principal de tokenização
4. peekToken() - Visualização de token futuro sem consumir
5. tokenizeAll() - Tokeniza todo o arquivo e retorna lista
6. hasMoreTokens() - Verifica se ainda há tokens disponíveis
7. reset() - Reinicializa estado do lexer para início
8. getCurrentPosition() - Retorna posição atual no arquivo
9. setVersion() - Configura versão do padrão C
10. readNextChar() - Lê próximo caractere com controle de posição
11. skipWhitespace() - Pula espaços em branco e atualiza posição
12. Integração com todos os componentes
```
*Dependências: TODOS os componentes anteriores*
*Tempo estimado: 2 dias*
*Marco: Lexer básico completo e funcional*

**4.2 Lexer - Reconhecimento de Tokens (Prioridade: CRÍTICA)**
```cpp
// Ordem de implementação:
1. recognizeIdentifier() - Identificadores e palavras-chave
2. recognizeNumber() - Literais numéricos (int, float, hex, octal)
3. recognizeString() - Literais de string com escape sequences
4. recognizeCharacter() - Literais de caractere
5. recognizeOperator() - Operadores simples e compostos
6. recognizeComment() - Comentários de linha e bloco (// e /* */)
7. isValidIdentifierStart() - Verifica início válido de identificador
8. isValidIdentifierChar() - Verifica caractere válido em identificador
```
*Tempo estimado: 3 dias*
*Marco: Reconhecimento completo de todos os tipos de tokens*

**4.3 Lexer - Tratamento de Erros (Prioridade: ALTA)**
```cpp
// Ordem de implementação:
1. handleError() - Tratamento geral de erros léxicos
2. Estratégias de recuperação após erros
3. Validação de tokens gerados
4. Integração completa com ErrorHandler
```
*Tempo estimado: 1 dia*
*Marco: Sistema robusto de tratamento de erros*

#### **FASE 5: Implementação de Testes (Semana 4)**

**5.1 Testes Unitários Básicos (Prioridade: ALTA)**
```cpp
// test_token.cpp - Ordem de implementação:
1. testTokenCreation() - Criação básica de tokens
2. testTokenTypes() - Verificação de tipos
3. testTokenComparison() - Comparação entre tokens
4. testTokenSerialization() - Serialização para string
5. testTokenValue() - Valores de literais
6. testTokenPosition() - Posicionamento no código
7. testTokenUtilities() - Funções utilitárias

// test_errors.cpp - Ordem de implementação:
1. testErrorReporting() - Relatório de erros
2. testWarningReporting() - Relatório de warnings
3. testErrorRecovery() - Recuperação de erros
4. testErrorFormatting() - Formatação de mensagens
5. testErrorContext() - Contexto de erros
6. testErrorSuggestions() - Sugestões de correção
```
*Dependências: Token, ErrorHandler implementados*
*Tempo estimado: 1 dia*
*Marco: Testes unitários básicos funcionais*

**5.2 Testes de Componentes (Prioridade: ALTA)**
```cpp
// test_states.cpp - Ordem de implementação:
1. testStateTransitions() - Transições de estado
2. testAcceptingStates() - Estados de aceitação
3. testErrorStates() - Estados de erro
4. testStateMachineReset() - Reset da máquina
5. testTransitionTable() - Tabela de transições
6. testStateValidation() - Validação de estados
```
*Dependências: LexerState, StateMachine implementados*
*Tempo estimado: 0.5 dia*
*Marco: Testes de máquina de estados completos*

**5.3 Testes do Lexer Principal (Prioridade: CRÍTICA)**
```cpp
// test_lexer.cpp - Ordem de implementação:
1. testLexerInitialization() - Inicialização do lexer
2. testTokenGeneration() - Geração de tokens
3. testIdentifierRecognition() - Reconhecimento de identificadores
4. testNumberRecognition() - Reconhecimento de números
5. testStringRecognition() - Reconhecimento de strings
6. testOperatorRecognition() - Reconhecimento de operadores
7. testCommentHandling() - Tratamento de comentários
8. testWhitespaceHandling() - Tratamento de espaços
9. testErrorHandling() - Tratamento de erros
10. testFileProcessing() - Processamento de arquivos
11. testPositionTracking() - Rastreamento de posição
12. testVersionSupport() - Suporte a versões do C
```
*Dependências: Lexer completo implementado*
*Tempo estimado: 1.5 dias*
*Marco: Testes completos do lexer funcionais*

**5.4 Testes de Integração (Prioridade: ALTA)**
```cpp
// test_c_samples.cpp - Ordem de implementação:
1. testHelloWorld() - Programa "Hello World"
2. testVariableDeclarations() - Declarações de variáveis
3. testFunctionDefinitions() - Definições de funções
4. testControlStructures() - Estruturas de controle
5. testPreprocessorDirectives() - Diretivas de pré-processador
6. testComplexExpressions() - Expressões complexas
7. testPointerOperations() - Operações com ponteiros
8. testStructUnionEnum() - Estruturas, uniões e enums
9. testArrayOperations() - Operações com arrays
10. testStringLiterals() - Literais de string
11. testNumericLiterals() - Literais numéricos
12. testCommentStyles() - Estilos de comentários

// test_versions.cpp - Ordem de implementação:
1. testC89Features() - Features específicas do C89
2. testC99Features() - Features específicas do C99
3. testC11Features() - Features específicas do C11
4. testC17Features() - Features específicas do C17
5. testC23Features() - Features específicas do C23
6. testVersionCompatibility() - Compatibilidade entre versões
7. testFeatureDetection() - Detecção de features
8. testKeywordVersioning() - Versionamento de palavras-chave
```
*Dependências: Sistema completo implementado*
*Tempo estimado: 2 dias*
*Marco: Testes de integração e compatibilidade completos*

### Marcos de Progresso

#### **Marco 1.1: Fundação (Fim da Semana 1)**
- ✅ Token completamente implementado
- ✅ ErrorHandler operacional
- ✅ LexerConfig com suporte a versões
- ✅ SymbolTable eficiente
- **Critério de Aceitação**: Testes unitários passando para todos os componentes base

#### **Marco 1.2: Infraestrutura (Meio da Semana 2)**
- ✅ LookaheadBuffer otimizado
- ✅ LexerLogger funcional
- ✅ LexerState com máquina de estados completa
- **Critério de Aceitação**: Máquina de estados reconhece transições básicas

#### **Marco 1.3: Lexer Básico (Fim da Semana 2)**
- ✅ Lexer inicializa e abre arquivos
- ✅ nextToken() implementado
- ✅ Reconhecimento de identificadores simples
- **Critério de Aceitação**: Lexer processa arquivo "Hello World" corretamente

#### **Marco 1.4: Reconhecimento Completo (Meio da Semana 3)**
- ✅ Todos os tipos de token reconhecidos
- ✅ Números em todas as bases (decimal, hex, octal)
- ✅ Strings com sequências de escape
- ✅ Operadores e delimitadores completos
- **Critério de Aceitação**: Lexer processa arquivos C complexos

#### **Marco 1.5: Sistema Robusto (Fim da Semana 3)**
- ✅ Tratamento completo de erros
- ✅ Recuperação após erros
- ✅ Suporte a todas as versões do C (C89-C23)
- ✅ Integração com arquivos de dados
- **Critério de Aceitação**: Lexer detecta e reporta erros corretamente

#### **Marco 1.6: Sistema Completo (Fim da Semana 3)**
- ✅ Lexer completo com todos os componentes integrados
- ✅ Tratamento robusto de erros e recuperação
- ✅ Suporte completo a todas as versões do C (C89-C23)
- ✅ Sistema de logging e debug operacional
- **Critério de Aceitação**: Lexer processa arquivos C complexos corretamente

#### **Marco 1.7: Testes Unitários (Início da Semana 4)**
- ✅ Testes unitários para Token e ErrorHandler
- ✅ Testes de componentes (SymbolTable, LookaheadBuffer, etc.)
- ✅ Testes da máquina de estados
- **Critério de Aceitação**: Todos os testes unitários passando

#### **Marco 1.8: Testes de Integração (Meio da Semana 4)**
- ✅ Testes completos do Lexer principal
- ✅ Testes com amostras de código C real
- ✅ Testes de compatibilidade entre versões
- **Critério de Aceitação**: Lexer processa corretamente código C de todas as versões

#### **Marco 1.9: Validação Final (Fim da Semana 4)**
- ✅ Cobertura de testes ≥ 95%
- ✅ Benchmarks de performance
- ✅ Documentação de API completa
- ✅ Validação com arquivos de dados de configuração
- **Critério de Aceitação**: Sistema pronto para integração com parser

### Estratégia de Testes Incrementais

```cpp
// Sequência de testes por fase:

FASE 1: Testes Unitários Básicos
- test_token_creation()
- test_error_reporting()
- test_config_loading()
- test_symbol_table_operations()

FASE 2: Testes de Integração Simples
- test_state_transitions()
- test_buffer_operations()
- test_logging_output()

FASE 3: Testes de Tokenização
- test_identifier_recognition()
- test_number_parsing()
- test_string_processing()
- test_operator_detection()

FASE 4: Testes Completos
- test_c89_compatibility()
- test_c99_features()
- test_error_recovery()
- test_performance_benchmarks()
```

### Métricas de Progresso

**Indicadores de Qualidade:**
- **Cobertura de Código**: Meta 95%+
- **Testes Passando**: Meta 100%
- **Performance**: Meta <1ms por token
- **Detecção de Erros**: Meta 99%+ de precisão

**Indicadores de Completude:**
- **Componentes Implementados**: 8/8 (Token, ErrorHandler, LexerConfig, SymbolTable, LookaheadBuffer, LexerLogger, LexerState, Lexer)
- **Métodos Implementados**: 154/154 (incluindo métodos de teste)
- **Tipos de Token Suportados**: 50+/50+
- **Versões do C Suportadas**: 5/5 (C89, C99, C11, C17, C23)
- **Fases de Implementação**: 5/5 (Base, Suporte, Estados, Lexer, Testes)

**Cronograma de Revisões:**
- **Revisão Semanal**: Progresso e bloqueadores
- **Revisão de Marco**: Critérios de aceitação
- **Revisão Final**: Integração e performance

---

*Esta documentação serve como guia arquitetural completo para a implementação do analisador léxico. A ordem de implementação proposta garante desenvolvimento eficiente com dependências resolvidas e marcos claros de progresso. Todos os **154 métodos especificados** estão agora organizados em **5 fases lógicas** que cobrem integralmente as funcionalidades necessárias, incluindo suporte completo às versões C89-C23, arquivos de dados de configuração e testes abrangentes. A sequência garante que cada componente seja implementado com suas dependências já resolvidas, permitindo desenvolvimento incremental e validação contínua.*