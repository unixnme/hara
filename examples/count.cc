#include <iostream>
#include <list>
#include "hara/Input.h"
#include "hara/String.h"

size_t count(const std::string &text, const std::string &key) {
    hara::Input input{text};
    std::string line;
    size_t counter = 0;
    while(input.GetLine(line)) {
        for (const auto &token : hara::String::Split(line)) {
            if (token == key) ++counter;
        }
    }
    return counter;
}

/**
 * count token [filename1 filename2 ...]
 * count # of occurrence of the given token
 * If filename is provided, read each file line by line, tokenize, and redirect to stdout
 * If no argument is provided, read from stdin line by line, tokenize, and redirect to stdout
 */
int main(int argc, const char** argv) {
    const std::string key{argv[1]};
    size_t counter = 0;
    if (argc == 2)
        counter += count("-", key);
    for (size_t idx = 2; idx < argc; ++idx)
        counter += count(argv[idx], key);

    std::cout << counter << std::endl;
    return 0;
}
