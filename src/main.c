#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "kernel_contract.h"

typedef void (*max_index_kernel)(size_t n, const float *a, const float *b, float *c, int32_t *idx);

#define CHECK_LENGTH 17
#define OUTPUT_GUARD 8

typedef struct
{
    const char *name;
    max_index_kernel function;
} kernel_entry;

static kernel_entry kernels[] = {{"C", max_index_c}, {"Scalar x86-64", max_index_scalar}};

static int check_result(const char *name, const float *actual_c, const int32_t *actual_idx,
                        const float *expected_c, const int32_t *expected_idx, size_t n)
{
    for (size_t i = 0; i < n; ++i)
    {
        if (actual_c[i] != expected_c[i] || actual_idx[i] != expected_idx[i])
        {
            fprintf(stderr, "%s mismatch at index %zu\n", name, i);
            return 0;
        }
    }
    return 1;
}

static int verify_known_answer(void)
{
    const float a[] = {3.0f, -5.0f, 2.0f, 7.0f, -1.0f};
    const float b[] = {1.0f, 4.0f, 2.0f, 7.0f, -2.0f};
    const float expected_c[] = {3.0f, 4.0f, 2.0f, 7.0f, -1.0f};
    const int32_t expected_idx[] = {0, 1, 0, 0, 0};
    float c[5];
    int32_t idx[5];
    int ok = 1;

    for (size_t k = 0; k < sizeof(kernels) / sizeof(kernels[0]); ++k)
    {
        kernels[k].function(5, a, b, c, idx);
        if (check_result(kernels[k].name, c, idx, expected_c, expected_idx, 5))
        {
            printf("%-14s known-answer PASS\n", kernels[k].name);
        }
        else
        {
            ok = 0;
        }
    }
    return ok;
}

static void initialize_input(float *a, float *b, size_t n)
{
    for (size_t i = 0; i < n; ++i)
    {
        a[i] = (float)(rand() % 100);
        b[i] = (float)(rand() % 100);
    }
}

static int verify_checks(void)
{
    float a[CHECK_LENGTH + OUTPUT_GUARD];
    float b[CHECK_LENGTH + OUTPUT_GUARD];
    float c[CHECK_LENGTH + OUTPUT_GUARD];
    float expected_c[CHECK_LENGTH];
    int32_t idx[CHECK_LENGTH + OUTPUT_GUARD];
    int32_t expected_idx[CHECK_LENGTH];
    int ok = 1;

    initialize_input(a, b, CHECK_LENGTH + OUTPUT_GUARD);
    for (size_t k = 0; k < sizeof(kernels) / sizeof(kernels[0]); ++k)
    {
        for (size_t n = 0; n <= CHECK_LENGTH; ++n)
        {
            max_index_c(n, a, b, expected_c, expected_idx);
            for (size_t i = 0; i < n + OUTPUT_GUARD; ++i)
            {
                c[i] = -999.0f;
                idx[i] = -999;
            }

            kernels[k].function(n, a, b, c, idx);
            if (!check_result(kernels[k].name, c, idx, expected_c, expected_idx, n))
            {
                fprintf(stderr, "check length %zu failed\n", n);
                ok = 0;
                break;
            }
            for (size_t i = n; i < n + OUTPUT_GUARD; ++i)
            {
                if (c[i] != -999.0f || idx[i] != -999)
                {
                    fprintf(stderr, "%s wrote past n=%zu\n", kernels[k].name, n);
                    ok = 0;
                    break;
                }
            }
            if (!ok)
            {
                break;
            }
        }

        if (ok)
        {
            printf("%-14s checks PASS\n", kernels[k].name);
        }
    }

    if (ok)
    {
        printf("Check lengths: 0-17\n");
    }
    return ok;
}

static int run_verification(void)
{
    int known_answer_ok;
    int checks_ok;

    printf("Build configuration: %s\n",
#ifdef NDEBUG
           "Release"
#else
           "Debug"
#endif
    );
    known_answer_ok = verify_known_answer();
    checks_ok = verify_checks();
    printf("Verification: %s\n", known_answer_ok && checks_ok ? "PASS" : "FAIL");
    return known_answer_ok && checks_ok;
}

int main(void)
{
    srand((unsigned int)time(NULL));
    return run_verification() ? 0 : 1;
}
