#include "gd2ts_converter.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/dir_access.hpp>

#include "parser/gdscript_parser.h"
#include "parser/ast_builder.h"
#include "transformer/ast_transformer.h"

using namespace godot;

GD2TSConverter::GD2TSConverter() {
    // Initialize with default config
    config["inferTypes"] = true;
    config["strictTypes"] = false;
    config["preserveComments"] = true;
    config["verbose"] = false;
}

GD2TSConverter::~GD2TSConverter() {
}

void GD2TSConverter::_bind_methods() {
    // Bind main methods
    ClassDB::bind_method(D_METHOD("transpile_file", "gd_path", "ts_output_path"), &GD2TSConverter::transpile_file);
    ClassDB::bind_method(D_METHOD("transpile_directory", "root_path", "options"), &GD2TSConverter::transpile_directory);
    ClassDB::bind_method(D_METHOD("transpile_string", "gdscript_source"), &GD2TSConverter::transpile_string);
    ClassDB::bind_method(D_METHOD("get_version"), &GD2TSConverter::get_version);
    ClassDB::bind_method(D_METHOD("set_config", "config"), &GD2TSConverter::set_config);
    ClassDB::bind_method(D_METHOD("get_config"), &GD2TSConverter::get_config);
}

Dictionary GD2TSConverter::transpile_file(const String &gd_path, const String &ts_output_path) {
    Dictionary result;
    result["input_path"] = gd_path;
    result["output_path"] = ts_output_path;

    // Read the GDScript file
    Ref<FileAccess> file = FileAccess::open(gd_path, FileAccess::READ);
    if (file.is_null()) {
        result["success"] = false;
        result["error"] = "Failed to open file: " + gd_path;
        UtilityFunctions::push_error("GD2TSConverter: Failed to open file: ", gd_path);
        return result;
    }

    String gdscript_content = file->get_as_text();
    file->close();

    // Convert to std::string
    std::string gd_source = std::string(gdscript_content.utf8().get_data());

    // Parse the GDScript
    gd2ts::GDScriptParser parser;
    if (!parser.parse(gd_source)) {
        result["success"] = false;
        result["error"] = "Failed to parse GDScript: " + String(parser.get_error().c_str());
        UtilityFunctions::push_error("GD2TSConverter: Parse error: ", parser.get_error().c_str());
        return result;
    }

    // Build AST
    gd2ts::ASTBuilder ast_builder;
    auto ast = ast_builder.build_ast(parser.get_root_node(), gd_source);
    if (!ast) {
        result["success"] = false;
        result["error"] = "Failed to build AST";
        UtilityFunctions::push_error("GD2TSConverter: Failed to build AST");
        return result;
    }

    // Transform to TypeScript
    gd2ts::ASTTransformer transformer;
    std::string ts_output = transformer.transform(ast);

    // Write to output file
    Ref<FileAccess> out_file = FileAccess::open(ts_output_path, FileAccess::WRITE);
    if (out_file.is_null()) {
        result["success"] = false;
        result["error"] = "Failed to write output file: " + ts_output_path;
        UtilityFunctions::push_error("GD2TSConverter: Failed to write output: ", ts_output_path);
        return result;
    }

    out_file->store_string(String(ts_output.c_str()));
    out_file->close();

    result["success"] = true;
    result["message"] = "Successfully transpiled to: " + ts_output_path;
    UtilityFunctions::print("GD2TSConverter: Successfully transpiled ", gd_path, " -> ", ts_output_path);

    return result;
}

Array GD2TSConverter::transpile_directory(const String &root_path, const Dictionary &options) {
    Array results;

    // TODO: Implement directory traversal and transpilation
    // For now, return empty array
    UtilityFunctions::print("GD2TSConverter: transpile_directory called with: ", root_path);

    return results;
}

String GD2TSConverter::get_version() const {
    return "0.1.0-alpha";
}

void GD2TSConverter::set_config(const Dictionary &p_config) {
    // Merge provided config with existing config
    Array keys = p_config.keys();
    for (int i = 0; i < keys.size(); i++) {
        Variant key = keys[i];
        config[key] = p_config[key];
    }
}

Dictionary GD2TSConverter::get_config() const {
    return config;
}

String GD2TSConverter::transpile_string(const String &gdscript_source) {
    // Convert to std::string
    std::string gd_source = std::string(gdscript_source.utf8().get_data());

    // Parse the GDScript
    gd2ts::GDScriptParser parser;
    if (!parser.parse(gd_source)) {
        UtilityFunctions::push_error("GD2TSConverter: Parse error: ", parser.get_error().c_str());
        return String("// Parse error: ") + String(parser.get_error().c_str());
    }

    // Build AST
    gd2ts::ASTBuilder ast_builder;
    auto ast = ast_builder.build_ast(parser.get_root_node(), gd_source);
    if (!ast) {
        UtilityFunctions::push_error("GD2TSConverter: Failed to build AST");
        return "// Failed to build AST\n";
    }

    // Transform to TypeScript
    gd2ts::ASTTransformer transformer;
    std::string ts_output = transformer.transform(ast);

    return String(ts_output.c_str());
}
