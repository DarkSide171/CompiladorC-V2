# Configuração do Pré-processador C

## Visão Geral

Este documento detalha todas as opções de configuração disponíveis para o pré-processador C, incluindo configurações de compilação, otimizações, caminhos de busca, macros predefinidas e opções de debug.

## Índice

1. [Configuração Básica](#configuração-básica)
2. [Caminhos de Busca](#caminhos-de-busca)
3. [Macros Predefinidas](#macros-predefinidas)
4. [Versões do Padrão C](#versões-do-padrão-c)
5. [Configurações de Performance](#configurações-de-performance)
6. [Configurações de Debug](#configurações-de-debug)
7. [Configurações de Cache](#configurações-de-cache)
8. [Configurações de Erro](#configurações-de-erro)
9. [Configurações Específicas de Plataforma](#configurações-específicas-de-plataforma)
10. [Arquivo de Configuração](#arquivo-de-configuração)
11. [Variáveis de Ambiente](#variáveis-de-ambiente)
12. [Exemplos de Configuração](#exemplos-de-configuração)

---

## Configuração Básica

### Estrutura de Configuração Principal

```cpp
// preprocessor_config.hpp
struct PreprocessorConfiguration {
    // === Configurações Básicas ===
    CVersion c_version = CVersion::C11;          // Versão do padrão C
    bool strict_mode = false;                    // Modo estrito de conformidade
    bool enable_extensions = true;               // Habilitar extensões específicas
    std::string target_platform = "generic";    // Plataforma alvo
    
    // === Caminhos e Arquivos ===
    std::vector<std::string> include_paths;      // Caminhos de inclusão
    std::vector<std::string> system_include_paths; // Caminhos de sistema
    std::string working_directory = ".";         // Diretório de trabalho
    
    // === Macros ===
    std::map<std::string, std::string> predefined_macros; // Macros predefinidas
    std::set<std::string> undefined_macros;      // Macros para remover
    bool enable_builtin_macros = true;          // Macros built-in (__FILE__, __LINE__, etc.)
    
    // === Comportamento ===
    bool preserve_comments = false;              // Preservar comentários
    bool preserve_whitespace = false;            // Preservar espaços em branco
    bool generate_line_directives = true;        // Gerar diretivas #line
    bool enable_trigraphs = false;               // Habilitar trigrafos
    bool enable_digraphs = true;                 // Habilitar dígrafos
    
    // === Performance ===
    PerformanceConfig performance;
    
    // === Debug ===
    DebugConfig debug;
    
    // === Cache ===
    CacheConfig cache;
    
    // === Tratamento de Erros ===
    ErrorConfig error_handling;
};
```

### Configuração Através de API

```cpp
// Exemplo de configuração programática
PreprocessorMain preprocessor;
PreprocessorConfiguration config;

// Configurações básicas
config.c_version = CVersion::C17;
config.strict_mode = true;
config.target_platform = "linux";

// Aplicar configuração
preprocessor.configure(config);

// Ou configurar individualmente
preprocessor.setVersion(CVersion::C17);
preprocessor.setStrictMode(true);
preprocessor.setTargetPlatform("linux");
```

---

## Caminhos de Busca

### Tipos de Caminhos

```cpp
struct PathConfiguration {
    // Caminhos para #include "arquivo.h" (busca local primeiro)
    std::vector<std::string> quote_include_paths;
    
    // Caminhos para #include <arquivo.h> (busca apenas em sistema)
    std::vector<std::string> angle_include_paths;
    
    // Caminhos de sistema (sempre incluídos)
    std::vector<std::string> system_paths;
    
    // Diretório de trabalho atual
    std::string working_directory;
    
    // Configurações de busca
    bool search_current_directory = true;        // Buscar no diretório atual
    bool case_sensitive_paths = true;            // Caminhos sensíveis a maiúsculas
    bool follow_symlinks = true;                 // Seguir links simbólicos
    int max_include_depth = 200;                 // Profundidade máxima de inclusão
};
```

### Configuração de Caminhos

```cpp
// Adicionar caminhos de inclusão
preprocessor.addIncludePath("/usr/include");
preprocessor.addIncludePath("/usr/local/include");
preprocessor.addIncludePath("./include");

// Definir caminhos de sistema
std::vector<std::string> system_paths = {
    "/usr/include",
    "/usr/include/linux",
    "/usr/lib/gcc/x86_64-linux-gnu/9/include"
};
preprocessor.setSystemIncludePaths(system_paths);

// Configurar busca
preprocessor.setMaxIncludeDepth(100);
preprocessor.setCaseSensitivePaths(true);
```

### Caminhos Padrão por Plataforma

```cpp
// Linux
std::vector<std::string> linux_system_paths = {
    "/usr/include",
    "/usr/local/include",
    "/usr/include/x86_64-linux-gnu",
    "/usr/lib/gcc/x86_64-linux-gnu/9/include"
};

// macOS
std::vector<std::string> macos_system_paths = {
    "/usr/include",
    "/usr/local/include",
    "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include"
};

// Windows (MinGW)
std::vector<std::string> windows_system_paths = {
    "C:/MinGW/include",
    "C:/MinGW/lib/gcc/mingw32/9.2.0/include",
    "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/include"
};
```

---

## Macros Predefinidas

### Configuração de Macros

```cpp
struct MacroConfiguration {
    // Macros definidas pelo usuário
    std::map<std::string, std::string> user_defined;
    
    // Macros de sistema automáticas
    bool enable_standard_macros = true;          // __STDC__, __STDC_VERSION__, etc.
    bool enable_compiler_macros = true;          // __GNUC__, __clang__, etc.
    bool enable_platform_macros = true;          // __linux__, __APPLE__, _WIN32, etc.
    bool enable_builtin_macros = true;           // __FILE__, __LINE__, __DATE__, etc.
    
    // Macros para remover
    std::set<std::string> undefined_macros;
    
    // Configurações de expansão
    int max_expansion_depth = 1000;              // Profundidade máxima de expansão
    bool enable_variadic_macros = true;          // Macros com argumentos variádicos
    bool enable_macro_concatenation = true;      // Operador ##
    bool enable_stringification = true;          // Operador #
};
```

### Macros Padrão por Versão C

```cpp
// C89/C90
std::map<std::string, std::string> c89_macros = {
    {"__STDC__", "1"},
    {"__STDC_VERSION__", "199409L"}
};

// C99
std::map<std::string, std::string> c99_macros = {
    {"__STDC__", "1"},
    {"__STDC_VERSION__", "199901L"},
    {"__STDC_HOSTED__", "1"},
    {"__STDC_IEC_559__", "1"},
    {"__STDC_IEC_559_COMPLEX__", "1"},
    {"__STDC_ISO_10646__", "201706L"}
};

// C11
std::map<std::string, std::string> c11_macros = {
    {"__STDC__", "1"},
    {"__STDC_VERSION__", "201112L"},
    {"__STDC_HOSTED__", "1"},
    {"__STDC_NO_ATOMICS__", "1"},
    {"__STDC_NO_COMPLEX__", "1"},
    {"__STDC_NO_THREADS__", "1"},
    {"__STDC_NO_VLA__", "1"}
};

// C17/C18
std::map<std::string, std::string> c17_macros = {
    {"__STDC__", "1"},
    {"__STDC_VERSION__", "201710L"},
    {"__STDC_HOSTED__", "1"}
};
```

### Macros de Plataforma

```cpp
// Linux
std::map<std::string, std::string> linux_macros = {
    {"__linux__", "1"},
    {"__gnu_linux__", "1"},
    {"__unix__", "1"},
    {"__unix", "1"},
    {"unix", "1"},
    {"_GNU_SOURCE", "1"}
};

// macOS
std::map<std::string, std::string> macos_macros = {
    {"__APPLE__", "1"},
    {"__MACH__", "1"},
    {"__unix__", "1"},
    {"__unix", "1"},
    {"unix", "1"}
};

// Windows
std::map<std::string, std::string> windows_macros = {
    {"_WIN32", "1"},
    {"_WIN64", "1"},  // Para 64-bit
    {"_MSC_VER", "1900"},  // Versão do compilador
    {"_MSC_FULL_VER", "190024210"}
};
```

---

## Versões do Padrão C

### Enumeração de Versões

```cpp
enum class CVersion {
    C89,        // C89/C90 (ISO/IEC 9899:1990)
    C94,        // C94 (ISO/IEC 9899-1:1994) - Amendment 1
    C99,        // C99 (ISO/IEC 9899:1999)
    C11,        // C11 (ISO/IEC 9899:2011)
    C17,        // C17/C18 (ISO/IEC 9899:2018)
    C23,        // C23 (ISO/IEC 9899:2023) - Draft
    GNU89,      // GNU C89 com extensões
    GNU99,      // GNU C99 com extensões
    GNU11,      // GNU C11 com extensões
    GNU17       // GNU C17 com extensões
};
```

### Configuração por Versão

```cpp
struct VersionConfiguration {
    CVersion version;
    
    // Recursos habilitados por versão
    struct Features {
        bool variadic_macros = false;
        bool long_long_type = false;
        bool flexible_array_members = false;
        bool designated_initializers = false;
        bool compound_literals = false;
        bool variable_length_arrays = false;
        bool inline_functions = false;
        bool restrict_keyword = false;
        bool complex_numbers = false;
        bool generic_selections = false;
        bool static_assertions = false;
        bool anonymous_structs_unions = false;
        bool thread_local_storage = false;
        bool atomic_types = false;
        bool alignas_alignof = false;
    } features;
    
    // Extensões específicas do compilador
    struct Extensions {
        bool gnu_extensions = false;
        bool microsoft_extensions = false;
        bool clang_extensions = false;
        bool custom_extensions = false;
    } extensions;
};
```

---

## Configurações de Performance

### Estrutura de Performance

```cpp
struct PerformanceConfig {
    // === Cache ===
    bool enable_file_cache = true;               // Cache de arquivos
    bool enable_macro_cache = true;              // Cache de expansões de macro
    bool enable_include_cache = true;            // Cache de includes processados
    
    size_t file_cache_size = 100 * 1024 * 1024; // 100MB
    size_t macro_cache_entries = 10000;          // 10k entradas
    size_t include_cache_entries = 1000;         // 1k entradas
    
    // === Otimizações ===
    bool optimize_macro_expansion = true;        // Otimizar expansão de macros
    bool optimize_include_processing = true;     // Otimizar processamento de includes
    bool optimize_conditional_evaluation = true; // Otimizar avaliação condicional
    
    // === Threading ===
    bool enable_parallel_processing = false;    // Processamento paralelo
    int max_worker_threads = 4;                 // Número máximo de threads
    
    // === Memory Management ===
    bool enable_memory_pooling = true;          // Pool de memória
    size_t memory_pool_size = 50 * 1024 * 1024; // 50MB
    bool enable_garbage_collection = true;      // Coleta de lixo periódica
    int gc_frequency = 1000;                     // A cada 1000 operações
    
    // === I/O Optimization ===
    bool enable_async_io = false;               // I/O assíncrono
    size_t io_buffer_size = 64 * 1024;          // 64KB buffer
    bool enable_file_mapping = true;            // Memory-mapped files
    
    // === Profiling ===
    bool enable_profiling = false;              // Profiling de performance
    std::string profile_output_file;            // Arquivo de saída do profiling
};
```

### Configuração de Performance

```cpp
// Configuração para desenvolvimento (debug)
PerformanceConfig debug_config;
debug_config.enable_file_cache = false;      // Sempre recarregar arquivos
debug_config.enable_profiling = true;        // Habilitar profiling
debug_config.profile_output_file = "profile.json";

// Configuração para produção (release)
PerformanceConfig release_config;
release_config.enable_file_cache = true;
release_config.file_cache_size = 200 * 1024 * 1024; // 200MB
release_config.optimize_macro_expansion = true;
release_config.enable_parallel_processing = true;
release_config.max_worker_threads = std::thread::hardware_concurrency();

// Aplicar configuração
preprocessor.setPerformanceConfig(release_config);
```

---

## Configurações de Debug

### Estrutura de Debug

```cpp
struct DebugConfig {
    // === Níveis de Debug ===
    enum class Level {
        NONE,           // Sem debug
        ERROR,          // Apenas erros
        WARNING,        // Erros e avisos
        INFO,           // Informações gerais
        VERBOSE,        // Informações detalhadas
        TRACE           // Rastreamento completo
    } level = Level::WARNING;
    
    // === Saídas de Debug ===
    std::string log_file;                       // Arquivo de log
    bool log_to_console = true;                 // Log no console
    bool log_to_file = false;                   // Log em arquivo
    
    // === Informações a Registrar ===
    bool log_file_operations = false;          // Operações de arquivo
    bool log_macro_expansions = false;         // Expansões de macro
    bool log_include_processing = false;       // Processamento de includes
    bool log_conditional_evaluation = false;   // Avaliação condicional
    bool log_error_recovery = false;           // Recuperação de erros
    
    // === Dump de Estado ===
    bool dump_preprocessor_state = false;      // Estado do pré-processador
    bool dump_macro_definitions = false;       // Definições de macro
    bool dump_include_stack = false;           // Pilha de includes
    bool dump_conditional_stack = false;       // Pilha condicional
    
    // === Timing ===
    bool measure_performance = false;          // Medir performance
    bool detailed_timing = false;              // Timing detalhado
    
    // === Memory Debugging ===
    bool track_memory_usage = false;           // Rastrear uso de memória
    bool detect_memory_leaks = false;          // Detectar vazamentos
    
    // === Formatação ===
    bool use_colors = true;                     // Usar cores no output
    bool show_timestamps = true;                // Mostrar timestamps
    bool show_thread_ids = false;               // Mostrar IDs de thread
};
```

### Configuração de Debug

```cpp
// Debug básico
DebugConfig basic_debug;
basic_debug.level = DebugConfig::Level::INFO;
basic_debug.log_to_console = true;
basic_debug.use_colors = true;

// Debug avançado
DebugConfig advanced_debug;
advanced_debug.level = DebugConfig::Level::TRACE;
advanced_debug.log_to_file = true;
advanced_debug.log_file = "preprocessor_debug.log";
advanced_debug.log_macro_expansions = true;
advanced_debug.log_include_processing = true;
advanced_debug.dump_preprocessor_state = true;
advanced_debug.measure_performance = true;

// Aplicar configuração
preprocessor.setDebugConfig(advanced_debug);
```

---

## Configurações de Cache

### Estrutura de Cache

```cpp
struct CacheConfig {
    // === Cache de Arquivos ===
    struct FileCache {
        bool enabled = true;
        size_t max_size = 100 * 1024 * 1024;    // 100MB
        size_t max_entries = 5000;              // 5000 arquivos
        std::chrono::seconds ttl{600};          // 10 minutos TTL
        bool compress_entries = false;          // Comprimir entradas
        bool persistent = false;                // Cache persistente
        std::string cache_directory;            // Diretório do cache
    } file_cache;
    
    // === Cache de Macros ===
    struct MacroCache {
        bool enabled = true;
        size_t max_entries = 10000;             // 10k macros
        bool cache_expansions = true;           // Cache de expansões
        bool cache_function_macros = true;      // Cache de macros funcionais
        std::chrono::seconds ttl{300};          // 5 minutos TTL
    } macro_cache;
    
    // === Cache de Includes ===
    struct IncludeCache {
        bool enabled = true;
        size_t max_entries = 1000;              // 1k includes
        bool cache_system_headers = true;       // Cache de headers de sistema
        bool cache_user_headers = true;         // Cache de headers do usuário
        std::chrono::seconds ttl{1800};         // 30 minutos TTL
    } include_cache;
    
    // === Configurações Gerais ===
    bool enable_lru_eviction = true;           // Eviction LRU
    bool enable_statistics = true;             // Estatísticas de cache
    int cleanup_frequency = 100;               // Limpeza a cada 100 operações
};
```

---

## Configurações de Erro

### Estrutura de Tratamento de Erros

```cpp
struct ErrorConfig {
    // === Níveis de Severidade ===
    enum class Severity {
        NOTE,
        WARNING,
        ERROR,
        FATAL
    };
    
    // === Comportamento ===
    bool warnings_as_errors = false;           // Tratar avisos como erros
    bool continue_on_error = true;              // Continuar após erro
    int max_errors = 100;                       // Máximo de erros
    int max_warnings = 1000;                    // Máximo de avisos
    
    // === Tipos de Erro ===
    bool enable_syntax_errors = true;
    bool enable_semantic_errors = true;
    bool enable_include_errors = true;
    bool enable_macro_errors = true;
    bool enable_conditional_errors = true;
    
    // === Tipos de Aviso ===
    bool warn_unused_macros = false;
    bool warn_redefined_macros = true;
    bool warn_missing_includes = true;
    bool warn_deprecated_features = true;
    bool warn_non_portable_code = false;
    bool warn_trigraphs = false;
    
    // === Formatação de Erros ===
    bool show_source_context = true;           // Mostrar contexto do código
    int context_lines = 3;                     // Linhas de contexto
    bool show_column_indicators = true;        // Indicadores de coluna
    bool use_colors = true;                     // Usar cores
    bool show_error_codes = false;             // Mostrar códigos de erro
    
    // === Recuperação de Erros ===
    bool enable_error_recovery = true;         // Recuperação de erros
    bool aggressive_recovery = false;          // Recuperação agressiva
    
    // === Saída ===
    std::string error_format = "gcc";          // Formato: gcc, msvc, clang
    std::string error_log_file;                // Arquivo de log de erros
};
```

---

## Configurações Específicas de Plataforma

### Linux

```cpp
PreprocessorConfiguration createLinuxConfig() {
    PreprocessorConfiguration config;
    
    // Versão e extensões
    config.c_version = CVersion::GNU11;
    config.enable_extensions = true;
    config.target_platform = "linux";
    
    // Caminhos de sistema
    config.include_paths = {
        "/usr/include",
        "/usr/local/include",
        "/usr/include/x86_64-linux-gnu"
    };
    
    // Macros específicas
    config.predefined_macros = {
        {"__linux__", "1"},
        {"__gnu_linux__", "1"},
        {"__unix__", "1"},
        {"_GNU_SOURCE", "1"},
        {"_DEFAULT_SOURCE", "1"}
    };
    
    return config;
}
```

### macOS

```cpp
PreprocessorConfiguration createMacOSConfig() {
    PreprocessorConfiguration config;
    
    config.c_version = CVersion::C17;
    config.target_platform = "macos";
    
    config.include_paths = {
        "/usr/include",
        "/usr/local/include",
        "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include"
    };
    
    config.predefined_macros = {
        {"__APPLE__", "1"},
        {"__MACH__", "1"},
        {"__unix__", "1"},
        {"__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__", "101500"}
    };
    
    return config;
}
```

### Windows

```cpp
PreprocessorConfiguration createWindowsConfig() {
    PreprocessorConfiguration config;
    
    config.c_version = CVersion::C17;
    config.target_platform = "windows";
    
    config.include_paths = {
        "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/include",
        "C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/ucrt",
        "C:/MinGW/include"
    };
    
    config.predefined_macros = {
        {"_WIN32", "1"},
        {"_WIN64", "1"},
        {"_MSC_VER", "1929"},
        {"_MSC_FULL_VER", "192930133"},
        {"_CRT_SECURE_NO_WARNINGS", "1"}
    };
    
    return config;
}
```

---

## Arquivo de Configuração

### Formato JSON

```json
{
  "preprocessor": {
    "version": "C17",
    "strict_mode": false,
    "target_platform": "linux",
    
    "paths": {
      "include_paths": [
        "./include",
        "/usr/include",
        "/usr/local/include"
      ],
      "system_paths": [
        "/usr/include",
        "/usr/include/linux"
      ],
      "working_directory": ".",
      "max_include_depth": 200
    },
    
    "macros": {
      "predefined": {
        "DEBUG": "1",
        "VERSION": "\"1.0.0\"",
        "MAX_BUFFER_SIZE": "4096"
      },
      "undefined": ["NDEBUG"],
      "enable_builtin": true,
      "max_expansion_depth": 1000
    },
    
    "behavior": {
      "preserve_comments": false,
      "preserve_whitespace": false,
      "generate_line_directives": true,
      "enable_trigraphs": false,
      "enable_digraphs": true
    },
    
    "performance": {
      "enable_file_cache": true,
      "file_cache_size": 104857600,
      "enable_parallel_processing": false,
      "max_worker_threads": 4
    },
    
    "debug": {
      "level": "WARNING",
      "log_to_console": true,
      "log_to_file": false,
      "use_colors": true,
      "measure_performance": false
    },
    
    "errors": {
      "warnings_as_errors": false,
      "continue_on_error": true,
      "max_errors": 100,
      "show_source_context": true,
      "context_lines": 3
    }
  }
}
```

### Carregamento de Configuração

```cpp
// Carregar configuração de arquivo JSON
PreprocessorConfiguration loadConfigFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Não foi possível abrir arquivo de configuração: " + filename);
    }
    
    nlohmann::json config_json;
    file >> config_json;
    
    PreprocessorConfiguration config;
    
    // Carregar configurações básicas
    if (config_json.contains("preprocessor")) {
        auto& pp_config = config_json["preprocessor"];
        
        if (pp_config.contains("version")) {
            config.c_version = parseVersion(pp_config["version"]);
        }
        
        if (pp_config.contains("strict_mode")) {
            config.strict_mode = pp_config["strict_mode"];
        }
        
        // Carregar caminhos
        if (pp_config.contains("paths")) {
            auto& paths = pp_config["paths"];
            
            if (paths.contains("include_paths")) {
                config.include_paths = paths["include_paths"];
            }
        }
        
        // Carregar macros
        if (pp_config.contains("macros")) {
            auto& macros = pp_config["macros"];
            
            if (macros.contains("predefined")) {
                config.predefined_macros = macros["predefined"];
            }
        }
        
        // ... carregar outras configurações
    }
    
    return config;
}

// Salvar configuração em arquivo
void saveConfigToFile(const PreprocessorConfiguration& config, const std::string& filename) {
    nlohmann::json config_json;
    
    config_json["preprocessor"]["version"] = versionToString(config.c_version);
    config_json["preprocessor"]["strict_mode"] = config.strict_mode;
    config_json["preprocessor"]["target_platform"] = config.target_platform;
    
    config_json["preprocessor"]["paths"]["include_paths"] = config.include_paths;
    config_json["preprocessor"]["macros"]["predefined"] = config.predefined_macros;
    
    // ... salvar outras configurações
    
    std::ofstream file(filename);
    file << config_json.dump(2);  // Indentação de 2 espaços
}
```

---

## Variáveis de Ambiente

### Variáveis Suportadas

```bash
# Caminhos de inclusão
export C_INCLUDE_PATH="/usr/local/include:/opt/include"
export CPLUS_INCLUDE_PATH="/usr/local/include/c++:/opt/include/c++"

# Macros predefinidas
export PREPROCESSOR_DEFINES="DEBUG=1,VERSION=\"1.0.0\",MAX_SIZE=4096"

# Configurações de debug
export PREPROCESSOR_DEBUG_LEVEL="INFO"
export PREPROCESSOR_LOG_FILE="/tmp/preprocessor.log"

# Configurações de performance
export PREPROCESSOR_CACHE_SIZE="200M"
export PREPROCESSOR_MAX_THREADS="8"

# Configurações de comportamento
export PREPROCESSOR_STRICT_MODE="1"
export PREPROCESSOR_C_VERSION="C17"
```

### Processamento de Variáveis

```cpp
PreprocessorConfiguration loadConfigFromEnvironment() {
    PreprocessorConfiguration config;
    
    // Caminhos de inclusão
    if (const char* include_path = std::getenv("C_INCLUDE_PATH")) {
        config.include_paths = splitPath(include_path, ':');
    }
    
    // Macros predefinidas
    if (const char* defines = std::getenv("PREPROCESSOR_DEFINES")) {
        config.predefined_macros = parseDefines(defines);
    }
    
    // Nível de debug
    if (const char* debug_level = std::getenv("PREPROCESSOR_DEBUG_LEVEL")) {
        config.debug.level = parseDebugLevel(debug_level);
    }
    
    // Versão do C
    if (const char* version = std::getenv("PREPROCESSOR_C_VERSION")) {
        config.c_version = parseVersion(version);
    }
    
    return config;
}

std::vector<std::string> splitPath(const std::string& path, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(path);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

std::map<std::string, std::string> parseDefines(const std::string& defines) {
    std::map<std::string, std::string> result;
    std::stringstream ss(defines);
    std::string define;
    
    while (std::getline(ss, define, ',')) {
        size_t eq_pos = define.find('=');
        if (eq_pos != std::string::npos) {
            std::string name = define.substr(0, eq_pos);
            std::string value = define.substr(eq_pos + 1);
            result[name] = value;
        } else {
            result[define] = "1";
        }
    }
    
    return result;
}
```

---

## Exemplos de Configuração

### Configuração para Desenvolvimento

```cpp
PreprocessorConfiguration createDevelopmentConfig() {
    PreprocessorConfiguration config;
    
    // Configurações básicas
    config.c_version = CVersion::C17;
    config.strict_mode = false;  // Mais permissivo durante desenvolvimento
    config.enable_extensions = true;
    
    // Debug habilitado
    config.debug.level = DebugConfig::Level::INFO;
    config.debug.log_to_console = true;
    config.debug.use_colors = true;
    config.debug.measure_performance = true;
    
    // Cache desabilitado para sempre recarregar arquivos
    config.performance.enable_file_cache = false;
    
    // Avisos habilitados
    config.error_handling.warn_unused_macros = true;
    config.error_handling.warn_redefined_macros = true;
    config.error_handling.warn_deprecated_features = true;
    
    // Macros de debug
    config.predefined_macros = {
        {"DEBUG", "1"},
        {"_DEBUG", "1"},
        {"DEVELOPMENT", "1"}
    };
    
    return config;
}
```

### Configuração para Produção

```cpp
PreprocessorConfiguration createProductionConfig() {
    PreprocessorConfiguration config;
    
    // Configurações básicas
    config.c_version = CVersion::C17;
    config.strict_mode = true;   // Mais rigoroso em produção
    config.enable_extensions = false;
    
    // Debug mínimo
    config.debug.level = DebugConfig::Level::ERROR;
    config.debug.log_to_console = false;
    config.debug.log_to_file = true;
    config.debug.log_file = "/var/log/preprocessor.log";
    
    // Performance otimizada
    config.performance.enable_file_cache = true;
    config.performance.file_cache_size = 200 * 1024 * 1024; // 200MB
    config.performance.optimize_macro_expansion = true;
    config.performance.enable_parallel_processing = true;
    
    // Tratamento rigoroso de erros
    config.error_handling.warnings_as_errors = true;
    config.error_handling.continue_on_error = false;
    
    // Macros de produção
    config.predefined_macros = {
        {"NDEBUG", "1"},
        {"PRODUCTION", "1"},
        {"OPTIMIZE", "1"}
    };
    
    return config;
}
```

### Configuração para Testes

```cpp
PreprocessorConfiguration createTestConfig() {
    PreprocessorConfiguration config;
    
    // Configurações básicas
    config.c_version = CVersion::C11;
    config.strict_mode = true;
    
    // Debug detalhado para testes
    config.debug.level = DebugConfig::Level::VERBOSE;
    config.debug.log_to_file = true;
    config.debug.log_file = "test_preprocessor.log";
    config.debug.log_macro_expansions = true;
    config.debug.dump_preprocessor_state = true;
    
    // Performance com profiling
    config.performance.enable_profiling = true;
    config.performance.profile_output_file = "test_profile.json";
    
    // Todos os avisos habilitados
    config.error_handling.warn_unused_macros = true;
    config.error_handling.warn_redefined_macros = true;
    config.error_handling.warn_missing_includes = true;
    config.error_handling.warn_non_portable_code = true;
    
    // Macros de teste
    config.predefined_macros = {
        {"TESTING", "1"},
        {"UNIT_TEST", "1"},
        {"DEBUG", "1"}
    };
    
    return config;
}
```

---

*Esta documentação de configuração fornece todas as opções disponíveis para personalizar o comportamento do pré-processador C de acordo com suas necessidades específicas.*