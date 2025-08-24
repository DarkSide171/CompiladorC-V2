# Módulo Lexer - Sistema de Build

## Estrutura do Projeto

```
src/lexer/
├── CMakeLists.txt          # Configuração da biblioteca lexer
├── include/                # Arquivos de cabeçalho
│   ├── lexer_config.hpp
│   ├── error_handler.hpp
│   └── token.hpp
├── src/                    # Implementações
│   ├── lexer_config.cpp
│   ├── error_handler.cpp
│   └── token.cpp
├── data/                   # Arquivos de dados (palavras-chave)
├── tests/                  # Testes
│   ├── CMakeLists.txt      # Configuração dos testes
│   └── unit/
│       └── test_lexer_config.cpp
└── README.md              # Este arquivo
```

## Como Compilar

### 1. Configuração Inicial

```bash
# No diretório raiz do projeto
mkdir -p build
cd build
cmake ..
```

### 2. Compilação

```bash
# Compilar tudo
make

# Ou compilar apenas a biblioteca do lexer
make lexer

# Ou compilar apenas os testes
make test_lexer_config
```

### 3. Executar Testes

```bash
# Executar todos os testes do projeto
ctest --output-on-failure

# Executar apenas os testes do lexer
make test_lexer

# Executar teste específico
./src/lexer/tests/test_lexer_config
```

## Targets Disponíveis

- `lexer` - Biblioteca estática do lexer
- `test_lexer_config` - Executável de teste da LexerConfig
- `test_lexer` - Target personalizado que executa todos os testes do lexer

## Configurações de Build

- **Debug**: `-g -O0 -Wall -Wextra -Wpedantic`
- **Release**: `-O3 -DNDEBUG`
- **Padrão C++**: C++17

## Adicionando Novos Testes

Para adicionar um novo teste, edite `tests/CMakeLists.txt` e adicione:

```cmake
create_lexer_test(nome_do_teste unit/arquivo_teste.cpp)
```

O teste será automaticamente incluído no target `test_lexer`.