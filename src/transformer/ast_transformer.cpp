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
        // Check if we have extends/class_name at module level - if so, wrap in a class
        std::string extends_type;
        std::string class_name;
        std::vector<ASTNodePtr> class_members;
        std::vector<ASTNodePtr> non_class_items;

        // First pass: collect extends, class_name, and categorize children
        for (const auto& child : ast->children) {
            if (!child) continue;

            if (child->type == NodeType::ExtendsStatement) {
                extends_type = child->text;
                // Extract just the type name (remove "extends " prefix)
                size_t space_pos = extends_type.find(' ');
                if (space_pos != std::string::npos) {
                    extends_type = extends_type.substr(space_pos + 1);
                }
            } else if (child->type == NodeType::ClassNameStatement) {
                auto class_decl = std::dynamic_pointer_cast<ClassDecl>(child);
                if (class_decl) {
                    class_name = class_decl->class_name;
                }
            } else if (child->type == NodeType::FunctionDeclaration ||
                       child->type == NodeType::VariableDeclaration ||
                       child->type == NodeType::ConstDeclaration ||
                       child->type == NodeType::SignalDeclaration ||
                       child->type == NodeType::EnumDeclaration) {
                class_members.push_back(child);
            } else {
                non_class_items.push_back(child);
            }
        }

        // If we have extends or class_name, wrap everything in a class
        if (!extends_type.empty() || !class_name.empty()) {
            // Output non-class items first (comments, etc.)
            for (const auto& child : non_class_items) {
                if (child->type == NodeType::Comment) {
                    auto comment_node = std::dynamic_pointer_cast<CommentNode>(child);
                    if (comment_node && !comment_node->comment_text.empty()) {
                        std::string comment = comment_node->comment_text;
                        if (!comment.empty() && comment[0] == '#') {
                            comment = comment.substr(1);
                        }
                        size_t first = comment.find_first_not_of(" \t");
                        if (first != std::string::npos) {
                            comment = comment.substr(first);
                        }
                        generator.write_line("// " + comment);
                    }
                }
            }

            // Write class declaration
            std::string class_decl;
            if (!class_name.empty()) {
                class_decl = "class " + class_name;
            } else {
                class_decl = "class";
            }

            if (!extends_type.empty()) {
                class_decl += " extends " + extends_type;
                track_type_usage(extends_type);
            }

            generator.write_block_start(class_decl);

            // Output class members
            for (const auto& child : class_members) {
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
                    default:
                        break;
                }
            }

            generator.write_block_end();
            generator.new_line();
        } else {
            // No class wrapping needed - output everything as is
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
                        {
                            auto comment_node = std::dynamic_pointer_cast<CommentNode>(child);
                            if (comment_node && !comment_node->comment_text.empty()) {
                                std::string comment = comment_node->comment_text;
                                if (!comment.empty() && comment[0] == '#') {
                                    comment = comment.substr(1);
                                }
                                size_t first = comment.find_first_not_of(" \t");
                                if (first != std::string::npos) {
                                    comment = comment.substr(first);
                                }
                                generator.write_line("// " + comment);
                            }
                        }
                        break;
                    default:
                        break;
                }
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
            import_stream << "// Import Godot types (uncomment when ready to use)\n";
            import_stream << "import { ";
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

    // Handle @tool annotation for class (matching ts2gd convention)
    if (class_node->is_tool) {
        generator.write_line("@tool");
    }

    // Handle other class-level annotations
    for (const auto& annotation : class_node->annotations) {
        generator.write_line("@" + annotation);
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
                {
                    auto comment_node = std::dynamic_pointer_cast<CommentNode>(child);
                    if (comment_node && !comment_node->comment_text.empty()) {
                        std::string comment = comment_node->comment_text;
                        // Remove leading # if present
                        if (!comment.empty() && comment[0] == '#') {
                            comment = comment.substr(1);
                        }
                        // Trim leading whitespace
                        size_t first = comment.find_first_not_of(" \t");
                        if (first != std::string::npos) {
                            comment = comment.substr(first);
                        }
                        generator.write_line("// " + comment);
                    }
                }
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

    // Handle @export annotation (matching ts2gd convention)
    if (var_node->is_export) {
        generator.write_line("@exports");
    }

    // Handle @onready annotation
    if (var_node->is_onready) {
        generator.write_line("@onready");
    }

    std::string var_decl = var_node->var_name;

    // Add type annotation
    if (!var_node->var_type.empty()) {
        std::string ts_type = type_mapper.map_type(var_node->var_type);
        var_decl += ": " + ts_type;
        track_type_usage(ts_type);
    }

    // Add initializer
    if (var_node->initializer) {
        var_decl += " = " + transform_expression(var_node->initializer);
    } else if (var_node->var_type.empty()) {
        // No type and no initializer - try to infer type as any
        var_decl += ": any";
    }

    // Handle setget (property getters/setters)
    // Note: This is a comment for future implementation
    // GDScript: var health = 100 setget set_health, get_health
    // TypeScript: property with getter/setter methods

    generator.write_statement(var_decl);
}

