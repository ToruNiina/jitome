#ifndef JITOME_TOKENIZER_HPP
#define JITOME_TOKENIZER_HPP
#include <memory>
#include <string>
#include <vector>

namespace jitome
{

enum class TokenKind
{
    immediate,
    identifier,
    operator_plus,
    operator_minus,
    operator_multiply,
    operator_division,
    left_paren,
    right_paren,
    comma
};

struct Token
{
    TokenKind  kind;
    std::string_view str;
    std::size_t begin, end; // region index
    std::shared_ptr<std::string> src;
};

template<typename Iter>
bool is_twochar(Iter iter, Iter end, const char c1, const char c2)
{
    return (iter != end && *iter == c1 &&
            std::next(iter) != end && *std::next(iter) == c2);
}

template<typename Iter>
bool is_newline(Iter iter, Iter end)
{
    return (iter != end && *iter == '\n') || is_twochar(iter, end, '\r', '\n');
}

template<typename Iter>
Iter skip_newline(Iter iter, Iter end)
{
    if(iter != end && *iter == '\n')
    {
        iter = std::next(iter, 1);
    }
    else if(is_twochar(iter, end, '\r', '\n'))
    {
        iter = std::next(iter, 2);
    }
    return iter;
}

template<typename Iter>
Iter skip_whitespace(Iter iter, Iter end)
{
    while(iter != end && (*iter == ' ' || *iter == '\t'))
    {
        ++iter;
    }
    return iter;
}
template<typename Iter>
Iter skip_comment_line(Iter iter, Iter end)
{
    if (is_twochar(iter, end, '/', '/'))
    {
        while(true)
        {
            if(is_newline(iter, end))
            {
                return skip_newline(iter, end);
            }
            ++iter;
        }
    }
    return iter;
}
template<typename Iter>
Iter skip_comment(Iter iter, Iter end)
{
    if (is_twochar(iter, end, '/', '*'))
    {
        iter = std::next(iter, 2);
        while(true)
        {
            if (is_twochar(iter, end, '*', '/'))
            {
                iter = std::next(iter, 2);
                break;
            }
            ++iter;
        }
    }
    return iter;
}

template<typename Iter>
Iter skip_negligible(Iter iter, Iter end)
{
    while(true)
    {
        const auto front = iter;
        iter = skip_comment     (iter, end);
        iter = skip_comment_line(iter, end);
        iter = skip_whitespace  (iter, end);

        if(front == iter)
        {
            break;
        }
    }
    return iter;
}

// template<typename Iter>
// Result<Token> scan_immediate(Iter& iter, Iter end, std::shared_ptr<std::string> src)
// {
//     ;
// }
//
// template<typename Iter>
// Result<Token> scan_immediate(Iter& iter, Iter end, std::shared_ptr<std::string> src)
// {
//     ;
// }
// template<typename Iter>
// Result<Token> scan_immediate(Iter& iter, Iter end, std::shared_ptr<std::string> src)
// {
//     ;
// }

template<typename Iter>
Result<Token> scan_token(Iter& iter, Iter end, std::shared_ptr<std::string> src)
{
    std::tie(iter, line, column) = skip_negligible(iter, end, line, column, src);
    if(iter == end)
    {
        return err("There is no token left.");
    }

    if(std::isdigit(*iter))
    {
        return scan_immediate(iter, end, line, column);
    }
    else if(std::isalpha(*iter))
    {
        return scan_identifier(iter, end, line, column);
    }
    else
    {
        return scan_operator(iter, end, line, column);
    }
}

inline Result<std::vector<Token>> tokenize(const std::string& str)
{
    std::shared_ptr<std::string> src = std::make_shared<std::string>(str);

    std::vector<token> tks;
    for(auto iter = src->begin(); iter != src->end(); ++iter)
    {
        if(auto tk = scan_token(iter, src->end()); tk.is_ok())
        {
            tks.push_back(std::move(tk.as_val()));
        }
    }
    return tks;
}

} // jitome
#endif// JITOME_TOKENIZER_HPP
