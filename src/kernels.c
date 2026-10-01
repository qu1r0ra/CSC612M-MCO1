#include "kernel_contract.h"

void max_index_c(size_t n, const float *a, const float *b, float *c, int32_t *idx)
{
    for (size_t i = 0; i < n; ++i)
    {
        if (a[i] >= b[i])
        {
            c[i] = a[i];
            idx[i] = 0;
        }
        else
        {
            c[i] = b[i];
            idx[i] = 1;
        }
    }
}
