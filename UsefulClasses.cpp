#include <iostream>

class CountingQueue {}; // A priority queue but all the elements are unique
class StandardHeap {};
class MedianHeap {};

#include <utility>


#include "oString.h"

int main()
{

    {
        oString os = oString("Short string..........ab");
        std::cout << os.c_str() << ", size: " << os.size() << "\n";
        const int size = os.size();
        for (int i = 0; i < size; i++) {
            os.pop_back();
            std::cout << os.c_str() << ", size: " << os.size() << "\n";
        }
    }

}
