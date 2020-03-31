#include <iostream>
#include <list>
#include "hara/Input.h"
#include "hara/String.h"


void tokenize(hara::Input &input, const std::function<bool(char)> &is_delimiter) {
    std::string line;
    while (input.GetLine(line)) {
        const auto tokens = hara::String::Split<std::list<std::string>>(line, is_delimiter);
        std::for_each(tokens.begin(), tokens.end(), [](const std::string &token) {
            std::cout << token;
        });
        std::cout << std::endl;
    }
}

/**
 * tokenize delimiter [filename1 filename2 ...]
 * replace delimiter by a space char
 * If filename is provided, read each file line by line, tokenize, and redirect to stdout
 * If no argument is provided, read from stdin line by line, tokenize, and redirect to stdout
 */
int main(int argc, const char** argv) {
    if (std::strlen(argv[1]) != 1) {
        std::cerr << "Delimiter must be a single character" << std::endl;
        return EXIT_FAILURE;
    }

    auto is_delimiter = [argv](char c) { return c == argv[1][0]; };
    if (argc == 2) {
        // read from stdin and write out to stdout line by line
        hara::Input input{"-"}; // "-" means stdin
        tokenize(input, is_delimiter);
    }

    // else read from each file provided
    for (int i = 2; i < argc; ++i) {
        hara::Input input{argv[i]};
        tokenize(input, is_delimiter);
    }

    return 0;
}
