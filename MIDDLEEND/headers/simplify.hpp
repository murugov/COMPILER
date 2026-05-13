#ifndef SIMPLIFY
#define SIMPLIFY

#include "CONFIG.hpp"

#include "tree.hpp"
#include "HashOp.hpp"
#include "math_func.hpp"
#include "is_zero.hpp"
#include "HashOp.hpp"

struct op_t
{
	hash_t hash;
	char   name[8];
	int    num_args;
	calc_t calc;
	diff_t diff;
};

void SimplifyNode(node_t* node);
double CalcExpression(node_t *node);


#endif