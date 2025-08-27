#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <chrono>
#include <fstream>
#include "preprocessor_logger.hpp"

namespace Preprocessor {

// ============================================================================
// ESTRUTURAS DE DADOS
// ============================================================================

/**
 * @brief Estatísticas de operações de arquivo
 */
struct FileStats {
    size_t files_read = 0;              // Arquivos lidos
    size_t files_cached = 0;            // Arquivos em cache
    size_t cache_hits = 0;              // Acertos de cache
    size_t cache_misses = 0;            // Perdas de cache
    size_t total_bytes_read = 0;        // Total de bytes lidos
    size_t circular_inclusions = 0;     // Inclusões circulares detectadas
    size_t path_resolutions = 0;        // Resoluções de caminho
    size_t dependency_updates = 0;      // Atualizações de dependência
    
    // Métodos utilitários
    double getCacheHitRatio() const {
        size_t total = cache_hits + cache_misses;
        return total > 0 ? static_cast<double>(cache_hits) / total : 0.0;
    }
    
    void reset() {
        files_read = files_cached = cache_hits = cache_misses = 0;
        total_bytes_read = circular_inclusions = path_resolutions = dependency_updates = 0;
    }
};

/**
 * @brief Informações de um arquivo em cache
 */
struct CachedFile {
    std::string content;                           // Conteúdo do arquivo
    std::chrono::system_clock::time_point timestamp; // Timestamp de cache
    std::chrono::system_clock::time_point last_modified; // Última modificação do arquivo
    size_t file_size;                             // Tamanho do arquivo
    std::string normalized_path;                  // Caminho normalizado
    std::string file_hash;                        // Hash do conteúdo para validação
    bool is_system_file;                         // Se é arquivo de sistema
    size_t access_count;                         // Contador de acessos
    std::chrono::system_clock::time_point last_access; // Último acesso
    
    CachedFile() : file_size(0), is_system_file(false), access_count(0) {}
    
    CachedFile(const std::string& content, size_t size, bool system_file = false)
        : content(content), timestamp(std::chrono::system_clock::now()),
          file_size(size), is_system_file(system_file),
          last_access(std::chrono::system_clock::now()),
          access_count(1) {}
    
    // Verifica se o cache expirou (TTL)
    bool isExpired(std::chrono::seconds ttl = std::chrono::seconds(300)) const {
        auto now = std::chrono::system_clock::now();
        return (now - timestamp) > ttl;
    }
    
    // Atualiza estatísticas de acesso
    void updateAccess() {
        last_access = std::chrono::system_clock::now();
        access_count++;
    }
};

/**
 * @brief Informações de dependência de arquivo
 */
struct FileDependency {
    std::string filepath;                    // Caminho do arquivo
    std::vector<std::string> dependencies;  // Lista de dependências
    std::chrono::system_clock::time_point last_modified; // Última modificação
    bool is_circular;                       // Se tem dependência circular
    
    FileDependency() : is_circular(false) {}
    
    FileDependency(const std::string& path) 
        : filepath(path), last_modified(std::chrono::system_clock::now()),
          is_circular(false) {}
};

// ============================================================================
// CLASSE PRINCIPAL
// ============================================================================

/**
 * @brief Gerenciador de arquivos para o pré-processador
 * 
 * Responsável por:
 * - Leitura e escrita de arquivos
 * - Resolução de caminhos de inclusão
 * - Cache de arquivos
 * - Detecção de inclusões circulares
 * - Gerenciamento de dependências
 */
class FileManager {
public:
    // ========================================================================
    // CONSTRUTORES E DESTRUTOR
    // ========================================================================
    
    /**
     * @brief Construtor com caminhos de busca
     * @param search_paths Caminhos de busca para inclusões
     * @param logger Ponteiro para o logger (pode ser nullptr)
     */
    explicit FileManager(const std::vector<std::string>& search_paths = {},
                        PreprocessorLogger* logger = nullptr);
    
    /**
     * @brief Destrutor
     */
    ~FileManager();
    
    // Construtor de cópia e operador de atribuição (deletados)
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;
    
    // Construtor de movimento e operador de atribuição de movimento
    FileManager(FileManager&& other) noexcept;
    FileManager& operator=(FileManager&& other) noexcept;
    
