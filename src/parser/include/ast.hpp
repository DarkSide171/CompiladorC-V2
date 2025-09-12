#ifndef AST_HPP
#define AST_HPP

#include "parser_types.hpp"
#include <vector>
#include <memory>
#include <string>
#include <map>

namespace Parser {

// Forward declarations
class ASTVisitor;
class AST;

// Base AST Node class
class ASTNode {
public:
    ASTNode(ASTNodeType type, const SourceRange& range = SourceRange())
        : nodeType(type), sourceRange(range) {}
    
    virtual ~ASTNode() = default;
    
    // Core interface
    ASTNodeType getType() const { return nodeType; }
    const SourceRange& getSourceRange() const { return sourceRange; }
    void setSourceRange(const SourceRange& range) { sourceRange = range; }
    
    // Value management
    virtual std::string getValue() const { return ""; }
    virtual void setValue(const std::string& value) { (void)value; }
    
    // Parent-child relationships
    ASTNode* getParent() const { return parent; }
    void setParent(ASTNode* parentNode) { parent = parentNode; }
    
    // Child management
    virtual size_t getChildCount() const { return 0; }
    virtual ASTNode* getChild(size_t index) const { (void)index; return nullptr; }
    virtual void addChild(ASTNodePtr child) { (void)child; }
    virtual void removeChild(size_t index) { (void)index; }
    
    // Visitor pattern
    virtual void accept(ASTVisitor& visitor) = 0;
    
    // Utility methods
    virtual std::string toString() const = 0;
    virtual ASTNodePtr clone() const = 0;
    
protected:
    ASTNodeType nodeType;
    SourceRange sourceRange;
    ASTNode* parent = nullptr;
};



// Translation Unit (root of AST)
class TranslationUnit : public ASTNode {
public:
    TranslationUnit() : ASTNode(ASTNodeType::TRANSLATION_UNIT) {}
    
    void addDeclaration(ASTNodePtr decl) {
        declarations.push_back(std::move(decl));
    }
    
    const std::vector<ASTNodePtr>& getDeclarations() const {
        return declarations;
    }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return declarations.size(); }
    ASTNode* getChild(size_t index) const override {
        return index < declarations.size() ? declarations[index].get() : nullptr;
    }
    
private:
    std::vector<ASTNodePtr> declarations;
};

// Expression base class
class Expression : public ASTNode {
public:
    Expression(ASTNodeType type, const SourceRange& range = SourceRange())
        : ASTNode(type, range) {}
    
    virtual ~Expression() = default;
};

// Statement base class
class Statement : public ASTNode {
public:
    Statement(ASTNodeType type, const SourceRange& range = SourceRange())
        : ASTNode(type, range) {}
    
    virtual ~Statement() = default;
};

// Declaration base class
class Declaration : public ASTNode {
public:
    Declaration(ASTNodeType type, const std::string& name = "", const SourceRange& range = SourceRange())
        : ASTNode(type, range), declarationName(name) {}
    
    virtual ~Declaration() = default;
    
    const std::string& getName() const { return declarationName; }
    void setName(const std::string& name) { declarationName = name; }
    
protected:
    std::string declarationName;
};

// Variable declaration
class VariableDeclaration : public Declaration {
public:
    VariableDeclaration(const std::string& name, const std::string& type, const SourceRange& range = SourceRange())
        : Declaration(ASTNodeType::VARIABLE_DECLARATION, name, range), variableType(type) {}
    
    const std::string& getType() const { return variableType; }
    void setType(const std::string& type) { variableType = type; }
    
    void setInitializer(ASTNodePtr init) { initializer = std::move(init); }
    const ASTNodePtr& getInitializer() const { return initializer; }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override {
        return initializer ? 1 : 0;
    }
    ASTNode* getChild(size_t index) const override {
        if (index == 0 && initializer) {
            return initializer.get();
        }
        return nullptr;
    }
    
private:
    std::string variableType;
    ASTNodePtr initializer;
};

// Declaration list for multiple variable declarations
class DeclarationList : public Declaration {
public:
    DeclarationList(const SourceRange& range = SourceRange())
        : Declaration(ASTNodeType::DECLARATION_LIST, "", range) {}
    
    void addDeclaration(ASTNodePtr decl) {
        declarations.push_back(std::move(decl));
    }
    
