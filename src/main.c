#include <windows.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "kernel_contract.h"

typedef void (*max_index_kernel)(size_t n, const float *a, const float *b, float *c, int32_t *idx);

#define REPETITIONS 30
#define CHECK_LENGTH 17
#define OUTPUT_GUARD 8
#define CHECK_CHUNK 4096

typedef struct
{
    const char *name;
    max_index_kernel function;
} kernel_entry;

static kernel_entry kernels[] = {
    {"C", max_index_c},
    {"Scalar x86-64", max_index_scalar},
    {"SIMD XMM", max_index_xmm},
    {"SIMD YMM", max_index_ymm}
};

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

static int verify_benchmark_output(const kernel_entry *kernel, size_t n, const float *a,
                                   const float *b, const float *c, const int32_t *idx)
{
    float expected_c[CHECK_CHUNK];
    int32_t expected_idx[CHECK_CHUNK];

    for (size_t start = 0; start < n; start += CHECK_CHUNK)
    {
        size_t count = n - start;
        if (count > CHECK_CHUNK)
        {
            count = CHECK_CHUNK;
        }
        max_index_c(count, a + start, b + start, expected_c, expected_idx);
        if (!check_result(kernel->name, c + start, idx + start, expected_c, expected_idx, count))
        {
            return 0;
        }
    }
    return 1;
}

static void print_samples(const char *name, size_t n, const float *a, const float *b,
                          const float *c, const int32_t *idx)
{
    printf("  %s first 5 (i: A,B -> C/idx):", name);
    for (size_t i = 0; i < 5; ++i)
    {
        printf(" [%zu] %.0f,%.0f->%.0f/%d", i, a[i], b[i], c[i], (int)idx[i]);
    }
    printf("\n  %s last 5 (i: A,B -> C/idx):", name);
    for (size_t i = n - 5; i < n; ++i)
    {
        printf(" [%zu] %.0f,%.0f->%.0f/%d", i, a[i], b[i], c[i], (int)idx[i]);
    }
    printf("\n");
}

static int benchmark_case(size_t n, long long frequency)
{
    float *a = (float *)malloc(n * sizeof(*a));
    float *b = (float *)malloc(n * sizeof(*b));
    float *c = (float *)malloc(n * sizeof(*c));
    int32_t *idx = (int32_t *)malloc(n * sizeof(*idx));
    int ok = 1;

    if (a == NULL || b == NULL || c == NULL || idx == NULL)
    {
        fprintf(stderr, "Could not allocate arrays for n=%zu\n", n);
        free(a);
        free(b);
        free(c);
        free(idx);
        return 0;
    }

    initialize_input(a, b, n);
    printf("\nVector length n=%zu\n", n);

    for (size_t k = 0; k < sizeof(kernels) / sizeof(kernels[0]); ++k)
    {
        LARGE_INTEGER start;
        LARGE_INTEGER finish;
        double total_ticks = 0.0;

        kernels[k].function(n, a, b, c, idx);
        for (int repetition = 0; repetition < REPETITIONS; ++repetition)
        {
            if (!QueryPerformanceCounter(&start))
            {
                fprintf(stderr, "Could not read the performance counter\n");
                ok = 0;
                break;
            }
            kernels[k].function(n, a, b, c, idx);
            if (!QueryPerformanceCounter(&finish))
            {
                fprintf(stderr, "Could not read the performance counter\n");
                ok = 0;
                break;
            }
            total_ticks += (double)(finish.QuadPart - start.QuadPart);
        }

        if (!ok)
        {
            break;
        }

        if (!verify_benchmark_output(&kernels[k], n, a, b, c, idx))
        {
            fprintf(stderr, "%s correctness FAIL\n", kernels[k].name);
            ok = 0;
            break;
        }

        printf("%-14s correctness PASS; average %.3f us over %d runs\n", kernels[k].name,
               total_ticks * 1000000.0 / ((double)frequency * REPETITIONS), REPETITIONS);
        print_samples(kernels[k].name, n, a, b, c, idx);
    }

    free(a);
    free(b);
    free(c);
    free(idx);
    return ok;
}

static int run_benchmarks(void)
{
    LARGE_INTEGER frequency;
    const size_t lengths[] = {1u << 20, 1u << 26, 1u << 28};

    if (!QueryPerformanceFrequency(&frequency))
    {
        fprintf(stderr, "Could not read the performance-counter frequency\n");
        return 0;
    }

    printf("Timing kernel calls only; %d runs per listed kernel.\n", REPETITIONS);
    for (size_t i = 0; i < sizeof(lengths) / sizeof(lengths[0]); ++i)
    {
        if (!benchmark_case(lengths[i], frequency.QuadPart))
        {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char **argv)
{
    srand((unsigned int)time(NULL));

    if (argc == 2 && strcmp(argv[1], "--verify") == 0)
    {
        return run_verification() ? 0 : 1;
    }
    if (argc == 2 && strcmp(argv[1], "--bench") == 0)
    {
        if (!run_verification())
        {
            return 1;
        }
        return run_benchmarks() ? 0 : 1;
    }

    fprintf(stderr, "Usage: %s --verify | --bench\n", argv[0]);
    return 2;
}
