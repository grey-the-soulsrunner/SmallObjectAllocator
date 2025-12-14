#include "SmallObjectAllocator.h"

std::size_t align_up(std::size_t typeAligned, std::size_t numberAligned){
    // this function compares the provided alignment with the required number
    // if it is bigger than the required it returns the closest upper multiple
    if( typeAligned <= numberAligned ) return numberAligned;
    else return (numberAligned * std::ceil(static_cast<double>(typeAligned) / numberAligned));
}