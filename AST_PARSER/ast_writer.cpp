#include "ast_parser.hpp"
#include "KeywordSet.cpp"

static int hash_keyword_cmp (const void *a, const void *b);


void ast_writer (node_t * __restrict node, FILE* stream)
{
    if (node == NULL) { fprintf (stream, " nil"); return; }

    fprintf (stream, "(");
    
    keyword_t* keyword = NULL;
    
    switch (node->type)
    {
        case ARG_LINK:
            fprintf (stream, "LINK: ");
            fprintf (stream, "\"@\" ");
            break;
        
        case ARG_NUM:
            fprintf (stream, "NUM: ");
            fprintf (stream, "\"%d\" ", node->item.num);
            break;

        case ARG_OP:
            fprintf (stream, "OP: ");
            keyword = (keyword_t*)bsearch(&(node->item.op), keyword_set, LEN_KEYWORD_SET, sizeof(keyword_t), hash_keyword_cmp);
            fprintf (stream, "\"%s\" ", keyword->c_name);
            break;
        
        case ARG_VAR:
            fprintf (stream, "VAR: ");
            fprintf (stream, "\"%s\" ", node->item.var);
            break;

        case ARG_FUNC:
            fprintf (stream, "FUNC: ");
            fprintf (stream, "\"%s\" ", node->item.func);
            break;

        default:
            fprintf (stream, "unknown: ");
            break;
    }

    ast_writer(node->left, stream);
    ast_writer(node->right, stream);

    fprintf (stream, ")");
}


static int hash_keyword_cmp(const void *key, const void *element) 
{
    hash_t a = *(const hash_t *)key;
    hash_t b = ((const keyword_t *)element)->hash;

    return (a > b) - (a < b);
}
