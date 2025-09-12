# CMakeLists.txt for Parser Module
cmake_minimum_required(VERSION 3.16)

# Parser library (Phase 1.4 - Basic components only)
set(PARSER_SOURCES
    src/ast.cpp
    src/ast_printer.cpp
    src/parser_config.cpp
    src/parser_logger.cpp
    src/parser.cpp
    src/grammar.cpp
    src/error_recovery.cpp
    src/parser_state.cpp
    src/token_stream.cpp
    src/incremental_parser.cpp
    src/parser_profiler.cpp
)

set(PARSER_HEADERS
    include/parser.hpp
    include/ast.hpp
    include/ast_printer.hpp
    include/grammar.hpp
    include/error_recovery.hpp
    include/parser_config.hpp
    include/parser_state.hpp
    include/parser_logger.hpp
    include/parser_types.hpp
    include/token_stream.hpp
    include/incremental_parser.hpp
    include/parser_profiler.hpp
)

# Create parser library
add_library(parser STATIC ${PARSER_SOURCES} ${PARSER_HEADERS})

# Include directories
target_include_directories(parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../lexer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../preprocessor/include
)

# Compiler features
target_compile_features(parser PUBLIC cxx_std_17)

# Compiler flags
target_compile_options(parser PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

# Data files installation
install(DIRECTORY data/
    DESTINATION share/parser/data
    FILES_MATCHING
    PATTERN "*.grammar"
    PATTERN "*.txt"
)

# Headers installation
install(FILES ${PARSER_HEADERS}
    DESTINATION include/parser
)

# Library installation
install(TARGETS parser
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Tests (optional)
option(BUILD_PARSER_TESTS "Build parser tests" OFF)

if(BUILD_PARSER_TESTS)
    # Individual test executables for Phase 1.4
    
    # AST Node tests
    add_executable(test_ast_node
        tests/unit/test_ast_node.cpp
    )
    target_link_libraries(test_ast_node parser)
    
    # AST tests
    add_executable(test_ast
        tests/unit/test_ast.cpp
    )
    target_link_libraries(test_ast parser)
    
    # Parser Config tests
    add_executable(test_parser_config
        tests/test_parser_config.cpp
    )
    target_link_libraries(test_parser_config parser lexer)
    
    # Parser Logger tests
    add_executable(test_parser_logger
        tests/test_parser_logger.cpp
    )
    target_link_libraries(test_parser_logger parser)
    
    # Individual unit tests
    add_executable(test_parser_unit tests/unit/test_parser.cpp)
    target_link_libraries(test_parser_unit parser lexer)
    
    add_executable(test_ast_unit tests/unit/test_ast.cpp)
    target_link_libraries(test_ast_unit parser)
    
    add_executable(test_grammar_unit tests/unit/test_grammar.cpp)
    target_link_libraries(test_grammar_unit parser)
    
    add_executable(test_error_recovery_unit tests/unit/test_error_recovery.cpp)
    target_link_libraries(test_error_recovery_unit parser lexer)
    
    add_executable(test_parser_config_unit tests/unit/test_parser_config.cpp)
    target_link_libraries(test_parser_config_unit parser)
    
    add_executable(test_parser_state_unit tests/unit/test_parser_state.cpp)
    target_link_libraries(test_parser_state_unit parser lexer)
    
    add_executable(test_parser_logger_unit tests/unit/test_parser_logger.cpp)
    target_link_libraries(test_parser_logger_unit parser)
    
    # Phase 3.3 - Parser Declarations Tests
    add_executable(test_parser_declarations tests/unit/test_parser_declarations.cpp)
    target_link_libraries(test_parser_declarations parser lexer)
    
    # Phase 3.4 - Parser Statements Tests
    add_executable(test_parser_statements tests/unit/test_parser_statements.cpp)
    target_link_libraries(test_parser_statements parser lexer)
    
    # Phase 3.4 - Comprehensive Parser Tests
    add_executable(test_parser_comprehensive tests/unit/test_parser_comprehensive.cpp)
    target_link_libraries(test_parser_comprehensive parser lexer)
    
    # Phase 3.5 - Parser Expressions Tests
    add_executable(test_parser_expressions tests/unit/test_parser_expressions.cpp)
    target_link_libraries(test_parser_expressions parser lexer)
    
    # Phase 3.7 - Parser Types Tests
    add_executable(test_parser_types tests/unit/test_parser_types.cpp)
    target_link_libraries(test_parser_types parser lexer)
    
    # Phase 3.7 - Precedence and Associativity Tests
    add_executable(test_precedence_associativity tests/unit/test_precedence_associativity.cpp)
    target_link_libraries(test_precedence_associativity parser lexer)
    
    # Integration tests
    add_executable(test_parser_integration
        tests/integration/test_parser_integration.cpp
    )
    target_link_libraries(test_parser_integration parser lexer)
    
    # Enable testing
    enable_testing()
    
    # Phase 1.4 Basic Tests
    add_test(NAME ASTNodeTests COMMAND test_ast_node)
    add_test(NAME ASTTests COMMAND test_ast)
    add_test(NAME ParserConfigTests COMMAND test_parser_config)
    add_test(NAME ParserLoggerTests COMMAND test_parser_logger)
    
    # Unit tests
    add_test(NAME ParserUnitTests COMMAND test_parser_unit)
    add_test(NAME ASTUnitTests COMMAND test_ast_unit)
    add_test(NAME GrammarUnitTests COMMAND test_grammar_unit)
    add_test(NAME ErrorRecoveryUnitTests COMMAND test_error_recovery_unit)
    add_test(NAME ParserConfigUnitTests COMMAND test_parser_config_unit)
    add_test(NAME ParserStateUnitTests COMMAND test_parser_state_unit)
    add_test(NAME ParserLoggerUnitTests COMMAND test_parser_logger_unit)
    
    # Phase 3.3 - Parser Declarations Tests
    add_test(NAME ParserDeclarationsTests COMMAND test_parser_declarations)
    
    # Phase 3.4 - Parser Statements Tests
    add_test(NAME ParserStatementsTests COMMAND test_parser_statements)
    
    # Phase 3.4 - Comprehensive Parser Tests
    add_test(NAME ParserComprehensiveTests COMMAND test_parser_comprehensive)
    
    # Phase 3.5 - Parser Expressions Tests
    add_test(NAME ParserExpressionsTests COMMAND test_parser_expressions)
    
    # Phase 3.7 - Parser Types Tests
    add_test(NAME ParserTypesTests COMMAND test_parser_types)
    
    # Phase 3.7 - Precedence and Associativity Tests
    add_test(NAME PrecedenceAssociativityTests COMMAND test_precedence_associativity)
    
    # Integration tests
    add_test(NAME ParserIntegrationTests COMMAND test_parser_integration)
    
    # Custom target to run Phase 1.4 tests
    add_custom_target(run_phase_1_4_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R "ASTNodeTests|ASTTests|ParserConfigTests|ParserLoggerTests"
        DEPENDS test_ast_node test_ast test_parser_config test_parser_logger
        COMMENT "Running Phase 1.4 Basic Tests"
    )
endif()

# Documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    
    if(EXISTS ${DOXYGEN_IN})
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(parser_docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating parser documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()