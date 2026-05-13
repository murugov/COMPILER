#include "parser.hpp"


front_err_t parser_init(parser_t *parser, FILE *stream)
{
    vec_t<char*> *lines = (vec_t<char*>*)calloc(1, sizeof(vec_t<char*>));
    if (lines == NULL) { return FRONT_ERROR; }

    vec_init(lines, MIN_VEC_CAP);
    read_lines(lines, stream);
    
    lexer_t *lexer = (lexer_t*)calloc(1, sizeof(lexer_t));
    if (lexer == NULL) { free_lines(lines); return FRONT_ERROR; }

    lexer_init(lexer, lines);
    parser->lexer = lexer;

    vec_t<ht_t<scope_elem_t*>*> *scopes = (vec_t<ht_t<scope_elem_t*>*>*)calloc(1, sizeof(vec_t<ht_t<scope_elem_t*>*>));
    if (scopes == NULL) { lexer_free(lexer); return FRONT_ERROR; }

    vec_init(scopes, MIN_VEC_CAP);

    parser->scopes    = scopes;
    parser->cur_scope = 0; 
    
    ht_t<scope_elem_t*> *global_scope = (ht_t<scope_elem_t*>*)calloc(HT_SIZE, sizeof(ht_t<scope_elem_t*>));
    if (global_scope == NULL) {vec_free(scopes); lexer_free(lexer); return FRONT_ERROR; }

    ht_init(global_scope);

    ON_DEBUG( LOG(DEBUG, "global_scope[%p]", global_scope); )

    vec_push_back(scopes, global_scope);
    CUR_SCOPE_POS++;

    ON_DEBUG( LOG(INFO, "Parser is initialized"); )
    return FRONT_SUCCESS;
}


front_err_t parser_free(parser_t *parser)
{
    if (parser == NULL) { return FRONT_SUCCESS; }

    lexer_free(parser->lexer);
    parser->lexer = NULL;

    if (parser->scopes)
    {
        for (size_t i = CUR_SCOPE_SIZE - 1; i > 0; i--)                 // CAREFULLY
        {
            ht_free(parser->scopes->data[i]);
        }
        
        vec_free(parser->scopes);
        free(parser->scopes);
        parser->scopes = NULL;
    }

    ON_DEBUG( LOG(INFO, "Parser is released"); )
    return FRONT_SUCCESS;
}


size_t match_token(parser_t *parser, hash_t hash)
{
    if (check_token(parser, hash))
    { 
        CUR_POS++; 
        return 1; 
    }
    return 0;
}


size_t check_token(parser_t *parser, hash_t hash)
{
    if (parser->lexer->cur_token_idx >= parser->lexer->tokens->size) { return 0; }
    
    return (CUR_HASH == hash);
}


token_t* consume_token(parser_t *parser, hash_t hash, const char *error_msg)
{
    if (check_token(parser, hash))
    {
        token_t *token = CUR_TOKEN;
        CUR_POS++;
        return token;
    }

    parser_report_error(parser, CUR_TOKEN, error_msg);
    return NULL;
}


void parser_report_error(parser_t *parser, token_t *token, const char *msg)
{    
    if (token)
    {
        printf(ANSI_COLOR_RED "Error at line %zu, column %zu: %s\n" ANSI_COLOR_RESET, parser->lexer->cur_line, parser->lexer->cur_col, msg);
        printf(ANSI_COLOR_RED "Token: type=%d, hash=0x%lX\n" ANSI_COLOR_RESET, token->type, token->hash);
    }
    else 
    {
        printf(ANSI_COLOR_RED "Error: %s\n" ANSI_COLOR_RESET, msg);
    }
}


node_t* parse_ast(parser_t *parser)
{
    node_t *program    = NULL;
    node_t *stmt       = NULL;
    node_t *last_stmt  = NULL;
    
    while (!check_token(parser, HASH_EOF))
    {
        if (CUR_TYPE == ARG_OP && CUR_HASH == HASH_DEF)
        {
            stmt = parse_func(parser);
        }
        else
        {
            stmt = parse_stmt(parser);
        }

        if (stmt == NULL) 
        { 
            node_purge(program);
            return NULL; 
        }

        if (last_stmt == NULL) 
        { 
            program = stmt; 
        }
        else 
        { 
            last_stmt->right = stmt; 
        }

        last_stmt = stmt;
    }

    set_parents(program, NULL);
    return program;
}