    const std::vector<ASTNodePtr>& getDeclarations() const {
        return declarations;
    }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override {
        return declarations.size();
    }
    ASTNode* getChild(size_t index) const override {
        if (index < declarations.size()) {
            return declarations[index].get();
        }
        return nullptr;
    }
    
private:
    std::vector<ASTNodePtr> declarations;
};

// Identifier expression
class Identifier : public Expression {
public:
    Identifier(const std::string& name, const SourceRange& range = SourceRange())
        : Expression(ASTNodeType::IDENTIFIER, range), name(name) {}
    
    const std::string& getName() const { return name; }
    
    // ASTNode interface
    std::string getValue() const override { return name; }
    void setValue(const std::string& value) override { name = value; }
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    
private:
    std::string name;
};

// Unary expression
class UnaryExpression : public Expression {
public:
    enum class Operator {
        PLUS, MINUS, LOGICAL_NOT, BITWISE_NOT,
        DEREFERENCE, ADDRESS_OF, SIZEOF,
        PRE_INCREMENT, PRE_DECREMENT,
        POST_INCREMENT, POST_DECREMENT
    };
    
    UnaryExpression(Operator op, ASTNodePtr operand, const SourceRange& range = SourceRange())
        : Expression(ASTNodeType::UNARY_EXPRESSION, range), 
          operator_(op), operand_(std::move(operand)) {}
    
    Operator getOperator() const { return operator_; }
    const ASTNodePtr& getOperand() const { return operand_; }
    
    std::string operatorToString(Operator op) const;
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 1; }
    ASTNode* getChild(size_t index) const override {
        return (index == 0) ? operand_.get() : nullptr;
    }
    
private:
    Operator operator_;
    ASTNodePtr operand_;
};

// Assignment expression
class AssignmentExpression : public Expression {
public:
    enum class Operator {
        ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, MULT_ASSIGN, DIV_ASSIGN,
        MOD_ASSIGN, LSHIFT_ASSIGN, RSHIFT_ASSIGN,
        AND_ASSIGN, XOR_ASSIGN, OR_ASSIGN
    };
    
    AssignmentExpression(Operator op, ASTNodePtr left, ASTNodePtr right, const SourceRange& range = SourceRange())
        : Expression(ASTNodeType::ASSIGNMENT_EXPRESSION, range),
          operator_(op), leftOperand(std::move(left)), rightOperand(std::move(right)) {}
    
    Operator getOperator() const { return operator_; }
    const ASTNodePtr& getLeft() const { return leftOperand; }
    const ASTNodePtr& getRight() const { return rightOperand; }
    
    std::string operatorToString(Operator op) const;
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 2; }
    ASTNode* getChild(size_t index) const override {
        if (index == 0) return leftOperand.get();
        if (index == 1) return rightOperand.get();
        return nullptr;
    }
    
private:
    Operator operator_;
    ASTNodePtr leftOperand;
    ASTNodePtr rightOperand;
};

// Ternary/Conditional expression
class TernaryExpression : public Expression {
public:
    TernaryExpression(ASTNodePtr condition, ASTNodePtr trueExpr, ASTNodePtr falseExpr, const SourceRange& range = SourceRange())
        : Expression(ASTNodeType::TERNARY_EXPRESSION, range),
          condition_(std::move(condition)), trueExpr_(std::move(trueExpr)), falseExpr_(std::move(falseExpr)) {}
    
    const ASTNodePtr& getCondition() const { return condition_; }
    const ASTNodePtr& getTrueExpression() const { return trueExpr_; }
    const ASTNodePtr& getFalseExpression() const { return falseExpr_; }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 3; }
    ASTNode* getChild(size_t index) const override {
        if (index == 0) return condition_.get();
        if (index == 1) return trueExpr_.get();
        if (index == 2) return falseExpr_.get();
        return nullptr;
    }
    
private:
    ASTNodePtr condition_;
    ASTNodePtr trueExpr_;
    ASTNodePtr falseExpr_;
};

// Call expression
class CallExpression : public Expression {
public:
    CallExpression(ASTNodePtr function, const SourceRange& range = SourceRange())
        : Expression(ASTNodeType::CALL_EXPRESSION, range), function_(std::move(function)) {}
    
    const ASTNodePtr& getFunction() const { return function_; }
    
