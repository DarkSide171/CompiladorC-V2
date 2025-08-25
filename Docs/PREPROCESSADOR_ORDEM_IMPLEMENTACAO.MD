## Cronograma de Implementação

### Estimativa de Desenvolvimento: 3-4 Semanas

#### **FASE 1: Estrutura Base (Semana 1)**

**1.1 Configuração do Projeto (Prioridade: CRÍTICA)**
```cpp
// Ordem de implementação:
1. Estrutura de diretórios
2. CMakeLists.txt para preprocessor
3. Headers básicos com interfaces
4. Configuração de testes unitários
```
*Tempo estimado: 1 dia*
*Marco: Projeto compilando com estrutura básica*

**1.2 Estruturas de Dados Fundamentais (Prioridade: CRÍTICA)**
```cpp
// directive.hpp/cpp - Ordem de implementação:
1. enum DirectiveType - Todos os tipos de diretivas
2. struct PreprocessorPosition - Posicionamento no código
3. class Directive - Representação de diretivas
4. Métodos básicos de validação
```
*Tempo estimado: 1 dia*
*Marco: Estruturas básicas funcionais*

**1.3 Sistema de Configuração (Prioridade: ALTA)**
```cpp
// preprocessor_config.hpp/cpp - Ordem de implementação:
1. enum CVersion - Versões do C suportadas
2. class PreprocessorConfig - Configurações gerais
3. initializeDefaultConfig() - Configurações padrão
4. loadConfiguration() / loadConfigFromFile() - Carregamento
5. validateConfiguration() / validateConfigValue() - Validação
6. parseConfigLine() - Parsing de configuração
7. applyConfigChanges() - Aplicação de mudanças
8. saveConfigToFile() - Salvamento
9. mergeConfigurations() / resetToDefaults() - Gerenciamento
10. validateCStandardCompatibility() / updateFeatureFlags() - Compatibilidade
11. resolveIncludePaths() / validateIncludePath() / normalizeIncludePath() - Caminhos
```
*Tempo estimado: 1 dia*
*Marco: Sistema de configuração operacional*

**1.4 Sistema de Logging (Prioridade: ALTA)**
```cpp
// preprocessor_logger.hpp/cpp - Ordem de implementação:
1. enum LogLevel - Níveis de log
2. class PreprocessorLogger - Logger principal
3. initializeLogger() - Inicialização do sistema
4. formatLogMessage() - Formatação de mensagens
5. writeToFile() / writeToConsole() - Métodos de saída
6. validateLogLevel() - Validação de níveis
7. addTimestamp() / addSourceLocation() - Enriquecimento
8. handleLogBuffering() / flushLogBuffer() - Buffer management
9. rotateLogFile() / compressOldLogs() - Gerenciamento de arquivos
10. calculateLogStatistics() / generateLogReport() - Relatórios
11. handleLogErrors() / optimizeLogPerformance() - Otimização
12. filterMessage() / validateLogFormat() - Validação avançada
```
*Tempo estimado: 1 dia*
*Marco: Sistema de logging funcional*

**1.5 Sistema de Estado (Prioridade: ALTA)**
```cpp
// preprocessor_state.hpp/cpp - Ordem de implementação:
1. enum ProcessingState - Estados de processamento
2. class PreprocessorState - Gerenciador de estado
3. pushState() / popState() / getCurrentState() - Pilha de estados
4. isEmpty() / getDepth() - Consultas de pilha
5. setFileContext() / getFileContext() - Contexto de arquivo
6. pushFileContext() / popFileContext() - Pilha de contexto
7. setCurrentLine() / getCurrentLine() / incrementLine() - Posição
8. setCurrentColumn() / getCurrentColumn() / incrementColumn() / resetColumn() - Coluna
9. setProcessingMode() / getProcessingMode() - Modo de processamento
10. enableConditionalBlock() / disableConditionalBlock() / isInConditionalBlock() - Blocos condicionais
11. setErrorState() / hasError() - Estado de erro
12. addProcessedFile() / wasFileProcessed() / getProcessedFiles() - Arquivos processados
```
*Tempo estimado: 1 dia*
*Marco: Sistema de estado funcional*

**1.6 Testes Básicos (Prioridade: ALTA)**
```cpp
// Ordem de implementação:
1. test_directive.cpp - Testes de estruturas básicas
2. test_config.cpp - Testes de configuração
3. test_logger.cpp - Testes de logging
4. test_state.cpp - Testes de estado
5. Configuração de CI/CD básica
```
*Tempo estimado: 1 dia*
*Marco: Testes básicos passando*

#### **FASE 2: Componentes Principais (Semana 2)**

