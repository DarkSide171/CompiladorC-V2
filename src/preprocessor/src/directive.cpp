#include "../include/directive.hpp"
#include "../include/preprocessor_state.hpp"
#include "../include/preprocessor_logger.hpp"
#include "../include/macro_processor.hpp"
#include "../include/file_manager.hpp"
#include "../include/conditional_processor.hpp"
#include "../include/preprocessor_lexer_interface.hpp"
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <cctype>

namespace Preprocessor {

// ============================================================================
// Implementação da classe Directive
// ============================================================================

Directive::Directive(DirectiveType type, const std::string& content, const PreprocessorPosition& pos)
    : type_(type), content_(content), position_(pos), valid_(false) {
    // Validar a diretiva durante a construção
    valid_ = validateSyntax() && validateArguments();
}

std::string Directive::toString() const {
    std::ostringstream oss;
    oss << "Directive{";
    oss << "type=" << directiveTypeToString(type_);
    oss << ", content=\"" << content_ << "\"";
    oss << ", position=(" << position_.original_line << "," << position_.original_column << ")";
    oss << ", file=\"" << position_.filename << "\"";
    oss << ", valid=" << (valid_ ? "true" : "false");
    oss << ", args=[";
    for (size_t i = 0; i < arguments_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << arguments_[i] << "\"";
    }
    oss << "]}";
    return oss.str();
}

bool Directive::validateSyntax() const {
    // Validação básica de sintaxe
    if (content_.empty()) {
        return false;
    }
    
    // Deve começar com #
    if (content_[0] != '#') {
        return false;
    }
    
    // Extrair nome da diretiva
    size_t start = 1;
    while (start < content_.length() && std::isspace(content_[start])) {
        start++;
    }
    
    if (start >= content_.length()) {
        return false; // Apenas # sem nome
    }
    
    size_t end = start;
    while (end < content_.length() && 
           (std::isalnum(content_[end]) || content_[end] == '_')) {
        end++;
    }
    
    if (end == start) {
        return false; // Nome vazio
    }
    
    std::string directive_name = content_.substr(start, end - start);
    
    // Verificar se é um tipo conhecido
    DirectiveType detected_type = stringToDirectiveType(directive_name);
    if (detected_type == DirectiveType::UNKNOWN && type_ != DirectiveType::UNKNOWN) {
        return false;
    }
    
    return true;
}

bool Directive::validateArguments() const {
    // Validação específica por tipo de diretiva
    switch (type_) {
        case DirectiveType::INCLUDE:
            // #include deve ter exatamente um argumento
            return arguments_.size() == 1 && !arguments_[0].empty();
            
        case DirectiveType::DEFINE:
            // #define deve ter pelo menos um argumento (nome da macro)
            return arguments_.size() >= 1 && !arguments_[0].empty();
            
        case DirectiveType::UNDEF:
            // #undef deve ter exatamente um argumento
            return arguments_.size() == 1 && !arguments_[0].empty();
            
        case DirectiveType::IF:
        case DirectiveType::ELIF:
            // #if e #elif devem ter pelo menos um argumento (expressão)
            return arguments_.size() >= 1;
            
        case DirectiveType::IFDEF:
        case DirectiveType::IFNDEF:
            // #ifdef e #ifndef devem ter exatamente um argumento
            return arguments_.size() == 1 && !arguments_[0].empty();
            
        case DirectiveType::ELSE:
        case DirectiveType::ENDIF:
            // #else e #endif não devem ter argumentos
            return arguments_.empty();
            
        case DirectiveType::ERROR:
        case DirectiveType::WARNING:
            // #error e #warning podem ter argumentos opcionais
            return true;
            
        case DirectiveType::PRAGMA:
            // #pragma deve ter pelo menos um argumento
            return arguments_.size() >= 1;
            
        case DirectiveType::LINE:
            // #line deve ter pelo menos um argumento (número da linha)
            return arguments_.size() >= 1 && !arguments_[0].empty();
            
        case DirectiveType::UNKNOWN:
            // Diretivas desconhecidas são consideradas inválidas
            return false;
            
        default:
            return false;
    }
}

void Directive::setArguments(const std::vector<std::string>& args) {
    arguments_ = args;
    // Revalidar após definir argumentos
    valid_ = validateSyntax() && validateArguments();
}

// ============================================================================
// Funções utilitárias
// ============================================================================

std::string directiveTypeToString(DirectiveType type) {
    static const std::unordered_map<DirectiveType, std::string> type_names = {
        {DirectiveType::INCLUDE, "include"},
        {DirectiveType::DEFINE, "define"},
        {DirectiveType::UNDEF, "undef"},
        {DirectiveType::IF, "if"},
        {DirectiveType::IFDEF, "ifdef"},
        {DirectiveType::IFNDEF, "ifndef"},
        {DirectiveType::ELSE, "else"},
        {DirectiveType::ELIF, "elif"},
        {DirectiveType::ENDIF, "endif"},
        {DirectiveType::ERROR, "error"},
        {DirectiveType::WARNING, "warning"},
        {DirectiveType::PRAGMA, "pragma"},
        {DirectiveType::LINE, "line"},
        {DirectiveType::UNKNOWN, "unknown"}
    };
    
    auto it = type_names.find(type);
    return (it != type_names.end()) ? it->second : "unknown";
}

DirectiveType stringToDirectiveType(const std::string& str) {
    static const std::unordered_map<std::string, DirectiveType> string_to_type = {
        {"include", DirectiveType::INCLUDE},
        {"define", DirectiveType::DEFINE},
        {"undef", DirectiveType::UNDEF},
        {"if", DirectiveType::IF},
        {"ifdef", DirectiveType::IFDEF},
        {"ifndef", DirectiveType::IFNDEF},
        {"else", DirectiveType::ELSE},
        {"elif", DirectiveType::ELIF},
        {"endif", DirectiveType::ENDIF},
        {"error", DirectiveType::ERROR},
        {"warning", DirectiveType::WARNING},
        {"pragma", DirectiveType::PRAGMA},
        {"line", DirectiveType::LINE}
    };
    
    // Converter para minúsculas para comparação case-insensitive
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    
    auto it = string_to_type.find(lower_str);
    return (it != string_to_type.end()) ? it->second : DirectiveType::UNKNOWN;
}

bool isConditionalDirective(DirectiveType type) {
    switch (type) {
        case DirectiveType::IF:
        case DirectiveType::IFDEF:
        case DirectiveType::IFNDEF:
        case DirectiveType::ELSE:
        case DirectiveType::ELIF:
        case DirectiveType::ENDIF:
            return true;
        default:
            return false;
    }
}

bool requiresArguments(DirectiveType type) {
    switch (type) {
        case DirectiveType::INCLUDE:
        case DirectiveType::DEFINE:
        case DirectiveType::UNDEF:
        case DirectiveType::IF:
        case DirectiveType::IFDEF:
        case DirectiveType::IFNDEF:
        case DirectiveType::ELIF:
        case DirectiveType::PRAGMA:
        case DirectiveType::LINE:
            return true;
            
        case DirectiveType::ELSE:
        case DirectiveType::ENDIF:
            return false;
            
        case DirectiveType::ERROR:
        case DirectiveType::WARNING:
            return false; // Argumentos são opcionais
            
        case DirectiveType::UNKNOWN:
        default:
            return false;
    }
}

// ============================================================================
// Implementação da classe DirectiveProcessor
// ============================================================================

DirectiveProcessor::DirectiveProcessor(PreprocessorState* state,
                                     PreprocessorLogger* logger,
                                     MacroProcessor* macro_processor,
                                     FileManager* file_manager,
                                     ConditionalProcessor* conditional_processor)
    : state_(state), logger_(logger), macro_processor_(macro_processor),
      file_manager_(file_manager), conditional_processor_(conditional_processor),
      external_error_handler_(nullptr) {
    // Construtor - inicialização dos ponteiros
}

// Métodos de parsing e validação

Directive DirectiveProcessor::parseDirective(const std::string& line, const PreprocessorPosition& pos) {
    try {
        // Normalizar a linha
        std::string normalized_line = normalizeDirectiveLine(line);
        
        // Extrair nome da diretiva
        std::string directive_name = extractDirectiveName(normalized_line);
        
        // Converter para tipo
        DirectiveType type = stringToDirectiveType(directive_name);
        
        // Criar objeto Directive
        Directive directive(type, normalized_line, pos);
        
        // Extrair argumentos
        std::string args = extractDirectiveArguments(normalized_line, directive_name);
        
        // Processar argumentos baseado no tipo
        std::vector<std::string> arguments;
        if (!args.empty()) {
            std::istringstream iss(args);
            std::string token;
            
            switch (type) {
                case DirectiveType::INCLUDE:
                    // Para #include, manter o argumento como está (pode ter < > ou " ")
                    arguments.push_back(args);
                    break;
                    
                case DirectiveType::DEFINE:
                    // Para #define, separar nome da macro do valor
                    if (iss >> token) {
                        arguments.push_back(token); // Nome da macro
                        std::string rest;
                        std::getline(iss, rest);
                        if (!rest.empty() && rest[0] == ' ') {
                            rest = rest.substr(1); // Remover espaço inicial
                        }
                        arguments.push_back(rest); // Valor da macro
                    }
                    break;
                    
                case DirectiveType::UNDEF:
                case DirectiveType::IFDEF:
                case DirectiveType::IFNDEF:
                    // Para estas diretivas, apenas o nome da macro
                    arguments.push_back(args);
                    break;
                    
                case DirectiveType::IF:
                case DirectiveType::ELIF:
                    // Para condicionais, manter expressão completa
                    arguments.push_back(args);
                    break;
                    
                case DirectiveType::ERROR:
                case DirectiveType::WARNING:
                case DirectiveType::PRAGMA:
                case DirectiveType::LINE:
                    // Para estas diretivas, manter argumentos como string única
                    arguments.push_back(args);
                    break;
                    
                default:
                    // Para diretivas desconhecidas ou sem argumentos
                    if (!args.empty()) {
                        arguments.push_back(args);
                    }
                    break;
            }
        }
        
        directive.setArguments(arguments);
        
        // Validar sintaxe
        bool syntax_valid = validateDirectiveSyntax(directive);
        directive.setValid(syntax_valid);
        
        // Log do parsing
        logDirectiveProcessing(directive, pos);
        
        return directive;
        
    } catch (const std::exception& e) {
        handleDirectiveErrors("Erro no parsing da diretiva: " + std::string(e.what()), pos);
        return Directive(DirectiveType::UNKNOWN, line, pos);
    }
}

bool DirectiveProcessor::validateDirectiveSyntax(const Directive& directive) {
    try {
        DirectiveType type = directive.getType();
        std::vector<std::string> args = directive.getArguments();
        
        // Verificar se diretiva é conhecida
        if (type == DirectiveType::UNKNOWN) {
            return false;
        }
        
        // Verificar argumentos obrigatórios
        if (requiresArguments(type) && args.empty()) {
            return false;
        }
        
        // Validações específicas por tipo
        switch (type) {
            case DirectiveType::INCLUDE:
                if (args.size() != 1) return false;
                // Verificar se tem < > ou " "
                {
                    std::string filename = args[0];
                    if (filename.empty()) return false;
                    if (!((filename.front() == '<' && filename.back() == '>') ||
                          (filename.front() == '"' && filename.back() == '"'))) {
                        return false;
                    }
                }
                break;
                
            case DirectiveType::DEFINE:
                if (args.empty() || args.size() > 2) return false;
                // Verificar se nome da macro é válido
                {
                    std::string macro_name = args[0];
                    if (macro_name.empty() || !std::isalpha(macro_name[0]) && macro_name[0] != '_') {
                        return false;
                    }
                    for (char c : macro_name) {
                        if (!std::isalnum(c) && c != '_') {
                            return false;
                        }
                    }
                }
                break;
                
            case DirectiveType::UNDEF:
            case DirectiveType::IFDEF:
            case DirectiveType::IFNDEF:
                if (args.size() != 1) return false;
                // Verificar se nome da macro é válido
                {
                    std::string macro_name = args[0];
                    if (macro_name.empty() || !std::isalpha(macro_name[0]) && macro_name[0] != '_') {
                        return false;
                    }
                }
                break;
                
            case DirectiveType::ELSE:
            case DirectiveType::ENDIF:
                if (!args.empty()) return false;
                break;
                
            case DirectiveType::IF:
            case DirectiveType::ELIF:
                if (args.size() != 1 || args[0].empty()) return false;
                break;
                
            case DirectiveType::ERROR:
            case DirectiveType::WARNING:
            case DirectiveType::PRAGMA:
            case DirectiveType::LINE:
                // Estas diretivas podem ter argumentos opcionais
                break;
                
            default:
                return false;
        }
        
        // Validar contexto
        if (!validateDirectiveContext(type)) {
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Erro na validação de sintaxe: " + std::string(e.what()));
        }
        return false;
    }
 }
 
