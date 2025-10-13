#ifndef AST_BUILDER_H
#define AST_BUILDER_H

#include "ast_node.h"
#include "tree_sitter_api.h"
#include <string>
#include <map>

namespace gd2ts {

class ASTBuilder {
private:
    std::string source_code;
    std::map<std::string, NodeType> node_type_map;

    // Helper functions
    std::string get_node_text(TSNode node) const;
    std::string get_node_type(TSNode node) const;
    bool node_is_named(TSNode node) const;
    bool node_is_null(TSNode node) const;

    // Node building functions
    ASTNodePtr build_node(TSNode node);
    ASTNodePtr build_class_declaration(TSNode node);
    ASTNodePtr build_class_name_statement(TSNode node);
    ASTNodePtr build_function_declaration(TSNode node);
    ASTNodePtr build_variable_declaration(TSNode node, bool is_export = false, bool is_onready = false);
    ASTNodePtr build_const_declaration(TSNode node);
    ASTNodePtr build_signal_declaration(TSNode node);
    ASTNodePtr build_enum_declaration(TSNode node);
    ASTNodePtr build_extends_statement(TSNode node);

    // Statement builders
    ASTNodePtr build_if_statement(TSNode node);
    ASTNodePtr build_for_statement(TSNode node);
    ASTNodePtr build_while_statement(TSNode node);
    ASTNodePtr build_match_statement(TSNode node);
    ASTNodePtr build_return_statement(TSNode node);
    ASTNodePtr build_expression_statement(TSNode node);
    ASTNodePtr build_assignment(TSNode node);

    // Expression builders
    ASTNodePtr build_expression(TSNode node);
    ASTNodePtr build_binary_expression(TSNode node);
    ASTNodePtr build_unary_expression(TSNode node);
    ASTNodePtr build_call_expression(TSNode node);
    ASTNodePtr build_attribute_expression(TSNode node);
    ASTNodePtr build_subscript_expression(TSNode node);
    ASTNodePtr build_identifier(TSNode node);
    ASTNodePtr build_literal(TSNode node);
    ASTNodePtr build_array_literal(TSNode node);
    ASTNodePtr build_dictionary_literal(TSNode node);
    ASTNodePtr build_get_node(TSNode node);

    // Type and parameter builders
    ASTNodePtr build_type(TSNode node);
    ASTNodePtr build_parameter(TSNode node);
    std::vector<ASTNodePtr> build_parameters(TSNode node);

    // Annotation and comment builders
    std::vector<std::string> extract_annotations(TSNode node);
    std::string extract_comment(TSNode node);

    // Helper: Get child by field name
    TSNode get_child_by_field(TSNode node, const char* field_name) const;

    // Helper: Get child by type
    TSNode get_child_by_type(TSNode node, const char* type_name) const;

    void initialize_node_type_map();

public:
    ASTBuilder();
    ~ASTBuilder() = default;

    // Main build function
    ASTNodePtr build_ast(TSNode root, const std::string& source);

    // Build from source string
    ASTNodePtr build_from_source(const std::string& source);
};

} // namespace gd2ts

#endif // AST_BUILDER_H
