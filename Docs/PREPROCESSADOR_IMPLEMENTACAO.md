# Documentação de Implementação do Pré-processador

Este documento detalha a arquitetura e especificações de implementação do pré-processador C, incluindo suporte para múltiplas versões da linguagem C e design modular extensível integrado ao compilador.

## Visão Geral

O pré-processador é responsável pela fase de pré-processamento da compilação, processando diretivas como `#include`, `#define`, `#ifdef` e outras antes da análise léxica. Este componente será implementado como um módulo separado que processa o código fonte e gera código expandido para o analisador léxico.

## Estrutura de Arquivos e Diretórios

### Organização do Pré-processador

```
src/preprocessor/
├── include/
│   ├── preprocessor.hpp          # Interface principal do pré-processador
│   ├── directive.hpp             # Definições de diretivas e tipos
│   ├── macro_processor.hpp       # Sistema de processamento de macros
│   ├── file_manager.hpp          # Gerenciamento de inclusão de arquivos
│   ├── conditional_processor.hpp # Processamento de compilação condicional
│   ├── expression_evaluator.hpp  # Avaliador de expressões constantes
│   ├── preprocessor_config.hpp   # Configurações e versões do C
│   ├── preprocessor_state.hpp    # Estados do processamento
│   ├── preprocessor_logger.hpp   # Sistema de logging
│   └── preprocessor_types.hpp    # Tipos e estruturas fundamentais
├── src/
│   ├── preprocessor.cpp          # Implementação principal
│   ├── directive.cpp             # Implementação de diretivas
│   ├── macro_processor.cpp       # Implementação de macros
│   ├── file_manager.cpp          # Implementação de inclusão
│   ├── conditional_processor.cpp # Implementação de condicionais
│   ├── expression_evaluator.cpp  # Implementação do avaliador
│   ├── preprocessor_config.cpp   # Implementação das configurações
│   ├── preprocessor_state.cpp    # Implementação de estados
│   └── preprocessor_logger.cpp   # Implementação do logging
├── data/
│   ├── directives/
│   │   ├── c89_directives.txt     # Diretivas do C89/C90
│   │   ├── c99_directives.txt     # Diretivas adicionais do C99
│   │   ├── c11_directives.txt     # Diretivas adicionais do C11
│   │   ├── c17_directives.txt     # Diretivas do C17
│   │   └── c23_directives.txt     # Diretivas adicionais do C23
│   ├── predefined_macros.txt      # Macros predefinidas por versão
│   └── system_headers.txt         # Caminhos de headers do sistema
└── tests/
    ├── unit/
    │   ├── test_preprocessor.cpp  # Testes unitários do preprocessor
    │   ├── test_macros.cpp        # Testes de processamento de macros
    │   ├── test_includes.cpp      # Testes de inclusão de arquivos
    │   ├── test_conditionals.cpp  # Testes de compilação condicional
    │   └── test_expressions.cpp   # Testes de avaliação de expressões
    ├── integration/
    │   ├── test_c_samples.cpp     # Testes com código C real
    │   └── test_versions.cpp      # Testes de compatibilidade por versão
    └── data/
        ├── valid_samples/         # Arquivos C válidos para teste
        │   ├── basic_macros.c
        │   ├── complex_includes.c
        │   ├── conditional_compilation.c
        │   └── version_specific/
        └── invalid_samples/       # Arquivos com erros para teste
            ├── macro_errors.c
            ├── include_errors.c
            └── conditional_errors.c
```

### Descrição dos Arquivos Principais

#### Headers (include/)
- **preprocessor.hpp**: Interface principal com métodos públicos do pré-processador
- **directive.hpp**: Enumerações de tipos de diretivas e estrutura Directive
- **macro_processor.hpp**: Classes para definição e expansão de macros
- **file_manager.hpp**: Gerenciamento de inclusão de arquivos e caminhos
- **conditional_processor.hpp**: Processamento de `#ifdef`, `#ifndef`, `#else`, `#endif`
- **expression_evaluator.hpp**: Avaliação de expressões constantes em diretivas
- **preprocessor_config.hpp**: Configurações de versão e features do C
- **preprocessor_state.hpp**: Estados do processamento e contexto
- **preprocessor_logger.hpp**: Sistema de logging e depuração
- **preprocessor_types.hpp**: Definições de tipos e estruturas fundamentais do preprocessador

#### Implementações (src/)
- **preprocessor.cpp**: Lógica principal de processamento e controle de fluxo
- **directive.cpp**: Métodos de reconhecimento e processamento de diretivas
- **macro_processor.cpp**: Algoritmos de expansão de macros e substituição
- **file_manager.cpp**: Operações de inclusão de arquivos e resolução de caminhos
- **conditional_processor.cpp**: Lógica de compilação condicional
- **expression_evaluator.cpp**: Avaliação de expressões aritméticas e lógicas
- **preprocessor_config.cpp**: Carregamento de configurações por versão
- **preprocessor_state.cpp**: Gerenciamento de estado e contexto
- **preprocessor_logger.cpp**: Formatação e saída de logs

