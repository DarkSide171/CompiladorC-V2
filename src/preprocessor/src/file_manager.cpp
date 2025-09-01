// File Manager - Implementação do gerenciador de arquivos
// Implementação da classe FileManager para gerenciamento de inclusão de arquivos

#include "../include/file_manager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <dirent.h>

namespace Preprocessor {

// ============================================================================
// CONSTRUTORES E DESTRUTOR
// ============================================================================

FileManager::FileManager(const std::vector<std::string>& search_paths, PreprocessorLogger* logger)
    : search_paths_(search_paths), logger_(logger),
      max_cache_size_(50 * 1024 * 1024), // 50MB padrão
      max_cache_entries_(1000),
      cache_ttl_(std::chrono::seconds(300)), // 5 minutos
      enable_cache_compression_(false),
      external_error_handler_(nullptr) {
    // Normaliza os caminhos de busca
    for (auto& path : search_paths_) {
        path = normalizeFilePath(path);
    }
    
    if (logger_) {
        logInfo("FileManager inicializado com " + std::to_string(search_paths_.size()) + " caminhos de busca");
        logInfo("Cache configurado: " + std::to_string(max_cache_size_ / (1024*1024)) + "MB, " + 
                std::to_string(max_cache_entries_) + " entradas, TTL: " + 
                std::to_string(cache_ttl_.count()) + "s");
    }
}

FileManager::~FileManager() {
    if (logger_) {
        logInfo("FileManager destruído. Estatísticas finais: " + 
                std::to_string(stats_.files_read) + " arquivos lidos, " +
                std::to_string(stats_.cache_hits) + " cache hits");
    }
}

FileManager::FileManager(FileManager&& other) noexcept
    : search_paths_(std::move(other.search_paths_)),
      file_cache_(std::move(other.file_cache_)),
      dependencies_(std::move(other.dependencies_)),
      circular_detection_set_(std::move(other.circular_detection_set_)),
      logger_(other.logger_),
      stats_(other.stats_),
      max_cache_size_(other.max_cache_size_),
      max_cache_entries_(other.max_cache_entries_),
      cache_ttl_(other.cache_ttl_),
      enable_cache_compression_(other.enable_cache_compression_),
      external_error_handler_(other.external_error_handler_) {
    other.logger_ = nullptr;
    other.stats_.reset();
}

FileManager& FileManager::operator=(FileManager&& other) noexcept {
    if (this != &other) {
        search_paths_ = std::move(other.search_paths_);
        file_cache_ = std::move(other.file_cache_);
        dependencies_ = std::move(other.dependencies_);
        circular_detection_set_ = std::move(other.circular_detection_set_);
        logger_ = other.logger_;
        stats_ = other.stats_;
        
        other.logger_ = nullptr;
        other.stats_.reset();
    }
    return *this;
}

// ============================================================================
// OPERAÇÕES BÁSICAS DE ARQUIVO
// ============================================================================

std::string FileManager::readFile(const std::string& filepath) {
    if (filepath.empty()) {
        throw std::runtime_error("Caminho de arquivo vazio");
    }
    
    // Normaliza o caminho
    std::string normalized_path = normalizeFilePath(filepath);
    
    // Verifica cache primeiro
    const CachedFile* cached = getCachedFile(normalized_path);
    if (cached) {
        stats_.cache_hits++;
        if (logger_) {
            logInfo("Arquivo lido do cache: " + normalized_path);
        }
        return cached->content;
    }
    
    stats_.cache_misses++;
    
    // Verifica se o arquivo existe
    if (!fileExists(normalized_path)) {
        throw std::runtime_error("Arquivo não encontrado: " + normalized_path);
    }
    
    // Lê o arquivo
    std::ifstream file(normalized_path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Não foi possível abrir o arquivo: " + normalized_path);
    }
    
    // Lê todo o conteúdo
    std::ostringstream content_stream;
    content_stream << file.rdbuf();
    std::string content = content_stream.str();
    
    file.close();
    
    // Atualiza estatísticas
    stats_.files_read++;
    stats_.total_bytes_read += content.size();
    
    // Armazena no cache
    cacheFile(normalized_path, content);
    
    // Atualiza dependências
    updateDependencies(normalized_path);
    
    if (logger_) {
        logInfo("Arquivo lido do disco: " + normalized_path + " (" + 
                std::to_string(content.size()) + " bytes)");
    }
    
    return content;
}

bool FileManager::writeFile(const std::string& filepath, const std::string& content) {
    if (filepath.empty()) {
        logError("[writeFile] Caminho de arquivo vazio para escrita");
        return false;
    }
    
    std::string normalized_path = normalizeFilePath(filepath);
    
    try {
         // Cria diretórios se necessário
         size_t last_slash = normalized_path.find_last_of("/\\");
         if (last_slash != std::string::npos) {
             std::string dir_path = normalized_path.substr(0, last_slash);
             
             struct stat st;
             if (stat(dir_path.c_str(), &st) != 0) {
                 // Cria diretório recursivamente (implementação simples)
                 std::string cmd = "mkdir -p \"" + dir_path + "\"";
                 system(cmd.c_str());
             }
         }
        
        // Escreve o arquivo
        std::ofstream file(normalized_path, std::ios::binary);
        if (!file.is_open()) {
            logError("[writeFile] Não foi possível criar o arquivo", normalized_path);
            return false;
        }
        
        file << content;
        file.close();
        
        // Atualiza cache se o arquivo já estava em cache
        if (file_cache_.find(normalized_path) != file_cache_.end()) {
            cacheFile(normalized_path, content);
        }
        
        // Remove hash do cache também
        file_hashes_.erase(normalized_path);
        
        if (logger_) {
            logInfo("Arquivo escrito: " + normalized_path + " (" + 
                    std::to_string(content.size()) + " bytes)");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        logError("[writeFile] Erro ao escrever arquivo: " + std::string(e.what()), normalized_path);
        return false;
    }
}

bool FileManager::fileExists(const std::string& filepath) const {
    if (filepath.empty()) {
        return false;
    }
    
    std::string normalized_path = normalizeFilePath(filepath);
    
    struct stat st;
     if (stat(normalized_path.c_str(), &st) == 0) {
         return S_ISREG(st.st_mode);
     }
     return false;
}

// ============================================================================
// RESOLUÇÃO DE INCLUSÕES
// ============================================================================

std::string FileManager::resolveInclude(const std::string& filename, 
                                       bool is_system, 
                                       const std::string& current_file) {
    if (filename.empty()) {
        logError("[resolveInclude] Nome de arquivo vazio para inclusão");
        throw std::runtime_error("Nome de arquivo vazio para inclusão");
    }
    
    if (logger_) {
        logInfo("[DEBUG] resolveInclude chamado para: " + filename + ", is_system: " + (is_system ? "true" : "false"));
        logInfo("[DEBUG] search_paths_ tem " + std::to_string(search_paths_.size()) + " caminhos");
    }
    
    stats_.path_resolutions++;
    
    std::string resolved_path;
    
    if (is_system) {
        // Para inclusões de sistema (<>), busca apenas nos caminhos de sistema
        if (logger_) {
            logInfo("[DEBUG] Buscando include de sistema nos caminhos configurados");
        }
        resolved_path = searchInPaths(filename, search_paths_);
    } else {
        // Para inclusões locais (""), busca primeiro no diretório do arquivo atual
         if (!current_file.empty()) {
             size_t last_slash = current_file.find_last_of("/\\");
             std::string current_dir;
             if (last_slash != std::string::npos) {
                 current_dir = current_file.substr(0, last_slash);
             }
            
            std::string local_path = resolveRelativePath(filename, current_dir);
            if (fileExists(local_path)) {
                resolved_path = local_path;
            }
        }
        
        // Se não encontrou localmente, busca nos caminhos de busca
        if (resolved_path.empty()) {
            resolved_path = searchInPaths(filename, search_paths_);
        }
    }
    
    if (resolved_path.empty()) {
        logError("[resolveInclude] Arquivo de inclusão não encontrado", filename);
        throw std::runtime_error("Arquivo de inclusão não encontrado: " + filename);
    }
    
    resolved_path = normalizeFilePath(resolved_path);
    
    if (logger_) {
        logInfo("Inclusão resolvida: " + filename + " -> " + resolved_path);
    }
    
    return resolved_path;
}

// ============================================================================
// GERENCIAMENTO DE CAMINHOS DE BUSCA
// ============================================================================

void FileManager::addSearchPath(const std::string& path) {
    if (path.empty()) {
        logWarning("Tentativa de adicionar caminho de busca vazio");
        return;
    }
    
    std::string normalized_path = normalizeFilePath(path);
    
    // Verifica se o caminho já existe
    auto it = std::find(search_paths_.begin(), search_paths_.end(), normalized_path);
    if (it == search_paths_.end()) {
        search_paths_.push_back(normalized_path);
        
        if (logger_) {
            logInfo("Caminho de busca adicionado: " + normalized_path);
        }
    }
}

void FileManager::setSearchPaths(const std::vector<std::string>& paths) {
    search_paths_.clear();
    
    for (const auto& path : paths) {
        if (!path.empty()) {
            search_paths_.push_back(normalizeFilePath(path));
        }
    }
    
    if (logger_) {
        logInfo("Caminhos de busca redefinidos: " + std::to_string(search_paths_.size()) + " caminhos");
    }
}

std::vector<std::string> FileManager::getSearchPaths() const {
    return search_paths_;
}

// ============================================================================
// DETECÇÃO DE INCLUSÕES CIRCULARES
// ============================================================================

bool FileManager::checkCircularInclusion(const std::string& filepath, 
                                        const std::vector<std::string>& include_stack) {
    if (filepath.empty()) {
        return false;
    }
    
    std::string normalized_path = normalizeFilePath(filepath);
    
    // Verifica se o arquivo já está na pilha de inclusões
    for (const auto& included_file : include_stack) {
        if (normalizeFilePath(included_file) == normalized_path) {
            stats_.circular_inclusions++;
            
            if (logger_) {
                logWarning("[checkCircularInclusion] Inclusão circular detectada", normalized_path);
                logWarning("[checkCircularInclusion] Pilha de inclusões:");
                for (size_t i = 0; i < include_stack.size(); ++i) {
                    logWarning("[checkCircularInclusion]   [" + std::to_string(i) + "] " + include_stack[i]);
                }
            }
            
            return true;
        }
    }
    
    return false;
}

// ============================================================================
// SISTEMA DE CACHE
// ============================================================================

void FileManager::clearCache() {
    size_t cached_files = file_cache_.size();
    file_cache_.clear();
    
    if (logger_) {
        logInfo("Cache limpo: " + std::to_string(cached_files) + " arquivos removidos");
    }
}

// ============================================================================
// GERENCIAMENTO DE DEPENDÊNCIAS
// ============================================================================

std::vector<std::string> FileManager::getDependencies() const {
    std::vector<std::string> all_dependencies;
    
    for (const auto& [filepath, dependency] : dependencies_) {
        all_dependencies.push_back(filepath);
        
        for (const auto& dep : dependency.dependencies) {
            all_dependencies.push_back(dep);
        }
    }
    
    // Remove duplicatas
    std::sort(all_dependencies.begin(), all_dependencies.end());
    all_dependencies.erase(
        std::unique(all_dependencies.begin(), all_dependencies.end()),
        all_dependencies.end()
    );
    
    return all_dependencies;
}

// ============================================================================
// INFORMAÇÕES DE ARQUIVO
// ============================================================================

size_t FileManager::getFileSize(const std::string& filepath) const {
    if (filepath.empty() || !fileExists(filepath)) {
        return 0;
    }
    
    struct stat st;
     if (stat(filepath.c_str(), &st) == 0) {
         return static_cast<size_t>(st.st_size);
     }
     return 0;
}

std::chrono::system_clock::time_point FileManager::getLastModified(const std::string& filepath) const {
    if (filepath.empty() || !fileExists(filepath)) {
        return std::chrono::system_clock::time_point{};
    }
    
    struct stat st;
     if (stat(filepath.c_str(), &st) == 0) {
         return std::chrono::system_clock::from_time_t(st.st_mtime);
     }
     return std::chrono::system_clock::time_point{};
}

// ============================================================================
// ESTATÍSTICAS
// ============================================================================

FileStats FileManager::getStatistics() const {
    return stats_;
}

void FileManager::resetStatistics() {
    stats_.reset();
    
    if (logger_) {
        logInfo("Estatísticas do FileManager resetadas");
    }
}

// ============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS DE BACKUP E RESTAURAÇÃO
// ============================================================================

bool FileManager::createBackup(const std::string& filepath, const std::string& backup_suffix) {
    try {
        std::string normalized_path = normalizeFilePath(filepath);
        if (!fileExists(normalized_path)) {
            logError("Arquivo não encontrado para backup: " + normalized_path);
            return false;
        }
        
        std::string backup_path = normalized_path + backup_suffix;
        
        // Lê o conteúdo do arquivo original
        std::string content = readFile(normalized_path);
        if (content.empty() && !fileExists(normalized_path)) {
            return false;
        }
        
        // Escreve o backup
        if (!writeFile(backup_path, content)) {
            logError("Falha ao criar backup: " + backup_path);
            return false;
        }
        
        logInfo("Backup criado: " + backup_path);
        return true;
        
    } catch (const std::exception& e) {
        logError("Erro ao criar backup: " + std::string(e.what()));
        return false;
    }
}

bool FileManager::restoreFromBackup(const std::string& filepath, const std::string& backup_suffix) {
    try {
        std::string normalized_path = normalizeFilePath(filepath);
        std::string backup_path = normalized_path + backup_suffix;
        
        if (!fileExists(backup_path)) {
            logError("Arquivo de backup não encontrado: " + backup_path);
            return false;
        }
        
        // Lê o conteúdo do backup
        std::string content = readFile(backup_path);
        if (content.empty() && !fileExists(backup_path)) {
            return false;
        }
        
        // Restaura o arquivo original
        if (!writeFile(normalized_path, content)) {
            logError("Falha ao restaurar do backup: " + normalized_path);
            return false;
        }
        
        // Remove do cache para forçar releitura
        file_cache_.erase(normalized_path);
        
        logInfo("Arquivo restaurado do backup: " + normalized_path);
        return true;
        
    } catch (const std::exception& e) {
        logError("Erro ao restaurar do backup: " + std::string(e.what()));
        return false;
    }
}

// ============================================================================
// IMPLEMENTAÇÃO DO CONTROLE DE ACESSO A ARQUIVOS
// ============================================================================

bool FileManager::lockFile(const std::string& filepath) {
    try {
        std::string normalized_path = normalizeFilePath(filepath);
        
        if (!fileExists(normalized_path)) {
            logError("Arquivo não encontrado para bloqueio: " + normalized_path);
            return false;
        }
        
        if (locked_files_.find(normalized_path) != locked_files_.end()) {
            logWarning("Arquivo já está bloqueado: " + normalized_path);
            return false;
        }
        
        locked_files_.insert(normalized_path);
        logInfo("Arquivo bloqueado: " + normalized_path);
        return true;
        
    } catch (const std::exception& e) {
        logError("Erro ao bloquear arquivo: " + std::string(e.what()));
        return false;
    }
}

bool FileManager::unlockFile(const std::string& filepath) {
    try {
        std::string normalized_path = normalizeFilePath(filepath);
        
        auto it = locked_files_.find(normalized_path);
        if (it == locked_files_.end()) {
            logWarning("Arquivo não está bloqueado: " + normalized_path);
            return false;
        }
        
        locked_files_.erase(it);
        logInfo("Arquivo desbloqueado: " + normalized_path);
        return true;
        
    } catch (const std::exception& e) {
        logError("Erro ao desbloquear arquivo: " + std::string(e.what()));
        return false;
    }
}

// ============================================================================
// IMPLEMENTAÇÃO DA COMPRESSÃO DE ARQUIVOS
// ============================================================================

bool FileManager::compressFile(const std::string& filepath, const std::string& compressed_path) {
    try {
        std::string normalized_path = normalizeFilePath(filepath);
        
        if (!fileExists(normalized_path)) {
            logError("Arquivo não encontrado para compressão: " + normalized_path);
            return false;
        }
        
        std::string output_path = compressed_path.empty() ? 
            normalized_path + ".gz" : compressed_path;
        
        // Lê o conteúdo do arquivo
        std::string content = readFile(normalized_path);
        if (content.empty() && !fileExists(normalized_path)) {
            return false;
        }
        
        // Simulação de compressão (implementação básica)
        // Em uma implementação real, usaria zlib ou similar
        std::string compressed_content = "COMPRESSED:" + content;
        
        if (!writeFile(output_path, compressed_content)) {
            logError("Falha ao escrever arquivo comprimido: " + output_path);
            return false;
        }
        
        logInfo("Arquivo comprimido: " + output_path);
        return true;
        
    } catch (const std::exception& e) {
        logError("Erro ao comprimir arquivo: " + std::string(e.what()));
        return false;
    }
}

bool FileManager::decompressFile(const std::string& compressed_path, const std::string& output_path) {
    try {
        std::string normalized_compressed = normalizeFilePath(compressed_path);
        
        if (!fileExists(normalized_compressed)) {
            logError("Arquivo comprimido não encontrado: " + normalized_compressed);
            return false;
        }
        
        // Lê o arquivo comprimido
        std::string compressed_content = readFile(normalized_compressed);
        if (compressed_content.empty()) {
            return false;
        }
        
        // Simulação de descompressão
        if (compressed_content.substr(0, 11) != "COMPRESSED:") {
            logError("Formato de arquivo comprimido inválido");
            return false;
        }
        
        std::string decompressed_content = compressed_content.substr(11);
        
        std::string final_output_path = output_path.empty() ? 
            normalized_compressed.substr(0, normalized_compressed.find_last_of('.')) : output_path;
        
        if (!writeFile(final_output_path, decompressed_content)) {
            logError("Falha ao escrever arquivo descomprimido: " + final_output_path);
            return false;
        }
        
        logInfo("Arquivo descomprimido: " + final_output_path);
        return true;
        
    } catch (const std::exception& e) {
        logError("Erro ao descomprimir arquivo: " + std::string(e.what()));
        return false;
    }
}

// ============================================================================
// IMPLEMENTAÇÃO DA VERIFICAÇÃO DE INTEGRIDADE
// ============================================================================

std::string FileManager::calculateFileHash(const std::string& filepath) const {
    try {
        std::string normalized_path = normalizeFilePath(filepath);
        
        if (!fileExists(normalized_path)) {
            logError("Arquivo não encontrado para cálculo de hash: " + normalized_path);
            return "";
        }
        
        // Verifica se já temos o hash em cache
        auto it = file_hashes_.find(normalized_path);
        if (it != file_hashes_.end()) {
            return it->second;
        }
        
        // Lê o conteúdo do arquivo usando std::ifstream diretamente
        std::ifstream file(normalized_path, std::ios::binary);
        if (!file.is_open()) {
            logError("Não foi possível abrir arquivo para hash: " + normalized_path);
            return "";
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        // Simulação de hash SHA-256 (implementação básica)
        // Em uma implementação real, usaria OpenSSL ou similar
        std::hash<std::string> hasher;
        size_t hash_value = hasher(content);
        
        std::stringstream ss;
        ss << std::hex << hash_value;
        std::string hash_result = ss.str();
        
        // Armazena no cache (cast para remover const)
        const_cast<FileManager*>(this)->file_hashes_[normalized_path] = hash_result;
        
        logInfo("Hash calculado para " + normalized_path + ": " + hash_result);
        return hash_result;
        
    } catch (const std::exception& e) {
        logError("Erro ao calcular hash: " + std::string(e.what()));
        return "";
    }
}

bool FileManager::verifyFileIntegrity(const std::string& filepath, const std::string& expected_hash) const {
    try {
        std::string normalized_path = normalizeFilePath(filepath);
        
        if (!fileExists(normalized_path)) {
            logError("Arquivo não encontrado para verificação: " + normalized_path);
            return false;
        }
        
        if (expected_hash.empty()) {
            logError("Hash esperado não pode estar vazio");
            return false;
        }
        
        std::string current_hash = calculateFileHash(normalized_path);
        if (current_hash.empty()) {
            return false;
        }
        
        bool integrity_ok = (current_hash == expected_hash);
        
        if (integrity_ok) {
            logInfo("Integridade verificada com sucesso: " + normalized_path);
        } else {
            logError("Falha na verificação de integridade: " + normalized_path + 
                    " (esperado: " + expected_hash + ", atual: " + current_hash + ")");
        }
        
        return integrity_ok;
        
    } catch (const std::exception& e) {
        logError("Erro ao verificar integridade: " + std::string(e.what()));
        return false;
    }
}

// ============================================================================
// IMPLEMENTAÇÃO DO MONITORAMENTO DE ARQUIVOS
// ============================================================================

bool FileManager::monitorFileChanges(const std::string& filepath) {
    try {
        std::string normalized_path = normalizeFilePath(filepath);
        
        if (!fileExists(normalized_path)) {
            logError("Arquivo não encontrado para monitoramento: " + normalized_path);
            return false;
        }
        
        if (monitored_files_.find(normalized_path) != monitored_files_.end()) {
            logWarning("Arquivo já está sendo monitorado: " + normalized_path);
            return true;
        }
        
        monitored_files_.insert(normalized_path);
        
        // Calcula hash inicial para detectar mudanças futuras
        calculateFileHash(normalized_path);
        
        logInfo("Monitoramento iniciado para: " + normalized_path);
        return true;
        
    } catch (const std::exception& e) {
        logError("Erro ao iniciar monitoramento: " + std::string(e.what()));
        return false;
    }
}

void FileManager::handleFileSystemEvents(const std::string& event_type, const std::string& filepath) {
    try {
        std::string normalized_path = normalizeFilePath(filepath);
        
        logInfo("Evento do sistema de arquivos: " + event_type + " em " + normalized_path);
        
        if (event_type == "MODIFIED" || event_type == "CHANGED") {
            // Remove do cache para forçar releitura
            file_cache_.erase(normalized_path);
            file_hashes_.erase(normalized_path);
            
            // Se está sendo monitorado, recalcula o hash
            if (monitored_files_.find(normalized_path) != monitored_files_.end()) {
                calculateFileHash(normalized_path);
            }
            
            logInfo("Cache invalidado para arquivo modificado: " + normalized_path);
            
        } else if (event_type == "DELETED" || event_type == "REMOVED") {
            // Remove todas as referências ao arquivo
            file_cache_.erase(normalized_path);
            file_hashes_.erase(normalized_path);
            locked_files_.erase(normalized_path);
            monitored_files_.erase(normalized_path);
            dependencies_.erase(normalized_path);
            
            logInfo("Referências removidas para arquivo deletado: " + normalized_path);
            
        } else if (event_type == "CREATED" || event_type == "ADDED") {
            logInfo("Novo arquivo detectado: " + normalized_path);
            
        } else {
            logWarning("Tipo de evento desconhecido: " + event_type);
        }
        
    } catch (const std::exception& e) {
        logError("Erro ao processar evento do sistema: " + std::string(e.what()));
    }
}

// ============================================================================
// MÉTODOS AUXILIARES PRIVADOS
// ============================================================================

std::string FileManager::searchInPaths(const std::string& filename, 
                                      const std::vector<std::string>& paths) const {
    if (logger_) {
        logInfo("Buscando arquivo: " + filename + " em " + std::to_string(paths.size()) + " caminhos");
    }
    
    for (const auto& search_path : paths) {
        std::string full_path = resolveRelativePath(filename, search_path);
        
        if (logger_) {
            logInfo("Testando caminho: " + full_path);
        }
        
        if (fileExists(full_path)) {
            if (logger_) {
                logInfo("Arquivo encontrado em: " + full_path);
            }
            return full_path;
        }
    }
    
    if (logger_) {
        logError("Arquivo não encontrado: " + filename);
    }
    return ""; // Não encontrado
}

std::string FileManager::resolveRelativePath(const std::string& filename, 
                                            const std::string& base_path) const {
    if (filename.empty()) {
        return "";
    }
    
    // Verifica se o filename é um caminho absoluto
     if (!filename.empty() && (filename[0] == '/' || 
         (filename.length() > 1 && filename[1] == ':'))) {
         return filename;
     }
     
     // Fallback para concatenação simples
      if (base_path.empty()) {
          return filename;
      }
      
      std::string result = base_path;
      if (result.back() != '/' && result.back() != '\\') {
          result += "/";
      }
      result += filename;
      
      return result;
}

void FileManager::cacheFile(const std::string& filepath, const std::string& content,
                           std::chrono::system_clock::time_point file_modified) {
    std::string normalized_path = normalizeFilePath(filepath);
    
    // Verifica se precisa otimizar cache antes de adicionar
    if (file_cache_.size() >= max_cache_entries_ || getCurrentCacheSize() >= max_cache_size_) {
        optimizeCache();
    }
    
    CachedFile cached_file(content, content.size());
    cached_file.normalized_path = normalized_path;
    cached_file.last_modified = file_modified;
    
    // Calcula hash se necessário
    if (!cached_file.file_hash.empty() || enable_cache_compression_) {
        cached_file.file_hash = calculateFileHash(filepath);
    }
    
    file_cache_[normalized_path] = std::move(cached_file);
    stats_.files_cached++;
    
    if (logger_) {
        logInfo("Arquivo cacheado: " + normalized_path + " (" + 
                std::to_string(content.size()) + " bytes)");
    }
}

const CachedFile* FileManager::getCachedFile(const std::string& filepath) const {
    std::string normalized_path = normalizeFilePath(filepath);
    
    auto it = file_cache_.find(normalized_path);
    if (it != file_cache_.end()) {
        // Verifica se o cache expirou
        if (it->second.isExpired(cache_ttl_)) {
            if (logger_) {
                logInfo("Cache expirado para: " + normalized_path);
            }
            return nullptr;
        }
        
        // Verifica se o arquivo foi modificado
        if (shouldInvalidateCache(normalized_path)) {
            if (logger_) {
                logInfo("Cache invalidado (arquivo modificado): " + normalized_path);
            }
            return nullptr;
        }
        
        // Atualiza estatísticas de acesso
        const_cast<CachedFile&>(it->second).updateAccess();
        return &it->second;
    }
    
    return nullptr;
}

bool FileManager::validateFilePath(const std::string& filepath) const {
    if (filepath.empty()) {
        return false;
    }
    
    // Verifica caracteres inválidos básicos
    const std::string invalid_chars = "<>:|?*";
    for (char c : invalid_chars) {
        if (filepath.find(c) != std::string::npos) {
            return false;
        }
    }
    
    // Verifica se não é muito longo (limite do sistema)
    if (filepath.length() > 4096) {
        return false;
    }
    
    return true;
}

std::string FileManager::normalizeFilePath(const std::string& filepath) const {
    if (filepath.empty()) {
        return "";
    }
     
    // Fallback para normalização manual básica
    std::string normalized = filepath;
      
    // Substitui barras invertidas por barras normais
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
      
    // Remove barras duplas
    size_t pos = 0;
    while ((pos = normalized.find("//", pos)) != std::string::npos) {
        normalized.replace(pos, 2, "/");
    }
      
    return normalized;
}

void FileManager::updateDependencies(const std::string& filepath) {
    std::string normalized_path = normalizeFilePath(filepath);
    
    // Cria ou atualiza entrada de dependência
    if (dependencies_.find(normalized_path) == dependencies_.end()) {
        dependencies_[normalized_path] = FileDependency(normalized_path);
        stats_.dependency_updates++;
    }
    
    // Atualiza timestamp
    dependencies_[normalized_path].last_modified = std::chrono::system_clock::now();
}

void FileManager::logError(const std::string& message, const std::string& filepath) const {
    if (logger_) {
        std::string context_message = "[FILE_MANAGER::FileManager] " + message;
        if (filepath.empty()) {
            logger_->error(context_message);
        } else {
            logger_->error(context_message + " [" + filepath + "]");
        }
    }
}

void FileManager::logWarning(const std::string& message, const std::string& filepath) const {
    if (logger_) {
        std::string context_message = "[FILE_MANAGER::FileManager] " + message;
        if (filepath.empty()) {
            logger_->warning(context_message);
        } else {
            logger_->warning(context_message + " [" + filepath + "]");
        }
    }
}

void FileManager::logInfo(const std::string& message, const std::string& filepath) const {
    if (logger_) {
        if (filepath.empty()) {
            logger_->info(message);
        } else {
            logger_->info(message + " [" + filepath + "]");
        }
    }
}

// ============================================================================
// MÉTODOS DE OTIMIZAÇÃO DE CACHE
// ============================================================================

void FileManager::configureCacheOptimization(size_t max_size, size_t max_entries,
                                            std::chrono::seconds ttl, bool enable_compression) {
    max_cache_size_ = max_size;
    max_cache_entries_ = max_entries;
    cache_ttl_ = ttl;
    enable_cache_compression_ = enable_compression;
    
    if (logger_) {
        logInfo("Cache reconfigurado: " + std::to_string(max_size / (1024*1024)) + "MB, " + 
                std::to_string(max_entries) + " entradas, TTL: " + std::to_string(ttl.count()) + "s");
    }
    
    // Otimiza cache com novas configurações
    optimizeCache();
}

void FileManager::optimizeCache() {
    size_t initial_size = file_cache_.size();
    size_t initial_memory = getCurrentCacheSize();
    
    // Remove entradas expiradas
    auto it = file_cache_.begin();
    while (it != file_cache_.end()) {
        if (it->second.isExpired(cache_ttl_) || shouldInvalidateCache(it->first)) {
            it = file_cache_.erase(it);
        } else {
            ++it;
        }
    }
    
    // Se ainda excede limites, remove entradas menos usadas
    if (file_cache_.size() > max_cache_entries_ || getCurrentCacheSize() > max_cache_size_) {
        evictLeastRecentlyUsed(max_cache_entries_ * 0.8); // Remove 20% das entradas
    }
    
    size_t final_size = file_cache_.size();
    size_t final_memory = getCurrentCacheSize();
    
    if (logger_ && (initial_size != final_size)) {
        logInfo("Cache otimizado: " + std::to_string(initial_size) + " -> " + 
                std::to_string(final_size) + " entradas, " + 
                std::to_string(initial_memory / 1024) + " -> " + 
                std::to_string(final_memory / 1024) + " KB");
    }
}

void FileManager::preloadFiles(const std::vector<std::string>& filepaths) {
    for (const auto& filepath : filepaths) {
        try {
            if (fileExists(filepath) && !getCachedFile(filepath)) {
                std::string content = readFile(filepath);
                auto last_modified = getLastModified(filepath);
                cacheFile(filepath, content, last_modified);
                
                if (logger_) {
                    logInfo("Arquivo pré-carregado: " + filepath);
                }
            }
        } catch (const std::exception& e) {
            if (logger_) {
                logWarning("Falha ao pré-carregar arquivo: " + filepath + " - " + e.what());
            }
        }
    }
}

bool FileManager::shouldInvalidateCache(const std::string& filepath) const {
    auto it = file_cache_.find(normalizeFilePath(filepath));
    if (it == file_cache_.end()) {
        return false;
    }
    
    try {
        auto current_modified = getLastModified(filepath);
        return current_modified > it->second.last_modified;
    } catch (const std::exception&) {
        // Se não conseguir verificar, assume que deve invalidar
        return true;
    }
}

void FileManager::evictLeastRecentlyUsed(size_t target_size) {
    if (file_cache_.size() <= target_size) {
        return;
    }
    
    // Cria vetor com entradas ordenadas por último acesso
    std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> entries;
    for (const auto& pair : file_cache_) {
        entries.emplace_back(pair.second.last_access, pair.first);
    }
    
    // Ordena por último acesso (mais antigos primeiro)
    std::sort(entries.begin(), entries.end());
    
    // Remove entradas mais antigas até atingir tamanho alvo
    size_t to_remove = file_cache_.size() - target_size;
    for (size_t i = 0; i < to_remove && i < entries.size(); ++i) {
        file_cache_.erase(entries[i].second);
    }
}

size_t FileManager::getCurrentCacheSize() const {
    size_t total_size = 0;
    for (const auto& pair : file_cache_) {
        total_size += pair.second.content.size();
        total_size += pair.second.normalized_path.size();
        total_size += pair.second.file_hash.size();
        total_size += sizeof(CachedFile); // Overhead da estrutura
    }
    return total_size;
}

void FileManager::setErrorHandler(void* errorHandler) {
    external_error_handler_ = errorHandler;
}

} // namespace Preprocessor