 // Métodos de processamento de diretivas básicas
 
 bool DirectiveProcessor::processIncludeDirective(const std::string& filename, const PreprocessorPosition& pos) {
     try {
         if (!file_manager_) {
             handleDirectiveErrors("FileManager não disponível para processar #include", pos);
             return false;
         }
         
         // Remover aspas ou < > do nome do arquivo
         std::string clean_filename = filename;
         if (clean_filename.length() >= 2) {
             if ((clean_filename.front() == '"' && clean_filename.back() == '"') ||
                 (clean_filename.front() == '<' && clean_filename.back() == '>')) {
                 clean_filename = clean_filename.substr(1, clean_filename.length() - 2);
             }
         }
         
         if (clean_filename.empty()) {
             handleDirectiveErrors("Nome de arquivo vazio em #include", pos);
             return false;
         }
         
         // Verificar se arquivo já foi incluído (proteção contra inclusão circular)
         if (state_ && state_->wasFileProcessed(clean_filename)) {
             if (logger_) {
                 logger_->warning("Arquivo já incluído: " + clean_filename);
             }
             return true; // Não é erro, apenas skip
         }
         
         // Processar inclusão através do FileManager
        std::string file_content;
        bool success = false;
        
        try {
            file_content = file_manager_->readFile(clean_filename);
            success = !file_content.empty();
        } catch (const std::exception& e) {
            // Arquivo não encontrado ou erro de leitura
            success = false;
        }
         
         if (success) {
             // Atualizar estado
             if (state_) {
                 state_->addProcessedFile(clean_filename);
             }
             
             // Log de sucesso
             if (logger_) {
                 logger_->info("Arquivo incluído com sucesso: " + clean_filename);
             }
             
             // Atualizar estatísticas
             updateDirectiveStatistics(DirectiveType::INCLUDE);
         } else {
             handleDirectiveErrors("Falha ao incluir arquivo: " + clean_filename, pos);
         }
         
         return success;
         
     } catch (const std::exception& e) {
         handleDirectiveErrors("Erro ao processar #include: " + std::string(e.what()), pos);
         return false;
     }
 }
 
