#ifndef JITOME_UTIL_HPP
#define JITOME_UTIL_HPP
#include <memory>
#include <type_traits>
#include <cstring>

namespace jitome
{

template<typename To, typename From>
To bit_cast(const From& from)
{
    static_assert(sizeof(From) == sizeof(To));
    To to;

    std::memcpy(reinterpret_cast<char*      >(std::addressof(to)),
                reinterpret_cast<const char*>(std::addressof(from)),
                sizeof(From));
    return to;
}

template<typename T>
struct copyable_dynamic_storage
{
    using value_type = T;

    copyable_dynamic_storage(): storage_(nullptr){}

    template<typename U>
    copyable_dynamic_storage(U&& v)
        : storage_(std::make_unique<T>(std::forward<U>(v)))
    {}

    copyable_dynamic_storage(const copyable_dynamic_storage& other)
        : storage_(other.storage_ ?
                   std::make_unique<value_type>(*other.storage_) : nullptr)
    {}

    copyable_dynamic_storage& operator=(const copyable_dynamic_storage& other)
    {
        if(other.storage_)
        {
            this->storage_ = std::make_unique<value_type>(*other.storage_);
        }
        else
        {
            this->storage_.release();
        }
    }

    copyable_dynamic_storage(copyable_dynamic_storage&&) = default;
    copyable_dynamic_storage& operator=(copyable_dynamic_storage&&) = default;

    operator bool() const noexcept {return static_cast<bool>(storage_);}

    operator value_type& () const
    {
        return this->get();
    }

    value_type& get() const
    {
        if(!storage_)
        {
            throw std::runtime_error("copyable_dynamic_storage: uninitialized");
        }
        return *storage_;
    }
    value_type& get(const std::nothrow_t&) const
    {
        return *storage_;
    }

    bool operator==(const copyable_dynamic_storage& other) const noexcept
    {
        if(!(this->storage_) || !(other.storage_))
        {
            return this->storage_ == nullptr &&
                   other.storage_ == nullptr ;
        }
        return *(this->storage_) == *(other.storage_);
    }
    bool operator!=(const copyable_dynamic_storage& other) const noexcept
    {
        return !(*this == other);
    }

  private:

    std::unique_ptr<T> storage_;
};


} // jitome
#endif// JITOME_UTIL_HPP
