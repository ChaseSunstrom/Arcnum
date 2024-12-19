#ifndef SPARK_TRAITS_HPP
#define SPARK_TRAITS_HPP

namespace Spark {

	template<typename _Ty, _Ty Val> struct IntegralConstant {
		static constexpr _Ty Value = Val;
		using ValueType            = _Ty;
		using Type                 = IntegralConstant;

		constexpr           operator ValueType() const noexcept { return Value; }
		constexpr ValueType operator()() const noexcept { return Value; }
	};

	template<bool B> using BoolConstant = IntegralConstant<bool, B>;
	using TrueType                      = BoolConstant<true>;
	using FalseType                     = BoolConstant<false>;

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



	// Type aliases for convenience
	template<typename _Ty> using RemoveRefT      = typename RemoveRef<_Ty>::Type;
	template<typename _Ty> using RemoveConstT    = typename RemoveConst<_Ty>::Type;
	template<typename _Ty> using RemoveVolatileT = typename RemoveVolatile<_Ty>::Type;
	template<typename _Ty> using RemoveCVT       = typename RemoveCV<_Ty>::Type;
	template<typename _Ty> using RemoveCVRefT    = typename RemoveCV<RemoveRefT<_Ty>>::Type;
	template<typename _Ty> using RemovePointerT  = typename RemovePointer<_Ty>::Type;
	template<typename _Ty> using RemoveExtentT   = typename RemoveExtent<_Ty>::Type;

	template<typename _Ty> struct IsArray : FalseType {};
	template<typename _Ty> struct IsArray<_Ty[]> : TrueType {};
	template<typename _Ty, size_t N> struct IsArray<_Ty[N]> : TrueType {};

	template<typename _Ty1, typename _Ty2> struct IsSame : FalseType {};
	template<typename _Ty> struct IsSame<_Ty, _Ty> : TrueType {};

	template<typename _Ty> struct IsVoid : IsSame<RemoveCVT<_Ty>, void> {};

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

	template<typename _Ty> struct IsFloatingPoint : FalseType {};
	template<> struct IsFloatingPoint<float> : TrueType {};
	template<> struct IsFloatingPoint<double> : TrueType {};
	template<> struct IsFloatingPoint<long double> : TrueType {};

	template<typename _Ty> struct IsArithmetic : BoolConstant<IsIntegral<_Ty>::Value || IsFloatingPoint<_Ty>::Value> {};

	template<typename _Ty> struct IsPointer : FalseType {};
	template<typename _Ty> struct IsPointer<_Ty*> : TrueType {};

	template<typename _Ty> struct IsLValueRef : FalseType {};
	template<typename _Ty> struct IsLValueRef<_Ty&> : TrueType {};

	template<typename _Ty> struct IsRValueRef : FalseType {};
	template<typename _Ty> struct IsRValueRef<_Ty&&> : TrueType {};

	template<typename _Ty> struct IsReference : BoolConstant<IsLValueRef<_Ty>::Value || IsRValueRef<_Ty>::Value> {};

	template<typename _Ty> struct IsConstRef : FalseType {};
	template<typename _Ty> struct IsConstRef<const _Ty&> : TrueType {};

	template<typename _Ty> struct IsConst : FalseType {};
	template<typename _Ty> struct IsConst<const _Ty> : TrueType {};

	template<typename _Ty> struct IsVolatile : FalseType {};
	template<typename _Ty> struct IsVolatile<volatile _Ty> : TrueType {};

	template<typename _Ty> struct IsCV : BoolConstant<IsConst<_Ty>::Value || IsVolatile<_Ty>::Value> {};

	template<typename T> struct AddRValueReference {
		using Type = T&&;
	};

	template<typename T> typename AddRValueReference<T>::Type Declval() noexcept;

	template<typename T, typename... Args> struct IsConstructible {
	  private:
		template<typename U, typename... A> static auto Test(int) -> decltype(U(Declval<A>()...), TrueType{});

		template<typename, typename...> static auto Test(...) -> FalseType;

	  public:
		static constexpr bool Value = decltype(Test<T, Args...>(0))::Value;
	};

