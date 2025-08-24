# Documentação do Analisador Léxico

Este documento detalha a arquitetura e especificações do analisador léxico (lexer) do compilador C, incluindo suporte para múltiplas versões da linguagem C e design modular extensível.

## Visão Geral

O analisador léxico é responsável pela primeira fase da compilação, convertendo o código fonte em uma sequência de tokens que será processada pelo analisador sintático. Este projeto iniciará com a interpretação do C básico, seguido pelo desenvolvimento de pacotes contendo os recursos específicos de cada versão da linguagem (C89, C90, C11, C17 e outras versões disponíveis).

## Estrutura de Arquivos e Diretórios

### Organização do Analisador Léxico

```
src/lexer/
├── include/
│   ├── lexer.hpp              # Interface principal do analisador léxico
│   ├── token.hpp              # Definições de tokens e tipos
│   ├── error_handler.hpp      # Sistema de tratamento de erros
│   ├── symbol_table.hpp       # Tabela de símbolos e palavras-chave
│   ├── lexer_config.hpp       # Configurações e versões do C
│   ├── lexer_state.hpp        # Estados da máquina de estados
│   ├── lookahead_buffer.hpp   # Buffer de lookahead
│   └── lexer_logger.hpp       # Sistema de logging
├── src/
│   ├── lexer.cpp              # Implementação principal do lexer
│   ├── token.cpp              # Implementação de tokens
│   ├── error_handler.cpp      # Implementação do tratamento de erros
│   ├── symbol_table.cpp       # Implementação da tabela de símbolos
│   ├── lexer_config.cpp       # Implementação das configurações
│   ├── lexer_state.cpp        # Implementação da máquina de estados
│   ├── lookahead_buffer.cpp   # Implementação do buffer
│   └── lexer_logger.cpp       # Implementação do sistema de logging
├── data/
│   ├── keywords/
│   │   ├── c89_keywords.txt   # Palavras-chave do C89/C90
│   │   ├── c99_keywords.txt   # Palavras-chave adicionais do C99
│   │   ├── c11_keywords.txt   # Palavras-chave adicionais do C11
│   │   ├── c17_keywords.txt   # Palavras-chave do C17
│   │   └── c23_keywords.txt   # Palavras-chave adicionais do C23
│   ├── operators.txt          # Lista de operadores suportados
│   └── escape_sequences.txt   # Sequências de escape válidas
└── tests/
    ├── unit/
    │   ├── test_lexer.cpp     # Testes unitários do lexer
    │   ├── test_token.cpp     # Testes de tokens
    │   ├── test_errors.cpp    # Testes de tratamento de erros
    │   └── test_states.cpp    # Testes da máquina de estados
    ├── integration/
    │   ├── test_c_samples.cpp # Testes com código C real
    │   └── test_versions.cpp  # Testes de compatibilidade por versão
    └── data/
        ├── valid_samples/     # Arquivos C válidos para teste
        │   ├── basic.c
        │   ├── complex.c
        │   └── version_specific/
        └── invalid_samples/   # Arquivos com erros para teste
            ├── syntax_errors.c
            └── lexical_errors.c
```

### Descrição dos Arquivos Principais

#### Headers (include/)
- **lexer.hpp**: Interface principal com métodos públicos do analisador
- **token.hpp**: Enumerações de tipos de tokens e estrutura Token
- **error_handler.hpp**: Classes para detecção e relatório de erros
- **symbol_table.hpp**: Gerenciamento de identificadores e palavras-chave
- **lexer_config.hpp**: Configurações de versão e features do C
- **lexer_state.hpp**: Estados da máquina de reconhecimento
- **lookahead_buffer.hpp**: Buffer circular para análise preditiva
- **lexer_logger.hpp**: Sistema de logging e depuração

#### Implementações (src/)
- **lexer.cpp**: Lógica principal de tokenização e controle de fluxo
- **token.cpp**: Métodos de criação e manipulação de tokens
- **error_handler.cpp**: Formatação de mensagens e estratégias de recuperação
- **symbol_table.cpp**: Algoritmos de busca e inserção de símbolos
- **lexer_config.cpp**: Carregamento de configurações por versão
- **lexer_state.cpp**: Transições e lógica da máquina de estados
- **lookahead_buffer.cpp**: Operações de buffer e gerenciamento de memória
- **lexer_logger.cpp**: Formatação e saída de logs