    void addArgument(ASTNodePtr arg) {
        arguments_.push_back(std::move(arg));
    }
    
    const std::vector<ASTNodePtr>& getArguments() const { return arguments_; }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 1 + arguments_.size(); }
    ASTNode* getChild(size_t index) const override {
        if (index == 0) return function_.get();
        if (index - 1 < arguments_.size()) return arguments_[index - 1].get();
        return nullptr;
    }
    
private:
    ASTNodePtr function_;
    std::vector<ASTNodePtr> arguments_;
};

// Member access expression
class MemberExpression : public Expression {
public:
    enum class AccessType { DOT, ARROW };
    
    MemberExpression(ASTNodePtr object, const std::string& member, AccessType type, const SourceRange& range = SourceRange())
        : Expression(ASTNodeType::MEMBER_EXPRESSION, range),
          object_(std::move(object)), memberName_(member), accessType_(type) {}
    
    const ASTNodePtr& getObject() const { return object_; }
    const std::string& getMemberName() const { return memberName_; }
    AccessType getAccessType() const { return accessType_; }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 1; }
    ASTNode* getChild(size_t index) const override {
        return (index == 0) ? object_.get() : nullptr;
    }
    
private:
    ASTNodePtr object_;
    std::string memberName_;
    AccessType accessType_;
};

// Array access expression
class ArrayAccess : public Expression {
public:
    ArrayAccess(ASTNodePtr array, ASTNodePtr index, const SourceRange& range = SourceRange())
        : Expression(ASTNodeType::ARRAY_ACCESS, range),
          array_(std::move(array)), index_(std::move(index)) {}
    
    const ASTNodePtr& getArray() const { return array_; }
    const ASTNodePtr& getIndex() const { return index_; }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 2; }
    ASTNode* getChild(size_t index) const override {
        if (index == 0) return array_.get();
        if (index == 1) return index_.get();
        return nullptr;
    }
    
private:
    ASTNodePtr array_;
    ASTNodePtr index_;
};

// Cast expression
class CastExpression : public Expression {
public:
    CastExpression(const std::string& targetType, ASTNodePtr expression, const SourceRange& range = SourceRange())
        : Expression(ASTNodeType::CAST_EXPRESSION, range),
          targetType_(targetType), expression_(std::move(expression)) {}
    
    const std::string& getTargetType() const { return targetType_; }
    const ASTNodePtr& getExpression() const { return expression_; }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 1; }
    ASTNode* getChild(size_t index) const override {
        return (index == 0) ? expression_.get() : nullptr;
    }
    
private:
    std::string targetType_;
    ASTNodePtr expression_;
};

// Literal expressions
class IntegerLiteral : public Expression {
public:
    IntegerLiteral(long long value, const SourceRange& range = SourceRange())
        : Expression(ASTNodeType::INTEGER_LITERAL, range), value_(value) {}
    
    long long getIntegerValue() const { return value_; }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 0; }
    ASTNode* getChild(size_t) const override { return nullptr; }
    
private:
    long long value_;
};

class FloatLiteral : public Expression {
public:
    FloatLiteral(double value, const SourceRange& range = SourceRange())
        : Expression(ASTNodeType::FLOAT_LITERAL, range), value_(value) {}
    
    double getFloatValue() const { return value_; }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 0; }
    ASTNode* getChild(size_t) const override { return nullptr; }
    
private:
    double value_;
};

class StringLiteral : public Expression {
public:
    StringLiteral(const std::string& value, const SourceRange& range = SourceRange())
        : Expression(ASTNodeType::STRING_LITERAL, range), value_(value) {}
    
    const std::string& getStringValue() const { return value_; }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 0; }
    ASTNode* getChild(size_t) const override { return nullptr; }
    
private:
    std::string value_;
};

class CharLiteral : public Expression {
public:
    CharLiteral(char value, const SourceRange& range = SourceRange())
        : Expression(ASTNodeType::CHAR_LITERAL, range), value_(value) {}
    
    char getCharValue() const { return value_; }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 0; }
    ASTNode* getChild(size_t) const override { return nullptr; }
    
private:
    char value_;
};

// Binary expression
class BinaryExpression : public Expression {
public:
    enum class Operator {
        ADD, SUB, MUL, DIV, MOD,
        EQ, NE, LT, LE, GT, GE,
        LOGICAL_AND, LOGICAL_OR,
        BITWISE_AND, BITWISE_OR, BITWISE_XOR,
        LSHIFT, RSHIFT,
        ASSIGN
    };
    
