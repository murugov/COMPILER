#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "CONFIG.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "IsBadPtr.hpp"
#include "TXTreader.hpp"
#include "GetHash.hpp"
#include "colors.hpp"

#define PATH_TO_SRC_KEY_FILE   "GENERATOR/src/keywords.txt"
#define PATH_TO_HASH_OP_FILE   "GENERATOR/reports/HashOp.hpp"
#define PATH_TO_OP_INSTR_SET   "GENERATOR/reports/OpInstrSet.cpp"
#define PATH_TO_KEYWORD_SET    "GENERATOR/reports/KeywordSet.cpp"

#define MAX_LEN_NAME_FUNC 16

typedef unsigned char count_t;
typedef char          *cmd_t;
typedef unsigned char num_t;

enum GenErr_t
{
    GEN_SUCCESS = 0,
    GEN_ERROR   = 1
};

struct funcInfo
{
    char   name[MAX_LEN_NAME_FUNC];
    char   op[MAX_LEN_NAME_FUNC];
    hash_t hash;
};

struct op_instr_t
{
    char name[MAX_LEN_NAME_FUNC];
    char op[MAX_LEN_NAME_FUNC];
    hash_t hash;
    int num_args; 
};

struct keyword_set_t
{
    char name[MAX_LEN_NAME_FUNC];
    char key_1[MAX_LEN_NAME_FUNC];
    char key_2[MAX_LEN_NAME_FUNC];
    hash_t hash;
};


void RemoveComments(char** arr_cmd, int *count_line);

GenErr_t GenHashOp(FILE *HashOpFile, char **arr_ptr, int count_line);
GenErr_t GenOpInstrSet(FILE *OpInstrSetFile, char **arr_ptr, int count_line);
GenErr_t GenKeywordSet(FILE *KeywordSetFile, char **arr_ptr, int count_line);

int CmpOpInstrSetByHash(const void *a, const void *b);
int CmpKeywordSetByHash(const void *a, const void *b);
int CmpByHash(const void *a, const void *b);


#endif