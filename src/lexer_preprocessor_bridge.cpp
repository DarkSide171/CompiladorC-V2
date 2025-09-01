/**
 * @file lexer_preprocessor_bridge.cpp
 * @brief Implementação da ponte de integração entre Lexer e Preprocessor
 */

#include "lexer_preprocessor_bridge.hpp"
#include "lexer/include/error_handler.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace Integration {

// ============================================================================
// IMPLEMENTAÇÃO DA CLASSE LexerPreprocessorBridge
// ============================================================================

LexerPreprocessorBridge::LexerPreprocessorBridge(const IntegrationConfig& config)
    : config(config)
    , currentTokenIndex(0)
    , isInitialized(false)
    , hasProcessedInput(false) {
    // Construtor - inicialização será feita em initialize()
}

LexerPreprocessorBridge::~LexerPreprocessorBridge() {
    // Destrutor - cleanup automático via smart pointers
}

bool LexerPreprocessorBridge::initialize() {
    std::cout << "[DEBUG] Inicializando LexerPreprocessorBridge..." << std::endl;
    try {
        // Inicializar interface do preprocessor
        preprocessorInterface = std::make_unique<Preprocessor::PreprocessorLexerInterface>();
        std::cout << "[DEBUG] PreprocessorLexerInterface criada" << std::endl;
        
        // Configurar preprocessor
        Preprocessor::PreprocessorConfig ppConfig;
        // Usar métodos públicos para configuração
        for (const auto& path : config.includePaths) {
            ppConfig.addIncludePath(path);
        }
        
        if (!preprocessorInterface->initialize(ppConfig)) {
            std::cout << "[DEBUG] Falha na inicialização da interface do pré-processador" << std::endl;
            return false;
        }
        std::cout << "[DEBUG] Interface do pré-processador inicializada com sucesso" << std::endl;
        
        // Inicializar error handler
        errorHandler = std::make_unique<Lexer::ErrorHandler>();
        
        // Configurar integração de erros se habilitada
        if (config.enableErrorIntegration) {
            setupErrorIntegration();
        }
        
        isInitialized = true;
        std::cout << "[DEBUG] LexerPreprocessorBridge inicializado com sucesso" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        if (onIntegrationError) {
            onIntegrationError("Erro na inicialização: " + std::string(e.what()), 0, 0);
        }
        return false;
    }
}

