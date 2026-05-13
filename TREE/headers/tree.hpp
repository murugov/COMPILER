#ifndef TREE_HPP
#define TREE_HPP

#include "CONFIG.hpp"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "hash_func.hpp"


typedef int    arg_t;

enum tree_err_t
{
    TREE_SUCCESS = 0,
    TREE_ERROR   = 1
};

enum type_t
{
    ARG_LINK = 0x00,
    ARG_NUM  = 0x01,
    ARG_OP   = 0x02,
    ARG_VAR  = 0x03,
    ARG_FUNC = 0x04
};

union val
{
    char   link;
    int    num;
    hash_t op;
    char*  var;
    char*  func;
};

val valLINK(char l);
val valNUM(int n);
val valOP(hash_t o);
val valVAR(const char* ptr, size_t len);
val valFUNC(const char* ptr, size_t len);

struct node_t
{
    arg_t  type;
    val    item;
    node_t *parent;
    node_t *left;
    node_t *right;
};

struct tree_t
{
    node_t *root;
    size_t size;
    size_t cap;
};

struct keyword_t
{
    const char* name;
    size_t      len;
    hash_t      hash;
    const char* c_name;
    size_t      c_len;
    hash_t      c_hash;
};

tree_err_t tree_init(tree_t *tree);
tree_err_t tree_free(tree_t *tree);

node_t* node_new(type_t type, val item, node_t * __restrict left, node_t * __restrict right);
node_t* node_copy(node_t * __restrict node);

void set_parents(node_t *node, node_t *parent);
void node_purge(node_t *node);

#define LINK_(link)      node_new(ARG_LINK, valLINK(link), NULL, NULL)
#define NUM_(num)        node_new(ARG_NUM,  valNUM(num), NULL, NULL)
#define OP_(op)          node_new(ARG_OP,   valOP((hash_t)op), NULL, NULL)
#define VAR_(var, len)   node_new(ARG_VAR,  valVAR(var, len), NULL, NULL)
#define FUNC_(func, len) node_new(ARG_FUNC, valFUNC(func, len), NULL, NULL)


#endif