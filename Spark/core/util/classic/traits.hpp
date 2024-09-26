#ifndef SPARK_TRAITS_HPP
#define SPARK_TRAITS_HPP

#include <type_traits>

namespace Spark {
    /**
     * @brief A template struct for integral constants.
     * @tparam _Ty The type of the constant.
     * @tparam Val The value of the constant.
     */
    template<typename _Ty, _Ty Val>
    struct IntegralConstant {
        static constexpr _Ty Value = Val;
        using ValueType = _Ty;
        using Type = IntegralConstant;

        constexpr operator ValueType() const noexcept { return Value; }
        constexpr ValueType operator()() const noexcept { return Value; }
    };

    /**
     * @brief Type alias for boolean constants.
     */
    template<bool B> using BoolConstant = IntegralConstant<bool, B>;
    using TrueType = BoolConstant<true>;
    using FalseType = BoolConstant<false>;

    /**
     * @brief Removes reference from a type.
     */
    template<typename _Ty> struct RemoveRef { using Type = _Ty; };
    template<typename _Ty> struct RemoveRef<_Ty&> { using Type = _Ty; };
    template<typename _Ty> struct RemoveRef<_Ty&&> { using Type = _Ty; };

    /**
     * @brief Removes const qualifier from a type.
     */
    template<typename _Ty> struct RemoveConst { using Type = _Ty; };
    template<typename _Ty> struct RemoveConst<const _Ty> { using Type = _Ty; };

    /**
     * @brief Removes volatile qualifier from a type.
     */
    template<typename _Ty> struct RemoveVolatile { using Type = _Ty; };
    template<typename _Ty> struct RemoveVolatile<volatile _Ty> { using Type = _Ty; };

    /**
     * @brief Removes both const and volatile qualifiers from a type.
     */
    template<typename _Ty>
    struct RemoveCV {
        using Type = typename RemoveConst<typename RemoveVolatile<_Ty>::Type>::Type;
    };

    /**
     * @brief Removes pointer from a type.
     */
    template<typename _Ty> struct RemovePointer { using Type = _Ty; };
    template<typename _Ty> struct RemovePointer<_Ty*> { using Type = _Ty; };

    /**
     * @brief Removes the extent (array dimension) from a type.
     */
    template<typename _Ty> struct RemoveExtent { using Type = _Ty; };
    template<typename _Ty> struct RemoveExtent<_Ty[]> { using Type = _Ty; };
    template<typename _Ty, size_t N> struct RemoveExtent<_Ty[N]> { using Type = _Ty; };

    // Type aliases for convenience
    template<typename _Ty> using RemoveRefT = typename RemoveRef<_Ty>::Type;
    template<typename _Ty> using RemoveConstT = typename RemoveConst<_Ty>::Type;
    template<typename _Ty> using RemoveVolatileT = typename RemoveVolatile<_Ty>::Type;
    template<typename _Ty> using RemoveCVT = typename RemoveCV<_Ty>::Type;
    template<typename _Ty> using RemovePointerT = typename RemovePointer<_Ty>::Type;
    template<typename _Ty> using RemoveExtentT = typename RemoveExtent<_Ty>::Type;

    /**
     * @brief Checks if a type is an array.
     */
    template<typename _Ty> struct IsArray : FalseType {};
    template<typename _Ty> struct IsArray<_Ty[]> : TrueType {};
    template<typename _Ty, size_t N> struct IsArray<_Ty[N]> : TrueType {};

    /**
     * @brief Checks if two types are the same.
     */
    template<typename _Ty1, typename _Ty2> struct IsSame : FalseType {};
    template<typename _Ty> struct IsSame<_Ty, _Ty> : TrueType {};

    /**
     * @brief Checks if a type is void.
     */
    template<typename _Ty>
    struct IsVoid : IsSame<RemoveCVT<_Ty>, void> {};