 bool DirectiveProcessor::processDefineDirective(const std::string& macro_name, const std::string& macro_value, const PreprocessorPosition& pos) {
     try {
         if (!macro_processor_) {
             handleDirectiveErrors("MacroProcessor não disponível para processar #define", pos);
             return false;
         }
         
         if (macro_name.empty()) {
             handleDirectiveErrors("Nome de macro vazio em #define", pos);
             return false;
         }
         
         // Validar nome da macro
         if (!std::isalpha(macro_name[0]) && macro_name[0] != '_') {
             handleDirectiveErrors("Nome de macro inválido: " + macro_name, pos);
             return false;
         }
         
         for (char c : macro_name) {
             if (!std::isalnum(c) && c != '_') {
                 handleDirectiveErrors("Caractere inválido no nome da macro: " + macro_name, pos);
                 return false;
             }
         }
         
         // Verificar se macro já está definida
         if (macro_processor_->isDefined(macro_name)) {
             if (logger_) {
                 logger_->warning("Redefinindo macro: " + macro_name);
             }
         }
         
         // Definir macro através do MacroProcessor
         bool success = macro_processor_->defineMacro(macro_name, macro_value, pos);
         
         if (success) {
             // Atualizar estado
             if (state_) {
                 state_->addProcessedFile("macro:" + macro_name);
             }
             
             // Log de sucesso
             if (logger_) {
                 std::string msg = "Macro definida: " + macro_name;
                 if (!macro_value.empty()) {
                     msg += " = " + macro_value;
                 }
                 logger_->info(msg);
             }
             
             // Atualizar estatísticas
             updateDirectiveStatistics(DirectiveType::DEFINE);
         } else {
             handleDirectiveErrors("Falha ao definir macro: " + macro_name, pos);
         }
         
         return success;
         
     } catch (const std::exception& e) {
         handleDirectiveErrors("Erro ao processar #define: " + std::string(e.what()), pos);
         return false;
     }
 }
 
