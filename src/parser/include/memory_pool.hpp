#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include "parser_types.hpp"
#include <memory>
#include <vector>
#include <atomic>

namespace Parser {

/**
 * Configuração básica para pools de memória
 */
struct PoolConfig {
    size_t initialSize;
    size_t maxSize;
    bool enableStats;
    
    PoolConfig() 
        : initialSize(64)
        , maxSize(1024)
        , enableStats(true) {}
};

/**
 * Estatísticas básicas de memória
 */
struct MemoryStatistics {
    size_t allocations = 0;
    size_t deallocations = 0;
    size_t currentUsed = 0;
    size_t peakUsed = 0;
};

/**
 * Pool básico para objetos - implementação header-only
 */
template<typename T>
class ObjectPool {
public:
    explicit ObjectPool(size_t initialSize = 32) {
        pool_.reserve(initialSize);
    }
    
    ~ObjectPool() {
        clear();
    }
    
    template<typename... Args>
    T* acquire(Args&&... args) {
        if (!pool_.empty()) {
            T* obj = pool_.back();
            pool_.pop_back();
            // Reconstrói o objeto
            new(obj) T(std::forward<Args>(args)...);
            stats_.allocations++;
            stats_.currentUsed++;
            if (stats_.currentUsed > stats_.peakUsed) {
                stats_.peakUsed = stats_.currentUsed;
            }
            return obj;
        }
        
        // Cria novo objeto se pool vazio
        T* obj = new T(std::forward<Args>(args)...);
        stats_.allocations++;
        stats_.currentUsed++;
        if (stats_.currentUsed > stats_.peakUsed) {
            stats_.peakUsed = stats_.currentUsed;
        }
        return obj;
    }
    
    void release(T* obj) {
        if (!obj) return;
        
        obj->~T();
        pool_.push_back(obj);
        stats_.deallocations++;
        if (stats_.currentUsed > 0) {
            stats_.currentUsed--;
        }
    }
    
    void clear() {
        for (T* obj : pool_) {
            delete obj;
        }
        pool_.clear();
    }
    
    const MemoryStatistics& getStats() const {
        return stats_;
    }
    
private:
    std::vector<T*> pool_;
    MemoryStatistics stats_;
};

/**
 * Factory simples para criação de nós AST
 */
class ASTNodeFactory {
public:
    ASTNodeFactory() = default;
    ~ASTNodeFactory() = default;
    
    // Cria nó usando alocação padrão
    template<typename T, typename... Args>
    std::unique_ptr<T> createNode(Args&&... args) {
        nodeCount_++;
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
    
    // Obtém estatísticas básicas
    MemoryStatistics getStatistics() const {
        MemoryStatistics stats;
        stats.allocations = nodeCount_.load();
        return stats;
    }
    
private:
    std::atomic<size_t> nodeCount_{0};
};

} // namespace Parser

#endif // MEMORY_POOL_HPP