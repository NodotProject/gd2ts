#include "ast_builder.h"
#include "gdscript_parser.h"
#include <cstring>

namespace gd2ts {

ASTBuilder::ASTBuilder() {
    initialize_node_type_map();
}

void ASTBuilder::initialize_node_type_map() {
    // Map tree-sitter node types to our NodeType enum
    node_type_map["source"] = NodeType::Source;
    node_type_map["class_definition"] = NodeType::ClassDeclaration;
    node_type_map["class_name_statement"] = NodeType::ClassNameStatement;
    node_type_map["function_definition"] = NodeType::FunctionDeclaration;
    node_type_map["variable_statement"] = NodeType::VariableDeclaration;
    node_type_map["export_variable_statement"] = NodeType::VariableDeclaration;
    node_type_map["onready_variable_statement"] = NodeType::VariableDeclaration;
    node_type_map["const_statement"] = NodeType::ConstDeclaration;
    node_type_map["signal_statement"] = NodeType::SignalDeclaration;
    node_type_map["enum_definition"] = NodeType::EnumDeclaration;
    node_type_map["extends_statement"] = NodeType::ExtendsStatement;
    node_type_map["if_statement"] = NodeType::IfStatement;
    node_type_map["for_statement"] = NodeType::ForStatement;
    node_type_map["while_statement"] = NodeType::WhileStatement;
    node_type_map["match_statement"] = NodeType::MatchStatement;
    node_type_map["return_statement"] = NodeType::ReturnStatement;
    node_type_map["expression_statement"] = NodeType::ExpressionStatement;
    node_type_map["assignment"] = NodeType::Assignment;
    node_type_map["augmented_assignment"] = NodeType::AugmentedAssignment;
    node_type_map["binary_operator"] = NodeType::BinaryExpression;
    node_type_map["unary_operator"] = NodeType::UnaryExpression;
    node_type_map["call"] = NodeType::CallExpression;
    node_type_map["attribute"] = NodeType::AttributeExpression;
    node_type_map["subscript"] = NodeType::SubscriptExpression;
    node_type_map["get_node"] = NodeType::GetNodeExpression;
    node_type_map["await_expression"] = NodeType::AwaitExpression;
    node_type_map["identifier"] = NodeType::Identifier;
    node_type_map["integer"] = NodeType::IntegerLiteral;
    node_type_map["float"] = NodeType::FloatLiteral;
    node_type_map["string"] = NodeType::StringLiteral;
    node_type_map["true"] = NodeType::BooleanLiteral;
    node_type_map["false"] = NodeType::BooleanLiteral;
    node_type_map["null"] = NodeType::NullLiteral;
    node_type_map["array"] = NodeType::ArrayLiteral;
    node_type_map["dictionary"] = NodeType::DictionaryLiteral;
    node_type_map["comment"] = NodeType::Comment;
}

std::string ASTBuilder::get_node_text(TSNode node) const {
    if (node_is_null(node)) return "";
    uint32_t start = ts_node_start_byte(node);
    uint32_t end = ts_node_end_byte(node);
    if (start >= source_code.length() || end > source_code.length()) return "";
    return source_code.substr(start, end - start);
}

std::string ASTBuilder::get_node_type(TSNode node) const {
    if (node_is_null(node)) return "";
    const char* type = ts_node_type(node);
    return type ? std::string(type) : "";
}

bool ASTBuilder::node_is_named(TSNode node) const {
    return ts_node_is_named(node);
}

bool ASTBuilder::node_is_null(TSNode node) const {
    return ts_node_is_null(node);
}

TSNode ASTBuilder::get_child_by_field(TSNode node, const char* field_name) const {
    // Use tree-sitter's native field API
    TSNode field_node = ts_node_child_by_field_name(node, field_name, strlen(field_name));

    // If the field exists, return it
    if (!node_is_null(field_node)) {
        return field_node;
    }

    // Fallback: try some common patterns
    TSNode null_node;
    memset(&null_node, 0, sizeof(TSNode));
    return null_node;
}

TSNode ASTBuilder::get_child_by_type(TSNode node, const char* type_name) const {
    uint32_t count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; i++) {
        TSNode child = ts_node_child(node, i);
        if (get_node_type(child) == type_name) {
            return child;
        }
    }
    TSNode null_node;
    memset(&null_node, 0, sizeof(TSNode));
    return null_node;
}

