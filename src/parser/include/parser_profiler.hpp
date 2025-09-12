#ifndef PARSER_PROFILER_HPP
#define PARSER_PROFILER_HPP

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <fstream>

namespace Parser {

// Performance metrics for a single operation
struct PerformanceMetric {
    std::string name;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    std::chrono::microseconds duration;
    size_t memoryUsed;
    size_t tokensProcessed;
    size_t nodesCreated;
    
    PerformanceMetric(const std::string& n) 
        : name(n), duration(0), memoryUsed(0), tokensProcessed(0), nodesCreated(0) {
        startTime = std::chrono::steady_clock::now();
    }
    
    void finish() {
        endTime = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    }
};

// Aggregated statistics for multiple operations of the same type
struct AggregatedStats {
    std::string operationName;
    size_t callCount;
    std::chrono::microseconds totalTime;
    std::chrono::microseconds minTime;
    std::chrono::microseconds maxTime;
    std::chrono::microseconds avgTime;
    size_t totalMemory;
    size_t totalTokens;
    size_t totalNodes;
    
    AggregatedStats(const std::string& name) 
        : operationName(name), callCount(0), totalTime(0), 
          minTime(std::chrono::microseconds::max()), maxTime(0),
          avgTime(0), totalMemory(0), totalTokens(0), totalNodes(0) {}
    
    void addMetric(const PerformanceMetric& metric) {
        callCount++;
        totalTime += metric.duration;
        totalMemory += metric.memoryUsed;
        totalTokens += metric.tokensProcessed;
        totalNodes += metric.nodesCreated;
        
        if (metric.duration < minTime) minTime = metric.duration;
        if (metric.duration > maxTime) maxTime = metric.duration;
        
        avgTime = callCount > 0 ? std::chrono::duration_cast<std::chrono::microseconds>(totalTime / callCount) : std::chrono::microseconds(0);
    }
};

// RAII class for automatic timing of operations
class ScopedTimer {
private:
    std::shared_ptr<PerformanceMetric> metric;
    class ParserProfiler* profiler;
    
public:
    ScopedTimer(const std::string& operationName, class ParserProfiler* prof);
    ~ScopedTimer();
    
    void setTokensProcessed(size_t count) { metric->tokensProcessed = count; }
    void setNodesCreated(size_t count) { metric->nodesCreated = count; }
    void setMemoryUsed(size_t bytes) { metric->memoryUsed = bytes; }
};

// Main profiler class for parser performance monitoring
class ParserProfiler {
private:
    std::unordered_map<std::string, AggregatedStats> aggregatedStats;
    std::vector<PerformanceMetric> recentMetrics;
    mutable std::mutex statsMutex;
    
    // Configuration
    bool enabled;
    size_t maxRecentMetrics;
    std::string logFilePath;
    std::ofstream logFile;
    
    // Memory tracking
    size_t peakMemoryUsage;
    size_t currentMemoryUsage;
    
public:
    explicit ParserProfiler(bool enable = true, size_t maxRecent = 1000)
        : enabled(enable), maxRecentMetrics(maxRecent), 
          peakMemoryUsage(0), currentMemoryUsage(0) {}
    
    ~ParserProfiler() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
    
    // Control methods
    void enable() { enabled = true; }
    void disable() { enabled = false; }
    bool isEnabled() const { return enabled; }
    
    // Timing methods
    ScopedTimer startTiming(const std::string& operationName) {
        return ScopedTimer(operationName, this);
    }
    
    void recordMetric(const PerformanceMetric& metric);
    
    // Statistics retrieval
    AggregatedStats getStats(const std::string& operationName) const;
    std::vector<AggregatedStats> getAllStats() const;
    std::vector<PerformanceMetric> getRecentMetrics(size_t count = 0) const;
    
    // Memory tracking
    void recordMemoryAllocation(size_t bytes);
    void recordMemoryDeallocation(size_t bytes);
    size_t getPeakMemoryUsage() const { return peakMemoryUsage; }
    size_t getCurrentMemoryUsage() const { return currentMemoryUsage; }
    
    // Reporting
    std::string generateReport() const;
    void printReport() const;
    void saveReportToFile(const std::string& filename) const;
    
    // Logging
    void enableLogging(const std::string& filename);
    void disableLogging();
    
    // Reset and cleanup
    void reset();
    void clearRecentMetrics();
    
    // Configuration
    void setMaxRecentMetrics(size_t max) { maxRecentMetrics = max; }
};

// Global profiler instance
extern std::unique_ptr<ParserProfiler> g_parserProfiler;

// Convenience macros for profiling
#define PROFILE_PARSER_OPERATION(name) \
    auto timer = Parser::g_parserProfiler ? Parser::g_parserProfiler->startTiming(name) : Parser::ScopedTimer("", nullptr)

#define PROFILE_TOKENS_PROCESSED(count) \
    if (Parser::g_parserProfiler && Parser::g_parserProfiler->isEnabled()) timer.setTokensProcessed(count)

#define PROFILE_NODES_CREATED(count) \
    if (Parser::g_parserProfiler && Parser::g_parserProfiler->isEnabled()) timer.setNodesCreated(count)

#define PROFILE_MEMORY_USED(bytes) \
    if (Parser::g_parserProfiler && Parser::g_parserProfiler->isEnabled()) timer.setMemoryUsed(bytes)

// Factory functions
std::unique_ptr<ParserProfiler> createParserProfiler(bool enabled = true);
void initializeGlobalProfiler(bool enabled = true);
void shutdownGlobalProfiler();

} // namespace Parser

#endif // PARSER_PROFILER_HPP