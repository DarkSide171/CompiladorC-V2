# CMakeLists.txt para testes de integração do preprocessor
cmake_minimum_required(VERSION 3.16)

# Configurações do projeto
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir diretórios necessários
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../lexer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Encontrar arquivos fonte necessários
file(GLOB LEXER_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../lexer/src/*.cpp"
)

file(GLOB PREPROCESSOR_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.cpp"
)

file(GLOB COMMON_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../common/src/*.cpp"
)

# Arquivo da ponte de integração (está no diretório raiz do src)
set(BRIDGE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../../../lexer_preprocessor_bridge.cpp")

# Verificar se o arquivo da ponte existe
if(NOT EXISTS ${BRIDGE_SOURCE})
    message(WARNING "Arquivo da ponte não encontrado: ${BRIDGE_SOURCE}")
endif()

# Configurações de compilação
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

# Configuração para Debug
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")

# Configuração para Release
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Teste de mapeamento de posições
add_executable(test_position_mapping
    test_position_mapping.cpp
    ${BRIDGE_SOURCE}
    ${LEXER_SOURCES}
    ${PREPROCESSOR_SOURCES}
    ${COMMON_SOURCES}
)

target_compile_definitions(test_position_mapping PRIVATE
    INTEGRATION_TESTS_ENABLED=1
)

# Teste de integração lexer-preprocessor
add_executable(test_lexer_preprocessor_integration
    test_lexer_preprocessor_integration.cpp
    ${BRIDGE_SOURCE}
    ${LEXER_SOURCES}
    ${PREPROCESSOR_SOURCES}
    ${COMMON_SOURCES}
)

target_compile_definitions(test_lexer_preprocessor_integration PRIVATE
    INTEGRATION_TESTS_ENABLED=1
)

# Teste end-to-end do pipeline
add_executable(test_end_to_end_pipeline
    test_end_to_end_pipeline.cpp
    ${BRIDGE_SOURCE}
    ${LEXER_SOURCES}
    ${PREPROCESSOR_SOURCES}
    ${COMMON_SOURCES}
)

target_compile_definitions(test_end_to_end_pipeline PRIVATE
    INTEGRATION_TESTS_ENABLED=1
)

# Teste de compatibilidade entre componentes (comentado até ser criado)
# add_executable(test_compatibility
#     test_compatibility.cpp
#     ${BRIDGE_SOURCE}
#     ${LEXER_SOURCES}
#     ${PREPROCESSOR_SOURCES}
#     ${COMMON_SOURCES}
# )
# 
# target_compile_definitions(test_compatibility PRIVATE
#     INTEGRATION_TESTS_ENABLED=1
# )

# Configurar propriedades dos executáveis
set_target_properties(
    test_position_mapping
    test_lexer_preprocessor_integration
    test_end_to_end_pipeline
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)

# Adicionar testes ao CTest se disponível
if(CMAKE_TESTING_ENABLED OR BUILD_TESTING)
    enable_testing()
    
    add_test(
        NAME PositionMappingTest
        COMMAND test_position_mapping
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    add_test(
        NAME LexerPreprocessorIntegrationTest
        COMMAND test_lexer_preprocessor_integration
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    add_test(
        NAME EndToEndPipelineTest
        COMMAND test_end_to_end_pipeline
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    # add_test(
    #     NAME CompatibilityTest
    #     COMMAND test_compatibility
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    # )
    
    # Configurar propriedades dos testes
    set_tests_properties(
        PositionMappingTest
        LexerPreprocessorIntegrationTest
        EndToEndPipelineTest
        PROPERTIES
        TIMEOUT 300  # 5 minutos de timeout
    )
endif()

# Target personalizado para executar todos os testes de integração
add_custom_target(run_integration_tests
    COMMAND echo "=== Executando Testes de Integração ==="
    COMMAND echo "1. Teste de Mapeamento de Posições:"
    COMMAND $<TARGET_FILE:test_position_mapping> || echo "Teste falhou"
    COMMAND echo ""
    COMMAND echo "2. Teste de Integração Lexer-Preprocessor:"
    COMMAND $<TARGET_FILE:test_lexer_preprocessor_integration> || echo "Teste falhou"
    COMMAND echo ""
    COMMAND echo "3. Teste End-to-End do Pipeline:"
    COMMAND $<TARGET_FILE:test_end_to_end_pipeline> || echo "Teste falhou"
    COMMAND echo ""
    # COMMAND echo "4. Teste de Compatibilidade:"
    # COMMAND $<TARGET_FILE:test_compatibility> || echo "Teste falhou"
    # COMMAND echo ""
    COMMAND echo "=== Testes de Integração Concluídos ==="
    DEPENDS 
        test_position_mapping
        test_lexer_preprocessor_integration
        test_end_to_end_pipeline
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Executando todos os testes de integração"
)

# Target para compilar todos os testes sem executar
add_custom_target(build_integration_tests
    DEPENDS 
        test_position_mapping
        test_lexer_preprocessor_integration
        test_end_to_end_pipeline
    COMMENT "Compilando todos os testes de integração"
)

# Target para limpeza específica dos testes
add_custom_target(clean_integration_tests
    COMMAND ${CMAKE_COMMAND} -E remove -f
        $<TARGET_FILE:test_position_mapping>
        $<TARGET_FILE:test_lexer_preprocessor_integration>
        $<TARGET_FILE:test_end_to_end_pipeline>
    COMMENT "Limpando executáveis dos testes de integração"
)

# Informações de debug
message(STATUS "Configuração dos Testes de Integração:")
message(STATUS "  - Diretório atual: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "  - Diretório de build: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "  - Arquivo da ponte: ${BRIDGE_SOURCE}")
message(STATUS "  - Fontes do lexer: ${LEXER_SOURCES}")
message(STATUS "  - Fontes do preprocessor: ${PREPROCESSOR_SOURCES}")
message(STATUS "  - Fontes comuns: ${COMMON_SOURCES}")

# Verificações de dependências
if(NOT LEXER_SOURCES)
    message(WARNING "Nenhum arquivo fonte do lexer encontrado")
endif()

if(NOT PREPROCESSOR_SOURCES)
    message(WARNING "Nenhum arquivo fonte do preprocessor encontrado")
endif()

# Configurações específicas do compilador
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(test_position_mapping PRIVATE -Wno-unused-parameter)
    target_compile_options(test_lexer_preprocessor_integration PRIVATE -Wno-unused-parameter)
    target_compile_options(test_end_to_end_pipeline PRIVATE -Wno-unused-parameter)
    # target_compile_options(test_compatibility PRIVATE -Wno-unused-parameter)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(test_position_mapping PRIVATE -Wno-unused-parameter)
    target_compile_options(test_lexer_preprocessor_integration PRIVATE -Wno-unused-parameter)
    target_compile_options(test_end_to_end_pipeline PRIVATE -Wno-unused-parameter)
    # target_compile_options(test_compatibility PRIVATE -Wno-unused-parameter)
endif()

# Configurações de linking se necessário
# target_link_libraries(test_position_mapping PRIVATE pthread)
# target_link_libraries(test_lexer_preprocessor_integration PRIVATE pthread)
# target_link_libraries(test_end_to_end_pipeline PRIVATE pthread)
# target_link_libraries(test_compatibility PRIVATE pthread)