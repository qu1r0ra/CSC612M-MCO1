# CSC612M MCO1

This project compares four kernels that compute the element-wise maximum of
two single-precision vectors and record which input supplied each result. The
shared interface is in [`src/kernel_contract.h`](src/kernel_contract.h). The
Visual Studio 2026 solution provides x64 Debug and Release configurations.

## Build and run

Build x64 Debug and Release with Visual Studio 2026. Run the commands in
[`build-commands.md`](build-commands.md) from a Visual Studio Developer
PowerShell.
NASM must be on `PATH` for the assembly build steps.

`--verify` runs the known-answer and size checks. `--bench` runs those checks first,
then times each listed kernel for 2^20, 2^26, and 2^28 elements. The assignment
allows reducing 2^30 to 2^28 when memory is limited; the four arrays at 2^30
require 16 GiB. Each kernel gets one warm-up and 30 timed calls per case.
Timing covers the kernel call only. The program prints A, B, C, and idx for the
first and last five elements.

The shared kernel signature is declared in `src/kernel_contract.h`:

```c
void max_index_<version>(size_t n, const float *a, const float *b,
                         float *c, int32_t *idx);
```

Assembly owners should keep the function identifier already assigned in their
starter file and add the function to the `kernels` list in `src/main.c` after
implementing it. All listed kernels use the same checks and benchmark.

## Kernel contract

For each element, the selected value is `A[i]` when `A[i] >= B[i]`; otherwise
it is `B[i]`. `idx[i]` is `0` for A and `1` for B. Equal values select A. The
comparison follows the written condition. Test and benchmark data stay finite.

The input generator fills A and B independently with `rand() % 100`. It seeds
`rand()` from the current time once at startup, so values change between runs.
Ties can occur naturally; the known-answer case also checks a tie and includes
negative values.

## Results

### Debug

|    n |          C mean (µs) | Scalar x86-64 (µs) | XMM (µs) | YMM (µs) |
| ---: | -------------------: | -----------------: | -------: | -------: |
| 2^20 | pending measurements |            pending |  pending |  pending |
| 2^26 | pending measurements |            pending |  pending |  pending |
| 2^28 | pending measurements |            pending |  pending |  pending |

### Release

|    n |          C mean (µs) | Scalar x86-64 (µs) | XMM (µs) | YMM (µs) |
| ---: | -------------------: | -----------------: | -------: | -------: |
| 2^20 | pending measurements |            pending |  pending |  pending |
| 2^26 | pending measurements |            pending |  pending |  pending |
| 2^28 | pending measurements |            pending |  pending |  pending |

### Speedup relative to C

| Configuration |    n | Scalar x86-64 | SIMD XMM | SIMD YMM |
| ------------- | ---: | ------------: | -------: | -------: |
| Debug         | 2^20 |       pending |  pending |  pending |
| Debug         | 2^26 |       pending |  pending |  pending |
| Debug         | 2^28 |       pending |  pending |  pending |
| Release       | 2^20 |       pending |  pending |  pending |
| Release       | 2^26 |       pending |  pending |  pending |
| Release       | 2^28 |       pending |  pending |  pending |

**Christian — after all kernels are measured:** calculate each speedup as the
C average time divided by the corresponding kernel average. Explain the results
using the timings, compiler output, and memory traffic.

### Program output

- Debug timing output for all completed kernels: **Christian — add
  `docs/results/debug-benchmark.png`.**
- Release timing output for all completed kernels: **Christian — add
  `docs/results/release-benchmark.png`.**
- C correctness output: **Christian — add `docs/results/c-correctness.png`.**
- Scalar correctness output: **Christian — add
  `docs/results/scalar-correctness.png`.**
- XMM correctness and size-check output: **Immanuel — add
  `docs/results/xmm-correctness.png` and `docs/results/xmm-checks.png`.**
- YMM correctness and size-check output: **Enzo — add
  `docs/results/ymm-correctness.png` and `docs/results/ymm-checks.png`.**

Save screenshots under `docs/results/` and embed them here with Markdown image
links. Keep the complete report, tables, and analysis in this README.

## Implementation notes

### C reference and harness

The C reference uses the specified comparison: it copies A and stores index 0
when `A[i] >= B[i]`; otherwise it copies B and stores index 1. A short known-
answer case checks greater-than, less-than, and tie results. Size checks use
lengths 0–17, covering every XMM and YMM remainder and multiple vector passes.
They compare every output with C and check eight output canaries after the
requested length.

The benchmark initializes A and B with pseudo-random values seeded from the
current time at startup. It warms each kernel once, then times 30 calls with
`QueryPerformanceCounter`. Initialization, correctness checks, and sample
printing are outside the timed interval. The full output is checked against C
in small chunks, avoiding a second pair of large expected arrays. A, B, C, and
idx use 16 bytes per element, so 2^30 requires 16 GiB.

Visual Studio builds C17 in both configurations. Debug uses `/Od`; Release
uses `/O2` and `/Oi`. Both target `/arch:AVX2`; the Release C reference was
inspected with `dumpbin` and contains an eight-float YMM loop plus a remainder
path. Treat the compiler-optimized C result as the baseline when discussing
speedups.

The scaffold was validated on an AMD Ryzen 5 5600X (6 cores, 12 logical
processors) with 15.9 GiB of installed memory, Windows 11 Pro 10.0.26200,
Visual Studio 2026 / MSVC 14.51.36231, and NASM 3.02. Record machine or
toolchain changes beside the timing tables. **Christian: relate the results to
the hardware, compiler settings, and generated C code.**

### Scalar x86-64

The kernel follows the Windows x64 ABI. It receives n in RCX, A in RDX, B in
R8, and C in R9; it reads the fifth argument, idx, from `[rsp + 8*5]` at entry.
Each loop iteration loads one float from each input, compares them with
`ucomiss`, writes the selected float and 32-bit index, advances each pointer,
and decrements the remaining count. `jae` chooses A for equality. The scalar
kernel passes known-answer and size checks in Debug and Release. **Christian: compare its measured performance with the other kernels after they are ready.**

### SIMD XMM

**Immanuel:** describe the XMM implementation, tail handling, checks, correctness results, and performance.

### SIMD YMM

**Enzo:** describe the YMM implementation, tail handling, `vzeroupper`, checks, correctness results, and performance.

### Problems and solutions

**All contributors:** describe problems, fixes, and useful observations from your assigned work.

## Project contents

- `src/main.c` owns the verification and benchmark harness.
- `src/kernels.c` contains the C reference kernel.
- `src/max_index_scalar.asm` contains the scalar x86-64 kernel.
- `src/max_index_xmm.asm` and `src/max_index_ymm.asm` are the SIMD owner starters.
- `docs/1261G01G1.png` is the stitched assignment-specification screenshot.
- `docs/results/` will contain the program output images embedded in this report.
