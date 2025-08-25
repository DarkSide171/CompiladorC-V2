# CMakeLists.txt para testes do preprocessor
# TODO: Configurar Google Test na Fase 3.3

cmake_minimum_required(VERSION 3.16)
project(PreprocessorTests)

# Define diretório de saída para executáveis de teste
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../build/tests)

# Testes unitários
add_executable(test_file_manager
    unit/test_file_manager.cpp
)

add_executable(test_config
    unit/test_config.cpp
)

add_executable(test_directive
    unit/test_directive.cpp
)

add_executable(test_logger
    unit/test_logger.cpp
)

add_executable(test_macro_processor
    unit/test_macro_processor.cpp
)

add_executable(test_state
    unit/test_state.cpp
)

add_executable(test_preprocessor
    unit/test_preprocessor.cpp
)

add_executable(test_expression_evaluator
    unit/test_expression_evaluator.cpp
)

# Configurar linkage e includes para todos os testes unitários
set(UNIT_TESTS test_file_manager test_config test_directive test_logger test_macro_processor test_state test_preprocessor test_expression_evaluator)

foreach(TEST_TARGET ${UNIT_TESTS})
    target_link_libraries(${TEST_TARGET}
        preprocessor
    )
    
    target_include_directories(${TEST_TARGET} PRIVATE
        ../include
    )
endforeach()

# Testes de integração
add_executable(preprocessor_integration_tests
    integration/test_integration.cpp
)

target_link_libraries(preprocessor_integration_tests
    preprocessor
)

target_include_directories(preprocessor_integration_tests PRIVATE
    ../include
)

# Configuração do C++17 para todos os testes
foreach(TEST_TARGET ${UNIT_TESTS})
    set_target_properties(${TEST_TARGET} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )
endforeach()

set_target_properties(preprocessor_integration_tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)