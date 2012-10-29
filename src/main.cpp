#include <iostream>

#include <iostream>
#include <cstdlib>
#include "demo.hpp"

using namespace std;

int main(int, char**) {
    Demo demo("demo", "device", "scene", "root", "renderer", "resources", "physics-world");
    demo.loadScene();
    demo.bindInputs();
    demo.runMainLoop();
    return EXIT_SUCCESS;
}
