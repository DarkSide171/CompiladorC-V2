#!/bin/bash

# Script de build para o CompiladorC
# Uso: ./build.sh [clean]

set -e  # Parar em caso de erro

echo "🔨 Compilando CompiladorC..."

# Se o argumento 'clean' for passado, limpar build
if [ "$1" = "clean" ]; then
    echo "🧹 Limpando arquivos de build..."
    rm -rf build/*
fi

# Criar diretório build se não existir
mkdir -p build

# Entrar no diretório build
cd build

# Configurar com CMake
echo "⚙️  Configurando projeto..."
cmake ..

# Compilar
echo "🔧 Compilando..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "✅ Compilação concluída!"
echo "📍 Executável disponível em: build/CompiladorC"
echo ""
echo "💡 Exemplos de uso:"
echo "   ./build/CompiladorC --help"
echo "   ./build/CompiladorC --summary arquivo.c"
echo "   ./build/CompiladorC --json --filter-keywords arquivo.c"