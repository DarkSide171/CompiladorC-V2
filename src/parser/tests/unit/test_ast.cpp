// Unit tests for AST module
#include "../../include/ast.hpp"
#include "../../include/parser_types.hpp"
#include <iostream>
#include <memory>
#include <cassert>
#include <string>
#include <sstream>

using namespace Parser;

// Test helper functions
void testPassed(const std::string& testName) {
    std::cout << "✅ " << testName << " - PASSED" << std::endl;
}

void testFailed(const std::string& testName, const std::string& reason) {
    std::cout << "❌ " << testName << " - FAILED: " << reason << std::endl;
}

// Test AST construction and basic operations
void testASTConstruction() {
    std::cout << "\n=== Testing AST Construction ===" << std::endl;
    
    // Test default constructor
    AST ast1;
    if (ast1.getRoot() == nullptr) {
        testPassed("AST default constructor");
    } else {
        testFailed("AST default constructor", "Root should be null");
    }
    
    // Test constructor with root
    auto root = std::make_unique<TranslationUnit>();
    ASTNode* rootPtr = root.get();
    AST ast2(std::move(root));
    
    if (ast2.getRoot() == rootPtr) {
        testPassed("AST constructor with root");
    } else {
        testFailed("AST constructor with root", "Root not set correctly");
    }
    
    // Test move constructor
    AST ast3(std::move(ast2));
    if (ast3.getRoot() == rootPtr && ast2.getRoot() == nullptr) {
        testPassed("AST move constructor");
    } else {
        testFailed("AST move constructor", "Move not performed correctly");
    }
}

// Test AST node creation and management
void testASTNodeCreation() {
    std::cout << "\n=== Testing AST Node Creation ===" << std::endl;
    
    AST ast;
    
    // Test createNode template method
    auto identifier = ast.createNode<Identifier>("testVar");
    if (identifier && identifier->getType() == ASTNodeType::IDENTIFIER) {
        testPassed("AST createNode for Identifier");
    } else {
        testFailed("AST createNode for Identifier", "Node not created correctly");
    }
    
    auto binaryExpr = ast.createNode<BinaryExpression>(
        BinaryExpression::Operator::ADD,
        std::make_unique<Identifier>("a"),
        std::make_unique<Identifier>("b")
    );
    
    if (binaryExpr && binaryExpr->getType() == ASTNodeType::BINARY_EXPRESSION) {
        testPassed("AST createNode for BinaryExpression");
    } else {
        testFailed("AST createNode for BinaryExpression", "Node not created correctly");
    }
    
    auto func = ast.createNode<FunctionDeclaration>("testFunc");
    if (func && func->getType() == ASTNodeType::FUNCTION_DECLARATION) {
        testPassed("AST createNode for FunctionDeclaration");
    } else {
        testFailed("AST createNode for FunctionDeclaration", "Node not created correctly");
    }
}

// Test building a complete AST tree
void testCompleteASTTree() {
    std::cout << "\n=== Testing Complete AST Tree ===" << std::endl;
    
    AST ast;
    
    // Create a translation unit
    auto tu = ast.createNode<TranslationUnit>();
    
    // Create a function: int main() { return 0; }
    auto mainFunc = ast.createNode<FunctionDeclaration>("main");
    
    // Create function body
    auto body = ast.createNode<CompoundStatement>();
    
    // Create return statement: return 0;
    auto returnStmt = ast.createNode<Identifier>("return_stmt"); // Simplified for test
    body->addStatement(std::move(returnStmt));
    
    mainFunc->setBody(std::move(body));
    tu->addDeclaration(std::move(mainFunc));
    
    // Set as root
    ast.setRoot(std::move(tu));
    
    // Test tree structure
    ASTNode* root = ast.getRoot();
    if (root && root->getType() == ASTNodeType::TRANSLATION_UNIT) {
        testPassed("Complete AST tree - root type");
    } else {
        testFailed("Complete AST tree - root type", "Root is not TranslationUnit");
    }
    
    if (root->getChildCount() == 1) {
        testPassed("Complete AST tree - root child count");
    } else {
        testFailed("Complete AST tree - root child count", "Expected 1 child");
    }
    
    ASTNode* funcNode = root->getChild(0);
    if (funcNode && funcNode->getType() == ASTNodeType::FUNCTION_DECLARATION) {
        testPassed("Complete AST tree - function node");
    } else {
        testFailed("Complete AST tree - function node", "Function node not found");
    }
}