#### Dados de Configuração (data/)
- **keywords/**: Arquivos de texto com palavras-chave por versão do C
- **operators.txt**: Lista completa de operadores reconhecidos
- **escape_sequences.txt**: Sequências de escape válidas em strings

#### Testes (tests/)
- **unit/**: Testes isolados de cada componente
- **integration/**: Testes de integração com código C real
- **data/**: Arquivos de exemplo para validação

### Flexibilidade e Evolução da Estrutura

#### Estrutura Base Estável
A estrutura apresentada foi projetada para ser **estável e completa** desde o início do desenvolvimento. Os componentes principais (lexer, token, error_handler, symbol_table) representam os pilares fundamentais de qualquer analisador léxico e não devem sofrer alterações estruturais significativas.

#### Pontos de Extensibilidade Planejados
**Arquivos que podem ser expandidos sem alteração estrutural:**
- **data/keywords/**: Novos arquivos para versões futuras do C
- **tests/data/**: Novos casos de teste conforme necessário
- **src/lexer_config.cpp**: Novas configurações sem alterar interface
- **include/lexer_state.hpp**: Novos estados para features específicas

#### Possíveis Ajustes Menores Durante Desenvolvimento
**Cenários que podem requerer pequenos ajustes:**
1. **Otimizações de Performance**: 
   - Adição de cache interno em `symbol_table.cpp`
   - Buffer de tamanho variável em `lookahead_buffer.cpp`

2. **Features Descobertas Durante Implementação**:
   - Novos tipos de erro em `error_handler.hpp`
   - Estados adicionais para casos edge em `lexer_state.hpp`

3. **Melhorias de Debugging**:
   - Níveis de log adicionais em `lexer_logger.hpp`
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

### Preparação para Integração Futura

A estrutura do analisador léxico foi projetada especificamente para **facilitar a integração** com os demais componentes do compilador (Parser, Semantic Analyzer, Code Generator). Esta seção detalha como a arquitetura atual suporta essa integração.

### Interfaces de Integração

#### 1. Interface com o Analisador Sintático (Parser)
**Pontos de Integração:**
- **Token Stream**: O parser consumirá tokens via `nextToken()` e `peekToken()`
- **Posição e Contexto**: Cada token carrega informações de linha/coluna para relatórios de erro
- **Recuperação de Erros**: Estratégias coordenadas entre lexer e parser
- **Lookahead**: Buffer permite análise preditiva necessária para parsing

**Estruturas Compartilhadas:**
```
Token {
    TokenType type;        // Usado pelo parser para decisões sintáticas
    string lexeme;         // Texto original para mensagens de erro
    Position position;     // Linha/coluna para relatórios precisos
    Value value;          // Valor processado (números, strings)
}
```

#### 2. Interface com o Analisador Semântico
**Pontos de Integração:**
- **Symbol Table**: Base para tabela de símbolos semântica
- **Type Information**: Tokens de tipo serão expandidos para análise semântica
- **Scope Management**: Estrutura preparada para hierarquia de escopos
- **Error Context**: Informações de posição para análise semântica

**Dados Compartilhados:**
- Identificadores coletados durante análise léxica
- Literais com tipos inferidos (int, float, char, string)
- Palavras-chave de tipo para construção de AST

#### 3. Interface com o Gerador de Código
**Pontos de Integração:**
- **Literal Values**: Valores processados prontos para geração
- **Symbol References**: Mapeamento de identificadores para endereços
- **Type Annotations**: Informações de tipo preservadas desde o lexer
- **Source Mapping**: Posições para debug information

### Arquitetura de Comunicação

#### Padrão Pipeline
**Fluxo de Dados:**
```
Source Code → Lexer → Token Stream → Parser → AST → Semantic → Annotated AST → CodeGen → Target Code
```

**Características do Design:**
- **Unidirecional**: Fluxo claro de dados entre componentes
- **Stateless**: Cada componente pode ser testado independentemente
- **Error Propagation**: Erros são coletados e reportados de forma coordenada
- **Modular**: Componentes podem ser substituídos sem afetar outros

#### Sistema de Erros Integrado
**Coordenação entre Componentes:**
- **Error Handler Compartilhado**: Mesmo sistema para todos os componentes
- **Context Preservation**: Informações de contexto passadas entre fases
- **Recovery Strategies**: Estratégias coordenadas de recuperação
- **Unified Reporting**: Relatórios consolidados de todos os componentes

### Estruturas de Dados Compartilhadas

#### Position e Source Location
**Usado por todos os componentes:**
```cpp
struct Position {
    int line;
    int column;
    string filename;
    int offset;  // Para source mapping
};
```

#### Symbol Information
**Base para expansão semântica:**
```cpp
struct SymbolInfo {
    string name;
    TokenType category;
    Position first_occurrence;
    vector<Position> all_occurrences;
    // Será expandido pelo semantic analyzer
};
```

#### Error Information
**Estrutura unificada de erros:**
```cpp
struct CompilerError {
    ErrorType type;        // Lexical, Syntax, Semantic, CodeGen
    Position position;
    string message;
    string suggestion;
    ErrorSeverity severity;
};
```

### Pontos de Extensão para Integração

#### 1. Token Enhancement
**Preparado para expansão:**
- Atributos semânticos podem ser adicionados ao Token
- Type information pode ser anexada durante parsing
- Symbol table references podem ser incluídas

#### 2. Error Context Enhancement
**Preparado para contexto rico:**
- Stack trace de parsing pode ser adicionado
- Semantic context pode ser incluído
- Code generation context pode ser preservado

#### 3. Symbol Table Evolution
**Base para tabela semântica:**
- Estrutura atual será base para tabela de símbolos completa
- Scoping será adicionado pelo semantic analyzer
- Type information será expandida

### Validação da Arquitetura de Integração

#### Testes de Integração Planejados
**Cenários de Teste:**
1. **Lexer → Parser**: Verificar consumo correto de tokens
2. **Parser → Semantic**: Validar preservação de informações
3. **Semantic → CodeGen**: Confirmar dados necessários
4. **End-to-End**: Compilação completa de programas C simples

#### Métricas de Qualidade da Integração
**Indicadores:**
- **Acoplamento**: Baixo acoplamento entre componentes
- **Coesão**: Alta coesão dentro de cada componente
- **Testabilidade**: Cada interface pode ser testada isoladamente
- **Performance**: Overhead mínimo na comunicação entre componentes

### Conclusão sobre Adequação para Integração

#### Pontos Fortes da Arquitetura Atual
**✅ Preparada para Integração:**
- Interfaces bem definidas e estáveis
- Estruturas de dados extensíveis
- Sistema de erros unificado
- Padrão pipeline claro
- Separação de responsabilidades

#### Garantias de Compatibilidade
**✅ Compatibilidade Futura:**
- Headers estáveis não quebrarão integração
- Estruturas de dados são extensíveis
- Sistema de configuração suporta diferentes componentes
- Logging integrado facilita debugging conjunto

**A estrutura do analisador léxico está completamente preparada para integração com os demais componentes do compilador, seguindo padrões estabelecidos e melhores práticas de arquitetura de software.**

## Arquitetura Modular

### 1. Componentes Principais

#### Classe Lexer (lexer.hpp/cpp)
**Responsabilidades:**
- Leitura e processamento do arquivo fonte
- Reconhecimento e classificação de tokens
- Controle de estado da análise léxica
- Integração com componentes de suporte

**Características Técnicas:**
- Buffer de lookahead (4 caracteres)
- Máquina de estados para reconhecimento
- Suporte a múltiplas versões do padrão C
- Sistema de configuração flexível
- Rastreamento de posição (linha/coluna)

**Interface Pública:**
- `nextToken()` - Obter próximo token
- `peekToken()` - Visualizar token sem consumir
- `tokenizeAll()` - Tokenizar arquivo completo
- `hasMoreTokens()` - Verificar disponibilidade
- `reset()` - Reinicializar estado

### 2. Módulos de Suporte

#### Sistema de Tokens (tokens.h)
**Responsabilidades:**
- Definição de tipos de tokens suportados
- Estrutura de dados para representação de tokens
- Classificação de palavras-chave por versão do C
- Utilitários para manipulação de tokens

**Categorias de Tokens:**
- **Literais**: Inteiros, floats, caracteres, strings
- **Identificadores**: Nomes de variáveis, funções, tipos
- **Palavras-chave**: Organizadas por versão (C89/90, C99, C11, C17, C23)
- **Operadores**: Aritméticos, relacionais, lógicos, bitwise, atribuição
- **Delimitadores**: Parênteses, chaves, colchetes, pontuação
- **Pré-processamento**: Diretivas do preprocessador
- **Especiais**: Fim de arquivo, tokens inválidos

**Estrutura Token:**
- Tipo do token (TokenType)
- Lexema (texto original)
- Posição (linha e coluna)
- Valor processado (para literais)
- Métodos utilitários de classificação

#### Sistema de Tratamento de Erros (error_handler.h)
**Responsabilidades:**
- Detecção e classificação de erros léxicos
- Relatório detalhado de erros com contexto
- Estratégias de recuperação de erros
- Formatação de mensagens para o usuário

**Tipos de Erros Léxicos:**
- Caracteres inválidos
- Strings/caracteres não terminados
- Comentários não fechados
- Formatos numéricos inválidos
- Sequências de escape inválidas
- Identificadores muito longos
- Diretivas de pré-processamento inválidas

**Funcionalidades:**
- Coleta de erros e warnings
- Formatação com contexto e sugestões
- Controle de limite máximo de erros
- Modo de recuperação configurável
- Relatórios detalhados e resumos

#### Tabela de Símbolos (symbol_table.h)
**Responsabilidades:**
- Registro de identificadores encontrados
- Classificação entre identificadores e palavras-chave
- Rastreamento de ocorrências
- Suporte a diferentes versões do padrão C

**Funcionalidades:**
- Inserção e busca de símbolos
- Rastreamento de primeira ocorrência
- Lista de todas as ocorrências
- Classificação automática de tokens
- Estatísticas de uso
- Integração com tabela de palavras-chave

## Estruturas de Dados Principais

### Máquina de Estados
**Conceito:** Sistema de estados finitos para reconhecimento de padrões léxicos

**Estados Principais:**
- **Básicos**: START, WHITESPACE
- **Identificadores**: IDENTIFIER_START, IDENTIFIER_CONTINUE
- **Números**: NUMBER_START, INTEGER_PART, DECIMAL_POINT, FRACTIONAL_PART, EXPONENT_SIGN, EXPONENT_DIGITS, NUMBER_SUFFIX
- **Strings**: STRING_START, STRING_CONTENT, STRING_ESCAPE, STRING_END
- **Caracteres**: CHAR_START, CHAR_CONTENT, CHAR_ESCAPE, CHAR_END
- **Comentários**: COMMENT_START, LINE_COMMENT, BLOCK_COMMENT, BLOCK_COMMENT_END
- **Operadores**: OPERATOR_SINGLE, OPERATOR_DOUBLE, OPERATOR_TRIPLE
- **Pré-processamento**: PREPROCESSOR_START, PREPROCESSOR_DIRECTIVE, PREPROCESSOR_ARGS
- **Erro**: ERROR_STATE, RECOVERY_STATE

### Buffer de Lookahead
**Conceito:** Buffer circular para análise preditiva de caracteres

**Características:**
- Tamanho configurável (padrão: 8 caracteres)
- Operações: peek(), consume(), putback()
- Preenchimento automático do buffer
- Suporte a múltiplos caracteres de antecipação

## Fluxo de Processamento Detalhado

### 1. Inicialização
```
1. Abrir arquivo fonte
2. Configurar padrão C (C89, C90, C11, etc.)
3. Inicializar tabela de palavras-chave
4. Configurar tratamento de erros
5. Preparar buffer de lookahead
```

### 2. Loop Principal de Tokenização
```
ENQUANTO não fim do arquivo:
    1. Ler próximo caractere
    2. Determinar estado inicial baseado no caractere
    3. Executar máquina de estados apropriada
    4. Construir token resultante
    5. Atualizar posição (linha/coluna)
    6. Tratar erros se necessário
    7. Retornar token ou continuar
FIM ENQUANTO
```

### 3. Reconhecimento de Padrões

#### Identificadores
```
Padrão: [a-zA-Z_][a-zA-Z0-9_]*

Algoritmo:
1. Verificar primeiro caractere (letra ou _)
2. Consumir caracteres alfanuméricos
3. Verificar se é palavra-chave
4. Criar token IDENTIFIER ou palavra-chave
```

#### Números
```
Padrões suportados:
- Inteiros: 123, 0x1A, 077, 0b1010 (C23)
- Ponto flutuante: 3.14, 1.5e-10, 0x1.2p3
- Sufixos: L, LL, U, F, etc.

Algoritmo:
1. Detectar base (decimal, octal, hex, binário)
2. Consumir dígitos válidos para a base
3. Processar ponto decimal se presente
4. Processar expoente se presente
5. Processar sufixos
6. Validar formato final
```

#### Strings
```
Padrão: "..."
Escape sequences: \n, \t, \\, \", \xHH, \ooo

Algoritmo:
1. Consumir caractere de abertura
2. Processar conteúdo até fechamento
3. Tratar sequências de escape
4. Validar fechamento
5. Construir valor da string
```

## Suporte a Versões do C

### Configuração por Versão
**Conceito:** Sistema de features flags para habilitar funcionalidades específicas de cada versão

**Versões Suportadas:**
- **C89/C90**: Padrão base com palavras-chave fundamentais
- **C99**: Adiciona inline, restrict, bool, complex, VLAs, designated initializers
- **C11**: Adiciona static_assert, generic selections, atomic types, thread_local, alignment
- **C17**: Mantém compatibilidade com C11 (correções técnicas)
- **C23**: Adiciona typeof, binary literals, decimal floating point, BitInt types

**Sistema de Features:**
- Estrutura VersionFeatures com flags booleanas
- Configuração automática baseada no padrão selecionado
- Validação de tokens específicos por versão
- Mensagens de erro contextualizadas por versão

## Tratamento de Erros e Recuperação

### Estratégias de Recuperação
**Objetivo:** Continuar a análise após encontrar erros, maximizando a detecção de problemas

**Técnicas Implementadas:**
1. **Caractere Inválido**: Pular caractere e continuar análise
2. **String não terminada**: Inserir aspas implícitas no final da linha
3. **Comentário não fechado**: Inserir */ implícito no final do arquivo
4. **Número mal formado**: Usar parte válida e reportar erro na porção inválida
5. **Identificador muito longo**: Truncar e continuar
6. **Escape sequence inválida**: Usar caractere literal e reportar erro

