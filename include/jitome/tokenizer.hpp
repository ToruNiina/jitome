#ifndef JITOME_TOKENIZER_HPP
#define JITOME_TOKENIZER_HPP
#include "error.hpp"
#include "result.hpp"
#include "token.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <deque>
#include <cassert>
#include <cctype>

namespace jitome
{

template<typename Iter, std::size_t N>
bool is_chars(Iter iter, Iter end, const char (&cs)[N])
{
    for(std::size_t i=0; i<N-1; ++i) // N-1 for null character
    {
        if(iter == end || *iter != cs[i]) {return false;}
        ++iter;
    }
    return true;
}

template<typename Iter, std::size_t N>
bool is_oneof(Iter iter, Iter end, const char (&cs)[N])
{
    static_assert(1 <= N);
    if(iter == end)
    {
        return false;
    }

    for(std::size_t i=0; i<N-1; ++i) // N-1 for null character
    {
        if(*iter == cs[i])
        {
            return true;
        }
    }
    return false;
}


template<typename Iter>
bool is_newline(Iter iter, Iter end)
{
    return (iter != end && *iter == '\n') || is_chars(iter, end, "\r\n");
}

template<typename Iter>
Iter skip_newline(Iter iter, Iter end)
{
    if(iter != end && *iter == '\n')
    {
        iter = std::next(iter, 1);
    }
    else if(is_chars(iter, end, "\r\n"))
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
    if (is_chars(iter, end, "//"))
    {
        while(iter != end)
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
    if (is_chars(iter, end, "/*"))
    {
        iter = std::next(iter, 2);
        while(iter != end)
        {
            if (is_chars(iter, end, "*/"))
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
    while(iter != end)
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

template<typename Iter>
Result<Token> scan_immediate(Iter& iter, Iter end, std::shared_ptr<std::string> src)
{
    if(iter == end)
    {
        return err("scan_immediate: no character left.");
    }
    const auto first = iter;

    // unary + and - are tokenized to operator_plus/minus.

    // immediate-real = immediate-real-integer-part
    //                  [ immediate-real-fractional-part ] [ immeidate-real-exponent-part ]

    // immediate-real-integer-part =  non-zero-digit *digit
    // immediate-real-integer-part =/ zero
    if(*iter == '0')
    {
        iter = std::next(iter);
    }
    else
    {
        while(iter != end && std::isdigit(*iter))
        {
            iter = std::next(iter);
        }
    }

    // immediate-real-fractional-part = %x2E(.) 1*digit
    if(iter != end && *iter == '.')
    {
        iter = std::next(iter);

        if(iter == end || !std::isdigit(*iter))
        {
            return err("scan_immediate: at least 1 digit should follow `.`");
        }
        while(iter != end && std::isdigit(*iter))
        {
            iter = std::next(iter);
        }
    }

    // immediate-real-exponent-part   = ( E / e ) [(+/-)] 1*digit
    if(iter != end && (*iter == 'e' || *iter == 'E'))
    {
        iter = std::next(iter);

        if(iter != end && (*iter == '+' || *iter == '-'))
        {
            iter = std::next(iter);
        }

        if(iter == end || !std::isdigit(*iter))
        {
            return err("scan_immediate: at least 1 digit should follow `e`");
        }
        while(iter != end && std::isdigit(*iter))
        {
            iter = std::next(iter);
        }
    }

    return make_token(TokenKind::Immediate, first, iter, std::move(src));
}

template<typename Iter>
Result<Token> scan_identifier(Iter& iter, Iter end, std::shared_ptr<std::string> src)
{
    if(iter == end)
    {
        return err("scan_immediate: no character left.");
    }
    const auto first = iter;

    if(!std::isalpha(*iter))
    {
        return err("scan_immediate: the first character of an identifier should be an alphabet.");
    }
    iter = std::next(iter);

    while(iter != end && (std::isalnum(*iter) || *iter == '_'))
    {
        iter = std::next(iter);
    }

    return make_token(TokenKind::Identifier, first, iter, std::move(src));
}

template<typename Iter>
Result<Token> scan_operator(Iter& iter, Iter end, std::shared_ptr<std::string> src)
{
    if(iter == end)
    {
        return err("scan_operator: no character left.");
    }
    const auto first = iter;

    if(is_oneof(iter, end, "+-*/"))
    {
        iter = std::next(iter);
        return make_token(TokenKind::Operator, first, iter, std::move(src));
    }
    else if(*iter == '(')
    {
        iter = std::next(iter);
        return make_token(TokenKind::LeftParen, first, iter, std::move(src));
    }
    else if(*iter == ')')
    {
        iter = std::next(iter);
        return make_token(TokenKind::RightParen, first, iter, std::move(src));
    }
    else if(*iter == '{')
    {
        iter = std::next(iter);
        return make_token(TokenKind::LeftCurly, first, iter, std::move(src));
    }
    else if(*iter == '}')
    {
        iter = std::next(iter);
        return make_token(TokenKind::RightCurly, first, iter, std::move(src));
    }
    else if(*iter == ',')
    {
        iter = std::next(iter);
        return make_token(TokenKind::Comma, first, iter, std::move(src));
    }
    return err(make_error_message("scan_operator: unknown operator appeared",
        make_token(TokenKind::Invalid, iter, std::next(iter), std::move(src))));
}

template<typename Iter>
Result<Token> scan_token(Iter& iter, Iter end, std::shared_ptr<std::string> src)
{
    iter = skip_negligible(iter, end);
    if(iter == end)
    {
        return err("There is no token left.");
    }
    assert(*iter != '\0');

    if(std::isdigit(*iter))
    {
        return scan_immediate(iter, end, std::move(src));
    }
    else if(std::isalpha(*iter))
    {
        return scan_identifier(iter, end, std::move(src));
    }
    else if(is_oneof(iter, end, "+-*/(){},"))
    {
        return scan_operator(iter, end, std::move(src));
    }
    else
    {
        return err(make_error_message("scan_token: unknown token appeared",
            make_token(TokenKind::Invalid, iter, std::next(iter), std::move(src))));
    }
}

inline Result<std::deque<Token>> tokenize(const std::string& str)
{
    std::shared_ptr<std::string> src = std::make_shared<std::string>(str);

    std::deque<Token> tks;
    auto iter = src->begin();
    while(iter != src->end())
    {
        if(*iter == '\0') {break;}

        auto tk = scan_token(iter, src->end(), src);

        if(tk.is_ok())
        {
            tks.push_back(std::move(tk.as_val()));
        }
        else
        {
            return err(tk.as_err().msg);
        }
    }
    return ok(tks);
}

} // jitome
#endif// JITOME_TOKENIZER_HPP
