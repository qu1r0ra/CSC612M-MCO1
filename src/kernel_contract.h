#ifndef KERNEL_CONTRACT_H
#define KERNEL_CONTRACT_H

#include <stddef.h>
#include <stdint.h>

void max_index_c(size_t n, const float *a, const float *b, float *c, int32_t *idx);

void max_index_scalar(size_t n, const float *a, const float *b, float *c, int32_t *idx);

void max_index_xmm(size_t n, const float *a, const float *b, float *c, int32_t *idx);

void max_index_ymm(size_t n, const float *a, const float *b, float *c, int32_t *idx);

#endif
