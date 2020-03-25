#include <iostream>
#include "hara/Input.h"
#include "hara/Output.h"
#include "hara/String.h"


void tokenize(hara::Input &input) {
    std::string line;
    while (input.GetLine(line)) {
        const auto tokens = hara::String::Split(line);
        std::for_each(tokens.begin(), tokens.end(), [](const std::string &token) {
            std::cout << token << std::endl;
        });
    }
}

/**
 * tokenize [filename1 filename2 ...]
 * If filename is provided, read each file line by line, tokenize, and redirect to stdout
 * If no argument is provided, read from stdin line by line, tokenize, and redirect to stdout
 */
int main(int argc, const char** argv) {
    hara::Output output{"-"}; // "-" means stdout
    if (argc == 1) {
        // read from stdin and write out to stdout line by line
        hara::Input input{"-"}; // "-" means stdin
        tokenize(input);
    }

    // else read from each file provided
    for (int i = 1; i < argc; ++i) {
        hara::Input input{argv[i]};
        tokenize(input);
    }

    return 0;
}
