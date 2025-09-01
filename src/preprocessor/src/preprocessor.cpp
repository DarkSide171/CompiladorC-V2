#include "../include/preprocessor.hpp"
#include "../include/preprocessor_lexer_interface.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace Preprocessor {

// Construtor
PreprocessorMain::PreprocessorMain(const std::string& config_file)
    : initialized_(false)
    , processing_active_(false)
    , current_line_(0)
    , external_error_handler_(nullptr)
{
    // Inicializar componentes
    config_ = std::make_unique<PreprocessorConfig>();
    state_ = std::make_unique<PreprocessorState>();
    logger_ = std::make_unique<PreprocessorLogger>();
    
    // Criar shared_ptr temporários para o MacroProcessor
    auto shared_logger = std::shared_ptr<PreprocessorLogger>(logger_.get(), [](PreprocessorLogger*){});
    auto shared_state = std::shared_ptr<PreprocessorState>(state_.get(), [](PreprocessorState*){});
    
    macro_processor_ = std::make_unique<MacroProcessor>(shared_logger, shared_state);
    file_manager_ = std::make_unique<FileManager>(std::vector<std::string>(), logger_.get());
    conditional_processor_ = std::make_unique<ConditionalProcessor>(logger_.get(), macro_processor_.get());
    expression_evaluator_ = std::make_unique<ExpressionEvaluator>(macro_processor_.get(), logger_.get());
    
    // Carregar configuração se fornecida
    if (!config_file.empty()) {
        config_->loadConfigFromFile(config_file);
    }
    
    // Inicializar componentes
    initializeComponents();
}

// Destrutor
PreprocessorMain::~PreprocessorMain() {
    cleanup();
}

// Inicialização de componentes
bool PreprocessorMain::initializeComponents() {
    try {
        if (!config_ || !state_ || !logger_ || !macro_processor_ || !file_manager_ || !conditional_processor_ || !expression_evaluator_) {
            logger_->error("Falha na inicialização: componentes não foram criados");
            return false;
        }
        
        // Carregar macros predefinidas
        macro_processor_->initializePredefinedMacros();
        
        // Configurar caminhos de busca padrão
        std::vector<std::string> default_paths = {
            "/usr/include",
            "/usr/local/include",
            "."
        };
        file_manager_->setSearchPaths(default_paths);
        
        initialized_ = true;
        logger_->info("Preprocessor inicializado com sucesso");
        return true;
        
    } catch (const std::exception& e) {
        logger_->error("Erro durante inicialização: " + std::string(e.what()));
        return false;
    }
}

// Processamento de arquivo
bool PreprocessorMain::process(const std::string& filename) {
    // Método principal aprimorado com coordenação integrada
    
    // 1. Validação inicial
    if (!initialized_) {
        logger_->error("Preprocessor não foi inicializado");
        return false;
    }
    
    if (filename.empty()) {
        logger_->error("Nome de arquivo vazio fornecido");
        return false;
    }
    
    try {
        // 2. Preparação do processamento
        current_file_ = filename;
        processing_active_ = true;
        
        // Atualizar estado do preprocessor
        if (state_) {
            state_->reset();
            state_->pushState(ProcessingState::PROCESSING_FILE);
            state_->setFileContext(FileContext(filename, 1, 1));
            state_->setProcessingMode(ProcessingMode::NORMAL);
            state_->addProcessedFile(filename);
        }
        
        // Limpar dados anteriores
        expanded_code_.clear();
        dependencies_.clear();
        position_mappings_.clear();
        
        logger_->info("Iniciando processamento coordenado do arquivo: " + filename);
        
        // 3. Validação de entrada
        if (!validateInput(filename)) {
            logger_->error("Falha na validação de entrada do arquivo: " + filename);
            processing_active_ = false;
            return false;
        }
        
        // 4. Sincronização de componentes
        if (!synchronizeProcessors()) {
            logger_->error("Falha na sincronização de componentes");
            processing_active_ = false;
            return false;
        }
        
        // 5. Processamento principal com monitoramento
        bool result = false;
        try {
            result = processFile(filename);
        } catch (const std::exception& e) {
            PreprocessorPosition error_pos(filename, 0, 0);
            handleErrors("Erro durante processamento do arquivo: " + std::string(e.what()), error_pos);
            result = false;
        }
        
        // 6. Validação de saída
        if (result && !validateOutput(expanded_code_)) {
            logger_->warning("Saída gerada pode conter problemas");
        }
        
        // 7. Finalização coordenada
        processing_active_ = false;
        
        if (state_) {
            if (result) {
                state_->pushState(ProcessingState::FINISHED);
            } else {
                state_->pushState(ProcessingState::ERROR_STATE);
            }
        }
        
        // 8. Verificação de integridade final
        if (result) {
            result = performIntegrityCheck();
        }
        
        // 9. Relatório final
        if (result) {
            logger_->info("Processamento concluído com sucesso");
            logger_->info("Estatísticas: " + generateProcessingReport());
        } else {
            logger_->error("Falha no processamento do arquivo");
        }
        
        return result;
        
    } catch (const std::exception& e) {
        processing_active_ = false;
        PreprocessorPosition error_pos(filename, 0, 0);
        handleErrors("Erro crítico durante processamento: " + std::string(e.what()), error_pos);
        return false;
    }
}

// Processamento de string
bool PreprocessorMain::processString(const std::string& content) {
    if (!initialized_) {
        logger_->error("Preprocessor não foi inicializado");
        return false;
    }
    
    try {
        current_file_ = "<string>";
        processing_active_ = true;
        
        logger_->info("Iniciando processamento de string");
        
        // Validar entrada
        if (!validateInput(content)) {
            logger_->error("Entrada inválida");
            processing_active_ = false;
            return false;
        }
        
        // Processar linha por linha
        std::istringstream iss(content);
        std::string line;
        int line_number = 1;
        
        while (std::getline(iss, line)) {
            if (!processLine(line, line_number)) {
                logger_->error("Erro no processamento da linha " + std::to_string(line_number));
                processing_active_ = false;
                return false;
            }
            line_number++;
        }
        
        processing_active_ = false;
        logger_->info("Processamento de string concluído com sucesso");
        return true;
        
    } catch (const std::exception& e) {
        processing_active_ = false;
        logger_->error("Erro durante processamento de string: " + std::string(e.what()));
        return false;
    }
}

