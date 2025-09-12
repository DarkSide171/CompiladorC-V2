#include "../include/parser_profiler.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Parser {

// Global profiler instance
std::unique_ptr<ParserProfiler> g_parserProfiler = nullptr;

// ScopedTimer implementation
ScopedTimer::ScopedTimer(const std::string& operationName, ParserProfiler* prof)
    : profiler(prof) {
    if (profiler && profiler->isEnabled()) {
        metric = std::make_shared<PerformanceMetric>(operationName);
    }
}

ScopedTimer::~ScopedTimer() {
    if (profiler && profiler->isEnabled() && metric) {
        metric->finish();
        profiler->recordMetric(*metric);
    }
}

// ParserProfiler implementation
void ParserProfiler::recordMetric(const PerformanceMetric& metric) {
    if (!enabled) return;
    
    std::lock_guard<std::mutex> lock(statsMutex);
    
    // Add to recent metrics
    recentMetrics.push_back(metric);
    if (recentMetrics.size() > maxRecentMetrics) {
        recentMetrics.erase(recentMetrics.begin());
    }
    
    // Update aggregated stats
    auto it = aggregatedStats.find(metric.name);
    if (it == aggregatedStats.end()) {
        aggregatedStats.emplace(metric.name, AggregatedStats(metric.name));
        it = aggregatedStats.find(metric.name);
    }
    it->second.addMetric(metric);
    
    // Log to file if enabled
    if (logFile.is_open()) {
        logFile << std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now().time_since_epoch()).count()
                << "," << metric.name
                << "," << metric.duration.count()
                << "," << metric.memoryUsed
                << "," << metric.tokensProcessed
                << "," << metric.nodesCreated << std::endl;
    }
}

AggregatedStats ParserProfiler::getStats(const std::string& operationName) const {
    std::lock_guard<std::mutex> lock(statsMutex);
    
    auto it = aggregatedStats.find(operationName);
    if (it != aggregatedStats.end()) {
        return it->second;
    }
    return AggregatedStats(operationName);
}

std::vector<AggregatedStats> ParserProfiler::getAllStats() const {
    std::lock_guard<std::mutex> lock(statsMutex);
    
    std::vector<AggregatedStats> result;
    result.reserve(aggregatedStats.size());
    
    for (const auto& pair : aggregatedStats) {
        result.push_back(pair.second);
    }
    
    // Sort by total time (descending)
    std::sort(result.begin(), result.end(),
             [](const AggregatedStats& a, const AggregatedStats& b) {
                 return a.totalTime > b.totalTime;
             });
    
    return result;
}

std::vector<PerformanceMetric> ParserProfiler::getRecentMetrics(size_t count) const {
    std::lock_guard<std::mutex> lock(statsMutex);
    
    if (count == 0 || count >= recentMetrics.size()) {
        return recentMetrics;
    }
    
    return std::vector<PerformanceMetric>(
        recentMetrics.end() - count, recentMetrics.end());
}

void ParserProfiler::recordMemoryAllocation(size_t bytes) {
    if (!enabled) return;
    
    std::lock_guard<std::mutex> lock(statsMutex);
    currentMemoryUsage += bytes;
    if (currentMemoryUsage > peakMemoryUsage) {
        peakMemoryUsage = currentMemoryUsage;
    }
}

void ParserProfiler::recordMemoryDeallocation(size_t bytes) {
    if (!enabled) return;
    
    std::lock_guard<std::mutex> lock(statsMutex);
    if (currentMemoryUsage >= bytes) {
        currentMemoryUsage -= bytes;
    }
}

std::string ParserProfiler::generateReport() const {
    std::lock_guard<std::mutex> lock(statsMutex);
    
    std::ostringstream report;
    report << "\n=== Parser Performance Report ===\n\n";
    
    // Memory statistics
    report << "Memory Usage:\n";
    report << "  Current: " << (currentMemoryUsage / 1024.0 / 1024.0) << " MB\n";
    report << "  Peak: " << (peakMemoryUsage / 1024.0 / 1024.0) << " MB\n\n";
    
    // Operation statistics
    report << "Operation Statistics:\n";
    report << std::left << std::setw(25) << "Operation"
           << std::setw(10) << "Calls"
           << std::setw(12) << "Total (ms)"
           << std::setw(12) << "Avg (μs)"
           << std::setw(12) << "Min (μs)"
           << std::setw(12) << "Max (μs)"
           << std::setw(10) << "Tokens"
           << std::setw(10) << "Nodes" << "\n";
    
    report << std::string(100, '-') << "\n";
    
    auto stats = getAllStats();
    for (const auto& stat : stats) {
        report << std::left << std::setw(25) << stat.operationName
               << std::setw(10) << stat.callCount
               << std::setw(12) << std::fixed << std::setprecision(2) 
               << (stat.totalTime.count() / 1000.0)
               << std::setw(12) << stat.avgTime.count()
               << std::setw(12) << stat.minTime.count()
               << std::setw(12) << stat.maxTime.count()
               << std::setw(10) << stat.totalTokens
               << std::setw(10) << stat.totalNodes << "\n";
    }
    
    report << "\n";
    
    // Recent activity
    if (!recentMetrics.empty()) {
        report << "Recent Activity (last " << std::min(size_t(10), recentMetrics.size()) << " operations):\n";
        
        size_t start = recentMetrics.size() > 10 ? recentMetrics.size() - 10 : 0;
        for (size_t i = start; i < recentMetrics.size(); ++i) {
            const auto& metric = recentMetrics[i];
            report << "  " << metric.name << ": " << metric.duration.count() << "μs\n";
        }
    }
    
    return report.str();
}

void ParserProfiler::printReport() const {
    std::cout << generateReport() << std::endl;
}

void ParserProfiler::saveReportToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << generateReport();
        file.close();
    }
}

void ParserProfiler::enableLogging(const std::string& filename) {
    std::lock_guard<std::mutex> lock(statsMutex);
    
    if (logFile.is_open()) {
        logFile.close();
    }
    
    logFilePath = filename;
    logFile.open(filename);
    
    if (logFile.is_open()) {
        // Write CSV header
        logFile << "timestamp,operation,duration_us,memory_bytes,tokens,nodes\n";
    }
}

void ParserProfiler::disableLogging() {
    std::lock_guard<std::mutex> lock(statsMutex);
    
    if (logFile.is_open()) {
        logFile.close();
    }
    logFilePath.clear();
}

void ParserProfiler::reset() {
    std::lock_guard<std::mutex> lock(statsMutex);
    
    aggregatedStats.clear();
    recentMetrics.clear();
    peakMemoryUsage = 0;
    currentMemoryUsage = 0;
}

void ParserProfiler::clearRecentMetrics() {
    std::lock_guard<std::mutex> lock(statsMutex);
    recentMetrics.clear();
}

// Factory functions
std::unique_ptr<ParserProfiler> createParserProfiler(bool enabled) {
    return std::make_unique<ParserProfiler>(enabled);
}

void initializeGlobalProfiler(bool enabled) {
    g_parserProfiler = createParserProfiler(enabled);
}

void shutdownGlobalProfiler() {
    g_parserProfiler.reset();
}

} // namespace Parser