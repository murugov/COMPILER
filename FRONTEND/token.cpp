#include "token.hpp"


token_t *token_new(type_t type, hash_t hash, char *start_ptr, size_t len, size_t line, size_t col)
{
    token_t *token = (token_t*)calloc(1, sizeof(token_t));
    if (token == NULL) { return NULL; }

    token->type      = type;
    token->hash      = hash;
    token->start_ptr = start_ptr;
    token->len       = len;
    token->line      = line;
    token->col       = col;

    return token;
}


token_err_t token_free(token_t *token)
{
    token->type      = ARG_NUM;
    token->hash      = 0;
    token->start_ptr = NULL;
    token->len       = 0;
    token->line      = 0;
    token->col       = 0;  
    free(token);  

    return TOKEN_SUCCESS;
}