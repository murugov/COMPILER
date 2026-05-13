#ifndef LEXER_HPP
#define LEXER_HPP

#include "CONFIG.hpp"

#include "token.hpp"
#include "hash_func.hpp"
#include "HashOp.hpp"

enum front_err_t
{
    FRONT_SUCCESS = 0,
    FRONT_ERROR   = 1
};

struct lexer_t
{
    vec_t<token_t*>* tokens;
    size_t           cur_token_idx;
    token_t*         peeked_token;
    vec_t<char*>*    lines;
    size_t           count_lines;
    size_t           cur_line;
    size_t           cur_col;
    char*            src_ptr;
};


front_err_t lexer_init(lexer_t *lexer, vec_t<char*> *lines);
front_err_t lexer_free(lexer_t *lexer);

void advance_token(lexer_t *lexer);
token_t* next_token(lexer_t *lexer);
token_t* peek_token(lexer_t *lexer);

front_err_t read_lines(vec_t<char*> *lines, FILE *stream);
front_err_t free_lines(vec_t<char*> *lines);

#endif