#include "hara/Input.h"
#include "hara/Output.h"
#include "hara/String.h"

/**
 * everyline input [output]
 * use '-' for input if reading from stdin
 * use '-' or omit output if writing to stdout
 */
int main(int argc, const char** argv) {
    const std::string input{argv[1]};
    const std::string output{argc >= 3 ? argv[2] : "-"};

    hara::Input in{input};
    hara::Output out{output};
    std::string line;
    while (in.GetLine(line)) {
        auto token = hara::String::Split(line);
        out.WriteLine(hara::String::Join(token, "\n"));
    }

    return 0;
}
