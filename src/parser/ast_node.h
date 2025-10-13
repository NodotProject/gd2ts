#ifndef AST_NODE_H
#define AST_NODE_H

#include <string>
#include <vector>
#include <memory>
#include "tree_sitter_api.h"

namespace gd2ts {

// Source location information
struct SourceLocation {
    uint32_t start_line;
    uint32_t start_column;
    uint32_t end_line;
    uint32_t end_column;
    uint32_t start_byte;
    uint32_t end_byte;

    SourceLocation() : start_line(0), start_column(0), end_line(0),
                       end_column(0), start_byte(0), end_byte(0) {}

    SourceLocation(TSNode node) {
        TSPoint start = ts_node_start_point(node);
        TSPoint end = ts_node_end_point(node);
        start_line = start.row + 1;  // Make 1-indexed
        start_column = start.column + 1;
        end_line = end.row + 1;
        end_column = end.column + 1;
        start_byte = ts_node_start_byte(node);
        end_byte = ts_node_end_byte(node);
    }
};

// AST Node types
enum class NodeType {
    // Root
    Source,

    // Declarations
    ClassDeclaration,
    ClassNameStatement,
    ExtendsStatement,
    FunctionDeclaration,
    SignalDeclaration,
    VariableDeclaration,
    ConstDeclaration,
    EnumDeclaration,

    // Statements
    ExpressionStatement,
    ReturnStatement,
    IfStatement,
    ElifClause,
    ElseClause,
    ForStatement,
    WhileStatement,
    MatchStatement,
    BreakStatement,
    ContinueStatement,
    PassStatement,
    Assignment,
    AugmentedAssignment,

    // Expressions
    Identifier,
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,
    BooleanLiteral,
    NullLiteral,
    ArrayLiteral,
    DictionaryLiteral,
    BinaryExpression,
    UnaryExpression,
    CallExpression,
    AttributeExpression,
    SubscriptExpression,
    GetNodeExpression,
    AwaitExpression,
    ConditionalExpression,
    LambdaExpression,

    // Types
    TypeAnnotation,
    InferredType,

    // Parameters
    Parameter,
    TypedParameter,
    DefaultParameter,
    TypedDefaultParameter,

    // Misc
    Annotation,
    Comment,
    Body,
    Unknown
};

// Forward declaration
class ASTNode;
using ASTNodePtr = std::shared_ptr<ASTNode>;

// Base AST Node class
class ASTNode {
public:
    NodeType type;
    std::string text;
    std::string node_type_str;  // Original tree-sitter node type
    SourceLocation location;
    std::vector<ASTNodePtr> children;
    std::string comment;  // Associated comment if any

    ASTNode(NodeType t) : type(t) {}
    virtual ~ASTNode() = default;

    // Helper methods
    bool has_children() const { return !children.empty(); }
    size_t child_count() const { return children.size(); }
    ASTNodePtr get_child(size_t index) const {
        return index < children.size() ? children[index] : nullptr;
    }
    void add_child(ASTNodePtr child) {
        if (child) children.push_back(child);
    }

    // Find first child of specific type
    ASTNodePtr find_child(NodeType t) const {
        for (const auto& child : children) {
            if (child && child->type == t) return child;
        }
        return nullptr;
    }

    // Find all children of specific type
    std::vector<ASTNodePtr> find_children(NodeType t) const {
        std::vector<ASTNodePtr> result;
        for (const auto& child : children) {
            if (child && child->type == t) result.push_back(child);
        }
        return result;
    }

    virtual std::string to_string() const {
        return node_type_str + " (" + text + ")";
    }
};

// Specific AST Node types

class ClassDecl : public ASTNode {
public:
    std::string class_name;
    std::string extends_type;
    bool is_tool = false;
    std::vector<std::string> annotations;

    // Children: variables, functions, signals, enums, inner classes

    ClassDecl() : ASTNode(NodeType::ClassDeclaration) {}
};

class FunctionDecl : public ASTNode {
public:
    std::string function_name;
    std::vector<ASTNodePtr> parameters;
    std::string return_type;
    ASTNodePtr body;
    bool is_static = false;
    bool is_virtual = false;
    std::vector<std::string> annotations;

