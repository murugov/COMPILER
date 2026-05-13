#ifndef VEC_FUNC_HPP
#define VEC_FUNC_HPP

#include "vector.hpp"


template <typename vec_elem_t>
vec_err_t vec_init(vec_t<vec_elem_t> *vec, const size_t cap)
{   
    if ((ssize_t)cap < 0) { return VEC_ERROR; }
    
    vec->data = (vec_elem_t*)calloc(cap, sizeof(vec_elem_t));
    if (vec->data == NULL) { return VEC_ERROR; }
    
    vec->size = 0;
    vec->cap  = cap;

    return VEC_SUCCESS;
}

template <typename vec_elem_t>
vec_err_t vec_free(vec_t<vec_elem_t> *vec)
{
    if (vec == NULL) { return VEC_SUCCESS; }

    free(vec->data);

    vec->data = NULL;
    vec->size = 0;
    vec->cap  = 0;

    return VEC_SUCCESS;
}

template <typename vec_elem_t>
vec_err_t vec_push_back(vec_t<vec_elem_t> *vec, const vec_elem_t val)
{
    if (vec->size >= vec->cap)
    {
        if (vec_grow(vec) == VEC_ERROR) { return VEC_ERROR; }
    }

    vec->data[vec->size] = val;
    (vec->size)++;

    return VEC_SUCCESS;
}

template <typename vec_elem_t>
vec_err_t vec_push_front(vec_t<vec_elem_t> *vec, const vec_elem_t val)
{
    const size_t vec_size = vec->size;

    if (vec_size >= vec->cap)
    {
        if (vec_grow(vec) == VEC_ERROR) { return VEC_ERROR; }
    }

    if (vec->size > 0)
    {
        memmove((vec->data) + 1, vec->data, vec_size * sizeof(vec_elem_t));
    }
    
    vec->data[0] = val;
    vec->size++;

    return VEC_SUCCESS;
}

template <typename vec_elem_t>
vec_err_t vec_pop_back(vec_t<vec_elem_t> *vec, vec_elem_t *val)
{
    
    if (vec->size == 0) { return VEC_ERROR; }

    (vec->size)--;
    *val = vec->data[vec->size];
    vec->data[vec->size] = 0;

    const size_t cap_div_by_4 = vec->cap >> 2;
    if (vec->size < cap_div_by_4 && cap_div_by_4 > MIN_VEC_CAP)
    {
        if (vec_shrink(vec) == VEC_ERROR) { return VEC_ERROR; }
    }

    return VEC_SUCCESS;
}


template <typename vec_elem_t>
vec_err_t vec_pop_front(vec_t<vec_elem_t> *vec, vec_elem_t *val)
{
    if (vec->size == 0) { return VEC_ERROR; }
    
    if (val) { *val = vec->data[0]; }
    
    if (vec->size > 1)
    {
        memmove(vec->data, vec->data + 1, (vec->size - 1) * sizeof(vec_elem_t));
    }
    
    vec->size--;
    vec->data[vec->size] = 0;
    
    const size_t cap_div_by_4 = vec->cap >> 2;
    if (vec->size < cap_div_by_4 && cap_div_by_4 >= MIN_VEC_CAP)
    {
        if (vec_shrink(vec) == VEC_ERROR) { return VEC_ERROR; }
    }
    
    return VEC_SUCCESS;
}

template <typename vec_elem_t>
inline vec_err_t vec_clear(vec_t<vec_elem_t> *vec)
{
    memset(vec->data, 0, (vec->size) * sizeof(vec_elem_t));
    vec->size = 0;

    return VEC_SUCCESS;
}

template <typename vec_elem_t>
inline bool vec_empty(vec_t<vec_elem_t> *vec)
{
    return (vec->size == 0) ? true: false;
}

template <typename vec_elem_t>
vec_err_t vec_grow(vec_t<vec_elem_t> *vec)
{
    const size_t old_cap = vec->cap;
    const size_t new_cap = old_cap << 1;
    
    vec_elem_t *new_data = (vec_elem_t*)realloc(vec->data, new_cap * sizeof(vec_elem_t));
    if (new_data == NULL) { return VEC_ERROR; }

    for (size_t i = old_cap; i < new_cap; i++) { new_data[i] = 0; }
    
    vec->cap  = new_cap;
    vec->data = new_data;

    return VEC_SUCCESS;
}

template <typename vec_elem_t>
vec_err_t vec_reserve(vec_t<vec_elem_t> *vec, size_t new_cap)
{    
    const size_t old_cap = vec->cap;

    vec_elem_t *new_data = (vec_elem_t*)realloc(vec->data, new_cap * sizeof(vec_elem_t));
    if (new_data == NULL) { return VEC_ERROR; }

    for (size_t i = old_cap; i < new_cap; i++) { new_data[i] = 0; }
    
    vec->cap  = new_cap;
    vec->data = new_data;

    return VEC_SUCCESS;
}

template <typename vec_elem_t>
vec_err_t vec_shrink(vec_t<vec_elem_t> *vec)
{
    const size_t old_cap = vec->cap;
    const size_t new_cap = old_cap >> 2;
        
    vec_elem_t *new_data = (vec_elem_t*)realloc(vec->data, new_cap * sizeof(vec_elem_t));
    if (new_data == NULL) { return VEC_ERROR; }

    vec->cap  = new_cap;
    vec->data = new_data;

    return VEC_SUCCESS;
}

template <typename vec_elem_t>
vec_err_t vec_truncate(vec_t<vec_elem_t> *vec, size_t new_cap)
{        
    if (vec->cap < new_cap) { return VEC_ERROR; }

    vec_elem_t *new_data = (vec_elem_t*)realloc(vec->data, new_cap * sizeof(vec_elem_t));
    if (new_data == NULL) { return VEC_ERROR; }

    vec->cap  = new_cap;
    vec->data = new_data;

    return VEC_SUCCESS;
}

template <typename vec_elem_t>
vec_err_t vec_remove(vec_t<vec_elem_t> *vec, size_t index)
{
    if (index >= vec->size) { return VEC_ERROR; }
    
    if (index < vec->size - 1)
    {
        memmove(vec->data + index, vec->data + index + 1, (vec->size - index - 1) * sizeof(vec_elem_t));
    }

    vec->size--;
    vec->data[vec->size] = 0;
    
    return VEC_SUCCESS;
}


#endif