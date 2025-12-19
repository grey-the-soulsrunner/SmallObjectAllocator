#pragma once
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <new>
#include <memory>
#include <vector>
#include <iostream>

std::size_t align_up(std::size_t, std::size_t);

template <typename T, std::size_t ChunkSize = 4096>
class SmallObjectAllocator{

friend std::size_t align_up(std::size_t, std::size_t);

public:
    SmallObjectAllocator(){
        compute_slot_parameters();
    }
    /* ~SmallObjectAllocator(){
        release_all_blocks;
    } */


    // allocation and deallocation of T type objects
    T* allocate();
    void deallocate(T* ptr);

private:
    // computing the sizes of slot sizes and alignment
    std::size_t slot_size_;
    std::size_t slot_align_;

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

    FreeNode* free_list_ = nullptr;

private:
    void compute_slot_parameters(){
        // here we use align_up function to enforce the cache-line alignment
        // then respecting the alignment we compute slot size using the same function as the logic remains
        slot_align_ = align_up(alignof(T), 64ULL);
        slot_align_ = align_up(alignof(FreeNode), slot_align_);
        slot_size_ = std::max(sizeof(T), sizeof(FreeNode));
        slot_size_ = align_up(slot_size_, slot_align_);
    }
    void release_all_blocks();
};