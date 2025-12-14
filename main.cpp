#include "SmallObjectAllocator.h"
#include <string>

int main (){
    SmallObjectAllocator<int[30]> smallInts;
    return 0;
}