#include <iostream>
 
void removeDups(char *str)
{
    char * curr = str;
    char * last = str;
    while(*curr++ != 0)
    {
        if(*curr != *(curr-1))
            last++;
        if(last != curr)
            *last = *curr;  
    }   
}

int main()
{
    char test1[] = "AA BBBB C DDDD EEE F GGGG";
    char test2[] = "HHHHHIII,,,,      WWWORLLLLDDD!!!!";

    std::cout << test1 << std::endl;
    std::cout << test2 << std::endl;

    removeDups(test1);
    removeDups(test2);

    std::cout << test1 << std::endl;
    std::cout << test2 << std::endl;

    return 0;
}