#ifndef LOOKAHEAD_BUFFER_HPP
#define LOOKAHEAD_BUFFER_HPP

#include <iostream>
#include <vector>
#include <stdexcept>

namespace Lexer {

/**
 * @class LookaheadBuffer
 * @brief Buffer circular para análise preditiva de caracteres
 * 
 * Características:
 * - Tamanho configurável (padrão: 8 caracteres)
 * - Operações: peek(), consume(), putback()
 * - Preenchimento automático do buffer
 * - Suporte a múltiplos caracteres de antecipação
 */
class LookaheadBuffer {
private:
    std::istream& input_stream;     // Stream de entrada
    std::vector<char> buffer;       // Buffer circular
    int buffer_size;                // Tamanho do buffer
    int head;                       // Posição de leitura
    int tail;                       // Posição de escrita
    int count;                      // Número de caracteres no buffer
    bool eof_reached;               // Flag para fim de arquivo
    
    // Métodos privados auxiliares
    void shiftBuffer();             // Move conteúdo do buffer para a esquerda
    bool needsRefill();             // Verifica se buffer precisa ser recarregado
    void expandBuffer(int newSize); // Expande tamanho do buffer dinamicamente
    
public:
    /**
     * @brief Construtor com stream e tamanho do buffer
     * @param input Stream de entrada
     * @param size Tamanho do buffer (padrão: 8)
     */
    LookaheadBuffer(std::istream& input, int size = 8);
    
    /**
     * @brief Destrutor
     */
    ~LookaheadBuffer() = default;
    
    /**
     * @brief Visualiza caractere sem consumir
     * @param offset Deslocamento a partir da posição atual (padrão: 0)
     * @return Caractere na posição especificada
     */
    char peek(int offset = 0);
    
    /**
     * @brief Consome e retorna próximo caractere
     * @return Próximo caractere do buffer
     */
    char consume();
    
    /**
     * @brief Retorna caractere para o buffer
     * @param c Caractere a ser retornado
     */
    void putback(char c);
    
    /**
     * @brief Verifica se há mais caracteres disponíveis
     * @return true se há mais caracteres, false caso contrário
     */
    bool hasMore() const;
    
    /**
     * @brief Preenche buffer com novos caracteres
     */
    void fillBuffer();
    
    /**
     * @brief Retorna tamanho atual do buffer
     * @return Tamanho do buffer
     */
    int getBufferSize() const;
    
    /**
     * @brief Limpa o buffer
     */
    void clear();
};

} // namespace Lexer

#endif // LOOKAHEAD_BUFFER_HPP