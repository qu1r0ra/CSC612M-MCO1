# CSC612M MCO1

This project compares four implementations of element-wise maximum with index
tracking: C, scalar x86-64, SIMD XMM, and SIMD YMM. The kernels share the
interface in [`src/kernel_contract.h`](src/kernel_contract.h). All four kernels
are registered in `src/main.c` and included in correctness checks and benchmarks.

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

## Program output

### Debug benchmark output

All four kernels at 2^20, 2^26, and 2^28 elements, showing correctness checks,
average kernel execution times over 30 runs, and the first and last five
maximum values and indices.

![Debug benchmark timings, correctness, and sample outputs for all four kernels](docs/results/Screenshot%201a%20-%20All%20debug%20results.png)

### Release benchmark output

All four kernels at the same three vector sizes, with correctness checks,
30-run average kernel execution times, and the first and last five maximum
values and indices.

![Release benchmark timings, correctness, and sample outputs for all four kernels](docs/results/Screenshot%201b%20-%20All%20release%20results.png)

### Correctness and boundary checks

Debug and Release known-answer checks for C, scalar x86-64, SIMD XMM, and SIMD
YMM. Size checks cover lengths 0–17, including every XMM and YMM remainder,
with output canaries checked for writes beyond the requested length.

![Debug and Release correctness and boundary checks for all four kernels](docs/results/Screenshot%201c%20-%20All%20correctness%20checks.png)

## Results

Average kernel-call times in microseconds are taken from
[`debug-results.txt`](debug-results.txt) and
[`release-results.txt`](release-results.txt), with 30 measured runs per case.
These results use GCC/MinGW 15.1.0; the project records Debug as `-O0` and
Release as `-O2 -DNDEBUG`. Speedup is the C average divided by the corresponding
kernel average, rounded to three decimal places. Values below 1 mean slower
than C.

### Debug

|    n |      C (µs) | Scalar x86-64 (µs) | SIMD XMM (µs) | SIMD YMM (µs) |
| ---: | ----------: | -----------------: | ------------: | ------------: |
| 2^20 |    5128.107 |           3828.083 |       273.503 |       209.400 |
| 2^26 |  322215.940 |         243788.150 |     32941.730 |     31388.833 |
| 2^28 | 1296980.887 |         987743.473 |    128155.327 |    124633.217 |

### Release

|    n |     C (µs) | Scalar x86-64 (µs) | SIMD XMM (µs) | SIMD YMM (µs) |
| ---: | ---------: | -----------------: | ------------: | ------------: |
| 2^20 |    661.510 |           3861.507 |       241.837 |       199.627 |
| 2^26 |  44598.540 |         242299.153 |     31567.547 |     31346.467 |
| 2^28 | 174270.850 |        1013190.390 |    134602.320 |    132083.837 |

### Speedup relative to C

| Configuration |    n | Scalar x86-64 | SIMD XMM | SIMD YMM |
| ------------: | ---: | ------------: | -------: | -------: |
|         Debug | 2^20 |        1.340x |  18.750x |  24.490x |
|         Debug | 2^26 |        1.322x |   9.781x |  10.265x |
|         Debug | 2^28 |        1.313x |  10.120x |  10.406x |
|       Release | 2^20 |        0.171x |   2.735x |   3.314x |
|       Release | 2^26 |        0.184x |   1.413x |   1.423x |
|       Release | 2^28 |        0.172x |   1.295x |   1.319x |

As shown in the table above, utilizing the **YMM registers** is the fastest in **all measured cases**. Comparing XMM and YMM, XMM processes four floats per iteration whereas YMM process eight floats per iteration with the same sequence of operations, which means YMM executes fewer vector-loop iterations. The speedup advantage is strongest at 2^20: YMM is 1.306 times faster than XMM in Debug and 1.211 times faster in Release.

However, despite YMM taking in double the number of floats, as shown, the highest speedup advantage is x1.306 and not a x2 speedup. This is becuase both implementations read and write two arrays each and move the same total data. The combined working set of these four arrays is 16 MiB, 1 GiB, and 4 GiB for vector lengths of 2^20, 2^26, and 2^28, respectively. At larger sizes, the small XMM/YMM gap is consistent with memory throughput limiting performance. In essence, while YMM can process more floats at a time, only those operations have increased performance, but not the data trasnfers to the array.

Release optimization also improves the C baseline, with Release C being more than 7x faster than Debug C at all three sizes of n. Inspection of the actual GCC Release executable shows a loop processing one element per iteration, with conditional moves and mask operations replacing the data-dependent selection branch. It does not contain an eight-element YMM loop. Debug C retains the branch and performs more stack accesses and address calculations.

Scalar assembly also improves on Debug C with a x1.3 speedup but **not** on on Release C which performs on x0.17-0.18 speed compared to Release C. The scalar assembly branches on each comparison. Because A and B are independently generated random values, the branch outcome varies frequently; branch mispredictions are a plausible contributor. The optimized C implementation avoids that selection branch, demonstrating why hand-written assembly is not automatically faster.