    BinaryExpression(Operator op, ASTNodePtr left, ASTNodePtr right, const SourceRange& range = SourceRange())
        : Expression(ASTNodeType::BINARY_EXPRESSION, range), 
          operator_(op), leftOperand(std::move(left)), rightOperand(std::move(right)) {}
    
    Operator getOperator() const { return operator_; }
    const ASTNodePtr& getLeft() const { return leftOperand; }
    const ASTNodePtr& getRight() const { return rightOperand; }
    
    // Utility method
    std::string operatorToString(Operator op) const;
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 2; }
    ASTNode* getChild(size_t index) const override {
        if (index == 0) return leftOperand.get();
        if (index == 1) return rightOperand.get();
        return nullptr;
    }
    
private:
    Operator operator_;
    ASTNodePtr leftOperand;
    ASTNodePtr rightOperand;
};

// Function declaration
class FunctionDeclaration : public Declaration {
public:
    FunctionDeclaration(const std::string& name, const SourceRange& range = SourceRange())
        : Declaration(ASTNodeType::FUNCTION_DECLARATION, name, range) {}
    
    FunctionDeclaration(const std::string& name, const std::string& returnType, const SourceRange& range = SourceRange())
        : Declaration(ASTNodeType::FUNCTION_DECLARATION, name, range), returnType_(returnType) {}
    
    void setBody(ASTNodePtr body) { functionBody = std::move(body); }
    const ASTNodePtr& getBody() const { return functionBody; }
    
    void setReturnType(const std::string& returnType) { returnType_ = returnType; }
    const std::string& getReturnType() const { return returnType_; }
    
    void addParameter(ASTNodePtr param) {
        parameters.push_back(std::move(param));
    }
    
    const std::vector<ASTNodePtr>& getParameters() const {
        return parameters;
    }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override {
        return parameters.size() + (functionBody ? 1 : 0);
    }
    ASTNode* getChild(size_t index) const override {
        if (index < parameters.size()) {
            return parameters[index].get();
        }
        if (index == parameters.size() && functionBody) {
            return functionBody.get();
        }
        return nullptr;
    }
    
    // Override getValue to return function name
    std::string getValue() const override { return getName(); }
    
private:
    std::vector<ASTNodePtr> parameters;
    ASTNodePtr functionBody;
    std::string returnType_;
};

// Compound statement (block)
class CompoundStatement : public Statement {
public:
    CompoundStatement(const SourceRange& range = SourceRange())
        : Statement(ASTNodeType::COMPOUND_STATEMENT, range) {}
    
    void addStatement(ASTNodePtr stmt) {
        statements.push_back(std::move(stmt));
    }
    
    const std::vector<ASTNodePtr>& getStatements() const {
        return statements;
    }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return statements.size(); }
    ASTNode* getChild(size_t index) const override {
        return index < statements.size() ? statements[index].get() : nullptr;
    }
    
private:
    std::vector<ASTNodePtr> statements;
};

// Return statement
class ReturnStatement : public Statement {
public:
    ReturnStatement(const SourceRange& range = SourceRange())
        : Statement(ASTNodeType::RETURN_STATEMENT, range) {}
    
    ReturnStatement(ASTNodePtr expression, const SourceRange& range = SourceRange())
        : Statement(ASTNodeType::RETURN_STATEMENT, range), returnExpression(std::move(expression)) {}
    
    const ASTNodePtr& getExpression() const { return returnExpression; }
    void setExpression(ASTNodePtr expr) { returnExpression = std::move(expr); }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return returnExpression ? 1 : 0; }
    ASTNode* getChild(size_t index) const override {
        return (index == 0 && returnExpression) ? returnExpression.get() : nullptr;
    }
    
private:
    ASTNodePtr returnExpression;
};

// If statement
class IfStatement : public Statement {
public:
    IfStatement(ASTNodePtr condition, ASTNodePtr thenStmt, const SourceRange& range = SourceRange())
        : Statement(ASTNodeType::IF_STATEMENT, range), 
          condition_(std::move(condition)), thenStatement(std::move(thenStmt)) {}
    
