#ifndef TYPE_MAPPER_H
#define TYPE_MAPPER_H

#include <string>
#include <map>

namespace gd2ts {

class TypeMapper {
private:
    std::map<std::string, std::string> type_map;

    void initialize_type_map();

public:
    TypeMapper();
    ~TypeMapper() = default;

    // Map GDScript type to TypeScript type
    std::string map_type(const std::string& gd_type) const;

    // Map GDScript type with hint for int/float distinction
    std::string map_type_with_hint(const std::string& gd_type, bool is_int_hint = false) const;

    // Check if type needs import
    bool needs_godot_import(const std::string& ts_type) const;

    // Get TypeScript type for literal value
    std::string infer_type_from_literal(const std::string& literal_value) const;
};

} // namespace gd2ts

#endif // TYPE_MAPPER_H
