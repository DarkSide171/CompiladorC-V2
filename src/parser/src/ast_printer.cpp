#include "../include/ast_printer.hpp"
#include <algorithm>
#include <iomanip>
#include <string>
#include <sstream>

namespace Parser {

// ASTPrinter Implementation

ASTPrinter::ASTPrinter(size_t indentSize, bool showTypes, bool showRanges)
    : indentLevel_(0), indentSize_(indentSize), showTypes_(showTypes), showRanges_(showRanges) {
}

std::string ASTPrinter::getOutput() const {
    return output_.str();
}

void ASTPrinter::clear() {
    output_.str("");
    output_.clear();
    indentLevel_ = 0;
}

std::string ASTPrinter::print(ASTNode& node, size_t indentSize, bool showTypes, bool showRanges) {
    ASTPrinter printer(indentSize, showTypes, showRanges);
    node.accept(printer);
    return printer.getOutput();
}

void ASTPrinter::addIndent() {
    for (size_t i = 0; i < indentLevel_ * indentSize_; ++i) {
        output_ << " ";
    }
}

void ASTPrinter::addTreeIndent(bool isLast) {
    for (size_t i = 0; i < indentLevel_; ++i) {
        if (i == indentLevel_ - 1) {
            output_ << (isLast ? "└── " : "├── ");
        } else {
            output_ << "│   ";
        }
    }
}

void ASTPrinter::addLine(const std::string& line) {
    addIndent();
    output_ << line << "\n";
}

void ASTPrinter::addTreeLine(const std::string& line, bool isLast) {
    addTreeIndent(isLast);
    output_ << line << "\n";
}

void ASTPrinter::addNodeType(const ASTNode& node) {
    if (showTypes_) {
        output_ << " [" << static_cast<int>(node.getType()) << "]";
    }
}

void ASTPrinter::addSourceRange(const ASTNode& node) {
    if (showRanges_) {
        // Note: Assuming SourceRange has line/column information
        // This would need to be adapted based on actual SourceRange implementation
        output_ << " @(" << node.getSourceRange().start.line 
                << ":" << node.getSourceRange().start.column
                << "-" << node.getSourceRange().end.line
                << ":" << node.getSourceRange().end.column << ")";
    }
}

std::string ASTPrinter::formatNodeHeader(const ASTNode& node, const std::string& name) {
    std::ostringstream header;
    header << name;
    addNodeType(node);
    addSourceRange(node);
    return header.str();
}

void ASTPrinter::visitChildrenWithIndent(ASTNode& node) {
    ++indentLevel_;
    visitChildren(node);
    --indentLevel_;
}

std::string ASTPrinter::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.length() + 10); // Reserve some extra space for escape sequences
    
    for (char c : str) {
        switch (c) {
            case '\n': result += "\\n"; break;
            case '\t': result += "\\t"; break;
            case '\r': result += "\\r"; break;
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\'': result += "\\'"; break;
            default:
                if (c >= 32 && c <= 126) {
                    result += c;
                } else {
                    result += "\\x" + std::to_string(static_cast<unsigned char>(c));
                }
                break;
        }
    }
    return result;
}

// Visitor implementations

void ASTPrinter::visit(TranslationUnit& node) {
    output_ << "TranslationUnit\n";
    ++indentLevel_;
    for (size_t i = 0; i < node.getDeclarations().size(); ++i) {
        bool isLast = (i == node.getDeclarations().size() - 1);
        addTreeIndent(isLast);
        node.getDeclarations()[i]->accept(*this);
    }
    --indentLevel_;
}

void ASTPrinter::visit(FunctionDeclaration& node) {
    std::ostringstream line;
    line << "FunctionDeclaration: ";
    
    // Add return type if available
    if (!node.getReturnType().empty()) {
        line << node.getReturnType() << " ";
    }
    
    line << node.getName();
    
    // Add parameters if any (similar to IfStatement showing condition)
    if (!node.getParameters().empty()) {
        line << " (";
        for (size_t i = 0; i < node.getParameters().size(); ++i) {
            if (i > 0) line << ", ";
            // Use CompactASTPrinter to get parameter representation
            CompactASTPrinter compactPrinter;
            node.getParameters()[i]->accept(compactPrinter);
            std::string paramContent = compactPrinter.getOutput();
            paramContent.erase(std::remove(paramContent.begin(), paramContent.end(), '\n'), paramContent.end());
            line << paramContent;
        }
        line << ")";
    }
    
    output_ << line.str() << "\n";
    
    // Only show the body (CompoundStatement) as child, not parameters
    if (node.getBody()) {
        ++indentLevel_;
        addTreeIndent(true);
        node.getBody()->accept(*this);
        --indentLevel_;
    }
}

