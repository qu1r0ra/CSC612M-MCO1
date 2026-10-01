# CSC612M MCO1

This project compares four implementations of element-wise maximum with index
tracking: C, scalar x86-64, SIMD XMM, and SIMD YMM. The kernels share the
interface in [`src/kernel_contract.h`](src/kernel_contract.h). The current
benchmark registers the C and scalar kernels; add the XMM and YMM kernels to
`src/main.c` after their implementations are ready.

## Build and run

Build x64 Debug and Release with the commands in
[`build-commands.md`](build-commands.md), from a Visual Studio Developer
PowerShell. NASM must be on `PATH` for the assembly build steps.

`--verify` runs a known-answer case and size checks. `--bench` runs those checks
first, then benchmarks each registered kernel at 2^20, 2^26, and 2^28 elements.
The assignment permits 2^28 or 2^29 when 2^30 is not practical; the four arrays
at 2^30 require 16 GiB before other memory use. Each kernel gets one warm-up and
30 timed calls per size. Timing covers the kernel call only. The program prints
A, B, C, and `idx` for the first and last five elements.

## Kernel contract and input

For each element, `C[i]` is `A[i]` when `A[i] >= B[i]`, and `B[i]` otherwise.
`idx[i]` is 0 when A supplies the maximum and 1 when B does. Equal values select
A. The test and benchmark inputs are finite.

The input generator initializes A and B independently with values from 0 to 99,
stored as floats. It seeds `rand()` once from the wall clock at startup. Ties can
occur naturally; the known-answer case also checks a tie and negative values.

## Results

Enter the average time in microseconds for each configuration. The speedup is
the C average divided by the corresponding kernel average.

### Debug

|    n |  C (µs) | Scalar x86-64 (µs) | SIMD XMM (µs) | SIMD YMM (µs) |
| ---: | ------: | -----------------: | ------------: | ------------: |
| 2^20 | pending |            pending |       pending |       pending |
| 2^26 | pending |            pending |       pending |       pending |
| 2^28 | pending |            pending |       pending |       pending |

### Release

|    n |  C (µs) | Scalar x86-64 (µs) | SIMD XMM (µs) | SIMD YMM (µs) |
| ---: | ------: | -----------------: | ------------: | ------------: |
| 2^20 | pending |            pending |       pending |       pending |
| 2^26 | pending |            pending |       pending |       pending |
| 2^28 | pending |            pending |       pending |       pending |

### Speedup relative to C

| Configuration |    n | Scalar x86-64 | SIMD XMM | SIMD YMM |
| ------------- | ---: | ------------: | -------: | -------: |
| Debug         | 2^20 |       pending |  pending |  pending |
| Debug         | 2^26 |       pending |  pending |  pending |
| Debug         | 2^28 |       pending |  pending |  pending |
| Release       | 2^20 |       pending |  pending |  pending |
| Release       | 2^26 |       pending |  pending |  pending |
| Release       | 2^28 |       pending |  pending |  pending |

**CJ:** After all four kernels are measured, report the speedups and
interpret the differences using the timings, compiler output, and memory
traffic.

## Program output

Add the screenshots under `docs/results/`. Embed each image here with a
repository-relative Markdown image link when it is available; keep the tables
and analysis in this README.

- Debug benchmark output (CJ): `docs/results/debug-benchmark.png`.
- Release benchmark output (CJ): `docs/results/release-benchmark.png`.
- C correctness output (CJ): `docs/results/c-correctness.png`.
- Scalar correctness output (CJ): `docs/results/scalar-correctness.png`.
- XMM correctness and size-check output (Immanuel):
  `docs/results/xmm-correctness.png` and `docs/results/xmm-checks.png`.
- YMM correctness and size-check output (Enzo):
  `docs/results/ymm-correctness.png` and `docs/results/ymm-checks.png`.

## Implementation notes

### C reference and harness

The C reference uses the specified comparison and index rule. The known-answer
case checks greater-than, less-than, and tie results. Size checks cover lengths
0–17, exercising every XMM and YMM remainder and multiple vector passes. Each
kernel's output is compared with C, and eight output canaries after the requested
length are checked for writes past the end.

The benchmark initializes A and B with pseudo-random values, warms each kernel
once, and times 30 calls with `QueryPerformanceCounter`. Initialization,
correctness checks, and sample printing are outside the timed interval. The full
output is checked against C in small chunks, avoiding a second pair of large
expected arrays. A, B, C, and `idx` use 16 bytes per element, so 2^30 elements
require 16 GiB.

Visual Studio builds C17 in both configurations. Debug uses `/Od`; Release uses
`/O2` and `/Oi`. Both target `/arch:AVX2`. The Release C reference was inspected
with `dumpbin` and contains an eight-float YMM loop plus a remainder path, so
performance comparisons use compiler-optimized C as the baseline.

The scaffold was validated on an AMD Ryzen 5 5600X (6 cores, 12 logical
processors), with 15.9 GiB of installed memory, Windows 11 Pro 10.0.26200,
Visual Studio 2026 / MSVC 14.51.36231, and NASM 3.02. Record any different
machine or toolchain beside the timing tables. **CJ:** relate the results
to the hardware, compiler settings, and generated C code.

### Scalar x86-64

The kernel follows the Windows x64 ABI. It receives n in RCX, A in RDX, B in R8,
and C in R9; it reads `idx` from `[rsp + 8*5]` at entry. Each iteration loads
one float from each input, compares them with `ucomiss`, writes the selected
float and 32-bit index, advances the pointers, and decrements the remaining
count. `jae` selects A when the values are equal. The scalar kernel passes the
known-answer and size checks in Debug and Release. **CJ:** compare its
measured performance with the other kernels after they are ready.

### SIMD XMM

**Immanuel:** describe the XMM implementation, tail handling, correctness
checks, and measured performance.

### SIMD YMM

**Enzo:** describe the YMM implementation, tail handling, `vzeroupper`,
correctness checks, and measured performance.

### Problems and solutions

**All contributors:** describe problems encountered, their fixes, and useful
observations from implementation and testing.

## Project contents

- `src/main.c` contains the verification and benchmark harness.
- `src/kernels.c` contains the C reference kernel.
- `src/max_index_scalar.asm` contains the scalar x86-64 kernel.
- `src/max_index_xmm.asm` and `src/max_index_ymm.asm` are the SIMD starter files.
- `docs/1261G01G1.png` is the stitched assignment-specification screenshot.
- `docs/results/` will contain the program-output screenshots embedded above.