    /**
     * @brief Checks if a type is integral.
     */
    template<typename _Ty> struct IsIntegral : FalseType {};
    template<> struct IsIntegral<bool> : TrueType {};
    template<> struct IsIntegral<char> : TrueType {};
    template<> struct IsIntegral<signed char> : TrueType {};
    template<> struct IsIntegral<unsigned char> : TrueType {};
    template<> struct IsIntegral<wchar_t> : TrueType {};
    template<> struct IsIntegral<char16_t> : TrueType {};
    template<> struct IsIntegral<char32_t> : TrueType {};
    template<> struct IsIntegral<short> : TrueType {};
    template<> struct IsIntegral<unsigned short> : TrueType {};
    template<> struct IsIntegral<int> : TrueType {};
    template<> struct IsIntegral<unsigned int> : TrueType {};
    template<> struct IsIntegral<long> : TrueType {};
    template<> struct IsIntegral<unsigned long> : TrueType {};
    template<> struct IsIntegral<long long> : TrueType {};
    template<> struct IsIntegral<unsigned long long> : TrueType {};

    /**
     * @brief Checks if a type is floating-point.
     */
    template<typename _Ty> struct IsFloatingPoint : FalseType {};
    template<> struct IsFloatingPoint<float> : TrueType {};
    template<> struct IsFloatingPoint<double> : TrueType {};
    template<> struct IsFloatingPoint<long double> : TrueType {};

    /**
     * @brief Checks if a type is arithmetic (integral or floating-point).
     */
    template<typename _Ty>
    struct IsArithmetic : BoolConstant<IsIntegral<_Ty>::Value || IsFloatingPoint<_Ty>::Value> {};

    /**
     * @brief Checks if a type is a pointer.
     */
    template<typename _Ty> struct IsPointer : FalseType {};
    template<typename _Ty> struct IsPointer<_Ty*> : TrueType {};

    /**
     * @brief Checks if a type is an lvalue reference.
     */
    template<typename _Ty> struct IsLValueRef : FalseType {};
    template<typename _Ty> struct IsLValueRef<_Ty&> : TrueType {};

    /**
     * @brief Checks if a type is an rvalue reference.
     */
    template<typename _Ty> struct IsRValueRef : FalseType {};
    template<typename _Ty> struct IsRValueRef<_Ty&&> : TrueType {};

    /**
     * @brief Checks if a type is any kind of reference.
     */
    template<typename _Ty>
    struct IsRef : BoolConstant<IsLValueRef<_Ty>::Value || IsRValueRef<_Ty>::Value> {};

    /**
     * @brief Checks if a type is a const reference.
     */
    template<typename _Ty> struct IsConstRef : FalseType {};
    template<typename _Ty> struct IsConstRef<const _Ty&> : TrueType {};

    /**
     * @brief Checks if a type is const-qualified.
     */
    template<typename _Ty> struct IsConst : FalseType {};
    template<typename _Ty> struct IsConst<const _Ty> : TrueType {};

    /**
     * @brief Checks if a type is volatile-qualified.
     */
    template<typename _Ty> struct IsVolatile : FalseType {};
    template<typename _Ty> struct IsVolatile<volatile _Ty> : TrueType {};

    /**
     * @brief Checks if a type is const- or volatile-qualified.
     */
    template<typename _Ty>
    struct IsCV : BoolConstant<IsConst<_Ty>::Value || IsVolatile<_Ty>::Value> {};

    /**
     * @brief Checks if a type is trivially copyable.
     */
    template<typename _Ty>
    struct IsTriviallyCopyable {
        static constexpr bool Value = __is_trivially_copyable(_Ty);
    };

    /**
     * @brief Checks if a type is trivially assignable.
     */
    template<typename _Ty1, typename _Ty2>
    struct IsTriviallyAssignable {
        static constexpr bool Value = __is_trivially_assignable(_Ty1, _Ty2);
    };

