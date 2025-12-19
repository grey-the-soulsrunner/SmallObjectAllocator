#include "SmallObjectAllocator.h"

std::size_t align_up(std::size_t value, std::size_t alignment){
    // this function returns the closest to the value number divisible by two
    // bitwise operator is used to avoid floating point and be precise with big numbers
    return (value + alignment - 1) & ~(alignment - 1);
}