**2.1 Processador de Macros (Prioridade: CRÍTICA)**
```cpp
// macro_processor.hpp/cpp - Ordem de implementação:
1. struct MacroInfo - Informações de macro
2. class MacroProcessor - Processador principal
3. defineMacro() / undefineMacro() - Definição e remoção
4. expandMacro() / expandMacroRecursively() - Expansão básica
5. isDefined() / getMacroValue() - Consultas
6. validateMacroName() / validateMacroValue() - Validação
7. handleMacroRedefinition() - Redefinição
8. defineFunctionMacro() / expandFunctionMacro() - Macros com parâmetros
9. handleStringification() / handleConcatenation() - Operadores # e ##
10. parseParameterList() / validateParameterCount() - Parâmetros
11. handleVariadicMacros() / expandVariadicArguments() - Macros variádicas
12. optimizeMacroExpansion() / cacheMacroResults() - Otimização
13. Tabela hash para armazenamento
14. Macros predefinidas básicas
15. Detecção de recursão infinita
```
*Tempo estimado: 2 dias*
*Marco: Macros simples funcionando*

**2.2 Gerenciador de Arquivos (Prioridade: CRÍTICA)**
```cpp
// file_manager.hpp/cpp - Ordem de implementação:
1. class FileManager - Gerenciador principal
2. readFile() / writeFile() - Operações básicas de arquivo
3. resolveInclude() - Resolução de includes
4. searchInPaths() / resolveRelativePath() - Busca em caminhos
5. cacheFile() / getCachedFile() - Sistema de cache
6. validateFilePath() / normalizeFilePath() - Validação de caminhos
7. updateDependencies() - Gerenciamento de dependências
8. checkFileExists() / getFileSize() / getLastModified() - Informações de arquivo
9. createBackup() / restoreFromBackup() - Backup
10. lockFile() / unlockFile() - Controle de acesso
11. compressFile() / decompressFile() - Compressão
12. calculateFileHash() / verifyFileIntegrity() - Integridade
13. monitorFileChanges() / handleFileSystemEvents() - Monitoramento
14. Detecção de inclusões circulares
15. Caminhos de busca configuráveis
```
*Tempo estimado: 2 dias*
*Marco: Inclusão de arquivos funcionando*

**2.3 Avaliador de Expressões (Prioridade: ALTA)**
```cpp
// expression_evaluator.hpp/cpp - Ordem de implementação:
1. class ExpressionEvaluator - Avaliador principal
2. tokenizeExpression() / parseTokens() - Tokenização e parsing
3. evaluateExpression() / evaluateSubexpression() - Avaliação principal
4. handleOperatorPrecedence() / applyOperator() - Precedência e aplicação
5. evaluateUnaryOperator() / evaluateBinaryOperator() - Operadores
6. handleParentheses() / validateParentheses() - Parênteses
7. convertToNumber() / validateNumericLiteral() - Conversão numérica
8. handleIdentifier() / resolveIdentifierValue() - Identificadores
9. optimizeExpression() / simplifyExpression() - Otimização
10. handleExpressionErrors() / reportSyntaxError() - Tratamento de erros
11. Operadores aritméticos e lógicos
12. Suporte ao operador defined()
13. Precedência de operadores
14. Tratamento de erros de avaliação
```
*Tempo estimado: 2 dias*
*Marco: Avaliação de expressões funcionando*

**2.4 Processador Principal (Prioridade: ALTA)**
```cpp
// preprocessor.hpp/cpp - Ordem de implementação:
1. class Preprocessor - Classe principal
2. initializeComponents() - Inicialização de componentes
3. processFile() / processString() - Processamento principal
4. processLine() - Processamento linha por linha
5. handleDirective() - Manipulação de diretivas
6. writeOutput() / generateOutput() - Geração de saída
7. updatePositionMapping() - Mapeamento de posições
8. handleErrors() / reportError() - Tratamento de erros
9. cleanup() / reset() - Limpeza e reset
10. validateInput() / validateOutput() - Validação
11. optimizeProcessing() / enableOptimizations() - Otimização
12. Integração dos componentes básicos
13. Fluxo de processamento principal
```
*Tempo estimado: 1 dia*
*Marco: Integração básica funcional*

#### **FASE 3: Processamento Condicional (Semana 3)**

**3.1 Processador Condicional (Prioridade: CRÍTICA)**
```cpp
// conditional_processor.hpp/cpp - Ordem de implementação:
1. class ConditionalProcessor - Processador principal
2. pushConditionalContext() / popConditionalContext() / getCurrentContext() - Pilha de contextos
3. evaluateCondition() - Avaliação de condições
4. validateConditionalStructure() - Validação de estruturas
5. handleNestedConditionals() - Condicionais aninhadas
6. processIfDirective() / processIfdefDirective() / processIfndefDirective() - Diretivas #if
7. processElseDirective() / processElifDirective() - Diretivas #else/#elif
8. processEndifDirective() - Diretiva #endif
9. isConditionTrue() / shouldProcessBlock() - Controle de processamento
10. handleConditionalErrors() - Tratamento de erros
11. optimizeConditionalEvaluation() - Otimização
12. Stack de contextos condicionais
13. Validação de estrutura aninhada
```
*Tempo estimado: 2 dias*
*Marco: Compilação condicional funcionando*