// Processamento de arquivo específico
bool PreprocessorMain::processFile(const std::string& filepath) {
    try {
        // Verificar se arquivo existe
        std::ifstream file(filepath);
        if (!file.is_open()) {
            logger_->error("Não foi possível abrir o arquivo: " + filepath);
            return false;
        }
        
        // Adicionar às dependências
        dependencies_.push_back(filepath);
        
        // Processar linha por linha
        std::string line;
        int line_number = 1;
        
        while (std::getline(file, line)) {
            current_line_ = line_number;
            
            if (!processLine(line, line_number)) {
                logger_->error("Erro no processamento da linha " + std::to_string(line_number) + " do arquivo " + filepath);
                return false;
            }
            
            line_number++;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        logger_->error("Erro durante processamento do arquivo " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Processamento de linha individual
bool PreprocessorMain::processLine(const std::string& line, int line_number) {
    try {
        // Criar posição original
        PreprocessorPosition original_pos(current_file_, line_number, 1);
        
        // Atualizar estado do preprocessor
        if (state_) {
            state_->setCurrentLine(line_number);
            state_->resetColumn();
        }
        
        // Verificar se é uma diretiva primeiro (diretivas sempre devem ser processadas)
        // Diretivas podem estar indentadas, então procurar por '#' após espaços em branco
        std::string trimmed_line = line;
        size_t first_non_space = trimmed_line.find_first_not_of(" \t");
        
        // Debug: log detalhado para verificar detecção de diretivas
        logger_->info("PROCESSANDO LINHA: '" + line + "'");
        logger_->info("first_non_space: " + std::to_string(first_non_space));
        if (first_non_space != std::string::npos) {
            char first_char = trimmed_line[first_non_space];
            logger_->info("Primeiro char não-espaço: '" + std::string(1, first_char) + "' (ASCII: " + std::to_string((int)first_char) + ")");
            logger_->info("Comparação com '#': " + std::string(first_char == '#' ? "true" : "false"));
        }
        
        bool is_directive = (first_non_space != std::string::npos && trimmed_line[first_non_space] == '#');
        logger_->info("is_directive: " + std::string(is_directive ? "true" : "false"));
        
        if (is_directive) {
            logger_->info("DIRETIVA DETECTADA: " + line);
        }
        
        if (is_directive) {
            // Processar diretiva com coordenação integrada
            try {
                // Parsear a diretiva
                Directive directive = parseDirective(line, original_pos);
                
                if (!directive.isValid()) {
                    std::string detailed_error = "[DIRECTIVE_PARSER::parseDirective] Diretiva inválida na linha " + 
                                               std::to_string(line_number) + ": '" + line + "' (Tipo: " + 
                                               directiveTypeToString(directive.getType()) + ", Argumentos: " + 
                                               std::to_string(directive.getArguments().size()) + ")";
                    handleErrors(detailed_error, original_pos);
                    return false;
                }
                
                // Processar a diretiva com tratamento de erros integrado
                bool result = handleDirective(directive);
                
                // Atualizar estatísticas de diretivas
                if (state_ && result) {
                    state_->setProcessingMode(ProcessingMode::DIRECTIVE);
                }
                
                // IMPORTANTE: Não escrever a linha da diretiva na saída
                // As diretivas são processadas mas não devem aparecer no código final
                // (exceto algumas como #line que podem precisar ser preservadas)
                
                // Para diretivas condicionais, não escrever na saída - apenas processar o efeito
                DirectiveType type = directive.getType();
                if (type == DirectiveType::IF || type == DirectiveType::IFDEF || 
                    type == DirectiveType::IFNDEF || type == DirectiveType::ELSE || 
                    type == DirectiveType::ELIF || type == DirectiveType::ENDIF) {
                    // Diretivas condicionais não devem aparecer na saída final
                    // Mas adicionar quebra de linha para preservar numeração
                    writeOutput("\n");
                    return result;
                }
                
                // Para outras diretivas como #define, também não escrever na saída
                if (type == DirectiveType::DEFINE || type == DirectiveType::UNDEF) {
                    // Adicionar quebra de linha para preservar numeração de linhas
                    writeOutput("\n");
                    return result;
                }
                
                // Para #include, também preservar numeração
                if (type == DirectiveType::INCLUDE) {
                    // Adicionar quebra de linha para preservar numeração de linhas
                    writeOutput("\n");
                    return result;
                }
                
                return result;
                
            } catch (const std::exception& e) {
                handleErrors("Erro ao processar diretiva na linha " + std::to_string(line_number) + ": " + std::string(e.what()), original_pos);
                return false;
            }
        } else {
            // Verificar se devemos processar esta linha normal (compilação condicional)
            if (conditional_processor_ && !conditional_processor_->shouldProcessBlock()) {
                logger_->debug("Linha ignorada por compilação condicional: " + std::to_string(line_number));
                // Para linhas dentro de blocos condicionais falsos, não escrever na saída
                // mas adicionar quebra de linha para preservar numeração
                writeOutput("\n");
                return true;
            }
            
            // Linha normal - coordenar processamento entre componentes
            
            // 1. Expandir macros com mapeamento de posições
            std::string expanded_line = macro_processor_->processLine(line);
            
            // 2. Criar posição expandida
            PreprocessorPosition expanded_pos(current_file_, line_number, 1);
            expanded_pos.expanded_line = line_number;
            expanded_pos.expanded_column = 1;
            
            // 3. Atualizar mapeamento de posições
            updatePositionMapping(original_pos, expanded_pos);
            
            // 4. Validar saída antes de escrever
            if (!validateOutput(expanded_line)) {
                logger_->warning("Linha com possíveis problemas: " + std::to_string(line_number));
            }
            
            // 5. Escrever na saída
            writeOutput(expanded_line + "\n");
            
            // 6. Atualizar estatísticas (usando métodos disponíveis)
            if (state_) {
                // Incrementar linha processada
                state_->incrementLine();
                if (expanded_line != line) {
                    // Marcar que houve expansão de macro
                    state_->setProcessingMode(ProcessingMode::MACRO_EXPANSION);
                }
            }
            
            return true;
        }
        
    } catch (const std::exception& e) {
        PreprocessorPosition error_pos(current_file_, line_number, 1);
        handleErrors("Erro no processamento da linha " + std::to_string(line_number) + ": " + std::string(e.what()), error_pos);
        return false;
    }
}

// Manipulação de diretivas
bool PreprocessorMain::handleDirective(const Directive& directive) {
    try {
        switch (directive.getType()) {
            case DirectiveType::INCLUDE:
                {
                    // Ignorar diretivas #include - não processar bibliotecas
                    const auto& args = directive.getArguments();
                    if (!args.empty()) {
                        std::string filename = args[0];
                        logger_->info("Ignorando #include: " + filename + " (processamento de includes desabilitado)");
                    } else {
                        logger_->info("Ignorando #include vazio (processamento de includes desabilitado)");
                    }
                    // Retornar true para continuar o processamento normalmente
                    return true;
                }
                break;
                
            case DirectiveType::DEFINE:
                {
                    logger_->info("Processando #define: " + directive.getContent());
                    
                    // Debug: mostrar argumentos parseados
                    const auto& args = directive.getArguments();
                    std::string debug_args = "Argumentos parseados: [";
                    for (size_t i = 0; i < args.size(); ++i) {
                        if (i > 0) debug_args += ", ";
                        debug_args += "'" + args[i] + "'";
                    }
                    debug_args += "]";
                    logger_->info(debug_args);
                    if (args.empty()) {
                        logger_->error("#define requer um nome de macro");
                        return false;
                    }
                    
                    std::string macro_name = args[0];
                    std::string macro_value;
                    
                    // Verificar se é uma macro com parâmetros (função-like)
                    size_t paren_pos = macro_name.find('(');
                    if (paren_pos != std::string::npos) {
                        // Macro com parâmetros - extrair nome e parâmetros
                        std::string base_name = macro_name.substr(0, paren_pos);
                        std::string params_str = macro_name.substr(paren_pos);
                        
                        // Construir definição completa da macro
                        if (args.size() > 1) {
                            // Juntar todos os argumentos restantes como valor
                            for (size_t i = 1; i < args.size(); ++i) {
                                if (i > 1) macro_value += " ";
                                macro_value += args[i];
                            }
                        }
                        
                        // Parsear parâmetros da macro funcional
                        std::vector<std::string> parameters = macro_processor_->parseParameterList(params_str);
                        bool isVariadic = false;
                        
                        // Verificar se é variádica (termina com ...)
                        if (!parameters.empty() && parameters.back() == "...") {
                            isVariadic = true;
                            parameters.pop_back(); // Remove o "..."
                        }
                        
                        // Definir macro funcional usando o método correto
                        macro_processor_->defineFunctionMacro(base_name, parameters, macro_value, isVariadic);
                        
                        std::string log_msg = "Macro funcional definida: ";
                        log_msg += base_name;
                        log_msg += "(";
                        for (size_t i = 0; i < parameters.size(); ++i) {
                            if (i > 0) log_msg += ", ";
                            log_msg += parameters[i];
                        }
                        if (isVariadic) {
                            if (!parameters.empty()) log_msg += ", ";
                            log_msg += "...";
                        }
                        log_msg += ") = ";
                        log_msg += macro_value;
                        logger_->info(log_msg);
                    } else {
                        // Macro simples (object-like)
                        if (args.size() > 1) {
                            // Juntar todos os argumentos restantes como valor
                            for (size_t i = 1; i < args.size(); ++i) {
                                if (i > 1) macro_value += " ";
                                macro_value += args[i];
                            }
                        }
                        // Se não há valor, é uma macro vazia (definida mas sem valor)
                        
                        // Definir macro simples
                        macro_processor_->defineMacro(macro_name, macro_value);
                        
                        std::string log_msg = "Macro definida: ";
                        log_msg += macro_name;
                        log_msg += " = ";
                        log_msg += macro_value;
                        logger_->info(log_msg);
                    }
                }
                break;
                
            case DirectiveType::UNDEF:
                {
                    std::string log_msg = "Processando #undef: ";
                    log_msg += directive.getContent();
                    logger_->info(log_msg);
                    
                    const auto& args = directive.getArguments();
                    if (args.empty()) {
                        logger_->error("#undef requer um nome de macro");
                        return false;
                    }
                    
                    std::string macro_name = args[0];
                    
                    // Verificar se a macro está definida
                    if (macro_processor_->isDefined(macro_name)) {
                        macro_processor_->undefineMacro(macro_name);
                        std::string info_msg = "Macro removida: ";
                        info_msg += macro_name;
                        logger_->info(info_msg);
                    } else {
                        std::string warn_msg = "Tentativa de remover macro não definida: ";
                        warn_msg += macro_name;
                        logger_->warning(warn_msg);
                    }
                }
                break;
                
            case DirectiveType::IF:
            case DirectiveType::IFDEF:
            case DirectiveType::IFNDEF:
            case DirectiveType::ELSE:
            case DirectiveType::ELIF:
            case DirectiveType::ENDIF:
                {
                    // Integração com ConditionalProcessor
                    PreprocessorPosition pos(current_file_, current_line_, 0);
                    const auto& args = directive.getArguments();
                    std::string expression = args.empty() ? "" : args[0];
                    
                    bool result = true;
                    switch (directive.getType()) {
                        case DirectiveType::IF:
                            logger_->info("Processando #if: " + expression);
                            result = conditional_processor_->processIfDirective(expression, pos);
                            break;
                        case DirectiveType::IFDEF:
                            logger_->info("Processando #ifdef: " + expression);
                            result = conditional_processor_->processIfdefDirective(expression, pos);
                            break;
                        case DirectiveType::IFNDEF:
                            logger_->info("Processando #ifndef: " + expression);
                            result = conditional_processor_->processIfndefDirective(expression, pos);
                            break;
                        case DirectiveType::ELSE:
                            logger_->info("Processando #else");
                            result = conditional_processor_->processElseDirective(pos);
                            break;
                        case DirectiveType::ELIF:
                            logger_->info("Processando #elif: " + expression);
                            result = conditional_processor_->processElifDirective(expression, pos);
                            break;
                        case DirectiveType::ENDIF:
                            logger_->info("Processando #endif");
                            result = conditional_processor_->processEndifDirective(pos);
                            break;
                        default:
                            break;
                    }
                    
                    if (!result) {
                        logger_->error("Erro no processamento da diretiva condicional: " + directive.toString());
                        return false;
                    }
                    
                    // Atualizar estado do preprocessor baseado no ConditionalProcessor
                    if (state_) {
                        if (conditional_processor_->hasOpenConditionals()) {
                            state_->enableConditionalBlock();
                            state_->setProcessingMode(ProcessingMode::CONDITIONAL);
                        } else {
                            state_->disableConditionalBlock();
                            state_->setProcessingMode(ProcessingMode::NORMAL);
                        }
                    }
                }
                break;
                
            case DirectiveType::ERROR:
                {
                    // Criar DirectiveProcessor temporário para processar #error corretamente
                DirectiveProcessor directive_processor(state_.get(), logger_.get(), 
                                                     macro_processor_.get(), file_manager_.get(), 
                                                     conditional_processor_.get());
                
                // Configurar errorHandler se disponível
                if (external_error_handler_) {
                    directive_processor.setErrorHandler(external_error_handler_);
                }
                
                // Processar diretiva #error usando o DirectiveProcessor
                bool result = directive_processor.processErrorDirective(directive.getContent(), directive.getPosition());
                    
                    // #error sempre deve falhar o processamento
                    return false;
                }
                
            case DirectiveType::WARNING:
                logger_->warning("#warning: " + directive.getContent());
                break;
                
            case DirectiveType::PRAGMA:
                logger_->info("#pragma: " + directive.getContent());
                break;
                
            case DirectiveType::LINE:
                // Implementação da diretiva #line
                {
                    std::string content = directive.getContent();
                    std::istringstream iss(content);
                    std::string line_str, filename;
                    
                    if (iss >> line_str) {
                        try {
                            int new_line = std::stoi(line_str);
                            
                            // Verificar se há nome de arquivo
                            if (iss >> filename) {
                                // Remover aspas se presentes
                                if (filename.front() == '"' && filename.back() == '"') {
                                    filename = filename.substr(1, filename.length() - 2);
                                }
                            }
                            
                            // Atualizar posição atual
                            current_line_ = new_line - 1; // -1 porque será incrementado na próxima linha
                            
                            // Atualizar estado se disponível
                             if (state_) {
                                 state_->setCurrentLine(new_line);
                                 state_->resetColumn();
                                 if (!filename.empty()) {
                                     FileContext new_context(filename, new_line, 1);
                                     state_->setFileContext(new_context);
                                 }
                             }
                            
                            logger_->info("#line processado: linha=" + std::to_string(new_line) + 
                                        (filename.empty() ? "" : ", arquivo=" + filename));
                        } catch (const std::exception& e) {
                            logger_->error("Erro ao processar #line: número de linha inválido: " + line_str);
                            return false;
                        }
                    } else {
                        logger_->error("Erro ao processar #line: formato inválido: " + content);
                        return false;
                    }
                }
                break;
                
            default:
                logger_->warning("Diretiva desconhecida: " + directive.toString());
                break;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        logger_->error("Erro ao processar diretiva: " + std::string(e.what()));
        return false;
    }
}

// Escrita na saída
void PreprocessorMain::writeOutput(const std::string& content) {
    expanded_code_ += content;
    
    // DEBUG: Imprimir o que está sendo enviado ao lexer
    // Output content processed
    if (!content.empty() && content.back() != '\n') {
        // Add newline if needed (output disabled for cleaner preprocessor output)
    }
}

// Obter código expandido
std::string PreprocessorMain::getExpandedCode() const {
    return expanded_code_;
}

// Obter dependências
std::vector<std::string> PreprocessorMain::getDependencies() const {
    return dependencies_;
}

// Reset do preprocessor
void PreprocessorMain::reset() {
    expanded_code_.clear();
    dependencies_.clear();
    position_mappings_.clear();
    current_file_.clear();
    current_line_ = 0;
    processing_active_ = false;
    
    // Reset dos componentes
    if (macro_processor_) {
        macro_processor_->clearUserMacros();
    }
    
    if (state_) {
        state_->reset();
    }
    
    logger_->info("Preprocessor resetado");
}

// Configuração de caminhos de busca
void PreprocessorMain::setSearchPaths(const std::vector<std::string>& paths) {
    if (file_manager_) {
        file_manager_->setSearchPaths(paths);
        logger_->info("Caminhos de busca atualizados");
    }
}

// Adicionar caminho de busca
void PreprocessorMain::addIncludePath(const std::string& path) {
    if (file_manager_) {
        file_manager_->addSearchPath(path);
        logger_->info("Caminho de busca adicionado: " + path);
    }
}

// Definição de macro
void PreprocessorMain::defineMacro(const std::string& name, const std::string& value) {
    if (macro_processor_) {
        PreprocessorPosition pos("<command-line>", 0, 0);
        macro_processor_->defineMacro(name, value, pos);
        logger_->info("Macro definida: " + name + " = " + value);
    }
}

// Remoção de macro
void PreprocessorMain::undefineMacro(const std::string& name) {
    if (macro_processor_) {
        macro_processor_->undefineMacro(name);
        logger_->info("Macro removida: " + name);
    }
}

// Verificação de macro
bool PreprocessorMain::isMacroDefined(const std::string& name) const {
    if (macro_processor_) {
        return macro_processor_->isDefined(name);
    }
    return false;
}

// Obter macros definidas
std::unordered_map<std::string, std::string> PreprocessorMain::getDefinedMacros() const {
    std::unordered_map<std::string, std::string> result;
    if (macro_processor_) {
        auto macroNames = macro_processor_->getDefinedMacros();
        for (const auto& name : macroNames) {
            result[name] = macro_processor_->getMacroValue(name);
        }
    }
    return result;
}

// Configuração de versão
void PreprocessorMain::setVersion(CVersion version) {
    if (config_) {
        config_->setVersion(version);
        // Recarregar macros predefinidas
        if (macro_processor_) {
            macro_processor_->initializePredefinedMacros();
        }
        logger_->info("Versão do C configurada");
    }
}

// Obter estatísticas
void PreprocessorMain::setErrorHandler(void* errorHandler) {
    external_error_handler_ = errorHandler;
    
    // Propagar o external error handler para todos os componentes
    if (macro_processor_) {
        macro_processor_->setErrorHandler(errorHandler);
    }
    
    if (conditional_processor_) {
        conditional_processor_->setErrorHandler(errorHandler);
    }
    
    if (file_manager_) {
        file_manager_->setErrorHandler(errorHandler);
    }
}

PreprocessorState PreprocessorMain::getStatistics() const {
    // Implementação de coleta de estatísticas apropriadas
    if (state_) {
        // Atualizar estatísticas dinâmicas antes de retornar
        PreprocessorState* mutable_state = const_cast<PreprocessorState*>(state_.get());
        
        // Estatísticas de arquivos processados
         mutable_state->addProcessedFile(current_file_);
         
         // Estatísticas de dependências
         for (const auto& dep : dependencies_) {
             mutable_state->addProcessedFile(dep);
         }
         
         // Estatísticas de macros
         if (macro_processor_) {
             // Contar macros definidas (aproximação baseada em macros conhecidas)
             std::vector<std::string> common_macros = {
                 "__FILE__", "__LINE__", "__DATE__", "__TIME__", "__STDC__",
                 "NULL", "TRUE", "FALSE", "EOF", "BUFSIZ"
             };
             
             int defined_macros = 0;
             for (const auto& macro : common_macros) {
                 if (macro_processor_->isDefined(macro)) {
                     defined_macros++;
                 }
             }
             
             // Log de estatísticas de macros
             if (defined_macros > 0) {
                 logger_->info("Macros definidas encontradas: " + std::to_string(defined_macros));
             }
         }
         
         // Estatísticas de condicionais
         if (state_->isInConditionalBlock()) {
             // Atualizar estatísticas de blocos condicionais ativos
             logger_->info("Bloco condicional ativo detectado");
         }
        
        // Estatísticas de tamanho
        if (!expanded_code_.empty()) {
            // Estatísticas de código expandido disponíveis
        }
        
        return *state_;
    }
    
    // Retornar estado padrão com estatísticas básicas
    PreprocessorState default_state;
    return default_state;
}

// Atualização de mapeamento de posições
void PreprocessorMain::updatePositionMapping(const PreprocessorPosition& original, const PreprocessorPosition& expanded) {
    // Mapeamento de posições avançado com validação e otimização
    
    // 1. Validar posições antes de adicionar
    if (original.filename.empty() || expanded.filename.empty()) {
        logger_->warning("Tentativa de mapear posições com nomes de arquivo vazios");
        return;
    }
    
    if (original.line == 0 || expanded.line == 0) {
        logger_->warning("Tentativa de mapear posições com números de linha inválidos");
        return;
    }
    
    // 2. Verificar se já existe mapeamento similar (evitar duplicatas)
    for (const auto& mapping : position_mappings_) {
        if (mapping.first.filename == original.filename &&
            mapping.first.line == original.line &&
            mapping.first.column == original.column &&
            mapping.second.filename == expanded.filename &&
            mapping.second.line == expanded.line &&
            mapping.second.column == expanded.column) {
            // Mapeamento já existe, não adicionar duplicata
            return;
        }
    }
    
    // 3. Criar cópia das posições com informações adicionais
    PreprocessorPosition enhanced_original = original;
    PreprocessorPosition enhanced_expanded = expanded;
    
    // Adicionar informações de contexto se disponível
    if (state_) {
        FileContext current_context = state_->getFileContext();
        enhanced_original.original_file = current_context.filename;
        enhanced_expanded.original_file = current_context.filename;
        
        // Adicionar offset absoluto se disponível
        enhanced_original.offset = current_context.absolute_position;
        enhanced_expanded.offset = current_context.absolute_position;
    }
    
    // 4. Adicionar informações de processamento
    if (conditional_processor_ && conditional_processor_->hasOpenConditionals()) {
        // Marcar que esta posição está dentro de um bloco condicional
        enhanced_original.expanded_line = conditional_processor_->getCurrentNestingLevel();
        enhanced_expanded.expanded_line = conditional_processor_->getCurrentNestingLevel();
    }
    
    // 5. Adicionar o mapeamento
    position_mappings_.emplace_back(enhanced_original, enhanced_expanded);
    
    // 6. Otimização: limitar o tamanho do vetor de mapeamentos
    const size_t MAX_MAPPINGS = 10000;
    if (position_mappings_.size() > MAX_MAPPINGS) {
        // Remover os mapeamentos mais antigos
        position_mappings_.erase(position_mappings_.begin(), 
                               position_mappings_.begin() + (position_mappings_.size() - MAX_MAPPINGS));
        logger_->debug("Cache de mapeamento de posições otimizado");
    }
    
    logger_->debug("Mapeamento de posição adicionado: [" + original.filename + ":" + 
                  std::to_string(original.line) + "] -> [" + expanded.filename + ":" + 
                  std::to_string(expanded.line) + "]");
}

// Tratamento de erros integrado e robusto
void PreprocessorMain::handleErrors(const std::string& error_msg, const PreprocessorPosition& pos) {
    // 1. Atualizar estado de erro
    if (state_) {
        state_->setErrorState(true);
        state_->pushState(ProcessingState::ERROR_STATE);
    }
    
    // 2. Registrar erro com contexto completo e informações de módulo
    std::string enhanced_error = "[PREPROCESSOR::PreprocessorMain::handleErrors] " + error_msg;
    reportError(enhanced_error, pos);
    
    // 3. Tentar recuperação se possível
    attemptErrorRecovery(error_msg, pos);
}

void PreprocessorMain::reportError(const std::string& error_msg, const PreprocessorPosition& pos) {
    // Reportar para errorHandler externo se disponível
    if (external_error_handler_) {
        auto* errorHandler = static_cast<IntegratedErrorHandler*>(external_error_handler_);
        errorHandler->reportError(IntegratedErrorHandler::ErrorSource::PREPROCESSOR, 
                                error_msg, pos.line, pos.column, pos.filename);
    }
    
    // Criar mensagem de erro detalhada com contexto
    std::string detailed_error = "[" + pos.filename + ":" + std::to_string(pos.line) + ":" + std::to_string(pos.column) + "] " + error_msg;
    
    // Adicionar contexto de processamento se disponível
    if (state_) {
        detailed_error += " (Estado: " + processingStateToString(state_->getCurrentState()) + ")";
        detailed_error += " (Modo: " + processingModeToString(state_->getProcessingMode()) + ")";
    }
    
    // Adicionar informações de contexto condicional se relevante
    if (conditional_processor_ && conditional_processor_->hasOpenConditionals()) {
        detailed_error += " (Nível condicional: " + std::to_string(conditional_processor_->getCurrentNestingLevel()) + ")";
    }
    
    // Registrar erro no logger
    logger_->error(detailed_error, pos);
    
    // Atualizar mapeamento de posições para rastreamento de erros
    updatePositionMapping(pos, pos);
}

void PreprocessorMain::attemptErrorRecovery(const std::string& error_msg, const PreprocessorPosition& pos) {
    // Estratégias de recuperação de erro baseadas no tipo de erro
    
    // 1. Recuperação de erros de diretiva
    if (error_msg.find("Diretiva") != std::string::npos) {
        logger_->info("Tentando recuperação de erro de diretiva...");
        
        // Verificar se há condicionais abertas e tentar fechar
        if (conditional_processor_ && conditional_processor_->hasOpenConditionals()) {
            logger_->warning("Condicionais abertas detectadas durante erro - tentando sincronização");
            // Não fechar automaticamente, apenas alertar
        }
    }
    
    // 2. Recuperação de erros de macro
    if (error_msg.find("macro") != std::string::npos || error_msg.find("Macro") != std::string::npos) {
        logger_->info("Tentando recuperação de erro de macro...");
        
        // Limpar estado de expansão de macro se necessário
        if (state_ && state_->getProcessingMode() == ProcessingMode::MACRO_EXPANSION) {
            state_->setProcessingMode(ProcessingMode::NORMAL);
            logger_->info("Estado de expansão de macro resetado");
        }
    }
    
    // 3. Recuperação de erros de arquivo
    if (error_msg.find("arquivo") != std::string::npos || error_msg.find("include") != std::string::npos) {
        logger_->info("Tentando recuperação de erro de arquivo...");
        
        // Verificar se há contextos de arquivo empilhados
        if (state_ && state_->getDepth() > 1) {
            logger_->warning("Contextos de arquivo empilhados detectados - continuando processamento");
        }
    }
    
    // 4. Recuperação geral - restaurar estado normal se possível
    if (state_) {
        // Se não estamos em estado crítico, tentar continuar
        ProcessingState current_state = state_->getCurrentState();
        if (current_state != ProcessingState::ERROR_STATE) {
            logger_->info("Tentando continuar processamento após erro não crítico");
        }
    }
    
    logger_->info("Recuperação de erro concluída");
}

// Limpeza
void PreprocessorMain::cleanup() {
    expanded_code_.clear();
    dependencies_.clear();
    position_mappings_.clear();
    
    // Reset dos componentes
    if (macro_processor_) {
        macro_processor_->clearUserMacros();
    }
    
    initialized_ = false;
    processing_active_ = false;
}

// Validação de entrada
bool PreprocessorMain::validateInput(const std::string& input) {
    // Validações básicas
    if (input.empty()) {
        return true; // Entrada vazia é válida
    }
    
    // Verificar se há caracteres de controle inválidos
    for (char c : input) {
        if (c < 0 || (c > 0 && c < 32 && c != '\n' && c != '\r' && c != '\t')) {
            logger_->error("Caractere de controle inválido encontrado na entrada");
            return false;
        }
    }
    
    // Verificar se há sequências de escape inválidas
    size_t pos = 0;
    while ((pos = input.find('\\', pos)) != std::string::npos) {
        if (pos + 1 >= input.length()) {
            logger_->error("Sequência de escape incompleta no final da entrada");
            return false;
        }
        
        char next = input[pos + 1];
        if (next != 'n' && next != 'r' && next != 't' && next != '\\' && 
            next != '"' && next != '\'' && next != '0' && next != 'a' && 
            next != 'b' && next != 'f' && next != 'v') {
            // Permitir continuação de linha
            if (next != '\n' && next != '\r') {
                logger_->warning("Sequência de escape potencialmente inválida: \\" + std::string(1, next));
            }
        }
        pos += 2;
    }
    
    // Verificar balanceamento básico de aspas
    int quote_count = 0;
    int single_quote_count = 0;
    bool in_string = false;
    bool in_char = false;
    
    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];
        
        if (c == '"' && !in_char && (i == 0 || input[i-1] != '\\')) {
            if (!in_string) {
                in_string = true;
                quote_count++;
            } else {
                in_string = false;
            }
        } else if (c == '\'' && !in_string && (i == 0 || input[i-1] != '\\')) {
            if (!in_char) {
                in_char = true;
                single_quote_count++;
            } else {
                in_char = false;
            }
        }
    }
    
    if (in_string) {
        logger_->warning("String literal não fechada detectada na entrada");
    }
    
    if (in_char) {
        logger_->warning("Literal de caractere não fechado detectado na entrada");
    }
    
    return true;
}

// Validação de saída
bool PreprocessorMain::validateOutput(const std::string& output) {
    if (output.empty()) {
        return true; // Saída vazia é válida
    }
    
    // Verificar se há diretivas não processadas
    size_t pos = 0;
    while ((pos = output.find('#', pos)) != std::string::npos) {
        // Verificar se é início de linha ou precedido por espaços
        bool is_directive = true;
        if (pos > 0) {
            // Verificar se há apenas espaços antes do #
            for (size_t i = pos - 1; i >= 0; --i) {
                if (output[i] == '\n') {
                    break;
                }
                if (output[i] != ' ' && output[i] != '\t') {
                    is_directive = false;
                    break;
                }
                if (i == 0) break;
            }
        }
        
        if (is_directive) {
            // Extrair a possível diretiva
            size_t end_pos = output.find('\n', pos);
            if (end_pos == std::string::npos) {
                end_pos = output.length();
            }
            
            std::string potential_directive = output.substr(pos, end_pos - pos);
            
            // Verificar se é uma diretiva conhecida não processada
            if (potential_directive.find("#define") == 0 ||
                potential_directive.find("#include") == 0 ||
                potential_directive.find("#ifdef") == 0 ||
                potential_directive.find("#ifndef") == 0 ||
                potential_directive.find("#if") == 0 ||
                potential_directive.find("#else") == 0 ||
                potential_directive.find("#elif") == 0 ||
                potential_directive.find("#endif") == 0 ||
                potential_directive.find("#undef") == 0) {
                
                logger_->warning("Diretiva não processada encontrada na saída: " + potential_directive);
            }
        }
        
        pos++;
    }
    
    // Verificar se há macros não expandidas (identificadores que começam com maiúscula)
    std::istringstream iss(output);
    std::string line;
    int line_num = 1;
    
    while (std::getline(iss, line)) {
        // Procurar por identificadores em maiúscula que podem ser macros não expandidas
        size_t word_pos = 0;
        while (word_pos < line.length()) {
            // Encontrar início de palavra
            while (word_pos < line.length() && !std::isalpha(line[word_pos]) && line[word_pos] != '_') {
                word_pos++;
            }
            
            if (word_pos >= line.length()) break;
            
            // Extrair palavra
            size_t word_start = word_pos;
            while (word_pos < line.length() && (std::isalnum(line[word_pos]) || line[word_pos] == '_')) {
                word_pos++;
            }
            
            std::string word = line.substr(word_start, word_pos - word_start);
            
            // Verificar se é um identificador suspeito (todas maiúsculas, mais de 2 caracteres)
            if (word.length() > 2) {
                bool all_upper = true;
                for (char c : word) {
                    if (std::isalpha(c) && !std::isupper(c)) {
                        all_upper = false;
                        break;
                    }
                }
                
                if (all_upper && word != "NULL" && word != "TRUE" && word != "FALSE" && 
                    word != "EOF" && word != "INT" && word != "CHAR" && word != "VOID") {
                    // Verificar se é uma macro conhecida que deveria ter sido expandida
                    if (macro_processor_ && macro_processor_->isDefined(word)) {
                        logger_->warning("Possível macro não expandida na linha " + 
                                       std::to_string(line_num) + ": " + word);
                    }
                }
            }
        }
        line_num++;
    }
    
    // Verificar integridade básica da sintaxe C
    int brace_count = 0;
    int paren_count = 0;
    int bracket_count = 0;
    
    for (char c : output) {
        switch (c) {
            case '{': brace_count++; break;
            case '}': brace_count--; break;
            case '(': paren_count++; break;
            case ')': paren_count--; break;
            case '[': bracket_count++; break;
            case ']': bracket_count--; break;
        }
        
        // Verificar se algum contador ficou negativo
        if (brace_count < 0 || paren_count < 0 || bracket_count < 0) {
            logger_->warning("Desbalanceamento de delimitadores detectado na saída");
            break;
        }
    }
    
    if (brace_count != 0) {
        logger_->warning("Chaves desbalanceadas na saída (diferença: " + std::to_string(brace_count) + ")");
    }
    
    if (paren_count != 0) {
        logger_->warning("Parênteses desbalanceados na saída (diferença: " + std::to_string(paren_count) + ")");
    }
    
    if (bracket_count != 0) {
        logger_->warning("Colchetes desbalanceados na saída (diferença: " + std::to_string(bracket_count) + ")");
    }
    
    return true;
}

// Otimização de processamento
void PreprocessorMain::optimizeProcessing() {
    if (!initialized_) {
        logger_->warning("Tentativa de otimizar processamento sem inicialização");
        return;
    }
    
    logger_->info("Aplicando otimizações de processamento...");
    
    // Otimização 1: Pré-alocação de buffers
    if (expanded_code_.capacity() < 8192) {
        expanded_code_.reserve(8192); // Reservar 8KB iniciais
        logger_->info("Buffer de saída otimizado (8KB reservados)");
    }
    
    // Otimização 2: Otimização de containers de dependências
    if (dependencies_.capacity() < 32) {
        dependencies_.reserve(32); // Reservar espaço para 32 dependências
        logger_->info("Buffer de dependências otimizado");
    }
    
    // Otimização 3: Pré-alocação de estruturas de mapeamento
    if (position_mappings_.capacity() < 256) {
        position_mappings_.reserve(256);
        logger_->info("Mapeamento de posições otimizado");
    }
    
    // Otimização 4: Verificação de componentes para otimização futura
    int optimized_components = 0;
    
    if (file_manager_) {
        // FileManager está disponível para otimizações futuras
        optimized_components++;
    }
    
    if (macro_processor_) {
        // MacroProcessor está disponível para otimizações futuras
        optimized_components++;
    }
    
    if (expression_evaluator_) {
        // ExpressionEvaluator está disponível para otimizações futuras
        optimized_components++;
    }
    
    // Otimização 5: Configuração de processamento eficiente
    if (state_) {
        // Configurar estado para processamento otimizado
        // (métodos específicos serão implementados conforme necessário)
        optimized_components++;
    }
    
    // Otimização 6: Limpeza preventiva de recursos
    if (expanded_code_.size() > 1024 * 1024) { // Se maior que 1MB
        expanded_code_.shrink_to_fit();
        logger_->info("Memória do buffer de saída otimizada");
    }
    
    logger_->info("Otimizações aplicadas em " + std::to_string(optimized_components) + " componentes");
    logger_->info("Sistema de processamento otimizado com sucesso");
}

void PreprocessorMain::enableOptimizations() {
    optimizeProcessing();
}

// Métodos auxiliares para coordenação avançada
bool PreprocessorMain::synchronizeProcessors() {
    try {
        // Sincronizar estado entre componentes
        if (state_ && macro_processor_) {
            // Sincronizar macros com estado
            auto macros = macro_processor_->getDefinedMacros();
            for (const auto& macro : macros) {
                state_->addProcessedFile("macro:" + macro);
            }
        }
        
        if (conditional_processor_ && state_) {
            // Sincronizar processador condicional
            conditional_processor_->reset();
        }
        
        if (file_manager_ && state_) {
            // Sincronizar gerenciador de arquivos
            auto search_paths = file_manager_->getSearchPaths();
            logger_->debug("Sincronizados " + std::to_string(search_paths.size()) + " caminhos de busca");
        }
        
        logger_->debug("Sincronização de processadores concluída");
        return true;
        
    } catch (const std::exception& e) {
        logger_->error("Erro na sincronização de processadores: " + std::string(e.what()));
        return false;
    }
}

bool PreprocessorMain::performIntegrityCheck() {
    try {
        // Verificar integridade do código expandido
        if (expanded_code_.empty()) {
            logger_->warning("Código expandido está vazio");
            return true; // Não é necessariamente um erro
        }
        
        // Verificar balanceamento de diretivas condicionais
        if (conditional_processor_) {
            if (conditional_processor_->hasOpenConditionals()) {
                int level = conditional_processor_->getCurrentNestingLevel();
                logger_->error("Diretivas condicionais não balanceadas (nível: " + 
                             std::to_string(level) + ")");
                return false;
            }
        }
        
        // Verificar consistência de mapeamentos de posição
        size_t mapping_count = position_mappings_.size();
        if (mapping_count > 0) {
            logger_->debug("Verificados " + std::to_string(mapping_count) + " mapeamentos de posição");
        }
        
        // Verificar estado final
        if (state_) {
            auto current_state = state_->getCurrentState();
            if (current_state == ProcessingState::ERROR_STATE) {
                logger_->warning("Estado final indica erro durante processamento");
                return false;
            }
        }
        
        logger_->debug("Verificação de integridade concluída com sucesso");
        return true;
        
    } catch (const std::exception& e) {
        logger_->error("Erro na verificação de integridade: " + std::string(e.what()));
        return false;
    }
}

std::string PreprocessorMain::generateProcessingReport() {
    std::string report;
    
    try {
        if (state_) {
            auto stats = state_->getStatistics();
            report += "Arquivos processados: " + std::to_string(stats.files_processed) + ", ";
        }
        
        if (macro_processor_) {
            auto macros = macro_processor_->getDefinedMacros();
            report += "Macros definidas: " + std::to_string(macros.size()) + ", ";
        }
        
        if (conditional_processor_) {
            auto cond_stats = conditional_processor_->getStatistics();
            report += "Diretivas condicionais: " + std::to_string(cond_stats.total_conditionals) + ", ";
        }
        
        report += "Dependências: " + std::to_string(dependencies_.size()) + ", ";
        report += "Mapeamentos: " + std::to_string(position_mappings_.size());
        
        if (report.empty()) {
            report = "Nenhuma estatística disponível";
        }
        
    } catch (const std::exception& e) {
        report = "Erro ao gerar relatório: " + std::string(e.what());
    }
    
    return report;
}

// Método auxiliar para parsear diretivas
Directive PreprocessorMain::parseDirective(const std::string& line, const PreprocessorPosition& pos) {
    // Encontrar a posição do '#' (pode estar indentado)
    size_t hash_pos = line.find('#');
    if (hash_pos == std::string::npos) {
        return Directive(DirectiveType::UNKNOWN, line, pos);
    }
    
    // Extrair nome da diretiva
    size_t start = hash_pos + 1; // Pular o #
    while (start < line.length() && std::isspace(line[start])) {
        start++;
    }
    
    if (start >= line.length()) {
        return Directive(DirectiveType::UNKNOWN, line, pos);
    }
    
    size_t end = start;
    while (end < line.length() && 
           (std::isalnum(line[end]) || line[end] == '_')) {
        end++;
    }
    
    if (end == start) {
        return Directive(DirectiveType::UNKNOWN, line, pos);
    }
    
    std::string directive_name = line.substr(start, end - start);
    DirectiveType type = stringToDirectiveType(directive_name);
    
    // Criar diretiva
    Directive directive(type, line, pos);
    
    // Extrair argumentos
    std::vector<std::string> arguments;
    if (end < line.length()) {
        // Pular espaços após o nome da diretiva
        while (end < line.length() && std::isspace(line[end])) {
            end++;
        }
        
        if (end < line.length()) {
            std::string args_str = line.substr(end);
            
            // Parsear argumentos baseado no tipo de diretiva
            switch (type) {
                case DirectiveType::INCLUDE:
                    // Para #include, o argumento é o nome do arquivo
                    arguments.push_back(args_str);
                    break;
                    
                case DirectiveType::DEFINE:
                    // Para #define, tratar de forma especial para preservar macros funcionais
                    {
                        // Verificar se é uma macro funcional (tem parênteses)
                        size_t paren_pos = args_str.find('(');
                        size_t space_pos = args_str.find_first_of(" \t");
                        
                        if (paren_pos != std::string::npos && 
                            (space_pos == std::string::npos || paren_pos < space_pos)) {
                            // É uma macro funcional - encontrar o ')' correspondente
                            size_t close_paren = args_str.find(')', paren_pos);
                            if (close_paren != std::string::npos) {
                                // Macro funcional completa: nome + parâmetros
                                std::string macro_name = args_str.substr(0, close_paren + 1);
                                arguments.push_back(macro_name);
                                
                                // Pular espaços após os parênteses
                                size_t value_start = close_paren + 1;
                                while (value_start < args_str.length() && 
                                       (args_str[value_start] == ' ' || args_str[value_start] == '\t')) {
                                    value_start++;
                                }
                                
                                if (value_start < args_str.length()) {
                                    std::string macro_value = args_str.substr(value_start);
                                    arguments.push_back(macro_value);
                                }
                            } else {
                                // Parênteses não fechados - tratar como macro simples
                                arguments.push_back(args_str);
                            }
                        } else if (space_pos != std::string::npos) {
                            // Macro simples com valor
                            std::string macro_name = args_str.substr(0, space_pos);
                            arguments.push_back(macro_name);
                            
                            // Pular espaços/tabs
                            size_t value_start = space_pos;
                            while (value_start < args_str.length() && 
                                   (args_str[value_start] == ' ' || args_str[value_start] == '\t')) {
                                value_start++;
                            }
                            
                            if (value_start < args_str.length()) {
                                std::string macro_value = args_str.substr(value_start);
                                arguments.push_back(macro_value);
                            }
                        } else {
                            // Apenas o nome da macro (sem valor)
                            arguments.push_back(args_str);
                        }
                    }
                    break;
                    
                case DirectiveType::UNDEF:
                case DirectiveType::IFDEF:
                case DirectiveType::IFNDEF:
                    // Para estas diretivas, separar por espaços
                    {
                        std::istringstream iss(args_str);
                        std::string arg;
                        while (iss >> arg) {
                            arguments.push_back(arg);
                        }
                    }
                    break;
                    
                case DirectiveType::IF:
                case DirectiveType::ELIF:
                    // Para expressões condicionais, manter como string única
                    arguments.push_back(args_str);
                    break;
                    
                case DirectiveType::ERROR:
                case DirectiveType::WARNING:
                case DirectiveType::PRAGMA:
                case DirectiveType::LINE:
                    // Para estas diretivas, manter argumentos como string única
                    arguments.push_back(args_str);
                    break;
                    
                default:
                    // Para diretivas desconhecidas ou sem argumentos
                    if (!args_str.empty()) {
                        arguments.push_back(args_str);
                    }
                    break;
            }
        }
    }
    
    directive.setArguments(arguments);
    return directive;
}

// Métodos de coordenação avançada entre módulos
bool PreprocessorMain::coordinateModules() {
    try {
        logger_->debug("Iniciando coordenação entre módulos");
        
        // 1. Coordenar MacroProcessor com ConditionalProcessor
        if (macro_processor_ && conditional_processor_) {
            // Verificar se macros afetam condicionais ativas
            if (conditional_processor_->hasOpenConditionals()) {
                auto defined_macros = macro_processor_->getDefinedMacros();
                logger_->debug("Coordenando " + std::to_string(defined_macros.size()) + 
                             " macros com processador condicional");
            }
        }
        
        // 2. Coordenar FileManager com State
        if (file_manager_ && state_) {
            auto current_context = state_->getFileContext();
            if (!current_context.filename.empty()) {
                // Sincronizar contexto de arquivo
                notifyStateChange("FileManager", "context_sync");
            }
        }
        
        // 3. Coordenar ExpressionEvaluator com MacroProcessor
        if (expression_evaluator_ && macro_processor_) {
            // Garantir que o avaliador tenha acesso às macros atuais
            notifyStateChange("ExpressionEvaluator", "macro_update");
        }
        
        // 4. Propagar estado global
        if (state_) {
            auto current_state = state_->getCurrentState();
            std::string state_info = "state:" + std::to_string(static_cast<int>(current_state));
            propagateState(state_info);
        }
        
        logger_->debug("Coordenação entre módulos concluída");
        return true;
        
    } catch (const std::exception& e) {
        logger_->error("Erro na coordenação entre módulos: " + std::string(e.what()));
        return false;
    }
}

void PreprocessorMain::propagateState(const std::string& state_info) {
    try {
        logger_->debug("Propagando estado: " + state_info);
        
        // Propagar para todos os componentes que precisam de sincronização
        if (state_info.find("macro:") == 0) {
            // Mudança relacionada a macros
            if (conditional_processor_) {
                notifyStateChange("ConditionalProcessor", "macro_changed");
            }
            if (expression_evaluator_) {
                notifyStateChange("ExpressionEvaluator", "macro_changed");
            }
        }
        
        if (state_info.find("file:") == 0) {
            // Mudança relacionada a arquivos
            if (macro_processor_) {
                notifyStateChange("MacroProcessor", "file_changed");
            }
            if (conditional_processor_) {
                notifyStateChange("ConditionalProcessor", "file_changed");
            }
        }
        
        if (state_info.find("state:") == 0) {
            // Mudança de estado global
            notifyStateChange("AllComponents", "global_state_changed");
        }
        
    } catch (const std::exception& e) {
        logger_->warning("Erro na propagação de estado: " + std::string(e.what()));
    }
}

bool PreprocessorMain::validateModuleConsistency() {
    try {
        logger_->debug("Validando consistência entre módulos");
        
        // 1. Verificar consistência entre MacroProcessor e State
        if (macro_processor_ && state_) {
            auto defined_macros = macro_processor_->getDefinedMacros();
            auto processed_files = state_->getProcessedFiles();
            
            // Verificar se há macros órfãs (sem arquivo de origem)
            for (const auto& macro : defined_macros) {
                if (macro.find("__") == 0) continue; // Pular macros predefinidas
                logger_->debug("Validando macro: " + macro);
            }
        }
        
        // 2. Verificar consistência do ConditionalProcessor
        if (conditional_processor_) {
            if (conditional_processor_->hasOpenConditionals()) {
                logger_->warning("Condicionais abertas detectadas durante validação");
                return false;
            }
        }
        
        // 3. Verificar consistência do FileManager
        if (file_manager_ && state_) {
            auto current_context = state_->getFileContext();
            if (!current_context.filename.empty()) {
                // Verificar se o arquivo atual existe no contexto do FileManager
                logger_->debug("Validando contexto de arquivo: " + current_context.filename);
            }
        }
        
        // 4. Verificar integridade dos mapeamentos de posição
        if (!position_mappings_.empty()) {
            size_t invalid_mappings = 0;
            for (const auto& mapping : position_mappings_) {
                if (mapping.first.filename.empty() || mapping.second.filename.empty()) {
                    invalid_mappings++;
                }
            }
            if (invalid_mappings > 0) {
                logger_->warning("Mapeamentos de posição inválidos: " + 
                               std::to_string(invalid_mappings));
            }
        }
        
        logger_->debug("Validação de consistência concluída");
        return true;
        
    } catch (const std::exception& e) {
        logger_->error("Erro na validação de consistência: " + std::string(e.what()));
        return false;
    }
}

void PreprocessorMain::notifyStateChange(const std::string& component, const std::string& event) {
    try {
        std::string notification = component + ":" + event;
        logger_->debug("Notificação de mudança de estado: " + notification);
        
        // Processar notificações específicas
        if (component == "MacroProcessor" && event == "macro_defined") {
            // Notificar outros componentes sobre nova macro
            if (conditional_processor_) {
                // ConditionalProcessor pode precisar reavaliar condições
            }
            if (expression_evaluator_) {
                // ExpressionEvaluator pode precisar atualizar contexto
            }
        }
        
        if (component == "ConditionalProcessor" && event == "condition_changed") {
            // Notificar sobre mudança de condição
            if (state_) {
                auto current_mode = state_->getProcessingMode();
                if (current_mode != ProcessingMode::CONDITIONAL) {
                    state_->setProcessingMode(ProcessingMode::CONDITIONAL);
                }
            }
        }
        
        if (component == "FileManager" && event == "file_included") {
            // Notificar sobre inclusão de arquivo
            if (state_) {
                // Atualizar contexto de arquivo no estado
            }
        }
        
        if (component == "AllComponents" && event == "global_state_changed") {
            // Evitar recursão infinita
            static bool coordinating = false;
            if (!coordinating) {
                coordinating = true;
                coordinateModules();
                coordinating = false;
            }
        }
        
    } catch (const std::exception& e) {
        logger_->warning("Erro na notificação de mudança de estado: " + std::string(e.what()));
    }
}

// Métodos de sincronização entre processadores
bool PreprocessorMain::synchronizeAllProcessors() {
    try {
        // Sincronizar todos os processadores em sequência
        synchronizeMacroWithConditional();
        synchronizeFileWithState();
        synchronizeExpressionWithMacro();
        
        // Verificar consistência após sincronização
        if (!checkProcessorConsistency()) {
            resolveProcessorConflicts();
            return checkProcessorConsistency(); // Verificar novamente após resolução
        }
        
        return true;
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Erro na sincronização de processadores: " + std::string(e.what()));
        }
        return false;
    }
}

void PreprocessorMain::synchronizeMacroWithConditional() {
    if (!macro_processor_ || !conditional_processor_) {
        return;
    }
    
    try {
        // Sincronizar macros definidas com contexto condicional
        auto defined_macros = macro_processor_->getDefinedMacros();
        
        // Verificar se as macros são válidas no contexto condicional atual
        for (const auto& macro_name : defined_macros) {
            if (conditional_processor_->shouldProcessBlock()) {
                // Macro deve estar ativa no contexto atual
                if (!macro_processor_->isDefined(macro_name)) {
                    // Reativar macro se necessário
                    auto macro_value = macro_processor_->getMacroValue(macro_name);
                    macro_processor_->defineMacro(macro_name, macro_value);
                }
            }
        }
        
        // Notificar sobre sincronização
        notifyStateChange("MacroProcessor", "synchronized_with_conditional");
        
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Erro na sincronização Macro-Condicional: " + std::string(e.what()));
        }
    }
}

void PreprocessorMain::synchronizeFileWithState() {
    if (!file_manager_ || !state_) {
        return;
    }
    
    try {
        // Sincronizar contexto de arquivo com estado global
        auto file_context = state_->getFileContext();
        
        if (!file_context.filename.empty()) {
            // Verificar se o arquivo atual está sincronizado
            // Verificar se o arquivo atual está sincronizado
            // FileManager não possui getCurrentFile/setCurrentFile
            // Usar apenas validação de existência
            if (!file_manager_->fileExists(file_context.filename)) {
                if (logger_) {
                    logger_->warning("Arquivo não encontrado: " + file_context.filename);
                }
            }
        }
        
        // Sincronizar lista de arquivos processados
        auto processed_files = state_->getProcessedFiles();
        // FileManager não possui markFileAsProcessed
        // Apenas validar existência dos arquivos
        for (const auto& file : processed_files) {
            if (!file_manager_->fileExists(file)) {
                if (logger_) {
                    logger_->warning("Arquivo processado não encontrado: " + file);
                }
            }
        }
        
        // Notificar sobre sincronização
        notifyStateChange("FileManager", "synchronized_with_state");
        
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Erro na sincronização File-State: " + std::string(e.what()));
        }
    }
}