    FunctionDecl() : ASTNode(NodeType::FunctionDeclaration) {}
};

class VariableDecl : public ASTNode {
public:
    std::string var_name;
    std::string var_type;
    bool is_inferred_type = false;
    ASTNodePtr initializer;
    bool is_export = false;
    bool is_onready = false;
    bool is_static = false;
    std::string getter;
    std::string setter;
    std::vector<std::string> annotations;

    VariableDecl() : ASTNode(NodeType::VariableDeclaration) {}
};

class ConstDecl : public ASTNode {
public:
    std::string const_name;
    std::string const_type;
    bool is_inferred_type = false;
    ASTNodePtr value;

    ConstDecl() : ASTNode(NodeType::ConstDeclaration) {}
};

class SignalDecl : public ASTNode {
public:
    std::string signal_name;
    std::vector<ASTNodePtr> parameters;

    SignalDecl() : ASTNode(NodeType::SignalDeclaration) {}
};

class EnumDecl : public ASTNode {
public:
    std::string enum_name;  // Empty for anonymous enums
    std::vector<std::pair<std::string, std::string>> members;  // name, value

    EnumDecl() : ASTNode(NodeType::EnumDeclaration) {}
};

class Parameter : public ASTNode {
public:
    std::string param_name;
    std::string param_type;
    ASTNodePtr default_value;
    bool is_variadic = false;

    Parameter() : ASTNode(NodeType::Parameter) {}
};

class BinaryExpr : public ASTNode {
public:
    std::string op;
    ASTNodePtr left;
    ASTNodePtr right;

    BinaryExpr() : ASTNode(NodeType::BinaryExpression) {}
};

class UnaryExpr : public ASTNode {
public:
    std::string op;
    ASTNodePtr operand;

    UnaryExpr() : ASTNode(NodeType::UnaryExpression) {}
};

class CallExpr : public ASTNode {
public:
    ASTNodePtr callee;
    std::vector<ASTNodePtr> arguments;

    CallExpr() : ASTNode(NodeType::CallExpression) {}
};

class IdentifierExpr : public ASTNode {
public:
    std::string name;

    IdentifierExpr() : ASTNode(NodeType::Identifier) {}
};

class LiteralExpr : public ASTNode {
public:
    std::string value;

    LiteralExpr(NodeType t) : ASTNode(t) {}
};

class IfStmt : public ASTNode {
public:
    ASTNodePtr condition;
    ASTNodePtr then_body;
    std::vector<ASTNodePtr> elif_clauses;
    ASTNodePtr else_body;

    IfStmt() : ASTNode(NodeType::IfStatement) {}
};

class ForStmt : public ASTNode {
public:
    std::string iterator_name;
    std::string iterator_type;
    ASTNodePtr iterable;
    ASTNodePtr body;

    ForStmt() : ASTNode(NodeType::ForStatement) {}
};

class WhileStmt : public ASTNode {
public:
    ASTNodePtr condition;
    ASTNodePtr body;

    WhileStmt() : ASTNode(NodeType::WhileStatement) {}
};

class ReturnStmt : public ASTNode {
public:
    ASTNodePtr value;

    ReturnStmt() : ASTNode(NodeType::ReturnStatement) {}
};

class MatchStmt : public ASTNode {
public:
    ASTNodePtr value;
    std::vector<ASTNodePtr> pattern_sections;

    MatchStmt() : ASTNode(NodeType::MatchStatement) {}
};

class AnnotationNode : public ASTNode {
public:
    std::string annotation_name;
    std::vector<ASTNodePtr> arguments;

    AnnotationNode() : ASTNode(NodeType::Annotation) {}
};

class CommentNode : public ASTNode {
public:
    std::string comment_text;
    bool is_doc_comment = false;

    CommentNode() : ASTNode(NodeType::Comment) {}
};

} // namespace gd2ts

#endif // AST_NODE_H
