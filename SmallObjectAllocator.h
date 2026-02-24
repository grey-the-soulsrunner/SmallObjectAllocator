#pragma once
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <initializer_list>
#include <new>
#include <memory>
#include <vector>
#include <iostream>

// aligns up the input number to values of power of two by disregarding lower bits
// bitwise operator is used to avoid floating point and be precise with big numbers
constexpr std::size_t align_up(std::size_t value, std::size_t alignment){ return (value + alignment - 1) & ~(alignment - 1); };

template <typename T, std::size_t ChunkSize = 4096>
class SmallObjectAllocator{

// stride invariant definition which defines the minimum size of a chunk in which a T type object is allocated
// this a design choice to make the allocator cache line aligned
static constexpr std::size_t kCacheLineAlignment = 64;
// static assertion to check if the alignment is power of two
static_assert((kCacheLineAlignment & (kCacheLineAlignment - 1)) == 0, "Cache-line alignment must be power of two!");
// static assetion that checks the division by 64
static_assert(ChunkSize % kCacheLineAlignment == 0, "ChunkSize must be divisible by the alignment value!");
// temporary design decision to ignore types bigger than the alignment value
static_assert(sizeof(T) <= kCacheLineAlignment, "Types biger than the alignment value are not supported yet!");

public:
    // default constructor that allocates buffer of ChunkSize in the Block object
    // free_list_ is constructued in the newly allocated memory
    SmallObjectAllocator(){
        blocks_[0].base = new std::byte[ChunkSize];
        free_list_ = new(blocks_[0].base) FreeNode();
    }
/* ~SmallObjectAllocator(){
        release_all_blocks;
    } */
   
   
    // allocation and deallocation of T type objects
    T* allocate();
    void deallocate(T* ptr);

    // a free-list class
    struct FreeNode{
        FreeNode* next = nullptr;
    };
    // class that holds the raw memory and counts used slots
    struct Block{
        std::byte* base = nullptr; // start of raw memory
        std::size_t capacity = ChunkSize / kCacheLineAlignment; // number of slots
        std::size_t used = 0; // number of allocated slots
    };

    FreeNode* free_list_; // base of the free list

    // vector that holds all the blocks created
    std::vector<Block> blocks_{Block()};

    private:
    // computing the sizes of slot sizes and alignment avoiding intializer_list max
    static constexpr std::size_t SlotAlignment = std::max(
                                                    std::max(alignof(T), alignof(FreeNode)),
                                                    kCacheLineAlignment);

    static_assert(SlotAlignment != 0, "Slot alignment can't be zero!");
    
    static_assert((SlotAlignment & (SlotAlignment - 1)) == 0, "Slot alignment must be power of two!");

    static constexpr std::size_t SlotSize = align_up(
        std::max(sizeof(T), sizeof(FreeNode)),
        SlotAlignment
        );
    void release_all_blocks();
};