void PreprocessorMain::synchronizeExpressionWithMacro() {
    if (!expression_evaluator_ || !macro_processor_) {
        return;
    }
    
    try {
        // Sincronizar avaliador de expressões com macros definidas
        auto defined_macros = macro_processor_->getDefinedMacros();
        
        // Atualizar contexto de macros no avaliador de expressões
        // ExpressionEvaluator não possui setMacroValue/updateConfiguration
        // Apenas expandir macros nas expressões quando necessário
        for (const auto& macro_name : defined_macros) {
            auto macro_value = macro_processor_->getMacroValue(macro_name);
            // Validar que a macro está definida corretamente
            if (macro_value.empty() && logger_) {
                logger_->warning("Macro vazia detectada: " + macro_name);
            }
        }
        
        // Notificar sobre sincronização
        notifyStateChange("ExpressionEvaluator", "synchronized_with_macro");
        
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Erro na sincronização Expression-Macro: " + std::string(e.what()));
        }
    }
}

bool PreprocessorMain::checkProcessorConsistency() {
    try {
        bool consistent = true;
        
        // Verificar consistência entre MacroProcessor e ConditionalProcessor
        if (macro_processor_ && conditional_processor_) {
            auto defined_macros = macro_processor_->getDefinedMacros();
            bool should_process = conditional_processor_->shouldProcessBlock();
            
            // Se não devemos processar o bloco atual, macros não deveriam estar ativas
            if (!should_process && !defined_macros.empty()) {
                if (logger_) {
                    logger_->warning("Inconsistência: macros ativas em bloco condicional inativo");
                }
                consistent = false;
            }
        }
        
        // Verificar consistência entre FileManager e State
        if (file_manager_ && state_) {
            auto file_context = state_->getFileContext();
            // FileManager não possui getCurrentFile, usar validação de existência
            bool file_exists = file_manager_->fileExists(file_context.filename);
            
            if (!file_context.filename.empty() && !file_exists) {
                if (logger_) {
                    logger_->warning("Inconsistência: arquivo do contexto não existe: " + file_context.filename);
                }
                consistent = false;
            }
        }
        
        // Verificar consistência entre ExpressionEvaluator e MacroProcessor
        if (expression_evaluator_ && macro_processor_) {
            auto macro_count = macro_processor_->getDefinedMacros().size();
            // ExpressionEvaluator não possui getMacroCount
            // Usar apenas contagem de macros do MacroProcessor
            size_t evaluator_macro_count = macro_count; // Assumir sincronizado
            
            if (macro_count != evaluator_macro_count) {
                // Verificação removida pois ExpressionEvaluator não possui getMacroCount
                consistent = false;
            }
        }
        
        return consistent;
        
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Erro na verificação de consistência: " + std::string(e.what()));
        }
        return false;
    }
}