At n = 2^20, XMM and YMM achieve speedups of 18.750× and 24.490× over Debug C, exceeding their respective lane counts of four and eight. These gains combine packed processing, fewer loop iterations, and branchless selection with reduced overhead compared with unoptimized C. Debug C performs more stack accesses and address calculations and retains a data-dependent selection branch. Therefore, lane count alone does not predict the overall speedup. Release optimization reduces the C baseline’s overhead, resulting in smaller SIMD speedups of 2.735× for XMM and 3.314× for YMM at the same size.

Finally, as shown in the screenshots, all recorded correctness checks pass. Timed runs do not have scalar remainders since all n are divisible by 8. Lengths 0-17 provide separate boundary-check evidence.

Do note that measurements have some limitations. While each reported time averages 30 calls, individual timings and statistics that measure variability of data is not recorded, so small differences like YMM's advantage over XMM at n = 2^26 cannot be established as statistically significant. The assembly instructions are identical between Debug and Release builds, so differences in their timings cannot be attributed to compiler optimization of those routines. The configurations also use separately generated inputs, and kernels execute in a fixed order. Memory bandwidth and branch mispredictions are plausible explanations for the observed patterns, but were not directly measured.

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

Similar to the `Scalar x86-64` implementation, it follows the Windows x64 ABI. Each 
iteration loads four single-precision floating-point values from A and B into `xmm0`
and `xmm1`. The elements are compared using `vmaxps` to extract the higher-value elements
and are loaded into `xmm3` which is stored into the destination array pointed by `r9`. 
The index value extraction is performed separately through the use of `vcmpgtps` 
such that if B > A it returns -1, else it returns 0. To transform the -1 values into 1, 
`vpsrld` was used to shift each element by 31 bits (i.e. 0xFFFF -> 0x0001 and 
0x0000 -> 0x0000). Finally, the resulting packed data is stored into the index 
array pointed to by `r10` which stores the resulting idx elements. 

In regard to tail handling, this is handled by iteratively checking whether the remaining
number of elements to be processed is greater than or equal to four. When the remaining number of
elements is less than four, the remaining values are processed using the same implementation
in `Scalar x86-64`. The SIMD XMM kernel passes the known-answer and size checks in Debug and Release. 
**Immanuel:** describe the XMM implementation, tail handling, correctness
checks, and measured performance.

### SIMD YMM

The YMM kernel follows the Windows x64 ABI and uses only volatile registers.
Each vector iteration loads eight unaligned floats from A and B, stores their
packed maximum, compares B > A for the index mask, and shifts the mask right by
31 bits to store eight 0/1 indices. The loop advances by eight elements; array
addresses remain scaled by four bytes per element. A scalar remainder handles
zero to seven elements with VEX-encoded scalar loads, compare, and stores, so
no operation accesses beyond n. The return path executes `vzeroupper`.

The YMM kernel is registered alongside C, scalar x86-64, and XMM, so
`--verify` and `--bench` include all four. NASM assembly and runtime
verification passed in GCC-built x64 Debug (`-O0`) and Release (`-O2 -DNDEBUG`)
configurations: each kernel passed the known-answer case and lengths 0–17 with
output canaries. The repository's documented MSBuild build could not be run
because Visual Studio and `msbuild` are unavailable in the environment.
Performance measurements are still pending; timing tables remain empty until
benchmark runs are available.

### Problems and solutions

**All contributors:** describe problems encountered, their fixes, and useful
observations from implementation and testing.

UMALI:
When I was assigned to work on the XMM implementation, I already had the idea that a mask would probably be useful for our task. Initially, I looked into our current scalar implementation, and I noticed that in the case of SIMD instructions, a `vmax` instruction instantly simplifies the calculation process needed. As such, the process of handling the symmetric operation became clearer to me, which resulted in just having to figure out how to map the elements into the proper register partitions. In regard to tackling the tail elements, or the elements that are left due to not fitting the allowed four 32FP per XMM register, I considered two main approaches: (1) to continue with a purely vectorized approach by redoing the calculation on the last few remaining elements in the input or (2) to simply adapt the scalar code to solve it. Ultimately, I realized that going with the former would entail additional complexity and did not bear that much weight considering that in any case, the maximum amount of remaining tail elements would at most be three, which resulted in me simply copying the scalar implementation to handle this. 

CHAN: 
When I stared working on this project hands-on, XMM was already implemented and that alone already acted as a working reference for the YMM implementation. I simply adapted it to process eight elements per iteration while retaining four-byte address scaling for each float and int idx. 

As for the analysis, the main challenge was explaining why doubling the vector width did not double performance. YMM’s advantage over XMM decreased at larger vector sizes, which is consistent with memory throughput limiting the benefit of wider processing. Another useful observation was that optimized Release C outperformed the scalar assembly. Inspection showed that GCC removed the C implementation’s data-dependent selection branch, while the scalar assembly retained it. These findings showed that register width and implementation language alone do not determine performance.

## Project contents

- `src/main.c` contains the verification and benchmark harness.
- `src/kernels.c` contains the C reference kernel.
- `src/max_index_scalar.asm` contains the scalar x86-64 kernel.
- `src/max_index_xmm.asm` and `src/max_index_ymm.asm` contain the SIMD kernels.
- `docs/1261G01G1.png` is the stitched assignment-specification screenshot.
- `docs/results/` contains the program-output screenshots embedded above.
