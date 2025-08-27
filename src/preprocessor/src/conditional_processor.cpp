#include "../include/conditional_processor.hpp"
#include "../include/preprocessor_logger.hpp"
#include "../include/expression_evaluator.hpp"
#include "../include/directive.hpp"
#include "../include/macro_processor.hpp"
#include <stdexcept>
#include <sstream>
#include <string>

namespace Preprocessor {

// ============================================================================
// Implementação da classe ConditionalProcessor
// ============================================================================

ConditionalProcessor::ConditionalProcessor(PreprocessorLogger* logger, MacroProcessor* macro_proc)
    : logger_(logger),
      evaluator_(std::make_unique<ExpressionEvaluator>(macro_proc, logger)),
      macro_processor_(macro_proc),
      optimization_enabled_(true),
      external_error_handler_(nullptr) {
    // Inicializar logger
    if (logger_) {
        logger_->info("ConditionalProcessor initialized");
    }
}

// ============================================================================
// Métodos de pilha de contextos
// ============================================================================

bool ConditionalProcessor::pushConditionalContext(bool condition, ConditionalType type,
                                                 const std::string& condition_str,
                                                 const PreprocessorPosition& pos) {
    try {
        ConditionalState state = condition ? ConditionalState::CONDITIONAL_TRUE 
                                           : ConditionalState::CONDITIONAL_FALSE;
        
        ConditionalContext context(type, state, condition_str, pos, getCurrentNestingLevel() + 1);
        context_stack_.push(context);
        
        // Atualizar estatísticas
        statistics_.total_conditionals++;
        if (condition) {
            statistics_.true_branches++;
        } else {
            statistics_.false_branches++;
        }
        
        if (logger_) {
            logger_->info("Pushed conditional context: " + conditionalTypeToString(type));
        }
        
        return true;
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Failed to push conditional context: " + std::string(e.what()));
        }
        statistics_.structure_errors++;
        return false;
    }
}

bool ConditionalProcessor::popConditionalContext() {
    try {
        if (context_stack_.empty()) {
            if (logger_) {
                logger_->error("Cannot pop from empty conditional context stack");
            }
            statistics_.structure_errors++;
            return false;
        }
        
        ConditionalContext context = context_stack_.top();
        context_stack_.pop();
        
        if (logger_) {
            logger_->info("Popped conditional context: " + conditionalTypeToString(context.type));
        }
        
        return true;
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Failed to pop conditional context: " + std::string(e.what()));
        }
        statistics_.structure_errors++;
        return false;
    }
}

ConditionalContext* ConditionalProcessor::getCurrentContext() {
    if (context_stack_.empty()) {
        return nullptr;
    }
    return &context_stack_.top();
}

const ConditionalContext* ConditionalProcessor::getCurrentContext() const {
    if (context_stack_.empty()) {
        return nullptr;
    }
    return &context_stack_.top();
}

// ============================================================================
// Métodos de avaliação de condições
// ============================================================================

bool ConditionalProcessor::evaluateCondition(const std::string& condition, 
                                           const PreprocessorPosition& pos) {
    try {
        if (!evaluator_) {
            if (logger_) {
                logger_->error("Expression evaluator not available");
            }
            statistics_.evaluation_errors++;
            return false;
        }
        
        return evaluator_->evaluateBooleanExpression(condition, pos);
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Failed to evaluate condition: " + std::string(e.what()));
        }
        statistics_.evaluation_errors++;
        return false;
    }
}

// ============================================================================
// Métodos de validação de estrutura
// ============================================================================

bool ConditionalProcessor::validateConditionalStructure() const {
    // Verificar se todas as estruturas condicionais estão balanceadas
    return context_stack_.empty();
}

bool ConditionalProcessor::handleNestedConditionals(const Directive& directive) {
    try {
        DirectiveType type = directive.getType();
        
        if (!isConditionalDirectiveType(type)) {
            return true; // Não é uma diretiva condicional
        }
        
        // Verificar nível de aninhamento
        const int MAX_NESTING_LEVEL = 64; // Limite padrão
        if (getCurrentNestingLevel() >= MAX_NESTING_LEVEL) {
            if (logger_) {
                logger_->error("Maximum nesting level exceeded");
            }
            statistics_.structure_errors++;
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Failed to handle nested conditionals: " + std::string(e.what()));
        }
        statistics_.structure_errors++;
        return false;
    }
}

// ============================================================================
// Métodos de processamento de diretivas
// ============================================================================