    /**
     * @brief Checks if a type is trivially destructible.
     */
    template<typename _Ty>
    struct IsTriviallyDestructible {
        static constexpr bool Value = __has_trivial_destructor(_Ty);
    };

    /**
     * @brief Checks if a type is trivially constructible.
     */
    template<typename _Ty>
    struct IsTriviallyConstructible {
        static constexpr bool Value = __is_trivially_constructible(_Ty);
    };

    /**
     * @brief Checks if a type is trivial (trivially copyable, destructible, constructible, and assignable).
     */
    template<typename _Ty>
    struct IsTrivial {
        static constexpr bool Value = IsTriviallyCopyable<_Ty>::Value &&
                                      IsTriviallyDestructible<_Ty>::Value &&
                                      IsTriviallyConstructible<_Ty>::Value &&
                                      IsTriviallyAssignable<_Ty, _Ty>::Value;
    };

    /**
     * @brief Checks if a type is default constructible.
     */
    template<typename _Ty>
    struct IsDefaultConstructible {
        static constexpr bool Value = __is_default_constructible(_Ty);
    };

    /**
     * @brief Checks if a type is copy constructible.
     */
    template<typename _Ty>
    struct IsCopyConstructible {
        static constexpr bool Value = __is_copy_constructible(_Ty);
    };

    /**
     * @brief Checks if a type is move constructible.
     */
    template<typename _Ty>
    struct IsMoveConstructible {
        static constexpr bool Value = __is_move_constructible(_Ty);
    };

    /**
     * @brief Checks if a type is copy assignable.
     */
    template<typename _Ty>
    struct IsCopyAssignable {
        static constexpr bool Value = __is_copy_assignable(_Ty);
    };

    /**
     * @brief Checks if a type is move assignable.
     */
    template<typename _Ty>
    struct IsMoveAssignable {
        static constexpr bool Value = __is_move_assignable(_Ty);
    };

    /**
     * @brief Checks if a type is destructible.
     */
    template<typename _Ty>
    struct IsDestructible {
        static constexpr bool Value = __is_destructible(_Ty);
    };

    /**
     * @brief Checks if a type is constructible.
     */
    template<typename _Ty>
    struct IsConstructible {
        static constexpr bool Value = IsDefaultConstructible<_Ty>::Value ||
                                      IsCopyConstructible<_Ty>::Value ||
                                      IsMoveConstructible<_Ty>::Value;
    };

    /**
     * @brief Checks if a type is assignable.
     */
    template<typename _Ty>
    struct IsAssignable {
        static constexpr bool Value = IsCopyAssignable<_Ty>::Value ||
                                      IsMoveAssignable<_Ty>::Value;
    };

    /**
     * @brief Checks if a type is nothrow default constructible.
     */
    template<typename _Ty>
    struct IsNothrowDefaultConstructible {
        static constexpr bool Value = __is_nothrow_default_constructible(_Ty);
    };

    /**
     * @brief Checks if a type is nothrow copy constructible.
     */
    template<typename _Ty>
    struct IsNothrowCopyConstructible {
        static constexpr bool Value = __is_nothrow_copy_constructible(_Ty);
    };

    /**
     * @brief Checks if a type is nothrow move constructible.
     */
    template<typename _Ty>
    struct IsNothrowMoveConstructible {
        static constexpr bool Value = __is_nothrow_move_constructible(_Ty);
    };

    /**
     * @brief Checks if a type is nothrow copy assignable.
     */
    template<typename _Ty>
    struct IsNothrowCopyAssignable {
        static constexpr bool Value = __is_nothrow_copy_assignable(_Ty);
    };

    /**
     * @brief Checks if a type is nothrow move assignable.
     */
    template<typename _Ty>
    struct IsNothrowMoveAssignable {
        static constexpr bool Value = __is_nothrow_move_assignable(_Ty);
    };

    /**
     * @brief Checks if a type is nothrow destructible.
     */
    template<typename _Ty>
    struct IsNothrowDestructible {
        static constexpr bool Value = __is_nothrow_destructible(_Ty);
    };