    // ========================================================================
    // OPERAÇÕES BÁSICAS DE ARQUIVO
    // ========================================================================
    
    /**
     * @brief Lê o conteúdo de um arquivo
     * @param filepath Caminho do arquivo
     * @return Conteúdo do arquivo
     * @throws std::runtime_error se não conseguir ler o arquivo
     */
    std::string readFile(const std::string& filepath);
    
    /**
     * @brief Escreve conteúdo em um arquivo
     * @param filepath Caminho do arquivo
     * @param content Conteúdo a ser escrito
     * @return true se escreveu com sucesso
     */
    bool writeFile(const std::string& filepath, const std::string& content);
    
    /**
     * @brief Verifica se um arquivo existe
     * @param filepath Caminho do arquivo
     * @return true se o arquivo existe
     */
    bool fileExists(const std::string& filepath) const;
    
    /**
     * @brief Verifica se um arquivo existe (alias para fileExists)
     * @param filepath Caminho do arquivo
     * @return true se o arquivo existe
     */
    bool checkFileExists(const std::string& filepath) const {
        return fileExists(filepath);
    }
    
    // ========================================================================
    // RESOLUÇÃO DE INCLUSÕES
    // ========================================================================
    
    /**
     * @brief Resolve caminho de inclusão
     * @param filename Nome do arquivo a incluir
     * @param is_system Se é inclusão de sistema (<>) ou local ("")
     * @param current_file Arquivo atual (para inclusões relativas)
     * @return Caminho resolvido do arquivo
     * @throws std::runtime_error se não conseguir resolver
     */
    std::string resolveInclude(const std::string& filename, 
                              bool is_system, 
                              const std::string& current_file = "");
    
    // ========================================================================
    // GERENCIAMENTO DE CAMINHOS DE BUSCA
    // ========================================================================
    
    /**
     * @brief Adiciona um caminho de busca
     * @param path Caminho a ser adicionado
     */
    void addSearchPath(const std::string& path);
    
    /**
     * @brief Define os caminhos de busca
     * @param paths Novos caminhos de busca
     */
    void setSearchPaths(const std::vector<std::string>& paths);
    
    /**
     * @brief Obtém os caminhos de busca configurados
     * @return Lista de caminhos de busca
     */
    std::vector<std::string> getSearchPaths() const;
    
    // ========================================================================
    // DETECÇÃO DE INCLUSÕES CIRCULARES
    // ========================================================================
    
    /**
     * @brief Verifica se há inclusão circular
     * @param filepath Caminho do arquivo
     * @param include_stack Pilha de inclusões atual
     * @return true se há inclusão circular
     */
    bool checkCircularInclusion(const std::string& filepath, 
                               const std::vector<std::string>& include_stack);
    
    // ========================================================================
    // SISTEMA DE CACHE
    // ========================================================================
    
    /**
     * @brief Limpa o cache de arquivos
     */
    void clearCache();
    
    /**
     * @brief Configura parâmetros de cache otimizado
     * @param max_size Tamanho máximo do cache em bytes
     * @param max_entries Número máximo de entradas
     * @param ttl TTL do cache em segundos
     * @param enable_compression Habilitar compressão
     */
    void configureCacheOptimization(size_t max_size = 50 * 1024 * 1024, // 50MB
                                   size_t max_entries = 1000,
                                   std::chrono::seconds ttl = std::chrono::seconds(300),
                                   bool enable_compression = false);
    
    /**
     * @brief Limpa cache expirado e otimiza memória
     */
    void optimizeCache();
    
    /**
     * @brief Pré-carrega arquivos frequentemente usados
     * @param filepaths Lista de arquivos para pré-carregar
     */
    void preloadFiles(const std::vector<std::string>& filepaths);
    
    // ========================================================================
    // GERENCIAMENTO DE DEPENDÊNCIAS
    // ========================================================================
    
    /**
     * @brief Obtém lista de dependências
     * @return Lista de todas as dependências
     */
    std::vector<std::string> getDependencies() const;
    
    // ========================================================================
    // INFORMAÇÕES DE ARQUIVO
    // ========================================================================
    
    /**
     * @brief Obtém o tamanho de um arquivo
     * @param filepath Caminho do arquivo
     * @return Tamanho do arquivo em bytes
     */
    size_t getFileSize(const std::string& filepath) const;
    
