#include <iostream>

class CountingHeap {};
class StandardHeap {};
class MedianHeap {};

#include <utility>


#include "oString.h"

int main()
{

    {
        oString os = oString("Short string..........ab");
        std::cout << os.c_str() << ", size: " << os.size() << "\n";
    }

}
