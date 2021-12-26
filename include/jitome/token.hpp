#ifndef JITOME_TOKEN_HPP
#define JITOME_TOKEN_HPP
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <ostream>

namespace jitome
{

enum class TokenKind
{
    Immediate,
    Identifier,
    Keyword,
    Operator,
    LeftParen,
    RightParen,
    LeftCurly,
    RightCurly,
    Comma,
    Invalid
};

inline std::ostream& operator<<(std::ostream& os, TokenKind k)
{
    switch(k)
    {
        case TokenKind::Immediate   : {os << "Immediate"   ; break;}
        case TokenKind::Identifier  : {os << "Identifier"  ; break;}
        case TokenKind::Keyword     : {os << "Keyword"     ; break;}
        case TokenKind::Operator    : {os << "Operator"    ; break;}
        case TokenKind::LeftParen   : {os << "LeftParen "  ; break;}
        case TokenKind::RightParen  : {os << "RightParen"  ; break;}
        case TokenKind::LeftCurly   : {os << "LeftCurly "  ; break;}
        case TokenKind::RightCurly  : {os << "RightCurly"  ; break;}
        case TokenKind::Comma       : {os << "Comma"       ; break;}
        case TokenKind::Invalid     : {os << "Invalid"     ; break;}
    }
    return os;
}

inline std::string to_string(TokenKind k)
{
    switch(k)
    {
        case TokenKind::Immediate   : {return std::string("Immediate"   );}
        case TokenKind::Identifier  : {return std::string("Identifier"  );}
        case TokenKind::Keyword     : {return std::string("Keyword"     );}
        case TokenKind::Operator    : {return std::string("Operator"    );}
        case TokenKind::LeftParen   : {return std::string("LeftParen "  );}
        case TokenKind::RightParen  : {return std::string("RightParen"  );}
        case TokenKind::LeftCurly   : {return std::string("LeftCurly "  );}
        case TokenKind::RightCurly  : {return std::string("RightCurly"  );}
        case TokenKind::Comma       : {return std::string("Comma"       );}
        case TokenKind::Invalid     : {return std::string("Invalid"     );}
    }
    return "Unknown";
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

inline std::ostream& operator<<(std::ostream& os, Token tk)
{
    os << "Token{" << tk.kind << ":" << tk.str << "}";
    return os;
}

} // jitome
#endif // JITOME_TOKEN_HPP
