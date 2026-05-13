#include "./parser.hpp"
#include "OpInstrSet.cpp"

static int CmpForBinSearch(const void *a, const void *b);


double CalcExpression(node_t *node)
{
    if (node == NULL) return NAN;
    
    switch (node->type) 
    {
        case ARG_OP:
        {
            size_t *index = (size_t*)bsearch(&node->item.op, op_instr_set, LEN_INSTR_SET, sizeof(op_instr_set[0]), CmpForBinSearch);;
            if (!IS_BAD_PTR(index))
            {
                double left_val = NAN, right_val = NAN;
                
                if (op_instr_set[*index].num_args >= 1) 
                {
                    right_val = CalcExpression(node->right);
                    if (isnan(right_val)) return NAN;
                }
                
                if (op_instr_set[*index].num_args == 2) 
                {
                    left_val = CalcExpression(node->left);
                    if (isnan(left_val)) return NAN;
                }
                
                calc_context context = {left_val, right_val};
                return op_instr_set[*index].calc(&context);
            }

            return NAN;
        }
        case ARG_VAR:
        {
            return NAN;
        }
        case ARG_NUM:
            return node->item.num;

        default:
            return NAN;
    }
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