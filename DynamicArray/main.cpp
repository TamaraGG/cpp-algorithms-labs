
#include <iostream>

#include "Array.h"

int main() {

    Array<int> a;
    for (int i = 0; i < 10; ++i)
        a.Insert(i + 1);
    for (int i = 0; i < a.size(); ++i)
        a[i] *= 2;

    std::cout << "Forward with hasNext/next:" << '\n';
    for (auto it = a.iterator(); it.hasNext(); it.next())
        std::cout << it.get() << std::endl;

    std::cout << "Forward with begin/end:" << '\n';
    for (int & it : a)
        std::cout << it << '\n';

    std::cout << "Reverse with rbegin/rend:" << '\n';
    for (auto it = a.rbegin(); it != a.rend(); ++it)
        std::cout << *it << '\n';

    return 0;
}
