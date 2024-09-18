#ifndef SPARK_TRAITS_HPP
#define SPARK_TRAITS_HPP

#include <type_traits>

namespace Spark {
	template<typename _Ty> struct RemoveRef {
		using Type = _Ty;
	};
	template<typename _Ty> struct RemoveRef<_Ty&> {
		using Type = _Ty;
	};
	template<typename _Ty> struct RemoveRef<_Ty&&> {
		using Type = _Ty;
	};

	template<typename _Ty> struct RemoveConst {
		using Type = _Ty;
	};
	template<typename _Ty> struct RemoveConst<const _Ty> {
		using Type = _Ty;
	};

	template<typename _Ty> struct RemoveVolatile {
		using Type = _Ty;
	};
	template<typename _Ty> struct RemoveVolatile<volatile _Ty> {
		using Type = _Ty;
	};

	template<typename _Ty> struct RemoveCV {
		using Type = typename RemoveConst<typename RemoveVolatile<_Ty>::Type>::Type;
	};

	template<typename _Ty> struct RemovePointer {
		using Type = _Ty;
	};
	template<typename _Ty> struct RemovePointer<_Ty*> {
		using Type = _Ty;
	};

	template<typename _Ty> struct RemoveExtent {
		using Type = _Ty;
	};
	template<typename _Ty> struct RemoveExtent<_Ty[]> {
		using Type = _Ty;
	};
	template<typename _Ty, size_t N> struct RemoveExtent<_Ty[N]> {
		using Type = _Ty;
	};

	template<typename _Ty> using RemoveRefT      = typename RemoveRef<_Ty>::Type;
	template<typename _Ty> using RemoveConstT    = typename RemoveConst<_Ty>::Type;
	template<typename _Ty> using RemoveVolatileT = typename RemoveVolatile<_Ty>::Type;
	template<typename _Ty> using RemoveCVT       = typename RemoveCV<_Ty>::Type;
	template<typename _Ty> using RemovePointerT  = typename RemovePointer<_Ty>::Type;
	template<typename _Ty> using RemoveExtentT   = typename RemoveExtent<_Ty>::Type;

	template<typename _Ty> struct IsArray {
		static constexpr bool Value = false;
	};
	template<typename _Ty> struct IsArray<_Ty[]> {
		static constexpr bool Value = true;
	};
	template<typename _Ty, size_t N> struct IsArray<_Ty[N]> {
		static constexpr bool Value = true;
	};

	template<typename _Ty1, typename _Ty2> struct IsSame {
		static constexpr bool Value = false;
	};

	template<typename _Ty> struct IsSame<_Ty, _Ty> {
		static constexpr bool Value = true;
	};

	template<typename _Ty> struct IsVoid {
		static constexpr bool Value = IsSame<_Ty, void>::Value;
	};

	template<typename _Ty> struct IsIntegral {
		static constexpr bool Value = false;
	};

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

	template<typename _Ty> struct IsFloatingPoint {
		static constexpr bool Value = false;
	};

	template<> struct IsFloatingPoint<float> {
		static constexpr bool Value = true;
	};

	template<> struct IsFloatingPoint<double> {
		static constexpr bool Value = true;
	};

	template<> struct IsFloatingPoint<long double> {
		static constexpr bool Value = true;
	};

	template<typename _Ty> struct IsArithmetic {
		static constexpr bool Value = IsIntegral<_Ty>::Value || IsFloatingPoint<_Ty>::Value;
	};

	template<typename _Ty> struct IsPointer {
		static constexpr bool Value = false;
	};

	template<typename _Ty> struct IsPointer<_Ty*> {
		static constexpr bool Value = true;
	};

	template<typename _Ty> struct IsLValueRef {
		static constexpr bool Value = false;
	};

	template<typename _Ty> struct IsLValueRef<_Ty&> {
		static constexpr bool Value = true;
	};

	template<typename _Ty> struct IsRValueRef {
		static constexpr bool Value = false;
	};

	template<typename _Ty> struct IsRValueRef<_Ty&&> {
		static constexpr bool Value = true;
	};

	template<typename _Ty> struct IsRef {
		static constexpr bool Value = IsLValueRef<_Ty>::Value || IsRValueRef<_Ty>::Value;
	};

	template<typename _Ty> struct IsConstRef {
		static constexpr bool Value = false;
	};

	template<typename _Ty> struct IsConstRef<const _Ty&> {
		static constexpr bool Value = true;
	};

	template<typename _Ty> struct IsConst {
		static constexpr bool Value = false;
	};

	template<typename _Ty> struct IsConst<const _Ty> {
		static constexpr bool Value = true;
	};

	template<typename _Ty> struct IsVolatile {
		static constexpr bool Value = false;
	};

