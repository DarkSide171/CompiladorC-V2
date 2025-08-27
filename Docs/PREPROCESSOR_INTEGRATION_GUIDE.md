# Guia de Integração do Pré-processador C

## Visão Geral

Este documento fornece instruções detalhadas sobre como integrar o pré-processador C com outros módulos do compilador, incluindo o analisador léxico, sintático e semântico.

## Índice

1. [Arquitetura de Integração](#arquitetura-de-integração)
2. [Integração com Analisador Léxico](#integração-com-analisador-léxico)
3. [Interface de Comunicação](#interface-de-comunicação)
4. [Mapeamento de Posições](#mapeamento-de-posições)
5. [Tratamento de Erros Integrado](#tratamento-de-erros-integrado)
6. [Pipeline de Compilação](#pipeline-de-compilação)
7. [Configuração e Inicialização](#configuração-e-inicialização)
8. [Testes de Integração](#testes-de-integração)
9. [Otimizações de Performance](#otimizações-de-performance)
10. [Troubleshooting](#troubleshooting)

---

## Arquitetura de Integração

### Visão Geral da Arquitetura

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Código Fonte  │───▶│  Pré-processador │───▶│ Analisador      │
│     (.c/.h)     │    │                  │    │ Léxico          │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                                │                        │
                                ▼                        ▼
                       ┌──────────────────┐    ┌─────────────────┐
                       │   Dependências   │    │    Tokens       │
                       │   Mapeamento     │    │                 │
                       │   Estatísticas   │    │                 │
                       └──────────────────┘    └─────────────────┘
                                                         │
                                                         ▼
                                                ┌─────────────────┐
                                                │ Analisador      │
                                                │ Sintático       │
                                                └─────────────────┘
```

### Componentes de Integração

1. **PreprocessorInterface**: Interface principal para comunicação
2. **PositionMapper**: Mapeia posições entre código original e expandido
3. **ErrorReporter**: Sistema unificado de relatório de erros
4. **DependencyTracker**: Rastreamento de dependências entre módulos
5. **ConfigurationManager**: Gerenciamento centralizado de configurações

---

## Integração com Analisador Léxico

### Interface de Integração

```cpp
// preprocessor_lexer_interface.hpp
#ifndef PREPROCESSOR_LEXER_INTERFACE_HPP
#define PREPROCESSOR_LEXER_INTERFACE_HPP

#include "preprocessor.hpp"
#include "position_mapper.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Compiler {

/**
 * Interface para integração entre pré-processador e analisador léxico
 */
class PreprocessorLexerInterface {
public:
    struct ProcessingResult {
        std::string expanded_code;           // Código expandido
        std::vector<std::string> dependencies; // Lista de dependências
        std::shared_ptr<PositionMapper> position_mapper; // Mapeador de posições
        Preprocessor::PreprocessorState statistics; // Estatísticas
        bool success;                        // Status do processamento
        std::string error_message;           // Mensagem de erro (se houver)
    };
    
    /**
     * Processa um arquivo e retorna resultado para o lexer
     */
    static ProcessingResult processForLexer(const std::string& filename,
                                           const CompilerConfiguration& config);
    
    /**
     * Processa uma string de código
     */
    static ProcessingResult processStringForLexer(const std::string& code,
                                                 const std::string& filename,
                                                 const CompilerConfiguration& config);
    
    /**
     * Configura o pré-processador para integração com lexer
     */
    static void configureLexerIntegration(Preprocessor::PreprocessorMain& preprocessor,
                                         const CompilerConfiguration& config);
};

/**
 * Configuração do compilador
 */
struct CompilerConfiguration {
    std::vector<std::string> include_paths;     // Caminhos de inclusão
    std::vector<std::pair<std::string, std::string>> predefined_macros; // Macros predefinidas
    Preprocessor::CVersion c_version;           // Versão do padrão C
    bool enable_warnings;                       // Habilitar avisos
    bool enable_debug_info;                     // Informações de debug
    bool optimize_performance;                  // Otimizações de performance
    std::string target_platform;               // Plataforma alvo
};

} // namespace Compiler

#endif // PREPROCESSOR_LEXER_INTERFACE_HPP
```

### Implementação da Interface

```cpp
// preprocessor_lexer_interface.cpp
#include "preprocessor_lexer_interface.hpp"
#include "position_mapper.hpp"
#include <iostream>
#include <filesystem>

namespace Compiler {

PreprocessorLexerInterface::ProcessingResult 
PreprocessorLexerInterface::processForLexer(const std::string& filename,
                                           const CompilerConfiguration& config) {
    ProcessingResult result;
    result.success = false;
    
    try {
        // Verificar se arquivo existe
        if (!std::filesystem::exists(filename)) {
            result.error_message = "Arquivo não encontrado: " + filename;
            return result;
        }
        
        // Criar e configurar pré-processador
        Preprocessor::PreprocessorMain preprocessor;
        configureLexerIntegration(preprocessor, config);
        
        // Criar mapeador de posições
        auto position_mapper = std::make_shared<PositionMapper>(filename);
        
        // Processar arquivo
        if (preprocessor.process(filename)) {
            result.expanded_code = preprocessor.getExpandedCode();
            result.dependencies = preprocessor.getDependencies();
            result.statistics = preprocessor.getStatistics();
            result.position_mapper = position_mapper;
            result.success = true;
            
            // Configurar mapeamento de posições
            position_mapper->buildMapping(result.expanded_code, preprocessor);
            
        } else {
            result.error_message = "Falha no processamento do arquivo";
        }
        
    } catch (const std::exception& e) {
        result.error_message = "Exceção durante processamento: " + std::string(e.what());
    }
    
    return result;
}

PreprocessorLexerInterface::ProcessingResult 
PreprocessorLexerInterface::processStringForLexer(const std::string& code,
                                                 const std::string& filename,
                                                 const CompilerConfiguration& config) {
    ProcessingResult result;
    result.success = false;
    
    try {
        // Criar e configurar pré-processador
        Preprocessor::PreprocessorMain preprocessor;
        configureLexerIntegration(preprocessor, config);
        
        // Criar mapeador de posições
        auto position_mapper = std::make_shared<PositionMapper>(filename);
        
        // Processar string
        if (preprocessor.processString(code)) {
            result.expanded_code = preprocessor.getExpandedCode();
            result.dependencies = preprocessor.getDependencies();
            result.statistics = preprocessor.getStatistics();
            result.position_mapper = position_mapper;
            result.success = true;
            
            // Configurar mapeamento de posições
            position_mapper->buildMapping(result.expanded_code, preprocessor);
            
        } else {
            result.error_message = "Falha no processamento da string";
        }
        
    } catch (const std::exception& e) {
        result.error_message = "Exceção durante processamento: " + std::string(e.what());
    }
    
    return result;
}

void PreprocessorLexerInterface::configureLexerIntegration(
    Preprocessor::PreprocessorMain& preprocessor,
    const CompilerConfiguration& config) {
    
    // Configurar caminhos de inclusão
    preprocessor.setSearchPaths(config.include_paths);
    
    // Configurar versão do padrão C
    preprocessor.setVersion(config.c_version);
    
    // Definir macros predefinidas
    for (const auto& macro : config.predefined_macros) {
        preprocessor.defineMacro(macro.first, macro.second);
    }
    
    // Configurar macros específicas da plataforma
    configurePlatformMacros(preprocessor, config.target_platform);
    
    // Configurar otimizações se habilitadas
    if (config.optimize_performance) {
        configurePerformanceOptimizations(preprocessor);
    }
}

void configurePlatformMacros(Preprocessor::PreprocessorMain& preprocessor,
                           const std::string& platform) {
    if (platform == "linux") {
        preprocessor.defineMacro("__linux__", "1");
        preprocessor.defineMacro("_GNU_SOURCE", "1");
    } else if (platform == "macos") {
        preprocessor.defineMacro("__APPLE__", "1");
        preprocessor.defineMacro("__MACH__", "1");
    } else if (platform == "windows") {
        preprocessor.defineMacro("_WIN32", "1");
        preprocessor.defineMacro("_MSC_VER", "1900");
    }
}

void configurePerformanceOptimizations(Preprocessor::PreprocessorMain& preprocessor) {
    // Configurações específicas de otimização seriam implementadas aqui
    // baseadas na API do pré-processador
}

} // namespace Compiler
```

---

## Interface de Comunicação

### Estrutura de Dados Compartilhadas

```cpp
// compiler_shared_types.hpp
#ifndef COMPILER_SHARED_TYPES_HPP
#define COMPILER_SHARED_TYPES_HPP

#include <string>
#include <vector>
#include <memory>

namespace Compiler {

/**
 * Posição no código fonte
 */
struct SourcePosition {
    std::string filename;    // Nome do arquivo
    int line;               // Linha (1-indexed)
    int column;             // Coluna (1-indexed)
    int offset;             // Offset absoluto no arquivo
    
    SourcePosition(const std::string& file = "", int l = 1, int c = 1, int off = 0)
        : filename(file), line(l), column(c), offset(off) {}
    
    bool operator==(const SourcePosition& other) const {
        return filename == other.filename && line == other.line && 
               column == other.column && offset == other.offset;
    }
    
    std::string toString() const {
        return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};

/**
 * Informações de diagnóstico (erros, avisos)
 */
struct Diagnostic {
    enum class Level {
        NOTE,
        WARNING,
        ERROR,
        FATAL
    };
    
    Level level;                    // Nível do diagnóstico
    std::string message;            // Mensagem
    SourcePosition position;        // Posição onde ocorreu
    std::string source_line;        // Linha do código fonte
    std::vector<SourcePosition> related_positions; // Posições relacionadas
    
    Diagnostic(Level lvl, const std::string& msg, const SourcePosition& pos)
        : level(lvl), message(msg), position(pos) {}
};

/**
 * Contexto de compilação compartilhado
 */
class CompilationContext {
public:
    // Diagnósticos coletados
    std::vector<Diagnostic> diagnostics;
    
    // Estatísticas de compilação
    struct Statistics {
        size_t files_processed = 0;
        size_t lines_processed = 0;
        size_t tokens_generated = 0;
        size_t errors_count = 0;
        size_t warnings_count = 0;
    } stats;
    
    // Métodos para adicionar diagnósticos
    void addError(const std::string& message, const SourcePosition& pos);
    void addWarning(const std::string& message, const SourcePosition& pos);
    void addNote(const std::string& message, const SourcePosition& pos);
    
    // Métodos para consultar estado
    bool hasErrors() const { return stats.errors_count > 0; }
    bool hasWarnings() const { return stats.warnings_count > 0; }
    size_t getErrorCount() const { return stats.errors_count; }
    size_t getWarningCount() const { return stats.warnings_count; }
    
    // Relatórios
    void printDiagnostics() const;
    std::string generateReport() const;
};

} // namespace Compiler

#endif // COMPILER_SHARED_TYPES_HPP
```

---

## Mapeamento de Posições

### Implementação do Mapeador de Posições

```cpp
// position_mapper.hpp
#ifndef POSITION_MAPPER_HPP
#define POSITION_MAPPER_HPP

#include "compiler_shared_types.hpp"
#include "preprocessor.hpp"
#include <map>
#include <vector>

namespace Compiler {

/**
 * Mapeia posições entre código original e código expandido
 */
class PositionMapper {
public:
    struct MappingEntry {
        SourcePosition original;     // Posição no código original
        SourcePosition expanded;     // Posição no código expandido
        bool is_macro_expansion;     // Se é resultado de expansão de macro
        std::string macro_name;      // Nome da macro (se aplicável)
    };
    
private:
    std::string original_filename;
    std::vector<MappingEntry> mappings;
    std::map<int, int> line_mapping;  // Mapa linha expandida -> linha original
    
public:
    explicit PositionMapper(const std::string& filename);
    
    /**
     * Constrói o mapeamento baseado no resultado do pré-processador
     */
    void buildMapping(const std::string& expanded_code,
                     const Preprocessor::PreprocessorMain& preprocessor);
    
    /**
     * Mapeia posição do código expandido para posição original
     */
    SourcePosition mapToOriginal(const SourcePosition& expanded_pos) const;
    
    /**
     * Mapeia posição original para posição no código expandido
     */
    SourcePosition mapToExpanded(const SourcePosition& original_pos) const;
    
    /**
     * Verifica se uma posição é resultado de expansão de macro
     */
    bool isMacroExpansion(const SourcePosition& expanded_pos) const;
    
    /**
     * Obtém nome da macro que gerou uma posição (se aplicável)
     */
    std::string getMacroName(const SourcePosition& expanded_pos) const;
    
    /**
     * Obtém todas as posições originais que contribuíram para uma linha expandida
     */
    std::vector<SourcePosition> getOriginalContributors(int expanded_line) const;
    
    /**
     * Gera relatório de mapeamento para debug
     */
    std::string generateMappingReport() const;
};

} // namespace Compiler

#endif // POSITION_MAPPER_HPP
```

```cpp
// position_mapper.cpp
#include "position_mapper.hpp"
#include <sstream>
#include <algorithm>

namespace Compiler {

PositionMapper::PositionMapper(const std::string& filename)
    : original_filename(filename) {
}

void PositionMapper::buildMapping(const std::string& expanded_code,
                                const Preprocessor::PreprocessorMain& preprocessor) {
    // Esta implementação seria baseada nas informações fornecidas
    // pelo pré-processador sobre expansões de macro e inclusões
    
    // Exemplo de construção de mapeamento:
    std::istringstream expanded_stream(expanded_code);
    std::string line;
    int expanded_line_num = 1;
    
    while (std::getline(expanded_stream, line)) {
        // Analisar linha para determinar origem
        SourcePosition expanded_pos("<expanded>", expanded_line_num, 1);
        SourcePosition original_pos;
        
        // Lógica para determinar posição original baseada em:
        // - Diretivas #line inseridas pelo pré-processador
        // - Informações de expansão de macro
        // - Mapeamento de inclusões
        
        if (line.find("#line") == 0) {
            // Processar diretiva #line para obter mapeamento
            parseLineDirective(line, original_pos);
        } else {
            // Usar último mapeamento conhecido
            original_pos = getLastKnownOriginalPosition(expanded_line_num);
        }
        
        MappingEntry entry;
        entry.original = original_pos;
        entry.expanded = expanded_pos;
        entry.is_macro_expansion = false; // Determinar baseado no contexto
        
        mappings.push_back(entry);
        line_mapping[expanded_line_num] = original_pos.line;
        
        expanded_line_num++;
    }
}

SourcePosition PositionMapper::mapToOriginal(const SourcePosition& expanded_pos) const {
    // Buscar entrada de mapeamento mais próxima
    auto it = std::lower_bound(mappings.begin(), mappings.end(), expanded_pos,
        [](const MappingEntry& entry, const SourcePosition& pos) {
            return entry.expanded.line < pos.line ||
                   (entry.expanded.line == pos.line && entry.expanded.column < pos.column);
        });
    
    if (it != mappings.end()) {
        // Ajustar posição baseada na diferença de colunas
        SourcePosition result = it->original;
        int column_offset = expanded_pos.column - it->expanded.column;
        result.column += column_offset;
        return result;
    }
    
    // Fallback: retornar posição aproximada
    auto line_it = line_mapping.find(expanded_pos.line);
    if (line_it != line_mapping.end()) {
        return SourcePosition(original_filename, line_it->second, expanded_pos.column);
    }
    
    return SourcePosition(original_filename, 1, 1);
}

SourcePosition PositionMapper::mapToExpanded(const SourcePosition& original_pos) const {
    // Buscar entrada de mapeamento correspondente
    for (const auto& entry : mappings) {
        if (entry.original.filename == original_pos.filename &&
            entry.original.line == original_pos.line) {
            
            SourcePosition result = entry.expanded;
            int column_offset = original_pos.column - entry.original.column;
            result.column += column_offset;
            return result;
        }
    }
    
    return SourcePosition("<expanded>", 1, 1);
}

bool PositionMapper::isMacroExpansion(const SourcePosition& expanded_pos) const {
    for (const auto& entry : mappings) {
        if (entry.expanded.line == expanded_pos.line) {
            return entry.is_macro_expansion;
        }
    }
    return false;
}

std::string PositionMapper::getMacroName(const SourcePosition& expanded_pos) const {
    for (const auto& entry : mappings) {
        if (entry.expanded.line == expanded_pos.line && entry.is_macro_expansion) {
            return entry.macro_name;
        }
    }
    return "";
}

std::vector<SourcePosition> PositionMapper::getOriginalContributors(int expanded_line) const {
    std::vector<SourcePosition> contributors;
    
    for (const auto& entry : mappings) {
        if (entry.expanded.line == expanded_line) {
            contributors.push_back(entry.original);
        }
    }
    
    return contributors;
}

std::string PositionMapper::generateMappingReport() const {
    std::ostringstream report;
    report << "=== Relatório de Mapeamento de Posições ===\n";
    report << "Arquivo original: " << original_filename << "\n";
    report << "Total de mapeamentos: " << mappings.size() << "\n\n";
    
    for (size_t i = 0; i < std::min(mappings.size(), size_t(20)); ++i) {
        const auto& entry = mappings[i];
        report << "Expandida " << entry.expanded.toString()
               << " <- Original " << entry.original.toString();
        
        if (entry.is_macro_expansion) {
            report << " (macro: " << entry.macro_name << ")";
        }
        report << "\n";
    }
    
    if (mappings.size() > 20) {
        report << "... (" << (mappings.size() - 20) << " entradas omitidas)\n";
    }
    
    return report.str();
}

void PositionMapper::parseLineDirective(const std::string& line, SourcePosition& pos) {
    // Implementar parsing de diretivas #line
    // Formato: #line número "arquivo"
    std::istringstream iss(line);
    std::string directive, number, filename;
    
    iss >> directive >> number >> filename;
    
    if (directive == "#line" && !number.empty()) {
        pos.line = std::stoi(number);
        if (!filename.empty() && filename.size() > 2) {
            // Remover aspas
            pos.filename = filename.substr(1, filename.size() - 2);
        }
    }
}

SourcePosition PositionMapper::getLastKnownOriginalPosition(int expanded_line) const {
    // Encontrar último mapeamento conhecido antes da linha especificada
    SourcePosition last_known(original_filename, 1, 1);
    
    for (const auto& entry : mappings) {
        if (entry.expanded.line < expanded_line) {
            last_known = entry.original;
        } else {
            break;
        }
    }
    
    return last_known;
}

} // namespace Compiler
```

---

## Tratamento de Erros Integrado

### Sistema Unificado de Erros

```cpp
// integrated_error_handler.hpp
#ifndef INTEGRATED_ERROR_HANDLER_HPP
#define INTEGRATED_ERROR_HANDLER_HPP

#include "compiler_shared_types.hpp"
#include "position_mapper.hpp"
#include <memory>
#include <functional>

namespace Compiler {

/**
 * Manipulador de erros integrado para todo o compilador
 */
class IntegratedErrorHandler {
public:
    using ErrorCallback = std::function<void(const Diagnostic&)>;
    
private:
    std::shared_ptr<CompilationContext> context;
    std::shared_ptr<PositionMapper> position_mapper;
    std::vector<ErrorCallback> error_callbacks;
    bool errors_as_warnings;
    bool suppress_warnings;
    
public:
    explicit IntegratedErrorHandler(std::shared_ptr<CompilationContext> ctx);
    
    /**
     * Configura o mapeador de posições
     */
    void setPositionMapper(std::shared_ptr<PositionMapper> mapper);
    
    /**
     * Adiciona callback para notificação de erros
     */
    void addErrorCallback(const ErrorCallback& callback);
    
    /**
     * Configurações de comportamento
     */
    void setErrorsAsWarnings(bool enable) { errors_as_warnings = enable; }
    void setSuppressWarnings(bool suppress) { suppress_warnings = suppress; }
    
    /**
     * Métodos para reportar erros do pré-processador
     */
    void reportPreprocessorError(const std::string& message,
                               const SourcePosition& pos,
                               const std::string& source_line = "");
    
    void reportPreprocessorWarning(const std::string& message,
                                 const SourcePosition& pos,
                                 const std::string& source_line = "");
    
    /**
     * Métodos para reportar erros do lexer
     */
    void reportLexerError(const std::string& message,
                        const SourcePosition& expanded_pos,
                        const std::string& source_line = "");
    
    /**
     * Métodos para reportar erros do parser
     */
    void reportParserError(const std::string& message,
                         const SourcePosition& expanded_pos,
                         const std::string& expected = "");
    
    /**
     * Relatórios e estatísticas
     */
    void printErrorSummary() const;
    bool hasErrors() const;
    bool shouldStopCompilation() const;
    
private:
    void addDiagnostic(Diagnostic::Level level,
                      const std::string& message,
                      const SourcePosition& pos,
                      const std::string& source_line = "");
    
    SourcePosition mapPosition(const SourcePosition& pos) const;
    void notifyCallbacks(const Diagnostic& diagnostic);
};

} // namespace Compiler

#endif // INTEGRATED_ERROR_HANDLER_HPP
```

---

## Pipeline de Compilação

### Orquestrador Principal

```cpp
// compilation_pipeline.hpp
#ifndef COMPILATION_PIPELINE_HPP
#define COMPILATION_PIPELINE_HPP

#include "preprocessor_lexer_interface.hpp"
#include "integrated_error_handler.hpp"
#include "compiler_shared_types.hpp"
#include <memory>

namespace Compiler {

/**
 * Pipeline principal de compilação
 */
class CompilationPipeline {
public:
    struct PipelineResult {
        bool success;
        std::string output_code;                    // Código final gerado
        std::vector<std::string> dependencies;      // Dependências
        CompilationContext::Statistics statistics;  // Estatísticas
        std::string error_summary;                  // Resumo de erros
    };
    
private:
    CompilerConfiguration config;
    std::shared_ptr<CompilationContext> context;
    std::shared_ptr<IntegratedErrorHandler> error_handler;
    
public:
    explicit CompilationPipeline(const CompilerConfiguration& cfg);
    
    /**
     * Compila um arquivo fonte
     */
    PipelineResult compileFile(const std::string& filename);
    
    /**
     * Compila código fonte direto
     */
    PipelineResult compileString(const std::string& code,
                               const std::string& filename = "<string>");
    
    /**
     * Configuração do pipeline
     */
    void updateConfiguration(const CompilerConfiguration& new_config);
    CompilerConfiguration getConfiguration() const { return config; }
    
    /**
     * Acesso ao contexto de compilação
     */
    std::shared_ptr<CompilationContext> getContext() const { return context; }
    
private:
    /**
     * Etapas do pipeline
     */
    bool runPreprocessingStage(const std::string& input,
                             const std::string& filename,
                             PreprocessorLexerInterface::ProcessingResult& result);
    
    bool runLexicalAnalysisStage(const PreprocessorLexerInterface::ProcessingResult& preproc_result,
                               std::vector<Token>& tokens);
    
    bool runSyntaxAnalysisStage(const std::vector<Token>& tokens,
                              std::shared_ptr<AST>& ast);
    
    bool runSemanticAnalysisStage(std::shared_ptr<AST>& ast);
    
    bool runCodeGenerationStage(std::shared_ptr<AST>& ast,
                              std::string& output_code);
    
    /**
     * Utilitários
     */
    void initializePipeline();
    void finalizePipeline(PipelineResult& result);
    bool shouldContinueAfterStage(const std::string& stage_name);
};

} // namespace Compiler

#endif // COMPILATION_PIPELINE_HPP
```

### Implementação do Pipeline

```cpp
// compilation_pipeline.cpp
#include "compilation_pipeline.hpp"
#include <iostream>
#include <chrono>

namespace Compiler {

CompilationPipeline::CompilationPipeline(const CompilerConfiguration& cfg)
    : config(cfg) {
    initializePipeline();
}

void CompilationPipeline::initializePipeline() {
    // Criar contexto de compilação
    context = std::make_shared<CompilationContext>();
    
    // Criar manipulador de erros
    error_handler = std::make_shared<IntegratedErrorHandler>(context);
    
    // Configurar callbacks de erro
    error_handler->addErrorCallback([](const Diagnostic& diag) {
        std::cerr << diag.position.toString() << ": ";
        
        switch (diag.level) {
            case Diagnostic::Level::ERROR:
                std::cerr << "error: ";
                break;
            case Diagnostic::Level::WARNING:
                std::cerr << "warning: ";
                break;
            case Diagnostic::Level::NOTE:
                std::cerr << "note: ";
                break;
            case Diagnostic::Level::FATAL:
                std::cerr << "fatal error: ";
                break;
        }
        
        std::cerr << diag.message << std::endl;
        
        if (!diag.source_line.empty()) {
            std::cerr << diag.source_line << std::endl;
        }
    });
}

CompilationPipeline::PipelineResult 
CompilationPipeline::compileFile(const std::string& filename) {
    PipelineResult result;
    result.success = false;
    
    std::cout << "Compilando arquivo: " << filename << std::endl;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        // Etapa 1: Pré-processamento
        PreprocessorLexerInterface::ProcessingResult preproc_result;
        if (!runPreprocessingStage("", filename, preproc_result)) {
            finalizePipeline(result);
            return result;
        }
        
        // Configurar mapeador de posições
        error_handler->setPositionMapper(preproc_result.position_mapper);
        
        if (!shouldContinueAfterStage("preprocessing")) {
            finalizePipeline(result);
            return result;
        }
        
        // Etapa 2: Análise Léxica
        std::vector<Token> tokens;
        if (!runLexicalAnalysisStage(preproc_result, tokens)) {
            finalizePipeline(result);
            return result;
        }
        
        if (!shouldContinueAfterStage("lexical_analysis")) {
            finalizePipeline(result);
            return result;
        }
        
        // Etapa 3: Análise Sintática
        std::shared_ptr<AST> ast;
        if (!runSyntaxAnalysisStage(tokens, ast)) {
            finalizePipeline(result);
            return result;
        }
        
        if (!shouldContinueAfterStage("syntax_analysis")) {
            finalizePipeline(result);
            return result;
        }
        
        // Etapa 4: Análise Semântica
        if (!runSemanticAnalysisStage(ast)) {
            finalizePipeline(result);
            return result;
        }
        
        if (!shouldContinueAfterStage("semantic_analysis")) {
            finalizePipeline(result);
            return result;
        }
        
        // Etapa 5: Geração de Código
        if (!runCodeGenerationStage(ast, result.output_code)) {
            finalizePipeline(result);
            return result;
        }
        
        // Sucesso!
        result.success = true;
        result.dependencies = preproc_result.dependencies;
        
    } catch (const std::exception& e) {
        error_handler->reportPreprocessorError(
            "Exceção durante compilação: " + std::string(e.what()),
            SourcePosition(filename, 1, 1)
        );
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "Compilação concluída em " << duration.count() << "ms" << std::endl;
    
    finalizePipeline(result);
    return result;
}

bool CompilationPipeline::runPreprocessingStage(
    const std::string& input,
    const std::string& filename,
    PreprocessorLexerInterface::ProcessingResult& result) {
    
    std::cout << "[1/5] Executando pré-processamento..." << std::endl;
    
    if (input.empty()) {
        // Processar arquivo
        result = PreprocessorLexerInterface::processForLexer(filename, config);
    } else {
        // Processar string
        result = PreprocessorLexerInterface::processStringForLexer(input, filename, config);
    }
    
    if (!result.success) {
        error_handler->reportPreprocessorError(
            "Falha no pré-processamento: " + result.error_message,
            SourcePosition(filename, 1, 1)
        );
        return false;
    }
    
    // Atualizar estatísticas
    context->stats.files_processed++;
    context->stats.lines_processed += result.statistics.lines_processed;
    
    std::cout << "  Linhas processadas: " << result.statistics.lines_processed << std::endl;
    std::cout << "  Macros expandidas: " << result.statistics.macros_expanded << std::endl;
    std::cout << "  Arquivos incluídos: " << result.dependencies.size() << std::endl;
    
    return true;
}

bool CompilationPipeline::runLexicalAnalysisStage(
    const PreprocessorLexerInterface::ProcessingResult& preproc_result,
    std::vector<Token>& tokens) {
    
    std::cout << "[2/5] Executando análise léxica..." << std::endl;
    
    // Aqui seria chamado o analisador léxico real
    // Por enquanto, simulamos o processo
    
    try {
        // Lexer lexer(preproc_result.expanded_code, error_handler);
        // tokens = lexer.tokenize();
        
        // Simulação:
        context->stats.tokens_generated = 1000; // Exemplo
        
        std::cout << "  Tokens gerados: " << context->stats.tokens_generated << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        error_handler->reportLexerError(
            "Erro na análise léxica: " + std::string(e.what()),
            SourcePosition("<expanded>", 1, 1)
        );
        return false;
    }
}

bool CompilationPipeline::shouldContinueAfterStage(const std::string& stage_name) {
    if (error_handler->hasErrors()) {
        std::cout << "Parando compilação após " << stage_name 
                  << " devido a erros" << std::endl;
        return false;
    }
    return true;
}

void CompilationPipeline::finalizePipeline(PipelineResult& result) {
    result.statistics = context->stats;
    result.error_summary = context->generateReport();
    
    if (context->hasErrors()) {
        std::cout << "\nCompilação falhou com " << context->getErrorCount() 
                  << " erro(s)" << std::endl;
    }
    
    if (context->hasWarnings()) {
        std::cout << "Avisos: " << context->getWarningCount() << std::endl;
    }
}

} // namespace Compiler
```

---

## Configuração e Inicialização

### Exemplo de Uso Completo

```cpp
// main_integration_example.cpp
#include "compilation_pipeline.hpp"
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo.c> [opções]" << std::endl;
        return 1;
    }
    
    // Configurar compilador
    Compiler::CompilerConfiguration config;
    
    // Caminhos de inclusão padrão
    config.include_paths = {
        "./include",
        "/usr/include",
        "/usr/local/include"
    };
    
    // Macros predefinidas
    config.predefined_macros = {
        {"__STDC__", "1"},
        {"__STDC_VERSION__", "201112L"},
        {"DEBUG", "1"}
    };
    
    // Configurações
    config.c_version = Preprocessor::CVersion::C11;
    config.enable_warnings = true;
    config.enable_debug_info = true;
    config.optimize_performance = true;
    config.target_platform = "linux";
    
    // Processar argumentos da linha de comando
    std::string input_file = argv[1];
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "-I") {
            config.include_paths.push_back(arg.substr(2));
        } else if (arg.substr(0, 2) == "-D") {
            std::string define = arg.substr(2);
            size_t eq_pos = define.find('=');
            if (eq_pos != std::string::npos) {
                config.predefined_macros.emplace_back(
                    define.substr(0, eq_pos),
                    define.substr(eq_pos + 1)
                );
            } else {
                config.predefined_macros.emplace_back(define, "1");
            }
        } else if (arg == "-Wall") {
            config.enable_warnings = true;
        } else if (arg == "-O2") {
            config.optimize_performance = true;
        }
    }
    
    // Criar pipeline de compilação
    Compiler::CompilationPipeline pipeline(config);
    
    // Compilar arquivo
    auto result = pipeline.compileFile(input_file);
    
    if (result.success) {
        std::cout << "\n=== Compilação Bem-sucedida ===" << std::endl;
        std::cout << "Arquivo de saída gerado com sucesso" << std::endl;
        std::cout << "Dependências: " << result.dependencies.size() << std::endl;
        
        // Salvar resultado
        std::string output_file = input_file + ".out";
        std::ofstream out(output_file);
        if (out.is_open()) {
            out << result.output_code;
            out.close();
            std::cout << "Saída salva em: " << output_file << std::endl;
        }
        
        return 0;
    } else {
        std::cerr << "\n=== Compilação Falhou ===" << std::endl;
        std::cerr << result.error_summary << std::endl;
        return 1;
    }
}
```

---

## Testes de Integração

### Framework de Testes

```cpp
// integration_tests.cpp
#include "compilation_pipeline.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

class IntegrationTestSuite {
private:
    std::string test_dir;
    int tests_run = 0;
    int tests_passed = 0;
    
public:
    IntegrationTestSuite(const std::string& dir) : test_dir(dir) {
        std::filesystem::create_directories(test_dir);
    }
    
    void runAllTests() {
        std::cout << "=== Executando Testes de Integração ===\n" << std::endl;
        
        testBasicPreprocessing();
        testMacroExpansion();
        testIncludeProcessing();
        testConditionalCompilation();
        testErrorHandling();
        testPositionMapping();
        testPerformance();
        
        printSummary();
    }
    
private:
    void testBasicPreprocessing() {
        std::cout << "Teste: Pré-processamento básico..." << std::endl;
        
        std::string test_code = R"(
#define MAX_SIZE 100
#include <stdio.h>

int main() {
    int array[MAX_SIZE];
    printf("Array size: %d\n", MAX_SIZE);
    return 0;
}
)";
        
        auto result = compileTestCode(test_code, "basic_test.c");
        
        assert(result.success);
        assert(result.output_code.find("100") != std::string::npos);
        assert(result.dependencies.size() > 0);
        
        recordTestResult("Basic Preprocessing", true);
    }
    
    void testMacroExpansion() {
        std::cout << "Teste: Expansão de macros..." << std::endl;
        
        std::string test_code = R"(
#define ADD(a, b) ((a) + (b))
#define SQUARE(x) ((x) * (x))

int main() {
    int result = ADD(SQUARE(3), SQUARE(4));
    return result;
}
)";
        
        auto result = compileTestCode(test_code, "macro_test.c");
        
        assert(result.success);
        assert(result.output_code.find("((3) * (3))") != std::string::npos);
        assert(result.output_code.find("((4) * (4))") != std::string::npos);
        
        recordTestResult("Macro Expansion", true);
    }
    
    void testIncludeProcessing() {
        std::cout << "Teste: Processamento de includes..." << std::endl;
        
        // Criar arquivo de header temporário
        std::string header_path = test_dir + "/test_header.h";
        std::ofstream header(header_path);
        header << "#define HEADER_CONSTANT 42\n";
        header << "int test_function(int x);\n";
        header.close();
        
        std::string test_code = R"(
#include "test_header.h"

int main() {
    int value = HEADER_CONSTANT;
    return test_function(value);
}
)";
        
        Compiler::CompilerConfiguration config;
        config.include_paths = {test_dir};
        
        auto result = compileTestCode(test_code, "include_test.c", config);
        
        assert(result.success);
        assert(result.output_code.find("42") != std::string::npos);
        assert(result.dependencies.size() > 0);
        
        // Verificar se o header está nas dependências
        bool header_found = false;
        for (const auto& dep : result.dependencies) {
            if (dep.find("test_header.h") != std::string::npos) {
                header_found = true;
                break;
            }
        }
        assert(header_found);
        
        recordTestResult("Include Processing", true);
    }
    
    void testConditionalCompilation() {
        std::cout << "Teste: Compilação condicional..." << std::endl;
        
        std::string test_code = R"(
#define DEBUG 1

int main() {
#ifdef DEBUG
    printf("Debug mode enabled\n");
#else
    printf("Release mode\n");
#endif

#if DEBUG > 0
    int debug_var = 1;
#endif

    return 0;
}
)";
        
        auto result = compileTestCode(test_code, "conditional_test.c");
        
        assert(result.success);
        assert(result.output_code.find("Debug mode enabled") != std::string::npos);
        assert(result.output_code.find("Release mode") == std::string::npos);
        assert(result.output_code.find("debug_var") != std::string::npos);
        
        recordTestResult("Conditional Compilation", true);
    }
    
    void testErrorHandling() {
        std::cout << "Teste: Tratamento de erros..." << std::endl;
        
        std::string test_code = R"(
#include "nonexistent_file.h"
#define INVALID_MACRO(

int main() {
    UNDEFINED_MACRO();
    return 0;
}
)";
        
        auto result = compileTestCode(test_code, "error_test.c");
        
        assert(!result.success);
        assert(!result.error_summary.empty());
        assert(result.statistics.errors_count > 0);
        
        recordTestResult("Error Handling", true);
    }
    
    void testPositionMapping() {
        std::cout << "Teste: Mapeamento de posições..." << std::endl;
        
        std::string test_code = R"(
#define LINE1 "First line"
#define LINE2 "Second line"

int main() {
    printf(LINE1);
    printf(LINE2);
    return 0;
}
)";
        
        Compiler::CompilerConfiguration config;
        Compiler::CompilationPipeline pipeline(config);
        
        auto result = pipeline.compileString(test_code, "position_test.c");
        
        // Verificar se o contexto tem informações de posição
        auto context = pipeline.getContext();
        assert(context != nullptr);
        
        recordTestResult("Position Mapping", true);
    }
    
    void testPerformance() {
        std::cout << "Teste: Performance..." << std::endl;
        
        // Gerar código de teste grande
        std::ostringstream large_code;
        large_code << "#include <stdio.h>\n";
        
        for (int i = 0; i < 1000; ++i) {
            large_code << "#define MACRO_" << i << " " << i << "\n";
        }
        
        large_code << "int main() {\n";
        for (int i = 0; i < 1000; ++i) {
            large_code << "    int var_" << i << " = MACRO_" << i << ";\n";
        }
        large_code << "    return 0;\n}\n";
        
        auto start = std::chrono::high_resolution_clock::now();
        auto result = compileTestCode(large_code.str(), "performance_test.c");
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        assert(result.success);
        assert(duration.count() < 5000); // Deve completar em menos de 5 segundos
        
        std::cout << "  Tempo de compilação: " << duration.count() << "ms" << std::endl;
        
        recordTestResult("Performance", true);
    }
    
    Compiler::CompilationPipeline::PipelineResult 
    compileTestCode(const std::string& code, 
                   const std::string& filename,
                   const Compiler::CompilerConfiguration& config = {}) {
        
        Compiler::CompilerConfiguration test_config = config;
        if (test_config.include_paths.empty()) {
            test_config.include_paths = {"/usr/include", test_dir};
        }
        
        test_config.c_version = Preprocessor::CVersion::C11;
        test_config.enable_warnings = true;
        
        Compiler::CompilationPipeline pipeline(test_config);
        return pipeline.compileString(code, filename);
    }
    
    void recordTestResult(const std::string& test_name, bool passed) {
        tests_run++;
        if (passed) {
            tests_passed++;
            std::cout << "  ✓ " << test_name << " PASSOU" << std::endl;
        } else {
            std::cout << "  ✗ " << test_name << " FALHOU" << std::endl;
        }
        std::cout << std::endl;
    }
    
    void printSummary() {
        std::cout << "=== Resumo dos Testes ===" << std::endl;
        std::cout << "Testes executados: " << tests_run << std::endl;
        std::cout << "Testes aprovados: " << tests_passed << std::endl;
        std::cout << "Testes falharam: " << (tests_run - tests_passed) << std::endl;
        std::cout << "Taxa de sucesso: " << (100.0 * tests_passed / tests_run) << "%" << std::endl;
        
        if (tests_passed == tests_run) {
            std::cout << "\n🎉 Todos os testes passaram!" << std::endl;
        } else {
            std::cout << "\n❌ Alguns testes falharam." << std::endl;
        }
    }
};

