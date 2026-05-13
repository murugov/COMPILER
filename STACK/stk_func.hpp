#include "stack.hpp"

#ifndef STACK_FUNC_HPP
#define STACK_FUNC_HPP


template <typename stk_elem_t>
stk_err_t stk_init(stk_t<stk_elem_t> *stk, const size_t cap)
{   
    if ((ssize_t)cap < 0) { return STK_ERROR; }
    
    stk->data = (stk_elem_t*)calloc(cap, sizeof(stk_elem_t));
    if (stk->data == NULL) { return STK_ERROR; }

    stk->size = 0;
    stk->cap  = cap;

    return STK_SUCCESS;
}

template <typename stk_elem_t>
stk_err_t stk_free(stk_t<stk_elem_t> *stk)
{
    if (stk == NULL) { return STK_SUCCESS; }

    free(stk->data);

    stk->data = NULL;
    stk->size = 0;
    stk->cap  = 0;

    return STK_SUCCESS;
}

template <typename stk_elem_t>
stk_err_t stk_push(stk_t<stk_elem_t> *stk, const stk_elem_t val)
{
    if (stk->size >= stk->cap)
    {
        if (stk_grow(stk) == STK_ERROR) { return STK_ERROR; }
    }

    stk->data[stk->size] = val;
    (stk->size)++;

    return STK_SUCCESS;
}

template <typename stk_elem_t>
stk_err_t stk_pop(stk_t<stk_elem_t> *stk, stk_elem_t *val)
{
    if (stk->size == 0) { return STK_ERROR; }

    (stk->size)--;
    *val = stk->data[stk->size];
    stk->data[stk->size] = 0;

    const size_t cap_div_by_4 = stk->cap >> 2;
    if (stk->size < cap_div_by_4 && cap_div_by_4 > MIN_STK_CAP)
    {
        if (stk_shrink(stk) == STK_ERROR) { return STK_ERROR; }
    }

    return STK_SUCCESS;
}

template <typename stk_elem_t>
inline stk_err_t stk_clear(stk_t<stk_elem_t> *stk)
{
    memset(stk->data, 0, (stk->size) * sizeof(stk_elem_t));
    stk->size = 0;

    return STK_SUCCESS;
}

template <typename stk_elem_t>
stk_err_t stk_grow(stk_t<stk_elem_t> *stk)
{
    const size_t old_cap = stk->cap;
    const size_t new_cap = old_cap << 1;

    stk_elem_t *new_data = (stk_elem_t*)realloc(stk->data, new_cap * sizeof(stk_elem_t));
    if (new_data == NULL) { return STK_ERROR; }

    for (size_t i = old_cap; i < new_cap; i++) { new_data[i] = 0; }
    
    stk->cap  = new_cap;
    stk->data = new_data;

    return STK_SUCCESS;
}

template <typename stk_elem_t>
stk_err_t stk_shrink(stk_t<stk_elem_t> *stk)
{
    const size_t old_cap = stk->cap;
    const size_t new_cap = old_cap >> 2;
        
    stk_elem_t *new_data = (stk_elem_t*)realloc(stk->data, new_cap * sizeof(stk_elem_t));
    if (new_data == NULL) { return STK_ERROR; }

    stk->cap  = new_cap;
    stk->data = new_data;

    return STK_SUCCESS;
}


#endif