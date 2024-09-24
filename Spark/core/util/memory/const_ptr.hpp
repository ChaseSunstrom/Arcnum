#ifndef SPARK_CONST_PTR_HPP
#define SPARK_CONST_PTR_HPP

#include <core/pch.hpp>
#include "ref_ptr.hpp"


namespace Spark {
    /**
     * @brief Alias template for a constant reference-counted pointer
     * @tparam _Ty The type of the object being pointed to
     */
    template <typename _Ty>
    using ConstPtr = const RefPtr<_Ty>;
    
    /**
     * @brief Performs a const_cast on a ConstPtr
     * @tparam To The type to cast to
     * @tparam From The type to cast from
     * @param from The ConstPtr to cast
     * @return A new ConstPtr of the desired type
     */
    template<typename To, typename From>
    ConstPtr<To> ConstCast(const ConstPtr<From>& from) {
        return ConstPtr<To>(static_cast<To*>(const_cast<From*>(from.Get())));
    }
}



#endif