#ifndef STACK_HPP
#define STACK_HPP

#include "CONFIG.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MIN_STK_CAP 8

enum stk_err_t
{
    STK_SUCCESS = 0,
    STK_ERROR   = 1
};

template <typename stk_elem_t>
struct stk_t
{
    stk_elem_t *data;
    size_t      size;
    size_t      cap;
};


template <typename stk_elem_t>
stk_err_t stk_init(stk_t<stk_elem_t> *stk, const size_t cap);

template <typename stk_elem_t>
stk_err_t stk_free(stk_t<stk_elem_t> *stk);

template <typename stk_elem_t>
stk_err_t stk_push(stk_t<stk_elem_t> *stk, const stk_elem_t val);

template <typename stk_elem_t>
stk_err_t stk_pop(stk_t<stk_elem_t> *stk, stk_elem_t *val);

template <typename stk_elem_t>
stk_err_t stk_clear(stk_t<stk_elem_t> *stk);

template <typename stk_elem_t>
stk_err_t stk_grow(stk_t<stk_elem_t> *stk);

template <typename stk_elem_t>
stk_err_t stk_shrink(stk_t<stk_elem_t> *stk);


#include "stk_func.hpp"

#endif