int main() {
    IntegrationTestSuite test_suite("./test_temp");
    test_suite.runAllTests();
    
    // Limpar arquivos temporários
    std::filesystem::remove_all("./test_temp");
    
    return 0;
}
```

---

## Troubleshooting

### Problemas Comuns e Soluções

#### 1. Erros de Mapeamento de Posições

**Problema**: Posições de erro apontam para código expandido em vez do original.

**Solução**:
```cpp
// Verificar se o PositionMapper está configurado
if (!position_mapper) {
    std::cerr << "Aviso: PositionMapper não configurado" << std::endl;
}

// Sempre mapear posições antes de reportar erros
SourcePosition original_pos = position_mapper->mapToOriginal(expanded_pos);
error_handler->reportError(message, original_pos);
```

#### 2. Performance Lenta

**Problema**: Compilação muito lenta com arquivos grandes.

**Solução**:
```cpp
// Habilitar otimizações de cache
config.optimize_performance = true;

// Configurar cache de arquivos
file_manager.configureCacheOptimization(
    100 * 1024 * 1024,  // 100MB cache
    5000,                // 5000 entradas
    std::chrono::seconds(600), // 10 min TTL
    true                 // Habilitar compressão
);

// Pré-carregar headers comuns
std::vector<std::string> common_headers = {
    "stdio.h", "stdlib.h", "string.h"
};
file_manager.preloadFiles(common_headers);
```

#### 3. Vazamentos de Memória

**Problema**: Uso excessivo de memória durante compilação.

**Solução**:
```cpp
// Limpar cache periodicamente
if (files_processed % 100 == 0) {
    file_manager.clearCache();
    macro_processor.clearCache();
}

// Usar smart pointers
std::shared_ptr<CompilationContext> context;
std::unique_ptr<PositionMapper> mapper;

// Reset do pré-processador após cada arquivo
preprocessor.reset();
```

#### 4. Erros de Inclusão Circular

**Problema**: Detecção incorreta de inclusões circulares.

**Solução**:
```cpp
// Implementar include guards automáticos
class IncludeGuardManager {
    std::set<std::string> processed_files;
    
public:
    bool shouldProcess(const std::string& filepath) {
        std::string canonical = std::filesystem::canonical(filepath);
        return processed_files.find(canonical) == processed_files.end();
    }
    
    void markProcessed(const std::string& filepath) {
        std::string canonical = std::filesystem::canonical(filepath);
        processed_files.insert(canonical);
    }
};
```

---

*Este guia de integração fornece uma base sólida para integrar o pré-processador C com outros componentes do compilador. Para implementações específicas, consulte a documentação de cada módulo individual.*