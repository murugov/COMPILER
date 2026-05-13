#include "tree.hpp"


tree_err_t tree_init(tree_t * __restrict tree)
{
    tree->root = NULL;
    tree->size = 0;
    tree->cap  = 0;

    return TREE_SUCCESS;
}

node_t* node_new(type_t type, val item, node_t * __restrict left, node_t * __restrict right)
{
    if (type != ARG_LINK && type != ARG_NUM && type != ARG_OP
        && type != ARG_VAR && type != ARG_FUNC)
    {
        return NULL;
    }

    node_t *node = (node_t*)calloc(1, sizeof(node_t));
    if (node == NULL) { return NULL; }

    node->item   = item;
    node->type   = type;
    node->parent = NULL;
    node->left   = left;
    node->right  = right;

    return node;
}

val valLINK(char l)  { val v; v.link = l; return v; }
val valNUM(int n) { val v; v.num = n; return v; }
val valOP(hash_t o)  { val v; v.op = o; return v; }
val valVAR(const char* ptr, size_t len)  { val v; v.var  = strndup(ptr, len); return v; }
val valFUNC(const char* ptr, size_t len) { val f; f.func = strndup(ptr, len); return f; }

node_t* node_copy(node_t * __restrict node)
{
    if (node == NULL) { return NULL; }

    node_t *new_node = (node_t*)calloc(1, sizeof(node_t));
    if (new_node == NULL) { return NULL; }

    new_node->type = node->type;
    new_node->parent = NULL;
    new_node->item = node->item;

    new_node->left  = node_copy(node->left);
    new_node->right = node_copy(node->right);

    return new_node;
}

void set_parents(node_t *node, node_t *parent)
{
    if (node == NULL) { return; }
    
    node->parent = parent;
    set_parents(node->left, node);
    set_parents(node->right, node);
}

tree_err_t tree_free(tree_t *tree)
{    
    if (tree->root != NULL) { node_purge(tree->root); }
    return TREE_SUCCESS;
}

void node_purge(node_t *node)
{
    if (node == NULL) { return; }

    if (node->left)  { node_purge(node->left); }
    if (node->right) { node_purge(node->right); }
    
    switch (node->type)
    {
        case ARG_VAR:
            if (node->item.var)
            {
                free(node->item.var);
                node->item.var = NULL;
            }
            break;
            
        case ARG_FUNC:
            if (node->item.func)
            {
                free(node->item.func);
                node->item.func = NULL;
            }
            break;
        
        case ARG_LINK:
        case ARG_NUM:
        case ARG_OP:
            break;
            
        default:
            ON_DEBUG( LOG(ERROR, "Unknown node type: %d", node->type); )
            break;
    }
    
    free(node);
    
    return;
}