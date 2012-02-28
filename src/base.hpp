//
// HealpixViewer - Base Types and Definitions
//
// Author: Bruno Santos <bsantos@av.it.pt>
//

#ifndef HV_BASE_HPP
#define HV_BASE_HPP

#include <cstddef>

#define HV_UNDEFINED_BOOL                              \
    struct undefined_bool_t {                          \
        void true_() {}                                \
    };                                                 \
    typedef void (undefined_bool_t::*undefined_bool)()

#define HV_UNDEFINED_BOOL_TRUE  &undefined_bool_t::true_
#define HV_UNDEFINED_BOOL_FALSE 0

namespace hv { //==============================================================

template<class T>
class rv_ : public T {
    rv_();
    ~rv_();
    rv_(rv_ const&);
    rv_& operator=(rv_ const&);
};

} // namespace hv =============================================================

#endif // HV_BASE_HPP
