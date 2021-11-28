#ifndef JITOME_ERROR_HPP
#define JITOME_ERROR_HPP
#include "token.hpp"
#include <algorithm>
#include <memory>
#include <string>

namespace jitome
{

inline std::string show_char_in_err_msg(const char c)
{
    if(std::isprint(c) && c != ' ' && c != '\t')
    {
        return std::string(1, c);
    }

    if(c == ' ')
    {
        return std::string(" ");
    }
    else if(c == '\t')
    {
        return std::string("\\t");
    }
    else if(c == '\r')
    {
        return std::string("\\r");
    }
    else if(c == '\0')
    {
        return std::string("\\0");
    }

    const int hi = c / 16;
    const int lo = c % 16;

    std::string str;
    str += (hi < 10) ? ('0' + hi) : ('A' + (hi - 10));
    str += (lo < 10) ? ('0' + lo) : ('A' + (lo - 10));
    return str;
}

inline std::string show_line_in_err_msg(const std::string& str)
{
    std::string retval;
    for(const char c : str)
    {
        retval += show_char_in_err_msg(c);
    }
    return retval;
}

inline std::string show_token_position(const Token& tk)
{
    using namespace std::literals::string_literals;

    auto first = tk.src->cbegin() + tk.begin;
    auto last  = first + tk.len;

    bool newline_found = false;
    std::size_t line = 1;
    std::string::const_iterator last_newline = tk.src->cbegin();
    for(auto iter = tk.src->cbegin(); iter != first; ++iter)
    {
        if(*iter == '\n')
        {
            newline_found = true;
            last_newline = iter;
            line += 1;
        }
    }
    const auto column = std::max<std::int64_t>(0, std::distance(last_newline, first)) + 1;

    auto line_begin = newline_found ? std::next(last_newline) : last_newline;
    auto line_end   = std::find(last_newline, tk.src->cend(), '\n');
    const std::string content = std::string(line_begin, line_end);

    const auto lnw = std::to_string(line).size();

    std::string msg;
    msg += std::string(lnw, ' ') + " |\n"s;
    msg += std::to_string(line)  + " | "s + show_line_in_err_msg(content) + "\n"s;
    msg += std::string(lnw, ' ') + " |"s + std::string(column, ' ') +
             std::string(tk.len, '^') + "- token: "s + to_string(tk.kind) +
             "["s + std::string(tk.str) + "]"s;

    return msg;
}

inline std::string make_error_message(std::string msg, const Token& tk)
{
    using namespace std::literals::string_literals;

    std::string error;
    error += "[error] " + msg + "\n"s;
    error += show_token_position(tk);
    return error;
}

} // jitome
#endif// JITOME_ERROR_HPP