 // Métodos de processamento de diretivas condicionais
 
 bool DirectiveProcessor::processIfDirective(const std::string& condition, const PreprocessorPosition& pos) {
     try {
         if (!conditional_processor_) {
             handleDirectiveErrors("ConditionalProcessor não disponível para processar #if", pos);
             return false;
         }
         
         if (condition.empty()) {
             handleDirectiveErrors("Condição vazia em #if", pos);
             return false;
         }
         
         // Processar condição através do ConditionalProcessor
         bool condition_result = conditional_processor_->evaluateCondition(condition, pos);
         
         // Iniciar bloco condicional
         bool success = conditional_processor_->pushConditionalContext(condition_result, ConditionalType::IF, condition, pos);
         
         if (success) {
             // Atualizar estado
             if (state_) {
                 state_->enableConditionalBlock();
             }
             
             // Log de sucesso
             if (logger_) {
                 std::string msg = "Bloco #if iniciado: " + condition + " = " + (condition_result ? "true" : "false");
                 logger_->info(msg);
             }
             
             // Atualizar estatísticas
             updateDirectiveStatistics(DirectiveType::IF);
         } else {
             handleDirectiveErrors("Falha ao processar #if: " + condition, pos);
         }
         
         return success;
         
     } catch (const std::exception& e) {
         handleDirectiveErrors("Erro ao processar #if: " + std::string(e.what()), pos);
         return false;
     }
 }
 
 bool DirectiveProcessor::processIfdefDirective(const std::string& macro_name, const PreprocessorPosition& pos) {
     try {
         if (!macro_processor_ || !conditional_processor_) {
             handleDirectiveErrors("Processadores não disponíveis para processar #ifdef", pos);
             return false;
         }
         
         if (macro_name.empty()) {
             handleDirectiveErrors("Nome de macro vazio em #ifdef", pos);
             return false;
         }
         
         // Verificar se macro está definida
         bool is_defined = macro_processor_->isDefined(macro_name);
         
         // Iniciar bloco condicional
         bool success = conditional_processor_->pushConditionalContext(is_defined, ConditionalType::IFDEF, macro_name, pos);
         
         if (success) {
             // Atualizar estado
             if (state_) {
                 state_->enableConditionalBlock();
             }
             
             // Log de sucesso
             if (logger_) {
                 std::string msg = "Bloco #ifdef iniciado: " + macro_name + " = " + (is_defined ? "definida" : "não definida");
                 logger_->info(msg);
             }
             
             // Atualizar estatísticas
             updateDirectiveStatistics(DirectiveType::IFDEF);
         } else {
             handleDirectiveErrors("Falha ao processar #ifdef: " + macro_name, pos);
         }
         
         return success;
         
     } catch (const std::exception& e) {
         handleDirectiveErrors("Erro ao processar #ifdef: " + std::string(e.what()), pos);
         return false;
     }
 }
 
 bool DirectiveProcessor::processIfndefDirective(const std::string& macro_name, const PreprocessorPosition& pos) {
     try {
         if (!macro_processor_ || !conditional_processor_) {
             handleDirectiveErrors("Processadores não disponíveis para processar #ifndef", pos);
             return false;
         }
         
         if (macro_name.empty()) {
             handleDirectiveErrors("Nome de macro vazio em #ifndef", pos);
             return false;
         }
         
         // Verificar se macro NÃO está definida
         bool is_not_defined = !macro_processor_->isDefined(macro_name);
         
         // Iniciar bloco condicional
         bool success = conditional_processor_->pushConditionalContext(is_not_defined, ConditionalType::IFNDEF, macro_name, pos);
         
         if (success) {
             // Atualizar estado
             if (state_) {
                 state_->enableConditionalBlock();
             }
             
             // Log de sucesso
             if (logger_) {
                 std::string msg = "Bloco #ifndef iniciado: " + macro_name + " = " + (is_not_defined ? "não definida" : "definida");
                 logger_->info(msg);
             }
             
             // Atualizar estatísticas
             updateDirectiveStatistics(DirectiveType::IFNDEF);
         } else {
             handleDirectiveErrors("Falha ao processar #ifndef: " + macro_name, pos);
         }
         
         return success;
         
     } catch (const std::exception& e) {
         handleDirectiveErrors("Erro ao processar #ifndef: " + std::string(e.what()), pos);
         return false;
     }
 }
 
 bool DirectiveProcessor::processElseDirective(const PreprocessorPosition& pos) {
     try {
         if (!conditional_processor_) {
             handleDirectiveErrors("ConditionalProcessor não disponível para processar #else", pos);
             return false;
         }
         
         // Processar #else através do ConditionalProcessor
         bool success = conditional_processor_->processElseDirective(pos);
         
         if (success) {
             // Log de sucesso
             if (logger_) {
                 logger_->info("Bloco #else processado");
             }
             
             // Atualizar estatísticas
             updateDirectiveStatistics(DirectiveType::ELSE);
         } else {
             handleDirectiveErrors("Falha ao processar #else", pos);
         }
         
         return success;
         
     } catch (const std::exception& e) {
         handleDirectiveErrors("Erro ao processar #else: " + std::string(e.what()), pos);
         return false;
     }
 }
 
