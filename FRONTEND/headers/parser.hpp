#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"
#include "hash_table.hpp"
#include "colors.hpp"
#include "dump.hpp"


struct parser_t
{
    lexer_t*                     lexer;
    vec_t<ht_t<scope_elem_t*>*>* scopes;
    size_t                       cur_scope;
};

front_err_t parser_init(parser_t *parser, FILE *stream);
front_err_t parser_free(parser_t *parser);

size_t match_token(parser_t *parser, hash_t hash);
size_t check_token(parser_t *parser, hash_t hash);
token_t* consume_token(parser_t *parser, hash_t hash, const char *error_msg);
void parser_report_error(parser_t *parser, token_t *token, const char *msg);

node_t* parse_ast(parser_t *parser);
node_t* parse_func(parser_t *parser);
node_t* parse_params(parser_t *parser);
node_t* parse_stmt(parser_t *parser);
node_t* parse_expr(parser_t *parser);
node_t* parse_term(parser_t *parser);
node_t* parse_factor(parser_t *parser);
node_t* parse_primary(parser_t *parser);
node_t* parse_if(parser_t *parser);
node_t* parse_while(parser_t *parser);
node_t* parse_var_init(parser_t *parser);
node_t* parse_assignment(parser_t *parser);
node_t* parse_return(parser_t *parser);
node_t* parse_call(parser_t *parser);
node_t* parse_cond(parser_t *parser);
node_t* parse_block(parser_t *parser);
node_t* parse_var(parser_t *parser);
node_t* parse_num(parser_t *parser);

#define CUR_TOKEN (parser->lexer->tokens->data[parser->lexer->cur_token_idx])
#define CUR_TYPE  (CUR_TOKEN->type)
#define CUR_HASH  (CUR_TOKEN->hash)
#define CUR_START (CUR_TOKEN->start_ptr)
#define CUR_LEN   (CUR_TOKEN->len)
#define CUR_POS   (parser->lexer->cur_token_idx)

#define PREV_TOKEN (parser->lexer->tokens->data[parser->lexer->cur_token_idx - 1])
#define PREV_TYPE  (PREV_TOKEN->type)
#define PREV_HASH  (PREV_TOKEN->hash)
#define PREV_START (PREV_TOKEN->start_ptr)
#define PREV_LEN   (PREV_TOKEN->len)
#define PREV_POS   (parser->lexer->cur_token_idx - 1)

#define NEXT_TOKEN (parser->lexer->tokens->data[parser->lexer->cur_token_idx + 1])
#define NEXT_TYPE  (NEXT_TOKEN->type)
#define NEXT_HASH  (NEXT_TOKEN->hash)
#define NEXT_START (NEXT_TOKEN->start_ptr)
#define NEXT_LEN   (NEXT_TOKEN->len)
#define NEXT_POS   (parser->lexer->cur_token_idx + 1)

#define CUR_SCOPE      (parser->scopes->data[parser->cur_scope])
#define CUR_SCOPE_POS  (parser->cur_scope)
#define CUR_SCOPE_SIZE (parser->scopes->size)

struct op_t
{
	hash_t hash;
	calc_t calc;
	diff_t diff;
	char   name[8];
	int    num_args;
};

#endif