# CMakeLists.txt para testes do preprocessor
# TODO: Configurar Google Test na Fase 3.3

cmake_minimum_required(VERSION 3.16)
project(PreprocessorTests)

# Define diretório de saída para executáveis de teste
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../build/tests)

# Testes unitários - Estrutura unificada após consolidação
# Testes consolidados principais
add_executable(test_debug_comprehensive unit/test_debug_comprehensive.cpp)
add_executable(test_expression_comprehensive unit/test_expression_comprehensive.cpp)
add_executable(test_macro_comprehensive unit/test_macro_comprehensive.cpp)
add_executable(test_performance_comprehensive unit/test_performance_comprehensive.cpp)
add_executable(test_directive_comprehensive unit/test_directive_comprehensive.cpp)
add_executable(test_utility_comprehensive unit/test_utility_comprehensive.cpp)
add_executable(test_system_comprehensive unit/test_system_comprehensive.cpp)
add_executable(test_optimization_comprehensive unit/test_optimization_comprehensive.cpp)

# Testes individuais mantidos
add_executable(test_preprocessor unit/test_preprocessor.cpp)
add_executable(test_directive_processor_phase4_1 unit/test_directive_processor_phase4_1.cpp)
add_executable(test_includes unit/test_includes.cpp)
add_executable(test_preprocessor_main unit/test_preprocessor_main.cpp)
add_executable(test_integration integration/test_integration.cpp)
add_executable(test_lexer_integration2 integration/test_lexer_integration.cpp)

# Configurar linkage e includes para todos os testes unitários
set(UNIT_TESTS 
    # Testes consolidados principais
    test_debug_comprehensive test_expression_comprehensive test_macro_comprehensive test_performance_comprehensive
    test_directive_comprehensive test_utility_comprehensive test_system_comprehensive test_optimization_comprehensive
    # Testes individuais mantidos
    test_preprocessor test_directive_processor_phase4_1 test_includes test_preprocessor_main
    test_integration test_lexer_integration2
)

foreach(TEST_TARGET ${UNIT_TESTS})
    target_link_libraries(${TEST_TARGET}
        preprocessor
    )
    
    target_include_directories(${TEST_TARGET} PRIVATE
        ../include
    )
endforeach()

# Testes de integração
add_executable(preprocessor_integration_tests integration/test_integration.cpp)
add_executable(test_lexer_integration integration/test_lexer_integration.cpp)

# Configurar linkage e includes para testes de integração
set(INTEGRATION_TESTS preprocessor_integration_tests test_lexer_integration)

foreach(TEST_TARGET ${INTEGRATION_TESTS})
    target_link_libraries(${TEST_TARGET} preprocessor)
    target_include_directories(${TEST_TARGET} PRIVATE ../include)
endforeach()

# Configuração do C++17 para todos os testes
set(ALL_TESTS ${UNIT_TESTS} ${INTEGRATION_TESTS})

foreach(TEST_TARGET ${ALL_TESTS})
    set_target_properties(${TEST_TARGET} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )
endforeach()