# CMakeLists.txt para o módulo Preprocessor

cmake_minimum_required(VERSION 3.16)
project(Preprocessor)

# Definir arquivos fonte do preprocessor
set(PREPROCESSOR_SOURCES
    src/preprocessor.cpp
    src/directive.cpp
    src/macro_processor.cpp
    src/file_manager.cpp
    src/conditional_processor.cpp
    src/expression_evaluator.cpp
    src/preprocessor_config.cpp
    src/preprocessor_state.cpp
    src/preprocessor_logger.cpp
    src/preprocessor_lexer_interface.cpp
)

# Definir arquivos de cabeçalho do preprocessor
set(PREPROCESSOR_HEADERS
    include/preprocessor.hpp
    include/directive.hpp
    include/macro_processor.hpp
    include/file_manager.hpp
    include/conditional_processor.hpp
    include/expression_evaluator.hpp
    include/preprocessor_config.hpp
    include/preprocessor_state.hpp
    include/preprocessor_logger.hpp
    include/preprocessor_lexer_interface.hpp
)

# Define diretório de saída para biblioteca
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../build/preprocessor)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../build/preprocessor)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../build/preprocessor)

# Criar biblioteca do preprocessor
add_library(preprocessor STATIC ${PREPROCESSOR_SOURCES} ${PREPROCESSOR_HEADERS})

# Definir diretórios de include para a biblioteca
target_include_directories(preprocessor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Configurações de compilação específicas do preprocessor
target_compile_features(preprocessor PRIVATE cxx_std_17)

# Propriedades da biblioteca
set_target_properties(preprocessor PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Adicionar flags de compilação específicas
target_compile_options(preprocessor PRIVATE
    $<$<CONFIG:Debug>:-g -O0 -Wall -Wextra -Wpedantic>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)
