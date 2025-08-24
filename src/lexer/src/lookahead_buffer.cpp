// Lookahead Buffer - Implementação do buffer de lookahead
// Implementação da classe LookaheadBuffer para gerenciamento de buffer

#include "../include/lookahead_buffer.hpp"
#include <algorithm>
#include <stdexcept>

namespace Lexer {

// Construtor
LookaheadBuffer::LookaheadBuffer(std::istream& input, int size)
    : input_stream(input), buffer_size(size), head(0), tail(0), count(0), eof_reached(false) {
    if (size <= 0) {
        throw std::invalid_argument("Buffer size must be positive");
    }
    buffer.resize(buffer_size);
    fillBuffer(); // Preenche o buffer inicialmente
}

// Visualiza caractere sem consumir
char LookaheadBuffer::peek(int offset) {
    if (offset < 0) {
        throw std::invalid_argument("Offset cannot be negative");
    }
    
    // Garante que temos caracteres suficientes no buffer
    while (count <= offset && !eof_reached) {
        fillBuffer();
    }
    
    // Se não temos caracteres suficientes, retorna EOF
    if (count <= offset) {
        return '\0'; // EOF
    }
    
    // Calcula a posição no buffer circular
    int pos = (head + offset) % buffer_size;
    return buffer[pos];
}

// Consome e retorna próximo caractere
char LookaheadBuffer::consume() {
    if (count == 0 && eof_reached) {
        return '\0'; // EOF
    }
    
    // Garante que temos pelo menos um caractere
    if (count == 0) {
        fillBuffer();
        if (count == 0) {
            return '\0'; // EOF
        }
    }
    
    char ch = buffer[head];
    head = (head + 1) % buffer_size;
    count--;
    
    // Tenta preencher o buffer se necessário
    if (needsRefill()) {
        fillBuffer();
    }
    
    return ch;
}

// Retorna caractere para o buffer
void LookaheadBuffer::putback(char c) {
    if (count >= buffer_size) {
        // Buffer cheio, expande se necessário
        expandBuffer(buffer_size * 2);
    }
    
    // Move head para trás
    head = (head - 1 + buffer_size) % buffer_size;
    buffer[head] = c;
    count++;
}

// Verifica se há mais caracteres disponíveis
bool LookaheadBuffer::hasMore() const {
    return count > 0 || !eof_reached;
}

// Preenche buffer com novos caracteres
void LookaheadBuffer::fillBuffer() {
    if (eof_reached) {
        return;
    }
    
    // Preenche até o buffer estar cheio ou EOF ser alcançado
    while (count < buffer_size && !eof_reached) {
        char ch;
        if (input_stream.get(ch)) {
            buffer[tail] = ch;
            tail = (tail + 1) % buffer_size;
            count++;
        } else {
            eof_reached = true;
        }
    }
}

// Retorna tamanho atual do buffer
int LookaheadBuffer::getBufferSize() const {
    return buffer_size;
}

// Limpa o buffer
void LookaheadBuffer::clear() {
    head = 0;
    tail = 0;
    count = 0;
    eof_reached = false;
    std::fill(buffer.begin(), buffer.end(), '\0');
}

// Métodos privados auxiliares

// Move conteúdo do buffer para a esquerda
void LookaheadBuffer::shiftBuffer() {
    if (count == 0) {
        head = 0;
        tail = 0;
        return;
    }
    
    // Se head não está no início, reorganiza o buffer
    if (head != 0) {
        std::vector<char> temp(count);
        for (int i = 0; i < count; i++) {
            temp[i] = buffer[(head + i) % buffer_size];
        }
        
        // Copia de volta para o início do buffer
        for (int i = 0; i < count; i++) {
            buffer[i] = temp[i];
        }
        
        head = 0;
        tail = count;
    }
}

// Verifica se buffer precisa ser recarregado
bool LookaheadBuffer::needsRefill() {
    return count < buffer_size / 2 && !eof_reached;
}

// Expande tamanho do buffer dinamicamente
void LookaheadBuffer::expandBuffer(int newSize) {
    if (newSize <= buffer_size) {
        return;
    }
    
    // Reorganiza o buffer antes de expandir
    shiftBuffer();
    
    // Expande o buffer
    buffer.resize(newSize);
    buffer_size = newSize;
    
    // Preenche o espaço adicional
    fillBuffer();
}

} // namespace Lexer