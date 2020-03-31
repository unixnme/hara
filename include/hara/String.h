#ifndef IO_STRING_H
#define IO_STRING_H

#include <string>
#include <vector>

namespace hara {

/** Common operations involving string
*
*/
class String {
public:
    template<typename OutputContainer = std::vector<std::string>>
    static OutputContainer Split(const std::string &input) {
        return Split(input, [](char c) { return std::isspace(c); });
    }

    template<typename OutputContainer = std::vector<std::string>, typename UnaryPredicate>
    static OutputContainer Split(const std::string &input, UnaryPredicate pred) {
        OutputContainer result;
        auto begin = input.begin();
        for (auto end = input.begin(); end != input.end(); ++end) {
            const auto c = *end;
            if (pred(c)) {
                if (begin != end) result.emplace_back(begin, end);
                begin = end + 1;
            }
        }
        if (begin != input.end()) result.emplace_back(begin, input.end());
        return result;
    }

    template<typename Container = std::vector<std::string>>
    static std::string Join(const Container &tokens,
                            const std::string &separator = std::string{" "}) {
        std::string joined;
        for (auto it = tokens.begin(); it != tokens.end(); ++it) {
            if (it != tokens.begin())
                joined.append(separator);
            joined.append(*it);
        }
        return joined;
    }
};

}

#endif //IO_STRING_H
