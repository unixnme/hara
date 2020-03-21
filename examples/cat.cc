#include <iostream>
#include "hara/Input.h"
#include "hara/Output.h"

int main(int argc, const char** argv) {
    hara::Output output{"-"}; // std
    if (argc == 1) {
        // read from std and write out to std line by line
        hara::Input input{"-"};
        std::string line;
        while (input.GetLine(line))
            output << line << std::endl;
    }

    // else read from each file provided
    for (int i = 1; i < argc; ++i) {
        hara::Input input{argv[i]};
        std::string line;
        while (input.GetLine(line))
            output << line << std::endl;
    }

    return 0;
}