#### Dados de Configuração (data/)
- **directives/**: Arquivos de texto com diretivas por versão do C
- **predefined_macros.txt**: Macros predefinidas pelo compilador
- **system_headers.txt**: Caminhos padrão para headers do sistema

#### Tipos Fundamentais (preprocessor_types.hpp)

O arquivo **preprocessor_types.hpp** contém as definições de tipos e estruturas fundamentais utilizadas em todo o sistema de preprocessamento. Este arquivo foi adicionado durante o desenvolvimento para centralizar as definições de tipos essenciais.

**Principais Componentes:**

- **PreprocessorPosition**: Estrutura avançada para rastreamento de posição no código fonte
  - Mantém informações tanto do arquivo original quanto do código expandido
  - Essencial para mapeamento preciso de erros e debugging
  - Suporta múltiplos construtores para compatibilidade
  - Campos principais:
    - `line`, `column`: Posição atual (compatibilidade)
    - `original_line`, `original_column`: Posição no arquivo original
    - `expanded_line`, `expanded_column`: Posição no código expandido
    - `filename`, `original_file`: Nomes dos arquivos
    - `offset`: Offset para source mapping

**Importância no Sistema:**
- **Rastreamento de Origem**: Permite mapear erros do código expandido de volta ao código original
- **Debugging Avançado**: Facilita a depuração de macros complexas e inclusões aninhadas
- **Compatibilidade**: Mantém interface compatível com o analisador léxico existente
- **Extensibilidade**: Base para futuras funcionalidades de source mapping

#### Testes (tests/)
- **unit/**: Testes isolados de cada componente
- **integration/**: Testes de integração com código C real
- **data/**: Arquivos de exemplo para validação

### Flexibilidade e Evolução da Estrutura

#### Estrutura Base Estável
A estrutura foi projetada para ser **estável e completa** desde o início do desenvolvimento. Os componentes principais (preprocessor, macro_processor, file_manager, conditional_processor) representam os pilares fundamentais de qualquer pré-processador C e não devem sofrer alterações estruturais significativas.

#### Pontos de Extensibilidade Planejados
**Arquivos que podem ser expandidos sem alteração estrutural:**
- **data/directives/**: Novos arquivos para versões futuras do C
- **tests/data/**: Novos casos de teste conforme necessário
- **src/preprocessor_config.cpp**: Novas configurações sem alterar interface
- **include/preprocessor_state.hpp**: Novos estados para features específicas

#### Possíveis Ajustes Menores Durante Desenvolvimento
**Cenários que podem requerer pequenos ajustes:**
1. **Otimizações de Performance**: 
   - Cache de arquivos incluídos em `file_manager.cpp`
   - Otimização de expansão de macros em `macro_processor.cpp`

2. **Features Descobertas Durante Implementação**:
   - Novos tipos de diretiva em `directive.hpp`
   - Estados adicionais para casos edge em `preprocessor_state.hpp`

3. **Melhorias de Debugging**:
   - Níveis de log adicionais em `preprocessor_logger.hpp`
   - Métricas de performance opcionais

#### Princípios de Estabilidade
**Para manter a estrutura estável:**
- **Interfaces bem definidas**: Headers não devem mudar após implementação inicial
- **Separação clara**: Cada arquivo tem responsabilidade única e bem definida
- **Extensibilidade por configuração**: Novos comportamentos via arquivos de dados
- **Testes abrangentes**: Validação contínua da estrutura

#### Conclusão sobre Estabilidade
**A estrutura é 95% definitiva** e foi projetada considerando:
- Experiência em desenvolvimento de compiladores
- Padrões estabelecidos na literatura
- Necessidades de todas as versões do C (C89-C23)
- Requisitos de manutenibilidade e testabilidade

**Mudanças esperadas são mínimas** e se limitarão a:
- Ajustes internos de implementação
- Adição de arquivos de dados
- Expansão de casos de teste
- Otimizações de performance pontuais

## Integração com Outros Componentes do Compilador

### Preparação para Integração

A estrutura do pré-processador foi projetada especificamente para **integração perfeita** com o analisador léxico existente e demais componentes do compilador. Esta seção detalha como a arquitetura suporta essa integração.

### Interfaces de Integração

#### 1. Interface com o Analisador Léxico
**Pontos de Integração:**
- **Código Processado**: O preprocessor gera código expandido para o lexer
- **Mapeamento de Posições**: Preservação de informações de linha/coluna originais
- **Tratamento de Erros**: Coordenação entre erros de preprocessamento e léxicos
- **Configuração Compartilhada**: Versão do C e configurações comuns

**Fluxo de Dados:**
```
Source Code → Preprocessor → Expanded Code → Lexer → Token Stream
```

#### 2. Interface com o Sistema de Arquivos
**Pontos de Integração:**
- **Inclusão de Arquivos**: Resolução de caminhos e leitura de headers
- **Dependências**: Rastreamento de arquivos incluídos
- **Cache**: Sistema de cache para arquivos frequentemente incluídos
- **Segurança**: Validação de caminhos e prevenção de inclusões circulares

#### 3. Interface com o Sistema de Erros
**Pontos de Integração:**
- **Error Handler Compartilhado**: Mesmo sistema do analisador léxico
- **Context Preservation**: Informações de contexto de preprocessamento
- **Recovery Strategies**: Estratégias coordenadas de recuperação
- **Unified Reporting**: Relatórios consolidados de todos os componentes

### Estruturas de Dados Compartilhadas

#### Position e Source Location
**Usado por todos os componentes:**
```cpp
struct PreprocessorPosition {
    int original_line;      // Linha no arquivo original
    int original_column;    // Coluna no arquivo original
    int expanded_line;      // Linha no código expandido
    int expanded_column;    // Coluna no código expandido
    string filename;        // Nome do arquivo atual
    string original_file;   // Arquivo original (para includes)
    int offset;            // Para source mapping
};
```

#### Macro Information
**Base para expansão e debugging:**
```cpp
struct MacroInfo {
    string name;
    vector<string> parameters;  // Para macros com parâmetros
    string definition;
    PreprocessorPosition definition_pos;
    vector<PreprocessorPosition> expansions;
    bool is_function_like;
    bool is_predefined;
};
```

#### Include Information
**Estrutura de dependências:**
```cpp
struct IncludeInfo {
    string included_file;
    string including_file;
    PreprocessorPosition include_pos;
    bool is_system_header;
    vector<string> search_paths;
};
```

### Arquitetura de Comunicação

#### Padrão Pipeline Estendido
**Fluxo de Dados:**
```
Source Code → Preprocessor → Expanded Code → Lexer → Token Stream → Parser → AST
```

**Características do Design:**
- **Preprocessing First**: Preprocessamento completo antes da análise léxica
- **Position Mapping**: Mapeamento bidirecional de posições
- **Error Coordination**: Coordenação de erros entre fases
- **Modular**: Preprocessor pode ser desabilitado para debugging

#### Sistema de Configuração Integrado
**Coordenação entre Componentes:**
- **Shared Config**: Configurações compartilhadas entre preprocessor e lexer
- **Version Consistency**: Garantia de consistência de versão do C
- **Feature Flags**: Controle granular de features por componente

## Arquitetura Modular

### 1. Componentes Principais

#### Classe Preprocessor (preprocessor.hpp/cpp)
**Responsabilidades:**
- Coordenação geral do processamento
- Leitura e escrita de arquivos
- Integração com componentes especializados
- Controle de fluxo de processamento

**Características Técnicas:**
- Buffer de entrada e saída
- Sistema de mapeamento de posições
- Suporte a múltiplas versões do padrão C
- Sistema de configuração flexível
- Rastreamento de dependências

**Interface Pública:**
- `process(filename)` - Processar arquivo principal
- `processString(content)` - Processar string de código
- `getExpandedCode()` - Obter código expandido
- `getDependencies()` - Obter lista de dependências
- `reset()` - Reinicializar estado

### 2. Módulos Especializados

#### Processador de Macros (macro_processor.hpp/cpp)
**Responsabilidades:**
- Definição e armazenamento de macros
- Expansão de macros simples e com parâmetros
- Detecção de recursão infinita
- Gerenciamento de macros predefinidas

**Funcionalidades:**
- **Macros Simples**: `#define PI 3.14159`
- **Macros com Parâmetros**: `#define MAX(a,b) ((a)>(b)?(a):(b))`
- **Macros Predefinidas**: `__FILE__`, `__LINE__`, `__DATE__`, `__TIME__`
- **Operadores Especiais**: `#` (stringification), `##` (concatenation)
- **Macros Condicionais**: `#undef`, redefinição

**Estrutura Macro:**
- Nome da macro
- Lista de parâmetros (se aplicável)
- Corpo da definição
- Posição de definição
- Flags de controle (predefinida, função, etc.)

#### Gerenciador de Arquivos (file_manager.hpp/cpp)
**Responsabilidades:**
- Resolução de caminhos de inclusão
- Leitura de arquivos de cabeçalho
- Prevenção de inclusões circulares
- Cache de arquivos frequentemente incluídos

**Tipos de Inclusão:**
- **System Headers**: `#include <stdio.h>`
- **Local Headers**: `#include "myheader.h"`
- **Caminhos Relativos**: Resolução baseada no arquivo atual
- **Caminhos de Busca**: Diretórios de sistema e usuário

**Funcionalidades:**
- Detecção de inclusões circulares
- Cache inteligente de headers
- Suporte a múltiplos caminhos de busca
- Tratamento de erros de arquivo não encontrado
- Estatísticas de inclusão

#### Processador Condicional (conditional_processor.hpp/cpp)
**Responsabilidades:**
- Processamento de diretivas condicionais
- Avaliação de expressões de teste
- Gerenciamento de blocos aninhados
- Controle de compilação condicional

**Diretivas Suportadas:**
- `#ifdef MACRO` - Teste de definição de macro
- `#ifndef MACRO` - Teste de não definição
- `#if expression` - Avaliação de expressão constante
- `#elif expression` - Else-if condicional
- `#else` - Alternativa
- `#endif` - Fim do bloco condicional

**Funcionalidades:**
- Stack de contextos condicionais
- Avaliação de expressões complexas
- Suporte a aninhamento profundo
- Detecção de blocos não fechados
- Otimização de blocos sempre verdadeiros/falsos

#### Avaliador de Expressões (expression_evaluator.hpp/cpp)
**Responsabilidades:**
- Avaliação de expressões constantes
- Suporte a operadores aritméticos e lógicos
- Resolução de macros em expressões
- Tratamento de erros de avaliação

**Operadores Suportados:**
- **Aritméticos**: `+`, `-`, `*`, `/`, `%`
- **Relacionais**: `<`, `>`, `<=`, `>=`, `==`, `!=`
- **Lógicos**: `&&`, `||`, `!`
- **Bitwise**: `&`, `|`, `^`, `~`, `<<`, `>>`
- **Especiais**: `defined(MACRO)`, `sizeof`

**Funcionalidades:**
- Parser de expressões com precedência
- Avaliação em tempo de compilação
- Suporte a constantes inteiras e caracteres
- Expansão de macros em expressões
- Relatório de erros detalhado

## Estruturas de Dados Principais

### Máquina de Estados do Preprocessor
**Conceito:** Sistema de estados para controle do processamento

**Estados Principais:**
- **NORMAL**: Processamento normal de código
- **DIRECTIVE**: Processando diretiva de preprocessamento
- **MACRO_DEFINITION**: Definindo macro
- **MACRO_EXPANSION**: Expandindo macro
- **CONDITIONAL_TRUE**: Dentro de bloco condicional verdadeiro
- **CONDITIONAL_FALSE**: Dentro de bloco condicional falso
- **CONDITIONAL_SKIP**: Pulando bloco condicional
- **INCLUDE_PROCESSING**: Processando arquivo incluído
- **ERROR_RECOVERY**: Recuperando de erro

### Tabela de Macros
**Estrutura:** Hash table otimizada para busca rápida

**Características:**
- Busca O(1) para macros definidas
- Suporte a redefinição com warning
- Escopo global e local
- Macros predefinidas imutáveis
- Estatísticas de uso

### Stack de Contextos
**Estrutura:** Pilha para gerenciamento de contexto

**Contextos:**
- Arquivo atual sendo processado
- Posição no arquivo
- Estado de processamento condicional
- Macros locais ativas
- Nível de aninhamento

### Buffer de Saída
**Estrutura:** Buffer eficiente para código expandido

**Características:**
- Crescimento dinâmico
- Mapeamento de posições
- Preservação de formatação
- Otimização de memória
- Suporte a streaming

## Considerações de Implementação

### Arquitetura Extensível
**Princípios de Design:**
- Separação clara de responsabilidades entre módulos
- Interfaces bem definidas para facilitar testes
- Configuração flexível para diferentes versões do C
- Sistema de plugins para extensões futuras

### Performance e Otimização
**Estratégias:**
- Cache de arquivos incluídos para reduzir I/O
- Hash tables otimizadas para lookup de macros
- Processamento streaming para arquivos grandes
- Minimização de alocações dinâmicas
- Otimização de expansão de macros frequentes

### Manutenibilidade
**Aspectos Importantes:**
- Código bem documentado com exemplos
- Testes abrangentes para cada funcionalidade
- Logging detalhado para depuração
- Estrutura modular para facilitar modificações
- Compatibilidade com ferramentas de debugging

## Especificação de Métodos por Arquivo

### Headers (include/)

#### preprocessor.hpp
**Classe Preprocessor:**
- `Preprocessor(const std::string& config_file)` - Construtor com arquivo de configuração
- `~Preprocessor()` - Destrutor que libera recursos
- `bool process(const std::string& filename)` - Processa arquivo principal
- `bool processString(const std::string& content)` - Processa string de código
- `std::string getExpandedCode() const` - Retorna código expandido
- `std::vector<std::string> getDependencies() const` - Lista de arquivos incluídos
- `void reset()` - Reinicializa estado do preprocessor
- `void setSearchPaths(const std::vector<std::string>& paths)` - Define caminhos de busca
- `void defineMacro(const std::string& name, const std::string& value)` - Define macro programaticamente
- `void undefineMacro(const std::string& name)` - Remove definição de macro
- `bool isMacroDefined(const std::string& name) const` - Verifica se macro está definida
- `void setVersion(CVersion version)` - Configura versão do padrão C
- `PreprocessorStats getStatistics() const` - Retorna estatísticas de processamento

#### directive.hpp
**Enum DirectiveType:**
- Definições de todos os tipos de diretivas suportadas

**Classe Directive:**
- `Directive(DirectiveType type, const std::string& content, PreprocessorPosition pos)` - Construtor básico
- `DirectiveType getType() const` - Retorna tipo da diretiva
- `std::string getContent() const` - Retorna conteúdo da diretiva
- `PreprocessorPosition getPosition() const` - Retorna posição da diretiva
- `std::vector<std::string> getArguments() const` - Retorna argumentos da diretiva
- `bool isValid() const` - Verifica se diretiva é válida
- `std::string toString() const` - Converte para representação string

#### macro_processor.hpp
**Classe MacroProcessor:**
- `MacroProcessor(PreprocessorLogger* logger)` - Construtor com logger
- `~MacroProcessor()` - Destrutor
- `bool defineMacro(const std::string& name, const std::string& definition, const PreprocessorPosition& pos)` - Define macro simples
- `bool defineFunctionMacro(const std::string& name, const std::vector<std::string>& params, const std::string& definition, const PreprocessorPosition& pos)` - Define macro com parâmetros
- `bool undefineMacro(const std::string& name)` - Remove definição de macro
- `bool isMacroDefined(const std::string& name) const` - Verifica definição
- `std::string expandMacro(const std::string& name, const std::vector<std::string>& args, const PreprocessorPosition& pos)` - Expande macro
- `std::string expandString(const std::string& input, const PreprocessorPosition& pos)` - Expande todas as macros em string
- `void loadPredefinedMacros(CVersion version)` - Carrega macros predefinidas
- `MacroInfo getMacroInfo(const std::string& name) const` - Obtém informações da macro
- `std::vector<std::string> getDefinedMacros() const` - Lista todas as macros definidas
- `void clearUserMacros()` - Remove todas as macros do usuário
- `MacroStats getStatistics() const` - Retorna estatísticas de uso

#### file_manager.hpp
**Classe FileManager:**
- `FileManager(const std::vector<std::string>& search_paths, PreprocessorLogger* logger)` - Construtor com caminhos
- `~FileManager()` - Destrutor
- `std::string resolveInclude(const std::string& filename, bool is_system, const std::string& current_file)` - Resolve caminho de inclusão
- `std::string readFile(const std::string& filepath)` - Lê conteúdo do arquivo
- `bool fileExists(const std::string& filepath) const` - Verifica existência
- `void addSearchPath(const std::string& path)` - Adiciona caminho de busca
- `void setSearchPaths(const std::vector<std::string>& paths)` - Define caminhos de busca
- `std::vector<std::string> getSearchPaths() const` - Retorna caminhos configurados
- `bool checkCircularInclusion(const std::string& filepath, const std::vector<std::string>& include_stack)` - Verifica inclusão circular
- `void clearCache()` - Limpa cache de arquivos
- `FileStats getStatistics() const` - Retorna estatísticas de arquivos
- `std::vector<std::string> getDependencies() const` - Lista dependências

#### conditional_processor.hpp
**Classe ConditionalProcessor:**
- `ConditionalProcessor(MacroProcessor* macro_proc, ExpressionEvaluator* expr_eval, PreprocessorLogger* logger)` - Construtor com dependências
- `~ConditionalProcessor()` - Destrutor
- `bool processIfdef(const std::string& macro_name, const PreprocessorPosition& pos)` - Processa #ifdef
- `bool processIfndef(const std::string& macro_name, const PreprocessorPosition& pos)` - Processa #ifndef
- `bool processIf(const std::string& expression, const PreprocessorPosition& pos)` - Processa #if
- `bool processElif(const std::string& expression, const PreprocessorPosition& pos)` - Processa #elif
- `bool processElse(const PreprocessorPosition& pos)` - Processa #else
- `bool processEndif(const PreprocessorPosition& pos)` - Processa #endif
- `bool shouldIncludeCode() const` - Verifica se código atual deve ser incluído
- `int getCurrentNestingLevel() const` - Retorna nível de aninhamento
- `bool hasOpenConditionals() const` - Verifica condicionais não fechadas
- `void reset()` - Reinicializa estado
- `ConditionalStats getStatistics() const` - Retorna estatísticas

#### expression_evaluator.hpp
**Classe ExpressionEvaluator:**
- `ExpressionEvaluator(MacroProcessor* macro_proc, PreprocessorLogger* logger)` - Construtor com dependências
- `~ExpressionEvaluator()` - Destrutor
- `long long evaluateExpression(const std::string& expression, const PreprocessorPosition& pos)` - Avalia expressão
- `bool evaluateBooleanExpression(const std::string& expression, const PreprocessorPosition& pos)` - Avalia como booleano
- `bool isValidExpression(const std::string& expression) const` - Verifica validade
- `std::string expandMacrosInExpression(const std::string& expression, const PreprocessorPosition& pos)` - Expande macros
- `void setStrictMode(bool strict)` - Define modo estrito de avaliação
- `ExpressionStats getStatistics() const` - Retorna estatísticas

#### preprocessor_config.hpp
**Classe PreprocessorConfig:**
- `PreprocessorConfig()` - Construtor padrão
- `~PreprocessorConfig()` - Destrutor
- `void setCStandard(CStandard standard)` - Define padrão C (C89, C99, C11, C17, C23)
- `CStandard getCStandard() const` - Obtém padrão C atual
- `void enableFeature(PreprocessorFeature feature)` - Habilita feature específica
- `void disableFeature(PreprocessorFeature feature)` - Desabilita feature específica
- `bool isFeatureEnabled(PreprocessorFeature feature) const` - Verifica se feature está habilitada
- `void setMaxIncludeDepth(size_t depth)` - Define profundidade máxima de includes
- `size_t getMaxIncludeDepth() const` - Obtém profundidade máxima de includes
- `void setMaxMacroExpansionDepth(size_t depth)` - Define profundidade máxima de expansão de macros
- `size_t getMacroExpansionDepth() const` - Obtém profundidade máxima de expansão
- `void addIncludePath(const std::string& path)` - Adiciona caminho de busca
- `void removeIncludePath(const std::string& path)` - Remove caminho de busca
- `std::vector<std::string> getIncludePaths() const` - Obtém caminhos de busca
- `void setWarningLevel(WarningLevel level)` - Define nível de warnings
- `WarningLevel getWarningLevel() const` - Obtém nível de warnings
- `void enableWarning(WarningType type)` - Habilita warning específico
- `void disableWarning(WarningType type)` - Desabilita warning específico
- `bool isWarningEnabled(WarningType type) const` - Verifica se warning está habilitado
- `void loadFromFile(const std::string& configFile)` - Carrega configuração de arquivo
- `void saveToFile(const std::string& configFile) const` - Salva configuração em arquivo
- `void reset()` - Restaura configurações padrão
- `ConfigStats getStatistics() const` - Retorna estatísticas de configuração

#### preprocessor_state.hpp
**Classe PreprocessorState:**
- `PreprocessorState()` - Construtor padrão
- `~PreprocessorState()` - Destrutor
- `void pushState(ProcessingState state)` - Empilha novo estado
- `ProcessingState popState()` - Desempilha estado atual
- `ProcessingState getCurrentState() const` - Obtém estado atual
- `bool isEmpty() const` - Verifica se pilha está vazia
- `size_t getDepth() const` - Obtém profundidade da pilha
- `void setFileContext(const FileContext& context)` - Define contexto de arquivo
- `FileContext getFileContext() const` - Obtém contexto de arquivo
- `void pushFileContext(const std::string& filename, size_t line, size_t column)` - Empilha contexto de arquivo
- `void popFileContext()` - Desempilha contexto de arquivo
- `void setCurrentLine(size_t line)` - Define linha atual
- `void setCurrentColumn(size_t column)` - Define coluna atual
- `size_t getCurrentLine() const` - Obtém linha atual
- `size_t getCurrentColumn() const` - Obtém coluna atual
- `void incrementLine()` - Incrementa linha atual
- `void incrementColumn()` - Incrementa coluna atual
- `void resetColumn()` - Reseta coluna para início da linha
- `void setProcessingMode(ProcessingMode mode)` - Define modo de processamento
- `ProcessingMode getProcessingMode() const` - Obtém modo de processamento
- `void enableConditionalBlock()` - Habilita bloco condicional
- `void disableConditionalBlock()` - Desabilita bloco condicional
- `bool isInConditionalBlock() const` - Verifica se está em bloco condicional
- `void setErrorState(bool hasError)` - Define estado de erro
- `bool hasError() const` - Verifica se há erro
- `void addProcessedFile(const std::string& filename)` - Adiciona arquivo processado
- `bool wasFileProcessed(const std::string& filename) const` - Verifica se arquivo foi processado
- `std::vector<std::string> getProcessedFiles() const` - Obtém arquivos processados
- `void reset()` - Reinicializa estado
- `StateStats getStatistics() const` - Retorna estatísticas de estado

#### preprocessor_logger.hpp
**Classe PreprocessorLogger:**
- `PreprocessorLogger()` - Construtor padrão
- `~PreprocessorLogger()` - Destrutor
- `void setLogLevel(LogLevel level)` - Define nível de log
- `LogLevel getLogLevel() const` - Obtém nível de log
- `void setOutputFile(const std::string& filename)` - Define arquivo de saída
- `void setOutputStream(std::ostream* stream)` - Define stream de saída
- `void enableConsoleOutput(bool enable)` - Habilita/desabilita saída no console
- `void enableFileOutput(bool enable)` - Habilita/desabilita saída em arquivo
- `void logDebug(const std::string& message, const PreprocessorPosition& pos = {})` - Log de debug
- `void logInfo(const std::string& message, const PreprocessorPosition& pos = {})` - Log de informação
- `void logWarning(const std::string& message, const PreprocessorPosition& pos = {})` - Log de warning
- `void logError(const std::string& message, const PreprocessorPosition& pos = {})` - Log de erro
- `void logFatal(const std::string& message, const PreprocessorPosition& pos = {})` - Log de erro fatal
- `void logMacroExpansion(const std::string& macroName, const std::string& expansion, const PreprocessorPosition& pos)` - Log de expansão de macro
- `void logFileInclude(const std::string& filename, const PreprocessorPosition& pos)` - Log de inclusão de arquivo
- `void logConditionalDirective(const std::string& directive, bool condition, const PreprocessorPosition& pos)` - Log de diretiva condicional
- `void logDirectiveProcessing(const std::string& directive, const PreprocessorPosition& pos)` - Log de processamento de diretiva
- `void startTimer(const std::string& operation)` - Inicia timer para operação
- `void endTimer(const std::string& operation)` - Finaliza timer para operação
- `double getElapsedTime(const std::string& operation) const` - Obtém tempo decorrido
- `void logPerformanceStats()` - Log de estatísticas de performance
- `void enableTimestamp(bool enable)` - Habilita/desabilita timestamp
- `void enableSourceLocation(bool enable)` - Habilita/desabilita localização no código
- `void setLogFormat(LogFormat format)` - Define formato de log
- `void flush()` - Força escrita do buffer
- `void reset()` - Reinicializa logger
- `LoggerStats getStatistics() const` - Retorna estatísticas do logger

### Implementações (src/)

#### preprocessor.cpp
**Métodos de Controle:**
- `initializeComponents()` - Inicializa componentes internos
- `processFile(const std::string& filepath)` - Processa arquivo específico
- `processLine(const std::string& line, int line_number)` - Processa linha individual
- `handleDirective(const Directive& directive)` - Manipula diretiva específica
- `writeOutput(const std::string& content)` - Escreve no buffer de saída
- `updatePositionMapping(const PreprocessorPosition& original, const PreprocessorPosition& expanded)` - Atualiza mapeamento

#### directive.cpp
**Métodos de Processamento de Diretivas:**
- `parseDirective(const std::string& line, size_t line_number)` - Analisa e identifica diretiva
- `validateDirectiveSyntax(const Directive& directive)` - Valida sintaxe da diretiva
- `processIncludeDirective(const std::string& filename, bool is_system_header, const PreprocessorPosition& pos)` - Processa #include
- `processDefineDirective(const std::string& definition, const PreprocessorPosition& pos)` - Processa #define
- `processUndefDirective(const std::string& macro_name, const PreprocessorPosition& pos)` - Processa #undef
- `processIfDirective(const std::string& condition, const PreprocessorPosition& pos)` - Processa #if
- `processIfdefDirective(const std::string& macro_name, const PreprocessorPosition& pos)` - Processa #ifdef
- `processIfndefDirective(const std::string& macro_name, const PreprocessorPosition& pos)` - Processa #ifndef
- `processElseDirective(const PreprocessorPosition& pos)` - Processa #else
- `processElifDirective(const std::string& condition, const PreprocessorPosition& pos)` - Processa #elif
- `processEndifDirective(const PreprocessorPosition& pos)` - Processa #endif
- `processPragmaDirective(const std::string& pragma_text, const PreprocessorPosition& pos)` - Processa #pragma
- `processErrorDirective(const std::string& message, const PreprocessorPosition& pos)` - Processa #error
- `processWarningDirective(const std::string& message, const PreprocessorPosition& pos)` - Processa #warning
- `processLineDirective(const std::string& line_info, const PreprocessorPosition& pos)` - Processa #line
- `extractDirectiveName(const std::string& line)` - Extrai nome da diretiva
- `extractDirectiveArguments(const std::string& line, const std::string& directive_name)` - Extrai argumentos
- `normalizeDirectiveLine(const std::string& line)` - Normaliza linha de diretiva
- `handleDirectiveErrors(const std::string& error_msg, const PreprocessorPosition& pos)` - Manipula erros de diretiva
- `validateDirectiveContext(DirectiveType type, const PreprocessorState& state)` - Valida contexto da diretiva
- `updateDirectiveStatistics(DirectiveType type)` - Atualiza estatísticas de uso
- `logDirectiveProcessing(const Directive& directive, const PreprocessorPosition& pos)` - Log de processamento
- `handleUnknownDirective(const std::string& directive_name, const PreprocessorPosition& pos)` - Manipula diretiva desconhecida
- `processConditionalChain(const std::vector<Directive>& chain)` - Processa cadeia condicional
- `validateConditionalNesting(const Directive& directive)` - Valida aninhamento condicional
- `optimizeDirectiveProcessing(const std::vector<Directive>& directives)` - Otimiza processamento

#### macro_processor.cpp
**Métodos de Processamento:**
- `parseDefinition(const std::string& definition)` - Analisa definição de macro
- `expandSimpleMacro(const MacroInfo& macro, const PreprocessorPosition& pos)` - Expande macro simples
- `expandFunctionMacro(const MacroInfo& macro, const std::vector<std::string>& args, const PreprocessorPosition& pos)` - Expande macro com parâmetros
- `handleStringification(const std::string& arg)` - Processa operador #
- `handleConcatenation(const std::string& left, const std::string& right)` - Processa operador ##
- `detectRecursion(const std::string& macro_name, const std::vector<std::string>& expansion_stack)` - Detecta recursão
- `validateMacroName(const std::string& name)` - Valida nome de macro
- `validateMacroDefinition(const std::string& definition)` - Valida definição

#### file_manager.cpp
**Métodos de Gerenciamento:**
- `searchInPaths(const std::string& filename, const std::vector<std::string>& paths)` - Busca em caminhos
- `resolveRelativePath(const std::string& filename, const std::string& base_path)` - Resolve caminho relativo
- `cacheFile(const std::string& filepath, const std::string& content)` - Armazena em cache
- `getCachedFile(const std::string& filepath)` - Recupera do cache
- `validateFilePath(const std::string& filepath)` - Valida caminho
- `normalizeFilePath(const std::string& filepath)` - Normaliza caminho
- `updateDependencies(const std::string& filepath)` - Atualiza lista de dependências

#### conditional_processor.cpp
**Métodos de Processamento:**
- `pushConditionalContext(bool condition, ConditionalType type)` - Adiciona contexto
- `popConditionalContext()` - Remove contexto
- `getCurrentContext()` - Obtém contexto atual
- `evaluateCondition(const std::string& condition, const PreprocessorPosition& pos)` - Avalia condição
- `validateConditionalStructure()` - Valida estrutura de condicionais
- `handleNestedConditionals(const Directive& directive)` - Manipula aninhamento

#### expression_evaluator.cpp
**Métodos de Avaliação:**
- `tokenizeExpression(const std::string& expression)` - Tokeniza expressão
- `parseExpression(const std::vector<Token>& tokens)` - Analisa expressão
- `evaluateOperator(const std::string& op, long long left, long long right)` - Avalia operador
- `evaluateUnaryOperator(const std::string& op, long long operand)` - Avalia operador unário
- `handleDefinedOperator(const std::string& macro_name)` - Processa defined()
- `convertToInteger(const std::string& value)` - Converte para inteiro
- `validateOperatorPrecedence(const std::vector<Token>& tokens)` - Valida precedência

#### preprocessor_config.cpp
**Métodos de Configuração:**
- `initializeDefaultConfig()` - Inicializa configurações padrão
- `loadConfigFromFile(const std::string& filepath)` - Carrega configuração de arquivo
- `parseConfigLine(const std::string& line)` - Analisa linha de configuração
- `validateConfigValue(const std::string& key, const std::string& value)` - Valida valor de configuração
- `loadConfiguration(const std::string& filepath)` - Carrega configuração de arquivo
- `validateConfiguration()` - Valida configuração atual
- `applyConfigChanges()` - Aplica mudanças de configuração
- `saveConfigToFile(const std::string& filepath)` - Salva configuração em arquivo
- `mergeConfigurations(const PreprocessorConfig& other)` - Mescla configurações
- `resetToDefaults()` - Restaura configurações padrão
- `validateCStandardCompatibility(CStandard standard)` - Valida compatibilidade do padrão C
- `updateFeatureFlags(CStandard standard)` - Atualiza flags de features
- `resolveIncludePaths()` - Resolve caminhos de inclusão
- `validateIncludePath(const std::string& path)` - Valida caminho de inclusão
- `normalizeIncludePath(const std::string& path)` - Normaliza caminho de inclusão
- `handleEnvironmentVariables()` - Manipula variáveis de ambiente
- `generateConfigReport()` - Gera relatório de configuração

#### preprocessor_state.cpp
**Métodos de Estado:**
- `initializeStateStack()` - Inicializa pilha de estados
- `validateStateTransition(ProcessingState from, ProcessingState to)` - Valida transição de estado
- `saveCurrentState()` - Salva estado atual
- `restoreState(const ProcessingState& state)` - Restaura estado
- `updateFilePosition(size_t line, size_t column)` - Atualiza posição no arquivo
- `calculateRelativePosition(const FileContext& context)` - Calcula posição relativa
- `handleFileContextChange(const std::string& newFile)` - Manipula mudança de contexto de arquivo
- `validateFileContext(const FileContext& context)` - Valida contexto de arquivo
- `mergeFileContexts(const FileContext& parent, const FileContext& child)` - Mescla contextos de arquivo
- `trackProcessingMode(ProcessingMode mode)` - Rastreia modo de processamento
- `handleConditionalNesting(bool condition)` - Manipula aninhamento condicional
- `validateConditionalDepth()` - Valida profundidade condicional
- `updateErrorContext(const std::string& errorMsg)` - Atualiza contexto de erro
- `generateStateReport()` - Gera relatório de estado
- `optimizeStateStorage()` - Otimiza armazenamento de estado

#### preprocessor_logger.cpp
**Métodos de Logging:**
- `initializeLogger()` - Inicializa sistema de logging
- `formatLogMessage(LogLevel level, const std::string& message, const PreprocessorPosition& pos)` - Formata mensagem de log
- `writeToFile(const std::string& message)` - Escreve em arquivo
- `writeToConsole(const std::string& message)` - Escreve no console
- `rotateLogFile()` - Rotaciona arquivo de log
- `validateLogLevel(LogLevel level)` - Valida nível de log
- `filterMessage(const std::string& message, LogLevel level)` - Filtra mensagem
- `addTimestamp(const std::string& message)` - Adiciona timestamp
- `addSourceLocation(const std::string& message, const PreprocessorPosition& pos)` - Adiciona localização no código
- `handleLogBuffering()` - Manipula buffer de log
- `flushLogBuffer()` - Descarrega buffer de log
- `compressOldLogs()` - Comprime logs antigos
- `calculateLogStatistics()` - Calcula estatísticas de log
- `handleLogErrors()` - Manipula erros de logging
- `optimizeLogPerformance()` - Otimiza performance de logging
- `generateLogReport()` - Gera relatório de logging
- `validateLogFormat(LogFormat format)` - Valida formato de log
- `handleAsyncLogging()` - Manipula logging assíncrono
- `synchronizeLogOutputs()` - Sincroniza saídas de log
- `cleanupLogResources()` - Limpa recursos de logging

### Arquivos de Teste (tests/unit/)

#### test_preprocessor.cpp
**Métodos de Teste do Preprocessor Principal:**
- `testPreprocessorInitialization()` - Testa inicialização do pré-processador
- `testBasicFileProcessing()` - Testa processamento básico de arquivos
- `testMultipleFileProcessing()` - Testa processamento de múltiplos arquivos
- `testErrorHandling()` - Testa tratamento de erros
- `testConfigurationLoading()` - Testa carregamento de configurações
- `testOutputGeneration()` - Testa geração de saída
- `testPositionMapping()` - Testa mapeamento de posições
- `testIntegrationWithComponents()` - Testa integração entre componentes
- `testPerformanceMetrics()` - Testa métricas de performance
- `testMemoryManagement()` - Testa gerenciamento de memória

#### test_macros.cpp
**Métodos de Teste de Macros:**
- `testSimpleMacroDefinition()` - Testa definição de macros simples
- `testFunctionMacroDefinition()` - Testa definição de macros com parâmetros
- `testMacroExpansion()` - Testa expansão de macros
- `testNestedMacroExpansion()` - Testa expansão aninhada de macros
- `testMacroRecursionDetection()` - Testa detecção de recursão
- `testStringificationOperator()` - Testa operador # (stringification)
- `testConcatenationOperator()` - Testa operador ## (concatenação)
- `testPredefinedMacros()` - Testa macros predefinidas
- `testMacroUndefinition()` - Testa remoção de macros
- `testMacroRedefinition()` - Testa redefinição de macros
- `testVariadicMacros()` - Testa macros variádicas
- `testMacroErrorHandling()` - Testa tratamento de erros em macros

#### test_includes.cpp
**Métodos de Teste de Inclusão de Arquivos:**
- `testBasicInclude()` - Testa inclusão básica de arquivos
- `testSystemHeaderInclude()` - Testa inclusão de headers do sistema
- `testRelativePathInclude()` - Testa inclusão com caminhos relativos
- `testAbsolutePathInclude()` - Testa inclusão com caminhos absolutos
- `testNestedIncludes()` - Testa inclusões aninhadas
- `testCircularIncludeDetection()` - Testa detecção de inclusão circular
- `testIncludePathResolution()` - Testa resolução de caminhos de inclusão
- `testIncludeGuards()` - Testa guardas de inclusão
- `testIncludeOnce()` - Testa pragma once
- `testIncludeErrorHandling()` - Testa tratamento de erros de inclusão
- `testIncludeCaching()` - Testa cache de arquivos incluídos
- `testIncludeDependencyTracking()` - Testa rastreamento de dependências

#### test_conditionals.cpp
**Métodos de Teste de Compilação Condicional:**
- `testIfDirective()` - Testa diretiva #if
- `testIfdefDirective()` - Testa diretiva #ifdef
- `testIfndefDirective()` - Testa diretiva #ifndef
- `testElseDirective()` - Testa diretiva #else
- `testElifDirective()` - Testa diretiva #elif
- `testEndifDirective()` - Testa diretiva #endif
- `testNestedConditionals()` - Testa condicionais aninhadas
- `testConditionalExpressions()` - Testa expressões condicionais
- `testConditionalErrorHandling()` - Testa tratamento de erros condicionais
- `testConditionalNestingValidation()` - Testa validação de aninhamento
- `testConditionalBlockSkipping()` - Testa pulo de blocos condicionais
- `testConditionalMacroInteraction()` - Testa interação com macros

#### test_expressions.cpp
**Métodos de Teste de Avaliação de Expressões:**
- `testBasicArithmetic()` - Testa operações aritméticas básicas
- `testLogicalOperations()` - Testa operações lógicas
- `testBitwiseOperations()` - Testa operações bitwise
- `testComparisonOperations()` - Testa operações de comparação
- `testUnaryOperations()` - Testa operações unárias
- `testOperatorPrecedence()` - Testa precedência de operadores
- `testParenthesesGrouping()` - Testa agrupamento com parênteses
- `testConstantEvaluation()` - Testa avaliação de constantes
- `testMacroInExpressions()` - Testa macros em expressões
- `testExpressionErrorHandling()` - Testa tratamento de erros em expressões
- `testComplexExpressions()` - Testa expressões complexas
- `testTypeConversions()` - Testa conversões de tipo

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
3. loadConfiguration() - Carregamento de configurações
4. validateConfiguration() - Validação de configurações
```
*Tempo estimado: 1 dia*
*Marco: Sistema de configuração operacional*

**1.4 Sistema de Logging (Prioridade: ALTA)**
```cpp
// preprocessor_logger.hpp/cpp - Ordem de implementação:
1. enum LogLevel - Níveis de log
2. class PreprocessorLogger - Logger principal
3. Métodos de formatação de mensagens
4. Integração com sistema de arquivos
```
*Tempo estimado: 1 dia*
*Marco: Sistema de logging funcional*

**1.5 Testes Básicos (Prioridade: ALTA)**
```cpp
// Ordem de implementação:
1. test_directive.cpp - Testes de estruturas básicas
2. test_config.cpp - Testes de configuração
3. test_logger.cpp - Testes de logging
4. Configuração de CI/CD básica
```
*Tempo estimado: 1 dia*
*Marco: Testes básicos passando*

#### **FASE 2: Componentes Principais (Semana 2)**

**2.1 Processador de Macros (Prioridade: CRÍTICA)**
```cpp
// macro_processor.hpp/cpp - Ordem de implementação:
1. struct MacroInfo - Informações de macro
2. class MacroProcessor - Processador principal
3. defineMacro() - Definição de macros simples
4. expandMacro() - Expansão básica
5. Tabela hash para armazenamento
6. Macros predefinidas básicas
```
*Tempo estimado: 2 dias*
*Marco: Macros simples funcionando*

**2.2 Gerenciador de Arquivos (Prioridade: CRÍTICA)**
```cpp
// file_manager.hpp/cpp - Ordem de implementação:
1. class FileManager - Gerenciador principal
2. resolveInclude() - Resolução de caminhos
3. readFile() - Leitura de arquivos
4. Sistema de cache básico
5. Detecção de inclusões circulares
6. Caminhos de busca configuráveis
```
*Tempo estimado: 2 dias*
*Marco: Inclusão de arquivos funcionando*

**2.3 Avaliador de Expressões (Prioridade: ALTA)**
```cpp
// expression_evaluator.hpp/cpp - Ordem de implementação:
1. class ExpressionEvaluator - Avaliador principal
2. tokenizeExpression() - Tokenização de expressões
3. evaluateExpression() - Avaliação básica
4. Operadores aritméticos e lógicos
5. Suporte ao operador defined()
6. Tratamento de erros de avaliação
```
*Tempo estimado: 1 dia*
*Marco: Avaliação de expressões funcionando*

#### **FASE 3: Processamento Condicional (Semana 3)**

**3.1 Processador Condicional (Prioridade: CRÍTICA)**
```cpp
// conditional_processor.hpp/cpp - Ordem de implementação:
1. class ConditionalProcessor - Processador principal
2. Stack de contextos condicionais
3. processIfdef() / processIfndef() - Condicionais simples
4. processIf() - Condicionais com expressões
5. processElse() / processElif() - Alternativas
6. processEndif() - Fechamento de blocos
7. Validação de estrutura aninhada
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
// preprocessor.hpp/cpp - Ordem de implementação:
1. class Preprocessor - Classe principal
2. Integração de todos os componentes
3. process() - Método principal de processamento
4. Coordenação entre módulos
5. Tratamento de erros integrado
6. Mapeamento de posições
```
*Tempo estimado: 1 dia*
*Marco: Preprocessor integrado funcionando*

#### **FASE 4: Otimização e Testes (Semana 4)**

**4.1 Testes Abrangentes (Prioridade: CRÍTICA)**
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

**4.2 Otimizações de Performance (Prioridade: MÉDIA)**
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

**4.3 Documentação e Finalização (Prioridade: ALTA)**
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

**4.4 Integração com Lexer (Prioridade: CRÍTICA)**
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