# Compilador C - Versão 2

## Descrição do Projeto

Este projeto consiste no desenvolvimento de um compilador completo para a linguagem C, implementado em C++. O compilador é projetado para traduzir código fonte em C para código de máquina, passando por todas as fases tradicionais de compilação.

## Objetivos

- Implementar um compilador funcional para um subconjunto da linguagem C
- Demonstrar o funcionamento das principais fases de compilação
- Criar uma base sólida e extensível para futuras melhorias
- Fornecer mensagens de erro claras e informativas
- Otimizar o código gerado quando possível

## Fases do Compilador

### 1. Análise Léxica (Lexer)
**Status: ✅ Implementado**

- **Objetivo**: Converter o código fonte em uma sequência de tokens
- **Funcionalidades Implementadas**:
  - ✅ Reconhecimento de palavras-chave (if, while, for, etc.)
  - ✅ Identificação de identificadores e literais
  - ✅ Processamento de operadores e delimitadores
  - ✅ Tratamento de comentários e espaços em branco
  - ✅ Detecção de erros léxicos
  - ✅ Múltiplos formatos de saída (verbose, summary, JSON)
  - ✅ Sistema de filtros por categoria de tokens
  - ✅ Estatísticas detalhadas e colorização
  - ✅ Suporte a múltiplos arquivos e diretórios

### 2. Análise Sintática (Parser)
**Status: 🔄 Em Planejamento**

- **Objetivo**: Construir uma árvore sintática abstrata (AST) a partir dos tokens
- **Funcionalidades**:
  - Implementação de gramática para subconjunto de C
  - Construção da AST
  - Detecção de erros sintáticos
  - Recuperação de erros para análise contínua

### 3. Análise Semântica
**Status: 🔄 Em Planejamento**

- **Objetivo**: Verificar a correção semântica do programa
- **Funcionalidades**:
  - Verificação de tipos
  - Análise de escopo e declarações
  - Verificação de compatibilidade de operações
  - Detecção de variáveis não declaradas
  - Verificação de retorno de funções

### 0.5. Pré-processador C
**Status: ✅ Implementado**

