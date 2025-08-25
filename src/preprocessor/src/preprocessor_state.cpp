#include "../include/preprocessor_state.hpp"
#include <stdexcept>
#include <sstream>
#include <algorithm>

namespace Preprocessor {

// ============================================================================
// IMPLEMENTAÇÃO DA CLASSE PreprocessorState
// ============================================================================

PreprocessorState::PreprocessorState() 
    : processing_mode_(ProcessingMode::NORMAL),
      in_conditional_block_(false),
      has_error_(false) {
    // Inicializa com estado IDLE
    state_stack_.push(ProcessingState::IDLE);
    statistics_.state_transitions = 1;
}

// ========================================================================
// MÉTODOS DE PILHA DE ESTADOS
// ========================================================================

void PreprocessorState::pushState(ProcessingState state) {
    if (!isValidStateTransition(getCurrentState(), state)) {
        throw std::runtime_error("Transição de estado inválida: " + 
                                processingStateToString(getCurrentState()) + 
                                " -> " + processingStateToString(state));
    }
    
    state_stack_.push(state);
    statistics_.state_transitions++;
    
    // Atualiza profundidade máxima da pilha
    if (state_stack_.size() > statistics_.max_stack_depth) {
        statistics_.max_stack_depth = state_stack_.size();
    }
    
    // Atualiza estatísticas específicas
    switch (state) {
        case ProcessingState::PROCESSING_CONDITIONAL:
        case ProcessingState::SKIPPING_CONDITIONAL:
            statistics_.conditional_blocks++;
            break;
        case ProcessingState::PROCESSING_MACRO:
            statistics_.macro_expansions++;
            break;
        case ProcessingState::PROCESSING_INCLUDE:
            statistics_.include_depth++;
            break;
        default:
            break;
    }
}

ProcessingState PreprocessorState::popState() {
    if (state_stack_.empty()) {
        throw std::runtime_error("Tentativa de desempilhar estado de pilha vazia");
    }
    
    ProcessingState state = state_stack_.top();
    state_stack_.pop();
    statistics_.state_transitions++;
    
    return state;
}

ProcessingState PreprocessorState::getCurrentState() const {
    if (state_stack_.empty()) {
        return ProcessingState::IDLE;
    }
    return state_stack_.top();
}

bool PreprocessorState::isEmpty() const {
    return state_stack_.empty();
}

size_t PreprocessorState::getDepth() const {
    return state_stack_.size();
}

// ========================================================================
// MÉTODOS DE CONTEXTO DE ARQUIVO
// ========================================================================

void PreprocessorState::setFileContext(const FileContext& context) {
    current_file_context_ = context;
}

FileContext PreprocessorState::getFileContext() const {
    return current_file_context_;
}

void PreprocessorState::pushFileContext(const std::string& filename, size_t line, size_t column) {
    // Salva contexto atual na pilha
    file_context_stack_.push(current_file_context_);
    
    // Define novo contexto
    current_file_context_ = FileContext(filename, line, column);
    
    // Adiciona arquivo à lista de processados
    addProcessedFile(filename);
}

void PreprocessorState::popFileContext() {
    if (file_context_stack_.empty()) {
        throw std::runtime_error("Tentativa de desempilhar contexto de arquivo de pilha vazia");
    }
    
    current_file_context_ = file_context_stack_.top();
    file_context_stack_.pop();
}

// ========================================================================
// MÉTODOS DE POSIÇÃO
// ========================================================================

void PreprocessorState::setCurrentLine(size_t line) {
    current_file_context_.line = line;
}

void PreprocessorState::setCurrentColumn(size_t column) {
    current_file_context_.column = column;
}

size_t PreprocessorState::getCurrentLine() const {
    return current_file_context_.line;
}

size_t PreprocessorState::getCurrentColumn() const {
    return current_file_context_.column;
}

void PreprocessorState::incrementLine() {
    current_file_context_.line++;
    current_file_context_.column = 1;  // Reset coluna para início da linha
    current_file_context_.absolute_position++;
}

void PreprocessorState::incrementColumn() {
    current_file_context_.column++;
    current_file_context_.absolute_position++;
}

void PreprocessorState::resetColumn() {
    current_file_context_.column = 1;
}

// ========================================================================
// MÉTODOS DE MODO DE PROCESSAMENTO
// ========================================================================

void PreprocessorState::setProcessingMode(ProcessingMode mode) {
    processing_mode_ = mode;
}

ProcessingMode PreprocessorState::getProcessingMode() const {
    return processing_mode_;
}

// ========================================================================
// MÉTODOS DE BLOCOS CONDICIONAIS
// ========================================================================

void PreprocessorState::enableConditionalBlock() {
    in_conditional_block_ = true;
}

void PreprocessorState::disableConditionalBlock() {
    in_conditional_block_ = false;
}

bool PreprocessorState::isInConditionalBlock() const {
    return in_conditional_block_;
}

// ========================================================================
// MÉTODOS DE ESTADO DE ERRO
// ========================================================================

void PreprocessorState::setErrorState(bool hasError) {
    has_error_ = hasError;
    if (hasError) {
        pushState(ProcessingState::ERROR_STATE);
    }
}

bool PreprocessorState::hasError() const {
    return has_error_;
}

// ========================================================================
// MÉTODOS DE ARQUIVOS PROCESSADOS
// ========================================================================

void PreprocessorState::addProcessedFile(const std::string& filename) {
    if (processed_files_.find(filename) == processed_files_.end()) {
        processed_files_.insert(filename);
        statistics_.files_processed++;
    }
}

bool PreprocessorState::wasFileProcessed(const std::string& filename) const {
    return processed_files_.find(filename) != processed_files_.end();
}

std::vector<std::string> PreprocessorState::getProcessedFiles() const {
    std::vector<std::string> files;
    files.reserve(processed_files_.size());
    
    for (const auto& filename : processed_files_) {
        files.push_back(filename);
    }
    
    std::sort(files.begin(), files.end());
    return files;
}

// ========================================================================
// MÉTODOS UTILITÁRIOS
// ========================================================================

void PreprocessorState::reset() {
    // Limpa pilhas
    while (!state_stack_.empty()) {
        state_stack_.pop();
    }
    while (!file_context_stack_.empty()) {
        file_context_stack_.pop();
    }
    
    // Reinicializa estado
    state_stack_.push(ProcessingState::IDLE);
    current_file_context_ = FileContext();
    processing_mode_ = ProcessingMode::NORMAL;
    in_conditional_block_ = false;
    has_error_ = false;
    processed_files_.clear();
    
    // Reinicializa estatísticas
    statistics_ = StateStats();
    statistics_.state_transitions = 1;
}

StateStats PreprocessorState::getStatistics() const {
    return statistics_;
}

bool PreprocessorState::validateState() const {
    // Verifica consistência básica
    if (state_stack_.empty()) {
        return false;
    }
    
    // Verifica se estado atual é válido
    ProcessingState current = getCurrentState();
    if (current == ProcessingState::ERROR_STATE && !has_error_) {
        return false;
    }
    
    // Verifica se contexto de arquivo é necessário
    if (requiresFileContext(current) && current_file_context_.filename.empty()) {
        return false;
    }
    
    // Verifica consistência das pilhas
    if (file_context_stack_.size() > state_stack_.size()) {
        return false;
    }
    
    return true;
}

std::string PreprocessorState::generateStateReport() const {
    std::ostringstream report;
    
    report << "=== RELATÓRIO DE ESTADO DO PRÉ-PROCESSADOR ===\n";
    report << "Estado atual: " << processingStateToString(getCurrentState()) << "\n";
    report << "Modo de processamento: " << processingModeToString(processing_mode_) << "\n";
    report << "Profundidade da pilha: " << getDepth() << "\n";
    report << "Em bloco condicional: " << (in_conditional_block_ ? "Sim" : "Não") << "\n";
    report << "Estado de erro: " << (has_error_ ? "Sim" : "Não") << "\n";
    
    report << "\n=== CONTEXTO DE ARQUIVO ===\n";
    report << "Arquivo: " << current_file_context_.filename << "\n";
    report << "Linha: " << current_file_context_.line << "\n";
    report << "Coluna: " << current_file_context_.column << "\n";
    report << "Posição absoluta: " << current_file_context_.absolute_position << "\n";
    
    report << "\n=== ESTATÍSTICAS ===\n";
    report << "Transições de estado: " << statistics_.state_transitions << "\n";
    report << "Profundidade máxima da pilha: " << statistics_.max_stack_depth << "\n";
    report << "Arquivos processados: " << statistics_.files_processed << "\n";
    report << "Blocos condicionais: " << statistics_.conditional_blocks << "\n";
    report << "Expansões de macro: " << statistics_.macro_expansions << "\n";
    report << "Profundidade de includes: " << statistics_.include_depth << "\n";
    
    report << "\n=== ARQUIVOS PROCESSADOS ===\n";
    auto files = getProcessedFiles();
    for (size_t i = 0; i < files.size(); ++i) {
        report << (i + 1) << ". " << files[i] << "\n";
    }
    
    return report.str();
}

// ============================================================================
// FUNÇÕES UTILITÁRIAS GLOBAIS
// ============================================================================

std::string processingStateToString(ProcessingState state) {
    switch (state) {
        case ProcessingState::IDLE:
            return "IDLE";
        case ProcessingState::PROCESSING_FILE:
            return "PROCESSING_FILE";
        case ProcessingState::PROCESSING_INCLUDE:
            return "PROCESSING_INCLUDE";
        case ProcessingState::PROCESSING_MACRO:
            return "PROCESSING_MACRO";
        case ProcessingState::PROCESSING_CONDITIONAL:
            return "PROCESSING_CONDITIONAL";
        case ProcessingState::SKIPPING_CONDITIONAL:
            return "SKIPPING_CONDITIONAL";
        case ProcessingState::PROCESSING_DIRECTIVE:
            return "PROCESSING_DIRECTIVE";
        case ProcessingState::ERROR_STATE:
            return "ERROR_STATE";
        case ProcessingState::FINISHED:
            return "FINISHED";
        default:
            return "UNKNOWN";
    }
}

std::string processingModeToString(ProcessingMode mode) {
    switch (mode) {
        case ProcessingMode::NORMAL:
            return "NORMAL";
        case ProcessingMode::MACRO_EXPANSION:
            return "MACRO_EXPANSION";
        case ProcessingMode::CONDITIONAL:
            return "CONDITIONAL";
        case ProcessingMode::INCLUDE:
            return "INCLUDE";
        case ProcessingMode::DIRECTIVE:
            return "DIRECTIVE";
        default:
            return "UNKNOWN";
    }
}

bool isValidStateTransition(ProcessingState from, ProcessingState to) {
    // Transições sempre válidas
    if (to == ProcessingState::ERROR_STATE || from == ProcessingState::ERROR_STATE) {
        return true;
    }
    
    switch (from) {
        case ProcessingState::IDLE:
            return to == ProcessingState::PROCESSING_FILE || 
                   to == ProcessingState::FINISHED;
        
        case ProcessingState::PROCESSING_FILE:
            return to == ProcessingState::PROCESSING_INCLUDE ||
                   to == ProcessingState::PROCESSING_MACRO ||
                   to == ProcessingState::PROCESSING_CONDITIONAL ||
                   to == ProcessingState::SKIPPING_CONDITIONAL ||
                   to == ProcessingState::PROCESSING_DIRECTIVE ||
                   to == ProcessingState::FINISHED;
        
        case ProcessingState::PROCESSING_INCLUDE:
            return to == ProcessingState::PROCESSING_FILE ||
                   to == ProcessingState::PROCESSING_MACRO ||
                   to == ProcessingState::PROCESSING_CONDITIONAL ||
                   to == ProcessingState::PROCESSING_DIRECTIVE;
        
        case ProcessingState::PROCESSING_MACRO:
            return to == ProcessingState::PROCESSING_FILE ||
                   to == ProcessingState::PROCESSING_INCLUDE ||
                   to == ProcessingState::PROCESSING_CONDITIONAL;
        
        case ProcessingState::PROCESSING_CONDITIONAL:
            return to == ProcessingState::PROCESSING_FILE ||
                   to == ProcessingState::PROCESSING_INCLUDE ||
                   to == ProcessingState::PROCESSING_MACRO ||
                   to == ProcessingState::PROCESSING_DIRECTIVE ||
                   to == ProcessingState::SKIPPING_CONDITIONAL;
        
        case ProcessingState::SKIPPING_CONDITIONAL:
            return to == ProcessingState::PROCESSING_FILE ||
                   to == ProcessingState::PROCESSING_INCLUDE ||
                   to == ProcessingState::PROCESSING_MACRO ||
                   to == ProcessingState::PROCESSING_DIRECTIVE ||
                   to == ProcessingState::PROCESSING_CONDITIONAL;
        
        case ProcessingState::PROCESSING_DIRECTIVE:
            return to == ProcessingState::PROCESSING_FILE ||
                   to == ProcessingState::PROCESSING_INCLUDE ||
                   to == ProcessingState::PROCESSING_MACRO ||
                   to == ProcessingState::PROCESSING_CONDITIONAL ||
                   to == ProcessingState::SKIPPING_CONDITIONAL;
        
        case ProcessingState::FINISHED:
            return to == ProcessingState::IDLE;
        
        default:
            return false;
    }
}

bool requiresFileContext(ProcessingState state) {
    switch (state) {
        case ProcessingState::PROCESSING_FILE:
        case ProcessingState::PROCESSING_INCLUDE:
        case ProcessingState::PROCESSING_MACRO:
        case ProcessingState::PROCESSING_CONDITIONAL:
        case ProcessingState::SKIPPING_CONDITIONAL:
        case ProcessingState::PROCESSING_DIRECTIVE:
            return true;
        
        case ProcessingState::IDLE:
        case ProcessingState::ERROR_STATE:
        case ProcessingState::FINISHED:
        default:
            return false;
    }
}

} // namespace Preprocessor