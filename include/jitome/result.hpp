#ifndef JITOME_RESULT_HPP
#define JITOME_RESULT_HPP
#include <optional>
#include <string>
#include <utility>
#include <variant>

namespace jitome
{

template<typename T>
struct result
{
    using value_type = T;

    struct error_type
    {
        std::string msg;
    };

    result(error_type err): storage_(std::move(err)) {}
    result(value_type val): storage_(std::move(val)) {}

    result& operator=(error_type err)
    {
        this->storage_ = std::move(err);
        return *this;
    }
    result& operator=(value_type val)
    {
        this->storage_ = std::move(val);
        return *this;
    }

    template<typename U>
    result(const result<U>& other)
    {
        if(other.is_err())
        {
            this->storage_ = other.as_err();
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
    result& operator=(const result<U>& other)
    {
        if(other.is_err())
        {
            this->storage_ = other.as_err();
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

    result(const result&)            = default;
    result(result&&)                 = default;
    result& operator=(const result&) = default;
    result& operator=(result&&)      = default;
    ~result() = default;

    bool is_val() const noexcept {return storage_.index() == 0;}
    bool is_err() const noexcept {return storage_.index() == 1;}

    value_type&       as_val() &      {return std::get<0>(storage_);}
    error_type&       as_err() &      {return std::get<1>(storage_);}
    value_type const& as_val() const& {return std::get<0>(storage_);}
    error_type const& as_err() const& {return std::get<1>(storage_);}
    value_type&&      as_val() &&     {return std::move(std::get<0>(storage_));}
    error_type&&      as_err() &&     {return std::move(std::get<1>(storage_));}

  private:

    std::variant<value_type, error_type> storage_;
};

template<>
struct result<void>
{
    using value_type = void;

    struct error_type
    {
        std::string msg;
    };

    result(error_type err): storage_(std::move(err)) {}
    result(): storage_(std::nullopt) {}

    result& operator=(error_type err)
    {
        this->storage_ = std::move(err);
        return *this;
    }

    result(const result&)            = default;
    result(result&&)                 = default;
    result& operator=(const result&) = default;
    result& operator=(result&&)      = default;
    ~result() = default;

    bool is_val() const noexcept {return !storage_.has_value();}
    bool is_err() const noexcept {return  storage_.has_value();}

    error_type&       as_err() &      {return storage_.value();}
    error_type const& as_err() const& {return storage_.value();}
    error_type&&      as_err() &&     {return std::move(storage_.value());}

  private:

    std::optional<error_type> storage_;
};

template<typename T>
result<remove_cvref_t<T>> ok(T&& val)
{
    return result<remove_cvref_t<T>>(std::forward<T>(val));
}
inline result<void> ok()
{
    return result<void>{};
}
template<typename T = void>
result<T> err(std::string msg)
{
    return result<T>(typename result<T>::error_type{std::move(msg)});
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const result<T>& res)
{
    if(res.is_val())
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