### Sistema de Mensagens
**Características:**
- Mensagens contextualizadas com posição exata
- Sugestões de correção quando possível
- Formatação consistente e legível
- Classificação por severidade (erro/warning)
- Limite configurável de erros máximos

## Sistema de Logging
**Objetivo:** Facilitar depuração e monitoramento do processo de análise léxica

**Níveis de Log:**
- **DEBUG**: Transições de estado, caracteres processados
- **INFO**: Tokens reconhecidos, marcos do processamento
- **WARNING**: Situações suspeitas mas não críticas
- **ERROR**: Erros léxicos detectados

**Funcionalidades:**
- Timestamps opcionais
- Filtragem por nível
- Log de tokens gerados
- Log de transições de estado
- Saída configurável (console, arquivo)
- Formatação estruturada

## Diagramas de Estados

### Reconhecimento de Números
**Fluxo de Estados:**
- **START** → **INTEGER** (dígito inicial)
- **INTEGER** → **INTEGER** (dígitos adicionais)
- **INTEGER** → **DECIMAL_POINT** (ponto decimal)
- **INTEGER** → **EXPONENT_SIGN** (notação científica e/E)
- **INTEGER** → **NUMBER_SUFFIX** (sufixos L, U, F, etc.)
- **DECIMAL_POINT** → **FRACTIONAL** (dígitos fracionários)
- **FRACTIONAL** → **EXPONENT_SIGN** (notação científica)
- **EXPONENT_SIGN** → **EXPONENT_DIGITS** (+/- ou dígito)
- **EXPONENT_DIGITS** → **NUMBER_SUFFIX** (sufixos opcionais)

