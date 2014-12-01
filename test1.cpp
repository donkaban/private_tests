#include <iostream>

// not thread safe! :)

template <typename T>
constexpr short bits_in() {return sizeof(T) * 8;}  

template <typename T>
const char *to_bin(int val)
{
    static char result[bits_in<T>() + 1] = {0};
    for(auto i = 0; i < bits_in<T>(); i++) 
        result[i] = (val & (0x80000000 >> i)) == 0 ? '0' : '1';
    return result;
}


int main()
{
    for (auto i =-32; i< 32; i++ )
        std::cout << to_bin<int>(i) << std::endl;
    return 0;
}