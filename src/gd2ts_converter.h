#ifndef GD2TS_CONVERTER_H
#define GD2TS_CONVERTER_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>

namespace godot {

class GD2TSConverter : public RefCounted {
    GDCLASS(GD2TSConverter, RefCounted)

private:
    Dictionary config;

protected:
    static void _bind_methods();

public:
    GD2TSConverter();
    ~GD2TSConverter();

    // Main API methods
    Dictionary transpile_file(const String &gd_path, const String &ts_output_path);
    Array transpile_directory(const String &root_path, const Dictionary &options);
    String get_version() const;
    void set_config(const Dictionary &p_config);
    Dictionary get_config() const;

    // Helper methods
    String transpile_string(const String &gdscript_source);
};

} // namespace godot

#endif // GD2TS_CONVERTER_H
