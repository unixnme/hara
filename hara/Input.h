#ifndef IO_INPUT_H
#define IO_INPUT_H

#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>

namespace hara {

    /** Minimalistic input-handling class
     * that could be from a file or piped stdin
     *
     * Only support forward reading methods
     *
     */
class Input {
public:
    explicit Input(const std::string &path) {
        std::ios::sync_with_stdio(false);
        if (path == "-") {
            // read from stdin
            buf = std::cin.rdbuf();
        } else {
            ifs = std::unique_ptr<std::ifstream>{new std::ifstream{path}};
            if (!ifs->is_open())
                throw std::runtime_error("Cannot read from " + path);
            buf = ifs->rdbuf();
        }
        in = std::unique_ptr<std::istream>{new std::istream{buf}};
    }

    explicit Input(const char* path) : Input{std::string{path}} {}

    /**
     * Get current line; does not return endl char
     */
    Input& GetLine(std::string &line) {
        std::getline(*in, line);
        return *this;
    }

    explicit operator bool() const { return in->operator bool(); }

    /**
     * Read the entire content from the current position
     * @return
     */
    std::string Read() {
        std::string content;
        std::string line;
        while (std::getline(*in, line))
            content += line + "\n";
        return content;
    }

    template<typename T>
    Input& operator>>(T& data) {
        *in >> data;
        return *this;
    }

private:
    std::streambuf *buf;
    std::unique_ptr<std::ifstream> ifs;
    std::unique_ptr<std::istream> in;
};

}


#endif //IO_INPUT_H
