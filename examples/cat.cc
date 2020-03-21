#include <iostream>
#include "hara/Input.h"
#include "hara/Output.h"

/**
 * cat [filename1 filename2 ...]
 * If filename is provided, read each file line by line and redirect to stdout
 * If no argument is provided, read from stdin line by line and redirect to stdout
 */
int main(int argc, const char** argv) {
    hara::Output output{"-"}; // "-" means stdout
    if (argc == 1) {
        // read from stdin and write out to stdout line by line
        hara::Input input{"-"}; // "-" means stdin
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