void ASTPrinter::visit(VariableDeclaration& node) {
    std::ostringstream line;
    line << "VariableDeclaration: " << node.getType() << " " << node.getName();
    
    if (node.getInitializer()) {
        line << " = ";
        CompactASTPrinter compactPrinter;
        node.getInitializer()->accept(compactPrinter);
        line << compactPrinter.getOutput();
    }
    
    output_ << line.str() << "\n";
    
    // Ainda visita os filhos com indentação para mostrar a estrutura detalhada
    if ((showTypes_ || node.getInitializer()) && node.getInitializer()) {
        ++indentLevel_;
        addTreeIndent(true);
        node.getInitializer()->accept(*this);
        --indentLevel_;
    }
}

void ASTPrinter::visit(DeclarationList& node) {
    output_ << formatNodeHeader(node, "DeclarationList") << "\n";
    ++indentLevel_;
    const auto& declarations = node.getDeclarations();
    for (size_t i = 0; i < declarations.size(); ++i) {
        bool isLast = (i == declarations.size() - 1);
        addTreeIndent(isLast);
        declarations[i]->accept(*this);
    }
    --indentLevel_;
}

void ASTPrinter::visit(CompoundStatement& node) {
    output_ << formatNodeHeader(node, "CompoundStatement") << "\n";
    ++indentLevel_;
    for (size_t i = 0; i < node.getStatements().size(); ++i) {
        bool isLast = (i == node.getStatements().size() - 1);
        addTreeIndent(isLast);
        node.getStatements()[i]->accept(*this);
    }
    --indentLevel_;
}

void ASTPrinter::visit(ExpressionStatement& node) {
    std::ostringstream line;
    line << "ExpressionStatement";
    
    // Mostra a expressão de forma compacta na mesma linha
    if (node.getExpression()) {
        CompactASTPrinter compactPrinter;
        node.getExpression()->accept(compactPrinter);
        std::string exprContent = compactPrinter.getOutput();
        
        // Remove quebras de linha para manter compacto
        exprContent.erase(std::remove(exprContent.begin(), exprContent.end(), '\n'), exprContent.end());
        
        if (!exprContent.empty()) {
            line << ": " << exprContent;
        }
    }
    
    output_ << formatNodeHeader(node, line.str()) << "\n";
    
    // Ainda visita os filhos com indentação para mostrar a estrutura detalhada
    if ((showTypes_ || node.getExpression()) && node.getExpression()) {
        ++indentLevel_;
        addTreeIndent(true);
        node.getExpression()->accept(*this);
        --indentLevel_;
    }
}

void ASTPrinter::visit(BinaryExpression& node) {
    std::ostringstream line;
    line << "BinaryExpression: " << node.operatorToString(node.getOperator());
    output_ << formatNodeHeader(node, line.str()) << "\n";
    
    ++indentLevel_;
    if (node.getLeft()) {
        addTreeIndent(false);
        output_ << "Left:\n";
        ++indentLevel_;
        addTreeIndent(true);
        node.getLeft()->accept(*this);
        --indentLevel_;
    }
    
    if (node.getRight()) {
        addTreeIndent(true);
        output_ << "Right:\n";
        ++indentLevel_;
        addTreeIndent(true);
        node.getRight()->accept(*this);
        --indentLevel_;
    }
    --indentLevel_;
}

void ASTPrinter::visit(UnaryExpression& node) {
    std::ostringstream line;
    line << "UnaryExpression: " << node.operatorToString(node.getOperator());
    output_ << formatNodeHeader(node, line.str()) << "\n";
    
    ++indentLevel_;
    addTreeLine("Operand:", true);
    ++indentLevel_;
    if (node.getOperand()) {
        node.getOperand()->accept(*this);
    }
    --indentLevel_;
    --indentLevel_;
}

