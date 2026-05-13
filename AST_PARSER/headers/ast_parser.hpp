#ifndef AST_PARSER_HPP
#define AST_PARSER_HPP

#include "tree.hpp"
#include "colors.hpp"

char* ast_reader(FILE *SourceFile);
node_t* node_reader(char ** __restrict cur_ptr, node_t * __restrict parent);
void ast_writer(node_t * __restrict node, FILE *stream);

#endif