    IfStatement(ASTNodePtr condition, ASTNodePtr thenStmt, ASTNodePtr elseStmt, const SourceRange& range = SourceRange())
        : Statement(ASTNodeType::IF_STATEMENT, range),
          condition_(std::move(condition)), thenStatement(std::move(thenStmt)), elseStatement(std::move(elseStmt)) {}
    
    const ASTNodePtr& getCondition() const { return condition_; }
    const ASTNodePtr& getThenStatement() const { return thenStatement; }
    const ASTNodePtr& getElseStatement() const { return elseStatement; }
    
    void setElseStatement(ASTNodePtr elseStmt) { elseStatement = std::move(elseStmt); }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 2 + (elseStatement ? 1 : 0); }
    ASTNode* getChild(size_t index) const override {
        if (index == 0) return condition_.get();
        if (index == 1) return thenStatement.get();
        if (index == 2 && elseStatement) return elseStatement.get();
        return nullptr;
    }
    
private:
    ASTNodePtr condition_;
    ASTNodePtr thenStatement;
    ASTNodePtr elseStatement;
};

// While statement
class WhileStatement : public Statement {
public:
    WhileStatement(ASTNodePtr condition, ASTNodePtr body, const SourceRange& range = SourceRange())
        : Statement(ASTNodeType::WHILE_STATEMENT, range),
          condition_(std::move(condition)), body_(std::move(body)) {}
    
    const ASTNodePtr& getCondition() const { return condition_; }
    const ASTNodePtr& getBody() const { return body_; }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 2; }
    ASTNode* getChild(size_t index) const override {
        if (index == 0) return condition_.get();
        if (index == 1) return body_.get();
        return nullptr;
    }
    
private:
    ASTNodePtr condition_;
    ASTNodePtr body_;
};

// For statement
class ForStatement : public Statement {
public:
    ForStatement(ASTNodePtr init, ASTNodePtr condition, ASTNodePtr update, ASTNodePtr body, const SourceRange& range = SourceRange())
        : Statement(ASTNodeType::FOR_STATEMENT, range),
          init_(std::move(init)), condition_(std::move(condition)), 
          update_(std::move(update)), body_(std::move(body)) {}
    
    const ASTNodePtr& getInit() const { return init_; }
    const ASTNodePtr& getCondition() const { return condition_; }
    const ASTNodePtr& getUpdate() const { return update_; }
    const ASTNodePtr& getBody() const { return body_; }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { 
        return (init_ ? 1 : 0) + (condition_ ? 1 : 0) + (update_ ? 1 : 0) + (body_ ? 1 : 0); 
    }
    ASTNode* getChild(size_t index) const override {
        size_t currentIndex = 0;
        if (init_ && currentIndex++ == index) return init_.get();
        if (condition_ && currentIndex++ == index) return condition_.get();
        if (update_ && currentIndex++ == index) return update_.get();
        if (body_ && currentIndex++ == index) return body_.get();
        return nullptr;
    }
    
private:
    ASTNodePtr init_;
    ASTNodePtr condition_;
    ASTNodePtr update_;
    ASTNodePtr body_;
};

// Break statement
class BreakStatement : public Statement {
public:
    BreakStatement(const SourceRange& range = SourceRange())
        : Statement(ASTNodeType::BREAK_STATEMENT, range) {}
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 0; }
    ASTNode* getChild(size_t) const override { return nullptr; }
};

// Continue statement
class ContinueStatement : public Statement {
public:
    ContinueStatement(const SourceRange& range = SourceRange())
        : Statement(ASTNodeType::CONTINUE_STATEMENT, range) {}
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return 0; }
    ASTNode* getChild(size_t) const override { return nullptr; }
};

// Expression statement
class ExpressionStatement : public Statement {
public:
    ExpressionStatement(const SourceRange& range = SourceRange())
        : Statement(ASTNodeType::EXPRESSION_STATEMENT, range) {}
    
    ExpressionStatement(ASTNodePtr expression, const SourceRange& range = SourceRange())
        : Statement(ASTNodeType::EXPRESSION_STATEMENT, range), expression_(std::move(expression)) {}
    
    const ASTNodePtr& getExpression() const { return expression_; }
    void setExpression(ASTNodePtr expression) { expression_ = std::move(expression); }
    
