#include "E:/Code/custom_libraries/list.hpp"
#include <iostream>
void print_list(const myLib::list<unsigned> &_list)
{
    for (unsigned index = 0; index < _list.size(); ++index)
        std::cout << _list.at(index) << ' ';
    std::cout << '\n';
}
int main()
{
    myLib::list<unsigned> list;

    list.push_front(0);
    list.push_back(1);
    list.add_before(0, 2);
    list.add_before(1, 3);

    print_list(list);
    return 0;
}