void ASTTransformer::transform_local_variable_declaration(const ASTNodePtr& node) {
    if (!node) return;

    auto var_node = std::dynamic_pointer_cast<VariableDecl>(node);
    if (!var_node) return;

    // For local variables, use let keyword by default
    // TODO: Could analyze for reassignment to optimize const usage
    std::string var_decl = "let " + var_node->var_name;

    // Add type annotation
    if (!var_node->var_type.empty()) {
        std::string ts_type = type_mapper.map_type(var_node->var_type);
        var_decl += ": " + ts_type;
        track_type_usage(ts_type);
    }

    // Add initializer
    if (var_node->initializer) {
        var_decl += " = " + transform_expression(var_node->initializer);
    } else if (var_node->var_type.empty()) {
        // No type and no initializer - still need let/const
        var_decl = "let " + var_node->var_name + ": any";
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
        case NodeType::VariableDeclaration:
            transform_local_variable_declaration(node);
            break;
        case NodeType::ConstDeclaration:
            transform_const_declaration(node);
            break;
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
            {
                auto comment_node = std::dynamic_pointer_cast<CommentNode>(node);
                if (comment_node && !comment_node->comment_text.empty()) {
                    std::string comment = comment_node->comment_text;
                    // Remove leading # if present
                    if (!comment.empty() && comment[0] == '#') {
                        comment = comment.substr(1);
                    }
                    // Trim leading whitespace
                    size_t first = comment.find_first_not_of(" \t");
                    if (first != std::string::npos) {
                        comment = comment.substr(first);
                    }
                    generator.write_line("// " + comment);
                }
            }
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
        // Parse range() parameters: range(stop), range(start, stop), range(start, stop, step)
        std::string range_params = iterable.substr(6, iterable.length() - 7);  // Extract content between ( )

        // Split by commas (simple approach - doesn't handle nested calls)
        std::vector<std::string> params;
        std::string current;
        int paren_depth = 0;
        for (char c : range_params) {
            if (c == '(' || c == '[' || c == '{') paren_depth++;
            else if (c == ')' || c == ']' || c == '}') paren_depth--;
            else if (c == ',' && paren_depth == 0) {
                params.push_back(current);
                current.clear();
                continue;
            }
            current += c;
        }
        if (!current.empty()) params.push_back(current);

        // Trim whitespace from params
        for (auto& p : params) {
            size_t start = p.find_first_not_of(" \t");
            size_t end = p.find_last_not_of(" \t");
            if (start != std::string::npos && end != std::string::npos) {
                p = p.substr(start, end - start + 1);
            }
        }

        std::string for_decl;
        if (params.size() == 1) {
            // range(stop): for (let i = 0; i < stop; i++)
            for_decl = "for (let " + for_node->iterator_name + " = 0; " +
                       for_node->iterator_name + " < " + params[0] + "; " +
                       for_node->iterator_name + "++)";
        } else if (params.size() == 2) {
            // range(start, stop): for (let i = start; i < stop; i++)
            for_decl = "for (let " + for_node->iterator_name + " = " + params[0] + "; " +
                       for_node->iterator_name + " < " + params[1] + "; " +
                       for_node->iterator_name + "++)";
        } else if (params.size() == 3) {
            // range(start, stop, step): for (let i = start; i < stop; i += step)
            std::string step = params[2];
            std::string step_op = " += ";

            // Check if step is negative (simple check)
            if (step[0] == '-' || step.find(" - ") != std::string::npos) {
                // Negative step: use > comparison and += (step is already negative)
                for_decl = "for (let " + for_node->iterator_name + " = " + params[0] + "; " +
                           for_node->iterator_name + " > " + params[1] + "; " +
                           for_node->iterator_name + " += " + step + ")";
            } else {
                for_decl = "for (let " + for_node->iterator_name + " = " + params[0] + "; " +
                           for_node->iterator_name + " < " + params[1] + "; " +
                           for_node->iterator_name + " += " + step + ")";
            }
        } else {
            // Fallback: use for-of with range helper
            for_decl = "for (const " + for_node->iterator_name + " of " + iterable + ")";
        }

        generator.write_block_start(for_decl);
    } else {
        // Regular for-of loop (for item in array)
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
    for (const auto& pattern_section : match_node->pattern_sections) {
        if (!pattern_section || !pattern_section->has_children()) continue;

        // Pattern section structure: [pattern(s), body]
        // GDScript allows multiple patterns per section: 1, 2, 3: ...
        // We need to generate separate case statements for each

        // Get patterns and body
        std::vector<ASTNodePtr> patterns;
        ASTNodePtr body = nullptr;

        // Heuristic: last child is body, rest are patterns
        if (pattern_section->children.size() >= 2) {
            body = pattern_section->children.back();
            for (size_t i = 0; i < pattern_section->children.size() - 1; ++i) {
                patterns.push_back(pattern_section->children[i]);
            }
        } else if (pattern_section->children.size() == 1) {
            // Only body, pattern might be in text
            body = pattern_section->children[0];
        }

        // Generate case statements for each pattern
        for (const auto& pattern : patterns) {
            if (!pattern) continue;

            std::string pattern_str = transform_expression(pattern);

            // Check for special patterns
            if (pattern_str == "_") {
                // Wildcard pattern becomes default
                generator.write("default:");
            } else if (pattern_str.find("..") != std::string::npos) {
                // Range pattern - not directly supported in switch
                // We'll convert this to if-else later, for now comment
                generator.write_line("// Range pattern not supported in switch: " + pattern_str);
                continue;
            } else if (pattern_str.find('[') != std::string::npos ||
                       pattern_str.find('{') != std::string::npos) {
                // Array/dict pattern matching - not supported in switch
                generator.write_line("// Complex pattern matching not supported: " + pattern_str);
                continue;
            } else {
                // Simple value pattern
                generator.write("case " + pattern_str + ":");
            }

            generator.new_line();
        }

        // Generate body
        if (body && body->has_children()) {
            generator.indent();
            for (const auto& stmt : body->children) {
                transform_statement(stmt);
            }
            // Add break statement if not already present
            generator.write_statement("break");
            generator.dedent();
        }
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
        auto child = node->children[0];

        // Check if the expression is actually an assignment (which is statement in our AST)
        if (child->type == NodeType::Assignment || child->type == NodeType::AugmentedAssignment) {
            transform_assignment(child);
        } else {
            std::string expr = transform_expression(child);
            generator.write_statement(expr);
        }
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
        case NodeType::AwaitExpression:
            return transform_await_expression(node);
        case NodeType::LambdaExpression:
            return transform_lambda_expression(node);
        case NodeType::ConditionalExpression:
            return transform_conditional_expression(node);
        default:
            // For unknown node types, return the text but pass through keyword replacement
            // This handles cases where the parser creates nodes with text like "self"
            return replace_keywords(node->text);
    }
}

std::string ASTTransformer::transform_binary_expression(const ASTNodePtr& node) {
    auto binary = std::dynamic_pointer_cast<BinaryExpr>(node);
    if (!binary) return "";

    std::string left = transform_expression(binary->left);
    std::string right = transform_expression(binary->right);
    std::string op = map_operator(binary->op);

    // Handle 'is' operator - convert to instanceof
    if (binary->op == "is") {
        return left + " instanceof " + right;
    }

    // Handle typeof comparisons with TYPE_* constants
    // GDScript: typeof(x) == TYPE_INT
    // TypeScript: typeof x === "number"
    if (left.find("typeof") == 0 && (op == "==" || op == "===")) {
        // Check if right side is a TYPE_* constant
        if (right == "TYPE_INT" || right == "TYPE_FLOAT") {
            return left + " === \"number\"";
        } else if (right == "TYPE_STRING") {
            return left + " === \"string\"";
        } else if (right == "TYPE_BOOL") {
            return left + " === \"boolean\"";
        } else if (right == "TYPE_OBJECT") {
            return left + " === \"object\"";
        } else if (right == "TYPE_NIL" || right == "TYPE_NULL") {
            // For null checks, we should use === null instead of typeof
            // Extract the variable from typeof
            size_t start = left.find("typeof ");
            if (start != std::string::npos) {
                std::string var = left.substr(start + 7);
                return var + " === null";
            }
        }
    }

    // Handle typeof(null) comparisons - these should become simpler null checks
    if (left == "null" && right.find("typeof") == 0) {
        // typeof(var) == typeof(null) -> var === null
        size_t start = right.find("typeof ");
        if (start != std::string::npos) {
            std::string var = right.substr(start + 7);
            return var + " === null";
        }
    } else if (left.find("typeof") == 0 && right == "null") {
        // typeof(var) == typeof(null) -> var === null
        size_t start = left.find("typeof ");
        if (start != std::string::npos) {
            std::string var = left.substr(start + 7);
            return var + " === null";
        }
    }

    // Check if this is a Vector/Transform operation that needs method call conversion
    // In GDScript: v1 + v2, v1 - v2, v1 * v2, v1 / scalar
    // In TypeScript: v1.add(v2), v1.sub(v2), v1.mul(v2), v1.div(scalar)

    // Detect if left operand might be a vector/transform type (heuristic based)
    // Common patterns: Vector2(...), Vector3(...), Transform2D(...), Transform3D(...)
    // or variables that might contain them
    bool might_be_vector = false;
    for (const std::string& vector_type : {
        "Vector2(", "Vector3(", "Vector4(", "Vector2i(", "Vector3i(", "Vector4i(",
        "Transform2D(", "Transform3D(", "Basis(", "Quaternion(",
        "Rect2(", "Rect2i(", "AABB(", "Plane("
    }) {
        if (left.find(vector_type) != std::string::npos) {
            might_be_vector = true;
            break;
        }
    }

    // Also check for chained method calls that might return vectors
    if (left.find(".normalized(") != std::string::npos ||
        left.find(".rotated(") != std::string::npos ||
        left.find(".direction_to(") != std::string::npos) {
        might_be_vector = true;
    }

    // If this might be a vector operation and the operator is arithmetic, convert to method call
    if (might_be_vector && (op == "+" || op == "-" || op == "*" || op == "/")) {
        std::string method;
        if (op == "+") method = "add";
        else if (op == "-") method = "sub";
        else if (op == "*") method = "mul";
        else if (op == "/") method = "div";

        // Need to wrap left expression in parens if it's complex
        std::string left_wrapped = left;
        if (left.find(' ') != std::string::npos && left[0] != '(') {
            left_wrapped = "(" + left + ")";
        }

        return left_wrapped + "." + method + "(" + right + ")";
    }

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

    // Handle typeof() function calls - convert to TypeScript typeof or remove
    if (callee == "typeof") {
        if (!call->arguments.empty()) {
            std::string arg = transform_expression(call->arguments[0]);
            // Check if argument is null - typeof(null) should be removed or converted
            if (arg == "null") {
                // typeof(null) in GDScript checks doesn't make sense in TS
                // This should be handled by the calling context
                return "null";
            }
            // Regular typeof call
            return "typeof " + arg;
        }
        return "typeof undefined";
    }

    // Handle special Godot function calls

    // Handle signal.emit() -> $signal.emit()
    if (callee.find(".emit") != std::string::npos) {
        size_t dot_pos = callee.find_last_of('.');
        if (dot_pos != std::string::npos) {
            std::string signal_name = callee.substr(0, dot_pos);
            // If it's a direct signal reference (not already prefixed), add $
            if (signal_name.find("this.$") == std::string::npos &&
                signal_name.find('$') == std::string::npos &&
                signal_name.find('.') == std::string::npos) {
                callee = "this.$" + signal_name + ".emit";
            }
        }
    }

    // Handle signal.connect() -> $signal.connect()
    if (callee.find(".connect") != std::string::npos) {
        size_t dot_pos = callee.find_last_of('.');
        if (dot_pos != std::string::npos) {
            std::string signal_name = callee.substr(0, dot_pos);
            // If it's a direct signal reference (not already prefixed), add $
            if (signal_name.find("this.$") == std::string::npos &&
                signal_name.find('$') == std::string::npos &&
                signal_name.find('.') == std::string::npos) {
                callee = "this.$" + signal_name + ".connect";
            }
        }
    }

    // Handle preload() -> resource loading
    if (callee == "preload") {
        std::string args_str;
        if (!call->arguments.empty()) {
            args_str = transform_expression(call->arguments[0]);
        }
        return "preload(" + args_str + ")";
    }

    // Handle load() -> resource loading
    if (callee == "load") {
        std::string args_str;
        if (!call->arguments.empty()) {
            args_str = transform_expression(call->arguments[0]);
        }
        return "load(" + args_str + ")";
    }

    std::string args = "(";
    for (size_t i = 0; i < call->arguments.size(); ++i) {
        if (i > 0) args += ", ";
        args += transform_expression(call->arguments[i]);
    }
    args += ")";

    return callee + args;
}

std::string ASTTransformer::transform_attribute_expression(const ASTNodePtr& node) {
    if (!node) return "";

    // Check if we have children
    if (node->children.empty()) {
        // Fall back to text if no children
        return replace_keywords(node->text);
    }

    // Build the attribute access by joining all children with dots
    // Tree-sitter creates attribute nodes with all parts as separate children
    // For example: mod.UIAnchor.Center has 3 children: [mod, UIAnchor, Center]
    //              get_tree().create_timer(1.0).timeout has 3 children: [call node, call node, timeout]
    std::string result;
    for (size_t i = 0; i < node->children.size(); ++i) {
        if (i > 0) result += ".";
        result += transform_expression(node->children[i]);
    }

    // Note: self->this conversion is already handled by transform_identifier via replace_keywords
    // No need to check here since transform_expression already does the conversion

    return result;
}

std::string ASTTransformer::transform_subscript_expression(const ASTNodePtr& node) {
    if (!node || node->children.size() < 2) return "";

    std::string object = transform_expression(node->children[0]);
    std::string index = transform_expression(node->children[1]);

    return object + "[" + index + "]";
}

std::string ASTTransformer::transform_identifier(const ASTNodePtr& node) {
    auto ident = std::dynamic_pointer_cast<IdentifierExpr>(node);
    std::string name;
    if (ident) {
        name = ident->name;
    } else {
        name = node->text;
    }

    // Check for $NodePath shorthand syntax
    name = transform_node_path_shorthand(name);

    // Replace GDScript keywords (including standalone 'self')
    return replace_keywords(name);
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
    // Dictionary children are "pair" nodes, each containing key and value
    for (size_t i = 0; i < node->children.size(); ++i) {
        if (i > 0) result += ", ";

        auto pair = node->children[i];
        if (pair && pair->node_type_str == "pair" && pair->children.size() >= 2) {
            // First child is key, second is value
            result += transform_expression(pair->children[0]) + ": ";
            result += transform_expression(pair->children[1]);
        }
    }
    result += "}";
    return result;
}

std::string ASTTransformer::transform_get_node(const ASTNodePtr& node) {
    if (!node || node->children.empty()) return "";

    std::string path = transform_expression(node->children[0]);

    // If the path is a string literal, we can use it directly
    // Handle both $NodePath and get_node("NodePath") syntax
    return "this.get_node(" + path + ")";
}

// Helper function to detect and transform $NodePath syntax in expressions
std::string ASTTransformer::transform_node_path_shorthand(const std::string& identifier) {
    // Check if this is a $ prefixed node path (e.g., $Player, $UI/Label)
    if (!identifier.empty() && identifier[0] == '$') {
        // Extract the path (everything after $)
        std::string path = identifier.substr(1);

        // Handle special cases
        if (path.empty()) {
            // Just $ by itself is not valid
            return identifier;
        }

        // Handle relative paths (..) and absolute paths (/)
        // GDScript: $"../Player" or $"/root/Player"
        // These should be preserved in the path string

        // Convert to get_node call with string literal
        // Note: In ts2gd convention, we use get_node() method
        return "this.get_node(\"" + path + "\")";
    }
    return identifier;
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

    // Handle self. prefix in attribute accesses
    // This handles cases where attribute expressions fall through to default case
    if (text.find("self.") == 0) {
        return "this." + text.substr(5);  // Replace "self." with "this."
    }

    // Handle self in middle of expressions (e.g., "method(self.property)")
    std::string result = text;
    size_t pos = 0;
    while ((pos = result.find("self.", pos)) != std::string::npos) {
        result.replace(pos, 5, "this.");
        pos += 5;
    }

    // Handle standalone self (e.g., "return self")
    // Need to be careful not to replace in words like "yourself"
    pos = 0;
    while ((pos = result.find("self", pos)) != std::string::npos) {
        // Check if this is a standalone "self" (not part of another word)
        bool is_standalone = true;
        if (pos > 0) {
            char before = result[pos - 1];
            if (std::isalnum(before) || before == '_') {
                is_standalone = false;
            }
        }
        if (pos + 4 < result.length()) {
            char after = result[pos + 4];
            if (std::isalnum(after) || after == '_') {
                is_standalone = false;
            }
        }

        if (is_standalone) {
            result.replace(pos, 4, "this");
            pos += 4;
        } else {
            pos += 4;
        }
    }

    // Check if this is a known autoload/singleton
    // Common Godot autoloads like Global, GameManager, etc.
    // In TypeScript, these should be accessed the same way
    // For now, we'll preserve them as-is
    // Future enhancement: Could parse project.godot to detect autoloads
    // and add proper type annotations

    return result;
}

std::string ASTTransformer::transform_await_expression(const ASTNodePtr& node) {
    if (!node || node->children.empty()) return "";

    std::string expr = transform_expression(node->children[0]);

    // In GDScript 2.0 (Godot 4.x): await signal or await coroutine
    // In TypeScript (ts2gd convention): await this.$signal or yield this.$signal

    // Check if this is awaiting a signal
    // Pattern: await signal_name or await obj.signal_name
    if (expr.find("this.$") != std::string::npos || expr.find('$') != std::string::npos) {
        // Already formatted as signal, use yield for ts2gd compatibility
        return "yield " + expr;
    }

    // For signal references without $, add it
    if (expr.find('.') == std::string::npos && expr.find('(') == std::string::npos) {
        // Simple identifier - likely a signal
        return "yield this.$" + expr;
    }

    // Otherwise, treat as regular await (for async functions)
    return "await " + expr;
}

std::string ASTTransformer::transform_lambda_expression(const ASTNodePtr& node) {
    if (!node) return "";

    // GDScript lambda: func(x, y): return x + y
    // TypeScript arrow function: (x, y) => x + y

    // For now, return a placeholder
    // Full implementation would need to parse lambda parameters and body
    // This is a complex feature that depends on tree-sitter parsing

    if (node->has_children()) {
        // Try to extract parameters and body
        std::string params = "()";
        std::string body = "";

        // Simple heuristic: if we have children, transform them
        if (node->children.size() >= 2) {
            // First child might be params, second might be body
            params = transform_expression(node->children[0]);
            body = transform_expression(node->children[1]);
        } else if (node->children.size() == 1) {
            body = transform_expression(node->children[0]);
        }

        return params + " => " + body;
    }

    return "() => {}";
}

std::string ASTTransformer::transform_conditional_expression(const ASTNodePtr& node) {
    if (!node || node->children.size() < 3) return "";

    // GDScript ternary: x if condition else y
    // TypeScript ternary: condition ? x : y

    // Assuming children are: [condition, true_expr, false_expr]
    std::string condition = transform_expression(node->children[0]);
    std::string true_expr = transform_expression(node->children[1]);
    std::string false_expr = transform_expression(node->children[2]);

    return condition + " ? " + true_expr + " : " + false_expr;
}

void ASTTransformer::set_indent_style(const std::string& style) {
    // Would need to recreate generator with new style
}

void ASTTransformer::set_use_semicolons(bool use) {
    // Would need to recreate generator with new setting
}

} // namespace gd2ts