void ASTPrinter::visit(AssignmentExpression& node) {
    std::ostringstream line;
    line << "AssignmentExpression: " << node.operatorToString(node.getOperator());
    output_ << formatNodeHeader(node, line.str()) << "\n";
    
    ++indentLevel_;
    if (node.getLeft()) {
        addTreeIndent(false);
        output_ << "Left:\n";
        ++indentLevel_;
        addTreeIndent(true);
        node.getLeft()->accept(*this);
        --indentLevel_;
    }
    
    if (node.getRight()) {
        addTreeIndent(true);
        output_ << "Right:\n";
        ++indentLevel_;
        addTreeIndent(true);
        node.getRight()->accept(*this);
        --indentLevel_;
    }
    --indentLevel_;
}

void ASTPrinter::visit(TernaryExpression& node) {
    output_ << formatNodeHeader(node, "TernaryExpression") << "\n";
    
    ++indentLevel_;
    if (node.getCondition()) {
        addTreeIndent(false);
        output_ << "Condition:\n";
        ++indentLevel_;
        addTreeIndent(true);
        node.getCondition()->accept(*this);
        --indentLevel_;
    }
    
    if (node.getTrueExpression()) {
        addTreeIndent(false);
        output_ << "TrueExpr:\n";
        ++indentLevel_;
        addTreeIndent(true);
        node.getTrueExpression()->accept(*this);
        --indentLevel_;
    }
    
    if (node.getFalseExpression()) {
        addTreeIndent(true);
        output_ << "FalseExpr:\n";
        ++indentLevel_;
        addTreeIndent(true);
        node.getFalseExpression()->accept(*this);
        --indentLevel_;
    }
    --indentLevel_;
}

void ASTPrinter::visit(CallExpression& node) {
    std::ostringstream line;
    line << "CallExpression";
    output_ << formatNodeHeader(node, line.str()) << "\n";
    
    ++indentLevel_;
    bool hasArgs = !node.getArguments().empty();
    if (node.getFunction()) {
        addTreeIndent(!hasArgs);
        output_ << "Callee:\n";
        ++indentLevel_;
        addTreeIndent(true);
        node.getFunction()->accept(*this);
        --indentLevel_;
    }
    
    if (hasArgs) {
        addTreeIndent(true);
        output_ << "Arguments:\n";
        ++indentLevel_;
        for (const auto& arg : node.getArguments()) {
            if (arg) {
                addTreeIndent(true);
                arg->accept(*this);
            }
        }
        --indentLevel_;
    }
    --indentLevel_;
}

void ASTPrinter::visit(MemberExpression& node) {
    std::ostringstream line;
    line << "MemberExpression: " << (node.getAccessType() == MemberExpression::AccessType::ARROW ? "->" : ".") << node.getMemberName();
    output_ << formatNodeHeader(node, line.str()) << "\n";
    
    ++indentLevel_;
    if (node.getObject()) {
        addTreeIndent(true);
        output_ << "Object:\n";
        ++indentLevel_;
        addTreeIndent(true);
        node.getObject()->accept(*this);
        --indentLevel_;
    }
    --indentLevel_;
}

void ASTPrinter::visit(ArrayAccess& node) {
    output_ << formatNodeHeader(node, "ArrayAccess") << "\n";
    
    ++indentLevel_;
    if (node.getArray()) {
        addTreeIndent(false);
        output_ << "Array:\n";
        ++indentLevel_;
        addTreeIndent(true);
        node.getArray()->accept(*this);
        --indentLevel_;
    }
    
    if (node.getIndex()) {
        addTreeIndent(true);
        output_ << "Index:\n";
        ++indentLevel_;
        addTreeIndent(true);
        node.getIndex()->accept(*this);
        --indentLevel_;
    }
    --indentLevel_;
}

void ASTPrinter::visit(CastExpression& node) {
    std::ostringstream line;
    line << "CastExpression: (" << node.getTargetType() << ")";
    output_ << formatNodeHeader(node, line.str()) << "\n";
    
    ++indentLevel_;
    if (node.getExpression()) {
        addTreeIndent(true);
        output_ << "Expression:\n";
        ++indentLevel_;
        addTreeIndent(true);
        node.getExpression()->accept(*this);
        --indentLevel_;
    }
    --indentLevel_;
}

