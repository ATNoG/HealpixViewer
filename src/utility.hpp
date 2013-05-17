//
// HealpixViewer - Base Types and Definitions
//
// Author: Bruno Santos <bsantos@av.it.pt>
//

#ifndef HV_UTILITY_HPP
#define HV_UTILITY_HPP

#include <cstddef>

#define HV_UNDEFINED_BOOL                              \
    struct undefined_bool_t {                          \
        void true_() {}                                \
    };                                                 \
    typedef void (undefined_bool_t::*undefined_bool)()

#define HV_UNDEFINED_BOOL_TRUE  &undefined_bool_t::true_
#define HV_UNDEFINED_BOOL_FALSE 0

namespace hv { //==============================================================

template<class T, T Value>
struct integral_constant {
	typedef T      type;
	static const T value = Value;
};

template<class T> struct is_reference         : integral_constant<bool, false> { };
template<class T> struct is_reference<T&>     : integral_constant<bool, true> { };
template<class T> struct is_const             : integral_constant<bool, false> { };
template<class T> struct is_const<T const>    : integral_constant<bool, true> { };
template<class T> struct is_volatile          : integral_constant<bool, false> { };
template<class T> struct is_volatile<T const> : integral_constant<bool, true> { };
template<class T> struct is_cv : integral_constant<bool, is_const<T>::value || is_volatile<T>::value> { };

template<class T> struct add_reference               { typedef T& type; };
template<class T> struct add_const                   { typedef T const type; };
template<class T> struct add_const_reference         { typedef T const& type; };

template<class T> struct remove_reference            { typedef T type; };
template<class T> struct remove_reference<T&>        { typedef T type; };
template<class T> struct remove_const                { typedef T type; };
template<class T> struct remove_const<T const>       { typedef T type; };
template<class T> struct remove_volatile             { typedef T type; };
template<class T> struct remove_volatile<T volatile> { typedef T type; };
template<class T> struct remove_pointer              { typedef T type; };
template<class T> struct remove_pointer<T*>          { typedef T type; };
template<class T> struct remove_cv {
	typedef typename remove_const<typename remove_volatile<T>::type>::type type;
};

template<class T>
class rv_ {
	typedef typename remove_reference<T>::type type;

	type& _ref;

public:
    explicit rv_(type& ref)
        : _ref(ref)
    { }

    T* operator->() { return &_ref; }
};

template<class T>
T move(T& from)
{
	return T(rv_<T>(from));
}

} // namespace hv =============================================================

#endif // HV_UTILITY_HPP
