#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "CONFIG.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define MIN_VEC_CAP 8

enum vec_err_t
{
    VEC_SUCCESS = 0,
    VEC_ERROR   = 1
};


template <typename vec_elem_t>
struct vec_t
{
    vec_elem_t* data;
    size_t      size;
    size_t      cap;
};


template <typename vec_elem_t>
vec_err_t vec_init(vec_t<vec_elem_t> *vec, const size_t cap);

template <typename vec_elem_t>
vec_err_t vec_free(vec_t<vec_elem_t> *vec);

template <typename vec_elem_t>
vec_err_t vec_push_back(vec_t<vec_elem_t> *vec, const vec_elem_t val);

template <typename vec_elem_t>
vec_err_t vec_push_front(vec_t<vec_elem_t> *vec, const vec_elem_t val);

template <typename vec_elem_t>
vec_err_t vec_pop_back(vec_t<vec_elem_t> *vec, vec_elem_t *val);

template <typename vec_elem_t>
vec_err_t vec_pop_front(vec_t<vec_elem_t> *vec, vec_elem_t *val);

template <typename vec_elem_t>
vec_err_t vec_clear(vec_t<vec_elem_t> *vec);

template <typename vec_elem_t>
bool vec_empty(vec_t<vec_elem_t> *vec);

template <typename vec_elem_t>
vec_err_t vec_grow(vec_t<vec_elem_t> *vec);

template <typename vec_elem_t>
vec_err_t vec_reserve(vec_t<vec_elem_t> *vec, const size_t new_cap);

template <typename vec_elem_t>
vec_err_t vec_shrink(vec_t<vec_elem_t> *vec);

template <typename vec_elem_t>
vec_err_t vec_truncate(vec_t<vec_elem_t> *vec, const size_t new_cap);

template <typename vec_elem_t>
vec_err_t vec_remove(vec_t<vec_elem_t> *vec, const size_t index);


#include "vec_func.hpp"

#endif