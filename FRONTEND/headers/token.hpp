#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "vector.hpp"
#include "tree.hpp"

enum token_err_t
{
    TOKEN_SUCCESS = 0,
    TOKEN_ERROR   = 1
};

struct token_t
{
    hash_t hash;
    char*  start_ptr;
    type_t type;
    size_t len;
    size_t line;
    size_t col;
};

token_t *token_new(type_t type, hash_t hash, char *start_ptr, size_t len, size_t line, size_t col);
token_err_t token_free(token_t *token);


#endif