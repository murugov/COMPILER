#ifndef DUMP_HPP
#define DUMP_HPP

#include "CONFIG.hpp"

#include "tree.hpp"
#include "stack.hpp"
#include "is_bad_ptr.hpp"

enum dump_err_t
{
    GEN_SUCCESS = 0,
    GEN_ERROR   = 1
};

dump_err_t dump_html(const char *title);
dump_err_t dump_tree(node_t *node, const char *func);
dump_err_t gen_dot(FILE *src, tree_t *tree, const char *func);

#endif