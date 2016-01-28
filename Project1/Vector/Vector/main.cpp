#include <iostream>
#include "Vector.h"

using std::cout;
using std::endl;

int main(void) {
    epl::vector<int> v;
    epl::vector<int> b{v};
    return 0;
}
