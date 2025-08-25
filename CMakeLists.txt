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

# Adicionar subdiretorios
add_subdirectory(src/lexer)
add_subdirectory(src/preprocessor)

# Criar executável principal
add_executable(CompiladorC src/main.cpp)

# Linkar com a biblioteca do lexer
target_link_libraries(CompiladorC PRIVATE lexer)

# Configurar diretórios de include para o executável
target_include_directories(CompiladorC PRIVATE
    src/lexer/include
)

# Habilitar testes
enable_testing()

# Adicionar testes
add_subdirectory(src/lexer/tests)
add_subdirectory(src/preprocessor/tests)
