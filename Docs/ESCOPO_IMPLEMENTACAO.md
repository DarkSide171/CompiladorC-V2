# Escopo de Implementação - Compilador C

Este documento define o escopo detalhado de implementação do compilador C, especificando quais funcionalidades da linguagem C serão suportadas em cada fase do desenvolvimento.

## Versão 1.0 - Compilador Básico

### Subconjunto da Linguagem C Suportado

#### Tipos de Dados
- [x] **Tipos Primitivos**:
  - `int` (32 bits)
  - `float` (32 bits)
  - `char` (8 bits)
  - `void` (para funções)

- [ ] **Arrays**:
  - Arrays unidimensionais de tipos primitivos
  - Declaração: `int arr[10];`
  - Acesso: `arr[index]`
  - Inicialização básica

#### Declarações
- [x] **Variáveis Globais**:
  ```c
  int global_var;
  float pi = 3.14;
  ```

- [x] **Variáveis Locais**:
  ```c
  int local_var;
  char c = 'a';
  ```

- [x] **Funções**:
  ```c
  int add(int a, int b) {
      return a + b;
  }
  
  void print_hello() {
      // implementação
  }
  ```

#### Expressões
- [x] **Aritméticas**:
  - Operadores: `+`, `-`, `*`, `/`, `%`
  - Precedência e associatividade corretas
  - Expressões parentizadas

- [x] **Relacionais**:
  - Operadores: `<`, `<=`, `>`, `>=`, `==`, `!=`
  - Retorno de valores booleanos (0 ou 1)

- [x] **Lógicas**:
  - Operadores: `&&`, `||`, `!`
  - Avaliação com curto-circuito

- [x] **Atribuição**:
  - Operador básico: `=`
  - Atribuições compostas: `+=`, `-=`, `*=`, `/=`

#### Estruturas de Controle
- [x] **Condicionais**:
  ```c
  if (condition) {
      // código
  }
  
  if (condition) {
      // código
  } else {
      // código alternativo
  }
  ```

- [x] **Loops**:
  ```c
  while (condition) {
      // código
  }
  
  for (init; condition; increment) {
      // código
  }
  ```

- [x] **Controle de Fluxo**:
  - `return` (com e sem valor)
  - `break` (em loops)
  - `continue` (em loops)

#### Funções
- [x] **Definição e Declaração**:
  - Funções com parâmetros
  - Funções sem parâmetros (`void`)
  - Tipos de retorno variados

- [x] **Chamadas de Função**:
  - Passagem de parâmetros por valor
  - Verificação de tipos de parâmetros
  - Verificação de número de parâmetros

#### Entrada e Saída (Básica)
- [ ] **Função printf** (simplificada):
  - Suporte a `%d`, `%f`, `%c`, `%s`
  - Apenas strings literais como formato

- [ ] **Função scanf** (simplificada):
  - Leitura de tipos básicos
  - Verificação básica de formato

## Funcionalidades NÃO Suportadas na V1.0

### Tipos de Dados Avançados
- Ponteiros (`*`)
- Estruturas (`struct`)
- Uniões (`union`)
- Enumerações (`enum`)
- Tipos definidos pelo usuário (`typedef`)
- Arrays multidimensionais
- Strings como tipo nativo

### Recursos Avançados
- Pré-processador (`#include`, `#define`, etc.)
- Modificadores de armazenamento (`static`, `extern`, `auto`, `register`)
- Qualificadores de tipo (`const`, `volatile`)
- Operadores avançados (`sizeof`, operadores bitwise)
- Recursão (limitação inicial)
- Alocação dinâmica de memória

### Estruturas de Controle Avançadas
- `switch/case`
- `do-while`
- `goto` e labels

## Especificações Técnicas

### Limitações da Implementação

#### Lexer
- **Tamanho máximo de identificadores**: 255 caracteres
- **Tamanho máximo de strings**: 1024 caracteres
- **Números inteiros**: -2³¹ a 2³¹-1
- **Números float**: Precisão simples IEEE 754
- **Comentários suportados**: `//` e `/* */`