bool ConditionalProcessor::processIfDirective(const std::string& expression, 
                                            const PreprocessorPosition& pos) {
    try {
        if (expression.empty()) {
            if (logger_) {
                logger_->error("#if directive requires a condition");
            }
            statistics_.structure_errors++;
            return false;
        }
        
        bool result = evaluateCondition(expression, pos);
        return pushConditionalContext(result, ConditionalType::IF, expression, pos);
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Failed to process #if directive: " + std::string(e.what()));
        }
        statistics_.structure_errors++;
        return false;
    }
}

bool ConditionalProcessor::processIfdefDirective(const std::string& macro_name, 
                                               const PreprocessorPosition& pos) {
    try {
        if (macro_name.empty()) {
            if (logger_) {
                logger_->error("#ifdef directive requires a macro name");
            }
            statistics_.structure_errors++;
            return false;
        }
        
        bool result = false;
        if (macro_processor_) {
            result = macro_processor_->isDefined(macro_name);
        }
        
        return pushConditionalContext(result, ConditionalType::IFDEF, macro_name, pos);
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Failed to process #ifdef directive: " + std::string(e.what()));
        }
        statistics_.structure_errors++;
        return false;
    }
}

bool ConditionalProcessor::processIfndefDirective(const std::string& macro_name, 
                                                const PreprocessorPosition& pos) {
    try {
        if (macro_name.empty()) {
            if (logger_) {
                logger_->error("#ifndef directive requires a macro name");
            }
            statistics_.structure_errors++;
            return false;
        }
        
        bool result = true; // Padrão: não definido
        if (macro_processor_) {
            result = !macro_processor_->isDefined(macro_name);
        }
        
        return pushConditionalContext(result, ConditionalType::IFNDEF, macro_name, pos);
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Failed to process #ifndef directive: " + std::string(e.what()));
        }
        statistics_.structure_errors++;
        return false;
    }
}

bool ConditionalProcessor::processElseDirective(const PreprocessorPosition& pos) {
    try {
        if (context_stack_.empty()) {
            if (logger_) {
                logger_->error("#else without matching #if");
            }
            statistics_.structure_errors++;
            return false;
        }
        
        ConditionalContext& context = const_cast<ConditionalContext&>(context_stack_.top());
        if (context.has_else) {
            if (logger_) {
                logger_->error("Multiple #else for same #if");
            }
            statistics_.structure_errors++;
            return false;
        }
        
        context.has_else = true;
        context.type = ConditionalType::ELSE;
        
        // Inverter o estado: se era verdadeiro, agora é falso e vice-versa
        if (!context.has_true_branch) {
            context.state = ConditionalState::CONDITIONAL_TRUE;
            context.has_true_branch = true;
            statistics_.true_branches++;
        } else {
            context.state = ConditionalState::CONDITIONAL_SKIP;
        }
        
        return true;
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Failed to process #else directive: " + std::string(e.what()));
        }
        statistics_.structure_errors++;
        return false;
    }
}

bool ConditionalProcessor::processElifDirective(const std::string& expression, 
                                              const PreprocessorPosition& pos) {
    try {
        if (context_stack_.empty()) {
            if (logger_) {
                logger_->error("#elif without matching #if");
            }
            statistics_.structure_errors++;
            return false;
        }
        
        if (expression.empty()) {
            if (logger_) {
                logger_->error("#elif directive requires a condition");
            }
            statistics_.structure_errors++;
            return false;
        }
        
        ConditionalContext& context = const_cast<ConditionalContext&>(context_stack_.top());
        if (context.has_else) {
            if (logger_) {
                logger_->error("#elif after #else");
            }
            statistics_.structure_errors++;
            return false;
        }
        
        context.type = ConditionalType::ELIF;
        context.condition = expression;
        
        // Se já teve uma condição verdadeira, pular este bloco
        if (context.has_true_branch) {
            context.state = ConditionalState::CONDITIONAL_SKIP;
        } else {
            // Avaliar a nova condição
            bool result = evaluateCondition(expression, pos);
            if (result) {
                context.state = ConditionalState::CONDITIONAL_TRUE;
                context.has_true_branch = true;
                statistics_.true_branches++;
            } else {
                context.state = ConditionalState::CONDITIONAL_FALSE;
                statistics_.false_branches++;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Failed to process #elif directive: " + std::string(e.what()));
        }
        statistics_.structure_errors++;
        return false;
    }
}

bool ConditionalProcessor::processEndifDirective(const PreprocessorPosition& pos) {
    try {
        if (context_stack_.empty()) {
            if (logger_) {
                logger_->error("#endif without matching #if");
            }
            statistics_.structure_errors++;
            return false;
        }
        
        return popConditionalContext();
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Failed to process #endif directive: " + std::string(e.what()));
        }
        statistics_.structure_errors++;
        return false;
    }
}

