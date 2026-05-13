#include "lexer.hpp" 
#include "KeywordSet.cpp"

#define IS_END(lexer) (lexer->cur_line > lexer->count_lines)

static void skip_spaces(lexer_t *lexer);
static token_t *read_num(lexer_t *lexer);
static token_t *read_name(lexer_t *lexer);

static int hash_keyword_cmp(const void *key, const void *element);


front_err_t lexer_init(lexer_t *lexer, vec_t<char*> *lines)
{
    vec_t<token_t*> *tokens = (vec_t<token_t*>*)calloc(1, sizeof(vec_t<token_t*>));
    if (tokens == NULL) { return FRONT_ERROR; }

    vec_init(tokens, MIN_VEC_CAP);

    lexer->tokens        = tokens;
    lexer->cur_token_idx = 0; 
    lexer->peeked_token  = NULL;
    lexer->lines         = lines;
    lexer->count_lines   = lines->size;
    lexer->cur_line      = 1;
    lexer->cur_col       = 1;
    lexer->src_ptr       = lines->data[0]; 

    peek_token(lexer);
    while (lexer->peeked_token && lexer->peeked_token->hash != HASH_EOF)    
    {
        // printf("cur_type: [%d];   src_ptr: [%c];    cur_hash: [0x%lX];    token_len = [%zu];    token_line = [%zu];   token_col = [%zu];\n", lexer->peeked_token->type, *lexer->peeked_token->start, lexer->peeked_token->hash, lexer->peeked_token->len, lexer->peeked_token->line, lexer->peeked_token->col);
        advance_token(lexer);
        peek_token(lexer);
    }

    vec_push_back(lexer->tokens, lexer->peeked_token);
    
    ON_DEBUG( LOG(INFO, "Lexer is initialized"); )
    return FRONT_SUCCESS;
}

front_err_t lexer_free(lexer_t *lexer)
{    
    if (lexer == NULL) { return FRONT_SUCCESS; }

    if (lexer->tokens)
    {
        const size_t tokens_size = lexer->tokens->size;
        for (size_t i = 0; i < tokens_size; i++)
        {
            if (lexer->tokens->data[i]) { token_free(lexer->tokens->data[i]); }
        }
        
        vec_free(lexer->tokens);
        free(lexer->tokens);
        lexer->tokens = NULL;
    }
    
    lexer->cur_token_idx = 0;
    lexer->peeked_token  = NULL;

    free_lines(lexer->lines);
    vec_free(lexer->lines);
    lexer->count_lines = 0;

    lexer->cur_line     = 0;
    lexer->cur_col      = 0;
    lexer->src_ptr      = NULL;
    
    free(lexer);

    ON_DEBUG( LOG(INFO, "Lexer is released"); )
    return FRONT_SUCCESS;
}


token_t *peek_token(lexer_t *lexer)
{
    if (lexer->peeked_token == NULL)
    {
        
        char  *saved_ptr  = lexer->src_ptr;
        size_t saved_line = lexer->cur_line;
        size_t saved_col  = lexer->cur_col;
        
        skip_spaces(lexer);
        lexer->peeked_token = next_token(lexer);

        lexer->src_ptr  = saved_ptr;
        lexer->cur_line = saved_line;
        lexer->cur_col  = saved_col;
    }

    return lexer->peeked_token;
}

void advance_token(lexer_t *lexer)
{
    token_t *token = lexer->peeked_token;
    vec_push_back(lexer->tokens, token);
    
    lexer->src_ptr  = token->start_ptr + token->len;
    lexer->cur_line = token->line;
    lexer->cur_col  = token->col + token->len;
    
    lexer->peeked_token = NULL;
}

void skip_spaces(lexer_t *lexer)
{
    char **data_lines = lexer->lines->data;

    while (!(IS_END(lexer)))
    {
        while (isspace(*(lexer->src_ptr))) { (lexer->src_ptr)++;  (lexer->cur_col)++; }
        
        if (*(lexer->src_ptr) == '\0' || *(lexer->src_ptr) == '#')
        {
            (lexer->cur_line)++;
            lexer->cur_col = 1;

            if (!IS_END(lexer))
            {
                lexer->src_ptr = data_lines[lexer->cur_line - 1];
            } 
            else
            {
                return;
            }
        }
        else
        {
            return;
        }
    }
}


token_t* next_token(lexer_t *lexer)
{
    if (IS_END(lexer)) { return token_new(ARG_OP, HASH_EOF, lexer->src_ptr, 0, lexer->cur_line, lexer->cur_col); }
    
    token_t *token = read_num(lexer);
    if (token != NULL) { return token; }

    hash_t id_hash = hash_func_str(lexer->src_ptr);
    keyword_t *key = (keyword_t*)bsearch(&id_hash, keyword_set, LEN_KEYWORD_SET, sizeof(keyword_t), hash_keyword_cmp);
    if (key != NULL && strncmp(lexer->src_ptr, key->name, key->len) == 0)
    {
        return token_new(ARG_OP, id_hash, lexer->src_ptr, key->len, lexer->cur_line, lexer->cur_col);
    }

    if (isalpha(*(lexer->src_ptr))) { return read_name(lexer); }

    return token_new(ARG_OP, HASH_UNDEF, lexer->src_ptr, 1, lexer->cur_line, lexer->cur_col);
}


static token_t* read_num(lexer_t *lexer)
{
    char *end_ptr = NULL;
    strtod(lexer->src_ptr, &end_ptr);

    size_t len = (size_t)(end_ptr - lexer->src_ptr);
    if (len == 0) { return NULL; }
    
    return token_new(ARG_NUM, 0, lexer->src_ptr, len, lexer->cur_line, lexer->cur_col);

}

static token_t* read_name(lexer_t *lexer)
{
    char *start_ptr = lexer->src_ptr;

    while (isalnum(*lexer->src_ptr) || *lexer->src_ptr == '_') { (lexer->src_ptr)++; }
    
    const size_t len = (size_t)(lexer->src_ptr - start_ptr);
    
    token_t *token = peek_token(lexer);
    hash_t hash = hash_func_str(start_ptr);

    if (token && token->type == ARG_OP && token->hash == HASH_LPAREN)
    {
        return token_new(ARG_FUNC, hash, start_ptr, len, lexer->cur_line, lexer->cur_col);
    }
    
    return token_new(ARG_VAR, hash, start_ptr, len, lexer->cur_line, lexer->cur_col);
}


static int hash_keyword_cmp(const void *key, const void *element) 
{
    hash_t a = *(const hash_t *)key;
    hash_t b = ((const keyword_t *)element)->hash;

    return (a > b) - (a < b);
}