node_t* parse_func(parser_t *parser)
{
    match_token(parser, HASH_DEF);
    
    if (CUR_TYPE != ARG_FUNC)
    {
        parser_report_error(parser, CUR_TOKEN, "Expected function name after 'def'");
        return NULL;
    }
    
    ht_t<scope_elem_t*> *scope = (ht_t<scope_elem_t*>*)calloc(HT_SIZE, sizeof(ht_t<scope_elem_t*>));
    if (scope == NULL) { return NULL; }

    ht_init(scope);
    vec_push_back(parser->scopes, scope);

    CUR_SCOPE_POS++;

    node_t *func_node = FUNC_(CUR_START, CUR_LEN);
    if (func_node == NULL) { CUR_SCOPE_POS--; return NULL; }
    
    match_token(parser, CUR_HASH);
    
    node_t *params = parse_params(parser);
    
    node_t *body = parse_block(parser);
    if (body == NULL) 
    { 
        CUR_SCOPE_POS--;
        if (params) { node_purge(params); } 
        return NULL; 
    }

    node_t *def_node = OP_(HASH_DEF);
    if (def_node == NULL)
    {
        CUR_SCOPE_POS--;
        node_purge(func_node);
        node_purge(params);
        node_purge(body);
        return NULL;
    }

    node_t *link_node_1 = LINK_('@');
    if (link_node_1 == NULL)
    { 
        CUR_SCOPE_POS--;
        node_purge(func_node);
        node_purge(params);
        node_purge(body);
        node_purge(def_node);
        return NULL;
    }

    node_t *link_node_2 = LINK_('@');
    if (link_node_2 == NULL)
    { 
        CUR_SCOPE_POS--;
        node_purge(func_node);
        node_purge(params);
        node_purge(body);
        node_purge(def_node);
        node_purge(link_node_1);
        return NULL;
    }

    link_node_1->left  = def_node;
    def_node->left     = func_node;
    def_node->right    = link_node_2;
    link_node_2->left  = params;
    link_node_2->right = body;

    if (CUR_SCOPE) 
    { 
        scope_elem_t *func_elem = (scope_elem_t*)calloc(1, sizeof(scope_elem_t));
        if (func_elem == NULL)
        {
            CUR_SCOPE_POS--;
            node_purge(func_node);
            node_purge(params);
            node_purge(body);
            node_purge(link_node_1);
            node_purge(link_node_2);
            node_purge(def_node);
            return NULL; 
        }
        
        func_elem->name            = strndup(CUR_START, CUR_LEN);
        func_elem->type            = ARG_FUNC;
        func_elem->offset          = 0;
        func_elem->size_of_stk_frm = CUR_SCOPE_SIZE;

        ht_insert<scope_elem_t*, equal_func_scope>(CUR_SCOPE, func_elem, hash_func_scope); 
    }
    
    return link_node_1;
}


node_t* parse_params(parser_t *parser)
{
    if (!consume_token(parser, HASH_LPAREN, "Expected '(' after function name")) { return NULL; }
    
    node_t *params = NULL;
    
    if (check_token(parser, HASH_RPAREN))
    {
        match_token(parser, HASH_RPAREN);
        return NULL;
    }
    
    do {
        if (CUR_TYPE != ARG_VAR)
        {
            parser_report_error(parser, CUR_TOKEN, "Expected parameter name");
            if (params) { node_purge(params); }
            return NULL;
        }

        node_t *param = parse_var(parser);
        if (param == NULL) 
        { 
            if (params) { node_purge(params); }
            return NULL; 
        }

        if (params == NULL)
        {
            params = param;
        }
        else
        {
            node_t *comma_node = OP_(HASH_COMMA);
            if (comma_node == NULL)
            {
                node_purge(params);
                return NULL;
            }
            
            param->left = params;
            params = param;
        }
        
    } while (match_token(parser, HASH_COMMA));
    
    if (!consume_token(parser, HASH_RPAREN, "Expected ')' after parameters")) 
    { 
        node_purge(params); 
        return NULL; 
    }
    
    return params;
}


