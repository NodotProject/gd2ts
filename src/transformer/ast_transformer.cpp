#include "ast_transformer.h"
#include <sstream>
#include <algorithm>

namespace gd2ts {

ASTTransformer::ASTTransformer() : generator("\t", true) {
}

std::string ASTTransformer::transform(const ASTNodePtr& ast) {
    if (!ast) {
        return "";
    }

    generator.clear();
    required_imports.clear();
    godot_types_used.clear();

    // Process root node
    if (ast->type == NodeType::Source) {
        // Process all top-level nodes
        for (const auto& child : ast->children) {
            if (!child) continue;

            switch (child->type) {
                case NodeType::ClassDeclaration:
                    transform_class_declaration(child);
                    break;
                case NodeType::FunctionDeclaration:
                    transform_function_declaration(child);
                    break;
                case NodeType::VariableDeclaration:
                    transform_variable_declaration(child);
                    break;
                case NodeType::ConstDeclaration:
                    transform_const_declaration(child);
                    break;
                case NodeType::SignalDeclaration:
                    transform_signal_declaration(child);
                    break;
                case NodeType::EnumDeclaration:
                    transform_enum_declaration(child);
                    break;
                case NodeType::Comment:
                    generator.write_line("// " + child->text);
                    break;
                default:
                    // Unknown top-level node
                    break;
            }
        }
    } else if (ast->type == NodeType::ClassDeclaration) {
        transform_class_declaration(ast);
    }

    // Generate imports at the beginning
    std::string output = generator.get_output();
    std::string imports;
    if (!godot_types_used.empty() || !required_imports.empty()) {
        std::stringstream import_stream;
        // Add Godot type imports if needed
        if (!godot_types_used.empty()) {
            import_stream << "// Import Godot types\n";
            import_stream << "// import { ";
            bool first = true;
            for (const auto& type : godot_types_used) {
                if (!first) import_stream << ", ";
                import_stream << type;
                first = false;
            }
            import_stream << " } from 'godot';\n\n";
        }
        imports = import_stream.str();
    }

    return imports + output;
}

void ASTTransformer::transform_class_declaration(const ASTNodePtr& node) {
    if (!node) return;

    auto class_node = std::dynamic_pointer_cast<ClassDecl>(node);
    if (!class_node) {
        // Fallback: try to extract from generic node
        class_node = std::make_shared<ClassDecl>();
        class_node->children = node->children;

        // Try to find extends and class_name
        for (const auto& child : node->children) {
            if (child->type == NodeType::ExtendsStatement) {
                class_node->extends_type = child->text;
            } else if (child->type == NodeType::ClassNameStatement) {
                class_node->class_name = child->text;
            }
        }
    }

    // Write class declaration
    if (!class_node->class_name.empty()) {
        std::string class_decl = "class " + class_node->class_name;
        if (!class_node->extends_type.empty()) {
            class_decl += " extends " + class_node->extends_type;
            track_type_usage(class_node->extends_type);
        }
        generator.write_block_start(class_decl);
    } else if (!class_node->extends_type.empty()) {
        std::string class_decl = "class extends " + class_node->extends_type;
        track_type_usage(class_node->extends_type);
        generator.write_block_start(class_decl);
    } else {
        generator.write_block_start("class");
    }

    // Process class body
    for (const auto& child : class_node->children) {
        if (!child) continue;

        switch (child->type) {
            case NodeType::VariableDeclaration:
                transform_variable_declaration(child);
                break;
            case NodeType::ConstDeclaration:
                transform_const_declaration(child);
                break;
            case NodeType::SignalDeclaration:
                transform_signal_declaration(child);
                break;
            case NodeType::EnumDeclaration:
                transform_enum_declaration(child);
                break;
            case NodeType::FunctionDeclaration:
                transform_function_declaration(child);
                break;
            case NodeType::ClassDeclaration:
                // Nested class
                transform_class_declaration(child);
                break;
            case NodeType::Comment:
                generator.write_line("// " + child->text);
                break;
            case NodeType::ExtendsStatement:
            case NodeType::ClassNameStatement:
                // Already handled above
                break;
            default:
                break;
        }
    }

    generator.write_block_end();
    generator.new_line();
}

void ASTTransformer::transform_function_declaration(const ASTNodePtr& node) {
    if (!node) return;

    auto func_node = std::dynamic_pointer_cast<FunctionDecl>(node);
    if (!func_node) return;

    // Write function signature
    std::string func_decl = func_node->function_name;
    func_decl += "(" + get_function_parameters(func_node->parameters) + ")";

    if (!func_node->return_type.empty()) {
        std::string ts_return_type = type_mapper.map_type(func_node->return_type);
        func_decl += ": " + ts_return_type;
        track_type_usage(ts_return_type);
    } else {
        func_decl += ": void";
    }

    generator.write_block_start(func_decl);

    // Process function body
    if (func_node->body && func_node->body->has_children()) {
        for (const auto& stmt : func_node->body->children) {
            transform_statement(stmt);
        }
    }

    generator.write_block_end();
    generator.new_line();
}

void ASTTransformer::transform_variable_declaration(const ASTNodePtr& node) {
    if (!node) return;

    auto var_node = std::dynamic_pointer_cast<VariableDecl>(node);
    if (!var_node) return;

    // Handle @export annotation
    if (var_node->is_export) {
        generator.write_line("@export");
    }

    // Handle @onready
    if (var_node->is_onready) {
        generator.write_line("@onready");
    }

    std::string var_decl = var_node->var_name;

    // Add type annotation
    if (!var_node->var_type.empty()) {
        std::string ts_type = type_mapper.map_type(var_node->var_type);
        var_decl += ": " + ts_type;
        track_type_usage(ts_type);
    } else if (var_node->initializer) {
        // Try to infer type from initializer
        std::string init_expr = transform_expression(var_node->initializer);
        std::string inferred_type = type_mapper.infer_type_from_literal(init_expr);
        if (inferred_type != "any") {
            var_decl += ": " + inferred_type;
        }
    }

    // Add initializer
    if (var_node->initializer) {
        var_decl += " = " + transform_expression(var_node->initializer);
    }

    generator.write_statement(var_decl);
}

void ASTTransformer::transform_const_declaration(const ASTNodePtr& node) {
    if (!node) return;

    auto const_node = std::dynamic_pointer_cast<ConstDecl>(node);
    if (!const_node) return;

    std::string const_decl = "const " + const_node->const_name;

    // Add type annotation
    if (!const_node->const_type.empty()) {
        std::string ts_type = type_mapper.map_type(const_node->const_type);
        const_decl += ": " + ts_type;
        track_type_usage(ts_type);
    }

    // Add value
    if (const_node->value) {
        const_decl += " = " + transform_expression(const_node->value);
    }

    generator.write_statement(const_decl);
}

void ASTTransformer::transform_signal_declaration(const ASTNodePtr& node) {
    if (!node) return;

    auto signal_node = std::dynamic_pointer_cast<SignalDecl>(node);
    if (!signal_node) return;

    // Transform to TypeScript signal syntax (following ts2gd convention)
    std::string signal_decl = "$" + signal_node->signal_name + "!: Signal";

    if (!signal_node->parameters.empty()) {
        signal_decl += "<[";
        for (size_t i = 0; i < signal_node->parameters.size(); ++i) {
            if (i > 0) signal_decl += ", ";
            auto param = std::dynamic_pointer_cast<Parameter>(signal_node->parameters[i]);
            if (param && !param->param_type.empty()) {
                signal_decl += type_mapper.map_type(param->param_type);
            } else {
                signal_decl += "any";
            }
        }
        signal_decl += "]>";
    }

    track_type_usage("Signal");
    generator.write_statement(signal_decl);
}

void ASTTransformer::transform_enum_declaration(const ASTNodePtr& node) {
    if (!node) return;

    auto enum_node = std::dynamic_pointer_cast<EnumDecl>(node);
    if (!enum_node) return;

    std::string enum_decl;
    if (!enum_node->enum_name.empty()) {
        enum_decl = "enum " + enum_node->enum_name;
    } else {
        enum_decl = "enum";
    }

    generator.write_block_start(enum_decl);

    for (size_t i = 0; i < enum_node->members.size(); ++i) {
        const auto& member = enum_node->members[i];
        std::string member_decl = member.first;
        if (!member.second.empty()) {
            member_decl += " = " + member.second;
        }
        if (i < enum_node->members.size() - 1) {
            member_decl += ",";
        }
        generator.write_line(member_decl);
    }

    generator.write_block_end();
}

void ASTTransformer::transform_statement(const ASTNodePtr& node) {
    if (!node) return;

    switch (node->type) {
        case NodeType::IfStatement:
            transform_if_statement(node);
            break;
        case NodeType::ForStatement:
            transform_for_statement(node);
            break;
        case NodeType::WhileStatement:
            transform_while_statement(node);
            break;
        case NodeType::MatchStatement:
            transform_match_statement(node);
            break;
        case NodeType::ReturnStatement:
            transform_return_statement(node);
            break;
        case NodeType::ExpressionStatement:
            transform_expression_statement(node);
            break;
        case NodeType::Assignment:
        case NodeType::AugmentedAssignment:
            transform_assignment(node);
            break;
        case NodeType::BreakStatement:
            generator.write_statement("break");
            break;
        case NodeType::ContinueStatement:
            generator.write_statement("continue");
            break;
        case NodeType::PassStatement:
            generator.write_line("// pass");
            break;
        case NodeType::Comment:
            generator.write_line("// " + node->text);
            break;
        default:
            // Try to transform as expression statement
            if (node->has_children()) {
                for (const auto& child : node->children) {
                    transform_statement(child);
                }
            }
            break;
    }
}

void ASTTransformer::transform_if_statement(const ASTNodePtr& node) {
    if (!node) return;

    auto if_node = std::dynamic_pointer_cast<IfStmt>(node);
    if (!if_node) return;

    // Main if condition
    std::string condition = transform_expression(if_node->condition);
    generator.write_block_start("if (" + condition + ")");

    // Then body
    if (if_node->then_body && if_node->then_body->has_children()) {
        for (const auto& stmt : if_node->then_body->children) {
            transform_statement(stmt);
        }
    }

    generator.dedent();
    generator.write("}");

    // Elif clauses (become else if)
    for (const auto& elif_clause : if_node->elif_clauses) {
        if (!elif_clause || !elif_clause->has_children()) continue;

        auto elif_condition = elif_clause->get_child(0);
        auto elif_body = elif_clause->get_child(1);

        if (elif_condition) {
            std::string elif_cond = transform_expression(elif_condition);
            generator.write(" else ");
            generator.write_block_start("if (" + elif_cond + ")");

            if (elif_body && elif_body->has_children()) {
                for (const auto& stmt : elif_body->children) {
                    transform_statement(stmt);
                }
            }

            generator.dedent();
            generator.write("}");
        }
    }

    // Else clause
    if (if_node->else_body && if_node->else_body->has_children()) {
        generator.write(" else ");
        generator.write_block_start("");
        for (const auto& stmt : if_node->else_body->children) {
            transform_statement(stmt);
        }
        generator.write_block_end();
    } else {
        generator.new_line();
    }

    generator.new_line();
}

void ASTTransformer::transform_for_statement(const ASTNodePtr& node) {
    if (!node) return;

    auto for_node = std::dynamic_pointer_cast<ForStmt>(node);
    if (!for_node) return;

    std::string iterable = transform_expression(for_node->iterable);

    // Check if it's a range() call - transform to C-style for loop
    if (iterable.find("range(") == 0) {
        // TODO: Parse range() parameters and generate proper for loop
        // For now, use for-of
        std::string for_decl = "for (const " + for_node->iterator_name + " of " + iterable + ")";
        generator.write_block_start(for_decl);
    } else {
        // Regular for-of loop
        std::string for_decl = "for (const " + for_node->iterator_name + " of " + iterable + ")";
        generator.write_block_start(for_decl);
    }

    // Body
    if (for_node->body && for_node->body->has_children()) {
        for (const auto& stmt : for_node->body->children) {
            transform_statement(stmt);
        }
    }

    generator.write_block_end();
    generator.new_line();
}

void ASTTransformer::transform_while_statement(const ASTNodePtr& node) {
    if (!node) return;

    auto while_node = std::dynamic_pointer_cast<WhileStmt>(node);
    if (!while_node) return;

    std::string condition = transform_expression(while_node->condition);
    generator.write_block_start("while (" + condition + ")");

    // Body
    if (while_node->body && while_node->body->has_children()) {
        for (const auto& stmt : while_node->body->children) {
            transform_statement(stmt);
        }
    }

    generator.write_block_end();
    generator.new_line();
}

void ASTTransformer::transform_match_statement(const ASTNodePtr& node) {
    if (!node) return;

    auto match_node = std::dynamic_pointer_cast<MatchStmt>(node);
    if (!match_node) return;

    // Transform match to switch
    std::string value = transform_expression(match_node->value);
    generator.write_block_start("switch (" + value + ")");

    // Process pattern sections
    for (const auto& pattern : match_node->pattern_sections) {
        // TODO: Implement full pattern matching transformation
        // For now, simple case transformation
        generator.write_line("// TODO: Pattern matching");
    }

    generator.write_block_end();
    generator.new_line();
}

void ASTTransformer::transform_return_statement(const ASTNodePtr& node) {
    if (!node) return;

    auto return_node = std::dynamic_pointer_cast<ReturnStmt>(node);
    if (!return_node) return;

    if (return_node->value) {
        std::string return_expr = transform_expression(return_node->value);
        generator.write_statement("return " + return_expr);
    } else {
        generator.write_statement("return");
    }
}

void ASTTransformer::transform_expression_statement(const ASTNodePtr& node) {
    if (!node) return;

    if (node->has_children() && node->children[0]) {
        std::string expr = transform_expression(node->children[0]);
        generator.write_statement(expr);
    }
}

void ASTTransformer::transform_assignment(const ASTNodePtr& node) {
    if (!node) return;

    if (node->children.size() >= 2) {
        std::string left = transform_expression(node->children[0]);
        std::string right = transform_expression(node->children[1]);

        // Check for augmented assignment
        if (node->type == NodeType::AugmentedAssignment && !node->text.empty()) {
            generator.write_statement(left + " " + node->text + " " + right);
        } else {
            generator.write_statement(left + " = " + right);
        }
    }
}

std::string ASTTransformer::transform_expression(const ASTNodePtr& node) {
    if (!node) return "";

    switch (node->type) {
        case NodeType::Identifier:
            return transform_identifier(node);
        case NodeType::IntegerLiteral:
        case NodeType::FloatLiteral:
        case NodeType::StringLiteral:
        case NodeType::BooleanLiteral:
        case NodeType::NullLiteral:
            return transform_literal(node);
        case NodeType::BinaryExpression:
            return transform_binary_expression(node);
        case NodeType::UnaryExpression:
            return transform_unary_expression(node);
        case NodeType::CallExpression:
            return transform_call_expression(node);
        case NodeType::AttributeExpression:
            return transform_attribute_expression(node);
        case NodeType::SubscriptExpression:
            return transform_subscript_expression(node);
        case NodeType::ArrayLiteral:
            return transform_array_literal(node);
        case NodeType::DictionaryLiteral:
            return transform_dictionary_literal(node);
        case NodeType::GetNodeExpression:
            return transform_get_node(node);
        default:
            return node->text;
    }
}

std::string ASTTransformer::transform_binary_expression(const ASTNodePtr& node) {
    auto binary = std::dynamic_pointer_cast<BinaryExpr>(node);
    if (!binary) return "";

    std::string left = transform_expression(binary->left);
    std::string right = transform_expression(binary->right);
    std::string op = map_operator(binary->op);

    return left + " " + op + " " + right;
}

std::string ASTTransformer::transform_unary_expression(const ASTNodePtr& node) {
    auto unary = std::dynamic_pointer_cast<UnaryExpr>(node);
    if (!unary) return "";

    std::string operand = transform_expression(unary->operand);
    std::string op = map_operator(unary->op);

    return op + operand;
}

std::string ASTTransformer::transform_call_expression(const ASTNodePtr& node) {
    auto call = std::dynamic_pointer_cast<CallExpr>(node);
    if (!call) return "";

    std::string callee = transform_expression(call->callee);

    std::string args = "(";
    for (size_t i = 0; i < call->arguments.size(); ++i) {
        if (i > 0) args += ", ";
        args += transform_expression(call->arguments[i]);
    }
    args += ")";

    return callee + args;
}

std::string ASTTransformer::transform_attribute_expression(const ASTNodePtr& node) {
    if (!node || node->children.size() < 2) return "";

    std::string object = transform_expression(node->children[0]);
    std::string attribute = transform_expression(node->children[1]);

    return object + "." + attribute;
}

std::string ASTTransformer::transform_subscript_expression(const ASTNodePtr& node) {
    if (!node || node->children.size() < 2) return "";

    std::string object = transform_expression(node->children[0]);
    std::string index = transform_expression(node->children[1]);

    return object + "[" + index + "]";
}

std::string ASTTransformer::transform_identifier(const ASTNodePtr& node) {
    auto ident = std::dynamic_pointer_cast<IdentifierExpr>(node);
    if (ident) {
        return replace_keywords(ident->name);
    }
    return replace_keywords(node->text);
}

std::string ASTTransformer::transform_literal(const ASTNodePtr& node) {
    if (node->type == NodeType::NullLiteral) {
        return "null";
    }
    return node->text;
}

std::string ASTTransformer::transform_array_literal(const ASTNodePtr& node) {
    std::string result = "[";
    for (size_t i = 0; i < node->children.size(); ++i) {
        if (i > 0) result += ", ";
        result += transform_expression(node->children[i]);
    }
    result += "]";
    return result;
}

std::string ASTTransformer::transform_dictionary_literal(const ASTNodePtr& node) {
    std::string result = "{";
    // Dictionary children come in key-value pairs
    for (size_t i = 0; i < node->children.size(); i += 2) {
        if (i > 0) result += ", ";
        result += transform_expression(node->children[i]) + ": ";
        if (i + 1 < node->children.size()) {
            result += transform_expression(node->children[i + 1]);
        }
    }
    result += "}";
    return result;
}

std::string ASTTransformer::transform_get_node(const ASTNodePtr& node) {
    if (!node || node->children.empty()) return "";

    std::string path = transform_expression(node->children[0]);
    return "this.get_node(" + path + ")";
}

std::string ASTTransformer::get_function_parameters(const std::vector<ASTNodePtr>& params) {
    std::string result;
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) result += ", ";

