#ifndef SPARK_TRAITS_HPP
#define SPARK_TRAITS_HPP

#include <type_traits>

namespace Spark {
	// IntegralConstant
	template<typename _Ty, _Ty Val> struct IntegralConstant {
		static constexpr _Ty Value = Val;
		using ValueType            = _Ty;
		using Type                 = IntegralConstant;

		constexpr           operator ValueType() const noexcept { return Value; }
		constexpr ValueType operator()() const noexcept { return Value; }
	};

	// BoolConstant and related aliases
	template<bool B> using BoolConstant = IntegralConstant<bool, B>;
	using TrueType                      = BoolConstant<true>;
	using FalseType                     = BoolConstant<false>;

	// Remove Reference
	template<typename _Ty> struct RemoveRef {
		using Type = _Ty;
	};
	template<typename _Ty> struct RemoveRef<_Ty&> {
		using Type = _Ty;
	};
	template<typename _Ty> struct RemoveRef<_Ty&&> {
		using Type = _Ty;
	};

	// Remove Const
	template<typename _Ty> struct RemoveConst {
		using Type = _Ty;
	};
	template<typename _Ty> struct RemoveConst<const _Ty> {
		using Type = _Ty;
	};

	// Remove Volatile
	template<typename _Ty> struct RemoveVolatile {
		using Type = _Ty;
	};
	template<typename _Ty> struct RemoveVolatile<volatile _Ty> {
		using Type = _Ty;
	};

	// Remove Const and Volatile
	template<typename _Ty> struct RemoveCV {
		using Type = typename RemoveConst<typename RemoveVolatile<_Ty>::Type>::Type;
	};

	// Remove Pointer
	template<typename _Ty> struct RemovePointer {
		using Type = _Ty;
	};
	template<typename _Ty> struct RemovePointer<_Ty*> {
		using Type = _Ty;
	};

	// Remove Extent
	template<typename _Ty> struct RemoveExtent {
		using Type = _Ty;
	};
	template<typename _Ty> struct RemoveExtent<_Ty[]> {
		using Type = _Ty;
	};
	template<typename _Ty, size_t  N> struct RemoveExtent<_Ty[N]> {
		using Type = _Ty;
	};

	// InvokeResult trait
	template<typename _Void, typename _Callable, typename... _Args>
	struct InvokeResultImpl {
		// Default case: not invocable
	};

	template<typename _Callable, typename... _Args>
	struct InvokeResultImpl<
		EnableIfT<IsSameV<decltype(DeclVal<_Callable>()(DeclVal<_Args>()...)), void>>,
		_Callable, _Args...
	> {
		using Type = void;
	};

	template<typename _Callable, typename... _Args>
	struct InvokeResultImpl<
		EnableIfT<!IsSameV<decltype(DeclVal<_Callable>()(DeclVal<_Args>()...)), void>>,
		_Callable, _Args...
	> {
		using Type = decltype(DeclVal<_Callable>()(DeclVal<_Args>()...));
	};

	template<typename _Callable, typename... _Args>
	struct InvokeResult : InvokeResultImpl<void, _Callable, _Args...> {};

	template<typename _Callable, typename... _Args>
	using InvokeResultT = typename InvokeResult<_Callable, _Args...>::Type;

	// Type aliases for convenience
	template<typename _Ty> using RemoveRefT      = typename RemoveRef<_Ty>::Type;
	template<typename _Ty> using RemoveConstT    = typename RemoveConst<_Ty>::Type;
	template<typename _Ty> using RemoveVolatileT = typename RemoveVolatile<_Ty>::Type;
	template<typename _Ty> using RemoveCVT       = typename RemoveCV<_Ty>::Type;
	template<typename _Ty> using RemovePointerT  = typename RemovePointer<_Ty>::Type;
	template<typename _Ty> using RemoveExtentT   = typename RemoveExtent<_Ty>::Type;

	// IsArray Trait
	template<typename _Ty> struct IsArray {
		static constexpr bool Value = false;
	};
	template<typename _Ty> struct IsArray<_Ty[]> {
		static constexpr bool Value = true;
	};
	template<typename _Ty, size_t  N> struct IsArray<_Ty[N]> {
		static constexpr bool Value = true;
	};

	// IsSame Trait
	template<typename _Ty1, typename _Ty2> struct IsSame {
		static constexpr bool Value = false;
	};

	template<typename _Ty> struct IsSame<_Ty, _Ty> {
		static constexpr bool Value = true;
	};

	// IsVoid Trait
	template<typename _Ty> struct IsVoid {
		static constexpr bool Value = IsSame<_Ty, void>::Value;
	};

