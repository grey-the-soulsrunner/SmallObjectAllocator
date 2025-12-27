#pragma once
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <initializer_list>
#include <new>
#include <memory>
#include <vector>
#include <iostream>

// aligns up the input number to values divided by two by disregarding lower bits
// bitwise operator is used to avoid floating point and be precise with big numbers
constexpr std::size_t align_up(std::size_t value, std::size_t alignment){ return (value + alignment - 1) & ~(alignment - 1); };

template <typename T, std::size_t ChunkSize = 4096>
class SmallObjectAllocator{

private:
// stride invariant is 64 for the slots
static constexpr std::size_t kCacheLineAlignment = 64;
// static assertion to check if the alignment is power of two
static_assert((kCacheLineAlignment & (kCacheLineAlignment - 1)) == 0, "Cache-line alignment must be power of two!");

public:
SmallObjectAllocator(){
}
/* ~SmallObjectAllocator(){
    release_all_blocks;
    } */
   
   
   // allocation and deallocation of T type objects
   T* allocate();
   void deallocate(T* ptr);
   
   // class that holds the raw memory and tracks free slots in itself
   struct Block{
       std::uint8_t* memory = nullptr;
       std::size_t used = 0;
    };
    
    // vector that holds all the blocks created
    std::vector<Block> blocks_;
    
    // a free-list class and variable
    struct FreeNode{
        FreeNode* next;
    };
    
    private:
    // computing the sizes of slot sizes and alignment avoiding intializer_list max
    static constexpr std::size_t SlotAlignment = std::max(
                                                    std::max(alignof(T), alignof(FreeNode)),
                                                    kCacheLineAlignment);

    static_assert(SlotAlignment != 0, "Slot alignment can't be zero!");
    static_assert(SlotAlignment & (SlotAlignment - 1) == 0, "Slot alignment must be power of two!");

    static constexpr std::size_t SlotSize = align_up(
     std::max(sizeof(T), sizeof(FreeNode)),
     SlotAlignment
     );

    FreeNode* free_list_ = nullptr;

private:
    void release_all_blocks();
};