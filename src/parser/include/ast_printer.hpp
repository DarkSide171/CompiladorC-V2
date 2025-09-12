#ifndef AST_PRINTER_HPP
#define AST_PRINTER_HPP

#include "ast.hpp"
#include <sstream>
#include <string>
#include <iomanip>

namespace Parser {

/**
 * @brief Visitor implementation for printing AST in a readable format
 * 
 * This class traverses the AST and generates a formatted text representation
 * that can be used for debugging, visualization, or documentation purposes.
 */
class ASTPrinter : public ASTVisitor {
public:
    /**
     * @brief Constructor
     * @param indentSize Number of spaces per indentation level (default: 2)
     * @param showTypes Whether to show node types in output (default: true)
     * @param showRanges Whether to show source ranges in output (default: false)
     */
    explicit ASTPrinter(size_t indentSize = 2, bool showTypes = true, bool showRanges = false);
    
    /**
     * @brief Get the formatted string representation of the AST
     * @return The formatted AST as a string
     */
    std::string getOutput() const;
    
    /**
     * @brief Clear the current output buffer
     */
    void clear();
    
    /**
     * @brief Print an AST to a string
     * @param node The root node to print
     * @param indentSize Number of spaces per indentation level
     * @param showTypes Whether to show node types
     * @param showRanges Whether to show source ranges
     * @return The formatted AST as a string
     */
    static std::string print(ASTNode& node, size_t indentSize = 2, bool showTypes = true, bool showRanges = false);
    
    // Declaration visitors
    void visit(TranslationUnit& node) override;
    void visit(VariableDeclaration& node) override;
    void visit(DeclarationList& node) override;
    void visit(FunctionDeclaration& node) override;
    
    // Statement visitors
    void visit(CompoundStatement& node) override;
    void visit(ExpressionStatement& node) override;
    
    // Expression visitors
    void visit(BinaryExpression& node) override;
    void visit(UnaryExpression& node) override;
    void visit(AssignmentExpression& node) override;
    void visit(TernaryExpression& node) override;
    void visit(CallExpression& node) override;
    void visit(MemberExpression& node) override;
    void visit(ArrayAccess& node) override;
    void visit(CastExpression& node) override;
    
    // Literal visitors
    void visit(IntegerLiteral& node) override;
    void visit(FloatLiteral& node) override;
    void visit(StringLiteral& node) override;
    void visit(CharLiteral& node) override;
    
    // Identifier visitor
    void visit(Identifier& node) override;
    
    // Statement visitors for new classes
    void visit(ReturnStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(BreakStatement& node) override;
    void visit(ContinueStatement& node) override;
    
    // Generic visit method
    void visit(ASTNode& node) override;
    
private:
    std::ostringstream output_;
    size_t indentLevel_;
    size_t indentSize_;
    bool showTypes_;
    bool showRanges_;
    
    /**
     * @brief Add indentation to the current line
     */
    void addIndent();
    
    /**
     * @brief Add tree-style indentation with branch symbols
     * @param isLast Whether this is the last child node
     */
    void addTreeIndent(bool isLast);
    
    /**
     * @brief Add a line with proper indentation
     * @param line The line content to add
     */
    void addLine(const std::string& line);
    
    /**
     * @brief Add a line with tree-style indentation
     * @param line The line content to add
     * @param isLast Whether this is the last child node
     */
    void addTreeLine(const std::string& line, bool isLast);
    
    /**
     * @brief Add node type information if enabled
     * @param node The node to get type information from
     */
    void addNodeType(const ASTNode& node);
    
    /**
     * @brief Add source range information if enabled
     * @param node The node to get range information from
     */
    void addSourceRange(const ASTNode& node);
    
    /**
     * @brief Format a node header with type and range information
     * @param node The node to format
     * @param name The display name for the node
     * @return The formatted header string
     */
    std::string formatNodeHeader(const ASTNode& node, const std::string& name);
    
    /**
     * @brief Visit children with increased indentation
     * @param node The parent node
     */
    void visitChildrenWithIndent(ASTNode& node);
    
    /**
     * @brief Escape special characters in strings for display
     * @param str The string to escape
     * @return The escaped string
     */
    std::string escapeString(const std::string& str);
};

/**
 * @brief Utility class for compact AST printing
 * 
 * This class provides a more compact representation of the AST,
 * suitable for single-line or minimal output scenarios.
 */
class CompactASTPrinter : public ASTVisitor {
public:
    CompactASTPrinter();
    
    /**
     * @brief Get the compact string representation
     * @return The compact AST representation
     */
    std::string getOutput() const;
    
    /**
     * @brief Clear the output buffer
     */
    void clear();
    
    /**
     * @brief Print an AST in compact format
     * @param node The root node to print
     * @return The compact AST representation
     */
    static std::string print(ASTNode& node);
    
    // Visitor methods (compact implementations)
    void visit(TranslationUnit& node) override;
    void visit(VariableDeclaration& node) override;
    void visit(DeclarationList& node) override;
    void visit(FunctionDeclaration& node) override;
    void visit(CompoundStatement& node) override;
    void visit(BinaryExpression& node) override;
    void visit(UnaryExpression& node) override;
    void visit(AssignmentExpression& node) override;
    void visit(TernaryExpression& node) override;
    void visit(CallExpression& node) override;
    void visit(MemberExpression& node) override;
    void visit(ArrayAccess& node) override;
    void visit(CastExpression& node) override;
    void visit(IntegerLiteral& node) override;
    void visit(FloatLiteral& node) override;
    void visit(StringLiteral& node) override;
    void visit(CharLiteral& node) override;
    void visit(Identifier& node) override;
    
    // Statement visitors
    void visit(ExpressionStatement& node) override;
    void visit(ReturnStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(BreakStatement& node) override;
    void visit(ContinueStatement& node) override;
    
    void visit(ASTNode& node) override;
    
private:
    std::ostringstream output_;
    bool needsSpace_;
    
    void addToken(const std::string& token);
};

} // namespace Parser

#endif // AST_PRINTER_HPP