**3.2 Macros Avançadas (Prioridade: ALTA)**
```cpp
// Extensão do macro_processor.cpp:
1. defineFunctionMacro() - Macros com parâmetros
2. expandFunctionMacro() - Expansão com argumentos
3. handleStringification() - Operador #
4. handleConcatenation() - Operador ##
5. Detecção de recursão infinita
6. Validação de argumentos
```
*Tempo estimado: 2 dias*
*Marco: Macros complexas funcionando*

**3.3 Integração de Componentes (Prioridade: CRÍTICA)**
```cpp
// Extensão do preprocessor.hpp/cpp - Ordem de implementação:
1. Integração de todos os componentes
2. process() - Método principal de processamento
3. Coordenação entre módulos
4. Tratamento de erros integrado
5. Mapeamento de posições avançado
6. Sincronização entre processadores
```
*Tempo estimado: 1 dia*
*Marco: Preprocessor integrado funcionando*

#### **FASE 4: Otimização e Testes (Semana 4)**

**4.1 Processamento de Diretivas Avançadas (Prioridade: MÉDIA)**
```cpp
// directive.hpp/cpp - Ordem de implementação:
1. class DirectiveProcessor - Processador de diretivas
2. parseDirective() / validateDirectiveSyntax() - Parsing e validação
3. processIncludeDirective() / processDefineDirective() - Diretivas básicas
4. processIfDirective() / processIfdefDirective() / processIfndefDirective() - Condicionais
5. processElseDirective() / processElifDirective() / processEndifDirective() - Estruturas condicionais
6. processUndefDirective() / processErrorDirective() / processWarningDirective() - Outras diretivas
7. processPragmaDirective() / processLineDirective() - Diretivas especiais
8. handleDirectiveErrors() / reportDirectiveError() - Tratamento de erros
9. validateDirectiveContext() / checkDirectiveNesting() - Validação de contexto
10. optimizeDirectiveProcessing() - Otimização
11. Parsing avançado de diretivas
12. Validação de sintaxe
13. Tratamento de erros específicos
```
*Tempo estimado: 2 dias*
*Marco: Processamento de diretivas robusto*

**4.2 Testes Abrangentes (Prioridade: CRÍTICA)**
```cpp
// Ordem de implementação:
1. test_macros.cpp - Testes de macros completos
2. test_includes.cpp - Testes de inclusão
3. test_conditionals.cpp - Testes condicionais
4. test_expressions.cpp - Testes de expressões
5. test_integration.cpp - Testes de integração
6. Casos de teste com código C real
```
*Tempo estimado: 2 dias*
*Marco: Cobertura de testes > 90%*

**4.3 Otimizações de Performance (Prioridade: MÉDIA)**
```cpp
// Ordem de implementação:
1. Otimização de cache de arquivos
2. Otimização de expansão de macros
3. Redução de alocações dinâmicas
4. Profiling e identificação de gargalos
5. Otimização de estruturas de dados
```
*Tempo estimado: 1 dia*
*Marco: Performance otimizada*

**4.4 Documentação e Finalização (Prioridade: ALTA)**
```cpp
// Ordem de implementação:
1. Documentação de APIs
2. Exemplos de uso
3. Guia de integração
4. Documentação de configuração
5. README e instruções de build
```
*Tempo estimado: 1 dia*
*Marco: Documentação completa*

**4.5 Integração com Lexer (Prioridade: CRÍTICA)**
```cpp
// Ordem de implementação:
1. Interface de integração com lexer
2. Testes de integração lexer-preprocessor
3. Validação de mapeamento de posições
4. Testes end-to-end
5. Ajustes finais de compatibilidade
```
*Tempo estimado: 1 dia*
*Marco: Integração completa e testada*

### Métricas de Progresso

**Indicadores de Qualidade:**
- **Cobertura de Código**: Meta 95%+
- **Testes Passando**: Meta 100%
- **Performance**: Meta <10ms para arquivos típicos
- **Compatibilidade**: Meta 99%+ com código C padrão

**Indicadores de Completude:**
- **Componentes Implementados**: 6/6 (Preprocessor, MacroProcessor, FileManager, ConditionalProcessor, ExpressionEvaluator, Config)
- **Métodos Implementados**: ~80 métodos especificados
- **Tipos de Diretiva Suportados**: 15+ diretivas
- **Versões do C Suportadas**: 5/5 (C89, C99, C11, C17, C23)
- **Fases de Implementação**: 4/4 (Base, Componentes, Condicional, Testes)

**Cronograma de Revisões:**
- **Revisão Semanal**: Progresso e bloqueadores
- **Revisão de Marco**: Critérios de aceitação
- **Revisão Final**: Integração e performance

---

*Esta documentação serve como guia arquitetural completo para a implementação do pré-processador C. A ordem de implementação proposta garante desenvolvimento eficiente com dependências resolvidas e marcos claros de progresso. Todos os **~80 métodos especificados** estão organizados em fases lógicas de desenvolvimento, permitindo validação contínua e integração incremental com o analisador léxico existente.*

*Versão: 1.0*  
*Data: Janeiro 2025*  
*Status: Especificação Completa para Implementação*