void ASTPrinter::visit(IntegerLiteral& node) {
    std::ostringstream line;
    line << "IntegerLiteral: " << node.getIntegerValue();
    output_ << formatNodeHeader(node, line.str()) << "\n";
}

void ASTPrinter::visit(FloatLiteral& node) {
    std::ostringstream line;
    line << "FloatLiteral: " << std::fixed << std::setprecision(6) << node.getFloatValue();
    output_ << formatNodeHeader(node, line.str()) << "\n";
}

void ASTPrinter::visit(StringLiteral& node) {
    std::ostringstream line;
    line << "StringLiteral: \"" << escapeString(node.getStringValue()) << "\"";
    output_ << formatNodeHeader(node, line.str()) << "\n";
}

void ASTPrinter::visit(CharLiteral& node) {
    std::ostringstream line;
    line << "CharLiteral: '" << escapeString(std::string(1, node.getCharValue())) << "'";
    output_ << formatNodeHeader(node, line.str()) << "\n";
}

void ASTPrinter::visit(Identifier& node) {
    std::ostringstream line;
    line << "Identifier: " << node.getName();
    output_ << formatNodeHeader(node, line.str()) << "\n";
}

// Statement visitors
void ASTPrinter::visit(ReturnStatement& node) {
    std::ostringstream line;
    line << "ReturnStatement";
    if (node.getExpression()) {
        line << " (";
        CompactASTPrinter compactPrinter;
        node.getExpression()->accept(compactPrinter);
        line << compactPrinter.getOutput() << ")";
    }
    output_ << formatNodeHeader(node, line.str()) << "\n";
    
    // Show the expression as a detailed child node if it exists
    if (node.getExpression()) {
        ++indentLevel_;
        addTreeIndent(true);
        node.getExpression()->accept(*this);
        --indentLevel_;
    }
}

void ASTPrinter::visit(IfStatement& node) {
    std::ostringstream line;
    line << "IfStatement";
    
    // Mostra a condição de forma compacta na linha principal
    if (node.getCondition()) {
        CompactASTPrinter compactPrinter;
        node.getCondition()->accept(compactPrinter);
        std::string condContent = compactPrinter.getOutput();
        condContent.erase(std::remove(condContent.begin(), condContent.end(), '\n'), condContent.end());
        line << " " << condContent;
    }
    
    output_ << line.str() << "\n";
    
    ++indentLevel_;
    
    // Mostra a condição como estrutura detalhada (similar ao FunctionDeclaration)
    if (node.getCondition()) {
        addTreeLine("Condition:", false);
        ++indentLevel_;
        node.getCondition()->accept(*this);
        --indentLevel_;
    }
    
    bool hasElse = node.getElseStatement() != nullptr;
    addTreeLine("Then:", !hasElse);
    ++indentLevel_;
    if (node.getThenStatement()) {
        node.getThenStatement()->accept(*this);
    }
    --indentLevel_;
    
    if (node.getElseStatement()) {
        addTreeLine("Else:", true);
        ++indentLevel_;
        node.getElseStatement()->accept(*this);
        --indentLevel_;
    }
    --indentLevel_;
}

void ASTPrinter::visit(WhileStatement& node) {
    output_ << formatNodeHeader(node, "WhileStatement") << "\n";
    
    ++indentLevel_;
    addTreeLine("Condition:", false);
    ++indentLevel_;
    if (node.getCondition()) {
        node.getCondition()->accept(*this);
    }
    --indentLevel_;
    
    addTreeLine("Body:", true);
    ++indentLevel_;
    if (node.getBody()) {
        node.getBody()->accept(*this);
    }
    --indentLevel_;
    --indentLevel_;
}