	template<typename T> struct IsDefaultConstructible : IsConstructible<T> {};

	template<typename T> struct IsCopyConstructible : IsConstructible<T, const T&> {};

	template<typename T> struct IsMoveConstructible : IsConstructible<T, T&&> {};

	template<typename T, typename U> struct IsAssignable {
	  private:
		template<typename V, typename W> static auto Test(int) -> decltype(Declval<V>() = Declval<W>(), TrueType{});

		template<typename, typename> static auto Test(...) -> FalseType;

	  public:
		static constexpr bool Value = decltype(Test<T, U>(0))::Value;
	};

	template<typename T> struct IsCopyAssignable : IsAssignable<T&, const T&> {};

	template<typename T> struct IsMoveAssignable : IsAssignable<T&, T&&> {};

	template<typename T> struct IsDestructible {
	  private:
		template<typename U> static auto Test(int) -> decltype(Declval<U&>().~U(), TrueType{});

		template<typename> static auto Test(...) -> FalseType;

	  public:
		static constexpr bool Value = decltype(Test<T>(0))::Value;
	};

	template<typename T> struct IsAnyConstructible {
		static constexpr bool Value = IsDefaultConstructible<T>::Value || IsCopyConstructible<T>::Value || IsMoveConstructible<T>::Value;
	};

	template<typename T> struct IsAnyAssignable {
		static constexpr bool Value = IsCopyAssignable<T>::Value || IsMoveAssignable<T>::Value;
	};

	template<typename _Ty> struct IsNothrowDefaultConstructible : BoolConstant<noexcept(_Ty())> {};

	template<typename _Ty> struct IsNothrowCopyConstructible : BoolConstant<noexcept(_Ty(Declval<const _Ty&>()))> {};

	template<typename _Ty> struct IsNothrowMoveConstructible : BoolConstant<noexcept(_Ty(Declval<_Ty&&>()))> {};

	template<typename _Ty> struct IsNothrowCopyAssignable : BoolConstant<noexcept(Declval<_Ty&>() = Declval<const _Ty&>())> {};

	template<typename _Ty> struct IsNothrowMoveAssignable : BoolConstant<noexcept(Declval<_Ty&>() = Declval<_Ty&&>())> {};

	template<typename _Ty> struct IsNothrowDestructible : BoolConstant<noexcept(Declval<_Ty&>().~_Ty())> {};

	template<typename _Ty> struct IsFunction : FalseType {};

	template<typename Ret, typename... Args> struct IsFunction<Ret(Args...)> : TrueType {};

	template<typename Ret, typename... Args> struct IsFunction<Ret(Args..., ...)> : TrueType {};

	template<typename _Ty> struct IsNullptrType : FalseType {};
	template<> struct IsNullptrType<decltype(nullptr)> : TrueType {};

	template<typename Ret, typename... Args> struct IsFunction<Ret (*)(Args...)> : TrueType {};

	template<typename Ret, typename... Args> struct IsFunction<Ret (*)(Args..., ...)> : TrueType {};

	template<typename _Ty> struct IsEnum : BoolConstant<__is_enum(_Ty)> {};

	template<typename _Ty> struct IsUnion : BoolConstant<__is_union(_Ty)> {};

	template<typename _Ty> struct IsClass : BoolConstant<__is_class(_Ty)> {};

	template<typename _Ty> struct IsMemberPointer : FalseType {};
	template<typename T, typename U> struct IsMemberPointer<T U::*> : TrueType {};

	template<typename _Ty> struct IsFundamental : BoolConstant<IsArithmetic<_Ty>::Value || IsVoid<_Ty>::Value || IsNullptrType<_Ty>::Value> {};

	template<typename _Ty> struct IsScalar : BoolConstant<IsArithmetic<_Ty>::Value || IsEnum<_Ty>::Value || IsPointer<_Ty>::Value || IsMemberPointer<_Ty>::Value || IsNullptrType<_Ty>::Value> {};

