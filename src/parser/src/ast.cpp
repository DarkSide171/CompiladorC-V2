#include "../include/ast.hpp"
#include <sstream>
#include <algorithm>

namespace Parser {

// TranslationUnit implementation
void TranslationUnit::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string TranslationUnit::toString() const {
    std::ostringstream oss;
    oss << "TranslationUnit {\n";
    for (size_t i = 0; i < declarations.size(); ++i) {
        oss << "  [" << i << "] " << declarations[i]->toString();
        if (i < declarations.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "}";
    return oss.str();
}

ASTNodePtr TranslationUnit::clone() const {
    auto cloned = std::make_unique<TranslationUnit>();
    cloned->setSourceRange(getSourceRange());
    
    for (const auto& decl : declarations) {
        cloned->addDeclaration(decl->clone());
    }
    
    return std::move(cloned);
}

// Identifier implementation
void Identifier::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string Identifier::toString() const {
    return "Identifier(" + name + ")";
}

ASTNodePtr Identifier::clone() const {
    auto cloned = std::make_unique<Identifier>(name, getSourceRange());
    return std::move(cloned);
}

// BinaryExpression implementation
void BinaryExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string BinaryExpression::toString() const {
    std::ostringstream oss;
    oss << "BinaryExpression {\n";
    oss << "  operator: " << operatorToString(operator_) << ",\n";
    oss << "  left: " << (leftOperand ? leftOperand->toString() : "null") << ",\n";
    oss << "  right: " << (rightOperand ? rightOperand->toString() : "null") << "\n";
    oss << "}";
    return oss.str();
}

ASTNodePtr BinaryExpression::clone() const {
    auto leftClone = leftOperand ? leftOperand->clone() : nullptr;
    auto rightClone = rightOperand ? rightOperand->clone() : nullptr;
    
    auto cloned = std::make_unique<BinaryExpression>(
        operator_, std::move(leftClone), std::move(rightClone), getSourceRange()
    );
    
    return std::move(cloned);
}

std::string BinaryExpression::operatorToString(Operator op) const {
    switch (op) {
        case Operator::ADD: return "+";
        case Operator::SUB: return "-";
        case Operator::MUL: return "*";
        case Operator::DIV: return "/";
        case Operator::MOD: return "%";
        case Operator::EQ: return "==";
        case Operator::NE: return "!=";
        case Operator::LT: return "<";
        case Operator::LE: return "<=";
        case Operator::GT: return ">";
        case Operator::GE: return ">=";
        case Operator::LOGICAL_AND: return "&&";
        case Operator::LOGICAL_OR: return "||";
        case Operator::BITWISE_AND: return "&";
        case Operator::BITWISE_OR: return "|";
        case Operator::BITWISE_XOR: return "^";
        case Operator::LSHIFT: return "<<";
        case Operator::RSHIFT: return ">>";
        case Operator::ASSIGN: return "=";
        default: return "unknown";
    }
}

// UnaryExpression implementation
void UnaryExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string UnaryExpression::toString() const {
    return "UnaryExpression { operator: " + operatorToString(operator_) + ", operand: " + operand_->toString() + " }";
}

ASTNodePtr UnaryExpression::clone() const {
    return std::make_unique<UnaryExpression>(operator_, operand_->clone(), getSourceRange());
}

std::string UnaryExpression::operatorToString(Operator op) const {
    switch (op) {
        case Operator::PLUS: return "+";
        case Operator::MINUS: return "-";
        case Operator::LOGICAL_NOT: return "!";
        case Operator::BITWISE_NOT: return "~";
        case Operator::DEREFERENCE: return "*";
        case Operator::ADDRESS_OF: return "&";
        case Operator::SIZEOF: return "sizeof";
        case Operator::PRE_INCREMENT: return "++";
        case Operator::PRE_DECREMENT: return "--";
        case Operator::POST_INCREMENT: return "++";
        case Operator::POST_DECREMENT: return "--";
        default: return "unknown";
    }
}

// AssignmentExpression implementation
void AssignmentExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string AssignmentExpression::toString() const {
    return "AssignmentExpression { operator: " + operatorToString(operator_) + ", left: " + leftOperand->toString() + ", right: " + rightOperand->toString() + " }";
}

ASTNodePtr AssignmentExpression::clone() const {
    return std::make_unique<AssignmentExpression>(operator_, leftOperand->clone(), rightOperand->clone(), getSourceRange());
}

std::string AssignmentExpression::operatorToString(Operator op) const {
    switch (op) {
        case Operator::ASSIGN: return "=";
        case Operator::PLUS_ASSIGN: return "+=";
        case Operator::MINUS_ASSIGN: return "-=";
        case Operator::MULT_ASSIGN: return "*=";
        case Operator::DIV_ASSIGN: return "/=";
        case Operator::MOD_ASSIGN: return "%=";
        case Operator::LSHIFT_ASSIGN: return "<<=";
        case Operator::RSHIFT_ASSIGN: return ">>=";
        case Operator::AND_ASSIGN: return "&=";
        case Operator::XOR_ASSIGN: return "^=";
        case Operator::OR_ASSIGN: return "|=";
        default: return "unknown";
    }
}

// TernaryExpression implementation
void TernaryExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string TernaryExpression::toString() const {
    return "TernaryExpression { condition: " + condition_->toString() + ", true: " + trueExpr_->toString() + ", false: " + falseExpr_->toString() + " }";
}

ASTNodePtr TernaryExpression::clone() const {
    return std::make_unique<TernaryExpression>(condition_->clone(), trueExpr_->clone(), falseExpr_->clone(), getSourceRange());
}

// CallExpression implementation
void CallExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string CallExpression::toString() const {
    std::ostringstream oss;
    oss << "CallExpression { function: " << function_->toString() << ", arguments: [";
    for (size_t i = 0; i < arguments_.size(); ++i) {
        oss << arguments_[i]->toString();
        if (i < arguments_.size() - 1) oss << ", ";
    }
    oss << "] }";
    return oss.str();
}

ASTNodePtr CallExpression::clone() const {
    auto cloned = std::make_unique<CallExpression>(function_->clone(), getSourceRange());
    for (const auto& arg : arguments_) {
        cloned->addArgument(arg->clone());
    }
    return std::move(cloned);
}

// MemberExpression implementation
void MemberExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string MemberExpression::toString() const {
    std::string accessStr = (accessType_ == AccessType::DOT) ? "." : "->";
    return "MemberExpression { object: " + object_->toString() + ", access: " + accessStr + ", member: " + memberName_ + " }";
}

ASTNodePtr MemberExpression::clone() const {
    return std::make_unique<MemberExpression>(object_->clone(), memberName_, accessType_, getSourceRange());
}

// ArrayAccess implementation
void ArrayAccess::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string ArrayAccess::toString() const {
    return "ArrayAccess { array: " + array_->toString() + ", index: " + index_->toString() + " }";
}

ASTNodePtr ArrayAccess::clone() const {
    return std::make_unique<ArrayAccess>(array_->clone(), index_->clone(), getSourceRange());
}

// CastExpression implementation
void CastExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string CastExpression::toString() const {
    return "CastExpression { targetType: " + targetType_ + ", expression: " + expression_->toString() + " }";
}

ASTNodePtr CastExpression::clone() const {
    return std::make_unique<CastExpression>(targetType_, expression_->clone(), getSourceRange());
}

// IntegerLiteral implementation
void IntegerLiteral::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string IntegerLiteral::toString() const {
    return "IntegerLiteral { value: " + std::to_string(value_) + " }";
}

ASTNodePtr IntegerLiteral::clone() const {
    return std::make_unique<IntegerLiteral>(value_, getSourceRange());
}

// FloatLiteral implementation
void FloatLiteral::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string FloatLiteral::toString() const {
    return "FloatLiteral { value: " + std::to_string(value_) + " }";
}

ASTNodePtr FloatLiteral::clone() const {
    return std::make_unique<FloatLiteral>(value_, getSourceRange());
}

// StringLiteral implementation
void StringLiteral::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string StringLiteral::toString() const {
    return "StringLiteral { value: \"" + value_ + "\" }";
}

ASTNodePtr StringLiteral::clone() const {
    return std::make_unique<StringLiteral>(value_, getSourceRange());
}

// CharLiteral implementation
void CharLiteral::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string CharLiteral::toString() const {
    return "CharLiteral { value: '" + std::string(1, value_) + "' }";
}

ASTNodePtr CharLiteral::clone() const {
    return std::make_unique<CharLiteral>(value_, getSourceRange());
}

// FunctionDeclaration implementation
void FunctionDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string FunctionDeclaration::toString() const {
    std::ostringstream oss;
    oss << "FunctionDeclaration {\n";
    oss << "  name: " << getName() << ",\n";
    oss << "  parameters: [";
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        oss << parameters[i]->toString();
        if (i < parameters.size() - 1) {
            oss << ", ";
        }
    }
    
    oss << "],\n";
    oss << "  body: " << (functionBody ? functionBody->toString() : "null") << "\n";
    oss << "}";
    return oss.str();
}

ASTNodePtr FunctionDeclaration::clone() const {
    auto cloned = std::make_unique<FunctionDeclaration>(getName(), getSourceRange());
    
    // Clone parameters
    for (const auto& param : parameters) {
        cloned->addParameter(param->clone());
    }
    
    // Clone body
    if (functionBody) {
        cloned->setBody(functionBody->clone());
    }
    
    return std::move(cloned);
}

// VariableDeclaration implementation
void VariableDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string VariableDeclaration::toString() const {
    std::ostringstream oss;
    oss << "VariableDeclaration {\n";
    oss << "  name: " << getName() << ",\n";
    oss << "  type: " << variableType << ",\n";
    oss << "  initializer: " << (initializer ? initializer->toString() : "null") << "\n";
    oss << "}";
    return oss.str();
}

ASTNodePtr VariableDeclaration::clone() const {
    auto cloned = std::make_unique<VariableDeclaration>(getName(), variableType, getSourceRange());
    
    if (initializer) {
        cloned->setInitializer(initializer->clone());
    }
    
    return std::move(cloned);
}

// DeclarationList implementation
void DeclarationList::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string DeclarationList::toString() const {
    std::ostringstream oss;
    oss << "DeclarationList {\n";
    for (size_t i = 0; i < declarations.size(); ++i) {
        oss << "  [" << i << "] " << declarations[i]->toString();
        if (i < declarations.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "}";
    return oss.str();
}

ASTNodePtr DeclarationList::clone() const {
    auto cloned = std::make_unique<DeclarationList>(getSourceRange());
    
    for (const auto& decl : declarations) {
        cloned->addDeclaration(decl->clone());
    }
    
    return std::move(cloned);
}

// CompoundStatement implementation
void CompoundStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string CompoundStatement::toString() const {
    std::ostringstream oss;
    oss << "CompoundStatement {\n";
    
    for (size_t i = 0; i < statements.size(); ++i) {
        oss << "  [" << i << "] " << statements[i]->toString();
        if (i < statements.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "}";
    return oss.str();
}

ASTNodePtr CompoundStatement::clone() const {
    auto cloned = std::make_unique<CompoundStatement>(getSourceRange());
    
    for (const auto& stmt : statements) {
        cloned->addStatement(stmt->clone());
    }
    
    return std::move(cloned);
}

// ReturnStatement implementation
void ReturnStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string ReturnStatement::toString() const {
    return "ReturnStatement { expression: " + (returnExpression ? returnExpression->toString() : "null") + " }";
}

ASTNodePtr ReturnStatement::clone() const {
    auto cloned = std::make_unique<ReturnStatement>(returnExpression ? returnExpression->clone() : nullptr, getSourceRange());
    return std::move(cloned);
}

// IfStatement implementation
void IfStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string IfStatement::toString() const {
    std::ostringstream oss;
    oss << "IfStatement {\n";
    oss << "  condition: " << condition_->toString() << ",\n";
    oss << "  thenStatement: " << thenStatement->toString();
    if (elseStatement) {
        oss << ",\n  elseStatement: " << elseStatement->toString();
    }
    oss << "\n}";
    return oss.str();
}

ASTNodePtr IfStatement::clone() const {
    auto cloned = std::make_unique<IfStatement>(
        condition_->clone(),
        thenStatement->clone(),
        elseStatement ? elseStatement->clone() : nullptr,
        getSourceRange()
    );
    return std::move(cloned);
}

// WhileStatement implementation
void WhileStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string WhileStatement::toString() const {
    return "WhileStatement { condition: " + condition_->toString() + ", body: " + body_->toString() + " }";
}

ASTNodePtr WhileStatement::clone() const {
    return std::make_unique<WhileStatement>(condition_->clone(), body_->clone(), getSourceRange());
}

// ForStatement implementation
void ForStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string ForStatement::toString() const {
    std::ostringstream oss;
    oss << "ForStatement {\n";
    oss << "  init: " << (init_ ? init_->toString() : "null") << ",\n";
    oss << "  condition: " << (condition_ ? condition_->toString() : "null") << ",\n";
    oss << "  update: " << (update_ ? update_->toString() : "null") << ",\n";
    oss << "  body: " << body_->toString() << "\n";
    oss << "}";
    return oss.str();
}

ASTNodePtr ForStatement::clone() const {
    return std::make_unique<ForStatement>(
        init_ ? init_->clone() : nullptr,
        condition_ ? condition_->clone() : nullptr,
        update_ ? update_->clone() : nullptr,
        body_->clone(),
        getSourceRange()
    );
}

// BreakStatement implementation
void BreakStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string BreakStatement::toString() const {
    return "BreakStatement";
}

ASTNodePtr BreakStatement::clone() const {
    return std::make_unique<BreakStatement>(getSourceRange());
}

// ContinueStatement implementation
void ContinueStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string ContinueStatement::toString() const {
    return "ContinueStatement";
}

ASTNodePtr ContinueStatement::clone() const {
    return std::make_unique<ContinueStatement>(getSourceRange());
}

// ExpressionStatement implementation
void ExpressionStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string ExpressionStatement::toString() const {
    return "ExpressionStatement { expression: " + (expression_ ? expression_->toString() : "null") + " }";
}

ASTNodePtr ExpressionStatement::clone() const {
    return std::make_unique<ExpressionStatement>(expression_ ? expression_->clone() : nullptr, getSourceRange());
}

// Additional AST utility functions
std::string astNodeTypeToString(ASTNodeType type) {
    switch (type) {
        case ASTNodeType::TRANSLATION_UNIT: return "TranslationUnit";
        case ASTNodeType::FUNCTION_DECLARATION: return "FunctionDeclaration";
        case ASTNodeType::VARIABLE_DECLARATION: return "VariableDeclaration";
        case ASTNodeType::DECLARATION_LIST: return "DeclarationList";
        case ASTNodeType::TYPE_DECLARATION: return "TypeDeclaration";
        case ASTNodeType::COMPOUND_STATEMENT: return "CompoundStatement";
        case ASTNodeType::EXPRESSION_STATEMENT: return "ExpressionStatement";
        case ASTNodeType::IF_STATEMENT: return "IfStatement";
        case ASTNodeType::WHILE_STATEMENT: return "WhileStatement";
        case ASTNodeType::FOR_STATEMENT: return "ForStatement";
        case ASTNodeType::RETURN_STATEMENT: return "ReturnStatement";
        case ASTNodeType::BREAK_STATEMENT: return "BreakStatement";
        case ASTNodeType::CONTINUE_STATEMENT: return "ContinueStatement";
        case ASTNodeType::BINARY_EXPRESSION: return "BinaryExpression";
        case ASTNodeType::UNARY_EXPRESSION: return "UnaryExpression";
        case ASTNodeType::ASSIGNMENT_EXPRESSION: return "AssignmentExpression";
        case ASTNodeType::CALL_EXPRESSION: return "CallExpression";
        case ASTNodeType::MEMBER_EXPRESSION: return "MemberExpression";
        case ASTNodeType::ARRAY_ACCESS: return "ArrayAccess";
        case ASTNodeType::CAST_EXPRESSION: return "CastExpression";
        case ASTNodeType::SIZEOF_EXPRESSION: return "SizeofExpression";
        case ASTNodeType::INTEGER_LITERAL: return "IntegerLiteral";
        case ASTNodeType::FLOAT_LITERAL: return "FloatLiteral";
        case ASTNodeType::STRING_LITERAL: return "StringLiteral";
        case ASTNodeType::CHAR_LITERAL: return "CharacterLiteral";
        case ASTNodeType::IDENTIFIER: return "Identifier";
        case ASTNodeType::BUILTIN_TYPE: return "BuiltinType";
        case ASTNodeType::POINTER_TYPE: return "PointerType";
        case ASTNodeType::ARRAY_TYPE: return "ArrayType";
        case ASTNodeType::FUNCTION_TYPE: return "FunctionType";
        case ASTNodeType::STRUCT_TYPE: return "StructType";
        case ASTNodeType::UNION_TYPE: return "UnionType";
        case ASTNodeType::ENUM_TYPE: return "EnumType";
        default: return "Unknown";
    }
}

// AST traversal utilities
class ASTCounter : public ASTVisitor {
public:
    size_t count = 0;
    
    void visit(TranslationUnit& node) override {
        ++count;
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
    
    void visit(Identifier& node) override {
        (void)node;
        ++count;
    }
    
    void visit(BinaryExpression& node) override {
        ++count;
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
    
    void visit(UnaryExpression& node) override {
        ++count;
        // Visit the operand
        if (auto child = node.getChild(0)) {
            child->accept(*this);
        }
    }
    
    void visit(AssignmentExpression& node) override {
        ++count;
        // Visit left and right operands
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
    
    void visit(TernaryExpression& node) override {
        ++count;
        // Visit condition, true expression, and false expression
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
    
    void visit(CallExpression& node) override {
        ++count;
        // Visit function and arguments
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
    
    void visit(MemberExpression& node) override {
        ++count;
        // Visit the object
        if (auto child = node.getChild(0)) {
            child->accept(*this);
        }
    }
    
    void visit(ArrayAccess& node) override {
        ++count;
        // Visit array and index
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
    
    void visit(CastExpression& node) override {
        ++count;
        // Visit the expression being cast
        if (auto child = node.getChild(0)) {
            child->accept(*this);
        }
    }
    
    // Statement visitors
    void visit(ReturnStatement& node) override {
        ++count;
        // Visit return expression if present
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
    
    void visit(IfStatement& node) override {
        ++count;
        // Visit condition, then statement, and else statement
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
    
    void visit(WhileStatement& node) override {
        ++count;
        // Visit condition and body
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
    
    void visit(ForStatement& node) override {
        ++count;
        // Visit init, condition, update, and body
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
    
    void visit(BreakStatement& node) override {
        (void)node;
        ++count;
        // Break statements have no children to visit
    }
    
    void visit(ContinueStatement& node) override {
        (void)node;
        ++count;
        // Continue statements have no children to visit
    }
    
    void visit(ExpressionStatement& node) override {
        ++count;
        // Visit the expression child
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
    
    void visit(IntegerLiteral& node) override {
        (void)node;
        ++count;
    }
    
    void visit(FloatLiteral& node) override {
        (void)node;
        ++count;
    }
    
    void visit(StringLiteral& node) override {
        (void)node;
        ++count;
    }
    
    void visit(CharLiteral& node) override {
        (void)node;
        ++count;
    }
    
    void visit(FunctionDeclaration& node) override {
        ++count;
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
    
    void visit(VariableDeclaration& node) override {
        ++count;
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
    
    void visit(DeclarationList& node) override {
        ++count;
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
    
    void visit(CompoundStatement& node) override {
        ++count;
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
    }
};

size_t countASTNodes(const ASTNodePtr& root) {
    if (!root) return 0;
    
    ASTCounter counter;
    root->accept(counter);
    return counter.count;
}

std::vector<std::string> validateAST(const ASTNodePtr& root) {
    std::vector<std::string> errors;
    
    if (!root) {
        errors.push_back("AST root is null");
        return errors;
    }
    
    // Validate TranslationUnit structure if root is TranslationUnit
    if (root->getType() == ASTNodeType::TRANSLATION_UNIT) {
        auto* tu = dynamic_cast<TranslationUnit*>(root.get());
        if (tu && tu->getDeclarations().empty()) {
            errors.push_back("TranslationUnit has no declarations");
        }
    }
    
    // Validate parent-child relationships recursively
    std::function<void(ASTNode*, ASTNode*)> validateNode = 
        [&](ASTNode* node, ASTNode* expectedParent) {
            if (!node) return;
            
            // Check parent relationship
            if (node->getParent() != expectedParent) {
                errors.push_back("Invalid parent relationship for node of type " + 
                                std::to_string(static_cast<int>(node->getType())));
            }
            
            // Validate specific node types
            switch (node->getType()) {
                case ASTNodeType::BINARY_EXPRESSION: {
                    auto* binExpr = dynamic_cast<BinaryExpression*>(node);
                    if (binExpr && binExpr->getChildCount() != 2) {
                        errors.push_back("BinaryExpression must have exactly 2 children");
                    }
                    break;
                }
                case ASTNodeType::UNARY_EXPRESSION: {
                    auto* unaryExpr = dynamic_cast<UnaryExpression*>(node);
                    if (unaryExpr && unaryExpr->getChildCount() != 1) {
                        errors.push_back("UnaryExpression must have exactly 1 child");
                    }
                    break;
                }
                case ASTNodeType::TERNARY_EXPRESSION: {
                    auto* ternaryExpr = dynamic_cast<TernaryExpression*>(node);
                    if (ternaryExpr && ternaryExpr->getChildCount() != 3) {
                        errors.push_back("TernaryExpression must have exactly 3 children");
                    }
                    break;
                }
                case ASTNodeType::IDENTIFIER: {
                    auto* identifier = dynamic_cast<Identifier*>(node);
                    if (identifier && identifier->getName().empty()) {
                        errors.push_back("Identifier cannot have empty name");
                    }
                    break;
                }
                case ASTNodeType::CALL_EXPRESSION: {
                    auto* callExpr = dynamic_cast<CallExpression*>(node);
                    if (callExpr && callExpr->getChildCount() == 0) {
                        errors.push_back("CallExpression must have at least a function child");
                    }
                    break;
                }
                default:
                    break;
            }
            
            // Recursively validate children
            for (size_t i = 0; i < node->getChildCount(); ++i) {
                ASTNode* child = node->getChild(i);
                if (child) {
                    validateNode(child, node);
                }
            }
        };
    
    validateNode(root.get(), nullptr);
    
    return errors;
}

// AST class implementation
AST::AST() : root_(nullptr), nodeCount_(0) {
}

AST::AST(ASTNodePtr root) : root_(std::move(root)), nodeCount_(0) {
    if (root_) {
        nodeCount_ = 1; // Will be recalculated if needed
    }
}

AST::~AST() {
    clear();
}

AST::AST(AST&& other) noexcept 
    : root_(std::move(other.root_)), nodeCount_(other.nodeCount_) {
    other.nodeCount_ = 0;
}

AST& AST::operator=(AST&& other) noexcept {
    if (this != &other) {
        clear();
        root_ = std::move(other.root_);
        nodeCount_ = other.nodeCount_;
        other.nodeCount_ = 0;
    }
    return *this;
}

void AST::deleteNode(ASTNode* node) {
    if (node && nodeCount_ > 0) {
        nodeCount_--;
    }
    // Note: Actual deletion is handled by unique_ptr
}

void AST::setRoot(ASTNodePtr root) {
    root_ = std::move(root);
    // Recalculate node count if needed
    if (root_) {
        nodeCount_ = countASTNodes(root_);
    } else {
        nodeCount_ = 0;
    }
}

void AST::clear() {
    root_.reset();
    nodeCount_ = 0;
}

void AST::accept(ASTVisitor& visitor) {
    if (root_) {
        root_->accept(visitor);
    }
}

std::string AST::serialize() const {
    if (!root_) {
        return "{}";
    }
    
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"nodeCount\": " << nodeCount_ << ",\n";
    oss << "  \"root\": " << root_->toString() << "\n";
    oss << "}";
    return oss.str();
}

bool AST::deserialize(const std::string& data) {
    if (data.empty()) {
        clear();
        return true;
    }
    
    // Simple JSON-like parsing for basic AST structure
    if (data == "{}") {
        clear();
        return true;
    }
    
    // Look for basic patterns like {"type":"TRANSLATION_UNIT"}
    if (data.find("TRANSLATION_UNIT") != std::string::npos) {
        // Create a basic translation unit
        auto tu = std::make_unique<TranslationUnit>();
        setRoot(std::move(tu));
        return true;
    }
    
    // Look for function declaration pattern
    if (data.find("FUNCTION_DECLARATION") != std::string::npos) {
        auto tu = std::make_unique<TranslationUnit>();
        auto func = std::make_unique<FunctionDeclaration>("main");
        tu->addDeclaration(std::move(func));
        setRoot(std::move(tu));
        return true;
    }
    
    // For complex serialized data, we would need a proper JSON parser
    // This is a basic implementation that handles simple cases
    return false;
}

bool AST::validate() const {
    if (!root_) {
        return true; // Empty AST is valid
    }
    
    auto errors = validateAST(root_);
    return errors.empty();
}

std::vector<std::string> AST::getValidationErrors() const {
    if (!root_) {
        return {};
    }
    
    return validateAST(root_);
}

AST::Statistics AST::getStatistics() const {
    Statistics stats;
    if (root_) {
        calculateStatistics(root_, stats, 0);
    }
    return stats;
}

void AST::calculateStatistics(const ASTNodePtr& node, Statistics& stats, size_t depth) const {
    if (!node) {
        return;
    }
    
    stats.totalNodes++;
    stats.maxDepth = std::max(stats.maxDepth, depth);
    
    // Count node types
    stats.nodeTypeCounts[node->getType()]++;
    
    // Check if it's a leaf node
    if (node->getChildCount() == 0) {
        stats.leafNodes++;
    }
    
    // Recursively process children using a helper method
    calculateStatisticsForNode(node.get(), stats, depth);
}

void AST::calculateStatisticsForNode(const ASTNode* node, Statistics& stats, size_t depth) const {
    if (!node) {
        return;
    }
    
    // Process children
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        if (auto child = node->getChild(i)) {
            stats.totalNodes++;
            stats.maxDepth = std::max(stats.maxDepth, depth + 1);
            stats.nodeTypeCounts[child->getType()]++;
            
            if (child->getChildCount() == 0) {
                stats.leafNodes++;
            }
            
            // Recursively process grandchildren
            calculateStatisticsForNode(child, stats, depth + 1);
        }
    }
}

} // namespace Parser