 bool DirectiveProcessor::processElifDirective(const std::string& condition, const PreprocessorPosition& pos) {
     try {
         if (!conditional_processor_) {
             handleDirectiveErrors("ConditionalProcessor não disponível para processar #elif", pos);
             return false;
         }
         
         if (condition.empty()) {
             handleDirectiveErrors("Condição vazia em #elif", pos);
             return false;
         }
         
         // Processar #elif através do ConditionalProcessor
         bool success = conditional_processor_->processElifDirective(condition, pos);
         
         if (success) {
             // Log de sucesso
             if (logger_) {
                 logger_->info("Bloco #elif processado: " + condition);
             }
             
             // Atualizar estatísticas
             updateDirectiveStatistics(DirectiveType::ELIF);
         } else {
             handleDirectiveErrors("Falha ao processar #elif: " + condition, pos);
         }
         
         return success;
         
     } catch (const std::exception& e) {
         handleDirectiveErrors("Erro ao processar #elif: " + std::string(e.what()), pos);
         return false;
     }
 }
 
 bool DirectiveProcessor::processEndifDirective(const PreprocessorPosition& pos) {
     try {
         if (!conditional_processor_) {
             handleDirectiveErrors("ConditionalProcessor não disponível para processar #endif", pos);
             return false;
         }
         
         // Processar #endif através do ConditionalProcessor
         bool success = conditional_processor_->processEndifDirective(pos);
         
         if (success) {
             // Atualizar estado
             if (state_ && state_->isInConditionalBlock()) {
                 state_->disableConditionalBlock();
             }
             
             // Log de sucesso
             if (logger_) {
                 logger_->info("Bloco condicional finalizado com #endif");
             }
             
             // Atualizar estatísticas
             updateDirectiveStatistics(DirectiveType::ENDIF);
         } else {
             handleDirectiveErrors("Falha ao processar #endif", pos);
         }
         
         return success;
         
     } catch (const std::exception& e) {
         handleDirectiveErrors("Erro ao processar #endif: " + std::string(e.what()), pos);
         return false;
     }
 }
 
 bool DirectiveProcessor::processUndefDirective(const std::string& macro_name, const PreprocessorPosition& pos) {
     try {
         if (macro_name.empty()) {
             handleDirectiveErrors("Nome de macro vazio em #undef", pos);
             return false;
         }
         
         if (!macro_processor_) {
             handleDirectiveErrors("MacroProcessor não disponível para processar #undef", pos);
             return false;
         }
         
         // Verificar se a macro está definida
         if (!macro_processor_->isDefined(macro_name)) {
             if (logger_) {
                 logger_->warning("Tentativa de #undef em macro não definida: " + macro_name);
             }
         }
         
         // Remover definição da macro
         bool success = macro_processor_->undefineMacro(macro_name);
         
         if (success) {
             // Log de sucesso
             if (logger_) {
                 logger_->info("Macro removida: " + macro_name);
             }
             
             // Atualizar estatísticas
             updateDirectiveStatistics(DirectiveType::UNDEF);
         } else {
             handleDirectiveErrors("Falha ao remover macro: " + macro_name, pos);
         }
         
         return success;
         
     } catch (const std::exception& e) {
         handleDirectiveErrors("Erro ao processar #undef: " + std::string(e.what()), pos);
         return false;
     }
 }
 
 bool DirectiveProcessor::processErrorDirective(const std::string& message, const PreprocessorPosition& pos) {
     try {
         // Construir mensagem de erro
         std::string error_msg = "#error: " + (message.empty() ? "Erro de compilação" : message);
         
         // Registrar erro
         if (logger_) {
             logger_->error(error_msg + " em " + pos.filename + ":" + std::to_string(pos.line));
         }
         
         // Atualizar estado de erro
         if (state_) {
             state_->setErrorState(true);
         }
         
         // Atualizar estatísticas
         updateDirectiveStatistics(DirectiveType::ERROR);
         
         // Reportar erro
         handleDirectiveErrors(error_msg, pos);
         
         // #error sempre falha o processamento
         return false;
         
     } catch (const std::exception& e) {
         handleDirectiveErrors("Erro ao processar #error: " + std::string(e.what()), pos);
         return false;
     }
 }
 
 bool DirectiveProcessor::processWarningDirective(const std::string& message, const PreprocessorPosition& pos) {
     try {
         // Construir mensagem de warning
         std::string warning_msg = "#warning: " + (message.empty() ? "Aviso de compilação" : message);
         
         // Registrar warning
         if (logger_) {
             logger_->warning(warning_msg + " em " + pos.filename + ":" + std::to_string(pos.line));
         }
         
         // Atualizar estatísticas
         updateDirectiveStatistics(DirectiveType::WARNING);
         
         // #warning não falha o processamento, apenas emite aviso
         return true;
         
     } catch (const std::exception& e) {
         handleDirectiveErrors("Erro ao processar #warning: " + std::string(e.what()), pos);
         return false;
     }
 }
 