    /**
     * @brief Checks if a type is nothrow constructible.
     */
    template<typename _Ty>
    struct IsNothrowConstructible {
        static constexpr bool Value = IsNothrowDefaultConstructible<_Ty>::Value ||
                                      IsNothrowCopyConstructible<_Ty>::Value ||
                                      IsNothrowMoveConstructible<_Ty>::Value;
    };

    /**
     * @brief Checks if a type is nothrow assignable.
     */
    template<typename _Ty>
    struct IsNothrowAssignable {
        static constexpr bool Value = IsNothrowCopyAssignable<_Ty>::Value ||
                                      IsNothrowMoveAssignable<_Ty>::Value;
    };

    /**
     * @brief Checks if a type is nothrow swappable.
     */
    template<typename _Ty>
    struct IsNothrowSwappable {
        static constexpr bool Value = __is_nothrow_swappable(_Ty);
    };

    /**
     * @brief Checks if a type is swappable.
     */
    template<typename _Ty>
    struct IsSwappable {
        static constexpr bool Value = __is_swappable(_Ty);
    };

    /**
     * @brief Checks if a type is trivially swappable.
     */
    template<typename _Ty>
    struct IsTriviallySwappable {
        static constexpr bool Value = __is_trivially_swappable(_Ty);
    };

    /**
     * @brief Checks if a type is an enumeration.
     */
    template<typename _Ty>
    struct IsEnum {
        static constexpr bool Value = __is_enum(_Ty);
    };

    /**
     * @brief Checks if a type is a union.
     */
    template<typename _Ty>
    struct IsUnion {
        static constexpr bool Value = __is_union(_Ty);
    };

    /**
     * @brief Checks if a type is a class.
     */
    template<typename _Ty>
    struct IsClass {
        static constexpr bool Value = __is_class(_Ty);
    };

    /**
     * @brief Checks if a type is a function.
     */
    template<typename T> struct IsFunction : FalseType {};

    template<typename Ret, typename... Args>
    struct IsFunction<Ret(Args...)> : TrueType {};

    template<typename Ret, typename... Args>
    struct IsFunction<Ret (*)(Args...)> : TrueType {};

    template<typename Ret, typename Class, typename... Args>
    struct IsFunction<Ret (Class::*)(Args...)> : TrueType {};

    template<typename Ret, typename Class, typename... Args>
    struct IsFunction<Ret (Class::*)(Args...) const> : TrueType {};

    /**
     * @brief Checks if a type is an object type.
     */
    template<typename _Ty>
    struct IsObject {
        static constexpr bool Value = __is_object(_Ty);
    };

    /**
     * @brief Checks if a type is a scalar type.
     */
    template<typename _Ty>
    struct IsScalar {
        static constexpr bool Value = __is_scalar(_Ty);
    };

    /**
     * @brief Checks if a type is a compound type.
     */
    template<typename _Ty>
    struct IsCompound {
        static constexpr bool Value = __is_compound(_Ty);
    };

    /**
     * @brief Adds a pointer to a type.
     */
    template<typename _Ty>
    struct AddPointer {
        using Type = typename RemoveRef<_Ty>::Type*;
    };

    /**
     * @brief Conditional type selection.
     */
    template<bool B, typename _Ty, typename F>
    struct Conditional {
        using Type = _Ty;
    };

    template<typename _Ty, typename F>
    struct Conditional<false, _Ty, F> {
        using Type = F;
    };

    template<bool B, typename _Ty, typename F>
    using ConditionalT = typename Conditional<B, _Ty, F>::Type;

    /**
     * @brief Checks if one type is a base of another.
     */
    template<typename Base, typename Derived>
    struct IsBaseOf {
        static constexpr bool Value = __is_base_of(Base, Derived);
    };

