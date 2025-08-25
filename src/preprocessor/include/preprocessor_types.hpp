#ifndef PREPROCESSOR_TYPES_HPP
#define PREPROCESSOR_TYPES_HPP

#include <string>

namespace Preprocessor {

/**
 * @brief Estrutura para representar posição no código fonte
 * 
 * Mantém informações de posicionamento tanto no arquivo original quanto
 * no código expandido, essencial para mapeamento de erros e debugging.
 */
struct PreprocessorPosition {
    int line;              ///< Linha atual (compatibilidade)
    int column;            ///< Coluna atual (compatibilidade)
    int original_line;      ///< Linha no arquivo original
    int original_column;    ///< Coluna no arquivo original
    int expanded_line;      ///< Linha no código expandido
    int expanded_column;    ///< Coluna no código expandido
    std::string filename;   ///< Nome do arquivo atual
    std::string original_file; ///< Arquivo original (para includes)
    int offset;            ///< Offset para source mapping
    
    /**
     * @brief Construtor padrão
     */
    PreprocessorPosition() 
        : line(1), column(1), original_line(0), original_column(0), 
          expanded_line(0), expanded_column(0), offset(0) {}
    
    /**
     * @brief Construtor com parâmetros (compatibilidade)
     */
    PreprocessorPosition(const std::string& file, int line_val, int column_val)
        : line(line_val), column(column_val), 
          original_line(line_val), original_column(column_val), 
          expanded_line(line_val), expanded_column(column_val),
          filename(file), original_file(file), offset(0) {}
    
    /**
     * @brief Construtor com parâmetros (nova versão)
     */
    PreprocessorPosition(int line_val, int column_val, const std::string& file)
        : line(line_val), column(column_val), 
          original_line(line_val), original_column(column_val), 
          expanded_line(line_val), expanded_column(column_val),
          filename(file), original_file(file), offset(0) {}
    
    /**
     * @brief Operador de igualdade
     */
    bool operator==(const PreprocessorPosition& other) const {
        return line == other.line && 
               column == other.column &&
               filename == other.filename;
    }
};

} // namespace Preprocessor

#endif // PREPROCESSOR_TYPES_HPP