### Reconhecimento de Strings
**Fluxo de Estados:**
- **START** → **STRING_CONTENT** (aspas de abertura)
- **STRING_CONTENT** → **STRING_CONTENT** (caracteres normais)
- **STRING_CONTENT** → **ESCAPE_SEQUENCE** (barra invertida)
- **STRING_CONTENT** → **END** (aspas de fechamento)
- **STRING_CONTENT** → **ERROR** (quebra de linha)
- **ESCAPE_SEQUENCE** → **STRING_CONTENT** (sequência válida)
- **ESCAPE_SEQUENCE** → **HEX_ESCAPE** (\x)
- **ESCAPE_SEQUENCE** → **OCTAL_ESCAPE** (\0-7)
- **ESCAPE_SEQUENCE** → **ERROR** (sequência inválida)

## Casos de Uso e Exemplos

### Tokenização de Código C Básico
**Entrada:** Função main simples com printf
**Processo:**
1. Reconhecimento de palavras-chave (int, void, return)
2. Identificação de nomes (main, printf)
3. Processamento de delimitadores (parênteses, chaves)
4. Análise de literais (string, número)
5. Geração de sequência de tokens com posições

**Resultado:** Lista ordenada de tokens com tipos, lexemas e posições

### Tratamento de Erros Comuns
**Cenários de Erro:**
1. **Números mal formados**: Dígitos seguidos de letras (123abc)
2. **Caracteres inválidos**: Múltiplos caracteres em literal ('ab')
3. **Strings não terminadas**: Ausência de aspas de fechamento
4. **Comentários não fechados**: Bloco /* sem */
5. **Sequências de escape inválidas**: \z, \999, etc.

