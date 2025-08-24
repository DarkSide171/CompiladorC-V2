// Testes unitários para SymbolTable
// Validação da implementação da Fase 2.1

#include "../../include/symbol_table.hpp"
#include "../../include/lexer_config.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <functional>

using namespace Lexer;

// Função auxiliar para capturar saída do cout
std::string captureOutput(std::function<void()> func) {
    std::ostringstream oss;
    std::streambuf* old_cout = std::cout.rdbuf(oss.rdbuf());
    func();
    std::cout.rdbuf(old_cout);
    return oss.str();
}

// Teste básico de construção e inicialização
void testBasicConstruction() {
    std::cout << "Testando construção básica...";
    
    SymbolTable table;
    assert(table.empty());
    assert(table.size() == 0);
    assert(table.getLoadFactor() == 0.0);
    
    std::cout << " ✓\n";
}

// Teste de inserção e busca de símbolos
void testInsertAndLookup() {
    std::cout << "Testando inserção e busca...";
    
    SymbolTable table;
    Position pos1(1, 1, 0);
    Position pos2(2, 5, 10);
    
    // Inserção de símbolos
    assert(table.insert("variable1", TokenType::IDENTIFIER, pos1));
    assert(table.insert("function1", TokenType::IDENTIFIER, pos2));
    assert(!table.insert("variable1", TokenType::IDENTIFIER, pos1)); // Duplicata
    
    assert(table.size() == 2);
    assert(!table.empty());
    
    // Busca de símbolos
    SymbolInfo* sym1 = table.lookup("variable1");
    assert(sym1 != nullptr);
    assert(sym1->name == "variable1");
    assert(sym1->category == TokenType::IDENTIFIER);
    assert(sym1->first_occurrence == pos1);
    assert(sym1->all_occurrences.size() == 1);
    
    SymbolInfo* sym2 = table.lookup("function1");
    assert(sym2 != nullptr);
    assert(sym2->name == "function1");
    
    // Símbolo inexistente
    assert(table.lookup("nonexistent") == nullptr);
    
    std::cout << " ✓\n";
}

// Teste de adição de ocorrências
void testAddOccurrence() {
    std::cout << "Testando adição de ocorrências...";
    
    SymbolTable table;
    Position pos1(1, 1, 0);
    Position pos2(3, 5, 15);
    Position pos3(5, 10, 25);
    
    // Insere símbolo
    assert(table.insert("var", TokenType::IDENTIFIER, pos1));
    
    // Adiciona ocorrências
    assert(table.addOccurrence("var", pos2));
    assert(table.addOccurrence("var", pos3));
    assert(!table.addOccurrence("nonexistent", pos1)); // Símbolo inexistente
    
    // Verifica ocorrências
    SymbolInfo* sym = table.lookup("var");
    assert(sym != nullptr);
    assert(sym->all_occurrences.size() == 3);
    assert(sym->all_occurrences[0] == pos1);
    assert(sym->all_occurrences[1] == pos2);
    assert(sym->all_occurrences[2] == pos3);
    
    std::cout << " ✓\n";
}

// Teste de verificação de palavras-chave
void testKeywordDetection() {
    std::cout << "Testando detecção de palavras-chave...";
    
    SymbolTable table;
    
    // Testa palavras-chave C89 básicas
    assert(table.isKeyword("int", CVersion::C89));
    assert(table.isKeyword("if", CVersion::C89));
    assert(table.isKeyword("while", CVersion::C89));
    assert(table.isKeyword("return", CVersion::C89));
    assert(table.isKeyword("struct", CVersion::C89));
    
    // Testa não palavras-chave
    assert(!table.isKeyword("variable", CVersion::C89));
    assert(!table.isKeyword("myFunction", CVersion::C89));
    assert(!table.isKeyword("123invalid", CVersion::C89));
    
    // Testa tipos de palavras-chave
    assert(table.getKeywordType("int") == TokenType::INT);
    assert(table.getKeywordType("if") == TokenType::IF);
    assert(table.getKeywordType("while") == TokenType::WHILE);
    assert(table.getKeywordType("return") == TokenType::RETURN);
    assert(table.getKeywordType("nonkeyword") == TokenType::IDENTIFIER);
    
    std::cout << " ✓\n";
}

