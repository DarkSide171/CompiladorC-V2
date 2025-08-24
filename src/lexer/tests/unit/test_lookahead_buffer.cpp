// Testes Unitários - LookaheadBuffer
// Testes para a classe LookaheadBuffer e seus métodos

#include "../../include/lookahead_buffer.hpp"
#include <iostream>
#include <sstream>
#include <cassert>
#include <string>

using namespace Lexer;

// Função auxiliar para criar um stream de teste
std::istringstream createTestStream(const std::string& content) {
    return std::istringstream(content);
}

// Teste do construtor
void testConstructor() {
    std::cout << "Testando construtor...";
    
    auto stream = createTestStream("hello");
    LookaheadBuffer buffer(stream, 4);
    
    assert(buffer.getBufferSize() == 4);
    assert(buffer.hasMore() == true);
    
    std::cout << " PASSOU\n";
}

// Teste de construtor com tamanho inválido
void testConstructorInvalidSize() {
    std::cout << "Testando construtor com tamanho inválido...";
    
    auto stream = createTestStream("test");
    
    try {
        LookaheadBuffer buffer(stream, 0);
        assert(false); // Não deveria chegar aqui
    } catch (const std::invalid_argument&) {
        // Esperado
    }
    
    try {
        LookaheadBuffer buffer(stream, -1);
        assert(false); // Não deveria chegar aqui
    } catch (const std::invalid_argument&) {
        // Esperado
    }
    
    std::cout << " PASSOU\n";
}

// Teste básico de peek
void testPeekBasic() {
    std::cout << "Testando peek básico...";
    
    auto stream = createTestStream("hello");
    LookaheadBuffer buffer(stream, 8);
    
    assert(buffer.peek(0) == 'h');
    assert(buffer.peek(1) == 'e');
    assert(buffer.peek(2) == 'l');
    assert(buffer.peek(3) == 'l');
    assert(buffer.peek(4) == 'o');
    
    // Peek não deve consumir
    assert(buffer.peek(0) == 'h');
    
    std::cout << " PASSOU\n";
}

// Teste de peek com offset inválido
void testPeekInvalidOffset() {
    std::cout << "Testando peek com offset inválido...";
    
    auto stream = createTestStream("test");
    LookaheadBuffer buffer(stream, 4);
    
    try {
        buffer.peek(-1);
        assert(false); // Não deveria chegar aqui
    } catch (const std::invalid_argument&) {
        // Esperado
    }
    
    std::cout << " PASSOU\n";
}

// Teste de peek além do EOF
void testPeekBeyondEOF() {
    std::cout << "Testando peek além do EOF...";
    
    auto stream = createTestStream("hi");
    LookaheadBuffer buffer(stream, 8);
    
    assert(buffer.peek(0) == 'h');
    assert(buffer.peek(1) == 'i');
    assert(buffer.peek(2) == '\0'); // EOF
    assert(buffer.peek(10) == '\0'); // Muito além do EOF
    
    std::cout << " PASSOU\n";
}

// Teste básico de consume
void testConsumeBasic() {
    std::cout << "Testando consume básico...";
    
    auto stream = createTestStream("abc");
    LookaheadBuffer buffer(stream, 4);
    
    assert(buffer.consume() == 'a');
    assert(buffer.consume() == 'b');
    assert(buffer.consume() == 'c');
    assert(buffer.consume() == '\0'); // EOF
    
    std::cout << " PASSOU\n";
}

// Teste de consume em stream vazio
void testConsumeEmptyStream() {
    std::cout << "Testando consume em stream vazio...";
    
    auto stream = createTestStream("");
    LookaheadBuffer buffer(stream, 4);
    
    assert(buffer.consume() == '\0'); // EOF imediato
    
    std::cout << " PASSOU\n";
}

// Teste de putback
void testPutback() {
    std::cout << "Testando putback...";
    
    auto stream = createTestStream("abc");
    LookaheadBuffer buffer(stream, 4);
    
    char ch = buffer.consume(); // 'a'
    assert(ch == 'a');
    
    buffer.putback(ch);
    assert(buffer.peek(0) == 'a');
    assert(buffer.consume() == 'a');
    assert(buffer.consume() == 'b');
    
    std::cout << " PASSOU\n";
}

