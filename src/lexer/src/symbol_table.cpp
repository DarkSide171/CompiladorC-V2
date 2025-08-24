// Symbol Table - Implementação da tabela de símbolos
// Implementação da classe SymbolTable para gerenciamento de símbolos

#include "../include/symbol_table.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>

namespace Lexer {

// Construtor
SymbolTable::SymbolTable(LexerConfig* lexer_config)
    : table_size(DEFAULT_SIZE), num_symbols(0), config(lexer_config) {
    table.resize(table_size);
}

// Função hash para strings (algoritmo djb2)
size_t SymbolTable::hashFunction(const std::string& name) const {
    size_t hash = 5381;
    for (char c : name) {
        hash = ((hash << 5) + hash) + static_cast<size_t>(c);
    }
    return hash % table_size;
}

// Encontra o slot para um símbolo
std::pair<size_t, int> SymbolTable::findSlot(const std::string& name) const {
    size_t index = hashFunction(name);
    const auto& bucket = table[index];
    
    for (int i = 0; i < static_cast<int>(bucket.size()); ++i) {
        if (bucket[i].name == name) {
            return {index, i};
        }
    }
    
    return {index, -1}; // Não encontrado
}

// Insere um novo símbolo na tabela
bool SymbolTable::insert(const std::string& name, TokenType type, const Position& pos) {
    // Verifica se o símbolo já existe
    auto [index, slot] = findSlot(name);
    if (slot != -1) {
        return false; // Símbolo já existe
    }
    
    // Verifica se precisa fazer rehash
    if (getLoadFactor() >= MAX_LOAD_FACTOR) {
        rehash();
        // Recalcula o índice após rehash
        index = hashFunction(name);
    }
    
    // Insere o novo símbolo
    table[index].emplace_back(name, type, pos);
    num_symbols++;
    
    return true;
}

// Busca um símbolo na tabela (versão não-const)
SymbolInfo* SymbolTable::lookup(const std::string& name) {
    auto [index, slot] = findSlot(name);
    if (slot != -1) {
        return &table[index][slot];
    }
    return nullptr;
}

// Busca um símbolo na tabela (versão const)
const SymbolInfo* SymbolTable::lookup(const std::string& name) const {
    auto [index, slot] = findSlot(name);
    if (slot != -1) {
        return &table[index][slot];
    }
    return nullptr;
}

// Verifica se um nome é uma palavra-chave
bool SymbolTable::isKeyword(const std::string& name, CVersion version) const {
    if (config) {
        return config->isKeyword(name);
    }
    
    // Fallback: verificação básica para palavras-chave C89
    static const std::unordered_set<std::string> c89_keywords = {
        "auto", "break", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if",
        "int", "long", "register", "return", "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
    };
    
    return c89_keywords.find(name) != c89_keywords.end();
}

// Retorna o tipo de uma palavra-chave
TokenType SymbolTable::getKeywordType(const std::string& name) const {
    if (config) {
        return config->getKeywordType(name);
    }
    
    // Fallback: mapeamento básico para palavras-chave C89
    static const std::unordered_map<std::string, TokenType> keyword_map = {
        {"auto", TokenType::AUTO}, {"break", TokenType::BREAK}, {"case", TokenType::CASE},
        {"char", TokenType::CHAR}, {"const", TokenType::CONST}, {"continue", TokenType::CONTINUE},
        {"default", TokenType::DEFAULT}, {"do", TokenType::DO}, {"double", TokenType::DOUBLE},
        {"else", TokenType::ELSE}, {"enum", TokenType::ENUM}, {"extern", TokenType::EXTERN},
        {"float", TokenType::FLOAT}, {"for", TokenType::FOR}, {"goto", TokenType::GOTO},
        {"if", TokenType::IF}, {"int", TokenType::INT}, {"long", TokenType::LONG},
        {"register", TokenType::REGISTER}, {"return", TokenType::RETURN}, {"short", TokenType::SHORT},
        {"signed", TokenType::SIGNED}, {"sizeof", TokenType::SIZEOF}, {"static", TokenType::STATIC},
        {"struct", TokenType::STRUCT}, {"switch", TokenType::SWITCH}, {"typedef", TokenType::TYPEDEF},
        {"union", TokenType::UNION}, {"unsigned", TokenType::UNSIGNED}, {"void", TokenType::VOID},
        {"volatile", TokenType::VOLATILE}, {"while", TokenType::WHILE}
    };
    
    auto it = keyword_map.find(name);
    return (it != keyword_map.end()) ? it->second : TokenType::IDENTIFIER;
}

// Adiciona uma nova ocorrência de um símbolo existente
bool SymbolTable::addOccurrence(const std::string& name, const Position& pos) {
    SymbolInfo* symbol = lookup(name);
    if (symbol) {
        symbol->all_occurrences.push_back(pos);
        return true;
    }
    return false;
}

// Retorna todos os símbolos registrados
std::vector<SymbolInfo> SymbolTable::getAllSymbols() const {
    std::vector<SymbolInfo> symbols;
    symbols.reserve(num_symbols);
    
    for (const auto& bucket : table) {
        for (const auto& symbol : bucket) {
            symbols.push_back(symbol);
        }
    }
    
    // Ordena por nome para saída consistente
    std::sort(symbols.begin(), symbols.end(), 
              [](const SymbolInfo& a, const SymbolInfo& b) {
                  return a.name < b.name;
              });
    
    return symbols;
}

// Imprime estatísticas de uso da tabela
void SymbolTable::printStatistics() const {
    std::cout << "\n=== Estatísticas da Tabela de Símbolos ===\n";
    std::cout << "Número total de símbolos: " << num_symbols << "\n";
    std::cout << "Tamanho da tabela: " << table_size << "\n";
    std::cout << "Fator de carga: " << std::fixed << std::setprecision(2) 
              << getLoadFactor() << "\n";
    
    // Estatísticas de distribuição
    size_t empty_buckets = 0;
    size_t max_bucket_size = 0;
    size_t total_collisions = 0;
    
    for (const auto& bucket : table) {
        if (bucket.empty()) {
            empty_buckets++;
        } else {
            max_bucket_size = std::max(max_bucket_size, bucket.size());
            if (bucket.size() > 1) {
                total_collisions += bucket.size() - 1;
            }
        }
    }
    
    std::cout << "Buckets vazios: " << empty_buckets << " (" 
              << std::fixed << std::setprecision(1)
              << (100.0 * empty_buckets / table_size) << "%)\n";
    std::cout << "Maior bucket: " << max_bucket_size << " símbolos\n";
    std::cout << "Total de colisões: " << total_collisions << "\n";
    
    // Lista os símbolos mais frequentes
    auto symbols = getAllSymbols();
    if (!symbols.empty()) {
        std::cout << "\n=== Símbolos Registrados ===\n";
        for (const auto& symbol : symbols) {
            std::cout << std::left << std::setw(20) << symbol.name 
                      << " | Ocorrências: " << symbol.all_occurrences.size()
                      << " | Primeira: (" << symbol.first_occurrence.line 
                      << "," << symbol.first_occurrence.column << ")\n";
        }
    }
}

// Reorganiza a tabela quando o fator de carga é alto
void SymbolTable::rehash() {
    // Salva os símbolos atuais
    auto old_symbols = getAllSymbols();
    
    // Dobra o tamanho da tabela (próximo número primo)
    size_t new_size = table_size * 2;
    // Encontra o próximo número primo
    while (!isPrime(new_size)) {
        new_size++;
    }
    
    // Reinicializa a tabela
    table_size = new_size;
    table.clear();
    table.resize(table_size);
    num_symbols = 0;
    
    // Reinsere todos os símbolos
    for (const auto& symbol : old_symbols) {
        insert(symbol.name, symbol.category, symbol.first_occurrence);
        // Adiciona as ocorrências adicionais
        for (size_t i = 1; i < symbol.all_occurrences.size(); ++i) {
            addOccurrence(symbol.name, symbol.all_occurrences[i]);
        }
    }
}

// Carrega palavras-chave de um arquivo
bool SymbolTable::loadKeywordsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string keyword;
    Position pos(1, 1, 0); // Posição padrão para palavras-chave
    
    while (std::getline(file, keyword)) {
        // Remove espaços em branco
        keyword.erase(0, keyword.find_first_not_of(" \t"));
        keyword.erase(keyword.find_last_not_of(" \t") + 1);
        
        if (!keyword.empty() && keyword[0] != '#') { // Ignora comentários
            TokenType type = getKeywordType(keyword);
            insert(keyword, type, pos);
        }
    }
    
    return true;
}

// Limpa todos os símbolos da tabela
void SymbolTable::clear() {
    for (auto& bucket : table) {
        bucket.clear();
    }
    num_symbols = 0;
}

// Função auxiliar para verificar se um número é primo
bool SymbolTable::isPrime(size_t n) const {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    
    for (size_t i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

} // namespace Lexer