    /**
     * @brief Enables a type if a condition is true.
     */
    template<bool B, typename _Ty = void>
    struct EnableIf {};

    template<typename _Ty>
    struct EnableIf<true, _Ty> {
        using Type = _Ty;
    };

    template<bool B, typename _Ty = void>
    using EnableIfT = typename EnableIf<B, _Ty>::Type;

    /**
     * @brief Checks if one type is convertible to another.
     */
    template<typename _Ty1, typename _Ty2>
    using IsConvertible = BoolConstant<__is_convertible_to(_Ty1, _Ty2)>;

    template<typename _Ty1, typename _Ty2>
    constexpr bool IsConvertibleV = __is_convertible_to(_Ty1, _Ty2);

    /**
     * @brief Concept to check if one type is derived from another.
     */
    template<typename Derived, typename Base>
    concept DerivedFrom = __is_base_of(Base, Derived) && __is_convertible_to(const volatile Derived*, const volatile Base*);

    // Value aliases for traits
    template<typename _Ty> constexpr bool IsArrayV = IsArray<_Ty>::Value;
    template<typename _Ty> constexpr bool IsIntegralV = IsIntegral<_Ty>::Value;
    template<typename _Ty> constexpr bool IsFloatingPointV = IsFloatingPoint<_Ty>::Value;
    template<typename _Ty> constexpr bool IsArithmeticV = IsArithmetic<_Ty>::Value;
    template<typename _Ty> constexpr bool IsFunctionV = IsFunction<_Ty>::Value;
    template<typename _Ty> constexpr bool IsPointerV = IsPointer<_Ty>::Value;
    template<typename _Ty> constexpr bool IsLValueRefV = IsLValueRef<_Ty>::Value;
    template<typename _Ty> constexpr bool IsRValueRefV = IsRValueRef<_Ty>::Value;
    template<typename _Ty> constexpr bool IsRefV = IsRef<_Ty>::Value;
    template<typename _Ty> constexpr bool IsConstRefV = IsConstRef<_Ty>::Value;
    template<typename _Ty> constexpr bool IsConstV = IsConst<_Ty>::Value;
    template<typename _Ty> constexpr bool IsVolatileV = IsVolatile<_Ty>::Value;
    template<typename _Ty> constexpr bool IsCVV = IsCV<_Ty>::Value;
    template<typename _Ty> constexpr bool IsTriviallyCopyableV = IsTriviallyCopyable<_Ty>::Value;
    template<typename _Ty> constexpr bool IsTriviallyDestructibleV = IsTriviallyDestructible<_Ty>::Value;
    template<typename _Ty> constexpr bool IsTriviallyConstructibleV = IsTriviallyConstructible<_Ty>::Value;
    template<typename _Ty1, typename _Ty2> constexpr bool IsTriviallyAssignableV = IsTriviallyAssignable<_Ty1, _Ty2>::Value;
    template<typename _Ty> constexpr bool IsTrivialV = IsTrivial<_Ty>::Value;
    template<typename _Ty> constexpr bool IsDefaultConstructibleV = IsDefaultConstructible<_Ty>::Value;
    template<typename _Ty> constexpr bool IsCopyConstructibleV = IsCopyConstructible<_Ty>::Value;
    template<typename _Ty> constexpr bool IsMoveConstructibleV = IsMoveConstructible<_Ty>::Value;
    template<typename _Ty> constexpr bool IsCopyAssignableV = IsCopyAssignable<_Ty>::Value;
    template<typename _Ty> constexpr bool IsMoveAssignableV = IsMoveAssignable<_Ty>::Value;
    template<typename _Ty> constexpr bool IsDestructibleV = IsDestructible<_Ty>::Value;
    template<typename _Ty> constexpr bool IsConstructibleV = IsConstructible<_Ty>::Value;
    template<typename _Ty> constexpr bool IsAssignableV = IsAssignable<_Ty>::Value;
    template<typename _Ty> constexpr bool IsNothrowDefaultConstructibleV = IsNothrowDefaultConstructible<_Ty>::Value;
    template<typename _Ty1, typename _Ty2> constexpr bool IsSameV = IsSame<_Ty1, _Ty2>::Value;
    template<typename Base, typename Derived> constexpr bool IsBaseOfV = IsBaseOf<Base, Derived>::Value;
    template<typename _Ty> using AddPointerT = typename AddPointer<_Ty>::Type;

