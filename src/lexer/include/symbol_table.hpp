#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "token.hpp"
#include "lexer_config.hpp"

namespace Lexer {

// Symbol Table - Tabela de símbolos para identificadores
// Implementação da classe SymbolTable para gerenciamento de símbolos

/**
 * @brief Estrutura que armazena informações sobre um símbolo
 */
struct SymbolInfo {
    std::string name;                           // Nome do símbolo
    TokenType category;                         // Categoria do token (IDENTIFIER, KEYWORD, etc.)
    Position first_occurrence;                  // Primeira ocorrência do símbolo
    std::vector<Position> all_occurrences;     // Todas as ocorrências do símbolo
    
    SymbolInfo() = default;
    SymbolInfo(const std::string& n, TokenType cat, const Position& pos)
        : name(n), category(cat), first_occurrence(pos) {
        all_occurrences.push_back(pos);
    }
};

/**
 * @brief Classe que implementa uma tabela de símbolos usando hash table
 */
class SymbolTable {
private:
    static const size_t DEFAULT_SIZE = 101;     // Tamanho padrão da tabela (número primo)
    static constexpr double MAX_LOAD_FACTOR = 0.75; // Fator de carga máximo
    
    std::vector<std::vector<SymbolInfo>> table; // Hash table com chaining
    size_t table_size;                          // Tamanho atual da tabela
    size_t num_symbols;                         // Número de símbolos na tabela
    LexerConfig* config;                        // Referência para configuração do lexer
    
    /**
     * @brief Função hash para strings
     * @param name Nome do símbolo
     * @return Índice hash
     */
    size_t hashFunction(const std::string& name) const;
    
    /**
     * @brief Encontra o slot para um símbolo
     * @param name Nome do símbolo
     * @return Par com índice da tabela e índice na lista (ou -1 se não encontrado)
     */
    std::pair<size_t, int> findSlot(const std::string& name) const;
    
    /**
     * @brief Reorganiza a tabela quando o fator de carga é alto
     */
    void rehash();
    
    /**
     * @brief Verifica se um número é primo (função auxiliar)
     * @param n Número a verificar
     * @return true se for primo
     */
    bool isPrime(size_t n) const;
    
public:
    /**
     * @brief Construtor da tabela de símbolos
     * @param lexer_config Ponteiro para configuração do lexer (opcional)
     */
    explicit SymbolTable(LexerConfig* lexer_config = nullptr);
    
    /**
     * @brief Destrutor
     */
    ~SymbolTable() = default;
    
    /**
     * @brief Insere um novo símbolo na tabela
     * @param name Nome do símbolo
     * @param type Tipo do token
     * @param pos Posição da primeira ocorrência
     * @return true se inserido com sucesso, false se já existia
     */
    bool insert(const std::string& name, TokenType type, const Position& pos);
    
    /**
     * @brief Busca um símbolo na tabela
     * @param name Nome do símbolo
     * @return Ponteiro para SymbolInfo ou nullptr se não encontrado
     */
    SymbolInfo* lookup(const std::string& name);
    
    /**
     * @brief Versão const do lookup
     * @param name Nome do símbolo
     * @return Ponteiro const para SymbolInfo ou nullptr se não encontrado
     */
    const SymbolInfo* lookup(const std::string& name) const;
    
    /**
     * @brief Verifica se um nome é uma palavra-chave
     * @param name Nome a verificar
     * @param version Versão do C (usa configuração atual se não especificada)
     * @return true se for palavra-chave
     */
    bool isKeyword(const std::string& name, CVersion version = CVersion::C89) const;
    
    /**
     * @brief Retorna o tipo de uma palavra-chave
     * @param name Nome da palavra-chave
     * @return Tipo do token da palavra-chave
     */
    TokenType getKeywordType(const std::string& name) const;
    
    /**
     * @brief Adiciona uma nova ocorrência de um símbolo existente
     * @param name Nome do símbolo
     * @param pos Posição da nova ocorrência
     * @return true se adicionado com sucesso
     */
    bool addOccurrence(const std::string& name, const Position& pos);
    
    /**
     * @brief Retorna todos os símbolos registrados
     * @return Vetor com todos os símbolos
     */
    std::vector<SymbolInfo> getAllSymbols() const;
    
    /**
     * @brief Imprime estatísticas de uso da tabela
     */
    void printStatistics() const;
    
    /**
     * @brief Carrega palavras-chave de um arquivo
     * @param filename Nome do arquivo
     * @return true se carregado com sucesso
     */
    bool loadKeywordsFromFile(const std::string& filename);
    
    /**
     * @brief Retorna o número de símbolos na tabela
     * @return Número de símbolos
     */
    size_t size() const { return num_symbols; }
    
    /**
     * @brief Verifica se a tabela está vazia
     * @return true se vazia
     */
    bool empty() const { return num_symbols == 0; }
    
    /**
     * @brief Limpa todos os símbolos da tabela
     */
    void clear();
    
    /**
     * @brief Retorna o fator de carga atual
     * @return Fator de carga (número de símbolos / tamanho da tabela)
     */
    double getLoadFactor() const { return static_cast<double>(num_symbols) / table_size; }
};

} // namespace Lexer

#endif // SYMBOL_TABLE_HPP