ASTNodePtr ASTBuilder::build_ast(TSNode root, const std::string& source) {
    source_code = source;
    return build_node(root);
}

ASTNodePtr ASTBuilder::build_from_source(const std::string& source) {
    source_code = source;
    GDScriptParser parser;
    if (!parser.parse(source)) {
        return nullptr;
    }
    TSNode root = parser.get_root_node();
    return build_node(root);
}

ASTNodePtr ASTBuilder::build_node(TSNode node) {
    if (node_is_null(node)) return nullptr;

    std::string type = get_node_type(node);

    // Route to appropriate builder based on node type
    if (type == "source" || type == "body") {
        auto ast_node = std::make_shared<ASTNode>(NodeType::Source);
        ast_node->node_type_str = type;
        ast_node->location = SourceLocation(node);

        uint32_t count = ts_node_child_count(node);
        for (uint32_t i = 0; i < count; i++) {
            TSNode child = ts_node_child(node, i);
            if (node_is_named(child)) {
                auto child_node = build_node(child);
                if (child_node) {
                    ast_node->add_child(child_node);
                }
            }
        }
        return ast_node;
    }
    else if (type == "class_definition") {
        return build_class_declaration(node);
    }
    else if (type == "class_name_statement") {
        return build_class_name_statement(node);
    }
    else if (type == "function_definition") {
        return build_function_declaration(node);
    }
    else if (type == "variable_statement") {
        return build_variable_declaration(node);
    }
    else if (type == "export_variable_statement") {
        return build_variable_declaration(node, true, false);
    }
    else if (type == "onready_variable_statement") {
        return build_variable_declaration(node, false, true);
    }
    else if (type == "const_statement") {
        return build_const_declaration(node);
    }
    else if (type == "signal_statement") {
        return build_signal_declaration(node);
    }
    else if (type == "enum_definition") {
        return build_enum_declaration(node);
    }
    else if (type == "extends_statement") {
        return build_extends_statement(node);
    }
    else if (type == "if_statement") {
        return build_if_statement(node);
    }
    else if (type == "for_statement") {
        return build_for_statement(node);
    }
    else if (type == "while_statement") {
        return build_while_statement(node);
    }
    else if (type == "match_statement") {
        return build_match_statement(node);
    }
    else if (type == "return_statement") {
        return build_return_statement(node);
    }
    else if (type == "expression_statement") {
        return build_expression_statement(node);
    }
    else if (type == "assignment" || type == "augmented_assignment") {
        return build_assignment(node);
    }
    else if (type == "comment") {
        auto comment_node = std::make_shared<CommentNode>();
        comment_node->node_type_str = type;
        comment_node->location = SourceLocation(node);
        comment_node->comment_text = get_node_text(node);
        return comment_node;
    }
    else {
        // Try to build as expression
        return build_expression(node);
    }
}