- **Objetivo**: Processar diretivas de pré-processamento antes da análise léxica
- **Funcionalidades Implementadas**:
  - ✅ Processamento de diretivas #include
  - ✅ Definição e expansão de macros (#define)
  - ✅ Compilação condicional (#if, #ifdef, #ifndef, #else, #elif, #endif)
  - ✅ Gerenciamento de arquivos e dependências
  - ✅ Avaliação de expressões constantes
  - ✅ Mapeamento de posições para debug
  - ✅ Cache de arquivos e otimizações de performance
  - ✅ Suporte completo aos padrões C89-C23
  - ✅ Interface de integração com lexer
  - ✅ Documentação completa de APIs

### 4. Geração de Código Intermediário
**Status: 🔄 Em Planejamento**

- **Objetivo**: Gerar representação intermediária do código
- **Funcionalidades**:
  - Geração de código de três endereços
  - Otimizações básicas
  - Preparação para geração de código final

### 5. Geração de Código Final
**Status: 🔄 Em Planejamento**

- **Objetivo**: Produzir código de máquina ou assembly
- **Funcionalidades**:
  - Geração de código assembly
  - Alocação de registradores
  - Otimizações de código

## 📁 Estrutura do Projeto

```
CompialdorC_v2/
├── src/
│   ├── preprocessor/       # Pré-processador C (Fase 0.5) ✅ IMPLEMENTADO
│   │   ├── include/        # Headers das classes principais
│   │   │   ├── preprocessor.hpp          # Interface principal
│   │   │   ├── macro_processor.hpp       # Processamento de macros
│   │   │   ├── file_manager.hpp          # Gerenciamento de arquivos
│   │   │   ├── directive_processor.hpp   # Processamento de diretivas
│   │   │   ├── conditional_processor.hpp # Compilação condicional
│   │   │   └── expression_evaluator.hpp  # Avaliação de expressões
│   │   ├── src/            # Implementações (.cpp)
│   │   ├── data/           # Headers padrão C e configurações
│   │   ├── tests/          # Testes unitários e de integração
│   │   └── CMakeLists.txt  # Configuração de build
│   ├── lexer/              # Analisador Léxico (Fase 1)
│   │   ├── include/        # Headers (.hpp)
│   │   ├── src/            # Implementações (.cpp)
│   │   ├── tests/          # Testes unitários
│   │   └── CMakeLists.txt  # Configuração CMake
│   ├── parser/             # Analisador Sintático (Fase 2 - Planejado)
│   ├── semantic/           # Analisador Semântico (Fase 3 - Planejado)
│   └── codegen/            # Geração de Código (Fases 4-5 - Planejado)
├── Tests/                  # Testes unitários e de integração
├── Docs/                   # Documentação técnica completa
│   ├── PREPROCESSADOR_IMPLEMENTACAO.md     # Especificação do preprocessador
│   ├── PREPROCESSOR_API_DOCUMENTATION.md   # Documentação das APIs
│   ├── PREPROCESSOR_USAGE_EXAMPLES.md      # Exemplos de uso
│   ├── PREPROCESSOR_INTEGRATION_GUIDE.md   # Guia de integração
│   ├── PREPROCESSOR_CONFIGURATION.md       # Configurações e opções
│   ├── ANALISADOR_SINTATICO.md             # Especificação do parser
│   └── PROJETO_COMPILADOR_STATUS.md        # Status geral
├── Examples/               # Exemplos de código C para teste
└── CMakeLists.txt          # Configuração de build
```

## Tecnologias Utilizadas

- **Linguagem**: C++17 ou superior
- **Build System**: CMake
- **IDE**: CLion
- **Testes**: Framework de testes a ser definido

## Como Compilar

### Pré-requisitos
- **C++17** ou superior
- **CMake 3.15** ou superior
- **Compilador**: GCC 7+, Clang 6+, ou MSVC 2019+

### Opção 1: Script de Build (Recomendado)
```bash
# Compilação simples
./build.sh

# Compilação com limpeza
./build.sh clean

# Compilação com testes
./build.sh test
```

### Opção 2: CMake Manual
```bash
# Configuração e compilação
mkdir -p build
cd build
cmake ..
make -j$(nproc)

# Executar testes
make test

# Instalar (opcional)
sudo make install
```

### Opção 3: Build Específico do Pré-processador
```bash
# Compilar apenas o pré-processador
cd src/preprocessor
mkdir -p build
cd build
cmake ..
make

# Executar testes do pré-processador
./tests/preprocessor_tests
```

## Como Usar

### Compilador Completo

Após a compilação, o executável `CompiladorC` estará disponível em `build/CompiladorC`.

```bash
# Ajuda geral
./build/CompiladorC --help

# Compilação completa (pré-processador + lexer)
./build/CompiladorC arquivo.c

# Apenas pré-processamento
./build/CompiladorC --preprocess-only arquivo.c

# Saída detalhada
./build/CompiladorC --verbose arquivo.c

# Definir macros
./build/CompiladorC -DDEBUG=1 -DVERSION=\"1.0\" arquivo.c

# Adicionar caminhos de inclusão
./build/CompiladorC -I./include -I/usr/local/include arquivo.c

# Especificar padrão C
./build/CompiladorC --std=c17 arquivo.c
```

### Pré-processador Standalone

```bash
# Executar apenas o pré-processador
./build/src/preprocessor/preprocessor_cli arquivo.c

# Pré-processamento com saída para arquivo
./build/src/preprocessor/preprocessor_cli -o arquivo_processado.c arquivo.c

# Mostrar dependências
./build/src/preprocessor/preprocessor_cli --show-dependencies arquivo.c

# Debug do pré-processador
./build/src/preprocessor/preprocessor_cli --debug --verbose arquivo.c

# Configuração personalizada
./build/src/preprocessor/preprocessor_cli --config config.json arquivo.c
```

### Opções do Compilador

**Controle de Fases:**
- `--preprocess-only` ou `-E`: Apenas pré-processamento
- `--compile-only` ou `-c`: Até análise sintática
- `--assemble-only` ou `-S`: Até geração de assembly

**Configuração:**
- `-D<macro>[=<valor>]`: Definir macro
- `-U<macro>`: Remover definição de macro
- `-I<diretório>`: Adicionar caminho de inclusão
- `--std=<padrão>`: Especificar padrão C (c89, c99, c11, c17)

**Saída e Debug:**
- `--verbose` ou `-v`: Saída detalhada
- `--quiet` ou `-q`: Saída mínima
- `--debug`: Informações de debug
- `-o <arquivo>`: Especificar arquivo de saída

**Otimização:**
- `--optimize` ou `-O`: Habilitar otimizações
- `--cache-size <tamanho>`: Tamanho do cache (ex: 100M)
- `--parallel`: Processamento paralelo quando possível

### Exemplos Práticos

```bash
# Projeto típico C
./build/CompiladorC -I./include -DDEBUG=1 src/main.c

# Código com headers de sistema
./build/CompiladorC -I/usr/include -std=c11 programa.c

# Debug completo do pré-processamento
./build/CompiladorC --preprocess-only --debug --verbose complex_file.c

# Compilação otimizada
./build/CompiladorC --optimize --cache-size 200M --parallel projeto.c

# Verificar dependências
./build/CompiladorC --show-dependencies --preprocess-only main.c
```

## 🗺️ Roadmap de Desenvolvimento

### ✅ Fase 1: Analisador Léxico (CONCLUÍDO)
- **Status**: ✅ Implementado e testado
- **Recursos**: Tokenização completa, tratamento de erros, suporte C89-C23
- **Preprocessamento**: ✅ Reconhecimento básico de diretivas implementado
- **Localização**: `src/lexer/`

### ✅ Fase 0.5: Pré-processador C (IMPLEMENTADO COMPLETAMENTE)
- **Status**: ✅ Implementação completa e funcional
- **Funcionalidades**: 
  - ✅ Processamento completo de diretivas (#include, #define, etc.)
  - ✅ Expansão de macros (simples e funcionais)
  - ✅ Compilação condicional (#if, #ifdef, #ifndef, #else, #elif, #endif)
  - ✅ Inclusão de arquivos com resolução de caminhos
  - ✅ Avaliação de expressões constantes
  - ✅ Gerenciamento de dependências
  - ✅ Cache de arquivos e otimizações
  - ✅ Interface de integração com lexer
- **Documentação**: `Docs/PREPROCESSOR_*.md` (5 documentos completos)
- **Testes**: Suite completa de testes unitários e de integração
- **APIs**: Documentação completa das interfaces públicas

### 🚀 Fase 2: Analisador Sintático (PRONTO PARA IMPLEMENTAÇÃO)
- **Status**: 📋 Especificação completa, pronto para implementação
- **Prioridade**: 🔴 Alta - próxima fase a ser implementada
- **Arquitetura**: Parser recursivo descendente
- **Documentação**: `Docs/ANALISADOR_SINTATICO.md`

### ⏳ Fase 3: Analisador Semântico (ESPECIFICAÇÃO COMPLETA)
- **Status**: 📋 Especificação completa, aguardando parser
- **Componentes**: Tabela de símbolos, verificação de tipos, análise de escopo

### ⏳ Fase 4: Geração de Código Intermediário (PLANEJAMENTO)
- **Status**: 📋 Planejamento inicial
- **Formato**: Three-Address Code ou representação similar

### ⏳ Fase 5: Geração de Código Final (CONCEITUAL)
- **Status**: 📋 Definição de arquitetura alvo
- **Opções**: Assembly x86-64, LLVM IR, ou outras arquiteturas

## Contribuição

Este é um projeto educacional focado no aprendizado dos conceitos de compiladores.

## Licença

A ser definida.