	template<typename _Ty> struct IsVolatile<volatile _Ty> {
		static constexpr bool Value = true;
	};

	template<typename _Ty> struct IsCV {
		static constexpr bool Value = IsConst<_Ty>::Value || IsVolatile<_Ty>::Value;
	};

	template<typename _Ty> struct IsTriviallyCopyable {
		static constexpr bool Value = __is_trivially_copyable(_Ty);
	};

	template<typename _Ty> struct IsTriviallyDestructible {
		static constexpr bool Value = __has_trivial_destructor(_Ty);
	};

	template<typename _Ty> struct IsTriviallyConstructible {
		static constexpr bool Value = __is_trivially_constructible(_Ty);
	};

	template<typename _Ty> struct IsTriviallyAssignable {
		static constexpr bool Value = __is_trivially_assignable(_Ty);
	};

	template<typename _Ty> struct IsTrivial {
		static constexpr bool Value = IsTriviallyCopyable<_Ty>::Value && IsTriviallyDestructible<_Ty>::Value && IsTriviallyConstructible<_Ty>::Value && IsTriviallyAssignable<_Ty>::Value;
	};

	template<typename _Ty> struct IsDefaultConstructible {
		static constexpr bool Value = __is_default_constructible(_Ty);
	};

	template<typename _Ty> struct IsCopyConstructible {
		static constexpr bool Value = __is_copy_constructible(_Ty);
	};

	template<typename _Ty> struct IsMoveConstructible {
		static constexpr bool Value = __is_move_constructible(_Ty);
	};

	template<typename _Ty> struct IsCopyAssignable {
		static constexpr bool Value = __is_copy_assignable(_Ty);
	};

	template<typename _Ty> struct IsMoveAssignable {
		static constexpr bool Value = __is_move_assignable(_Ty);
	};

	template<typename _Ty> struct IsDestructible {
		static constexpr bool Value = __is_destructible(_Ty);
	};

	template<typename _Ty> struct IsConstructible {
		static constexpr bool Value = IsDefaultConstructible<_Ty>::Value || IsCopyConstructible<_Ty>::Value || IsMoveConstructible<_Ty>::Value;
	};

	template<typename _Ty> struct IsAssignable {
		static constexpr bool Value = IsCopyAssignable<_Ty>::Value || IsMoveAssignable<_Ty>::Value;
	};

	template<typename _Ty> struct IsNothrowDefaultConstructible {
		static constexpr bool Value = __is_nothrow_default_constructible(_Ty);
	};

	template<typename _Ty> struct IsNothrowCopyConstructible {
		static constexpr bool Value = __is_nothrow_copy_constructible(_Ty);
	};

	template<typename _Ty> struct IsNothrowMoveConstructible {
		static constexpr bool Value = __is_nothrow_move_constructible(_Ty);
	};

	template<typename _Ty> struct IsNothrowCopyAssignable {
		static constexpr bool Value = __is_nothrow_copy_assignable(_Ty);
	};

	template<typename _Ty> struct IsNothrowMoveAssignable {
		static constexpr bool Value = __is_nothrow_move_assignable(_Ty);
	};

	template<typename _Ty> struct IsNothrowDestructible {
		static constexpr bool Value = __is_nothrow_destructible(_Ty);
	};

	template<typename _Ty> struct IsNothrowConstructible {
		static constexpr bool Value = IsNothrowDefaultConstructible<_Ty>::Value || IsNothrowCopyConstructible<_Ty>::Value || IsNothrowMoveConstructible<_Ty>::Value;
	};

	template<typename _Ty> struct IsNothrowAssignable {
		static constexpr bool Value = IsNothrowCopyAssignable<_Ty>::Value || IsNothrowMoveAssignable<_Ty>::Value;
	};

	template<typename _Ty> struct IsNothrowSwappable {
		static constexpr bool Value = __is_nothrow_swappable(_Ty);
	};

	template<typename _Ty> struct IsSwappable {
		static constexpr bool Value = __is_swappable(_Ty);
	};

	template<typename _Ty> struct IsTriviallySwappable {
		static constexpr bool Value = __is_trivially_swappable(_Ty);
	};

	template<typename _Ty> struct IsEnum {
		static constexpr bool Value = __is_enum(_Ty);
	};

	template<typename _Ty> struct IsUnion {
		static constexpr bool Value = __is_union(_Ty);
	};

	template<typename _Ty> struct IsClass {
		static constexpr bool Value = __is_class(_Ty);
	};

	template<typename _Ty> struct IsFunction {
		static constexpr bool Value = __is_function(_Ty);
	};

	template<typename _Ty> struct IsObject {
		static constexpr bool Value = __is_object(_Ty);
	};

