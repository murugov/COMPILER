#ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include "CONFIG.hpp"

#include "GetHash.hpp"
#include "stack.hpp"
#include "tree.hpp"
#include "hash_table.hpp"
enum HashOp
{
	HASH_IF        = 0xD1D,
	HASH_ELSE      = 0x2F8D39,
	HASH_FOR       = 0x18CC9,
	HASH_WHILE     = 0x6BDCB31,
	HASH_RETURN    = 0xC84E3D30,
	HASH_DEF       = 0x18405,
	HASH_INIT      = 0x316510,
	HASH_EOF       = 0x10CDC,
	HASH_UNDEF     = 0x4D24AAC,
	HASH_LPAREN    = 0x28,
	HASH_RPAREN    = 0x29,
	HASH_LBRACKET  = 0x5B,
	HASH_RBRACKET  = 0x5D,
	HASH_LBRACE    = 0x7B,
	HASH_RBRACE    = 0x7D,
	HASH_COMMA     = 0x2C,
	HASH_SEMICOLON = 0x3B,
	HASH_COLON     = 0x3A,
	HASH_EQ        = 0x3D,
	HASH_EQEQ      = 0x7A0,
	HASH_NE        = 0x43C,
	HASH_GT        = 0x3E,
	HASH_LT        = 0x3C,
	HASH_GE        = 0x7BF,
	HASH_LE        = 0x781,
	HASH_ADD       = 0x2B,
	HASH_SUB       = 0x2D,
	HASH_MUL       = 0x2A,
	HASH_DIV       = 0x2F,
	HASH_POW       = 0x5E,
	HASH_SQRT      = 0x35FD20,
	HASH_E         = 0x65,
	HASH_LN        = 0xD82,
	HASH_LOG       = 0x1A344,
	HASH_SIN       = 0x1BCD8,
	HASH_COS       = 0x18187,
	HASH_TAN       = 0x1BFA1,
	HASH_COT       = 0x18188,
	HASH_SINH      = 0x35DE90,
	HASH_COSH      = 0x2EAFC1,
	HASH_TANH      = 0x3634E7,
	HASH_COTH      = 0x2EAFE0,
	HASH_ARCSIN    = 0xABFB4946,
	HASH_ARCCOS    = 0xABFB0DF5,
	HASH_ARCTAN    = 0xABFB4C0F,
	HASH_ARCCOT    = 0xABFB0DF6,
	HASH_ARCSINH   = 0x14D36DDFE2,
	HASH_ARCCOSH   = 0x14D366B113,
	HASH_ARCTANH   = 0x14D36E3639,
	HASH_ARCCOTH   = 0x14D366B132
};
#include "dump.hpp"
#include "colors.hpp"

#define FP "r0"
#define TP "r1"

#define GLOBAL_OFFSET     1000
#define MAX_NUM_FUNC_ARGS 8

extern FILE *ReportFile;

enum backErr_t
{
    BACK_SUCCESS = 0,
    BACK_ERROR   = 1
};

struct ntElem_t
{
    const char* name;
    int         offset;
};

struct trans_t
{
    node_t*                  node;
    stk_t<ht_t<ntElem_t*>*>* name_tables;
    int                      cur_name_table;
	int						 size_of_stk_frm;
    int                      global_offset;
};

backErr_t transCtor(trans_t *trans, const char *src);
backErr_t transDtor(trans_t *trans);

backErr_t TranslateTree(trans_t *trans, const char* report);
backErr_t CompileOnlyFunc(trans_t *trans);
backErr_t FindAndCompileInits(trans_t *trans);

backErr_t TranslatePrimaryStatement(trans_t *trans);
backErr_t TranslateCalc(trans_t *trans);
backErr_t TranslateVarInit(trans_t *trans);
backErr_t TranslateFuncDef(trans_t *trans);
backErr_t TranslateAssignment(trans_t *trans);
backErr_t TranslateCondition(trans_t *trans);
backErr_t TranslateFuncCall(trans_t *trans);

backErr_t TranslateIf(trans_t * trans);
backErr_t TranslateWhile (trans_t *trans);
backErr_t TranslateReturn(trans_t *trans);

backErr_t TranslateOp(trans_t *trans);
backErr_t TranslateVar(trans_t *trans);


#define TR_NODE      (trans->node)
#define TR_NODE_NUM  (trans->node->item.num)
#define TR_NODE_HASH (trans->node->item.op)
#define TR_NODE_VAR  (trans->node->item.var)
#define TR_NODE_FUNC (trans->node->item.func)
#define TR_NODE_TYPE (trans->node->type)
#define TR_NODE_L    (trans->node->left)
#define TR_NODE_R    (trans->node->right)

#define CUR_NAME_TABLE      (trans->name_tables->data[trans->cur_name_table])
#define CUR_NAME_TABLE_POS  (trans->cur_name_table)
#define CUR_NAME_TABLE_SIZE (trans->name_tables->size)

#define PUSH(ptr)  fprintf(ReportFile, "push %s\n", ptr)
#define PUSHM(ptr) fprintf(ReportFile, "push [%s]\n", ptr)
#define POP(ptr)   fprintf(ReportFile, "pop %s\n", ptr)
#define POPM(ptr)  fprintf(ReportFile, "pop [%s]\n", ptr)

#endif