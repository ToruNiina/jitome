#ifndef JITOME_TOKEN_HPP
#define JITOME_TOKEN_HPP
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace jitome
{

enum class TokenKind
{
    Immediate,
    Identifier,
    Operator,
    LeftBracket,
    RightBracket,
    Comma,
    Invalid
};

inline std::ostream& operator<<(std::ostream& os, TokenKind k)
{
    switch(k)
    {
        case TokenKind::Immediate   : {os << "Immediate"   ; break;}
        case TokenKind::Identifier  : {os << "Identifier"  ; break;}
        case TokenKind::Operator    : {os << "Operator"    ; break;}
        case TokenKind::LeftBracket : {os << "LeftBracket "; break;}
        case TokenKind::RightBracket: {os << "RightBracket"; break;}
        case TokenKind::Comma       : {os << "Comma"       ; break;}
        case TokenKind::Invalid     : {os << "Invalid"     ; break;}
    }
    return os;
}

struct Token
{
    TokenKind  kind;
    std::string_view str;
    std::size_t begin, len; // region index
    std::shared_ptr<std::string> src;
};

template<typename Iter>
Token make_token(TokenKind k, Iter first, Iter last, std::shared_ptr<std::string> src)
{
    std::string_view whole(*src);
    std::size_t begin = std::distance(src->begin(), first);
    std::size_t len   = std::distance(first, last);
    return Token{k, whole.substr(begin, len), begin, len, std::move(src)};
}

} // jitome
#endif // JITOME_TOKEN_HPP
