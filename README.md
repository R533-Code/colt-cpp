## Colt-Cpp:
Contains helpers used throughout the Colt Compiler.
These include portable bit-fields, endianness utilities, io, argument parsing and data structures.

![Build and Tests](https://github.com/R533-Code/colt-cpp/actions/workflows/cmake-multi-platform.yml/badge.svg)

## Implemented Features:
- ✅ Feature is done and tested.
- ⚠️ Feature is nearly done (more testing needed).
- ❌ Feature is not ready for use.


|Name|Status|Note|
|----|------|----|
|Portable Bitfields|✅| |
|Detect SIMD Support|✅|If needed, detecting more instructions sets could be added.|
|Endian Detection|✅|Through macro or enum.|
|Endian Conversions|✅|Implemented using intrinsic|
|`Option`|⚠️| |
|`Expect`|⚠️| |
|Unicode Counting and Indexing|⚠️| API still needs some refactoring.|
|Unicode SIMD Utilities|⚠️| |
|Unicode Aware `StringView`|✅| |
|Unicode Aware `String`|❌| |
|Improved Memory Allocators|⚠️| |
|Global or Local Allocator Reference|❌| |
|Big Integer|⚠️| |
|Big Rational|TODO| |
|Compile-time Map and BiMap|✅| |
|Compile-time Math Functions|✅| |
|Overflow checks|⚠️| |
|Macro FOR_EACH|✅| |
|Macro assert_true|✅| Works at compile time.|
|Macro switch_no_default|✅| |