        auto param = std::dynamic_pointer_cast<Parameter>(params[i]);
        if (!param) continue;

        result += param->param_name;

        if (!param->param_type.empty()) {
            std::string ts_type = type_mapper.map_type(param->param_type);
            result += ": " + ts_type;
            track_type_usage(ts_type);
        }

        if (param->default_value) {
            result += " = " + transform_expression(param->default_value);
        }
    }
    return result;
}

std::string ASTTransformer::get_return_type(const std::string& gd_type) {
    if (gd_type.empty()) {
        return "void";
    }
    return type_mapper.map_type(gd_type);
}

std::string ASTTransformer::map_operator(const std::string& op) {
    // Most operators are the same, but some differ
    if (op == "and") return "&&";
    if (op == "or") return "||";
    if (op == "not") return "!";
    return op;
}

void ASTTransformer::track_type_usage(const std::string& type) {
    if (type_mapper.needs_godot_import(type)) {
        godot_types_used.insert(type);
    }
}

std::string ASTTransformer::replace_keywords(const std::string& text) {
    if (text == "self") return "this";
    return text;
}

void ASTTransformer::set_indent_style(const std::string& style) {
    // Would need to recreate generator with new style
}

void ASTTransformer::set_use_semicolons(bool use) {
    // Would need to recreate generator with new setting
}

} // namespace gd2ts