// Teste de putback múltiplo
void testMultiplePutback() {
    std::cout << "Testando putback múltiplo...";
    
    auto stream = createTestStream("abc");
    LookaheadBuffer buffer(stream, 4);
    
    char a = buffer.consume();
    char b = buffer.consume();
    
    buffer.putback(b);
    buffer.putback(a);
    
    assert(buffer.consume() == 'a');
    assert(buffer.consume() == 'b');
    assert(buffer.consume() == 'c');
    
    std::cout << " PASSOU\n";
}

// Teste de hasMore
void testHasMore() {
    std::cout << "Testando hasMore...";
    
    auto stream = createTestStream("ab");
    LookaheadBuffer buffer(stream, 4);
    
    assert(buffer.hasMore() == true);
    
    buffer.consume(); // 'a'
    assert(buffer.hasMore() == true);
    
    buffer.consume(); // 'b'
    assert(buffer.hasMore() == false);
    
    std::cout << " PASSOU\n";
}

// Teste de clear
void testClear() {
    std::cout << "Testando clear...";
    
    auto stream = createTestStream("hello");
    LookaheadBuffer buffer(stream, 4);
    
    // Consome alguns caracteres
    buffer.consume();
    buffer.consume();
    
    buffer.clear();
    
    // Após clear, não deve haver mais caracteres no buffer
    // mas o stream ainda pode ter dados
    assert(buffer.hasMore() == true); // Stream ainda tem dados
    
    std::cout << " PASSOU\n";
}

// Teste de buffer pequeno com texto longo
void testSmallBufferLongText() {
    std::cout << "Testando buffer pequeno com texto longo...";
    
    auto stream = createTestStream("abcdefghijklmnop");
    LookaheadBuffer buffer(stream, 3); // Buffer muito pequeno
    
    // Deve conseguir ler todos os caracteres
    std::string result;
    char ch;
    while ((ch = buffer.consume()) != '\0') {
        result += ch;
    }
    
    assert(result == "abcdefghijklmnop");
    
    std::cout << " PASSOU\n";
}

// Teste de peek e consume intercalados
void testPeekConsumeInterleaved() {
    std::cout << "Testando peek e consume intercalados...";
    
    auto stream = createTestStream("abcde");
    LookaheadBuffer buffer(stream, 8);
    
    assert(buffer.peek(0) == 'a');
    assert(buffer.peek(1) == 'b');
    
    assert(buffer.consume() == 'a');
    
    assert(buffer.peek(0) == 'b');
    assert(buffer.peek(1) == 'c');
    
    assert(buffer.consume() == 'b');
    assert(buffer.consume() == 'c');
    
    assert(buffer.peek(0) == 'd');
    
    std::cout << " PASSOU\n";
}

// Teste de expansão automática do buffer
void testBufferExpansion() {
    std::cout << "Testando expansão automática do buffer...";
    
    auto stream = createTestStream("abc");
    LookaheadBuffer buffer(stream, 2); // Buffer pequeno
    
    // Consome um caractere
    char a = buffer.consume();
    
    // Faz muitos putbacks para forçar expansão
    buffer.putback(a);
    buffer.putback('x');
    buffer.putback('y');
    
    // Buffer deve ter expandido automaticamente
    assert(buffer.getBufferSize() >= 4);
    
    std::cout << " PASSOU\n";
}

// Função principal de teste
int main() {
    std::cout << "=== Testes do LookaheadBuffer ===\n\n";
    
    try {
        testConstructor();
        testConstructorInvalidSize();
        testPeekBasic();
        testPeekInvalidOffset();
        testPeekBeyondEOF();
        testConsumeBasic();
        testConsumeEmptyStream();
        testPutback();
        testMultiplePutback();
        testHasMore();
        testClear();
        testSmallBufferLongText();
        testPeekConsumeInterleaved();
        testBufferExpansion();
        
        std::cout << "\n=== TODOS OS TESTES PASSARAM! ===\n";
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\nERRO: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "\nERRO DESCONHECIDO!" << std::endl;
        return 1;
    }
}