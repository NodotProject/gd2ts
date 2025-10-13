#ifndef TS_GENERATOR_H
#define TS_GENERATOR_H

#include <string>
#include <sstream>
#include <vector>

namespace gd2ts {

class TSGenerator {
private:
    std::stringstream output;
    int indentation;
    bool at_line_start;
    std::string indent_string;  // "\t" or "    " etc
    bool use_semicolons;

    void write_indent();

public:
    TSGenerator(const std::string& indent_style = "\t", bool semicolons = true);
    ~TSGenerator() = default;

    // Core writing methods
    void write(const std::string& text);
    void write_line(const std::string& text = "");
    void new_line();

    // Indentation control
    void indent();
    void dedent();
    int get_indentation() const { return indentation; }
    void set_indentation(int level) { indentation = level; }

    // Get final output
    std::string get_output() const;
    void clear();

    // Convenience methods for common patterns
    void write_block_start(const std::string& prefix = "");  // writes "prefix {"
    void write_block_end(bool semicolon = false);  // writes "}"

    // Statement helpers
    void write_statement(const std::string& stmt);  // Adds semicolon if enabled
};

} // namespace gd2ts

#endif // TS_GENERATOR_H
