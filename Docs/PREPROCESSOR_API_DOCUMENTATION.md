# Documentação de APIs do Pré-processador C

## Visão Geral

Este documento fornece documentação completa das APIs públicas do pré-processador C, incluindo todas as classes principais, seus métodos, parâmetros e exemplos de uso.

## Índice

1. [PreprocessorMain](#preprocessormain) - Classe principal do pré-processador
2. [MacroProcessor](#macroprocessor) - Processamento de macros
3. [FileManager](#filemanager) - Gerenciamento de arquivos
4. [DirectiveProcessor](#directiveprocessor) - Processamento de diretivas
5. [ConditionalProcessor](#conditionalprocessor) - Compilação condicional
6. [ExpressionEvaluator](#expressionevaluator) - Avaliação de expressões
7. [Estruturas de Dados](#estruturas-de-dados) - Tipos e estruturas auxiliares
8. [Exemplos de Integração](#exemplos-de-integração)

---

## PreprocessorMain

**Namespace:** `Preprocessor`  
**Header:** `preprocessor.hpp`

### Descrição

Classe principal do pré-processador C, responsável pela coordenação geral do processamento, leitura e escrita de arquivos, integração com componentes especializados e controle de fluxo de processamento.

### Construtores

```cpp
// Construtor com arquivo de configuração opcional
explicit PreprocessorMain(const std::string& config_file = "");
```

**Parâmetros:**
- `config_file`: Caminho para arquivo de configuração (opcional)

### Métodos Principais

#### `bool process(const std::string& filename)`

Processa um arquivo C/C++.

**Parâmetros:**
- `filename`: Caminho do arquivo a ser processado

**Retorno:**
- `true`: Processamento bem-sucedido
- `false`: Erro durante o processamento

**Exemplo:**
```cpp
PreprocessorMain preprocessor;
if (preprocessor.process("main.c")) {
    std::cout << "Processamento concluído com sucesso" << std::endl;
}
```

#### `bool processString(const std::string& content)`

Processa uma string contendo código C/C++.

**Parâmetros:**
- `content`: Conteúdo do código a ser processado

**Retorno:**
- `true`: Processamento bem-sucedido
- `false`: Erro durante o processamento

#### `std::string getExpandedCode() const`

Retorna o código expandido após o processamento.

**Retorno:**
- String com o código expandido

#### `std::vector<std::string> getDependencies() const`

Retorna lista de arquivos incluídos durante o processamento.

**Retorno:**
- Vetor com caminhos dos arquivos incluídos

#### `void reset()`

Reinicializa o estado do pré-processador para um novo processamento.

### Métodos de Configuração

#### `void setSearchPaths(const std::vector<std::string>& paths)`

Define caminhos de busca para inclusões.

**Parâmetros:**
- `paths`: Vetor com caminhos de busca para includes

#### `void defineMacro(const std::string& name, const std::string& value)`

Define uma macro programaticamente.

**Parâmetros:**
- `name`: Nome da macro
- `value`: Valor da macro

#### `void undefineMacro(const std::string& name)`

Remove definição de uma macro.

**Parâmetros:**
- `name`: Nome da macro a ser removida

#### `bool isMacroDefined(const std::string& name) const`

Verifica se uma macro está definida.

**Parâmetros:**
- `name`: Nome da macro

**Retorno:**
- `true`: Macro está definida
- `false`: Macro não está definida

#### `void setVersion(CVersion version)`

Configura a versão do padrão C.

**Parâmetros:**
- `version`: Versão do padrão C (C89, C99, C11, C17, C23)

#### `PreprocessorState getStatistics() const`

Retorna estatísticas de processamento.

**Retorno:**
- Estrutura com estatísticas detalhadas

---

## MacroProcessor

**Namespace:** `Preprocessor`  
**Header:** `macro_processor.hpp`

### Descrição

Processador de macros responsável por definição, expansão e gerenciamento de macros do pré-processador C.

### Construtores

```cpp
// Construtor padrão
MacroProcessor();

// Construtor com logger e estado
MacroProcessor(std::shared_ptr<PreprocessorLogger> logger,
               std::shared_ptr<PreprocessorState> state);
```

### Operações Básicas de Macro

#### `bool defineMacro(const std::string& name, const std::string& value, const PreprocessorPosition& position = PreprocessorPosition())`

Define uma macro simples (object-like).

**Parâmetros:**
- `name`: Nome da macro
- `value`: Valor da macro
- `position`: Posição onde foi definida (opcional)

**Retorno:**
- `true`: Macro definida com sucesso
- `false`: Erro na definição

#### `bool defineFunctionMacro(const std::string& name, const std::vector<std::string>& parameters, const std::string& body, bool isVariadic = false, const PreprocessorPosition& position = PreprocessorPosition())`

Define uma macro funcional (function-like).

**Parâmetros:**
- `name`: Nome da macro
- `parameters`: Lista de parâmetros
- `body`: Corpo da macro
- `isVariadic`: Se é variádica (opcional)
- `position`: Posição onde foi definida (opcional)

**Retorno:**
- `true`: Macro definida com sucesso
- `false`: Erro na definição

#### `bool undefineMacro(const std::string& name)`

Remove uma macro.

**Parâmetros:**
- `name`: Nome da macro a remover

**Retorno:**
- `true`: Macro removida com sucesso
- `false`: Macro não encontrada

### Consultas de Macro

#### `bool isDefined(const std::string& name) const`

Verifica se uma macro está definida.

**Parâmetros:**
- `name`: Nome da macro

**Retorno:**
- `true`: Macro está definida
- `false`: Macro não está definida

#### `std::string getMacroValue(const std::string& name) const`

Obtém o valor de uma macro.

**Parâmetros:**
- `name`: Nome da macro

**Retorno:**
- Valor da macro ou string vazia se não definida

#### `const MacroInfo* getMacroInfo(const std::string& name) const`

Obtém informações completas de uma macro.

**Parâmetros:**
- `name`: Nome da macro

**Retorno:**
- Ponteiro para `MacroInfo` ou `nullptr` se não encontrada

#### `std::vector<std::string> getDefinedMacros() const`

Obtém lista de todas as macros definidas.

**Retorno:**
- Vetor com nomes das macros

### Expansão de Macros

#### `std::string expandMacro(const std::string& name)`

Expande uma macro simples.

**Parâmetros:**
- `name`: Nome da macro

**Retorno:**
- Texto expandido

#### `std::string expandFunctionMacro(const std::string& name, const std::vector<std::string>& arguments)`

Expande uma macro funcional.

**Parâmetros:**
- `name`: Nome da macro
- `arguments`: Lista de argumentos

**Retorno:**
- Texto expandido

#### `std::string expandMacroRecursively(const std::string& text)`

Expande todas as macros em um texto recursivamente.

**Parâmetros:**
- `text`: Texto contendo macros

**Retorno:**
- Texto com todas as macros expandidas

#### `std::string processLine(const std::string& line)`

Processa uma linha completa, expandindo macros encontradas.

**Parâmetros:**
- `line`: Linha de código

**Retorno:**
- Linha processada com macros expandidas

### Métodos de Otimização

#### `void setCacheEnabled(bool enabled)`

Habilita ou desabilita o cache de expansões.

**Parâmetros:**
- `enabled`: Se o cache deve ser habilitado

#### `void clearCache()`

Limpa o cache de expansões.

#### `void configureCacheOptimization(size_t maxCacheSize, bool enablePrecompilation = true)`

Configura otimizações de cache.

**Parâmetros:**
- `maxCacheSize`: Tamanho máximo do cache
- `enablePrecompilation`: Se deve habilitar pré-compilação

#### `void preloadFrequentMacros(const std::vector<std::string>& macroNames)`

Pré-carrega macros frequentemente usadas.

**Parâmetros:**
- `macroNames`: Lista de nomes de macros para pré-carregar

### Métodos de Estatísticas

#### `std::string getStatistics() const`

Retorna estatísticas de uso do processador de macros.

**Retorno:**
- String formatada com estatísticas

#### `void resetStatistics()`

Reinicializa as estatísticas.

---

## FileManager

**Namespace:** `Preprocessor`  
**Header:** `file_manager.hpp`

### Descrição

Gerenciador de arquivos responsável por leitura, escrita, cache de arquivos, resolução de inclusões e detecção de dependências circulares.

### Construtores

```cpp
// Construtor com caminhos de busca e logger opcionais
explicit FileManager(const std::vector<std::string>& search_paths = {},
                    PreprocessorLogger* logger = nullptr);
```

**Parâmetros:**
- `search_paths`: Caminhos de busca para inclusões (opcional)
- `logger`: Ponteiro para logger (opcional)

### Operações Básicas de Arquivo

#### `std::string readFile(const std::string& filepath)`

Lê o conteúdo de um arquivo.

**Parâmetros:**
- `filepath`: Caminho do arquivo

**Retorno:**
- Conteúdo do arquivo

**Exceções:**
- `std::runtime_error`: Se não conseguir ler o arquivo

#### `bool writeFile(const std::string& filepath, const std::string& content)`

Escreve conteúdo em um arquivo.

**Parâmetros:**
- `filepath`: Caminho do arquivo
- `content`: Conteúdo a ser escrito

**Retorno:**
- `true`: Escrita bem-sucedida
- `false`: Erro na escrita

#### `bool fileExists(const std::string& filepath) const`

Verifica se um arquivo existe.

**Parâmetros:**
- `filepath`: Caminho do arquivo

**Retorno:**
- `true`: Arquivo existe
- `false`: Arquivo não existe

### Resolução de Inclusões

#### `std::string resolveInclude(const std::string& filename, bool is_system, const std::string& current_file = "")`

Resolve caminho de inclusão.

**Parâmetros:**
- `filename`: Nome do arquivo a incluir
- `is_system`: Se é inclusão de sistema (`<>`) ou local (`""`)
- `current_file`: Arquivo atual para inclusões relativas (opcional)

**Retorno:**
- Caminho resolvido do arquivo

**Exceções:**
- `std::runtime_error`: Se não conseguir resolver

### Gerenciamento de Caminhos de Busca

#### `void addSearchPath(const std::string& path)`

Adiciona um caminho de busca.

**Parâmetros:**
- `path`: Caminho a ser adicionado

#### `void setSearchPaths(const std::vector<std::string>& paths)`

Define os caminhos de busca.

**Parâmetros:**
- `paths`: Novos caminhos de busca

#### `std::vector<std::string> getSearchPaths() const`

Retorna os caminhos de busca atuais.

**Retorno:**
- Vetor com caminhos de busca

### Detecção de Dependências

#### `bool checkCircularInclusion(const std::string& filepath, const std::vector<std::string>& include_stack)`

Verifica inclusões circulares.

**Parâmetros:**
- `filepath`: Caminho do arquivo
- `include_stack`: Pilha de inclusões atual

**Retorno:**
- `true`: Inclusão circular detectada
- `false`: Sem inclusão circular

#### `std::vector<std::string> getDependencies() const`

Retorna lista de dependências.

**Retorno:**
- Vetor com caminhos das dependências

### Gerenciamento de Cache

#### `void clearCache()`

Limpa o cache de arquivos.

#### `void configureCacheOptimization(size_t max_size = 50 * 1024 * 1024, size_t max_entries = 1000, std::chrono::seconds ttl = std::chrono::seconds(300), bool enable_compression = false)`

Configura otimizações de cache.

**Parâmetros:**
- `max_size`: Tamanho máximo do cache em bytes (padrão: 50MB)
- `max_entries`: Número máximo de entradas (padrão: 1000)
- `ttl`: Time-to-live do cache (padrão: 300s)
- `enable_compression`: Se deve habilitar compressão (padrão: false)

#### `void preloadFiles(const std::vector<std::string>& filepaths)`

Pré-carrega arquivos no cache.

**Parâmetros:**
- `filepaths`: Lista de caminhos para pré-carregar

### Informações de Arquivo

#### `size_t getFileSize(const std::string& filepath) const`

Retorna o tamanho de um arquivo.

**Parâmetros:**
- `filepath`: Caminho do arquivo

**Retorno:**
- Tamanho do arquivo em bytes

#### `std::chrono::system_clock::time_point getLastModified(const std::string& filepath) const`

Retorna a data de última modificação.

**Parâmetros:**
- `filepath`: Caminho do arquivo

**Retorno:**
- Timestamp da última modificação

### Estatísticas

#### `FileStats getStatistics() const`

Retorna estatísticas de uso do gerenciador de arquivos.

**Retorno:**
- Estrutura `FileStats` com estatísticas

#### `void resetStatistics()`

Reinicializa as estatísticas.

---

## DirectiveProcessor

**Namespace:** `Preprocessor`  
**Header:** `directive.hpp`

### Descrição

Processador de diretivas responsável pelo parsing, validação e processamento de todas as diretivas do pré-processador.

### Construtores

```cpp
DirectiveProcessor(PreprocessorState* state,
                  PreprocessorLogger* logger,
                  MacroProcessor* macro_processor,
                  FileManager* file_manager,
                  ConditionalProcessor* conditional_processor);
```

**Parâmetros:**
- `state`: Estado do pré-processador
- `logger`: Sistema de logging
- `macro_processor`: Processador de macros
- `file_manager`: Gerenciador de arquivos
- `conditional_processor`: Processador condicional

### Métodos de Parsing e Validação

#### `Directive parseDirective(const std::string& line, const PreprocessorPosition& pos)`

Faz o parsing de uma linha de diretiva.

**Parâmetros:**
- `line`: Linha contendo a diretiva
- `pos`: Posição no código fonte

**Retorno:**
- Objeto `Directive` parseado

#### `bool validateDirectiveSyntax(const Directive& directive)`

Valida a sintaxe de uma diretiva.

**Parâmetros:**
- `directive`: Diretiva a ser validada

**Retorno:**
- `true`: Sintaxe válida
- `false`: Sintaxe inválida

### Processamento de Diretivas Específicas

#### `bool processIncludeDirective(const std::string& filename, bool is_system, const PreprocessorPosition& pos)`

Processa diretiva `#include`.

**Parâmetros:**
- `filename`: Nome do arquivo a incluir
- `is_system`: Se é inclusão de sistema
- `pos`: Posição no código fonte

**Retorno:**
- `true`: Processamento bem-sucedido
- `false`: Erro no processamento

#### `bool processDefineDirective(const std::string& macro_definition, const PreprocessorPosition& pos)`

Processa diretiva `#define`.

**Parâmetros:**
- `macro_definition`: Definição da macro
- `pos`: Posição no código fonte

**Retorno:**
- `true`: Processamento bem-sucedido
- `false`: Erro no processamento

#### `bool processUndefDirective(const std::string& macro_name, const PreprocessorPosition& pos)`

Processa diretiva `#undef`.

**Parâmetros:**
- `macro_name`: Nome da macro a remover
- `pos`: Posição no código fonte

**Retorno:**
- `true`: Processamento bem-sucedido
- `false`: Erro no processamento

### Tratamento de Erros

#### `bool handleDirectiveErrors(const std::string& error_msg, const PreprocessorPosition& pos)`

Trata erros de processamento de diretivas.

**Parâmetros:**
- `error_msg`: Mensagem de erro
- `pos`: Posição onde ocorreu o erro

**Retorno:**
- `true`: Erro tratado com sucesso
- `false`: Erro crítico

### Validação de Contexto

#### `bool validateDirectiveContext(const Directive& directive)`

Valida o contexto de uma diretiva.

**Parâmetros:**
- `directive`: Diretiva a ser validada

**Retorno:**
- `true`: Contexto válido
- `false`: Contexto inválido

#### `bool checkDirectiveNesting(const Directive& directive)`

Verifica aninhamento de diretivas.

**Parâmetros:**
- `directive`: Diretiva a ser verificada

**Retorno:**
- `true`: Aninhamento válido
- `false`: Aninhamento inválido

### Otimização

#### `void optimizeDirectiveProcessing()`

Otimiza o processamento de diretivas.

---

## ConditionalProcessor

**Namespace:** `Preprocessor`  
**Header:** `conditional_processor.hpp`

### Descrição

Processador de compilação condicional responsável por gerenciar diretivas condicionais e a pilha de contextos.

### Construtores

```cpp
ConditionalProcessor(PreprocessorLogger* logger, MacroProcessor* macro_proc);
```

**Parâmetros:**
- `logger`: Logger para mensagens
- `macro_proc`: Processador de macros

### Gerenciamento de Contexto

#### `bool pushConditionalContext(bool condition, ConditionalType type, const std::string& condition_str, const PreprocessorPosition& pos)`

Adiciona um novo contexto condicional à pilha.

**Parâmetros:**
- `condition`: Resultado da avaliação da condição
- `type`: Tipo da condicional
- `condition_str`: String da condição original
- `pos`: Posição no código fonte

**Retorno:**
- `true`: Contexto adicionado com sucesso
- `false`: Erro ao adicionar contexto

#### `bool popConditionalContext()`

Remove o contexto condicional do topo da pilha.

**Retorno:**
- `true`: Contexto removido com sucesso
- `false`: Pilha vazia ou erro

#### `ConditionalContext* getCurrentContext()`

Retorna o contexto condicional atual.

**Retorno:**
- Ponteiro para o contexto atual ou `nullptr` se pilha vazia

### Avaliação de Condições

#### `bool evaluateCondition(const std::string& condition, const PreprocessorPosition& pos)`

Avalia uma expressão condicional.

**Parâmetros:**
- `condition`: Expressão a ser avaliada
- `pos`: Posição no código fonte

**Retorno:**
- `true`: Condição verdadeira
- `false`: Condição falsa

### Processamento de Diretivas Condicionais

#### `bool processIfDirective(const std::string& expression, const PreprocessorPosition& pos)`

Processa diretiva `#if`.

**Parâmetros:**
- `expression`: Expressão a ser avaliada
- `pos`: Posição no código fonte

**Retorno:**
- `true`: Processamento bem-sucedido
- `false`: Erro no processamento

#### `bool processIfdefDirective(const std::string& macro_name, const PreprocessorPosition& pos)`

Processa diretiva `#ifdef`.

**Parâmetros:**
- `macro_name`: Nome da macro a verificar
- `pos`: Posição no código fonte

**Retorno:**
- `true`: Processamento bem-sucedido
- `false`: Erro no processamento

#### `bool processIfndefDirective(const std::string& macro_name, const PreprocessorPosition& pos)`

Processa diretiva `#ifndef`.

**Parâmetros:**
- `macro_name`: Nome da macro a verificar
- `pos`: Posição no código fonte

**Retorno:**
- `true`: Processamento bem-sucedido
- `false`: Erro no processamento

#### `bool processElseDirective(const PreprocessorPosition& pos)`

Processa diretiva `#else`.

**Parâmetros:**
- `pos`: Posição no código fonte

**Retorno:**
- `true`: Processamento bem-sucedido
- `false`: Erro no processamento

#### `bool processElifDirective(const std::string& expression, const PreprocessorPosition& pos)`

Processa diretiva `#elif`.

**Parâmetros:**
- `expression`: Expressão a ser avaliada
- `pos`: Posição no código fonte

**Retorno:**
- `true`: Processamento bem-sucedido
- `false`: Erro no processamento

#### `bool processEndifDirective(const PreprocessorPosition& pos)`

Processa diretiva `#endif`.

**Parâmetros:**
- `pos`: Posição no código fonte

**Retorno:**
- `true`: Processamento bem-sucedido
- `false`: Erro no processamento

### Consultas de Estado

#### `bool isConditionTrue() const`

Verifica se a condição atual é verdadeira.

**Retorno:**
- `true`: Condição verdadeira
- `false`: Condição falsa

#### `bool shouldProcessBlock() const`

Verifica se o bloco atual deve ser processado.

**Retorno:**
- `true`: Bloco deve ser processado
- `false`: Bloco deve ser ignorado

#### `int getCurrentNestingLevel() const`

Retorna o nível atual de aninhamento.

**Retorno:**
- Nível de aninhamento (0 = nível raiz)

#### `bool hasOpenConditionals() const`

Verifica se há condicionais abertas.

**Retorno:**
- `true`: Há condicionais abertas
- `false`: Nenhuma condicional aberta

### Validação e Tratamento de Erros

#### `bool validateConditionalStructure() const`

Valida a estrutura condicional atual.

**Retorno:**
- `true`: Estrutura válida
- `false`: Estrutura inválida

#### `bool handleConditionalErrors(const std::string& error_msg, const PreprocessorPosition& pos)`

Trata erros de processamento condicional.

**Parâmetros:**
- `error_msg`: Mensagem de erro
- `pos`: Posição onde ocorreu o erro

**Retorno:**
- `true`: Erro tratado
- `false`: Erro crítico

### Estatísticas e Relatórios

#### `ConditionalStats getStatistics() const`

Retorna estatísticas de processamento condicional.

**Retorno:**
- Estrutura `ConditionalStats` com estatísticas

#### `std::string generateStatusReport() const`

Gera relatório de status do processador condicional.

**Retorno:**
- String formatada com relatório

### Controle

#### `void reset()`

Reinicializa o processador condicional.

#### `void optimizeConditionalEvaluation(bool enable = true)`

Habilita ou desabilita otimizações de avaliação.

**Parâmetros:**
- `enable`: Se deve habilitar otimizações

---

## ExpressionEvaluator

**Namespace:** `Preprocessor`  
**Header:** `expression_evaluator.hpp`

### Descrição

Avaliador de expressões constantes responsável por avaliar expressões em diretivas condicionais.

### Construtores

```cpp
// Construtor com logger e processador de macros opcionais
ExpressionEvaluator(PreprocessorLogger* logger = nullptr,
                   MacroProcessor* macro_processor = nullptr);
```

**Parâmetros:**
- `logger`: Logger para mensagens (opcional)
- `macro_processor`: Processador de macros (opcional)

### Métodos Principais

#### `long long evaluateExpression(const std::string& expression)`

Avalia uma expressão constante.

**Parâmetros:**
- `expression`: Expressão a ser avaliada

**Retorno:**
- Resultado da avaliação

**Exceções:**
- `std::runtime_error`: Erro na avaliação

#### `bool evaluateBooleanExpression(const std::string& expression)`

Avalia uma expressão como booleana.

**Parâmetros:**
- `expression`: Expressão a ser avaliada

**Retorno:**
- `true`: Expressão verdadeira (≠ 0)
- `false`: Expressão falsa (= 0)

### Métodos de Tokenização

#### `std::vector<ExpressionToken> tokenizeExpression(const std::string& expression)`

Tokeniza uma expressão.

**Parâmetros:**
- `expression`: Expressão a ser tokenizada

**Retorno:**
- Vetor de tokens

### Métodos de Validação

#### `bool validateExpression(const std::string& expression)`

Valida a sintaxe de uma expressão.

**Parâmetros:**
- `expression`: Expressão a ser validada

**Retorno:**
- `true`: Sintaxe válida
- `false`: Sintaxe inválida

#### `bool validateParentheses(const std::string& expression)`

Valida o balanceamento de parênteses.

**Parâmetros:**
- `expression`: Expressão a ser validada

**Retorno:**
- `true`: Parênteses balanceados
- `false`: Parênteses desbalanceados

### Tratamento de Erros

#### `void handleExpressionErrors(const std::string& error_msg, const std::string& expression)`

Trata erros de avaliação de expressões.

**Parâmetros:**
- `error_msg`: Mensagem de erro
- `expression`: Expressão que causou o erro

---

## Estruturas de Dados

### PreprocessorPosition

```cpp
struct PreprocessorPosition {
    std::string filename;    // Nome do arquivo
    int line;               // Número da linha
    int column;             // Número da coluna
    
    PreprocessorPosition(const std::string& file = "", int l = 1, int c = 1);
};
```

### MacroInfo

```cpp
struct MacroInfo {
    std::string name;                    // Nome da macro
    std::string value;                   // Valor/corpo da macro
    MacroType type;                      // Tipo da macro
    std::vector<std::string> parameters; // Parâmetros (para macros funcionais)
    bool isVariadic;                     // Se é uma macro variádica
    PreprocessorPosition definedAt;      // Onde foi definida
    bool isPredefined;                   // Se é uma macro predefinida
    int expansionCount;                  // Contador de expansões
};
```

### FileStats

```cpp
struct FileStats {
    size_t files_read = 0;              // Arquivos lidos
    size_t files_cached = 0;            // Arquivos em cache
    size_t cache_hits = 0;              // Acertos de cache
    size_t cache_misses = 0;            // Perdas de cache
    size_t total_bytes_read = 0;        // Total de bytes lidos
    size_t circular_inclusions = 0;     // Inclusões circulares detectadas
    size_t path_resolutions = 0;        // Resoluções de caminho
    size_t dependency_updates = 0;      // Atualizações de dependência
    
    double getCacheHitRatio() const;    // Taxa de acerto do cache
    void reset();                       // Reinicializa estatísticas
};
```

### ConditionalStats

```cpp
struct ConditionalStats {
    size_t total_conditionals;      // Total de condicionais processadas
    size_t true_branches;           // Branches verdadeiras executadas
    size_t false_branches;          // Branches falsas puladas
    size_t max_nesting_level;       // Nível máximo de aninhamento
    size_t evaluation_errors;       // Erros de avaliação
    size_t structure_errors;        // Erros de estrutura
};
```

---

## Exemplos de Integração

### Exemplo Básico de Uso

```cpp
#include "preprocessor.hpp"

int main() {
    // Criar instância do pré-processador
    Preprocessor::PreprocessorMain preprocessor;
    
    // Configurar caminhos de busca
    std::vector<std::string> paths = {"/usr/include", "./include"};
    preprocessor.setSearchPaths(paths);
    
    // Definir macros
    preprocessor.defineMacro("DEBUG", "1");
    preprocessor.defineMacro("VERSION", "\"1.0.0\"");
    
    // Processar arquivo
    if (preprocessor.process("main.c")) {
        std::string expanded = preprocessor.getExpandedCode();
        std::cout << "Código expandido:\n" << expanded << std::endl;
        
        // Obter dependências
        auto deps = preprocessor.getDependencies();
        std::cout << "Dependências:" << std::endl;
        for (const auto& dep : deps) {
            std::cout << "  " << dep << std::endl;
        }
    } else {
        std::cerr << "Erro no processamento" << std::endl;
    }
    
    return 0;
}
```

### Exemplo de Uso Avançado com Componentes Individuais

```cpp
#include "macro_processor.hpp"
#include "file_manager.hpp"
#include "conditional_processor.hpp"

int main() {
    // Criar logger
    auto logger = std::make_shared<Preprocessor::PreprocessorLogger>();
    auto state = std::make_shared<Preprocessor::PreprocessorState>();
    
    // Criar processador de macros
    Preprocessor::MacroProcessor macro_proc(logger, state);
    
    // Definir macros
    macro_proc.defineMacro("PI", "3.14159");
    macro_proc.defineFunctionMacro("MAX", {"a", "b"}, "((a) > (b) ? (a) : (b))");
    
    // Expandir macros
    std::string result = macro_proc.expandMacro("PI");
    std::cout << "PI = " << result << std::endl;
    
    // Criar gerenciador de arquivos
    std::vector<std::string> search_paths = {"/usr/include", "./include"};
    Preprocessor::FileManager file_mgr(search_paths, logger.get());
    
    // Configurar cache
    file_mgr.configureCacheOptimization(100 * 1024 * 1024, 2000);
    
    // Ler arquivo
    try {
        std::string content = file_mgr.readFile("example.h");
        std::cout << "Arquivo lido com sucesso" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }
    
    // Criar processador condicional
    Preprocessor::ConditionalProcessor cond_proc(logger.get(), &macro_proc);
    
    // Processar condicionais
    Preprocessor::PreprocessorPosition pos("test.c", 10, 1);
    if (cond_proc.processIfdefDirective("DEBUG", pos)) {
        if (cond_proc.shouldProcessBlock()) {
            std::cout << "Bloco DEBUG ativo" << std::endl;
        }
    }
    
    return 0;
}
```

---

## Notas de Implementação

### Thread Safety

- **PreprocessorMain**: Não é thread-safe. Use instâncias separadas para threads diferentes.
- **MacroProcessor**: Thread-safe para operações de leitura, requer sincronização para escritas.
- **FileManager**: Thread-safe com cache interno protegido.
- **ConditionalProcessor**: Não é thread-safe devido ao estado da pilha.
- **ExpressionEvaluator**: Thread-safe para avaliações.

### Performance

- Use cache quando possível (`setCacheEnabled(true)` no MacroProcessor)
- Configure otimizações de cache no FileManager
- Pré-carregue arquivos e macros frequentemente usados
- Monitore estatísticas para identificar gargalos

### Tratamento de Erros

- Métodos que podem falhar retornam `bool` ou lançam exceções
- Use os sistemas de logging integrados para diagnóstico
- Verifique sempre os valores de retorno dos métodos críticos

### Compatibilidade

- Suporte completo para padrões C89, C99, C11, C17 e C23
- Macros predefinidas configuráveis por versão
- Validação de sintaxe específica por padrão

---

*Documentação gerada automaticamente para o Pré-processador C v1.0*  
*Última atualização: Janeiro 2025*