	// IsIntegral Trait
	template<typename _Ty> struct IsIntegral {
		static constexpr bool Value = false;
	};
	// Specializations for integral types
	template<> struct IsIntegral<bool> {
		static constexpr bool Value = true;
	};
	template<> struct IsIntegral<char> {
		static constexpr bool Value = true;
	};
	template<> struct IsIntegral<signed char> {
		static constexpr bool Value = true;
	};
	template<> struct IsIntegral<unsigned char> {
		static constexpr bool Value = true;
	};
	template<> struct IsIntegral<wchar_t> {
		static constexpr bool Value = true;
	};
	template<> struct IsIntegral<char16_t> {
		static constexpr bool Value = true;
	};
	template<> struct IsIntegral<char32_t> {
		static constexpr bool Value = true;
	};
	template<> struct IsIntegral<short> {
		static constexpr bool Value = true;
	};
	template<> struct IsIntegral<unsigned short> {
		static constexpr bool Value = true;
	};
	template<> struct IsIntegral<int> {
		static constexpr bool Value = true;
	};
	template<> struct IsIntegral<unsigned int> {
		static constexpr bool Value = true;
	};
	template<> struct IsIntegral<long> {
		static constexpr bool Value = true;
	};
	template<> struct IsIntegral<unsigned long> {
		static constexpr bool Value = true;
	};
	template<> struct IsIntegral<long long> {
		static constexpr bool Value = true;
	};
	template<> struct IsIntegral<unsigned long long> {
		static constexpr bool Value = true;
	};

	// IsFloatingPoint Trait
	template<typename _Ty> struct IsFloatingPoint {
		static constexpr bool Value = false;
	};
	// Specializations for floating-point types
	template<> struct IsFloatingPoint<float> {
		static constexpr bool Value = true;
	};
	template<> struct IsFloatingPoint<double> {
		static constexpr bool Value = true;
	};
	template<> struct IsFloatingPoint<long double> {
		static constexpr bool Value = true;
	};

	// IsArithmetic Trait
	template<typename _Ty> struct IsArithmetic {
		static constexpr bool Value = IsIntegral<_Ty>::Value || IsFloatingPoint<_Ty>::Value;
	};

	// IsPointer Trait
	template<typename _Ty> struct IsPointer {
		static constexpr bool Value = false;
	};
	template<typename _Ty> struct IsPointer<_Ty*> {
		static constexpr bool Value = true;
	};

	// IsLValueRef Trait
	template<typename _Ty> struct IsLValueRef {
		static constexpr bool Value = false;
	};
	template<typename _Ty> struct IsLValueRef<_Ty&> {
		static constexpr bool Value = true;
	};

	// IsRValueRef Trait
	template<typename _Ty> struct IsRValueRef {
		static constexpr bool Value = false;
	};
	template<typename _Ty> struct IsRValueRef<_Ty&&> {
		static constexpr bool Value = true;
	};

	// IsRef Trait
	template<typename _Ty> struct IsRef {
		static constexpr bool Value = IsLValueRef<_Ty>::Value || IsRValueRef<_Ty>::Value;
	};

	// IsConstRef Trait
	template<typename _Ty> struct IsConstRef {
		static constexpr bool Value = false;
	};
	template<typename _Ty> struct IsConstRef<const _Ty&> {
		static constexpr bool Value = true;
	};

	// IsConst Trait
	template<typename _Ty> struct IsConst {
		static constexpr bool Value = false;
	};
	template<typename _Ty> struct IsConst<const _Ty> {
		static constexpr bool Value = true;
	};

	// IsVolatile Trait
	template<typename _Ty> struct IsVolatile {
		static constexpr bool Value = false;
	};
	template<typename _Ty> struct IsVolatile<volatile _Ty> {
		static constexpr bool Value = true;
	};

	// IsCV Trait
	template<typename _Ty> struct IsCV {
		static constexpr bool Value = IsConst<_Ty>::Value || IsVolatile<_Ty>::Value;
	};

	// Trivially Copyable Traits using compiler built-ins
	template<typename _Ty> struct IsTriviallyCopyable {
		static constexpr bool Value = __is_trivially_copyable(_Ty);
	};

	// Trivially Assignable Traits using compiler built-ins
	template<typename _Ty1, typename _Ty2> struct IsTriviallyAssignable {
		static constexpr bool Value = __is_trivially_assignable(_Ty1, _Ty2);
	};

	// Trivially Destructible Traits using compiler built-ins
	template<typename _Ty> struct IsTriviallyDestructible {
		static constexpr bool Value = __has_trivial_destructor(_Ty);
	};

	// Trivially Constructible Traits using compiler built-ins
	template<typename _Ty> struct IsTriviallyConstructible {
		static constexpr bool Value = __is_trivially_constructible(_Ty);
	};