void ASTPrinter::visit(ForStatement& node) {
    output_ << "ForStatement\n";
    
    ++indentLevel_;
    
    // Init
    if (node.getInit()) {
        CompactASTPrinter compactPrinter;
        node.getInit()->accept(compactPrinter);
        std::string initContent = compactPrinter.getOutput();
        initContent.erase(std::remove(initContent.begin(), initContent.end(), '\n'), initContent.end());
        addTreeLine("Init: " + initContent, false);
    }
    
    // Condition
    if (node.getCondition()) {
        CompactASTPrinter compactPrinter;
        node.getCondition()->accept(compactPrinter);
        std::string condContent = compactPrinter.getOutput();
        condContent.erase(std::remove(condContent.begin(), condContent.end(), '\n'), condContent.end());
        addTreeLine("Condition: " + condContent, false);
    }
    
    // Update
    if (node.getUpdate()) {
        CompactASTPrinter compactPrinter;
        node.getUpdate()->accept(compactPrinter);
        std::string updateContent = compactPrinter.getOutput();
        updateContent.erase(std::remove(updateContent.begin(), updateContent.end(), '\n'), updateContent.end());
        addTreeLine("Update: " + updateContent, false);
    }
    
    // Body
    if (node.getBody()) {
        addTreeLine("Body:", true);
        ++indentLevel_;
        node.getBody()->accept(*this);
        --indentLevel_;
    }
    
    --indentLevel_;
}

void ASTPrinter::visit(BreakStatement& node) {
    output_ << formatNodeHeader(node, "BreakStatement") << "\n";
}

void ASTPrinter::visit(ContinueStatement& node) {
    output_ << formatNodeHeader(node, "ContinueStatement") << "\n";
}

void ASTPrinter::visit(ASTNode& node) {
    std::ostringstream line;
    line << "ASTNode [type=" << static_cast<int>(node.getType()) << "]";
    output_ << formatNodeHeader(node, line.str()) << "\n";
    visitChildrenWithIndent(node);
}

// CompactASTPrinter Implementation

CompactASTPrinter::CompactASTPrinter() : needsSpace_(false) {
}

std::string CompactASTPrinter::getOutput() const {
    return output_.str();
}

void CompactASTPrinter::clear() {
    output_.str("");
    output_.clear();
    needsSpace_ = false;
}

std::string CompactASTPrinter::print(ASTNode& node) {
    CompactASTPrinter printer;
    node.accept(printer);
    return printer.getOutput();
}

void CompactASTPrinter::addToken(const std::string& token) {
    if (needsSpace_ && !token.empty()) {
        output_ << " ";
    }
    output_ << token;
    needsSpace_ = !token.empty();
}

// Compact visitor implementations
void CompactASTPrinter::visit(TranslationUnit& node) {
    visitChildren(node);
}

void CompactASTPrinter::visit(VariableDeclaration& node) {
    addToken(node.getType());
    addToken(node.getName());
    if (node.getInitializer()) {
        addToken("=");
        node.getInitializer()->accept(*this);
    }
}

void CompactASTPrinter::visit(DeclarationList& node) {
    const auto& declarations = node.getDeclarations();
    for (size_t i = 0; i < declarations.size(); ++i) {
        declarations[i]->accept(*this);
        if (i < declarations.size() - 1) {
            addToken(",");
        }
    }
}

void CompactASTPrinter::visit(FunctionDeclaration& node) {
    addToken(node.getName());
    addToken("(");
    
    const auto& params = node.getParameters();
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) addToken(",");
        if (params[i]) {
            params[i]->accept(*this);
        }
    }
    
    addToken(")");
    if (node.getBody()) {
        node.getBody()->accept(*this);
    }
}

void CompactASTPrinter::visit(CompoundStatement& node) {
    addToken("{");
    visitChildren(node);
    addToken("}");
}

void CompactASTPrinter::visit(BinaryExpression& node) {
    if (node.getLeft()) {
        node.getLeft()->accept(*this);
    }
    addToken(node.operatorToString(node.getOperator()));
    if (node.getRight()) {
        node.getRight()->accept(*this);
    }
}

void CompactASTPrinter::visit(UnaryExpression& node) {
    // Handle postfix operators differently
    if (node.getOperator() == UnaryExpression::Operator::POST_INCREMENT ||
        node.getOperator() == UnaryExpression::Operator::POST_DECREMENT) {
        // For postfix: operand first, then operator
        if (node.getOperand()) {
            node.getOperand()->accept(*this);
        }
        addToken(node.operatorToString(node.getOperator()));
    } else {
        // For prefix: operator first, then operand
        addToken(node.operatorToString(node.getOperator()));
        if (node.getOperand()) {
            node.getOperand()->accept(*this);
        }
    }
}

