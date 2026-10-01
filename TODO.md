# MCO1 TODO

Checklist for the assignment deliverables. Keep this file updated as the work is completed.

## Kernels and verification

- [x] Define the shared kernel interface and C reference implementation.
- [x] Implement the scalar x86-64 kernel.
- [x] Add known-answer checks, comparisons against C, and size checks for lengths 0–17 with output canaries.
- [x] Add kernel-only timing with one warm-up and 30 measured calls for 2^20, 2^26, and 2^28 elements.
- [x] Verify the C and scalar kernels pass known-answer and size checks in Debug and Release.
- [ ] Implement the XMM kernel, including tail handling; add it to the harness after it passes the checks. **Owner: Immanuel.**
- [ ] Implement the YMM kernel, including tail handling and `vzeroupper`; add it to the harness after it passes the checks. **Owner: Enzo.**
- [ ] Build and run all four kernels in Debug and Release; check every output against C.
- [x] Use 2^28 as the large-size fallback and document that the four arrays at 2^30 require 16 GiB.

## Results and report

- [ ] Run each completed kernel at every required size in Debug and Release; record average execution times.
- [ ] Fill the timing and speedup tables in `README.md`.
- [ ] Add and embed Debug and Release benchmark-output screenshots showing all four kernels.
- [ ] Add and embed correctness screenshots for C, scalar x86-64, XMM, and YMM.
- [ ] Add and embed XMM and YMM size-check screenshots.
- [ ] Analyze the measured speedups, compiler-generated C SIMD, memory traffic, and differences from theoretical expectations.
- [ ] Document implementation problems, fixes, and useful observations.
- [ ] Check that the README contains the complete report and embeds repository images inline.

## Project delivery

- [x] Keep the Visual Studio 2026 x64 Debug/Release solution, source files, and build commands in the repository.
- [x] Save the stitched assignment screenshot as `docs/1261G01G1.png`.
- [x] Confirm the screenshot filename's group number is correct.
- [ ] Upload the specification screenshot to the quiz using the required group filename; do not upload the main deliverables there.
- [ ] Make the completed repository accessible to the instructor and submit the main deliverables through the designated SIMD project submission.
