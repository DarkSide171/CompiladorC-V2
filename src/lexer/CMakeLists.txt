# CMakeLists.txt para o módulo Lexer
cmake_minimum_required(VERSION 3.16)
project(Lexer)

# Definir arquivos fonte do lexer
set(LEXER_SOURCES
    src/lexer_config.cpp
    src/error_handler.cpp
    src/token.cpp
    src/symbol_table.cpp
    src/lookahead_buffer.cpp
    src/lexer_logger.cpp
    src/lexer_state.cpp
    src/lexer.cpp
)

# Definir arquivos de cabeçalho do lexer
set(LEXER_HEADERS
    include/lexer_config.hpp
    include/error_handler.hpp
    include/token.hpp
    include/symbol_table.hpp
    include/lookahead_buffer.hpp   
    include/lexer_logger.hpp
    include/lexer_state.hpp
    include/lexer.hpp
)

# Define diretório de saída para biblioteca
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../build/lexer)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../build/lexer)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../build/lexer)

# Criar biblioteca do lexer
add_library(lexer STATIC ${LEXER_SOURCES} ${LEXER_HEADERS})

# Definir diretórios de include para a biblioteca
target_include_directories(lexer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Configurações de compilação específicas do lexer
target_compile_features(lexer PRIVATE cxx_std_17)

# Propriedades da biblioteca
set_target_properties(lexer PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Adicionar flags de compilação específicas
target_compile_options(lexer PRIVATE
    $<$<CONFIG:Debug>:-g -O0 -Wall -Wextra -Wpedantic>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)