ASTNodePtr ASTBuilder::build_class_declaration(TSNode node) {
    auto class_decl = std::make_shared<ClassDecl>();
    class_decl->node_type_str = "class_definition";
    class_decl->location = SourceLocation(node);

    // Get class name
    TSNode name_node = get_child_by_field(node, "name");
    if (!node_is_null(name_node)) {
        class_decl->class_name = get_node_text(name_node);
    }

    // Get extends
    TSNode extends_node = get_child_by_field(node, "extends");
    if (!node_is_null(extends_node)) {
        TSNode type_node = ts_node_child(extends_node, 1);  // Skip 'extends' keyword
        if (!node_is_null(type_node) && node_is_named(type_node)) {
            class_decl->extends_type = get_node_text(type_node);
        }
    }

    // Get annotations
    class_decl->annotations = extract_annotations(node);

    // Get body
    TSNode body_node = get_child_by_field(node, "body");
    if (!node_is_null(body_node)) {
        uint32_t count = ts_node_child_count(body_node);
        for (uint32_t i = 0; i < count; i++) {
            TSNode child = ts_node_child(body_node, i);
            if (node_is_named(child)) {
                auto child_node = build_node(child);
                if (child_node) {
                    class_decl->add_child(child_node);
                }
            }
        }
    }

    return class_decl;
}

ASTNodePtr ASTBuilder::build_class_name_statement(TSNode node) {
    auto class_decl = std::make_shared<ClassDecl>();
    class_decl->node_type_str = "class_name_statement";
    class_decl->location = SourceLocation(node);
    class_decl->type = NodeType::ClassNameStatement;

    // Get class name
    TSNode name_node = get_child_by_field(node, "name");
    if (!node_is_null(name_node)) {
        class_decl->class_name = get_node_text(name_node);
    }

    // Get extends if present
    TSNode extends_node = get_child_by_field(node, "extends");
    if (!node_is_null(extends_node)) {
        TSNode type_node = ts_node_child(extends_node, 1);
        if (!node_is_null(type_node) && node_is_named(type_node)) {
            class_decl->extends_type = get_node_text(type_node);
        }
    }

    return class_decl;
}

ASTNodePtr ASTBuilder::build_function_declaration(TSNode node) {
    auto func_decl = std::make_shared<FunctionDecl>();
    func_decl->node_type_str = "function_definition";
    func_decl->location = SourceLocation(node);

    // Get function name
    TSNode name_node = get_child_by_field(node, "name");
    if (!node_is_null(name_node)) {
        func_decl->function_name = get_node_text(name_node);
    }

    // Get parameters
    TSNode params_node = get_child_by_field(node, "parameters");
    if (!node_is_null(params_node)) {
        func_decl->parameters = build_parameters(params_node);
    }

    // Get return type
    TSNode return_type_node = get_child_by_field(node, "return_type");
    if (!node_is_null(return_type_node)) {
        TSNode type_node = ts_node_child(return_type_node, 0);
        if (!node_is_null(type_node) && node_is_named(type_node)) {
            func_decl->return_type = get_node_text(type_node);
        }
    }

    // Check for static keyword
    TSNode static_node = get_child_by_type(node, "static_keyword");
    func_decl->is_static = !node_is_null(static_node);

    // Get annotations
    func_decl->annotations = extract_annotations(node);

    // Get body
    TSNode body_node = get_child_by_field(node, "body");
    if (!node_is_null(body_node)) {
        func_decl->body = build_node(body_node);
    }

    return func_decl;
}

std::vector<ASTNodePtr> ASTBuilder::build_parameters(TSNode params_node) {
    std::vector<ASTNodePtr> parameters;

    uint32_t count = ts_node_child_count(params_node);
    for (uint32_t i = 0; i < count; i++) {
        TSNode child = ts_node_child(params_node, i);
        if (node_is_named(child)) {
            auto param = build_parameter(child);
            if (param) {
                parameters.push_back(param);
            }
        }
    }

    return parameters;
}