	template<typename _Ty> struct IsScalar {
		static constexpr bool Value = __is_scalar(_Ty);
	};

	template<typename _Ty> struct IsCompound {
		static constexpr bool Value = __is_compound(_Ty);
	};

	template<typename _Ty> struct IsConstExpr {
		static constexpr bool Value = __is_constexpr(_Ty);
	};

	template<typename _Ty> using IsArrayV                       = IsArray<_Ty>::Value;
	template<typename _Ty> using IsIntegralV                    = IsIntegral<_Ty>::Value;
	template<typename _Ty> using IsFloatingPointV               = IsFloatingPoint<_Ty>::Value;
	template<typename _Ty> using IsArithmeticV                  = IsArithmetic<_Ty>::Value;
	template<typename _Ty> using IsPointerV                     = IsPointer<_Ty>::Value;
	template<typename _Ty> using IsLValueRefV                   = IsLValueRef<_Ty>::Value;
	template<typename _Ty> using IsRValueRefV                   = IsRValueRef<_Ty>::Value;
	template<typename _Ty> using IsRefV                         = IsRef<_Ty>::Value;
	template<typename _Ty> using IsConstRef                     = IsRef<_Ty>::Value;
	template<typename _Ty> using IsConstV                       = IsConst<_Ty>::Value;
	template<typename _Ty> using IsVolatileV                    = IsVolatile<_Ty>::Value;
	template<typename _Ty> using IsCVV                          = IsCV<_Ty>::Value;
	template<typename _Ty> using IsTriviallyCopyableV           = IsTriviallyCopyable<_Ty>::Value;
	template<typename _Ty> using IsTriviallyDestructibleV       = IsTriviallyDestructible<_Ty>::Value;
	template<typename _Ty> using IsTriviallyConstructibleV      = IsTriviallyConstructible<_Ty>::Value;
	template<typename _Ty> using IsTriviallyAssignableV         = IsTriviallyAssignable<_Ty>::Value;
	template<typename _Ty> using IsTrivialV                     = IsTrivial<_Ty>::Value;
	template<typename _Ty> using IsDefaultConstructibleV        = IsDefaultConstructible<_Ty>::Value;
	template<typename _Ty> using IsCopyConstructibleV           = IsCopyConstructible<_Ty>::Value;
	template<typename _Ty> using IsMoveConstructibleV           = IsMoveConstructible<_Ty>::Value;
	template<typename _Ty> using IsCopyAssignableV              = IsCopyAssignable<_Ty>::Value;
	template<typename _Ty> using IsMoveAssignableV              = IsMoveAssignable<_Ty>::Value;
	template<typename _Ty> using IsDestructibleV                = IsDestructible<_Ty>::Value;
	template<typename _Ty> using IsConstructibleV               = IsConstructible<_Ty>::Value;
	template<typename _Ty> using IsAssignableV                  = IsAssignable<_Ty>::Value;
	template<typename _Ty> using IsNothrowDefaultConstructibleV = IsNothrowDefaultConstructible<_Ty>::Value;
	template<typename _Ty> using IsConstExprV                   = IsConstExpr<_Ty>::Value;

	template<typename _Ty> struct AddPointer {
		using Type = typename RemoveRef<_Ty>::Type*;
	};

	template<bool _Bty, typename _Ty, typename _Fty> struct Conditional {
		using Type = _Ty;
	};
	template<typename _Ty, typename _Fty> struct Conditional<false, _Ty, _Fty> {
		using Type = _Fty;
	};

	template<typename _Ty> struct Decay {
	  private:
		using _Uty = RemoveRefT<_Ty>;

	  public:
		using Type = typename Conditional<IsArray<_Uty>::Value, typename RemoveExtent<_Uty>::Type*, typename Conditional<IsFunction<_Uty>::Value, typename AddPointer<_Uty>::Type, RemoveCVT<_Uty>>::Type>::Type;
	};

	template<bool _Bty, typename _Ty = void> struct EnableIf {};

	template<typename _Ty> struct EnableIf<true, _Ty> {
		using Type = _Ty;
	};

	template <typename _Ty1, typename _Ty2> struct IsConvertible {

	};

	

	template<typename _Ty> using AddPointerT                      = typename AddPointer<_Ty>::Type;
	template<bool _Bty, typename _Ty, typename _Fty> using ConditionalT = typename Conditional<_Bty, _Ty, _Fty>::Type;
	template<typename _Ty> using DecayT                           = typename Decay<_Ty>::Type;
	template<bool _Bty, typename _Ty = void> using EnableIfT         = typename EnableIf<_Bty, _Ty>::Type;

} // namespace Spark

#endif // SPARK_TRAITS_HPP