	// IsTrivial Trait
	template<typename _Ty> struct IsTrivial {
		static constexpr bool Value = IsTriviallyCopyable<_Ty>::Value && IsTriviallyDestructible<_Ty>::Value && IsTriviallyConstructible<_Ty>::Value && IsTriviallyAssignable<_Ty>::Value;
	};

	// Constructibility Traits using compiler built-ins
	template<typename _Ty> struct IsDefaultConstructible {
		static constexpr bool Value = __is_default_constructible(_Ty);
	};
	template<typename _Ty> struct IsCopyConstructible {
		static constexpr bool Value = __is_copy_constructible(_Ty);
	};
	template<typename _Ty> struct IsMoveConstructible {
		static constexpr bool Value = __is_move_constructible(_Ty);
	};

	// Assignability Traits using compiler built-ins
	template<typename _Ty> struct IsCopyAssignable {
		static constexpr bool Value = __is_copy_assignable(_Ty);
	};
	template<typename _Ty> struct IsMoveAssignable {
		static constexpr bool Value = __is_move_assignable(_Ty);
	};

	// Destructibility Trait using compiler built-ins
	template<typename _Ty> struct IsDestructible {
		static constexpr bool Value = __is_destructible(_Ty);
	};

	// IsConstructible Trait
	template<typename _Ty> struct IsConstructible {
		static constexpr bool Value = IsDefaultConstructible<_Ty>::Value || IsCopyConstructible<_Ty>::Value || IsMoveConstructible<_Ty>::Value;
	};

	// IsAssignable Trait
	template<typename _Ty> struct IsAssignable {
		static constexpr bool Value = IsCopyAssignable<_Ty>::Value || IsMoveAssignable<_Ty>::Value;
	};

	// Nothrow Constructibility Traits using compiler built-ins
	template<typename _Ty> struct IsNothrowDefaultConstructible {
		static constexpr bool Value = __is_nothrow_default_constructible(_Ty);
	};
	template<typename _Ty> struct IsNothrowCopyConstructible {
		static constexpr bool Value = __is_nothrow_copy_constructible(_Ty);
	};
	template<typename _Ty> struct IsNothrowMoveConstructible {
		static constexpr bool Value = __is_nothrow_move_constructible(_Ty);
	};

	// Nothrow Assignability Traits using compiler built-ins
	template<typename _Ty> struct IsNothrowCopyAssignable {
		static constexpr bool Value = __is_nothrow_copy_assignable(_Ty);
	};
	template<typename _Ty> struct IsNothrowMoveAssignable {
		static constexpr bool Value = __is_nothrow_move_assignable(_Ty);
	};

	// Nothrow Destructible Trait using compiler built-ins
	template<typename _Ty> struct IsNothrowDestructible {
		static constexpr bool Value = __is_nothrow_destructible(_Ty);
	};

	// Nothrow Constructible Trait
	template<typename _Ty> struct IsNothrowConstructible {
		static constexpr bool Value = IsNothrowDefaultConstructible<_Ty>::Value || IsNothrowCopyConstructible<_Ty>::Value || IsNothrowMoveConstructible<_Ty>::Value;
	};

	// Nothrow Assignable Trait
	template<typename _Ty> struct IsNothrowAssignable {
		static constexpr bool Value = IsNothrowCopyAssignable<_Ty>::Value || IsNothrowMoveAssignable<_Ty>::Value;
	};

	// Swappability Traits using compiler built-ins
	template<typename _Ty> struct IsNothrowSwappable {
		static constexpr bool Value = __is_nothrow_swappable(_Ty);
	};
	template<typename _Ty> struct IsSwappable {
		static constexpr bool Value = __is_swappable(_Ty);
	};
	template<typename _Ty> struct IsTriviallySwappable {
		static constexpr bool Value = __is_trivially_swappable(_Ty);
	};

	// Type Classification Traits using compiler built-ins
	template<typename _Ty> struct IsEnum {
		static constexpr bool Value = __is_enum(_Ty);
	};
	template<typename _Ty> struct IsUnion {
		static constexpr bool Value = __is_union(_Ty);
	};
	template<typename _Ty> struct IsClass {
		static constexpr bool Value = __is_class(_Ty);
	};
	template<typename T> struct IsFunction : FalseType {};

	template<typename Ret, typename... Args> struct IsFunction<Ret(Args...)> : TrueType {};

	template<typename Ret, typename... Args> struct IsFunction<Ret (*)(Args...)> : TrueType {};

	template<typename Ret, typename Class, typename... Args> struct IsFunction<Ret (Class::*)(Args...)> : TrueType {};

	template<typename Ret, typename Class, typename... Args> struct IsFunction<Ret (Class::*)(Args...) const> : TrueType {};

