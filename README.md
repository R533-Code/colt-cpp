## Colt-Cpp:
Contains helpers used throughout the Colt Compiler.
These include portable bit-fields, endianness utilities, io, argument parsing and data structures.

![Build and Test on Linux (x86_64)](https://github.com/R533-Code/colt-cpp/actions/workflows/cmake-linux-x86_64.yml/badge.svg)
![Build and Test on Linux (ARMv8)](https://github.com/R533-Code/colt-cpp/actions/workflows/cmake-linux-arm/badge.svg)
![Build and Test on Windows (x86_64)](https://github.com/R533-Code/colt-cpp/actions/workflows/cmake-windows-x86_64.yml/badge.svg)
[![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/R533-Code/colt-cpp/badge)](https://scorecard.dev/viewer/?uri=github.com/R533-Code/colt-cpp)
![CodeQL](https://github.com/R533-Code/colt-cpp/actions/workflows/codeql.yml/badge.svg)


## Implemented Features:
- ✅ Feature is done and tested.
- ⚠️ Feature is nearly done (more testing needed).
- ❌ Feature is not ready for use.


| Name                            | Description                                                                                                                  | Status | Note                                                                                     |
| ------------------------------- | ---------------------------------------------------------------------------------------------------------------------------- | ------ | ---------------------------------------------------------------------------------------- |
| `Bitfields`                     | Zero-overhead bitfields whose layout is guarenteed to be portable (except for endianness).                                   | ✅      | Supports hashing and serialization.                                                      |
| `choose_simd_function`          | Used to choose a different function at runtime depending on `CPUID`                                                          | ✅      | If needed, detecting more instructions sets could be added.                              |
| `TargetEndian` Endian Detection | Represents the target endianness.                                                                                            | ✅      | There is also the `COLT_LITTLE_ENDIAN` and `COLT_BIG_ENDIAN` macros.                     |
| Endian Conversions              | Functions such as `[hlb]to[hlb]` to go from host endianness to another endianness or vice-versa.                             | ✅      | Implemented using intrinsics.                                                            |
|                                 |                                                                                                                              |        |
| `uhash` and `hash_append`       | Implements universal hashing as described in [`N3980`](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3980.html). | ⚠️      | The current implemented `hash_algorithm`s are `fnv1a` and `SipHash-2-4`.                 |
|                                 |                                                                                                                              |        |
| `Option`                        | Optional vocabulary type with monadic operations.                                                                            | ✅      | More unit tests could be added.                                                          |
| `Expect`                        | Expected vocabulary type with monadic operations                                                                             | ✅      | More unit tests could be added.                                                          |
|                                 |                                                                                                                              |        |
| Unicode Counting and Indexing   | Provides `countlen`, `unitlen`, `strlen`, `index_front`, `index_back` Unicode aware functions.                               | ✅      | More unit tests could be added.                                                          |
| Unicode SIMD Utilities          | Provides SIMD functions to optimize `strlen` and `unitlen`.                                                                  | ⚠️      | For now, `x86_64` versions are provided with little to no `ARM NEON` support.            |
| Unicode Aware `StringView`      | View over Unicode data in any of `UTF8`, `UTF16-[BL]E`,`UTF32-[BL]E`.                                                        | ✅      | A type-erased `StringView` could also be added, whose encoding is determined at runtime. |
| Unicode Aware `String`          | Contiguous Unicode aware `String` with `SSO`, `count` and `middle` caching, and const segment optimization.                  | ❌      | The implementation is a work in progress.                                                |
|                                 |                                                                                                                              |        |
| Memory Allocators               | Provides a framework of composable allocators that allocates and deallocates `MemBlock`                                      | ⚠️      | More allocators could be added.                                                          |
| Memory Allocators Reference     | Reference to a local or global allocator, used by all data structures.                                                       | ❌      | Not implemented yet.                                                                     |
|                                 |                                                                                                                              |        |
| `BigInteger`                    | Arbitrary precision big integer.                                                                                             | ✅      | More unit tests could be added. *                                                        |
| `BigRational`                   | Arbitrary precision big rational (`x/y`).                                                                                    | ⚠️      | Some API improvements could be done. *                                                   |
|                                 |                                                                                                                              |        |
| Compile-time `Map` and `BiMap`  | Hash (bi)map created at compile-time.                                                                                        | ✅      |                                                                                          |
| Compile-time Math Functions     | `constexpr` mathematical functions.                                                                                          | ✅      |                                                                                          |
| Overflow checks                 | Provides `checked_(add\|sub\|mul\|div\|mod)` to detect overflow/underflow.                                                   | ⚠️      | More unit tests are needed.                                                              |
|                                 |                                                                                                                              |        |
| Macro `FOR_EACH`                | Macro that expands another macro for each of its arguments.                                                                  | ✅      | For `MSVC`, this macro needs `/Zc:preprocessor`.                                         |
| Macro `assert_true`             | Assert that all conditions are true.                                                                                         | ✅      | Works at compile time.                                                                   |
| Macro `switch_no_default`       | Provides an unreachable default branch for a switch statement.                                                               | ✅      |                                                                                          |
|                                 |                                                                                                                              |        |
| OS `DynamicLib`                 | Represents a dynamically loaded library in a platform agnostic way.                                                          | ✅      | For Linux, `-Wl,-export-dynamic` must be specified for self-introspection.               |

\* These classes are wrappers over [`GMP`](https://gmplib.org/), which makes them already well-tested.

## Unicode SIMD Utilities:
This project uses `simdutf` to improve the performance of Unicode-related utilities.
However, more functions were needed, and I had to roll out my own SIMD versions in some cases.

- ✅ Function is written and tested.
- ⚠️ Function is written but not tested.
- ❌ Function is not written.

All `x86_64` SIMD functions are tested using [`sde`](https://www.intel.com/content/www/us/en/developer/articles/tool/software-development-emulator.html).
All `NEON` SIMD functions are tested using [`QEMU`](https://www.qemu.org/).

|        | `unitlen16` | `unitlen32` | `strlen8` | `strlen16` |
| ------ | ----------- | ----------- | --------- | ---------- |
| SSE2   | ✅           | ✅           | ✅         | ✅          |
| SSE4.2 | ❌           | ❌           | ❌         | ❌          |
| AVX2   | ✅           | ✅           | ✅         | ✅          |
| AVX512 | ✅           | ✅           | ✅         | ✅          |
| NEON   | ✅           | ✅           | ❌         | ❌          |