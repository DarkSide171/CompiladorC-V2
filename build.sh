#!/bin/bash

# Script de build para o CompiladorC
# Uso: ./build.sh [clean]

set -e  # Parar em caso de erro

echo "🔨 Compilando CompiladorC..."

# Se o argumento 'clean' for passado, limpar build
if [ "$1" = "clean" ]; then
    echo "🧹 Limpando arquivos de build..."
    rm -rf build/*
    rm -rf src/*/build
fi

# Criar estrutura de diretórios build
mkdir -p build/preprocessor build/tests build/lexer build/parser build/semantic

# Entrar no diretório build principal
cd build

# Configurar projeto principal com CMake
echo "⚙️  Configurando projeto principal..."
cmake -DBUILD_PARSER_TESTS=ON ..

# Compilar projeto principal (inclui todas as bibliotecas e testes)
echo "🔧 Compilando projeto principal..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Compilar testes específicos do parser
echo "🧪 Compilando testes do parser..."
make test_grammar_unit test_ast_unit test_error_recovery_unit test_parser_config_unit test_parser_state_unit test_parser_logger_unit

echo "✅ Compilação concluída!"
echo "📍 Arquivos gerados:"
echo "   📚 Biblioteca Preprocessor: build/preprocessor/libpreprocessor.a"
echo "   📚 Biblioteca Lexer: build/lexer/liblexer.a"
echo "   📚 Biblioteca Parser: build/src/parser/libparser.a"
echo "   🧪 Testes: build/tests/"
echo "   🧪 Testes Parser: build/src/parser/test_*_unit"
echo ""
echo "🚀 Para executar os testes do parser:"
echo "   cd build && ./src/parser/test_grammar_unit"
echo "   cd build && ./src/parser/test_ast_unit"
echo "   cd build && ctest -R 'GrammarUnitTests|ASTUnitTests'"
echo ""