// ============================================================================
// Métodos de controle de processamento
// ============================================================================

bool ConditionalProcessor::isConditionTrue() const {
    const ConditionalContext* context = getCurrentContext();
    if (!context) {
        return true; // Sem contexto condicional, processar normalmente
    }
    
    return context->state == ConditionalState::CONDITIONAL_TRUE;
}

bool ConditionalProcessor::shouldProcessBlock() const {
    // Verificar toda a pilha de contextos
    std::stack<ConditionalContext> temp_stack = context_stack_;
    
    while (!temp_stack.empty()) {
        const ConditionalContext& context = temp_stack.top();
        if (context.state != ConditionalState::CONDITIONAL_TRUE) {
            return false;
        }
        temp_stack.pop();
    }
    
    return true;
}

// ============================================================================
// Métodos de tratamento de erros
// ============================================================================

bool ConditionalProcessor::handleConditionalErrors(const std::string& error_msg, 
                                                  const PreprocessorPosition& pos) {
    if (logger_) {
        std::ostringstream oss;
        oss << "Conditional error at " << pos.filename 
            << ":" << pos.original_line << ":" << pos.original_column 
            << " - " << error_msg;
        logger_->error(oss.str());
    }
    
    statistics_.structure_errors++;
    return false;
}

// ============================================================================
// Métodos de otimização
// ============================================================================

void ConditionalProcessor::optimizeConditionalEvaluation(bool enable) {
    optimization_enabled_ = enable;
    
    if (logger_) {
        std::string status = enable ? "enabled" : "disabled";
        logger_->info("Conditional evaluation optimization " + status);
    }
}

// ============================================================================
// Métodos utilitários
// ============================================================================

int ConditionalProcessor::getCurrentNestingLevel() const {
    return static_cast<int>(context_stack_.size());
}

bool ConditionalProcessor::hasOpenConditionals() const {
    return !context_stack_.empty();
}

void ConditionalProcessor::reset() {
    while (!context_stack_.empty()) {
        context_stack_.pop();
    }
    
    statistics_ = ConditionalStats(); // Reset statistics
    
    if (logger_) {
        logger_->info("ConditionalProcessor reset completed");
    }
}

ConditionalStats ConditionalProcessor::getStatistics() const {
    ConditionalStats stats = statistics_;
    stats.max_nesting_level = getCurrentNestingLevel();
    return stats;
}

std::string ConditionalProcessor::generateStatusReport() const {
    std::ostringstream oss;
    oss << "ConditionalProcessor Status Report:\n";
    oss << "  Current nesting level: " << getCurrentNestingLevel() << "\n";
    oss << "  Has open conditionals: " << (hasOpenConditionals() ? "Yes" : "No") << "\n";
    
    ConditionalStats stats = getStatistics();
    oss << "  Total conditionals: " << stats.total_conditionals << "\n";
    oss << "  True branches: " << stats.true_branches << "\n";
    oss << "  False branches: " << stats.false_branches << "\n";
    oss << "  Max nesting level: " << stats.max_nesting_level << "\n";
    oss << "  Evaluation errors: " << stats.evaluation_errors << "\n";
    oss << "  Structure errors: " << stats.structure_errors << "\n";
    oss << "  Optimization enabled: " << (optimization_enabled_ ? "Yes" : "No") << "\n";
    
    return oss.str();
}

void ConditionalProcessor::setErrorHandler(void* errorHandler) {
    external_error_handler_ = errorHandler;
}

// ============================================================================
// Funções auxiliares
// ============================================================================

std::string conditionalTypeToString(ConditionalType type) {
    switch (type) {
        case ConditionalType::IF: return "if";
        case ConditionalType::IFDEF: return "ifdef";
        case ConditionalType::IFNDEF: return "ifndef";
        case ConditionalType::ELSE: return "else";
        case ConditionalType::ELIF: return "elif";
        default: return "unknown";
    }
}

std::string conditionalStateToString(ConditionalState state) {
    switch (state) {
        case ConditionalState::CONDITIONAL_TRUE: return "true";
        case ConditionalState::CONDITIONAL_FALSE: return "false";
        case ConditionalState::CONDITIONAL_SKIP: return "skip";
        default: return "unknown";
    }
}

bool isConditionalDirectiveType(DirectiveType type) {
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

} // namespace Preprocessor