# NwLib
A small, dependency-free C++23 utility library made by Nikt-Wazny-PL. More-or-less a collection of helpers for C++ code.

## Sub-libraries

### `nw::stl` - Experimental


### `nw::mem`
  `nw::mem` is a collection of memory-management utilities.

  * `nw::mem::arena` - A simple arena/bump allocator.
  * `nw::mem::arena_box<T>` - A handle-based, reallocation-safe container for objects of type `T`. Also supports `T[]`.
  * `nw::mem::buffer` - A simple raw memory buffer containing a pointer, capacity, and ownership flag.
  * `nw::mem::box<T, TDestroyer>` - A simple `std::unique_ptr`-like owning pointer. There is no particular reason to use it over `std::unique_ptr`; it's mainly provided as part of the library's own memory utilities.
  * `nw::mem::ref<T>` - An intrusive, atomically reference-counted smart pointer.
  * `nw::mem::sync<T>` - A wrapper `nw::mem::ref` and a `std::mutex` that gurantees thread-safe acceses.
  * `nw::mem::weak<T>` - A weak reference for `nw::mem::ref` objects.
  * `nw::mem::ref_counted` - Base class for objects managed by `nw::mem::ref`.

### `nw::utils`
  `nw::utils` is more-or-less a "random bullshit go" collection of utilities that don't fit neatly into another category.

  * `nw::utils::random` - A lightweight pseudo-random number generator based on MurmurHash64.
  * `nw::utils::uuid` - A 128-bit randomly generated identifier. It does not conform to any UUID standard.

## Supported Platforms
  Currently, NwLib officially supports:
  * Windows
  
  Linux support is currently in progress, with MacOS support being a possible future target.

## Requirements
  * C++23-compatible compiler
  * No external dependencies