**Resposta do Sistema:**
- Detecção precisa da posição do erro
- Mensagem descritiva do problema
- Sugestão de correção quando aplicável
- Continuação da análise (recuperação)

## Estratégia de Testes

### Categorias de Teste

**Testes Funcionais:**
1. **Tokens Básicos**: Verificação de reconhecimento correto de identificadores, números, strings, operadores
2. **Palavras-chave**: Validação de todas as palavras-chave por versão do C (C89-C23)
3. **Comentários**: Processamento de comentários de linha e bloco, incluindo casos aninhados
4. **Literais Numéricos**: Inteiros, floats, diferentes bases (decimal, octal, hex, binário), sufixos
5. **Strings e Caracteres**: Sequências de escape, strings longas, caracteres especiais
6. **Pré-processamento**: Reconhecimento de diretivas básicas (#include, #define, etc.)

**Testes de Robustez:**
1. **Tratamento de Erros**: Validação de todos os tipos de erro léxico
2. **Recuperação**: Verificação da continuação correta após erros
3. **Casos Extremos**: Arquivos vazios, apenas espaços, caracteres Unicode
4. **Performance**: Arquivos grandes, tokens muito longos

**Framework de Testes:**
- Sistema de casos de teste parametrizados
- Comparação automática de tokens esperados vs. gerados
- Relatórios detalhados de cobertura
- Testes de regressão automatizados

## Considerações de Implementação

### Arquitetura Extensível
**Princípios de Design:**
- Separação clara de responsabilidades entre módulos
- Interfaces bem definidas para facilitar testes
- Configuração flexível para diferentes versões do C
- Sistema de plugins para extensões futuras

### Performance e Otimização
**Estratégias:**
- Buffer de lookahead otimizado para reduzir I/O
- Tabelas de hash para lookup rápido de palavras-chave
- Máquina de estados eficiente com transições diretas
- Minimização de alocações dinâmicas durante tokenização

### Manutenibilidade
**Aspectos Importantes:**
- Código bem documentado com exemplos
- Testes abrangentes para cada funcionalidade
- Logging detalhado para depuração
- Estrutura modular para facilitar modificações

## Especificação de Métodos por Arquivo

### Headers (include/)

#### lexer.hpp
**Classe Lexer:**
- `Lexer(const std::string& filename, ErrorHandler* eh)` - Construtor que inicializa o lexer com arquivo fonte e handler de erros
- `~Lexer()` - Destrutor que libera recursos e fecha arquivos
- `Token nextToken()` - Retorna o próximo token da sequência de entrada
- `Token peekToken(int offset = 1)` - Visualiza token futuro sem consumir da entrada
- `std::vector<Token> tokenizeAll()` - Tokeniza todo o arquivo e retorna lista completa
- `bool hasMoreTokens()` - Verifica se ainda há tokens disponíveis para leitura
- `void reset()` - Reinicializa o estado do lexer para o início do arquivo
- `Position getCurrentPosition()` - Retorna posição atual (linha/coluna) no arquivo
- `void setVersion(CVersion version)` - Configura versão do padrão C a ser utilizada

#### token.hpp
**Enum TokenType:**
- Definições de todos os tipos de tokens suportados

**Classe Token:**
- `Token(TokenType type, const std::string& lexeme, Position pos)` - Construtor básico do token
- `Token(TokenType type, const std::string& lexeme, Position pos, Value val)` - Construtor com valor processado
- `TokenType getType() const` - Retorna o tipo do token
- `std::string getLexeme() const` - Retorna o texto original do token
- `Position getPosition() const` - Retorna posição do token no código fonte
- `Value getValue() const` - Retorna valor processado (para literais)
- `bool isKeyword() const` - Verifica se o token é uma palavra-chave
- `bool isOperator() const` - Verifica se o token é um operador
- `bool isLiteral() const` - Verifica se o token é um literal
- `std::string toString() const` - Converte token para representação string

#### error_handler.hpp
**Classe ErrorHandler:**
- `ErrorHandler(int maxErrors = 100)` - Construtor com limite máximo de erros
- `void reportError(const std::string& message, Position pos)` - Reporta erro léxico com posição
- `void reportWarning(const std::string& message, Position pos)` - Reporta warning com contexto
- `bool hasErrors() const` - Verifica se há erros registrados
- `int getErrorCount() const` - Retorna número total de erros
- `int getWarningCount() const` - Retorna número total de warnings
- `std::vector<CompilerError> getErrors() const` - Retorna lista de todos os erros
- `void printErrorSummary() const` - Imprime resumo de erros e warnings
- `void setRecoveryMode(RecoveryMode mode)` - Configura estratégia de recuperação

#### symbol_table.hpp
**Classe SymbolTable:**
- `SymbolTable()` - Construtor que inicializa tabela vazia
- `void insert(const std::string& name, TokenType type, Position pos)` - Insere novo símbolo na tabela
- `SymbolInfo* lookup(const std::string& name)` - Busca símbolo por nome
- `bool isKeyword(const std::string& name, CVersion version)` - Verifica se é palavra-chave
- `TokenType getKeywordType(const std::string& name)` - Retorna tipo da palavra-chave
- `void addOccurrence(const std::string& name, Position pos)` - Adiciona nova ocorrência de símbolo
- `std::vector<SymbolInfo> getAllSymbols() const` - Retorna todos os símbolos registrados
- `void printStatistics() const` - Imprime estatísticas de uso dos símbolos

#### lexer_config.hpp
**Classe LexerConfig:**
- `LexerConfig(CVersion version = C89)` - Construtor com versão padrão do C
- `void setVersion(CVersion version)` - Define versão do padrão C
- `CVersion getVersion() const` - Retorna versão atual configurada
- `bool isFeatureEnabled(Feature feature) const` - Verifica se feature está habilitada
- `void loadKeywords(const std::string& filename)` - Carrega palavras-chave de arquivo
- `std::set<std::string> getKeywords() const` - Retorna conjunto de palavras-chave
- `void enableFeature(Feature feature)` - Habilita feature específica
- `void disableFeature(Feature feature)` - Desabilita feature específica

#### lexer_state.hpp
**Enum LexerState:**
- Definições de todos os estados da máquina de estados

**Classe StateMachine:**
- `StateMachine()` - Construtor que inicializa estado START
- `LexerState getCurrentState() const` - Retorna estado atual da máquina
- `void transition(char input)` - Executa transição baseada no caractere
- `bool isAcceptingState() const` - Verifica se estado atual é de aceitação
- `bool isErrorState() const` - Verifica se estado atual é de erro
- `void reset()` - Reinicializa máquina para estado START
- `TokenType getTokenType() const` - Retorna tipo de token do estado atual

#### lookahead_buffer.hpp
**Classe LookaheadBuffer:**
- `LookaheadBuffer(std::istream& input, int size = 8)` - Construtor com stream e tamanho do buffer
- `char peek(int offset = 0)` - Visualiza caractere sem consumir
- `char consume()` - Consome e retorna próximo caractere
- `void putback(char c)` - Retorna caractere para o buffer
- `bool hasMore() const` - Verifica se há mais caracteres disponíveis
- `void fillBuffer()` - Preenche buffer com novos caracteres
- `int getBufferSize() const` - Retorna tamanho atual do buffer

#### lexer_logger.hpp
**Classe LexerLogger:**
- `LexerLogger(LogLevel level = INFO)` - Construtor com nível de log padrão
- `void setLogLevel(LogLevel level)` - Define nível mínimo de log
- `void debug(const std::string& message)` - Log de nível DEBUG
- `void info(const std::string& message)` - Log de nível INFO
- `void warning(const std::string& message)` - Log de nível WARNING
- `void error(const std::string& message)` - Log de nível ERROR
- `void logToken(const Token& token)` - Log específico para tokens gerados
- `void logStateTransition(LexerState from, LexerState to)` - Log de transições de estado
- `void setOutputFile(const std::string& filename)` - Define arquivo de saída dos logs

### Implementações (src/)

#### lexer.cpp
**Métodos Privados Adicionais:**
- `char readNextChar()` - Lê próximo caractere do buffer com controle de posição
- `void skipWhitespace()` - Pula espaços em branco e atualiza posição
- `Token recognizeIdentifier()` - Reconhece identificadores e palavras-chave
- `Token recognizeNumber()` - Reconhece literais numéricos (int, float, hex, etc.)
- `Token recognizeString()` - Reconhece literais de string com escape sequences
- `Token recognizeCharacter()` - Reconhece literais de caractere
- `Token recognizeOperator()` - Reconhece operadores simples e compostos
- `Token recognizeComment()` - Processa comentários de linha e bloco
- `void handleError(const std::string& message)` - Trata erros léxicos e executa recuperação
- `bool isValidIdentifierStart(char c)` - Verifica se caractere pode iniciar identificador
- `bool isValidIdentifierChar(char c)` - Verifica se caractere pode continuar identificador

#### token.cpp
**Métodos Utilitários:**
- `std::string tokenTypeToString(TokenType type)` - Converte tipo de token para string
- `bool isKeywordToken(TokenType type)` - Verifica se tipo representa palavra-chave
- `bool isOperatorToken(TokenType type)` - Verifica se tipo representa operador
- `bool isLiteralToken(TokenType type)` - Verifica se tipo representa literal
- `int getOperatorPrecedence(TokenType type)` - Retorna precedência do operador
- `Associativity getOperatorAssociativity(TokenType type)` - Retorna associatividade do operador

#### error_handler.cpp
**Métodos de Formatação:**
- `std::string formatError(const CompilerError& error)` - Formata erro para exibição
- `std::string getErrorContext(Position pos, const std::string& source)` - Extrai contexto do erro
- `std::string generateSuggestion(const CompilerError& error)` - Gera sugestão de correção
- `void logError(const CompilerError& error)` - Registra erro no sistema de log

#### symbol_table.cpp
**Métodos de Hash e Busca:**
- `size_t hashFunction(const std::string& key)` - Função hash para símbolos
- `void rehash()` - Reorganiza tabela quando fator de carga é alto
- `SymbolInfo* findSlot(const std::string& name)` - Encontra slot para símbolo
- `void loadKeywordsFromFile(const std::string& filename)` - Carrega palavras-chave de arquivo

#### lexer_config.cpp
**Métodos de Configuração:**
- `void initializeFeatures(CVersion version)` - Inicializa features baseado na versão
- `void parseConfigFile(const std::string& filename)` - Lê configurações de arquivo
- `bool validateVersion(CVersion version)` - Valida se versão é suportada

#### lexer_state.cpp
**Métodos de Transição:**
- `LexerState getNextState(LexerState current, char input)` - Calcula próximo estado
- `bool isValidTransition(LexerState from, LexerState to)` - Valida transição de estado
- `void buildTransitionTable()` - Constrói tabela de transições

#### lookahead_buffer.cpp
**Métodos de Buffer:**
- `void shiftBuffer()` - Move conteúdo do buffer para a esquerda
- `bool needsRefill()` - Verifica se buffer precisa ser recarregado
- `void expandBuffer(int newSize)` - Expande tamanho do buffer dinamicamente

#### lexer_logger.cpp
**Métodos de Formatação:**
- `std::string formatLogMessage(LogLevel level, const std::string& msg)` - Formata mensagem de log
- `std::string getCurrentTimestamp()` - Gera timestamp para logs
- `void writeToFile(const std::string& message)` - Escreve log em arquivo
- `void writeToConsole(const std::string& message)` - Escreve log no console

### Arquivos de Dados (data/)

#### keywords/c89_keywords.txt
**Conteúdo Esperado:**
- `loadC89Keywords()` - Carrega palavras-chave básicas do C89/C90
- `validateC89Syntax(const std::string& keyword)` - Valida sintaxe específica do C89
- Lista de palavras-chave: `auto`, `break`, `case`, `char`, `const`, `continue`, `default`, `do`, `double`, `else`, `enum`, `extern`, `float`, `for`, `goto`, `if`, `int`, `long`, `register`, `return`, `short`, `signed`, `sizeof`, `static`, `struct`, `switch`, `typedef`, `union`, `unsigned`, `void`, `volatile`, `while`

#### keywords/c99_keywords.txt
**Conteúdo Esperado:**
- `loadC99Keywords()` - Carrega palavras-chave adicionais do C99
- `validateC99Features(const std::string& keyword)` - Valida recursos específicos do C99
- Palavras-chave adicionais: `inline`, `restrict`, `_Bool`, `_Complex`, `_Imaginary`

#### keywords/c11_keywords.txt
**Conteúdo Esperado:**
- `loadC11Keywords()` - Carrega palavras-chave adicionais do C11
- `validateC11Features(const std::string& keyword)` - Valida recursos específicos do C11
- Palavras-chave adicionais: `_Alignas`, `_Alignof`, `_Atomic`, `_Static_assert`, `_Noreturn`, `_Thread_local`, `_Generic`

#### keywords/c17_keywords.txt
**Conteúdo Esperado:**
- `loadC17Keywords()` - Carrega palavras-chave do C17 (compatibilidade com C11)
- `validateC17Compatibility()` - Valida compatibilidade com padrão C17
- Mantém compatibilidade total com C11

#### keywords/c23_keywords.txt
**Conteúdo Esperado:**
- `loadC23Keywords()` - Carrega palavras-chave adicionais do C23
- `validateC23Features(const std::string& keyword)` - Valida recursos específicos do C23
- Palavras-chave adicionais: `typeof`, `typeof_unqual`, `_BitInt`, `_Decimal128`, `_Decimal32`, `_Decimal64`

#### operators.txt
**Conteúdo Esperado:**
- `loadOperators()` - Carrega lista completa de operadores
- `getOperatorInfo(const std::string& op)` - Retorna informações do operador
- `validateOperatorSequence(const std::string& sequence)` - Valida sequência de operadores
- Lista de operadores: `+`, `-`, `*`, `/`, `%`, `++`, `--`, `==`, `!=`, `<`, `>`, `<=`, `>=`, `&&`, `||`, `!`, `&`, `|`, `^`, `~`, `<<`, `>>`, `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=`, `?`, `:`, `,`, `.`, `->`, `[]`, `()`, `{}`

#### escape_sequences.txt
**Conteúdo Esperado:**
- `loadEscapeSequences()` - Carrega sequências de escape válidas
- `validateEscapeSequence(const std::string& seq)` - Valida sequência de escape
- `processEscapeSequence(const std::string& seq)` - Processa e converte sequência
- Sequências suportadas: `\n`, `\t`, `\r`, `\b`, `\f`, `\a`, `\v`, `\\`, `\'`, `\"`, `\?`, `\0`, `\xHH`, `\ooo`, `\uHHHH`, `\UHHHHHHHH`

### Arquivos de Teste (tests/)

#### tests/unit/test_lexer.cpp
**Métodos de Teste:**
- `testBasicTokenization()` - Testa tokenização básica de identificadores e números
- `testKeywordRecognition()` - Testa reconhecimento de palavras-chave por versão
- `testOperatorRecognition()` - Testa reconhecimento de operadores simples e compostos
- `testStringLiterals()` - Testa processamento de strings com escape sequences
- `testNumericLiterals()` - Testa literais numéricos em diferentes bases
- `testCommentHandling()` - Testa processamento de comentários de linha e bloco
- `testErrorRecovery()` - Testa recuperação após erros léxicos
- `testPositionTracking()` - Testa rastreamento correto de posição no código

#### tests/unit/test_token.cpp
**Métodos de Teste:**
- `testTokenCreation()` - Testa criação de tokens com diferentes tipos
- `testTokenComparison()` - Testa comparação entre tokens
- `testTokenSerialization()` - Testa conversão de tokens para string
- `testTokenValue()` - Testa extração de valores de literais
- `testTokenPosition()` - Testa informações de posição dos tokens

#### tests/unit/test_errors.cpp
**Métodos de Teste:**
- `testErrorReporting()` - Testa relatório de erros léxicos
- `testErrorRecovery()` - Testa estratégias de recuperação
- `testErrorLimits()` - Testa limite máximo de erros
- `testWarningGeneration()` - Testa geração de warnings
- `testErrorFormatting()` - Testa formatação de mensagens de erro

#### tests/unit/test_states.cpp
**Métodos de Teste:**
- `testStateTransitions()` - Testa transições da máquina de estados
- `testAcceptingStates()` - Testa estados de aceitação
- `testErrorStates()` - Testa estados de erro
- `testStateReset()` - Testa reinicialização da máquina

#### tests/integration/test_c_samples.cpp
**Métodos de Teste:**
- `testBasicCProgram()` - Testa programa C básico completo
- `testComplexExpressions()` - Testa expressões complexas
- `testFunctionDefinitions()` - Testa definições de funções
- `testStructDeclarations()` - Testa declarações de estruturas
- `testPreprocessorDirectives()` - Testa diretivas de pré-processamento

#### tests/integration/test_versions.cpp
**Métodos de Teste:**
- `testC89Compatibility()` - Testa compatibilidade com C89/C90
- `testC99Features()` - Testa recursos específicos do C99
- `testC11Features()` - Testa recursos específicos do C11
- `testC17Compatibility()` - Testa compatibilidade com C17
- `testC23Features()` - Testa recursos específicos do C23
- `testVersionSwitching()` - Testa mudança dinâmica de versões

## Próximos Passos

**Fase de Implementação:**
1. **Marco 1.1**: Estrutura básica das classes e interfaces
2. **Marco 1.2**: Reconhecimento de tokens fundamentais
3. **Marco 1.3**: Operadores e delimitadores completos
4. **Marco 1.4**: Sistema de comentários e pré-processamento
5. **Marco 1.5**: Tratamento robusto de erros e recuperação
6. **Marco 1.6**: Implementação dos arquivos de dados (keywords, operators, escape_sequences)
7. **Marco 1.7**: Testes unitários e de integração completos

**Integração:**
- Testes unitários para cada componente
- Testes de integração com casos reais
- Documentação de API para desenvolvedores
- Benchmarks de performance
- Validação com arquivos de dados de configuração

**Estrutura de Dados Necessária:**
- Criação da pasta `data/` com subpasta `keywords/`
- Implementação dos arquivos de texto com palavras-chave por versão
- Arquivos de configuração para operadores e sequências de escape
- Estrutura completa de testes unitários e de integração

---

## Ordem de Implementação e Dependências

Esta seção define a sequência otimizada de implementação dos componentes do analisador léxico, baseada na análise de dependências entre classes e métodos. A ordem proposta minimiza retrabalho e permite testes incrementais.

### Análise de Dependências

```
Dependências Principais:
Token ← ErrorHandler ← LexerConfig ← SymbolTable ← LexerState ← LookaheadBuffer ← LexerLogger ← Lexer

Dependências Secundárias:
- Lexer depende de todos os componentes
- LexerState depende de Token e ErrorHandler
- SymbolTable depende de Token e LexerConfig
- LookaheadBuffer é independente (apenas I/O)
- LexerLogger é independente (apenas logging)
```

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