    /**
     * @brief Obtém a data de última modificação
     * @param filepath Caminho do arquivo
     * @return Timestamp da última modificação
     */
    std::chrono::system_clock::time_point getLastModified(const std::string& filepath) const;
    
    // ========================================================================
    // ESTATÍSTICAS
    // ========================================================================
    
    /**
     * @brief Obtém estatísticas de operações de arquivo
     * @return Estrutura com estatísticas
     */
    FileStats getStatistics() const;
    
    /**
     * @brief Reseta as estatísticas
     */
    void resetStatistics();
    
    // ========================================================================
    // MÉTODOS DE BACKUP E RESTAURAÇÃO
    // ========================================================================
    
    /**
     * @brief Cria backup de um arquivo
     * @param filepath Caminho do arquivo original
     * @param backup_suffix Sufixo para o arquivo de backup (padrão: ".bak")
     * @return true se o backup foi criado com sucesso
     */
    bool createBackup(const std::string& filepath, const std::string& backup_suffix = ".bak");
    
    /**
     * @brief Restaura arquivo a partir do backup
     * @param filepath Caminho do arquivo original
     * @param backup_suffix Sufixo do arquivo de backup (padrão: ".bak")
     * @return true se a restauração foi bem-sucedida
     */
    bool restoreFromBackup(const std::string& filepath, const std::string& backup_suffix = ".bak");
    
    // ========================================================================
    // CONTROLE DE ACESSO A ARQUIVOS
    // ========================================================================
    
    /**
     * @brief Bloqueia um arquivo para acesso exclusivo
     * @param filepath Caminho do arquivo
     * @return true se o arquivo foi bloqueado com sucesso
     */
    bool lockFile(const std::string& filepath);
    
    /**
     * @brief Desbloqueia um arquivo
     * @param filepath Caminho do arquivo
     * @return true se o arquivo foi desbloqueado com sucesso
     */
    bool unlockFile(const std::string& filepath);
    
    // ========================================================================
    // COMPRESSÃO DE ARQUIVOS
    // ========================================================================
    
    /**
     * @brief Comprime um arquivo
     * @param filepath Caminho do arquivo original
     * @param compressed_path Caminho do arquivo comprimido (opcional)
     * @return true se a compressão foi bem-sucedida
     */
    bool compressFile(const std::string& filepath, const std::string& compressed_path = "");
    
    /**
     * @brief Descomprime um arquivo
     * @param compressed_path Caminho do arquivo comprimido
     * @param output_path Caminho do arquivo descomprimido (opcional)
     * @return true se a descompressão foi bem-sucedida
     */
    bool decompressFile(const std::string& compressed_path, const std::string& output_path = "");
    
    // ========================================================================
    // VERIFICAÇÃO DE INTEGRIDADE
    // ========================================================================
    
    /**
     * @brief Calcula hash de um arquivo
     * @param filepath Caminho do arquivo
     * @return Hash do arquivo (SHA-256)
     */
    std::string calculateFileHash(const std::string& filepath) const;
    
    /**
     * @brief Verifica integridade de um arquivo
     * @param filepath Caminho do arquivo
     * @param expected_hash Hash esperado
     * @return true se a integridade está correta
     */
    bool verifyFileIntegrity(const std::string& filepath, const std::string& expected_hash) const;
    
    // ========================================================================
    // MONITORAMENTO DE ARQUIVOS
    // ========================================================================
    
    /**
     * @brief Monitora mudanças em arquivos
     * @param filepath Caminho do arquivo a monitorar
     * @return true se o monitoramento foi iniciado com sucesso
     */
    bool monitorFileChanges(const std::string& filepath);
    
    /**
     * @brief Manipula eventos do sistema de arquivos
     * @param event_type Tipo do evento
     * @param filepath Caminho do arquivo
     */
    void handleFileSystemEvents(const std::string& event_type, const std::string& filepath);
    
    /**
     * @brief Define um manipulador de erros externo
     * @param errorHandler Ponteiro para o manipulador de erros
     */
    void setErrorHandler(void* errorHandler);

private:
    // ========================================================================
    // MEMBROS PRIVADOS
    // ========================================================================
    
