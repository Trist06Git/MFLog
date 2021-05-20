
#include <iostream>
#include <vector>

class C {
    public:
    int a = -1;
    std::string s = "hello";
    C() {
        a = 1;
        s = "test";
    }
};

int main(int argc, char** argv) {
    std::vector<C> vec;
    for (int i = 0; i < 10; i++) {
        vec.push_back(C());
    }
    for (int i = 0; i < vec.capacity(); i++) {
        C* c = vec.data()+i;
        std::cout << i << " : " << c->a << "," << c->s << std::endl;
    }
    return 0;
}