ASTNodePtr ASTBuilder::build_parameter(TSNode node) {
    auto param = std::make_shared<Parameter>();
    param->node_type_str = get_node_type(node);
    param->location = SourceLocation(node);

    std::string type = get_node_type(node);

    if (type == "identifier") {
        // Simple parameter
        param->param_name = get_node_text(node);
    }
    else if (type == "typed_parameter") {
        // Typed parameter
        TSNode id_node = get_child_by_type(node, "identifier");
        if (!node_is_null(id_node)) {
            param->param_name = get_node_text(id_node);
        }
        TSNode type_node = get_child_by_field(node, "type");
        if (!node_is_null(type_node)) {
            TSNode type_id = ts_node_child(type_node, 0);
            if (!node_is_null(type_id) && node_is_named(type_id)) {
                param->param_type = get_node_text(type_id);
            }
        }
    }
    else if (type == "default_parameter") {
        // Parameter with default value
        TSNode id_node = get_child_by_type(node, "identifier");
        if (!node_is_null(id_node)) {
            param->param_name = get_node_text(id_node);
        }
        TSNode value_node = get_child_by_field(node, "value");
        if (!node_is_null(value_node)) {
            param->default_value = build_expression(value_node);
        }
    }
    else if (type == "typed_default_parameter") {
        // Typed parameter with default value
        TSNode id_node = get_child_by_type(node, "identifier");
        if (!node_is_null(id_node)) {
            param->param_name = get_node_text(id_node);
        }
        TSNode type_node = get_child_by_field(node, "type");
        if (!node_is_null(type_node)) {
            TSNode type_id = ts_node_child(type_node, 0);
            if (!node_is_null(type_id) && node_is_named(type_id)) {
                param->param_type = get_node_text(type_id);
            }
        }
        TSNode value_node = get_child_by_field(node, "value");
        if (!node_is_null(value_node)) {
            param->default_value = build_expression(value_node);
        }
    }

    return param;
}

ASTNodePtr ASTBuilder::build_variable_declaration(TSNode node, bool is_export, bool is_onready) {
    auto var_decl = std::make_shared<VariableDecl>();
    var_decl->node_type_str = get_node_type(node);
    var_decl->location = SourceLocation(node);
    var_decl->is_export = is_export;
    var_decl->is_onready = is_onready;

    // Get variable name
    TSNode name_node = get_child_by_field(node, "name");
    if (!node_is_null(name_node)) {
        var_decl->var_name = get_node_text(name_node);
    }

    // Get type
    TSNode type_node = get_child_by_field(node, "type");
    if (!node_is_null(type_node)) {
        std::string type_str = get_node_type(type_node);
        if (type_str == "inferred_type") {
            var_decl->is_inferred_type = true;
        } else {
            TSNode type_id = ts_node_child(type_node, 0);
            if (!node_is_null(type_id) && node_is_named(type_id)) {
                var_decl->var_type = get_node_text(type_id);
            }
        }
    }

    // Get initializer
    TSNode value_node = get_child_by_field(node, "value");
    if (!node_is_null(value_node)) {
        var_decl->initializer = build_expression(value_node);
    }

    // Get annotations
    var_decl->annotations = extract_annotations(node);

    return var_decl;
}

ASTNodePtr ASTBuilder::build_const_declaration(TSNode node) {
    auto const_decl = std::make_shared<ConstDecl>();
    const_decl->node_type_str = "const_statement";
    const_decl->location = SourceLocation(node);

    // Get const name
    TSNode name_node = get_child_by_field(node, "name");
    if (!node_is_null(name_node)) {
        const_decl->const_name = get_node_text(name_node);
    }

    // Get type
    TSNode type_node = get_child_by_field(node, "type");
    if (!node_is_null(type_node)) {
        std::string type_str = get_node_type(type_node);
        if (type_str == "inferred_type") {
            const_decl->is_inferred_type = true;
        } else {
            TSNode type_id = ts_node_child(type_node, 0);
            if (!node_is_null(type_id) && node_is_named(type_id)) {
                const_decl->const_type = get_node_text(type_id);
            }
        }
    }

    // Get value
    TSNode value_node = get_child_by_field(node, "value");
    if (!node_is_null(value_node)) {
        const_decl->value = build_expression(value_node);
    }

    return const_decl;
}