    std::vector<std::string> search_paths_;              // Caminhos de busca
    std::unordered_map<std::string, CachedFile> file_cache_; // Cache de arquivos
    std::unordered_map<std::string, FileDependency> dependencies_; // Dependências
    std::unordered_set<std::string> circular_detection_set_; // Detecção circular
    PreprocessorLogger* logger_;                         // Logger
    mutable FileStats stats_;                           // Estatísticas
    
    // Configurações de cache otimizado
    size_t max_cache_size_;                             // Tamanho máximo do cache (bytes)
    size_t max_cache_entries_;                          // Número máximo de entradas
    std::chrono::seconds cache_ttl_;                    // TTL do cache
    bool enable_cache_compression_;                     // Compressão de cache
    
    // Arquivos e monitoramento
    std::unordered_set<std::string> locked_files_;           // Arquivos bloqueados
    std::unordered_map<std::string, std::string> file_hashes_; // Cache de hashes
    std::unordered_set<std::string> monitored_files_;        // Arquivos monitorados
    
    // Tratamento de erros externo
    void* external_error_handler_;                           // Manipulador de erros externo
    
    // ========================================================================
    // MÉTODOS AUXILIARES PRIVADOS
    // ========================================================================
    
    /**
     * @brief Busca arquivo nos caminhos especificados
     * @param filename Nome do arquivo
     * @param paths Caminhos onde buscar
     * @return Caminho completo se encontrado, string vazia caso contrário
     */
    std::string searchInPaths(const std::string& filename, 
                             const std::vector<std::string>& paths) const;
    
    /**
     * @brief Resolve caminho relativo
     * @param filename Nome do arquivo
     * @param base_path Caminho base
     * @return Caminho resolvido
     */
    std::string resolveRelativePath(const std::string& filename, 
                                   const std::string& base_path) const;
    
    /**
     * @brief Armazena arquivo no cache com otimizações
     * @param filepath Caminho do arquivo
     * @param content Conteúdo do arquivo
     * @param file_modified Timestamp de modificação do arquivo
     */
    void cacheFile(const std::string& filepath, const std::string& content,
                   std::chrono::system_clock::time_point file_modified = std::chrono::system_clock::now());
    
    /**
     * @brief Verifica se o cache precisa ser invalidado
     * @param filepath Caminho do arquivo
     * @return true se precisa invalidar
     */
    bool shouldInvalidateCache(const std::string& filepath) const;
    
    /**
     * @brief Remove entradas menos usadas do cache (LRU)
     * @param target_size Tamanho alvo após limpeza
     */
    void evictLeastRecentlyUsed(size_t target_size);
    
    /**
     * @brief Calcula tamanho atual do cache em bytes
     * @return Tamanho do cache
     */
    size_t getCurrentCacheSize() const;
    
    /**
     * @brief Recupera arquivo do cache
     * @param filepath Caminho do arquivo
     * @return Ponteiro para arquivo em cache ou nullptr
     */
    const CachedFile* getCachedFile(const std::string& filepath) const;
    
    /**
     * @brief Valida caminho de arquivo
     * @param filepath Caminho a validar
     * @return true se válido
     */
    bool validateFilePath(const std::string& filepath) const;
    
    /**
     * @brief Normaliza caminho de arquivo
     * @param filepath Caminho a normalizar
     * @return Caminho normalizado
     */
    std::string normalizeFilePath(const std::string& filepath) const;
    
    /**
     * @brief Atualiza dependências de um arquivo
     * @param filepath Caminho do arquivo
     */
    void updateDependencies(const std::string& filepath);
    
    /**
     * @brief Registra erro no logger
     * @param message Mensagem de erro
     * @param filepath Caminho do arquivo relacionado
     */
    void logError(const std::string& message, const std::string& filepath = "") const;
    
    /**
     * @brief Registra aviso no logger
     * @param message Mensagem de aviso
     * @param filepath Caminho do arquivo relacionado
     */
    void logWarning(const std::string& message, const std::string& filepath = "") const;
    
    /**
     * @brief Registra informação no logger
     * @param message Mensagem informativa
     * @param filepath Caminho do arquivo relacionado
     */
    void logInfo(const std::string& message, const std::string& filepath = "") const;
};

} // namespace Preprocessor

#endif // FILE_MANAGER_HPP