 bool DirectiveProcessor::processPragmaDirective(const std::string& pragma_text, const PreprocessorPosition& pos) {
     try {
         if (pragma_text.empty()) {
             if (logger_) {
                 logger_->warning("Diretiva #pragma vazia em " + pos.filename + ":" + std::to_string(pos.line));
             }
             return true; // #pragma vazia é válida, mas não faz nada
         }
         
         // Extrair comando pragma
         std::istringstream iss(pragma_text);
         std::string pragma_command;
         iss >> pragma_command;
         
         // Log de processamento
         if (logger_) {
             logger_->info("Processando #pragma: " + pragma_command);
         }
         
         // Processar pragmas conhecidos
         if (pragma_command == "once") {
             // #pragma once - evitar inclusões múltiplas
             if (file_manager_) {
                 // Marcar arquivo como "include once"
                 // Implementação específica dependeria do FileManager
                 if (logger_) {
                     logger_->info("#pragma once aplicado ao arquivo: " + pos.filename);
                 }
             }
         } else if (pragma_command == "pack") {
             // #pragma pack - controle de alinhamento de estruturas
             if (logger_) {
                 logger_->info("#pragma pack processado (implementação específica do compilador)");
             }
         } else if (pragma_command == "warning") {
             // #pragma warning - controle de warnings
             if (logger_) {
                 logger_->info("#pragma warning processado");
             }
         } else {
             // Pragma desconhecido - apenas log
             if (logger_) {
                 logger_->warning("#pragma desconhecido: " + pragma_command);
             }
         }
         
         // Atualizar estatísticas
         updateDirectiveStatistics(DirectiveType::PRAGMA);
         
         return true;
         
     } catch (const std::exception& e) {
         handleDirectiveErrors("Erro ao processar #pragma: " + std::string(e.what()), pos);
         return false;
     }
 }
 
 bool DirectiveProcessor::processLineDirective(const std::string& line_info, const PreprocessorPosition& pos) {
     try {
         if (line_info.empty()) {
             handleDirectiveErrors("Informação de linha vazia em #line", pos);
             return false;
         }
         
         std::istringstream iss(line_info);
         std::string line_number_str;
         std::string filename;
         
         // Extrair número da linha
         if (!(iss >> line_number_str)) {
             handleDirectiveErrors("Número de linha inválido em #line", pos);
             return false;
         }
         
         // Converter para número
         size_t new_line_number;
         try {
             new_line_number = std::stoull(line_number_str);
         } catch (const std::exception&) {
             handleDirectiveErrors("Número de linha inválido em #line: " + line_number_str, pos);
             return false;
         }
         
         // Extrair nome do arquivo (opcional)
         std::string remaining_text;
         std::getline(iss, remaining_text);
         if (!remaining_text.empty()) {
             // Remover espaços iniciais
             size_t start = remaining_text.find_first_not_of(" \t");
             if (start != std::string::npos) {
                 filename = remaining_text.substr(start);
                 // Remover aspas se presentes
                 if (filename.front() == '"' && filename.back() == '"' && filename.length() > 1) {
                     filename = filename.substr(1, filename.length() - 2);
                 }
             }
         }
         
         // Atualizar contexto de arquivo
         if (state_) {
             if (!filename.empty()) {
                 // Criar novo contexto com arquivo e linha especificados
                 FileContext new_context(filename, new_line_number, 1);
                 state_->setFileContext(new_context);
             } else {
                 // Apenas atualizar linha atual
                 state_->setCurrentLine(new_line_number);
             }
         }
         
         // Log de processamento
         if (logger_) {
             std::string log_msg = "#line processado: linha " + std::to_string(new_line_number);
             if (!filename.empty()) {
                 log_msg += ", arquivo: " + filename;
             }
             logger_->info(log_msg);
         }
         
         // Atualizar estatísticas
         updateDirectiveStatistics(DirectiveType::LINE);
         
         return true;
         
     } catch (const std::exception& e) {
         handleDirectiveErrors("Erro ao processar #line: " + std::string(e.what()), pos);
         return false;
     }
 }
 
 void DirectiveProcessor::setErrorHandler(void* errorHandler) {
    external_error_handler_ = errorHandler;
}

void DirectiveProcessor::handleDirectiveErrors(const std::string& error_message, const PreprocessorPosition& pos) {
    try {
        // Reportar para errorHandler externo se disponível
         if (external_error_handler_) {
              auto* errorHandler = static_cast<IntegratedErrorHandler*>(external_error_handler_);
              errorHandler->reportError(IntegratedErrorHandler::ErrorSource::PREPROCESSOR, error_message, pos.line, pos.column, pos.filename);
          }
        
        // Construir mensagem de erro completa
        std::string full_error = "Erro de diretiva em " + pos.filename + ":" + 
                                std::to_string(pos.line) + ":" + std::to_string(pos.column) + 
                                " - " + error_message;
        
        // Log do erro
        if (logger_) {
            logger_->error(full_error);
        }
        
        // Atualizar estado de erro
        if (state_) {
            state_->setErrorState(true);
        }
        
        // Criar diretiva temporária para reportar erro
        Directive error_directive(DirectiveType::UNKNOWN, "", pos);
        
        reportDirectiveError(error_directive, error_message);
        
    } catch (const std::exception& e) {
        // Erro crítico no tratamento de erros
        if (logger_) {
            logger_->error("Falha crítica no tratamento de erro de diretiva: " + std::string(e.what()));
        }
    }
}
 
