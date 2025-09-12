// Unit tests for ASTNode classes
#include "../../include/ast.hpp"
#include "../../include/parser_types.hpp"
#include <iostream>
#include <memory>
#include <cassert>
#include <string>

using namespace Parser;

// Test helper functions
void testPassed(const std::string& testName) {
    std::cout << "✅ " << testName << " - PASSED" << std::endl;
}

void testFailed(const std::string& testName, const std::string& reason) {
    std::cout << "❌ " << testName << " - FAILED: " << reason << std::endl;
}

// Test ASTNode basic functionality
void testASTNodeBasics() {
    std::cout << "\n=== Testing ASTNode Basics ===" << std::endl;
    
    // Test Identifier creation
    SourceRange range(Position(1, 5), Position(1, 10));
    auto identifier = std::make_unique<Identifier>("testVar", range);
    
    // Test type
    if (identifier->getType() == ASTNodeType::IDENTIFIER) {
        testPassed("Identifier type check");
    } else {
        testFailed("Identifier type check", "Wrong node type");
    }
    
    // Test source range
    if (identifier->getSourceRange().start.line == 1 && 
        identifier->getSourceRange().end.column == 10) {
        testPassed("Source range assignment");
    } else {
        testFailed("Source range assignment", "Range not set correctly");
    }
    
    // Test value
    if (identifier->getValue() == "testVar") {
        testPassed("Identifier value");
    } else {
        testFailed("Identifier value", "Value not set correctly");
    }
    
    // Test toString
    std::string expected = "Identifier(testVar)";
    if (identifier->toString() == expected) {
        testPassed("Identifier toString");
    } else {
        testFailed("Identifier toString", "Expected: " + expected + ", Got: " + identifier->toString());
    }
}

// Test parent-child relationships
void testParentChildRelationships() {
    std::cout << "\n=== Testing Parent-Child Relationships ===" << std::endl;
    
    auto compound = std::make_unique<CompoundStatement>();
    auto identifier = std::make_unique<Identifier>("child");
    
    // Test initial parent (should be null)
    if (identifier->getParent() == nullptr) {
        testPassed("Initial parent is null");
    } else {
        testFailed("Initial parent is null", "Parent should be null initially");
    }
    
    // Test adding child
    ASTNode* identifierPtr = identifier.get();
    compound->addStatement(std::move(identifier));
    
    // Test child count
    if (compound->getChildCount() == 1) {
        testPassed("Child count after adding");
    } else {
        testFailed("Child count after adding", "Expected 1 child");
    }
    
    // Test getting child
    ASTNode* retrievedChild = compound->getChild(0);
    if (retrievedChild == identifierPtr) {
        testPassed("Child retrieval");
    } else {
        testFailed("Child retrieval", "Retrieved child doesn't match");
    }
    
    // Test out of bounds access
    if (compound->getChild(1) == nullptr) {
        testPassed("Out of bounds child access");
    } else {
        testFailed("Out of bounds child access", "Should return nullptr");
    }
}

// Test BinaryExpression functionality
void testBinaryExpression() {
    std::cout << "\n=== Testing BinaryExpression ===" << std::endl;
    
    auto left = std::make_unique<Identifier>("a");
    auto right = std::make_unique<Identifier>("b");
    
    auto binaryExpr = std::make_unique<BinaryExpression>(
        BinaryExpression::Operator::ADD, 
        std::move(left), 
        std::move(right)
    );
    
    // Test type
    if (binaryExpr->getType() == ASTNodeType::BINARY_EXPRESSION) {
        testPassed("BinaryExpression type");
    } else {
        testFailed("BinaryExpression type", "Wrong node type");
    }
    
    // Test child count
    if (binaryExpr->getChildCount() == 2) {
        testPassed("BinaryExpression child count");
    } else {
        testFailed("BinaryExpression child count", "Expected 2 children");
    }
    
    // Test children
    ASTNode* leftChild = binaryExpr->getChild(0);
    ASTNode* rightChild = binaryExpr->getChild(1);
    
    if (leftChild && leftChild->getType() == ASTNodeType::IDENTIFIER &&
        rightChild && rightChild->getType() == ASTNodeType::IDENTIFIER) {
        testPassed("BinaryExpression children types");
    } else {
        testFailed("BinaryExpression children types", "Children have wrong types");
    }
    
    // Test toString
    std::string result = binaryExpr->toString();
    if (result.find("ADD") != std::string::npos && 
        result.find("Identifier(a)") != std::string::npos &&
        result.find("Identifier(b)") != std::string::npos) {
        testPassed("BinaryExpression toString");
    } else {
        testFailed("BinaryExpression toString", "Missing expected content: " + result);
    }
}

// Test FunctionDeclaration functionality
void testFunctionDeclaration() {
    std::cout << "\n=== Testing FunctionDeclaration ===" << std::endl;
    
    auto func = std::make_unique<FunctionDeclaration>("testFunc");
    
    // Test type
    if (func->getType() == ASTNodeType::FUNCTION_DECLARATION) {
        testPassed("FunctionDeclaration type");
    } else {
        testFailed("FunctionDeclaration type", "Wrong node type");
    }
    
    // Test name
    if (func->getValue() == "testFunc") {
        testPassed("FunctionDeclaration name");
    } else {
        testFailed("FunctionDeclaration name", "Name not set correctly");
    }
    
    // Test adding parameters
    auto param1 = std::make_unique<Identifier>("param1");
    auto param2 = std::make_unique<Identifier>("param2");
    
    func->addParameter(std::move(param1));
    func->addParameter(std::move(param2));
    
    // Test parameter count
    if (func->getParameters().size() == 2) {
        testPassed("FunctionDeclaration parameter count");
    } else {
        testFailed("FunctionDeclaration parameter count", "Expected 2 parameters");
    }
    
    // Test child count (parameters + body)
    if (func->getChildCount() == 2) { // Only parameters, no body set
        testPassed("FunctionDeclaration child count");
    } else {
        testFailed("FunctionDeclaration child count", "Expected 2 children (parameters)");
    }
}

