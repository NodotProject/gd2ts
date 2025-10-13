#include "type_mapper.h"
#include <algorithm>
#include <cctype>

namespace gd2ts {

TypeMapper::TypeMapper() {
    initialize_type_map();
}

void TypeMapper::initialize_type_map() {
    // Basic types
    type_map["int"] = "number";
    type_map["float"] = "number";
    type_map["String"] = "string";
    type_map["bool"] = "boolean";
    type_map["void"] = "void";
    type_map["Variant"] = "any";

    // Collections
    type_map["Array"] = "Array<any>";
    type_map["Dictionary"] = "Record<string, any>";
    type_map["PackedByteArray"] = "Uint8Array";
    type_map["PackedInt32Array"] = "Int32Array";
    type_map["PackedInt64Array"] = "BigInt64Array";
    type_map["PackedFloat32Array"] = "Float32Array";
    type_map["PackedFloat64Array"] = "Float64Array";
    type_map["PackedStringArray"] = "string[]";
    type_map["PackedVector2Array"] = "Vector2[]";
    type_map["PackedVector3Array"] = "Vector3[]";
    type_map["PackedColorArray"] = "Color[]";

    // Math types (these will use Godot type definitions)
    type_map["Vector2"] = "Vector2";
    type_map["Vector2i"] = "Vector2i";
    type_map["Vector3"] = "Vector3";
    type_map["Vector3i"] = "Vector3i";
    type_map["Vector4"] = "Vector4";
    type_map["Vector4i"] = "Vector4i";
    type_map["Rect2"] = "Rect2";
    type_map["Rect2i"] = "Rect2i";
    type_map["Transform2D"] = "Transform2D";
    type_map["Transform3D"] = "Transform3D";
    type_map["Plane"] = "Plane";
    type_map["Quaternion"] = "Quaternion";
    type_map["AABB"] = "AABB";
    type_map["Basis"] = "Basis";
    type_map["Color"] = "Color";

    // Other common types
    type_map["NodePath"] = "NodePath";
    type_map["RID"] = "RID";
    type_map["Callable"] = "Callable";
    type_map["Signal"] = "Signal";

    // Node types (keep as-is, they exist in Godot type definitions)
    // We don't need to map every single node type, unknown types pass through
}

std::string TypeMapper::map_type(const std::string& gd_type) const {
    if (gd_type.empty()) {
        return "any";
    }

    // Check if it's in our map
    auto it = type_map.find(gd_type);
    if (it != type_map.end()) {
        return it->second;
    }

    // Check for typed arrays like Array[int]
    if (gd_type.find("Array[") == 0) {
        size_t start = gd_type.find('[');
        size_t end = gd_type.find(']');
        if (start != std::string::npos && end != std::string::npos && end > start) {
            std::string inner_type = gd_type.substr(start + 1, end - start - 1);
            return "Array<" + map_type(inner_type) + ">";
        }
    }

    // Check for Dictionary[K, V]
    if (gd_type.find("Dictionary[") == 0) {
        size_t start = gd_type.find('[');
        size_t comma = gd_type.find(',');
        size_t end = gd_type.find(']');
        if (start != std::string::npos && comma != std::string::npos &&
            end != std::string::npos && comma > start && end > comma) {
            std::string key_type = gd_type.substr(start + 1, comma - start - 1);
            std::string val_type = gd_type.substr(comma + 1, end - comma - 1);
            // Trim whitespace
            key_type.erase(0, key_type.find_first_not_of(" \t"));
            key_type.erase(key_type.find_last_not_of(" \t") + 1);
            val_type.erase(0, val_type.find_first_not_of(" \t"));
            val_type.erase(val_type.find_last_not_of(" \t") + 1);
            return "Record<" + map_type(key_type) + ", " + map_type(val_type) + ">";
        }
    }

    // Unknown type - likely a class name, pass through as-is
    // (Will be a Godot class or user-defined class)
    return gd_type;
}

std::string TypeMapper::map_type_with_hint(const std::string& gd_type, bool is_int_hint) const {
    std::string base_type = map_type(gd_type);

    // Add comment hint for int types
    if (base_type == "number" && is_int_hint) {
        return "number /* int */";
    }

    return base_type;
}

bool TypeMapper::needs_godot_import(const std::string& ts_type) const {
    // Check if this is a Godot-specific type that needs import
    // For now, we'll check common ones
    if (ts_type == "number" || ts_type == "string" || ts_type == "boolean" ||
        ts_type == "void" || ts_type == "any") {
        return false;
    }

    if (ts_type.find("Array<") == 0 || ts_type.find("Record<") == 0) {
        return false;
    }

    // Typed arrays
    if (ts_type.find("Array") != std::string::npos ||
        ts_type.find("Int8Array") != std::string::npos ||
        ts_type.find("Uint8Array") != std::string::npos) {
        return false;
    }

    // Everything else likely needs Godot import
    return true;
}

std::string TypeMapper::infer_type_from_literal(const std::string& literal_value) const {
    if (literal_value.empty()) {
        return "any";
    }

    // Check for string literals
    if (literal_value[0] == '"' || literal_value[0] == '\'') {
        return "string";
    }

    // Check for boolean
    if (literal_value == "true" || literal_value == "false") {
        return "boolean";
    }

    // Check for null
    if (literal_value == "null") {
        return "null";
    }

    // Check for numbers
    bool has_dot = literal_value.find('.') != std::string::npos;
    bool all_numeric = true;
    for (char c : literal_value) {
        if (!std::isdigit(c) && c != '.' && c != '-' && c != '+' &&
            c != 'e' && c != 'E' && c != 'x' && c != 'X') {
            all_numeric = false;
            break;
        }
    }

    if (all_numeric) {
        return "number";
    }

    // Check for array literal
    if (literal_value[0] == '[') {
        return "Array<any>";
    }

    // Check for dictionary literal
    if (literal_value[0] == '{') {
        return "Record<string, any>";
    }

    return "any";
}

} // namespace gd2ts
