# CMakeLists.txt para testes do Lexer

# Função auxiliar para criar testes
function(create_lexer_test test_name source_file)
    add_executable(${test_name} ${source_file})
    
    # Linkar com a biblioteca do lexer
    target_link_libraries(${test_name} PRIVATE lexer)
    
    # Incluir diretórios necessários
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    )
    
    # Configurações de compilação
    target_compile_features(${test_name} PRIVATE cxx_std_17)
    
    # Adicionar como teste do CTest
    add_test(NAME ${test_name} COMMAND ${test_name})
    
    # Definir propriedades do teste
    set_tests_properties(${test_name} PROPERTIES
        TIMEOUT 30
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

# Criar testes unitários
create_lexer_test(test_lexer_config unit/test_lexer_config.cpp)

# Adicionar mais testes conforme necessário
# create_lexer_test(test_token unit/test_token.cpp)
# create_lexer_test(test_error_handler unit/test_error_handler.cpp)

# Definir um target personalizado para executar todos os testes do lexer
add_custom_target(test_lexer
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_lexer_config
    COMMENT "Executando todos os testes do lexer"
)