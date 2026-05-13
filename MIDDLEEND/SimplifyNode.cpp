#include "simplify.hpp"
#include "DSL.hpp"
#include "OpInstrSet.cpp"
#include "KeywordSet.cpp"


static int global_number_changes = 0;

static double ConstFold(node_t *node);
static node_t *RemoveNeutralElem(node_t *node);
static void ReplaceNode(node_t *old_node, node_t *new_node);
static node_t *SwapParentAndChild(node_t *parent, node_t *child);
static int CmpForBinSearch(const void *a, const void *b);


void SimplifyNode(node_t* node)
{
    if (!node) return;
    
    int changes;
    do {
        changes = global_number_changes;
        ConstFold(node);
        RemoveNeutralElem(node);
    } while (changes != global_number_changes);
}


static double ConstFold(node_t *node)
{
    if (!node) return NAN;
    
    switch (node->type) 
    {
        case ARG_LINK:
        {
            double left_val  = ConstFold(node->left);
            double right_val = ConstFold(node->right);
            
            if (!isnan(left_val) && !isnan(right_val)) 
            {
                node_t* num_node = NUM_(left_val);
                ReplaceNode(node, num_node);
                return left_val;
            }
            return NAN;
        }

        case ARG_NUM:
            return node->item.num;

        case ARG_OP:
        {
            op_t* found = (op_t*)bsearch(&node->item.op, op_instr_set, LEN_INSTR_SET, sizeof(op_instr_set[0]), CmpForBinSearch);
            
            if (found)
            {
                double left_val  = ConstFold(node->left);
                double right_val = ConstFold(node->right);

                if (!isnan(left_val) && !isnan(right_val)) 
                {
                    calc_context context = {left_val, right_val};
                    double result = found->calc(&context);
                    
                    node_t* num_node = NUM_(result);
                    ReplaceNode(node, num_node);
                    return result;
                }
            }
            return NAN;
        }
        
        case ARG_VAR:
        case ARG_FUNC:
            return NAN;

        default:
            return NAN;
    }
}

static node_t *RemoveNeutralElem(node_t *node)
{
    if (!node) return NULL;
    
    node->left  = RemoveNeutralElem(node->left);
    node->right = RemoveNeutralElem(node->right);
    
    if (node->type != ARG_OP) return node;
        
    bool left_is_num  = (node->left  && node->left->type  == ARG_NUM);
    bool right_is_num = (node->right && node->right->type == ARG_NUM);
    
    if ((left_is_num && !node->left) || (right_is_num && !node->right))
        return node;
   
    switch (node->item.op)
    {
        case HASH_MUL:
            if (left_is_num && is_zero(node->left->item.num - 1.0))
            {
                return SwapParentAndChild(node, node->right);
            }
            else if (right_is_num && is_zero(node->right->item.num - 1.0))
            {
                return SwapParentAndChild(node, node->left);
            }
            else if ((left_is_num  && is_zero(node->left->item.num)) || 
                     (right_is_num && is_zero(node->right->item.num)))
            {
                node_t* zero_node = NUM_(0.0);
                ReplaceNode(node, zero_node);
                return zero_node;
            }
            break;

        case HASH_DIV:
            if (right_is_num && is_zero(node->right->item.num - 1.0))
            {
                return SwapParentAndChild(node, node->left);
            }
            break;

        case HASH_ADD:
            if (left_is_num && is_zero(node->left->item.num))
            {
                return SwapParentAndChild(node, node->right);
            }
            else if (right_is_num && is_zero(node->right->item.num))
            {
                return SwapParentAndChild(node, node->left);
            }
            break;

        case HASH_SUB:
            if (left_is_num && is_zero(node->left->item.num))
            {
                node_t* neg_node = MUL_(NUM_(-1.0), node->right);
                ReplaceNode(node, neg_node);
                return neg_node;
            }
            else if (right_is_num && is_zero(node->right->item.num))
            {
                return SwapParentAndChild(node, node->left);
            }
            break;

        case HASH_POW:
            if (right_is_num && is_zero(node->right->item.num))
            {
                node_t* one_node = NUM_(1.0);
                ReplaceNode(node, one_node);
                return one_node;
            }
            else if (right_is_num && is_zero(node->right->item.num - 1.0))
            {
                return SwapParentAndChild(node, node->left);
            }
            else if (left_is_num && is_zero(node->left->item.num - 1.0))
            {
                node_t* one_node = NUM_(1.0);
                ReplaceNode(node, one_node);
                return one_node;
            }
            break;
        
        default:
            break;
    }
    
    return node; 
}


static void ReplaceNode(node_t *old_node, node_t *new_node)
{
    if (!old_node || !new_node) return;
    
    new_node->parent = old_node->parent;
    
    if (old_node->parent)
    {
        if (old_node->parent->left == old_node)
            old_node->parent->left = new_node;
        else if (old_node->parent->right == old_node)
            old_node->parent->right = new_node;
            
        FreeNodes(old_node);
        global_number_changes++;
    }
}


static node_t *SwapParentAndChild(node_t *parent, node_t *child)
{
    if (!parent || !child) return NULL;

    node_t *new_node = CopyNode(child);
    if (!new_node) return parent;
    
    ReplaceNode(parent, new_node);
    return new_node;
}


static int CmpForBinSearch(const void *a, const void *b)
{
    const hash_t *hash_a = (const hash_t*)a;
    const op_t   *op_b   = (const op_t*)b;
    
    if (*hash_a > op_b->hash)
        return 1;
    if (*hash_a < op_b->hash)
        return -1;
    return 0;
}