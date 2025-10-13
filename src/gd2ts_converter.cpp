#include "gd2ts_converter.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

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

    // TODO: Implement actual transpilation
    // For now, return a placeholder result
    result["success"] = false;
    result["error"] = "Not yet implemented";
    result["input_path"] = gd_path;
    result["output_path"] = ts_output_path;

    UtilityFunctions::print("GD2TSConverter: transpile_file called with: ", gd_path);

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
    // TODO: Implement string transpilation
    // For now, return placeholder
    UtilityFunctions::print("GD2TSConverter: transpile_string called with ", gdscript_source.length(), " characters");
    return "// TypeScript output will appear here\n";
}