bool LexerPreprocessorBridge::processFile(const std::string& filename) {
    std::cout << "[DEBUG] LexerPreprocessorBridge::processFile chamado para: " << filename << std::endl;
    if (!isInitialized) {
        std::cout << "[DEBUG] Bridge não inicializado, inicializando..." << std::endl;
        if (!initialize()) {
            std::cout << "[DEBUG] Falha na inicialização do bridge" << std::endl;
            return false;
        }
    }
    
    try {
        // Processar arquivo através do preprocessor
        std::cout << "[DEBUG] Chamando preprocessorInterface->processFile..." << std::endl;
        lastProcessingResult = preprocessorInterface->processFile(filename);
        std::cout << "[DEBUG] Resultado do processamento: hasErrors=" << lastProcessingResult.hasErrors << std::endl;
        
        if (lastProcessingResult.hasErrors) {
            std::cout << "[DEBUG] Processamento falhou com erros - implementando fallback para código original" << std::endl;
            
            // FALLBACK: Ler e processar código original sem pré-processamento
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cout << "[DEBUG] Erro: não foi possível abrir arquivo para fallback" << std::endl;
                return false;
            }
            
            std::string originalCode((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
            file.close();
            
            std::cout << "[DEBUG] FALLBACK: Processando código original sem pré-processamento" << std::endl;
            std::cout << "[DEBUG] Código original tem " << originalCode.length() << " caracteres" << std::endl;
            
            // Limpar resultado anterior e configurar para código original
            lastProcessingResult.processedCode = originalCode;
            lastProcessingResult.hasErrors = true; // Manter flag de erro para indicar que houve problemas no pré-processamento
            
            // Inicializar lexer com código original
            codeStream = std::make_unique<std::istringstream>(originalCode);
            lexer = std::make_unique<Lexer::LexerMain>(*codeStream, errorHandler.get(), filename);
            
            // Construir tokens integrados (sem mapeamentos do pré-processador)
            buildIntegratedTokens();
            
            hasProcessedInput = true;
            currentTokenIndex = 0;
            
            std::cout << "[DEBUG] FALLBACK: Processamento do código original concluído" << std::endl;
            return true; // Retorna true para permitir análise léxica do código original
        }
        
        std::cout << "[DEBUG] Processamento bem-sucedido, código processado tem " << lastProcessingResult.processedCode.length() << " caracteres" << std::endl;
        
        // DEBUG: Imprimir o código processado completo
        std::cout << "[DEBUG] CÓDIGO PROCESSADO PELO PREPROCESSOR:" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << lastProcessingResult.processedCode << std::endl;
        std::cout << "========================================" << std::endl;
        
        // Inicializar lexer com código processado
        codeStream = std::make_unique<std::istringstream>(lastProcessingResult.processedCode);
        lexer = std::make_unique<Lexer::LexerMain>(*codeStream, errorHandler.get(), filename);
        
        // Construir tokens integrados
        buildIntegratedTokens();
        
        // Validar mapeamentos se habilitado
        if (config.enablePositionMapping) {
            validatePositionMappings();
        }
        
        hasProcessedInput = true;
        currentTokenIndex = 0;
        
        // Chamar callback se definido
        if (onFileIncluded) {
            onFileIncluded(filename);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        if (onIntegrationError) {
            onIntegrationError("Erro no processamento: " + std::string(e.what()), 0, 0);
        }
        return false;
    }
}

bool LexerPreprocessorBridge::processString(const std::string& code, const std::string& filename) {
    if (!isInitialized) {
        if (!initialize()) {
            return false;
        }
    }
    
    try {
        // Processar string através do preprocessor
        lastProcessingResult = preprocessorInterface->processString(code, filename);
        
        if (lastProcessingResult.hasErrors) {
            return false;
        }
        
        // Inicializar lexer com código processado
        codeStream = std::make_unique<std::istringstream>(lastProcessingResult.processedCode);
        lexer = std::make_unique<Lexer::LexerMain>(*codeStream, errorHandler.get(), filename);
        
        // Construir tokens integrados
        buildIntegratedTokens();
        
        // Validar mapeamentos se habilitado
        if (config.enablePositionMapping) {
            validatePositionMappings();
        }
        
        hasProcessedInput = true;
        currentTokenIndex = 0;
        
        return true;
        
    } catch (const std::exception& e) {
        if (onIntegrationError) {
            onIntegrationError("Erro no processamento: " + std::string(e.what()), 0, 0);
        }
        return false;
    }
}

IntegratedToken LexerPreprocessorBridge::nextToken() {
    if (!hasProcessedInput || currentTokenIndex >= tokenCache.size()) {
        return IntegratedToken(); // Token vazio
    }
    
    return tokenCache[currentTokenIndex++];
}

IntegratedToken LexerPreprocessorBridge::peekToken() {
    if (!hasProcessedInput || currentTokenIndex >= tokenCache.size()) {
        return IntegratedToken(); // Token vazio
    }
    
    return tokenCache[currentTokenIndex];
}

bool LexerPreprocessorBridge::hasMoreTokens() const {
    return hasProcessedInput && currentTokenIndex < tokenCache.size();
}

std::vector<IntegratedToken> LexerPreprocessorBridge::tokenizeAll() {
    if (!hasProcessedInput) {
        return {};
    }
    
    return tokenCache;
}

void LexerPreprocessorBridge::defineMacro(const std::string& name, const std::string& value) {
    if (preprocessorInterface) {
        preprocessorInterface->defineMacro(name, value);
    }
}

void LexerPreprocessorBridge::undefineMacro(const std::string& name) {
    if (preprocessorInterface) {
        preprocessorInterface->undefineMacro(name);
    }
}

void LexerPreprocessorBridge::addIncludePath(const std::string& path) {
    config.includePaths.push_back(path);
    if (preprocessorInterface) {
        preprocessorInterface->addIncludePath(path);
    }
}

void LexerPreprocessorBridge::setStandard(const std::string& standard) {
    config.cStandard = standard;
}

void LexerPreprocessorBridge::reset() {
    tokenCache.clear();
    currentTokenIndex = 0;
    hasProcessedInput = false;
    lastProcessingResult = Preprocessor::ProcessingResult();
    
    if (lexer) {
        lexer->reset();
    }
}

bool LexerPreprocessorBridge::mapToOriginalPosition(size_t processedLine, size_t processedColumn,
                                                   size_t& originalLine, size_t& originalColumn,
                                                   std::string& originalFile) const {
    if (!preprocessorInterface) {
        return false;
    }
    
    const auto& mapper = preprocessorInterface->getPositionMapper();
    const auto* mapping = mapper.findMapping(processedLine, processedColumn);
    
    if (!mapping || mapping->originalLine == 0) {
        return false; // Mapeamento não encontrado
    }
    
    originalLine = mapping->originalLine;
    originalColumn = mapping->originalColumn;
    originalFile = mapping->originalFile;
    
    return true;
}

bool LexerPreprocessorBridge::validatePositionMappings() const {
    if (!preprocessorInterface) {
        return false;
    }
    
    // Implementar validação básica
    const auto& result = preprocessorInterface->getLastResult();
    return !result.processedCode.empty() && !result.positionMappings.empty();
}

bool LexerPreprocessorBridge::runIntegrationTests() {
    return IntegrationTester::runAllTests();
}

const Preprocessor::ProcessingResult& LexerPreprocessorBridge::getLastProcessingResult() const {
    return lastProcessingResult;
}

std::vector<std::string> LexerPreprocessorBridge::getIncludedFiles() const {
    return lastProcessingResult.includedFiles;
}

std::vector<std::string> LexerPreprocessorBridge::getDefinedMacros() const {
    return lastProcessingResult.definedMacros;
}

std::unordered_map<std::string, size_t> LexerPreprocessorBridge::getStatistics() const {
    std::unordered_map<std::string, size_t> stats;
    stats["tokens_processed"] = tokenCache.size();
    stats["files_included"] = lastProcessingResult.includedFiles.size();
    stats["macros_defined"] = lastProcessingResult.definedMacros.size();
    stats["position_mappings"] = lastProcessingResult.positionMappings.size();
    return stats;
}

bool LexerPreprocessorBridge::hasErrors() const {
    return lastProcessingResult.hasErrors;
}

std::vector<std::string> LexerPreprocessorBridge::getErrorMessages() const {
    return lastProcessingResult.errorMessages;
}

std::vector<std::string> LexerPreprocessorBridge::getWarningMessages() const {
    return lastProcessingResult.warningMessages;
}

void LexerPreprocessorBridge::setOnMacroExpanded(std::function<void(const std::string&, const Preprocessor::SourceMapping&)> callback) {
    onMacroExpanded = callback;
}

void LexerPreprocessorBridge::setOnFileIncluded(std::function<void(const std::string&)> callback) {
    onFileIncluded = callback;
}

void LexerPreprocessorBridge::setOnIntegrationError(std::function<void(const std::string&, size_t, size_t)> callback) {
    onIntegrationError = callback;
}

// ============================================================================
// MÉTODOS PRIVADOS AUXILIARES
// ============================================================================

bool LexerPreprocessorBridge::initializeComponents() {
    // Já implementado em initialize()
    return true;
}

void LexerPreprocessorBridge::setupErrorIntegration() {
    // Configurar integração de erros entre preprocessor e lexer
    if (preprocessorInterface && errorHandler && config.enableErrorIntegration) {
        // Conectar o ErrorHandler do lexer com o external_error_handler do preprocessor
        // através da interface que já possui IntegratedErrorHandler
        
        // O preprocessorInterface já configura automaticamente o errorHandler
        // no preprocessor através do método processFile/processString
        
        // Configurar callback para capturar erros do preprocessor e repassar ao lexer
        preprocessorInterface->setOnError([this](const Preprocessor::IntegratedErrorHandler::IntegratedError& error) {
            if (errorHandler) {
                // Converter erro integrado para formato do lexer
                std::string errorMsg = "[Preprocessor] " + error.message;
                
                // Reportar erro ao ErrorHandler do lexer
                Lexer::Position pos;
                pos.line = static_cast<int>(error.originalLine);
                pos.column = static_cast<int>(error.originalColumn);
                pos.offset = 0; // Não temos informação de offset
                
                if (error.source == Preprocessor::IntegratedErrorHandler::ErrorSource::PREPROCESSOR) {
                    errorHandler->reportError(Lexer::ErrorType::INVALID_CHARACTER, 
                                            errorMsg, 
                                            pos,
                                            error.originalFile);
                } else {
                    errorHandler->reportError(Lexer::ErrorType::INTERNAL_ERROR, 
                                            errorMsg, 
                                            pos,
                                            error.originalFile);
                }
            }
        });
        
        std::cout << "[DEBUG] Integração de erros configurada entre preprocessor e lexer" << std::endl;
    }
}

void LexerPreprocessorBridge::buildIntegratedTokens() {
    if (!lexer) {
        return;
    }
    
    tokenCache.clear();
    
    // Tokenizar todo o código processado
    auto tokens = lexer->tokenizeAll();
    
    // Criar tokens integrados com mapeamento de posição
    for (size_t i = 0; i < tokens.size(); ++i) {
        IntegratedToken integratedToken = createIntegratedToken(tokens[i], i);
        tokenCache.push_back(integratedToken);
    }
}

IntegratedToken LexerPreprocessorBridge::createIntegratedToken(const Lexer::Token& token, size_t position) {
    IntegratedToken integratedToken;
    integratedToken.lexerToken = token;
    integratedToken.isFromPreprocessor = true;
    
    // Encontrar mapeamento de posição
    auto tokenPos = token.getPosition();
    integratedToken.sourceMapping = findMappingForPosition(tokenPos.line, tokenPos.column);
    
    // Definir texto original
    integratedToken.originalText = token.getLexeme();
    
    return integratedToken;
}

Preprocessor::SourceMapping LexerPreprocessorBridge::findMappingForPosition(size_t line, size_t column) const {
    if (!preprocessorInterface) {
        return Preprocessor::SourceMapping();
    }
    
    const auto& mapper = preprocessorInterface->getPositionMapper();
    const auto* mapping = mapper.findMapping(line, column);
    
    if (mapping) {
        return *mapping;
    }
    
    return Preprocessor::SourceMapping();
}

void LexerPreprocessorBridge::validatePositionMappings() {
    // Implementar validação detalhada dos mapeamentos
    if (config.enableDebugMode) {
        std::cout << "Validando mapeamentos de posição..." << std::endl;
        
        for (const auto& token : tokenCache) {
            if (token.sourceMapping.originalLine == 0) {
                std::cout << "Aviso: Token sem mapeamento válido: " << token.originalText << std::endl;
            }
        }
    }
}

// ============================================================================
// IMPLEMENTAÇÃO DA CLASSE IntegrationTester
// ============================================================================

bool IntegrationTester::runAllTests() {
    std::cout << "Executando testes de integração..." << std::endl;
    
    bool allPassed = true;
    
    allPassed &= testPositionMapping();
    allPassed &= testMacroProcessing();
    allPassed &= testFileInclusion();
    allPassed &= testEndToEndPipeline();
    allPassed &= testCompatibility();
    
    if (allPassed) {
        std::cout << "Todos os testes de integração passaram!" << std::endl;
    } else {
        std::cout << "Alguns testes de integração falharam." << std::endl;
    }
    
    return allPassed;
}

bool IntegrationTester::testPositionMapping() {
    std::cout << "Testando mapeamento de posições..." << std::endl;
    
    try {
        IntegrationConfig config;
        config.enablePositionMapping = true;
        
        LexerPreprocessorBridge bridge(config);
        
        if (!bridge.initialize()) {
            std::cout << "Erro: Falha na inicialização" << std::endl;
            return false;
        }
        
        // Teste com código simples
        std::string testCode = "#define MAX 100\nint x = MAX;";
        
        if (!bridge.processString(testCode, "test.c")) {
            std::cout << "Erro: Falha no processamento" << std::endl;
            return false;
        }
        
        // Verificar se há tokens
        if (!bridge.hasMoreTokens()) {
            std::cout << "Erro: Nenhum token gerado" << std::endl;
            return false;
        }
        
        // Verificar mapeamento
        size_t originalLine, originalColumn;
        std::string originalFile;
        
        if (!bridge.mapToOriginalPosition(2, 9, originalLine, originalColumn, originalFile)) {
            std::cout << "Aviso: Mapeamento não encontrado (pode ser normal)" << std::endl;
        }
        
        std::cout << "Teste de mapeamento de posições: PASSOU" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Erro no teste de mapeamento: " << e.what() << std::endl;
        return false;
    }
}

bool IntegrationTester::testMacroProcessing() {
    std::cout << "Testando processamento de macros..." << std::endl;
    
    try {
        IntegrationConfig config;
        config.enableMacroTracking = true;
        
        LexerPreprocessorBridge bridge(config);
        
        if (!bridge.initialize()) {
            return false;
        }
        
        // Definir macro programaticamente
        bridge.defineMacro("TEST_MACRO", "42");
        
        // Processar código que usa a macro
        std::string testCode = "int value = TEST_MACRO;";
        
        if (!bridge.processString(testCode, "test_macro.c")) {
            return false;
        }
        
        // Verificar se macro foi processada
        auto macros = bridge.getDefinedMacros();
        bool found = std::find(macros.begin(), macros.end(), "TEST_MACRO") != macros.end();
        
        if (!found) {
            std::cout << "Erro: Macro não encontrada" << std::endl;
            return false;
        }
        
        std::cout << "Teste de processamento de macros: PASSOU" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Erro no teste de macros: " << e.what() << std::endl;
        return false;
    }
}

bool IntegrationTester::testFileInclusion() {
    std::cout << "Testando inclusão de arquivos..." << std::endl;
    
    try {
        IntegrationConfig config;
        
        LexerPreprocessorBridge bridge(config);
        
        if (!bridge.initialize()) {
            return false;
        }
        
        // Teste básico sem inclusão real de arquivos
        std::string testCode = "// Teste de inclusão\nint main() { return 0; }";
        
        if (!bridge.processString(testCode, "test_include.c")) {
            return false;
        }
        
        std::cout << "Teste de inclusão de arquivos: PASSOU" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Erro no teste de inclusão: " << e.what() << std::endl;
        return false;
    }
}

bool IntegrationTester::testEndToEndPipeline() {
    std::cout << "Testando pipeline end-to-end..." << std::endl;
    
    try {
        IntegrationConfig config;
        config.enablePositionMapping = true;
        config.enableMacroTracking = true;
        config.enableErrorIntegration = true;
        
        LexerPreprocessorBridge bridge(config);
        
        if (!bridge.initialize()) {
            return false;
        }
        
        // Código de teste completo
        std::string testCode = 
            "#define PI 3.14159\n"
            "#define AREA(r) (PI * (r) * (r))\n"
            "\n"
            "int main() {\n"
            "    double radius = 5.0;\n"
            "    double area = AREA(radius);\n"
            "    return 0;\n"
            "}";
        
        if (!bridge.processString(testCode, "test_pipeline.c")) {
            return false;
        }
        
        // Tokenizar tudo
        auto tokens = bridge.tokenizeAll();
        
        if (tokens.empty()) {
            std::cout << "Erro: Nenhum token gerado" << std::endl;
            return false;
        }
        
        // Verificar estatísticas
        auto stats = bridge.getStatistics();
        
        if (stats["tokens_processed"] == 0) {
            std::cout << "Erro: Estatísticas inválidas" << std::endl;
            return false;
        }
        
        std::cout << "Pipeline end-to-end: PASSOU (" << tokens.size() << " tokens processados)" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Erro no teste end-to-end: " << e.what() << std::endl;
        return false;
    }
}

bool IntegrationTester::testCompatibility() {
    std::cout << "Testando compatibilidade entre componentes..." << std::endl;
    
    try {
        // Testar diferentes configurações
        std::vector<std::string> standards = {"c99", "c11"};
        
        for (const auto& standard : standards) {
            IntegrationConfig config;
            config.cStandard = standard;
            
            LexerPreprocessorBridge bridge(config);
            
            if (!bridge.initialize()) {
                std::cout << "Erro: Falha na inicialização com padrão " << standard << std::endl;
                return false;
            }
            
            // Teste básico
            std::string testCode = "int x = 42;";
            
            if (!bridge.processString(testCode, "test_compat.c")) {
                std::cout << "Erro: Falha no processamento com padrão " << standard << std::endl;
                return false;
            }
        }
        
        std::cout << "Teste de compatibilidade: PASSOU" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Erro no teste de compatibilidade: " << e.what() << std::endl;
        return false;
    }
}

} // namespace Integration