	template<typename _Ty> struct IsCompound : BoolConstant<!IsFundamental<_Ty>::Value> {};

	template<typename _Ty> struct IsObject : BoolConstant<!IsFunction<_Ty>::Value && !IsReference<_Ty>::Value && !IsVoid<_Ty>::Value> {};

	template<typename _Ty> struct AddPointer {
		using Type = typename RemoveRef<_Ty>::Type*;
	};

	template<typename _Ty> using AddPointerT = typename AddPointer<_Ty>::Type;

	template<bool B, typename _Ty, typename F> struct Conditional {
		using Type = _Ty;
	};

	template<typename _Ty, typename F> struct Conditional<false, _Ty, F> {
		using Type = F;
	};

	template<bool B, typename _Ty, typename F> using ConditionalT = typename Conditional<B, _Ty, F>::Type;

	template<typename Base, typename Derived> struct IsBaseOf {
	  private:
		template<typename B, typename D> static auto Test(int) -> decltype(static_cast<const volatile B*>(static_cast<D*>(nullptr)), TrueType{});

		template<typename, typename> static auto Test(...) -> FalseType;

	  public:
		static constexpr bool Value = decltype(Test<Base, Derived>(0))::Value;
	};

	template<bool B, typename _Ty = void> struct EnableIf {};

	template<typename _Ty> struct EnableIf<true, _Ty> {
		using Type = _Ty;
	};

	template<bool B, typename _Ty = void> using EnableIfT = typename EnableIf<B, _Ty>::Type;

	template<bool _FirstValue, class _First, class... _Rest> struct _Conjunction {
		using Type = _First;
	};

	template<class _True, class _Next, class... _Rest> struct _Conjunction<true, _True, _Next, _Rest...> {
		using Type = typename _Conjunction<_Next::Value, _Next, _Rest...>::Type;
	};

	template<class... _Traits> struct Conjunction : TrueType {}; // Empty case is true

	template<class _First, class... _Rest> struct Conjunction<_First, _Rest...> : _Conjunction<_First::Value, _First, _Rest...>::Type {
		// First false trait, or last trait if none are false
	};

	template<class _Trait> struct Negation : BoolConstant<!static_cast<bool>(_Trait::Value)> {};

	template<class... _Traits> constexpr bool ConjunctionV = Conjunction<_Traits...>::Value;

	template<class _Trait> constexpr bool NegationV        = Negation<_Trait>::Value;


	template<typename From, typename To> struct IsConvertible {
	  private:
		static void TestFunc(To);

		template<typename F, typename = decltype(TestFunc(Declval<F>()))> static TrueType Test(int);

		template<typename> static FalseType Test(...);

	  public:
		static constexpr bool Value = decltype(Test<From>(0))::Value;
	};

	template<typename Derived, typename Base>
	concept DerivedFrom = IsBaseOf<Base, Derived>::Value && IsConvertible<const volatile Derived*, const volatile Base*>::Value;

	template<bool> struct Select {
		template<class _Ty1, class> using Apply = _Ty1;
	};

	template<> struct Select<false> {
		template<class, class _Ty2> using Apply = _Ty2;
	};

	template<typename _Ty> struct Decay {
		using Uty  = RemoveRefT<_Ty>;
		using Ty2  = typename Conditional<IsFunction<Uty>::Value, AddPointerT<Uty>, RemoveCVT<Uty>>::Type;
		using Type = typename Conditional<IsArray<Uty>::Value, AddPointerT<RemoveExtentT<Uty>>, Ty2>::Type;
	};

	template<class _Ty> using DecayT = typename Decay<_Ty>::Type;