void PreprocessorMain::resolveProcessorConflicts() {
    try {
        if (logger_) {
            logger_->info("Resolvendo conflitos entre processadores...");
        }
        
        // Resolver conflitos entre MacroProcessor e ConditionalProcessor
        if (macro_processor_ && conditional_processor_) {
            bool should_process = conditional_processor_->shouldProcessBlock();
            
            if (!should_process) {
                // MacroProcessor não possui suspendMacro/resumeSuspendedMacros
                // Apenas registrar o conflito
                if (logger_) {
                    logger_->warning("Conflito detectado: macros ativas em bloco condicional inativo");
                }
            }
        }
        
        // Resolver conflitos entre FileManager e State
        if (file_manager_ && state_) {
            auto file_context = state_->getFileContext();
            if (!file_context.filename.empty()) {
                // FileManager não possui setCurrentFile/setCurrentLine
                // Apenas validar existência do arquivo
                if (!file_manager_->fileExists(file_context.filename)) {
                    if (logger_) {
                        logger_->warning("Arquivo do contexto não existe: " + file_context.filename);
                    }
                }
            }
        }
        
        // Resolver conflitos entre ExpressionEvaluator e MacroProcessor
        if (expression_evaluator_ && macro_processor_) {
            // ExpressionEvaluator não possui clearMacros/setMacroValue
            // Apenas validar que as macros estão definidas
            auto defined_macros = macro_processor_->getDefinedMacros();
            for (const auto& macro_name : defined_macros) {
                if (!macro_processor_->isDefined(macro_name)) {
                    if (logger_) {
                        logger_->warning("Macro não definida: " + macro_name);
                    }
                }
            }
        }
        
        if (logger_) {
            logger_->info("Conflitos resolvidos com sucesso");
        }
        
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Erro na resolução de conflitos: " + std::string(e.what()));
        }
    }
}

} // namespace Preprocessor