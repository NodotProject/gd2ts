#ifndef GDSCRIPT_PARSER_H
#define GDSCRIPT_PARSER_H

#include <string>
#include <memory>

// Forward declarations for tree-sitter types
extern "C" {
    typedef struct TSParser TSParser;
    typedef struct TSTree TSTree;
    typedef struct TSNode TSNode;
    typedef struct TSLanguage TSLanguage;
}

namespace gd2ts {

class GDScriptParser {
private:
    TSParser* parser;
    TSTree* tree;
    const TSLanguage* language;

public:
    GDScriptParser();
    ~GDScriptParser();

    // Parse GDScript source code
    bool parse(const std::string& source);

    // Get the root node of the syntax tree
    TSNode get_root_node() const;

    // Clean up current tree
    void reset();

    // Check if tree is valid
    bool is_valid() const;

    // Get error information
    std::string get_error() const;
};

} // namespace gd2ts

#endif // GDSCRIPT_PARSER_H