ASTNodePtr ASTBuilder::build_signal_declaration(TSNode node) {
    auto signal_decl = std::make_shared<SignalDecl>();
    signal_decl->node_type_str = "signal_statement";
    signal_decl->location = SourceLocation(node);

    // Get signal name
    TSNode name_node = get_child_by_field(node, "name");
    if (!node_is_null(name_node)) {
        signal_decl->signal_name = get_node_text(name_node);
    }

    // Get parameters if any
    TSNode params_node = get_child_by_field(node, "parameters");
    if (!node_is_null(params_node)) {
        signal_decl->parameters = build_parameters(params_node);
    }

    return signal_decl;
}

ASTNodePtr ASTBuilder::build_enum_declaration(TSNode node) {
    auto enum_decl = std::make_shared<EnumDecl>();
    enum_decl->node_type_str = "enum_definition";
    enum_decl->location = SourceLocation(node);

    // Get enum name (optional for anonymous enums)
    TSNode name_node = get_child_by_field(node, "name");
    if (!node_is_null(name_node)) {
        enum_decl->enum_name = get_node_text(name_node);
    }

    // Get body
    TSNode body_node = get_child_by_field(node, "body");
    if (!node_is_null(body_node)) {
        uint32_t count = ts_node_child_count(body_node);
        for (uint32_t i = 0; i < count; i++) {
            TSNode child = ts_node_child(body_node, i);
            if (node_is_named(child) && get_node_type(child) == "enumerator") {
                TSNode left_node = get_child_by_field(child, "left");
                TSNode right_node = get_child_by_field(child, "right");

                std::string key = !node_is_null(left_node) ? get_node_text(left_node) : "";
                std::string value = !node_is_null(right_node) ? get_node_text(right_node) : "";

                enum_decl->members.push_back({key, value});
            }
        }
    }

    return enum_decl;
}

ASTNodePtr ASTBuilder::build_extends_statement(TSNode node) {
    auto extends_node = std::make_shared<ASTNode>(NodeType::ExtendsStatement);
    extends_node->node_type_str = "extends_statement";
    extends_node->location = SourceLocation(node);
    extends_node->text = get_node_text(node);
    return extends_node;
}

// Statement builders

ASTNodePtr ASTBuilder::build_if_statement(TSNode node) {
    auto if_stmt = std::make_shared<IfStmt>();
    if_stmt->node_type_str = "if_statement";
    if_stmt->location = SourceLocation(node);

    // Get condition
    TSNode cond_node = get_child_by_field(node, "condition");
    if (!node_is_null(cond_node)) {
        if_stmt->condition = build_expression(cond_node);
    }

    // Get body
    TSNode body_node = get_child_by_field(node, "body");
    if (!node_is_null(body_node)) {
        if_stmt->then_body = build_node(body_node);
    }

    // Get elif and else clauses
    uint32_t count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; i++) {
        TSNode child = ts_node_child(node, i);
        std::string type = get_node_type(child);
        if (type == "elif_clause") {
            if_stmt->elif_clauses.push_back(build_node(child));
        } else if (type == "else_clause") {
            if_stmt->else_body = build_node(child);
        }
    }

    return if_stmt;
}

ASTNodePtr ASTBuilder::build_for_statement(TSNode node) {
    auto for_stmt = std::make_shared<ForStmt>();
    for_stmt->node_type_str = "for_statement";
    for_stmt->location = SourceLocation(node);

    // Get iterator
    TSNode left_node = get_child_by_field(node, "left");
    if (!node_is_null(left_node)) {
        for_stmt->iterator_name = get_node_text(left_node);
    }

    // Get type if present
    TSNode type_node = get_child_by_field(node, "type");
    if (!node_is_null(type_node)) {
        TSNode type_id = ts_node_child(type_node, 0);
        if (!node_is_null(type_id) && node_is_named(type_id)) {
            for_stmt->iterator_type = get_node_text(type_id);
        }
    }

    // Get iterable
    TSNode right_node = get_child_by_field(node, "right");
    if (!node_is_null(right_node)) {
        for_stmt->iterable = build_expression(right_node);
    }

    // Get body
    TSNode body_node = get_child_by_field(node, "body");
    if (!node_is_null(body_node)) {
        for_stmt->body = build_node(body_node);
    }

    return for_stmt;
}

