# MCO1 TODO

Checklist for the assignment deliverables, based on the current saved README,
source files, benchmark logs, and embedded screenshots. Status indicates available
evidence, not an awarded rubric score.

## Kernels and verification

- [x] Define the shared kernel interface and C reference implementation.
- [x] Implement the scalar x86-64 kernel.
- [x] Add known-answer checks, comparisons against C, and size checks for lengths 0–17 with output canaries.
- [x] Add kernel-only timing with one warm-up and 30 measured calls for 2^20, 2^26, and 2^28 elements.
- [x] Verify the C and scalar kernels pass known-answer and size checks in Debug and Release.
- [x] Implement the XMM kernel with tail handling and register it in the harness. **Owner: Immanuel.**
- [x] Implement and register the YMM kernel with tail handling and `vzeroupper`. **Owner: Enzo.**
- [x] Verify all four kernels' known-answer and size checks in GCC-built Debug and Release; compare outputs with C and check canaries for lengths 0–17.
- [x] Use 2^28 as the large-size fallback and document that the four arrays at 2^30 require 16 GiB.

Current verification and benchmark evidence uses GCC/MinGW 15.1.0 Debug (`-O0`)
and Release (`-O2 -DNDEBUG`). The Visual Studio solution builds with `msbuild`
in x64 Debug and Release and passes `--verify` for all four kernels (checked
2026-10-13); the recorded measurements remain the GCC results.

## Results and report

- [x] Run all four kernels at every selected size in GCC Debug and Release; record 30-run average execution times.
- [x] Fill the timing and speedup tables in `README.md` from the latest saved logs.
- [x] Add and embed Debug and Release benchmark-output screenshots showing all four kernels.
- [x] Add and embed correctness screenshots for C, scalar x86-64, XMM, and YMM.
- [x] Add and embed XMM and YMM size-check screenshots.
- [x] Analyze measured speedups, actual GCC-generated C code, memory traffic, theoretical expectations, and measurement limitations.
- [x] Document the CHAN contribution's YMM methodology, analysis challenge, and useful observations.
- [x] Include all required report sections and embed repository images inline; remaining factual consistency work is listed below.

## Assignment #8

| Item | Requirement | Status |
| --- | --- | --- |
| a.i | Program-output screenshots with execution times for all cases | Complete |
| a.ii | Comparative execution-time table and performance analysis | Complete |
| a.iii | C correctness screenshot | Complete |
| a.iv | Scalar x86-64 correctness screenshot | Complete |
| a.v | SIMD XMM correctness screenshot | Complete |
| a.vi | SIMD YMM correctness screenshot | Complete |
| a.vii | XMM and YMM boundary-check screenshot | Complete |
| a.viii | Explain fastest kernels, performance differences, and theoretical versus measured speedups | Complete |
| a.ix | Discuss problems, solutions, methodology, and useful observations | Complete: CHAN contribution is saved |
| b | Source and project files needed to load and execute the program | Present and included in the previously verified push |

Combined screenshots cover multiple requirements. Current benchmark evidence
uses the GCC configurations described above.

## Rubric

| Criterion | Points | Evidence status |
| --- | ---: | --- |
| C main with initialization and correct calls/parameter passing | 5 | Covered |
| Display timing and correctness: C | 10 | Covered |
| Display timing and correctness: scalar x86-64 | 10 | Covered |
| Display timing and correctness: SIMD XMM | 10 | Covered |
| Display timing and correctness: SIMD YMM | 10 | Covered |
| Boundary handling | 10 | Covered |
| Comparative results table | 5 | Covered |
| Analysis of results | 20 | Covered |
| Discussion of implementation | 10 | Kernel explanations and CHAN observations present |
| Completeness of uploaded files | 5 | Project, sources, logs, and screenshots were verified pushed; latest README/TODO updates still need pushing |
| Following instructions | 5 | Final factual review and submission pending; recorded benchmarks use GCC |

## Final factual review

- [x] Reconcile the old MSVC/dumpbin eight-float C discussion with the actual GCC scalar C baseline used for the recorded results. **Shared report / C-section owner.**
- [x] Correct the XMM mask example to `0xFFFFFFFF >> 31 = 1`, and describe the XMM tail as a scalar loop with the scalar kernel's logic rather than the same implementation. **XMM-section owner: Immanuel.**
- [x] Remove the outdated YMM statement that measurements are pending and the timing tables are empty. **YMM-section owner: Enzo.**
- [x] Remove the harness paragraph's CJ prompt.
- [ ] Replace the Problems section's "All contributors" prompt with the CJ and Immanuel entries. **CJ and Immanuel.**
- [ ] Add Enzo's CPU, RAM, and OS to the README if he sends them. The assignment requires neither machine specifications nor his GCC commands; `build-commands.md` stays as the Visual Studio build instructions. **Shared report.**

## Project delivery

- [x] Keep the Visual Studio 2026 x64 Debug/Release solution, source files, and build commands in the repository.
- [x] Save the stitched assignment screenshot as `docs/1261G01G1.png`.
- [x] Confirm the screenshot filename's group number is correct.
- [x] Verify that the implementation, timing logs, and embedded screenshots were pushed in commit `c0b2c12`.
- [ ] Commit and push the latest README corrections/CHAN observations and this updated checklist.
- [ ] Upload the specification screenshot to the quiz using the required group filename; do not upload the main deliverables there.
- [ ] Confirm instructor access and submit the main deliverables through the designated SIMD project submission.