 void DirectiveProcessor::reportDirectiveError(const Directive& directive, const std::string& error_msg) {
     try {
         // Estrutura de relatório de erro
         std::ostringstream error_report;
         
         error_report << "\n=== ERRO DE DIRETIVA ===\n";
         error_report << "Arquivo: " << directive.getPosition().filename << "\n";
         error_report << "Linha: " << directive.getPosition().line << "\n";
         error_report << "Coluna: " << directive.getPosition().column << "\n";
         error_report << "Tipo: " << static_cast<int>(directive.getType()) << "\n";
         error_report << "Mensagem: " << error_msg << "\n";
         
         // Adicionar contexto se disponível
         if (state_) {
             error_report << "Estado atual: " << static_cast<int>(state_->getCurrentState()) << "\n";
             error_report << "Modo de processamento: " << static_cast<int>(state_->getProcessingMode()) << "\n";
             error_report << "Em bloco condicional: " << (state_->isInConditionalBlock() ? "Sim" : "Não") << "\n";
         }
         
         // Adicionar informações de contexto condicional
         if (conditional_processor_) {
             error_report << "Nível de aninhamento condicional: " << conditional_processor_->getCurrentNestingLevel() << "\n";
             error_report << "Condicionais abertas: " << (conditional_processor_->hasOpenConditionals() ? "Sim" : "Não") << "\n";
         }
         
         error_report << "========================\n";
         
         // Log do relatório completo
         if (logger_) {
             logger_->error(error_report.str());
         }
         
         // Também enviar para stderr se necessário
         std::cerr << error_report.str() << std::flush;
         
     } catch (const std::exception& e) {
         // Fallback para erro crítico
         if (logger_) {
             logger_->error("Falha ao gerar relatório de erro: " + std::string(e.what()));
         }
         std::cerr << "ERRO CRÍTICO: Falha ao reportar erro de diretiva: " << e.what() << std::endl;
     }
 }
 
 // Métodos auxiliares
 
 std::string DirectiveProcessor::extractDirectiveName(const std::string& line) {
     try {
         std::string trimmed = line;
         
         // Remover espaços no início
         size_t start = trimmed.find_first_not_of(" \t");
         if (start != std::string::npos) {
             trimmed = trimmed.substr(start);
         }
         
         // Deve começar com #
         if (trimmed.empty() || trimmed[0] != '#') {
             return "";
         }
         
         // Remover o #
         trimmed = trimmed.substr(1);
         
         // Encontrar o nome da diretiva
         size_t end = trimmed.find_first_of(" \t\n\r");
         if (end == std::string::npos) {
             return trimmed;
         }
         
         return trimmed.substr(0, end);
         
     } catch (const std::exception& e) {
         if (logger_) {
             logger_->error("Erro ao extrair nome da diretiva: " + std::string(e.what()));
         }
         return "";
     }
 }
 
 std::string DirectiveProcessor::extractDirectiveArguments(const std::string& line, const std::string& directive_name) {
     try {
         // Encontrar posição após o nome da diretiva
         size_t hash_pos = line.find('#');
         if (hash_pos == std::string::npos) {
             return "";
         }
         
         size_t directive_pos = line.find(directive_name, hash_pos);
         if (directive_pos == std::string::npos) {
             return "";
         }
         
         size_t args_start = directive_pos + directive_name.length();
         
         // Pular espaços
         while (args_start < line.length() && (line[args_start] == ' ' || line[args_start] == '\t')) {
             args_start++;
         }
         
         if (args_start >= line.length()) {
             return "";
         }
         
         // Extrair argumentos até o final da linha
         std::string args = line.substr(args_start);
         
         // Remover espaços no final
         size_t end = args.find_last_not_of(" \t\n\r");
         if (end != std::string::npos) {
             args = args.substr(0, end + 1);
         }
         
         return args;
         
     } catch (const std::exception& e) {
         if (logger_) {
             logger_->error("Erro ao extrair argumentos da diretiva: " + std::string(e.what()));
         }
         return "";
     }
 }
 
 std::string DirectiveProcessor::normalizeDirectiveLine(const std::string& line) {
     try {
         std::string normalized = line;
         
         // Remover comentários de linha
         size_t comment_pos = normalized.find("//");
         if (comment_pos != std::string::npos) {
             // Verificar se não está dentro de string
             bool in_string = false;
             char quote_char = '\0';
             
             for (size_t i = 0; i < comment_pos; ++i) {
                 if (!in_string && (normalized[i] == '"' || normalized[i] == '\'')) {
                     in_string = true;
                     quote_char = normalized[i];
                 } else if (in_string && normalized[i] == quote_char && (i == 0 || normalized[i-1] != '\\')) {
                     in_string = false;
                 }
             }
             
             if (!in_string) {
                 normalized = normalized.substr(0, comment_pos);
             }
         }
         
         // Remover espaços extras
         std::string result;
         bool prev_space = false;
         
         for (char c : normalized) {
             if (c == ' ' || c == '\t') {
                 if (!prev_space) {
                     result += ' ';
                     prev_space = true;
                 }
             } else {
                 result += c;
                 prev_space = false;
             }
         }
         
         // Remover espaços no final
         while (!result.empty() && result.back() == ' ') {
             result.pop_back();
         }
         
         return result;
         
     } catch (const std::exception& e) {
         if (logger_) {
             logger_->error("Erro ao normalizar linha da diretiva: " + std::string(e.what()));
         }
         return line;
     }
 }
 
