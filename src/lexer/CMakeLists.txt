# CMakeLists.txt para o módulo Lexer

# Definir arquivos fonte do lexer
set(LEXER_SOURCES
    src/lexer_config.cpp
    src/error_handler.cpp
    src/token.cpp
)

# Definir arquivos de cabeçalho do lexer
set(LEXER_HEADERS
    include/lexer_config.hpp
    include/error_handler.hpp
    include/token.hpp
)

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