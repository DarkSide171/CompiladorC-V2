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
create_lexer_test(test_symbol_table unit/test_symbol_table.cpp)
create_lexer_test(test_lookahead_buffer unit/test_lookahead_buffer.cpp)
create_lexer_test(test_lexer_logger unit/test_lexer_logger.cpp)
create_lexer_test(test_states unit/test_states.cpp)
create_lexer_test(test_lexer unit/test_lexer.cpp)
create_lexer_test(test_lexer_phase42 unit/test_lexer_phase42.cpp)
create_lexer_test(test_advanced_literals unit/test_advanced_literals.cpp)

# Adicionar mais testes conforme necessário
# create_lexer_test(test_token unit/test_token.cpp)
create_lexer_test(test_errors unit/test_errors.cpp)
create_lexer_test(test_error_recovery unit/test_error_recovery.cpp)

# Definir um target personalizado para executar todos os testes do lexer
add_custom_target(run_all_lexer_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_lexer_config test_symbol_table test_lookahead_buffer test_lexer_logger test_states test_lexer test_lexer_phase42 test_advanced_literals test_errors test_error_recovery
    COMMENT "Executando todos os testes do lexer"
)