 void DirectiveProcessor::updateDirectiveStatistics(DirectiveType type) {
     try {
         if (state_) {
             // Atualizar estatísticas no estado
             // Implementação específica depende da estrutura de PreprocessorState
         }
         
         if (logger_) {
             logger_->debug("Estatísticas atualizadas para diretiva: " + directiveTypeToString(type));
         }
         
     } catch (const std::exception& e) {
         if (logger_) {
             logger_->error("Erro ao atualizar estatísticas: " + std::string(e.what()));
         }
     }
 }
 
 void DirectiveProcessor::logDirectiveProcessing(const Directive& directive, const PreprocessorPosition& pos) {
     try {
         if (logger_) {
             std::string msg = "Processando diretiva " + directiveTypeToString(directive.getType()) +
                              " na linha " + std::to_string(pos.original_line) +
                              " do arquivo " + pos.filename;
             logger_->debug(msg);
         }
         
     } catch (const std::exception& e) {
         if (logger_) {
             logger_->error("Erro no log de processamento: " + std::string(e.what()));
         }
     }
 }
 
 bool DirectiveProcessor::validateDirectiveContext(DirectiveType type) {
      if (!state_) {
          return false;
      }

      try {
          // Verificar se a diretiva é válida no contexto atual
          switch (type) {
              case DirectiveType::ELSE:
              case DirectiveType::ELIF:
              case DirectiveType::ENDIF:
                  // Verificar se estamos em um bloco condicional
                  if (!state_->isInConditionalBlock()) {
                      if (logger_) {
                          logger_->error("Diretiva condicional fora de contexto");
                      }
                      return false;
                  }
                  break;

              case DirectiveType::INCLUDE:
                  // Include sempre permitido no contexto atual
                  break;

              case DirectiveType::DEFINE:
              case DirectiveType::UNDEF:
                  // Verificar se não estamos em um bloco condicional falso
                  if (state_->isInConditionalBlock()) {
                      // Diretiva será ignorada se bloco for falso, mas é válida
                      return true;
                  }
                  break;

              default:
                  break;
          }

          // Verificar aninhamento de diretivas condicionais
          if (!checkDirectiveNesting(type)) {
              return false;
          }

          return true;

      } catch (const std::exception& e) {
          if (logger_) {
              logger_->error("Erro na validação de contexto: " + std::string(e.what()));
          }
          return false;
      }
  }

 bool DirectiveProcessor::checkDirectiveNesting(DirectiveType type) {
      if (!conditional_processor_) {
          return true; // Sem processador condicional, não há aninhamento para verificar
      }

      try {
          // Verificar limites de aninhamento
          const size_t MAX_NESTING_LEVEL = 64; // Limite razoável para aninhamento
          size_t current_level = conditional_processor_->getCurrentNestingLevel();

          switch (type) {
              case DirectiveType::IF:
              case DirectiveType::IFDEF:
              case DirectiveType::IFNDEF:
                  // Verificar se não excedemos o limite de aninhamento
                  if (current_level >= MAX_NESTING_LEVEL) {
                      if (logger_) {
                          logger_->error("Aninhamento de diretivas condicionais muito profundo (máximo: " + 
                                        std::to_string(MAX_NESTING_LEVEL) + ")");
                      }
                      return false;
                  }
                  break;

              case DirectiveType::ELSE:
                  // Verificar se já existe um #else no contexto atual
                  if (conditional_processor_->hasOpenConditionals()) {
                      auto context = conditional_processor_->getCurrentContext();
                      if (context && context->has_else) {
                          if (logger_) {
                              logger_->error("Múltiplas diretivas #else no mesmo bloco condicional");
                          }
                          return false;
                      }
                  } else {
                      if (logger_) {
                          logger_->error("Diretiva #else sem #if correspondente");
                      }
                      return false;
                  }
                  break;

              case DirectiveType::ELIF:
                  // Verificar se não estamos após um #else
                  if (conditional_processor_->hasOpenConditionals()) {
                      auto context = conditional_processor_->getCurrentContext();
                      if (context && context->has_else) {
                          if (logger_) {
                              logger_->error("Diretiva #elif após #else");
                          }
                          return false;
                      }
                  } else {
                      if (logger_) {
                          logger_->error("Diretiva #elif sem #if correspondente");
                      }
                      return false;
                  }
                  break;

              case DirectiveType::ENDIF:
                  // Verificar se existe um bloco condicional para fechar
                  if (!conditional_processor_->hasOpenConditionals()) {
                      if (logger_) {
                          logger_->error("Diretiva #endif sem #if correspondente");
                      }
                      return false;
                  }
                  break;

              default:
                  break;
          }

          return true;

      } catch (const std::exception& e) {
          if (logger_) {
              logger_->error("Erro na verificação de aninhamento: " + std::string(e.what()));
          }
          return false;
      }
  }

 void DirectiveProcessor::optimizeDirectiveProcessing() {
      // Implementação de otimizações de processamento de diretivas
      
      // 1. Otimização de cache para diretivas frequentes
       if (logger_) {
           logger_->info("Otimizando processamento de diretivas...");
       }
       
       // 2. Pré-carregamento de includes comuns
       if (file_manager_) {
           std::vector<std::string> common_includes = {
               "stdio.h", "stdlib.h", "string.h", "math.h", "stdint.h"
           };
           file_manager_->preloadFiles(common_includes);
       }
       
       // 3. Otimização do cache de macros
       if (macro_processor_) {
           macro_processor_->optimizeCache(300); // 5 minutos TTL
       }
       
       if (logger_) {
           logger_->info("Otimização de diretivas concluída");
       }
  }

 } // namespace Preprocessor