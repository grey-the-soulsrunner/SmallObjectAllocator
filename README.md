# SmallObjectAllocator

## Overview

This project implements a single-threaded, cache-line–aligned small object allocator designed for objects with sizes in the 16–64 byte range.

The allocator focuses on:
* predictable memory layout
* fast allocation/deallocation
* explicit control over alignment and fragmentation

It is intended as a learning and exploration project for low-level memory management, allocator design, and performance trade-offs in modern C++.

## Design Goals

* Reduce allocation latency compared to std::allocator for small, uniform object sizes
* Enforce cache-line alignment to improve cache locality and prepare for future multithreaded extensions
* Avoid general-purpose heap behavior (no coalescing, no best-fit / first-fit logic)
* Keep the design simple, explicit, and inspectable

## Non-goals:

* General-purpose replacement for malloc / new
* Thread safety (for now)
* Zero fragmentation

## Key Design Decisions
1. Type-Specific Allocator
The allocator is templated on T and manages memory only for objects of that type.

This allows:

* compile-time computation of slot size and alignment
* simpler layout guarantees
* no need to support heterogeneous allocations

2. Slot-Based Allocation

Memory is divided into fixed-size slots, where:

* each slot can hold either an object of type T or a FreeNode
* slot size ≥ max(sizeof(T), sizeof(FreeNode))
* slot size is aligned to the enforced alignment

This eliminates internal fragmentation within slots and enables O(1) allocation.

3. Cache-Line Alignment

All slots are aligned to cache-line boundaries (currently 64 bytes).

Why:

* improves spatial locality
* avoids false sharing when extended to multithreaded use
* simplifies reasoning about memory layout

Alignment is enforced at compile time using constexpr computations and static_assert.

4. Free List Embedded in Slots

Free slots are linked together using an intrusive free list, where:

* the free list node (FreeNode) is stored directly inside unused slots
* no external metadata or side allocations are required

This minimizes memory overhead and keeps allocation/deallocation constant-time.

5. Block-Based Growth

Memory is acquired in blocks (chunks) consisting of multiple slots.

Each block tracks:

* base address
* total capacity
* number of used slots

Blocks are never partially returned to the system; memory is reclaimed in batches, not per-object.

This trades memory efficiency for speed and simplicity.

## Alignment & Slot Computation

Slot parameters are computed at compile time:

* slot_align = max(alignof(T), alignof(FreeNode), cache_line_alignment)
* slot_size = align_up(max(sizeof(T), sizeof(FreeNode)), slot_align)

This guarantees:

* correct alignment for all stored objects
* valid storage for free list nodes
* predictable slot stride

### Current Limitations

* Single-threaded only
* No support for over-aligned types beyond the enforced alignment
* No per-object deallocation to the OS
* Fragmentation between blocks is possible
* These limitations are intentional for clarity and educational value.

### Possible Extensions

* Thread-safe variant using per-thread blocks or lock-free free lists
* Separate size classes for 16–64 and 64–128 byte objects
* NUMA-aware allocation
* Allocation statistics and instrumentation
* Benchmark suite against std::allocator and jemalloc

### Why This Project Exists

This allocator is not about reinventing malloc.

It exists to:

* deeply understand memory layout, alignment, and cache behavior
* reason about allocator trade-offs instead of blindly using STL defaults
* demonstrate low-level C++ competence through concrete, inspectable code

### Build Requirements

* C++17-compatible compiler
* No external dependencies

### Disclaimer

This project is for educational and experimental purposes.
It should not be used in production without careful review, benchmarking, and testing.