void CompactASTPrinter::visit(AssignmentExpression& node) {
    if (node.getLeft()) {
        node.getLeft()->accept(*this);
    }
    addToken(node.operatorToString(node.getOperator()));
    if (node.getRight()) {
        node.getRight()->accept(*this);
    }
}

void CompactASTPrinter::visit(TernaryExpression& node) {
    if (node.getCondition()) {
        node.getCondition()->accept(*this);
    }
    addToken("?");
    if (node.getTrueExpression()) {
        node.getTrueExpression()->accept(*this);
    }
    addToken(":");
    if (node.getFalseExpression()) {
        node.getFalseExpression()->accept(*this);
    }
}

void CompactASTPrinter::visit(CallExpression& node) {
    if (node.getFunction()) {
        node.getFunction()->accept(*this);
    }
    addToken("(");
    
    const auto& args = node.getArguments();
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) addToken(",");
        if (args[i]) {
            args[i]->accept(*this);
        }
    }
    
    addToken(")");
}

void CompactASTPrinter::visit(MemberExpression& node) {
    if (node.getObject()) {
        node.getObject()->accept(*this);
    }
    addToken(node.getAccessType() == MemberExpression::AccessType::ARROW ? "->" : ".");
    addToken(node.getMemberName());
}

void CompactASTPrinter::visit(ArrayAccess& node) {
    if (node.getArray()) {
        node.getArray()->accept(*this);
    }
    addToken("[");
    if (node.getIndex()) {
        node.getIndex()->accept(*this);
    }
    addToken("]");
}

void CompactASTPrinter::visit(CastExpression& node) {
    addToken("(");
    addToken(node.getTargetType());
    addToken(")");
    if (node.getExpression()) {
        node.getExpression()->accept(*this);
    }
}

void CompactASTPrinter::visit(IntegerLiteral& node) {
    addToken(std::to_string(node.getIntegerValue()));
}

void CompactASTPrinter::visit(FloatLiteral& node) {
    addToken(std::to_string(node.getFloatValue()));
}

void CompactASTPrinter::visit(StringLiteral& node) {
    addToken("\"" + node.getStringValue() + "\"");
}

void CompactASTPrinter::visit(CharLiteral& node) {
    addToken("'" + std::string(1, node.getCharValue()) + "'");
}

void CompactASTPrinter::visit(Identifier& node) {
    addToken(node.getName());
}

// Statement visitors for CompactASTPrinter
void CompactASTPrinter::visit(ExpressionStatement& node) {
    visitChildren(node);
    addToken(";");
}

void CompactASTPrinter::visit(ReturnStatement& node) {
    addToken("return");
    if (node.getExpression()) {
        node.getExpression()->accept(*this);
    }
    addToken(";");
}

void CompactASTPrinter::visit(IfStatement& node) {
    addToken("if");
    if (node.getCondition()) {
        node.getCondition()->accept(*this);
    }
    if (node.getThenStatement()) {
        node.getThenStatement()->accept(*this);
    }
    if (node.getElseStatement()) {
        addToken("else");
        node.getElseStatement()->accept(*this);
    }
}

void CompactASTPrinter::visit(WhileStatement& node) {
    addToken("while");
    addToken("(");
    if (node.getCondition()) {
        node.getCondition()->accept(*this);
    }
    addToken(")");
    if (node.getBody()) {
        node.getBody()->accept(*this);
    }
}

void CompactASTPrinter::visit(ForStatement& node) {
    addToken("for");
    addToken("(");
    if (node.getInit()) {
        node.getInit()->accept(*this);
    }
    addToken(";");
    if (node.getCondition()) {
        node.getCondition()->accept(*this);
    }
    addToken(";");
    if (node.getUpdate()) {
        node.getUpdate()->accept(*this);
    }
    addToken(")");
    if (node.getBody()) {
        node.getBody()->accept(*this);
    }
}

void CompactASTPrinter::visit(BreakStatement& node) {
    (void)node;
    addToken("break;");
}

void CompactASTPrinter::visit(ContinueStatement& node) {
    (void)node;
    addToken("continue;");
}

void CompactASTPrinter::visit(ASTNode& node) {
    addToken("[" + std::to_string(static_cast<int>(node.getType())) + "]");
    visitChildren(node);
}

} // namespace Parser