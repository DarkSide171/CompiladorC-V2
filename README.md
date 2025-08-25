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
│   ├── preprocessor/       # Preprocessador C (Nova Fase 0.5)
│   │   ├── include/        # Headers do preprocessador
│   │   ├── src/            # Implementações
│   │   ├── data/           # Headers padrão e configurações
│   │   ├── tests/          # Testes do preprocessador
│   │   └── CMakeLists.txt  # Build do preprocessador
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
│   ├── PREPROCESSADOR.md   # Especificação do preprocessador
│   ├── ANALISADOR_SINTATICO.md # Especificação do parser
│   └── PROJETO_COMPILADOR_STATUS.md # Status geral
├── Examples/               # Exemplos de código C para teste
└── CMakeLists.txt          # Configuração de build
```

## Tecnologias Utilizadas

- **Linguagem**: C++17 ou superior
- **Build System**: CMake
- **IDE**: CLion
- **Testes**: Framework de testes a ser definido

## Como Compilar

### Opção 1: Script de Build (Recomendado)
```bash
# Compilação simples
./build.sh

# Compilação com limpeza
./build.sh clean
```

### Opção 2: CMake Manual
```bash
mkdir -p build
cd build
cmake ..
make
```

## Como Usar

Após a compilação, o executável `CompiladorC` estará disponível em `build/CompiladorC`.

### Exemplos de Uso

```bash
# Ajuda
./build/CompiladorC --help

# Análise completa (verbose)
./build/CompiladorC arquivo.c

# Saída resumida
./build/CompiladorC --summary arquivo.c

# Saída em JSON
./build/CompiladorC --json arquivo.c

# Filtrar apenas palavras-chave
./build/CompiladorC --filter-keywords arquivo.c

# Combinar filtros
./build/CompiladorC --summary --filter-operators --filter-keywords arquivo.c

# Processar diretório inteiro
./build/CompiladorC --summary src/
```

### Opções Disponíveis

**Formatos de Saída:**
- `--verbose` ou `-v`: Saída detalhada com cores (padrão)
- `--summary` ou `-s`: Saída resumida com estatísticas
- `--json` ou `-j`: Saída em formato JSON

**Filtros por Categoria:**
- `--filter-keywords`: Apenas palavras-chave
- `--filter-operators`: Apenas operadores
- `--filter-identifiers`: Apenas identificadores
- `--filter-literals`: Apenas literais
- `--filter-delimiters`: Apenas delimitadores
- `--filter-punctuation`: Apenas pontuação
- `--filter-preprocessor`: Apenas diretivas de preprocessador

## 🗺️ Roadmap de Desenvolvimento

### ✅ Fase 1: Analisador Léxico (CONCLUÍDO)
- **Status**: ✅ Implementado e testado
- **Recursos**: Tokenização completa, tratamento de erros, suporte C89-C23
- **Preprocessamento**: ✅ Reconhecimento básico de diretivas implementado
- **Localização**: `src/lexer/`

### ✅ Capacidades de Preprocessamento (FUNCIONAL BÁSICO)
- **Status**: ✅ Reconhecimento de diretivas implementado
- **Funcionalidades**: Tokenização de `#include`, `#define`, `#ifdef`, etc.
- **Limitações**: Sem expansão de macros ou inclusão de arquivos
- **Conclusão**: Suficiente para desenvolvimento do parser
- **Documentação**: `Docs/ANALISE_PREPROCESSAMENTO_ATUAL.md`

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