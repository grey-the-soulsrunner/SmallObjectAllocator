#include "SmallObjectAllocator.h"
#include <string>
#include <vector>

int main (){
    SmallObjectAllocator<int> smallInts;
    SmallObjectAllocator<int>::FreeNode* start = smallInts.free_list_;
    int* iptr = smallInts.allocate();
    *iptr = 1532;
    int* iptr2 = smallInts.allocate();
    *iptr2 = 12354;
    std::cout << "here is out integer: " << *iptr << std::endl;
    smallInts.deallocate(iptr);
    smallInts.deallocate(iptr2);
    iptr = nullptr;
    iptr2 = nullptr;
    return 0;
}