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

# Adicionar subdiretorios
add_subdirectory(src/lexer)

# Biblioteca principal será criada quando necessário
# add_library(CompialdorC_v2 STATIC library.cpp)

# Habilitar testes
enable_testing()

# Adicionar testes
add_subdirectory(src/lexer/tests)
