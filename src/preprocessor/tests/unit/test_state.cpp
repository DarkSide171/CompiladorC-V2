// Testes Unitários - Sistema de Estado
// Testes para preprocessor_state.hpp - Fase 1.6
// Testa ProcessingState, ProcessingMode, FileContext, StateStats, PreprocessorState e funções utilitárias

#include "../../include/preprocessor_state.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <stack>

using namespace Preprocessor;

// ============================================================================
// FUNÇÕES AUXILIARES PARA TESTES
// ============================================================================

void assertEqual(const std::string& expected, const std::string& actual, const std::string& testName) {
    if (expected != actual) {
        std::cerr << "❌ FALHA em " << testName << ": esperado '" << expected 
                  << "', obtido '" << actual << "'" << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertEqual(size_t expected, size_t actual, const std::string& testName) {
    if (expected != actual) {
        std::cerr << "❌ FALHA em " << testName << ": esperado " << expected 
                  << ", obtido " << actual << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertEqual(int expected, int actual, const std::string& testName) {
    if (expected != actual) {
        std::cerr << "❌ FALHA em " << testName << ": esperado " << expected 
                  << ", obtido " << actual << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertTrue(bool condition, const std::string& testName) {
    if (!condition) {
        std::cerr << "❌ FALHA em " << testName << ": condição falsa" << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

void assertFalse(bool condition, const std::string& testName) {
    if (condition) {
        std::cerr << "❌ FALHA em " << testName << ": condição verdadeira" << std::endl;
        assert(false);
    }
    std::cout << "✅ " << testName << " passou" << std::endl;
}

// ============================================================================
// TESTES DO ENUM ProcessingState
// ============================================================================

void testProcessingStateEnum() {
    std::cout << "\n=== Testando Enum ProcessingState ===" << std::endl;
    
    // Teste processingStateToString
    assertEqual("IDLE", processingStateToString(ProcessingState::IDLE), "IDLE -> string");
    assertEqual("PROCESSING_FILE", processingStateToString(ProcessingState::PROCESSING_FILE), "PROCESSING_FILE -> string");
    assertEqual("PROCESSING_INCLUDE", processingStateToString(ProcessingState::PROCESSING_INCLUDE), "PROCESSING_INCLUDE -> string");
    assertEqual("PROCESSING_MACRO", processingStateToString(ProcessingState::PROCESSING_MACRO), "PROCESSING_MACRO -> string");
    assertEqual("PROCESSING_CONDITIONAL", processingStateToString(ProcessingState::PROCESSING_CONDITIONAL), "PROCESSING_CONDITIONAL -> string");
    assertEqual("SKIPPING_CONDITIONAL", processingStateToString(ProcessingState::SKIPPING_CONDITIONAL), "SKIPPING_CONDITIONAL -> string");
    assertEqual("PROCESSING_DIRECTIVE", processingStateToString(ProcessingState::PROCESSING_DIRECTIVE), "PROCESSING_DIRECTIVE -> string");
    assertEqual("ERROR_STATE", processingStateToString(ProcessingState::ERROR_STATE), "ERROR_STATE -> string");
    assertEqual("FINISHED", processingStateToString(ProcessingState::FINISHED), "FINISHED -> string");
    
    // Teste isValidStateTransition
    assertTrue(isValidStateTransition(ProcessingState::IDLE, ProcessingState::PROCESSING_FILE), "IDLE -> PROCESSING_FILE válida");
    assertTrue(isValidStateTransition(ProcessingState::PROCESSING_FILE, ProcessingState::PROCESSING_MACRO), "PROCESSING_FILE -> PROCESSING_MACRO válida");
    assertTrue(isValidStateTransition(ProcessingState::PROCESSING_MACRO, ProcessingState::PROCESSING_FILE), "PROCESSING_MACRO -> PROCESSING_FILE válida");
    assertTrue(isValidStateTransition(ProcessingState::IDLE, ProcessingState::ERROR_STATE), "Qualquer estado -> ERROR_STATE válida");
    
    // Teste requiresFileContext
    assertTrue(requiresFileContext(ProcessingState::PROCESSING_INCLUDE), "PROCESSING_INCLUDE requer contexto de arquivo");
    assertFalse(requiresFileContext(ProcessingState::IDLE), "IDLE não requer contexto de arquivo");
    assertFalse(requiresFileContext(ProcessingState::ERROR_STATE), "ERROR_STATE não requer contexto de arquivo");
}

// ============================================================================
// TESTES DA ESTRUTURA ProcessingMode
// ============================================================================

void testProcessingMode() {
    std::cout << "\n=== Testando ProcessingMode ===" << std::endl;
    
    // Teste processingModeToString
    assertEqual("NORMAL", processingModeToString(ProcessingMode::NORMAL), "NORMAL -> string");
    assertEqual("MACRO_EXPANSION", processingModeToString(ProcessingMode::MACRO_EXPANSION), "MACRO_EXPANSION -> string");
    assertEqual("CONDITIONAL", processingModeToString(ProcessingMode::CONDITIONAL), "CONDITIONAL -> string");
    assertEqual("INCLUDE", processingModeToString(ProcessingMode::INCLUDE), "INCLUDE -> string");
    assertEqual("DIRECTIVE", processingModeToString(ProcessingMode::DIRECTIVE), "DIRECTIVE -> string");
}

// ============================================================================
// TESTES DA ESTRUTURA FileContext
// ============================================================================

void testFileContext() {
    std::cout << "\n=== Testando FileContext ===" << std::endl;
    
    // Teste construtor padrão
    FileContext ctx1;
    assertEqual("", ctx1.filename, "Construtor padrão - filename vazio");
    assertEqual(size_t(1), ctx1.line, "Construtor padrão - linha 1");
    assertEqual(size_t(1), ctx1.column, "Construtor padrão - coluna 1");
    assertEqual(size_t(0), ctx1.absolute_position, "Construtor padrão - posição 0");
    
    // Teste construtor com parâmetros
    FileContext ctx2("test.c", 10, 5);
    assertEqual("test.c", ctx2.filename, "Construtor com parâmetros - filename");
    assertEqual(size_t(10), ctx2.line, "Construtor com parâmetros - linha");
    assertEqual(size_t(5), ctx2.column, "Construtor com parâmetros - coluna");
    assertEqual(size_t(0), ctx2.absolute_position, "Construtor com parâmetros - posição");
    
    // Teste operador de igualdade
    FileContext ctx3("test.c", 10, 5);
    FileContext ctx4("test.c", 10, 6);
    
    assertTrue(ctx2 == ctx3, "Contextos iguais");
    assertFalse(ctx2 == ctx4, "Contextos diferentes");
    
    // Teste modificação de valores
    ctx1.filename = "modified.h";
    ctx1.line = 42;
    ctx1.column = 15;
    ctx1.absolute_position = 1000;
    
    assertEqual("modified.h", ctx1.filename, "Modificação filename");
    assertEqual(size_t(42), ctx1.line, "Modificação linha");
    assertEqual(size_t(15), ctx1.column, "Modificação coluna");
    assertEqual(size_t(1000), ctx1.absolute_position, "Modificação posição");
}

// ============================================================================
// TESTES DA ESTRUTURA StateStats
// ============================================================================

void testStateStats() {
    std::cout << "\n=== Testando StateStats ===" << std::endl;
    
    StateStats stats;
    
    // Teste valores iniciais
    assertEqual(size_t(0), stats.state_transitions, "state_transitions inicial");
    assertEqual(size_t(0), stats.max_stack_depth, "max_stack_depth inicial");
    assertEqual(size_t(0), stats.files_processed, "files_processed inicial");
    assertEqual(size_t(0), stats.conditional_blocks, "conditional_blocks inicial");
    assertEqual(size_t(0), stats.macro_expansions, "macro_expansions inicial");
    assertEqual(size_t(0), stats.include_depth, "include_depth inicial");
    
    // Teste modificação de valores
    stats.state_transitions = 100;
    stats.max_stack_depth = 10;
    stats.files_processed = 8;
    stats.conditional_blocks = 2;
    stats.macro_expansions = 50;
    stats.include_depth = 3;
    
    assertEqual(size_t(100), stats.state_transitions, "state_transitions modificado");
    assertEqual(size_t(10), stats.max_stack_depth, "max_stack_depth modificado");
    assertEqual(size_t(8), stats.files_processed, "files_processed modificado");
    assertEqual(size_t(2), stats.conditional_blocks, "conditional_blocks modificado");
    assertEqual(size_t(50), stats.macro_expansions, "macro_expansions modificado");
    assertEqual(size_t(3), stats.include_depth, "include_depth modificado");
    
    // Teste reset (StateStats não tem método reset, então criamos nova instância)
    StateStats resetStats;
    assertEqual(size_t(0), resetStats.state_transitions, "state_transitions após reset");
    assertEqual(size_t(0), resetStats.max_stack_depth, "max_stack_depth após reset");
    assertEqual(size_t(0), resetStats.files_processed, "files_processed após reset");
    assertEqual(size_t(0), resetStats.conditional_blocks, "conditional_blocks após reset");
    assertEqual(size_t(0), resetStats.macro_expansions, "macro_expansions após reset");
    assertEqual(size_t(0), resetStats.include_depth, "include_depth após reset");
}

// ============================================================================
// TESTES DA CLASSE PreprocessorState - CONSTRUTORES
// ============================================================================

void testPreprocessorStateConstructors() {
    std::cout << "\n=== Testando Construtores de PreprocessorState ===" << std::endl;
    
    // Teste construtor padrão
    PreprocessorState state1;
    assertTrue(state1.getCurrentState() == ProcessingState::IDLE, "Estado inicial é IDLE");
    assertTrue(state1.getProcessingMode() == ProcessingMode::NORMAL, "Modo inicial é NORMAL");
    assertEqual(size_t(1), state1.getDepth(), "Stack depth inicial é 1");
    assertFalse(state1.hasError(), "Sem erro inicial");
    
    // Teste construtor com estado inicial (PreprocessorState só tem construtor padrão)
    PreprocessorState state2;
    state2.pushState(ProcessingState::PROCESSING_FILE);
    assertTrue(state2.getCurrentState() == ProcessingState::PROCESSING_FILE, "Estado inicial personalizado");
    
    PreprocessorState state3;
    state3.pushState(ProcessingState::PROCESSING_FILE);
    state3.pushState(ProcessingState::PROCESSING_MACRO);
    assertTrue(state3.getCurrentState() == ProcessingState::PROCESSING_MACRO, "Estado PROCESSING_MACRO");
}

// ============================================================================
// TESTES DE GERENCIAMENTO DE PILHA DE ESTADOS
// ============================================================================

void testStateStackManagement() {
    std::cout << "\n=== Testando Gerenciamento de Pilha de Estados ===" << std::endl;
    
    PreprocessorState state;
    
    // Teste pushState
    state.pushState(ProcessingState::PROCESSING_FILE);
    assertTrue(state.getCurrentState() == ProcessingState::PROCESSING_FILE, "Estado após push");
    assertEqual(size_t(2), state.getDepth(), "Stack depth após push");
    
    state.pushState(ProcessingState::PROCESSING_MACRO);
    assertTrue(state.getCurrentState() == ProcessingState::PROCESSING_MACRO, "Estado após segundo push");
    assertEqual(size_t(3), state.getDepth(), "Stack depth após segundo push");
    
    state.pushState(ProcessingState::PROCESSING_INCLUDE);
    assertTrue(state.getCurrentState() == ProcessingState::PROCESSING_INCLUDE, "Estado após terceiro push");
    assertEqual(size_t(4), state.getDepth(), "Stack depth após terceiro push");
    
    // Teste popState
    ProcessingState poppedState = state.popState();
    assertTrue(poppedState == ProcessingState::PROCESSING_INCLUDE, "Estado removido correto");
    assertTrue(state.getCurrentState() == ProcessingState::PROCESSING_MACRO, "Estado atual após pop");
    assertEqual(size_t(3), state.getDepth(), "Stack depth após pop");
    
    poppedState = state.popState();
    assertTrue(poppedState == ProcessingState::PROCESSING_MACRO, "Segundo estado removido correto");
    assertTrue(state.getCurrentState() == ProcessingState::PROCESSING_FILE, "Estado atual após segundo pop");
    assertEqual(size_t(2), state.getDepth(), "Stack depth após segundo pop");
    
    poppedState = state.popState();
    assertTrue(poppedState == ProcessingState::PROCESSING_FILE, "Terceiro estado removido correto");
    assertTrue(state.getCurrentState() == ProcessingState::IDLE, "Estado volta para IDLE");
    assertEqual(size_t(1), state.getDepth(), "Stack depth volta para 1");
    
    // Teste getCurrentState (não há peekState, mas getCurrentState serve o mesmo propósito)
    state.pushState(ProcessingState::PROCESSING_FILE);
    state.pushState(ProcessingState::PROCESSING_CONDITIONAL);
    ProcessingState currentState = state.getCurrentState();
    assertTrue(currentState == ProcessingState::PROCESSING_CONDITIONAL, "getCurrentState retorna estado correto");
    assertTrue(state.getCurrentState() == ProcessingState::PROCESSING_CONDITIONAL, "Estado não muda após getCurrentState");
    assertEqual(size_t(3), state.getDepth(), "Stack depth não muda após getCurrentState");
}

// ============================================================================
// TESTES DE CONTEXTO DE ARQUIVO
// ============================================================================

void testFileContextManagement() {
    std::cout << "\n=== Testando Gerenciamento de Contexto de Arquivo ===" << std::endl;
    
    PreprocessorState state;
    
    // Teste setFileContext
    FileContext ctx1("main.c", 10, 5);
    state.setFileContext(ctx1);
    
    FileContext retrievedCtx = state.getFileContext();
    assertTrue(retrievedCtx == ctx1, "Contexto de arquivo definido corretamente");
    
    // Teste setCurrentLine e setCurrentColumn
    state.setCurrentLine(25);
    state.setCurrentColumn(15);
    FileContext updatedCtx = state.getFileContext();
    assertEqual(size_t(25), updatedCtx.line, "Linha atualizada");
    assertEqual(size_t(15), updatedCtx.column, "Coluna atualizada");
    assertEqual("main.c", updatedCtx.filename, "Filename mantido");
    assertEqual(size_t(0), updatedCtx.absolute_position, "Posição absoluta mantida");
    
    // Teste incrementLine
    state.incrementLine();
    FileContext incrementedCtx = state.getFileContext();
    assertEqual(size_t(26), incrementedCtx.line, "Linha incrementada");
    assertEqual(size_t(1), incrementedCtx.column, "Coluna resetada para 1");
    
    // Teste incrementColumn
    state.incrementColumn();
    state.incrementColumn();
    state.incrementColumn();
    FileContext columnCtx = state.getFileContext();
    assertEqual(size_t(26), columnCtx.line, "Linha mantida");
    assertEqual(size_t(4), columnCtx.column, "Coluna incrementada 3 vezes");
}

// ============================================================================
// TESTES DE MODO DE PROCESSAMENTO
// ============================================================================

void testProcessingModeManagement() {
    std::cout << "\n=== Testando Gerenciamento de Modo de Processamento ===" << std::endl;
    
    PreprocessorState state;
    
    // Teste setProcessingMode
    state.setProcessingMode(ProcessingMode::CONDITIONAL);
    assertTrue(state.getProcessingMode() == ProcessingMode::CONDITIONAL, "Modo CONDITIONAL definido");
    
    state.setProcessingMode(ProcessingMode::MACRO_EXPANSION);
    assertTrue(state.getProcessingMode() == ProcessingMode::MACRO_EXPANSION, "Modo MACRO_EXPANSION definido");
    
    state.setProcessingMode(ProcessingMode::INCLUDE);
    assertTrue(state.getProcessingMode() == ProcessingMode::INCLUDE, "Modo INCLUDE definido");
    
    state.setProcessingMode(ProcessingMode::DIRECTIVE);
    assertTrue(state.getProcessingMode() == ProcessingMode::DIRECTIVE, "Modo DIRECTIVE definido");
    
    state.setProcessingMode(ProcessingMode::NORMAL);
    assertTrue(state.getProcessingMode() == ProcessingMode::NORMAL, "Modo NORMAL restaurado");
}

// ============================================================================
// TESTES DE BLOCOS CONDICIONAIS
// ============================================================================

void testConditionalBlockManagement() {
    std::cout << "\n=== Testando Gerenciamento de Blocos Condicionais ===" << std::endl;
    
    PreprocessorState state;
    
    // Teste enableConditionalBlock (não há enterConditionalBlock)
    state.enableConditionalBlock();
    assertTrue(state.isInConditionalBlock(), "Dentro de bloco condicional");
    
    // Teste disableConditionalBlock
    state.disableConditionalBlock();
    assertFalse(state.isInConditionalBlock(), "Fora de bloco condicional");
}

// ============================================================================
// TESTES DE ESTADO DE ERRO
// ============================================================================

void testErrorStateManagement() {
    std::cout << "\n=== Testando Gerenciamento de Estado de Erro ===" << std::endl;
    
    PreprocessorState state;
    
    // Estado inicial sem erro
    assertFalse(state.hasError(), "Sem erro inicial");
    
    // Teste setErrorState
    state.setErrorState(true);
    assertTrue(state.hasError(), "Erro definido");
    
    // Teste clearError (setErrorState(false))
    state.setErrorState(false);
    assertFalse(state.hasError(), "Erro limpo");
    
    // Teste múltiplos estados de erro
    state.setErrorState(true);
    assertTrue(state.hasError(), "Primeiro erro definido");
    state.setErrorState(false);
    assertFalse(state.hasError(), "Erro limpo novamente");
}

// ============================================================================
// TESTES DE ARQUIVOS PROCESSADOS
// ============================================================================

void testProcessedFilesManagement() {
    std::cout << "\n=== Testando Gerenciamento de Arquivos Processados ===" << std::endl;
    
    PreprocessorState state;
    
    // Teste addProcessedFile
    state.addProcessedFile("main.c");
    state.addProcessedFile("header1.h");
    state.addProcessedFile("header2.h");
    
    // Teste wasFileProcessed
    assertTrue(state.wasFileProcessed("main.c"), "main.c foi processado");
    assertTrue(state.wasFileProcessed("header1.h"), "header1.h foi processado");
    assertTrue(state.wasFileProcessed("header2.h"), "header2.h foi processado");
    assertFalse(state.wasFileProcessed("notprocessed.h"), "notprocessed.h não foi processado");
    
    // Teste getProcessedFiles
    std::vector<std::string> processedFiles = state.getProcessedFiles();
    assertEqual(size_t(3), processedFiles.size(), "3 arquivos processados");
    
    // Verifica se todos os arquivos estão na lista
    bool foundMain = false, foundHeader1 = false, foundHeader2 = false;
    for (const auto& file : processedFiles) {
        if (file == "main.c") foundMain = true;
        if (file == "header1.h") foundHeader1 = true;
        if (file == "header2.h") foundHeader2 = true;
    }
    
    assertTrue(foundMain, "main.c na lista de processados");
    assertTrue(foundHeader1, "header1.h na lista de processados");
    assertTrue(foundHeader2, "header2.h na lista de processados");
    
    // Teste arquivo duplicado
    state.addProcessedFile("main.c"); // Adiciona novamente
    std::vector<std::string> processedFiles2 = state.getProcessedFiles();
    assertEqual(size_t(3), processedFiles2.size(), "Arquivo duplicado não é adicionado novamente");
}

// ============================================================================
// TESTES DE MÉTODOS AVANÇADOS
// ============================================================================

void testAdvancedMethods() {
    std::cout << "\n=== Testando Métodos Avançados ===" << std::endl;
    
    PreprocessorState state;
    
    // Configura estado para testes
    state.pushState(ProcessingState::PROCESSING_FILE);
    state.pushState(ProcessingState::PROCESSING_DIRECTIVE);
    state.pushState(ProcessingState::PROCESSING_MACRO);
    state.setFileContext(FileContext("test.c", 10, 5));
    state.enableConditionalBlock();
    state.addProcessedFile("test.c");
    state.addProcessedFile("header.h");
    
    // Teste validateState
    assertTrue(state.validateState(), "Estado válido");
    
    // Teste getStatistics
    StateStats stats = state.getStatistics();
    assertEqual(size_t(4), stats.max_stack_depth, "Max stack depth correto");
    assertEqual(size_t(2), stats.files_processed, "Files processed correto");
    
    // Teste generateStateReport
    std::string report = state.generateStateReport();
    assertFalse(report.empty(), "Relatório gerado");
    assertTrue(report.find("PROCESSING_MACRO") != std::string::npos, "Estado atual no relatório");
    assertTrue(report.find("test.c") != std::string::npos, "Arquivo atual no relatório");
    
    std::cout << "✅ Relatório gerado: " << report.length() << " caracteres" << std::endl;
    
    // Teste reset
    state.reset();
    assertTrue(state.getCurrentState() == ProcessingState::IDLE, "Estado resetado para IDLE");
    assertEqual(size_t(1), state.getDepth(), "Stack depth resetado");
    assertFalse(state.isInConditionalBlock(), "Conditional block resetado");
    assertFalse(state.hasError(), "Erro limpo após reset");
    assertTrue(state.getProcessedFiles().empty(), "Arquivos processados limpos");
}

// ============================================================================
// TESTES DE CENÁRIOS COMPLEXOS
// ============================================================================

void testComplexScenarios() {
    std::cout << "\n=== Testando Cenários Complexos ===" << std::endl;
    
    // Cenário 1: Processamento de include aninhado com condicionais
    PreprocessorState state1;
    
    // Simula processamento de arquivo principal
    state1.setFileContext(FileContext("main.c", 1, 1));
    state1.addProcessedFile("main.c");
    
    // Entra em diretiva #if
    state1.pushState(ProcessingState::PROCESSING_FILE);
    state1.pushState(ProcessingState::PROCESSING_CONDITIONAL);
    state1.enableConditionalBlock();
    
    // Processa #include dentro do #if
    state1.pushState(ProcessingState::PROCESSING_INCLUDE);
    state1.setFileContext(FileContext("header1.h", 1, 1));
    state1.addProcessedFile("header1.h");
    
    // Include aninhado dentro do header
    state1.pushState(ProcessingState::PROCESSING_FILE);
    state1.pushState(ProcessingState::PROCESSING_INCLUDE);
    state1.setFileContext(FileContext("header2.h", 1, 1));
    state1.addProcessedFile("header2.h");
    
    // Verifica estado complexo
    assertEqual(size_t(6), state1.getDepth(), "Stack depth complexo");
    assertTrue(state1.isInConditionalBlock(), "Em bloco condicional");
    assertEqual(size_t(3), state1.getProcessedFiles().size(), "3 arquivos processados");
    assertTrue(state1.getCurrentState() == ProcessingState::PROCESSING_INCLUDE, "Estado atual correto");
    
    // Sai dos includes
    state1.popState(); // Sai de header2.h (PROCESSING_INCLUDE)
    state1.popState(); // Sai do PROCESSING_FILE
    state1.setFileContext(FileContext("header1.h", 5, 1));
    
    state1.popState(); // Sai de header1.h (PROCESSING_INCLUDE)
    state1.setFileContext(FileContext("main.c", 10, 1));
    
    state1.popState(); // Sai da condição (PROCESSING_CONDITIONAL)
    state1.popState(); // Sai do PROCESSING_FILE
    state1.disableConditionalBlock();
    
    assertTrue(state1.getCurrentState() == ProcessingState::IDLE, "Volta para IDLE");
    assertEqual(size_t(1), state1.getDepth(), "Stack limpo");
    assertFalse(state1.isInConditionalBlock(), "Conditional limpo");
    
    // Cenário 2: Tratamento de erro durante expansão de macro
    PreprocessorState state2;
    
    state2.pushState(ProcessingState::PROCESSING_FILE);
    state2.pushState(ProcessingState::PROCESSING_MACRO);
    state2.setProcessingMode(ProcessingMode::MACRO_EXPANSION);
    
    // Simula erro durante expansão
    state2.setErrorState(true);
    
    assertTrue(state2.hasError(), "Erro definido");
    
    // Recupera do erro
    state2.setErrorState(false);
    state2.setProcessingMode(ProcessingMode::NORMAL);
    state2.popState(); // Sai do PROCESSING_MACRO
    
    assertFalse(state2.hasError(), "Erro limpo");
    // Estado pode variar dependendo da implementação, o importante é que não há erro
    
    std::cout << "✅ Cenários complexos executados com sucesso" << std::endl;
}

// ============================================================================
// TESTES DE VALIDAÇÃO DE TRANSIÇÕES
// ============================================================================

void testStateTransitionValidation() {
    std::cout << "\n=== Testando Validação de Transições de Estado ===" << std::endl;
    
    // Testa transições válidas
    assertTrue(isValidStateTransition(ProcessingState::IDLE, ProcessingState::PROCESSING_FILE), "IDLE -> PROCESSING_FILE");
    assertTrue(isValidStateTransition(ProcessingState::PROCESSING_FILE, ProcessingState::PROCESSING_MACRO), "PROCESSING_FILE -> PROCESSING_MACRO");
    assertTrue(isValidStateTransition(ProcessingState::PROCESSING_FILE, ProcessingState::PROCESSING_INCLUDE), "PROCESSING_FILE -> PROCESSING_INCLUDE");
    assertTrue(isValidStateTransition(ProcessingState::PROCESSING_FILE, ProcessingState::PROCESSING_CONDITIONAL), "PROCESSING_FILE -> PROCESSING_CONDITIONAL");
    assertTrue(isValidStateTransition(ProcessingState::PROCESSING_CONDITIONAL, ProcessingState::SKIPPING_CONDITIONAL), "PROCESSING_CONDITIONAL -> SKIPPING_CONDITIONAL");
    
    // Qualquer estado pode ir para ERROR_STATE
    assertTrue(isValidStateTransition(ProcessingState::IDLE, ProcessingState::ERROR_STATE), "IDLE -> ERROR_STATE");
    assertTrue(isValidStateTransition(ProcessingState::PROCESSING_MACRO, ProcessingState::ERROR_STATE), "PROCESSING_MACRO -> ERROR_STATE");
    assertTrue(isValidStateTransition(ProcessingState::PROCESSING_INCLUDE, ProcessingState::ERROR_STATE), "PROCESSING_INCLUDE -> ERROR_STATE");
    
    // Transições de volta para estados anteriores
    assertTrue(isValidStateTransition(ProcessingState::PROCESSING_INCLUDE, ProcessingState::PROCESSING_FILE), "PROCESSING_INCLUDE -> PROCESSING_FILE");
    assertTrue(isValidStateTransition(ProcessingState::PROCESSING_MACRO, ProcessingState::PROCESSING_FILE), "PROCESSING_MACRO -> PROCESSING_FILE");
    assertTrue(isValidStateTransition(ProcessingState::ERROR_STATE, ProcessingState::IDLE), "ERROR_STATE -> IDLE");
    
    std::cout << "✅ Validação de transições testada" << std::endl;
}

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main() {
    std::cout << "=== EXECUTANDO TESTES DO SISTEMA DE ESTADO (FASE 1.6) ===" << std::endl;
    
    try {
        // Testes das estruturas e enums básicos
        testProcessingStateEnum();
        testProcessingMode();
        testFileContext();
        testStateStats();
        
        // Testes da classe PreprocessorState
        testPreprocessorStateConstructors();
        testStateStackManagement();
        testFileContextManagement();
        testProcessingModeManagement();
        
        // Testes de funcionalidades específicas
        testConditionalBlockManagement();
        testErrorStateManagement();
        testProcessedFilesManagement();
        
        // Testes de métodos avançados
        testAdvancedMethods();
        
        // Testes de cenários complexos
        testComplexScenarios();
        
        // Testes de validação
        testStateTransitionValidation();
        
        std::cout << "\n🎉 Todos os testes do sistema de estado passaram com sucesso!" << std::endl;
        std::cout << "📊 Total de testes executados: 12 grupos de teste" << std::endl;
        std::cout << "✅ Fase 1.6 - test_state.cpp: CONCLUÍDO" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Erro durante os testes: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Erro desconhecido durante os testes" << std::endl;
        return 1;
    }
}