# NwLib
A small, dependency-free C++23 utility library made by Nikt-Wazny-PL. More-or-less a collection of helpers for C++ code.

## Sub-libraries

### `nwlib::fs` - Experimental
  `nwlib::fs` is an experimental set of filesystem utilities.
  
  * `nwlib::fs::path` - A simple `std::filesystem::path` alternative that can act as both an owning string and a non-owning view.

### `nwlib::mem`
  `nwlib::mem` is a collection of memory-management utilities.

  * `nwlib::mem::arena` - A simple arena/bump allocator.
  * `nwlib::mem::arena_box<T>` - A handle-based, reallocation-safe container for objects of type `T`. Also supports `T[]`.
  * `nwlib::mem::buffer` - A simple raw memory buffer containing a pointer, capacity, and ownership flag.
  * `nwlib::mem::box<T, TDestroyer>` - A simple `std::unique_ptr`-like owning pointer. There is no particular reason to use it over `std::unique_ptr`; it's mainly provided as part of the library's own memory utilities.
  * `nwlib::mem::ref<T>` - An intrusive, atomically reference-counted smart pointer.
  * `nwlib::mem::weak<T>` - A weak reference for `nwlib::mem::ref` objects.
  * `nwlib::mem::ref_counted` - Base class for objects managed by `nwlib::mem::ref`.

### `nwlib::utils`
  `nwlib::utils` is more-or-less a "random bullshit go" collection of utilities that don't fit neatly into another category.

  * `nwlib::utils::random` - A lightweight pseudo-random number generator based on MurmurHash64.
  * `nwlib::utils::uuid` - A 128-bit randomly generated identifier. It does not conform to any UUID standard.

## Supported Platforms
  Currently, NwLib officially supports:
  * Windows
  
  Linux support is currently in progress, with MacOS support being a possible future target.

## Requirements
  * C++23-compatible compiler
  * No external dependencies