ASTNodePtr ASTBuilder::build_while_statement(TSNode node) {
    auto while_stmt = std::make_shared<WhileStmt>();
    while_stmt->node_type_str = "while_statement";
    while_stmt->location = SourceLocation(node);

    // Get condition
    TSNode cond_node = get_child_by_field(node, "condition");
    if (!node_is_null(cond_node)) {
        while_stmt->condition = build_expression(cond_node);
    }

    // Get body
    TSNode body_node = get_child_by_field(node, "body");
    if (!node_is_null(body_node)) {
        while_stmt->body = build_node(body_node);
    }

    return while_stmt;
}

ASTNodePtr ASTBuilder::build_match_statement(TSNode node) {
    auto match_stmt = std::make_shared<MatchStmt>();
    match_stmt->node_type_str = "match_statement";
    match_stmt->location = SourceLocation(node);

    // Get value to match
    TSNode value_node = get_child_by_field(node, "value");
    if (!node_is_null(value_node)) {
        match_stmt->value = build_expression(value_node);
    }

    // Get body with pattern sections
    TSNode body_node = get_child_by_field(node, "body");
    if (!node_is_null(body_node)) {
        uint32_t count = ts_node_child_count(body_node);
        for (uint32_t i = 0; i < count; i++) {
            TSNode child = ts_node_child(body_node, i);
            if (node_is_named(child) && get_node_type(child) == "pattern_section") {
                match_stmt->pattern_sections.push_back(build_node(child));
            }
        }
    }

    return match_stmt;
}

ASTNodePtr ASTBuilder::build_return_statement(TSNode node) {
    auto return_stmt = std::make_shared<ReturnStmt>();
    return_stmt->node_type_str = "return_statement";
    return_stmt->location = SourceLocation(node);

    // Get return value if present
    uint32_t count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; i++) {
        TSNode child = ts_node_child(node, i);
        if (node_is_named(child)) {
            return_stmt->value = build_expression(child);
            break;
        }
    }

    return return_stmt;
}

ASTNodePtr ASTBuilder::build_expression_statement(TSNode node) {
    auto expr_stmt = std::make_shared<ASTNode>(NodeType::ExpressionStatement);
    expr_stmt->node_type_str = "expression_statement";
    expr_stmt->location = SourceLocation(node);

    // Get the expression
    uint32_t count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; i++) {
        TSNode child = ts_node_child(node, i);
        if (node_is_named(child)) {
            auto child_node = build_node(child);
            if (child_node) {
                expr_stmt->add_child(child_node);
            }
        }
    }

    return expr_stmt;
}

ASTNodePtr ASTBuilder::build_assignment(TSNode node) {
    auto assign = std::make_shared<ASTNode>(NodeType::Assignment);
    assign->node_type_str = get_node_type(node);
    assign->location = SourceLocation(node);

    // Get left and right
    TSNode left_node = get_child_by_field(node, "left");
    TSNode right_node = get_child_by_field(node, "right");

    if (!node_is_null(left_node)) {
        auto left_expr = build_expression(left_node);
        if (left_expr) assign->add_child(left_expr);
    }

    if (!node_is_null(right_node)) {
        auto right_expr = build_expression(right_node);
        if (right_expr) assign->add_child(right_expr);
    }

    return assign;
}

// Expression builders