    template<bool>
    struct Select {
        template<class _Ty1, class>
        using Apply = _Ty1;
    };

    template<>
    struct Select<false> {
        template<class, class _Ty2>
        using Apply = _Ty2;
    };

    /**
     * @brief Decays a type, removing references and cv-qualifiers.
     */
    template<typename _Ty>
    struct Decay {
        using Uty = RemoveRefT<_Ty>;
        using Ty2 = typename Select<IsFunction<Uty>::Value>::template Apply<AddPointerT<Uty>, RemoveCVT<Uty>>;
        using Type = typename Select<IsArray<Uty>::Value>::template Apply<AddPointerT<RemoveExtentT<Uty>>, Ty2>;
    };

    template<class _Ty>
    using DecayT = typename Decay<_Ty>::Type;

    /**
     * @brief Helper struct for InvokeResult.
     */
    template<typename _Void, typename _Callable, typename... _Args>
    struct InvokeResultImpl {
        // Default case: not invocable
    };
    
    template<typename _Callable, typename... _Args>
    struct InvokeResultImpl<EnableIfT<IsSameV<decltype(DeclVal<_Callable>()(DeclVal<_Args>()...)), void>>, _Callable, _Args...> {
        using Type = void;
    };

    template<typename _Callable, typename... _Args>
    struct InvokeResultImpl<EnableIfT<!IsSameV<decltype(DeclVal<_Callable>()(DeclVal<_Args>()...)), void>>, _Callable, _Args...> {
        using Type = decltype(DeclVal<_Callable>()(DeclVal<_Args>()...));
    };

    /**
     * @brief Determines the result type of invoking a callable with given arguments.
     */
    template<typename _Callable, typename... _Args>
    struct InvokeResult : InvokeResultImpl<void, _Callable, _Args...> {};

    template<typename _Callable, typename... _Args>
    using InvokeResultT = typename InvokeResult<_Callable, _Args...>::Type;

    /**
     * @brief Determines the common type among multiple types.
     */
    template<typename... _Types>
    struct CommonType;

    // Base case: empty or single type
    template<>
    struct CommonType<> {};

    template<typename _Ty>
    struct CommonType<_Ty> {
        using Type = DecayT<_Ty>;
    };

    // Recursive case: two or more types
    template<typename _Ty1, typename _Ty2, typename... _Rest>
    struct CommonType<_Ty1, _Ty2, _Rest...> {
    private:
        using Type1 = typename CommonType<_Ty1, _Ty2>::Type;
        using Type2 = typename CommonType<_Rest...>::Type;
    public:
        using Type = typename CommonType<Type1, Type2>::Type;
    };

    // Specialization for two types
    template<typename _Ty1, typename _Ty2>
    struct CommonType<_Ty1, _Ty2> {
    private:
        using Decay1 = DecayT<_Ty1>;
        using Decay2 = DecayT<_Ty2>;
        
        template<typename T1, typename T2>
        static auto test(i32) -> decltype(true ? DeclVal<T1>() : DeclVal<T2>());
        
        template<typename, typename>
        static auto test(...) -> void;

        using ResultType = decltype(test<Decay1, Decay2>(0));
    public:
        using Type = typename Select<!IsSameV<ResultType, void>>::template Apply<DecayT<ResultType>, void>;
    };

    template<typename... _Types>
    using CommonTypeT = typename CommonType<_Types...>::Type;

} // namespace Spark

#endif // SPARK_TRAITS_HPP
