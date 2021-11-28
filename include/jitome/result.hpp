#ifndef JITOME_RESULT_HPP
#define JITOME_RESULT_HPP
#include "traits.hpp"
#include <optional>
#include <ostream>
#include <string>
#include <utility>
#include <variant>

namespace jitome
{

template<typename T>
struct Result
{
    using value_type = T;

    struct error_type
    {
        std::string msg;
    };

    Result(error_type err): storage_(std::move(err)) {}
    Result(value_type val): storage_(std::move(val)) {}

    Result& operator=(error_type err)
    {
        this->storage_ = std::move(err);
        return *this;
    }
    Result& operator=(value_type val)
    {
        this->storage_ = std::move(val);
        return *this;
    }

    template<typename U>
    Result(const Result<U>& other)
    {
        if(other.is_err())
        {
            this->storage_ = error_type{other.as_err().msg};
        }
        else
        {
            if constexpr (std::is_same_v<U, void>)
            {
                this->storage_ = value_type{};
            }
            else
            {
                this->storage_ = other.as_val();
            }
        }
    }

    template<typename U>
    Result& operator=(const Result<U>& other)
    {
        if(other.is_err())
        {
            this->storage_ = error_type{other.as_err().msg};
        }
        else
        {
            if constexpr (std::is_same_v<U, void>)
            {
                this->storage_ = value_type{};
            }
            else
            {
                this->storage_ = other.as_val();
            }
        }
        return *this;
    }

    Result(const Result&)            = default;
    Result(Result&&)                 = default;
    Result& operator=(const Result&) = default;
    Result& operator=(Result&&)      = default;
    ~Result() = default;

    bool is_ok () const noexcept {return storage_.index() == 0;}
    bool is_val() const noexcept {return storage_.index() == 0;}
    bool is_err() const noexcept {return storage_.index() == 1;}

    value_type&       as_val() &      {if(!this->is_val()) {throw std::runtime_error("Result: bad as_val()");} return std::get<0>(storage_);}
    error_type&       as_err() &      {if(!this->is_err()) {throw std::runtime_error("Result: bad as_err()");} return std::get<1>(storage_);}
    value_type const& as_val() const& {if(!this->is_val()) {throw std::runtime_error("Result: bad as_val()");} return std::get<0>(storage_);}
    error_type const& as_err() const& {if(!this->is_err()) {throw std::runtime_error("Result: bad as_err()");} return std::get<1>(storage_);}
    value_type&&      as_val() &&     {if(!this->is_val()) {throw std::runtime_error("Result: bad as_val()");} return std::move(std::get<0>(storage_));}
    error_type&&      as_err() &&     {if(!this->is_err()) {throw std::runtime_error("Result: bad as_err()");} return std::move(std::get<1>(storage_));}

  private:

    std::variant<value_type, error_type> storage_;
};

template<>
struct Result<void>
{
    using value_type = void;

    struct error_type
    {
        std::string msg;
    };

    Result(error_type err): storage_(std::move(err)) {}
    Result(): storage_(std::nullopt) {}

    Result& operator=(error_type err)
    {
        this->storage_ = std::move(err);
        return *this;
    }

    Result(const Result&)            = default;
    Result(Result&&)                 = default;
    Result& operator=(const Result&) = default;
    Result& operator=(Result&&)      = default;
    ~Result() = default;

    bool is_ok () const noexcept {return !storage_.has_value();}
    bool is_val() const noexcept {return !storage_.has_value();}
    bool is_err() const noexcept {return  storage_.has_value();}

    error_type&       as_err() &      {return storage_.value();}
    error_type const& as_err() const& {return storage_.value();}
    error_type&&      as_err() &&     {return std::move(storage_.value());}

  private:

    std::optional<error_type> storage_;
};

template<typename T>
Result<remove_cvref_t<T>> ok(T&& val)
{
    return Result<remove_cvref_t<T>>(std::forward<T>(val));
}
inline Result<void> ok()
{
    return Result<void>{};
}
template<typename T = void>
Result<T> err(std::string msg)
{
    return Result<T>(typename Result<T>::error_type{std::move(msg)});
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Result<T>& res)
{
    if(res.is_ok ())
    {
        if constexpr(std::is_same_v<T, void>)
        {
            os << "OK()";
        }
        else
        {
            os << "OK(" << res.as_val() << ")";
        }
    }
    else
    {
        os << "ERR(" << res.as_err() << ")";
    }
    return os;
}


} // jitome
#endif// JITOME_RESULT_HPP