// Test AST validation
void testASTValidation() {
    std::cout << "\n=== Testing AST Validation ===" << std::endl;
    
    AST ast;
    
    // Test validation of empty AST
    if (!ast.validate()) {
        testPassed("Empty AST validation (should fail)");
    } else {
        testFailed("Empty AST validation (should fail)", "Empty AST should not validate");
    }
    
    // Create valid AST
    auto tu = ast.createNode<TranslationUnit>();
    auto func = ast.createNode<FunctionDeclaration>("validFunc");
    tu->addDeclaration(std::move(func));
    ast.setRoot(std::move(tu));
    
    if (ast.validate()) {
        testPassed("Valid AST validation");
    } else {
        testFailed("Valid AST validation", "Valid AST should pass validation");
    }
    
    // Test validation errors
    auto errors = ast.getValidationErrors();
    if (errors.empty()) {
        testPassed("Valid AST has no validation errors");
    } else {
        testFailed("Valid AST has no validation errors", "Should have no errors");
    }
}

// Test AST statistics
void testASTStatistics() {
    std::cout << "\n=== Testing AST Statistics ===" << std::endl;
    
    AST ast;
    
    // Create AST with known structure
    auto tu = ast.createNode<TranslationUnit>();
    
    // Add function with parameters and body
    auto func = ast.createNode<FunctionDeclaration>("testFunc");
    func->addParameter(ast.createNode<Identifier>("param1"));
    func->addParameter(ast.createNode<Identifier>("param2"));
    
    auto body = ast.createNode<CompoundStatement>();
    body->addStatement(ast.createNode<Identifier>("stmt1"));
    body->addStatement(ast.createNode<Identifier>("stmt2"));
    func->setBody(std::move(body));
    
    tu->addDeclaration(std::move(func));
    ast.setRoot(std::move(tu));
    
    // Get statistics
    auto stats = ast.getStatistics();
    
    // Test total nodes (TU + Func + 2 params + body + 2 stmts = 7)
    if (stats.totalNodes >= 6) { // At least 6 nodes
        testPassed("AST statistics - total nodes");
    } else {
        testFailed("AST statistics - total nodes", "Expected at least 6 nodes, got " + std::to_string(stats.totalNodes));
    }
    
    // Test max depth (should be at least 3: TU -> Func -> Body -> Stmt)
    if (stats.maxDepth >= 3) {
        testPassed("AST statistics - max depth");
    } else {
        testFailed("AST statistics - max depth", "Expected depth >= 3, got " + std::to_string(stats.maxDepth));
    }
    
    // Test node type counts
    if (stats.nodeTypeCounts.find(ASTNodeType::TRANSLATION_UNIT) != stats.nodeTypeCounts.end() &&
        stats.nodeTypeCounts.at(ASTNodeType::TRANSLATION_UNIT) == 1) {
        testPassed("AST statistics - node type counts");
    } else {
        testFailed("AST statistics - node type counts", "TranslationUnit count incorrect");
    }
}

// Test AST serialization
void testASTSerialization() {
    std::cout << "\n=== Testing AST Serialization ===" << std::endl;
    
    AST ast;
    
    // Create simple AST
    auto tu = ast.createNode<TranslationUnit>();
    auto func = ast.createNode<FunctionDeclaration>("serializeTest");
    tu->addDeclaration(std::move(func));
    ast.setRoot(std::move(tu));
    
    // Test serialization
    std::string serialized = ast.serialize();
    if (!serialized.empty()) {
        testPassed("AST serialization produces output");
    } else {
        testFailed("AST serialization produces output", "Serialization returned empty string");
    }
    
    // Test that serialization contains expected content
    if (serialized.find("TranslationUnit") != std::string::npos &&
        serialized.find("serializeTest") != std::string::npos) {
        testPassed("AST serialization contains expected content");
    } else {
        testFailed("AST serialization contains expected content", "Missing expected content in: " + serialized);
    }
}

