#ifndef AST_TRANSFORMER_H
#define AST_TRANSFORMER_H

#include "../parser/ast_node.h"
#include "../generator/ts_generator.h"
#include "type_mapper.h"
#include <string>
#include <memory>
#include <set>

namespace gd2ts {

class ASTTransformer {
private:
    TSGenerator generator;
    TypeMapper type_mapper;
    std::set<std::string> required_imports;
    std::set<std::string> godot_types_used;

    // Transform methods for declarations
    void transform_class_declaration(const ASTNodePtr& node);
    void transform_function_declaration(const ASTNodePtr& node);
    void transform_variable_declaration(const ASTNodePtr& node);
    void transform_const_declaration(const ASTNodePtr& node);
    void transform_signal_declaration(const ASTNodePtr& node);
    void transform_enum_declaration(const ASTNodePtr& node);

    // Transform methods for statements
    void transform_statement(const ASTNodePtr& node);
    void transform_if_statement(const ASTNodePtr& node);
    void transform_for_statement(const ASTNodePtr& node);
    void transform_while_statement(const ASTNodePtr& node);
    void transform_match_statement(const ASTNodePtr& node);
    void transform_return_statement(const ASTNodePtr& node);
    void transform_expression_statement(const ASTNodePtr& node);
    void transform_assignment(const ASTNodePtr& node);

    // Transform methods for expressions
    std::string transform_expression(const ASTNodePtr& node);
    std::string transform_binary_expression(const ASTNodePtr& node);
    std::string transform_unary_expression(const ASTNodePtr& node);
    std::string transform_call_expression(const ASTNodePtr& node);
    std::string transform_attribute_expression(const ASTNodePtr& node);
    std::string transform_subscript_expression(const ASTNodePtr& node);
    std::string transform_identifier(const ASTNodePtr& node);
    std::string transform_literal(const ASTNodePtr& node);
    std::string transform_array_literal(const ASTNodePtr& node);
    std::string transform_dictionary_literal(const ASTNodePtr& node);
    std::string transform_get_node(const ASTNodePtr& node);

    // Helper methods
    std::string get_function_parameters(const std::vector<ASTNodePtr>& params);
    std::string get_return_type(const std::string& gd_type);
    std::string map_operator(const std::string& op);
    void track_type_usage(const std::string& type);
    void generate_imports();

    // Special keyword handling
    std::string replace_keywords(const std::string& text);

public:
    ASTTransformer();
    ~ASTTransformer() = default;

    // Main transform function
    std::string transform(const ASTNodePtr& ast);

    // Configuration
    void set_indent_style(const std::string& style);
    void set_use_semicolons(bool use);
};

} // namespace gd2ts

#endif // AST_TRANSFORMER_H
