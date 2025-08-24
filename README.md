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
**Status: 🔄 Em Planejamento**

- **Objetivo**: Converter o código fonte em uma sequência de tokens
- **Funcionalidades**:
  - Reconhecimento de palavras-chave (if, while, for, etc.)
  - Identificação de identificadores e literais
  - Processamento de operadores e delimitadores
  - Tratamento de comentários e espaços em branco
  - Detecção de erros léxicos

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

## Estrutura do Projeto

```
CompialdorC_v2/
├── src/
│   ├── lexer/          # Analisador léxico
│   ├── parser/         # Analisador sintático
│   ├── semantic/       # Analisador semântico
│   ├── codegen/        # Geração de código
│   └── utils/          # Utilitários comuns
├── Tests/              # Testes unitários e de integração
├── Docs/               # Documentação detalhada
├── Examples/           # Exemplos de código C para teste
└── CMakeLists.txt      # Configuração de build
```

## Tecnologias Utilizadas

- **Linguagem**: C++17 ou superior
- **Build System**: CMake
- **IDE**: CLion
- **Testes**: Framework de testes a ser definido

## Como Compilar

```bash
mkdir build
cd build
cmake ..
make
```

## Roadmap de Desenvolvimento

- [ ] **Fase 1**: Implementação do Analisador Léxico
- [ ] **Fase 2**: Implementação do Analisador Sintático
- [ ] **Fase 3**: Implementação do Analisador Semântico
- [ ] **Fase 4**: Geração de Código Intermediário
- [ ] **Fase 5**: Geração de Código Final
- [ ] **Fase 6**: Otimizações e Melhorias

## Contribuição

Este é um projeto educacional focado no aprendizado dos conceitos de compiladores.

## Licença

A ser definida.