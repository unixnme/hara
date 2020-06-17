#include <iostream>
#include "hara/Input.h"
#include "hara/Output.h"
#include "hara/String.h"
#include "hara/PrefixTree.h"

int Usage(const char *program) {
    std::cerr << "Usage: " << program << " VOCAB INPUT [OUTPUT]" << std::endl;
    std::cerr << "\tVOCAB: file containing vocabulary" << std::endl;
    std::cerr << "\tINPUT: text file to encode; use '-' to read from stdin" << std::endl;
    std::cerr << "\tOUTPUT: output file to print result; use '-' or omit to print to stdout" << std::endl;
    return EXIT_FAILURE;
}

int main(int argc, const char** argv) {
    if (argc != 3 && argc != 4) return Usage(argv[1]);
    const std::string vocab_file{argv[1]};
    const std::string input_file{argv[2]};
    const std::string output_file{argc == 4 ? argv[3] : "-"};

    hara::PrefixTree<char, std::string> prefixTree;
    {
        hara::Input input{vocab_file};
        auto tokens = hara::String::Split(input.Read());
        for (const auto &token : tokens) {
            prefixTree.Insert(std::vector<char>{token.begin(), token.end()}, token);
        }
    }

    {
        hara::Input input{input_file};
        hara::Output output{output_file};
        std::string line;
        while (input.GetLine(line)) {
            std::vector<std::string> encoded;
            for (const auto &token : hara::String::Split(line)) {
                std::vector<char> unit;
                std::string result = "<unk>";
                for (auto idx = 0; idx < token.size(); ++idx) {
                    auto c = token.at(idx);
                    unit.push_back(c);
                    auto node = prefixTree.Find(unit);
                    if (node.Empty()) {
                        encoded.push_back(std::move(result));
                        result = "<unk>";
                        --idx;
                        unit.clear();
                    } else {
                        result = std::string{unit.begin(), unit.end()};
                    }
                }
                encoded.push_back(std::move(result));
            }
            output << hara::String::Join(encoded) << std::endl;
        }
    }

    return 0;
}