node_t* parse_stmt(parser_t *parser)
{
    node_t *stmt = NULL;

    if (CUR_TYPE == ARG_OP)
    {
        switch (CUR_HASH)
        {
            case HASH_IF:
                return parse_if(parser);
            case HASH_WHILE:
                return parse_while(parser);
            case HASH_RETURN:
                return parse_return(parser);
            case HASH_INIT:
                return parse_var_init(parser);
            default:
                break;
        }
    }

    if (CUR_TYPE == ARG_VAR)
    {
        if (CUR_POS + 1< (parser->lexer->tokens)->size &&                                  // CAREFULLY
            (parser->lexer->tokens)->data[CUR_POS + 1]->hash == HASH_EQ)
        {
            return parse_assignment(parser);
        }
    }
    
    if (CUR_TYPE == ARG_FUNC)
    {
        stmt = parse_call(parser);
        if (stmt)
        {
            node_t *semic_node = OP_(HASH_SEMICOLON);
            if (semic_node == NULL)
            {
                node_purge(stmt);
                return NULL;
            }
            
            consume_token(parser, HASH_SEMICOLON, "Expected ';' after function call");
            semic_node->left = stmt;
            return semic_node;
        }
    }
    
    stmt = parse_expr(parser);
    if (stmt)
    {
        node_t *semic_node = OP_(HASH_SEMICOLON);
        if (semic_node == NULL)
        {
            node_purge(stmt);
            return NULL;
        }
        
        consume_token(parser, HASH_SEMICOLON, "Expected ';' after expression");
        semic_node->left = stmt;
        return semic_node;
    }
    
    return NULL;
}


node_t* parse_expr(parser_t *parser)
{
    node_t *node = parse_term(parser);
    if (node == NULL) { return NULL; }
    
    while (match_token(parser, HASH_ADD) || match_token(parser, HASH_SUB))
    {
        hash_t op_hash = PREV_HASH;
        
        node_t *right_node = parse_term(parser);
        if (right_node == NULL)
        {
            node_purge(node);
            return NULL;
        }
        
        node_t *op_node = OP_(op_hash);
        if (op_node == NULL )
        {
            node_purge(node);
            node_purge(right_node);
            return NULL;
        }
        
        op_node->left  = node;
        op_node->right = right_node;
        node           = op_node;
    }
    
    return node;
}


node_t* parse_term(parser_t *parser)
{
    node_t *node = parse_factor(parser);
    if (node == NULL) { return NULL; }
    
    while (match_token(parser, HASH_MUL) || match_token(parser, HASH_DIV))
    {
        hash_t op_hash = PREV_HASH;
        
        node_t *right_node = parse_factor(parser);
        if (right_node == NULL)
        {
            node_purge(node);
            return NULL;
        }
        
        node_t *op_node = OP_(op_hash);
        if (op_node == NULL)
        {
            node_purge(node);
            node_purge(right_node);
            return NULL;
        }
        
        op_node->left  = node;
        op_node->right = right_node;
        node           = op_node;
    }
    
    return node;
}


node_t* parse_factor(parser_t *parser)
{
    node_t *node = parse_primary(parser);
    if (node == NULL) { return NULL; }

    while (match_token(parser, HASH_POW))
    {
        node_t *right_node = parse_primary(parser);
        if (right_node == NULL)
        {
            node_purge(node);
            return NULL;
        }
        
        node_t *op_node = OP_(HASH_POW);
        if (op_node == NULL)
        {
            node_purge(node);
            node_purge(right_node);
            return NULL;
        }
        
        op_node->left  = node;
        op_node->right = right_node;
        node           = op_node;
    }
    
    return node;
}


node_t* parse_primary(parser_t *parser)
{
    if (CUR_HASH == HASH_INIT) { return parse_var_init(parser); }

    switch (CUR_TYPE)
    {
        case ARG_NUM:
            return parse_num(parser);
        case ARG_VAR:
            return parse_var(parser);
        case ARG_FUNC:
            return parse_call(parser);
        case ARG_LINK:
        case ARG_OP:
        default:
            break;
    }

    if (match_token(parser, HASH_LPAREN))
    {
        node_t* expr = parse_expr(parser);
        if (expr == NULL) { return NULL; }
        
        if (!consume_token(parser, HASH_RPAREN, "Expected ')' after expression"))
        {
            node_purge(expr);
            return NULL;
        }
        
        return expr;
    }
    
    parser_report_error(parser, CUR_TOKEN, "Expected number, variable, function, or '('");
    return NULL;
}


