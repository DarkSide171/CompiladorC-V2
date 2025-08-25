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
cmake ..

# Compilar projeto principal (inclui todas as bibliotecas e testes)
echo "🔧 Compilando projeto principal..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "✅ Compilação concluída!"
echo "📍 Arquivos gerados:"
echo "   📚 Biblioteca Preprocessor: build/preprocessor/libpreprocessor.a"
echo "   📚 Biblioteca Lexer: build/lexer/liblexer.a"
echo "   🧪 Testes: build/tests/"
echo ""
echo "💡 Para executar os testes do Preprocessor:"
echo "   ./build/tests/test_file_manager"
echo "   ./build/tests/test_config"
echo "   ./build/tests/test_directive"
echo "   ./build/tests/test_logger"
echo "   ./build/tests/test_macro_processor"
echo "   ./build/tests/test_state"
echo ""
echo "💡 Para executar os testes do Lexer:"
echo "   ./build/tests/test_lexer_config"
echo "   ./build/tests/test_symbol_table"
echo "   ./build/tests/test_lookahead_buffer"
echo "   ./build/tests/test_lexer_logger"
echo "   ./build/tests/test_states"
echo "   ./build/tests/test_lexer"
echo "   ./build/tests/test_token"
echo "   ./build/tests/test_errors"
echo "   ./build/tests/test_c_samples"