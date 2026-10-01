# Build and Run

Run these commands in a Visual Studio 2026 Developer PowerShell from the repository root. NASM must be on `PATH` for the assembly build steps.

```powershell
msbuild .\CSC612M-MCO1.sln /m /p:Configuration=Debug /p:Platform=x64
msbuild .\CSC612M-MCO1.sln /m /p:Configuration=Release /p:Platform=x64

.\out\x64\Debug\simd_bench.exe --verify
.\out\x64\Release\simd_bench.exe --verify

.\out\x64\Debug\simd_bench.exe --bench
.\out\x64\Release\simd_bench.exe --bench
```