#### Parser
- **Profundidade máxima de aninhamento**: 50 níveis
- **Número máximo de parâmetros por função**: 20
- **Tamanho máximo de arrays**: 10000 elementos

#### Semantic Analyzer
- **Número máximo de variáveis por escopo**: 1000
- **Profundidade máxima de escopo**: 20 níveis
- **Número máximo de funções**: 100

#### Code Generator
- **Número máximo de temporários**: 10000
- **Número máximo de labels**: 1000

### Mensagens de Erro

#### Erros Léxicos
- Caractere inválido
- String não terminada
- Número mal formado
- Comentário não fechado

#### Erros Sintáticos
- Token inesperado
- Expressão mal formada
- Declaração inválida
- Estrutura de controle incompleta

#### Erros Semânticos
- Variável não declarada
- Redeclaração de variável
- Incompatibilidade de tipos
- Função não declarada
- Número incorreto de parâmetros
- Tipo de retorno incompatível

## Casos de Teste

### Programas de Exemplo Suportados

#### 1. Hello World
```c
void main() {
    printf("Hello, World!\n");
}
```

#### 2. Calculadora Simples
```c
int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

void main() {
    int x = 10;
    int y = 5;
    int sum = add(x, y);
    int diff = subtract(x, y);
    
    printf("Sum: %d\n", sum);
    printf("Difference: %d\n", diff);
}
```

#### 3. Fatorial
```c
int factorial(int n) {
    int result = 1;
    int i;
    
    for (i = 1; i <= n; i++) {
        result = result * i;
    }
    
    return result;
}

void main() {
    int num = 5;
    int fact = factorial(num);
    printf("Factorial of %d is %d\n", num, fact);
}
```

#### 4. Busca em Array
```c
int search(int arr[], int size, int target) {
    int i;
    
    for (i = 0; i < size; i++) {
        if (arr[i] == target) {
            return i;
        }
    }
    
    return -1;
}

void main() {
    int numbers[5] = {10, 20, 30, 40, 50};
    int target = 30;
    int index = search(numbers, 5, target);
    
    if (index != -1) {
        printf("Found at index %d\n", index);
    } else {
        printf("Not found\n");
    }
}
```

## Critérios de Aceitação

### Para cada fase:

#### Lexer
- [ ] Reconhece todos os tokens definidos
- [ ] Reporta erros léxicos com localização
- [ ] Ignora comentários e espaços em branco
- [ ] Mantém informações de linha e coluna

#### Parser
- [ ] Constrói AST correta para programas válidos
- [ ] Detecta erros sintáticos
- [ ] Implementa recuperação de erros
- [ ] Suporta toda a gramática definida

#### Semantic Analyzer
- [ ] Verifica declarações e uso de variáveis
- [ ] Implementa verificação de tipos
- [ ] Gerencia escopos corretamente
- [ ] Valida chamadas de função

#### Code Generator
- [ ] Gera código intermediário correto
- [ ] Implementa todas as construções suportadas
- [ ] Otimizações básicas funcionam

#### Final Code Generator
- [ ] Produz assembly válido
- [ ] Código gerado executa corretamente
- [ ] Alocação de registradores funciona

## Roadmap de Expansão (Versões Futuras)

### Versão 1.1
- Suporte a ponteiros básicos
- Estruturas simples
- Pré-processador básico

### Versão 1.2
- Arrays multidimensionais
- Strings como tipo nativo
- Mais funções de biblioteca

### Versão 2.0
- Otimizações avançadas
- Suporte completo a ponteiros
- Alocação dinâmica de memória
- Múltiplas arquiteturas alvo

## Métricas de Qualidade

- **Cobertura de testes**: Mínimo 80%
- **Detecção de erros**: 95% dos erros comuns
- **Performance**: Compilação de 1000 linhas em < 1 segundo
- **Memória**: Uso máximo de 100MB para programas de até 10000 linhas