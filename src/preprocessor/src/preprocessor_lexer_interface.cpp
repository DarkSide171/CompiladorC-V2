#include "../include/preprocessor_lexer_interface.hpp"
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <iterator>

namespace Preprocessor {

// PositionMapper Implementation
void PositionMapper::addMapping(const SourceMapping& mapping) {
    mappings.push_back(mapping);
    lineToMappingIndex[mapping.processedLine] = mappings.size() - 1;
}

const SourceMapping* PositionMapper::findMapping(size_t processedLine, size_t processedColumn) const {
    auto it = lineToMappingIndex.find(processedLine);
    if (it != lineToMappingIndex.end()) {
        return &mappings[it->second];
    }
    
    // Busca o mapeamento mais próximo
    for (const auto& mapping : mappings) {
        if (mapping.processedLine <= processedLine) {
            return &mapping;
        }
    }
    
    return nullptr;
}

bool PositionMapper::mapToOriginal(size_t processedLine, size_t processedColumn,
                                  size_t& originalLine, size_t& originalColumn,
                                  std::string& originalFile) const {
    const SourceMapping* mapping = findMapping(processedLine, processedColumn);
    if (mapping) {
        originalLine = mapping->originalLine;
        originalColumn = mapping->originalColumn + (processedColumn - mapping->processedColumn);
        originalFile = mapping->originalFile;
        return true;
    }
    return false;
}

std::string PositionMapper::getOriginalFile(size_t processedLine) const {
    const SourceMapping* mapping = findMapping(processedLine, 0);
    return mapping ? mapping->originalFile : "";
}

bool PositionMapper::isFromMacroExpansion(size_t processedLine) const {
    const SourceMapping* mapping = findMapping(processedLine, 0);
    return mapping ? mapping->fromMacroExpansion : false;
}

std::string PositionMapper::getMacroName(size_t processedLine) const {
    const SourceMapping* mapping = findMapping(processedLine, 0);
    return mapping ? mapping->macroName : "";
}

void PositionMapper::clear() {
    mappings.clear();
    lineToMappingIndex.clear();
}

// IntegratedErrorHandler Implementation
void IntegratedErrorHandler::reportError(ErrorSource source, const std::string& message,
                                       size_t line, size_t column, const std::string& context) {
    IntegratedError error(source, message, line, column, line, column, "", context);
    
    if (positionMapper) {
        std::string originalFile;
        positionMapper->mapToOriginal(line, column, error.originalLine, 
                                    error.originalColumn, originalFile);
        error.originalFile = originalFile;
    }
    
    errors.push_back(error);
}

void IntegratedErrorHandler::reportWarning(ErrorSource source, const std::string& message,
                                         size_t line, size_t column, const std::string& context) {
    IntegratedError warning(source, message, line, column, line, column, "", context);
    
    if (positionMapper) {
        std::string originalFile;
        positionMapper->mapToOriginal(line, column, warning.originalLine, 
                                    warning.originalColumn, originalFile);
        warning.originalFile = originalFile;
    }
    
    warnings.push_back(warning);
}

// Métodos inline definidos no header

void IntegratedErrorHandler::clear() {
    errors.clear();
    warnings.clear();
}

// PreprocessorLexerInterface Implementation
PreprocessorLexerInterface::PreprocessorLexerInterface() 
    : isInitialized(false) {
    positionMapper = std::make_unique<PositionMapper>();
    errorHandler = std::make_unique<IntegratedErrorHandler>(positionMapper.get());
}

PreprocessorLexerInterface::~PreprocessorLexerInterface() = default;

bool PreprocessorLexerInterface::initialize(const PreprocessorConfig& config) {
    try {
        preprocessor = std::make_unique<PreprocessorMain>();
        isInitialized = true;
        return true;
    } catch (const std::exception& e) {
        errorHandler->reportError(IntegratedErrorHandler::ErrorSource::INTEGRATION,
                                "Failed to initialize preprocessor: " + std::string(e.what()),
                                0, 0);
        return false;
    }
}

ProcessingResult PreprocessorLexerInterface::processFile(const std::string& filename) {
    lastResult.clear();
    
    if (!isInitialized) {
        lastResult.addError("Interface not initialized");
        return lastResult;
    }
    
    try {
        // Processa o arquivo através do preprocessador real
        bool preprocessorSuccess = false;
        if (preprocessor) {
            // Configurar errorHandler no preprocessor
            preprocessor->setErrorHandler(errorHandler.get());
            
            std::cout << "[DEBUG] Tentando processar arquivo com pré-processador: " << filename << std::endl;
            preprocessorSuccess = preprocessor->process(filename);
            std::cout << "[DEBUG] Resultado do pré-processador: " << (preprocessorSuccess ? "SUCESSO" : "FALHA") << std::endl;
            if (preprocessorSuccess) {
                lastResult.processedCode = preprocessor->getExpandedCode();
                lastResult.includedFiles = preprocessor->getDependencies();
            } else {
                // Pré-processador falhou, vamos coletar os erros
                if (errorHandler) {
                    const auto& errors = errorHandler->getErrors();
                    const auto& warnings = errorHandler->getWarnings();
                    
                    std::cout << "[DEBUG] Coletando erros do errorHandler: " << errors.size() << " erros, " << warnings.size() << " avisos" << std::endl;
                    
                    for (const auto& error : errors) {
                        lastResult.addError(error.message);
                        std::cout << "[DEBUG] Erro adicionado: " << error.message << std::endl;
                    }
                    
                    for (const auto& warning : warnings) {
                        lastResult.addWarning(warning.message);
                        std::cout << "[DEBUG] Aviso adicionado: " << warning.message << std::endl;
                    }
                }
            }
        }
        
        if (!preprocessorSuccess) {
            // Fallback: lê o arquivo diretamente se o preprocessador falhar
            std::ifstream file(filename);
            if (file.is_open()) {
                std::string content((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
                lastResult.processedCode = content;
            } else {
                lastResult.addError("Could not open file: " + filename);
                return lastResult;
            }
        }
        
        // Constrói mapeamentos básicos
        buildPositionMappings(filename, lastResult.processedCode);
        
        // Coleta informações sobre macros
        collectMacroInformation();
        
        // Coleta erros e avisos do errorHandler (apenas se o pré-processador teve sucesso)
        if (preprocessorSuccess && errorHandler) {
            const auto& errors = errorHandler->getErrors();
            const auto& warnings = errorHandler->getWarnings();
            
            for (const auto& error : errors) {
                lastResult.addError(error.message);
            }
            
            for (const auto& warning : warnings) {
                lastResult.addWarning(warning.message);
            }
        }
        
    } catch (const std::exception& e) {
        lastResult.addError("Processing error: " + std::string(e.what()));
        errorHandler->reportError(IntegratedErrorHandler::ErrorSource::PREPROCESSOR,
                                e.what(), 0, 0);
    }
    
    // Define hasErrors baseado na presença de mensagens de erro
    lastResult.hasErrors = !lastResult.errorMessages.empty();
    std::cout << "[DEBUG] hasErrors definido como: " << (lastResult.hasErrors ? "true" : "false") << std::endl;
    std::cout << "[DEBUG] Número de erros: " << lastResult.errorMessages.size() << std::endl;
    
    return lastResult;
}

ProcessingResult PreprocessorLexerInterface::processString(const std::string& code, const std::string& filename) {
    // Preserva macros definidas antes de limpar
    auto preservedMacros = lastResult.definedMacros;
    auto preservedMacroDefinitions = lastResult.macroDefinitions;
    
    lastResult.clear();
    
    // Restaura macros preservadas
    lastResult.definedMacros = preservedMacros;
    lastResult.macroDefinitions = preservedMacroDefinitions;
    
    if (!isInitialized) {
        lastResult.addError("Interface not initialized");
        return lastResult;
    }
    
    try {
        // Simula processamento básico
        lastResult.processedCode = "// Processed: \n" + code;
        
        // Constrói mapeamentos básicos
        buildPositionMappings(filename, lastResult.processedCode);
        
        // Coleta informações sobre macros
        collectMacroInformation();
        
    } catch (const std::exception& e) {
        lastResult.addError("Processing error: " + std::string(e.what()));
        errorHandler->reportError(IntegratedErrorHandler::ErrorSource::PREPROCESSOR,
                                e.what(), 0, 0);
    }
    
    return lastResult;
}

void PreprocessorLexerInterface::defineMacro(const std::string& name, const std::string& value) {
    if (preprocessor) {
        preprocessor->defineMacro(name, value);
        lastResult.definedMacros.push_back(name);
        lastResult.macroDefinitions[name] = value;
    }
}

void PreprocessorLexerInterface::undefineMacro(const std::string& name) {
    if (preprocessor) {
        preprocessor->undefineMacro(name);
        auto it = std::find(lastResult.definedMacros.begin(), lastResult.definedMacros.end(), name);
        if (it != lastResult.definedMacros.end()) {
            lastResult.definedMacros.erase(it);
        }
        lastResult.macroDefinitions.erase(name);
    }
}

void PreprocessorLexerInterface::addIncludePath(const std::string& path) {
    if (preprocessor) {
        std::vector<std::string> current_paths = preprocessor->getDependencies();
        current_paths.push_back(path);
        preprocessor->setSearchPaths(current_paths);
    }
}

void PreprocessorLexerInterface::setStandard(const std::string& version) {
    if (preprocessor) {
        CVersion c_version = CVersion::C99; // padrão
        if (version == "c89" || version == "C89") {
            c_version = CVersion::C89;
        } else if (version == "c99" || version == "C99") {
            c_version = CVersion::C99;
        } else if (version == "c11" || version == "C11") {
            c_version = CVersion::C11;
        } else if (version == "c17" || version == "C17") {
            c_version = CVersion::C17;
        } else if (version == "c23" || version == "C23") {
            c_version = CVersion::C23;
        }
        preprocessor->setVersion(c_version);
    }
}

void PreprocessorLexerInterface::reset() {
    lastResult.clear();
    if (positionMapper) {
        positionMapper->clear();
    }
    if (errorHandler) {
        errorHandler->clear();
    }
}

std::unordered_map<std::string, size_t> PreprocessorLexerInterface::getStatistics() const {
    std::unordered_map<std::string, size_t> stats;
    stats["processed_lines"] = lastResult.processedCode.empty() ? 0 : 
        std::count(lastResult.processedCode.begin(), lastResult.processedCode.end(), '\n') + 1;
    stats["included_files"] = lastResult.includedFiles.size();
    stats["defined_macros"] = lastResult.definedMacros.size();
    stats["errors"] = lastResult.errorMessages.size();
    stats["warnings"] = lastResult.warningMessages.size();
    return stats;
}

std::vector<std::string> PreprocessorLexerInterface::getDependencies() const {
    return lastResult.includedFiles;
}

void PreprocessorLexerInterface::buildPositionMappings(const std::string& filename, const std::string& processedCode) {
    if (!positionMapper) return;
    
    // Cria mapeamentos básicos linha por linha
    size_t line = 1;
    for (size_t i = 0; i < processedCode.length(); ++i) {
        if (processedCode[i] == '\n') {
            SourceMapping mapping(line, 1, line, 1, filename);
            positionMapper->addMapping(mapping);
            lastResult.positionMappings.push_back(mapping);
            ++line;
        }
    }
    
    // Adiciona pelo menos um mapeamento se não houver quebras de linha
    if (lastResult.positionMappings.empty() && !processedCode.empty()) {
        SourceMapping mapping(1, 1, 1, 1, filename);
        positionMapper->addMapping(mapping);
        lastResult.positionMappings.push_back(mapping);
    }
}

void PreprocessorLexerInterface::collectMacroInformation() {
    if (!preprocessor) return;
    
    try {
        // Obter dependências do preprocessor
        auto dependencies = preprocessor->getDependencies();
        lastResult.includedFiles = dependencies;
        
        // Obter estatísticas do preprocessor
        auto preprocessor_state = preprocessor->getStatistics();
        auto state_stats = preprocessor_state.getStatistics();
        
        // Obter macros definidas do MacroProcessor através do PreprocessorMain
        auto definedMacros = preprocessor->getDefinedMacros();
        lastResult.definedMacros.clear();
        lastResult.macroDefinitions.clear();
        
        // Coletar macros definidas reais
        for (const auto& macro : definedMacros) {
            lastResult.definedMacros.push_back(macro.first);
            lastResult.macroDefinitions[macro.first] = macro.second;
        }
        
        // Extrair informações de arquivos processados
        size_t files_count = state_stats.files_processed;
        size_t macros_count = lastResult.definedMacros.size();
        
        std::cout << "[DEBUG] Arquivos processados: " << files_count << std::endl;
        std::cout << "[DEBUG] Macros definidas: " << macros_count << std::endl;
        
        std::cout << "[DEBUG] Macros coletadas: " << lastResult.definedMacros.size() << std::endl;
        std::cout << "[DEBUG] Arquivos incluídos: " << lastResult.includedFiles.size() << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "[DEBUG] Erro ao coletar informações de macros: " << e.what() << std::endl;
    }
}

// CompilationPipeline Implementation
CompilationPipeline::CompilationPipeline() : pipelineReady(false) {
    interface = std::make_unique<PreprocessorLexerInterface>();
}

CompilationPipeline::~CompilationPipeline() = default;

bool CompilationPipeline::initialize(const PreprocessorConfig& config) {
    pipelineReady = interface->initialize(config);
    return pipelineReady;
}

bool CompilationPipeline::processFile(const std::string& filename) {
    if (!pipelineReady) return false;
    
    currentFile = filename;
    ProcessingResult result = interface->processFile(filename);
    return !result.hasErrors;
}

bool CompilationPipeline::processString(const std::string& code, const std::string& filename) {
    if (!pipelineReady) return false;
    
    currentFile = filename;
    ProcessingResult result = interface->processString(code, filename);
    return !result.hasErrors;
}

const std::string& CompilationPipeline::getProcessedCode() const {
    static const std::string empty;
    return interface ? interface->getProcessedCode() : empty;
}

// Método getStatistics removido - não existe na declaração

void CompilationPipeline::reset() {
    if (interface) {
        interface->reset();
    }
    currentFile.clear();
}

} // namespace Preprocessor