	template<typename _Ty> struct IsObject {
		static constexpr bool Value = __is_object(_Ty);
	};
	template<typename _Ty> struct IsScalar {
		static constexpr bool Value = __is_scalar(_Ty);
	};
	template<typename _Ty> struct IsCompound {
		static constexpr bool Value = __is_compound(_Ty);
	};

	// AddPointer Trait
	template<typename _Ty> struct AddPointer {
		using Type = typename RemoveRef<_Ty>::Type*;
	};

	// Conditional Trait
	template<bool B, typename _Ty, typename F> struct Conditional {
		using Type = _Ty;
	};
	template<typename _Ty, typename F> struct Conditional<false, _Ty, F> {
		using Type = F;
	};

	template<bool B, typename _Ty, typename F> using ConditionalT = typename Conditional<B, _Ty, F>::Type;

	// IsBaseOf Trait using compiler built-ins
	template<typename Base, typename Derived>
	struct IsBaseOf {
		static constexpr bool Value = __is_base_of(Base, Derived);
	};

	// EnableIf Trait
	template<bool B, typename _Ty = void> struct EnableIf {};
	template<typename _Ty> struct EnableIf<true, _Ty> {
		using Type = _Ty;
	};

	template<bool B, typename _Ty = void> using EnableIfT                = typename EnableIf<B, _Ty>::Type;

	// IsConvertible Trait using compiler built-ins
	template<typename _Ty1, typename _Ty2> using IsConvertible           = BoolConstant<__is_convertible_to(_Ty1, _Ty2)>;
	template<typename _Ty1, typename _Ty2> constexpr bool IsConvertibleV = __is_convertible_to(_Ty1, _Ty2);

	// DerivedFrom Concept using compiler built-ins
	template<typename Derived, typename Base>
	concept DerivedFrom                                                                  = __is_base_of(Base, Derived) && __is_convertible_to(const volatile Derived*, const volatile Base*);

	// Value aliases for traits
	template<typename _Ty> constexpr bool                 IsArrayV                       = IsArray<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsIntegralV                    = IsIntegral<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsFloatingPointV               = IsFloatingPoint<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsArithmeticV                  = IsArithmetic<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsFunctionV                    = IsFunction<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsPointerV                     = IsPointer<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsLValueRefV                   = IsLValueRef<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsRValueRefV                   = IsRValueRef<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsRefV                         = IsRef<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsConstRefV                    = IsConstRef<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsConstV                       = IsConst<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsVolatileV                    = IsVolatile<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsCVV                          = IsCV<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsTriviallyCopyableV           = IsTriviallyCopyable<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsTriviallyDestructibleV       = IsTriviallyDestructible<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsTriviallyConstructibleV      = IsTriviallyConstructible<_Ty>::Value;
	template<typename _Ty1, typename _Ty2> constexpr bool IsTriviallyAssignableV         = IsTriviallyAssignable<_Ty1, _Ty2>::Value;
	template<typename _Ty> constexpr bool                 IsTrivialV                     = IsTrivial<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsDefaultConstructibleV        = IsDefaultConstructible<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsCopyConstructibleV           = IsCopyConstructible<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsMoveConstructibleV           = IsMoveConstructible<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsCopyAssignableV              = IsCopyAssignable<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsMoveAssignableV              = IsMoveAssignable<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsDestructibleV                = IsDestructible<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsConstructibleV               = IsConstructible<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsAssignableV                  = IsAssignable<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsNothrowDefaultConstructibleV = IsNothrowDefaultConstructible<_Ty>::Value;
	template<typename _Ty1, typename _Ty2> constexpr bool IsSameV                        = IsSame<_Ty1, _Ty2>::Value;
	template<typename Base, typename Derived> constexpr bool IsBaseOfV                   = IsBaseOf<Base, Derived>::Value;
	template<typename _Ty> using AddPointerT                                             = AddPointer<_Ty>::Type;

	template<bool> struct Select {
		template<class _Ty1, class> using Apply = _Ty1;
	};

	template<> struct Select<false> {
		template<class, class _Ty2> using Apply = _Ty2;
	};

	// Decay Trait
	template<typename _Ty> struct Decay {
		using Uty  = RemoveRefT<_Ty>;
		using Ty2  = typename Select<IsFunction<Uty>::Value>::template Apply<AddPointerT<Uty>, RemoveCVT<Uty>>;
		using Type = typename Select<IsArray<Uty>::Value>::template Apply<AddPointerT<RemoveExtentT<Uty>>, Ty2>;
	};

	template<class _Ty> using DecayT = typename Decay<_Ty>::Type;

} // namespace Spark

#endif // SPARK_TRAITS_HPP
