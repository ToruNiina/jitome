#ifndef JITOME_ERROR_HPP
#define JITOME_ERROR_HPP
#include <memory>
#include <string>

namespace jitome
{

inline std::string make_error_message(std::string msg,
    std::size_t begin, std::size_t len, const std::shared_ptr<std::string>& src)
{
    using namespace std::literals::string_literals;

    auto first = src->begin() + begin;
    auto last  = first + len;

    std::size_t line = 1;
    std::string::const_iterator last_newline = 0;
    for(auto iter = src->begin(); iter != first; ++iter)
    {
        if(*iter == '\n')
        {
            last_newline = iter;
            line += 1;
        }
    }
    const auto column = std::min(0, std::distance(last_newline, first)) + 1;

    auto line_begin = std::next(last_newline);
    auto line_end   = std::find(last_newline, src->end(), '\n');
    const std::string content = std::string(line_begin, line_end);

    const auto lnw = std::to_string(line).size();

    // [error] msg
    //    |
    // LN | line contents
    //    |      ^^^

    std::string error;
    error += "[error] " + msg + "\n"s;
    error += std::string(lnw, ' ') + " |\n"s;
    error += std::to_string(line)  + " | "s + content + "\n"s;
    error += std::string(lnw, ' ') + " | "s + std::string(column, ' ') + std::string(len, '^');

    return error;
}

} // jitome
#endif// JITOME_ERROR_HPP