	template<typename _Callable, typename... _Args> struct InvokeResult {
	  private:
		template<typename F, typename... A> static auto Test(int) -> decltype(Declval<F>()(Declval<A>()...), void(), TrueType{});

		template<typename, typename...> static auto Test(...) -> FalseType;

	  public:
		static constexpr bool Value = decltype(Test<_Callable, _Args...>(0))::Value;
		using Type                  = decltype(Declval<_Callable>()(Declval<_Args>()...));
	};

	template<typename _Callable, typename... _Args> using InvokeResultT = typename InvokeResult<_Callable, _Args...>::Type;

	template<typename... _Types> struct CommonType;

	template<> struct CommonType<> {};

	template<typename _Ty> struct CommonType<_Ty> {
		using Type = DecayT<_Ty>;
	};

	template<typename _Ty1, typename _Ty2, typename... _Rest> struct CommonType<_Ty1, _Ty2, _Rest...> {
	  private:
		using Type1 = typename CommonType<_Ty1, _Ty2>::Type;
		using Type2 = typename CommonType<_Rest...>::Type;

	  public:
		using Type = typename CommonType<Type1, Type2>::Type;
	};

	template<typename _Ty1, typename _Ty2> constexpr bool IsSameV        = IsSame<_Ty1, _Ty2>::Value;
	template<typename _Ty1, typename _Ty2> constexpr bool IsBaseOfV      = IsBaseOf<_Ty1, _Ty2>::Value;
	template<typename _Ty1, typename _Ty2> constexpr bool IsConvertibleV = IsConvertible<_Ty1, _Ty2>::Value;

	template<typename _Ty1, typename _Ty2> struct CommonType<_Ty1, _Ty2> {
	  private:
		using Decay1 = DecayT<_Ty1>;
		using Decay2 = DecayT<_Ty2>;

		template<typename T1, typename T2> static auto Test(int) -> decltype(true ? Declval<T1>() : Declval<T2>());

		template<typename, typename> static auto Test(...) -> void;

		using ResultType = decltype(Test<Decay1, Decay2>(0));

	  public:
		using Type = typename Conditional<!IsSameV<ResultType, void>, DecayT<ResultType>, void>::Type;
	};

	template<typename... _Types> using CommonTypeT                                       = typename CommonType<_Types...>::Type;

	// Value aliases for traits
	template<typename _Ty> constexpr bool                 IsArrayV                       = IsArray<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsIntegralV                    = IsIntegral<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsFloatingPointV               = IsFloatingPoint<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsArithmeticV                  = IsArithmetic<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsFunctionV                    = IsFunction<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsPointerV                     = IsPointer<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsLValueRefV                   = IsLValueRef<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsRValueRefV                   = IsRValueRef<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsReferenceV                   = IsReference<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsConstRefV                    = IsConstRef<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsConstV                       = IsConst<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsVolatileV                    = IsVolatile<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsCVV                          = IsCV<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsDefaultConstructibleV        = IsDefaultConstructible<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsCopyConstructibleV           = IsCopyConstructible<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsMoveConstructibleV           = IsMoveConstructible<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsCopyAssignableV              = IsCopyAssignable<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsMoveAssignableV              = IsMoveAssignable<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsDestructibleV                = IsDestructible<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsNothrowDefaultConstructibleV = IsNothrowDefaultConstructible<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsNothrowCopyConstructibleV    = IsNothrowCopyConstructible<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsNothrowMoveConstructibleV    = IsNothrowMoveConstructible<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsNothrowCopyAssignableV       = IsNothrowCopyAssignable<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsNothrowMoveAssignableV       = IsNothrowMoveAssignable<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsNothrowDestructibleV         = IsNothrowDestructible<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsEnumV                        = IsEnum<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsUnionV                       = IsUnion<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsClassV                       = IsClass<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsMemberPointerV               = IsMemberPointer<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsFundamentalV                 = IsFundamental<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsScalarV                      = IsScalar<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsCompoundV                    = IsCompound<_Ty>::Value;
	template<typename _Ty> constexpr bool                 IsObjectV                      = IsObject<_Ty>::Value;

} // namespace Spark

#endif // SPARK_TRAITS_HPP
