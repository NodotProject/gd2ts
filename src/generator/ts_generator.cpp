#include "ts_generator.h"

namespace gd2ts {

TSGenerator::TSGenerator(const std::string& indent_style, bool semicolons)
    : indentation(0), at_line_start(true), indent_string(indent_style),
      use_semicolons(semicolons) {
}

void TSGenerator::write_indent() {
    if (at_line_start && indentation > 0) {
        for (int i = 0; i < indentation; ++i) {
            output << indent_string;
        }
        at_line_start = false;
    }
}

void TSGenerator::write(const std::string& text) {
    if (text.empty()) return;
    write_indent();
    output << text;
}

void TSGenerator::write_line(const std::string& text) {
    write(text);
    new_line();
}

void TSGenerator::new_line() {
    output << "\n";
    at_line_start = true;
}

void TSGenerator::indent() {
    indentation++;
}

void TSGenerator::dedent() {
    if (indentation > 0) {
        indentation--;
    }
}

std::string TSGenerator::get_output() const {
    return output.str();
}

void TSGenerator::clear() {
    output.str("");
    output.clear();
    indentation = 0;
    at_line_start = true;
}

void TSGenerator::write_block_start(const std::string& prefix) {
    if (!prefix.empty()) {
        write(prefix + " ");
    }
    write_line("{");
    indent();
}

void TSGenerator::write_block_end(bool semicolon) {
    dedent();
    if (semicolon && use_semicolons) {
        write_line("};");
    } else {
        write_line("}");
    }
}

void TSGenerator::write_statement(const std::string& stmt) {
    write(stmt);
    if (use_semicolons) {
        write(";");
    }
    new_line();
}

} // namespace gd2ts