node_t* parse_var_init(parser_t *parser)
{
    match_token(parser, HASH_INIT);
    
    if (CUR_TYPE != ARG_VAR)
    {
        parser_report_error(parser, CUR_TOKEN, "Expected variable name after 'init'");
        return NULL;
    }
    
    node_t *semic_node = OP_(HASH_SEMICOLON);
    if (semic_node == NULL) { return NULL; }

    node_t *assig_node = parse_assignment(parser);
    if (assig_node == NULL)
    {
        node_purge(semic_node);
        return NULL;
    }

    semic_node->left = assig_node;
    assig_node->item.op = HASH_INIT;
    return semic_node;
}


node_t* parse_assignment(parser_t *parser)
{
    if (CUR_TYPE != ARG_VAR)
    {
        parser_report_error(parser, CUR_TOKEN, "Expected variable name for assignment");
        return NULL;
    }

    node_t *var_node = VAR_(CUR_START, CUR_LEN);
    if (var_node == NULL) { return NULL; }

    if (CUR_SCOPE)
    { 
        scope_elem_t *var_elem = (scope_elem_t*)calloc(1, sizeof(scope_elem_t));
        if (var_elem == NULL)
        {
            node_purge(var_node);
            return NULL; 
        }
        
        var_elem->name            = strndup(CUR_START, (size_t)CUR_LEN);
        var_elem->type            = ARG_VAR;
        var_elem->offset          = CUR_SCOPE_SIZE;
        var_elem->size_of_stk_frm = 0;

        if(PREV_HASH == HASH_INIT && ht_find<scope_elem_t*, equal_func_scope>(CUR_SCOPE, var_elem, hash_func_scope))
        {
            node_purge(var_node);
            free(var_elem);
            parser_report_error(parser, CUR_TOKEN, "Redeclarated variable");
            return NULL;
        }
        else if(PREV_HASH != HASH_INIT && !(ht_find<scope_elem_t*, equal_func_scope>(CUR_SCOPE, var_elem, hash_func_scope)))
        {
            node_purge(var_node);
            free(var_elem);
            parser_report_error(parser, CUR_TOKEN, "Undeclarated variable");
            return NULL;
        }

        ht_insert<scope_elem_t*, equal_func_scope>(CUR_SCOPE, var_elem, hash_func_scope);
    }

    match_token(parser, CUR_HASH);
    
    if (!consume_token(parser, HASH_EQ, "Expected '=' after variable name"))
    {
        node_purge(var_node);
        return NULL;
    }
    
    node_t *value = parse_expr(parser);
    if (value == NULL)
    {
        node_purge(var_node);
        return NULL;
    }
    
    if (!consume_token(parser, HASH_SEMICOLON, "Expected ';' after assignment")) 
    { 
        node_purge(var_node);
        node_purge(value); 
        return NULL; 
    }
    
    node_t *semic_node = OP_(HASH_SEMICOLON);
    if (semic_node == NULL)
    {
        node_purge(var_node);
        node_purge(value);
        return NULL;
    }
    
    node_t *assign_node = OP_(HASH_EQ);
    if (assign_node == NULL)
    {
        node_purge(value);
        node_purge(semic_node);
        return NULL;
    }
    
    assign_node->left  = var_node;
    assign_node->right = value;

    semic_node->left = assign_node;
    
    return semic_node;
}


