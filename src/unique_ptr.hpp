//
// HealpixViewer - Automatic memory managment utility with unique ownership
//                 semantics
//
// Author: Bruno Santos <bsantos@av.it.pt>
//

#ifndef HV_UNIQUE_PTR_HPP
#define HV_UNIQUE_PTR_HPP

#include "utility.hpp"
#include <utility>

namespace hv { //==============================================================

template<class T>
class unique_ptr {
    unique_ptr(unique_ptr&);
    unique_ptr& operator=(unique_ptr&);

    HV_UNDEFINED_BOOL;

public:
    typedef T* pointer;

public:
    explicit unique_ptr(pointer ptr = 0)
       : _ptr(ptr)
    { }

    unique_ptr(rv_<unique_ptr> rhs)
       : _ptr(rhs->release())
    { }

    unique_ptr& operator=(rv_<unique_ptr> rhs)
    {
        reset(rhs->release());
        return *this;
    }

    operator rv_<unique_ptr>()
    {
        return rv_<unique_ptr>(*this);
    }

    ~unique_ptr()
    {
        if (_ptr)
            delete _ptr;
    }

    void reset(pointer p = 0)
    {
        if (_ptr)
            delete _ptr;
        _ptr = p;
    }

    pointer release()
    {
        T* tmp = _ptr;
        _ptr = 0;
        return tmp;
    }

    typename add_reference<T>::type operator*() const
    {
        return *_ptr;
    }

    pointer operator->() const
    {
        return _ptr;
    }

    pointer get() const
    {
        return _ptr;
    }

    void swap(unique_ptr& rhs)
    {
        std::swap(_ptr, rhs._ptr);
    }

    operator undefined_bool() const
    {
        return _ptr ? HV_UNDEFINED_BOOL_TRUE : HV_UNDEFINED_BOOL_FALSE;
    }

private:
    T* _ptr;
};

template<class T>
class unique_ptr<T[0]> {
    unique_ptr(unique_ptr&);
    unique_ptr& operator=(unique_ptr&);

    HV_UNDEFINED_BOOL;

public:
    typedef T* pointer;

public:
    explicit unique_ptr(pointer ptr = 0)
       : _ptr(ptr)
    { }

    unique_ptr(rv_<unique_ptr> rhs)
       : _ptr(rhs->release())
    { }

    unique_ptr& operator=(rv_<unique_ptr> rhs)
    {
        reset(rhs->release());
        return *this;
    }

    operator rv_<unique_ptr>()
    {
        return rv_<unique_ptr>(*this);
    }

    ~unique_ptr()
    {
        if (_ptr)
            delete[] _ptr;
    }

    void reset(pointer p = 0)
    {
        if (_ptr)
            delete[] _ptr;
        _ptr = p;
    }

    pointer release()
    {
        T* tmp = _ptr;
        _ptr = 0;
        return tmp;
    }

    typename add_reference<T>::type operator[](size_t idx) const
    {
        return _ptr[idx];
    }

    pointer get() const
    {
        return _ptr;
    }

    void swap(unique_ptr& rhs)
    {
        std::swap(_ptr, rhs._ptr);
    }

    operator undefined_bool() const
    {
        return _ptr ? HV_UNDEFINED_BOOL_TRUE : HV_UNDEFINED_BOOL_FALSE;
    }

private:
    T* _ptr;
};

template<class T>
inline void swap(unique_ptr<T>& lhs, unique_ptr<T>& rhs)
{
    lhs.swap(rhs);
}

template<class T>
inline bool operator==(unique_ptr<T> const& lhs, unique_ptr<T> const& rhs)
{
    return lhs.get() == rhs.get();
}

template<class T>
inline bool operator!=(unique_ptr<T> const& lhs, unique_ptr<T> const& rhs)
{
    return lhs.get() != rhs.get();
}

template<class T>
inline bool operator<(unique_ptr<T> const& lhs, unique_ptr<T> const& rhs)
{
    return lhs.get() < rhs.get();
}

template<class T>
inline bool operator<=(unique_ptr<T> const& lhs, unique_ptr<T> const& rhs)
{
    return lhs.get() <= rhs.get();
}

template<class T>
inline bool operator>(unique_ptr<T> const& lhs, unique_ptr<T> const& rhs)
{
    return lhs.get() > rhs.get();
}

template<class T>
inline bool operator>=(unique_ptr<T> const& lhs, unique_ptr<T> const& rhs)
{
    return lhs.get() >= rhs.get();
}


} // namespace hv =============================================================

#endif // HV_UNIQUE_PTR_HPP