    // ASTNode interface
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
    ASTNodePtr clone() const override;
    size_t getChildCount() const override { return expression_ ? 1 : 0; }
    ASTNode* getChild(size_t index) const override {
        return (index == 0 && expression_) ? expression_.get() : nullptr;
    }
    
private:
    ASTNodePtr expression_;
};

// AST Management class
class AST {
public:
    AST();
    explicit AST(ASTNodePtr root);
    ~AST();
    
    // Copy constructor and assignment operator (deleted due to unique_ptr)
    AST(const AST& other) = delete;
    AST& operator=(const AST& other) = delete;
    
    // Move constructor and assignment operator
    AST(AST&& other) noexcept;
    AST& operator=(AST&& other) noexcept;
    
    // Node creation and management
    template<typename T, typename... Args>
    std::unique_ptr<T> createNode(Args&&... args) {
        auto node = std::make_unique<T>(std::forward<Args>(args)...);
        nodeCount_++;
        return node;
    }
    
    void deleteNode(ASTNode* node);
    
    // Root management
    ASTNode* getRoot() const { return root_.get(); }
    const ASTNodePtr& getRootPtr() const { return root_; }
    void setRoot(ASTNodePtr root);
    
    // Statistics
    size_t getNodeCount() const { return nodeCount_; }
    void clear();
    
    // Visitor pattern
    void accept(ASTVisitor& visitor);
    
    // Serialization
    std::string serialize() const;
    bool deserialize(const std::string& data);
    
    // Validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Statistics
    struct Statistics {
        size_t totalNodes = 0;
        size_t maxDepth = 0;
        size_t leafNodes = 0;
        std::map<ASTNodeType, size_t> nodeTypeCounts;
    };
    
    Statistics getStatistics() const;
    
private:
    ASTNodePtr root_;
    size_t nodeCount_;
    
    void calculateStatistics(const ASTNodePtr& node, Statistics& stats, size_t depth = 0) const;
    void calculateStatisticsForNode(const ASTNode* node, Statistics& stats, size_t depth) const;
};

// ASTVisitor interface
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // Declarations
    virtual void visit(TranslationUnit& node) = 0;
    virtual void visit(VariableDeclaration& node) = 0;
    virtual void visit(DeclarationList& node) = 0;
    virtual void visit(FunctionDeclaration& node) = 0;
    
    // Statement visitors
    virtual void visit(CompoundStatement& node) = 0;
    virtual void visit(ExpressionStatement& node) = 0;
    virtual void visit(ReturnStatement& node) = 0;
    virtual void visit(IfStatement& node) = 0;
    virtual void visit(WhileStatement& node) = 0;
    virtual void visit(ForStatement& node) = 0;
    virtual void visit(BreakStatement& node) = 0;
    virtual void visit(ContinueStatement& node) = 0;
    
    // Expression visitors
    virtual void visit(BinaryExpression& node) = 0;
    virtual void visit(UnaryExpression& node) = 0;
    virtual void visit(AssignmentExpression& node) = 0;
    virtual void visit(TernaryExpression& node) = 0;
    virtual void visit(CallExpression& node) = 0;
    virtual void visit(MemberExpression& node) = 0;
    virtual void visit(ArrayAccess& node) = 0;
    virtual void visit(CastExpression& node) = 0;
    
    // Literal visitors
    virtual void visit(IntegerLiteral& node) = 0;
    virtual void visit(FloatLiteral& node) = 0;
    virtual void visit(StringLiteral& node) = 0;
    virtual void visit(CharLiteral& node) = 0;
    
    // Identifier visitor
    virtual void visit(Identifier& node) = 0;
    
    // Generic visit method
    virtual void visit(ASTNode& node) {
        (void)node;
        // Default implementation - can be overridden
    }
    
    /**
     * @brief Determines whether to visit children of a node
     * @param node The AST node being visited
     * @return true if children should be visited, false otherwise
     */
    virtual bool shouldVisitChildren(const ASTNode& node) const {
        (void)node;
        return true;
    }
    
    /**
     * @brief Visits all children of a node
     * @param node The parent node whose children should be visited
     */
    virtual void visitChildren(ASTNode& node) {
        if (!shouldVisitChildren(node)) {
            return;
        }
        
        // Visit all children of the node
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            ASTNode* child = node.getChild(i);
            if (child) {
                child->accept(*this);
            }
        }
    }
};

} // namespace Parser

#endif // AST_HPP