node_t* parse_if(parser_t *parser)
{
    match_token(parser, HASH_IF);
    
    if (!consume_token(parser, HASH_LPAREN, "Expected '(' after 'if'")) { return NULL; }
    
    node_t *if_node = OP_(HASH_IF);
    if (if_node == NULL) { return NULL; }

    node_t *cond = parse_cond(parser);
    if (cond == NULL)
    {
        node_purge(if_node);
        return NULL;
    }
    
    if (!consume_token(parser, HASH_RPAREN, "Expected ')' after cond"))
    {
        node_purge(if_node);
        node_purge(cond);
        return NULL;
    }
    
    node_t *if_body = parse_block(parser);
    if (if_body == NULL)
    {
        node_purge(if_node);
        node_purge(cond);
        return NULL;
    }

    if_node->left = cond;
    
    node_t *else_body = NULL;
    node_t *else_node = NULL;

    if (check_token(parser, HASH_ELSE))
    {
        match_token(parser, HASH_ELSE);
        
        else_body = parse_block(parser);
        if (else_body == NULL)
        {
            node_purge(if_node);
            node_purge(cond);
            node_purge(if_body);
            return NULL;
        }
        
        else_node = OP_(HASH_ELSE);
        if (else_node == NULL)
        {
            node_purge(if_node);
            node_purge(cond);
            node_purge(if_body);
            node_purge(else_body);
            return NULL;
        }
        
        if_node->right   = else_node;
        else_node->left  = if_body;
        else_node->right = else_body;
    }
    else
    {
        if_node->right = if_body;
    }

    node_t *link_node = LINK_('@');
    if (link_node == NULL) 
    {
        node_purge(if_node);
        node_purge(cond);
        node_purge(if_body);

        if (else_body) { node_purge(else_body); }
        if (else_node) { node_purge(else_node); }

        return NULL; 
    }
    
    link_node->left = if_node;

    return link_node;
}


node_t* parse_while(parser_t *parser)
{
    match_token(parser, HASH_WHILE);
    
    if (!consume_token(parser, HASH_LPAREN, "Expected '(' after 'while'")) { return NULL; }

    node_t *cond = parse_cond(parser);
    if (cond == NULL) { return NULL; }
    
    if (!consume_token(parser, HASH_RPAREN, "Expected ')' after cond"))
    { 
        node_purge(cond);
        return NULL;
    }
    
    node_t *body = parse_block(parser);
    if (body == NULL)
    {
        node_purge(cond);
        return NULL;
    }
    
    node_t* while_node = OP_(HASH_WHILE);
    if (while_node == NULL)
    {
        node_purge(cond);
        node_purge(body);
        return NULL;
    }
    
    while_node->left  = cond;
    while_node->right = body;

    node_t *link_node = LINK_('@');
    if (link_node == NULL)
    {
        node_purge(body);
        node_purge(cond);
        node_purge(while_node);
        return NULL;
    }
    
    link_node->left = while_node;
    
    return link_node;
}


node_t* parse_return(parser_t *parser)
{
    match_token(parser, HASH_RETURN);
    
    node_t *val = parse_expr(parser);
    if (val == NULL) { return NULL; }
    
    if (!consume_token(parser, HASH_SEMICOLON, "Expected ';' after return")) 
    { 
        node_purge(val); 
        return NULL; 
    }
    
    node_t *return_node = OP_(HASH_RETURN);
    if (return_node == NULL)
    {
        node_purge(val);
        return NULL;
    }
    
    return_node->left = val;
    
    return return_node;
}


node_t* parse_call(parser_t *parser)
{
    if (CUR_TYPE != ARG_FUNC)
    {
        parser_report_error(parser, CUR_TOKEN, "Expected function name");
        return NULL;
    }
    
    token_t *func_token = CUR_TOKEN;
    match_token(parser, func_token->hash);
    
    if (!consume_token(parser, HASH_LPAREN, "Expected '(' after function name")) { return NULL; }
    
    node_t *args = NULL;
    
    if (!match_token(parser, HASH_RPAREN))
    {
        node_t *arg = parse_expr(parser);
        if (arg == NULL) { return NULL; }
        
        args = arg;
        
        while (match_token(parser, HASH_COMMA))
        {
            node_t *next_arg = parse_expr(parser);
            if (next_arg == NULL) 
            { 
                node_purge(args); 
                return NULL; 
            }
            
            node_t *comma_node = OP_(HASH_COMMA);
            if (comma_node == NULL)
            {
                node_purge(next_arg);
                node_purge(args);
                return NULL;
            }
            
            comma_node->left  = args;
            comma_node->right = next_arg;
            args = comma_node;
        }
        
        if (!consume_token(parser, HASH_RPAREN, "Expected ')' after arguments")) 
        { 
            node_purge(args); 
            return NULL; 
        }
    }
    
    node_t *call_node = FUNC_(func_token->start_ptr, func_token->len);
    if (call_node == NULL) 
    { 
        node_purge(args); 
        return NULL; 
    }
    
    call_node->left = args;
    
    return call_node;
}


