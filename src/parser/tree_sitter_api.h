#ifndef GD2TS_TREE_SITTER_API_H_
#define GD2TS_TREE_SITTER_API_H_

// Include the official tree-sitter API header
#include <tree_sitter/api.h>

// Declare the GDScript language function
#ifdef __cplusplus
extern "C" {
#endif

const TSLanguage *tree_sitter_gdscript(void);

#ifdef __cplusplus
}
#endif

#endif // GD2TS_TREE_SITTER_API_H_
