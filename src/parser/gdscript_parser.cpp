#include "gdscript_parser.h"
#include <cstring>

// Tree-sitter GDScript language
extern "C" {
    const TSLanguage *tree_sitter_gdscript(void);
}

namespace gd2ts {

GDScriptParser::GDScriptParser() : parser(nullptr), tree(nullptr), language(nullptr) {
    // Initialize tree-sitter parser
    parser = ts_parser_new();

    // Get GDScript language
    language = tree_sitter_gdscript();

    // Set the language
    if (parser && language) {
        ts_parser_set_language(parser, language);
    }
}

GDScriptParser::~GDScriptParser() {
    reset();

    if (parser) {
        ts_parser_delete(parser);
        parser = nullptr;
    }
}

bool GDScriptParser::parse(const std::string& source) {
    // Clean up previous tree
    reset();

    if (!parser || !language) {
        return false;
    }

    // Parse the source code
    tree = ts_parser_parse_string(
        parser,
        nullptr,  // old tree (for incremental parsing)
        source.c_str(),
        source.length()
    );

    return tree != nullptr;
}

TSNode GDScriptParser::get_root_node() const {
    if (tree) {
        return ts_tree_root_node(tree);
    }

    // Return null node if no tree
    TSNode null_node;
    memset(&null_node, 0, sizeof(TSNode));
    return null_node;
}

void GDScriptParser::reset() {
    if (tree) {
        ts_tree_delete(tree);
        tree = nullptr;
    }
}

bool GDScriptParser::is_valid() const {
    return tree != nullptr && parser != nullptr;
}

std::string GDScriptParser::get_error() const {
    if (!parser) {
        return "Parser not initialized";
    }
    if (!language) {
        return "GDScript language not loaded";
    }
    if (!tree) {
        return "No parse tree available";
    }

    TSNode root = ts_tree_root_node(tree);
    if (ts_node_has_error(root)) {
        return "Parse tree contains errors";
    }

    return "";
}

} // namespace gd2ts