ASTNodePtr ASTBuilder::build_expression(TSNode node) {
    if (node_is_null(node)) return nullptr;

    std::string type = get_node_type(node);

    if (type == "identifier") {
        return build_identifier(node);
    }
    else if (type == "integer" || type == "float" || type == "string" ||
             type == "true" || type == "false" || type == "null") {
        return build_literal(node);
    }
    else if (type == "binary_operator") {
        return build_binary_expression(node);
    }
    else if (type == "unary_operator") {
        return build_unary_expression(node);
    }
    else if (type == "call") {
        return build_call_expression(node);
    }
    else if (type == "attribute") {
        return build_attribute_expression(node);
    }
    else if (type == "subscript") {
        return build_subscript_expression(node);
    }
    else if (type == "array") {
        return build_array_literal(node);
    }
    else if (type == "dictionary") {
        return build_dictionary_literal(node);
    }
    else if (type == "get_node") {
        return build_get_node(node);
    }
    else {
        // Generic expression node
        auto expr = std::make_shared<ASTNode>(NodeType::Unknown);
        expr->node_type_str = type;
        expr->location = SourceLocation(node);
        expr->text = get_node_text(node);
        return expr;
    }
}

ASTNodePtr ASTBuilder::build_identifier(TSNode node) {
    auto id = std::make_shared<IdentifierExpr>();
    id->node_type_str = "identifier";
    id->location = SourceLocation(node);
    id->name = get_node_text(node);
    id->text = id->name;
    return id;
}

ASTNodePtr ASTBuilder::build_literal(TSNode node) {
    std::string type = get_node_type(node);
    NodeType node_type;

    if (type == "integer") node_type = NodeType::IntegerLiteral;
    else if (type == "float") node_type = NodeType::FloatLiteral;
    else if (type == "string") node_type = NodeType::StringLiteral;
    else if (type == "true" || type == "false") node_type = NodeType::BooleanLiteral;
    else if (type == "null") node_type = NodeType::NullLiteral;
    else node_type = NodeType::Unknown;

    auto lit = std::make_shared<LiteralExpr>(node_type);
    lit->node_type_str = type;
    lit->location = SourceLocation(node);
    lit->value = get_node_text(node);
    lit->text = lit->value;
    return lit;
}

ASTNodePtr ASTBuilder::build_binary_expression(TSNode node) {
    auto binary = std::make_shared<BinaryExpr>();
    binary->node_type_str = "binary_operator";
    binary->location = SourceLocation(node);

    // Get operator
    TSNode op_node = get_child_by_field(node, "op");
    if (!node_is_null(op_node)) {
        binary->op = get_node_text(op_node);
    }

    // Get left and right operands
    TSNode left_node = get_child_by_field(node, "left");
    if (!node_is_null(left_node)) {
        binary->left = build_expression(left_node);
    }

    TSNode right_node = get_child_by_field(node, "right");
    if (!node_is_null(right_node)) {
        binary->right = build_expression(right_node);
    }

    return binary;
}

ASTNodePtr ASTBuilder::build_unary_expression(TSNode node) {
    auto unary = std::make_shared<UnaryExpr>();
    unary->node_type_str = "unary_operator";
    unary->location = SourceLocation(node);

    // Get operator and operand
    unary->text = get_node_text(node);

    uint32_t count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; i++) {
        TSNode child = ts_node_child(node, i);
        if (node_is_named(child)) {
            unary->operand = build_expression(child);
        } else {
            unary->op = get_node_text(child);
        }
    }

    return unary;
}

ASTNodePtr ASTBuilder::build_call_expression(TSNode node) {
    auto call = std::make_shared<CallExpr>();
    call->node_type_str = "call";
    call->location = SourceLocation(node);

    // Get callee
    uint32_t count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; i++) {
        TSNode child = ts_node_child(node, i);
        std::string type = get_node_type(child);

        if (type != "arguments" && node_is_named(child)) {
            call->callee = build_expression(child);
        } else if (type == "arguments") {
            // Get arguments
            uint32_t arg_count = ts_node_child_count(child);
            for (uint32_t j = 0; j < arg_count; j++) {
                TSNode arg = ts_node_child(child, j);
                if (node_is_named(arg)) {
                    auto arg_expr = build_expression(arg);
                    if (arg_expr) {
                        call->arguments.push_back(arg_expr);
                    }
                }
            }
        }
    }

    return call;
}

