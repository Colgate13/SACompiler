#!/bin/bash

echo "=== Detectando compilador disponível ==="

# Detecta qual compilador está disponível
if command -v clang &> /dev/null; then
    COMPILER="clang"
    echo "✓ Clang encontrado!"
elif command -v gcc &> /dev/null; then
    COMPILER="gcc"
    echo "✓ GCC encontrado!"
else
    echo "✗ Erro: Nem GCC nem Clang estão instalados!"
    echo "Instale um deles:"
    echo "  sudo apt-get install gcc"
    echo "  ou"
    echo "  sudo apt-get install clang"
    exit 1
fi

echo "Compilador selecionado: $COMPILER"
echo ""

echo "=== PASSO 1: Montando o assembly com NASM ==="
nasm -f elf64 programa.asm -o programa.o
if [ $? -eq 0 ]; then
    echo "✓ Montagem bem-sucedida! Gerado: programa.o"
else
    echo "✗ Erro na montagem"
    exit 1
fi

echo ""
echo "=== PASSO 2: Linkando com $COMPILER (usa libc para printf) ==="
$COMPILER programa.o -o programa -no-pie
if [ $? -eq 0 ]; then
    echo "✓ Linkagem bem-sucedida! Gerado: programa (executável)"
else
    echo "✗ Erro na linkagem"
    exit 1
fi

echo ""
echo "=== PASSO 3: Executando o programa ==="
echo "Saída esperada:"
echo "8    (5 + 3)"
echo "16   ((5 + 3) * 2)"
echo "17   (((5 + 3) * 2) + 1)"
echo "2    (10 - (5 + 3))"
echo "17   ((5 * 3) + (5 - 3))"
echo ""
echo "Saída real:"
./programa

echo ""
echo "=== Sucesso com $COMPILER! ==="