node_t* parse_cond(parser_t *parser)
{
    node_t *left_node = parse_expr(parser);
    if (left_node == NULL) { return NULL; }
    
    if (check_token(parser, HASH_LT)   ||
        check_token(parser, HASH_GT)   ||
        check_token(parser, HASH_LE)   ||
        check_token(parser, HASH_GE)   ||
        check_token(parser, HASH_EQEQ) ||
        check_token(parser, HASH_NE))
    {
        hash_t op_hash = CUR_HASH;
        match_token(parser, op_hash);
        
        node_t *right_node = parse_expr(parser);
        if (right_node == NULL)
        {
            node_purge(left_node);
            return NULL;
        }
        
        node_t *cmp_node = OP_(op_hash);
        if (cmp_node == NULL)
        {
            node_purge(left_node);
            node_purge(right_node);
            return NULL;
        }
        
        cmp_node->left  = left_node;
        cmp_node->right = right_node;

        return cmp_node;
    }
    
    return left_node;
}


node_t* parse_block(parser_t *parser)
{
    if (!consume_token(parser, HASH_LBRACE, "Expected '{'")) { return NULL; }
    
    node_t *first_stmt = NULL;
    node_t *last_stmt  = NULL;
    
    while (!check_token(parser, HASH_RBRACE) && !check_token(parser, HASH_EOF))
    {
        node_t *stmt = parse_stmt(parser);
        
        if (stmt == NULL) 
        {
            if (first_stmt) { node_purge(first_stmt); }
            return NULL;
        }
        
        if (first_stmt == NULL)
        {
            first_stmt = stmt;
            last_stmt = stmt;
        }
        else
        {
            last_stmt->right = stmt;
            last_stmt = stmt;
        }
    }
    
    if (!consume_token(parser, HASH_RBRACE, "Expected '}'"))
    {
        if (first_stmt) { node_purge(first_stmt); }
        return NULL;
    }
    
    return first_stmt;
}


node_t* parse_var(parser_t *parser)
{
    if (CUR_TYPE != ARG_VAR)
    {
        parser_report_error(parser, CUR_TOKEN, "Expected variable");
        return NULL;
    }
    
    node_t *var_node = VAR_(CUR_START, CUR_LEN);

    if (CUR_SCOPE)
    { 
        scope_elem_t *var_elem = (scope_elem_t*)calloc(1, sizeof(scope_elem_t));
        if (var_elem == NULL)
        {
            node_purge(var_node);
            return NULL; 
        }
        
        var_elem->name            = strndup(CUR_START, (size_t)CUR_LEN);
        var_elem->type            = ARG_VAR;
        var_elem->offset          = CUR_SCOPE_SIZE;
        var_elem->size_of_stk_frm = 0;

        if(CUR_POS != 0 && PREV_HASH == HASH_INIT && ht_find<scope_elem_t*, equal_func_scope>(CUR_SCOPE, var_elem, hash_func_scope))
        {
            node_purge(var_node);
            free(var_elem);
            parser_report_error(parser, CUR_TOKEN, "Redeclarated variable");
            return NULL;
        }
        else if(CUR_POS != 0 && PREV_HASH == HASH_COMMA && ht_find<scope_elem_t*, equal_func_scope>(CUR_SCOPE, var_elem, hash_func_scope))
        {
            node_purge(var_node);
            free(var_elem);
            parser_report_error(parser, CUR_TOKEN, "Redeclarated variable");
            return NULL;
        }
        ht_insert<scope_elem_t*, equal_func_scope>(CUR_SCOPE, var_elem, hash_func_scope);
    }
    
    match_token(parser, CUR_HASH);

    return var_node;
}


node_t* parse_num(parser_t *parser)
{
    if (CUR_TYPE != ARG_NUM)
    {
        parser_report_error(parser, CUR_TOKEN, "Expected number");
        return NULL;
    }
    
    token_t *num_token = CUR_TOKEN;
    match_token(parser, CUR_HASH);
    
    int val = 0;
    sscanf(num_token->start_ptr, "%d", &val);
    
    return NUM_(val);
}