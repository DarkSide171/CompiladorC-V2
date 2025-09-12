cmake_minimum_required(VERSION 3.16)
project(CompialdorC_v2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configurações de compilação
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Opções de compilação
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Incluir diretórios
include_directories(src/lexer/include)
include_directories(src/preprocessor/include)
include_directories(src/parser/include)

# Opções de build
option(BUILD_PARSER_TESTS "Build parser tests" OFF)

# Adicionar subdiretorios
add_subdirectory(src/lexer)
add_subdirectory(src/preprocessor)
add_subdirectory(src/parser)

# Criar executável principal
add_executable(CompiladorC src/main.cpp src/lexer_preprocessor_bridge.cpp)

# Linkar com as bibliotecas do lexer, preprocessor e parser
target_link_libraries(CompiladorC PRIVATE lexer preprocessor parser)

# Configurar diretórios de include para o executável
target_include_directories(CompiladorC PRIVATE
    src/lexer/include
    src/preprocessor/include
    src/parser/include
)

# Criar executável apenas para o pré-processador
add_executable(PreprocessorOnly src/main_preprocessor_only.cpp)

# Linkar apenas com a biblioteca do preprocessor
target_link_libraries(PreprocessorOnly PRIVATE preprocessor)

# Configurar diretórios de include para o executável do preprocessor
target_include_directories(PreprocessorOnly PRIVATE
    src/preprocessor/include
)

# Criar executável apenas para o parser
add_executable(ParserOnly src/main_parser_only.cpp)

# Linkar com as bibliotecas do lexer e parser
target_link_libraries(ParserOnly PRIVATE lexer parser)

# Configurar diretórios de include para o executável do parser
target_include_directories(ParserOnly PRIVATE
    src/lexer/include
    src/parser/include
)

# Habilitar testes
enable_testing()

# Adicionar testes
add_subdirectory(src/lexer/tests)
add_subdirectory(src/preprocessor/tests)
