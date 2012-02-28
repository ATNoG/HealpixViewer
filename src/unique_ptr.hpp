//
// HealpixViewer - Automatic memory managment utility with unique ownership
//                 semantics
//
// Author: Bruno Santos <bsantos@av.it.pt>
//

#ifndef HV_UNIQUE_PTR_HPP
#define HV_UNIQUE_PTR_HPP

#include "base.hpp"

namespace hv { //==============================================================

template<class T>
class unique_ptr {
    unique_ptr(unique_ptr&);
    unique_ptr& operator=(unique_ptr&);

    void operator==(unique_ptr const&) const;
    void operator!=(unique_ptr const&) const;

    HV_UNDEFINED_BOOL;

public:
    explicit unique_ptr(T* ptr = 0)
       : _ptr(ptr)
    { }

    unique_ptr(rv_<unique_ptr>& rhs)
       : _ptr(rhs._ptr)
    {
        rhs._ptr = 0;
    }

    unique_ptr& operator=(rv_<unique_ptr>& rhs)
    {
        reset(rhs._ptr);
        rhs._ptr = 0;
    }

    ~unique_ptr()
    {
        if (_ptr)
            delete _ptr;
    }

    void reset(T* p = 0)
    {
        if (_ptr)
            delete _ptr;
        _ptr = p;
    }

    T* release()
    {
        T* tmp = _ptr;
        _ptr = 0;
        return tmp;
    }

    T& operator*() const
    {
        return *_ptr;
    }

    T* operator->() const
    {
        return _ptr;
    }

    T* get() const
    {
        return _ptr;
    }

    void swap(unique_ptr& rhs)
    {
        T* tmp = rhs._ptr;
        rhs._ptr = _ptr;
        _ptr = tmp;
    }

    bool operator!()
    {
        return !_ptr;
    }

    operator undefined_bool_t()
    {
        return _ptr ? HV_UNDEFINED_BOOL_TRUE : HV_UNDEFINED_BOOL_FALSE;
    }

    operator rv_<unique_ptr>&()
    {
        return move();
    }

    rv_<unique_ptr>& move()
    {
        return static_cast<rv_<unique_ptr>&>(*this);
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
class unique_array {
    unique_array(unique_array&);
    unique_array& operator=(unique_array&);

    void operator==(unique_array const&) const;
    void operator!=(unique_array const&) const;

    HV_UNDEFINED_BOOL;

public:
    explicit unique_array(T ptr[] = 0)
       : _ptr(ptr)
    { }

    unique_array(rv_<unique_array>& rhs)
       : _ptr(rhs._ptr)
    {
        rhs._ptr = 0;
    }

    unique_array& operator=(rv_<unique_array>& rhs)
    {
        reset(rhs._ptr);
        rhs._ptr = 0;
    }

    ~unique_array()
    {
        if (_ptr)
            delete[] _ptr;
    }

    void reset(T p[] = 0)
    {
        if (_ptr)
            delete[] _ptr;
        _ptr = p;
    }

    T* release()
    {
        T* tmp = _ptr;
        _ptr = 0;
        return tmp;
    }

    T& operator[](std::ptrdiff_t idx) const
    {
        return _ptr[idx];
    }

    T* get() const
    {
        return _ptr;
    }

    void swap(unique_array& rhs)
    {
        T* tmp = rhs._ptr;
        rhs._ptr = _ptr;
        _ptr = tmp;
    }

    bool operator!()
    {
        return !_ptr;
    }

    operator undefined_bool_t()
    {
        return _ptr ? HV_UNDEFINED_BOOL_TRUE : HV_UNDEFINED_BOOL_FALSE;
    }

    operator rv_<unique_array>&()
    {
        return move();
    }

    rv_<unique_array>& move()
    {
        return static_cast<rv_<unique_array>&>(*this);
    }

private:
    T* _ptr;
};

template<class T>
inline void swap(unique_array<T>& lhs, unique_array<T>& rhs)
{
    lhs.swap(rhs);
}

} // namespace hv =============================================================

#endif // HV_UNIQUE_PTR_HPP
