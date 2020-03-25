#ifndef IO_OUTPUT_H
#define IO_OUTPUT_H

#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>

namespace hara {

/** Minimalistic output-handling class
 * that could be to a file or to stdout
 *
 * Only support forward writing methods
 *
 */
class Output {
public:
    explicit Output(const std::string &path) {
        std::ios::sync_with_stdio(false);
        if (path == "-") {
#ifdef HARA_VERBOSE
            std::cerr << "Writing to stdout" << std::endl;
#endif
            buf = std::cout.rdbuf();
        } else {
#ifdef HARA_VERBOSE
            std::cerr << "Writing to " << path << std::endl;
#endif
            ofs = std::unique_ptr<std::ofstream>{new std::ofstream{path}};
            if (!ofs->is_open())
                throw std::runtime_error("Cannot write to " + path);
            buf = ofs->rdbuf();
        }
        out = std::unique_ptr<std::ostream>{new std::ostream{buf}};
    }

    explicit Output(const char* path) : Output{std::string{path}} {}

    /**
     * write the line + endl char
     * @param line
     */
    void WriteLine(const std::string &line) {
        *out << line << std::endl;
    }

    void Write(const std::string &content) {
        *out << content;
    }

    template <typename T>
    Output& operator<<(const T& data) {
        *out << data;
        return *this;
    }

    Output& operator<<(std::ostream& (*manip)(std::ostream&)) {
        *out << manip;
        return *this;
    }

private:
    std::streambuf *buf;
    std::unique_ptr<std::ofstream> ofs;
    std::unique_ptr<std::ostream> out;
};

}

#endif //IO_OUTPUT_H
