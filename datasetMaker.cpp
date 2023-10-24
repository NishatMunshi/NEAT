#include<fstream>
#include "E:/Code/custom_libraries/randomNumberGenerator.hpp"
#include<list>
#include <iostream>
int main()
{

    std::ofstream file("xor.txt");

    for(unsigned i =0 ;i < 100; ++i)
    {
    auto randomX = random_16.generate(0 ,1) ?  1 :-1;
    auto randomY = random_16.generate(0 ,1) ? 1 : -1;

    auto result = randomX xor randomY ? 1 : -1;
        file << randomX << ' ' << randomY << ' '<< result <<'\n';
    }

    file.close();
    return 0;

}