// Test TranslationUnit functionality
void testTranslationUnit() {
    std::cout << "\n=== Testing TranslationUnit ===" << std::endl;
    
    auto tu = std::make_unique<TranslationUnit>();
    
    // Test type
    if (tu->getType() == ASTNodeType::TRANSLATION_UNIT) {
        testPassed("TranslationUnit type");
    } else {
        testFailed("TranslationUnit type", "Wrong node type");
    }
    
    // Test initial state
    if (tu->getChildCount() == 0) {
        testPassed("TranslationUnit initial child count");
    } else {
        testFailed("TranslationUnit initial child count", "Should start with 0 children");
    }
    
    // Add declarations
    auto func1 = std::make_unique<FunctionDeclaration>("func1");
    auto func2 = std::make_unique<FunctionDeclaration>("func2");
    
    tu->addDeclaration(std::move(func1));
    tu->addDeclaration(std::move(func2));
    
    // Test child count after adding
    if (tu->getChildCount() == 2) {
        testPassed("TranslationUnit child count after adding");
    } else {
        testFailed("TranslationUnit child count after adding", "Expected 2 children");
    }
    
    // Test declarations access
    if (tu->getDeclarations().size() == 2) {
        testPassed("TranslationUnit declarations access");
    } else {
        testFailed("TranslationUnit declarations access", "Expected 2 declarations");
    }
}

// Test cloning functionality
void testCloning() {
    std::cout << "\n=== Testing Cloning ===" << std::endl;
    
    // Test Identifier cloning
    auto original = std::make_unique<Identifier>("original", SourceRange(Position(1, 1), Position(1, 8)));
    auto cloned = original->clone();
    
    if (cloned->getType() == original->getType() &&
        cloned->getValue() == original->getValue() &&
        cloned->getSourceRange().start.line == original->getSourceRange().start.line) {
        testPassed("Identifier cloning");
    } else {
        testFailed("Identifier cloning", "Clone doesn't match original");
    }
    
    // Test BinaryExpression cloning
    auto left = std::make_unique<Identifier>("left");
    auto right = std::make_unique<Identifier>("right");
    auto binaryOriginal = std::make_unique<BinaryExpression>(
        BinaryExpression::Operator::MUL,
        std::move(left),
        std::move(right)
    );
    
    auto binaryCloned = binaryOriginal->clone();
    
    if (binaryCloned->getType() == binaryOriginal->getType() &&
        binaryCloned->getChildCount() == binaryOriginal->getChildCount()) {
        testPassed("BinaryExpression cloning");
    } else {
        testFailed("BinaryExpression cloning", "Clone doesn't match original");
    }
}

// Test visitor pattern
void testVisitorPattern() {
    std::cout << "\n=== Testing Visitor Pattern ===" << std::endl;
    
    class TestVisitor : public ASTVisitor {
    public:
        int visitCount = 0;
        
        void visit(TranslationUnit& node) override { visitCount++; }
        void visit(Identifier& node) override { visitCount++; }
        void visit(BinaryExpression& node) override { visitCount++; }
        void visit(UnaryExpression& node) override { visitCount++; }
        void visit(AssignmentExpression& node) override { visitCount++; }
        void visit(TernaryExpression& node) override { visitCount++; }
        void visit(CallExpression& node) override { visitCount++; }
        void visit(MemberExpression& node) override { visitCount++; }
        void visit(ArrayAccess& node) override { visitCount++; }
        void visit(CastExpression& node) override { visitCount++; }
        void visit(IntegerLiteral& node) override { visitCount++; }
        void visit(FloatLiteral& node) override { visitCount++; }
        void visit(StringLiteral& node) override { visitCount++; }
        void visit(CharLiteral& node) override { visitCount++; }
        void visit(VariableDeclaration& node) override { visitCount++; }
        void visit(FunctionDeclaration& node) override { visitCount++; }
        void visit(CompoundStatement& node) override { visitCount++; }
        
        // Statement visitors for new classes
        void visit(ReturnStatement& node) override { visitCount++; }
        void visit(IfStatement& node) override { visitCount++; }
        void visit(WhileStatement& node) override { visitCount++; }
        void visit(ForStatement& node) override { visitCount++; }
        void visit(BreakStatement& node) override { visitCount++; }
        void visit(ContinueStatement& node) override { visitCount++; }
        void visit(ExpressionStatement& node) override { visitCount++; }
    };
    
    TestVisitor visitor;
    
    // Test visiting different node types
    auto identifier = std::make_unique<Identifier>("test");
    identifier->accept(visitor);
    
    auto tu = std::make_unique<TranslationUnit>();
    tu->accept(visitor);
    
    if (visitor.visitCount == 2) {
        testPassed("Visitor pattern basic functionality");
    } else {
        testFailed("Visitor pattern basic functionality", "Expected 2 visits, got " + std::to_string(visitor.visitCount));
    }
}

int main() {
    std::cout << "\n🧪 Running ASTNode Unit Tests\n" << std::endl;
    
    try {
        testASTNodeBasics();
        testParentChildRelationships();
        testBinaryExpression();
        testFunctionDeclaration();
        testTranslationUnit();
        testCloning();
        testVisitorPattern();
        
        std::cout << "\n✅ All ASTNode tests completed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\n❌ Test execution failed: " << e.what() << std::endl;
        return 1;
    }
}