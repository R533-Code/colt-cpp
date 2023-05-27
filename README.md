# COLT-CPP:
C++ utilities with the main purpose of being used by the `Colt` compiler.

![Clang15](https://github.com/R533-Code/colt-cpp/actions/workflows/cmake_clang15.yml/badge.svg)
![Clang16](https://github.com/R533-Code/colt-cpp/actions/workflows/cmake_clang16.yml/badge.svg)
![g++12](https://github.com/R533-Code/colt-cpp/actions/workflows/cmake_g++12.yml/badge.svg)
![MSVC](https://github.com/R533-Code/colt-cpp/actions/workflows/cmake_msvc.yml/badge.svg)

## List of Utilities:
### io:
Helpers to read/write to the console.
- `input.h`: Console input facilities
- `print.h`: Console output facilities (using `{fmt}`)
### math:
`constexpr` mathematical functions and helpers.
- `math.h`: `constexpr` math utilities

### mem:
Efficient and composable allocators.
- `allocator_traits.h`: Allocator concepts and helpers
- `block.h`: Result of an allocation
- `composable_alloc.h`: Advanced composable allocators
- `global_alloc.h`: Default allocator all `colt-cpp` data structures use
- `simple_alloc.h`: Simple allocators building blocks
- `sizes.h`: Byte size helpers

### meta:
Compile-time and meta-programming utilities.
- `rtti.h`: Custom fast Run Time Type Informations
- `string_literal.h`: Used to pass string literals as templates parameters
- `traits.h`: Helper traits and meta-programming utilities.
- `type_list.h`: Compile-time type list

### refl:
Reflection utilities.
- `enum.h`: Magic enum using `DECLARE_ENUM_WITH_TYPE`
- `refl.h`: Reflection utilities for types

### structs:
Data structures and helpers.
- `expect.h`: `Expect<Exp, Err>` type to represent an expected result
- `option.h`: `Option<T>` type to represent an optional value
- `span.h`: `Span<T>` type to represent owning or non owning views over objects
- `vector.h`: Allocator aware dynamic array

### util:
- `assert_true.h`: `assert_true` assertions that work at compile-time
- `benchmark.h`: Benchmarking utilities
- `contracts.h`: `COLT_PRE` and `COLT_POST` macro to create contracts
- `debug_level.h`: Debugging level of the library
- `macro.h`: Macro utilities (`COLT_FOR_EACH`...)
- `on_exit.h`: `ON_SCOPE_EXIT` macro to perform an action on scope exit
- `typedefs.h`: Type definitions used throughout the library
- `volatile.h`: For volatile load/stores
