#ifndef TREE_SITTER_API_H_
#define TREE_SITTER_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef uint16_t TSSymbol;
typedef uint16_t TSFieldId;
typedef struct TSLanguage TSLanguage;
typedef struct TSParser TSParser;
typedef struct TSTree TSTree;

typedef struct {
  uint32_t row;
  uint32_t column;
} TSPoint;

typedef struct {
  uint32_t context[4];
  const void *id;
  const TSTree *tree;
} TSNode;

typedef struct {
  TSPoint start_point;
  TSPoint end_point;
  uint32_t start_byte;
  uint32_t end_byte;
} TSRange;

// Parser functions
TSParser *ts_parser_new(void);
void ts_parser_delete(TSParser *self);
bool ts_parser_set_language(TSParser *self, const TSLanguage *language);
const TSLanguage *ts_parser_language(const TSParser *self);
TSTree *ts_parser_parse_string(
  TSParser *self,
  const TSTree *old_tree,
  const char *string,
  uint32_t length
);

// Tree functions
void ts_tree_delete(TSTree *self);
TSNode ts_tree_root_node(const TSTree *self);

// Node functions
const char *ts_node_type(TSNode self);
TSSymbol ts_node_symbol(TSNode self);
uint32_t ts_node_start_byte(TSNode self);
uint32_t ts_node_end_byte(TSNode self);
TSPoint ts_node_start_point(TSNode self);
TSPoint ts_node_end_point(TSNode self);
uint32_t ts_node_child_count(TSNode self);
TSNode ts_node_child(TSNode self, uint32_t child_index);
TSNode ts_node_named_child(TSNode self, uint32_t child_index);
uint32_t ts_node_named_child_count(TSNode self);
TSNode ts_node_next_sibling(TSNode self);
TSNode ts_node_prev_sibling(TSNode self);
TSNode ts_node_next_named_sibling(TSNode self);
TSNode ts_node_prev_named_sibling(TSNode self);
TSNode ts_node_parent(TSNode self);
bool ts_node_is_null(TSNode self);
bool ts_node_is_named(TSNode self);
bool ts_node_is_missing(TSNode self);
bool ts_node_is_extra(TSNode self);
bool ts_node_has_changes(TSNode self);
bool ts_node_has_error(TSNode self);
char *ts_node_string(TSNode self);

#ifdef __cplusplus
}
#endif

#endif // TREE_SITTER_API_H_
