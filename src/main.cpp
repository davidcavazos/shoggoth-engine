#include <iostream>

#include <iostream>
#include <cstdlib>
#include "demo.hpp"

using namespace std;

int main(int, char**) {
    Demo game("game", "root");
    game.loadScene();
    game.bindInputs();
    game.runMainLoop();
    return EXIT_SUCCESS;
}
