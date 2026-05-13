#ifndef DSL_H
#define DSL_H

#define dL DerivativeNode(diff_params->node_left, diff_params->hash_indep_var)
#define dR DerivativeNode(diff_params->node_right, diff_params->hash_indep_var)
#define L  CopyNode(diff_params->node_left)
#define R  CopyNode(diff_params->node_right)



#define ADD_(node_left, node_right) NewNode(ARG_OP, valOP(GetHash("+")), node_left, node_right)
#define SUB_(node_left, node_right) NewNode(ARG_OP, valOP(GetHash("-")), node_left, node_right)
#define MUL_(node_left, node_right) NewNode(ARG_OP, valOP(GetHash("*")), node_left, node_right)
#define DIV_(node_left, node_right) NewNode(ARG_OP, valOP(GetHash("/")), node_left, node_right)
#define POW_(node_left, node_right) NewNode(ARG_OP, valOP(GetHash("^")), node_left, node_right)
#define SQRT_(node_right) NewNode(ARG_OP, valOP(GetHash("sqrt")), NULL, node_right)



#define E_(node_right) NewNode(ARG_OP, valOP(GetHash("e")), NULL, node_right)
#define LN_(node_right)  NewNode(ARG_OP, valOP(GetHash("ln")), NULL, node_right)
#define LOG_(node_left, node_right) NewNode(ARG_OP, valOP(GetHash("log")), node_left, node_right)



#define SIN_(node_right) NewNode(ARG_OP, valOP(GetHash("sin")), NULL, node_right)
#define COS_(node_right) NewNode(ARG_OP, valOP(GetHash("cos")), NULL, node_right)
#define TAN_(node_right) NewNode(ARG_OP, valOP(GetHash("tan")), NULL, node_right)
#define COT_(node_right) NewNode(ARG_OP, valOP(GetHash("cot")), NULL, node_right)



#define COSH_(node_right) NewNode(ARG_OP, valOP(GetHash("cosh")), NULL, node_right)
#define SINH_(node_right) NewNode(ARG_OP, valOP(GetHash("sinh")), NULL, node_right)
#define TANH_(node_right) NewNode(ARG_OP, valOP(GetHash("tanh")), NULL, node_right)
#define COTH_(node_right) NewNode(ARG_OP, valOP(GetHash("coth")), NULL, node_right)



#define ARCSIN_(node_right) NewNode(ARG_OP, valOP(GetHash("arcsin")), NULL, node_right)
#define ARCCOS_(node_right) NewNode(ARG_OP, valOP(GetHash("arccos")), NULL, node_right)
#define ARCTAN_(node_right) NewNode(ARG_OP, valOP(GetHash("arctan")), NULL, node_right)
#define ARCCOT_(node_right) NewNode(ARG_OP, valOP(GetHash("arccot")), NULL, node_right)



#define ARCSINH_(node_right) NewNode(ARG_OP, valOP(GetHash("arcsinh")), NULL, node_right)
#define ARCCOSH_(node_right) NewNode(ARG_OP, valOP(GetHash("arccosh")), NULL, node_right)
#define ARCTANH_(node_right) NewNode(ARG_OP, valOP(GetHash("arctanh")), NULL, node_right)
#define ARCCOTH_(node_right) NewNode(ARG_OP, valOP(GetHash("arccoth")), NULL, node_right)



#endif