ASTNodePtr ASTBuilder::build_attribute_expression(TSNode node) {
    auto attr = std::make_shared<ASTNode>(NodeType::AttributeExpression);
    attr->node_type_str = "attribute";
    attr->location = SourceLocation(node);
    attr->text = get_node_text(node);

    // Build children
    uint32_t count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; i++) {
        TSNode child = ts_node_child(node, i);
        if (node_is_named(child)) {
            auto child_node = build_expression(child);
            if (child_node) {
                attr->add_child(child_node);
            }
        }
    }

    return attr;
}

ASTNodePtr ASTBuilder::build_subscript_expression(TSNode node) {
    auto subscript = std::make_shared<ASTNode>(NodeType::SubscriptExpression);
    subscript->node_type_str = "subscript";
    subscript->location = SourceLocation(node);
    subscript->text = get_node_text(node);

    // Build children
    uint32_t count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; i++) {
        TSNode child = ts_node_child(node, i);
        if (node_is_named(child)) {
            auto child_node = build_expression(child);
            if (child_node) {
                subscript->add_child(child_node);
            }
        }
    }

    return subscript;
}

ASTNodePtr ASTBuilder::build_array_literal(TSNode node) {
    auto array = std::make_shared<ASTNode>(NodeType::ArrayLiteral);
    array->node_type_str = "array";
    array->location = SourceLocation(node);

    // Build elements
    uint32_t count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; i++) {
        TSNode child = ts_node_child(node, i);
        if (node_is_named(child)) {
            auto elem = build_expression(child);
            if (elem) {
                array->add_child(elem);
            }
        }
    }

    return array;
}

ASTNodePtr ASTBuilder::build_dictionary_literal(TSNode node) {
    auto dict = std::make_shared<ASTNode>(NodeType::DictionaryLiteral);
    dict->node_type_str = "dictionary";
    dict->location = SourceLocation(node);

    // Build pairs
    uint32_t count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; i++) {
        TSNode child = ts_node_child(node, i);
        if (node_is_named(child) && get_node_type(child) == "pair") {
            auto pair = std::make_shared<ASTNode>(NodeType::Unknown);
            pair->node_type_str = "pair";
            pair->location = SourceLocation(child);

            TSNode left = get_child_by_field(child, "left");
            TSNode right = get_child_by_field(child, "value");

            if (!node_is_null(left)) {
                auto left_expr = build_expression(left);
                if (left_expr) pair->add_child(left_expr);
            }
            if (!node_is_null(right)) {
                auto right_expr = build_expression(right);
                if (right_expr) pair->add_child(right_expr);
            }

            dict->add_child(pair);
        }
    }

    return dict;
}

ASTNodePtr ASTBuilder::build_get_node(TSNode node) {
    auto get_node_expr = std::make_shared<ASTNode>(NodeType::GetNodeExpression);
    get_node_expr->node_type_str = "get_node";
    get_node_expr->location = SourceLocation(node);
    get_node_expr->text = get_node_text(node);
    return get_node_expr;
}

std::vector<std::string> ASTBuilder::extract_annotations(TSNode node) {
    std::vector<std::string> annotations;

    uint32_t count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; i++) {
        TSNode child = ts_node_child(node, i);
        if (get_node_type(child) == "annotations") {
            uint32_t anno_count = ts_node_child_count(child);
            for (uint32_t j = 0; j < anno_count; j++) {
                TSNode anno = ts_node_child(child, j);
                if (get_node_type(anno) == "annotation") {
                    annotations.push_back(get_node_text(anno));
                }
            }
        }
    }

    return annotations;
}

std::string ASTBuilder::extract_comment(TSNode node) {
    // Look for comment nodes before this node
    // This is a simplified implementation
    return "";
}

} // namespace gd2ts