// Teste com configuração do lexer
void testWithLexerConfig() {
    std::cout << "Testando integração com LexerConfig...";
    
    LexerConfig config(CVersion::C99);
    SymbolTable table(&config);
    
    // Testa palavras-chave através da configuração
    assert(table.isKeyword("int", CVersion::C89));
    assert(table.isKeyword("inline", CVersion::C99)); // C99 keyword
    
    std::cout << " ✓\n";
}

// Teste de obtenção de todos os símbolos
void testGetAllSymbols() {
    std::cout << "Testando obtenção de todos os símbolos...";
    
    SymbolTable table;
    Position pos(1, 1, 0);
    
    // Insere vários símbolos
    table.insert("zebra", TokenType::IDENTIFIER, pos);
    table.insert("alpha", TokenType::IDENTIFIER, pos);
    table.insert("beta", TokenType::IDENTIFIER, pos);
    
    auto symbols = table.getAllSymbols();
    assert(symbols.size() == 3);
    
    // Verifica se estão ordenados por nome
    assert(symbols[0].name == "alpha");
    assert(symbols[1].name == "beta");
    assert(symbols[2].name == "zebra");
    
    std::cout << " ✓\n";
}

// Teste de limpeza da tabela
void testClear() {
    std::cout << "Testando limpeza da tabela...";
    
    SymbolTable table;
    Position pos(1, 1, 0);
    
    // Insere símbolos
    table.insert("sym1", TokenType::IDENTIFIER, pos);
    table.insert("sym2", TokenType::IDENTIFIER, pos);
    assert(table.size() == 2);
    
    // Limpa tabela
    table.clear();
    assert(table.empty());
    assert(table.size() == 0);
    assert(table.lookup("sym1") == nullptr);
    
    std::cout << " ✓\n";
}

// Teste de fator de carga e rehash
void testLoadFactorAndRehash() {
    std::cout << "Testando fator de carga e rehash...";
    
    SymbolTable table;
    Position pos(1, 1, 0);
    
    // Insere muitos símbolos para forçar rehash
    for (int i = 0; i < 100; ++i) {
        std::string name = "symbol" + std::to_string(i);
        table.insert(name, TokenType::IDENTIFIER, pos);
    }
    
    assert(table.size() == 100);
    
    // Verifica se todos os símbolos ainda podem ser encontrados
    for (int i = 0; i < 100; ++i) {
        std::string name = "symbol" + std::to_string(i);
        assert(table.lookup(name) != nullptr);
    }
    
    // Verifica fator de carga
    double loadFactor = table.getLoadFactor();
    assert(loadFactor > 0.0 && loadFactor <= 1.0);
    
    std::cout << " ✓\n";
}

// Teste de estatísticas
void testPrintStatistics() {
    std::cout << "Testando impressão de estatísticas...";
    
    SymbolTable table;
    Position pos(1, 1, 0);
    
    // Insere alguns símbolos
    table.insert("var1", TokenType::IDENTIFIER, pos);
    table.insert("var2", TokenType::IDENTIFIER, pos);
    table.addOccurrence("var1", Position(2, 1, 10));
    
    // Captura saída das estatísticas
    std::string output = captureOutput([&table]() {
        table.printStatistics();
    });
    
    // Verifica se contém informações esperadas
    assert(output.find("Número total de símbolos: 2") != std::string::npos);
    assert(output.find("var1") != std::string::npos);
    assert(output.find("var2") != std::string::npos);
    
    std::cout << " ✓\n";
}

// Função principal de teste
int main() {
    std::cout << "=== Testes da SymbolTable (Fase 2.1) ===\n\n";
    
    try {
        testBasicConstruction();
        testInsertAndLookup();
        testAddOccurrence();
        testKeywordDetection();
        testWithLexerConfig();
        testGetAllSymbols();
        testClear();
        testLoadFactorAndRehash();
        testPrintStatistics();
        
        std::cout << "\n✅ Todos os testes da SymbolTable passaram com sucesso!\n";
        std::cout << "🎉 Fase 2.1 implementada corretamente!\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\n❌ Erro durante os testes: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cout << "\n❌ Erro desconhecido durante os testes\n";
        return 1;
    }
}