// Test AST visitor pattern integration
void testASTVisitorIntegration() {
    std::cout << "\n=== Testing AST Visitor Integration ===" << std::endl;
    
    class CountingVisitor : public ASTVisitor {
    public:
        int tuCount = 0, funcCount = 0, idCount = 0, compoundCount = 0, binaryCount = 0;
        
        void visit(TranslationUnit& node) override {
            tuCount++;
            // Visit children
            for (size_t i = 0; i < node.getChildCount(); ++i) {
                if (auto child = node.getChild(i)) {
                    child->accept(*this);
                }
            }
        }
        
        void visit(FunctionDeclaration& node) override {
            funcCount++;
            // Visit children
            for (size_t i = 0; i < node.getChildCount(); ++i) {
                if (auto child = node.getChild(i)) {
                    child->accept(*this);
                }
            }
        }
        
        void visit(Identifier& node) override { idCount++; }
        void visit(CompoundStatement& node) override {
            compoundCount++;
            // Visit children
            for (size_t i = 0; i < node.getChildCount(); ++i) {
                if (auto child = node.getChild(i)) {
                    child->accept(*this);
                }
            }
        }
        void visit(BinaryExpression& node) override { binaryCount++; }
        
        // Additional required visit methods
        void visit(UnaryExpression& node) override { /* count if needed */ }
        void visit(AssignmentExpression& node) override { /* count if needed */ }
        void visit(TernaryExpression& node) override { /* count if needed */ }
        void visit(CallExpression& node) override { /* count if needed */ }
        void visit(MemberExpression& node) override { /* count if needed */ }
        void visit(ArrayAccess& node) override { /* count if needed */ }
        void visit(CastExpression& node) override { /* count if needed */ }
        void visit(IntegerLiteral& node) override { /* count if needed */ }
        void visit(FloatLiteral& node) override { /* count if needed */ }
        void visit(StringLiteral& node) override { /* count if needed */ }
        void visit(CharLiteral& node) override { /* count if needed */ }
        void visit(VariableDeclaration& node) override { /* count if needed */ }
        
        // Statement visitors for new classes
        void visit(ReturnStatement& node) override { /* count if needed */ }
        void visit(IfStatement& node) override { /* count if needed */ }
        void visit(WhileStatement& node) override { /* count if needed */ }
        void visit(ForStatement& node) override { /* count if needed */ }
        void visit(BreakStatement& node) override { /* count if needed */ }
        void visit(ContinueStatement& node) override { /* count if needed */ }
        void visit(ExpressionStatement& node) override { /* count if needed */ }
    };
    
    AST ast;
    
    // Create AST with multiple node types
    auto tu = ast.createNode<TranslationUnit>();
    auto func = ast.createNode<FunctionDeclaration>("visitorTest");
    func->addParameter(ast.createNode<Identifier>("param"));
    
    auto body = ast.createNode<CompoundStatement>();
    body->addStatement(ast.createNode<Identifier>("stmt"));
    func->setBody(std::move(body));
    
    tu->addDeclaration(std::move(func));
    ast.setRoot(std::move(tu));
    
    // Apply visitor
    CountingVisitor visitor;
    ast.accept(visitor);
    
    // Test visitor counts
    if (visitor.tuCount == 1) {
        testPassed("Visitor integration - TranslationUnit count");
    } else {
        testFailed("Visitor integration - TranslationUnit count", "Expected 1, got " + std::to_string(visitor.tuCount));
    }
    
    if (visitor.funcCount == 1) {
        testPassed("Visitor integration - FunctionDeclaration count");
    } else {
        testFailed("Visitor integration - FunctionDeclaration count", "Expected 1, got " + std::to_string(visitor.funcCount));
    }
    
    if (visitor.idCount == 2) { // param + stmt
        testPassed("Visitor integration - Identifier count");
    } else {
        testFailed("Visitor integration - Identifier count", "Expected 2, got " + std::to_string(visitor.idCount));
    }
}

// Test AST clear and reset
void testASTClearAndReset() {
    std::cout << "\n=== Testing AST Clear and Reset ===" << std::endl;
    
    AST ast;
    
    // Create AST
    auto tu = ast.createNode<TranslationUnit>();
    auto func = ast.createNode<FunctionDeclaration>("clearTest");
    tu->addDeclaration(std::move(func));
    ast.setRoot(std::move(tu));
    
    // Verify AST has content
    if (ast.getRoot() != nullptr) {
        testPassed("AST has content before clear");
    } else {
        testFailed("AST has content before clear", "AST should have content");
    }
    
    // Clear AST
    ast.clear();
    
    // Verify AST is cleared
    if (ast.getRoot() == nullptr) {
        testPassed("AST clear functionality");
    } else {
        testFailed("AST clear functionality", "AST should be empty after clear");
    }
    
    // Test statistics after clear
    auto stats = ast.getStatistics();
    if (stats.totalNodes == 0) {
        testPassed("AST statistics after clear");
    } else {
        testFailed("AST statistics after clear", "Should have 0 nodes after clear");
    }
}

int main() {
    std::cout << "\n🧪 Running AST Unit Tests\n" << std::endl;
    
    try {
        testASTConstruction();
        testASTNodeCreation();
        testCompleteASTTree();
        testASTValidation();
        testASTStatistics();
        testASTSerialization();
        testASTVisitorIntegration();
        testASTClearAndReset();
        
        std::cout << "\n✅ All AST tests completed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\n❌ Test execution failed: " << e.what() << std::endl;
        return 1;
    }
}