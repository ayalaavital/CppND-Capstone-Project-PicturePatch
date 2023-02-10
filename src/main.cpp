#include <iostream>
#include <exception>
#include <memory>

#include "app.h"


int main() {

    try {
        App app = App();
        app.run();
    } catch (std::exception& e) {
        std::cout <<  e.what() << std::endl;
    }

    return 0;
}